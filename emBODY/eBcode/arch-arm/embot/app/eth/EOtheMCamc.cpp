
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#if defined(USE_EOtheMCamc)

// marco.accame: include what we need
#include "stdlib.h"
#include "string.h"

#include "EoCommon.h"
#include "EOtheErrorManager.h"
#include "EoError.h"
#include "EOVtheCallbackManager.h"
#include "embot_app_eth_theErrorManager.h"
#include "embot_os_theScheduler.h"

#include "EOrop.h"
#include "EoProtocolMC.h"

#if defined(USE_EMBOT_theServices)  
#include "embot_app_eth_theServices.h"
#else
#include "EOtheServices.h"
#endif

#include "embot_app_eth_theEncoderReader.h"

#include "Controller.h"

#include "EOtheEntities.h"

#elif defined(EOTHESERVICES_disable_theMotionController)
#endif //#if defined(EOTHESERVICES_disable_theMotionController)

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMotionController.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#if defined(USE_EOtheMCamc)

#include "EOtheMCamc_hid.h"

#elif defined(EOTHESERVICES_disable_theMotionController)
#else
#endif

#if  defined(USE_EOtheMCamc)

static EOtheMotionController s_eo_themotcon = {}; 
static const char s_eobj_ownname[] = "EOtheMCamc";

    // provide implementation
    
static void s_mc_synchservice(eOmn_serv_state_t s)
{
#if defined(USE_EMBOT_theServices)
    embot::app::eth::theServices::getInstance().synch(embot::app::eth::Service::Category::mc, static_cast<embot::app::eth::Service::State>(s));
#else        
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mc, s);
#endif      
}    

static eOresult_t s_eo_motioncontrol_foc_onendofverify_encoder(EOaService* s, eObool_t operationisok);
static eOresult_t s_eo_motioncontrol_onstop_search4focs(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok);
static void s_eo_motioncontrol_send_periodic_error_report(void *par);
static eObool_t s_eo_motioncontrol_isID32relevant(uint32_t id32);
static eOresult_t s_eo_motioncontrol_updatedPositionsFromEncoders(EOtheMotionController *p);
static eOresult_t s_eo_motioncontrol_updatePositionFromEncoder(uint8_t index, embot::app::eth::encoder::v1::valueInfo *valueinfo);


static void s_eo_motioncontrol_UpdateJointMotorStatus(EOtheMotionController *p);


static void s_eo_motioncontrol_UpdateJointMotorStatus(EOtheMotionController *p)
{
    const uint8_t transmit_decoupled_pwms = 0;
    
    eOmc_joint_status_t *jstatus = NULL;
    eOmc_motor_status_t *mstatus = NULL;
    
    uint8_t jId = 0;
        
    for(jId = 0; jId<p->numofjomos; jId++)
    {
        if(NULL != (jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), jId)))
        {
//            #warning TODO-AMC-MC: add MController
            MController_get_joint_state(jId, jstatus);            
            MController_get_pid_state(jId, &jstatus->core.ofpid, transmit_decoupled_pwms);
        }
    }   
    
    for(jId = 0; jId<p->numofjomos; jId++)
    {
        if(NULL != (mstatus = eo_entities_GetMotorStatus(eo_entities_GetHandle(), jId)))
        {
 //           #warning TODO-AMC-MC: add MController
            MController_get_motor_state(jId, mstatus);
        }
    }
}


static eOresult_t s_eo_motioncontrol_updatePositionFromEncoder(uint8_t index, embot::app::eth::encoder::v1::valueInfo *valueinfo)
{
    eOresult_t res = eores_OK;
    
    if(valueinfo->errortype == embot::app::eth::encoder::v1::Error::NOTCONNECTED) // skip if the encoder is not connected (i.e is on 2foc.)
    {
        return res;
    }
        
    if(valueinfo->placement == embot::app::eth::encoder::v1::Placement::atjoint)
    {
        if(valueinfo->errortype !=embot::app::eth::encoder::v1::Error::NONE)
        {
          
            MController_invalid_absEncoder_fbk(index, embot::core::tointegral(valueinfo->errortype));
            res = eores_NOK_generic;
        }
        else
        { 
            // marco.accame on 03jul2023: MController_update_absEncoder_fbk() needs uint32_t* BUT MController_update_motor_pos_fbk() asks for an int32_t value            
            MController_update_absEncoder_fbk(index, valueinfo->value);
        }
    }
    else if(valueinfo->placement == embot::app::eth::encoder::v1::Placement::atmotor)
    {
        // marco.accame on 03jul2023: MController_update_absEncoder_fbk() needs uint32_t* BUT MController_update_motor_pos_fbk() asks for an int32_t value
        MController_update_motor_pos_fbk(index, static_cast<int32_t>(valueinfo->value[0]));
    }
    else
    {
        //do nothing;
    }
    return res;
} 


