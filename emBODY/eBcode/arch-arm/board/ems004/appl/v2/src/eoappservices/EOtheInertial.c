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
#include "EoError.h"
#include "EOtheEntities.h"

#include "EOtheCANservice.h"
#include "EOtheCANmapping.h"
#include "EOtheCANprotocol.h"

#include "EoProtocolAS.h"

#include "EOMtheEMSappl.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheInertial.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheInertial_hid.h"


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

static void s_eo_inertial_process_inertial_resolution(eOas_inertialresolution_t resolution, eOas_inertial_status_t* status);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheInertial s_eo_theinertial = 
{
    .initted            = eobool_false,
    .thereisinertial        = eobool_false,
    .protindex          = 0,
    .id32               = eo_prot_ID32dummy,
    .command            = {0}
};

//static const char s_eobj_ownname[] = "EOtheInertial";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheInertial* eo_inertial_Initialise(void)
{
    if(eobool_true == s_eo_theinertial.initted)
    {
        return(&s_eo_theinertial);
    }
    

    if(0 == eo_entities_NumOfMaises(eo_entities_GetHandle()))
    {
        s_eo_theinertial.thereisinertial = eobool_false;
    }
    else
    {
        s_eo_theinertial.thereisinertial = eobool_true;
        s_eo_theinertial.protindex = 0;
        s_eo_theinertial.id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_inertial, s_eo_theinertial.protindex, eoprot_tag_none);
    }

    s_eo_theinertial.initted = eobool_true;
    
    return(&s_eo_theinertial);   
}


extern EOtheInertial* eo_inertial_GetHandle(void)
{
    return(eo_inertial_Initialise());
}


extern eOresult_t eo_inertial_Start(EOtheInertial *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertial.thereisinertial)
    {   // nothing to do because we dont have a inertial board
        return(eores_OK);
    }
    
       
    // now, i do things. 
    eOas_inertial_config_t *cfg = eo_entities_GetMaisConfig(eo_entities_GetHandle(), s_eo_theinertial.protindex);

    if(NULL == cfg)
    {   // we dont have inertial
        return(eores_OK);
    }

//    s_eo_theinertial.datarate = cfg->datarate;      // it must be 10
//    s_eo_theinertial.mode = cfg->mode;              // it must be eoas_inertialmode_txdatacontinuously

    s_eo_theinertial.command.class = eocanprot_msgclass_pollingAnalogSensor;    
    
    // set txmode
    s_eo_theinertial.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    s_eo_theinertial.command.value = &cfg->mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_theinertial.command, s_eo_theinertial.id32);
    
    // set datarate
    s_eo_theinertial.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
    s_eo_theinertial.command.value = &cfg->datarate;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_theinertial.command, s_eo_theinertial.id32);    
    
    return(eores_OK);       
}


extern eOresult_t eo_inertial_Config(EOtheInertial *p, eOas_inertial_config_t* inertialcfg)
{
    if((NULL == p) || (NULL == inertialcfg))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertial.thereisinertial)
    {   // nothing to do because we dont have a inertial board
        return(eores_OK);
    }  

    // ok, now we do something. 
    
    eOresult_t res = eores_OK;
    
    if(eores_OK != (res = eo_inertial_ConfigMode(p, (eOas_inertialmode_t)inertialcfg->mode)))
    {
        return(res);
    }
    
    if(eores_OK != (res = eo_inertial_ConfigDataRate(p, inertialcfg->datarate)))
    {
        return(res);
    }   
    
    if(eores_OK != (res = eo_inertial_ConfigResolution(p, (eOas_inertialresolution_t)inertialcfg->resolution)))
    {
        return(res);
    }
       
    return(eores_OK);     
}


