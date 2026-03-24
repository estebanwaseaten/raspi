// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

#include <iostream>
#include <fstream>    //ifstream

#include <fcntl.h>      // O_RDWR | O_SYNC etc
#include <unistd.h>     //close()

#include <raspi/raspi.h>

#include "raspi_spi_dwapbssi.h"
#include "raspi_spi_bcm2835.h"
#include "raspi_gpio_dwapbssi.h"
#include "raspi_gpio_bcm2835.h"

// everything should be more or less in
// https://github.com/raspberrypi/userland/blob/master/host_applications/linux/libs/bcm_host/bcm_host.c
// file:///C:/Users/danie/Downloads/devicetree-specification-v0.4.pdf

raspi_info::raspi_info()
{
    SoC = getSOC();            // try to fetch all needed information (extend this as needed). Start with the SoC:

    //this should be replaced by a kernel module, because it is considered unsafe:
    mem_handle = open("/dev/mem", O_RDWR | O_SYNC);
    if( mem_handle < 0 )
	{
		cout << "Direct memory access impossible without sudo. Using spidev instead." << endl;
        method = RASPI_SPI_METHOD_SPIDEV;
        init_spidev();
	}
    else
    {
        //deviceRegWrite( gpio_devnum, 0x044, 128 );  //enables chip select 0 pin
        //myDevices[gpio_devnum-1]->dumpRegs( 0x044, 8 );
        switch ( SoC )
        {
            case bcm2712:       //raspi5
                spi_devnum = addDevice( "spi0", DEVICE_TYPE_SPI );
                gpio_devnum = addDevice( "gpio0", 0 );

                myDevices[gpio_devnum-1]->regBackup();
                myDevices[gpio_devnum-1]->setSettings();

                myDevices[spi_devnum-1]->regBackup();
                myDevices[spi_devnum-1]->setSettings();

                method = RASPI_SPI_METHOD_DRA;
                init_dra();
                break;
            case bcm2711:       //raspi4
                spi_devnum = addDevice( "spi0", DEVICE_TYPE_SPI );
                gpio_devnum = addDevice( "gpio", 0 );

                myDevices[spi_devnum-1]->regBackup();
                myDevices[spi_devnum-1]->setSettings();

                myDevices[gpio_devnum-1]->regBackup();
                myDevices[gpio_devnum-1]->setSettings();

                method = RASPI_SPI_METHOD_DRA;
                init_dra();
                break;
            default:
                cout << "Hardware unknown. Using spidev instead." << endl;
        		method = RASPI_SPI_METHOD_SPIDEV;
                init_spidev();
                break;
        }
    }
}

raspi_info::~raspi_info()
{
    if ( method == RASPI_SPI_METHOD_DRA )
    {
        switch ( SoC )
        {
            case bcm2712:       //raspi5
                myDevices[spi_devnum-1]->regRestore();
                myDevices[gpio_devnum-1]->regRestore();
                break;
            case bcm2711:       //raspi4
                myDevices[spi_devnum-1]->regRestore();
                myDevices[gpio_devnum-1]->regRestore();
                break;
            default:
                break;
        }
    }

    //destroy all devices...
    for (size_t i = 0; i < myDevices.size(); i++)
    {
        delete myDevices[i];
    }

    if ( mem_handle >= 0)
    {
        close(mem_handle);
    }
}

int raspi_info::spidev_set_handle( int handle )
{
    if( handle == SPI_HANDLE_A )
    {
        if( spi_handle_A < 0 ) //not valid
        {
            return -1;
        }
        else
        {
            cout << "*** spidev set to handle A" << endl;
            spi_handle_select = spi_handle_A;
            spi_valid = true;
        }
    }
    else if( handle == SPI_HANDLE_B )
    {
        if ( spi_handle_B < 0 ) //not valid
        {
            return -1;
        }
        else
        {
            cout << "*** spidev set to handle B" << endl;
            spi_handle_select = spi_handle_B;
            spi_valid = true;
        }
    }
    else
    {
        cout << "*** spidev set to handle NONE" << endl;
        spi_handle_select = -1;
        spi_valid = false;
    }
    return 0;
}

