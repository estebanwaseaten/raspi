// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

 #include "raspi_gpio_bcm2835.h"

void gpio_bcm2835::regBackup()
{
    regRead( 0x000, &backupRegs[0] );
    regRead( 0x004, &backupRegs[1] );
}

void gpio_bcm2835::regRestore()
{
    regWrite( 0x000, backupRegs[0] );
    regWrite( 0x004, backupRegs[1] );
}

void gpio_bcm2835::setSettings()
{
    regWrite( 0x000, 613566756 );
    regWrite( 0x004, 613566756 );
}

/*
https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf

chip select pins:
GPIO7 High SPI0_CE1_N
GPIO8 High SPI0_CE0_N
9, 10, 11 are also SPI pins

GPIO base: 0x7e200000
offset: 0x00: GPFSEL0   --> function select for pins 0 - 9:
FSEL7: bits 23:21
FSEL8: bits 26:24
FSEL9: bits 29:27

GPFSEL1 :
2:0     FSEL10
5:3     FSEL11

0010 0100 1001 0010 0100 1001 0010 0100 = 0x24924924 should set all pins in this register to alternat function 1


000 = GPIO Pin 9 is an input
001 = GPIO Pin 9 is an output
100 = GPIO Pin 9 takes alternate function 0
101 = GPIO Pin 9 takes alternate function 1
110 = GPIO Pin 9 takes alternate function 2
111 = GPIO Pin 9 takes alternate function 3
011 = GPIO Pin 9 takes alternate function 4
010 = GPIO Pin 9 takes alternate function 5

pins (7), 8, 9, 10 and 11 should be set to alternate function 0 --> 100

SPI0: 0x7e204000
SPI0: 0x7e204000

0x00 CS SPI Master Control and Status
0x04 FIFO SPI Master TX and RX FIFOs
0x08 CLK SPI Master Clock Divider
0x0c DLEN SPI Master Data Length
0x10 LTOH SPI LoSSI mode TOH
0x14 DC SPI DMA DREQ Controls

9.6.1. Polled
1. Set CS, CPOL, CPHA as required and set TA = 1
2. Poll TXD writing bytes to SPI_FIFO, RXD reading bytes from SPI_FIFO until all data written
3. Poll DONE until it goes to 1
4. Set TA = 0

You should place:
• A memory write barrier before the first write to a peripheral
• A memory read barrier after the last read of a peripheral*/
