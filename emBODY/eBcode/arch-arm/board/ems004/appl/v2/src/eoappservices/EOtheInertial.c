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

static void s_eo_inertial_configcanmessage(icubCanProto_inertial_config_t *canprotoconfig, eOas_inertial_enableflags_t enableflags);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const eOas_inertial_config_t s_eo_default_inertialconfig =
{
    .id         = eoas_inertial_id_hand_palm,
    .datarate   = 10,
    .filler     = {0}
};


static EOtheInertial s_eo_theinertial = 
{
    .initted            = eobool_false,
    .thereisinertial    = eobool_false,
    .protindex          = 0,
    .id                 = eoas_inertial_id_hand_palm,
    .acceltype          = accel_intext,
    .gyrotype           = gyro_ext,
    .istransmitting     = eobool_false,
    .supportedsensors   = icubCanProto_inertial_sensorflag_internaldigitalaccelerometer | 
                          icubCanProto_inertial_sensorflag_externaldigitalgyroscope | 
                          icubCanProto_inertial_sensorflag_externaldigitalaccelerometer,
    .id32               = eo_prot_ID32dummy,
    .config             = {0},
    .command            = {0}
};



//static const char s_eobj_ownname[] = "EOtheInertial";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

// so far a board can manage only one kind of inertial identifier ... 
// that is to say: the ems eb2 of the hand can use only the inertial of the hand_palm but not any other
//                 the ems of the leg-skin can handle only the inertial of the foot_palm but not any other.

extern EOtheInertial* eo_inertial_Initialise(eOas_inertialidentifier_t id)
{
    if(eobool_true == s_eo_theinertial.initted)
    {
        return(&s_eo_theinertial);
    }
    

    if((eoas_inertial_id_none == id) || (0 == eo_entities_NumOfInertials(eo_entities_GetHandle())))
    {
        s_eo_theinertial.thereisinertial = eobool_false;
    }
    else
    {
        s_eo_theinertial.thereisinertial = eobool_true;
        s_eo_theinertial.protindex = 0;
        
        s_eo_theinertial.id = id;
        if((eoas_inertial_id_hand_palm == id) || (eoas_inertial_id_foot_palm == id))
        {
            s_eo_theinertial.gyrotype           = gyro_ext;
            s_eo_theinertial.acceltype          = accel_intext;
            s_eo_theinertial.supportedsensors   = icubCanProto_inertial_sensorflag_internaldigitalaccelerometer | 
                                                  icubCanProto_inertial_sensorflag_externaldigitalgyroscope | 
                                                  icubCanProto_inertial_sensorflag_externaldigitalaccelerometer;
        }
        else
        {   // any other future inertial sensor, for instance a eoas_inertial_id_arm_wrist or eoas_inertial_id_arm_elbow
            s_eo_theinertial.gyrotype           = gyro_none;
            s_eo_theinertial.acceltype          = accel_int;
            s_eo_theinertial.supportedsensors   = icubCanProto_inertial_sensorflag_internaldigitalaccelerometer;
        }
        
        s_eo_theinertial.istransmitting = eobool_false;
        
        s_eo_theinertial.id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_inertial, s_eo_theinertial.protindex, eoprot_tag_none);
        memcpy(&s_eo_theinertial.config, &s_eo_default_inertialconfig, sizeof(eOas_inertial_config_t));
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


extern eOresult_t eo_inertial_Config(EOtheInertial *p, eOas_inertial_config_t* inertialcfg)
{
    if((NULL == p) || (NULL == inertialcfg))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertial.thereisinertial)
    {   // nothing to do because we dont have a inertial board
        // send up diagnostics
        // error -> "Object EOtheInertial does not not support any inertial, thus will NOT process any command."         
        return(eores_OK);
    }  

    // ok, now we do something. 
    
    
    // the mtb does not have a can message which just configures the inertial sensor. it only has a start message with some parameters.
    // thus in here we just copy the config into the local copy of the object. we dont prepare the canframe
    
    s_eo_theinertial.config.id          = inertialcfg->id;
    s_eo_theinertial.config.datarate    = inertialcfg->datarate;
    
    // however we do some sanity checks
    
    if(s_eo_theinertial.id != s_eo_theinertial.config.id)
    {
        s_eo_theinertial.config.id = s_eo_theinertial.id;
        // and also change the configuration of the inertial ...
        eOas_inertial_config_t *cfg = eo_entities_GetInertialConfig(eo_entities_GetHandle(), s_eo_theinertial.protindex);
        cfg->id = s_eo_theinertial.id;
        
        // send up diagnostics about the fact that we support a different inertial ...
        // error -> "Object EOtheInertial does not not support the requested inertial. The supported will be used instead. In param16 msb there is the supported one and in lsb the requested"             
    }
    
    if((0 == s_eo_theinertial.config.datarate) || (s_eo_theinertial.config.datarate > 10))
    {
        s_eo_theinertial.config.datarate = 10;
        // and also change the configuration of the inertial ...
        eOas_inertial_config_t *cfg = eo_entities_GetInertialConfig(eo_entities_GetHandle(), s_eo_theinertial.protindex);
        cfg->datarate = 10;
        
        // send up diagnostics about the fact that we support a different datarate ...
        // error -> "Object EOtheInertial does not not support the requested datarate. The supported will be used instead. in param16 msb there is the supported one and in lsb the requested"                     
    }

    
    return(eores_OK);     
}


