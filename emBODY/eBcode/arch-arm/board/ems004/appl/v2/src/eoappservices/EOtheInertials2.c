/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
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

#include "EOtheInertials2.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheInertials2_hid.h"

#if defined(EOTHESERVICES_disable_theInertials2)

    // provide empty implementation, so that we dont need to change the caller of the API
    
    extern EOtheInertials2* eo_inertials2_Initialise(void) 
    {   
        return NULL; 
    }

    extern EOtheInertials2* eo_inertials2_GetHandle(void)   
    { 
        return NULL; 
    }

    extern eOmn_serv_state_t eo_inertials2_GetServiceState(EOtheInertials2 *p) 
    { 
        return eomn_serv_state_notsupported; 
    }
    
    // in some cases, we need to alert the pc104 that the board does not support this service
    extern eOresult_t eo_inertials2_SendReport(EOtheInertials2 *p)
    {
        static const char s_eobj_ownname[] = "EOtheInertials2";
        eOerrmanDescriptor_t errdes = {};
        errdes.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials_failed_notsupported);  
        errdes.sourcedevice = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress = 0;
        errdes.par16 = errdes.par64 = 0;
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);
        return eores_OK;
    }


    extern eOresult_t eo_inertials2_Verify(EOtheInertials2 *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify)
    {
        // we alert the host that the verification of the service has failed
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials, eomn_serv_state_failureofverify);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        } 
        
        // we tell that the reason is that this service is not supported
        eo_inertials2_SendReport(NULL);
               
        return eores_NOK_generic;
    }

    extern eOresult_t eo_inertials2_Activate(EOtheInertials2 *p, const eOmn_serv_configuration_t * servcfg)
    {
        eo_inertials2_SendReport(NULL);
        return eores_NOK_generic;
    }

    extern eOresult_t eo_inertials2_Deactivate(EOtheInertials2 *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_inertials2_Start(EOtheInertials2 *p)
    {
        eo_inertials2_SendReport(NULL);
        return eores_NOK_generic;
    }

    extern eOresult_t eo_inertials2_SetRegulars(EOtheInertials2 *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
    {
        if(NULL != arrayofid32)
        {
            eo_inertials2_SendReport(NULL);
        }
        return eores_NOK_generic;
    }

    extern eOresult_t eo_inertials2_Tick(EOtheInertials2 *p, eObool_t resetstatus)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_inertials2_Stop(EOtheInertials2 *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_inertials2_Transmission(EOtheInertials2 *p, eObool_t on)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_inertials2_Config(EOtheInertials2 *p, eOas_inertial_config_t* config)
    {
        eo_inertials2_SendReport(NULL);
        return eores_NOK_generic;
    }
    
    extern eOresult_t eo_inertials2_AcceptCANframe(EOtheInertials2 *p, eOcanframe_t *frame, eOcanport_t port, eOas_inertial_type_t type)
    {
        return eores_NOK_generic;
    }


#elif !defined(EOTHESERVICES_disable_theInertials2)


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

static eOresult_t s_eo_inertials2_verify_local_sensors(EOtheInertials2 *p, const eOmn_serv_configuration_t * servcfg);

static eOresult_t s_eo_inertials2_TXstart(EOtheInertials2 *p);

static eOresult_t s_eo_inertials2_TXstop(EOtheInertials2 *p);

static eObool_t s_eo_inertials2_activestate_can_accept_canframe(void);

static void s_eo_inertials2_build_maps(EOtheInertials2* p, uint64_t enablemask);

static eOresult_t s_eo_inertials2_onstop_search4mtbs(void *par, EOtheCANdiscovery2* p, eObool_t searchisok);

static void s_eo_inertials2_send_periodic_error_report(void *p);

static eObool_t s_eo_inertials2_isID32relevant(uint32_t id32);

static eObool_t s_eo_inertials2_get_id(eObrd_canlocation_t loc, eOas_inertial_type_t type, uint16_t *id);

static void s_eo_inertials2_presenceofcanboards_reset(EOtheInertials2 *p);
static void s_eo_inertials2_presenceofcanboards_start(EOtheInertials2 *p);
static void s_eo_inertials2_presenceofcanboards_touch(EOtheInertials2 *p, eObrd_canlocation_t loc);
static void s_eo_inertials2_presenceofcanboards_tick(EOtheInertials2 *p);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const eOas_inertial_config_t s_eo_default_inertialconfig =
{
    EO_INIT(.datarate)              50,
    EO_INIT(.filler)                {0},
    EO_INIT(.enabled)               0
};



static EOtheInertials2 s_eo_theinertials2 = 
{
    EO_INIT(.service) 
    {
        EO_INIT(.initted)               eobool_false,
        EO_INIT(.active)                eobool_false,
        EO_INIT(.activateafterverify)   eobool_false,
        EO_INIT(.started)               eobool_false,
        EO_INIT(.onverify)              NULL,
        EO_INIT(.state)                 eomn_serv_state_notsupported,
        EO_INIT(.tmpcfg)                NULL,
        EO_INIT(.servconfig)            { EO_INIT(.type) eomn_serv_NONE },
    },
    EO_INIT(.diagnostics) 
    {
        EO_INIT(.reportTimer)           NULL,
        EO_INIT(.reportPeriod)          0, // 10*EOK_reltime1sec, // with 0 we dont periodically report
        EO_INIT(.errorDescriptor)       {0},
        EO_INIT(.errorType)             eo_errortype_info,
        EO_INIT(.errorCallbackCount)    0, 
        EO_INIT(.repetitionOKcase)      0 // 10 // with 0 we transmit report only once at succesful activation
    },     
    EO_INIT(.sharedcan)
    {
        EO_INIT(.boardproperties)       NULL,
        EO_INIT(.entitydescriptor)      NULL,
        EO_INIT(.discoverytargets)      NULL,
        EO_INIT(.ondiscoverystop)       {0},
        EO_INIT(.command)               {0}, 
    },   
    
    EO_INIT(.numofmtbs)                 0,

    EO_INIT(.sensorsconfig)             {0},  
    EO_INIT(.fifoofinertialdata)        NULL,
    
    EO_INIT(.configured)                eobool_false,
    
    EO_INIT(.canmap_mtb_accel_int)      {0},
    EO_INIT(.canmap_mtb_accel_ext)      {0},
    EO_INIT(.canmap_mtb_gyros_ext)      {0},
    EO_INIT(.canmap_mtb_active)         {0},
    EO_INIT(.ethmap_mems_active)        0,
    
    EO_INIT(.fromcan2id)                {NOID16},
    EO_INIT(.frommems2id)               {NOID16},
    EO_INIT(.memsparam)                 {255},
    EO_INIT(.memsconfig)                { {mems_gyroscope_l3g4200, hal_gyroscope_range_250dps, 0} },  

    EO_INIT(.inertial2)                 NULL,
    EO_INIT(.id32ofregulars)            NULL,
    EO_INIT(.arrayofsensors)            NULL,
    EO_INIT(.not_heardof_target)        {0},
    EO_INIT(.not_heardof_status)        {0},
    EO_INIT(.not_heardof_counter)       0,
    EO_INIT(.transmissionisactive)      eobool_false
};



static const char s_eobj_ownname[] = "EOtheInertials2";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern EOtheInertials2* eo_inertials2_Initialise(void)
{
    EOtheInertials2* p = &s_eo_theinertials2;
    if(eobool_true == p->service.initted)
    {
        return(p);
    }
            
    p->numofmtbs = 0;
    p->service.servconfig.type = eomn_serv_NONE;
        
    p->sharedcan.boardproperties = eo_vector_New(sizeof(eObrd_canproperties_t), eo_inertials2_maxnumberofMTBboards, NULL, NULL, NULL, NULL);
    
    p->sharedcan.entitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), eo_inertials2_maxnumberofMTBboards, NULL, NULL, NULL, NULL);
    
    p->sharedcan.discoverytargets = eo_array_New(1, sizeof(eOcandiscovery_target_t),  NULL);
    
    p->canmap_mtb_accel_int[0] = p->canmap_mtb_accel_int[1] = 0;
    p->canmap_mtb_accel_ext[0] = p->canmap_mtb_accel_ext[1] = 0;
    p->canmap_mtb_gyros_ext[0] = p->canmap_mtb_gyros_ext[1] = 0;
    p->canmap_mtb_active[0]    = p->canmap_mtb_active[1]    = 0;
    p->ethmap_mems_active = 0;
    memset(p->fromcan2id, NOID08, sizeof(p->fromcan2id));       
    memset(p->frommems2id, NOID08, sizeof(p->frommems2id));     
    memset(p->memsparam, 255, sizeof(p->memsparam));
    
    s_eo_inertials2_presenceofcanboards_reset(p);
    
    p->inertial2 = NULL;
    p->id32ofregulars = eo_array_New(inertials_maxRegulars, sizeof(uint32_t), NULL);
    p->arrayofsensors = eo_array_New(eOas_inertials_maxnumber, sizeof(eOas_inertial_descriptor_t), NULL);
    
    memcpy(&p->sensorsconfig, &s_eo_default_inertialconfig, sizeof(eOas_inertial_config_t));
    p->fifoofinertialdata = eo_vector_New(sizeof(eOas_inertial_data_t), 32, NULL, 0, NULL, NULL);
    
    eo_mems_Initialise(NULL);
    
    p->diagnostics.reportTimer = eo_timer_New();
    p->diagnostics.errorType = eo_errortype_error;
    p->diagnostics.errorDescriptor.sourceaddress = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials_not_verified_yet);  
    
    p->service.initted = eobool_true;    
    p->service.active = eobool_false;
    p->service.started = eobool_false;
    p->transmissionisactive = eobool_false;
    p->service.state = eomn_serv_state_idle;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials, p->service.state);
    
    return(p);   
}


