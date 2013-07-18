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

/* @file       EoProtocolSK_rom.c
    @brief      This file keeps ....
    @author     marco.accame@iit.it
    @date       06/06/2013
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoCommon.h"
#include "EOnv_hid.h"
#include "EOconstvector_hid.h"


#include "EoSkin.h"
#include "EoProtocolSK.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolSK_rom.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eoprot_sk_rom_epid2index_of_folded_descriptors(eOprotID32_t id);

static uint16_t s_eoprot_sk_rom_skin_ramoffset(uint16_t tag);




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// - default value of a skin

const eOsk_skin_t eoprot_sk_rom_skin_defaultvalue = { 0 };



// - descriptors for the variables of a skin

EOnv_rom_t eoprot_sk_rom_descriptor_skin_wholeitem =
{   
    EO_INIT(.capacity)  sizeof(eoprot_sk_rom_skin_defaultvalue),
    EO_INIT(.rwmode)    eoprot_rwm_sk_skin_wholeitem,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_sk_rom_skin_defaultvalue,
    EO_INIT(.init)      eoprot_fun_INIT_sk_skin_wholeitem,
    EO_INIT(.update)    eoprot_fun_UPDT_sk_skin_wholeitem
};


EOnv_rom_t eoprot_sk_rom_descriptor_skin_config_sigmode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_sk_rom_skin_defaultvalue.config.sigmode),
    EO_INIT(.rwmode)    eoprot_rwm_sk_skin_config_sigmode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_sk_rom_skin_defaultvalue.config.sigmode,
    EO_INIT(.init)      eoprot_fun_INIT_sk_skin_config_sigmode,
    EO_INIT(.update)    eoprot_fun_UPDT_sk_skin_config_sigmode
};


EOnv_rom_t eoprot_sk_rom_descriptor_skin_status_arrayof10canframes =
{   
    EO_INIT(.capacity)  sizeof(eoprot_sk_rom_skin_defaultvalue.status.arrayof10canframes),
    EO_INIT(.rwmode)    eoprot_rwm_sk_skin_status_arrayof10canframes,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_sk_rom_skin_defaultvalue.status.arrayof10canframes,
    EO_INIT(.init)      eoprot_fun_INIT_sk_skin_status_arrayof10canframes,
    EO_INIT(.update)    eoprot_fun_UPDT_sk_skin_status_arrayof10canframes
};



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


const EOnv_rom_t * const eoprot_sk_rom_folded_descriptors[] =
{
    // here are eoprot_tags_sk_skin_numberof descriptors for the skins (equal for every skin)
    &eoprot_sk_rom_descriptor_skin_wholeitem,
    &eoprot_sk_rom_descriptor_skin_config_sigmode,
    &eoprot_sk_rom_descriptor_skin_status_arrayof10canframes
         
};  EO_VERIFYsizeof(eoprot_sk_rom_folded_descriptors, sizeof(EOnv_rom_t*)*(eoprot_tags_sk_skin_numberof));




const EOconstvector  eoprot_sk_rom_constvector_of_folded_descriptors_dat = 
{
    EO_INIT(.size)              sizeof(eoprot_sk_rom_folded_descriptors)/sizeof(EOnv_rom_t*), 
    EO_INIT(.item_size)         sizeof(EOnv_rom_t*),
    EO_INIT(.item_array_data)   eoprot_sk_rom_folded_descriptors
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eoprot_sk_rom_skin_get_offset(eOprotTag_t tag)
{
    return(s_eoprot_sk_rom_skin_ramoffset(tag));
}

    
extern void* eoprot_sk_rom_get_nvrom(eOprotID32_t id)
{
    uint16_t indexoffoldeddescriptors = s_eoprot_sk_rom_epid2index_of_folded_descriptors(id);
    
    if(EOK_uint16dummy == indexoffoldeddescriptors)
    {
        return(NULL);
    }
    
    return((void*)eoprot_sk_rom_folded_descriptors[indexoffoldeddescriptors]);   
}

extern uint16_t eoprot_sk_rom_get_sizeofvar(eOprotID32_t id)
{     
    EOnv_rom_t* rom = eoprot_sk_rom_get_nvrom(id);  
    if(NULL == rom)
    {
        return(0);
    }    
    return(rom->capacity); 
}

extern uint16_t eoprot_sk_rom_get_prognum(eOprotID32_t id)
{   // we assume that the variables are inserted in a progressive way without holes. and even if there are a few holes never mind.
    return(eoprot_ID2tag(id));
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eoprot_sk_rom_epid2index_of_folded_descriptors(eOprotID32_t id)
{      
    uint16_t tag = eoprot_ID2tag(id);
    
    if(EOK_uint16dummy == tag)
    {
        return(EOK_uint16dummy);
    }
    
    eOprotEntity_t entity = eoprot_ID2entity(id);
    
    switch(entity)
    {
        case eosk_entity_skin:
        {   // nothing to add as the comm vars are in first position
            ; 
        } break;
        
        default:
        {   // error
            tag = EOK_uint16dummy;
        } break;
    
    }
    
    return(tag);   
}


// returns the offset form the start of the struct eOsk_comm_t of the variable with a given tag 
static uint16_t s_eoprot_sk_rom_skin_ramoffset(uint16_t tag)
{   
    //return(eoprot_sk_rom_comm_offsets[tag]);
    uint32_t tmp = ((uint32_t) eoprot_sk_rom_folded_descriptors[tag]->resetval) - ((uint32_t) &eoprot_sk_rom_skin_defaultvalue);
    return((uint16_t)tmp); 
}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



