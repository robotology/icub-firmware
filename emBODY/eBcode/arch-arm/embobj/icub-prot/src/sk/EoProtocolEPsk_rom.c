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

static uint16_t s_eoprot_ep_sk_rom_strain_ramoffset(uint16_t tag);
static uint16_t s_eoprot_ep_sk_rom_mais_ramoffset(uint16_t tag);


static void s_eoprot_ep_sk_rom_strain_INIT_config(const EOnv* nv);
static void s_eoprot_ep_sk_rom_strain_UPDT_config(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_sk_rom_strain_INIT_config__mode(const EOnv* nv);
static void s_eoprot_ep_sk_rom_strain_UPDT_config__mode(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_sk_rom_strain_INIT_config__datarate(const EOnv* nv);
static void s_eoprot_ep_sk_rom_strain_UPDT_config__datarate(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_sk_rom_strain_INIT_config__signaloncefullscale(const EOnv* nv);
static void s_eoprot_ep_sk_rom_strain_UPDT_config__signaloncefullscale(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_sk_rom_strain_INIT_status(const EOnv* nv);
static void s_eoprot_ep_sk_rom_strain_UPDT_status(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_sk_rom_strain_INIT_status__fullscale(const EOnv* nv);
static void s_eoprot_ep_sk_rom_strain_UPDT_status__fullscale(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_sk_rom_strain_INIT_status__calibratedvalues(const EOnv* nv);
static void s_eoprot_ep_sk_rom_strain_UPDT_status__calibratedvalues(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_sk_rom_strain_INIT_status__uncalibratedvalues(const EOnv* nv);
static void s_eoprot_ep_sk_rom_strain_UPDT_status__uncalibratedvalues(const EOnv* nv, const eOropdescriptor_t* rd);


static void s_eoprot_ep_sk_rom_mais_INIT_config(const EOnv* nv);
static void s_eoprot_ep_sk_rom_mais_UPDT_config(const EOnv* nv, const eOropdescriptor_t* rd);


static void s_eoprot_ep_sk_rom_mais_INIT_config__mode(const EOnv* nv);
static void s_eoprot_ep_sk_rom_mais_UPDT_config__mode(const EOnv* nv, const eOropdescriptor_t* rd);


static void s_eoprot_ep_sk_rom_mais_INIT_config__datarate(const EOnv* nv);
static void s_eoprot_ep_sk_rom_mais_UPDT_config__datarate(const EOnv* nv, const eOropdescriptor_t* rd);


static void s_eoprot_ep_sk_rom_mais_INIT_config__resolution(const EOnv* nv);
static void s_eoprot_ep_sk_rom_mais_UPDT_config__resolution(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_sk_rom_mais_INIT_status(const EOnv* nv);
static void s_eoprot_ep_sk_rom_mais_UPDT_status(const EOnv* nv, const eOropdescriptor_t* rd);

static void s_eoprot_ep_sk_rom_mais_INIT_status__the15values(const EOnv* nv);
static void s_eoprot_ep_sk_rom_mais_UPDT_status__the15values(const EOnv* nv, const eOropdescriptor_t* rd);




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// - default value of a strain

const eOsk_strain_t eoprot_ep_sk_rom_strain_defaultvalue =
{
    EO_INIT(.config)
    {
        EO_INIT(.mode)                          eoas_strainmode_acquirebutdonttx,
        EO_INIT(.datarate)                      1,    
        EO_INIT(.signaloncefullscale)           eobool_true,  
        EO_INIT(.filler01)                      {0xf1}
    },
    EO_INIT(.inputs)
    {
        EO_INIT(.filler04)                      {0xf1, 0xf2, 0xf3, 0xf4}
    },
    EO_INIT(.status)                       
    {
        EO_INIT(.fullscale)                     
        {
            EO_INIT(.head)
            {
                EO_INIT(.capacity)              6,
                EO_INIT(.itemsize)              2,
                EO_INIT(.size)                  0
            },
            EO_INIT(.data)                      {0}
        },
        EO_INIT(.calibratedvalues)
        {
            EO_INIT(.head)
            {
                EO_INIT(.capacity)              6,
                EO_INIT(.itemsize)              2,
                EO_INIT(.size)                  0
            },
            EO_INIT(.data)                      {0}
        },
        EO_INIT(.uncalibratedvalues)            
        {
            EO_INIT(.head)
            {
                EO_INIT(.capacity)              6,
                EO_INIT(.itemsize)              2,
                EO_INIT(.size)                  0
            },
            EO_INIT(.data)                      {0}
        }
    }
}; 

// - default value of a appl

const eOsk_mais_t eoprot_ep_sk_rom_mais_defaultvalue =
{
    EO_INIT(.config)
    {
        EO_INIT(.mode)                          eoas_maismode_acquirebutdonttx,
        EO_INIT(.datarate)                      1,    
        EO_INIT(.resolution)                    eoas_maisresolution_08,  
        EO_INIT(.filler01)                      {0xf1}
    },
    EO_INIT(.inputs)
    {
        EO_INIT(.filler04)                      {0xf1, 0xf2, 0xf3, 0xf4}
    },
    EO_INIT(.status)                       
    {
        EO_INIT(.the15values)                   
        {
            EO_INIT(.head)
            {
                EO_INIT(.capacity)              16, // if resolution is eoas_maisresolution_08
                EO_INIT(.itemsize)              1,  // 1 byte if resolution is eoas_maisresolution_08
                EO_INIT(.size)                  0
            },
            EO_INIT(.data)                      {0}
        }
    }
};  


// - descriptors for the variables of a strain

EOnv_rom_t eoprot_ep_sk_rom_strain_descriptor_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_sk_rom_strain_defaultvalue.config),
    EO_INIT(.funtyp)    eoprot_ep_sk_strain_funtyp_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_sk_rom_strain_defaultvalue.config,
    EO_INIT(.init)      s_eoprot_ep_sk_rom_strain_INIT_config,
    EO_INIT(.update)    s_eoprot_ep_sk_rom_strain_UPDT_config
};

EOnv_rom_t eoprot_ep_sk_rom_strain_descriptor_config__mode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_sk_rom_strain_defaultvalue.config.mode),
    EO_INIT(.funtyp)    eoprot_ep_sk_strain_funtyp_config__mode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_sk_rom_strain_defaultvalue.config.mode,
    EO_INIT(.init)      s_eoprot_ep_sk_rom_strain_INIT_config__mode,
    EO_INIT(.update)    s_eoprot_ep_sk_rom_strain_UPDT_config__mode
};

EOnv_rom_t eoprot_ep_sk_rom_strain_descriptor_config__datarate =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_sk_rom_strain_defaultvalue.config.datarate),
    EO_INIT(.funtyp)    eoprot_ep_sk_strain_funtyp_config__datarate,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_sk_rom_strain_defaultvalue.config.datarate,
    EO_INIT(.init)      s_eoprot_ep_sk_rom_strain_INIT_config__datarate,
    EO_INIT(.update)    s_eoprot_ep_sk_rom_strain_UPDT_config__datarate
};

EOnv_rom_t eoprot_ep_sk_rom_strain_descriptor_config__signaloncefullscale =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_sk_rom_strain_defaultvalue.config.signaloncefullscale),
    EO_INIT(.funtyp)    eoprot_ep_sk_strain_funtyp_config__signaloncefullscale,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_sk_rom_strain_defaultvalue.config.signaloncefullscale,
    EO_INIT(.init)      s_eoprot_ep_sk_rom_strain_INIT_config__signaloncefullscale,
    EO_INIT(.update)    s_eoprot_ep_sk_rom_strain_UPDT_config__signaloncefullscale
};

