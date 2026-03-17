// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * register offsets for the raspi spi control in the raspberry 5
 * terminology is taken from the Synopsis DesignWare DW_apb_ssi Databook to make it easier to compare
 * in general, please consider updating the terminology:
 * https://www.oshwa.org/a-resolution-to-redefine-spi-signal-names/
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

#ifndef raspi_SSI_dwapbssi_regs_h
#define raspi_SSI_dwapbssi_regs_h

// from linux/drivers/spi/spi-dw.h:
/* Register offsets (Generic for both DWC APB SSI and DWC SSI IP-cores) */
#define DW_SSI_CTRLR0		    0x00     //CTRLR0 controls serial transfer - write only if DW_SSI_SSIENR is disabled
#define DW_SSI_CTRLR1		    0x04     //CTRLR1 only exists when chip is master device. controls end of serial transfers when in receive only mode.
#define DW_SSI_SSIENR		    0x08     //SSIENR enables and disables DW_apb_ssi
#define DW_SSI_MWCR             0x0c     //MWCR microwire control register
#define DW_SSI_SER              0x10     //SER only exists when chip is master device. slave enable register
#define DW_SSI_BAUDR			0x14     //BAUDR only exists when chip is master device. clock divider...
#define DW_SSI_TXFTLR			0x18
#define DW_SSI_RXFTLR			0x1c
#define DW_SSI_TXFLR			0x20
#define DW_SSI_RXFLR			0x24
#define DW_SSI_SR			    0x28      // status register
#define DW_SSI_IMR			    0x2c      // interrupt mask register
#define DW_SSI_ISR			    0x30      // interrupt status register
#define DW_SSI_RISR			    0x34      // raw interrupt status register
#define DW_SSI_TXOICR			0x38      // Transmit FIFO Overflow Interrupt Clear Registers
#define DW_SSI_RXOICR			0x3c      // Receive FIFO Overflow Interrupt Clear Register
#define DW_SSI_RXUICR			0x40      // Receive FIFO Underflow Interrupt Clear Register
#define DW_SSI_MSTICR			0x44      // Multi-Master Interrupt Clear Register
#define DW_SSI_ICR			    0x48      // Interrupt Clear Register
#define DW_SSI_DMACR			0x4c      // DMA Control Register
#define DW_SSI_DMATDLR			0x50      // DMA Transmit Data Level ...
#define DW_SSI_DMARDLR			0x54      // DMA Receive Data Leve
#define DW_SSI_IDR			    0x58      // identification register: contains peripherals identification code
#define DW_SSI_VERSION			0x5c      // hex representation of synopsis component version

#define DW_SSI_DR0			    0x60      // data register if SSI_MAX_XFER_SIZE = 32 --> all 32 bits are valid, otherwise only [15:0] are valid
#define DW_SSI_DR35			    0x60 + (35*4) //  these are the registers to write into FIFO and read out of FIFO

#define DW_SSI_RX_SAMPLE_DLY	0xf0      // RX Sample Delay Register
#define DW_SSI_SPI_CTRL0		0xf4      // SPI control register, only valid if SSI_SPI_MODE is dual or quad or octal
#define DW_SSI_TXD_DRIVE_EDGE   0xf8      //  Transmit Drive Edge Register
// 0xfc is reserved

// DW_SSI_CTRLR0
// reserved: 31:25
#define DW_SSI_CTRLR0_SSTE          (1 << 24) //slave select toggle enable (controls toggle between slave select)
// reserved: 23
#define DW_SSI_CTRLR0_SPI_FRF0      (1 << 22)    //SPI frame format 0: standard; 1: dual; 2: quad; 3: octal
#define DW_SSI_CTRLR0_SPI_FRF1      (1 << 21)    //SPI frame format
#define DW_SSI_CTRLR0_SPI_FRF_MASK  (3 << 21)

