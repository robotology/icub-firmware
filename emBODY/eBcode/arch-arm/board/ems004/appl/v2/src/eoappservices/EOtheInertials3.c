/*
 * Copyright (C) 2018 iCub Facility - Istituto Italiano di Tecnologia
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

#include "EOMtheEMSappl.h"

#include "EOVtheCallbackManager.h"

#include "EOtheMEMS.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheInertials3.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheInertials3_hid.h"

#if defined(EOTHESERVICES_disable_theInertials3)

    // provide empty implementation, so that we dont need to change the caller of the API
    
    extern EOtheInertials3* eo_inertials3_Initialise(void) 
    {   
        return NULL; 
    }

    extern EOtheInertials3* eo_inertials3_GetHandle(void)   
    { 
        return NULL; 
    }

    extern eOmn_serv_state_t eo_inertials3_GetServiceState(EOtheInertials3 *p) 
    { 
        return eomn_serv_state_notsupported; 
    }
    
    // in some cases, we need to alert the pc104 that the board does not support this service
    extern eOresult_t eo_inertials3_SendReport(EOtheInertials3 *p)
    {
        static const char s_eobj_ownname[] = "EOtheInertials3";
        eOerrmanDescriptor_t errdes = {};
        errdes.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials3_failed_notsupported);  
        errdes.sourcedevice = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress = 0;
        errdes.par16 = errdes.par64 = 0;
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);
        return eores_OK;
    }


    extern eOresult_t eo_inertials3_Verify(EOtheInertials3 *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify)
    {
        // we alert the host that the verification of the service has failed
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials3, eomn_serv_state_failureofverify);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        } 
        
        // we tell that the reason is that this service is not supported
        eo_inertials3_SendReport(NULL);
               
        return eores_NOK_generic;
    }

    extern eOresult_t eo_inertials3_Activate(EOtheInertials3 *p, const eOmn_serv_configuration_t * servcfg)
    {
        eo_inertials3_SendReport(NULL);
        return eores_NOK_generic;
    }

    extern eOresult_t eo_inertials3_Deactivate(EOtheInertials3 *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_inertials3_Start(EOtheInertials3 *p)
    {
        eo_inertials3_SendReport(NULL);
        return eores_NOK_generic;
    }

    extern eOresult_t eo_inertials3_SetRegulars(EOtheInertials3 *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
    {
        //eo_inertials3_SendReport(NULL);
        return eores_NOK_generic;
    }

    extern eOresult_t eo_inertials3_Tick(EOtheInertials3 *p, eObool_t resetstatus)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_inertials3_Stop(EOtheInertials3 *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_inertials3_Transmission(EOtheInertials3 *p, eObool_t on)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_inertials3_Config(EOtheInertials3 *p, eOas_inertial3_config_t* config)
    {
        eo_inertials3_SendReport(NULL);
        return eores_NOK_generic;
    }
    
    extern eOresult_t eo_inertials3_AcceptCANframe(EOtheInertials3 *p, eOcanframe_t *frame, eOcanport_t port, eOas_inertial3_type_t type)
    {
        return eores_NOK_generic;
    }


#elif !defined(EOTHESERVICES_disable_theInertials3)
    
// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define NOID16 0xffff
#define NOID08 0xff


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

static eOresult_t s_eo_inertials3_verify_local_sensors(EOtheInertials3 *p, const eOmn_serv_configuration_t * servcfg);

static eOresult_t s_eo_inertials3_TXstart(EOtheInertials3 *p);

static eOresult_t s_eo_inertials3_TXstop(EOtheInertials3 *p);

static eObool_t s_eo_inertials3_activestate_can_accept_canframe(void);

static void s_eo_inertials3_build_maps(EOtheInertials3* p, uint32_t enablemask);

static eOresult_t s_eo_inertials3_onstop_search4canboards(void *par, EOtheCANdiscovery2* p, eObool_t searchisok);

static void s_eo_inertials3_send_periodic_error_report(void *p);

static eObool_t s_eo_inertials3_isID32relevant(uint32_t id32);

static eObool_t s_eo_inertials3_get_id(eObrd_canlocation_t loc, eOas_inertial3_type_t type, uint8_t *id);

static void s_eo_inertials3_presenceofcanboards_reset(EOtheInertials3 *p);
static void s_eo_inertials3_presenceofcanboards_build(EOtheInertials3 *p);
static void s_eo_inertials3_presenceofcanboards_start(EOtheInertials3 *p);
static void s_eo_inertials3_presenceofcanboards_touch(EOtheInertials3 *p, eObrd_canlocation_t loc);
static void s_eo_inertials3_presenceofcanboards_tick(EOtheInertials3 *p);

static void s_eo_inertials3_imu_configure(EOtheInertials3 *p);
static void s_eo_inertials3_imu_transmission(EOtheInertials3 *p, eObool_t on, uint8_t period);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const eOas_inertial3_config_t s_eo_default_inertial3config =
{
    .datarate                       = 50,
    .numberofitemstofillateachtick  = 0,
    .filler                         = {0},
    .enabled                        = 0
};



static EOtheInertials3 s_eo_theinertials3 = 
{
    .service = 
    {
        .initted                = eobool_false,
        .active                 = eobool_false,
        .activateafterverify    = eobool_false,
        .started                = eobool_false,
        .onverify               = NULL,
        .state                  = eomn_serv_state_notsupported,
        .tmpcfg                 = NULL,
        .servconfig             = { .type = eomn_serv_NONE },
    },
    .diagnostics = 
    {
        .reportTimer            = NULL,
        .reportPeriod           = 0, // 10*EOK_reltime1sec, // with 0 we dont periodically report
        .errorDescriptor        = {0},
        .errorType              = eo_errortype_info,
        .errorCallbackCount     = 0, 
        .repetitionOKcase       = 0 // 10 // with 0 we transmit report only once at succesful activation
    },     
    .sharedcan =
    {
        .boardproperties        = NULL,
        .entitydescriptor       = NULL,
        .discoverytargets       = NULL,
        .ondiscoverystop        = {0},
        .command                = {0}, 
    },   
    
    .configured                 = eobool_false,
    .numofcanboards             = {0},

    .sensorsconfig              = {0},      
    .canmap_brd_active          = {0},    
    .ethmap_mems_active         = 0,
    
    .frommems2id                = {NOID16},
    .memsparam                  = {255},
    .memsconfig                 = { {mems_gyroscope_l3g4200, hal_gyroscope_range_250dps, 0} }, 
      
    .fifoofinertial3data        = NULL,
    
//    .fromcan2id)                {NOID08},
        
    
#if 0     
    .canmap_mtb_accel_int)      {0},
    .canmap_mtb_accel_ext)      {0},
    .canmap_mtb_gyros_ext)      {0},
#endif
    
    .inertial3                  = NULL,
    .id32ofregulars             = NULL,
    .setofboardinfos            = {0},
    .arrayofsensordescriptors   = NULL,
    .not_heardof_target         = {0},
    .not_heardof_status         = {0},
    .not_heardof_counter        = 0,
    .transmissionisactive       = eobool_false
};



static const char s_eobj_ownname[] = "EOtheInertials3";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheInertials3* eo_inertials3_Initialise(void)
{
    EOtheInertials3* p = &s_eo_theinertials3;
    if(eobool_true == p->service.initted)
    {
        return(p);
    }
            
    p->service.servconfig.type = eomn_serv_NONE;
        
    p->sharedcan.boardproperties = eo_vector_New(sizeof(eObrd_canproperties_t), eo_inertials3_maxnumberofCANboards, NULL, NULL, NULL, NULL);
    
    p->sharedcan.entitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), eo_inertials3_maxnumberofCANboards, NULL, NULL, NULL, NULL);
    
    p->sharedcan.discoverytargets = eo_array_New(eOas_inertials3_boardinfos_maxnumber, sizeof(eOcandiscovery_target_t),  NULL);
    
    memset(p->canmap_brd_active, 0, sizeof(p->canmap_brd_active));
    p->configured = eobool_false;
    memset(p->numofcanboards, 0, sizeof(p->numofcanboards));
    
    p->ethmap_mems_active = 0;
       
//    memset(p->fromcan2id, NOID08, sizeof(p->fromcan2id));       
    memset(p->frommems2id, NOID08, sizeof(p->frommems2id));     
    memset(p->memsparam, 255, sizeof(p->memsparam));
    
    s_eo_inertials3_presenceofcanboards_reset(p);
    
    p->inertial3 = NULL;
    p->id32ofregulars = eo_array_New(inertials3_maxRegulars, sizeof(uint32_t), NULL);
    memset(&p->setofboardinfos, 0, sizeof(p->setofboardinfos));
    p->arrayofsensordescriptors = eo_array_New(eOas_inertials3_descriptors_maxnumber, sizeof(eOas_inertial3_descriptor_t), NULL);
    
    memcpy(&p->sensorsconfig, &s_eo_default_inertial3config, sizeof(eOas_inertial3_config_t));
    p->fifoofinertial3data = eo_vector_New(sizeof(eOas_inertial3_data_t), eo_inertials3_fifocapacity, NULL, 0, NULL, NULL);
    
    eo_mems_Initialise(NULL);
    
    p->diagnostics.reportTimer = eo_timer_New();
    p->diagnostics.errorType = eo_errortype_error;
    p->diagnostics.errorDescriptor.sourceaddress = 0;
    p->diagnostics.errorDescriptor.sourcedevice = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials3_not_verified_yet);  
    
    p->service.initted = eobool_true;    
    p->service.active = eobool_false;
    p->service.started = eobool_false;
    p->transmissionisactive = eobool_false;
    p->service.state = eomn_serv_state_idle;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials3, p->service.state);
    
    return(p);   
}


extern EOtheInertials3* eo_inertials3_GetHandle(void)
{
    if(eobool_true == s_eo_theinertials3.service.initted)
    {
        return(&s_eo_theinertials3);
    }
    
    return(NULL);
}


extern eOmn_serv_state_t eo_inertials3_GetServiceState(EOtheInertials3 *p)
{
    if(NULL == p)
    {
        return(eomn_serv_state_notsupported);
    } 

    return(p->service.state);
}


extern eOresult_t eo_inertials3_SendReport(EOtheInertials3 *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
    
    eOerror_value_t errorvalue = eoerror_code2value(p->diagnostics.errorDescriptor.code);
    
    switch(errorvalue)
    {
        case eoerror_value_CFG_inertials3_failed_candiscovery:
        {
            eo_candiscovery2_SendLatestSearchResults(eo_candiscovery2_GetHandle());            
        } break;
        
        default:
        {
            // dont send any additional info
        } break;
    }    
    
    return(eores_OK);      
}


extern eOresult_t eo_inertials3_Verify(EOtheInertials3 *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify)
{
    eOresult_t res = eores_NOK_generic;
    
    if((NULL == p) || (NULL == servcfg))
    {
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials3, p->service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }        
        return(eores_NOK_nullpointer);
    } 

    if(eomn_serv_AS_inertials3 != servcfg->type)
    {
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials3, p->service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }        
        return(eores_NOK_generic);
    }   
    
 
    if(eobool_true == p->service.active)
    {
        eo_inertials3_Deactivate(p);        
    }   
    
    p->service.state = eomn_serv_state_verifying; 
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials3, p->service.state);    

    // make sure the timer is not running
    eo_timer_Stop(p->diagnostics.reportTimer);    

    p->service.onverify = onverify;
    p->service.activateafterverify = activateafterverify;
    
    // i get all the sensors.
    memcpy(&p->setofboardinfos, &servcfg->data.as.inertial3.setofboardinfos, sizeof(eOas_inertial3_setof_boardinfos_t));
    memcpy(p->arrayofsensordescriptors, &servcfg->data.as.inertial3.arrayofdescriptor, sizeof(eOas_inertial3_arrayof_descriptors_t));
    
    
    // at first we verify sensors which are local (if any)
    
    if(eores_OK != (res = s_eo_inertials3_verify_local_sensors(p, servcfg)))
    {        
        return res;        
    }
    
    // now we deal with sensors on can:
    // step-1: check vs max number of can boards in config
    // step-2: do discovery of all boards (let us begin w/ strain2 only at first)
    
    // force a cleaned discoverytargets before we add the target
    eo_array_Reset(p->sharedcan.discoverytargets);
    
    
    // the number of sensor descriptors
    uint8_t numofsensors = eo_array_Size(p->arrayofsensordescriptors);
    uint16_t overlappedmap[2] = {0};
    for(uint8_t b=0; b<eoas_inertial3_supportedboards_numberof(); b++)
    {
        eOcandiscovery_target_t trgt = {0};
        eObool_t found = eobool_false;
        eObrd_cantype_t brdtype = eoas_inertial3_supportedboards_gettype(b);
        const eObrd_info_t * brdinfo = eoas_inertial3_setof_boardinfos_find(&p->setofboardinfos, brdtype);
        if(NULL == brdinfo)
        {   // we dont have this boardtype 
            continue;
        }
        memmove(&trgt.info, brdinfo, sizeof(eObrd_info_t)); 
        trgt.canmap[eOcanport1] =  trgt.canmap[eOcanport2] = 0;
        // now i search for this brdtype inside the arrayofsensordescriptors
        for(uint8_t s=0; s<numofsensors; s++)
        {
            eOas_inertial3_descriptor_t *des = (eOas_inertial3_descriptor_t*) eo_array_At(p->arrayofsensordescriptors, s);
            if(NULL != des)
            {
                if((eobrd_place_can == des->on.any.place) && (brdtype == des->typeofboard))
                {
                    found = eobool_true;
                    eo_common_hlfword_bitset(&trgt.canmap[des->on.can.port], des->on.can.addr); 
                    eo_common_hlfword_bitset(&overlappedmap[des->on.can.port], des->on.can.addr); 
                }
            }            
        }
        if(eobool_true == found)
        {   // we have found a valid discovery target. we add it to the array
            eo_array_PushBack(p->sharedcan.discoverytargets, &trgt);
        }
    }
    
    
    
    // now we have a complete array of targets and also numofboards. we do some checks ...
    
    // we check that we have at least one discovery target    
    if(0 == eo_array_Size(p->sharedcan.discoverytargets))
    {
        p->diagnostics.errorDescriptor.sourcedevice       = eo_errman_sourcedevice_localboard;
        p->diagnostics.errorDescriptor.sourceaddress      = 0;
        p->diagnostics.errorDescriptor.par16              = 1;
        p->diagnostics.errorDescriptor.par64              = numofsensors;
       
        EOaction_strg astrg = {0};
        EOaction *act = (EOaction*)&astrg;
        eo_action_SetCallback(act, s_eo_inertials3_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));        
        
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials3_failed_generic);
        p->diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if(0 != p->diagnostics.reportPeriod)
        {
            p->diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);   
        }  
        
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials3, p->service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }    
        
        return(eores_NOK_generic);                 
                
    }
    
    
    uint8_t numofboards = eo_common_hlfword_bitsetcount(overlappedmap[eOcanport1]) + eo_common_hlfword_bitsetcount(overlappedmap[eOcanport2]);
    // we check that numofboards is ok.    
    if(numofboards > eo_inertials3_maxnumberofCANboards)
    {        
        p->diagnostics.errorDescriptor.sourcedevice       = eo_errman_sourcedevice_localboard;
        p->diagnostics.errorDescriptor.sourceaddress      = 0;
        p->diagnostics.errorDescriptor.par16              = (numofboards << 8) | (eo_inertials3_maxnumberofCANboards & 0x00ff);
        p->diagnostics.errorDescriptor.par64              = (overlappedmap[eOcanport2] << 16) | (overlappedmap[eOcanport1]);
       
        EOaction_strg astrg = {0};
        EOaction *act = (EOaction*)&astrg;
        eo_action_SetCallback(act, s_eo_inertials3_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));        
        
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials3_failed_toomanyboards);
        p->diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if(0 != p->diagnostics.reportPeriod)
        {
            p->diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);   
        }  
        
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials3, p->service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }    
        
        return(eores_NOK_generic); 
    }    

    // step-2: can discovery
    
    
    p->sharedcan.ondiscoverystop.function = s_eo_inertials3_onstop_search4canboards;
    p->sharedcan.ondiscoverystop.parameter = (void*)servcfg;
    
    // start discovery of can boards  
    eo_candiscovery2_Start2(eo_candiscovery2_GetHandle(), p->sharedcan.discoverytargets, &p->sharedcan.ondiscoverystop);   
   
    return(eores_OK);   
}


extern eOresult_t eo_inertials3_Deactivate(EOtheInertials3 *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == p->service.active)
    {
        // i force to eomn_serv_state_idle because it may be that state was eomn_serv_state_verified or eomn_serv_state_failureofverify
        p->service.state = eomn_serv_state_idle; 
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials3, p->service.state);
        return(eores_OK);        
    } 
    
    if(eobool_true == p->service.started)
    {
        eo_inertials3_Stop(p);
    }
    
    eo_inertials3_SetRegulars(p, NULL, NULL);
      
    eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_inertial3, p->sharedcan.entitydescriptor); 
    
    eo_canmap_UnloadBoards(eo_canmap_GetHandle(), p->sharedcan.boardproperties);
     
    
    eo_entities_SetNumOfInertials3(eo_entities_GetHandle(), 0);
    
    p->inertial3 = NULL;
    
    // no need to reset setofboardinfos
    eo_array_Reset(p->arrayofsensordescriptors);
    
    eo_vector_Clear(p->fifoofinertial3data);
                
    memset(&p->service.servconfig, 0, sizeof(eOmn_serv_configuration_t));
    p->service.servconfig.type = eomn_serv_NONE;
    
    eo_vector_Clear(p->sharedcan.boardproperties);
    eo_vector_Clear(p->sharedcan.entitydescriptor);
    eo_array_Reset(p->sharedcan.discoverytargets);
    
    // make sure the timer is not running
    eo_timer_Stop(p->diagnostics.reportTimer);  
    
    p->sensorsconfig.enabled = 0;
    p->configured = eobool_false;
    memset(p->numofcanboards, 0, sizeof(p->numofcanboards));
    p->service.active = eobool_false;    
    p->service.state = eomn_serv_state_idle;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials3, p->service.state);
    
    return(eores_OK);
}


extern eOresult_t eo_inertials3_Activate(EOtheInertials3 *p, const eOmn_serv_configuration_t * servcfg)
{
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    } 

    if(eomn_serv_AS_inertials3 != servcfg->type)
    {
        return(eores_NOK_generic);
    }
    
    if(eobool_true == p->service.active)
    {
        eo_inertials3_Deactivate(p);        
    }   
    
    eo_entities_SetNumOfInertials3(eo_entities_GetHandle(), 1);
    
    if(0 == eo_entities_NumOfInertials3(eo_entities_GetHandle()))
    {
        p->service.active = eobool_false;
        return(eores_NOK_generic);
    }
    
    p->inertial3 = eo_entities_GetInertial3(eo_entities_GetHandle(), 0);
    
    // now service config stuff
    
    memcpy(&p->service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));

    
            
    //#warning TODO: save memory be avoiding the duplication of p->service.servconfig into arrayofsensordescriptors and setofboardinfos
            
    // now i get all the sensors info from ...
    memcpy(&p->setofboardinfos, &servcfg->data.as.inertial3.setofboardinfos, sizeof(eOas_inertial3_setof_boardinfos_t));
    memcpy(p->arrayofsensordescriptors, &servcfg->data.as.inertial3.arrayofdescriptor, sizeof(eOas_inertial3_arrayof_descriptors_t));
    
    
    // now i build maps for check of presence
    
    s_eo_inertials3_presenceofcanboards_build(p);
    
    
    // now i must build the canbusmapping[] for EOtheCANmapping object
    
    uint8_t numofsensors = eo_array_Size(p->arrayofsensordescriptors);    

    uint16_t canbusmapping[2] = {0};
    eObrd_cantype_t boardtypes[2][16] = {eobrd_cantype_none};
    
    for(uint8_t i=0; i<numofsensors; i++)
    {
        eOas_inertial3_descriptor_t *des = (eOas_inertial3_descriptor_t*) eo_array_At(p->arrayofsensordescriptors, i);
        if(NULL != des)
        {
            if(eobrd_place_can == des->on.any.place)
            {
                eo_common_hlfword_bitset(&canbusmapping[des->on.can.port], des->on.can.addr);  
                boardtypes[des->on.can.port][des->on.can.addr] = (eObrd_cantype_t)des->typeofboard;
            }
        }
    }    
    
    
    eObrd_canproperties_t prop = {0};
    // only this stays constant:
    prop.location.insideindex = eobrd_caninsideindex_none;  
    // the others must be set
    prop.location.port = 0; prop.location.addr = 0; 
    prop.type = eobrd_cantype_none;
    prop.requiredprotocol.major = 0;
    prop.requiredprotocol.minor = 0;

    
    eOcanmap_entitydescriptor_t des = {0};
    // only this stays constant: 
    des.location.insideindex = eobrd_caninsideindex_none;
     // the others must be set
    des.location.port = 0; des.location.addr = 0; 
    des.index = entindex00;
       

    // ok, we have one of these boards 
    for(uint8_t j=0; j<eOcanports_number; j++)
    {
        for(uint8_t k=1; k<15; k++)
        {
            if(eobool_true == eo_common_hlfword_bitcheck(canbusmapping[j], k))
            {   // i pushback. i dont verify vs the capacity of the vector because eo_inertials3_Verify() has already done it
                
                prop.type = boardtypes[j][k];
                const eObrd_info_t * brdinfo = eoas_inertial3_setof_boardinfos_find(&p->setofboardinfos, (eObrd_cantype_t)prop.type);
                
                if(NULL == brdinfo)
                {
                    prop.requiredprotocol.major = 0;
                    prop.requiredprotocol.minor = 0;                    
                }
                else
                {
                    prop.requiredprotocol.major = brdinfo->protocol.major;
                    prop.requiredprotocol.minor = brdinfo->protocol.minor;                    
                }
                
                prop.location.port = j;
                prop.location.addr = k;
                eo_vector_PushBack(p->sharedcan.boardproperties, &prop);
                
                des.location.port = j;
                des.location.addr = k;
                des.index = entindex00;                        
                eo_vector_PushBack(p->sharedcan.entitydescriptor, &des);                
            }
        }
    }



    
    // load the can mapping 
    eo_canmap_LoadBoards(eo_canmap_GetHandle(), p->sharedcan.boardproperties); 
    
    // load the entity mapping.
    eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_inertial3, p->sharedcan.entitydescriptor);   
    
    
    // build the maps that can translate a received can message into the id to be put into ...
    // we assume that 
    s_eo_inertials3_build_maps(p, 0xffffffff);
    p->configured = eobool_false; // still false
    p->service.active = eobool_true;    
    p->service.state = eomn_serv_state_activated;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials3, p->service.state);    

    
    return(eores_OK);   
}



extern eOresult_t eo_inertials3_Start(EOtheInertials3 *p)
{
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
    
//    if(eobool_true == p->service.started)
//    {   // if running we stop before
//        eo_inertials3_Stop(p);
//    } 
    

    // ok, now we do something.    
         
    //s_eo_inertials3_TXstart(p);

    p->service.started = eobool_true;    
    p->service.state = eomn_serv_state_started;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials3, p->service.state);
    
    
    return(eores_OK);      
}

extern eOresult_t eo_inertials3_SetRegulars(EOtheInertials3 *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
    
    return(eo_service_hid_SetRegulars(p->id32ofregulars, arrayofid32, s_eo_inertials3_isID32relevant, numberofthem));
}


extern eOresult_t eo_inertials3_Stop(EOtheInertials3 *p)
{
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
        

    // ok, now we do something.    
      
    s_eo_inertials3_TXstop(p);
    
               
    p->service.started = eobool_false;
    p->service.state = eomn_serv_state_activated;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials3, p->service.state);
    
    // reset the various buffers
//    eOas_inertial_data_t *data = &p->inertial3->status.data;
//    memset(data, 0, sizeof(eOas_inertial_data_t)); 
    eo_vector_Clear(p->fifoofinertial3data);
    
    
    // remove all regulars related to inertials entity ... no, dont do that.
    //eo_inertials3_SetRegulars(p, NULL, NULL);
    
    return(eores_OK);     
}


extern eOresult_t eo_inertials3_Transmission(EOtheInertials3 *p, eObool_t on)
{
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

    if(eobool_false == p->configured)
    {   // nothing to do because we dont have a configured service 
        return(eores_OK);
    } 

    if(eobool_true == on)
    {
        s_eo_inertials3_TXstart(p);
    }
    else
    {
        s_eo_inertials3_TXstop(p);
    }

    return(eores_OK);
}


extern eOresult_t eo_inertials3_Tick(EOtheInertials3 *p, eObool_t resetstatus)
{
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

    if(eobool_false == p->configured)
    {   // nothing to do because we dont have a configured service 
        return(eores_OK);
    }    
    
    if(eobool_true == p->transmissionisactive)
    {
        s_eo_inertials3_presenceofcanboards_tick(p);
    }

    if(eobool_false == resetstatus)
    {   // nothing to do because we cannot overwrite the status of inertial 
        return(eores_OK);
    }      

    // i can update the status of the inertial because the old value has already been transmitted.
        
    // the status data of the inertial entity is in ...
    
    EOarray * statusarrayofdata = (EOarray*) &p->inertial3->status.arrayofdata;
    
    eo_array_Reset(statusarrayofdata);
    
    eOas_inertial3_data_t i3data = {0};
    
    eOmems_sensor_t sensor = mems_gyroscope_l3g4200;
    eOas_inertial_data_t memsdata = {0};
    // i put new data in status with this rule:
    // first: if we have a mems i add it; second: if we have something inside fifoofinertial3data.
    // if nothing: i set data invalid
    if(eores_OK == eo_mems_Get(eo_mems_GetHandle(), &memsdata, eok_reltimeZERO, &sensor, NULL))
    {
        //eo_errman_Trace(eo_errman_GetHandle(), "tx mems", s_eobj_ownname);
        // ok, now i fill data
        uint8_t index = (mems_gyroscope_l3g4200 == sensor) ? (inertials3_mems_gyro) : (inertials3_mems_accel);
        i3data.id = p->frommems2id[index];
        i3data.typeofsensor = (mems_gyroscope_l3g4200 == sensor) ? eoas_inertial3_gyros_ems_st_l3g4200d : eoas_inertial3_accel_ems_st_lis3x;
        i3data.status.general = 0; // ok so far
        i3data.timestamp = memsdata.timestamp / 1000; // nothing so far.
        i3data.x = memsdata.x;
        i3data.y = memsdata.y;
        i3data.z = memsdata.z;
        eo_array_PushBack(statusarrayofdata, &i3data);
    }
    else if(eobool_false == eo_vector_Empty(p->fifoofinertial3data))
    {
        uint8_t fifosize = eo_vector_Size(p->fifoofinertial3data);
        uint8_t capacity =  eo_array_Capacity(statusarrayofdata); 
        uint8_t nitems = (fifosize < capacity) ? fifosize : capacity;
        for(uint8_t i=0; i<nitems; i++)
        {
            eOas_inertial3_data_t * item = (eOas_inertial3_data_t*) eo_vector_Front(p->fifoofinertial3data);
            if(NULL != item)
            {
                eo_array_PushBack(statusarrayofdata, item);            
                eo_vector_PopFront(p->fifoofinertial3data);   
                //eo_errman_Trace(eo_errman_GetHandle(), "tx mtb", s_eobj_ownname);
            }  
        }        
    }
    else
    {
        // the arraystatus is already resized to zero.       
    }
                 
    return(eores_OK);        
}



extern eOresult_t eo_inertials3_Config(EOtheInertials3 *p, eOas_inertial3_config_t* config)
{
    if((NULL == p) || (NULL == config))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
  
// we allow doing things also if we are not in running mode yet    
//    if(eobool_false == p->service.started)
//    {   // not running, thus we do nothing
//        return(eores_OK);
//    }
    
    
    // ok, now we do something. 
    
    // at first we copy the target config into the local config
    
    p->sensorsconfig.datarate           = config->datarate;
    p->sensorsconfig.enabled            = config->enabled;
    
    // then we check enabled mask and datarate
       
    uint8_t originalrate = p->sensorsconfig.datarate;
    if(p->sensorsconfig.datarate < 5)
    {
        p->sensorsconfig.datarate = 5;        
    }
    
    if(p->sensorsconfig.datarate > 200)
    {
        p->sensorsconfig.datarate = 200;          
    }
    
    if(originalrate != p->sensorsconfig.datarate)
    {
        eOerrmanDescriptor_t errdes = {0};
        errdes.sourcedevice       = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress      = 0;
        errdes.par16              = (originalrate << 8) | (p->sensorsconfig.datarate);
        errdes.par64              = 0;        
        errdes.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials3_changed_requestedrate);
        p->diagnostics.errorType = eo_errortype_warning;                
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &errdes);        
    }
        
    
    // now ... we need to change the masks according to p->sensorsconfig.maskofenabled
    
    s_eo_inertials3_build_maps(p, p->sensorsconfig.enabled);
    
    
    p->configured = eobool_true;    
 
    // ok guys: now we transmit the configuration to each one of the sensors which have a non-zero bit inside teh p->config2.enabled mask.
    // WAIT A MINUTE: we do it inside eo_inertials3_Start() !!!!!!!!!!!!

    
    return(eores_OK);           
}


extern eOresult_t eo_inertials3_AcceptCANframe(EOtheInertials3 *p, eOcanframe_t *frame, eOcanport_t port, eOas_inertial3_type_t type)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
  
    // we must be in run mode to accept frames, because the tick must be working   
    if(eobool_false == p->service.started)
    {   // not running, thus we dont accept
        return(eores_OK);
    } 

    if(eobool_false == p->configured)
    {   // nothing to do because we dont have a configured service 
        return(eores_OK);
    }  
    
    if(eobool_false == s_eo_inertials3_activestate_can_accept_canframe())
    {
        return(eores_OK);
    }
    
    // VERY IMPORTANT: the MTB can board does not tell whether we have an internal or external accel. it should do that but it does not.
    // hence, in type we use indifferently eoas_inertial_accel_mtb_int or eoas_inertial_accel_mtb_ext
    
    // the inertial entity is in ...
    //p->inertial2 = p->inertial2;    
    // however, we dont use it ...
    // we put what we have inside a fifo of eOas_inertial3_data_t
    eOas_inertial3_data_t data = {0};
       
    
    eObrd_canlocation_t loc = {0};    
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
    loc.insideindex = eobrd_caninsideindex_none;
    
    uint8_t id = NOID08;

    
    if(eobool_true == eo_vector_Full(p->fifoofinertial3data))
    {
        // damn... a loss of can frames
        eOerrmanDescriptor_t des = {0};
        des.code            = eoerror_code_get(eoerror_category_InertialSensor, eoerror_value_IS_arrayofinertialdataoverflow);
        des.par16           = (frame->id & 0x0fff) | ((frame->size & 0x000f) << 12);
        des.par64           = eo_common_canframe_data2u64((eOcanframe_t*)frame);
        des.sourceaddress   = EOCANPROT_FRAME_GET_SOURCE(frame);
        des.sourcedevice    = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &des);    
        
        // BUT dont want to lose NEW data ... i could just call a pop front to remove oldest data
        //eo_vector_PopFront(p->fifoofinertial3data);  
        // BUT .. as i dont want to througth data away, i decide that i put it inside the status.
        // in short: i call eo_inertials3_Tick(p, true) which already does that        
        eo_inertials3_Tick(p, eobool_true);      
    }
        
    switch(type)
    {
        case eoas_inertial3_accel_mtb_int:
        case eoas_inertial3_accel_mtb_ext:
        case eoas_inertial3_gyros_mtb_ext:
        {
            // case of a canframe with [xx|yy|zz]
            data.x = (int16_t)((frame->data[1]<<8) + frame->data[0]);
            data.y = (int16_t)((frame->data[3]<<8) + frame->data[2]);
            data.z = (int16_t)((frame->data[5]<<8) + frame->data[4]);               
        } break;
        
        case eoas_inertial3_imu_acc:     
        case eoas_inertial3_imu_mag:
        case eoas_inertial3_imu_gyr:
        case eoas_inertial3_imu_eul:
        case eoas_inertial3_imu_lia:
        case eoas_inertial3_imu_grv:
        {
            // case of a canframe with [seq|snsr|x-lsb|x-msb|y-lsb|y-msb|z-lsb|z-msb]
            data.seq = frame->data[0];
            data.x = (int16_t)(((int16_t)frame->data[3]<<8) + frame->data[2]);
            data.y = (int16_t)(((int16_t)frame->data[5]<<8) + frame->data[4]);
            data.z = (int16_t)(((int16_t)frame->data[7]<<8) + frame->data[6]);    
        } break;
        
        case eoas_inertial3_imu_qua:
        {
            // case of a canframe with [w-lsb|w-msb|x-lsb|x-msb|y-lsb|y-msb|z-lsb|z-msb]
            data.w = (int16_t)((int16_t)(frame->data[1]<<8) + frame->data[0]);
            data.x = (int16_t)((int16_t)(frame->data[3]<<8) + frame->data[2]);
            data.y = (int16_t)((int16_t)(frame->data[5]<<8) + frame->data[4]);
            data.z = (int16_t)((int16_t)(frame->data[7]<<8) + frame->data[6]);   
        } break;
        
        case eoas_inertial3_imu_status:
        {
            // case of a canframe with [seq|gyrsta|accsta|magsta|(acquisitiontime32bitusec)]
            data.seq = frame->data[0];
            data.status.general = 0;
            data.status.calib.gyr |= (frame->data[1]& 0x0003); // gyro
            data.status.calib.acc |= (frame->data[2]& 0x0003); // acc
            data.status.calib.mag |= (frame->data[3]& 0x0003); // mag
        } break;
        
        default:
        {
            
        } break;
    }
    
    if(eobool_false == s_eo_inertials3_get_id(loc, type, &id))
    {
        // send up diagnostics
        eOerrmanDescriptor_t des = {0};
        des.code            = eoerror_code_get(eoerror_category_InertialSensor, eoerror_value_IS_unknownsensor);
        des.par16           = (frame->id & 0x0fff) | ((frame->size & 0x000f) << 12);
        des.par64           = eo_common_canframe_data2u64((eOcanframe_t*)frame);
        des.sourceaddress   = EOCANPROT_FRAME_GET_SOURCE(frame);
        des.sourcedevice    = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &des); 
        return(eores_OK);        
    }
    
    data.typeofsensor = type;
    data.timestamp =  eov_sys_LifeTimeGet(eov_sys_GetHandle()) / 1000;
    data.id = id;
        
    // ok, now we can pushback data into the fifoofinertial3data   
    eo_vector_PushBack(p->fifoofinertial3data, &data);
    
#if 0    
        eOerrmanDescriptor_t des = {0};
        des.code            = eoerror_code_get(eoerror_category_InertialSensor, eoerror_value_IS_arrayofinertialdataoverflow);
        des.par16           = (frame->id & 0x0fff) | ((frame->size & 0x000f) << 12);
        des.par64           = eo_common_canframe_data2u64((eOcanframe_t*)frame);
        des.sourceaddress   = EOCANPROT_FRAME_GET_SOURCE(frame);
        des.sourcedevice    = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &des);  
#endif
    
    s_eo_inertials3_presenceofcanboards_touch(p, loc);
    
    return(eores_OK);      
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// -- in here .... so that all things related to Inertial are in a unique place


extern void eoprot_fun_INIT_as_inertial3_config(const EOnv* nv)
{
    eOas_inertial3_config_t* config = (eOas_inertial3_config_t*) eo_nv_RAM(nv);
    
    config->datarate = s_eo_default_inertial3config.datarate;
    config->enabled = s_eo_default_inertial3config.enabled;
    config->numberofitemstofillateachtick = 0;
}


extern void eoprot_fun_INIT_as_inertial3_status(const EOnv* nv)
{
    eOas_inertial3_status_t* status = (eOas_inertial3_status_t*) eo_nv_RAM(nv);  
    // must initialise it as an array
    
    uint8_t capacity = eOas_inertials3_data_maxnumber;
    uint8_t itemsize = sizeof(eOas_inertial3_data_t);
    uint8_t size = 0;
    EOarray* array = NULL;

    array = eo_array_New(capacity, itemsize, &status->arrayofdata);
    eo_array_Resize(array, size);
}

                
extern eObool_t eocanprotINperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_INERTIAL3_MSG(eOcanframe_t *frame, eOcanport_t port)
{   
    EOtheInertials3 *p = &s_eo_theinertials3;  
    
    if(NULL == frame)
    {
        return(eobool_true);
    }    
    
    // marco.accame:        
    // i dont want to put data in the fifoofinertial3data when we are not in the control loop.
    // moreover: i put it in its inside only if we have called _Start() 
    
    if(eobool_false == p->service.started)
    {   // it is not running
        return(eobool_true);
    } 

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

static eOresult_t s_eo_inertials3_verify_local_sensors(EOtheInertials3 *p, const eOmn_serv_configuration_t * servcfg)
{
    eOresult_t res = eores_OK;
    
    uint64_t errormask = 0; // at most i have eOas_inertials3_maxnumber = 48 items
    uint8_t numoferrors = 0;
    uint8_t numofsensors = eo_array_Size(p->arrayofsensordescriptors);    
    for(uint8_t i=0; i<numofsensors; i++)
    {
        eOas_inertial3_descriptor_t *des = (eOas_inertial3_descriptor_t*) eo_array_At(p->arrayofsensordescriptors, i);
        if(NULL != des)
        {
            if(eobrd_place_eth == des->on.any.place)
            {
                // it can be ... eoas_inertial_gyros_ems_st_l3g4200d only
                if((eoas_inertial3_gyros_ems_st_l3g4200d == des->typeofsensor) && (eobrd_ems4 == des->typeofboard) && (eobool_true == eo_mems_IsSensorSupported(eo_mems_GetHandle(), mems_gyroscope_l3g4200)))
                {
                    // ok, verify is ok . we keep res = eores_OK.
                }
//                else if((eoas_inertial_accel_ems_st_lis3x == des->type) && (hal_true == hal_accelometer_supported_is(hal_acceleromter1)))
//                {
//                }    
                else
                {
                    // we dont support it ... must do something
                    res = eores_NOK_generic; 
                    eo_common_dword_bitset(&errormask, i);
                    numoferrors ++;
                }
            }
        }
    }

    if(eores_OK == res)
    {
        // must enable .... 
    }
    else
    {        
        p->diagnostics.errorDescriptor.sourcedevice       = eo_errman_sourcedevice_localboard;
        p->diagnostics.errorDescriptor.sourceaddress      = 0;
        p->diagnostics.errorDescriptor.par16              = numoferrors;
        p->diagnostics.errorDescriptor.par64              = 0;
       
        EOaction_strg astrg = {0};
        EOaction *act = (EOaction*)&astrg;
        eo_action_SetCallback(act, s_eo_inertials3_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));        
        
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials3_failed_unsupportedsensor);
        p->diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if(0 != p->diagnostics.reportPeriod)
        {
            p->diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);   
        }  
        
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials3, p->service.state);
        if(NULL != p->service.onverify)
        {
            p->service.onverify(p, eobool_false); 
        }    
             
    }
        
    return(res);
}


static eOresult_t s_eo_inertials3_TXstart(EOtheInertials3 *p)
{ 
    if(eobool_false == p->configured)
    {   // dont have a configured service
        return(eores_OK);
    }    
    
    if((0 == p->canmap_brd_active[0]) && (0 == p->canmap_brd_active[1]) && (0 == p->ethmap_mems_active))
    {   // no can boards or onboard sensors configured
        return(eores_OK);
    } 
    
    if(0 == p->sensorsconfig.enabled)
    {   // no can boards or local mems enabled
        return(eores_OK);
    } 
    
    if(eobool_true == eo_common_byte_bitcheck(p->ethmap_mems_active, inertials3_mems_gyro))
    {
        eo_mems_Config(eo_mems_GetHandle(), &p->memsconfig[inertials3_mems_gyro]);
        eo_mems_Start(eo_mems_GetHandle());
    }                        
 
    
    //#warning create the can message for inertials3 config and place it in another place ...         
    s_eo_inertials3_imu_configure(p);

    //#warning create the can message for inertials3 tx start and place it in here ...     
    s_eo_inertials3_imu_transmission(p, eobool_true, p->sensorsconfig.datarate);
    
    
    p->transmissionisactive = eobool_true;
    s_eo_inertials3_presenceofcanboards_start(p);
    
    return(eores_OK);   
}


static eOresult_t s_eo_inertials3_TXstop(EOtheInertials3 *p)
{
    if(eobool_false == p->configured)
    {   // nothing to do because we dont have a configured service 
        return(eores_OK);
    }     

    //#warning create the inetrials3 can message
    
    // stop the imus
    s_eo_inertials3_imu_transmission(p, eobool_false, p->sensorsconfig.datarate);
    
    // stop mtbs
    //#warning TODO: s_eo_inertials3_mtb_transmission();
    
    // stop the mems
    eo_mems_Stop(eo_mems_GetHandle());

    p->transmissionisactive = eobool_false;
    s_eo_inertials3_presenceofcanboards_reset(p);
               
    return(eores_OK);
}


//#define EOINERTIALS3_ALWAYS_ACCEPT_CANFRAMES_IN_ACTIVE_MODE
static eObool_t s_eo_inertials3_activestate_can_accept_canframe(void)
{
#if     defined(EOINERTIALS3_ALWAYS_ACCEPT_CANFRAMES_IN_ACTIVE_MODE)
    // in this case the service.active inertials always accepts can frames, even if the run mode is not.
    // it is up to robotInterface to put system in run mode, so that the overflow of fifo is avoided
    return(eobool_true);
#else
    // in this mode the service.active inertials accepts can frames only if we are in run mode, thus the fifo can be emptied.    
    eOsmStatesEMSappl_t applstate = eo_sm_emsappl_STerr;
    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &applstate);   
    if(eo_sm_emsappl_STrun == applstate)
    {
        return(eobool_true);
    }    
    return(eobool_false);    
#endif       
}


static void s_eo_inertials3_build_maps(EOtheInertials3* p, uint32_t enablemask)
{
    uint8_t numofsensors = eo_array_Size(p->arrayofsensordescriptors);    
  
    // at first we disable all. 
    
    memset(p->canmap_brd_active, 0, sizeof(p->canmap_brd_active));
    p->ethmap_mems_active = 0;
    memset(p->frommems2id, NOID08, sizeof(p->frommems2id));
    memset(p->memsparam, 255, sizeof(p->memsparam));    
    
//    memset(p->fromcan2id, NOID08, sizeof(p->fromcan2id));


//    memset(p->canmap_mtb_accel_int, 0, sizeof(p->canmap_mtb_accel_int));
//    memset(p->canmap_mtb_accel_ext, 0, sizeof(p->canmap_mtb_accel_ext));
//    memset(p->canmap_mtb_gyros_ext, 0, sizeof(p->canmap_mtb_gyros_ext));
    p->ethmap_mems_active = 0;

    for(uint8_t i=0; i<numofsensors; i++)
    {
        eOas_inertial3_descriptor_t *des = (eOas_inertial3_descriptor_t*) eo_array_At(p->arrayofsensordescriptors, i);
        if(NULL != des)
        {
            eObool_t enabled = eo_common_word_bitcheck(enablemask, i);
            
            if(eobool_false == enabled)
            {
                continue;
            }
            
            if(eobrd_place_can == des->on.any.place)
            {                
                eo_common_hlfword_bitset(&p->canmap_brd_active[des->on.can.port], des->on.can.addr); 

                switch(des->typeofsensor)
                {
//                    case eoas_inertial_accel_mtb_int:
//                    {
//                        p->fromcan2id[des->on.can.port][des->on.can.addr][eoas_inertial_accel_mtb_int-eoas_inertial_accel_mtb_int] = i;
//                        eo_common_hlfword_bitset(&p->canmap_mtb_accel_int[des->on.can.port], des->on.can.addr);                        
//                    } break;

//                    case eoas_inertial_accel_mtb_ext:
//                    {       
//                        p->fromcan2id[des->on.can.port][des->on.can.addr][eoas_inertial_accel_mtb_ext-eoas_inertial_accel_mtb_int] = i;
//                        eo_common_hlfword_bitset(&p->canmap_mtb_accel_ext[des->on.can.port], des->on.can.addr);
//                    } break;  
                    
//                    case eoas_inertial_gyros_mtb_ext:
//                    {                        
//                        p->fromcan2id[des->on.can.port][des->on.can.addr][eoas_inertial_gyros_mtb_ext-eoas_inertial_accel_mtb_int] = i;
//                        eo_common_hlfword_bitset(&p->canmap_mtb_gyros_ext[des->on.can.port], des->on.can.addr);                        
//                    } break; 
                    
                    default:
                    {
                    } break;
                }
                
            }
            else if(eobrd_place_eth == des->on.any.place)
            {           
                uint8_t n = (eoas_inertial3_gyros_ems_st_l3g4200d == des->typeofsensor) ? (inertials3_mems_gyro) : (inertials3_mems_accel);  

                if(inertials3_mems_gyro == n)
                {
                    eo_common_byte_bitset(&p->ethmap_mems_active, n);                
                    p->frommems2id[n] = i;  
                    p->memsparam[n] = hal_gyroscope_range_500dps;     
                    if((des->on.eth.id > 0) && (des->on.eth.id < 4))
                    {   // hack to change the range
                        p->memsparam[n] = (hal_gyroscope_range_t) ((uint8_t)(des->on.eth.id)-1);
                    }
                    p->memsconfig[n].acquisitionrate = p->sensorsconfig.datarate * EOK_reltime1ms;
                    p->memsconfig[n].sensor = mems_gyroscope_l3g4200;
                    p->memsconfig[n].properties.gyroscope.range = (hal_gyroscope_range_t)p->memsparam[n];
                }
                else 
                {
                    // ethmap_mems_active stays unset, hence EOtheMEMS will not be started 
                }
            }
        }
    }    
    
}


// it is the last discovery of can boards... 
static eOresult_t s_eo_inertials3_onstop_search4canboards(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok)
{
    const eOmn_serv_configuration_t * servcfg = (const eOmn_serv_configuration_t *)par;
    EOtheInertials3 *p = &s_eo_theinertials3;
    
    if(eobool_true == searchisok)
    {
        p->service.state = eomn_serv_state_verified;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials3, p->service.state);
    }
    else
    {   
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials3, p->service.state);
    }
    
    if((eobool_true == searchisok) && (eobool_true == p->service.activateafterverify))
    {
        eo_inertials3_Activate(p, servcfg);        
    }

    p->diagnostics.errorDescriptor.sourcedevice      = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.sourceaddress     = 0;
    p->diagnostics.errorDescriptor.par16             = 0;
    p->diagnostics.errorDescriptor.par64             = 0;
   
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, s_eo_inertials3_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));

    if(eobool_true == searchisok)
    {        
        p->diagnostics.errorType = eo_errortype_debug;
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials3_ok);
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if((0 != p->diagnostics.repetitionOKcase) && (0 != p->diagnostics.reportPeriod))
        {
            p->diagnostics.errorCallbackCount = p->diagnostics.repetitionOKcase;        
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }
    
    
    if(eobool_false == searchisok)
    { 
        const eOcandiscovery_target_t *ptrgt = (const eOcandiscovery_target_t*) eo_array_At(p->sharedcan.discoverytargets, 0);
        
        if(NULL == ptrgt)
        {
            return eores_NOK_generic;
        }        
        // in case we fail the discovery, it is better to put inside par64 the failure of discovery.
        // maskofmissing, maskofwronglydetected, 
        
        uint16_t totalcanmap[2] = {0};        
        for(uint8_t i=0; i<eo_array_Size(p->sharedcan.discoverytargets); i++)
        {
            const eOcandiscovery_target_t *ptrgt = (const eOcandiscovery_target_t*) eo_array_At(p->sharedcan.discoverytargets, i);
            if(NULL != ptrgt)
            {
                totalcanmap[0] |= ptrgt->canmap[0];
                totalcanmap[1] |= ptrgt->canmap[1];
            }            
        }
        
        const eOcandiscovery_detection_t* detection = eo_candiscovery2_GetDetection(eo_candiscovery2_GetHandle());
                
        uint16_t maskofmissingCAN1 = totalcanmap[0] & (~detection->replies[0]);
        uint16_t maskofmissingCAN2 = totalcanmap[1] & (~detection->replies[1]);
        uint16_t maskofincompatibleCAN1 = detection->incompatibilities[0]; 
        uint16_t maskofincompatibleCAN2 = detection->incompatibilities[1]; 
        
        p->diagnostics.errorDescriptor.par64 = ((uint64_t)maskofmissingCAN1) | ((uint64_t)maskofmissingCAN2<<16) | ((uint64_t)maskofincompatibleCAN1<<32) | ((uint64_t)maskofincompatibleCAN2<<32);
        
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials3_failed_candiscovery);
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
        p->service.onverify(p, searchisok); 
    }    
    
    return(eores_OK);
    
}


static void s_eo_inertials3_send_periodic_error_report(void *par)
{
    EOtheInertials3* p = (EOtheInertials3*) par;
    
    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);

    if(eoerror_value_CFG_inertials3_failed_candiscovery == eoerror_code2value(p->diagnostics.errorDescriptor.code))
    {   // if i dont find some boards, i keep on sending the discovery results up. it is a temporary diagnostics tricks until we use the verification of services at bootstrap
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

static eObool_t s_eo_inertials3_isID32relevant(uint32_t id32)
{
    static const uint32_t mask0 = (((uint32_t)eoprot_endpoint_analogsensors) << 24) | (((uint32_t)eoprot_entity_as_inertial3) << 16);
    
    if((id32 & mask0) == mask0)
    {
        return(eobool_true);
    }
    
    return(eobool_false); 
}

static eObool_t s_eo_inertials3_get_id(eObrd_canlocation_t loc, eOas_inertial3_type_t type, uint8_t *id)
{
    // EOtheInertials3* p = &s_eo_theinertials3;
    
    eObool_t ret = eobool_true;
    
    switch(type)
    {
        case eoas_inertial3_accel_mtb_int:
        case eoas_inertial3_accel_mtb_ext:
        case eoas_inertial3_gyros_mtb_ext:
        case eoas_inertial3_imu_acc:     
        case eoas_inertial3_imu_mag:
        case eoas_inertial3_imu_gyr:
        case eoas_inertial3_imu_eul:
        case eoas_inertial3_imu_qua:
        case eoas_inertial3_imu_lia:
        case eoas_inertial3_imu_grv:
        case eoas_inertial3_imu_status:
        {
            ret = eobool_true;            
        } break;
        
        default:
        {
            ret = eobool_false;
        } break;
    }
    
    if(eobool_false == ret)
    {
        return ret;
    }
    
    // marco.accame on 5 april 2018: 
    // with the new imu we can have many types of sensors on the same board. it becomes heavy to keep a map from (loc, type) to identifier.
    // if the identifier is 1 byte with the position inside the arrayofsensordescriptors and the number of possible inertial sensor is
    // reduced to 4 (acc, gyr, lin, status) then we need 2*16*4 = 128 bytes for such a map.... 
    // HENCE: the original idea of sending up an id = the order of the arrayofsensordescriptors as sent by pc104 is not feasible.
    // for now i just send up 8 bits with two nibbles [canx, addr] and i build the map inside the pc104.
    
//    #warning TODO: if we save some ram by removing the double arrayofsensordescriptors, then we may add a map of 128 bytes.

    *id = (loc.port << 4) | loc.addr;
    
#if 0    
    uint8_t *candidates = p->fromcan2id[loc.port][loc.addr];

    switch(type)
    {
        case eoas_inertial3_accel_mtb_int:
        {
            *id = candidates[0];
        } break;
        case eoas_inertial3_accel_mtb_ext:
        {
            *id = candidates[1];
        } break;
        case eoas_inertial3_gyros_mtb_ext:
        {
            *id = candidates[2];
        } break;
        case eoas_inertial3_imu_acc:
        {
            *id = candidates[0];
        } break;            

        case eoas_inertial3_imu_gyr:
        {
            *id = candidates[1];
        } break;
        //case eoas_inertial3_imu_eul:
        //case eoas_inertial3_imu_qua:
        case eoas_inertial3_imu_lia:
        {
            *id = candidates[2];
        } break;
        //case eoas_inertial3_imu_grv:
        case eoas_inertial3_imu_status:
        {
            *id = candidates[3];
        } break;
        
        default:
        {
            *id = NOID08;
        }
    }

#endif


    if(NOID08 == *id)
    {
        return(eobool_false);
    }
        
    return(ret);
}


static void s_eo_inertials3_presenceofcanboards_reset(EOtheInertials3 *p)
{
    memset(p->not_heardof_target, 0, sizeof(p->not_heardof_target));
    memset(p->not_heardof_status, 0, sizeof(p->not_heardof_status));
    p->not_heardof_counter = 0;
}

static void s_eo_inertials3_presenceofcanboards_build(EOtheInertials3 *p)
{
    s_eo_inertials3_presenceofcanboards_reset(p);

    // now i must build the canmaps used to monitor presence 
    uint8_t numofsensors = eo_array_Size(p->arrayofsensordescriptors);   
    
    p->not_heardof_target[eOcanport1] = p->not_heardof_target[eOcanport2] = 0;

    for(uint8_t i=0; i<numofsensors; i++)
    {
        eOas_inertial3_descriptor_t *des = (eOas_inertial3_descriptor_t*) eo_array_At(p->arrayofsensordescriptors, i);
        if(NULL != des)
        {
            if(eobrd_place_can == des->on.any.place)
            {
                eo_common_hlfword_bitset(&p->not_heardof_target[des->on.can.port], des->on.can.addr);  
            }
        }
    }    
}


static void s_eo_inertials3_presenceofcanboards_start(EOtheInertials3 *p)
{
    // prepare not_heardof_target ... it is equal to canmap_brd_active in or with those
    // boards which are searched in discovery
    
//    if(0 == (p->sharedcan.discoverytarget.info.protocol.major + p->sharedcan.discoverytarget.info.protocol.minor +
//             p->sharedcan.discoverytarget.info.firmware.major + p->sharedcan.discoverytarget.info.firmware.minor + p->sharedcan.discoverytarget.info.firmware.build))
//    {
//        memset(p->not_heardof_target, 0, sizeof(p->not_heardof_status));
//    }
//    else
//    {
        memcpy(p->not_heardof_target, p->canmap_brd_active, sizeof(p->not_heardof_status));
//    }
//    
    memcpy(p->not_heardof_status, p->not_heardof_target, sizeof(p->not_heardof_status));
    p->not_heardof_counter = 0;
}


static void s_eo_inertials3_presenceofcanboards_touch(EOtheInertials3 *p, eObrd_canlocation_t loc)
{
    eo_common_hlfword_bitclear(&p->not_heardof_status[loc.port], loc.addr);
}


static void s_eo_inertials3_presenceofcanboards_tick(EOtheInertials3 *p)
{
    if((p->not_heardof_counter++) > 1000)
    {        
        if((0 != p->not_heardof_status[0]) || (0 != p->not_heardof_status[1]))
        {
            eOerrmanDescriptor_t errdes = {0};
            errdes.sourcedevice       = eo_errman_sourcedevice_localboard;
            errdes.sourceaddress      = 0;
            errdes.par16              = eobrd_unknown;
            errdes.par64              = (p->not_heardof_status[0] << 16) | p->not_heardof_status[1];        
            errdes.code = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_boards_lostcontact);
            p->diagnostics.errorType = eo_errortype_warning;                
            eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &errdes);
        } 

        memcpy(p->not_heardof_status, p->not_heardof_target, sizeof(p->not_heardof_status));
        p->not_heardof_counter = 0;    
    }
}

static void s_eo_inertials3_imu_transmission(EOtheInertials3 *p, eObool_t on, uint8_t period)
{

    icubCanProto_imu_transmit_t transmit = {0};
    transmit.period = (eobool_true == on) ? (period) : (0);
    
    p->sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;
    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__IMU_TRANSMIT;
    p->sharedcan.command.value = &transmit;
    
    eObrd_canlocation_t location = {0};
    location.insideindex = eobrd_caninsideindex_none;
    
    for(uint8_t port=0; port<2; port++)
    {
        location.port = port;
        for(uint8_t addr=1; addr<15; addr++)
        {            
            if(eobool_true == eo_common_hlfword_bitcheck(p->canmap_brd_active[port], addr))
            {                                             
                location.addr = addr;
                eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &p->sharedcan.command, location);
            }
        }
    }    
}

static void s_eo_inertials3_imu_configure(EOtheInertials3 *p)
{
    icubCanProto_imu_config_t imuconfig = {0};
    imuconfig.fusionmode = 1;
    
    p->sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;
    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__IMU_CONFIG_SET;
    p->sharedcan.command.value = &imuconfig;
    
    eObrd_canlocation_t location = {0};
    location.insideindex = eobrd_caninsideindex_none;
    uint8_t numofsensors = eo_array_Size(p->arrayofsensordescriptors);
    for(uint8_t port=0; port<2; port++)
    {
        location.port = port;
        for(uint8_t addr=1; addr<15; addr++)
        {            
            if(eobool_true == eo_common_hlfword_bitcheck(p->canmap_brd_active[port], addr))
            {                                             
                location.addr = addr;
                // and now i fill enabledsensors and i send the message
                imuconfig.enabledsensors = 0; 
                //we want alway read the status
                eo_common_hlfword_bitset(&imuconfig.enabledsensors, icubCanProto_imu_status);
                
                for(uint8_t i=0; i<numofsensors; i++)
                {
                    eOas_inertial3_descriptor_t *des = (eOas_inertial3_descriptor_t*) eo_array_At(p->arrayofsensordescriptors, i);
                    if(NULL != des)
                    {
                        if((des->on.any.place == eobrd_place_can) && (des->on.can.port == port) && (des->on.can.addr == addr))
                        {
                            // ok, i have the board. i must enable the sensors 
                            icubCanProto_imu_sensor_t ps = eoas_inertial3_imu_to_canproto((eOas_inertial3_type_t)des->typeofsensor);
                            if(icubCanProto_imu_none != ps)
                            {
                                eo_common_hlfword_bitset(&imuconfig.enabledsensors, ps);
                            }
                        }
                    }
                }
                eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &p->sharedcan.command, location);
            }
        }
    }   
}


#endif // #elif !defined(EOTHESERVICES_disable_theInertials3)


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



