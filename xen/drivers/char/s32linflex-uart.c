/*
 * xen/drivers/char/s32linflex-uart.c
 *
 * Driver for NXP Linflex UART.
 *
 * Peter van der Perk <peter.vander.perk@nxp.com>
 * Copyright 2018, 2021 NXP
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms and conditions of the GNU General Public
 * License, version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

#include <xen/config.h>
#include <xen/console.h>
#include <xen/errno.h>
#include <xen/serial.h>
#include <xen/init.h>
#include <xen/irq.h>
#include <xen/mm.h>
#include <asm/device.h>
#include <asm/s32linflex-uart.h>
#include <asm/io.h>

#define s32linflex_uart_readl(uart, off)       \
			readl((uart)->regs + (off))
#define s32linflex_uart_writel(uart, off, val) \
			writel((val), (uart)->regs + (off))
#define s32linflex_uart_readb(uart, off)       \
			readb((uart)->regs + (off))
#define s32linflex_uart_writeb(uart, off, val) \
			writeb((val), (uart)->regs + (off))

#define LIN_CLK_FREQ	(125000000)	/* LIN_CLK freq taken from Linux */
#define LIN_BAUDRATE	(115200)	/* LINFlex UART Baud Rate  */
#define LIN_DATABITS	(8)		/* LINFlex UART Data Bits  */
#define LIN_STOPBITS	(1)		/* LINFlex UART Stop Bits  */
#define LIN_PARITYNONE	('n')		/* LINFlex UART Parity None */

static struct s32linflex_uart {
	unsigned int baud, clock_hz, data_bits, parity, stop_bits, fifo_size;
	unsigned int irq;
	char __iomem *regs;
	struct irqaction irqaction;
	struct vuart_info vuart;
} s32_com = {0};

static u32 s32linflex_uart_ldiv_multiplier(struct s32linflex_uart *uart)
{
	u32 mul = LINFLEX_LDIV_MULTIPLIER;
	u32 cr;

	cr = s32linflex_uart_readl(uart, UARTCR);

	if (cr & UARTCR_ROSE)
		mul = UARTCR_OSR_GET(cr);

	return mul;
}

static void __init s32linflex_uart_init_preirq(struct serial_port *port)
{
	struct s32linflex_uart *uart = port->uart;
	u32 ctrl;
	u32 clk, baud_rate;
	u32 ibr, fbr, divisr, dividr;

	/* init mode */
	ctrl = LINCR1_INIT;
	s32linflex_uart_writel(uart, LINCR1, ctrl);

	/* waiting for init mode entry */
	while ((s32linflex_uart_readl(uart, LINSR) & LINSR_LINS_MASK) !=
		LINSR_LINS_INITMODE)
		;

	/* set Master Mode */
	ctrl |= LINCR1_MME;
	s32linflex_uart_writel(uart, LINCR1, ctrl);

	/* set UART bit to allow writing other bits */
	s32linflex_uart_writel(uart, UARTCR, UARTCR_UART);

	/* provide data bits, parity, stop bit, etc */
	clk = uart->clock_hz;
	baud_rate = uart->baud;

	divisr = clk;
	dividr = (u32)(baud_rate * s32linflex_uart_ldiv_multiplier(uart));

	ibr = (u32)(divisr / dividr);
	fbr = (u32)((divisr % dividr) * 16 / dividr) & 0xF;

	s32linflex_uart_writel(uart, LINIBRR, ibr);
	s32linflex_uart_writel(uart, LINFBRR, fbr);

	/* Set preset timeout register value */
	s32linflex_uart_writel(uart, UARTPTO, 0xF);

	/* 8 bit data, no parity, Tx and Rx enabled, UART mode */
	s32linflex_uart_writel(uart, UARTCR, UARTCR_PC1 | UARTCR_RXEN |
		UARTCR_TXEN | UARTCR_PC0 | UARTCR_WL0 | UARTCR_UART);

	ctrl = s32linflex_uart_readl(uart, LINCR1);
	ctrl &= ~LINCR1_INIT;

	/* end init mode */
	s32linflex_uart_writel(uart, LINCR1, ctrl);
}

static void s32linflex_uart_interrupt(int irq, void *data,
					struct cpu_user_regs *regs)
{
	struct serial_port *port = data;
	struct s32linflex_uart *uart = port->uart;
	unsigned int sts;

	sts = s32linflex_uart_readl(uart, UARTSR);

	if (sts & UARTSR_DRF)
		serial_rx_interrupt(port, regs);

	if (sts & UARTSR_DTF)
		serial_tx_interrupt(port, regs);
}

static void __init s32linflex_uart_init_postirq(struct serial_port *port)
{
	struct s32linflex_uart *uart = port->uart;
	unsigned int temp;

	uart->irqaction.handler = s32linflex_uart_interrupt;
	uart->irqaction.name = "s32linflex_uart";
	uart->irqaction.dev_id = port;

	temp = s32linflex_uart_readl(uart, LINIER);
	temp |= (LINIER_DRIE | LINIER_DTIE);
	s32linflex_uart_writel(uart, LINIER, temp);
}

static void s32linflex_uart_suspend(struct serial_port *port)
{
	BUG();
}

