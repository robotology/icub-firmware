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

#include "EoProtocolAS.h"

#include "EOMtheEMSappl.h"

#include "EOtheCANprotocol_functions.h"

#include "EOMtheEMSconfigurator.h"

#include "EOMtheEMStransceiver.h"

#include "EOemsController.h"

#include "EOVtheCallbackManager.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheEncoderReader.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheEncoderReader_hid.h"


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

static eOresult_t s_eo_encoderreader_onstop_verifyreading(void *par, eObool_t readingisok);


static eo_appEncReader_enc_type_t s_eo_encoderreader_GetEncoderType(uint8_t sensortype);

static eo_appEncReader_encoder_position_t s_eo_encoderreader_GetEncoderPosition(uint8_t sensorport);

static void s_eo_encoderreader_config_ereader(const eOmn_serv_arrayof_4jomodescriptors_t * jomodes, eOcallback_t callback, void* arg);

static void readencoder(void* p);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheEncoderReader s_eo_theencoderreader = 
{
    .initted                = eobool_false,
    .active                 = eobool_false,
    .arrayofjomodes         = { 0 },
    .reader                 = NULL,
    //.readerconfig           = {0},    
    .onverify               = NULL,
    .activateafterverify    = eobool_false,
    .waitreadtimer          = NULL,
    .numofjomos             = 0
};

//static const char s_eobj_ownname[] = "EOtheEncoderReader";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheEncoderReader* eo_encoderreader_Initialise(void)
{
    if(eobool_true == s_eo_theencoderreader.initted)
    {
        return(&s_eo_theencoderreader);
    }
    
    s_eo_theencoderreader.active = eobool_false;



    s_eo_theencoderreader.waitreadtimer = eo_timer_New();
    
    s_eo_theencoderreader.reader = NULL; // in future use: Initialise() 
        
    s_eo_theencoderreader.initted = eobool_true;
    
    return(&s_eo_theencoderreader);   
}


extern EOtheEncoderReader* eo_encoderreader_GetHandle(void)
{
    if(eobool_true == s_eo_theencoderreader.initted)
    {
        return(&s_eo_theencoderreader);
    }
    
    return(NULL);
}


extern eOresult_t eo_encoderreader_Verify(EOtheEncoderReader *p, const eOmn_serv_arrayof_4jomodescriptors_t * jomodes, eOencoderreader_onendofoperation_fun_t onverify, eObool_t activateafterverify)
{
    if((NULL == p) || (NULL == jomodes))
    {
        return(eores_NOK_nullpointer);
    }  
 
// DONT Deactivate ... we may want just to check again ....    
//    if(eobool_true == s_eo_theencoderreader.active)
//    {
//        eo_encoderreader_Deactivate(p);        
//    }   
    

    s_eo_theencoderreader.onverify = onverify;
    s_eo_theencoderreader.activateafterverify = activateafterverify;
                    
    s_eo_encoderreader_config_ereader(jomodes, NULL, NULL);
    
    eo_appEncReader_StartRead(s_eo_theencoderreader.reader);
    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    
    // now i start a timer of 2 ms. and at its expiry i exec s_eo_encoderreader_read_after.    
    eo_action_SetCallback(act, readencoder, (void*)jomodes, eov_callbackman_GetTask(eov_callbackman_GetHandle()));     
    eo_timer_Start(s_eo_theencoderreader.waitreadtimer, eok_abstimeNOW, 2*eok_reltime1sec, eo_tmrmode_ONESHOT, act);   
     
//    eObool_t readingisok = eobool_true;
//    s_eo_encoderreader_onstop_verifyreading((void*)jomodes, readingisok);
//    
    return(eores_OK);   
}


extern eOresult_t eo_encoderreader_Deactivate(EOtheEncoderReader *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == s_eo_theencoderreader.active)
    {
        return(eores_OK);        
    } 
    
    
    
    
//    // send stop messages to strain, unload the entity-can-mapping and the board-can-mapping, reset all things inside this object
//    // what else .... ?
//    
//        
//    if(eobool_true == s_eo_theencoderreader.itistransmitting)
//    {
//        eo_encoderreader_TXstop(&s_eo_theencoderreader);
//    }
//    
//    eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_strain, s_eo_theencoderreader.canentitydescriptor); 
//    
//    eo_canmap_UnloadBoards(eo_canmap_GetHandle(), s_eo_theencoderreader.canboardproperties);
//     
//    
//    eo_entities_SetNumOfStrains(eo_entities_GetHandle(), 0);
//    
//    s_eo_theencoderreader.strain = NULL;
//    
//    memset(&s_eo_theencoderreader.arrayofjomodes, 0, sizeof(eOmn_serv_arrayof_4jomodescriptors_t));
//    s_eo_theencoderreader.servconfig.type = eomn_serv_NONE;
//    
//    eo_vector_Clear(s_eo_theencoderreader.canboardproperties);
//    eo_vector_Clear(s_eo_theencoderreader.canentitydescriptor);
//    
//    
//    s_eo_theencoderreader.active = eobool_false;
    
    return(eores_OK);
}


