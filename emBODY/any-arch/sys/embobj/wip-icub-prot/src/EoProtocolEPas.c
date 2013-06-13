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

/* @file       EoProtocolEPas.c
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

#include "EoProtocolEPas_rom.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolEPas.h"


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

// - guard on max number of entities
EO_VERIFYproposition(eoprot_ep_as_ded34, eoprot_ep_as_entities_numberof <= eoprot_entities_numberof);

// - guard on tags ...
EO_VERIFYproposition(eoprot_ep_as_tagsnum_st, eoprot_ep_as_strain_tags_numberof == eoprot_ep_as_strain_funtyps_numberof);
EO_VERIFYproposition(eoprot_ep_as_tagsmax_st, eoprot_ep_as_strain_tags_numberof <= eoprot_tags_numberof);

EO_VERIFYproposition(eoprot_ep_as_tagsnum_ma, eoprot_ep_as_mais_tags_numberof == eoprot_ep_as_mais_funtyps_numberof);
EO_VERIFYproposition(eoprot_ep_as_tagsmax_ma, eoprot_ep_as_mais_tags_numberof <= eoprot_tags_numberof);


// - some more constants

typedef enum
{
    eoprot_ep_as_vars_numberof_leftupperarm   = eoprot_ep_as_strains_numberof_leftupperarm*eoprot_ep_as_strain_tags_numberof+
                                                eoprot_ep_as_maises_numberof_leftupperarm*eoprot_ep_as_mais_tags_numberof,
    eoprot_ep_as_vars_numberof_leftlowerarm   = eoprot_ep_as_strains_numberof_leftlowerarm*eoprot_ep_as_strain_tags_numberof+
                                                eoprot_ep_as_maises_numberof_leftlowerarm*eoprot_ep_as_mais_tags_numberof,                                                
    eoprot_ep_as_vars_numberof_rightupperarm  = eoprot_ep_as_strains_numberof_rightupperarm*eoprot_ep_as_strain_tags_numberof+
                                                eoprot_ep_as_maises_numberof_rightupperarm*eoprot_ep_as_mais_tags_numberof,                                                   
    eoprot_ep_as_vars_numberof_rightlowerarm  = eoprot_ep_as_strains_numberof_rightlowerarm*eoprot_ep_as_strain_tags_numberof+
                                                eoprot_ep_as_maises_numberof_rightlowerarm*eoprot_ep_as_mais_tags_numberof,  
    eoprot_ep_as_vars_numberof_leftupperleg   = eoprot_ep_as_strains_numberof_leftupperleg*eoprot_ep_as_strain_tags_numberof+
                                                eoprot_ep_as_maises_numberof_leftupperleg*eoprot_ep_as_mais_tags_numberof,  
    eoprot_ep_as_vars_numberof_rightupperleg  = eoprot_ep_as_strains_numberof_rightupperleg*eoprot_ep_as_strain_tags_numberof+
                                                eoprot_ep_as_maises_numberof_rightupperleg*eoprot_ep_as_mais_tags_numberof                                                 
} eOprot_ep_as_vars_numberof_t;
  

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint8_t s_eoprot_ep_as_ep2arrayindex(eOprotEP_t ep);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const uint8_t s_eoprot_ep_as_number_of_strains[] = 
{   
    eoprot_ep_as_strains_numberof_leftupperarm,
    eoprot_ep_as_strains_numberof_leftlowerarm,
    eoprot_ep_as_strains_numberof_rightupperarm,
    eoprot_ep_as_strains_numberof_rightlowerarm,
    eoprot_ep_as_strains_numberof_leftupperleg,
    eoprot_ep_as_strains_numberof_rightupperleg
};    

static const uint8_t s_eoprot_ep_as_number_of_maises[] = 
{   
    eoprot_ep_as_maises_numberof_leftupperarm,
    eoprot_ep_as_maises_numberof_leftlowerarm,
    eoprot_ep_as_maises_numberof_rightupperarm,
    eoprot_ep_as_maises_numberof_rightlowerarm,
    eoprot_ep_as_maises_numberof_leftupperleg,
    eoprot_ep_as_maises_numberof_rightupperleg
};  


static const uint16_t s_eoprot_ep_as_number_of_variables[] = 
{   
    eoprot_ep_as_vars_numberof_leftupperarm,
    eoprot_ep_as_vars_numberof_leftlowerarm,
    eoprot_ep_as_vars_numberof_rightupperarm,
    eoprot_ep_as_vars_numberof_rightlowerarm,
    eoprot_ep_as_vars_numberof_leftupperleg,
    eoprot_ep_as_vars_numberof_rightupperleg    
};  

static const uint16_t s_eoprot_ep_as_sizeof[] = 
{   
    sizeof(eOprot_ep_as_leftupperarm_t),
    sizeof(eOprot_ep_as_leftlowerarm_t),
    sizeof(eOprot_ep_as_rightupperarm_t),
    sizeof(eOprot_ep_as_rightlowerarm_t),
    sizeof(eOprot_ep_as_leftupperleg_t),
    sizeof(eOprot_ep_as_rightupperleg_t)        
}; 

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eoprot_ep_as_variables_numberof_Get(eOprotEP_t ep)
{
    return(s_eoprot_ep_as_number_of_variables[s_eoprot_ep_as_ep2arrayindex(ep)]);
}

extern uint16_t eoprot_ep_as_variable_progressivenumber_Get(eOprotEP_t ep, eOprotID_t id)
{
    return(eoprot_ep_as_rom_epid2progressivenumber(ep, id));
}

extern uint16_t eoprot_ep_as_strains_numberof_Get(eOprotEP_t ep)
{
    return(s_eoprot_ep_as_number_of_strains[s_eoprot_ep_as_ep2arrayindex(ep)]);
}

extern uint16_t eoprot_ep_as_maises_numberof_Get(eOprotEP_t ep)
{
    return(s_eoprot_ep_as_number_of_maises[s_eoprot_ep_as_ep2arrayindex(ep)]);
}


extern uint16_t eoprot_ep_as_ram_sizeof_Get(eOprotEP_t ep)
{
    return(s_eoprot_ep_as_sizeof[s_eoprot_ep_as_ep2arrayindex(ep)]);
}

extern uint16_t eoprot_ep_as_variable_ram_sizeof_Get(eOprotEP_t ep, eOprotID_t id)
{
    return(eoprot_ep_as_rom_epid2sizeofvar(ep, id));
}

extern void* eoprot_ep_as_variable_ram_Extract(void* epram, eOprotEP_t ep, eOprotID_t id)
{
    return(eoprot_ep_as_rom_dataepid2nvram(epram, ep, id));
}

extern void* eoprot_ep_as_variable_rom_Get(eOprotEP_t ep, eOprotID_t id)
{
    return(eoprot_ep_as_rom_epid2nvrom(ep, id));
}

extern const eOas_strain_t* eoprot_ep_as_strain_default_Get(void)
{
    return(&eoprot_ep_as_rom_strain_defaultvalue);
}

extern const eOas_mais_t* eoprot_ep_as_mais_default_Get(void)
{
    return(&eoprot_ep_as_rom_mais_defaultvalue);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

//#warning --> the function s_eoprot_ep_as_ep2arrayindex() does not make a control upon validity of its argument ...
static uint8_t s_eoprot_ep_as_ep2arrayindex(eOprotEP_t ep)
{ 
    uint8_t arrayindex = ep&0xf;
    return(arrayindex);    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



