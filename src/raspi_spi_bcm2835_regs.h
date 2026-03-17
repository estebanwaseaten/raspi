// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * register offsets for the raspi spi control
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

#ifndef raspi_spi_bcm2835_regs_h
#define raspi_spi_bcm2835_regs_h


// https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
#define BCM2835_SPI_REG_CS      0x0     //SPI Master Control and Status
#define BCM2835_SPI_REG_FIFO    0x4     //SPI Master TX and RX FIFOs
#define BCM2835_SPI_REG_CLK     0x8     //SPI Master Clock Divider
#define BCM2835_SPI_REG_DLEN    0xc     //SPI Master Data Length
#define BCM2835_SPI_REG_LTOH    0x10    //SPI LOSSI mode TOH
#define BCM2835_SPI_REG_DC      0x14    //SPI DMA DREQ Controls

/* Set bit:
 * 0000 | 0100 -> 0100
 * 1111 | 0100 -> 1111
 * Unset bit:
 * 1111 & 1011 -> 1011
 * 0000 & 1011 -> 0000
 *
 * set: register |= BCM2835_SPI_REG_CS_LEN_LONG
 * unset: register &= ~4BCM2835_SPI_REG_CS_LEN_LONG
 */

// 1. BCM2835_SPI_REG_CS
// 31:26 reserved
#define BCM2835_SPI_REG_CS_LEN_LONG (1 << 25)   //enable long data in lossi
#define BCM2835_SPI_REG_CS_DMA_LEN (1 << 24)    //enable DMA in lossi

#define BCM2835_SPI_REG_CS_CSPOL2 (1 << 23)       //chip select 2 polarity (0 is active low)
#define BCM2835_SPI_REG_CS_CSPOL1 (1 << 22)       //(0 is active low)
#define BCM2835_SPI_REG_CS_CSPOL0 (1 << 21)       //(1 is active high)

#define BCM2835_SPI_REG_CS_RXF (1 << 20)        //RX FIFO is full
#define BCM2835_SPI_REG_CS_RXR (1 << 19)        //RX FIFO is more full?
#define BCM2835_SPI_REG_CS_TXD (1 << 18)        //TX FIFO can accept data (1)
#define BCM2835_SPI_REG_CS_RXD (1 << 17)        //RX FIFO contains data (at least 1 byte)
#define BCM2835_SPI_REG_CS_DONE (1 << 16)       //transfer is done (1) returns low as soon as one puts sth into TX FIFO

#define BCM2835_SPI_REG_CS_LEN (1 << 13)        //LEN lossi enable
#define BCM2835_SPI_REG_CS_REN (1 << 12)        //REN read enable

#define BCM2835_SPI_REG_CS_ADCS (1 << 11)       // automatically deassert chip select after transfer
#define BCM2835_SPI_REG_CS_INTR (1 << 10)       // INTR interrupt on RXR=1
#define BCM2835_SPI_REG_CS_INTD (1 << 9)        // INTD interrupt on done

#define BCM2835_SPI_REG_CS_DMAEN (1 << 8)        //DMAEN DMA enable - enables DMA operation

#define BCM2835_SPI_REG_CS_TA (1 << 7)          //transfer active

#define BCM2835_SPI_REG_CS_CSPOL (1 << 6)       //chip select polarity (1=active high)
#define BCM2835_SPI_REG_CS_CLEAR_FIFO_ALL (3 << 5) //clear FIFO
#define BCM2835_SPI_REG_CS_CLEAR_FIFO_TX (1 << 5) //clear FIFO TX
#define BCM2835_SPI_REG_CS_CLEAR_FIFO_RX (2 << 5) //clear FIFO RX
#define BCM2835_SPI_REG_CS_CPOL (1 << 3)        //clock polarity (1=rest state high)
#define BCM2835_SPI_REG_CS_CPHA (1 << 2)        //clock phase  (0=first transition at middle of data bit, 1=first transition at beginning of data bit)
#define BCM2835_SPI_REG_CS_CS0_MASK (3)             // how does one set this??? & inverted
#define BCM2835_SPI_REG_CS_CS1 (1 << 0)
#define BCM2835_SPI_REG_CS_CS2 (1 << 1)

//#define














#endif
