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

#ifndef _EOPROTOCOLEPMC_ROM_H_
#define _EOPROTOCOLEPMC_ROM_H_




/** @file       EoProtocolEPmc_rom.h
	@brief      This header file gives the constant configuration for the NVs in mc endpoints
	@author     marco.accame@iit.it
	@date       05/29/2013
**/

/** @defgroup eo_EoProtocolEPmc_rom Configuation of the NVs ..
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoMotionControl.h"
#include "EoProtocolCommon.h"
#include "EoProtocolEPmc.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section



// - declaration of public user-defined types ------------------------------------------------------------------------- 


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOmc_joint_t eoprot_ep_mc_rom_joint_defaultvalue;
extern const eOmc_motor_t eoprot_ep_mc_rom_motor_defaultvalue;
extern const eOmc_controller_t eoprot_ep_mc_rom_controller_defaultvalue;


// - declaration of extern public functions ---------------------------------------------------------------------------

//extern void* eoprot_ep_mc_rom_get_nvram(void* data, eOprotBRD_t brd, eOprotID32_t id);
extern uint16_t eoprot_ep_mc_rom_joint_get_offset(eOprotTag_t tag);
extern uint16_t eoprot_ep_mc_rom_motor_get_offset(eOprotTag_t tag);
extern uint16_t eoprot_ep_mc_rom_controller_get_offset(eOprotTag_t tag);

extern void* eoprot_ep_mc_rom_get_nvrom(eOprotID32_t id);
extern uint16_t eoprot_ep_mc_rom_get_sizeofvar(eOprotID32_t id);
extern uint16_t eoprot_ep_mc_rom_get_prognum(eOprotID32_t id);



/** @}            
    end of group eo_EoProtocolEPmc_rom  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------










