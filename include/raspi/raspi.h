// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

#ifndef raspi_h
#define raspi_h

#include <filesystem>
#include <string>
#include <vector>

#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <sys/mman.h>

#include <devicetree/devicetree.h>

//from pigpio - using the device tree tto get those values...
//static volatile uint32_t pi_peri_phys  = 0x20000000;
// BCM2835:  pi_peri_phys = 0x20000000;
// BCM2835:  pi_peri_phys = 0x20000000;
// BCM2836 and BCM2837: pi_peri_phys = 0x3F000000;
// BCM2711: pi_peri_phys = 0xFE000000;
// BCM2712: pi_peri_phys = ??
//#define PI_PERI_PHYS 0xFE000000 // 0xFC000000 according to manual
//#define SPI_BASE ( PI_PERI_PHYS + 0x00204000 )
//#define SPI_LEN 0x18


using namespace std;

#define RASPI_SPI_METHOD_NONE 0
#define RASPI_SPI_METHOD_DRA 1          //direct register access
#define RASPI_SPI_METHOD_SPIDEV 2       //spidev

#define DEVICE_TYPE_SPI 1

//offset into each peripheral register block RP1 (raspberry pi 5 peripheral chip)
#define RP1_RW_OFFSET			 0x0000             //normal read/write
#define RP1_XOR_OFFSET			 0x1000          // atomic XOR on write. read does not advance RF/RWF regs --> debugging FIFO
#define RP1_SET_OFFSET			 0x2000     // atomic bitmask set on write
#define RP1_CLR_OFFSET			 0x3000    // atomic bitmask clear on write


enum SoC {  // P[0]="2835"; P[1]="836"; P[2]="2837"; P[3]="2711"
  bcmUNKNOWN,       //--> use spidev or?
  bcm2835,
  bcm2836,
  bcm2837,
  bcm2711,          //raspi4
  bcm2712           //raspi5
};

enum spiHandles {
    SPI_HANDLE_NONE,
    SPI_HANDLE_A,
    SPI_HANDLE_B
};

int getSOC();


class raspi_info
{
public:
    raspi_info();
    ~raspi_info();

    int         addDevice( string name, uint32_t devType  );

    uint64_t    getDeviceBaseAddr( int dev );
    uint64_t    getDeviceSize( int dev );

    int         deviceDumpRegs( int dev );
    int         deviceDumpRegs( int dev, int offset, int count );
    int         deviceRegWrite( int dev, uint32_t offset, uint32_t value );
    int         deviceRegRead( int dev, uint32_t offset, uint32_t *value );

    void        init_spidev();
    void        init_dra();
    void        close_spidev();
    void        setSpeed( uint32_t speed_Hz ); //needed?
    void        setSpiTransferStruct();
    int         spidev_set_handle( int handle );

    int         spiTransfer( uint8_t *bytes, uint32_t byteCount );
    int         pioctl( int deviceIndex, uint64_t cmd, void *arg );  //communication forwarded to the device
    int         spictl( uint64_t cmd, void *arg );

    bool        canAccesDMA(){ return mem_handle >= 0; };
    int         getSoC();

private:
    int             SoC;
    vector<device*> myDevices;
    int             mem_handle;

    int         spi_devnum = -1;
    int         gpio_devnum = -1;


    uint32_t    backupRegs[10];


    // SPI subsystem
    int     method = RASPI_SPI_METHOD_NONE;
    bool    spi_valid = false;

    // using spidev:
    int spi_handle_A;
    int spi_handle_B;
    int spi_handle_select;

    const char *devicePathA = "/dev/spidev0.0";			//device path     could change depending on...
    const char *devicePathB = "/dev/spidev1.0";			//device path to the other chip select/enable of

    uint8_t mode = SPI_MODE_0;
    uint32_t speed = 1600000;		// maximum: 10.000.000		--> transfer rate of  931,51 kbytes/s	(only for selftest)
                                    // mcp6201 => datasheet max speed: 1600000 (1.6 MHz)
                                    // 1.000.000 => 24 kHz DAQ at 3.3V                              24 ksps
                                    // 5.000.000 => 80 kHz      seems to be the sweet spot for 3.3V 76 ksps
                                    // 10.000.000 => 100 kHz seems to work, but is already a bit strange
                                    // 20.000.000 => strongly reduced intensity, not really more datapoints acquired
    uint16_t delay = 0;
    uint8_t bitsPerWord = 8;


    //using dma or direct register access:



    // can we use this for dma?
    struct spi_ioc_transfer mySpiTransfer;		//has to be initialized to zeros!

    uint8_t    bytesIn[2];      //referenced from spiTransfer structure
    uint8_t    bytesOut[2];     //referenced from spiTransfer structure

};



#endif
