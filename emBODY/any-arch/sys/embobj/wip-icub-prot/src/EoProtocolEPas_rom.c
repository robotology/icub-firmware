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

/* @file       EoProtocolEPas_rom.c
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


#include "EoAnalogSensors.h"
#include "EoProtocolEPas.h"
#include "EoProtocolEPas_fun.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolEPas_rom.h"


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

static uint16_t s_eoprot_ep_as_rom_epid2index_of_folded_descriptors(eOprotID_t id);

static uint16_t s_eoprot_ep_as_rom_strain_ramoffset(uint16_t tag);
static uint16_t s_eoprot_ep_as_rom_mais_ramoffset(uint16_t tag);




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// - default value of a strain

#if 1
const eOas_strain_t eoprot_ep_as_rom_strain_defaultvalue = { 0 };
#else
const eOas_strain_t eoprot_ep_as_rom_strain_defaultvalue =
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
#endif


// - default value of a mais

#if 1
const eOas_mais_t eoprot_ep_as_rom_mais_defaultvalue = { 0 };
#else
const eOas_mais_t eoprot_ep_as_rom_mais_defaultvalue =
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
#endif


// - descriptors for the variables of a strain

EOnv_rom_t eoprot_ep_as_rom_strain_descriptor_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_as_rom_strain_defaultvalue.config),
    EO_INIT(.rwmode)    eoprot_ep_as_strain_rwmode_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_as_rom_strain_defaultvalue.config,
    EO_INIT(.init)      eoprot_ep_as_fun_INIT_strain_config,
    EO_INIT(.update)    eoprot_ep_as_fun_UPDT_strain_config
};

EOnv_rom_t eoprot_ep_as_rom_strain_descriptor_config__mode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_as_rom_strain_defaultvalue.config.mode),
    EO_INIT(.rwmode)    eoprot_ep_as_strain_rwmode_config__mode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_as_rom_strain_defaultvalue.config.mode,
    EO_INIT(.init)      eoprot_ep_as_fun_INIT_strain_config__mode,
    EO_INIT(.update)    eoprot_ep_as_fun_UPDT_strain_config__mode
};

EOnv_rom_t eoprot_ep_as_rom_strain_descriptor_config__datarate =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_as_rom_strain_defaultvalue.config.datarate),
    EO_INIT(.rwmode)    eoprot_ep_as_strain_rwmode_config__datarate,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_as_rom_strain_defaultvalue.config.datarate,
    EO_INIT(.init)      eoprot_ep_as_fun_INIT_strain_config__datarate,
    EO_INIT(.update)    eoprot_ep_as_fun_UPDT_strain_config__datarate
};

EOnv_rom_t eoprot_ep_as_rom_strain_descriptor_config__signaloncefullscale =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_as_rom_strain_defaultvalue.config.signaloncefullscale),
    EO_INIT(.rwmode)    eoprot_ep_as_strain_rwmode_config__signaloncefullscale,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_as_rom_strain_defaultvalue.config.signaloncefullscale,
    EO_INIT(.init)      eoprot_ep_as_fun_INIT_strain_config__signaloncefullscale,
    EO_INIT(.update)    eoprot_ep_as_fun_UPDT_strain_config__signaloncefullscale
};

EOnv_rom_t eoprot_ep_as_rom_strain_descriptor_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_as_rom_strain_defaultvalue.status),
    EO_INIT(.rwmode)    eoprot_ep_as_strain_rwmode_status,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_as_rom_strain_defaultvalue.status,
    EO_INIT(.init)      eoprot_ep_as_fun_INIT_strain_status,
    EO_INIT(.update)    eoprot_ep_as_fun_UPDT_strain_status
};

EOnv_rom_t eoprot_ep_as_rom_strain_descriptor_status__fullscale =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_as_rom_strain_defaultvalue.status.fullscale),
    EO_INIT(.rwmode)    eoprot_ep_as_strain_rwmode_status__fullscale,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_as_rom_strain_defaultvalue.status.fullscale,
    EO_INIT(.init)      eoprot_ep_as_fun_INIT_strain_status__fullscale,
    EO_INIT(.update)    eoprot_ep_as_fun_UPDT_strain_status__fullscale
};

EOnv_rom_t eoprot_ep_as_rom_strain_descriptor_status__calibratedvalues =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_as_rom_strain_defaultvalue.status.calibratedvalues),
    EO_INIT(.rwmode)    eoprot_ep_as_strain_rwmode_status__calibratedvalues,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_as_rom_strain_defaultvalue.status.calibratedvalues,
    EO_INIT(.init)      eoprot_ep_as_fun_INIT_strain_status__calibratedvalues,
    EO_INIT(.update)    eoprot_ep_as_fun_UPDT_strain_status__calibratedvalues
};

EOnv_rom_t eoprot_ep_as_rom_strain_descriptor_status__uncalibratedvalues =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_as_rom_strain_defaultvalue.status.uncalibratedvalues),
    EO_INIT(.rwmode)    eoprot_ep_as_strain_rwmode_status__uncalibratedvalues,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_as_rom_strain_defaultvalue.status.uncalibratedvalues,
    EO_INIT(.init)      eoprot_ep_as_fun_INIT_strain_status__uncalibratedvalues,
    EO_INIT(.update)    eoprot_ep_as_fun_UPDT_strain_status__uncalibratedvalues
};

// - descriptors for the variables of a mais

EOnv_rom_t eoprot_ep_as_rom_mais_descriptor_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_as_rom_mais_defaultvalue.config),
    EO_INIT(.rwmode)    eoprot_ep_as_mais_rwmode_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_as_rom_mais_defaultvalue.config,
    EO_INIT(.init)      eoprot_ep_as_fun_INIT_mais_config,
    EO_INIT(.update)    eoprot_ep_as_fun_UPDT_mais_config
};

EOnv_rom_t eoprot_ep_as_rom_mais_descriptor_config__mode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_as_rom_mais_defaultvalue.config.mode),
    EO_INIT(.rwmode)    eoprot_ep_as_mais_rwmode_config__mode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_as_rom_mais_defaultvalue.config.mode,
    EO_INIT(.init)      eoprot_ep_as_fun_INIT_mais_config__mode,
    EO_INIT(.update)    eoprot_ep_as_fun_UPDT_mais_config__mode
};

EOnv_rom_t eoprot_ep_as_rom_mais_descriptor_config__datarate =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_as_rom_mais_defaultvalue.config.datarate),
    EO_INIT(.rwmode)    eoprot_ep_as_mais_rwmode_config__datarate,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_as_rom_mais_defaultvalue.config.datarate,
    EO_INIT(.init)      eoprot_ep_as_fun_INIT_mais_config__datarate,
    EO_INIT(.update)    eoprot_ep_as_fun_UPDT_mais_config__datarate
};

EOnv_rom_t eoprot_ep_as_rom_mais_descriptor_config__resolution =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_as_rom_mais_defaultvalue.config.resolution),
    EO_INIT(.rwmode)    eoprot_ep_as_mais_rwmode_config__resolution,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_as_rom_mais_defaultvalue.config.resolution,
    EO_INIT(.init)      eoprot_ep_as_fun_INIT_mais_config__resolution,
    EO_INIT(.update)    eoprot_ep_as_fun_UPDT_mais_config__resolution
};

EOnv_rom_t eoprot_ep_as_rom_mais_descriptor_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_as_rom_mais_defaultvalue.status),
    EO_INIT(.rwmode)    eoprot_ep_as_mais_rwmode_status,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_as_rom_mais_defaultvalue.status,
    EO_INIT(.init)      eoprot_ep_as_fun_INIT_mais_status,
    EO_INIT(.update)    eoprot_ep_as_fun_UPDT_mais_status
};

EOnv_rom_t eoprot_ep_as_rom_mais_descriptor_status__the15values =
{   
    EO_INIT(.capacity)  sizeof(eoprot_ep_as_rom_mais_defaultvalue.status.the15values),
    EO_INIT(.rwmode)    eoprot_ep_as_mais_rwmode_status__the15values,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_ep_as_rom_mais_defaultvalue.status.the15values,
    EO_INIT(.init)      eoprot_ep_as_fun_INIT_mais_status__the15values,
    EO_INIT(.update)    eoprot_ep_as_fun_UPDT_mais_status__the15values
};



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


const EOnv_rom_t * const eoprot_ep_as_rom_folded_descriptors[] =
{
    // here are eoprot_ep_as_strain_tags_numberof descriptors for the strains (equal for every strain)
    &eoprot_ep_as_rom_strain_descriptor_config,
    &eoprot_ep_as_rom_strain_descriptor_config__mode,
    &eoprot_ep_as_rom_strain_descriptor_config__datarate,
    &eoprot_ep_as_rom_strain_descriptor_config__signaloncefullscale,
    &eoprot_ep_as_rom_strain_descriptor_status,
    &eoprot_ep_as_rom_strain_descriptor_status__fullscale,
    &eoprot_ep_as_rom_strain_descriptor_status__calibratedvalues,
    &eoprot_ep_as_rom_strain_descriptor_status__uncalibratedvalues,

    // here are eoprot_ep_as_mais_tags_numberof descriptors for the mais (equal for every mais)
    &eoprot_ep_as_rom_mais_descriptor_config,
    &eoprot_ep_as_rom_mais_descriptor_config__mode,
    &eoprot_ep_as_rom_mais_descriptor_config__datarate,
    &eoprot_ep_as_rom_mais_descriptor_config__resolution,
    &eoprot_ep_as_rom_mais_descriptor_status,
    &eoprot_ep_as_rom_mais_descriptor_status__the15values
         
};  EO_VERIFYsizeof(eoprot_ep_as_rom_folded_descriptors, sizeof(EOnv_rom_t*)*(eoprot_ep_as_strain_tags_numberof+eoprot_ep_as_mais_tags_numberof));





const EOconstvector  eoprot_ep_as_rom_constvector_of_folded_descriptors_dat = 
{
    EO_INIT(.size)              sizeof(eoprot_ep_as_rom_folded_descriptors)/sizeof(EOnv_rom_t*), 
    EO_INIT(.item_size)         sizeof(EOnv_rom_t*),
    EO_INIT(.item_array_data)   eoprot_ep_as_rom_folded_descriptors
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eoprot_ep_as_rom_strain_get_offset(eOprotTag_t tag)
{
    return(s_eoprot_ep_as_rom_strain_ramoffset(tag));
}

extern uint16_t eoprot_ep_as_rom_mais_get_offset(eOprotTag_t tag)
{
    return(s_eoprot_ep_as_rom_mais_ramoffset(tag));
}

extern void* eoprot_ep_as_rom_get_nvrom(eOprotID_t id)
{
    uint16_t indexoffoldeddescriptors = s_eoprot_ep_as_rom_epid2index_of_folded_descriptors(id);
    
    if(EOK_uint16dummy == indexoffoldeddescriptors)
    {
        return(NULL);
    }
    
    return((void*)eoprot_ep_as_rom_folded_descriptors[indexoffoldeddescriptors]);   
}

extern uint16_t eoprot_ep_as_rom_get_sizeofvar(eOprotID_t id)
{     
    EOnv_rom_t* rom = eoprot_ep_as_rom_get_nvrom(id);  
    if(NULL == rom)
    {
        return(0);
    }    
    return(rom->capacity); 
}

extern uint16_t eoprot_ep_as_rom_get_prognum(eOprotID_t id)
{   // we assume that the variables are inserted in a progressive way without holes. and even if there are a few holes never mind.
    return(eoprot_ep_variable_ID2tag(eoprot_endpoint_analogsensors, id));
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eoprot_ep_as_rom_epid2index_of_folded_descriptors(eOprotID_t id)
{      
    uint16_t tag = eoprot_ep_variable_ID2tag(eoprot_endpoint_analogsensors, id);
    
    if(EOK_uint16dummy == tag)
    {
        return(EOK_uint16dummy);
    }
    
    eOprotEntity_t entity = eoprot_ep_variable_ID2entity(eoprot_endpoint_analogsensors, id);
    
    switch(entity)
    {
        case eoas_entity_strain:
        {   // nothing to add as the strain vars are in first position
            ; 
        } break;
        
        case eoas_entity_mais:
        {   // must add the number of vars in a strain
            tag += eoprot_ep_as_strain_tags_numberof; 
        } break;      

        default:
        {   // error
            tag = EOK_uint16dummy;
        } break;
    
    }
    
    return(tag);   
}


// returns the offset form the start of the struct eOas_strain_t of the variable with a given tag 
static uint16_t s_eoprot_ep_as_rom_strain_ramoffset(uint16_t tag)
{   
    //return(eoprot_ep_as_rom_strain_offsets[tag]);
    uint32_t tmp = ((uint32_t) eoprot_ep_as_rom_folded_descriptors[tag]->resetval) - ((uint32_t) &eoprot_ep_as_rom_strain_defaultvalue);
    return((uint16_t)tmp); 
}

// returns the offset form the start of the struct eOas_mais_t of the variable with a given tag 
static uint16_t s_eoprot_ep_as_rom_mais_ramoffset(uint16_t tag)
{
    //return(eoprot_ep_as_rom_mais_offsets[tag]);
    uint32_t tmp = ((uint32_t) eoprot_ep_as_rom_folded_descriptors[tag]->resetval) - ((uint32_t) &eoprot_ep_as_rom_mais_defaultvalue);
    return((uint16_t)tmp); 
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



