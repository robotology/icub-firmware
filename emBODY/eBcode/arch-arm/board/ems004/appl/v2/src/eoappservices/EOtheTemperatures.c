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

#include "EOtheTemperatures.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheTemperatures_hid.h"


#if defined(EOTHESERVICES_disable_theTemperatures)

    // provide empty implementation, so that we dont need to change the caller of the API
    
    extern EOtheTemperatures* eo_temperatures_Initialise(void) 
    {   
        return NULL; 
    }

    extern EOtheTemperatures* eo_temperatures_GetHandle(void)   
    { 
        return NULL; 
    }

    extern eOmn_serv_state_t eo_temperatures_GetServiceState(EOtheTemperatures *p) 
    { 
        return eomn_serv_state_notsupported; 
    }
    
    // in some cases, we need to alert the pc104 that the board does not support this service
    extern eOresult_t eo_temperatures_SendReport(EOtheTemperatures *p)
    {
        static const char s_eobj_ownname[] = "EOtheTemperatures";
        eOerrmanDescriptor_t errdes = {};
        errdes.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_temperatures_failed_notsupported);  
        errdes.sourcedevice = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress = 0;
        errdes.par16 = errdes.par64 = 0;
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);
        return eores_OK;
    }


    extern eOresult_t eo_temperatures_Verify(EOtheTemperatures *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify)
    {
        // we alert the host that the verification of the service has failed
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_temperatures, eomn_serv_state_failureofverify);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        } 
        
        // we tell that the reason is that this service is not supported
        eo_temperatures_SendReport(NULL);
               
        return eores_NOK_generic;
    }

    extern eOresult_t eo_temperatures_Activate(EOtheTemperatures *p, const eOmn_serv_configuration_t * servcfg)
    {
        eo_temperatures_SendReport(NULL);
        return eores_NOK_generic;
    }

    extern eOresult_t eo_temperatures_Deactivate(EOtheTemperatures *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_temperatures_Start(EOtheTemperatures *p)
    {
        eo_temperatures_SendReport(NULL);
        return eores_NOK_generic;
    }

    extern eOresult_t eo_temperatures_SetRegulars(EOtheTemperatures *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
    {
        if(NULL != arrayofid32)
        {
            eo_temperatures_SendReport(NULL);
        }
        return eores_NOK_generic;
    }

    extern eOresult_t eo_temperatures_Tick(EOtheTemperatures *p, eObool_t resetstatus)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_temperatures_Stop(EOtheTemperatures *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_temperatures_Transmission(EOtheTemperatures *p, eObool_t on)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_temperatures_Config(EOtheTemperatures *p, eOas_temperature_config_t* config)
    {
        eo_temperatures_SendReport(NULL);
        return eores_NOK_generic;
    }
    
    extern eOresult_t eo_temperatures_AcceptCANframe(EOtheTemperatures *p, eOas_temperature_type_t type, eOcanframe_t *frame, eOcanport_t port)
    {
        return eores_NOK_generic;
    }

    extern eObool_t eocanprotASperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_THERMOMETER_MSG(eOcanframe_t *frame, eOcanport_t port)
    {   
        return(eobool_false);
    } 
    
#elif !defined(EOTHESERVICES_disable_theTemperatures)

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


static eOresult_t s_eo_temperatures_TXstart(EOtheTemperatures *p);

static eOresult_t s_eo_temperatures_TXstop(EOtheTemperatures *p);

static eObool_t s_eo_temperatures_activestate_can_accept_canframe(void);

static void s_eo_temperatures_build_maps(EOtheTemperatures* p, uint32_t enablemask);

static eOresult_t s_eo_temperatures_onstop_search4canboards(void *par, EOtheCANdiscovery2* p, eObool_t searchisok);

static void s_eo_temperatures_send_periodic_error_report(void *p);

static eObool_t s_eo_temperatures_isID32relevant(uint32_t id32);

static eObool_t s_eo_temperatures_get_id(eObrd_canlocation_t loc, eOas_temperature_type_t type, uint8_t *id);

static void s_eo_temperatures_presenceofcanboards_reset(EOtheTemperatures *p);
static void s_eo_temperatures_presenceofcanboards_build(EOtheTemperatures *p);
static void s_eo_temperatures_presenceofcanboards_start(EOtheTemperatures *p);
static void s_eo_temperatures_presenceofcanboards_touch(EOtheTemperatures *p, eObrd_canlocation_t loc);
static void s_eo_temperatures_presenceofcanboards_tick(EOtheTemperatures *p);

static void s_eo_temperatures_chip_configure(EOtheTemperatures *p);
static void s_eo_temperatures_chip_transmission(EOtheTemperatures *p, eObool_t on, uint8_t period);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const eOas_temperature_config_t s_eo_default_temperatureconfig =
{
    .datarate                       = 2,
    .enabled                        = 0
};



static EOtheTemperatures s_eo_thetemperature = 
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

    .fifooftemperaturedata        = NULL,
    
    .temperature                  = NULL,
    .id32ofregulars             = NULL,
    .setofboardinfos            = {0},
    .arrayofsensordescriptors   = NULL,
    .not_heardof_target         = {0},
    .not_heardof_status         = {0},
    .not_heardof_counter        = 0,
    .not_heardtimeoutmilli      = 0,               
    .transmissionisactive       = eobool_false
};



static const char s_eobj_ownname[] = "EOtheTemperatures";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheTemperatures* eo_temperatures_Initialise(void)
{
    EOtheTemperatures* p = &s_eo_thetemperature;
    if(eobool_true == p->service.initted)
    {
        return(p);
    }
            
    p->service.servconfig.type = eomn_serv_NONE;
        
    p->sharedcan.boardproperties = eo_vector_New(sizeof(eObrd_canproperties_t), eo_temperatures_maxnumberofCANboards, NULL, NULL, NULL, NULL);
    
    p->sharedcan.entitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), eo_temperatures_maxnumberofCANboards, NULL, NULL, NULL, NULL);
    
    p->sharedcan.discoverytargets = eo_array_New(eOas_temperature_boardinfos_maxnumber, sizeof(eOcandiscovery_target_t),  NULL);
    
    memset(p->canmap_brd_active, 0, sizeof(p->canmap_brd_active));
    p->configured = eobool_false;
    memset(p->numofcanboards, 0, sizeof(p->numofcanboards));
    
    
    s_eo_temperatures_presenceofcanboards_reset(p);
    
    p->temperature = NULL;
    p->id32ofregulars = eo_array_New(temperature_maxRegulars, sizeof(uint32_t), NULL);
    memset(&p->setofboardinfos, 0, sizeof(p->setofboardinfos));
    p->arrayofsensordescriptors = eo_array_New(eOas_temperature_descriptors_maxnumber, sizeof(eOas_temperature_descriptor_t), NULL);
    
    memcpy(&p->sensorsconfig, &s_eo_default_temperatureconfig, sizeof(eOas_temperature_config_t));
    p->fifooftemperaturedata = eo_vector_New(sizeof(eOas_temperature_data_t), eo_temperatures_fifocapacity, NULL, 0, NULL, NULL);

    p->diagnostics.reportTimer = eo_timer_New();
    p->diagnostics.errorType = eo_errortype_error;
    p->diagnostics.errorDescriptor.sourceaddress = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_temperatures_not_verified_yet);  
    
    p->service.initted = eobool_true;    
    p->service.active = eobool_false;
    p->service.started = eobool_false;
    p->transmissionisactive = eobool_false;
    p->service.state = eomn_serv_state_idle;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_temperatures, p->service.state);
    
    return(p);   
}


