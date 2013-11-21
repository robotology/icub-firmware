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


// --- memory mapping for shal-system: it can be sharSERV or shalBASE + shalPART + shalINFO. 
// --- they are on top of flash (for 16k), on top of ram (for 2k), on top of eeprom (for 2k)

#define EENV_MEMMAP_SHALSYSTEM_ROMSIZE              (16*1024)
#define EENV_MEMMAP_SHALSYSTEM_ROMADDR              (EENV_ROMSTART + EENV_ROMSIZE - EENV_MEMMAP_SHALSYSTEM_ROMSIZE)

#define EENV_MEMMAP_SHALSYSTEM_RAMSIZE              (2*1024)
#define EENV_MEMMAP_SHALSYSTEM_RAMADDR              (EENV_RAMSTART +  EENV_RAMSIZE - EENV_MEMMAP_SHALSYSTEM_RAMSIZE)

#define EENV_MEMMAP_SHALSYSTEM_STGSIZE              (2*1024)
#define EENV_MEMMAP_SHALSYSTEM_STGADDR              (EENV_STGSTART +  EENV_STGSIZE - EENV_MEMMAP_SHALSYSTEM_STGSIZE)


// - shalbase: the lowest
#define EENV_MEMMAP_SHALBASE_ROMSIZE                (8*1024)
#define EENV_MEMMAP_SHALBASE_ROMADDR                (EENV_MEMMAP_SHALSYSTEM_ROMADDR + 0)
#define EENV_MEMMAP_SHALBASE_RAMSIZE                (256)
#define EENV_MEMMAP_SHALBASE_RAMADDR                (EENV_MEMMAP_SHALSYSTEM_RAMADDR + 0)
#define EENV_MEMMAP_SHALBASE_STGSIZE                (256)
#define EENV_MEMMAP_SHALBASE_STGADDR                (EENV_MEMMAP_SHALSYSTEM_STGADDR + 0)


#define EENV_MEMMAP_SHALBASE_RAMFOR_ZIDATA          (128)
#define EENV_MEMMAP_SHALBASE_RAMFOR_RWDATA          (EENV_MEMMAP_SHALBASE_RAMSIZE - EENV_MEMMAP_SHALBASE_RAMFOR_ZIDATA)


// - shalpart: the middle
#define EENV_MEMMAP_SHALPART_ROMSIZE                (4*1024)
#define EENV_MEMMAP_SHALPART_ROMADDR                (EENV_MEMMAP_SHALBASE_ROMADDR + EENV_MEMMAP_SHALBASE_ROMSIZE)
#define EENV_MEMMAP_SHALPART_RAMSIZE                (1024)
#define EENV_MEMMAP_SHALPART_RAMADDR                (EENV_MEMMAP_SHALBASE_RAMADDR + EENV_MEMMAP_SHALBASE_RAMSIZE)
#define EENV_MEMMAP_SHALPART_STGSIZE                (1024)
#define EENV_MEMMAP_SHALPART_STGADDR                (EENV_MEMMAP_SHALBASE_STGADDR + EENV_MEMMAP_SHALBASE_STGSIZE)

#define EENV_MEMMAP_SHALPART_RAMFOR_ZIDATA          (1024)
#define EENV_MEMMAP_SHALPART_RAMFOR_RWDATA          (EENV_MEMMAP_SHALPART_RAMSIZE - EENV_MEMMAP_SHALPART_RAMFOR_ZIDATA)

// - shalinfo: the top
#define EENV_MEMMAP_SHALINFO_ROMSIZE                (4*1024)
#define EENV_MEMMAP_SHALINFO_ROMADDR                (EENV_MEMMAP_SHALPART_ROMADDR + EENV_MEMMAP_SHALPART_ROMSIZE)
#define EENV_MEMMAP_SHALINFO_RAMSIZE                (768)
#define EENV_MEMMAP_SHALINFO_RAMADDR                (EENV_MEMMAP_SHALPART_RAMADDR + EENV_MEMMAP_SHALPART_RAMSIZE)
#define EENV_MEMMAP_SHALINFO_STGSIZE                (768)
#define EENV_MEMMAP_SHALINFO_STGADDR                (EENV_MEMMAP_SHALPART_STGADDR + EENV_MEMMAP_SHALPART_STGSIZE)

#define EENV_MEMMAP_SHALINFO_RAMFOR_ZIDATA          (768)
#define EENV_MEMMAP_SHALINFO_RAMFOR_RWDATA          (EENV_MEMMAP_SHALINFO_RAMSIZE - EENV_MEMMAP_SHALINFO_RAMFOR_ZIDATA)


