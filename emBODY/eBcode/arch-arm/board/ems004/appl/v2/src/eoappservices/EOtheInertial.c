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

#include "EOVtheSystem.h"

#include "hal_trace.h"

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

// marco.accame: we can later on retrieve the canmapofsupportedsensors[][] from a config message coming from robotInterface.
//               now, for short we dont do it and we store the configurations inside here.

const eOas_inertial_serviceconfig_t eo_inertial_cfg_eb2 = 
{
    .canmapofsupportedsensors =
    {
        {   // can1   
            eoas_inertial_pos_none,                 // adr 00 
            eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, 
            eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none       
        },
        {   // can2
            eoas_inertial_pos_none,                 // adr 00 
            eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none,
            eoas_inertial_pos_l_upper_arm_4,        // adr 08
            eoas_inertial_pos_l_upper_arm_1,        // adr 09
            eoas_inertial_pos_l_upper_arm_3,        // adr 10
            eoas_inertial_pos_l_upper_arm_2,        // adr 11           
            eoas_inertial_pos_l_forearm_1,          // adr 12
            eoas_inertial_pos_l_forearm_2,          // adr 13
            eoas_inertial_pos_l_hand                // adr 14        
        }  
    }    
};

const eOas_inertial_serviceconfig_t eo_inertial_cfg_eb4 = 
{
    .canmapofsupportedsensors =
    {
        {   // can1  
            eoas_inertial_pos_none,                 // adr 00 
            eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, 
            eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none       
        },
        {   // can2
            eoas_inertial_pos_none,                 // adr 00 
            eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none,
            eoas_inertial_pos_r_upper_arm_4,        // adr 08
            eoas_inertial_pos_r_upper_arm_1,        // adr 09
            eoas_inertial_pos_r_upper_arm_3,        // adr 10
            eoas_inertial_pos_r_upper_arm_2,        // adr 11           
            eoas_inertial_pos_r_forearm_1,          // adr 12
            eoas_inertial_pos_r_forearm_2,          // adr 13
            eoas_inertial_pos_r_hand                // adr 14        
        }  
    }   
};


const eOas_inertial_serviceconfig_t eo_inertial_cfg_eb10 = 
{
    .canmapofsupportedsensors =
    {
        {   // can1  
            eoas_inertial_pos_none,                 // adr 00 
            eoas_inertial_pos_l_upper_leg_1,        // adr 01
            eoas_inertial_pos_l_upper_leg_2,        // adr 02
            eoas_inertial_pos_l_upper_leg_3,        // adr 03
            eoas_inertial_pos_l_upper_leg_4,        // adr 04 
            eoas_inertial_pos_l_upper_leg_5,        // adr 05 
            eoas_inertial_pos_l_upper_leg_6,        // adr 06 
            eoas_inertial_pos_l_upper_leg_7,        // adr 07 
            eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none       
        },
        {   // can2
            eoas_inertial_pos_none,                 // adr 00 
            eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none,
            eoas_inertial_pos_l_lower_leg_1,        // adr 08
            eoas_inertial_pos_l_lower_leg_2,        // adr 09
            eoas_inertial_pos_l_lower_leg_3,        // adr 10
            eoas_inertial_pos_l_lower_leg_4,        // adr 11
            eoas_inertial_pos_l_foot_2,             // adr 12
            eoas_inertial_pos_l_foot_1,             // adr 13   
            eoas_inertial_pos_none                  // adr 14
        }  
    }       
};