extern EOtheTemperatures* eo_temperatures_GetHandle(void)
{
    if(eobool_true == s_eo_thetemperature.service.initted)
    {
        return(&s_eo_thetemperature);
    }
    
    return(NULL);
}


extern eOmn_serv_state_t eo_temperatures_GetServiceState(EOtheTemperatures *p)
{
    if(NULL == p)
    {
        return(eomn_serv_state_notsupported);
    } 

    return(p->service.state);
}


extern eOresult_t eo_temperatures_SendReport(EOtheTemperatures *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
    
    eOerror_value_t errorvalue = eoerror_code2value(p->diagnostics.errorDescriptor.code);
    
    switch(errorvalue)
    {
        case eoerror_value_CFG_temperatures_failed_candiscovery:
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


extern eOresult_t eo_temperatures_Verify(EOtheTemperatures *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify)
{
    //eOresult_t res = eores_NOK_generic;
    
    if((NULL == p) || (NULL == servcfg))
    {
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_temperatures, p->service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }        
        return(eores_NOK_nullpointer);
    } 

    if(eomn_serv_AS_temperatures != servcfg->type)
    {
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_temperatures, p->service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }        
        return(eores_NOK_generic);
    }   
    
 
    if(eobool_true == p->service.active)
    {
        eo_temperatures_Deactivate(p);        
    }   
    
    p->service.state = eomn_serv_state_verifying; 
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_temperatures, p->service.state);    

    // make sure the timer is not running
    eo_timer_Stop(p->diagnostics.reportTimer);    

    p->service.onverify = onverify;
    p->service.activateafterverify = activateafterverify;
    
    // i get all the sensors.
    memcpy(&p->setofboardinfos, &servcfg->data.as.temperature.setofboardinfos, sizeof(eOas_temperature_setof_boardinfos_t));
    memcpy(p->arrayofsensordescriptors, &servcfg->data.as.temperature.arrayofdescriptor, sizeof(eOas_temperature_arrayof_descriptors_t));
    
    
    // now we deal with sensors on can:
    // step-1: check vs max number of can boards in config
    // step-2: do discovery of all boards (let us begin w/ strain2 only at first)
    
    // force a cleaned discoverytargets before we add the target
    eo_array_Reset(p->sharedcan.discoverytargets);
    
    
    // the number of sensor descriptors
    uint8_t numofsensors = eo_array_Size(p->arrayofsensordescriptors);
    uint16_t overlappedmap[2] = {0};
    for(uint8_t b=0; b<eoas_temperature_supportedboards_numberof(); b++)
    {
        eOcandiscovery_target_t trgt = {0};
        eObool_t found = eobool_false;
        eObrd_cantype_t brdtype = eoas_temperature_supportedboards_gettype(b);
        const eObrd_info_t * brdinfo = eoas_temperature_setof_boardinfos_find(&p->setofboardinfos, brdtype);
        if(NULL == brdinfo)
        {   // we dont have this boardtype 
            continue;
        }
        memmove(&trgt.info, brdinfo, sizeof(eObrd_info_t)); 
        trgt.canmap[eOcanport1] =  trgt.canmap[eOcanport2] = 0;
        // now i search for this brdtype inside the arrayofsensordescriptors
        for(uint8_t s=0; s<numofsensors; s++)
        {
            eOas_temperature_descriptor_t *des = (eOas_temperature_descriptor_t*) eo_array_At(p->arrayofsensordescriptors, s);
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
        eo_action_SetCallback(act, s_eo_temperatures_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));        
        
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_temperatures_failed_generic);
        p->diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if(0 != p->diagnostics.reportPeriod)
        {
            p->diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);   
        }  
        
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_temperatures, p->service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }    
        
        return(eores_NOK_generic);                 
                
    }
    
    
    uint8_t numofboards = eo_common_hlfword_bitsetcount(overlappedmap[eOcanport1]) + eo_common_hlfword_bitsetcount(overlappedmap[eOcanport2]);
    // we check that numofboards is ok.    
    if(numofboards > eo_temperatures_maxnumberofCANboards)
    {        
        p->diagnostics.errorDescriptor.sourcedevice       = eo_errman_sourcedevice_localboard;
        p->diagnostics.errorDescriptor.sourceaddress      = 0;
        p->diagnostics.errorDescriptor.par16              = (numofboards << 8) | (eo_temperatures_maxnumberofCANboards & 0x00ff);
        p->diagnostics.errorDescriptor.par64              = (overlappedmap[eOcanport2] << 16) | (overlappedmap[eOcanport1]);
       
        EOaction_strg astrg = {0};
        EOaction *act = (EOaction*)&astrg;
        eo_action_SetCallback(act, s_eo_temperatures_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));        
        
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_temperatures_failed_toomanyboards);
        p->diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if(0 != p->diagnostics.reportPeriod)
        {
            p->diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);   
        }  
        
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_temperatures, p->service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }    
        
        return(eores_NOK_generic); 
    }    

    // step-2: can discovery
    
    
    p->sharedcan.ondiscoverystop.function = s_eo_temperatures_onstop_search4canboards;
    p->sharedcan.ondiscoverystop.parameter = (void*)servcfg;
    
    // start discovery of can boards  
    eo_candiscovery2_Start2(eo_candiscovery2_GetHandle(), p->sharedcan.discoverytargets, &p->sharedcan.ondiscoverystop);   
   
    return(eores_OK);   
}