EOnv_rom_t eoprot_ep_sk_rom_strain_descriptor_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_sk_rom_strain_defaultvalue.status),
    EO_INIT(.funtyp)    eoprot_ep_sk_strain_funtyp_status,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_sk_rom_strain_defaultvalue.status,
    EO_INIT(.init)      s_eoprot_ep_sk_rom_strain_INIT_status,
    EO_INIT(.update)    s_eoprot_ep_sk_rom_strain_UPDT_status
};

EOnv_rom_t eoprot_ep_sk_rom_strain_descriptor_status__fullscale =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_sk_rom_strain_defaultvalue.status.fullscale),
    EO_INIT(.funtyp)    eoprot_ep_sk_strain_funtyp_status__fullscale,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_sk_rom_strain_defaultvalue.status.fullscale,
    EO_INIT(.init)      s_eoprot_ep_sk_rom_strain_INIT_status__fullscale,
    EO_INIT(.update)    s_eoprot_ep_sk_rom_strain_UPDT_status__fullscale
};

EOnv_rom_t eoprot_ep_sk_rom_strain_descriptor_status__calibratedvalues =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_sk_rom_strain_defaultvalue.status.calibratedvalues),
    EO_INIT(.funtyp)    eoprot_ep_sk_strain_funtyp_status__calibratedvalues,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_sk_rom_strain_defaultvalue.status.calibratedvalues,
    EO_INIT(.init)      s_eoprot_ep_sk_rom_strain_INIT_status__calibratedvalues,
    EO_INIT(.update)    s_eoprot_ep_sk_rom_strain_UPDT_status__calibratedvalues
};