extern eOresult_t eo_inertial_Start(EOtheInertial *p, eOas_inertial_enableflags_t enableflags)
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

    // ok, now we do something.    

    if(eobool_true == s_eo_theinertial.istransmitting)
    {   // if transmitting we stop before
        eo_inertial_Stop(p);
    }       
 
    icubCanProto_inertial_config_t canprotoconfig = {0};

    
    // -- config the mode
    
    s_eo_inertial_configcanmessage(&canprotoconfig, enableflags);


    // -- send command 
    //    .... even if we have a canprotoconfig.enabledsensors equal to zero 
    //    .... in such a case we send twice the command over can (once with eo_inertial_Stop() and another time in here) 
    
    s_eo_theinertial.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_theinertial.command.type  = ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP;
    s_eo_theinertial.command.value = &canprotoconfig;
    
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_theinertial.command, s_eo_theinertial.id32);  


    if(0 == canprotoconfig.enabledsensors)
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

    // ok, now we do something.    

    // if it is not transmitting we dont send a useless command on can.
    
    if(eobool_false == s_eo_theinertial.istransmitting)
    {
         return(eores_OK);
    }

    icubCanProto_inertial_config_t canprotoconfig = {0};
    
    canprotoconfig.enabledsensors   = icubCanProto_inertial_sensorflag_none;
    canprotoconfig.period           = s_eo_theinertial.config.datarate;

    s_eo_theinertial.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_theinertial.command.type  = ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP;
    s_eo_theinertial.command.value = &canprotoconfig;
    
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_theinertial.command, s_eo_theinertial.id32);    
    
    
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

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// -- in here .... so that all things related to Inertial are in a unique place


extern void eoprot_fun_INIT_as_inertial_config(const EOnv* nv)
{
    eOas_inertial_config_t* config = eo_nv_RAM(nv);
    
    config->id          = s_eo_default_inertialconfig.id;
    config->datarate    = s_eo_default_inertialconfig.datarate;
}