extern eOresult_t eo_encoderreader_Activate(EOtheEncoderReader *p, const eOmn_serv_arrayof_4jomodescriptors_t * jomodes)
{
    if((NULL == p) || (NULL == jomodes))
    {
        return(eores_NOK_nullpointer);
    }    
    
    if(eobool_true == s_eo_theencoderreader.active)
    {
        eo_encoderreader_Deactivate(p);        
    }   
 
             
//    memcpy(&s_eo_theencoderreader.arrayofjomodes, jomodes, sizeof(eOmn_serv_arrayof_4jomodescriptors_t));
//                    
//    s_eo_encoderreader_config_ereader(NULL, NULL);
//    
//
    eo_appEncReader_StartRead(s_eo_theencoderreader.reader);
                     
    s_eo_theencoderreader.active = eobool_true;        
    
    return(eores_OK);   
}



extern eOresult_t eo_encoderreader_StartReading(EOtheEncoderReader *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theencoderreader.active)
    {   // nothing to do because we dont have a strain board
        return(eores_OK);
    }   

    eo_appEncReader_StartRead(s_eo_theencoderreader.reader);
    
    
    return(eores_OK);
}

extern eObool_t eo_encoderreader_IsReadingAvailable(EOtheEncoderReader *p)
{
    if(NULL == p)
    {
        return(eobool_false);
    }
    
    if(eobool_false == s_eo_theencoderreader.active)
    {   // nothing to do because we dont have a strain board
        return(eobool_false);
    }   

    eObool_t res = eo_appEncReader_isReady(s_eo_theencoderreader.reader);

    return(res);
}



extern eOresult_t eo_encoderreader_Read(EOtheEncoderReader *p, uint8_t position, uint32_t *primary, uint32_t *secondary, hal_encoder_errors_flags *errors)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theencoderreader.active)
    {   // nothing to do because we dont have a strain board
        return(eores_OK);
    }   

    if(position > eo_appEncReader_joint_position5)
    {
        return(eores_NOK_generic);
    }

    eOresult_t res = eo_appEncReader_GetJointValue(s_eo_theencoderreader.reader, (eo_appEncReader_joint_position_t)position, primary, secondary, errors); 
    
    
    return(res);
}



//extern eOresult_t eo_encoderreader_OnDiscoveryStop(EOtheEncoderReader *p, EOtheCANdiscovery2 *discovery2, eObool_t searchisok)
//{   
//    if(NULL == p)
//    {
//        return(eores_NOK_nullpointer);
//    }
//    
//    // wip part. we should: 
//    // 1. send up a ok result about the strain ...
//    // 2. call eo_encoderreader_Initialise()
//    // 3. return OK
//    
//    
//    if(NULL == p)
//    {
//        eo_encoderreader_Initialise();
//    }

//    return(eores_OK); 
//}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// - in here i put the functions used to initialise the values in ram of the joints and motors ... better in here rather than elsewhere.



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static eOresult_t s_eo_encoderreader_onstop_verifyreading(void *par, eObool_t readingisok)
{
    
    if((eobool_true == readingisok) && (eobool_true == s_eo_theencoderreader.activateafterverify))
    {
        const eOmn_serv_arrayof_4jomodescriptors_t * jomodes = (const eOmn_serv_arrayof_4jomodescriptors_t *)par;
        eo_encoderreader_Activate(&s_eo_theencoderreader, jomodes);        
    }
    
    if(NULL != s_eo_theencoderreader.onverify)
    {
        s_eo_theencoderreader.onverify(&s_eo_theencoderreader, readingisok); 
    }    
    
    return(eores_OK);   
}


