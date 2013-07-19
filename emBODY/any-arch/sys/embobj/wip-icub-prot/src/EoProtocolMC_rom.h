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

#ifndef _EOPROTOCOLMC_ROM_H_
#define _EOPROTOCOLMC_ROM_H_




/** @file       EoProtocolMC_rom.h
	@brief      This header file gives the constant configuration for the NVs in mc endpoints
	@author     marco.accame@iit.it
	@date       05/29/2013
**/

/** @defgroup eo_EoProtocolMC_rom Configuation of the NVs ..
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoProtocol.h"
#include "EoMotionControl.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

extern uint16_t eoprot_mc_rom_get_offset(eOprotEntity_t ent, eOprotTag_t tag);

extern void* eoprot_mc_rom_get_nvrom(eOprotID32_t id);
extern uint16_t eoprot_mc_rom_get_sizeofvar(eOprotID32_t id);
extern uint16_t eoprot_mc_rom_get_prognum(eOprotID32_t id);


/** @}            
    end of group eo_EoProtocolMC_rom  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------