extern void eoprot_fun_INIT_as_inertial_status(const EOnv* nv)
{
    eOas_inertial_status_t *status = eo_nv_RAM(nv);  
    
    status->accelerometer.x = status->accelerometer.y = status->accelerometer.y = status->accelerometer.z = 0;
    status->accelerometer.ffu = 0;
    
    status->gyroscope.x = status->gyroscope.y = status->gyroscope.y = status->gyroscope.z = 0;
    status->gyroscope.ffu = 0;
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_inertial_configcanmessage(icubCanProto_inertial_config_t *canprotoconfig, eOas_inertial_enableflags_t enableflags)
{
    
    // -- config the period
    
    canprotoconfig->period           = s_eo_theinertial.config.datarate;   


    // config the enabled sensors
    
    canprotoconfig->enabledsensors = 0;
    
    // enableflags is: now, accel only, gyro only or both.
    // let us see
    switch(enableflags)
    {
        case eoas_inertial_enable_none: 
        {
            canprotoconfig->enabledsensors = 0;
            
        } break;

        case eoas_inertial_enable_accelerometer: 
        {
            canprotoconfig->enabledsensors = 0;
            
            // must see if accel is internal or external.
            if((s_eo_theinertial.supportedsensors & icubCanProto_inertial_sensorflag_externaldigitalaccelerometer) == icubCanProto_inertial_sensorflag_externaldigitalaccelerometer)
            {   // we have the external
                canprotoconfig->enabledsensors |= icubCanProto_inertial_sensorflag_externaldigitalaccelerometer;
            }
            else if((s_eo_theinertial.supportedsensors & icubCanProto_inertial_sensorflag_internaldigitalaccelerometer) == icubCanProto_inertial_sensorflag_internaldigitalaccelerometer)
            {   // we have the internal
                canprotoconfig->enabledsensors |= icubCanProto_inertial_sensorflag_internaldigitalaccelerometer;
            }
            else
            {
                // send up diagnostics
                // error -> "Object EOtheInertial supports an inertial which does not have this mode. In param16 ..."  
            }
            
        } break;
        
        case eoas_inertial_enable_gyroscope: 
        {
            canprotoconfig->enabledsensors = 0;
            
            // must see if gyro is external.
            if((s_eo_theinertial.supportedsensors & icubCanProto_inertial_sensorflag_externaldigitalgyroscope) == icubCanProto_inertial_sensorflag_externaldigitalgyroscope)
            {   // we have the external
                canprotoconfig->enabledsensors |= icubCanProto_inertial_sensorflag_externaldigitalgyroscope;
            }
            else
            {
                // send up diagnostics
                // error -> "Object EOtheInertial supports an inertial which does not have this mode. In param16 ..."  
            }
            
        } break;  
        
        case eoas_inertial_enable_accelgyro: 
        {
            canprotoconfig->enabledsensors = 0;
            
            // must see if accel is internal or external.
            if((s_eo_theinertial.supportedsensors & icubCanProto_inertial_sensorflag_externaldigitalaccelerometer) == icubCanProto_inertial_sensorflag_externaldigitalaccelerometer)
            {   // we have the external
                canprotoconfig->enabledsensors |= icubCanProto_inertial_sensorflag_externaldigitalaccelerometer;
            }
            else if((s_eo_theinertial.supportedsensors & icubCanProto_inertial_sensorflag_internaldigitalaccelerometer) == icubCanProto_inertial_sensorflag_internaldigitalaccelerometer)
            {   // we have the internal
                canprotoconfig->enabledsensors |= icubCanProto_inertial_sensorflag_internaldigitalaccelerometer;
            }
            else
            {
                // send up diagnostics
                // error -> "Object EOtheInertial supports an inertial which does not have this mode. In param16 ..."  
            }
            
            // must see if gyro is external.
            if((s_eo_theinertial.supportedsensors & icubCanProto_inertial_sensorflag_externaldigitalgyroscope) == icubCanProto_inertial_sensorflag_externaldigitalgyroscope)
            {   // we have the external
                canprotoconfig->enabledsensors |= icubCanProto_inertial_sensorflag_externaldigitalgyroscope;
            }
            else
            {
                // send up diagnostics
                // error -> "Object EOtheInertial supports an inertial which does not have this mode. In param16 ..."  
            }
            
        } break;     

        default: 
        {
            canprotoconfig->enabledsensors = 0;
            // send up diagnostics
            // error -> "Object EOtheInertial does not recognise this mode. In param16 ..."  
        } break;
        
    }
    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



