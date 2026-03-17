// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */



#ifndef raspi_spi_bcm2835_h
#define raspi_spi_bcm2835_h

#define DEVICE_TYPE_SPI 1

#include <devicetree/devicetree.h>
#include <raspi/raspi_spi.h>





class spi_bcm2835 : public device
{
public:
    spi_bcm2835( string devName, bool verbose, uint32_t type, bool checkStatus ) : device( devName, verbose, type, checkStatus )
    { cout << "bcm2835 constructor" << endl; myRegisterCount = mySize/4;};
    ~spi_bcm2835();

    static string getCompatibility(){ return "brcm,bcm2835-spi"; };

    void regBackup();
    void regRestore();
    void setSettings();


	int pioctl( uint64_t cmd, void *arg );


private:


    void doTransfer( rspi_ioc_transfer *transfer, uint8_t size );

    //debug:
	void testTransfer();

	rspi_ioc_transfer current_transfer;

	uint8_t 	spi_mode = RSPI_MODE_0;
	uint8_t		spi_lsbFirst = RSPI_LSB_FIRST;
	uint8_t		spi_bitsPerWord = 8;
	uint32_t	spi_maxSpeedHz = 100000;



};





#endif
