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

/* @file       EoProtocolMN_rom.c
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

#include "EoProtocolMN.h"
#include "EoMotionControl.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolMN_rom.h"


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

static uint16_t s_eoprot_mn_rom_epid2index_of_folded_descriptors(eOprotID32_t id);
static uint16_t s_eoprot_mn_rom_entity_offset_of_tag(const void* entityrom, eOprotTag_t tag);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// - default value of a comm
static const eOmn_comm_t eoprot_mn_rom_comm_defaultvalue = { 0 };

// - default value of a appl
static const eOmn_appl_t eoprot_mn_rom_appl_defaultvalue = { 0 };


// - descriptors for the variables of a comm

static EOnv_rom_t eoprot_mn_rom_descriptor_comm_wholeitem =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mn_rom_comm_defaultvalue),
    EO_INIT(.rwmode)    eoprot_rwm_mn_comm_wholeitem,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mn_rom_comm_defaultvalue,
    EO_INIT(.init)      eoprot_fun_INIT_mn_comm_wholeitem,
    EO_INIT(.update)    eoprot_fun_UPDT_mn_comm_wholeitem
};

static EOnv_rom_t eoprot_mn_rom_descriptor_comm_cmmnds_ropsigcfg =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mn_rom_comm_defaultvalue.cmmnds.ropsigcfg),
    EO_INIT(.rwmode)    eoprot_rwm_mn_comm_cmmnds_ropsigcfg,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mn_rom_comm_defaultvalue.cmmnds.ropsigcfg,
    EO_INIT(.init)      eoprot_fun_INIT_mn_comm_cmmnds_ropsigcfg,
    EO_INIT(.update)    eoprot_fun_UPDT_mn_comm_cmmnds_ropsigcfg
};


// - descriptors for the variables of a appl


static EOnv_rom_t eoprot_mn_rom_descriptor_appl_wholeitem =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mn_rom_appl_defaultvalue),
    EO_INIT(.rwmode)    eoprot_rwm_mn_appl_wholeitem,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mn_rom_appl_defaultvalue,
    EO_INIT(.init)      eoprot_fun_INIT_mn_appl_wholeitem,
    EO_INIT(.update)    eoprot_fun_UPDT_mn_appl_wholeitem
};

static EOnv_rom_t eoprot_mn_rom_descriptor_appl_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mn_rom_appl_defaultvalue.config),
    EO_INIT(.rwmode)    eoprot_rwm_mn_appl_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mn_rom_appl_defaultvalue.config,
    EO_INIT(.init)      eoprot_fun_INIT_mn_appl_config,
    EO_INIT(.update)    eoprot_fun_UPDT_mn_appl_config
};

static EOnv_rom_t eoprot_mn_rom_descriptor_appl_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mn_rom_appl_defaultvalue.status),
    EO_INIT(.rwmode)    eoprot_rwm_mn_appl_status,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mn_rom_appl_defaultvalue.status,
    EO_INIT(.init)      eoprot_fun_INIT_mn_appl_status,
    EO_INIT(.update)    eoprot_fun_UPDT_mn_appl_status
};

static EOnv_rom_t eoprot_mn_rom_descriptor_appl_cmmnds_go2state =
{   
    EO_INIT(.capacity)  sizeof(eoprot_mn_rom_appl_defaultvalue.cmmnds.go2state),
    EO_INIT(.rwmode)    eoprot_rwm_mn_appl_cmmnds_go2state,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_mn_rom_appl_defaultvalue.cmmnds.go2state,
    EO_INIT(.init)      eoprot_fun_INIT_mn_appl_cmmnds_go2state,
    EO_INIT(.update)    eoprot_fun_UPDT_mn_appl_cmmnds_go2state
};


// -- the folded array of descriptors

const static EOnv_rom_t * const eoprot_mn_rom_folded_descriptors[] =
{
    // here are eoprot_mn_tags_comm_numberof descriptors for the comms (equal for every comm)
    &eoprot_mn_rom_descriptor_comm_wholeitem,
    &eoprot_mn_rom_descriptor_comm_cmmnds_ropsigcfg,

    // here are eoprot_mn_tags_appl_numberof descriptors for the appls (equal for every appl)
    &eoprot_mn_rom_descriptor_appl_wholeitem,
    &eoprot_mn_rom_descriptor_appl_config,
    &eoprot_mn_rom_descriptor_appl_status,
    &eoprot_mn_rom_descriptor_appl_cmmnds_go2state
         
};  EO_VERIFYsizeof(eoprot_mn_rom_folded_descriptors, sizeof(EOnv_rom_t*)*(eoprot_tags_mn_comm_numberof+eoprot_tags_mn_appl_numberof));



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------



const EOconstvector  eoprot_mn_rom_constvector_of_folded_descriptors_dat = 
{
    EO_INIT(.size)              sizeof(eoprot_mn_rom_folded_descriptors)/sizeof(EOnv_rom_t*), 
    EO_INIT(.item_size)         sizeof(EOnv_rom_t*),
    EO_INIT(.item_array_data)   eoprot_mn_rom_folded_descriptors
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern uint16_t eoprot_mn_rom_get_offset(eOprotEntity_t entity, eOprotTag_t tag)
{
    const void* startofrom = NULL;
    
    switch(entity)
    {
        case eomn_entity_comm:
        {   
            startofrom = &eoprot_mn_rom_comm_defaultvalue; 
        } break;
        
        case eomn_entity_appl:
        {   
            startofrom = &eoprot_mn_rom_appl_defaultvalue;
        } break;  
       
        default:
        {   
            return(EOK_uint16dummy);
        } //break; // commented the break just to remove a warning  
    }    
    
    return(s_eoprot_mn_rom_entity_offset_of_tag(startofrom, tag));
}


extern void* eoprot_mn_rom_get_nvrom(eOprotID32_t id)
{
    uint16_t indexoffoldeddescriptors = s_eoprot_mn_rom_epid2index_of_folded_descriptors(id);
    
    if(EOK_uint16dummy == indexoffoldeddescriptors)
    {
        return(NULL);
    }
    
    return((void*)eoprot_mn_rom_folded_descriptors[indexoffoldeddescriptors]);   
}

extern uint16_t eoprot_mn_rom_get_sizeofvar(eOprotID32_t id)
{     
    EOnv_rom_t* rom = eoprot_mn_rom_get_nvrom(id);  
    if(NULL == rom)
    {
        return(0);
    }    
    return(rom->capacity); 
}

extern uint16_t eoprot_mn_rom_get_prognum(eOprotID32_t id)
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

static uint16_t s_eoprot_mn_rom_epid2index_of_folded_descriptors(eOprotID32_t id)
{      
    uint16_t ret = eoprot_ID2tag(id);
    
    // dont check validity of the tag. we could check inside the case xxxx: by verifying if ret is higher than 
    // the max number of tags for that entity.
       
    eOprotEntity_t entity = eoprot_ID2entity(id);
    
    switch(entity)
    {
        case eomn_entity_comm:
        {   // nothing to add as the comm vars are in first position
            ; 
        } break;
        
        case eomn_entity_appl:
        {   // must add the number of vars in a comm
            ret += eoprot_tags_mn_comm_numberof; 
        } break;      

        default:
        {   // error
            ret = EOK_uint16dummy;
        } break;
    
    }
    
    return(ret);   
}

// returns the offset of the variable with a given tag from the start of the entity
static uint16_t s_eoprot_mn_rom_entity_offset_of_tag(const void* entityrom, eOprotTag_t tag)
{
    uint32_t tmp = ((uint32_t) eoprot_mn_rom_folded_descriptors[tag]->resetval) - ((uint32_t) entityrom);
    return((uint16_t)tmp); 
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