// - sharSERV: there is only one shared library which contains shalBASE, shalPART, shalINFO. the memory mapping must be the same.
#define EENV_MEMMAP_SHARSERV_ROMSIZE                EENV_MEMMAP_SHALSYSTEM_ROMSIZE
#define EENV_MEMMAP_SHARSERV_ROMADDR                EENV_MEMMAP_SHALSYSTEM_ROMADDR
#define EENV_MEMMAP_SHARSERV_RAMSIZE                EENV_MEMMAP_SHALSYSTEM_RAMSIZE
#define EENV_MEMMAP_SHARSERV_RAMADDR                EENV_MEMMAP_SHALSYSTEM_RAMADDR
#define EENV_MEMMAP_SHARSERV_STGSIZE                EENV_MEMMAP_SHALSYSTEM_STGSIZE
#define EENV_MEMMAP_SHARSERV_STGADDR                EENV_MEMMAP_SHALSYSTEM_STGADDR


#define EENV_MEMMAP_SHARSERV_RAMFOR_ZIDATA          (EENV_MEMMAP_SHALBASE_RAMFOR_ZIDATA+EENV_MEMMAP_SHALPART_RAMFOR_ZIDATA+EENV_MEMMAP_SHALINFO_RAMFOR_ZIDATA)
#define EENV_MEMMAP_SHARSERV_RAMFOR_RWDATA          (EENV_MEMMAP_SHALSYSTEM_RAMSIZE - EENV_MEMMAP_SHARSERV_RAMFOR_ZIDATA)


// --- memory mapping for shal-abstraction-layer: hal, osal, fsal, ipal
// --- they are just below the shal-system: flash (80k), ram (12k). no eeprom.
// REMOVED BECAUSE WE SHALL NEVER USE THEM

#define EENV_MEMMAP_SHALABSLAY_ROMSIZE              (#error shal abslay IS NOT USED)
#define EENV_MEMMAP_SHALABSLAY_ROMADDR              (#error shal abslay IS NOT USED)

#define EENV_MEMMAP_SHALABSLAY_RAMSIZE              (#error shal abslay IS NOT USED)
#define EENV_MEMMAP_SHALABSLAY_RAMADDR              (#error shal abslay IS NOT USED)

#define EENV_MEMMAP_SHALABSLAY_STGSIZE              (#error shal abslay does not have eeprom)
#define EENV_MEMMAP_SHALABSLAY_STGADDR              (#error shal abslay does not have eeprom)



// --- memory mapping for e-processes: eLoader, eUpdater, aand eApplication.

// --- eloader
#define EENV_MEMMAP_ELOADER_ROMSIZE                 (24*1024)
#define EENV_MEMMAP_ELOADER_ROMADDR                 (EENV_ROMSTART + 0)
#define EENV_MEMMAP_ELOADER_RAMSIZE                 (EENV_RAMSIZE-EENV_MEMMAP_SHALSYSTEM_RAMSIZE)
#define EENV_MEMMAP_ELOADER_RAMADDR                 (EENV_RAMSTART) 


// --- eupdater
#ifdef EENV_EUPDATER_FORCE_CODE_OFFSET_TO_ZERO            
   #define EENV_MEMMAP_EUPDATER_ROMADDR             (EENV_ROMSTART + 0)              
#else
#define EENV_MEMMAP_EUPDATER_ROMADDR                (EENV_MEMMAP_ELOADER_ROMADDR + EENV_MEMMAP_ELOADER_ROMSIZE)
#endif
#define EENV_MEMMAP_EUPDATER_ROMSIZE                (72*1024)
#define EENV_MEMMAP_EUPDATER_RAMADDR                (EENV_RAMSTART) 
#define EENV_MEMMAP_EUPDATER_RAMSIZE                (EENV_RAMSIZE-EENV_MEMMAP_SHALSYSTEM_RAMSIZE)



// --- eapplication
#ifdef EENV_EAPPLICATION_FORCE_CODE_OFFSET_TO_ZERO
	// the application starts from zero and can stay in all the flash except the top 16k where can be placed the shared libraries            
   	#define EENV_MEMMAP_EAPPLICATION_ROMADDR         	(EENV_ROMSTART + 0)              
   	#define EENV_MEMMAP_EAPPLICATION_ROMSIZE       		(EENV_ROMSIZE-EENV_MEMMAP_SHALSYSTEM_ROMSIZE)
#else
	#define EENV_MEMMAP_EAPPLICATION_ROMADDR       		(EENV_MEMMAP_EUPDATER_ROMADDR + EENV_MEMMAP_EUPDATER_ROMSIZE)
	//#define EENV_MEMMAP_EAPPLICATION_ROMSIZE            (144*1024)    
    // --> we use all the flash until the 256k. not shalsystem anymore
    #define EENV_MEMMAP_EAPPLICATION_ROMSIZE            (160*1024)
#endif

#define EENV_MEMMAP_EAPPLICATION_RAMADDR            (EENV_RAMSTART) 
#define EENV_MEMMAP_EAPPLICATION_RAMSIZE            (EENV_RAMSIZE-EENV_MEMMAP_SHALSYSTEM_RAMSIZE)
#define EENV_MEMMAP_EAPPLICATION_STGADDR            (EENV_STGSTART) 
#define EENV_MEMMAP_EAPPLICATION_STGSIZE            (EENV_STGSIZE-EENV_MEMMAP_SHALSYSTEM_STGSIZE)



#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




