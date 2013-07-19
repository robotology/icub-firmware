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
static uint16_t s_eoprot_as_rom_entity_offset_of_tag(const void* entityrom, eOprotTag_t tag);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// - default value of a strain
static const eOas_strain_t eoprot_as_rom_strain_defaultvalue = { 0 };

// - default value of a mais
static const eOas_mais_t eoprot_as_rom_mais_defaultvalue = { 0 };

// - default value of a extorque
static const eOas_mais_t eoprot_as_rom_extorque_defaultvalue = { 0 };


// - descriptors for the variables of a strain

static EOnv_rom_t eoprot_as_rom_descriptor_strain_wholeitem =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_wholeitem,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_wholeitem
};

static EOnv_rom_t eoprot_as_rom_descriptor_strain_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue.config),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue.config,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_config,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_config
};

static EOnv_rom_t eoprot_as_rom_descriptor_strain_config_mode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue.config.mode),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_config_mode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue.config.mode,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_config_mode,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_config_mode
};

static EOnv_rom_t eoprot_as_rom_descriptor_strain_config_datarate =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue.config.datarate),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_config_datarate,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue.config.datarate,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_config_datarate,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_config_datarate
};

static EOnv_rom_t eoprot_as_rom_descriptor_strain_config_signaloncefullscale =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue.config.signaloncefullscale),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_config_signaloncefullscale,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue.config.signaloncefullscale,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_config_signaloncefullscale,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_config_signaloncefullscale
};

static EOnv_rom_t eoprot_as_rom_descriptor_strain_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue.status),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_status,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue.status,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_status,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_status
};

static EOnv_rom_t eoprot_as_rom_descriptor_strain_status_fullscale =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue.status.fullscale),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_status_fullscale,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue.status.fullscale,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_status_fullscale,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_status_fullscale
};

static EOnv_rom_t eoprot_as_rom_descriptor_strain_status_calibratedvalues =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue.status.calibratedvalues),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_status_calibratedvalues,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue.status.calibratedvalues,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_status_calibratedvalues,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_status_calibratedvalues
};

static EOnv_rom_t eoprot_as_rom_descriptor_strain_status_uncalibratedvalues =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_strain_defaultvalue.status.uncalibratedvalues),
    EO_INIT(.rwmode)    eoprot_rwm_as_strain_status_uncalibratedvalues,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_strain_defaultvalue.status.uncalibratedvalues,
    EO_INIT(.init)      eoprot_fun_INIT_as_strain_status_uncalibratedvalues,
    EO_INIT(.update)    eoprot_fun_UPDT_as_strain_status_uncalibratedvalues
};

// - descriptors for the variables of a mais

static EOnv_rom_t eoprot_as_rom_descriptor_mais_wholeitem =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_mais_defaultvalue),
    EO_INIT(.rwmode)    eoprot_rwm_as_mais_wholeitem,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_mais_defaultvalue,
    EO_INIT(.init)      eoprot_fun_INIT_as_mais_wholeitem,
    EO_INIT(.update)    eoprot_fun_UPDT_as_mais_wholeitem
};

static EOnv_rom_t eoprot_as_rom_descriptor_mais_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_mais_defaultvalue.config),
    EO_INIT(.rwmode)    eoprot_rwm_as_mais_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_mais_defaultvalue.config,
    EO_INIT(.init)      eoprot_fun_INIT_as_mais_config,
    EO_INIT(.update)    eoprot_fun_UPDT_as_mais_config
};

static EOnv_rom_t eoprot_as_rom_descriptor_mais_config_mode =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_mais_defaultvalue.config.mode),
    EO_INIT(.rwmode)    eoprot_rwm_as_mais_config_mode,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_mais_defaultvalue.config.mode,
    EO_INIT(.init)      eoprot_fun_INIT_as_mais_config_mode,
    EO_INIT(.update)    eoprot_fun_UPDT_as_mais_config_mode
};

static EOnv_rom_t eoprot_as_rom_descriptor_mais_config_datarate =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_mais_defaultvalue.config.datarate),
    EO_INIT(.rwmode)    eoprot_rwm_as_mais_config_datarate,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_mais_defaultvalue.config.datarate,
    EO_INIT(.init)      eoprot_fun_INIT_as_mais_config_datarate,
    EO_INIT(.update)    eoprot_fun_UPDT_as_mais_config_datarate
};

static EOnv_rom_t eoprot_as_rom_descriptor_mais_config_resolution =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_mais_defaultvalue.config.resolution),
    EO_INIT(.rwmode)    eoprot_rwm_as_mais_config_resolution,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_mais_defaultvalue.config.resolution,
    EO_INIT(.init)      eoprot_fun_INIT_as_mais_config_resolution,
    EO_INIT(.update)    eoprot_fun_UPDT_as_mais_config_resolution
};

static EOnv_rom_t eoprot_as_rom_descriptor_mais_status =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_mais_defaultvalue.status),
    EO_INIT(.rwmode)    eoprot_rwm_as_mais_status,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_mais_defaultvalue.status,
    EO_INIT(.init)      eoprot_fun_INIT_as_mais_status,
    EO_INIT(.update)    eoprot_fun_UPDT_as_mais_status
};