static void s32linflex_uart_resume(struct serial_port *port)
{
	BUG();
}


static int s32linflex_uart_tx_ready(struct serial_port *port)
{
	struct s32linflex_uart *uart = port->uart;

	return (s32linflex_uart_readb(uart, UARTSR) & UARTSR_DTF) ? 1 : 0;
}

static void s32linflex_uart_putc(struct serial_port *port, char c)
{
	struct s32linflex_uart *uart = port->uart;

	s32linflex_uart_writeb(uart, BDRL, c);

	s32linflex_uart_writeb(uart, UARTSR,
		(s32linflex_uart_readb(uart, UARTSR) | UARTSR_DTF));
}

static int s32linflex_uart_getc(struct serial_port *port, char *pc)
{
	struct s32linflex_uart *uart = port->uart;
	int ch;

	if (!(s32linflex_uart_readb(uart, UARTSR) & UARTSR_DRF))
		return 0;

	if (!(s32linflex_uart_readl(uart, UARTSR) & UARTSR_RMB))
		return 0;

	ch = s32linflex_uart_readl(uart, BDRM);
	*pc = ch & 0xff;

	s32linflex_uart_writeb(uart, UARTSR,
		(s32linflex_uart_readb(uart, UARTSR) |
			(UARTSR_DRF | UARTSR_RMB)));


	return 1;
}

static int __init s32linflex_uart_irq(struct serial_port *port)
{
	struct s32linflex_uart *uart = port->uart;

	return ((uart->irq > 0) ? uart->irq : -1);
}


static const struct vuart_info *s32linflex_uart_vuart_info(
	struct serial_port *port)
{
	struct s32linflex_uart *uart = port->uart;

	return &uart->vuart;
}

static void s32linflex_uart_start_tx(struct serial_port *port)
{
	struct s32linflex_uart *uart = port->uart;
	unsigned long temp;

	temp = s32linflex_uart_readl(uart, LINIER);
	s32linflex_uart_writel(uart, LINIER, temp | LINIER_DTIE);
}

static void s32linflex_uart_stop_tx(struct serial_port *port)
{
	struct s32linflex_uart *uart = port->uart;
	unsigned long temp;

	temp = s32linflex_uart_readl(uart, LINIER);
	temp &= ~(LINIER_DTIE);
	s32linflex_uart_writel(uart, LINIER, temp);
}


static struct uart_driver __read_mostly s32linflex_uart_driver = {
	.init_preirq = s32linflex_uart_init_preirq,
	.init_postirq = s32linflex_uart_init_postirq,
	.endboot = NULL,
	.suspend = s32linflex_uart_suspend,
	.resume = s32linflex_uart_resume,
	.tx_ready = s32linflex_uart_tx_ready,
	.putc = s32linflex_uart_putc,
	.getc = s32linflex_uart_getc,
	.irq = s32linflex_uart_irq,
	.start_tx = s32linflex_uart_start_tx,
	.stop_tx = s32linflex_uart_stop_tx,
	.vuart_info = s32linflex_uart_vuart_info,
};

static int __init s32linflex_uart_init(struct dt_device_node *dev,
					const void *data)
{
	const char *config = data;
	struct s32linflex_uart *uart;
	int res;
	u64 addr, size;

	if (config && strcmp(config, ""))
		printk(KERN_WARNING "WARNING: UART configuration is not supported\n");

	uart = &s32_com;

	uart->clock_hz = LIN_CLK_FREQ;
	uart->baud = LIN_BAUDRATE;
	uart->data_bits = LIN_DATABITS;
	uart->parity = LIN_PARITYNONE;
	uart->stop_bits = LIN_STOPBITS;

	res = dt_device_get_address(dev, 0, &addr, &size);
	if (res) {
		printk(KERN_ERR "s32linflex-uart: Unable to retrieve the base address of the UART\n");
		return res;
	}

	res = platform_get_irq(dev, 0);
	if (res < 0) {
		printk(KERN_ERR "s32linflex-uart: Unable to retrieve the IRQ\n");
		return -EINVAL;
	}
	uart->irq = res;

	uart->regs = ioremap_nocache(addr, size);
	if (!uart->regs) {
		printk(KERN_ERR "s32linflex-uart: Unable to map the UART memory\n");
		return -ENOMEM;
	}

	uart->vuart.base_addr = addr;
	uart->vuart.size = size;
	uart->vuart.data_off = BDRL;
	uart->vuart.status_off = UARTSR;
	uart->vuart.status = UARTSR_DTF;

	/* Register with generic serial driver */
	serial_register_uart(SERHND_DTUART, &s32linflex_uart_driver, uart);

	dt_device_set_used_by(dev, DOMID_XEN);

	return 0;
}

static const struct dt_device_match s32linflex_uart_dt_compat[] __initconst = {
	DT_MATCH_COMPATIBLE("fsl,s32-linflexuart"),
	{},
};

DT_DEVICE_START(s32linflex_uart, "NXP Linflex UART", DEVICE_SERIAL)
	.dt_match = s32linflex_uart_dt_compat,
	.init = s32linflex_uart_init,
DT_DEVICE_END
/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