extern eOresult_t eo_temperatures_Deactivate(EOtheTemperatures *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == p->service.active)
    {
        // i force to eomn_serv_state_idle because it may be that state was eomn_serv_state_verified or eomn_serv_state_failureofverify
        p->service.state = eomn_serv_state_idle; 
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_temperatures, p->service.state);
        return(eores_OK);        
    } 
    
    if(eobool_true == p->service.started)
    {
        eo_temperatures_Stop(p);
    }
    
    eo_temperatures_SetRegulars(p, NULL, NULL);
      
    eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_temperature, p->sharedcan.entitydescriptor); 
    
    eo_canmap_UnloadBoards(eo_canmap_GetHandle(), p->sharedcan.boardproperties);
     
    
    eo_entities_SetNumOfTemperatures(eo_entities_GetHandle(), 0);
    
    p->temperature = NULL;
    
    // no need to reset setofboardinfos
    eo_array_Reset(p->arrayofsensordescriptors);
    
    eo_vector_Clear(p->fifooftemperaturedata);
                
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
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_temperatures, p->service.state);
    
    return(eores_OK);
}


extern eOresult_t eo_temperatures_Activate(EOtheTemperatures *p, const eOmn_serv_configuration_t * servcfg)
{
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    } 

    if(eomn_serv_AS_temperatures != servcfg->type)
    {
        return(eores_NOK_generic);
    }
    
    if(eobool_true == p->service.active)
    {
        eo_temperatures_Deactivate(p);        
    }   
    
    eo_entities_SetNumOfTemperatures(eo_entities_GetHandle(), 1);
    
    if(0 == eo_entities_NumOfTemperatures(eo_entities_GetHandle()))
    {
        p->service.active = eobool_false;
        return(eores_NOK_generic);
    }
    
    p->temperature = eo_entities_GetTemperature(eo_entities_GetHandle(), 0);
    
    // now service config stuff
    
    memcpy(&p->service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));

    
            
    //#warning TODO: save memory be avoiding the duplication of p->service.servconfig into arrayofsensordescriptors and setofboardinfos
            
    // now i get all the sensors info from ...
    memcpy(&p->setofboardinfos, &servcfg->data.as.temperature.setofboardinfos, sizeof(eOas_temperature_setof_boardinfos_t));
    memcpy(p->arrayofsensordescriptors, &servcfg->data.as.temperature.arrayofdescriptor, sizeof(eOas_temperature_arrayof_descriptors_t));
    
    
    // now i build maps for check of presence
    
    s_eo_temperatures_presenceofcanboards_build(p);
    
    
    // now i must build the canbusmapping[] for EOtheCANmapping object
    
    uint8_t numofsensors = eo_array_Size(p->arrayofsensordescriptors);    

    uint16_t canbusmapping[2] = {0};
    eObrd_cantype_t boardtypes[2][16] = {eobrd_cantype_none};
    
    for(uint8_t i=0; i<numofsensors; i++)
    {
        eOas_temperature_descriptor_t *des = (eOas_temperature_descriptor_t*) eo_array_At(p->arrayofsensordescriptors, i);
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
            {   // i pushback. i dont verify vs the capacity of the vector because eo_temperatures_Verify() has already done it
                
                prop.type = boardtypes[j][k];
                const eObrd_info_t * brdinfo = eoas_temperature_setof_boardinfos_find(&p->setofboardinfos, (eObrd_cantype_t)prop.type);
                
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
    eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_temperature, p->sharedcan.entitydescriptor);   
    
    
    // build the maps that can translate a received can message into the id to be put into ...
    // we assume that 
    s_eo_temperatures_build_maps(p, 0xffffffff);
    p->configured = eobool_false; // still false
    p->service.active = eobool_true;    
    p->service.state = eomn_serv_state_activated;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_temperatures, p->service.state);    

    
    return(eores_OK);   
}



