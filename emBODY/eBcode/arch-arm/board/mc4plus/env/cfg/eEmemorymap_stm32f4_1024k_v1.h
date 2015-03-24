/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/


// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EEMEMORYMAP_STM32F1_256K_V1_H_
#define _EEMEMORYMAP_STM32F1_256K_V1_H_


/** @file       eEmemorymap_stm32f1_256k_v1.h
    @brief      This header file defines the memory map for the ems001 board (v1).
    @author     marco.accame@iit.it
    @date       11/20/2013
**/

// - public #define  --------------------------------------------------------------------------------------------------

//#warning --> clean memory-map by: removing shalsys apart from shared ram used in ipc


// --- general properties
// --- mcbstm32c and ems001 have those common params. ems001 may have more storage size in eeprom

#define EENV_ROMSTART                               (0x08000000)
#define EENV_ROMSIZE                                (256*1024)

#define EENV_RAMSTART                               (0x20000000)
#define EENV_RAMSIZE                                (64*1024)

#define EENV_STGSTART                               (0)
#define EENV_STGSIZE                                (8*1024)


// - moduleinfo for a process or a shalib is shifted 512 byte after the entry point
#define EENV_MODULEINFO_OFFSET                      (512)


// --- memory mapping for shared-services. it may be used as a shared library or included in the code application.

#if     defined(SHARSERV_MODE_SHALIB)
    #error --> SHARSERV_MODE_SHALIB is not compatible with this memory mapping
#else

// does not use code. uses 64 bytes ram for base (ipc). uses 2k eeprom for partition table and for shared info
#define EENV_MEMMAP_SHARSERV_ROMSIZE                (0)
#define EENV_MEMMAP_SHARSERV_ROMADDR                (EENV_ROMSTART + EENV_ROMSIZE - EENV_MEMMAP_SHARSERV_ROMSIZE)
#define EENV_MEMMAP_SHARSERV_RAMSIZE                (64)
#define EENV_MEMMAP_SHARSERV_RAMADDR                (EENV_RAMSTART + EENV_RAMSIZE - EENV_MEMMAP_SHARSERV_RAMSIZE)
#define EENV_MEMMAP_SHARSERV_STGSIZE                (2*1024)
#define EENV_MEMMAP_SHARSERV_STGADDR                (EENV_STGSTART + EENV_STGSIZE - EENV_MEMMAP_SHARSERV_STGSIZE)

// now the modules of shared-services which require memory. only part and info. 1k eack 

// placed at 64k-64 or at 0x200FFC0 in ram
#define EENV_MEMMAP_SHARSERV_BASE_RAMSIZE           (64)
#define EENV_MEMMAP_SHARSERV_BASE_RAMADDR           (EENV_MEMMAP_SHARSERV_RAMADDR)

// placed at 6k
#define EENV_MEMMAP_SHARSERV_PART_STGSIZE           (1*1024)
#define EENV_MEMMAP_SHARSERV_PART_STGADDR           (EENV_MEMMAP_SHARSERV_STGADDR)

// placed at 7k
#define EENV_MEMMAP_SHARSERV_INFO_STGSIZE           (1*1024)
#define EENV_MEMMAP_SHARSERV_INFO_STGADDR           (EENV_MEMMAP_SHARSERV_PART_STGADDR + EENV_MEMMAP_SHARSERV_PART_STGSIZE)

#endif


// --- memory mapping for e-processes: eLoader, eUpdater, aand eApplication.

// --- eloader
#define EENV_MEMMAP_ELOADER_ROMSIZE                 (24*1024)
#define EENV_MEMMAP_ELOADER_ROMADDR                 (EENV_ROMSTART + 0)
#define EENV_MEMMAP_ELOADER_RAMSIZE                 (EENV_RAMSIZE-EENV_MEMMAP_SHARSERV_RAMSIZE)
#define EENV_MEMMAP_ELOADER_RAMADDR                 (EENV_RAMSTART) 


// --- eupdater
#ifdef EENV_EUPDATER_FORCE_CODE_OFFSET_TO_ZERO            
   #define EENV_MEMMAP_EUPDATER_ROMADDR             (EENV_ROMSTART + 0)              
#else
#define EENV_MEMMAP_EUPDATER_ROMADDR                (EENV_MEMMAP_ELOADER_ROMADDR + EENV_MEMMAP_ELOADER_ROMSIZE)
#endif
#define EENV_MEMMAP_EUPDATER_ROMSIZE                (72*1024)
#define EENV_MEMMAP_EUPDATER_RAMADDR                (EENV_RAMSTART) 
#define EENV_MEMMAP_EUPDATER_RAMSIZE                (EENV_RAMSIZE-EENV_MEMMAP_SHARSERV_RAMSIZE)


// --- eapplication
#ifdef EENV_EAPPLICATION_FORCE_CODE_OFFSET_TO_ZERO
	// the application starts from zero and can stay in all the flash           
   	#define EENV_MEMMAP_EAPPLICATION_ROMADDR         	(EENV_ROMSTART + 0)              
   	#define EENV_MEMMAP_EAPPLICATION_ROMSIZE       		(EENV_ROMSIZE-EENV_MEMMAP_SHARSERV_ROMSIZE)
#else
    // the application starts from top of e-updater until end of flash    
	#define EENV_MEMMAP_EAPPLICATION_ROMADDR       		(EENV_MEMMAP_EUPDATER_ROMADDR + EENV_MEMMAP_EUPDATER_ROMSIZE)
    #define EENV_MEMMAP_EAPPLICATION_ROMSIZE            (160*1024)
#endif

#define EENV_MEMMAP_EAPPLICATION_RAMADDR            (EENV_RAMSTART) 
#define EENV_MEMMAP_EAPPLICATION_RAMSIZE            (EENV_RAMSIZE-EENV_MEMMAP_SHARSERV_RAMSIZE)
#define EENV_MEMMAP_EAPPLICATION_STGADDR            (EENV_STGSTART) 
#define EENV_MEMMAP_EAPPLICATION_STGSIZE            (EENV_STGSIZE-EENV_MEMMAP_SHARSERV_STGSIZE)

// 24+72+160 = 5256

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




