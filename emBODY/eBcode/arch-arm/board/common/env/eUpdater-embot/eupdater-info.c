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

/* @file       eupdater-info.c
	@brief      This file keeps the module info of the updatre
	@author     marco.accame@iit.it
    @date       01/11/2012
**/

// marco.accame@iit.it: revised on sept 2020.
// - removed osal
// - cleaned

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "eEcommon.h"
#include "eEmemorymap.h"

#include "hal.h"
#include "ipal.h"

#include <stdio.h>
#include <stdarg.h>

#include "EOtheErrormanager.h"

#include "EOMtheSystem.h"


extern const hal_cfg_t hal_cfg;
extern const ipal_cfg_t ipal_cfg;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eupdater-info.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

#if defined(BOARD_ems001)

#error -> add a const eEmoduleInfo_t eupdater_modinfo __attribute__((at(EENV_MEMMAP_EUPDATER_ROMADDR+EENV_MODULEINFO_OFFSET)))""

#elif defined(BOARD_ems4) | defined(BOARD_mc4plus) | defined(BOARD_mc2plus)


#if __ARMCOMPILER_VERSION > 6000000
#warning CAVEAT: we map in flash @ 0x08008200 in a non-parametric mode
const eEmoduleExtendedInfo_t eupdater_modinfo_extended __attribute__((section(".ARM.__at_0x08008200"))) =   
#else    
const eEmoduleExtendedInfo_t eupdater_modinfo_extended __attribute__((at(EENV_MEMMAP_EUPDATER_ROMADDR+EENV_MODULEINFO_OFFSET))) = 
#endif
{
    .moduleinfo     =
    {
        .info           =
        {
            .entity     =
            {
                .type       = ee_entity_process,
                .signature  = ee_procUpdater,
                .version    = 
                { 
                    .major = 3, 
                    .minor = 0
                },  
                .builddate  = 
                {
                    .year  = 2020,
                    .month = 8,
                    .day   = 8,
                    .hour  = 14,
                    .min   = 14
                }
            },
            .rom        = 
            {   
                .addr   = EENV_MEMMAP_EUPDATER_ROMADDR,
                .size   = EENV_MEMMAP_EUPDATER_ROMSIZE
            },
            .ram        = 
            {   
                .addr   = EENV_MEMMAP_EUPDATER_RAMADDR,
                .size   = EENV_MEMMAP_EUPDATER_RAMSIZE
            },
            .storage    = 
            {
                .type   = ee_strg_none,
                .size   = 0,
                .addr   = 0
            },
            .communication  = ee_commtype_eth,  // later on we may also add can1 and can2
            .name           = "eUpdater"
        },
        .protocols  =
        {
            .udpprotversion  = { .major = 1, .minor = 1},
            .can1protversion = { .major = 0, .minor = 0},
            .can2protversion = { .major = 0, .minor = 1},
            .gtwprotversion  = { .major = 1, .minor = 0}
        },
        .extra      = {"EXT"}
    },
    .compilationdatetime    = __DATE__ " " __TIME__,
    .userdefined            = {0}
};



#else
    #error --> specify BOARD_name
#endif

const eOmsystem_cfg_t eupdater_syscfg =
{
    .codespaceoffset    = (EENV_MEMMAP_EUPDATER_ROMADDR-EENV_ROMSTART),
    .halcfg             = &hal_cfg,
    .osalcfg            = NULL,
    .fsalcfg            = NULL
};

const ipal_cfg_t* const eupdater_ipal_cfg = &ipal_cfg;


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


//#define UPDATER_USE_TRACE
//#define UPDATER_USE_FULL_TRACE



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