extern eOresult_t eo_temperatures_Start(EOtheTemperatures *p)
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
//        eo_temperatures_Stop(p);
//    } 
    

    // ok, now we do something.    
         
    //s_eo_temperatures_TXstart(p);

    p->service.started = eobool_true;    
    p->service.state = eomn_serv_state_started;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_temperatures, p->service.state);
    
    
    return(eores_OK);      
}

extern eOresult_t eo_temperatures_SetRegulars(EOtheTemperatures *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
    
    return(eo_service_hid_SetRegulars(p->id32ofregulars, arrayofid32, s_eo_temperatures_isID32relevant, numberofthem));
}


extern eOresult_t eo_temperatures_Stop(EOtheTemperatures *p)
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
      
    s_eo_temperatures_TXstop(p);
    
               
    p->service.started = eobool_false;
    p->service.state = eomn_serv_state_activated;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_temperatures, p->service.state);
    
    // reset the various buffers
    eo_vector_Clear(p->fifooftemperaturedata);
    
    
    // remove all regulars related to inertials entity ... no, dont do that.
    //eo_temperatures_SetRegulars(p, NULL, NULL);
    
    return(eores_OK);     
}


extern eOresult_t eo_temperatures_Transmission(EOtheTemperatures *p, eObool_t on)
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
        s_eo_temperatures_TXstart(p);
    }
    else
    {
        s_eo_temperatures_TXstop(p);
    }

    return(eores_OK);
}