extern EOtheInertials2* eo_inertials2_GetHandle(void)
{
    if(eobool_true == s_eo_theinertials2.service.initted)
    {
        return(&s_eo_theinertials2);
    }
    
    return(NULL);
}


extern eOmn_serv_state_t eo_inertials2_GetServiceState(EOtheInertials2 *p)
{
    if(NULL == p)
    {
        return(eomn_serv_state_notsupported);
    } 

    return(p->service.state);
}


extern eOresult_t eo_inertials2_SendReport(EOtheInertials2 *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
    
    eOerror_value_t errorvalue = eoerror_code2value(p->diagnostics.errorDescriptor.code);
    
    switch(errorvalue)
    {
        case eoerror_value_CFG_inertials_failed_candiscovery:
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


extern eOresult_t eo_inertials2_Verify(EOtheInertials2 *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify)
{
    eOresult_t res = eores_NOK_generic;
    
    if((NULL == p) || (NULL == servcfg))
    {
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials, p->service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }        
        return(eores_NOK_nullpointer);
    } 

    if(eomn_serv_AS_inertials != servcfg->type)
    {
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials, p->service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }        
        return(eores_NOK_generic);
    }   
    
 
    if(eobool_true == p->service.active)
    {
        eo_inertials2_Deactivate(p);        
    }   
    
    p->service.state = eomn_serv_state_verifying; 
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials, p->service.state);    

    // make sure the timer is not running
    eo_timer_Stop(p->diagnostics.reportTimer);    

    p->service.onverify = onverify;
    p->service.activateafterverify = activateafterverify;
    
    // i get all the sensors.
    memcpy(p->arrayofsensors, &servcfg->data.as.inertial.arrayofsensors, sizeof(eOas_inertial_arrayof_sensors_t));
    
    
    // at first we verify sensors which are local (if any)
    
    if(eores_OK != (res = s_eo_inertials2_verify_local_sensors(p, servcfg)))
    {        
        return res;        
    }
    
    eOcandiscovery_target_t trgt = {0};

    // then ... those on can
    trgt.info.type = eobrd_cantype_mtb;
    trgt.info.protocol.major = servcfg->data.as.inertial.mtbversion.protocol.major; 
    trgt.info.protocol.minor = servcfg->data.as.inertial.mtbversion.protocol.minor;
    trgt.info.firmware.major = servcfg->data.as.inertial.mtbversion.firmware.major; 
    trgt.info.firmware.minor = servcfg->data.as.inertial.mtbversion.firmware.minor;
    trgt.info.firmware.build = servcfg->data.as.inertial.mtbversion.firmware.build;   
    
    
    // now i must build the canmaps ... but only for discovery
    uint8_t numofsensors = eo_array_Size(p->arrayofsensors);    
    trgt.canmap[eOcanport1] =  trgt.canmap[eOcanport2] = 0;
    p->numofmtbs = 0;
    for(uint8_t i=0; i<numofsensors; i++)
    {
        eOas_inertial_descriptor_t *des = (eOas_inertial_descriptor_t*) eo_array_At(p->arrayofsensors, i);
        if(NULL != des)
        {
            if(eobrd_place_can == des->on.any.place)
            {
                eo_common_hlfword_bitset(&trgt.canmap[des->on.can.port], des->on.can.addr);  
            }
        }
    }
    
    uint8_t numofboards = eo_common_hlfword_bitsetcount(trgt.canmap[eOcanport1]) +
                          eo_common_hlfword_bitsetcount(trgt.canmap[eOcanport2]);
    
    if(numofboards > eo_inertials2_maxnumberofMTBboards)
    {
        
        p->diagnostics.errorDescriptor.sourcedevice       = eo_errman_sourcedevice_localboard;
        p->diagnostics.errorDescriptor.sourceaddress      = 0;
        p->diagnostics.errorDescriptor.par16              = (numofboards << 8) | (eo_inertials2_maxnumberofMTBboards & 0x00ff);
        p->diagnostics.errorDescriptor.par64              = (trgt.canmap[eOcanport2] << 16) | (trgt.canmap[eOcanport1]);
       
        EOaction_strg astrg = {0};
        EOaction *act = (EOaction*)&astrg;
        eo_action_SetCallback(act, s_eo_inertials2_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));        
        
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials_failed_toomanyboards);
        p->diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if(0 != p->diagnostics.reportPeriod)
        {
            p->diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);   
        }  
        
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials, p->service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }    
        
        return(eores_NOK_generic); 
    }    
             
    // force a cleaned discoverytargets before we add the target
    eo_array_Reset(p->sharedcan.discoverytargets);    
    eo_array_PushBack(p->sharedcan.discoverytargets, &trgt);              
    p->sharedcan.ondiscoverystop.function = s_eo_inertials2_onstop_search4mtbs;
    p->sharedcan.ondiscoverystop.parameter = (void*)servcfg;
    
    // start discovery of can boards  
    eo_candiscovery2_Start2(eo_candiscovery2_GetHandle(), p->sharedcan.discoverytargets, &p->sharedcan.ondiscoverystop);   
   
    return(eores_OK);   
}