static eOresult_t s_eo_motioncontrol_updatedPositionsFromEncoders(EOtheMotionController *p)
{

    eOresult_t res = eores_OK;
    
    // wait for the encoders for some time
    for (uint8_t i=0; i<30; ++i)
    {
//        if (eo_encoderreader_IsReadingAvailable(eo_encoderreader_GetHandle()))
//        {
//            break;
//        }
        if(true == embot::app::eth::theEncoderReader::getInstance().IsReadingAvailable())
        {
            break;
        }
        
//        hal_sys_delay(5*hal_RELTIME_1microsec);
        embot::core::wait(5*embot::core::time1microsec);
    }        
        
    
 //   if(eobool_false == eo_encoderreader_IsReadingAvailable(eo_encoderreader_GetHandle()))
    if(false == embot::app::eth::theEncoderReader::getInstance().IsReadingAvailable())
    {
        for(uint8_t i=0; i<p->numofjomos; i++)
        {
 //           #warning TODO-AMC-MC: add MController  
            MController_timeout_absEncoder_fbk(i);
        }
        
        return(eores_NOK_timeout);
    }
    
    // read the encoders        
    for(uint8_t i=0; i<p->numofjomos; i++)
    {
        embot::app::eth::encoder::v1::valueInfo encoder1valueinfo {};
        embot::app::eth::encoder::v1::valueInfo encoder2valueinfo {};

        embot::app::eth::theEncoderReader::getInstance().Read(i, encoder1valueinfo, encoder2valueinfo);
                   
        if(eores_OK != s_eo_motioncontrol_updatePositionFromEncoder(i, &encoder1valueinfo))
        {
            res = eores_NOK_generic;
        }
        if(eores_OK != s_eo_motioncontrol_updatePositionFromEncoder(i, &encoder2valueinfo))
        {
            res = eores_NOK_generic;
        }
        
    } 
    
    embot::app::eth::theEncoderReader::getInstance().Diagnostics_Tick();
    
    return(res);

}

static eObool_t s_eo_motioncontrol_isID32relevant(uint32_t id32)
{
    static const uint32_t mask0 = (((uint32_t)eoprot_endpoint_motioncontrol) << 24) | (((uint32_t)eoprot_entity_mc_joint) << 16);
    static const uint32_t mask1 = (((uint32_t)eoprot_endpoint_motioncontrol) << 24) | (((uint32_t)eoprot_entity_mc_motor) << 16);
    static const uint32_t mask2 = (((uint32_t)eoprot_endpoint_motioncontrol) << 24) | (((uint32_t)eoprot_entity_mc_controller) << 16);
    
    if((id32 & mask0) == mask0)
    {
        return(eobool_true);
    }
    if((id32 & mask1) == mask1)
    {
        return(eobool_true);
    } 
    if((id32 & mask2) == mask2)
    {
        return(eobool_true);
    }    
    
    return(eobool_false); 
}

static eOresult_t s_eo_motioncontrol_foc_onendofverify_encoder(EOaService* s, eObool_t operationisok)
{    
    EOtheMotionController* p = &s_eo_themotcon;
    
    if(eobool_true == operationisok)
    {
        eo_candiscovery2_Start2(eo_candiscovery2_GetHandle(), p->sharedcan.discoverytargets, &p->sharedcan.ondiscoverystop);        
    }    
    else
    {
        // the encoder reader fails. we dont even start the discovery of the foc boards. we just issue an error report and call onverify() w/ false argument
        
        p->service.state = eomn_serv_state_failureofverify;
        s_mc_synchservice(p->service.state);
        
        // prepare things
        p->diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
        p->diagnostics.errorDescriptor.sourceaddress    = 0;
        p->diagnostics.errorDescriptor.par16            = 0;
        p->diagnostics.errorDescriptor.par64            = 0;    
        EOaction_strg astrg = {0};
        EOaction *act = (EOaction*)&astrg;
        eo_action_SetCallback(act, s_eo_motioncontrol_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));    
        

        // fill error description. and transmit it
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_foc_failed_encoders_verify);
        p->diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if(0 != p->diagnostics.reportPeriod)
        {
            p->diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
  
        // call onverify
        if(NULL != p->service.onverify)
        {
            p->service.onverify(p->service.onverifyarg, eobool_false); 
        }            
                
    }  
    
    return(eores_OK);    
}