const eOas_inertial_serviceconfig_t eo_inertial_cfg_eb11 = 
{
    .canmapofsupportedsensors =
    {
        {   // can1  
            eoas_inertial_pos_none,                 // adr 00 
            eoas_inertial_pos_r_upper_leg_1,        // adr 01
            eoas_inertial_pos_r_upper_leg_2,        // adr 02
            eoas_inertial_pos_r_upper_leg_3,        // adr 03
            eoas_inertial_pos_r_upper_leg_4,        // adr 04 
            eoas_inertial_pos_r_upper_leg_5,        // adr 05 
            eoas_inertial_pos_r_upper_leg_6,        // adr 06 
            eoas_inertial_pos_r_upper_leg_7,        // adr 07 
            eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none       
        },
        {   // can2
            eoas_inertial_pos_none,                 // adr 00 
            eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none, eoas_inertial_pos_none,
            eoas_inertial_pos_r_lower_leg_1,        // adr 08
            eoas_inertial_pos_r_lower_leg_2,        // adr 09
            eoas_inertial_pos_r_lower_leg_3,        // adr 10
            eoas_inertial_pos_r_lower_leg_4,        // adr 11
            eoas_inertial_pos_r_foot_2,             // adr 12
            eoas_inertial_pos_r_foot_1,             // adr 13   
            eoas_inertial_pos_none                  // adr 14
        }  
    } 
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_inertial_build_maps(const eOas_inertial_serviceconfig_t* cfg);

static eObool_t s_eo_inertial_get_canlocation(eOas_inertial_position_t targetposition, eOcanmap_location_t *loc);

static eObool_t s_eo_inertial_get_sensor(eOcanmap_location_t location, eOas_inertial_position_t *position);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const eOas_inertial_sensorsconfig_t s_eo_default_inertialsensorsconfig =
{
    .enabled    = 0,
    .datarate   = 50,
    .filler     = {0}
};



static EOtheInertial s_eo_theinertial = 
{
    .initted                = eobool_false,
    .configured             = eobool_false,
    .thereisinertial        = eobool_false,
    .protindex              = 0,
    .istransmitting         = eobool_false,
    .sensorsconfig          = {0},
    .command                = {0},
    .serviceconfig          = {0},
    .supportedmask64        = 0,
    .inertial2              = NULL,
    .fifo                   = NULL
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
    

    if(0 == eo_entities_NumOfInertials(eo_entities_GetHandle()))
    {
        s_eo_theinertial.thereisinertial = eobool_false;
    }
    else
    {
        s_eo_theinertial.thereisinertial = eobool_true;
        s_eo_theinertial.protindex = 0;
        
        s_eo_theinertial.istransmitting = eobool_false;
               
        memcpy(&s_eo_theinertial.sensorsconfig, &s_eo_default_inertialsensorsconfig, sizeof(eOas_inertial_sensorsconfig_t));
        s_eo_theinertial.inertial2 = eo_entities_GetInertial(eo_entities_GetHandle(), s_eo_theinertial.protindex);       
        s_eo_theinertial.fifo = eo_vector_New(sizeof(eOas_inertial_data_t), 32, NULL, 0, NULL, NULL);
    }
    

    s_eo_theinertial.initted = eobool_true;
    s_eo_theinertial.configured = eobool_false;
    
    return(&s_eo_theinertial);   
}


extern eOresult_t eo_inertial_ServiceConfig(EOtheInertial *p, const eOas_inertial_serviceconfig_t* cfg)
{    
    // we can configure the service how many times we want. however the object must be initted and must there be an inertial entity
 
    if((NULL == p) || (NULL == cfg))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertial.initted)
    {
        return(eores_NOK_generic);
    }
    
    if(eobool_false == s_eo_theinertial.thereisinertial)
    {
        return(eores_NOK_generic);
    }
    
    if(eobool_true == s_eo_theinertial.istransmitting)
    {
        eo_inertial_Stop(p);
    }
    
    // ok, now we can build the maps.
    s_eo_inertial_build_maps(cfg);
    
    // and set it configured
    s_eo_theinertial.configured = eobool_false;
    
    return(eores_OK);  
}



extern EOtheInertial* eo_inertial_Initialise2(const eOas_inertial_serviceconfig_t* cfg)
{
    if(eobool_true == s_eo_theinertial.initted)
    {
        return(&s_eo_theinertial);
    }
    

    if((NULL == cfg) || (0 == eo_entities_NumOfInertials(eo_entities_GetHandle())))
    {
        s_eo_theinertial.thereisinertial = eobool_false;
    }
    else
    {
        s_eo_theinertial.thereisinertial = eobool_true;
        s_eo_theinertial.protindex = 0;
        
        s_eo_theinertial.istransmitting = eobool_false;
        
        
        memcpy(&s_eo_theinertial.sensorsconfig, &s_eo_default_inertialsensorsconfig, sizeof(eOas_inertial_sensorsconfig_t));

        s_eo_theinertial.inertial2 = eo_entities_GetInertial(eo_entities_GetHandle(), s_eo_theinertial.protindex);        
        s_eo_theinertial.fifo = eo_vector_New(sizeof(eOas_inertial_data_t), 32, NULL, 0, NULL, NULL);        
        
        s_eo_inertial_build_maps(cfg);
        s_eo_theinertial.configured = eobool_true;
    }
    

    s_eo_theinertial.initted = eobool_true;
    
    return(&s_eo_theinertial);   
}



