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

#include "EOMtheEMSappl.h"

#include "EOVtheCallbackManager.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheInertials.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheInertials_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

// marco.accame: we can later on retrieve the canmapofsupportedsensors[][] from a config message coming from robotInterface.
//               now, for short we dont do it and we store the configurations inside here.

const eOas_inertial_serviceconfig_t eo_inertials_cfg_eb2 = 
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

const eOas_inertial_serviceconfig_t eo_inertials_cfg_eb4 = 
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


const eOas_inertial_serviceconfig_t eo_inertials_cfg_eb10 = 
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


const eOas_inertial_serviceconfig_t eo_inertials_cfg_eb11 = 
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

static eObool_t s_eo_inertials_activestate_can_accept_canframe(void);

static void s_eo_inertials_build_maps(const eOas_inertial_serviceconfig_t* cfg);

static eObool_t s_eo_inertials_get_canlocation(eOas_inertial_position_t targetposition, eOcanmap_location_t *loc);

static eObool_t s_eo_inertials_get_sensor(eOcanmap_location_t location, eOas_inertial_position_t *position);

static eOresult_t s_eo_inertials_onstop_search4mtbs(void *par, EOtheCANdiscovery2* p, eObool_t searchisok);

static void s_eo_inertials_send_periodic_error_report(void *p);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const eOas_inertial_sensorsconfig_t s_eo_default_inertialsensorsconfig =
{
    .accelerometers         = 0,
    .gyroscopes             = 0,
    .datarate               = 50,
    .filler                 = {0}
};



static EOtheInertials s_eo_theinertials = 
{
    .service = 
    {
        .servconfig             = { .type = eomn_serv_NONE },
        .initted                = eobool_false,
        .active                 = eobool_false,
        .activateafterverify    = eobool_false,
        .running                = eobool_false,
        .onverify               = NULL            
    },
    .diagnostics = 
    {
        .reportTimer            = NULL,
        .reportPeriod           = 10*EOK_reltime1sec,
        .errorDescriptor        = {0},
        .errorType              = eo_errortype_info,
        .errorCallbackCount     = 0,
        .repetitionOKcase       = 10
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
    .serviceconfig              = {0},
    .supportedmask64            = 0,    
    .fifoofinertialdata         = NULL,
    
    .configured                 = eobool_false,

    .inertial2                  = NULL
};



static const char s_eobj_ownname[] = "EOtheInertials";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern EOtheInertials* eo_inertials_Initialise(void)
{
    if(eobool_true == s_eo_theinertials.service.initted)
    {
        return(&s_eo_theinertials);
    }
    
    s_eo_theinertials.service.active = eobool_false;
        
    s_eo_theinertials.numofmtbs = 0;
    s_eo_theinertials.service.servconfig.type = eomn_serv_NONE;
    
    
    s_eo_theinertials.sharedcan.boardproperties = eo_vector_New(sizeof(eOcanmap_board_properties_t), eo_inertials_maxnumberofMTBboards, NULL, NULL, NULL, NULL);
    
    s_eo_theinertials.sharedcan.entitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), eo_inertials_maxnumberofMTBboards, NULL, NULL, NULL, NULL);
    

    s_eo_theinertials.inertial2 = NULL;
    memcpy(&s_eo_theinertials.sensorsconfig, &s_eo_default_inertialsensorsconfig, sizeof(eOas_inertial_sensorsconfig_t));
    s_eo_theinertials.fifoofinertialdata = eo_vector_New(sizeof(eOas_inertial_data_t), 32, NULL, 0, NULL, NULL);
    
    s_eo_theinertials.diagnostics.reportTimer = eo_timer_New();
    s_eo_theinertials.diagnostics.errorType = eo_errortype_error;
    s_eo_theinertials.diagnostics.errorDescriptor.sourceaddress = eo_errman_sourcedevice_localboard;
    s_eo_theinertials.diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials_not_verified_yet);  
    
    s_eo_theinertials.service.initted = eobool_true;
    s_eo_theinertials.service.active = eobool_false;
    s_eo_theinertials.service.running = eobool_false;

    
    return(&s_eo_theinertials);   
}


extern EOtheInertials* eo_inertials_GetHandle(void)
{
    if(eobool_true == s_eo_theinertials.service.initted)
    {
        return(&s_eo_theinertials);
    }
    
    return(NULL);
}


extern eOresult_t eo_inertials_Verify(EOtheInertials *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify)
{
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    } 

    if(eomn_serv_AS_inertial != servcfg->type)
    {
        return(eores_NOK_generic);
    }   
    
 