static eOresult_t s_eo_motioncontrol_onstop_search4focs(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok)
{
    EOtheMotionController* p = &s_eo_themotcon;
    
    if(eobool_true == searchisok)
    {
        p->service.state = eomn_serv_state_verified;
        s_mc_synchservice(p->service.state);
    }
    else
    {   
        p->service.state = eomn_serv_state_failureofverify;
        s_mc_synchservice(p->service.state);
    }    
    
    if((eobool_true == searchisok) && (eobool_true == p->service.activateafterverify))
    {
        const eOmn_serv_configuration_t * mcserv = &p->service.servconfig; // or also: (const eOmn_serv_configuration_t *)par;
        eo_motioncontrol_Activate(p, mcserv);
    }
    
    p->diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.sourceaddress    = 0;
    p->diagnostics.errorDescriptor.par16            = 0;
    p->diagnostics.errorDescriptor.par64            = 0;    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, s_eo_motioncontrol_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));    
    
    if(eobool_true == searchisok)
    {        
        p->diagnostics.errorType = eo_errortype_debug;
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_foc_ok);
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if((0 != p->diagnostics.repetitionOKcase) && (0 != p->diagnostics.reportPeriod))
        {
            p->diagnostics.errorCallbackCount = p->diagnostics.repetitionOKcase;        
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    } 

    if(eobool_false == searchisok)
    {
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_foc_failed_candiscovery_of_foc);
        p->diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if(0 != p->diagnostics.reportPeriod)
        {
            p->diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }     
           
    if(NULL != p->service.onverify)
    {
        p->service.onverify(p->service.onverifyarg, searchisok); 
    }    
    
    return(eores_OK);   
}
    
static void s_eo_motioncontrol_send_periodic_error_report(void *par)
{
    EOtheMotionController* p = (EOtheMotionController*)par;
    p->diagnostics.errorDescriptor.par64 ++;
    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
    
    if(eoerror_value_CFG_mc_foc_failed_candiscovery_of_foc == eoerror_code2value(p->diagnostics.errorDescriptor.code) ||
       eoerror_value_CFG_mc_mc4_failed_candiscovery_of_mc4 == eoerror_code2value(p->diagnostics.errorDescriptor.code)
      )
    {   // if i dont find the focs / mc4s, i keep on sending the discovery results up. it is a temporary diagnostics tricks until we use the verification of services at bootstrap
        eo_candiscovery2_SendLatestSearchResults(eo_candiscovery2_GetHandle());
    }
    
    if(EOK_int08dummy != p->diagnostics.errorCallbackCount)
    {
        p->diagnostics.errorCallbackCount--;
    }
    if(0 == p->diagnostics.errorCallbackCount)
    {
        eo_timer_Stop(p->diagnostics.reportTimer);
    }
}

//void OnAsk(const EOnv* nv, const eOropdescriptor_t* rd)
//{
//    eOprotEndpoint_t endpoint = eoprot_ID2endpoint(rd->id32);
//    eOprotEntity_t entity = eoprot_ID2entity(rd->id32);
//    eOprotIndex_t index = eoprot_ID2index(rd->id32);
//    eOprotTag_t tag = eoprot_ID2tag(rd->id32);
//    
//    embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};    
//    const char * tagstr = eoprot_TAG2string(endpoint, entity, tag);
//    
//    embot::app::eth::theErrorManager::getInstance().trace(
//        std::string("ask<") + tagstr + ", " + std::to_string(index) + ">", {s_eobj_ownname, thr}); 
//        
//}

// public functions

extern EOtheMotionController* eo_motioncontrol_Initialise(void) 
{   
    EOtheMotionController *p = &s_eo_themotcon;
    
    if(eobool_true == p->service.initted)
    {
        return(p);
    }


//    eOropdescriptor_t tt = eok_ropdesc_basic;
//    tt.ropcode = eo_ropcode_ask;
//    tt.id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, 0, eoprot_tag_mc_joint_config);
//    OnAsk(nullptr, &tt);
    
//    eoprot_config_onask_endpoint_set(eoprot_endpoint_motioncontrol, OnAsk);    

    p->numofjomos = 0;

    p->service.tmpcfg = NULL;
    p->service.servconfig.type = eomn_serv_NONE;
    
    // up to to 12 mc4 OR upto 4 foc (the MAIS is managed directly by the EOtheMAIS object, and the PSCs boards are managed by the EOthePSC)
    p->sharedcan.boardproperties = eo_vector_New(sizeof(eObrd_canproperties_t), eo_motcon_maxJOMOs, NULL, NULL, NULL, NULL);
    
    // up to 12 jomos
    p->sharedcan.entitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), eo_motcon_maxJOMOs, NULL, NULL, NULL, NULL);
    
    p->sharedcan.discoverytargets = eo_array_New(1, sizeof(eOcandiscovery_target_t),  NULL);
    
    p->id32ofregulars = eo_array_New(motioncontrol_maxRegulars, sizeof(uint32_t), NULL);
    
//    #warning TODO add the controller
    p->ctrlobjs.thecontroller = MController_new(eo_motcon_standardJOMOs, eo_motcon_standardENCOs);
  