extern EOtheInertial* eo_inertial_GetHandle(void)
{
    if(eobool_true == s_eo_theinertial.initted)
    {
        return(&s_eo_theinertial);
    }
    
    return(NULL);
}



extern eOresult_t eo_inertial_SensorsConfig(EOtheInertial *p, eOas_inertial_sensorsconfig_t* config)
{
    if((NULL == p) || (NULL == config))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertial.thereisinertial)
    {   // nothing to do because we dont have an inertial board
        // send up diagnostics
        // error -> "Object EOtheInertial does not not support any inertial, thus will NOT process any command."         
        return(eores_OK);
    }  
    
    if(eobool_false == s_eo_theinertial.configured)
    {
        // nothing to do because we dont have a configured object et
        // send up diagnostics
        // error -> "Object EOtheInertial is not yet configured, thus will NOT process any command."   
        return(eores_OK);
    }

    // ok, now we do something. 
    
    // at first we copy the target config into the local config
    
    s_eo_theinertial.sensorsconfig.datarate   = config->datarate;
    s_eo_theinertial.sensorsconfig.enabled    = config->enabled;
    
    // then we check enabled mask and datarate
    
    // the enabled must be in bitwise AND with the supported ones
    s_eo_theinertial.sensorsconfig.enabled    &= s_eo_theinertial.supportedmask64;
    if(s_eo_theinertial.sensorsconfig.enabled != config->enabled)
    {
        // send up diagnostics
        // warning -> "Object EOtheInertial does not not support some of the inertials."               
    }
    
    if(0 == s_eo_theinertial.sensorsconfig.enabled)
    {
        // send up diagnostics
        // warning -> "Object EOtheInertial does not have a valid configuration."            
    }

    
    if(s_eo_theinertial.sensorsconfig.datarate < 10)
    {
        s_eo_theinertial.sensorsconfig.datarate = 10;
        // send up diagnostics
        // warning -> "Object EOtheInertial has changed the requested datarate."         
    }
    
    if(s_eo_theinertial.sensorsconfig.datarate > 200)
    {
        s_eo_theinertial.sensorsconfig.datarate = 200;
        // send up diagnostics
        // warning -> "Object EOtheInertial has changed the requested datarate."             
    }
    
 
    // ok guys: now we transmit the configuration to each one of the sensors which have a non-zero bit inside teh s_eo_theinertial.config2.enabled mask.
    // WAIT A MINUTE: we do it inside eo_inertial_Start() !!!!!!!!!!!!

    
    return(eores_OK);           
}



