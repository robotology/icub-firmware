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

/* @file       EoProtocolAS_rom.c
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

#include "EoProtocolAS.h"

#include "EoAnalogSensors.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolAS_rom.h"


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

static uint16_t s_eoprot_as_rom_epid2index_of_folded_descriptors(eOprotID32_t id);

static uint16_t s_eoprot_as_rom_strain_ramoffset(uint16_t tag);
static uint16_t s_eoprot_as_rom_mais_ramoffset(uint16_t tag);




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// - default value of a strain

const eOas_strain_t eoprot_as_rom_strain_defaultvalue = { 0 };


// - default value of a mais

const eOas_mais_t eoprot_as_rom_mais_defaultvalue = { 0 };



// - descriptors for the variables of a strain

EOnv_rom_t eoprot_as_rom_descriptor_strain_wholeitem =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_wholeitem,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_wholeitem
};

EOnv_rom_t eoprot_as_rom_descriptor_strain_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue.config),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue.config,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_config,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_config
};

EOnv_rom_t eoprot_as_rom_descriptor_strain_config_mode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue.config.mode),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_config_mode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue.config.mode,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_config_mode,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_config_mode
};

EOnv_rom_t eoprot_as_rom_descriptor_strain_config_datarate =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue.config.datarate),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_config_datarate,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue.config.datarate,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_config_datarate,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_config_datarate
};

EOnv_rom_t eoprot_as_rom_descriptor_strain_config_signaloncefullscale =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue.config.signaloncefullscale),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_config_signaloncefullscale,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue.config.signaloncefullscale,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_config_signaloncefullscale,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_config_signaloncefullscale
};

EOnv_rom_t eoprot_as_rom_descriptor_strain_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue.status),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_status,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue.status,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_status,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_status
};

EOnv_rom_t eoprot_as_rom_descriptor_strain_status_fullscale =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue.status.fullscale),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_status_fullscale,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue.status.fullscale,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_status_fullscale,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_status_fullscale
};

EOnv_rom_t eoprot_as_rom_descriptor_strain_status_calibratedvalues =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue.status.calibratedvalues),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_status_calibratedvalues,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue.status.calibratedvalues,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_status_calibratedvalues,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_status_calibratedvalues
};

EOnv_rom_t eoprot_as_rom_descriptor_strain_status_uncalibratedvalues =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue.status.uncalibratedvalues),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_status_uncalibratedvalues,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue.status.uncalibratedvalues,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_status_uncalibratedvalues,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_status_uncalibratedvalues
};

// - descriptors for the variables of a mais

EOnv_rom_t eoprot_as_rom_descriptor_mais_wholeitem =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_mais_defaultvalue),
    EO_INIT(.rwmode)    eoprot_rwm_as_mais_wholeitem,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_mais_defaultvalue,
    EO_INIT(.init)      eoprot_fun_INIT_as_mais_wholeitem,
    EO_INIT(.update)    eoprot_fun_UPDT_as_mais_wholeitem
};

EOnv_rom_t eoprot_as_rom_descriptor_mais_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_mais_defaultvalue.config),
    EO_INIT(.rwmode)    eoprot_rwm_as_mais_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_mais_defaultvalue.config,
    EO_INIT(.init)      eoprot_fun_INIT_as_mais_config,
    EO_INIT(.update)    eoprot_fun_UPDT_as_mais_config
};

EOnv_rom_t eoprot_as_rom_descriptor_mais_config_mode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_mais_defaultvalue.config.mode),
    EO_INIT(.rwmode)    eoprot_rwm_as_mais_config_mode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_mais_defaultvalue.config.mode,
    EO_INIT(.init)      eoprot_fun_INIT_as_mais_config_mode,
    EO_INIT(.update)    eoprot_fun_UPDT_as_mais_config_mode
};

EOnv_rom_t eoprot_as_rom_descriptor_mais_config_datarate =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_mais_defaultvalue.config.datarate),
    EO_INIT(.rwmode)    eoprot_rwm_as_mais_config_datarate,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_mais_defaultvalue.config.datarate,
    EO_INIT(.init)      eoprot_fun_INIT_as_mais_config_datarate,
    EO_INIT(.update)    eoprot_fun_UPDT_as_mais_config_datarate
};

EOnv_rom_t eoprot_as_rom_descriptor_mais_config_resolution =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_mais_defaultvalue.config.resolution),
    EO_INIT(.rwmode)    eoprot_rwm_as_mais_config_resolution,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_mais_defaultvalue.config.resolution,
    EO_INIT(.init)      eoprot_fun_INIT_as_mais_config_resolution,
    EO_INIT(.update)    eoprot_fun_UPDT_as_mais_config_resolution
};

EOnv_rom_t eoprot_as_rom_descriptor_mais_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_mais_defaultvalue.status),
    EO_INIT(.rwmode)    eoprot_rwm_as_mais_status,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_mais_defaultvalue.status,
    EO_INIT(.init)      eoprot_fun_INIT_as_mais_status,
    EO_INIT(.update)    eoprot_fun_UPDT_as_mais_status
};

EOnv_rom_t eoprot_as_rom_descriptor_mais_status_the15values =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_mais_defaultvalue.status.the15values),
    EO_INIT(.rwmode)    eoprot_rwm_as_mais_status_the15values,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_mais_defaultvalue.status.the15values,
    EO_INIT(.init)      eoprot_fun_INIT_as_mais_status_the15values,
    EO_INIT(.update)    eoprot_fun_UPDT_as_mais_status_the15values
};



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


const EOnv_rom_t * const eoprot_as_rom_folded_descriptors[] =
{
    // here are eoprot_tags_as_strain_numberof descriptors for the strains (equal for every strain)
    &eoprot_as_rom_descriptor_strain_wholeitem,
    &eoprot_as_rom_descriptor_strain_config,
    &eoprot_as_rom_descriptor_strain_config_mode,
    &eoprot_as_rom_descriptor_strain_config_datarate,
    &eoprot_as_rom_descriptor_strain_config_signaloncefullscale,
    &eoprot_as_rom_descriptor_strain_status,
    &eoprot_as_rom_descriptor_strain_status_fullscale,
    &eoprot_as_rom_descriptor_strain_status_calibratedvalues,
    &eoprot_as_rom_descriptor_strain_status_uncalibratedvalues,

    // here are eoprot_tags_as_mais_numberof descriptors for the mais (equal for every mais)
    &eoprot_as_rom_descriptor_mais_wholeitem,
    &eoprot_as_rom_descriptor_mais_config,
    &eoprot_as_rom_descriptor_mais_config_mode,
    &eoprot_as_rom_descriptor_mais_config_datarate,
    &eoprot_as_rom_descriptor_mais_config_resolution,
    &eoprot_as_rom_descriptor_mais_status,
    &eoprot_as_rom_descriptor_mais_status_the15values
         
};  EO_VERIFYsizeof(eoprot_as_rom_folded_descriptors, sizeof(EOnv_rom_t*)*(eoprot_tags_as_strain_numberof+eoprot_tags_as_mais_numberof));





const EOconstvector  eoprot_as_rom_constvector_of_folded_descriptors_dat = 
{
    EO_INIT(.size)              sizeof(eoprot_as_rom_folded_descriptors)/sizeof(EOnv_rom_t*), 
    EO_INIT(.item_size)         sizeof(EOnv_rom_t*),
    EO_INIT(.item_array_data)   eoprot_as_rom_folded_descriptors
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eoprot_as_rom_strain_get_offset(eOprotTag_t tag)
{
    return(s_eoprot_as_rom_strain_ramoffset(tag));
}

extern uint16_t eoprot_as_rom_mais_get_offset(eOprotTag_t tag)
{
    return(s_eoprot_as_rom_mais_ramoffset(tag));
}

extern void* eoprot_as_rom_get_nvrom(eOprotID32_t id)
{
    uint16_t indexoffoldeddescriptors = s_eoprot_as_rom_epid2index_of_folded_descriptors(id);
    
    if(EOK_uint16dummy == indexoffoldeddescriptors)
    {
        return(NULL);
    }
    
    return((void*)eoprot_as_rom_folded_descriptors[indexoffoldeddescriptors]);   
}

extern uint16_t eoprot_as_rom_get_sizeofvar(eOprotID32_t id)
{     
    EOnv_rom_t* rom = eoprot_as_rom_get_nvrom(id);  
    if(NULL == rom)
    {
        return(0);
    }    
    return(rom->capacity); 
}

extern uint16_t eoprot_as_rom_get_prognum(eOprotID32_t id)
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

static uint16_t s_eoprot_as_rom_epid2index_of_folded_descriptors(eOprotID32_t id)
{      
    uint16_t tag = eoprot_ID2tag(id);
    
    if(EOK_uint16dummy == tag)
    {
        return(EOK_uint16dummy);
    }
    
    eOprotEntity_t entity = eoprot_ID2entity(id);
    
    switch(entity)
    {
        case eoas_entity_strain:
        {   // nothing to add as the strain vars are in first position
            ; 
        } break;
        
        case eoas_entity_mais:
        {   // must add the number of vars in a strain
            tag += eoprot_tags_as_strain_numberof; 
        } break;      

        default:
        {   // error
            tag = EOK_uint16dummy;
        } break;
    
    }
    
    return(tag);   
}


// returns the offset form the start of the struct eOas_strain_t of the variable with a given tag 
static uint16_t s_eoprot_as_rom_strain_ramoffset(uint16_t tag)
{   
    //return(eoprot_as_rom_strain_offsets[tag]);
    uint32_t tmp = ((uint32_t) eoprot_as_rom_folded_descriptors[tag]->resetval) - ((uint32_t) &eoprot_as_rom_strain_defaultvalue);
    return((uint16_t)tmp); 
}

// returns the offset form the start of the struct eOas_mais_t of the variable with a given tag 
static uint16_t s_eoprot_as_rom_mais_ramoffset(uint16_t tag)
{
    //return(eoprot_as_rom_mais_offsets[tag]);
    uint32_t tmp = ((uint32_t) eoprot_as_rom_folded_descriptors[tag]->resetval) - ((uint32_t) &eoprot_as_rom_mais_defaultvalue);
    return((uint16_t)tmp); 
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



