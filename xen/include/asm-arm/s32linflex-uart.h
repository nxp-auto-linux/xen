/*
 * xen/include/asm-arm/s32linflex-uart.h
 *
 * Freescale linflexuart serial port driver constants
 *
 * Peter van der Perk <peter.vander.perk@nxp.com>
 * Copyright 2018, 2021 NXP
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

#ifndef __ASM_ARM_S32LINFLEX_UART_H
#define __ASM_ARM_S32LINFLEX_UART_H

#define LINCR1	0x0000	/* LIN control register				*/
#define LINIER	0x0004	/* LIN interrupt enable register		*/
#define LINSR	0x0008	/* LIN status register				*/
#define LINESR	0x000C	/* LIN error status register			*/
#define UARTCR	0x0010	/* UART mode control register			*/
#define UARTSR	0x0014	/* UART mode status register			*/
#define LINTCSR	0x0018	/* LIN timeout control status register		*/
#define LINOCR	0x001C	/* LIN output compare register			*/
#define LINTOCR	0x0020	/* LIN timeout control register			*/
#define LINFBRR	0x0024	/* LIN fractional baud rate register		*/
#define LINIBRR	0x0028	/* LIN integer baud rate register		*/
#define LINCFR	0x002C	/* LIN checksum field register			*/
#define LINCR2	0x0030	/* LIN control register 2			*/
#define BIDR	0x0034	/* Buffer identifier register			*/
#define BDRL	0x0038	/* Buffer data register least significant	*/
#define BDRM	0x003C	/* Buffer data register most significant	*/
#define IFER	0x0040	/* Identifier filter enable register		*/
#define IFMI	0x0044	/* Identifier filter match index		*/
#define IFMR	0x0048	/* Identifier filter mode register		*/
#define GCR	0x004C	/* Global control register			*/
#define UARTPTO	0x0050	/* UART preset timeout register			*/
#define UARTCTO	0x0054	/* UART current timeout register		*/
/* The offsets for DMARXE/DMATXE in master mode only			*/
#define DMATXE	0x0058	/* DMA Tx enable register			*/
#define DMARXE	0x005C	/* DMA Rx enable register			*/

/*
 *	CONSTANT DEFINITIONS
 */


#define US1_TDRE            (1 << 7)
#define US1_RDRF            (1 << 5)
#define UC2_TE              (1 << 3)
#define LINCR1_INIT         (1 << 0)
#define LINCR1_MME          (1 << 4)
#define LINCR1_BF           (1 << 7)
#define LINSR_LINS_INITMODE (0x00001000)
#define LINSR_LINS_MASK     (0x0000F000)
#define LINIER_SZIE         (1 << 15)
#define LINIER_OCIE         (1 << 14)
#define LINIER_BEIE         (1 << 13)
#define LINIER_CEIE         (1 << 12)
#define LINIER_HEIE         (1 << 11)
#define LINIER_FEIE         (1 << 8)
#define LINIER_BOIE         (1 << 7)
#define LINIER_LSIE         (1 << 6)
#define LINIER_WUIE         (1 << 5)
#define LINIER_DBFIE        (1 << 4)
#define LINIER_DBEIETOIE    (1 << 3)
#define LINIER_DRIE         (1 << 2)
#define LINIER_DTIE         (1 << 1)
#define LINIER_HRIE         (1 << 0)
#define UARTCR_UART         (1 << 0)
#define UARTCR_WL0          (1 << 1)
#define UARTCR_PCE          (1 << 2)
#define UARTCR_PC0          (1 << 3)
#define UARTCR_TXEN         (1 << 4)
#define UARTCR_RXEN         (1 << 5)
#define UARTCR_PC1          (1 << 6)
#define UARTCR_TFBM         (1 << 8)
#define UARTCR_RFBM         (1 << 9)
#define UARTCR_TDFL         (0x0000E000)
#define UARTCR_ROSE         (1 << 23)
#define UARTSR_DTF          (1 << 1)
#define UARTSR_DRF          (1 << 2)
#define UARTSR_RMB          (1 << 9)

#define LINFLEX_LDIV_MULTIPLIER (16)
#define UARTCR_OSR_MASK     (0xF << 24)
#define UARTCR_OSR_GET(uartcr)  (((uartcr) & UARTCR_OSR_MASK) >> 24)
#define UARTCR_OSR(val)     ((val << 24) & UARTCR_OSR_MASK)

#endif /* __ASM_ARM_S32LINFLEX_UART_H */

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