extern eOresult_t eo_temperatures_Tick(EOtheTemperatures *p, eObool_t resetstatus)
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
        s_eo_temperatures_presenceofcanboards_tick(p);
    }

    if(eobool_false == resetstatus)
    {   // nothing to do because we cannot overwrite the status of inertial 
        return(eores_OK);
    }      

    // i can update the status of the inertial because the old value has already been transmitted.
        
    // the status data of the inertial entity is in ...
    
    EOarray * statusarrayofdata = (EOarray*) &p->temperature->status.arrayofdata;
    
    eo_array_Reset(statusarrayofdata);


    // i put new data in status with this rule:
    // first: if we have something inside fifooftemperaturedata.
    // if nothing: i set data invalid
    if(eobool_false == eo_vector_Empty(p->fifooftemperaturedata))
    {
        uint8_t fifosize = eo_vector_Size(p->fifooftemperaturedata);
        uint8_t capacity =  eo_array_Capacity(statusarrayofdata); 
        uint8_t nitems = (fifosize < capacity) ? fifosize : capacity;
        for(uint8_t i=0; i<nitems; i++)
        {
            eOas_temperature_data_t * item = (eOas_temperature_data_t*) eo_vector_Front(p->fifooftemperaturedata);
            if(NULL != item)
            {
                eo_array_PushBack(statusarrayofdata, item);            
                eo_vector_PopFront(p->fifooftemperaturedata);   
                //eo_errman_Trace(eo_errman_GetHandle(), "tx temp", s_eobj_ownname);
            }  
        }        
    }
    else
    {
        // the arraystatus is already resized to zero.       
    }
                 
    return(eores_OK);        
}



extern eOresult_t eo_temperatures_Config(EOtheTemperatures *p, eOas_temperature_config_t* config)
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
    if(p->sensorsconfig.datarate < 1)
    {
        p->sensorsconfig.datarate = 1;        
    }
    
    
    if(originalrate != p->sensorsconfig.datarate)
    {
        eOerrmanDescriptor_t errdes = {0};
        errdes.sourcedevice       = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress      = 0;
        errdes.par16              = (originalrate << 8) | (p->sensorsconfig.datarate);
        errdes.par64              = 0;        
        errdes.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_temperatures_changed_requestedrate);
        p->diagnostics.errorType = eo_errortype_warning;                
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &errdes);        
    }
        
    
    // now ... we need to change the masks according to p->sensorsconfig.maskofenabled
    
    s_eo_temperatures_build_maps(p, p->sensorsconfig.enabled);
    
    
    p->configured = eobool_true;    
 
    // ok guys: now we transmit the configuration to each one of the sensors which have a non-zero bit inside teh p->config2.enabled mask.
    // WAIT A MINUTE: we do it inside eo_temperatures_Start() !!!!!!!!!!!!

    
    return(eores_OK);           
}


