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

#ifndef _EOPROTOCOLAS_ROM_H_
#define _EOPROTOCOLAS_ROM_H_




/** @file       EoProtocolAS_rom.h
	@brief      This header file gives the constant configuration for the NVs in mc endpoints
	@author     marco.accame@iit.it
	@date       05/29/2013
**/

/** @defgroup eo_EoProtocolAS_rom Configuration of the NVs ..
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoProtocol.h"
#include "EoProtocolEPs.h"
#include "EoProtocolAS.h"
#include "EoAnalogSensors.h"
#include "EOnv_hid.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const EOnv_rom_t * const eoprot_as_rom_folded_descriptors[];
extern const uint8_t eoprot_as_rom_tags_numberof[eoprot_entities_as_numberof];
extern const uint16_t eoprot_as_rom_entities_sizeof[eoprot_entities_as_numberof];
extern const uint32_t* eoprot_as_rom_entities_defval[eoprot_entities_as_numberof];


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section


/** @}            
    end of group eo_EoProtocolAS_rom 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------