//    #warning TODO add the encoder reader
//    p->ctrlobjs.theencoderreader = eo_encoderreader_Initialise();
    embot::app::eth::theEncoderReader::getInstance().initialise();
            

    p->diagnostics.reportTimer = eo_timer_New();   
    p->diagnostics.errorType = eo_errortype_error;
    p->diagnostics.errorDescriptor.sourceaddress = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_not_verified_yet);    

    p->service.initted = eobool_true;
    p->service.active = eobool_false;
    p->service.started = eobool_false;
    p->service.state = eomn_serv_state_idle;    
//    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mc, p->service.state);    
    s_mc_synchservice(p->service.state);
    
        
    return(p);
}

extern EOtheMotionController* eo_motioncontrol_GetHandle(void)   
{ 
    if(eobool_true == s_eo_themotcon.service.initted)
    {
        return(&s_eo_themotcon);
    }
    
    return(NULL);
}


extern eOmn_serv_state_t eo_motioncontrol_GetServiceState(EOtheMotionController *p)
{
    if(NULL == p)
    {
        return(eomn_serv_state_notsupported);
    } 

    return(p->service.state);    
}


extern eOresult_t eo_motioncontrol_SendReport(EOtheMotionController *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
    
    eOerror_value_t errorvalue = eoerror_code2value(p->diagnostics.errorDescriptor.code);
    
    switch(errorvalue)
    {
        case eoerror_value_CFG_mc_foc_failed_candiscovery_of_foc:
//        case eoerror_value_CFG_mc_mc4_failed_candiscovery_of_mc4:
        {
            eo_candiscovery2_SendLatestSearchResults(eo_candiscovery2_GetHandle());            
        } break;
        
//        case eoerror_value_CFG_mc_mc4_failed_mais_verify:
//        case eoerror_value_CFG_mc_mc4plusmais_failed_candiscovery_of_mais:
//        {
//            eo_mais_SendReport(eo_mais_GetHandle());
//        } break;
        
//        case eoerror_value_CFG_mc_mc2pluspsc_failed_candiscovery_of_pscs:
//        {
//            eo_psc_SendReport(eo_psc_GetHandle());
//        } break;

//        case eoerror_value_CFG_mc_mc4plusfaps_failed_candiscovery_of_pmc:
//        {
//            eo_pos_SendReport(eo_pos_GetHandle());
//        } break;
        
        case eoerror_value_CFG_mc_foc_failed_encoders_verify:
//        case eoerror_value_CFG_mc_mc4plus_failed_encoders_verify:
//        case eoerror_value_CFG_mc_mc4plusmais_failed_encoders_verify:
//        case eoerror_value_CFG_mc_mc2pluspsc_failed_encoders_verify:
//        case eoerror_value_CFG_mc_mc4plusfaps_failed_encoders_verify:
        {
            embot::app::eth::theEncoderReader::getInstance().SendReport();
        } break;
        
        default:
        {
            // dont send any additional info
        } break;
    }
    
    
    return(eores_OK);      
}


extern eOmotioncontroller_mode_t eo_motioncontrol_GetMode(EOtheMotionController *p)
{
    if(NULL == p)
    {
        return(eo_motcon_mode_NONE);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eo_motcon_mode_NONE);
    } 

    return((eOmotioncontroller_mode_t)p->service.servconfig.type);
}