// DONT Deactivate ... we may want just to check again ....    
//    if(eobool_true == s_eo_theinertials.service.active)
//    {
//        eo_inertials_Deactivate(p);        
//    }   
    
    // make sure the timer is not running
    eo_timer_Stop(s_eo_theinertials.diagnostics.reportTimer);    

    s_eo_theinertials.service.onverify = onverify;
    s_eo_theinertials.service.activateafterverify = activateafterverify;


    s_eo_theinertials.sharedcan.discoverytarget.info.type = eobrd_cantype_mtb;
    s_eo_theinertials.sharedcan.discoverytarget.info.protocol.major = servcfg->data.as.inertial.version.protocol.major; 
    s_eo_theinertials.sharedcan.discoverytarget.info.protocol.minor = servcfg->data.as.inertial.version.protocol.minor;
    s_eo_theinertials.sharedcan.discoverytarget.info.firmware.major = servcfg->data.as.inertial.version.firmware.major; 
    s_eo_theinertials.sharedcan.discoverytarget.info.firmware.minor = servcfg->data.as.inertial.version.firmware.minor;
    s_eo_theinertials.sharedcan.discoverytarget.info.firmware.build = servcfg->data.as.inertial.version.firmware.build;    
    
    // now i must do discovery of the mtb boards. the one used for the inertials are all in the canmap
    s_eo_theinertials.sharedcan.discoverytarget.canmap[eOcanport1] = servcfg->data.as.inertial.canmap[eOcanport1];
    s_eo_theinertials.sharedcan.discoverytarget.canmap[eOcanport2] = servcfg->data.as.inertial.canmap[eOcanport2];

    
    uint8_t numofboards = eo_common_hlfword_bitsetcount(s_eo_theinertials.sharedcan.discoverytarget.canmap[eOcanport1]) +
                          eo_common_hlfword_bitsetcount(s_eo_theinertials.sharedcan.discoverytarget.canmap[eOcanport2]);
    
    if(numofboards > eo_inertials_maxnumberofMTBboards)
    {
        
        s_eo_theinertials.diagnostics.errorDescriptor.sourcedevice       = eo_errman_sourcedevice_localboard;
        s_eo_theinertials.diagnostics.errorDescriptor.sourceaddress      = 0;
        s_eo_theinertials.diagnostics.errorDescriptor.par16              = (numofboards << 8) | (eo_inertials_maxnumberofMTBboards & 0x00ff);
        s_eo_theinertials.diagnostics.errorDescriptor.par64              = (s_eo_theinertials.sharedcan.discoverytarget.canmap[eOcanport2] << 16) | (s_eo_theinertials.sharedcan.discoverytarget.canmap[eOcanport1]);
       
        EOaction_strg astrg = {0};
        EOaction *act = (EOaction*)&astrg;
        eo_action_SetCallback(act, s_eo_inertials_send_periodic_error_report, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle()));        
        
        s_eo_theinertials.diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials_failed_toomanyboards);
        s_eo_theinertials.diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), s_eo_theinertials.diagnostics.errorType, NULL, s_eobj_ownname, &s_eo_theinertials.diagnostics.errorDescriptor);
        
        if(0 != s_eo_theinertials.diagnostics.reportPeriod)
        {
            s_eo_theinertials.diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(s_eo_theinertials.diagnostics.reportTimer, eok_abstimeNOW, s_eo_theinertials.diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);   
        }            
        
        return(eores_NOK_generic); 
    }
              
    

    s_eo_theinertials.sharedcan.ondiscoverystop.function = s_eo_inertials_onstop_search4mtbs;
    s_eo_theinertials.sharedcan.ondiscoverystop.parameter = (void*)servcfg;
    
    // start discovery   
    eo_candiscovery2_Start(eo_candiscovery2_GetHandle(), &s_eo_theinertials.sharedcan.discoverytarget, &s_eo_theinertials.sharedcan.ondiscoverystop);   
   
    return(eores_OK);   
}



extern eOresult_t eo_inertials_Deactivate(EOtheInertials *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == s_eo_theinertials.service.active)
    {
        return(eores_OK);        
    } 
    

    eo_inertials_Stop(&s_eo_theinertials);
    s_eo_theinertials.service.running = eobool_false;

        
    eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_inertial, s_eo_theinertials.sharedcan.entitydescriptor); 
    
    eo_canmap_UnloadBoards(eo_canmap_GetHandle(), s_eo_theinertials.sharedcan.boardproperties);
     
    
    eo_entities_SetNumOfInertials(eo_entities_GetHandle(), 0);
    
    s_eo_theinertials.numofmtbs = 0;
    
    s_eo_theinertials.inertial2 = NULL;
    
    eo_vector_Clear(s_eo_theinertials.fifoofinertialdata);
                
    memset(&s_eo_theinertials.service.servconfig, 0, sizeof(eOmn_serv_configuration_t));
    s_eo_theinertials.service.servconfig.type = eomn_serv_NONE;
    
    eo_vector_Clear(s_eo_theinertials.sharedcan.boardproperties);
    eo_vector_Clear(s_eo_theinertials.sharedcan.entitydescriptor);
    
    // make sure the timer is not running
    eo_timer_Stop(s_eo_theinertials.diagnostics.reportTimer);  
    
    s_eo_theinertials.service.active = eobool_false;
    
    return(eores_OK);
}