extern eOresult_t eo_inertial_Start(EOtheInertial *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertial.thereisinertial)
    {   // nothing to do because we dont have an inertial board
        // send up diagnostics
        // error -> "Object EOtheInertial does not not support any inertial, thus will NOT process any command."          
        return(eores_OK);
    } 

    if(eobool_false == s_eo_theinertial.configured)
    {
        // nothing to do because we dont have a configured object et
        // send up diagnostics
        // error -> "Object EOtheInertial is not yet configured, thus will NOT process any command."   
        return(eores_OK);
    }    

    // ok, now we do something.    

    if(eobool_true == s_eo_theinertial.istransmitting)
    {   // if transmitting we stop before
        eo_inertial_Stop(p);
    }       
    
    
    if(0 == s_eo_theinertial.sensorsconfig.enabled)
    {
        // send up diagnostics
        // error -> "Object EOtheInertial does not contain a valid configuration, thus will NOT activate any sensor."         
    } 

    
    eObool_t sentconfig = eobool_false;
 
    icubCanProto_inertial_config_t canprotoconfig = {0};
    
    canprotoconfig.period           = s_eo_theinertial.sensorsconfig.datarate;
    canprotoconfig.enabledsensors   = icubCanProto_inertial_sensorflag_internaldigitalaccelerometer; // unless it is a hand board ....
    
    s_eo_theinertial.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_theinertial.command.type  = ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP;
    s_eo_theinertial.command.value = &canprotoconfig;

    uint8_t i = 0;
    const uint8_t maxsensors = 8*sizeof(s_eo_theinertial.sensorsconfig.enabled);
    eOcanmap_location_t location = {0};
    
    for(i=0; i<maxsensors; i++)
    {

        if(1 == EOAS_ISPOSENABLED(s_eo_theinertial.sensorsconfig.enabled, i))
        {
            
            eOas_inertial_position_t position = (eOas_inertial_position_t)i;
            
            if((eoas_inertial_pos_l_hand == position) || (eoas_inertial_pos_r_hand == position))
            {
                canprotoconfig.enabledsensors = icubCanProto_inertial_sensorflag_externaldigitalgyroscope | icubCanProto_inertial_sensorflag_externaldigitalaccelerometer;
            }
            else
            {
                canprotoconfig.enabledsensors = icubCanProto_inertial_sensorflag_internaldigitalaccelerometer;
            }

            // fill location and send 
            if(eobool_true == s_eo_inertial_get_canlocation(position, &location))
            {
                sentconfig = eobool_true;
                eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &s_eo_theinertial.command, location);  
            }                           
            
        }      
        
    }


    if(eobool_false == sentconfig)
    {
        s_eo_theinertial.istransmitting = eobool_false;
    }
    else
    {
        s_eo_theinertial.istransmitting = eobool_true;
    }
    
    // send up diagnostics
    // debug -> "Object EOtheInertial has programmed the mtb. In param16 in msb there is the mode, in lsb there is datarate"  
    
    return(eores_OK);  
    
}



extern eOresult_t eo_inertial_Stop(EOtheInertial *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertial.thereisinertial)
    {   // nothing to do because we dont have an inertial board
        
        // send up diagnostics
        // error -> "Object EOtheInertial does not not support any inertial, thus will NOT process any command."
        return(eores_OK);
    }  
    
    if(eobool_false == s_eo_theinertial.configured)
    {
        // nothing to do because we dont have a configured object et
        // send up diagnostics
        // error -> "Object EOtheInertial is not yet configured, thus will NOT process any command."   
        return(eores_OK);
    }    

    // ok, now we do something.    

    // if it is not transmitting we dont send a useless command on can.
    
    if(eobool_false == s_eo_theinertial.istransmitting)
    {
         return(eores_OK);
    }

    icubCanProto_inertial_config_t canprotoconfig = {0};
    
    canprotoconfig.enabledsensors   = icubCanProto_inertial_sensorflag_none;
    canprotoconfig.period           = s_eo_theinertial.sensorsconfig.datarate;

    s_eo_theinertial.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_theinertial.command.type  = ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP;
    s_eo_theinertial.command.value = &canprotoconfig;
    
    
    uint8_t i = 0;
    const uint8_t maxsensors = 8*sizeof(s_eo_theinertial.sensorsconfig.enabled);
    eOcanmap_location_t location = {0};
    
    for(i=0; i<maxsensors; i++)
    {
        if(1 == EOAS_ISPOSENABLED(s_eo_theinertial.sensorsconfig.enabled, i))
        {            
            eOas_inertial_position_t position = (eOas_inertial_position_t)i;            

            // fill location and send 
            if(eobool_true == s_eo_inertial_get_canlocation(position, &location))
            {
                eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &s_eo_theinertial.command, location);  
            }                                       
        }             
    }    
           
    
    s_eo_theinertial.istransmitting = eobool_false;
 
    // send up diagnostics
    // debug -> "Object EOtheInertial has programmed the mtb. In param16 in msb there is the mode, in lsb there is datarate"
    
    return(eores_OK);  
    
}

extern eObool_t eo_inertial_IsTransmitting(EOtheInertial *p)
{
    if(NULL == p)
    {
        return(eobool_false);
    }
    
    if(eobool_false == s_eo_theinertial.thereisinertial)
    {   // nothing to do because we dont have an inertial board
        return(eobool_false);
    }  

    return(s_eo_theinertial.istransmitting);
}    


