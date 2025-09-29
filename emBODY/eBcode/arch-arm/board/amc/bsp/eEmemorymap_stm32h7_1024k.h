
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------
#ifndef __EEMEMORYMAP_STM32H7_1024K_H_
#define __EEMEMORYMAP_STM32H7_1024K_H_


// - public #define  --------------------------------------------------------------------------------------------------



// --- general properties

#define EENV_ROMSTART                               (0x08000000)
#define EENV_ROMSIZE                                (1024*1024)

#define EENV_RAMSTART                               (0x24000000)
#define EENV_RAMSIZE                                (512*1024)

#define EENV_STGSTART                               (0)
#define EENV_STGSIZE                                (8*1024)


// - moduleinfo for a process or a shalib is shifted some byte after the entry point
//   but it must be beyond the exception vectors. we use an offset of 1k
//   the rule is that a Cortex MPU use addresses from 0x0 to 0x0040+4n, where n is the max number of IRQn.
//   and in stm32h745.h we have WAKEUP_PIN_IRQn = 149, so up to 0x294 = 660 < 1024
//   see rule in https://developer.arm.com/documentation/dui0552/a/the-cortex-m3-processor/exception-model/vector-table
//   and https://developer.arm.com/documentation/dui0646/b/BABIFJFG for M7

#define EENV_MODULEINFO_OFFSET                      (1024)


// --- memory mapping for shared-services. it may be used as a shared library or included in the code application.

#if     defined(SHARSERV_MODE_SHALIB)
    #error --> SHARSERV_MODE_SHALIB is not compatible with this memory mapping
#else

// 0x2407FFC0 7FFC0
// does not use code. uses 64 bytes ram for base (ipc). uses 2k on top of eeprom for partition table and for shared info
#define EENV_MEMMAP_SHARSERV_ROMSIZE                (0)
#define EENV_MEMMAP_SHARSERV_ROMADDR                (EENV_ROMSTART + EENV_ROMSIZE - EENV_MEMMAP_SHARSERV_ROMSIZE)
#define EENV_MEMMAP_SHARSERV_RAMSIZE                (64)
//#define EENV_MEMMAP_SHARSERV_RAMADDR                (EENV_RAMSTART + EENV_RAMSIZE - EENV_MEMMAP_SHARSERV_RAMSIZE)
#define EENV_MEMMAP_SHARSERV_RAMADDR                0x2001FFC0
#define EENV_MEMMAP_SHARSERV_STGSIZE                (2*1024)
#define EENV_MEMMAP_SHARSERV_STGADDR                (EENV_STGSTART + EENV_STGSIZE - EENV_MEMMAP_SHARSERV_STGSIZE)

// now the modules of shared-services which require memory. 1024 bytes for part, 768 bytes for info, 256 bytes for itself. 

// placed at 256k-64 or at 0x2407FFC0 in ram
#define EENV_MEMMAP_SHARSERV_BASE_RAMSIZE           (EENV_MEMMAP_SHARSERV_RAMSIZE)
#define EENV_MEMMAP_SHARSERV_BASE_RAMADDR           (EENV_MEMMAP_SHARSERV_RAMADDR)

#if ((EENV_MEMMAP_SHARSERV_BASE_RAMADDR) == 0x2407FFC0)
#define EENV_SHARSERV_BASE_RAMADDR_AT ".ARM.__at_0x2407FFC0"
#error pls invalidate cache
#elif ((EENV_MEMMAP_SHARSERV_BASE_RAMADDR) == 0x2001FFC0)
#define EENV_SHARSERV_BASE_RAMADDR_AT ".ARM.__at_0x2001FFC0"
#else
    #error verify EENV_MODULEINFO_LOADER_AT 
#endif

// ".ARM.__at_0x2407FFC0"

// placed at 6k in eeprom
#define EENV_MEMMAP_SHARSERV_PART_STGSIZE           (1024)
#define EENV_MEMMAP_SHARSERV_PART_STGADDR           (EENV_MEMMAP_SHARSERV_STGADDR)

// placed at 7k in eeprom
#define EENV_MEMMAP_SHARSERV_INFO_STGSIZE           (768)
#define EENV_MEMMAP_SHARSERV_INFO_STGADDR           (EENV_MEMMAP_SHARSERV_PART_STGADDR + EENV_MEMMAP_SHARSERV_PART_STGSIZE)

// placed at 7k+768 in eeprom
#define EENV_MEMMAP_SHARSERV_OWN_STGSIZE            (256)
#define EENV_MEMMAP_SHARSERV_OWN_STGADDR            (EENV_MEMMAP_SHARSERV_INFO_STGADDR + EENV_MEMMAP_SHARSERV_INFO_STGSIZE)

#endif


// --- memory mapping for e-processes: eLoader, eUpdater, aand eApplication.
// -   rom pages sizes are: 128k, 128k, 128k, 128k, 128k, 128k, 128k, 128k

// --- eloader: 128k [0x08000000, 0x08000000 + 0x00020000 = 0x08020000]
#define EENV_MEMMAP_ELOADER_ROMSIZE                 (128*1024)
#define EENV_MEMMAP_ELOADER_ROMADDR                 (EENV_ROMSTART + 0)
#define EENV_MEMMAP_ELOADER_RAMSIZE                 (EENV_RAMSIZE-EENV_MEMMAP_SHARSERV_RAMSIZE)
#define EENV_MEMMAP_ELOADER_RAMADDR                 (EENV_RAMSTART) 
#define EENV_MEMMAP_ELOADER_STGADDR                 (EENV_STGSTART) 
#define EENV_MEMMAP_ELOADERSTGSIZE                  (0)