extern eOresult_t eo_inertials2_Deactivate(EOtheInertials2 *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == p->service.active)
    {
        // i force to eomn_serv_state_idle because it may be that state was eomn_serv_state_verified or eomn_serv_state_failureofverify
        p->service.state = eomn_serv_state_idle; 
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials, p->service.state);
        return(eores_OK);        
    } 
    
    if(eobool_true == p->service.started)
    {
        eo_inertials2_Stop(p);
    }
    
    eo_inertials2_SetRegulars(p, NULL, NULL);
      
    eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_inertial, p->sharedcan.entitydescriptor); 
    
    eo_canmap_UnloadBoards(eo_canmap_GetHandle(), p->sharedcan.boardproperties);
     
    
    eo_entities_SetNumOfInertials(eo_entities_GetHandle(), 0);
    
    p->numofmtbs = 0;
    
    p->inertial2 = NULL;
    
    eo_array_Reset(p->arrayofsensors);
    
    eo_vector_Clear(p->fifoofinertialdata);
                
    memset(&p->service.servconfig, 0, sizeof(eOmn_serv_configuration_t));
    p->service.servconfig.type = eomn_serv_NONE;
    
    eo_vector_Clear(p->sharedcan.boardproperties);
    eo_vector_Clear(p->sharedcan.entitydescriptor);
    eo_array_Reset(p->sharedcan.discoverytargets);
    
    // make sure the timer is not running
    eo_timer_Stop(p->diagnostics.reportTimer);  
    
    p->sensorsconfig.enabled = 0;
    p->configured = eobool_false;
    p->service.active = eobool_false;    
    p->service.state = eomn_serv_state_idle;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials, p->service.state);
    
    return(eores_OK);
}


