// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

 #include "raspi_gpio_dwapbssi.h"

 //regWrite( 0x044, 128 );


void gpio_dwapbssi::regBackup()
{
    regRead( 0x044, &backupRegs[0] );     //backs up following config
}

void gpio_dwapbssi::regRestore()
{
    regWrite( 0x044, backupRegs[0] );      //restores backup
}

void gpio_dwapbssi::setSettings()
{
    regWrite( 0x044, 128 );               //enables chip select 0 pin
}