extern eOresult_t eo_motioncontrol_Verify2(EOtheMotionController *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, void *arg, eObool_t activateafterverify)
{   
    //eo_errman_Trace(eo_errman_GetHandle(), "called: eo_motioncontrol_Verify2()", s_eobj_ownname);
    
    if((NULL == p) || (NULL == servcfg))
    {
        s_eo_themotcon.service.state = eomn_serv_state_failureofverify;
        s_mc_synchservice(p->service.state);
        if(NULL != onverify)
        {
            onverify(arg, eobool_false); 
        }        
        return(eores_NOK_nullpointer);
    }
    
    if(eo_motcon_mode_foc != servcfg->type)  
    {
        embot::core::print("[ERROR] eo_motioncontrol_Verify2: servcfg->type unrecognized");
        p->service.state = eomn_serv_state_failureofverify;
        s_mc_synchservice(p->service.state);
        if(NULL != onverify)
        {
            onverify(arg, eobool_false); 
        }         
        return(eores_NOK_generic);
    }

    if(eobool_true == p->service.active)
    {
        eo_motioncontrol_Deactivate(p);        
    } 
    
    p->service.state = eomn_serv_state_verifying;
    s_mc_synchservice(p->service.state);

    // make sure the timer is not running
    eo_timer_Stop(p->diagnostics.reportTimer);        
    
    
    // marco.accame on 13 may 2016: i prefer copying what the pointer servcfg contains inside the object. 
    // the reason is that we are NOT guaranteed that the memory pointer by servcfg stays constant all the life of _Verify() actions, which can last well after the exit of the function.
    // in the past, servcfg was a pointer to ROM data stored in FLASH, hence it stayed constant, but with runtime configuration it is data kept in the RAM of a UDP packet which is not guaranteed to stay unchanged
    // in here we use memmove() to be more guaranteed in case that source and destination are in the same location.
    memmove(&p->service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));
    
    if(eo_motcon_mode_foc == p->service.servconfig.type)
    {
        // we shall always use this jomodescriptors in the form of a EOconstarray. it is a pointer to the actual data inside p->service.servconfig
        p->ctrlobjs.jomodescriptors = eo_constarray_Load((const EOarray*)&p->service.servconfig.data.mc.foc_based.arrayofjomodescriptors);
        
        p->service.onverify = onverify;
        p->service.onverifyarg = arg;
        p->service.activateafterverify = activateafterverify;

        // 1. prepare the can discovery for foc boards 
        eOcandiscovery_target_t trgt = {0};
        trgt.info.type = p->service.servconfig.data.mc.foc_based.type;
        trgt.info.protocol.major = p->service.servconfig.data.mc.foc_based.version.protocol.major; 
        trgt.info.protocol.minor = p->service.servconfig.data.mc.foc_based.version.protocol.minor;
        trgt.info.firmware.major = p->service.servconfig.data.mc.foc_based.version.firmware.major; 
        trgt.info.firmware.minor = p->service.servconfig.data.mc.foc_based.version.firmware.minor;   
        trgt.info.firmware.build = p->service.servconfig.data.mc.foc_based.version.firmware.build;   
                
        
        uint8_t numofjomos = eo_constarray_Size(p->ctrlobjs.jomodescriptors);
        uint8_t i = 0;
        for(i=0; i<numofjomos; i++)
        {
            const eOmc_jomo_descriptor_t *jomodes = (eOmc_jomo_descriptor_t*) eo_constarray_At(p->ctrlobjs.jomodescriptors, i);
            eo_common_hlfword_bitset(&trgt.canmap[jomodes->actuator.foc.canloc.port], jomodes->actuator.foc.canloc.addr);         
        }
        
        // force a cleaned discoverytargets before we add the target
        eo_array_Reset(p->sharedcan.discoverytargets);                
        eo_array_PushBack(p->sharedcan.discoverytargets, &trgt);
        p->sharedcan.ondiscoverystop.function = s_eo_motioncontrol_onstop_search4focs;
        p->sharedcan.ondiscoverystop.parameter = (void*)&p->service.servconfig;
        
        eOmn_serv_diagn_cfg_t dc = {0, 0};
        embot::app::eth::theEncoderReader::getInstance().Verify({p->ctrlobjs.jomodescriptors, dc}, {s_eo_motioncontrol_foc_onendofverify_encoder, p}, true);
        
    }

    return(eores_OK);  
}

extern eOresult_t eo_motioncontrol_Activate(EOtheMotionController *p, const eOmn_serv_configuration_t * servcfg)
{
    // eo_errman_Trace(eo_errman_GetHandle(), "called: eo_motioncontrol_Activate()", s_eobj_ownname);
    
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    }  

    if((eo_motcon_mode_foc != servcfg->type))
    {
        embot::core::print("[Error] eo_motioncontrol_Activate: unrecognized servcfg->type");
        return(eores_NOK_generic);
    }    
    
    if(eobool_true == p->service.active)
    {
        eo_motioncontrol_Deactivate(p);        
    } 

    // we use memmove() because it may be that source and destination are in the same location.
    memmove(&p->service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));    

    if(eo_motcon_mode_foc == p->service.servconfig.type)
    {

        p->ctrlobjs.jomodescriptors = eo_constarray_Load((const EOarray*)&p->service.servconfig.data.mc.foc_based.arrayofjomodescriptors);
        
        uint8_t numofjomos = eo_constarray_Size(p->ctrlobjs.jomodescriptors);
        
        eo_entities_SetNumOfJoints(eo_entities_GetHandle(), numofjomos);
        eo_entities_SetNumOfMotors(eo_entities_GetHandle(), numofjomos);
        

        if(0 == eo_entities_NumOfJoints(eo_entities_GetHandle()))
        {
            p->service.active = eobool_false;
            return(eores_NOK_generic);
        }
        else
        {                          
            p->numofjomos = numofjomos;
            
            uint8_t i = 0;
                        
            // load the can mapping 
            for(i=0; i<numofjomos; i++)
            {
                const eOmc_jomo_descriptor_t *jomodes = (eOmc_jomo_descriptor_t*) eo_constarray_At(p->ctrlobjs.jomodescriptors, i);
                
                eObrd_canproperties_t prop = {0};
                
                prop.type = p->service.servconfig.data.mc.foc_based.type;
                prop.location.port = jomodes->actuator.foc.canloc.port;
                prop.location.addr = jomodes->actuator.foc.canloc.addr;
                prop.location.insideindex = jomodes->actuator.foc.canloc.insideindex;
                prop.requiredprotocol.major = p->service.servconfig.data.mc.foc_based.version.protocol.major;
                prop.requiredprotocol.minor = p->service.servconfig.data.mc.foc_based.version.protocol.minor;
                
                eo_vector_PushBack(p->sharedcan.boardproperties, &prop);            
            }
            eo_canmap_LoadBoards(eo_canmap_GetHandle(), p->sharedcan.boardproperties); 
            
            // load the entity mapping.
            for(i=0; i<numofjomos; i++)
            {
                const eOmc_jomo_descriptor_t *jomodes = (eOmc_jomo_descriptor_t*) eo_constarray_At(p->ctrlobjs.jomodescriptors, i);
                
                eOcanmap_entitydescriptor_t des = {0};
                
                des.location.port = jomodes->actuator.foc.canloc.port;
                des.location.addr = jomodes->actuator.foc.canloc.addr;
                des.location.insideindex = jomodes->actuator.foc.canloc.insideindex;
                des.index = (eOcanmap_entityindex_t)i;

                eo_vector_PushBack(p->sharedcan.entitydescriptor, &des);            
            }        
            eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, p->sharedcan.entitydescriptor); 
            eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, p->sharedcan.entitydescriptor);        

            // init the encoders
            eOmn_serv_diagn_cfg_t dc = {0, 0};
            //dc.mode = servcfg->diagnosticsmode;
            //dc.par16 = servcfg->diagnosticsparam;
            embot::app::eth::theEncoderReader::getInstance().Activate({p->ctrlobjs.jomodescriptors, dc});
            
            // init the emscontroller.
            // marco.accame.TODO: change the emsController. see comments below
            //                the emscontroller is not a singleton which can be initted and deinitted. 
            // it should have a _Initialise(), a _GetHandle(), a _Config(cfg) and a _Deconfig().
