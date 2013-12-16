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

/* @file       EoProtocolEPs.c
    @brief      This file keeps ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoProtocolAS.h"
#include "EoProtocolMC.h"
#include "EoProtocolMN.h"
#include "EoProtocolSK.h"

#include "EoProtocolAS_rom.h"
#include "EoProtocolMC_rom.h"
#include "EoProtocolMN_rom.h"
#include "EoProtocolSK_rom.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolEPs.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


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
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


extern const uint8_t eoprot_ep_board_numberofthem = eoprot_boards_maxnumberof;

extern const uint8_t eoprot_ep_entities_numberof[] =
{   // very important: use order of eOprot_endpoint_t: pos 0 is eoprot_endpoint_management etc.
    eoprot_entities_mn_numberof,
    eoprot_entities_mc_numberof,
    eoprot_entities_as_numberof,
    eoprot_entities_sk_numberof
};  EO_VERIFYsizeof(eoprot_ep_entities_numberof, eoprot_endpoints_numberof*sizeof(uint8_t)); 

extern const EOnv_rom_t * const * eoprot_ep_folded_descriptors[] = 
{   // very important: use order of eOprot_endpoint_t: pos 0 is eoprot_endpoint_management etc.
    eoprot_mn_rom_folded_descriptors,
    eoprot_mc_rom_folded_descriptors,
    eoprot_as_rom_folded_descriptors,
    eoprot_sk_rom_folded_descriptors
};  EO_VERIFYsizeof(eoprot_ep_folded_descriptors, eoprot_endpoints_numberof*sizeof(const EOnv_rom_t * const *)); 

extern const uint16_t* eoprot_ep_entities_sizeof[] =
{   // very important: use order of eOprot_endpoint_t: pos 0 is eoprot_endpoint_management etc.
    eoprot_mn_rom_entities_sizeof,
    eoprot_mc_rom_entities_sizeof,
    eoprot_as_rom_entities_sizeof,
    eoprot_sk_rom_entities_sizeof
};  EO_VERIFYsizeof(eoprot_ep_entities_sizeof, eoprot_endpoints_numberof*sizeof(uint16_t*)); 

extern const uint32_t** eoprot_ep_entities_defval[] =
{   // very important: use order of eOprot_endpoint_t: pos 0 is eoprot_endpoint_management etc.
    (const uint32_t**)&eoprot_mn_rom_entities_defval,
    (const uint32_t**)&eoprot_mc_rom_entities_defval,
    (const uint32_t**)&eoprot_as_rom_entities_defval,
    (const uint32_t**)&eoprot_sk_rom_entities_defval   
};

extern const uint8_t* eoprot_ep_tags_numberof[] =
{   // very important: use order of eOprot_endpoint_t: pos 0 is eoprot_endpoint_management etc.
    eoprot_mn_rom_tags_numberof,
    eoprot_mc_rom_tags_numberof,
    eoprot_as_rom_tags_numberof,
    eoprot_sk_rom_tags_numberof
};  EO_VERIFYsizeof(eoprot_ep_tags_numberof, eoprot_endpoints_numberof*sizeof(uint8_t*)); 



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section



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



