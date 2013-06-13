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

/* @file       EoProtocolEPsk_rom.c
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
#include "EoProtocolEPsk.h"
#include "EoProtocolEPsk_fun.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolEPsk_rom.h"


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

static uint16_t s_eoprot_ep_sk_rom_skin_ramoffset(uint16_t tag);



static void s_eoprot_ep_sk_rom_skin_INIT_config__sigmode(const EOnv* nv);
static void s_eoprot_ep_sk_rom_skin_UPDT_config__sigmode(const EOnv* nv, const eOropdescriptor_t* rd);


static void s_eoprot_ep_sk_rom_skin_INIT_status__arrayof10canframes(const EOnv* nv);
static void s_eoprot_ep_sk_rom_skin_UPDT_status__arrayof10canframes(const EOnv* nv, const eOropdescriptor_t* rd);





// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// - default value of a skin

const eOsk_skin_t eoprot_ep_sk_rom_skin_defaultvalue =
{
    EO_INIT(.config)
    {
        EO_INIT(.sigmode)                       eosk_sigmode_dontsignal,
        EO_INIT(.filler03)                      {0xf1, 0xf2, 0xf3}
    },
    EO_INIT(.inputs)
    {
        EO_INIT(.filler04)                      {0xf1, 0xf2, 0xf3, 0xf4}
    },
    EO_INIT(.status)                       
    {
        EO_INIT(.arrayof10canframes)                     
        {
            EO_INIT(.head)
            {
                EO_INIT(.capacity)              10,
                EO_INIT(.itemsize)              sizeof(eOutil_canframe_t),
                EO_INIT(.size)                  0
            },
            EO_INIT(.data)                      {0}
        }
    }
}; 



// - descriptors for the variables of a skin


EOnv_rom_t eoprot_ep_sk_rom_skin_descriptor_config__sigmode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_sk_rom_skin_defaultvalue.config.sigmode),
    EO_INIT(.funtyp)    eoprot_ep_sk_skin_funtyp_config__sigmode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_sk_rom_skin_defaultvalue.config.sigmode,
    EO_INIT(.init)      s_eoprot_ep_sk_rom_skin_INIT_config__sigmode,
    EO_INIT(.update)    s_eoprot_ep_sk_rom_skin_UPDT_config__sigmode
};


EOnv_rom_t eoprot_ep_sk_rom_skin_descriptor_status__arrayof10canframes =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_sk_rom_skin_defaultvalue.status.arrayof10canframes),
    EO_INIT(.funtyp)    eoprot_ep_sk_skin_funtyp_status__arrayof10canframes,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_sk_rom_skin_defaultvalue.status.arrayof10canframes,
    EO_INIT(.init)      s_eoprot_ep_sk_rom_skin_INIT_status__arrayof10canframes,
    EO_INIT(.update)    s_eoprot_ep_sk_rom_skin_UPDT_status__arrayof10canframes
};



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


const EOnv_rom_t * const eoprot_ep_sk_rom_folded_descriptors[] =
{
    // here are eoprot_ep_sk_skin_tags_numberof descriptors for the skins (equal for every skin)
    &eoprot_ep_sk_rom_skin_descriptor_config__sigmode,
    &eoprot_ep_sk_rom_skin_descriptor_status__arrayof10canframes
         
};  EO_VERIFYsizeof(eoprot_ep_sk_rom_folded_descriptors, sizeof(EOnv_rom_t*)*(eoprot_ep_sk_skin_tags_numberof));




const EOconstvector  eoprot_ep_sk_rom_constvector_of_folded_descriptors_dat = 
{
    EO_INIT(.size)              sizeof(eoprot_ep_sk_rom_folded_descriptors)/sizeof(EOnv_rom_t*), 
    EO_INIT(.item_size)         sizeof(EOnv_rom_t*),
    EO_INIT(.item_array_data)   eoprot_ep_sk_rom_folded_descriptors
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eoprot_ep_sk_rom_epid2ramoffset(eOprotEP_t ep, eOprotID_t id)
{
    uint16_t offset = 0;
    uint16_t tag = eoprot_ep_variable_ID2tag(ep, id);
    
    if(EOK_uint16dummy == tag)
    {
        return(EOK_uint16dummy);
    }
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(ep, id);
    uint16_t index = eoprot_ep_variable_ID2index(ep, id);
    
    
    switch(entity)
    {
        case eosk_entity_skin:
        {   // the skins (if any) are all displaced at the beginning of the data
            offset = index*sizeof(eOsk_skin_t) + s_eoprot_ep_sk_rom_skin_ramoffset(tag); 
        } break;
        

        default:
        {   // error
            offset = EOK_uint16dummy;
        } break;
    
    }
    
    return(offset);  
}

extern void* eoprot_ep_sk_rom_dataepid2nvram(void* data, eOprotEP_t ep, eOprotID_t id)
{
    uint8_t* startofdata = data;
    uint16_t offset = eoprot_ep_sk_rom_epid2ramoffset(ep, id);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   

    return(&startofdata[offset]);
}


extern uint16_t eoprot_ep_sk_rom_epid2index_of_folded_descriptors(eOprotEP_t ep, eOprotID_t id)
{      
    uint16_t tag = eoprot_ep_variable_ID2tag(ep, id);
    
    if(EOK_uint16dummy == tag)
    {
        return(EOK_uint16dummy);
    }
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(ep, id);
    
    switch(entity)
    {
        case eosk_entity_skin:
        {   // nothing to add as the skin vars are in first position
            ; 
        } break;
        
        default:
        {   // error
            tag = EOK_uint16dummy;
        } break;
    
    }
    
    return(tag);   
}

extern void* eoprot_ep_sk_rom_epid2nvrom(eOprotEP_t ep, eOprotID_t id)
{
    uint16_t indexoffoldeddescriptors = eoprot_ep_sk_rom_epid2index_of_folded_descriptors(ep, id);
    
    if(EOK_uint16dummy == indexoffoldeddescriptors)
    {
        return(NULL);
    }
    
    return((void*)&eoprot_ep_sk_rom_folded_descriptors[indexoffoldeddescriptors]);   
}

extern uint16_t eoprot_ep_sk_rom_epid2progressivenumber(eOprotEP_t ep, eOprotID_t id)
{     
    uint16_t tag = eoprot_ep_variable_ID2tag(ep, id);
    uint16_t index = eoprot_ep_variable_ID2index(ep, id);               // ok, even if index shall always be 1
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(ep, id);
    uint16_t prognum = 0;
    
    if((EOK_uint16dummy == tag) || (EOK_uint16dummy == index) || (EOK_uint16dummy == entity))
    {
        return(EOK_uint16dummy);
    }
    
      
    switch(entity)
    {
        case eosk_entity_skin:
        {   // the skins are the first ones   
            prognum = index*eoprot_ep_sk_skin_tags_numberof + tag;
        } break;
                
        default:
        {   // error
            prognum = EOK_uint16dummy;
        } break;
    
    }
    
    return(prognum); 
}

extern uint16_t eoprot_ep_sk_rom_ep2variablesnumberof(eOprotEP_t ep)
{     
    uint16_t total = eoprot_ep_sk_skins_numberof_Get(ep)*eoprot_ep_sk_skin_tags_numberof;
    
    return(total); 
}

extern uint16_t eoprot_ep_sk_rom_epid2sizeofvar(eOprotEP_t ep, eOprotID_t id)
{     
    EOnv_rom_t* rom = eoprot_ep_sk_rom_epid2nvrom(ep, id);  
    if(NULL == rom)
    {
        return(0);
    }    
    return(rom->capacity); 
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

// returns the offset form the start of the struct eOmc_skin_t of the variable with a given tag 
static uint16_t s_eoprot_ep_sk_rom_skin_ramoffset(uint16_t tag)
{   
    //return(eoprot_ep_sk_rom_skin_offsets[tag]);
    uint32_t tmp = ((uint32_t) eoprot_ep_sk_rom_folded_descriptors[tag]->resetval) - ((uint32_t) &eoprot_ep_sk_rom_skin_defaultvalue);
    return((uint16_t)tmp); 
}



// - the functions called by the descriptors


static void s_eoprot_ep_sk_rom_skin_INIT_config__sigmode(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_INIT_skin_config__sigmode(nv, index);
}
static void s_eoprot_ep_sk_rom_skin_UPDT_config__sigmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_UPDT_skin_config__sigmode(nv, rd, index);
}

static void s_eoprot_ep_sk_rom_skin_INIT_status__arrayof10canframes(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_INIT_skin_status__arrayof10canframes(nv, index);
}
static void s_eoprot_ep_sk_rom_skin_UPDT_status__arrayof10canframes(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_UPDT_skin_status__arrayof10canframes(nv, rd, index);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