extern eOresult_t eo_inertials2_Activate(EOtheInertials2 *p, const eOmn_serv_configuration_t * servcfg)
{
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    } 

    if(eomn_serv_AS_inertials != servcfg->type)
    {
        return(eores_NOK_generic);
    }
    
    if(eobool_true == p->service.active)
    {
        eo_inertials2_Deactivate(p);        
    }   
    
    eo_entities_SetNumOfInertials(eo_entities_GetHandle(), 1);
    
    if(0 == eo_entities_NumOfInertials(eo_entities_GetHandle()))
    {
        p->service.active = eobool_false;
        return(eores_NOK_generic);
    }
    
       
    memcpy(&p->service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));


    p->inertial2 = eo_entities_GetInertial(eo_entities_GetHandle(), 0);
    
    
    // now i must add all the mtb boards. i iterate per canbus
    
    eObrd_canproperties_t prop = {0};
    prop.type = eobrd_cantype_mtb;
    prop.location.port = 0; prop.location.addr = 0; prop.location.insideindex = eobrd_caninsideindex_none;
    prop.requiredprotocol.major = servcfg->data.as.inertial.mtbversion.protocol.major;
    prop.requiredprotocol.minor = servcfg->data.as.inertial.mtbversion.protocol.minor;

    
    eOcanmap_entitydescriptor_t des = {0};
    des.location.port = 0; des.location.addr = 0; des.location.insideindex = eobrd_caninsideindex_none;
    des.index = entindex00;
    
    
    // now i get all the sensors.
    memcpy(p->arrayofsensors, &servcfg->data.as.inertial.arrayofsensors, sizeof(eOas_inertial_arrayof_sensors_t));
    
    // now i must build the canbusmapping[]
    uint8_t numofsensors = eo_array_Size(p->arrayofsensors);    
    p->numofmtbs = 0;
    uint16_t canbusmapping[2] = {0};
    for(uint8_t i=0; i<numofsensors; i++)
    {
        eOas_inertial_descriptor_t *des = (eOas_inertial_descriptor_t*) eo_array_At(p->arrayofsensors, i);
        if(NULL != des)
        {
            if(eobrd_place_can == des->on.any.place)
            {
                eo_common_hlfword_bitset(&canbusmapping[des->on.can.port], des->on.can.addr);  
            }
        }
    }    
    
    uint8_t j = 0;
    uint8_t k = 0;

    for(j=0; j<eOcanports_number; j++)
    {
        for(k=1; k<15; k++)
        {
            if(eobool_true == eo_common_hlfword_bitcheck(canbusmapping[j], k))
            {   // i pushback. i dont verify vs the capacity of the vector because eo_inertials2_Verify() has already done it
                prop.location.port = j;
                prop.location.addr = k;
                eo_vector_PushBack(p->sharedcan.boardproperties, &prop);
                
                des.location.port = j;
                des.location.addr = k;
                des.index = entindex00;                        
                eo_vector_PushBack(p->sharedcan.entitydescriptor, &des);
                
                p->numofmtbs++;
            }
        }
    }

    
    // load the can mapping 
    eo_canmap_LoadBoards(eo_canmap_GetHandle(), p->sharedcan.boardproperties); 
    
    // load the entity mapping.
    eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_inertial, p->sharedcan.entitydescriptor);   
    
    
    // build the maps that can translate a received can message into the id to be put into ...
    // we assume that 
    s_eo_inertials2_build_maps(p, 0xffffffffffffffff);
    p->configured = eobool_false; // still false
    p->service.active = eobool_true;    
    p->service.state = eomn_serv_state_activated;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials, p->service.state);    

    
    return(eores_OK);   
}



extern eOresult_t eo_inertials2_Start(EOtheInertials2 *p)
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
//        eo_inertials2_Stop(p);
//    } 
    

    // ok, now we do something.    
         
    //s_eo_inertials2_TXstart(p);

    p->service.started = eobool_true;    
    p->service.state = eomn_serv_state_started;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials, p->service.state);
    
    
    return(eores_OK);      
}

extern eOresult_t eo_inertials2_SetRegulars(EOtheInertials2 *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
    
    return(eo_service_hid_SetRegulars(p->id32ofregulars, arrayofid32, s_eo_inertials2_isID32relevant, numberofthem));
}


extern eOresult_t eo_inertials2_Stop(EOtheInertials2 *p)
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
      
    s_eo_inertials2_TXstop(p);
    
               
    p->service.started = eobool_false;
    p->service.state = eomn_serv_state_activated;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials, p->service.state);
    
    // reset the various buffers
    eOas_inertial_data_t *data = &p->inertial2->status.data;
    memset(data, 0, sizeof(eOas_inertial_data_t)); 
    eo_vector_Clear(p->fifoofinertialdata);
    
    
    // remove all regulars related to inertials entity ... no, dont do that.
    //eo_inertials2_SetRegulars(p, NULL, NULL);
    
    return(eores_OK);     
}