extern eOresult_t eo_inertials_Activate(EOtheInertials *p, const eOmn_serv_configuration_t * servcfg)
{
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    } 

    if(eomn_serv_AS_inertial != servcfg->type)
    {
        return(eores_NOK_generic);
    }
    
    if(eobool_true == s_eo_theinertials.service.active)
    {
        eo_inertials_Deactivate(p);        
    }   
    
    eo_entities_SetNumOfInertials(eo_entities_GetHandle(), 1);
    
    if(0 == eo_entities_NumOfInertials(eo_entities_GetHandle()))
    {
        s_eo_theinertials.service.active = eobool_false;
        return(eores_NOK_generic);
    }
    
       
    memcpy(&s_eo_theinertials.service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));


    s_eo_theinertials.inertial2 = eo_entities_GetInertial(eo_entities_GetHandle(), 0);
    
    
    // now i must add all the mtb boards. i iterate per canbus
    
    eOcanmap_board_properties_t prop = 
    {
        .type               = eobrd_cantype_mtb, 
        .location           = { .port = 0, .addr = 0, .insideindex = eocanmap_insideindex_none },
        .requiredprotocol   = { .major = servcfg->data.as.inertial.version.protocol.major, .minor = servcfg->data.as.inertial.version.protocol.minor }
    };  
    
    eOcanmap_entitydescriptor_t des = 
    {
        .location   = { .port = 0, .addr = 0, .insideindex = eocanmap_insideindex_none },
        .index      = entindex00
    };        
    
    s_eo_theinertials.numofmtbs = 0;
    
    uint8_t j = 0;
    uint8_t k = 0;

    for(j=0; j<eOcanports_number; j++)
    {
        uint16_t canbusmapping = servcfg->data.as.inertial.canmap[j];
        for(k=1; k<15; k++)
        {
            if(eobool_true == eo_common_hlfword_bitcheck(canbusmapping, k))
            {   // i pushback. i dont verify vs teh capacity of the vector because eo_inertials_Verify() has already done it
                prop.location.port = j;
                prop.location.addr = k;
                eo_vector_PushBack(s_eo_theinertials.sharedcan.boardproperties, &prop);
                
                des.location.port = j;
                des.location.addr = k;
                des.index = entindex00;                        
                eo_vector_PushBack(s_eo_theinertials.sharedcan.entitydescriptor, &des);
                
                s_eo_theinertials.numofmtbs++;
            }
        }
    }

    
    // load the can mapping 
    eo_canmap_LoadBoards(eo_canmap_GetHandle(), s_eo_theinertials.sharedcan.boardproperties); 
    
    // load the entity mapping.
    eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_inertial, s_eo_theinertials.sharedcan.entitydescriptor);   

    s_eo_theinertials.service.active = eobool_true;        

    
    return(eores_OK);   
}



extern eOresult_t eo_inertials_SendReport(EOtheInertials *p)
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


//#warning THINK of what to use instead of eo_inertials_ServiceConfig()
extern eOresult_t eo_inertials_ServiceConfig(EOtheInertials *p, const eOas_inertial_serviceconfig_t* cfg)
{    
    // we can configure the service how many times we want. however the object must be initted and must there be an inertial entity
 
    if((NULL == p) || (NULL == cfg))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertials.service.active)
    {   // nothing to do because we dont have a inertials service active
        return(eores_OK);
    }  
    
    
    if(eobool_true == s_eo_theinertials.service.running)
    {
        eo_inertials_Stop(p);
    }
    
    // ok, now we can build the maps.
    s_eo_inertials_build_maps(cfg);
    
    // and set it configured
    s_eo_theinertials.configured = eobool_true;
    
    return(eores_OK);  
}




extern eOresult_t eo_inertials_SensorsConfig(EOtheInertials *p, eOas_inertial_sensorsconfig_t* config)
{
    if((NULL == p) || (NULL == config))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertials.service.active)
    {   // nothing to do because we dont have a inertials service active
        return(eores_OK);
    }  
    
    if(eobool_false == s_eo_theinertials.configured)
    {
        // nothing to do because we dont have a configured object yet 
        return(eores_OK);
    }

    
    // ok, now we do something. 
    
    // at first we copy the target config into the local config
    
    s_eo_theinertials.sensorsconfig.datarate         = config->datarate;
    s_eo_theinertials.sensorsconfig.accelerometers   = config->accelerometers;
    s_eo_theinertials.sensorsconfig.gyroscopes       = config->gyroscopes;
    
    // then we check enabled mask and datarate
    
    // the enabled must be in bitwise AND with the supported ones
    s_eo_theinertials.sensorsconfig.accelerometers   &= s_eo_theinertials.supportedmask64;
    if(s_eo_theinertials.sensorsconfig.accelerometers != config->accelerometers)
    {
        // send up diagnostics
        // warning -> "Object EOtheInertials does not not support some accelerometers."               
    }
    
    if(0 == s_eo_theinertials.sensorsconfig.accelerometers)
    {
        // send up diagnostics
        // warning -> "Object EOtheInertials does not have a valid configuration."            
    }
    
    s_eo_theinertials.sensorsconfig.gyroscopes   &= s_eo_theinertials.supportedmask64;
    if(s_eo_theinertials.sensorsconfig.gyroscopes != config->gyroscopes)
    {
        // send up diagnostics
        // warning -> "Object EOtheInertials does not not support some gyroscopes."               
    }
    
    if(0 == s_eo_theinertials.sensorsconfig.gyroscopes)
    {
        // send up diagnostics
        // warning -> "Object EOtheInertials does not have a valid configuration."            
    }
    
    if(s_eo_theinertials.sensorsconfig.datarate < 10)
    {
        s_eo_theinertials.sensorsconfig.datarate = 10;
        // send up diagnostics
        // warning -> "Object EOtheInertials has changed the requested datarate."         
    }
    
    if(s_eo_theinertials.sensorsconfig.datarate > 200)
    {
        s_eo_theinertials.sensorsconfig.datarate = 200;
        // send up diagnostics
        // warning -> "Object EOtheInertials has changed the requested datarate."             
    }
    
 
    // ok guys: now we transmit the configuration to each one of the sensors which have a non-zero bit inside teh s_eo_theinertials.config2.enabled mask.
    // WAIT A MINUTE: we do it inside eo_inertials_Start() !!!!!!!!!!!!

    
    return(eores_OK);           
}


