/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "EoCommon.h"
#include "EOtheErrorManager.h"
#include "EOtheEntities.h"

#include "EOtheCANservice.h"
#include "EOtheCANmapping.h"
#include "EOtheCANprotocol.h"

#include "EOtheEMSapplBody.h"

#include "EoProtocolAS.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMAIS.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMAIS_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheMAIS s_eo_themais = 
{
    .initted            = eobool_false,
    .thereismais        = eobool_false,
    .datarate           = 0,
    .mode               = 0
};

//static const char s_eobj_ownname[] = "EOtheMAIS";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheMAIS* eo_mais_Initialise(void)
{
    if(eobool_true == s_eo_themais.initted)
    {
        return(&s_eo_themais);
    }
    

    if(0 == eo_entities_NumOfMaises(eo_entities_GetHandle()))
    {
        s_eo_themais.thereismais = eobool_false;
    }
    else
    {
        s_eo_themais.thereismais = eobool_true;
    }

    s_eo_themais.initted = eobool_true;
    
    return(&s_eo_themais);   
}


extern EOtheMAIS* eo_mais_GetHandle(void)
{
    return(eo_mais_Initialise());
}


extern eOresult_t eo_mais_Start(EOtheMAIS *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themais.thereismais)
    {   // nothing to do because we dont have a mais board
        return(eores_OK);
    }
    
       
    // now, i do things. 
    
    const uint8_t number = 0; 
    eOas_mais_config_t *cfg = eo_entities_GetMaisConfig(eo_entities_GetHandle(), number);

    if(NULL == cfg)
    {   // we dont have mais
        return(eores_OK);
    }

    s_eo_themais.datarate = cfg->datarate;       // it must be 10
    s_eo_themais.mode = cfg->mode;            // it must be eoas_maismode_txdatacontinuously
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_mais, number, eoprot_tag_none);

    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingAnalogSensor;    
    
    // set txmode
    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    command.value = &s_eo_themais.mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
    
    // set datarate
    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
    command.value = &s_eo_themais.datarate;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);    
    
    return(eores_OK);       
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// -- in here .... so that all things related to MAIS are in a unique place


extern void eoprot_fun_INIT_as_mais_config(const EOnv* nv)
{
    eOas_mais_config_t* maiscfg = eo_nv_RAM(nv);
    
    maiscfg->datarate = 10;
    maiscfg->mode = eoas_maismode_txdatacontinuously;
    maiscfg->resolution = eoas_maisresolution_08;
}


extern void eoprot_fun_INIT_as_mais_status(const EOnv* nv)
{
    eOas_mais_status_t *status = eo_nv_RAM(nv);  
    
    // marco.accame: i init as for eoas_maisresolution_08 
    //               the array the15values can be initted for size 0 or 15 as i now use teh proper eo_array_Assign() method
    uint8_t capacity    = 15;
    uint8_t itemsize    = 1;
    uint8_t size        = 15;
    EOarray* array = eo_array_New(capacity, itemsize, &status->the15values);
    eo_array_Resize(array, size);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