extern eOresult_t eo_inertials2_Transmission(EOtheInertials2 *p, eObool_t on)
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
        s_eo_inertials2_TXstart(p);
    }
    else
    {
        s_eo_inertials2_TXstop(p);
    }

    return(eores_OK);
}


extern eOresult_t eo_inertials2_Tick(EOtheInertials2 *p, eObool_t resetstatus)
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
        s_eo_inertials2_presenceofcanboards_tick(p);
    }

    if(eobool_false == resetstatus)
    {   // nothing to do because we cannot overwrite the status of inertial 
        return(eores_OK);
    }       
    
    // i can update the status of the inertial because the old value has already been transmitted.
        
    // the status data of the inertial entity is in ...
    eOas_inertial_data_t *data = &p->inertial2->status.data;
    memset(data, 0, sizeof(eOas_inertial_data_t)); 
    
    eOmems_sensor_t sensor = mems_gyroscope_l3g4200;
    // if we have a mems, then if we have a fifoofinertialdata, then NOID16
    if(eores_OK == eo_mems_Get(eo_mems_GetHandle(), data, eok_reltimeZERO, &sensor, NULL))
    {
        //eo_errman_Trace(eo_errman_GetHandle(), "tx mems", s_eobj_ownname);
        // ok, i adjust the id
        uint8_t index = (mems_gyroscope_l3g4200 == sensor) ? (mems_gyro) : (mems_accel);
        data->id = p->frommems2id[index];
    }
    else if(eobool_false == eo_vector_Empty(p->fifoofinertialdata))
    {
        eOas_inertial_data_t * item = (eOas_inertial_data_t*) eo_vector_Front(p->fifoofinertialdata);
        if(NULL != item)
        {
            memcpy(data, item, sizeof(eOas_inertial_data_t));   
            eo_vector_PopFront(p->fifoofinertialdata);   
            //eo_errman_Trace(eo_errman_GetHandle(), "tx mtb", s_eobj_ownname);
        }        
    }
    else
    {
        // just reset the status->data 
        data->id = NOID16;
        data->timestamp = eov_sys_LifeTimeGet(eov_sys_GetHandle());          
    }
                 
    return(eores_OK);        
}



extern eOresult_t eo_inertials2_Config(EOtheInertials2 *p, eOas_inertial_config_t* config)
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
        errdes.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials_changed_requestedrate);
        p->diagnostics.errorType = eo_errortype_warning;                
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &errdes);        
    }
        
    
    // now ... we need to change the masks according to p->sensorsconfig.maskofenabled
    
    s_eo_inertials2_build_maps(p, p->sensorsconfig.enabled);
    
    
    p->configured = eobool_true;    
 
    // ok guys: now we transmit the configuration to each one of the sensors which have a non-zero bit inside teh p->config2.enabled mask.
    // WAIT A MINUTE: we do it inside eo_inertials2_Start() !!!!!!!!!!!!

    
    return(eores_OK);           
}


extern eOresult_t eo_inertials2_AcceptCANframe(EOtheInertials2 *p, eOcanframe_t *frame, eOcanport_t port, eOas_inertial_type_t type)
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
    
    if(eobool_false == s_eo_inertials2_activestate_can_accept_canframe())
    {
        return(eores_OK);
    }
    
    // VERY IMPORTANT: the MTB can board does not tell whether we have an internal or external accel. it should do that but it does not.
    // hence, in type we use indifferently eoas_inertial_accel_mtb_int or eoas_inertial_accel_mtb_ext
    
    // the inertial entity is in ...
    //p->inertial2 = p->inertial2;    
    // however, we dont use it ...
    // we put what we have inside a fifo of eOas_inertial_data_t
    eOas_inertial_data_t data = {0};
       
    
    eObrd_canlocation_t loc = {0};    
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
    loc.insideindex = eobrd_caninsideindex_none;
    
    uint16_t id = NOID16;

    if(eobool_false == s_eo_inertials2_get_id(loc, type, &id))
    {
        // send up diagnostics
        // error -> "Object EOtheInertials2 has received a message from an unsupported inertial"
        return(eores_OK);        
    }
    
    if(eobool_true == eo_vector_Full(p->fifoofinertialdata))
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
        //eo_vector_PopFront(p->fifoofinertialdata);  
        // BUT .. as i dont want to througth data away, i decide that i put it inside the status.
        // in short: i call eo_inertials2_Tick(p, true) which already does that        
        eo_inertials2_Tick(p, eobool_true);      
    }
    
    data.timestamp =  eov_sys_LifeTimeGet(eov_sys_GetHandle());
    data.id = id;
    data.x = (int16_t)((frame->data[1]<<8) + frame->data[0]);
    data.y = (int16_t)((frame->data[3]<<8) + frame->data[2]);
    data.z = (int16_t)((frame->data[5]<<8) + frame->data[4]);
    
    // ok, now we can pushback data into the fifoofinertialdata
    
    eo_vector_PushBack(p->fifoofinertialdata, &data);
    
    s_eo_inertials2_presenceofcanboards_touch(p, loc);
    
    return(eores_OK);      
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// -- in here .... so that all things related to Inertial are in a unique place


extern void eoprot_fun_INIT_as_inertial_config(const EOnv* nv)
{
    eOas_inertial_config_t* config = (eOas_inertial_config_t*) eo_nv_RAM(nv);
    
    config->datarate = s_eo_default_inertialconfig.datarate;
    config->enabled = s_eo_default_inertialconfig.enabled;
}


