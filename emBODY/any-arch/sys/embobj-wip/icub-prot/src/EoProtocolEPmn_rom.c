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

/* @file       EoProtocolEPmn_rom.c
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


#include "EoManagement.h"
#include "EoProtocolEPmn.h"
#include "EoProtocolEPmn_fun.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolEPmn_rom.h"


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

static uint16_t s_eoprot_ep_mn_rom_comm_ramoffset(uint16_t tag);
static uint16_t s_eoprot_ep_mn_rom_appl_ramoffset(uint16_t tag);


static void s_eoprot_ep_mn_rom_comm_INIT_cmmnds__ropsigcfg(const EOnv* nv);
static void s_eoprot_ep_mn_rom_comm_UPDT_cmmnds__ropsigcfg(const EOnv* nv, const eOropdescriptor_t* rd);


static void s_eoprot_ep_mn_rom_appl_INIT_config(const EOnv* nv);
static void s_eoprot_ep_mn_rom_appl_UPDT_config(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mn_rom_appl_INIT_status(const EOnv* nv);
static void s_eoprot_ep_mn_rom_appl_UPDT_status(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_mn_rom_appl_INIT_cmmnds__go2state(const EOnv* nv);
static void s_eoprot_ep_mn_rom_appl_UPDT_cmmnds__go2state(const EOnv* nv, const eOropdescriptor_t* rd);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// - default value of a comm

const eOmn_comm_t eoprot_ep_mn_rom_comm_defaultvalue =
{
    EO_INIT(.config)             
    {
        EO_INIT(.filler04)                  {0xf1, 0xf2, 0xf3, 0xf4} 
    },
    EO_INIT(.status)             
    {
        EO_INIT(.filler04)                  {0xf1, 0xf2, 0xf3, 0xf4} 
    },
    EO_INIT(.cmmnds)                       
    {
        EO_INIT(.ropsigcfg)                 
        {
            EO_INIT(.array)
            {
                EO_INIT(.head)
                {
                    EO_INIT(.capacity)      NUMOFROPSIGCFG,
                    EO_INIT(.itemsize)      sizeof(eOropSIGcfg_t),
                    EO_INIT(.size)          0
                },
                EO_INIT(.data)              {0}    
            },
            EO_INIT(.cmmnd)                 ropsigcfg_cmd_none,
            EO_INIT(.filler03)              {0xf1, 0xf2, 0xf3}
        }
    }
}; 

// - default value of a appl

const eOmn_appl_t eoprot_ep_mn_rom_appl_defaultvalue =
{
    EO_INIT(.config)
    {
        EO_INIT(.cycletime)         1000,
        EO_INIT(.filler04)          {0xf1, 0xf2, 0xf3, 0xf4}    
    },
    EO_INIT(.status)
    {
        EO_INIT(.currstate)         applstate_config,  
        EO_INIT(.runmode)           applrunMode__default,
        EO_INIT(.filler06)          {0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6}    
    },
    EO_INIT(.cmmnds)
    {
        EO_INIT(.go2state)          applstate_config,        
        EO_INIT(.filler07)          {0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7}    
    }       
};  


// - descriptors for the variables of a comm

EOnv_rom_t eoprot_ep_mn_rom_comm_descriptor_cmmnds__ropsigcfg =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mn_rom_comm_defaultvalue.cmmnds.ropsigcfg),
    EO_INIT(.funtyp)    eoprot_ep_mn_comm_funtyp_cmmnds__ropsigcfg,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mn_rom_comm_defaultvalue.cmmnds.ropsigcfg,
    EO_INIT(.init)      s_eoprot_ep_mn_rom_comm_INIT_cmmnds__ropsigcfg,
    EO_INIT(.update)    s_eoprot_ep_mn_rom_comm_UPDT_cmmnds__ropsigcfg
};


// - descriptors for the variables of a appl

EOnv_rom_t eoprot_ep_mn_rom_comm_descriptor_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mn_rom_appl_defaultvalue.config),
    EO_INIT(.funtyp)    eoprot_ep_mn_appl_funtyp_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mn_rom_appl_defaultvalue.config,
    EO_INIT(.init)      s_eoprot_ep_mn_rom_appl_INIT_config,
    EO_INIT(.update)    s_eoprot_ep_mn_rom_appl_UPDT_config
};

EOnv_rom_t eoprot_ep_mn_rom_comm_descriptor_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mn_rom_appl_defaultvalue.status),
    EO_INIT(.funtyp)    eoprot_ep_mn_appl_funtyp_status,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mn_rom_appl_defaultvalue.status,
    EO_INIT(.init)      s_eoprot_ep_mn_rom_appl_INIT_status,
    EO_INIT(.update)    s_eoprot_ep_mn_rom_appl_UPDT_status
};

EOnv_rom_t eoprot_ep_mn_rom_comm_descriptor_cmmnds__go2state =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_mn_rom_appl_defaultvalue.cmmnds.go2state),
    EO_INIT(.funtyp)    eoprot_ep_mn_appl_funtyp_cmmnds__go2state,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_mn_rom_appl_defaultvalue.cmmnds.go2state,
    EO_INIT(.init)      s_eoprot_ep_mn_rom_appl_INIT_cmmnds__go2state,
    EO_INIT(.update)    s_eoprot_ep_mn_rom_appl_UPDT_cmmnds__go2state
};




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


const EOnv_rom_t * const eoprot_ep_mn_rom_folded_descriptors[] =
{
    // here are eoprot_ep_mn_comm_tags_numberof descriptors for the comms (equal for every comm)
    &eoprot_ep_mn_rom_comm_descriptor_cmmnds__ropsigcfg,

    // here are eoprot_ep_mn_appl_tags_numberof descriptors for the appls (equal for every appl)
    &eoprot_ep_mn_rom_comm_descriptor_config,
    &eoprot_ep_mn_rom_comm_descriptor_status,
    &eoprot_ep_mn_rom_comm_descriptor_cmmnds__go2state
         
};  EO_VERIFYsizeof(eoprot_ep_mn_rom_folded_descriptors, sizeof(EOnv_rom_t*)*(eoprot_ep_mn_comm_tags_numberof+eoprot_ep_mn_appl_tags_numberof));





const EOconstvector  eoprot_ep_mn_rom_constvector_of_folded_descriptors_dat = 
{
    EO_INIT(.size)              sizeof(eoprot_ep_mn_rom_folded_descriptors)/sizeof(EOnv_rom_t*), 
    EO_INIT(.item_size)         sizeof(EOnv_rom_t*),
    EO_INIT(.item_array_data)   eoprot_ep_mn_rom_folded_descriptors
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eoprot_ep_mn_rom_epid2ramoffset(eOprotEP_t ep, eOprotID_t id)
{
    uint16_t offset = 0;
    uint16_t tag = eoprot_ep_variable_ID2tag(ep, id);
    
    if(EOK_uint16dummy == tag)
    {
        return(EOK_uint16dummy);
    }
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(ep, id);
    uint16_t index = eoprot_ep_variable_ID2index(ep, id);
    
//    if(eoprot_ep_mn_emsboard != ep)
//    {
//        return(EOK_uint16dummy);
//    }
    
    switch(entity)
    {
        case eomn_entity_comm:
        {   // the comms are all displaced at the beginning of the data
            offset = index*sizeof(eOmn_comm_t) + s_eoprot_ep_mn_rom_comm_ramoffset(tag); 
        } break;
        
        case eomn_entity_appl:
        {   // the appls are placed after all the comms in the mn endpoint emsboard (the only one so far).
            uint16_t numberofcomms = 1; // eoprot_ep_mn_comms_numberof_Get(ep);
            offset = numberofcomms*sizeof(eOmn_comm_t) + index*sizeof(eOmn_appl_t) + s_eoprot_ep_mn_rom_appl_ramoffset(tag);  
        } break;      
        
        default:
        {   // error
            offset = EOK_uint16dummy;
        } break;
    
    }
    
    return(offset);  
}

extern void* eoprot_ep_mn_rom_dataepid2nvram(void* data, eOprotEP_t ep, eOprotID_t id)
{
    uint8_t* startofdata = data;
    uint16_t offset = eoprot_ep_mn_rom_epid2ramoffset(ep, id);
    
    if(EOK_uint16dummy == offset)
    {
        return(NULL);
    }   

    return(&startofdata[offset]);
}


extern uint16_t eoprot_ep_mn_rom_epid2index_of_folded_descriptors(eOprotEP_t ep, eOprotID_t id)
{      
    uint16_t tag = eoprot_ep_variable_ID2tag(ep, id);
    
    if(EOK_uint16dummy == tag)
    {
        return(EOK_uint16dummy);
    }
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(ep, id);
    
    switch(entity)
    {
        case eomn_entity_comm:
        {   // nothing to add as the comm vars are in first position
            ; 
        } break;
        
        case eomn_entity_appl:
        {   // must add the number of vars in a comm
            tag += eoprot_ep_mn_comm_tags_numberof; 
        } break;      

        default:
        {   // error
            tag = EOK_uint16dummy;
        } break;
    
    }
    
    return(tag);   
}

extern void* eoprot_ep_mn_rom_epid2nvrom(eOprotEP_t ep, eOprotID_t id)
{
    uint16_t indexoffoldeddescriptors = eoprot_ep_mn_rom_epid2index_of_folded_descriptors(ep, id);
    
    if(EOK_uint16dummy == indexoffoldeddescriptors)
    {
        return(NULL);
    }
    
    return((void*)&eoprot_ep_mn_rom_folded_descriptors[indexoffoldeddescriptors]);   
}

extern uint16_t eoprot_ep_mn_rom_epid2progressivenumber(eOprotEP_t ep, eOprotID_t id)
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
        case eomn_entity_comm:
        {   // the comms are the first ones   
            prognum = index*eoprot_ep_mn_comm_tags_numberof + tag;
        } break;
        
        case eomn_entity_appl:
        {   // the appls are after all comms
            prognum = eoprot_ep_mn_comms_numberof_Get(ep)*eoprot_ep_mn_comm_tags_numberof + 
                      index*eoprot_ep_mn_appl_tags_numberof + tag; 
        } break;      
        
        default:
        {   // error
            prognum = EOK_uint16dummy;
        } break;
    
    }
    
    return(prognum); 
}

extern uint16_t eoprot_ep_mn_rom_ep2variablesnumberof(eOprotEP_t ep)
{     
    uint16_t total = eoprot_ep_mn_comms_numberof_Get(ep)*eoprot_ep_mn_comm_tags_numberof + 
                     eoprot_ep_mn_appls_numberof_Get(ep)*eoprot_ep_mn_appl_tags_numberof;;
    
    return(total); 
}

extern uint16_t eoprot_ep_mn_rom_epid2sizeofvar(eOprotEP_t ep, eOprotID_t id)
{     
    EOnv_rom_t* rom = eoprot_ep_mn_rom_epid2nvrom(ep, id);  
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

// returns the offset form the start of the struct eOmc_comm_t of the variable with a given tag 
static uint16_t s_eoprot_ep_mn_rom_comm_ramoffset(uint16_t tag)
{   
    //return(eoprot_ep_mn_rom_comm_offsets[tag]);
    uint32_t tmp = ((uint32_t) eoprot_ep_mn_rom_folded_descriptors[tag]->resetval) - ((uint32_t) &eoprot_ep_mn_rom_comm_defaultvalue);
    return((uint16_t)tmp); 
}

// returns the offset form the start of the struct eOmc_appl_t of the variable with a given tag 
static uint16_t s_eoprot_ep_mn_rom_appl_ramoffset(uint16_t tag)
{
    //return(eoprot_ep_mn_rom_appl_offsets[tag]);
    uint32_t tmp = ((uint32_t) eoprot_ep_mn_rom_folded_descriptors[tag]->resetval) - ((uint32_t) &eoprot_ep_mn_rom_appl_defaultvalue);
    return((uint16_t)tmp); 
}


// - the functions called by the descriptors


static void s_eoprot_ep_mn_rom_comm_INIT_cmmnds__ropsigcfg(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mn_fun_INIT_comm_cmmnds__ropsigcfg(nv, index);
}
static void s_eoprot_ep_mn_rom_comm_UPDT_cmmnds__ropsigcfg(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mn_fun_UPDT_comm_cmmnds__ropsigcfg(nv, rd, index);
}


static void s_eoprot_ep_mn_rom_appl_INIT_config(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mn_fun_INIT_appl_config(nv, index);
}
static void s_eoprot_ep_mn_rom_appl_UPDT_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mn_fun_UPDT_appl_config(nv, rd, index);
}

static void s_eoprot_ep_mn_rom_appl_INIT_status(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mn_fun_INIT_appl_status(nv, index);
}
static void s_eoprot_ep_mn_rom_appl_UPDT_status(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mn_fun_UPDT_appl_status(nv, rd, index);
}

static void s_eoprot_ep_mn_rom_appl_INIT_cmmnds__go2state(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mn_fun_INIT_appl_cmmnds__go2state(nv, index);
}
static void s_eoprot_ep_mn_rom_appl_UPDT_cmmnds__go2state(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_mn_fun_UPDT_appl_cmmnds__go2state(nv, rd, index);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



