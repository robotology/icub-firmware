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

#ifndef _EOPROTOCOLEPMN_ROM_H_
#define _EOPROTOCOLEPMN_ROM_H_




/** @file       EoProtocolEPmn_rom.h
	@brief      This header file gives the constant configuration for the NVs in mc endpoints
	@author     marco.accame@iit.it
	@date       05/29/2013
**/

/** @defgroup eo_EoProtocolEPmn_rom Configuration of the NVs ..
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoManagement.h"
#include "EoProtocolCommon.h"
#include "EoProtocolEPmn.h" 


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section



// - declaration of public user-defined types ------------------------------------------------------------------------- 


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOmn_comm_t eoprot_ep_mn_rom_comm_defaultvalue;
extern const eOmn_appl_t eoprot_ep_mn_rom_appl_defaultvalue;


// - declaration of extern public functions ---------------------------------------------------------------------------

extern uint16_t eoprot_ep_mn_rom_comm_get_offset(eOprotTag_t tag);
extern uint16_t eoprot_ep_mn_rom_appl_get_offset(eOprotTag_t tag);

extern void* eoprot_ep_mn_rom_get_nvrom(eOprotID_t id);
extern uint16_t eoprot_ep_mn_rom_get_sizeofvar(eOprotID_t id);
extern uint16_t eoprot_ep_mn_rom_get_prognum(eOprotID_t id);


/** @}            
    end of group eo_EoProtocolEPmn_rom  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------










