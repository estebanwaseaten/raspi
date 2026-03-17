// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */



#ifndef raspi_spi_h
#define raspi_spi_h

#include <linux/ioctl.h>	//needed for RSPI_IOC_MESSAGE
#include <asm/types.h>


#define SET_BIT(p,mask) ((p) |= (mask))
#define CLR_BIT(p,mask) ((p) &= (~mask))
#define CHK_BIT(p,mask) (((p) & (mask)) >= mask)




//#define RSPI_IOC_MESSAGE(N) (N*sizeof(struct rspi_ioc_transfer))
struct rspi_ioc_transfer {
	uint64_t		tx_buf;
	uint64_t		rx_buf;

	uint32_t		len;
	uint32_t		speed_hz;

	uint16_t		delay_usecs;
	uint8_t			bits_per_word;
	uint8_t			cs_change;
	uint32_t		pad;
};

#define RSPI_IOC_MAGIC			'k'

#define RSPI_MSGSIZE(N) \
	((((N)*(sizeof (struct rspi_ioc_transfer))) < (1 << _IOC_SIZEBITS)) \
		? ((N)*(sizeof (struct rspi_ioc_transfer))) : 0)
#define RSPI_IOC_MESSAGE(N) _IOW(RSPI_IOC_MAGIC, 0, char[RSPI_MSGSIZE(N)])

//borrowed from spidev.h:
/* Read / Write of SPI mode (SPI_MODE_0..SPI_MODE_3) (limited to 8 bits) */
#define RSPI_IOC_RD_MODE			_IOR(RSPI_IOC_MAGIC, 1, __u8)
#define RSPI_IOC_WR_MODE			_IOW(RSPI_IOC_MAGIC, 1, __u8)

/* Read / Write SPI bit justification */
#define RSPI_IOC_RD_LSB_FIRST		_IOR(RSPI_IOC_MAGIC, 2, __u8)
#define RSPI_IOC_WR_LSB_FIRST		_IOW(RSPI_IOC_MAGIC, 2, __u8)

/* Read / Write SPI device word length (1..N) */
#define RSPI_IOC_RD_BITS_PER_WORD	_IOR(RSPI_IOC_MAGIC, 3, __u8)
#define RSPI_IOC_WR_BITS_PER_WORD	_IOW(RSPI_IOC_MAGIC, 3, __u8)

/* Read / Write SPI device default max speed hz */
#define RSPI_IOC_RD_MAX_SPEED_HZ		_IOR(RSPI_IOC_MAGIC, 4, __u32)
#define RSPI_IOC_WR_MAX_SPEED_HZ		_IOW(RSPI_IOC_MAGIC, 4, __u32)

/* Read / Write of the SPI mode field */
#define RSPI_IOC_RD_MODE32		_IOR(RSPI_IOC_MAGIC, 5, __u32)
#define RSPI_IOC_WR_MODE32		_IOW(RSPI_IOC_MAGIC, 5, __u32)

//borrowed from <linux/spi/spidev> Copyright (C) 2006 SWAPP Andrea Paterniani <a.paterniani@swapp-eng.it>
#define RSPI_CPHA		0x01
#define RSPI_CPOL		0x02

#define RSPI_MODE_0		(0|0)
#define RSPI_MODE_1		(0|RSPI_CPHA)
#define RSPI_MODE_2		(RSPI_CPOL|0)
#define RSPI_MODE_3		(RSPI_CPOL|RSPI_CPHA)

#define RSPI_CS_HIGH		0x04
#define RSPI_LSB_FIRST		0x08
#define RSPI_3WIRE		0x10
#define RSPI_LOOP		0x20
#define RSPI_NO_CS		0x40
#define RSPI_READY		0x80



#endif