extern eOresult_t eo_inertial_Receive(EOtheInertial *p, eOas_inertial_type_t type, eOcanframe_t *frame, eOcanport_t port)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertial.thereisinertial)
    {   // nothing to do because we dont have an inertial board
        
        // send up diagnostics
        // error -> "Object EOtheInertial does not not support any inertial, thus will NOT process any command."
        return(eores_OK);
    }  

    if(eobool_false == s_eo_theinertial.configured)
    {
        // nothing to do because we dont have a configured object yet
        // send up diagnostics
        // error -> "Object EOtheInertial is not yet configured, thus will NOT process any command."   
        return(eores_OK);
    }
    
    
    // teh inertial entity is in ...
    s_eo_theinertial.inertial2 = s_eo_theinertial.inertial2; 
    
    // however, we dont use it ...
    // we put what we have inside a fifo of eOas_inertial_data_t
    eOas_inertial_data_t data = {0};
    
   
    
    eOcanmap_location_t loc = {0};    
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
    loc.insideindex = eocanmap_insideindex_none;
    
    eOas_inertial_position_t position = eoas_inertial_pos_none;

    if(eobool_false == s_eo_inertial_get_sensor(loc, &position))
    {
        // send up diagnostics
        // error -> "Object EOtheInertial has received a message from an unsupported inertial"
        return(eores_OK);        
    }
    
    if(eobool_true == eo_vector_Full(s_eo_theinertial.fifo))
    {
        // send up diagnostics
        // error -> "Object EOtheInertial has lost some data"

//        char str[128] = {0};        
//        eOas_inertial_data_t * item = (eOas_inertial_data_t*) eo_vector_Front(s_eo_theinertial.fifo);
//        int ms = (int)(item->timestamp/1000);
//        snprintf(str, sizeof(str), "\n overflow. removing sensor type = %d @ t = %d ms", item->type, ms);
//        hal_trace_puts(str);        
        
        // dont want to lose new data ... i could just call a pop front to remove oldest data
        //eo_vector_PopFront(s_eo_theinertial.fifo);  
        // BUT .. as i dont want to througth data away, i decide that i put it inside the status.
        // in short: i call eo_inertial_RefreshStatusOfEntity() which already does that        
        eo_inertial_RefreshStatusOfEntity(p);      
    }
    
    data.timestamp =  eov_sys_LifeTimeGet(eov_sys_GetHandle());
    data.type = type;
    data.position = position;
    data.x = (int16_t)((frame->data[1]<<8) + frame->data[0]);
    data.y = (int16_t)((frame->data[3]<<8) + frame->data[2]);
    data.z = (int16_t)((frame->data[5]<<8) + frame->data[4]);
    
    // ok, now we can pushback data into the fifo
    
    eo_vector_PushBack(s_eo_theinertial.fifo, &data);

//    char str[128] = {0};    
//    int mms = (int)(data.timestamp/1000);
//    snprintf(str, sizeof(str), "\n added sensor type = %d @ t = %d ms", data.type, mms);
//    hal_trace_puts(str);  
    
    return(eores_OK);      
}


extern eOresult_t eo_inertial_RefreshStatusOfEntity(EOtheInertial *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertial.thereisinertial)
    {   // nothing to do because we dont have an inertial board
        
        // send up diagnostics
        // error -> "Object EOtheInertial does not not support any inertial, thus will NOT process any command."
        return(eores_OK);
    }  
    
    if(eobool_false == s_eo_theinertial.configured)
    {
        // nothing to do because we dont have a configured object yet
        // send up diagnostics
        // error -> "Object EOtheInertial is not yet configured, thus will NOT process any command."   
        return(eores_OK);
    }    

    // the inertial entity is in ...
    eOas_inertial_data_t *data = &s_eo_theinertial.inertial2->status.data;
    memset(data, 0, sizeof(eOas_inertial_data_t)); 
    
    if(eobool_true == eo_vector_Empty(s_eo_theinertial.fifo))
    {
        // just reset the status->data 
        data->type = eoas_inertial_type_none;
        data->position = eoas_inertial_pos_none;
        data->timestamp = eov_sys_LifeTimeGet(eov_sys_GetHandle());           
    }
    else
    {
        // else ... retrieve the item from fifo, copy it into status->data and remove it from fifo 
        eOas_inertial_data_t * item = (eOas_inertial_data_t*) eo_vector_Front(s_eo_theinertial.fifo);
        if(NULL != item)
        {
            memcpy(data, item, sizeof(eOas_inertial_data_t));   
            eo_vector_PopFront(s_eo_theinertial.fifo);   
            
//            char str[128] = {0};
//            int ms = (int)(data->timestamp/1000);
//            int now = eov_sys_LifeTimeGet(eov_sys_GetHandle()) /  1000;
//            snprintf(str, sizeof(str), "\nstatus has: sensor type = %d @ t = %d ms. time now is %d", data->type, ms, now);
//            hal_trace_puts(str);
        }
    }
    
    return(eores_OK); 
       
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// -- in here .... so that all things related to Inertial are in a unique place


