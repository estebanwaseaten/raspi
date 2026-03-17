// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

 #include "raspi_spi_bcm2835.h"
 #include "raspi_spi_bcm2835_regs.h"

spi_bcm2835::~spi_bcm2835()
{

}

void spi_bcm2835::regBackup()
{
    regRead( 0x0, &backupRegs[0] );
    regRead( 0x8, &backupRegs[2] );
}

void spi_bcm2835::regRestore()
{
    regWrite( 0x0, backupRegs[0] );
    regWrite( 0x8, backupRegs[2] );
}


void spi_bcm2835::setSettings()
{
    volatile uint32_t *spiDevice_base_reg = (uint32_t*)getVirtualReg();

    CLR_BIT(spiDevice_base_reg[0], BCM2835_SPI_REG_CS_CSPOL0);
    CLR_BIT(spiDevice_base_reg[0], BCM2835_SPI_REG_CS_CSPOL1);
    CLR_BIT(spiDevice_base_reg[0], BCM2835_SPI_REG_CS_CSPOL2);
    CLR_BIT(spiDevice_base_reg[0], BCM2835_SPI_REG_CS_CSPOL);

    CLR_BIT(spiDevice_base_reg[0], BCM2835_SPI_REG_CS_INTR); //turn off interrupt
    CLR_BIT(spiDevice_base_reg[0], BCM2835_SPI_REG_CS_INTD); //turn off interrupt

    //clock polarity and phase:
    CLR_BIT(spiDevice_base_reg[0], BCM2835_SPI_REG_CS_CPOL);
    CLR_BIT(spiDevice_base_reg[0], BCM2835_SPI_REG_CS_CPHA);

    //turn off DMA
    CLR_BIT(spiDevice_base_reg[0], BCM2835_SPI_REG_CS_DMAEN);

    //use CS0
    CLR_BIT(spiDevice_base_reg[0], BCM2835_SPI_REG_CS_CS0_MASK);

    spiDevice_base_reg[2] = 2 << 6; //speed
}






//( SPI_IOC_MESSAGE(1), &spiTransfer )
int spi_bcm2835::pioctl( uint64_t cmd, void *arg ) //should be implemted by driver
{
    //cout << "bcm2835 spi ioctl - supported. work in progress." << endl;

    //cout << "_IOC_TYPE: " << _IOC_TYPE(cmd) << endl;    //--> thats the test magic character
    if ( _IOC_TYPE(cmd) != RSPI_IOC_MAGIC )     //message was not meant for us
    {
        return -1;
    }

    //cout << "_IOC_DIR: " << _IOC_DIR(cmd) << endl;      //direction is 1 for write and 2 for read

    //cout << "_IOC_NR: " << _IOC_NR(cmd) << endl;        // "command nr" IOC_MESSAGE = 0
    //cout << "_IOC_SIZE: " << _IOC_SIZE(cmd) << endl;    // bytes, I think


    switch ( _IOC_NR(cmd) ) {
        case 0: //do data transfer
            doTransfer( (rspi_ioc_transfer*)arg, _IOC_SIZE(cmd) );
            break;
        case 1:         //read/set mode
            break;
        case 2:         //
            break;
        case 3:         //RSPI_IOC_WR_BITS_PER_WORD

            break;
        case 4:         //RSPI_IOC_WR_MAX_SPEED_HZ
            break;
        default:
            return -1;
            break;
    }




    return 0;
}

void spi_bcm2835::doTransfer( rspi_ioc_transfer *transfer, uint8_t size )
{
    volatile uint32_t *spiDevice_base_reg = (uint32_t*)getVirtualReg();

    __sync_synchronize();
    //dumpRegs();

    SET_BIT(spiDevice_base_reg[0], BCM2835_SPI_REG_CS_TA);

    volatile uint8_t *txPtr = (uint8_t*)transfer->tx_buf;
    volatile uint8_t *rxPtr = (uint8_t*)transfer->rx_buf;

    int i = 0;
    int j = 0;

    while( !( CHK_BIT( spiDevice_base_reg[0], BCM2835_SPI_REG_CS_DONE ) && (i == transfer->len) && (j == transfer->len)) )
    {
        //if fifo can accept data: insert
        if( CHK_BIT( spiDevice_base_reg[0], BCM2835_SPI_REG_CS_TXD ) && i < transfer->len)  //wait until FIFO can except data
        {
            spiDevice_base_reg[1] = txPtr[i];
            i++;
        }

        //if fifo has data: extract
        if( CHK_BIT(spiDevice_base_reg[0], BCM2835_SPI_REG_CS_RXD ) && j < transfer->len )
        {
            rxPtr[j] = spiDevice_base_reg[1];
            j++;
        }
        //cout << dec << endl << "i: " << i << " j: " << j << endl;
    }

    CLR_BIT(spiDevice_base_reg[0], BCM2835_SPI_REG_CS_TA);

    __sync_synchronize();
}


void spi_bcm2835::testTransfer()
{
    //ACCESS only works if spi is turned ON(???) by sudo rapsi-config!!!! mapping should be fine...
    cout << "attempting test transfer: " << endl;

    // send data in transfer.tx_buf and read into transfer.rx_buf
    // all other info should also be in transfer struct...

    this->printMemPtr(); //does not seem to access the correct class...

    //direct register access: spi interface starts at "myBusAddr"!

    volatile uint32_t *spiDevice_base_reg = (uint32_t*)getVirtualReg();
    //just fooling around for now
    cout << "device virtual memory pointer: " << (uint32_t*)spiDevice_base_reg << endl;       //not sure where the minus is lost...

/* 10.6.1 Polled
a) Set CS, CPOL, CPHA as required and set TA = 1.
b) Poll TXD writing bytes to SPI_FIFO, RXD reading bytes from SPI_FIFO until all data written.
c) Poll DONE until it goes to 1.
d) SetTA=0.

        says ...writing BYTES...
*/


    __sync_synchronize();
    spiDevice_base_reg[0] = 0;
    spiDevice_base_reg[1] = 0;
    spiDevice_base_reg[2] = 0;//1 << 7;   //clock
    spiDevice_base_reg[3] = 0;  //obly used with dma
    spiDevice_base_reg[4] = 0;
    spiDevice_base_reg[5] = 0;

    dumpRegs();
    cout << endl;

    //spiDevice_base_reg[0] |= BCM2835_SPI_REG_CS_LEN_LONG;
    spiDevice_base_reg[0] |= BCM2835_SPI_REG_CS_TA; //set transfer active
    spiDevice_base_reg[0] |= BCM2835_SPI_REG_CS_ADCS; // auto deassert
    spiDevice_base_reg[1] = 0xFF; //write to FIFO - only seems to receive the last byte
    spiDevice_base_reg[1] = 0xFF; //write to FIFO

    //FIFO needs to be filled byte by bytes

    int i = 0;
    while( !( spiDevice_base_reg[0] & BCM2835_SPI_REG_CS_DONE) )
    {
        if (i == 0 ) {
            //dumpRegs();
            cout << endl;
        }
            i++;
    }


    cout << "count: " << i << " FIFO: " << spiDevice_base_reg[1] << endl;
    cout << "FIFO2: " << spiDevice_base_reg[1] << endl;
    cout << "FIFO3: " << spiDevice_base_reg[1] << endl;
    cout << "FIFO4: " << spiDevice_base_reg[1] << endl;
    cout << "FIFO5: " << spiDevice_base_reg[1] << endl;



    dumpRegs();

    __sync_synchronize();

}
