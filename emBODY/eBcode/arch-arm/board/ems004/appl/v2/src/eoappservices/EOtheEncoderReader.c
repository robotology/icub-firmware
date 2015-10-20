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

static void s_eo_encodereader_send_periodic_error_report(void *p);

static eo_appEncReader_enc_type_t s_eo_encoderreader_GetEncoderType(uint8_t sensortype);

static eo_appEncReader_encoder_position_t s_eo_encoderreader_GetEncoderPosition(uint8_t sensorport);

static void s_eo_encoderreader_config_ereader(const eOmn_serv_arrayof_4jomodescriptors_t * jomodes, eOcallback_t callback, void* arg);

static void s_eo_encoderreader_read_encoders(void* p);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheEncoderReader s_eo_theencoderreader = 
{
    .initted                    = eobool_false,
    .active                     = eobool_false,
    .arrayofjomodes             = { 0 },
    .reader                     = NULL,   
    .onverify                   = NULL,
    .activateafterverify        = eobool_false,
    .waitreadtimer              = NULL,
    .numofjomos                 = 0,
    .numofencoders              = 0,
    .errorflags                 = {0},
    .errorReportTimer           = NULL,
    .errorDescriptor            = {0},
    .errorType                  = eo_errortype_info,
    .errorCallbackCount         = 0,
    .repetitionOKcase           = 10,
    .reportPeriod               = 10*EOK_reltime1sec 
};

static const char s_eobj_ownname[] = "EOtheEncoderReader";


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
    
    s_eo_theencoderreader.errorReportTimer = eo_timer_New();
        
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
    
    // make sure the timer is not running
    eo_timer_Stop(s_eo_theencoderreader.errorReportTimer);   
    
    
    s_eo_theencoderreader.onverify = onverify;
    s_eo_theencoderreader.activateafterverify = activateafterverify;
                    
    s_eo_encoderreader_config_ereader(jomodes, NULL, NULL);
    
    eo_appEncReader_StartRead(s_eo_theencoderreader.reader);
    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    
    // now i start a timer of 2 ms. and at its expiry i exec s_eo_encoderreader_read_encoders().    
    eo_action_SetCallback(act, s_eo_encoderreader_read_encoders, (void*)jomodes, eov_callbackman_GetTask(eov_callbackman_GetHandle()));     
    eo_timer_Start(s_eo_theencoderreader.waitreadtimer, eok_abstimeNOW, 2*eok_reltime1ms, eo_tmrmode_ONESHOT, act);   
     
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
    
    #warning TODO: eo_encoderreader_Deactivate() must be terminated. see comments below
    
    // in order to do this function we should change EOappEncReader so that it can be de-initted. 
    
    // to do: 
    // deinit EOappEncReader, undo what in s_eo_encoderreader_config_ereader()