extern eOresult_t eo_inertials_Start(EOtheInertials *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertials.service.active)
    {   // nothing to do because we dont have an active service
        return(eores_OK);
    }  

    if(eobool_false == s_eo_theinertials.configured)
    {   // dont have a configured service
        return(eores_OK);
    }    

    // ok, now we do something.    

    if(eobool_true == s_eo_theinertials.service.running)
    {   // if transmitting we stop before
        eo_inertials_Stop(p);
    }       
    
    
    if((0 == s_eo_theinertials.sensorsconfig.accelerometers) && (0 == s_eo_theinertials.sensorsconfig.gyroscopes))
    {
        // send up diagnostics
        // error -> "Object EOtheInertials does not contain a valid configuration, thus will NOT activate any sensor."         
    } 

    
    eObool_t sentconfig = eobool_false;
 
    icubCanProto_inertial_config_t canprotoconfig = {0};
    
    canprotoconfig.period           = s_eo_theinertials.sensorsconfig.datarate;
    canprotoconfig.enabledsensors   = icubCanProto_inertial_sensorflag_none;
    
    s_eo_theinertials.sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_theinertials.sharedcan.command.type  = ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP;
    s_eo_theinertials.sharedcan.command.value = &canprotoconfig;

    uint8_t i = 0;
    eOcanmap_location_t location = {0};
    
    for(i=0; i<eoas_inertial_pos_max_numberof; i++)
    {
        canprotoconfig.enabledsensors = icubCanProto_inertial_sensorflag_none;

        if(1 == EOAS_ISPOSENABLED(s_eo_theinertials.sensorsconfig.accelerometers, i))
        {
            // for jorhabib: he needs the external accelerometer on feet. long term solution is to introduce an externalaccelerometers mask and an internalgyroscope mask
            //#define FOR_JORHABIB
            #if defined(FOR_JORHABIB)
            if((eoas_inertial_pos_l_foot_1 == i) || (eoas_inertial_pos_l_foot_2 == i) || (eoas_inertial_pos_r_foot_1 == i) || (eoas_inertial_pos_r_foot_2 == i))
            {
                canprotoconfig.enabledsensors |= icubCanProto_inertial_sensorflag_externaldigitalaccelerometer;
            }
            else
            {
                canprotoconfig.enabledsensors |= icubCanProto_inertial_sensorflag_internaldigitalaccelerometer;
            }
            #else
            canprotoconfig.enabledsensors |= icubCanProto_inertial_sensorflag_internaldigitalaccelerometer;
            #endif
        }

        if(1 == EOAS_ISPOSENABLED(s_eo_theinertials.sensorsconfig.gyroscopes, i))
        {
            canprotoconfig.enabledsensors |= icubCanProto_inertial_sensorflag_externaldigitalgyroscope;
        } 

        if(icubCanProto_inertial_sensorflag_none != canprotoconfig.enabledsensors)
        {
            // ok, we have at least a gyro or an accel or them both. thus we send the config to the can location
            eOas_inertial_position_t position = (eOas_inertial_position_t)i;
            if(eobool_true == s_eo_inertials_get_canlocation(position, &location))
            {
                sentconfig = eobool_true;
                eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &s_eo_theinertials.sharedcan.command, location);  
            } 
            
        }       
    }


    if(eobool_false == sentconfig)
    {
        s_eo_theinertials.service.running = eobool_false;
    }
    else
    {
        s_eo_theinertials.service.running = eobool_true;
    }
    
    // send up diagnostics
    // debug -> "Object EOtheInertials has programmed the mtb. In param16 in msb there is the mode, in lsb there is datarate"  
    
    return(eores_OK);  
    
}



//extern eOresult_t eo_inertials_StartOLD(EOtheInertials *p)
//{
//    if(NULL == p)
//    {
//        return(eores_NOK_nullpointer);
//    }
//    
//    if(eobool_false == s_eo_theinertials.thereisinertial)
//    {   // nothing to do because we dont have an inertial board
//        // send up diagnostics
//        // error -> "Object EOtheInertials does not not support any inertial, thus will NOT process any command."          
//        return(eores_OK);
//    } 

//    if(eobool_false == s_eo_theinertials.configured)
//    {
//        // nothing to do because we dont have a configured object et
//        // send up diagnostics
//        // error -> "Object EOtheInertials is not yet configured, thus will NOT process any command."   
//        return(eores_OK);
//    }    