#if ((EENV_MEMMAP_ELOADER_ROMADDR+EENV_MODULEINFO_OFFSET) == 0x08000400)
#define EENV_MODULEINFO_LOADER_AT ".ARM.__at_0x08000400"
#else
    #error verify EENV_MODULEINFO_LOADER_AT 
#endif




// --- eupdater: 256K [0x08020000, 0x08020000 + 0x00040000 = 0x08060000]
#ifdef EENV_EUPDATER_FORCE_CODE_OFFSET_TO_ZERO            
   #define EENV_MEMMAP_EUPDATER_ROMADDR             (EENV_ROMSTART + 0)              
#else
    #define EENV_MEMMAP_EUPDATER_ROMADDR            (EENV_MEMMAP_ELOADER_ROMADDR + EENV_MEMMAP_ELOADER_ROMSIZE)
#endif
#define EENV_MEMMAP_EUPDATER_ROMSIZE                (256*1024)
#define EENV_MEMMAP_EUPDATER_RAMADDR                (EENV_RAMSTART) 
#define EENV_MEMMAP_EUPDATER_RAMSIZE                (EENV_RAMSIZE-EENV_MEMMAP_SHARSERV_RAMSIZE)
#define EENV_MEMMAP_EUPDATER_STGADDR                (EENV_STGSTART) 
#define EENV_MEMMAP_EUPDATERSTGSIZE                 (0)

#if ((EENV_MEMMAP_EUPDATER_ROMADDR+EENV_MODULEINFO_OFFSET) == 0x08020400)
#define EENV_MODULEINFO_UPDATER_AT ".ARM.__at_0x08020400"
#else
    #error verify EENV_MODULEINFO_UPDATER_AT 
#endif



// --- eapplication: 512K [0x08060000, 0x08060000 + 0x00080000 = 0x080E0000]
#ifdef EENV_EAPPLICATION_FORCE_CODE_OFFSET_TO_ZERO
	// the application starts from zero and can stay in all the flash           
   	#define EENV_MEMMAP_EAPPLICATION_ROMADDR        (EENV_ROMSTART + 0)              
   	#define EENV_MEMMAP_EAPPLICATION_ROMSIZE        (EENV_ROMSIZE-EENV_MEMMAP_SHARSERV_ROMSIZE)
#else
    // the application starts from top of e-updater (384K) until 384K+512K. 
	#define EENV_MEMMAP_EAPPLICATION_ROMADDR        (EENV_MEMMAP_EUPDATER_ROMADDR + EENV_MEMMAP_EUPDATER_ROMSIZE)
    #define EENV_MEMMAP_EAPPLICATION_ROMSIZE        (512*1024)
#endif
#define EENV_MEMMAP_EAPPLICATION_RAMADDR            (EENV_RAMSTART) 
#define EENV_MEMMAP_EAPPLICATION_RAMSIZE            (EENV_RAMSIZE-EENV_MEMMAP_SHARSERV_RAMSIZE)
#define EENV_MEMMAP_EAPPLICATION_STGADDR            (EENV_STGSTART) 
#define EENV_MEMMAP_EAPPLICATION_STGSIZE            (EENV_STGSIZE-EENV_MEMMAP_SHARSERV_STGSIZE)

#if ((EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET) == 0x08060400)
#define EENV_MODULEINFO_APPLICATION_AT ".ARM.__at_0x08060400"
#else
    #error verify EENV_MODULEINFO_APPLICATION_AT 
#endif




// --- eapplication 2: 1024K [0x08100000, 0x08100000 + 0x00100000 = 0x08200000]

#define EENV_MEMMAP_EAPPLICATION_2_ROMADDR      0x08100000
#define EENV_MEMMAP_EAPPLICATION_2_ROMSIZE      (512*1024)

#define EENV_MEMMAP_EAPPLICATION_2_RAMADDR      0x10000000
#define EENV_MEMMAP_EAPPLICATION_2_RAMSIZE      0x00048000


#if ((EENV_MEMMAP_EAPPLICATION_2_ROMADDR+EENV_MODULEINFO_OFFSET) == 0x08100400)
#define EENV_MODULEINFO_APPLICATION_2_AT ".ARM.__at_0x08100400"
#else
    #error verify EENV_MODULEINFO_APPLICATION_2_AT 
#endif


// --- emaintainer: 512K [0x08060000, 0x08060000 + 0x00080000 = 0x080E0000]
#define EENV_MEMMAP_EMAINTAINER_ROMADDR        EENV_MEMMAP_EAPPLICATION_ROMADDR
#define EENV_MEMMAP_EMAINTAINER_ROMSIZE        EENV_MEMMAP_EAPPLICATION_ROMSIZE

#define EENV_MEMMAP_EMAINTAINER_RAMADDR        EENV_MEMMAP_EAPPLICATION_RAMADDR 
#define EENV_MEMMAP_EMAINTAINER_RAMSIZE        EENV_MEMMAP_EAPPLICATION_RAMSIZE


#if ((EENV_MEMMAP_EMAINTAINER_ROMADDR+EENV_MODULEINFO_OFFSET) == 0x08060400)
#define EENV_MODULEINFO_MAINTAINER_AT ".ARM.__at_0x08060400"
#else
    #error verify EENV_MODULEINFO_MAINTAINER_AT 
#endif



#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