extern void eoprot_fun_INIT_as_inertial_status(const EOnv* nv)
{
    eOas_inertial_status_t* status = (eOas_inertial_status_t*) eo_nv_RAM(nv);  
    
    status->data.timestamp = 0;
    status->data.id = 999;
    status->data.x = status->data.y = status->data.z = 0;
}


extern eObool_t eocanprotINperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_INERTIAL_MSG(eOcanframe_t *frame, eOcanport_t port)
{   
    EOtheInertials2 *p = &s_eo_theinertials2;  
    
    if(NULL == frame)
    {
        return(eobool_true);
    }    
    
    // marco.accame:        
    // i dont want to put data in the fifoofinertialdata when we are not in the control loop.
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

static eOresult_t s_eo_inertials2_verify_local_sensors(EOtheInertials2 *p, const eOmn_serv_configuration_t * servcfg)
{
    eOresult_t res = eores_OK;
    uint64_t errormask = 0; // at most i have eOas_inertials_maxnumber = 48 items
    uint8_t numoferrors = 0;
    uint8_t numofsensors = eo_array_Size(p->arrayofsensors);    
    for(uint8_t i=0; i<numofsensors; i++)
    {
        eOas_inertial_descriptor_t *des = (eOas_inertial_descriptor_t*) eo_array_At(p->arrayofsensors, i);
        if(NULL != des)
        {
            if(eobrd_place_eth == des->on.any.place)
            {
                // it can be ... eoas_inertial_gyros_ems_st_l3g4200d only
                if((eoas_inertial_gyros_ems_st_l3g4200d == des->type) && (eobool_true == eo_mems_IsSensorSupported(eo_mems_GetHandle(), mems_gyroscope_l3g4200)))
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
        eo_action_SetCallback(act, s_eo_inertials2_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));        
        
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials_failed_unsupportedsensor);
        p->diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if(0 != p->diagnostics.reportPeriod)
        {
            p->diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);   
        }  
        
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials, p->service.state);
        if(NULL != p->service.onverify)
        {
            p->service.onverify(p, eobool_false); 
        }    
             
    }
        
    return(res);
}


static eOresult_t s_eo_inertials2_TXstart(EOtheInertials2 *p)
{ 
    if(eobool_false == p->configured)
    {   // dont have a configured service
        return(eores_OK);
    }    
    
    if((0 == p->canmap_mtb_active[0]) && (0 == p->canmap_mtb_active[1]) && (0 == p->ethmap_mems_active))
    {   // no mtb boards or onboard sensors configured
        return(eores_OK);
    } 
    
    if(0 == p->sensorsconfig.enabled)
    {   // no mtb boards or local mems enabled
        return(eores_OK);
    } 
    
    if(eobool_true == eo_common_byte_bitcheck(p->ethmap_mems_active, mems_gyro))
    {
        eo_mems_Config(eo_mems_GetHandle(), &p->memsconfig[mems_gyro]);
        eo_mems_Start(eo_mems_GetHandle());
    }                        
 
    icubCanProto_inertial_config_t canprotoconfig = {0};
    
    canprotoconfig.period = p->sensorsconfig.datarate;
    canprotoconfig.enabledsensors = icubCanProto_inertial_sensorflag_none;
    
    p->sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;
    p->sharedcan.command.type  = ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP;
    p->sharedcan.command.value = &canprotoconfig;

    eObrd_canlocation_t location = {0};
    location.insideindex = eobrd_caninsideindex_none;
    for(uint8_t port=0; port<2; port++)
    {
        location.port = port;
        for(uint8_t addr=1; addr<15; addr++)
        {                        
            if(eobool_true == eo_common_hlfword_bitcheck(p->canmap_mtb_active[port], addr))
            {               
                canprotoconfig.enabledsensors = icubCanProto_inertial_sensorflag_none;
                // prepare canprotoconfig.enabledsensors ...
                if(eobool_true == eo_common_hlfword_bitcheck(p->canmap_mtb_accel_int[port], addr))
                {
                    canprotoconfig.enabledsensors |= icubCanProto_inertial_sensorflag_internaldigitalaccelerometer;
                }
                if(eobool_true == eo_common_hlfword_bitcheck(p->canmap_mtb_accel_ext[port], addr))
                {
                    canprotoconfig.enabledsensors |= icubCanProto_inertial_sensorflag_externaldigitalaccelerometer;
                }     
                if(eobool_true == eo_common_hlfword_bitcheck(p->canmap_mtb_gyros_ext[port], addr))
                {
                    canprotoconfig.enabledsensors |= icubCanProto_inertial_sensorflag_externaldigitalgyroscope;
                }                  
                location.addr = addr;
                eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &p->sharedcan.command, location);
            }
        }
    }
    
    p->transmissionisactive = eobool_true;
    s_eo_inertials2_presenceofcanboards_start(p);
    
    return(eores_OK);   
}


