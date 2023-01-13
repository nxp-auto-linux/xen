/*
 * xen/arch/arm/arm64/debug-s32.inc
 *
 * S32 UART specific debug code
 *
 * Driver for NXP Linflex UART.
 *
 * Peter van der Perk <peter.vander.perk@nxp.com>
 * Copyright 2018, 2021, 2023 NXP
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

#include <asm/asm_defns.h>
#include <asm/s32linflex-uart.h>

/*
 * S32-Linflex UART wait UART to be ready to transmit
 * xb: register which contains the UART base address
 * c: scratch register number
 */
.macro early_uart_ready xb, c
        ldr   w\c, [\xb, #UARTCR]       /* <= Control Register */
        and   w\c, w\c, #UARTCR_TFBM    /* Check Buffer/FIFO (0/1) Mode */
        cbz   w\c, 2f                   /* Buffer Mode => return */
1:
        ldrb  w\c, [\xb, #UARTSR]       /* <= Status Register */
        tst   w\c, #UARTSR_DTF          /* FIFO Mode => Check DTF bit */
        b.ne  1b
2:
.endm

/*
 * S32-Linflex UART transmit character
 * xb: register which contains the UART base address
 * wt: register which contains the character to transmit
 */
.macro early_uart_transmit xb, wt
        strb  \wt, [\xb, #BDRL]

        ldr   \wt, [\xb, #UARTCR]       /* <= Control Register */
        and   \wt, \wt, #UARTCR_TFBM    /* Check Buffer/FIFO (0/1) Mode */
        cbnz  \wt, 2f                   /* FIFO Mode => goto exit */

3:      /* Buffer Mode */
        ldrb  \wt, [\xb, #UARTSR]       /* <= Status Register */
        and   \wt, \wt, #UARTSR_DTF     /* Check Transmission Completed */
        cbz   \wt, 3b

        ldr   \wt, [\xb, #UARTSR]       /* <= Status Register */
        orr   \wt, \wt, #UARTSR_DTF     /* Clear DTF bit */
        str   \wt, [\xb, #UARTSR]
2:
.endm

/*
 * Local variables:
 * mode: ASM
 * indent-tabs-mode: nil
 * End:
 */