extern eOresult_t eo_temperatures_AcceptCANframe(EOtheTemperatures *p, eOas_temperature_type_t type, eOcanframe_t *frame, eOcanport_t port)
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
    
    if(eobool_false == s_eo_temperatures_activestate_can_accept_canframe())
    {
        return(eores_OK);
    }
    
    eOas_temperature_data_t data = {0};
       
    
    eObrd_canlocation_t loc = {0};    
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
    loc.insideindex = eobrd_caninsideindex_none;
    
    uint8_t id = NOID08;

    
    if(eobool_true == eo_vector_Full(p->fifooftemperaturedata))
    {
        // damn... a loss of can frames
        eOerrmanDescriptor_t des = {0};
        des.code            = eoerror_code_get(eoerror_category_AnalogSensor, eoerror_value_AS_arrayoftemperaturedataoverflow);
        des.par16           = (frame->id & 0x0fff) | ((frame->size & 0x000f) << 12);
        des.par64           = eo_common_canframe_data2u64((eOcanframe_t*)frame);
        des.sourceaddress   = EOCANPROT_FRAME_GET_SOURCE(frame);
        des.sourcedevice    = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &des);    
        
        // BUT dont want to lose NEW data ... i could just call a pop front to remove oldest data
        //eo_vector_PopFront(p->fifooftemperaturedata);  
        // BUT .. as i dont want to througth data away, i decide that i put it inside the status.
        // in short: i call eo_temperatures_Tick(p, true) which already does that        
        eo_temperatures_Tick(p, eobool_true);      
    }
        

    
    if(eobool_false == s_eo_temperatures_get_id(loc, type, &id))
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
    data.value = (int16_t)((frame->data[2]<<8) + frame->data[1]);   // data[1] is a mask which tells if the value is in [2][1] or in [4][3] ... but it is always in first position
    data.timestamp =  eov_sys_LifeTimeGet(eov_sys_GetHandle()) / 1000;
    data.id = id;
        
    // ok, now we can pushback data into the fifooftemperaturedata   
    eo_vector_PushBack(p->fifooftemperaturedata, &data);
    
#if 0    
        eOerrmanDescriptor_t des = {0};
        des.code            = eoerror_code_get(eoerror_category_InertialSensor, eoerror_value_IS_arrayofinertialdataoverflow);
        des.par16           = (frame->id & 0x0fff) | ((frame->size & 0x000f) << 12);
        des.par64           = eo_common_canframe_data2u64((eOcanframe_t*)frame);
        des.sourceaddress   = EOCANPROT_FRAME_GET_SOURCE(frame);
        des.sourcedevice    = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &des);  
#endif
    
    s_eo_temperatures_presenceofcanboards_touch(p, loc);
    
    return(eores_OK);      
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// -- in here .... so that all things related to Inertial are in a unique place


extern void eoprot_fun_INIT_as_temperature_config(const EOnv* nv)
{
    eOas_temperature_config_t* config = (eOas_temperature_config_t*) eo_nv_RAM(nv);
    
    config->datarate = s_eo_default_temperatureconfig.datarate;
    config->enabled = s_eo_default_temperatureconfig.enabled;
}