#define DW_SSI_CTRLR0_DFS_32_MASK   (0x1F << 16)  //DATA FRAME SIZE: if SSI_MAX_XFER_SIZE is configured to 32.
#define DW_SSI_CTRLR0_DFS_32_8BIT   (0x7 << 16)
#define DW_SSI_CTRLR0_DFS_32_16BIT  (0xf << 16)
#define DW_SSI_CTRLR0_DFS_32_24BIT  (0x17 << 16)
#define DW_SSI_CTRLR0_DFS_32_32BIT  (0x1f << 16)
#define DW_SSI_CTRLR0_CFS_MASK      (0xf << 12) //Control Frame Size (length of control word of microwire frame format (1-16bits))
#define DW_SSI_CTRLR0_SRL           (1 << 11)   //shift register loop (only for testing, otherwise clear)
#define DW_SSI_CTRLR0_SLV_OE        (1 << 10)   //slave output enable (only if the dw_apb_ssi is a slave serial device)
#define DW_SSI_CTRLR0_TMOD_MASK     (3 << 8)    //transfer mode (set to 0b00 for transmit and receive to be valid, 01 is transmit only, 10 is receive only, 11 is EEPROM read)
#define DW_SSI_CTRLR0_TMOD_TX       (1 << 8)
#define DW_SSI_CTRLR0_TMOD_RX       (1 << 9)
#define DW_SSI_CTRLR0_SCPOL         (1 << 7)    //serial clock polarity (0 low, 1 high)
#define DW_SSI_CTRLR0_SCPH          (1 << 6)    //serial clock phase (0 middle, 1 start)
#define DW_SSI_CTRLR0_FRF_MASK      (3 << 4)    //frame format (0: motorola spi; 1: texas_ssp; 2: ns_microwire; 3: reserved)
#define DW_SSI_CTRLR0_DFS_MASK      (0xFF)      //DATA FRAME SIZE: volid if SSI_MAX_XFER_SIZE is 16. 0x7: 8bit; 0xf: 16bit (and everything between 4 and 16 bits)
#define DW_SSI_CTRLR0_DFS_8BIT      (0x7)
#define DW_SSI_CTRLR0_DFS_16BIT     (0xf)
//DW_SSI_CTRLR1
//reserved: 31:16
#define DW_SSI_CTRLR1_NDF_MASK      (0xFFFF)    //Number of data frames. if TMOD = 10 or 11 for continuous receive reg_value + 1 frames (up to 64kB)

//DW_SSI_SSIENR
//reserved: 31:1
#define DW_SSI_SSIENR_SSI_ENABLE    (1 << 0)    //enables/disables all operation
#define DW_SSI_SSIENR_SSI_DISABLE   0

//DW_SSI_MWCR
//reserved: 31:3
#define DW_SSI_MWCR_MHS             (1 << 2)    //microwire handshaking
#define DW_SSI_MWCR_MDD             (1 << 1)    //microwire control (0: receive; 1: transmit)
#define DW_SSI_MWCR_MWMOD           (1 << 0)    //microwire transfer mode (0: non_seqiential; 1: sequential)

//DW_SSI_SER
//reserved: 31:y
// x:0                                          //slave select enable flag
#define DW_SSI_SER_CS_0             (1 << 0)    //chip select 1
#define DW_SSI_SER_CS_1             (1 << 1)    //chip select 2
#define DW_SSI_SER_CS_2             (1 << 2)    //chip select 3
#define DW_SSI_SER_CS_3             (1 << 3)    //chip select 4

//DW_SSI_BAUDR
//reserved: 31:16
#define DW_SSI_BAUDR_SCKDV_MASK     0xFFFFFFFF  //lsb always 0 --> even value SCKDV is between 2 and 65534

//DW_SSI_TXFTLR
//reserved: 31:y
#define DW_SSI_TXFTLR_TFT           //controls level of entries at which transmit FIFO triggers interrupt.

//DW_SSI_RXFTLR
//reserved: 31:y
#define DW_SSI_RXFTLR_TFT           //controls level of entries at which receive FIFO triggers interrupt.

//DW_SSI_TXFLR
//reserved: 31:y
#define DW_SSI_TXFLR_TXTFL           //Contains the number of valid data entries in the transmit FIFO

//DW_SSI_RXFLR
//reserved: 31:y
#define DW_SSI_RXFLR_RXTFL           //Contains the number of valid data entries in the receive FIFO


//DW_SSI_SR
//reserved: 31:7
#define DW_SSI_SR_DCOL              (1 << 6)    //data collision error
#define DW_SSI_SR_TXE               (1 << 5)    //transmission error
#define DW_SSI_SR_RFF               (1 << 4)    //receive FIFO full
#define DW_SSI_SR_RFNE              (1 << 3)    //receive FIFO not empty
#define DW_SSI_SR_TFE               (1 << 2)    //transmit FIFO empty
#define DW_SSI_SR_TFNF              (1 << 1)    //transmit FIFO not full
#define DW_SSI_SR_BUSY              (1 << 0)    //serial transfer is in progress

//DW_SSI_DMACR
//reserved: 31:2
#define DW_SSI_DMACR_TDMAE          (1 << 1)    //transmit DMA enable
#define DW_SSI_DMACR_RDMAE          (1 << 0)    //receive DMA enable


//DW_SSI_SPI_CTRL0
//reserved: 31:19
#define DW_SSI_SPI_CTRL0_            (1 << 18)   //Read data strobe enable bit
// ...












#endif
