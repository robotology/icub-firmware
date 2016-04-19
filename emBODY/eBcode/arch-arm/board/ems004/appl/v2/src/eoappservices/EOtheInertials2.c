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

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheInertials2.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheInertials2_hid.h"


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

static eOresult_t s_eo_inertials2_TXstart(EOtheInertials2 *p);

static eOresult_t s_eo_inertials2_TXstop(EOtheInertials2 *p);

static eObool_t s_eo_inertials2_activestate_can_accept_canframe(void);

static void s_eo_inertials2_build_maps(EOtheInertials2* p, uint64_t enablemask);

static eOresult_t s_eo_inertials2_onstop_search4mtbs(void *par, EOtheCANdiscovery2* p, eObool_t searchisok);

static void s_eo_inertials2_send_periodic_error_report(void *p);

static eObool_t s_eo_inertials2_isID32relevant(uint32_t id32);

static eObool_t s_eo_inertials2_get_id(eOcanmap_location_t loc, eOas_inertial_type_t type, uint16_t *id);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const eOas_inertial_config_t s_eo_default_inertialconfig =
{
    .datarate               = 50,
    .filler                 = {0},
    .enabled                = 0
};



static EOtheInertials2 s_eo_theinertials2 = 
{
    .service = 
    {
        .servconfig             = { .type = eomn_serv_NONE },
        .initted                = eobool_false,
        .active                 = eobool_false,
        .activateafterverify    = eobool_false,
        .running                = eobool_false,
        .onverify               = NULL,
        .state                  = eomn_serv_state_notsupported          
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
        .discoverytarget        = {0},
        .ondiscoverystop        = {0},
        .command                = {0}, 
    },

    .sensorsconfig              = {0},  
    .fifoofinertialdata         = NULL,
    
    .configured                 = eobool_false,
    
    .canmap_mtb_accel_int       = {0},
    .canmap_mtb_accel_ext       = {0},
    .canmap_mtb_gyros_ext       = {0},
    .canmap_mtb_active          = {0},
    
    .fromcan2id                 = {99},

    .inertial2                  = NULL,
    .id32ofregulars             = NULL
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
        
    p->sharedcan.boardproperties = eo_vector_New(sizeof(eOcanmap_board_properties_t), eo_inertials2_maxnumberofMTBboards, NULL, NULL, NULL, NULL);
    
    p->sharedcan.entitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), eo_inertials2_maxnumberofMTBboards, NULL, NULL, NULL, NULL);
    
    p->canmap_mtb_accel_int[0] = p->canmap_mtb_accel_int[1] = 0;
    p->canmap_mtb_accel_ext[0] = p->canmap_mtb_accel_ext[1] = 0;
    p->canmap_mtb_gyros_ext[0] = p->canmap_mtb_gyros_ext[1] = 0;
    p->canmap_mtb_active[0]    = p->canmap_mtb_active[1]    = 0;
    memset(p->fromcan2id, 99, sizeof(p->fromcan2id));

    p->inertial2 = NULL;
    p->id32ofregulars = eo_array_New(inertials_maxRegulars, sizeof(uint32_t), NULL);
    p->arrayofsensors = eo_array_New(eOas_inertials_maxnumber, sizeof(eOas_inertial_descriptor_t), NULL);
    
    memcpy(&p->sensorsconfig, &s_eo_default_inertialconfig, sizeof(eOas_inertial_config_t));
    p->fifoofinertialdata = eo_vector_New(sizeof(eOas_inertial_data_t), 32, NULL, 0, NULL, NULL);
    
    p->diagnostics.reportTimer = eo_timer_New();
    p->diagnostics.errorType = eo_errortype_error;
    p->diagnostics.errorDescriptor.sourceaddress = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials_not_verified_yet);  
    
    p->service.initted = eobool_true;    
    p->service.active = eobool_false;
    p->service.running = eobool_false;
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


    p->sharedcan.discoverytarget.info.type = eobrd_cantype_mtb;
    p->sharedcan.discoverytarget.info.protocol.major = servcfg->data.as.inertial.mtbversion.protocol.major; 
    p->sharedcan.discoverytarget.info.protocol.minor = servcfg->data.as.inertial.mtbversion.protocol.minor;
    p->sharedcan.discoverytarget.info.firmware.major = servcfg->data.as.inertial.mtbversion.firmware.major; 
    p->sharedcan.discoverytarget.info.firmware.minor = servcfg->data.as.inertial.mtbversion.firmware.minor;
    p->sharedcan.discoverytarget.info.firmware.build = servcfg->data.as.inertial.mtbversion.firmware.build;   

    // now i get all the sensors.
    memcpy(p->arrayofsensors, &servcfg->data.as.inertial.arrayofsensors, sizeof(eOas_inertial_arrayof_sensors_t));
    
    // so far we dont care about verifying sensors which are local ...
    
    // now i must build the canmaps ... but only for discovery
    uint8_t numofsensors = eo_array_Size(p->arrayofsensors);    
    p->sharedcan.discoverytarget.canmap[eOcanport1] =  p->sharedcan.discoverytarget.canmap[eOcanport2] = 0;
    p->numofmtbs = 0;
    for(uint8_t i=0; i<numofsensors; i++)
    {
        eOas_inertial_descriptor_t *des = (eOas_inertial_descriptor_t*) eo_array_At(p->arrayofsensors, i);
        if(NULL != des)
        {
            if(eobrd_place_can == des->on.any.place)
            {
                eo_common_hlfword_bitset(&p->sharedcan.discoverytarget.canmap[des->on.can.port], des->on.can.addr);  
            }
        }
    }
    
    uint8_t numofboards = eo_common_hlfword_bitsetcount(p->sharedcan.discoverytarget.canmap[eOcanport1]) +
                          eo_common_hlfword_bitsetcount(p->sharedcan.discoverytarget.canmap[eOcanport2]);
    
    if(numofboards > eo_inertials2_maxnumberofMTBboards)
    {
        
        p->diagnostics.errorDescriptor.sourcedevice       = eo_errman_sourcedevice_localboard;
        p->diagnostics.errorDescriptor.sourceaddress      = 0;
        p->diagnostics.errorDescriptor.par16              = (numofboards << 8) | (eo_inertials2_maxnumberofMTBboards & 0x00ff);
        p->diagnostics.errorDescriptor.par64              = (p->sharedcan.discoverytarget.canmap[eOcanport2] << 16) | (p->sharedcan.discoverytarget.canmap[eOcanport1]);
       
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
             
                  
    p->sharedcan.ondiscoverystop.function = s_eo_inertials2_onstop_search4mtbs;
    p->sharedcan.ondiscoverystop.parameter = (void*)servcfg;
    
    // start discovery of can boards  
    eo_candiscovery2_Start(eo_candiscovery2_GetHandle(), &p->sharedcan.discoverytarget, &p->sharedcan.ondiscoverystop);   
   
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
    
    if(eobool_true == p->service.running)
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
    
    eOcanmap_board_properties_t prop = 
    {
        .type               = eobrd_cantype_mtb, 
        .location           = { .port = 0, .addr = 0, .insideindex = eocanmap_insideindex_none },
        .requiredprotocol   = { .major = servcfg->data.as.inertial.mtbversion.protocol.major, .minor = servcfg->data.as.inertial.mtbversion.protocol.minor }
    };  
    
    eOcanmap_entitydescriptor_t des = 
    {
        .location   = { .port = 0, .addr = 0, .insideindex = eocanmap_insideindex_none },
        .index      = entindex00
    };        
    
    
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

    if(eobool_true == p->service.running)
    {   // it is already running
        return(eores_OK);
    } 
    
//    if(eobool_true == p->service.running)
//    {   // if running we stop before
//        eo_inertials2_Stop(p);
//    } 
    

    // ok, now we do something.    
         
    //s_eo_inertials2_TXstart(p);

    p->service.running = eobool_true;    
    p->service.state = eomn_serv_state_running;
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
    
    if(eobool_false == p->service.running)
    {   // it is already stopped
        return(eores_OK);
    } 
        

    // ok, now we do something.    
      
    s_eo_inertials2_TXstop(p);
    
               
    p->service.running = eobool_false;
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

    if(eobool_false == p->service.running)
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

    if(eobool_false == p->service.running)
    {   // not running, thus we do nothing
        return(eores_OK);
    } 

    if(eobool_false == p->configured)
    {   // nothing to do because we dont have a configured service 
        return(eores_OK);
    }    

    if(eobool_false == resetstatus)
    {   // nothing to do because we cannot overwrite the status of inertial 
        return(eores_OK);
    }       
    
    // i can update the status of the inertial because the old value has already been transmitted.
        
    // the status data of the inertial entity is in ...
    eOas_inertial_data_t *data = &p->inertial2->status.data;
    memset(data, 0, sizeof(eOas_inertial_data_t)); 
    
    if(eobool_true == eo_vector_Empty(p->fifoofinertialdata))
    {
        // just reset the status->data 
        data->id = 999;
        data->timestamp = eov_sys_LifeTimeGet(eov_sys_GetHandle());           
    }
    else
    {
        // else ... retrieve the item from fifoofinertialdata, copy it into status->data and remove it from fifoofinertialdata 
        eOas_inertial_data_t * item = (eOas_inertial_data_t*) eo_vector_Front(p->fifoofinertialdata);
        if(NULL != item)
        {
            memcpy(data, item, sizeof(eOas_inertial_data_t));   
            eo_vector_PopFront(p->fifoofinertialdata);   
        }
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
//    if(eobool_false == p->service.running)
//    {   // not running, thus we do nothing
//        return(eores_OK);
//    }
    
    
    // ok, now we do something. 
    
    // at first we copy the target config into the local config
    
    p->sensorsconfig.datarate           = config->datarate;
    p->sensorsconfig.enabled            = config->enabled;
    
    // then we check enabled mask and datarate
       
    if(p->sensorsconfig.datarate < 10)
    {
        p->sensorsconfig.datarate = 10;
        // send up diagnostics
        // warning -> "Object EOtheInertials2 has changed the requested datarate."         
    }
    
    if(p->sensorsconfig.datarate > 200)
    {
        p->sensorsconfig.datarate = 200;
        // send up diagnostics
        // warning -> "Object EOtheInertials2 has changed the requested datarate."             
    }
    
    // now ... we need to change the masks according to p->sensorsconfig.maskofenabled
    
    s_eo_inertials2_build_maps(p, p->sensorsconfig.enabled);
    
    
    p->configured = eobool_true;    
 
    // ok guys: now we transmit the configuration to each one of the sensors which have a non-zero bit inside teh p->config2.enabled mask.
    // WAIT A MINUTE: we do it inside eo_inertials2_Start() !!!!!!!!!!!!

    
    return(eores_OK);           
}


extern eOresult_t eo_inertials2_AcceptCANframe(EOtheInertials2 *p, eOas_inertial_type_t type, eOcanframe_t *frame, eOcanport_t port)
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
    if(eobool_false == p->service.running)
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
       
    
    eOcanmap_location_t loc = {0};    
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
    loc.insideindex = eocanmap_insideindex_none;
    
    uint16_t id = 999;

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
        des.code            = eoerror_code_get(eoerror_category_Skin, eoerror_value_IS_arrayofinertialdataoverflow);
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
    
    return(eores_OK);      
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// -- in here .... so that all things related to Inertial are in a unique place


extern void eoprot_fun_INIT_as_inertials_config(const EOnv* nv)
{
    eOas_inertial_config_t* config = eo_nv_RAM(nv);
    
    config->datarate = s_eo_default_inertialconfig.datarate;
    config->enabled = s_eo_default_inertialconfig.enabled;
}


extern void eoprot_fun_INIT_as_inertials_status(const EOnv* nv)
{
    eOas_inertial_status_t *status = eo_nv_RAM(nv);  
    
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
    
    if(eobool_false == p->service.running)
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


static eOresult_t s_eo_inertials2_TXstart(EOtheInertials2 *p)
{ 
    if(eobool_false == p->configured)
    {   // dont have a configured service
        return(eores_OK);
    }    
    
    if((0 == p->canmap_mtb_active[0]) && (0 == p->canmap_mtb_active[1]))
    {   // no mtb boards configured
        return(eores_OK);
    } 
    
    if(0 == p->sensorsconfig.enabled)
    {   // no mtb boards enabled
        return(eores_OK);
    } 
  
 
    icubCanProto_inertial_config_t canprotoconfig = {0};
    
    canprotoconfig.period = p->sensorsconfig.datarate;
    canprotoconfig.enabledsensors = icubCanProto_inertial_sensorflag_none;
    
    p->sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;
    p->sharedcan.command.type  = ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP;
    p->sharedcan.command.value = &canprotoconfig;

    eOcanmap_location_t location = {0};
    location.insideindex = eocanmap_insideindex_none;
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

    p->sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;
    p->sharedcan.command.type  = ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP;
    p->sharedcan.command.value = &canprotoconfig;
    
    
    eOcanmap_location_t location = {0};
    location.insideindex = eocanmap_insideindex_none;
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

    // send up diagnostics
    // debug -> "Object EOtheInertials2 has programmed the mtb. In param16 in msb there is the mode, in lsb there is datarate"    
               
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
    memset(p->fromcan2id, 99, sizeof(p->fromcan2id));
    memset(p->canmap_mtb_active, 0, sizeof(p->canmap_mtb_active));
    memset(p->canmap_mtb_accel_int, 0, sizeof(p->canmap_mtb_accel_int));
    memset(p->canmap_mtb_accel_ext, 0, sizeof(p->canmap_mtb_accel_ext));
    memset(p->canmap_mtb_gyros_ext, 0, sizeof(p->canmap_mtb_gyros_ext));
    

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
        }
    }    
    
}