static eOresult_t s_eo_inertials2_TXstop(EOtheInertials2 *p)
{
    if(eobool_false == p->configured)
    {   // nothing to do because we dont have a configured service 
        return(eores_OK);
    }     

    icubCanProto_inertial_config_t canprotoconfig = {0};
    
    canprotoconfig.enabledsensors   = icubCanProto_inertial_sensorflag_none;
    canprotoconfig.period           = p->sensorsconfig.datarate;

    p->sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;
    p->sharedcan.command.type  = ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP;
    p->sharedcan.command.value = &canprotoconfig;
    
    
    eObrd_canlocation_t location = {0};
    location.insideindex = eobrd_caninsideindex_none;
    for(uint8_t port=0; port<2; port++)
    {            
        location.port = port;
        for(uint8_t addr=1; addr<15; addr++)
        {
            if(eobool_true == eo_common_hlfword_bitcheck(p->canmap_mtb_active[port], addr))
            {
                location.addr = addr;
                eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &p->sharedcan.command, location);
            }
        }
    }
    
    
    // stop the mems
    eo_mems_Stop(eo_mems_GetHandle());


    p->transmissionisactive = eobool_false;
    s_eo_inertials2_presenceofcanboards_reset(p);
               
    return(eores_OK);
}


//#define EOINERTIALS_ALWAYS_ACCEPT_CANFRAMES_IN_ACTIVE_MODE
static eObool_t s_eo_inertials2_activestate_can_accept_canframe(void)
{
#if     defined(EOINERTIALS_ALWAYS_ACCEPT_CANFRAMES_IN_ACTIVE_MODE)
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


static void s_eo_inertials2_build_maps(EOtheInertials2* p, uint64_t enablemask)
{
    uint8_t numofsensors = eo_array_Size(p->arrayofsensors);    
  
    // at first we disable all. 
    memset(p->fromcan2id, NOID08, sizeof(p->fromcan2id));
    memset(p->frommems2id, NOID08, sizeof(p->frommems2id));
    memset(p->memsparam, 255, sizeof(p->memsparam));
    
    memset(p->canmap_mtb_active, 0, sizeof(p->canmap_mtb_active));
    memset(p->canmap_mtb_accel_int, 0, sizeof(p->canmap_mtb_accel_int));
    memset(p->canmap_mtb_accel_ext, 0, sizeof(p->canmap_mtb_accel_ext));
    memset(p->canmap_mtb_gyros_ext, 0, sizeof(p->canmap_mtb_gyros_ext));
    p->ethmap_mems_active = 0;

    for(uint8_t i=0; i<numofsensors; i++)
    {
        eOas_inertial_descriptor_t *des = (eOas_inertial_descriptor_t*) eo_array_At(p->arrayofsensors, i);
        if(NULL != des)
        {
            eObool_t enabled = eo_common_dword_bitcheck(enablemask, i);
            
            if(eobool_false == enabled)
            {
                continue;
            }
            
            if(eobrd_place_can == des->on.any.place)
            {                
                eo_common_hlfword_bitset(&p->canmap_mtb_active[des->on.can.port], des->on.can.addr); 

                switch(des->type)
                {
                    case eoas_inertial_accel_mtb_int:
                    {
                        p->fromcan2id[des->on.can.port][des->on.can.addr][eoas_inertial_accel_mtb_int-eoas_inertial_accel_mtb_int] = i;
                        eo_common_hlfword_bitset(&p->canmap_mtb_accel_int[des->on.can.port], des->on.can.addr);                        
                    } break;

                    case eoas_inertial_accel_mtb_ext:
                    {       
                        p->fromcan2id[des->on.can.port][des->on.can.addr][eoas_inertial_accel_mtb_ext-eoas_inertial_accel_mtb_int] = i;
                        eo_common_hlfword_bitset(&p->canmap_mtb_accel_ext[des->on.can.port], des->on.can.addr);
                    } break;  
                    
                    case eoas_inertial_gyros_mtb_ext:
                    {                        
                        p->fromcan2id[des->on.can.port][des->on.can.addr][eoas_inertial_gyros_mtb_ext-eoas_inertial_accel_mtb_int] = i;
                        eo_common_hlfword_bitset(&p->canmap_mtb_gyros_ext[des->on.can.port], des->on.can.addr);                        
                    } break; 
                    
                    default:
                    {
                    } break;
                }
                
            }
            else if(eobrd_place_eth == des->on.any.place)
            {           
                uint8_t n = (eoas_inertial_gyros_ems_st_l3g4200d == des->type) ? (mems_gyro) : (mems_accel);  

                if(mems_gyro == n)
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


static eOresult_t s_eo_inertials2_onstop_search4mtbs(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok)
{
    const eOmn_serv_configuration_t * servcfg = (const eOmn_serv_configuration_t *)par;
    EOtheInertials2 *p = &s_eo_theinertials2;
    const eOcandiscovery_target_t *ptrgt = (const eOcandiscovery_target_t*) eo_array_At(p->sharedcan.discoverytargets, 0);
    
    if(NULL == ptrgt)
    {
        return eores_NOK_generic;
    }
    
    if(eobool_true == searchisok)
    {
        p->service.state = eomn_serv_state_verified;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials, p->service.state);
    }
    else
    {   
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_inertials, p->service.state);
    }
    
    if((eobool_true == searchisok) && (eobool_true == p->service.activateafterverify))
    {
        eo_inertials2_Activate(p, servcfg);        
    }

    p->diagnostics.errorDescriptor.sourcedevice      = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.sourceaddress     = 0;
    p->diagnostics.errorDescriptor.par16             = 0;
    p->diagnostics.errorDescriptor.par64             = 0;
   
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, s_eo_inertials2_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));

    if(eobool_true == searchisok)
    {        
        p->diagnostics.errorType = eo_errortype_debug;
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials_ok);
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if((0 != p->diagnostics.repetitionOKcase) && (0 != p->diagnostics.reportPeriod))
        {
            p->diagnostics.errorCallbackCount = p->diagnostics.repetitionOKcase;        
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }
    
    
    if(eobool_false == searchisok)
    {   
        // in case we fail the discovery, it is better to put inside par64 the failure of discovery.
        // maskofmissing, maskofwronglydetected, 
        
        const eOcandiscovery_detection_t* detection = eo_candiscovery2_GetDetection(eo_candiscovery2_GetHandle());
                
        uint16_t maskofmissingCAN1 = ptrgt->canmap[0] & (~detection->replies[0]);
        uint16_t maskofmissingCAN2 = ptrgt->canmap[1] & (~detection->replies[1]);
        uint16_t maskofincompatibleCAN1 = detection->incompatibilities[0]; 
        uint16_t maskofincompatibleCAN2 = detection->incompatibilities[1]; 
        
        p->diagnostics.errorDescriptor.par64 = ((uint64_t)maskofmissingCAN1) | ((uint64_t)maskofmissingCAN2<<16) | ((uint64_t)maskofincompatibleCAN1<<32) | ((uint64_t)maskofincompatibleCAN2<<32);
        
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials_failed_candiscovery);
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


static void s_eo_inertials2_send_periodic_error_report(void *par)
{
    EOtheInertials2* p = (EOtheInertials2*) par;
    
    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);

    if(eoerror_value_CFG_inertials_failed_candiscovery == eoerror_code2value(p->diagnostics.errorDescriptor.code))
    {   // if i dont find the mtbs, i keep on sending the discovery results up. it is a temporary diagnostics tricks until we use the verification of services at bootstrap
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

static eObool_t s_eo_inertials2_isID32relevant(uint32_t id32)
{
    static const uint32_t mask0 = (((uint32_t)eoprot_endpoint_analogsensors) << 24) | (((uint32_t)eoprot_entity_as_inertial) << 16);
    
    if((id32 & mask0) == mask0)
    {
        return(eobool_true);
    }
    
    return(eobool_false); 
}

static eObool_t s_eo_inertials2_get_id(eObrd_canlocation_t loc, eOas_inertial_type_t type, uint16_t *id)
{
    EOtheInertials2* p = &s_eo_theinertials2;
    
    if((eoas_inertial_accel_mtb_int != type) && (eoas_inertial_accel_mtb_ext != type) && (eoas_inertial_gyros_mtb_ext != type))
    {
        return(eobool_false);
    }
    
    uint16_t *candidates = p->fromcan2id[loc.port][loc.addr];
    uint8_t index = type - eoas_inertial_accel_mtb_int;
    
    *id = candidates[index];
    
    if((NOID16 == *id) && (eoas_inertial_accel_mtb_int == type))
    {
        // it is possible that we have have configured an external accel ... try assigning it
        *id = candidates[eoas_inertial_accel_mtb_ext-eoas_inertial_accel_mtb_int];
    }
    
    if(NOID16 == *id)
    {
        return(eobool_false);
    }
        
    return(eobool_true);
}


static void s_eo_inertials2_presenceofcanboards_reset(EOtheInertials2 *p)
{
    memset(p->not_heardof_target, 0, sizeof(p->not_heardof_target));
    memset(p->not_heardof_status, 0, sizeof(p->not_heardof_status));
    p->not_heardof_counter = 0;
}


static void s_eo_inertials2_presenceofcanboards_start(EOtheInertials2 *p)
{
    // prepare not_heardof_target ... it is equal to canmap_mtb_active in or with those
    // boards which are searched in discovery
    
    const eOcandiscovery_target_t *ptrgt = (const eOcandiscovery_target_t*) eo_array_At(p->sharedcan.discoverytargets, 0);
    
    if(NULL == ptrgt)
    {
        return;
    }
    
    if(0 == (ptrgt->info.protocol.major + ptrgt->info.protocol.minor +
             ptrgt->info.firmware.major + ptrgt->info.firmware.minor + ptrgt->info.firmware.build))
    {
        memset(p->not_heardof_target, 0, sizeof(p->not_heardof_status));
    }
    else
    {
        memcpy(p->not_heardof_target, p->canmap_mtb_active, sizeof(p->not_heardof_status));
    }
    
    memcpy(p->not_heardof_status, p->not_heardof_target, sizeof(p->not_heardof_status));
    p->not_heardof_counter = 0;
}


static void s_eo_inertials2_presenceofcanboards_touch(EOtheInertials2 *p, eObrd_canlocation_t loc)
{
    eo_common_hlfword_bitclear(&p->not_heardof_status[loc.port], loc.addr);
}


static void s_eo_inertials2_presenceofcanboards_tick(EOtheInertials2 *p)
{
    if((p->not_heardof_counter++) > 1000)
    {        
        if((0 != p->not_heardof_status[0]) || (0 != p->not_heardof_status[1]))
        {
            eOerrmanDescriptor_t errdes = {0};
            errdes.sourcedevice       = eo_errman_sourcedevice_localboard;
            errdes.sourceaddress      = 0;
            errdes.par16              = eobrd_mtb;
            errdes.par64              = (p->not_heardof_status[0] << 16) | p->not_heardof_status[1];        
            errdes.code = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_boards_lostcontact);
            p->diagnostics.errorType = eo_errortype_warning;                
            eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &errdes);
        } 

        memcpy(p->not_heardof_status, p->not_heardof_target, sizeof(p->not_heardof_status));
        p->not_heardof_counter = 0;    
    }
}


#endif // #elif !defined(EOTHESERVICES_disable_theInertials2)


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