//            if(NULL == p->ctrlobjs.thecontroller)
//            {
//                //p->ctrlobjs.thecontroller = eo_emsController_Init((eOeomc_ctrlboard_t)p->service.servconfig.data.mc.foc_based.boardtype4mccontroller, emscontroller_actuation_2FOC, numofjomos);
//                p->ctrlobjs.thecontroller = MController_new(numofjomos, numofjomos);
//                //MController_config_board((eOeomc_ctrlboard_t)p->service.servconfig.data.mc.foc_based.boardtype4mccontroller, HARDWARE_2FOC);                
//                MController_config_board(&p->service.servconfig);
//            }

//#warning TODO-AMC-MC: add MController_config_board                
            MController_config_board(&p->service.servconfig);
            
            // proxy config
//            s_eo_motioncontrol_proxy_config(p, eobool_true);
            
            p->service.active = eobool_true;
            p->service.state = eomn_serv_state_activated;
//            eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mc, p->service.state);    
            s_mc_synchservice(p->service.state);
        }
    
    }      
    
    return(eores_OK);  
}

extern eOresult_t eo_motioncontrol_Deactivate(EOtheMotionController *p)
{
    // eo_errman_Trace(eo_errman_GetHandle(), "called: eo_motioncontrol_Deactivate()", s_eobj_ownname);
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == p->service.active)
    {
        // i force to eomn_serv_state_idle because it may be that state was eomn_serv_state_verified or eomn_serv_state_failureofverify
        p->service.state = eomn_serv_state_idle; 
        //eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mc, p->service.state);
        s_mc_synchservice(p->service.state);
        return(eores_OK);        
    } 
        
    
    // at first we stop the service
    if(eobool_true == p->service.started)
    {
        eo_motioncontrol_Stop(p);   
    }    

    eo_motioncontrol_SetRegulars(p, NULL, NULL);
    

    if(eo_motcon_mode_foc == p->service.servconfig.type)
    {       
        // for foc-based, we must ... deconfig mc foc boards, unload them, set num of entities = 0, clear status, deactivate encoder 
        
        eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, p->sharedcan.entitydescriptor); 
        eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, p->sharedcan.entitydescriptor); 
        

        eo_canmap_UnloadBoards(eo_canmap_GetHandle(), p->sharedcan.boardproperties); 
        
        //eo_encoderreader_Deactivate(eo_encoderreader_GetHandle());
        embot::app::eth::theEncoderReader::getInstance().Deactivate();
        
        // marco.accame.TODO: call deinit of mcontroller
        
        // now i reset 
        eo_vector_Clear(p->sharedcan.entitydescriptor);
        eo_vector_Clear(p->sharedcan.boardproperties);   
        eo_array_Reset(p->sharedcan.discoverytargets);        

        // proxy deconfig
