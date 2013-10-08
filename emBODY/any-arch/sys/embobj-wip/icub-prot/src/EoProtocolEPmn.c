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

/* @file       EoProtocolEPmn.c
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

#include "EoProtocolEPmn_rom.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolEPmn.h"


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

// - guard on motion control tags ...
EO_VERIFYproposition(eoprot_ep_mn_tagsnum_co, eoprot_ep_mn_comm_tags_numberof == eoprot_ep_mn_comm_funtyps_numberof);
EO_VERIFYproposition(eoprot_ep_mn_tagsnum_ap, eoprot_ep_mn_appl_tags_numberof == eoprot_ep_mn_appl_funtyps_numberof);



// - some more constants

typedef enum
{
    eoprot_ep_mn_vars_numberof_emsboard       = eoprot_ep_mn_comms_numberof_emsboard*eoprot_ep_mn_comm_tags_numberof+
                                                eoprot_ep_mn_appls_numberof_emsboard*eoprot_ep_mn_appl_tags_numberof
} eOprot_ep_mn_vars_numberof_t;
  

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint8_t s_eoprot_ep_mn_ep2arrayindex(eOprotEP_t ep);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const uint8_t s_eoprot_ep_mn_number_of_comms[] = 
{   
    eoprot_ep_mn_comms_numberof_emsboard
};    

static const uint8_t s_eoprot_ep_mn_number_of_appls[] = 
{   
    eoprot_ep_mn_appls_numberof_emsboard
};  


static const uint16_t s_eoprot_ep_mn_number_of_variables[] = 
{   
    eoprot_ep_mn_vars_numberof_emsboard
};  

static const uint16_t s_eoprot_ep_mn_sizeof[] = 
{   
    sizeof(eOprot_ep_mn_emsboard_t)
}; 

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eoprot_ep_mn_variables_numberof_Get(eOprotEP_t ep)
{
    return(s_eoprot_ep_mn_number_of_variables[s_eoprot_ep_mn_ep2arrayindex(ep)]);
}

extern uint16_t eoprot_ep_mn_variable_progressivenumber_Get(eOprotEP_t ep, eOprotID_t id)
{
    return(eoprot_ep_mn_rom_epid2progressivenumber(ep, id));
}

extern uint16_t eoprot_ep_mn_comms_numberof_Get(eOprotEP_t ep)
{
    return(s_eoprot_ep_mn_number_of_comms[s_eoprot_ep_mn_ep2arrayindex(ep)]);
}

extern uint16_t eoprot_ep_mn_appls_numberof_Get(eOprotEP_t ep)
{
    return(s_eoprot_ep_mn_number_of_appls[s_eoprot_ep_mn_ep2arrayindex(ep)]);
}


extern uint16_t eoprot_ep_mn_ram_sizeof_Get(eOprotEP_t ep)
{
    return(s_eoprot_ep_mn_sizeof[s_eoprot_ep_mn_ep2arrayindex(ep)]);
}

extern uint16_t eoprot_ep_mn_variable_ram_sizeof_Get(eOprotEP_t ep, eOprotID_t id)
{
    return(eoprot_ep_mn_rom_epid2sizeofvar(ep, id));
}

extern void* eoprot_ep_mn_variable_ram_Extract(void* epram, eOprotEP_t ep, eOprotID_t id)
{
    return(eoprot_ep_mn_rom_dataepid2nvram(epram, ep, id));
}

extern void* eoprot_ep_mn_variable_rom_Get(eOprotEP_t ep, eOprotID_t id)
{
    return(eoprot_ep_mn_rom_epid2nvrom(ep, id));
}

extern const eOmn_comm_t* eoprot_ep_mn_comm_default_Get(void)
{
    return(&eoprot_ep_mn_rom_comm_defaultvalue);
}

extern const eOmn_appl_t* eoprot_ep_mn_appl_default_Get(void)
{
    return(&eoprot_ep_mn_rom_appl_defaultvalue);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

#warning --> the function s_eoprot_ep_mn_ep2arrayindex() does not make a control upon validity of its argument ...
static uint8_t s_eoprot_ep_mn_ep2arrayindex(eOprotEP_t ep)
{   // we have only one endpoint thus only one index equal to 0.
    uint8_t arrayindex = 0;
    return(arrayindex);    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