static eOresult_t s_eo_inertials2_onstop_search4mtbs(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok)
{
    const eOmn_serv_configuration_t * servcfg = (const eOmn_serv_configuration_t *)par;
    EOtheInertials2 *p = &s_eo_theinertials2;
    
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
    p->diagnostics.errorDescriptor.par16             = servcfg->data.sk.skin.numofpatches;
    p->diagnostics.errorDescriptor.par64             = (servcfg->data.sk.skin.version.firmware.minor)       | (servcfg->data.sk.skin.version.firmware.major << 8) |
                                                       (servcfg->data.sk.skin.version.protocol.minor << 16) | (servcfg->data.sk.skin.version.protocol.major << 24) |
                                                       ((uint64_t)p->sharedcan.discoverytarget.canmap[eOcanport1] << 32) | ((uint64_t)p->sharedcan.discoverytarget.canmap[eOcanport2] << 48);
   
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
                
        uint16_t maskofmissingCAN1 = p->sharedcan.discoverytarget.canmap[0] & (~detection->replies[0]);
        uint16_t maskofmissingCAN2 = p->sharedcan.discoverytarget.canmap[1] & (~detection->replies[1]);
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

static eObool_t s_eo_inertials2_get_id(eOcanmap_location_t loc, eOas_inertial_type_t type, uint16_t *id)
{
    EOtheInertials2* p = &s_eo_theinertials2;
    
    if((eoas_inertial_accel_mtb_int != type) && (eoas_inertial_accel_mtb_ext != type) && (eoas_inertial_gyros_mtb_ext != type))
    {
        return(eobool_false);
    }
    
    uint16_t *candidates = p->fromcan2id[loc.port][loc.addr];
    uint8_t index = type - eoas_inertial_accel_mtb_int;
    
    *id = candidates[index];
    
    if((99 == *id) && (eoas_inertial_accel_mtb_int == type))
    {
        // it is possible that we have have configured an external accel ... try assigning it
        *id = candidates[eoas_inertial_accel_mtb_ext-eoas_inertial_accel_mtb_int];
    }
    
    if(99 == *id)
    {
        return(eobool_false);
    }
        
    return(eobool_true);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