EOnv_rom_t eoprot_ep_sk_rom_strain_descriptor_status__uncalibratedvalues =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_sk_rom_strain_defaultvalue.status.uncalibratedvalues),
    EO_INIT(.funtyp)    eoprot_ep_sk_strain_funtyp_status__uncalibratedvalues,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_sk_rom_strain_defaultvalue.status.uncalibratedvalues,
    EO_INIT(.init)      s_eoprot_ep_sk_rom_strain_INIT_status__uncalibratedvalues,
    EO_INIT(.update)    s_eoprot_ep_sk_rom_strain_UPDT_status__uncalibratedvalues
};

// - descriptors for the variables of a mais

EOnv_rom_t eoprot_ep_sk_rom_mais_descriptor_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_sk_rom_mais_defaultvalue.config),
    EO_INIT(.funtyp)    eoprot_ep_sk_mais_funtyp_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_sk_rom_mais_defaultvalue.config,
    EO_INIT(.init)      s_eoprot_ep_sk_rom_mais_INIT_config,
    EO_INIT(.update)    s_eoprot_ep_sk_rom_mais_UPDT_config
};

EOnv_rom_t eoprot_ep_sk_rom_mais_descriptor_config__mode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_sk_rom_mais_defaultvalue.config.mode),
    EO_INIT(.funtyp)    eoprot_ep_sk_mais_funtyp_config__mode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_sk_rom_mais_defaultvalue.config.mode,
    EO_INIT(.init)      s_eoprot_ep_sk_rom_mais_INIT_config__mode,
    EO_INIT(.update)    s_eoprot_ep_sk_rom_mais_UPDT_config__mode
};

EOnv_rom_t eoprot_ep_sk_rom_mais_descriptor_config__datarate =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_sk_rom_mais_defaultvalue.config.datarate),
    EO_INIT(.funtyp)    eoprot_ep_sk_mais_funtyp_config__datarate,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_sk_rom_mais_defaultvalue.config.datarate,
    EO_INIT(.init)      s_eoprot_ep_sk_rom_mais_INIT_config__datarate,
    EO_INIT(.update)    s_eoprot_ep_sk_rom_mais_UPDT_config__datarate
};

EOnv_rom_t eoprot_ep_sk_rom_mais_descriptor_config__resolution =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_sk_rom_mais_defaultvalue.config.resolution),
    EO_INIT(.funtyp)    eoprot_ep_sk_mais_funtyp_config__resolution,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_sk_rom_mais_defaultvalue.config.resolution,
    EO_INIT(.init)      s_eoprot_ep_sk_rom_mais_INIT_config__resolution,
    EO_INIT(.update)    s_eoprot_ep_sk_rom_mais_UPDT_config__resolution
};

EOnv_rom_t eoprot_ep_sk_rom_mais_descriptor_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_sk_rom_mais_defaultvalue.status),
    EO_INIT(.funtyp)    eoprot_ep_sk_mais_funtyp_status,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_sk_rom_mais_defaultvalue.status,
    EO_INIT(.init)      s_eoprot_ep_sk_rom_mais_INIT_status,
    EO_INIT(.update)    s_eoprot_ep_sk_rom_mais_UPDT_status
};

EOnv_rom_t eoprot_ep_sk_rom_mais_descriptor_status__the15values =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_sk_rom_mais_defaultvalue.status.the15values),
    EO_INIT(.funtyp)    eoprot_ep_sk_mais_funtyp_status__the15values,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_sk_rom_mais_defaultvalue.status.the15values,
    EO_INIT(.init)      s_eoprot_ep_sk_rom_mais_INIT_status__the15values,
    EO_INIT(.update)    s_eoprot_ep_sk_rom_mais_UPDT_status__the15values
};



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