//    // ok, now we do something.    

//    if(eobool_true == s_eo_theinertials.service.running)
//    {   // if transmitting we stop before
//        eo_inertials_Stop(p);
//    }       
//    
//    
//    if((0 == s_eo_theinertials.sensorsconfig.accelerometers) && (0 == s_eo_theinertials.sensorsconfig.gyroscopes))
//    {
//        // send up diagnostics
//        // error -> "Object EOtheInertials does not contain a valid configuration, thus will NOT activate any sensor."         
//    } 

//    
//    eObool_t sentconfig = eobool_false;
// 
//    icubCanProto_inertial_config_t canprotoconfig = {0};
//    
//    canprotoconfig.period           = s_eo_theinertials.sensorsconfig.datarate;
//    canprotoconfig.enabledsensors   = icubCanProto_inertial_sensorflag_none;
//    
//    s_eo_theinertials.command.class = eocanprot_msgclass_pollingAnalogSensor;
//    s_eo_theinertials.command.type  = ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP;
//    s_eo_theinertials.command.value = &canprotoconfig;

//    uint8_t i = 0;
//    eOcanmap_location_t location = {0};
//    
//    for(i=0; i<eoas_inertial_pos_max_numberof; i++)
//    {
//        canprotoconfig.enabledsensors = icubCanProto_inertial_sensorflag_none;

//        if(1 == EOAS_ISPOSENABLED(s_eo_theinertials.sensorsconfig.accelerometers, i))
//        {
//            // for jorhabib: he needs the external accelerometer on feet. long term solution is to introduce an externalaccelerometers mask and an internalgyroscope mask
//            //#define FOR_JORHABIB
//            #if defined(FOR_JORHABIB)
//            if((eoas_inertial_pos_l_foot_1 == i) || (eoas_inertial_pos_l_foot_2 == i) || (eoas_inertial_pos_r_foot_1 == i) || (eoas_inertial_pos_r_foot_2 == i))
//            {
//                canprotoconfig.enabledsensors |= icubCanProto_inertial_sensorflag_externaldigitalaccelerometer;
//            }
//            else
//            {
//                canprotoconfig.enabledsensors |= icubCanProto_inertial_sensorflag_internaldigitalaccelerometer;
//            }
//            #else
//            canprotoconfig.enabledsensors |= icubCanProto_inertial_sensorflag_internaldigitalaccelerometer;
//            #endif
//        }

//        if(1 == EOAS_ISPOSENABLED(s_eo_theinertials.sensorsconfig.gyroscopes, i))
//        {
//            canprotoconfig.enabledsensors |= icubCanProto_inertial_sensorflag_externaldigitalgyroscope;
//        } 

//        if(icubCanProto_inertial_sensorflag_none != canprotoconfig.enabledsensors)
//        {
//            // ok, we have at least a gyro or an accel or them both. thus we send the config to the can location
//            eOas_inertial_position_t position = (eOas_inertial_position_t)i;
//            if(eobool_true == s_eo_inertials_get_canlocation(position, &location))
//            {
//                sentconfig = eobool_true;
//                eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &s_eo_theinertials.command, location);  
//            } 
//            
//        }       
//    }


//    if(eobool_false == sentconfig)
//    {
//        s_eo_theinertials.service.running = eobool_false;
//    }
//    else
//    {
//        s_eo_theinertials.service.running = eobool_true;
//    }
//    
//    // send up diagnostics
//    // debug -> "Object EOtheInertials has programmed the mtb. In param16 in msb there is the mode, in lsb there is datarate"  
//    
//    return(eores_OK);  
//    
//}



extern eOresult_t eo_inertials_Stop(EOtheInertials *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertials.service.active)
    {   // nothing to do because we dont have an active service
        return(eores_OK);
    }  
    
    if(eobool_false == s_eo_theinertials.configured)
    {   // nothing to do because we dont have a configured service 
        return(eores_OK);
    }    

    // ok, now we do something.    

    // if it is not transmitting we dont send a useless command on can.
    
    if(eobool_false == s_eo_theinertials.service.running)
    {
         return(eores_OK);
    }

    icubCanProto_inertial_config_t canprotoconfig = {0};
    
    canprotoconfig.enabledsensors   = icubCanProto_inertial_sensorflag_none;
    canprotoconfig.period           = s_eo_theinertials.sensorsconfig.datarate;

    s_eo_theinertials.sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_theinertials.sharedcan.command.type  = ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP;
    s_eo_theinertials.sharedcan.command.value = &canprotoconfig;
    
    
    uint8_t i = 0;
    eOcanmap_location_t location = {0};
    
    for(i=0; i<eoas_inertial_pos_max_numberof; i++)
    {
        if((1 == EOAS_ISPOSENABLED(s_eo_theinertials.sensorsconfig.accelerometers, i)) || (1 == EOAS_ISPOSENABLED(s_eo_theinertials.sensorsconfig.gyroscopes, i)))
        {            
            eOas_inertial_position_t position = (eOas_inertial_position_t)i;            

            // fill location and send 
            if(eobool_true == s_eo_inertials_get_canlocation(position, &location))
            {
                eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &s_eo_theinertials.sharedcan.command, location);  
            }                                       
        }             
    }    
           
    
    s_eo_theinertials.service.running = eobool_false;
 
    // send up diagnostics
    // debug -> "Object EOtheInertials has programmed the mtb. In param16 in msb there is the mode, in lsb there is datarate"
    
    return(eores_OK);  
    
}