//        s_eo_motioncontrol_proxy_config(p, eobool_false);        
    
    }

    
    // Reset the communicated fault state to dummy for each motor
    uint8_t n_motors = eo_entities_NumOfMotors(eo_entities_GetHandle());
    
    for(uint8_t mId = 0; mId < n_motors; mId++)
    {
       eOmc_motor_status_t *mstatus = eo_entities_GetMotorStatus(eo_entities_GetHandle(), mId);
       if (NULL != mstatus)
       {
           mstatus->mc_fault_state = eoerror_code_dummy;
       }
    }
    
    p->numofjomos = 0;
    eo_entities_SetNumOfJoints(eo_entities_GetHandle(), 0);
    eo_entities_SetNumOfMotors(eo_entities_GetHandle(), 0);
    
    p->ctrlobjs.jomodescriptors = NULL;
    
    memset(&p->service.servconfig, 0, sizeof(eOmn_serv_configuration_t));
    p->service.servconfig.type = eo_motcon_mode_NONE;
        
    p->service.onverify = NULL;
    p->service.onverifyarg = NULL;
    p->service.activateafterverify = eobool_false;
    p->sharedcan.ondiscoverystop.function = NULL;
    p->sharedcan.ondiscoverystop.parameter = NULL;
    
    // make sure the timer is not running
    eo_timer_Stop(p->diagnostics.reportTimer);  
    
    p->service.active = eobool_false;    
    p->service.state = eomn_serv_state_idle; 
//    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mc, p->service.state);  
    s_mc_synchservice(p->service.state);    
    
    
    return(eores_OK);
}

extern eOresult_t eo_motioncontrol_Start(EOtheMotionController *p)
{ 
    // eo_errman_Trace(eo_errman_GetHandle(), "called: eo_motioncontrol_Start()", s_eobj_ownname);  
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }    
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    } 
        
    if(eobool_true == p->service.started)
    {   // it is already running
        return(eores_OK);
    }
       
    p->service.started = eobool_true;    
    p->service.state = eomn_serv_state_started;
//    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mc, p->service.state);    
    s_mc_synchservice(p->service.state); 
    

    // focbased: just init a read of the encoder
    if(eo_motcon_mode_foc == p->service.servconfig.type)
    {   
        // just start a reading of encoders        
        //eo_encoderreader_StartReading(eo_encoderreader_GetHandle());
        embot::app::eth::theEncoderReader::getInstance().StartReading();
    }
    else
    {    
        embot::core::print("[ERROR] eo_motioncontrol_Start: unrecognized servconfig.type");
        eo_errman_Trace(eo_errman_GetHandle(), "eo_motioncontrol_Start()", s_eobj_ownname);
    }
    
    return(eores_OK);    
}


extern eOresult_t eo_motioncontrol_SetRegulars(EOtheMotionController *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  

#if defined(USE_EMBOT_theServices)
    auto isok = [](uint32_t id){ return eobool_true == s_eo_motioncontrol_isID32relevant(id); };
    bool r = embot::app::eth::theServices::getInstance().setregulars(p->id32ofregulars, arrayofid32, isok, numberofthem);
    return r ? eores_OK : eores_NOK_generic;
#else    
    //return(eo_service_hid_SetRegulars(id32ofregulars, arrayofid32, s_ft_isID32relevant, numberofthem));
    return(eo_service_hid_SetRegulars(p->id32ofregulars, arrayofid32, s_eo_motioncontrol_isID32relevant, numberofthem));
#endif  
    
    
}

extern eOresult_t eo_motioncontrol_AddRegulars(EOtheMotionController *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  

#if defined(USE_EMBOT_theServices)
    auto isok = [](uint32_t id){ return eobool_true == s_eo_motioncontrol_isID32relevant(id); };
    bool r = embot::app::eth::theServices::getInstance().setregulars(p->id32ofregulars, arrayofid32, isok, numberofthem);
    return r ? eores_OK : eores_NOK_generic;
#else    
    //return(eo_service_hid_AddRegulars(p->id32ofregulars, arrayofid32, s_eo_motioncontrol_isID32relevant, numberofthem));
    return(eo_service_hid_AddRegulars(p->id32ofregulars, arrayofid32, s_eo_motioncontrol_isID32relevant, numberofthem));
#endif      
//    return(eo_service_hid_AddRegulars(p->id32ofregulars, arrayofid32, s_eo_motioncontrol_isID32relevant, numberofthem));
}


extern eOresult_t eo_motioncontrol_Tick(EOtheMotionController *p)
{   
    // static uint32_t adc_values[4] = {0, 0, 0, 0}; 
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }    
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated 
        return(eores_OK);
    } 
    
    if(eobool_false == p->service.started)
    {   // not running, thus we do nothing
        return(eores_OK);
    }
       
    
    // in here we ... see descriotion in s_delaymotorconfig_eval()
//    s_delaymotorconfig_eval(p);
    
    
    // 1) read positions from both joint and motor encoders and update motor controller engine.
    s_eo_motioncontrol_updatedPositionsFromEncoders(p);
    