const EOnv_rom_t * const eoprot_ep_sk_rom_folded_descriptors[] =
{
    // here are eoprot_ep_sk_strain_tags_numberof descriptors for the strains (equal for every strain)
    &eoprot_ep_sk_rom_strain_descriptor_config,
    &eoprot_ep_sk_rom_strain_descriptor_config__mode,
    &eoprot_ep_sk_rom_strain_descriptor_config__datarate,
    &eoprot_ep_sk_rom_strain_descriptor_config__signaloncefullscale,
    &eoprot_ep_sk_rom_strain_descriptor_status,
    &eoprot_ep_sk_rom_strain_descriptor_status__fullscale,
    &eoprot_ep_sk_rom_strain_descriptor_status__calibratedvalues,
    &eoprot_ep_sk_rom_strain_descriptor_status__uncalibratedvalues,

    // here are eoprot_ep_sk_appl_tags_numberof descriptors for the mais (equal for every appl)
    &eoprot_ep_sk_rom_mais_descriptor_config,
    &eoprot_ep_sk_rom_mais_descriptor_config__mode,
    &eoprot_ep_sk_rom_mais_descriptor_config__datarate,
    &eoprot_ep_sk_rom_mais_descriptor_config__resolution,
    &eoprot_ep_sk_rom_mais_descriptor_status,
    &eoprot_ep_sk_rom_mais_descriptor_status__the15values
         
};  EO_VERIFYsizeof(eoprot_ep_sk_rom_folded_descriptors, sizeof(EOnv_rom_t*)*(eoprot_ep_sk_strain_tags_numberof+eoprot_ep_sk_mais_tags_numberof));





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
        case eoas_entity_strain:
        {   // the strains (if any) are all displaced at the beginning of the data
            offset = index*sizeof(eOsk_strain_t) + s_eoprot_ep_sk_rom_strain_ramoffset(tag); 
        } break;
        
        case eoas_entity_mais:
        {   // the appls are placed after all the strains 
            uint16_t numberofstrains = eoprot_ep_sk_strains_numberof_Get(ep);
            offset = numberofstrains*sizeof(eOsk_strain_t) + index*sizeof(eOsk_mais_t) + s_eoprot_ep_sk_rom_mais_ramoffset(tag);  
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
        case eoas_entity_strain:
        {   // nothing to add as the strain vars are in first position
            ; 
        } break;
        
        case eoas_entity_mais:
        {   // must add the number of vars in a strain
            tag += eoprot_ep_sk_strain_tags_numberof; 
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
        case eoas_entity_strain:
        {   // the strains are the first ones   
            prognum = index*eoprot_ep_sk_strain_tags_numberof + tag;
        } break;
        
        case eoas_entity_mais:
        {   // the appls are after all strains
            prognum = eoprot_ep_sk_strains_numberof_Get(ep)*eoprot_ep_sk_strain_tags_numberof + 
                      index*eoprot_ep_sk_mais_tags_numberof + tag; 
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
    uint16_t total = eoprot_ep_sk_strains_numberof_Get(ep)*eoprot_ep_sk_strain_tags_numberof + 
                     eoprot_ep_sk_maises_numberof_Get(ep)*eoprot_ep_sk_mais_tags_numberof;;
    
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

// returns the offset form the start of the struct eOmc_strain_t of the variable with a given tag 
static uint16_t s_eoprot_ep_sk_rom_strain_ramoffset(uint16_t tag)
{   
    //return(eoprot_ep_sk_rom_strain_offsets[tag]);
    uint32_t tmp = ((uint32_t) eoprot_ep_sk_rom_folded_descriptors[tag]->resetval) - ((uint32_t) &eoprot_ep_sk_rom_strain_defaultvalue);
    return((uint16_t)tmp); 
}

// returns the offset form the start of the struct eOmc_appl_t of the variable with a given tag 
static uint16_t s_eoprot_ep_sk_rom_mais_ramoffset(uint16_t tag)
{
    //return(eoprot_ep_sk_rom_appl_offsets[tag]);
    uint32_t tmp = ((uint32_t) eoprot_ep_sk_rom_folded_descriptors[tag]->resetval) - ((uint32_t) &eoprot_ep_sk_rom_mais_defaultvalue);
    return((uint16_t)tmp); 
}


// - the functions called by the descriptors


static void s_eoprot_ep_sk_rom_strain_INIT_config(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_INIT_strain_config(nv, index);
}
static void s_eoprot_ep_sk_rom_strain_UPDT_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_UPDT_strain_config(nv, rd, index);
}

static void s_eoprot_ep_sk_rom_strain_INIT_config__mode(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_INIT_strain_config__mode(nv, index);
}
static void s_eoprot_ep_sk_rom_strain_UPDT_config__mode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_UPDT_strain_config__mode(nv, rd, index);
}

static void s_eoprot_ep_sk_rom_strain_INIT_config__datarate(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_INIT_strain_config__datarate(nv, index);
}
static void s_eoprot_ep_sk_rom_strain_UPDT_config__datarate(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_UPDT_strain_config__datarate(nv, rd, index);
}

static void s_eoprot_ep_sk_rom_strain_INIT_config__signaloncefullscale(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_INIT_strain_config__signaloncefullscale(nv, index);
}
static void s_eoprot_ep_sk_rom_strain_UPDT_config__signaloncefullscale(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_UPDT_strain_config__signaloncefullscale(nv, rd, index);
}

static void s_eoprot_ep_sk_rom_strain_INIT_status(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_INIT_strain_status(nv, index);
}
static void s_eoprot_ep_sk_rom_strain_UPDT_status(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_UPDT_strain_status(nv, rd, index);
}

static void s_eoprot_ep_sk_rom_strain_INIT_status__fullscale(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_INIT_strain_status__fullscale(nv, index);
}
static void s_eoprot_ep_sk_rom_strain_UPDT_status__fullscale(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_UPDT_strain_status__fullscale(nv, rd, index);
}

static void s_eoprot_ep_sk_rom_strain_INIT_status__calibratedvalues(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_INIT_strain_status__calibratedvalues(nv, index);
}
static void s_eoprot_ep_sk_rom_strain_UPDT_status__calibratedvalues(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_UPDT_strain_status__calibratedvalues(nv, rd, index);
}

static void s_eoprot_ep_sk_rom_strain_INIT_status__uncalibratedvalues(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_INIT_strain_status__uncalibratedvalues(nv, index);
}
static void s_eoprot_ep_sk_rom_strain_UPDT_status__uncalibratedvalues(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_UPDT_strain_status__uncalibratedvalues(nv, rd, index);
}


static void s_eoprot_ep_sk_rom_mais_INIT_config(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_INIT_mais_config(nv, index);
}
static void s_eoprot_ep_sk_rom_mais_UPDT_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_UPDT_mais_config(nv, rd, index);
}