extern eOresult_t eo_inertial_ConfigMode(EOtheInertial *p, eOas_inertialmode_t mode)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertial.thereisinertial)
    {   // nothing to do because we dont have a inertial board
        return(eores_OK);
    }  

    // ok, now we do something.     
 
    eOsmStatesEMSappl_t currentstate = eo_sm_emsappl_STerr;

    s_eo_theinertial.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_theinertial.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    s_eo_theinertial.command.value = &mode;
    
    // if pc104 tell me to enable inertial tx, before to send cmd verify if i'm in RUN state:
    // if yes ==> ok no problem
    // if no ==> i'll send cmd when go to RUN state
    if(eoas_inertialmode_txdatacontinuously == mode)
    {
        // only if the appl is in RUN state enable inertial tx
        eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &currentstate);
        if(eo_sm_emsappl_STrun == currentstate)
        {
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_theinertial.command, s_eo_theinertial.id32);
        }
    }
    else
    {
         eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_theinertial.command, s_eo_theinertial.id32);
    }
    
    return(eores_OK);
}


extern eOresult_t eo_inertial_ConfigDataRate(EOtheInertial *p, uint8_t datarate)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertial.thereisinertial)
    {   // nothing to do because we dont have a inertial board
        return(eores_OK);
    }  

    // ok, now we do something.     
 

    s_eo_theinertial.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_theinertial.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
    s_eo_theinertial.command.value = &datarate;
    
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_theinertial.command, s_eo_theinertial.id32);    
    
    return(eores_OK);  
}

extern eOresult_t eo_inertial_ConfigResolution(EOtheInertial *p, eOas_inertialresolution_t resolution)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertial.thereisinertial)
    {   // nothing to do because we dont have a inertial board
        return(eores_OK);
    }  

    // ok, now we do something.     
 
    s_eo_theinertial.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_theinertial.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION;
    s_eo_theinertial.command.value = &resolution;
    
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_theinertial.command, s_eo_theinertial.id32);    
    
    eOas_inertial_status_t *status = eo_entities_GetMaisStatus(eo_entities_GetHandle(), s_eo_theinertial.protindex);
    if(NULL == status)
    {
        return(eores_NOK_generic); //error
    }     

    s_eo_inertial_process_inertial_resolution(resolution, status);  
    
    return(eores_OK);          
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// -- in here .... so that all things related to Inertial are in a unique place


extern void eoprot_fun_INIT_as_inertial_config(const EOnv* nv)
{
    eOas_inertial_config_t* config = eo_nv_RAM(nv);
    
    config->id          = eoas_inertial_id_hand_palm;
    config->datarate    = 10;
}


extern void eoprot_fun_INIT_as_inertial_status(const EOnv* nv)
{
    eOas_inertial_status_t *status = eo_nv_RAM(nv);  
    
    status->accelerometer.x = status->accelerometer.y = status->accelerometer.y = status->accelerometer.z = status->accelerometer.ffu = 0;
    status->gyroscope.x = status->gyroscope.y = status->gyroscope.y = status->gyroscope.z = status->gyroscope.ffu = 0;
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_inertial_process_inertial_resolution(eOas_inertialresolution_t resolution, eOas_inertial_status_t* status)
{
    uint8_t size = 0;
    uint8_t itemsize = 1;
    uint8_t capacity = 0;
    
    if(eoas_inertialresolution_08 == resolution)
    {
        capacity    = 15;
        itemsize    = 1;
        size        = 15;
        //#warning acemor-> nella inertial nel caso di risoluzione a 8 bit perche' la capacity di the15values e' 16 e non 15?
        // penso sia un errore. metto a 15. anche perche' nel parser dei pacchetti can la size viene messa a 15
        //status->the15values.head.capacity = 16;
        //status->the15values.head.itemsize = 1;
        //status->the15values.head.size = 16;
    }
    else if(eoas_inertialresolution_16 == resolution)
    {
        capacity    = 15;
        itemsize    = 2;
        size        = 15;
        //#warning acemor-> nella inertial ho messo la capacity di the15values a 15 anche nel caso di risoluzione a 16 bit
        //status->the15values.head.capacity = 16;
        //status->the15values.head.itemsize = 2;
        //status->the15values.head.size = 16;
    } 

    EOarray* array = eo_array_New(capacity, itemsize, &status->the15values);
    eo_array_Resize(array, size);      
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