//    eo_encoderreader_StartReading(eo_encoderreader_GetHandle()); // Restart the reading of the encoders
    embot::app::eth::theEncoderReader::getInstance().StartReading();
    
    // 2) perform motor control
//#warning TODO-AMC-MC: add MController      
    MController_do();
    
    // 3) update status of motors and joints
    s_eo_motioncontrol_UpdateJointMotorStatus(p);
    

    return(eores_OK);    
}


extern eOresult_t eo_motioncontrol_Stop(EOtheMotionController *p)
{ 
    // eo_errman_Trace(eo_errman_GetHandle(), "called: eo_motioncontrol_Stop()", s_eobj_ownname);
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }    
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated 
        return(eores_OK);
    } 
    
    if(eobool_false == p->service.started)
    {   // it is already stopped
        return(eores_OK);
    }
    
//#warning TODO-AMC-MC: add MController      
    MController_go_idle();
      
    p->service.started = eobool_false;
    p->service.state = eomn_serv_state_activated;
//    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mc, p->service.state);
    s_mc_synchservice(p->service.state);
    
    // remove all regulars related to motion control entities: joint, motor, controller ... no, dont do that
    //eo_motioncontrol_SetRegulars(p, NULL, NULL);
    
    // eo_errman_Trace(eo_errman_GetHandle(), "eo_motioncontrol_Stop()", s_eobj_ownname);
    
    return(eores_OK);    
}

    extern eOresult_t eo_motioncontrol_Transmission(EOtheMotionController *p, eObool_t on)
    {
        return eores_NOK_generic;
    }

    
    
extern eOresult_t eo_motioncontrol_AcceptCANframe(EOtheMotionController *p, eOcanframe_t *frame, eOcanport_t port, eOmotioncontroller_canframe_t cftype)
{
    // not yet implemented.
    // so far handling of can frames is managed externally
    return eores_OK;
} 
    
extern eOresult_t eo_motioncontrol_ConfigMotor(EOtheMotionController *p, uint8_t num, eOmc_motor_config_t *mc)
{
//    embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};    
//    embot::app::eth::theErrorManager::getInstance().trace(
//    std::string("motconfig-eth"), {s_eobj_ownname, thr}); 

    eOmotioncontroller_mode_t mcmode = eo_motioncontrol_GetMode(p);
    if(eo_motcon_mode_foc == mcmode)
    {
        // config motor straigth away
//        #warning TODO-AMC-MC: add MController  
        MController_config_motor(num, mc);
    }
    else
    {
        embot::core::print("[ERROR] eo_motioncontrol_ConfigMotor: mcmode unrecognized");
    }
   
    return eores_OK;
}


extern "C"
{
// - in here i put the functions used to initialise the values in ram of the joints and motors ... better in here rather than elsewhere.

static const eOmc_joint_t s_joint_default_value =
{   // to simplify we set everything to zero and then we edit eoprot_fun_INIT_mc_joint*() functions 
    0
};


static const eOmc_motor_t s_motor_default_value =
{   // to simplify we set everything to zero and then we edit eoprot_fun_INIT_mc_motor*() functions
    0
}; 


extern void eoprot_fun_INIT_mc_joint_config(const EOnv* nv)
{
    eOmc_joint_config_t *cfg = (eOmc_joint_config_t*)eo_nv_RAM(nv);
    memmove(cfg, &s_joint_default_value.config, sizeof(eOmc_joint_config_t));
}

extern void eoprot_fun_INIT_mc_joint_status(const EOnv* nv)
{
    eOmc_joint_status_t *sta = (eOmc_joint_status_t*)eo_nv_RAM(nv);
    memmove(sta, &s_joint_default_value.status, sizeof(eOmc_joint_status_t));
    
    sta->core.modes.controlmodestatus = eomc_controlmode_notConfigured;
    sta->core.modes.interactionmodestatus = eOmc_interactionmode_stiff;
}

extern void eoprot_fun_INIT_mc_motor_config(const EOnv* nv)
{
    eOmc_motor_config_t *cfg = (eOmc_motor_config_t*)eo_nv_RAM(nv);
    memmove(cfg, &s_motor_default_value.config, sizeof(eOmc_motor_config_t));
}

extern void eoprot_fun_INIT_mc_motor_status(const EOnv* nv)
{
    eOmc_motor_status_t *sta = (eOmc_motor_status_t*)eo_nv_RAM(nv);
    memmove(sta, &s_motor_default_value.status, sizeof(eOmc_motor_status_t));
    
    // Initialize the fault state to dummy since code zero is assigned to unspecified system error
    sta->mc_fault_state = eoerror_code_dummy;
}    
    
}

#elif defined(EOTHESERVICES_disable_theMotionController)   
#else

#endif // #elif !defined(EOTHESERVICES_disable_theMotionController)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