static void s_eoprot_ep_sk_rom_mais_INIT_config__mode(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_INIT_mais_config__mode(nv, index);
}
static void s_eoprot_ep_sk_rom_mais_UPDT_config__mode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_UPDT_mais_config__mode(nv, rd, index);
}


static void s_eoprot_ep_sk_rom_mais_INIT_config__datarate(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_INIT_mais_config__datarate(nv, index);
}
static void s_eoprot_ep_sk_rom_mais_UPDT_config__datarate(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_UPDT_mais_config__datarate(nv, rd, index);
}


static void s_eoprot_ep_sk_rom_mais_INIT_config__resolution(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_INIT_mais_config__resolution(nv, index);
}
static void s_eoprot_ep_sk_rom_mais_UPDT_config__resolution(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_UPDT_mais_config__resolution(nv, rd, index);
}

static void s_eoprot_ep_sk_rom_mais_INIT_status(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_INIT_mais_status(nv, index);
}
static void s_eoprot_ep_sk_rom_mais_UPDT_status(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_UPDT_mais_status(nv, rd, index);
}

static void s_eoprot_ep_sk_rom_mais_INIT_status__the15values(const EOnv* nv)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_INIT_mais_status__the15values(nv, index);
}
static void s_eoprot_ep_sk_rom_mais_UPDT_status__the15values(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint16_t index = eoprot_ep_variable_ID2index(nv->ep, nv->id);
    eoprot_ep_sk_fun_UPDT_mais_status__the15values(nv, rd, index);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