// not used
//extern eObool_t eo_inertials_IsTransmitting(EOtheInertials *p)
//{
//    if(NULL == p)
//    {
//        return(eobool_false);
//    }
//    
//    if(eobool_false == s_eo_theinertials.service.active)
//    {   // false because we have an inactive service
//        return(eobool_false);
//    }  

//    return(s_eo_theinertials.service.running);
//}    


extern eOresult_t eo_inertials_AcceptCANframe(EOtheInertials *p, eOas_inertial_type_t type, eOcanframe_t *frame, eOcanport_t port)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertials.service.active)
    {   // nothing to do because we dont have an active service        
        return(eores_OK);
    }  

    if(eobool_false == s_eo_theinertials.configured)
    {   // nothing to do because we dont have a configured service 
        return(eores_OK);
    }  
    
    if(eobool_false == s_eo_inertials_activestate_can_accept_canframe())
    {
        return(eores_OK);
    }
    
    // the inertial entity is in ...
    //s_eo_theinertials.inertial2 = s_eo_theinertials.inertial2;    
    // however, we dont use it ...
    // we put what we have inside a fifo of eOas_inertial_data_t
    eOas_inertial_data_t data = {0};
       
    
    eOcanmap_location_t loc = {0};    
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
    loc.insideindex = eocanmap_insideindex_none;
    
    eOas_inertial_position_t position = eoas_inertial_pos_none;

    if(eobool_false == s_eo_inertials_get_sensor(loc, &position))
    {
        // send up diagnostics
        // error -> "Object EOtheInertials has received a message from an unsupported inertial"
        return(eores_OK);        
    }
    
    if(eobool_true == eo_vector_Full(s_eo_theinertials.fifoofinertialdata))
    {
        // damn... a loss of can frames
        #warning TODO: add a proper message for loss of inertial can frames and remove eoerror_value_SK_arrayofcandataoverflow
        eOerrmanDescriptor_t des = {0};
        des.code            = eoerror_code_get(eoerror_category_Skin, eoerror_value_SK_arrayofcandataoverflow);
        des.par16           = (frame->id & 0x0fff) | ((frame->size & 0x000f) << 12);
        des.par64           = eo_common_canframe_data2u64((eOcanframe_t*)frame);
        des.sourceaddress   = EOCANPROT_FRAME_GET_SOURCE(frame);
        des.sourcedevice    = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &des);    
        
        // BUT dont want to lose NEW data ... i could just call a pop front to remove oldest data
        //eo_vector_PopFront(s_eo_theinertials.fifoofinertialdata);  
        // BUT .. as i dont want to througth data away, i decide that i put it inside the status.
        // in short: i call eo_inertials_Tick(p, true) which already does that        
        eo_inertials_Tick(p, eobool_true);      
    }
    
    data.timestamp =  eov_sys_LifeTimeGet(eov_sys_GetHandle());
    data.type = type;
    data.position = position;
    data.x = (int16_t)((frame->data[1]<<8) + frame->data[0]);
    data.y = (int16_t)((frame->data[3]<<8) + frame->data[2]);
    data.z = (int16_t)((frame->data[5]<<8) + frame->data[4]);
    
    // ok, now we can pushback data into the fifoofinertialdata
    
    eo_vector_PushBack(s_eo_theinertials.fifoofinertialdata, &data);

//    char str[128] = {0};    
//    int mms = (int)(data.timestamp/1000);
//    snprintf(str, sizeof(str), "\n added sensor type = %d @ t = %d ms", data.type, mms);
//    hal_trace_puts(str);  
    
    return(eores_OK);      
}

extern eOresult_t eo_inertials_Tick(EOtheInertials *p, eObool_t regularROPSjustTransmitted)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_theinertials.service.active)
    {   // nothing to do because we dont have an active service        
        return(eores_OK);
    }  

    if(eobool_false == s_eo_theinertials.configured)
    {   // nothing to do because we dont have a configured service 
        return(eores_OK);
    }    

    if(eobool_false == regularROPSjustTransmitted)
    {   // nothing to do because we cannot overwrite the status of inertial 
        return(eores_OK);
    }       
    
    // i can update the status of teh inertial because the old value has already been transmitted.
    
    
    // the status data of the inertial entity is in ...
    eOas_inertial_data_t *data = &s_eo_theinertials.inertial2->status.data;
    memset(data, 0, sizeof(eOas_inertial_data_t)); 
    
    if(eobool_true == eo_vector_Empty(s_eo_theinertials.fifoofinertialdata))
    {
        // just reset the status->data 
        data->type = eoas_inertial_type_none;
        data->position = eoas_inertial_pos_none;
        data->timestamp = eov_sys_LifeTimeGet(eov_sys_GetHandle());           
    }
    else
    {
        // else ... retrieve the item from fifoofinertialdata, copy it into status->data and remove it from fifoofinertialdata 
        eOas_inertial_data_t * item = (eOas_inertial_data_t*) eo_vector_Front(s_eo_theinertials.fifoofinertialdata);
        if(NULL != item)
        {
            memcpy(data, item, sizeof(eOas_inertial_data_t));   
            eo_vector_PopFront(s_eo_theinertials.fifoofinertialdata);   
        }
    }
    
    return(eores_OK);        
}



