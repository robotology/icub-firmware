/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#ifndef _EEMEMORYMAP_DSPIC33H_
#define _EEMEMORYMAP_DSPIC33H_


// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eEmemorymap_dspic33.h
    @brief      This header file defines memory map for the dspic33 system.
    @author     marco.accame@iit.it
    @date       09/21/2011
**/

// - public #define  --------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// --- general properties ---------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------

#define EE_MEMMAP_ROM_ADDR                              (0x00000000)
#define EE_MEMMAP_ROM_SIZE                              (0x15800)
#define EE_MEMMAP_ROM_TOP_ADDR                          (EE_MEMMAP_ROM_ADDR+EE_MEMMAP_ROM_SIZE)
#define EE_MEMMAP_ROM_PAGE_SIZE                         (1024L)

#define EE_MEMMAP_RAM_ADDR                              (0x800)
#define EE_MEMMAP_RAM_SIZE                              (0x4000)
#define EE_MEMMAP_RAM_TOP                               (EE_MEMMAP_RAM_ADDR+EE_MEMMAP_RAM_SIZE)


// --------------------------------------------------------------------------------------------------------------------
// --- basic partition ------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------

#define EE_MEMMAP_STRG_PAGES                            (3L)
#define EE_MEMMAP_STRG_ADDR                             (EE_MEMMAP_ROM_TOP_ADDR-EE_MEMMAP_STRG_PAGES*EE_MEMMAP_ROM_PAGE_SIZE)
#define EE_MEMMAP_STRG_SIZE                             (EE_MEMMAP_STRG_PAGES*EE_MEMMAP_ROM_PAGE_SIZE)

// --- basic partition: storage. from up: 1 page for shared ro, 1 page for shared rw, 1 page for application ---------

#define EE_MEMMAP_STRG_SHRO_ADDR                        (EE_MEMMAP_STRG_ADDR+2L*EE_MEMMAP_ROM_PAGE_SIZE)
#define EE_MEMMAP_STRG_SHRO_SIZE                        (1L*EE_MEMMAP_ROM_PAGE_SIZE)

#define EE_MEMMAP_STRG_SHRW_ADDR                        (EE_MEMMAP_STRG_ADDR+1L*EE_MEMMAP_ROM_PAGE_SIZE)
#define EE_MEMMAP_STRG_SHRW_SIZE                        (1L*EE_MEMMAP_ROM_PAGE_SIZE)

#define EE_MEMMAP_STRG_APPL_ADDR                        (EE_MEMMAP_STRG_ADDR)
#define EE_MEMMAP_STRG_APPL_SIZE                        (1L*EE_MEMMAP_ROM_PAGE_SIZE)


// --- basic partition: process loader --------------------------------------------------------------------------------- 
// -- loader is below storage 
#define EE_MEMMAP_LOADER_ROM_SIZE                       (0x7C00)
#define EE_MEMMAP_LOADER_ROM_ADDR                       (EE_MEMMAP_STRG_ADDR-EE_MEMMAP_LOADER_ROM_SIZE)

#define EE_MEMMAP_LOADER_RAM_ADDR                       (EE_MEMMAP_RAM_ADDR)
#define EE_MEMMAP_LOADER_RAM_SIZE                       (EE_MEMMAP_RAM_SIZE)

// --- basic partition: process application --------------------------------------------------------------------------- 
// -- application is below loader and upon 0x0000: if total is 0x15800, stg is 3k = 0xc00, and loader is 0x7C00, 
// -- then application is 0xd000
#define EE_MEMMAP_APPLICATION_ROM_ADDR                  (EE_MEMMAP_ROM_ADDR+0x200) //(EE_MEMMAP_ROM_ADDR)
#define EE_MEMMAP_APPLICATION_ROM_SIZE                  (EE_MEMMAP_LOADER_ROM_ADDR-EE_MEMMAP_APPLICATION_ROM_ADDR)

#define EE_MEMMAP_APPLICATION_RAM_ADDR                  (EE_MEMMAP_RAM_ADDR)
#define EE_MEMMAP_APPLICATION_RAM_SIZE                  (EE_MEMMAP_RAM_SIZE)



// this is a guard
// typedef int EE_MEMMAP_guard00[((EE_MEMMAP_APPLICATION_ROM_ADDR+EE_MEMMAP_APPLICATION_ROM_SIZE) < EE_MEMMAP_STRG_ADDR) ? 1 :-1];


// --------------------------------------------------------------------------------------------------------------------
// --- details of storage ---------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------

// --- storage for shared ro sync-ed by bootloader: partition table, board info, bootloader info ----------------------

#define EE_MEMMAP_STRG_SHRO_PTABLE_ADDR             (EE_MEMMAP_STRG_SHRO_ADDR)
#define EE_MEMMAP_STRG_SHRO_PTABLE_SIZE             (128L)

#define EE_MEMMAP_STRG_SHRO_BRDINFO_ADDR            (EE_MEMMAP_STRG_SHRO_ADDR+128L)
#define EE_MEMMAP_STRG_SHRO_BRDINFO_SIZE            (128L)

#define EE_MEMMAP_STRG_SHRO_LOADERINFO_ADDR         (EE_MEMMAP_STRG_SHRO_ADDR+256L)
#define EE_MEMMAP_STRG_SHRO_LOADERINFO_SIZE         (128L)


// --- storage for shared rw initted by bootloader and modified by bldr or appl: device info, procs info, ipc --------

#define EE_MEMMAP_STRG_SHRW_DEVINFO_ADDR            (EE_MEMMAP_STRG_SHRW_ADDR)
#define EE_MEMMAP_STRG_SHRW_DEVINFO_SIZE            (128L)

#define EE_MEMMAP_STRG_SHRW_APPLICINFO_ADDR         (EE_MEMMAP_STRG_SHRW_ADDR+128L)
#define EE_MEMMAP_STRG_SHRW_APPLICINFO_SIZE         (128L)

#define EE_MEMMAP_STRG_SHRW_IPROCCOMM_ADDR          (EE_MEMMAP_STRG_SHRW_ADDR+256L)
#define EE_MEMMAP_STRG_SHRW_IPROCCOMM_SIZE          (128L)




#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