extern void eoprot_fun_INIT_as_temperature_status(const EOnv* nv)
{
    eOas_temperature_status_t* status = (eOas_temperature_status_t*) eo_nv_RAM(nv);  
    // must initialise it as an array
    
    uint8_t capacity = eOas_temperature_data_maxnumber;
    uint8_t itemsize = sizeof(eOas_temperature_data_t);
    uint8_t size = 0;
    EOarray* array = NULL;

    array = eo_array_New(capacity, itemsize, &status->arrayofdata);
    eo_array_Resize(array, size);
}

                
extern eObool_t eocanprotASperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_THERMOMETER_MSG(eOcanframe_t *frame, eOcanport_t port)
{   
    EOtheTemperatures *p = &s_eo_thetemperature;  
    
    if(NULL == frame)
    {
        return(eobool_true);
    }    
    
    // marco.accame:        
    // i dont want to put data in the fifooftemperaturedata when we are not in the control loop.
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



static eOresult_t s_eo_temperatures_TXstart(EOtheTemperatures *p)
{ 
    if(eobool_false == p->configured)
    {   // dont have a configured service
        return(eores_OK);
    }    
    
    if((0 == p->canmap_brd_active[0]) && (0 == p->canmap_brd_active[1]))
    {   // no can boards or onboard sensors configured
        return(eores_OK);
    } 
    
    if(0 == p->sensorsconfig.enabled)
    {   // no can boards or local mems enabled
        return(eores_OK);
    } 
                        
  
    s_eo_temperatures_chip_configure(p);

    s_eo_temperatures_chip_transmission(p, eobool_true, p->sensorsconfig.datarate);
    
    
    p->transmissionisactive = eobool_true;
    s_eo_temperatures_presenceofcanboards_start(p);
    
    return(eores_OK);   
}


static eOresult_t s_eo_temperatures_TXstop(EOtheTemperatures *p)
{
    if(eobool_false == p->configured)
    {   // nothing to do because we dont have a configured service 
        return(eores_OK);
    }     
  
    // stop the tx
    s_eo_temperatures_chip_transmission(p, eobool_false, p->sensorsconfig.datarate);
       
    p->transmissionisactive = eobool_false;
    s_eo_temperatures_presenceofcanboards_reset(p);
               
    return(eores_OK);
}


//#define EOTEMPERATURE3_ALWAYS_ACCEPT_CANFRAMES_IN_ACTIVE_MODE
static eObool_t s_eo_temperatures_activestate_can_accept_canframe(void)
{
#if     defined(EOTEMPERATURE3_ALWAYS_ACCEPT_CANFRAMES_IN_ACTIVE_MODE)
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


static void s_eo_temperatures_build_maps(EOtheTemperatures* p, uint32_t enablemask)
{
    uint8_t numofsensors = eo_array_Size(p->arrayofsensordescriptors);    
  
    // at first we disable all. 
    
    memset(p->canmap_brd_active, 0, sizeof(p->canmap_brd_active));

    for(uint8_t i=0; i<numofsensors; i++)
    {
        eOas_temperature_descriptor_t *des = (eOas_temperature_descriptor_t*) eo_array_At(p->arrayofsensordescriptors, i);
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
            }

        }
    }    
    
}


// it is the last discovery of can boards... 
static eOresult_t s_eo_temperatures_onstop_search4canboards(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok)
{
    const eOmn_serv_configuration_t * servcfg = (const eOmn_serv_configuration_t *)par;
    EOtheTemperatures *p = &s_eo_thetemperature;
    
    if(eobool_true == searchisok)
    {
        p->service.state = eomn_serv_state_verified;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_temperatures, p->service.state);
    }
    else
    {   
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_temperatures, p->service.state);
    }
    
    if((eobool_true == searchisok) && (eobool_true == p->service.activateafterverify))
    {
        eo_temperatures_Activate(p, servcfg);        
    }

    p->diagnostics.errorDescriptor.sourcedevice      = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.sourceaddress     = 0;
    p->diagnostics.errorDescriptor.par16             = 0;
    p->diagnostics.errorDescriptor.par64             = 0;
   
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, s_eo_temperatures_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));

    if(eobool_true == searchisok)
    {        
        p->diagnostics.errorType = eo_errortype_debug;
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_temperatures_ok);
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
        
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_temperatures_failed_candiscovery);
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


static void s_eo_temperatures_send_periodic_error_report(void *par)
{
    EOtheTemperatures* p = (EOtheTemperatures*) par;
    
    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);

    if(eoerror_value_CFG_temperatures_failed_candiscovery == eoerror_code2value(p->diagnostics.errorDescriptor.code))
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

static eObool_t s_eo_temperatures_isID32relevant(uint32_t id32)
{
    static const uint32_t mask0 = (((uint32_t)eoprot_endpoint_analogsensors) << 24) | (((uint32_t)eoprot_entity_as_temperature) << 16);
    
    if((id32 & mask0) == mask0)
    {
        return(eobool_true);
    }
    
    return(eobool_false); 
}

