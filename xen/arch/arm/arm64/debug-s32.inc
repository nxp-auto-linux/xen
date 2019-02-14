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
