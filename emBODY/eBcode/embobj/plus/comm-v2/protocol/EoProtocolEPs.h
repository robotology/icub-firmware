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

#ifndef _EOPROTOCOLEPS_H_
#define _EOPROTOCOLEPS_H_




/** @file       EoProtocolEPs.h
	@brief      This header file gives ...
	@author     marco.accame@iit.it
	@date       05/29/2013
**/

/** @defgroup eo_EoProtocolEPs Ccewcwe
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoProtocol.h"
#include "EOnv_hid.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const uint16_t eoprot_ep_board_numberofthem;
extern const uint8_t eoprot_ep_entities_numberof[eoprot_endpoints_numberof];
extern const EOnv_rom_t * const * eoprot_ep_folded_descriptors[eoprot_endpoints_numberof];
extern const uint16_t* eoprot_ep_entities_sizeof[eoprot_endpoints_numberof]; 
extern const uint32_t** eoprot_ep_entities_defval[eoprot_endpoints_numberof]; 
extern const uint8_t* eoprot_ep_tags_numberof[eoprot_endpoints_numberof];


// - declaration of extern public functions ---------------------------------------------------------------------------

EO_extern_inline uint8_t eoprot_ep_ep2index(eOprotEndpoint_t ep)
{   // dont use control that ep is lower than eoprot_endpoints_numberof ... use sensibly !
    return(ep);
}

/** @}            
    end of group eo_EoProtocolEPs  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------










