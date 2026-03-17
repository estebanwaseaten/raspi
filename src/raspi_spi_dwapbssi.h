// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */



#ifndef raspi_spi_dwapbssi_h
#define raspi_spi_dwapbssi_h



#include <devicetree/devicetree.h>
#include <raspi/raspi_spi.h>


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




/* not all platforms use <asm-generic/ioctl.h> or _IOC_TYPECHECK() ... *
#define SPI_MSGSIZE(N) \
	((((N)*(sizeof (struct spi_ioc_transfer))) < (1 << _IOC_SIZEBITS)) \
		? ((N)*(sizeof (struct spi_ioc_transfer))) : 0)
#define SPI_IOC_MESSAGE(N) _IOW(SPI_IOC_MAGIC, 0, char[SPI_MSGSIZE(N)]) /**/

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
//#define RSPI_IOC_RD_MODE32		_IOR(RSPI_IOC_MAGIC, 5, __u32)
//#define RSPI_IOC_WR_MODE32		_IOW(RSPI_IOC_MAGIC, 5, __u32)





class spi_dwapbssi : public device
{
public:
    spi_dwapbssi( string devName, bool verbose, uint32_t type, bool checkStatus ) : device( devName, verbose, type, checkStatus ){ cout << "spidevice constructor" << endl; myRegisterCount = mySize/4;};
    ~spi_dwapbssi();
    static string getCompatibility(){ return "snps,dw-apb-ssi"; };

    void regBackup();
    void regRestore();
    void setSettings();

	int pioctl( uint64_t cmd, void *arg );




private:
    void doTransfer( rspi_ioc_transfer *transfer, uint8_t size );
    void testTransfer();

    void init();


	rspi_ioc_transfer current_transfer;

	uint8_t 	spi_mode = RSPI_MODE_0;
	uint8_t		spi_lsbFirst = RSPI_LSB_FIRST;
	uint8_t		spi_bitsPerWord = 8;
	uint32_t	spi_maxSpeedHz = 100000;
};





#endif