//extern eOresult_t eo_inertials_RefreshStatusOfEntity(EOtheInertials *p)
//{
//    if(NULL == p)
//    {
//        return(eores_NOK_nullpointer);
//    }
//    
//    if(eobool_false == s_eo_theinertials.service.active)
//    {   // nothing to do because we dont have an active service        
//        return(eores_OK);
//    }  
// 
//    if(eobool_false == s_eo_theinertials.configured)
//    {   // nothing to do because we dont have a configured service 
//        return(eores_OK);
//    }   
// 
//    // the inertial entity is in ...
//    eOas_inertial_data_t *data = &s_eo_theinertials.inertial2->status.data;
//    memset(data, 0, sizeof(eOas_inertial_data_t)); 
//    
//    if(eobool_true == eo_vector_Empty(s_eo_theinertials.fifoofinertialdata))
//    {
//        // just reset the status->data 
//        data->type = eoas_inertial_type_none;
//        data->position = eoas_inertial_pos_none;
//        data->timestamp = eov_sys_LifeTimeGet(eov_sys_GetHandle());           
//    }
//    else
//    {
//        // else ... retrieve the item from fifoofinertialdata, copy it into status->data and remove it from fifoofinertialdata 
//        eOas_inertial_data_t * item = (eOas_inertial_data_t*) eo_vector_Front(s_eo_theinertials.fifoofinertialdata);
//        if(NULL != item)
//        {
//            memcpy(data, item, sizeof(eOas_inertial_data_t));   
//            eo_vector_PopFront(s_eo_theinertials.fifoofinertialdata);   
//            
////            char str[128] = {0};
////            int ms = (int)(data->timestamp/1000);
////            int now = eov_sys_LifeTimeGet(eov_sys_GetHandle()) /  1000;
////            snprintf(str, sizeof(str), "\nstatus has: sensor type = %d @ t = %d ms. time now is %d", data->type, ms, now);
////            hal_trace_puts(str);
//        }
//    }
//    
//    return(eores_OK);        
//}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// -- in here .... so that all things related to Inertial are in a unique place


extern void eoprot_fun_INIT_as_inertials_config(const EOnv* nv)
{
    eOas_inertial_config_t* config = eo_nv_RAM(nv);
    
    memset(&config->service, 0, sizeof(eOas_inertial_serviceconfig_t));
    
    config->sensors.accelerometers  = s_eo_default_inertialsensorsconfig.accelerometers;
    config->sensors.gyroscopes      = s_eo_default_inertialsensorsconfig.gyroscopes;
    config->sensors.datarate        = s_eo_default_inertialsensorsconfig.datarate;
}


extern void eoprot_fun_INIT_as_inertials_status(const EOnv* nv)
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
    // i dont want to put data in the fifoofinertialdata when we are not in the control loop.

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

//#define EOINERTIALS_ALWAYS_ACCEPT_CANFRAMES_IN_ACTIVE_MODE
static eObool_t s_eo_inertials_activestate_can_accept_canframe(void)
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


static void s_eo_inertials_build_maps(const eOas_inertial_serviceconfig_t* cfg)
{
    // copy the new service configuration
    memcpy(&s_eo_theinertials.serviceconfig, cfg, sizeof(eOas_inertial_serviceconfig_t));
    
    // at first i reset the supported mask
    s_eo_theinertials.supportedmask64 = 0;
        
    // then i search for in can1 and can2 which sensors we are asked to support
    uint8_t i = 0;
    uint8_t j = 0;
    
    for(i=0; i<2; i++)
    {
        for(j=0; j<15; j++)
        {
            eOas_inertial_position_t position = (eOas_inertial_position_t)s_eo_theinertials.serviceconfig.canmapofsupportedsensors[i][j];
            if(eoas_inertial_pos_none != position)
            {
                // ok, we add this position to tthe supported mask
                s_eo_theinertials.supportedmask64 |= EOAS_ENABLEPOS(position);                
            }
        }
    }
       
}


static eObool_t s_eo_inertials_get_canlocation(eOas_inertial_position_t targetposition, eOcanmap_location_t *loc)
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
            eOas_inertial_position_t position = (eOas_inertial_position_t)s_eo_theinertials.serviceconfig.canmapofsupportedsensors[i][j];
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



static eObool_t s_eo_inertials_get_sensor(eOcanmap_location_t location, eOas_inertial_position_t *position)
{       
    eOas_inertial_position_t internalposition = (eOas_inertial_position_t)s_eo_theinertials.serviceconfig.canmapofsupportedsensors[location.port][location.addr];
    
    if(eoas_inertial_pos_none == internalposition)
    {
        return(eobool_false);
    }

    *position = internalposition;   
    return(eobool_true);
}