static eObool_t s_eo_temperatures_get_id(eObrd_canlocation_t loc, eOas_temperature_type_t type, uint8_t *id)
{
    // EOtheTemperatures* p = &s_eo_thetemperature;
    
    eObool_t ret = eobool_true;
    
    switch(type)
    {
        case eoas_temperature_t1:
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
    
    //#warning TODO: if we save some ram by removing the double arrayofsensordescriptors, then we may add a map of 128 bytes.

    *id = (loc.port << 4) | loc.addr;
    

    if(NOID08 == *id)
    {
        return(eobool_false);
    }
        
    return(ret);
}


static void s_eo_temperatures_presenceofcanboards_reset(EOtheTemperatures *p)
{
    memset(p->not_heardof_target, 0, sizeof(p->not_heardof_target));
    memset(p->not_heardof_status, 0, sizeof(p->not_heardof_status));
    p->not_heardof_counter = 0;
    p->not_heardtimeoutmilli = 1000;
}

static void s_eo_temperatures_presenceofcanboards_build(EOtheTemperatures *p)
{
    s_eo_temperatures_presenceofcanboards_reset(p);

    // now i must build the canmaps used to monitor presence 
    uint8_t numofsensors = eo_array_Size(p->arrayofsensordescriptors);   
    
    p->not_heardof_target[eOcanport1] = p->not_heardof_target[eOcanport2] = 0;

    for(uint8_t i=0; i<numofsensors; i++)
    {
        eOas_temperature_descriptor_t *des = (eOas_temperature_descriptor_t*) eo_array_At(p->arrayofsensordescriptors, i);
        if(NULL != des)
        {
            if(eobrd_place_can == des->on.any.place)
            {
                eo_common_hlfword_bitset(&p->not_heardof_target[des->on.can.port], des->on.can.addr);  
            }
        }
    }    
}


static void s_eo_temperatures_presenceofcanboards_start(EOtheTemperatures *p)
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


static void s_eo_temperatures_presenceofcanboards_touch(EOtheTemperatures *p, eObrd_canlocation_t loc)
{
    eo_common_hlfword_bitclear(&p->not_heardof_status[loc.port], loc.addr);
}


static void s_eo_temperatures_presenceofcanboards_tick(EOtheTemperatures *p)
{
    if((p->not_heardof_counter++) > p->not_heardtimeoutmilli)
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

static void s_eo_temperatures_chip_transmission(EOtheTemperatures *p, eObool_t on, uint8_t periodsec)
{
    icubCanProto_thermo_transmit_t transmit = {0};
    transmit.periodsec = (eobool_true == on) ? (periodsec) : (0);
    
    p->not_heardtimeoutmilli = periodsec * 1000 + 5000;
    
    p->sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;
    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__THERMOMETER_TRANSMIT;
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

static void s_eo_temperatures_chip_configure(EOtheTemperatures *p)
{
    icubCanProto_thermo_config_t tconfig = {0};
    tconfig.sensormask = 1;
    
    p->sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;
    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__THERMOMETER_CONFIG_SET;
    p->sharedcan.command.value = &tconfig;
    
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
                // and now i fill sensormask and i send the message.... but sensormask is always 1
                tconfig.sensormask = 0; 
                for(uint8_t i=0; i<numofsensors; i++)
                {
                    eOas_temperature_descriptor_t *des = (eOas_temperature_descriptor_t*) eo_array_At(p->arrayofsensordescriptors, i);
                    if(NULL != des)
                    {
                        if((des->on.any.place == eobrd_place_can) && (des->on.can.port == port) && (des->on.can.addr == addr))
                        {
                            // ok, i have the board. i must enable the sensors 
                            icubCanProto_thermo_sensor_t ps = icubCanProto_thermo_t1; // eoas_temperature_chip_to_canproto(des->typeofsensor);
//                            if(icubCanProto_thermo_none != ps)
//                            {
                                eo_common_byte_bitset(&tconfig.sensormask, ps);
//                            }
                        }
                    }
                }
                eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &p->sharedcan.command, location);
            }
        }
    }   
}


#endif // #elif !defined(EOTHESERVICES_disable_theTemperatures)


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



