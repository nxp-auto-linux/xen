/*
 * xen/arch/arm/arm64/debug-s32.inc
 *
 * S32V UART specific debug code
 *
 * Driver for NXP Linflex UART.
 *
 * Peter van der Perk <peter.vander.perk@nxp.com>
 * Copyright 2018 NXP
 *
 */

#include <asm/asm_defns.h>
#include <asm/s32linflex-uart.h>

/*
 * S32V-Linflex UART wait UART to be ready to transmit
 * xb: register which contains the UART base address
 * c: scratch register number
 */
.macro early_uart_ready xb, c
1:
        ldrb  w\c, [\xb, #UARTSR]
        tst   w\c, #UARTSR_DTF
        b.ne  1b
.endm

/*
 * S32V-Linflex UART transmit character
 * xb: register which contains the UART base address
 * wt: register which contains the character to transmit
 */
.macro early_uart_transmit xb, wt
        strb  \wt, [\xb, #BDRL]
.endm

/*
 * Local variables:
 * mode: ASM
 * indent-tabs-mode: nil
 * End:
 */