static eOresult_t s_eo_inertials_onstop_search4mtbs(void *par, EOtheCANdiscovery2* p, eObool_t searchisok)
{
    const eOmn_serv_configuration_t * servcfg = (const eOmn_serv_configuration_t *)par;
    
    if((eobool_true == searchisok) && (eobool_true == s_eo_theinertials.service.activateafterverify))
    {
        eo_inertials_Activate(&s_eo_theinertials, servcfg);        
    }

    s_eo_theinertials.diagnostics.errorDescriptor.sourcedevice      = eo_errman_sourcedevice_localboard;
    s_eo_theinertials.diagnostics.errorDescriptor.sourceaddress     = 0;
    s_eo_theinertials.diagnostics.errorDescriptor.par16             = servcfg->data.sk.skin.numofpatches;
    s_eo_theinertials.diagnostics.errorDescriptor.par64             = (servcfg->data.sk.skin.version.firmware.minor)       | (servcfg->data.sk.skin.version.firmware.major << 8) |
                                                                      (servcfg->data.sk.skin.version.protocol.minor << 16) | (servcfg->data.sk.skin.version.protocol.major << 24) |
                                                                      ((uint64_t)s_eo_theinertials.sharedcan.discoverytarget.canmap[eOcanport1] << 32) | ((uint64_t)s_eo_theinertials.sharedcan.discoverytarget.canmap[eOcanport2] << 48);
   
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, s_eo_inertials_send_periodic_error_report, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle()));

    if(eobool_true == searchisok)
    {        
        s_eo_theinertials.diagnostics.errorType = eo_errortype_debug;
        s_eo_theinertials.diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials_ok);
        eo_errman_Error(eo_errman_GetHandle(), s_eo_theinertials.diagnostics.errorType, NULL, s_eobj_ownname, &s_eo_theinertials.diagnostics.errorDescriptor);
        
        if((0 != s_eo_theinertials.diagnostics.repetitionOKcase) && (0 != s_eo_theinertials.diagnostics.reportPeriod))
        {
            s_eo_theinertials.diagnostics.errorCallbackCount = s_eo_theinertials.diagnostics.repetitionOKcase;        
            eo_timer_Start(s_eo_theinertials.diagnostics.reportTimer, eok_abstimeNOW, s_eo_theinertials.diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }
    
    
    if(eobool_false == searchisok)
    {   
        // in case we fail the discovery, it is better to put inside par64 the failure of discovery.
        // maskofmissing, maskofwronglydetected, 
        
        const eOcandiscovery_detection_t* detection = eo_candiscovery2_GetDetection(eo_candiscovery2_GetHandle());
                
        uint16_t maskofmissingCAN1 = s_eo_theinertials.sharedcan.discoverytarget.canmap[0] & (~detection->replies[0]);
        uint16_t maskofmissingCAN2 = s_eo_theinertials.sharedcan.discoverytarget.canmap[1] & (~detection->replies[1]);
        uint16_t maskofincompatibleCAN1 = detection->incompatibilities[0]; 
        uint16_t maskofincompatibleCAN2 = detection->incompatibilities[1]; 
        
        s_eo_theinertials.diagnostics.errorDescriptor.par64 = ((uint64_t)maskofmissingCAN1) | ((uint64_t)maskofmissingCAN2<<16) | ((uint64_t)maskofincompatibleCAN1<<32) | ((uint64_t)maskofincompatibleCAN2<<32);
        
        s_eo_theinertials.diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials_failed_candiscovery);
        s_eo_theinertials.diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), s_eo_theinertials.diagnostics.errorType, NULL, s_eobj_ownname, &s_eo_theinertials.diagnostics.errorDescriptor);
        
        if(0 != s_eo_theinertials.diagnostics.reportPeriod)
        {
            s_eo_theinertials.diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(s_eo_theinertials.diagnostics.reportTimer, eok_abstimeNOW, s_eo_theinertials.diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }  
    
    if(NULL != s_eo_theinertials.service.onverify)
    {
        s_eo_theinertials.service.onverify(&s_eo_theinertials, searchisok); 
    }    
    
    return(eores_OK);
    
}


static void s_eo_inertials_send_periodic_error_report(void *p)
{
    eo_errman_Error(eo_errman_GetHandle(), s_eo_theinertials.diagnostics.errorType, NULL, s_eobj_ownname, &s_eo_theinertials.diagnostics.errorDescriptor);

    if(eoerror_value_CFG_inertials_failed_candiscovery == eoerror_code2value(s_eo_theinertials.diagnostics.errorDescriptor.code))
    {   // if i dont find the mtbs, i keep on sending the discovery results up. it is a temporary diagnostics tricks until we use the verification of services at bootstrap
        eo_candiscovery2_SendLatestSearchResults(eo_candiscovery2_GetHandle());
    }
    
    if(EOK_int08dummy != s_eo_theinertials.diagnostics.errorCallbackCount)
    {
        s_eo_theinertials.diagnostics.errorCallbackCount--;
    }
    if(0 == s_eo_theinertials.diagnostics.errorCallbackCount)
    {
        eo_timer_Stop(s_eo_theinertials.diagnostics.reportTimer);
    }
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



