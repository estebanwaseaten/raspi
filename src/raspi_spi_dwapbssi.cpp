// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

 #include "raspi_spi_dwapbssi.h"
 #include "raspi_spi_dwapbssi_regs.h"


spi_dwapbssi::~spi_dwapbssi()
{

}


void spi_dwapbssi::regBackup()
{
    /*for (size_t i = 0; i < myRegisterCount; i++)
    {
        regRead( i*4, &backupRegs[i] );
    }*/
    regRead( DW_SSI_SSIENR, &backupRegs[0] );
    regRead( DW_SSI_CTRLR0, &backupRegs[1] );
    regRead( DW_SSI_BAUDR, &backupRegs[2] );
    regRead( DW_SSI_TXFTLR, &backupRegs[3] );
    regRead( DW_SSI_RXFTLR, &backupRegs[4] );
    regRead( DW_SSI_SER, &backupRegs[5] );
}

void spi_dwapbssi::regRestore()
{
    regWrite( DW_SSI_SSIENR, backupRegs[0] );
    regWrite( DW_SSI_CTRLR0, backupRegs[1] );
    regWrite( DW_SSI_BAUDR, backupRegs[2] );
    regWrite( DW_SSI_TXFTLR, backupRegs[3] );
    regWrite( DW_SSI_RXFTLR, backupRegs[4] );
    regWrite( DW_SSI_SER, backupRegs[5] );
}

void spi_dwapbssi::setSettings()
{
    //dumpRegs();
    //abort();

    volatile uint32_t *spiDevice_base_reg = (uint32_t*)getVirtualReg();


    spiDevice_base_reg[DW_SSI_SSIENR/4] = DW_SSI_SSIENR_SSI_DISABLE;    //make sure its disabled

    CLR_BIT( spiDevice_base_reg[DW_SSI_CTRLR0/4], DW_SSI_CTRLR0_SSTE );             //chip select toggles high between data frames

    CLR_BIT(spiDevice_base_reg[DW_SSI_CTRLR0/4], DW_SSI_CTRLR0_SPI_FRF_MASK);       //standard SPI format

    CLR_BIT(spiDevice_base_reg[DW_SSI_CTRLR0/4], DW_SSI_CTRLR0_TMOD_MASK);          //transmit and receive

    CLR_BIT( spiDevice_base_reg[DW_SSI_CTRLR0/4], DW_SSI_CTRLR0_SCPOL );            //inactive low
    SET_BIT( spiDevice_base_reg[DW_SSI_CTRLR0/4], DW_SSI_CTRLR0_SCPH );             //0: clck toggles in middle, 1: clck toggles at start

    CLR_BIT(spiDevice_base_reg[DW_SSI_CTRLR0/4], DW_SSI_CTRLR0_FRF_MASK);           //selects MOTOROLA SPI

    CLR_BIT(spiDevice_base_reg[DW_SSI_CTRLR0/4], DW_SSI_CTRLR0_DFS_MASK);
    SET_BIT( spiDevice_base_reg[DW_SSI_CTRLR0/4], DW_SSI_CTRLR0_DFS_16BIT );

    spiDevice_base_reg[DW_SSI_BAUDR/4] = 126;
    spiDevice_base_reg[DW_SSI_BAUDR/4] = 126/2;

    //5. set FIFO threshold: TXFTLR and RXFTLR
    spiDevice_base_reg[DW_SSI_TXFTLR/4] = 2;
    spiDevice_base_reg[DW_SSI_RXFTLR/4] = 2;

    spiDevice_base_reg[DW_SSI_SER/4] = DW_SSI_SER_CS_0;

}

//( SPI_IOC_MESSAGE(1), &spiTransfer )

int spi_dwapbssi::pioctl( uint64_t cmd, void *arg ) //should be implemted by driver
{
    //cout << "spiDevice ioctl - supported. work in progress." << endl;
    //cout << "\tcmd: " << cmd << endl;
    //cout << "\targ: " << arg << endl;

    volatile uint32_t *spiDevice_base_reg = (uint32_t*)getVirtualReg();

    switch ( cmd ) {
        case RSPI_IOC_MESSAGE(1):
            doTransfer( (rspi_ioc_transfer*)arg, _IOC_SIZE(cmd) );
            break;
        case RSPI_IOC_WR_MAX_SPEED_HZ:
            //

            /*spiDevice_base_reg[DW_SSI_SSIENR/4] = DW_SSI_SSIENR_SSI_DISABLE;

            CLR_BIT( spiDevice_base_reg[DW_SSI_CTRLR0/4], DW_SSI_CTRLR0_SCPOL );
            SET_BIT( spiDevice_base_reg[DW_SSI_CTRLR0/4], DW_SSI_CTRLR0_SCPH );
            CLR_BIT(spiDevice_base_reg[DW_SSI_CTRLR0/4], DW_SSI_CTRLR0_TMOD_MASK);            //transmit and receive

            spiDevice_base_reg[DW_SSI_BAUDR/4] = 32;

            //5. set FIFO threshold: TXFTLR and RXFTLR
            spiDevice_base_reg[DW_SSI_TXFTLR/4] = 2;
            spiDevice_base_reg[DW_SSI_RXFTLR/4] = 2;

            spiDevice_base_reg[DW_SSI_SER/4] = DW_SSI_SER_CS_1;*/
            break;

        default:
            break;
    }

    return 0;
}