//    
//    s_eo_theencoderreader.active = eobool_false;


    // make sure the timer is not running
    eo_timer_Stop(s_eo_theencoderreader.errorReportTimer);  
    
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
 
    #warning TODO: eo_encoderreader_Activate() should be changed. see comments below.
    // since EOappEncReader can only be crated, but not initted or de-inittedd, we must change it so that it can do that.
    

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
    {   // nothing to do because we dont have it
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
    {   // nothing to do because we dont have it
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
    {   // nothing to do because we dont have it
        return(eores_OK);
    }   

    if(position > eo_appEncReader_joint_position5)
    {
        return(eores_NOK_generic);
    }

    eOresult_t res = eo_appEncReader_GetJointValue(s_eo_theencoderreader.reader, (eo_appEncReader_joint_position_t)position, primary, secondary, errors); 
    
    
    return(res);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// - in here i put the functions used to initialise the values in ram of the joints and motors ... better in here rather than elsewhere.



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static eOresult_t s_eo_encoderreader_onstop_verifyreading(void *par, eObool_t readingisok)
{
    const eOmn_serv_arrayof_4jomodescriptors_t * jomodes = (const eOmn_serv_arrayof_4jomodescriptors_t *)par;
        
    if((eobool_true == readingisok) && (eobool_true == s_eo_theencoderreader.activateafterverify))
    {
        eo_encoderreader_Activate(&s_eo_theencoderreader, jomodes);        
    }

    s_eo_theencoderreader.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
    s_eo_theencoderreader.errorDescriptor.sourceaddress    = 0;
    s_eo_theencoderreader.errorDescriptor.par16            = s_eo_theencoderreader.numofjomos | (s_eo_theencoderreader.numofencoders << 8);
    memcpy(&s_eo_theencoderreader.errorDescriptor.par64, &s_eo_theencoderreader.errorflags[0], sizeof(s_eo_theencoderreader.errorflags));    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, s_eo_encodereader_send_periodic_error_report, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle()));
        
    if(eobool_true == readingisok)
    {        
        s_eo_theencoderreader.errorType = eo_errortype_debug;
        s_eo_theencoderreader.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_encoders_ok);
        eo_errman_Error(eo_errman_GetHandle(), s_eo_theencoderreader.errorType, NULL, s_eobj_ownname, &s_eo_theencoderreader.errorDescriptor);
        
        if((0 != s_eo_theencoderreader.repetitionOKcase) && (0 != s_eo_theencoderreader.reportPeriod))
        {
            s_eo_theencoderreader.errorCallbackCount = 20;        
            eo_timer_Start(s_eo_theencoderreader.errorReportTimer, eok_abstimeNOW, s_eo_theencoderreader.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }    
       
    if(eobool_false == readingisok)
    {
        s_eo_theencoderreader.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_encoders_failed_verify);
        s_eo_theencoderreader.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), s_eo_theencoderreader.errorType, NULL, s_eobj_ownname, &s_eo_theencoderreader.errorDescriptor);
        
        if(0 != s_eo_theencoderreader.reportPeriod)
        {
            s_eo_theencoderreader.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(s_eo_theencoderreader.errorReportTimer, eok_abstimeNOW, s_eo_theencoderreader.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }    
    
    if(NULL != s_eo_theencoderreader.onverify)
    {
        s_eo_theencoderreader.onverify(&s_eo_theencoderreader, readingisok); 
    }    
    
    return(eores_OK);   
}


static void s_eo_encodereader_send_periodic_error_report(void *p)
{
    eo_errman_Error(eo_errman_GetHandle(), s_eo_theencoderreader.errorType, NULL, s_eobj_ownname, &s_eo_theencoderreader.errorDescriptor);
    
    if(EOK_int08dummy != s_eo_theencoderreader.errorCallbackCount)
    {
        s_eo_theencoderreader.errorCallbackCount--;
    }
    if(0 == s_eo_theencoderreader.errorCallbackCount)
    {
        eo_timer_Stop(s_eo_theencoderreader.errorReportTimer);
    }
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
    uint8_t numberofencoders = 0;
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
                numberofencoders++;
                config.SPI_streams[config.joints[i].primary_enc_position%2].numberof++;
                config.SPI_streams[config.joints[i].primary_enc_position%2].type = hal_encoder_t1;
            }

            if(eo_appEncReader_enc_type_NONE != config.joints[i].extra_encoder)
            {
                numberofencoders++;
                config.SPI_streams[config.joints[i].extra_enc_position%2].numberof++;
                config.SPI_streams[config.joints[i].extra_enc_position%2].type = hal_encoder_t1;
            } 
        }        
    }
    
    s_eo_theencoderreader.numofencoders = numberofencoders;
    s_eo_theencoderreader.reader = eo_appEncReader_New(&config);
    
}


static void s_eo_encoderreader_read_encoders(void* p)
{
    eObool_t readingisok = eobool_true;
    
    uint8_t i = 0;
    
    memset(&s_eo_theencoderreader.errorflags, 0, sizeof(s_eo_theencoderreader.errorflags));
    
    for(i=0; i< s_eo_theencoderreader.numofjomos; i++)
    {
        uint32_t primary = 0;
        uint32_t secondary = 0;

        eo_appEncReader_GetJointValue(s_eo_theencoderreader.reader, (eo_appEncReader_joint_position_t)i, &primary, &secondary, &s_eo_theencoderreader.errorflags[i]);
        if((0 != s_eo_theencoderreader.errorflags[i].chip_error) || (0 != s_eo_theencoderreader.errorflags[i].data_error) || 
           (0 != s_eo_theencoderreader.errorflags[i].data_notready) || (0 != s_eo_theencoderreader.errorflags[i].tx_error) )
        {
            readingisok = eobool_false;
        }            
    }
    
    
    s_eo_encoderreader_onstop_verifyreading((void*)p, readingisok);   
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