static eo_appEncReader_enc_type_t s_eo_encoderreader_GetEncoderType(uint8_t sensortype)
{
    eo_appEncReader_enc_type_t type = eo_appEncReader_enc_type_NONE;
    
    switch(sensortype)
    {
        case eomn_serv_mc_sensor_encoder_aea:   type = eo_appEncReader_enc_type_AEA;    break;
        case eomn_serv_mc_sensor_encoder_amo:   type = eo_appEncReader_enc_type_AMO;    break;
        case eomn_serv_mc_sensor_encoder_inc:   type = eo_appEncReader_enc_type_INC;    break;
        case eomn_serv_mc_sensor_hall:          type = eo_appEncReader_enc_type_ADH;    break;
        case eomn_serv_mc_sensor_none:          type = eo_appEncReader_enc_type_NONE;   break;
        default:                                type = eo_appEncReader_enc_type_NONE;   break;        
    }
    
    return(type);    
}


static eo_appEncReader_encoder_position_t s_eo_encoderreader_GetEncoderPosition(uint8_t sensorport)
{
    eo_appEncReader_encoder_position_t pos = eo_appEncReader_encoder_positionNONE;
    if(sensorport <= eo_appEncReader_encoder_position5)
    {
        pos = (eo_appEncReader_encoder_position_t)sensorport;
    }
    
    return(pos);    
}



static void s_eo_encoderreader_config_ereader(const eOmn_serv_arrayof_4jomodescriptors_t * jomodes, eOcallback_t callback, void* arg)
{
    
    if(NULL != s_eo_theencoderreader.reader)
    {
        return;
    }
    
    
    memcpy(&s_eo_theencoderreader.arrayofjomodes, jomodes, sizeof(eOmn_serv_arrayof_4jomodescriptors_t));
    
    // now... use the servcfg
    uint8_t i = 0;
    
    EOconstarray* carray = eo_constarray_Load((EOarray*)&s_eo_theencoderreader.arrayofjomodes);

    uint8_t numofjomos = eo_constarray_Size(carray);
    s_eo_theencoderreader.numofjomos = numofjomos;

        
    eOappEncReader_cfg_t config;
    memset(&config, 0, sizeof(config));
    
    config.SPI_callbackOnLastRead = callback;
    config.SPI_callback_arg = arg;
    
    for(i=0; i<eOappEncReader_joint_numberof; i++)
    {
        const eOmn_serv_jomo_descriptor_t *jomodes = (eOmn_serv_jomo_descriptor_t*) eo_constarray_At(carray, i);
        
        if(NULL == jomodes)
        {   // i am beyond the number of jomos
            config.joints[i].primary_encoder = config.joints[i].extra_encoder = eo_appEncReader_enc_type_NONE;
            config.joints[i].primary_enc_position = config.joints[i].extra_enc_position = eo_appEncReader_encoder_positionNONE;
        }
        else 
        {
            // must convert things 
            config.joints[i].primary_encoder = s_eo_encoderreader_GetEncoderType(jomodes->sensor.type);
            config.joints[i].primary_enc_position = s_eo_encoderreader_GetEncoderPosition(jomodes->sensor.port);
            config.joints[i].extra_encoder = s_eo_encoderreader_GetEncoderType(jomodes->extrasensor.type);
            config.joints[i].extra_enc_position = s_eo_encoderreader_GetEncoderPosition(jomodes->extrasensor.port);        

            if(eo_appEncReader_enc_type_NONE != config.joints[i].primary_encoder)
            {
                config.SPI_streams[config.joints[i].primary_enc_position%2].numberof++;
                config.SPI_streams[config.joints[i].primary_enc_position%2].type = hal_encoder_t1;
            }

            if(eo_appEncReader_enc_type_NONE != config.joints[i].extra_encoder)
            {
                config.SPI_streams[config.joints[i].extra_enc_position%2].numberof++;
                config.SPI_streams[config.joints[i].extra_enc_position%2].type = hal_encoder_t1;
            } 
        }        
    }
    
    s_eo_theencoderreader.reader = eo_appEncReader_New(&config);
    
}


static void readencoder(void* p)
{
    eObool_t readingisok = eobool_true;
    
    uint8_t i = 0;
    
    for(i=0; i< s_eo_theencoderreader.numofjomos; i++)
    {
        uint32_t primary = 0;
        uint32_t secondary = 0;
        hal_encoder_errors_flags errors = {0};
        eo_appEncReader_GetJointValue(s_eo_theencoderreader.reader, (eo_appEncReader_joint_position_t)i, &primary, &secondary, &errors);
        if((0 != errors.chip_error) || (0 != errors.data_error) || (0 != errors.data_notready) || (0 != errors.tx_error))
        {
            readingisok = eobool_false;
        }            
    }
    
    
    s_eo_encoderreader_onstop_verifyreading((void*)p, readingisok);   
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