extern void eoprot_fun_INIT_as_inertial_config(const EOnv* nv)
{
    eOas_inertial_config_t* config = eo_nv_RAM(nv);
    
    memset(&config->service, 0, sizeof(eOas_inertial_serviceconfig_t));
    
    config->sensors.enabled     = s_eo_default_inertialsensorsconfig.enabled;
    config->sensors.datarate    = s_eo_default_inertialsensorsconfig.datarate;
}


extern void eoprot_fun_INIT_as_inertial_status(const EOnv* nv)
{
    eOas_inertial_status_t *status = eo_nv_RAM(nv);  
    
    status->data.type = eoas_inertial_type_none;
    status->data.position = eoas_inertial_pos_none;
    status->data.timestamp = 0;
    status->data.x = status->data.y = status->data.z = 0;
}

extern eObool_t eocanprotINperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_INERTIAL_MSG(eOcanframe_t *frame, eOcanport_t port)
{    
    if(NULL == frame)
    {
        return(eobool_true);
    }    
    
    // marco.accame:        
    // i dont want to put data in the fifo when we are not in the control loop.

    eOsmStatesEMSappl_t applstate = eo_sm_emsappl_STcfg;
    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &applstate);   
    if(eo_sm_emsappl_STrun != applstate)
    {
        return(eobool_true);
    }
    
    return(eobool_false);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eo_inertial_build_maps(const eOas_inertial_serviceconfig_t* cfg)
{
    // copy the new service configuration
    memcpy(&s_eo_theinertial.serviceconfig, cfg, sizeof(eOas_inertial_serviceconfig_t));
    
    // at first i reset the supported mask
    s_eo_theinertial.supportedmask64 = 0;
        
    // then i search for in can1 and can2 which sensors we are asked to support
    uint8_t i = 0;
    uint8_t j = 0;
    
    for(i=0; i<2; i++)
    {
        for(j=0; j<15; j++)
        {
            eOas_inertial_position_t position = (eOas_inertial_position_t)s_eo_theinertial.serviceconfig.canmapofsupportedsensors[i][j];
            if(eoas_inertial_pos_none != position)
            {
                // ok, we add this position to tthe supported mask
                s_eo_theinertial.supportedmask64 |= EOAS_ENABLEPOS(position);                
            }
        }
    }
       
}


static eObool_t s_eo_inertial_get_canlocation(eOas_inertial_position_t targetposition, eOcanmap_location_t *loc)
{   
    // this function is not optimised in search because it is seldom called. 
    // we call it only in configuration of the sensors to search for the address to where to send teh can message 
    
    if(eoas_inertial_pos_none == targetposition)
    {
        return(eobool_false);
    }
            
    uint8_t i = 0;
    uint8_t j = 0;
    
    for(i=0; i<2; i++)
    {
        for(j=0; j<15; j++)
        {
            eOas_inertial_position_t position = (eOas_inertial_position_t)s_eo_theinertial.serviceconfig.canmapofsupportedsensors[i][j];
            if(targetposition == position)
            {
                loc->port = i;
                loc->addr = j;
                loc->insideindex = eocanmap_insideindex_none;
                
                return(eobool_true);
            }
        }
    }        
    
    return(eobool_false);
}



static eObool_t s_eo_inertial_get_sensor(eOcanmap_location_t location, eOas_inertial_position_t *position)
{       
    eOas_inertial_position_t internalposition = (eOas_inertial_position_t)s_eo_theinertial.serviceconfig.canmapofsupportedsensors[location.port][location.addr];
    
    if(eoas_inertial_pos_none == internalposition)
    {
        return(eobool_false);
    }

    *position = internalposition;   
    return(eobool_true);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