static EOnv_rom_t eoprot_as_rom_descriptor_mais_status_the15values =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_mais_defaultvalue.status.the15values),
    EO_INIT(.rwmode)    eoprot_rwm_as_mais_status_the15values,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_mais_defaultvalue.status.the15values,
    EO_INIT(.init)      eoprot_fun_INIT_as_mais_status_the15values,
    EO_INIT(.update)    eoprot_fun_UPDT_as_mais_status_the15values
};

// - descriptors for the variables of an extorque

static EOnv_rom_t eoprot_as_rom_descriptor_extorque_wholeitem =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_extorque_defaultvalue),
    EO_INIT(.rwmode)    eoprot_rwm_as_extorque_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_extorque_defaultvalue,
    EO_INIT(.init)      eoprot_fun_INIT_as_extorque_wholeitem,
    EO_INIT(.update)    eoprot_fun_UPDT_as_extorque_wholeitem
};

static EOnv_rom_t eoprot_as_rom_descriptor_extorque_config =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_extorque_defaultvalue.config),
    EO_INIT(.rwmode)    eoprot_rwm_as_extorque_config,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_extorque_defaultvalue.config,
    EO_INIT(.init)      eoprot_fun_INIT_as_extorque_config,
    EO_INIT(.update)    eoprot_fun_UPDT_as_extorque_config
};

static EOnv_rom_t eoprot_as_rom_descriptor_extorque_inputs =
{   
    EO_INIT(.capacity)  sizeof(eoprot_as_rom_extorque_defaultvalue.inputs),
    EO_INIT(.rwmode)    eoprot_rwm_as_extorque_inputs,
    EO_INIT(.dummy)     0,    
    EO_INIT(.resetval)  (const void*)&eoprot_as_rom_extorque_defaultvalue.inputs,
    EO_INIT(.init)      eoprot_fun_INIT_as_extorque_inputs,
    EO_INIT(.update)    eoprot_fun_UPDT_as_extorque_inputs
};


// -- the folded array of descriptors

static const EOnv_rom_t * const eoprot_as_rom_folded_descriptors[] =
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
    &eoprot_as_rom_descriptor_mais_status_the15values,
    
    // here are eoprot_tags_as_extorque_numberof descriptors for the extorque (equal for every extorque)
    &eoprot_as_rom_descriptor_extorque_wholeitem,
    &eoprot_as_rom_descriptor_extorque_config,    
    &eoprot_as_rom_descriptor_extorque_inputs
         
};  EO_VERIFYsizeof(eoprot_as_rom_folded_descriptors, sizeof(EOnv_rom_t*)*(eoprot_tags_as_strain_numberof+eoprot_tags_as_mais_numberof+eoprot_tags_as_extorque_numberof));




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


const EOconstvector  eoprot_as_rom_constvector_of_folded_descriptors_dat = 
{
    EO_INIT(.size)              sizeof(eoprot_as_rom_folded_descriptors)/sizeof(EOnv_rom_t*), 
    EO_INIT(.item_size)         sizeof(EOnv_rom_t*),
    EO_INIT(.item_array_data)   eoprot_as_rom_folded_descriptors
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eoprot_as_rom_get_offset(eOprotEntity_t entity, eOprotTag_t tag)
{
    const void* startofrom = NULL;
    
    switch(entity)
    {
        case eoas_entity_strain:
        {   
            startofrom = &eoprot_as_rom_strain_defaultvalue; 
        } break;
        
        case eoas_entity_mais:
        {   
            startofrom = &eoprot_as_rom_mais_defaultvalue;
        } break;  

        case eoas_entity_extorque:
        {   
            startofrom = &eoprot_as_rom_extorque_defaultvalue; 
        } break;          

        default:
        {   
            return(EOK_uint16dummy);
        } //break;   
    }    
    
    return(s_eoprot_as_rom_entity_offset_of_tag(startofrom, tag));

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
    uint16_t ret = eoprot_ID2tag(id);
    
    // dont check validity of the tag. we could check inside the case xxxx: by verifying if ret is higher than 
    // the max number of tags for that entity.
       
    eOprotEntity_t entity = eoprot_ID2entity(id);
    
    switch(entity)
    {
        case eoas_entity_strain:
        {   // nothing to add as the strain vars are in first position
            ; 
        } break;
        
        case eoas_entity_mais:
        {   // must add the number of vars in a strain
            ret += eoprot_tags_as_strain_numberof; 
        } break;  

        case eoas_entity_extorque:
        {   // must add the number of vars in a strain + the number of vars in a mais
            ret += eoprot_tags_as_strain_numberof; 
            ret += eoprot_tags_as_mais_numberof; 
        } break;          

        default:
        {   // error
            ret = EOK_uint16dummy;
        } break;
    
    }
    
    return(ret);   
}

// returns the offset of the variable with a given tag from the start of the entity
static uint16_t s_eoprot_as_rom_entity_offset_of_tag(const void* entityrom, eOprotTag_t tag)
{
    uint32_t tmp = ((uint32_t) eoprot_as_rom_folded_descriptors[tag]->resetval) - ((uint32_t) entityrom);
    return((uint16_t)tmp); 
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



