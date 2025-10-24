
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EEMEMORYMAP_SHAREDSERVICES_DUALCORE_H_
#define __EEMEMORYMAP_SHAREDSERVICES_DUALCORE_H_

#if 0
	this contains the default macros needed by the shared services group w/ eEsharedServices.c and related files
#endif


#if !defined(STM32HAL_dualcore_BOOT_cm7master) && !defined(STM32HAL_dualcore_BOOT_cm4master)
	#error use this file only for a dualcore project
#endif

// macros used by EENV that must stay as they are

// the offset in flash after the start of teh partition in which the eEmoduleInfo_t data must be placed
#define EENV_MODULEINFO_OFFSET                      (1024)

// the location in eeprom where the storage is placed.
// 0
// 8k
// 2k
// 26
#define EENV_STGSTART                               (0)
#define EENV_STGSIZE                                (8*1024)
#define EENV_MEMMAP_SHARSERV_STGSIZE                (2*1024)
#define EENV_MEMMAP_SHARSERV_STGADDR                (EENV_STGSTART + EENV_STGSIZE - EENV_MEMMAP_SHARSERV_STGSIZE)

//static_assert(EENV_MEMMAP_SHARSERV_STGADDR == (6*1024), "");

// placed at 6k in eeprom. PARTition table of the processes
// it contains the partTableStorage_t w/ all the info about the processes and their boot modes
// it is what the FirmwareUpdater cares about
#define EENV_MEMMAP_SHARSERV_PART_STGSIZE           (1024)
#define EENV_MEMMAP_SHARSERV_PART_STGADDR           (EENV_MEMMAP_SHARSERV_STGADDR)

// placed at 7k in eeprom. various INFOrmation about the board
// it contains info such as: eEboardInfo_t, the MAC and IP address and others not used (such as application specific storage)
#define EENV_MEMMAP_SHARSERV_INFO_STGSIZE           (768)
#define EENV_MEMMAP_SHARSERV_INFO_STGADDR           (EENV_MEMMAP_SHARSERV_PART_STGADDR + EENV_MEMMAP_SHARSERV_PART_STGSIZE)

// placed at 7k+768 in eeprom. user specific information
#define EENV_MEMMAP_SHARSERV_OWN_STGSIZE            (256)
#define EENV_MEMMAP_SHARSERV_OWN_STGADDR            (EENV_MEMMAP_SHARSERV_INFO_STGADDR + EENV_MEMMAP_SHARSERV_INFO_STGSIZE)

// not used because SHARSERV_MODE_SHALIB is undefined (thus, no shared libraries in flash)
#define EENV_MEMMAP_SHARSERV_ROMADDR 0
#define EENV_MEMMAP_SHARSERV_ROMSIZE 0
#define EENV_MEMMAP_SHARSERV_RAMADDR 0
#define EENV_MEMMAP_SHARSERV_RAMSIZE 0

// used just for default values of the board properties, but in dualcore boards we have more banks, so keep them 0 and 0
#define EENV_RAMSTART 0
#define EENV_RAMSIZE 0
#define EENV_ROMSTART 0
#define EENV_ROMSIZE 0 

// placement of IPC ram used by bootstrapping MPU. the size is fixed to be 64. the address depends on the MPU
#define EENV_MEMMAP_SHARSERV_BASE_RAMSIZE (64)
#if defined(STM32HAL_dualcore_BOOT_cm7master)
// we place it on top of SRAM1SRAM2 @ 0x1000000 that is 256k. caveat: the scatter file must use this ram up to 0x1003FFC0
#define EENV_MEMMAP_SHARSERV_BASE_RAMADDR (0x10000000+256*1024-EENV_MEMMAP_SHARSERV_BASE_RAMSIZE)
#define EENV_SHARSERV_BASE_RAMADDR_AT ".ARM.__at_0x1003FFC0"
static_assert(EENV_MEMMAP_SHARSERV_BASE_RAMADDR == 0x1003FFC0, "");
#elif defined(STM32HAL_dualcore_BOOT_cm4master)
// we place it on top of SRAM1SRAM2 @ 0x1000000 that is 256k. caveat: the scatter file must use this ram up to 0x1003FFC0
#define EENV_MEMMAP_SHARSERV_BASE_RAMADDR (0x10000000+256*1024-EENV_MEMMAP_SHARSERV_BASE_RAMSIZE)
#define EENV_SHARSERV_BASE_RAMADDR_AT ".ARM.__at_0x1003FFC0"
static_assert(EENV_MEMMAP_SHARSERV_BASE_RAMADDR == 0x1003FFC0, "");
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