void raspi_info::init_spidev()
{
	spi_handle_A = open( devicePathA, O_RDWR );
	cout << "*** fopen A response: " << spi_handle_A << endl;

    spi_handle_B = open( devicePathB, O_RDWR );
    cout << "*** fopen B response: " << spi_handle_B << endl;

    spi_handle_select = spi_handle_A;
	if( spi_handle_A < 0 )
	{
			cout << "could not open SPI device A" << endl;
            spi_handle_select = spi_handle_B;
			if( spi_handle_B < 0 )
			{
				cout << "could not open SPI device B" << endl;
                spi_handle_select = -1;
			}
			spi_valid = false;
			return;
	}

    if( spi_handle_B < 0 )
	{
			cout << "could not open SPI device B" << endl;
			spi_valid = false;
			return;
	}

	// set ioctl parameters to default values
	if( ioctl(spi_handle_A, SPI_IOC_WR_MODE, &mode) == -1 )
		cout << "error" << endl;

    if( ioctl(spi_handle_B, SPI_IOC_WR_MODE, &mode) == -1 )
		cout << "error" << endl;

	if( ioctl(spi_handle_A, SPI_IOC_WR_BITS_PER_WORD, &bitsPerWord) == -1 )
		cout << "error" << endl;

    if( ioctl(spi_handle_B, SPI_IOC_WR_BITS_PER_WORD, &bitsPerWord) == -1 )
		cout << "error" << endl;

	if(  ioctl(spi_handle_A, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1 )
		cout << "error" << endl;

	if(  ioctl(spi_handle_B, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1 )
		cout << "error" << endl;/**/

	setSpiTransferStruct();

	//channels = 1;
	spi_valid = true;
}

void raspi_info::close_spidev()
{
	if (spi_handle_A != -1)
	{
		close(spi_handle_A);
	}

    if (spi_handle_B != -1)
	{
		close(spi_handle_B);
	}
}


void raspi_info::init_dra()
{

	//maybe we try a parallel implementation of spidev functions emplying a fake ioctl and the same data structures and constants (defined in raspi.h and raspi_spi.h)
	if( spictl( RSPI_IOC_WR_MODE, &mode ) < 0 )
    {
        cout << "pioctl error setting RSPI_IOC_WR_MODE" << endl;
        return;
    }

	if( spictl( RSPI_IOC_WR_MODE, &mode ) < 0 )
    {
        cout << "pioctl error setting RSPI_IOC_WR_MODE" << endl;
        return;
    }

	if( spictl( RSPI_IOC_WR_BITS_PER_WORD, &bitsPerWord ) < 0 )
	{
		cout << "pioctl error setting RSPI_IOC_WR_BITS_PER_WORD" << endl;
        return;
	}

	if( spictl( RSPI_IOC_WR_BITS_PER_WORD, &bitsPerWord ) < 0 )
	{
		cout << "pioctl error setting RSPI_IOC_WR_BITS_PER_WORD" << endl;
        return;
	}

	if( spictl( RSPI_IOC_WR_MAX_SPEED_HZ, &speed ) < 0 )
	{
		cout << "pioctl error setting RSPI_IOC_WR_MAX_SPEED_HZ" << endl;
        return;
	}

	if( spictl( RSPI_IOC_WR_MAX_SPEED_HZ, &speed ) < 0 )
	{
		cout << "pioctl error setting RSPI_IOC_WR_MAX_SPEED_HZ" << endl;
        return;
	}

    setSpiTransferStruct();

	spi_valid = true;

	//testing:
	bytesIn[0] = 0x00;
	bytesIn[1] = 0x00;

	bytesOut[0] = 0;
	bytesOut[1] = 0;

	//myRaspi->pioctl( spi_devnum, RSPI_IOC_MESSAGE(1), &mySpiTransfer );
	//myRaspi->pioctl( spi_devnum, RSPI_IOC_MESSAGE(1), &mySpiTransfer );
	//myRaspi->spictl( RSPI_IOC_WR_MAX_SPEED_HZ, &speed );

	//abort();

	cout << "bytesOut[0]: " << hex << (uint16_t)bytesOut[0] << endl << "bytesOut[1]: " << hex << (uint16_t)bytesOut[1] << endl;
}

void raspi_info::setSpeed( uint32_t speed_Hz )
{
	speed = speed_Hz;
	setSpiTransferStruct();
}

void raspi_info::setSpiTransferStruct()
{
	//setup spi transfer structure: (init unspecified elements to zero)
	mySpiTransfer = {
			.tx_buf = (uint64_t)&bytesIn,
			.rx_buf = (uint64_t)&bytesOut,
			.len = 2,
			.speed_hz = speed,
			.delay_usecs = 0,
			.bits_per_word = bitsPerWord,
		};
}

int raspi_info::spiTransfer( uint8_t *bytes, uint32_t byteCount )
{

    mySpiTransfer.tx_buf = (uint64_t)bytes;
    mySpiTransfer.rx_buf = (uint64_t)bytes;
    mySpiTransfer.len = byteCount;

    switch( method )        //could replace this with a pointer to a function maybe?? is that faster
    {
        case RASPI_SPI_METHOD_DRA:
            return myDevices[spi_devnum-1]->pioctl( SPI_IOC_MESSAGE(1), &mySpiTransfer );
            break;
        case RASPI_SPI_METHOD_SPIDEV:
            return ioctl(spi_handle_select, SPI_IOC_MESSAGE(1), &mySpiTransfer);
            break;
        case RASPI_SPI_METHOD_NONE:
        default:
            return -1;
            break;
    }
}


/*
 * searches for device and adds it to the raspberry info.
 * also performs memory mapping.
 * also tries to find the correct driver/subclass
 */
int raspi_info::addDevice( string name, uint32_t devType )
{
    if( mem_handle < 0 )
    {
        return -1;
    }

    device *myDevice;
    bool checkStatus = false;
    myDevice = new device( name, false, devType, checkStatus );
    cout << "device: " << myDevice->getCompatible() << "." << endl;

    //cout << "drvr: " << spi_bcm2835::getCompatibility() << "." << endl;
    //cout << "drvr: " << spi_dwapbssi::getCompatibility() << "." << endl;

    if ( myDevice->getCompatible() == spi_bcm2835::getCompatibility() )
    {
        cout << "compatible with spi_bcm2835::" << endl;
        spi_bcm2835 *myNewDevice = new spi_bcm2835( name, false, devType, checkStatus );
        myNewDevice->mapMemory( mem_handle );
        myDevices.push_back( myNewDevice );
        delete myDevice;
    }
    else if ( myDevice->getCompatible() == spi_dwapbssi::getCompatibility() )
    {
        cout << "compatible with spi_dwapbssi::" << endl;
        spi_dwapbssi *myNewDevice = new spi_dwapbssi( name, false, devType, checkStatus );
        myNewDevice->mapMemory( mem_handle );
        myDevices.push_back( myNewDevice );
        delete myDevice;
    }
    else if ( myDevice->getCompatible() == gpio_bcm2835::getCompatibility() )
    {
        cout << "compatible with gpio_bcm2835::" << endl;
        gpio_bcm2835 *myNewDevice = new gpio_bcm2835( name, false, devType, checkStatus );
        myNewDevice->mapMemory( mem_handle );
        myDevices.push_back( myNewDevice );
        delete myDevice;
    }
    else if ( myDevice->getCompatible() == gpio_dwapbssi::getCompatibility() )
    {
        cout << "compatible with gpio_dwapbssi::" << endl;
        gpio_dwapbssi *myNewDevice = new gpio_dwapbssi( name, false, devType, checkStatus );
        myNewDevice->mapMemory( mem_handle );
        myDevices.push_back( myNewDevice );
        delete myDevice;
    }
    else
    {
        cout << "no compatible driver found" << endl;
        myDevice->mapMemory( mem_handle );
        myDevices.push_back( myDevice );
    }

    return myDevices.size();    //device number starting at 1
}



int raspi_info::spictl( uint64_t cmd, void *arg )
{
    if ( spi_devnum > myDevices.size() )
    {
        return -1;
    }
    return myDevices[spi_devnum-1]->pioctl( cmd, arg );
}

uint64_t raspi_info::getDeviceBaseAddr( int dev )
{
    if ( dev > myDevices.size() )
    {
        return -1;
    }
    return myDevices[dev-1]->getBaseAdress();
}

uint64_t raspi_info::getDeviceSize( int dev )
{
    if ( dev > myDevices.size() )
    {
        return -1;
    }
    return myDevices[dev-1]->getSizeBytes();
}

int raspi_info::getSoC()
{
    return SoC;
}

int getSOC()
{
    ifstream compatibility;
    vector<string> parameters;
    string currentSegment;
    string buffer;

    compatibility.open("/proc/device-tree/compatible");
    getline(compatibility,buffer);

    //extract null terminated strings from buffer:
    for (char c : buffer)
    {
       if (c == '\0')
       {
           parameters.push_back(currentSegment);
          // cout << currentSegment << endl;
           currentSegment.clear();
       }
       else
       {
           currentSegment += c;
       }
   }

   if (!currentSegment.empty()) {
       parameters.push_back(currentSegment);
   }

   //second entry should read brcm,bcmXXXX
   int pos = parameters[1].find_first_of(",", 0);
   string token = parameters[1].substr(pos+1, parameters[1].length());
   cout << "SoC: " << token << endl;

   if( token == "bcm2835" )
   {
       return bcm2835;
   }
   else if( token == "bcm2836" )
   {
       return bcm2836;
   }
   else if( token == "bcm2837" )
   {
       return bcm2837;
   }
   else if( token == "bcm2711" )
   {
       return bcm2711;
   }
   else if( token == "bcm2712" )
   {
       return bcm2712;
   }
   return bcmUNKNOWN;
}

// debug
int raspi_info::deviceRegWrite( int dev, uint32_t offset, uint32_t value )
{
    if ( dev > myDevices.size() )
    {
        return -1;
    }
    return myDevices[dev-1]->regWrite( offset, value );
}

int raspi_info::deviceRegRead( int dev, uint32_t offset, uint32_t *value )
{
    if ( dev > myDevices.size() )
    {
        return -1;
    }
    return myDevices[dev-1]->regRead( offset, value );
}

int raspi_info::deviceDumpRegs( int dev )
{

    if ( dev > myDevices.size() )
    {
        return -1;
    }
    cout << "base: 0x" << hex << myDevices[dev-1]->getBaseAdress() << " count: " << dec << myDevices[dev-1]->getSizeBytes()/4 << endl;
    myDevices[dev-1]->dumpRegs();
    return 0;
}

int raspi_info::deviceDumpRegs( int dev, int offset, int count )
{
    if ( dev > myDevices.size() )
    {
        return -1;
    }
    cout << "base: 0x" << hex << myDevices[dev-1]->getBaseAdress() << " count: " << dec << count << endl;
    myDevices[dev-1]->dumpRegs( offset, count );
    return 0;
}