void spi_dwapbssi::doTransfer( rspi_ioc_transfer *transfer, uint8_t size )
{
    volatile uint32_t *spiDevice_base_reg = (uint32_t*)getVirtualReg();

    volatile uint8_t *txPtr = (uint8_t*)transfer->tx_buf;
    volatile uint8_t *rxPtr = (uint8_t*)transfer->rx_buf;


    //8. enable SSIENR
    spiDevice_base_reg[DW_SSI_SSIENR/4] = DW_SSI_SSIENR_SSI_ENABLE;

    //9. write data
    //spiDevice_base_reg[DW_SSI_DR0/4] = transfer;
    //spiDevice_base_reg[DW_SSI_DR0/4] = transfer;

    //10. poll BUSY & read FIFO (interrupts for larger amounts of data?)
    bool done = false;
    int txCount = 0;
    int rxCount = 0;
    while( !done )
    {
        if( CHK_BIT(spiDevice_base_reg[DW_SSI_SR/4], (DW_SSI_SR_TFNF)) && (txCount < transfer->len) )
        {
            spiDevice_base_reg[DW_SSI_DR0/4] = txPtr[txCount];
            txCount++;
        }

        if( CHK_BIT(spiDevice_base_reg[DW_SSI_SR/4], (DW_SSI_SR_RFNE)) && (rxCount < transfer->len) )
        {
            rxPtr[rxCount] = spiDevice_base_reg[DW_SSI_DR0/4];
            rxCount++;
        }


        /*
        while( CHK_BIT(spiDevice_base_reg[DW_SSI_SR/4], (DW_SSI_SR_TFNF)) && (txCount < transfer->len) )
        {
            spiDevice_base_reg[DW_SSI_DR0/4] = txPtr[txCount];
            txCount++;
        }

        while( CHK_BIT(spiDevice_base_reg[DW_SSI_SR/4], (DW_SSI_SR_RFNE)) && (rxCount < transfer->len) )
        {
            rxPtr[rxCount] = spiDevice_base_reg[DW_SSI_DR0/4];
            rxCount++;
        }*/

        if ( (txCount >= transfer->len) && (rxCount >= transfer->len) ) //&& !CHK_BIT(spiDevice_base_reg[DW_SSI_SR/4], (DW_SSI_SR_BUSY)) )
        {
            done = true;
        }
    }

    //11. disable SSIENR again
    //spiDevice_base_reg[DW_SSI_SSIENR/4] = DW_SSI_SSIENR_SSI_DISABLE;
}



void spi_dwapbssi::testTransfer()
{
    //cout << "attempting test transfer: " << endl;

    volatile uint32_t *spiDevice_base_reg = (uint32_t*)getVirtualReg();
    //cout << "device virtual memory pointer: " << (uint32_t*)spiDevice_base_reg << endl;       //not sure where the minus is lost...

    /*  1. disable dw_apb_ssi
        2. config master (CTRLR0, CTRLR1, BAUDR, TXFTLR, RXFTLR, IMR, SER ...)
        3. enable dw_apb_ssi
        4. write data to Tx FIFO
        5. wait until transfer is done (busy?)
        6. read Rx FIFO
    */


    //__sync_synchronize();


    //1.    //maybe calculate the needed pointers first?
    //1. disable SSIENR)
    spiDevice_base_reg[DW_SSI_SSIENR/4] = DW_SSI_SSIENR_SSI_DISABLE;

    //dumpRegs();

    //2. CTRLR0: set clock polarity and serial clock phase
    CLR_BIT( spiDevice_base_reg[DW_SSI_CTRLR0/4], DW_SSI_CTRLR0_SCPOL );
    SET_BIT( spiDevice_base_reg[DW_SSI_CTRLR0/4], DW_SSI_CTRLR0_SCPH );
    //SET_BIT( spiDevice_base_reg[DW_SSI_CTRLR0/4], DW_SSI_CTRLR0_TMOD_RX );          //receive only
    //or
    CLR_BIT(spiDevice_base_reg[DW_SSI_CTRLR0/4], DW_SSI_CTRLR0_TMOD_MASK);            //transmit and receive

    //3. receive only? -> CTRLLR1 number of frames minus 1 (??)

    //4. write baud rate BAUDR
    //base clock rate is 200 MHz --> we would like 1.6 MHz --> factor: 125 --> make it 128
    //spiDevice_base_reg[DW_SSI_BAUDR/4] = 88; // 88 seems to be the maximum didvide
    spiDevice_base_reg[DW_SSI_BAUDR/4] = 32;    //--> 125kHz


    //5. set FIFO threshold: TXFTLR and RXFTLR
    spiDevice_base_reg[DW_SSI_TXFTLR/4] = 2;
    spiDevice_base_reg[DW_SSI_RXFTLR/4] = 2;

    //(6. write IMR for interrupts)

    //7. write SER register to enable target chip for selection (transmission begins as soon as there is something in the FIFO)
    spiDevice_base_reg[DW_SSI_SER/4] = DW_SSI_SER_CS_1;

    //8. enable SSIENR
    spiDevice_base_reg[DW_SSI_SSIENR/4] = DW_SSI_SSIENR_SSI_ENABLE;

    //9. write data
    spiDevice_base_reg[DW_SSI_DR0/4] = 0xFF;
    spiDevice_base_reg[DW_SSI_DR0/4] = 0xF0;

    //10. poll BUSY & read FIFO (interrupts for larger amounts of data?)
    int i = 0;
    while( CHK_BIT(spiDevice_base_reg[DW_SSI_SR/4], (DW_SSI_SR_BUSY)) )
    {
        i++;
    }
    //cout << "i: " << i << "fifo1: " << spiDevice_base_reg[DW_SSI_DR0/4] << endl;



    //11. disable SSIENR again
    spiDevice_base_reg[DW_SSI_SSIENR/4] = DW_SSI_SSIENR_SSI_DISABLE;

    //__sync_synchronize();

//    abort();

}
