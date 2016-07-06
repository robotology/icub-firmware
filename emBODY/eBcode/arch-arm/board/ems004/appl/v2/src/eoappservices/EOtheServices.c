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

#include "EoCommon.h"

#include "EOMtheEMStransceiver.h"

#include "EOconstvector_hid.h"

#include "EOtheCANservice.h"
#include "EOtheCANprotocol.h"

#include "EOtheEntities.h"
#include "EOtheMotionController.h"
#include "EOtheSTRAIN.h"
#include "EOtheMAIS.h"
#include "EOtheSKIN.h"
#include "EOtheInertials2.h"

#include "EOtheETHmonitor.h"

#include "EOMtheEMSconfigurator.h"

#include "EOVtheCallbackManager.h"

#include "EOtheBoardConfig.h"

#include "EoError.h"

#include "EOMtheEMSappl.h"

#include "EoProtocolMN.h"

#include "EOtheBOARDtransceiver.h"

#include "testRTC.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheServices.h"


//#if !defined(EOSERVICES_USE_RUNTIME_ACTIVATION)
//// used only for activation at bootstrap ... dont use it for runtime config .. they are disabled ...
//extern eOresult_t eo_services_Activate(EOtheServices *p);

////extern eObool_t eo_services_startupactivation_AllActivated(EOtheServices *p);
////extern eOresult_t eo_services_startupactivation_SendFailureReport(EOtheServices *p);

//    #error DONT UNDEF EOSERVICES_USE_RUNTIME_ACTIVATION
//#endif

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheServices_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

//#define EOSERVICES_USE_RUNTIME_ACTIVATION

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

static void s_eo_services_initialise(EOtheServices *p);

// function used by EOtheCANservice
static void s_can_cbkonrx(void *arg);


// functions used by eo_services_ProcessCommand()
static eOresult_t s_eo_services_process_verifyactivate(EOtheServices *p, eOmn_serv_category_t category, const eOmn_serv_configuration_t* config);
static eOresult_t s_eo_services_process_start(EOtheServices *p, eOmn_serv_category_t category);
static eOresult_t s_eo_services_process_stop(EOtheServices *p, eOmn_serv_category_t category, eObool_t and_deactivate);
static eOresult_t s_eo_services_process_deactivate(EOtheServices *p, eOmn_serv_category_t category);
static eOresult_t s_eo_services_process_regsig(EOtheServices *p, eOmn_serv_category_t category, eOmn_serv_arrayof_id32_t* arrayofid32);
static eOresult_t s_eo_services_process_failure(EOtheServices *p, eOmn_serv_operation_t operation, eOmn_serv_category_t category);

// others
static eOresult_t s_eo_services_start(EOtheServices *p, eOmn_serv_category_t category);
static eOresult_t s_eo_services_stop(EOtheServices *p, eOmn_serv_category_t category, uint8_t *numofregulars, eOsmStatesEMSappl_t *state, eObool_t and_deactivate);

// function used to send result of command back to robotInterface
static void send_rop_command_result(void);

// other functions used to send specific results to robotInterface
static eOresult_t s_eo_services_alert_afterverify_service(eObool_t operationisok, eOmn_serv_category_t category, eOmn_serv_type_t type, eOservice_type_t servtype);

// functions used for runtime activation
static eOresult_t s_services_callback_afterverify_skin(EOaService* p, eObool_t operationisok);
static eOresult_t s_services_callback_afterverify_mais(EOaService* p, eObool_t operationisok);
static eOresult_t s_services_callback_afterverify_strain(EOaService* p, eObool_t operationisok);
static eOresult_t s_services_callback_afterverify_inertial(EOaService* p, eObool_t operationisok);
static eOresult_t s_services_callback_afterverify_motioncontrol(EOaService* p, eObool_t operationisok);
static eOresult_t s_eo_services_alert_afterverify_service(eObool_t operationisok, eOmn_serv_category_t category, eOmn_serv_type_t type, eOservice_type_t servtype);


//#if !defined(EOSERVICES_USE_RUNTIME_ACTIVATION)
// 
//// functions used for activation at startup .... you may undef them out
//static void s_services_startupactivation_activate_services_deferred(void);
//static void s_services_startupactivation_activate_services_now(void *p);
//static eOresult_t s_services_startupactivation_after_verify_motion(EOaService* p, eObool_t operationisok);
//static eOresult_t s_services_startupactivation_after_verify_mais(EOaService* p, eObool_t operationisok);
//static eOresult_t s_services_startupactivation_after_verify_strain(EOaService* p, eObool_t operationisok);
//static eOresult_t s_services_startupactivation_after_verify_skin(EOaService* p, eObool_t operationisok);
//static eOresult_t s_services_startupactivation_after_verify_inertials(EOaService* p, eObool_t operationisok);
// 
//#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
    
static EOtheServices s_eo_theservices = 
{
    .initted            = eobool_false,
    .nvset              = NULL,
    .timer              = NULL,
    .board              = eo_prot_BRDdummy,
    .mnservice          = NULL,
    .running            = {eobool_false},
    .ipaddress          = 0,
    .startupactivationstate = 
    {
        .allactivated       = eobool_false,
        .failedservice      = eo_service_none,
    }
};

static const char s_eobj_ownname[] = "EOtheServices";


static const eOprot_EPcfg_t s_eo_theservices_theEPcfgsOthersMAX[] =
{  
    {           
        .endpoint           = eoprot_endpoint_motioncontrol,
        .numberofentities   = {12, 12, 1, 0, 0, 0, 0}     
    },     
    {        
        .endpoint           = eoprot_endpoint_analogsensors,
        .numberofentities   = {1, 1, 1, 1, 0, 0, 0}        
    },
    {        
        .endpoint           = eoprot_endpoint_skin,
        .numberofentities   = {2, 0, 0, 0, 0, 0, 0}        
    }     
};

static const EOconstvector s_eo_theservices_vectorof_EPcfg_max = 
{
    .capacity       = sizeof(s_eo_theservices_theEPcfgsOthersMAX)/sizeof(eOprot_EPcfg_t),
    .size           = sizeof(s_eo_theservices_theEPcfgsOthersMAX)/sizeof(eOprot_EPcfg_t),
    .item_size      = sizeof(eOprot_EPcfg_t),
    .dummy          = 0,
    .stored_items   = (void*)s_eo_theservices_theEPcfgsOthersMAX,
    .functions      = NULL   
};

static const EOconstvector * s_eo_theservices_the_vectorof_EPcfgs = &s_eo_theservices_vectorof_EPcfg_max;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

#warning used to test spichainof3: remove code under TEST_SPICHAINOF3

#undef TEST_SPICHAINOF3
#if defined(TEST_SPICHAINOF3)
#include "EOmcController.h"
 
static const eOmn_serv_configuration_t s_serv_config_mc_cer_hand_xx =
{   // eb10
    .type       = eomn_serv_MC_mc4plus,
    .filler     = {0},
    .data.mc.mc4plus_based = 
    {
        .boardtype4mccontroller = emscontroller_board_CER_HAND,
        .filler                 = {0},
        .arrayofjomodescriptors =
        {
            .head   = 
            {
                .capacity       = 4,
                .itemsize       = sizeof(eOmn_serv_jomo_descriptor_t),
                .size           = 2,
                .internalmem    = 0                    
            },
            .data   =
            {
                { // joint 0: first
                    .actuator.pwm   =
                    {
                        .port   = eomn_serv_mc_port_mc4plus_pwmP3         
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_spichainof3,
                        .port   = eomn_serv_mc_port_mc4plus_spiP10,
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                        //.type   = eomn_serv_mc_sensor_none,
                        //.port   = 0, 
                        //.pos    = eomn_serv_mc_sensor_pos_none
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = eomn_serv_mc_port_mc4plus_qencP3,
                        .pos    = eomn_serv_mc_sensor_pos_atmotor
                    }
                },
                { // joint 1
                    .actuator.pwm   =
                    {
                        .port   = eomn_serv_mc_port_mc4plus_pwmP2             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_spichainof3,
                        .port   = eomn_serv_mc_port_mc4plus_spiP11,
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                        //.type   = eomn_serv_mc_sensor_none,
                        //.port   = 0, 
                        //.pos    = eomn_serv_mc_sensor_pos_none
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = eomn_serv_mc_port_mc4plus_qencP2,
                        .pos    = eomn_serv_mc_sensor_pos_atmotor
                        
                    }
                },                
                { // joint 2
                    .actuator.pwm   =
                    {
                        .port   = eomn_serv_mc_port_none
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = 0, 
                        .pos    = eomn_serv_mc_sensor_pos_none
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = 0, 
                        .pos    = eomn_serv_mc_sensor_pos_none
                    }
                },                
                { // joint 3
                    .actuator.pwm   =
                    {
                        .port   = eomn_serv_mc_port_none                          
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = 0, 
                        .pos    = eomn_serv_mc_sensor_pos_none
                    },
                    .extrasensor    =
                    {
                       .type   = eomn_serv_mc_sensor_none,
                        .port   = 0, 
                        .pos    = eomn_serv_mc_sensor_pos_none
                    }
                }            
            }
        },
        .jomocoupling       =
        {
            .joint2set      = 
            {   // each joint is on a different set 
                0, 1, 2, 3 
            },
            .joint2motor    = 
            {   // zero matrix: use matrix embedded in controller and seecetd by boardtype4mccontroller
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) }        
            },
            .joint2encoder  = 
            {   // identical matrix
                { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) } 
            }  
        }         
    }
 
};
#endif

extern EOtheServices* eo_services_Initialise(eOipv4addr_t ipaddress)
{
    if(eobool_true == s_eo_theservices.initted)
    {
        return(&s_eo_theservices);
    }
    EOtheServices *p = &s_eo_theservices;
    
    p->initted = eobool_true;
    p->nvset = eom_emstransceiver_GetNVset(eom_emstransceiver_GetHandle()); 
    p->timer = eo_timer_New();
    
    
    p->startupactivationstate.allactivated = eobool_false;
    p->startupactivationstate.failedservice = eo_service_none;
    p->mnservice = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_management, eoprot_entity_mn_service, 0);
    
    p->ipaddress = ipaddress;
    
    p->board = ipaddress >> 24; 
    if(0 == p->board)
    {
        p->board = 1;
        // and issue an error .......
    }    
    p->board --;
    
    // we initialise the services
    s_eo_services_initialise(p);


#if defined(TEST_SPICHAINOF3)
    {
    
        eOmn_service_cmmnds_command_t command = 
        {
            0           
        };
        
        command.operation = eomn_serv_operation_verifyactivate;
        command.category = eomn_serv_category_mc;
        memcpy(&command.parameter.configuration, &s_serv_config_mc_cer_hand_xx, sizeof(s_serv_config_mc_cer_hand_xx));
        
    
        // ok, we have received a command for a given service. we ask the object theservices to manage the thing
    
        eo_services_ProcessCommand(eo_services_GetHandle(), &command);
    }
#endif

#if defined(TESTRTC_IS_ACTIVE)    
    testRTC_init();
#endif
    
    return(&s_eo_theservices);
}
 

    
extern EOtheServices* eo_services_GetHandle(void)
{
    if(eobool_true == s_eo_theservices.initted)
    {
        return(&s_eo_theservices);
    }
    
    return(NULL);
}


extern eOmn_serv_state_t eo_service_GetState(EOtheServices *p, eOmn_serv_category_t category)
{
    eOmn_serv_state_t state = eomn_serv_state_notsupported;
    
    if(NULL == p)
    {
        return(state);
    }  

    switch(category)
    {
        
        case eomn_serv_category_mais:
        {
            state = eo_mais_GetServiceState(eo_mais_GetHandle());
        } break;
        
        case eomn_serv_category_mc:
        {
            state = eo_motioncontrol_GetServiceState(eo_motioncontrol_GetHandle());
        } break;
        
        case eomn_serv_category_strain:
        {
            state = eo_strain_GetServiceState(eo_strain_GetHandle());   
        } break;
        
        case eomn_serv_category_inertials:
        {
            state = eo_inertials2_GetServiceState(eo_inertials2_GetHandle());   
        } break;
        
        case eomn_serv_category_skin:
        {
            state = eo_skin_GetServiceState(eo_skin_GetHandle());
        } break;
        
        case eomn_serv_category_none:
        case eomn_serv_category_all:
        case eomn_serv_category_unknown:
        default:
        {
            state = eomn_serv_state_notsupported;
        } break;

    }

    return(state);    
}


extern eOresult_t eo_services_ProcessCommand(EOtheServices *p, eOmn_service_cmmnds_command_t *command)
{
    if((NULL == p) || (NULL == command))
    {
        return(eores_NOK_nullpointer);
    }
       
    eOmn_serv_operation_t operation = (eOmn_serv_operation_t)command->operation;
    eOmn_serv_category_t category = (eOmn_serv_category_t)command->category;
    const eOmn_serv_configuration_t *config = &command->parameter.configuration;
    eOmn_serv_arrayof_id32_t *arrayofid32 = &command->parameter.arrayofid32;
        
    switch(operation)
    {
        case eomn_serv_operation_verifyactivate:
        {
//            eo_errman_Trace(eo_errman_GetHandle(), "COMMAND: verifiactivate()", s_eobj_ownname);
            s_eo_services_process_verifyactivate(p, category, config);            
        } break;
        
        case eomn_serv_operation_start:
        {
//            eo_errman_Trace(eo_errman_GetHandle(), "COMMAND: start()", s_eobj_ownname);
            s_eo_services_process_start(p, category);
        } break;
        
        case eomn_serv_operation_stop:
        {
//            eo_errman_Trace(eo_errman_GetHandle(), "COMMAND: stop()", s_eobj_ownname);            
            s_eo_services_process_stop(p, category, eobool_true);
        } break;
        
        case eomn_serv_operation_deactivate:
        {
//            eo_errman_Trace(eo_errman_GetHandle(), "COMMAND: deactivate()", s_eobj_ownname);            
            s_eo_services_process_deactivate(p, category);
        } break;        
        
        case eomn_serv_operation_regsig_load:
        {
//            eo_errman_Trace(eo_errman_GetHandle(), "COMMAND: load regsig()", s_eobj_ownname);            
            s_eo_services_process_regsig(p, category, arrayofid32);
        } break;        

        case eomn_serv_operation_regsig_clear:
        {
//            eo_errman_Trace(eo_errman_GetHandle(), "COMMAND: clear regsig()", s_eobj_ownname);            
            s_eo_services_process_regsig(p, category, NULL);
        } break;  
        
        default:
        {
//            eo_errman_Trace(eo_errman_GetHandle(), "COMMAND: failure()", s_eobj_ownname);
            // send failure
            s_eo_services_process_failure(p, operation, category);
        } break;
        
    }
    
    return(eores_OK);
}



//#if !defined(EOSERVICES_USE_RUNTIME_ACTIVATION)
//// it is the previous eo_services_StartLegacyMode()
//extern eOresult_t eo_services_Activate(EOtheServices *p)
//{
//    if((NULL == p))
//    {
//        return(eores_NOK_nullpointer);
//    }
//   
//    // activation is done by a timer which will expire in 100 ms.
//    s_services_startupactivation_activate_services_deferred();
// 
//    return(eores_OK);
//}
// 
////extern eObool_t eo_services_startupactivation_AllActivated(EOtheServices *p)
////{    
////    if(NULL == p)
////    {
////        return(eobool_false);
////    }    
////        
////    return(s_eo_theservices.startupactivationstate.allactivated);    
////}
////  
////  
////  
//// extern eOresult_t eo_services_startupactivation_SendFailureReport(EOtheServices *p)
////{    
////    if(NULL == p)
////    {
////        return(eores_NOK_nullpointer);
////    }
////  
////    if(eobool_true == p->allactivated)
////    {
////        // so far dont send any OK report
////        return(eores_OK);
////    }
////        
////    switch(s_eo_theservices.startupactivationstate.failedservice)
////    {
////        case eo_service_mc:
////        {
////            eo_motioncontrol_SendReport(eo_motioncontrol_GetHandle());
////        } break;
////  
////        case eo_service_strain:
////        {
////            eo_strain_SendReport(eo_strain_GetHandle());
////        } break;
////        
////        case eo_service_mais:
////        {
////            eo_mais_SendReport(eo_mais_GetHandle());            
////        } break; 
////  
////        case eo_service_skin:
////        {
////            eo_skin_SendReport(eo_skin_GetHandle());
////        } break;   
////   
////   
////        case eo_service_inertial:
////        {
////            eo_inertials2_SendReport(eo_inertials2_GetHandle());   
////        } break;        
////        
////        case eo_service_none:
////        {
////            // send message that the services are not verified yet.           
////            eOerrmanDescriptor_t errorDescriptor = {0};
////            errorDescriptor.sourceaddress = eo_errman_sourcedevice_localboard;
////            errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_services_not_verified_yet);
////            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errorDescriptor);            
////        } break;
////        
////        default:
////        {
////            
////        } break;
////        
////    }
////        
////    return(eores_OK);    
////}
//   
//#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


extern void eoprot_fun_INIT_mn_service_wholeitem(const EOnv* nv)
{    
    eOmn_service_t* mnservice = eo_nv_RAM(nv);
    
    // we init the stateofservice with eomn_serv_state_notsupported. 
    // the _Initialise() funtion of mais, strain etc (if called) will change it to eomn_serv_state_idle
    // then, those object will move the state according to what happens
    memset(&mnservice->status.stateofservice, eomn_serv_state_notsupported, sizeof(mnservice->status.stateofservice));    
    
    memset(&mnservice->status.filler, 3, sizeof(mnservice->status.filler));
 
    mnservice->status.commandresult.operation = eomn_serv_operation_none;
}


extern eOresult_t eo_service_hid_SynchServiceState(EOtheServices *p, eOmn_serv_category_t category, eOmn_serv_state_t servstate)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }   
    
    if((eomn_serv_category_none == category) || (eomn_serv_category_all == category) || (eomn_serv_category_unknown == category))
    {
        return(eores_NOK_generic);
    }
    
    p->mnservice->status.stateofservice[category] = servstate;
    
    return(eores_OK);
}


extern eOresult_t eo_service_hid_SetRegulars(EOarray* id32ofregulars, eOmn_serv_arrayof_id32_t* arrayofid32, eObool_t (*isID32relevant)(uint32_t), uint8_t* numberofthem)
{
    EOarray* id32array = (EOarray*)arrayofid32;    
        
    uint8_t size = 0;
    uint8_t i = 0;
    
    EOtransceiver* boardtransceiver = eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle());
    
    eOropdescriptor_t ropdesc;
    memcpy(&ropdesc.control, &eok_ropctrl_basic, sizeof(eOropctrl_t));
    ropdesc.control.plustime        = eobool_false;
    ropdesc.control.plussign        = eobool_false;
    ropdesc.ropcode                 = eo_ropcode_sig;
    ropdesc.id32                    = eo_prot_ID32dummy;    
    ropdesc.signature               = eo_rop_SIGNATUREdummy;  
    uint32_t* id32 = NULL;
    
    // at first we remove all regulars inside id32ofregulars and we reset it
    size = eo_array_Size(id32ofregulars);
    for(i=0; i<size; i++)
    {
        id32 = (uint32_t*)eo_array_At(id32ofregulars, i);
        if(NULL != id32)
        {
            ropdesc.id32 = *id32;
            eo_transceiver_RegularROP_Unload(boardtransceiver, &ropdesc);
        }       
    }    
    eo_array_Reset(id32ofregulars);
    
    
    // then i load the new id32s ... if there are any
    if((NULL != id32array) && (0 != eo_array_Size(id32array)))
    {
        // get all the id32 from id32array (but not more than ...) and: 1. push back into id32ofregulars, 2. load the regular
        size = eo_array_Size(id32array);
        
        eOropdescriptor_t ropdesc;
        memcpy(&ropdesc.control, &eok_ropctrl_basic, sizeof(eOropctrl_t));
        ropdesc.control.plustime    = 0;
        ropdesc.control.plussign    = 0;
        ropdesc.ropcode             = eo_ropcode_sig;
        ropdesc.signature           = eo_rop_SIGNATUREdummy;   
        
        for(i=0; i<size; i++)
        {
            id32 = (uint32_t*)eo_array_At(id32array, i);
            if(NULL != id32)
            { 
                // filter them 
                eObool_t itisrelevant = eobool_true;
                if(NULL != isID32relevant)
                {
                    itisrelevant = isID32relevant(*id32);                   
                }
                
                if(eobool_true == itisrelevant)
                {
                    ropdesc.id32 = *id32;     
                    if(eores_OK == eo_transceiver_RegularROP_Load(boardtransceiver, &ropdesc))
                    {
                        eo_array_PushBack(id32ofregulars, id32);
                        if(eobool_true == eo_array_Full(id32ofregulars))
                        {   // cannot add any more regulars
                            break;
                        }
                    }
                    else
                    {
                        
                    }
                   
                }                
            }           
        }
    }
    
    if(NULL != numberofthem)
    {
        *numberofthem = eo_array_Size(id32ofregulars);
    }
    
    return(eores_OK);     
}


extern eOresult_t eo_service_hid_AddRegulars(EOarray* id32ofregulars, eOmn_serv_arrayof_id32_t* arrayofid32, eObool_t (*isID32relevant)(uint32_t), uint8_t* numberofthem)
{
    EOarray* id32array = (EOarray*)arrayofid32;    
        
    uint8_t size = 0;
    uint8_t i = 0;
    
    EOtransceiver* boardtransceiver = eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle());
    
    eOropdescriptor_t ropdesc;
    memcpy(&ropdesc.control, &eok_ropctrl_basic, sizeof(eOropctrl_t));
    ropdesc.control.plustime        = eobool_false;
    ropdesc.control.plussign        = eobool_false;
    ropdesc.ropcode                 = eo_ropcode_sig;
    ropdesc.id32                    = eo_prot_ID32dummy;    
    ropdesc.signature               = eo_rop_SIGNATUREdummy;  
    uint32_t* id32 = NULL;
    

    // then i load the new id32s ... if there are any
    if((NULL != id32array) && (0 != eo_array_Size(id32array)))
    {
        // get all the id32 from id32array (but not more than ...) and: 1. push back into id32ofregulars, 2. load the regular
        size = eo_array_Size(id32array);
        
        eOropdescriptor_t ropdesc;
        memcpy(&ropdesc.control, &eok_ropctrl_basic, sizeof(eOropctrl_t));
        ropdesc.control.plustime    = 0;
        ropdesc.control.plussign    = 0;
        ropdesc.ropcode             = eo_ropcode_sig;
        ropdesc.signature           = eo_rop_SIGNATUREdummy;   
        
        for(i=0; i<size; i++)
        {
            id32 = (uint32_t*)eo_array_At(id32array, i);
            if(NULL != id32)
            { 
                // filter them 
                eObool_t itisrelevant = eobool_true;
                if(NULL != isID32relevant)
                {
                    itisrelevant = isID32relevant(*id32);                   
                }
                
                if(eobool_true == itisrelevant)
                {
                    ropdesc.id32 = *id32;     
                    if(eores_OK == eo_transceiver_RegularROP_Load(boardtransceiver, &ropdesc))
                    {
                        eo_array_PushBack(id32ofregulars, id32);
                        if(eobool_true == eo_array_Full(id32ofregulars))
                        {   // cannot add any more regulars
                            break;
                        }
                    }
                    else
                    {
                        
                    }
                   
                }                
            }           
        }
    }
    
    if(NULL != numberofthem)
    {
        *numberofthem = eo_array_Size(id32ofregulars);
    }
    
    return(eores_OK);     
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_can_cbkonrx(void *arg)
{
    EOMtask *task = (EOMtask *)arg;
    eom_task_isrSetEvent(task, emsconfigurator_evt_userdef00);
}

static void s_eo_services_initialise(EOtheServices *p)
{
    eOprotBRD_t brd = p->board;
    
    {   // A. protocol: max capabilities
        
        // 1. set the board number. the value of the generic board is 99. 
        //    the correct value is used only for retrieving it later on and perform specific actions based on the board number
        eo_nvset_BRDlocalsetnumber(p->nvset, brd);
        
        // 2. i initialise the nvset at max capabilities
        uint16_t numofepcfgs = eo_constvector_Size(s_eo_theservices_the_vectorof_EPcfgs);
        uint8_t i = 0;
        for(i=0; i<numofepcfgs; i++)
        {
            eOprot_EPcfg_t* epcfg = (eOprot_EPcfg_t*) eo_constvector_At(s_eo_theservices_the_vectorof_EPcfgs, i);
            if(eobool_true == eoprot_EPcfg_isvalid(epcfg))
            {
                eo_nvset_LoadEP(p->nvset, epcfg, eobool_true);
            }                        
        }
        
        // 3. the callbacks
        // marco.accame on 30 sept 2015: so far i define all the callbacks. however:
        // a. we may decide to define EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME and thus we must later on to load a proper callback. 
        //    BUT maybe better not.
        // b. if not, i MUST later on re-write the callbacks, so that:
        //    a. we can understand if a service is configured (use proper object) and we something only if it configured.
        //    b. make sure that when we use a get entity, we use EOtheEntities which does not address joints beyond those configured
        
    }   // A.
    
    {   // B. the entities: only initted but not started or activated yets
        
        eo_entities_Initialise();
        eo_canmap_Initialise(NULL);
        eo_canprot_Initialise(NULL);
        eo_strain_Initialise();  
        eo_mais_Initialise();        
        eo_motioncontrol_Initialise();    
        eo_skin_Initialise(); 
        eo_inertials2_Initialise();               
    }
    
    {   // C.  can services and discovery.
        // so far i do in here what i need without any container
             
        // can-services
        eOcanserv_cfg_t config = {.mode = eocanserv_mode_straight};   
        
        config.mode                 = eocanserv_mode_straight;
        config.canstabilizationtime = 7*eok_reltime1sec;
        config.rxqueuesize[0]       = 64;
        config.rxqueuesize[1]       = 64;
        config.txqueuesize[0]       = 64;
        config.txqueuesize[1]       = 64;  
        config.onrxcallback[0]      = s_can_cbkonrx; 
        config.onrxargument[0]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());    
        config.onrxcallback[1]      = s_can_cbkonrx; 
        config.onrxargument[1]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()); 

            
        // inside eo_canserv_Initialise() it is called hal_can_supported_is(canx) to see if we can init the can bus as requested.
        eo_canserv_Initialise(&config);   
        
        // can-discovery
        eo_candiscovery2_Initialise(NULL);        
    } 

    {   // D. ethmonitor: init and start
        eo_ethmonitor_Initialise(NULL);        
        eo_ethmonitor_Start(eo_ethmonitor_GetHandle());
    }
    
}


static eOresult_t s_eo_services_alert_afterverify_service(eObool_t operationisok, eOmn_serv_category_t category, eOmn_serv_type_t type, eOservice_type_t servtype)
{
    s_eo_theservices.mnservice->status.commandresult.category = category;
    s_eo_theservices.mnservice->status.commandresult.operation = eomn_serv_operation_verifyactivate;
    s_eo_theservices.mnservice->status.commandresult.type = type;   
    
    if(eobool_false == operationisok)
    {        
        s_eo_theservices.mnservice->status.commandresult.latestcommandisok = eobool_false;    
        s_eo_theservices.mnservice->status.commandresult.data[0] = eomn_serv_state_failureofverify;
        
        // also mark it as failed ... maybe to be removed
        s_eo_theservices.startupactivationstate.failedservice = servtype;
    }
    else
    {
        s_eo_theservices.mnservice->status.commandresult.latestcommandisok = eobool_true;     
        s_eo_theservices.mnservice->status.commandresult.data[0] = eomn_serv_state_activated;
    }
    
    send_rop_command_result();       
    
    return(eores_OK);       
}


static eOresult_t s_services_callback_afterverify_inertial(EOaService* p, eObool_t operationisok)
{
    if(eobool_false == operationisok)
    {
        eo_inertials2_SendReport(eo_inertials2_GetHandle());
        eo_inertials2_Deactivate(eo_inertials2_GetHandle());
    }
    
    s_eo_services_alert_afterverify_service(operationisok, eomn_serv_category_inertials, eomn_serv_AS_inertials, eo_service_inertials);
       
    return(eores_OK);
}


static eOresult_t s_services_callback_afterverify_mais(EOaService* p, eObool_t operationisok)
{   
    if(eobool_false == operationisok)
    {
        eo_mais_SendReport(eo_mais_GetHandle());
        eo_mais_Deactivate(eo_mais_GetHandle());
    }

    s_eo_services_alert_afterverify_service(operationisok, eomn_serv_category_mais, eomn_serv_AS_mais, eo_service_mais);
    
    return(eores_OK);    
}


static eOresult_t s_services_callback_afterverify_strain(EOaService* p, eObool_t operationisok)
{
    if(eobool_false == operationisok)
    {
        eo_strain_SendReport(eo_strain_GetHandle());
        eo_strain_Deactivate(eo_strain_GetHandle());
    }
    
    s_eo_services_alert_afterverify_service(operationisok, eomn_serv_category_strain, eomn_serv_AS_strain, eo_service_strain);
      
    return(eores_OK);
}


static eOresult_t s_services_callback_afterverify_skin(EOaService* p, eObool_t operationisok)
{   
    if(eobool_false == operationisok)
    {
        eo_skin_SendReport(eo_skin_GetHandle());
        eo_skin_Deactivate(eo_skin_GetHandle());
    }

    s_eo_services_alert_afterverify_service(operationisok, eomn_serv_category_skin, eomn_serv_SK_skin, eo_service_skin);
    
    return(eores_OK);
}


static eOresult_t s_services_callback_afterverify_motioncontrol(EOaService* p, eObool_t operationisok)
{
    if(eobool_false == operationisok)
    {
        eo_motioncontrol_SendReport(eo_motioncontrol_GetHandle());
        eo_motioncontrol_Deactivate(eo_motioncontrol_GetHandle());
    }
    
    s_eo_services_alert_afterverify_service(operationisok, eomn_serv_category_mc, eomn_serv_MC_generic, eo_service_mc);
    
    return(eores_OK);
}



static eOresult_t s_eo_services_process_verifyactivate(EOtheServices *p, eOmn_serv_category_t category, const eOmn_serv_configuration_t* config)
{    
    eObool_t uselocalconfig = (eomn_serv_NONE == config->type) ? (eobool_true) : (eobool_false);
    eOerrmanDescriptor_t errorDescriptor = {0};
    errorDescriptor.sourceaddress = eo_errman_sourcedevice_localboard;
    
    switch(category)
    {   
        case eomn_serv_category_mc:
        {
            if(eobool_true == uselocalconfig)
            {
#undef TEST_14APR16 
#if defined(TEST_14APR16)        
                config = eoboardconfig_code2motion_serv_configuration(1); // eb2
#else
                config = eoboardconfig_code2motion_serv_configuration(s_eo_theservices.board);
#endif 
                
                errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_using_onboard_config);
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &errorDescriptor);                            
            }

            // if it is NULL, every function of kind _Verify() issues a failure
            eo_motioncontrol_Verify(eo_motioncontrol_GetHandle(), config, s_services_callback_afterverify_motioncontrol, eobool_true);            
        } break;
        
        case eomn_serv_category_inertials:
        {
            if(eobool_true == uselocalconfig)
            {
                config = eoboardconfig_code2inertials_serv_configuration(s_eo_theservices.board);
                
                errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_inertials_using_onboard_config);
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &errorDescriptor);                    
            }
            eo_inertials2_Verify(eo_inertials2_GetHandle(), config, s_services_callback_afterverify_inertial, eobool_true);            
        } break; 
        
        case eomn_serv_category_strain: 
        {
            if(eobool_true == uselocalconfig)  
            {
                config = eoboardconfig_code2strain_serv_configuration(s_eo_theservices.board);
                
                errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_strain_using_onboard_config);
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &errorDescriptor);                    
            }                
            eo_strain_Verify(eo_strain_GetHandle(), config, s_services_callback_afterverify_strain, eobool_true);
        } break;

        case eomn_serv_category_mais: 
        {
            if(eobool_true == uselocalconfig)  
            {
                config = eoboardconfig_code2mais_serv_configuration(s_eo_theservices.board);
                
                errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mais_using_onboard_config);
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &errorDescriptor);                    
            }               
            eo_mais_Verify(eo_mais_GetHandle(), config, s_services_callback_afterverify_mais, eobool_true);
        } break;

        case eomn_serv_category_skin:
        {
            if(eobool_true == uselocalconfig)  
            {
                config = eoboardconfig_code2skin_serv_configuration(s_eo_theservices.board);
                
                errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_skin_using_onboard_config);
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &errorDescriptor);                    
            }               
            eo_skin_Verify(eo_skin_GetHandle(), config, s_services_callback_afterverify_skin, eobool_true);            
        } break;
        
        default:
        case eomn_serv_category_none:
        case eomn_serv_category_all:
        case eomn_serv_category_unknown:
        {
            // send a failure about wrong param
            p->mnservice->status.commandresult.latestcommandisok = eobool_false;
            p->mnservice->status.commandresult.category = category;
            p->mnservice->status.commandresult.operation = eomn_serv_operation_verifyactivate;
            p->mnservice->status.commandresult.type = config->type;
            p->mnservice->status.commandresult.data[0] = eomn_serv_state_notsupported;
            
            send_rop_command_result();               
        } break;        
        
    }
       
    return(eores_OK);
}



static eOresult_t s_eo_services_process_start(EOtheServices *p, eOmn_serv_category_t category)
{
    // check if the category is ok and if it is supported. 
    
    // in here we must put for for two phases: 
    // phase1: all services are verified and activated at bootstrap. teh command verifyactivate does nothing apart sending a ok/nok back to robotInterface
    // phase2: at bootstratp service are just _Initalise()-ed and every verification-activation is done in here. 
    
    // if not supported send up a negative reply.
    if((eomn_serv_category_none == category) || (eomn_serv_category_all == category) || (eomn_serv_category_unknown == category))
    {
        // send a failure about wrong param
        p->mnservice->status.commandresult.latestcommandisok = eobool_false;
        p->mnservice->status.commandresult.category = category;
        p->mnservice->status.commandresult.operation = eomn_serv_operation_start;
        p->mnservice->status.commandresult.type = eomn_serv_NONE;
        p->mnservice->status.commandresult.data[0] = eomn_serv_state_notsupported;
        
        send_rop_command_result();   
        
        return(eores_OK);
    }
    
    // now have a look at state. it must be eomn_serv_state_activated or eomn_serv_state_started  
    if((eomn_serv_state_started == p->mnservice->status.stateofservice[category]) || (eomn_serv_state_activated == p->mnservice->status.stateofservice[category]))
    {     
        // ok
    } 
    else
    {
        // send a failure about wrong state of the board ... dont send it if the service is already started.
        p->mnservice->status.commandresult.latestcommandisok = eobool_false;
        p->mnservice->status.commandresult.category = category;
        p->mnservice->status.commandresult.operation = eomn_serv_operation_start;
        p->mnservice->status.commandresult.type = eomn_serv_NONE;        
        p->mnservice->status.commandresult.data[0] = p->mnservice->status.stateofservice[category];
        
        send_rop_command_result();   
        
        return(eores_OK);
    }
    
    
    
    // now we start the service.
    if(eores_OK == s_eo_services_start(p, category))
    {
        p->mnservice->status.commandresult.latestcommandisok = eobool_true;        
    }
    else
    {
        p->mnservice->status.commandresult.latestcommandisok = eobool_false;
    }
    
    // ok, we have it activated. is the config coherent with what we already have? for instance ... the eOmn_serv_type_t, the number of ...?
    // we decide for now to avoid that.
    
    // we just send the command up telling that things are ok.
    
    p->mnservice->status.commandresult.category = category;
    p->mnservice->status.commandresult.operation = eomn_serv_operation_start;
    p->mnservice->status.commandresult.type = eomn_serv_NONE;
    p->mnservice->status.commandresult.data[0] = p->mnservice->status.stateofservice[category];
    
    send_rop_command_result();    
    
    return(eores_OK);        
}

static eOresult_t s_eo_services_process_deactivate(EOtheServices *p, eOmn_serv_category_t category)
{
    return(s_eo_services_process_stop(p, category, eobool_true));
}

static eOresult_t s_eo_services_process_stop(EOtheServices *p, eOmn_serv_category_t category, eObool_t and_deactivate)
{
    if((eomn_serv_category_none == category) || (eomn_serv_category_unknown == category))
    {
        // send a failure about wrong param
        p->mnservice->status.commandresult.latestcommandisok = eobool_false;
        p->mnservice->status.commandresult.category = category;
        p->mnservice->status.commandresult.operation = eomn_serv_operation_stop;
        p->mnservice->status.commandresult.type = eomn_serv_NONE;
        p->mnservice->status.commandresult.data[0] = eomn_serv_state_notsupported;
        
        send_rop_command_result();   
        
        return(eores_OK);
    }
    
    uint8_t numofregulars = 0;
    eOsmStatesEMSappl_t state = eo_sm_emsappl_STcfg;
    
    // now we stop the service.
    if(eores_OK == s_eo_services_stop(p, category, &numofregulars, &state, and_deactivate))
    {
        p->mnservice->status.commandresult.latestcommandisok = eobool_true;        
    }
    else
    {
        p->mnservice->status.commandresult.latestcommandisok = eobool_false;
    }
    
    p->mnservice->status.commandresult.category = category;
    p->mnservice->status.commandresult.operation = eomn_serv_operation_stop;
    p->mnservice->status.commandresult.type = eomn_serv_NONE;
    
#if 0
    // todo: in here we have some extra results which robot-interface can parse and print.... add it later on.
    eOmn_service_command_stop_result_data_t resdata = {0};
    resdata.applstate = (eOmn_appl_state_t)state;
    resdata.totalregulars = eo_transceiver_RegularROP_ArrayID32Size(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
    resdata.serviceregulars = numofregulars;
    memcpy(p->mnservice->status.commandresult.data, &resdata, sizeof(resdata));
#endif
    
    send_rop_command_result();    
    
    return(eores_OK);        
}


static eOresult_t s_eo_services_process_regsig(EOtheServices *p, eOmn_serv_category_t category, eOmn_serv_arrayof_id32_t* arrayofid32)
{
    uint8_t number = 0;

    if((eomn_serv_category_none == category) || (eomn_serv_category_unknown == category))
    {
        // send a failure about wrong param
        p->mnservice->status.commandresult.latestcommandisok = eobool_false;
        p->mnservice->status.commandresult.category = category;
        p->mnservice->status.commandresult.operation = (NULL != arrayofid32) ? (eomn_serv_operation_regsig_load) : (eomn_serv_operation_regsig_clear);
        p->mnservice->status.commandresult.type = eomn_serv_NONE;
        p->mnservice->status.commandresult.data[0] = eomn_serv_state_notsupported;
        
        send_rop_command_result();   
        
        return(eores_OK);
    }
     
    eOresult_t res = eores_NOK_generic;
    
    // now we operate ... by calling the specific function of the service.
    switch(category)
    {
        case eomn_serv_category_mc:
        {
            if(NULL == arrayofid32)
            {
                res = eo_motioncontrol_SetRegulars(eo_motioncontrol_GetHandle(), arrayofid32, &number); 
            }
            else
            {
                res = eo_motioncontrol_AddRegulars(eo_motioncontrol_GetHandle(), arrayofid32, &number); 
            }
        } break;
        
        case eomn_serv_category_strain:
        {
            res = eo_strain_SetRegulars(eo_strain_GetHandle(), arrayofid32, &number);
        } break;

        case eomn_serv_category_mais:
        {
            res = eo_mais_SetRegulars(eo_mais_GetHandle(), arrayofid32, &number);
        } break;    

        case eomn_serv_category_skin:
        {
            res = eo_skin_SetRegulars(eo_skin_GetHandle(), arrayofid32, &number);
        } break;        

        case eomn_serv_category_inertials:
        {
            res = eo_inertials2_SetRegulars(eo_inertials2_GetHandle(), arrayofid32, &number);
        } break;
        
        default:
        {
            res = eores_NOK_generic;
        } break;
    }

    p->mnservice->status.commandresult.latestcommandisok = (eores_OK == res) ? (eobool_true) : (eobool_false);
       
    p->mnservice->status.commandresult.category = category;
    p->mnservice->status.commandresult.operation = (NULL != arrayofid32) ? (eomn_serv_operation_regsig_load) : (eomn_serv_operation_regsig_clear);
    p->mnservice->status.commandresult.type = eomn_serv_NONE;
    p->mnservice->status.commandresult.data[0] = p->mnservice->status.stateofservice[category];
    
    send_rop_command_result();    
    
    return(eores_OK);        
}


static eOresult_t s_eo_services_process_failure(EOtheServices *p, eOmn_serv_operation_t operation, eOmn_serv_category_t category)
{
    p->mnservice->status.commandresult.latestcommandisok = eobool_false;
    p->mnservice->status.commandresult.category = category;
    p->mnservice->status.commandresult.operation = operation;
    p->mnservice->status.commandresult.type = eomn_serv_NONE;

    send_rop_command_result(); 

    return(eores_OK);    
}


static void send_rop_command_result(void)
{
    eOropdescriptor_t ropdesc;

    memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
    ropdesc.ropcode = eo_ropcode_sig;
    ropdesc.id32    = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_service, 0, eoprot_tag_mn_service_status_commandresult);
    ropdesc.data    = NULL; // so that data from the EOnv is retrieved (which is: p->mnservice->status.commandresult)          
    
    eom_emsappl_Transmit_OccasionalROP(eom_emsappl_GetHandle(), &ropdesc);
}


static eOresult_t s_eo_services_start(EOtheServices *p, eOmn_serv_category_t category)
{
    eOresult_t res = eores_NOK_generic;
    
    switch(category)
    {
        case eomn_serv_category_mc:
        {
            res = eo_motioncontrol_Start(eo_motioncontrol_GetHandle());
        } break;
        
        case eomn_serv_category_strain:
        {
            res = eo_strain_Start(eo_strain_GetHandle());
        } break;

        case eomn_serv_category_mais:
        {
            res = eo_mais_Start(eo_mais_GetHandle());
        } break;    

        case eomn_serv_category_skin:
        {
            res = eo_skin_Start(eo_skin_GetHandle());
        } break;        

        case eomn_serv_category_inertials:
        {
            res = eo_inertials2_Start(eo_inertials2_GetHandle());
        } break;
        
        default:
        {
            res = eores_NOK_generic;
        } break;
    }
    
    
    if(eores_OK == res)
    {
        // must send the application in run mode ...
        p->running[category] = eobool_true;
        res = eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STrun);        
    }
    
    return(res);
}


static eOresult_t s_eo_services_stop(EOtheServices *p, eOmn_serv_category_t category, uint8_t *numofregulars, eOsmStatesEMSappl_t *state, eObool_t and_deactivate)
{
    eOresult_t res = eores_NOK_generic;    
    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), state);   
    
    // now we stop the service. and we also clear the regulars. and if and_deactivate is true we also deactivate
    switch(category)
    {
        case eomn_serv_category_mc:
        {
            res = eo_motioncontrol_Stop(eo_motioncontrol_GetHandle());
            eo_motioncontrol_SetRegulars(eo_motioncontrol_GetHandle(), NULL, numofregulars);
            p->running[eomn_serv_category_mc] = eobool_false;
            if(eobool_true == and_deactivate)
            {
                eo_motioncontrol_Deactivate(eo_motioncontrol_GetHandle());
            }
        } break;
        
        case eomn_serv_category_strain:
        {
            res = eo_strain_Stop(eo_strain_GetHandle());
            eo_strain_SetRegulars(eo_strain_GetHandle(), NULL, numofregulars);
            p->running[eomn_serv_category_strain] = eobool_false;
            if(eobool_true == and_deactivate)
            {
                eo_strain_Deactivate(eo_strain_GetHandle());
            }
        } break;

        case eomn_serv_category_mais:
        {
            res = eores_OK; // prefer not to stop mais in order to avoid problems with the mc4can boards going in hw fault .... 
            eo_mais_Stop(eo_mais_GetHandle());
            eo_mais_SetRegulars(eo_mais_GetHandle(), NULL, numofregulars);
            p->running[eomn_serv_category_mais] = eobool_false;
            if(eobool_true == and_deactivate)
            {
                eo_mais_Deactivate(eo_mais_GetHandle());
            }
        } break;    

        case eomn_serv_category_skin:
        {
            res = eo_skin_Stop(eo_skin_GetHandle());
            eo_skin_SetRegulars(eo_skin_GetHandle(), NULL, numofregulars);
            p->running[eomn_serv_category_skin] = eobool_false;
            if(eobool_true == and_deactivate)
            {
                eo_skin_Deactivate(eo_skin_GetHandle());
            }
        } break;        

        case eomn_serv_category_inertials:
        {
            res = eo_inertials2_Stop(eo_inertials2_GetHandle());
            eo_inertials2_SetRegulars(eo_inertials2_GetHandle(), NULL, numofregulars);
            p->running[eomn_serv_category_inertials] = eobool_false;
            if(eobool_true == and_deactivate)
            {
                eo_inertials2_Deactivate(eo_inertials2_GetHandle());
            }
        } break;
        
        case eomn_serv_category_all:
        {
            eo_motioncontrol_Stop(eo_motioncontrol_GetHandle()); 
            eo_motioncontrol_SetRegulars(eo_motioncontrol_GetHandle(), NULL, NULL);
            p->running[eomn_serv_category_mc] = eobool_false;
            if(eobool_true == and_deactivate)
            {
                eo_motioncontrol_Deactivate(eo_motioncontrol_GetHandle());
            }
           
            eo_mais_Stop(eo_mais_GetHandle());
            eo_mais_SetRegulars(eo_mais_GetHandle(), NULL, NULL);
            p->running[eomn_serv_category_mais] = eobool_false;
            if(eobool_true == and_deactivate)
            {
                eo_mais_Deactivate(eo_mais_GetHandle());
            }
            
            eo_strain_Stop(eo_strain_GetHandle());
            eo_strain_SetRegulars(eo_strain_GetHandle(), NULL, NULL);
            p->running[eomn_serv_category_strain] = eobool_false;
            if(eobool_true == and_deactivate)
            {
                eo_strain_Deactivate(eo_strain_GetHandle());
            }
            
            eo_skin_Stop(eo_skin_GetHandle());
            eo_skin_SetRegulars(eo_skin_GetHandle(), NULL, NULL);
            p->running[eomn_serv_category_skin] = eobool_false;
            if(eobool_true == and_deactivate)
            {
                eo_skin_Deactivate(eo_skin_GetHandle());
            }
            
            eo_inertials2_Stop(eo_inertials2_GetHandle());
            eo_inertials2_SetRegulars(eo_inertials2_GetHandle(), NULL, NULL);  
            p->running[eomn_serv_category_inertials] = eobool_false;
            if(eobool_true == and_deactivate)
            {            
                eo_inertials2_Deactivate(eo_inertials2_GetHandle());  
            }                
            
            // if i dont use the service command to load the rop, then it is safe to remove all rops anyway.
            eo_transceiver_RegularROPs_Clear(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
            
            *numofregulars = eo_transceiver_RegularROP_ArrayID32Size(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
            
            // also i send the application in config mode            
            res = eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STcfg);            
        } break;
        
        default:
        {
            res = eores_NOK_generic;
        } break;
    }
       
    
    if(eores_OK == res)
    {
        // must send the application in cfg mode ... only if there is no service running anymore 
        eObool_t anyrunning = eobool_false;        
        for(uint8_t i=0; i<eomn_serv_categories_numberof; i++)
        {
            if(eobool_true == p->running[i])
            {
                anyrunning = eobool_true;
                break;
            }           
        }
        
        if(eobool_false == anyrunning)
        {
            res = eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STcfg);
            if(eo_sm_emsappl_STerr != *state)
            {
                *state = eo_sm_emsappl_STcfg;
            }
        } 
    }
    
    return(res);
}



//#if !defined(EOSERVICES_USE_RUNTIME_ACTIVATION)
// 
//static void s_services_startupactivation_activate_services_deferred(void)
//{
//    EOaction_strg astrg = {0};
//    EOaction *act = (EOaction*)&astrg;
//    
//    eo_action_SetCallback(act, s_services_startupactivation_activate_services_now, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
//    
//    eo_timer_Start(s_eo_theservices.timer, eok_abstimeNOW, 100*eok_reltime1ms, eo_tmrmode_ONESHOT, act);          
//}
// 
// 
//static void s_services_startupactivation_activate_services_now(void *p)
//{
//    // depending on the board number we have something to do which is different
//    // at first we do motion control, then strain, then skin, then inertials.
//    
//    const eOmn_serv_configuration_t * servcfg = NULL;
//    
//    if(NULL != (servcfg = eoboardconfig_code2motion_serv_configuration(s_eo_theservices.board)))
//    {
//        eo_motioncontrol_Verify(eo_motioncontrol_GetHandle(), servcfg, s_services_startupactivation_after_verify_motion, eobool_true); 
//    }
//    else if(NULL != (servcfg = eoboardconfig_code2mais_serv_configuration(s_eo_theservices.board)))
//    {
//        eo_mais_Verify(eo_mais_GetHandle(), servcfg, s_services_startupactivation_after_verify_mais, eobool_true); 
//    }
//    else if(NULL != (servcfg = eoboardconfig_code2strain_serv_configuration(s_eo_theservices.board)))
//    {
//        eo_strain_Verify(eo_strain_GetHandle(), servcfg, s_services_startupactivation_after_verify_strain, eobool_true); 
//    }
//    else if(NULL != (servcfg = eoboardconfig_code2skin_serv_configuration(s_eo_theservices.board)))
//    {
//        eo_skin_Verify(eo_skin_GetHandle(), servcfg, s_services_startupactivation_after_verify_skin, eobool_true); 
//    }
//    else if(NULL != (servcfg = eoboardconfig_code2inertials_serv_configuration(s_eo_theservices.board)))
//    {
//        eo_inertials2_Verify(eo_inertials2_GetHandle(), servcfg, s_services_startupactivation_after_verify_inertials, eobool_true); 
//    } 
//    else
//    {   // nothing else to verify .. actually none is activated but in here tehre was nothing to activate
//        s_eo_theservices.startupactivationstate.allactivated = eobool_true;
//    }    
//}
// 
// 
//static eOresult_t s_services_startupactivation_after_verify_motion(EOaService* p, eObool_t operationisok)
//{
//    if(eobool_false == operationisok)
//    {
//        s_eo_theservices.startupactivationstate.failedservice = eo_service_mc;
//        return(eores_OK);
//    }
//    
//    const eOmn_serv_configuration_t * servcfg = NULL;
//    
//    if(NULL != (servcfg = eoboardconfig_code2mais_serv_configuration(s_eo_theservices.board)))
//    {
//        eo_mais_Verify(eo_mais_GetHandle(), servcfg, s_services_startupactivation_after_verify_mais, eobool_true); 
//    }
//    else if(NULL != (servcfg = eoboardconfig_code2strain_serv_configuration(s_eo_theservices.board)))
//    {
//        eo_strain_Verify(eo_strain_GetHandle(), servcfg, s_services_startupactivation_after_verify_strain, eobool_true); 
//    }
//    else if(NULL != (servcfg = eoboardconfig_code2skin_serv_configuration(s_eo_theservices.board)))
//    {
//        eo_skin_Verify(eo_skin_GetHandle(), servcfg, s_services_startupactivation_after_verify_skin, eobool_true); 
//    }
//    else if(NULL != (servcfg = eoboardconfig_code2inertials_serv_configuration(s_eo_theservices.board)))
//    {
//        eo_inertials2_Verify(eo_inertials2_GetHandle(), servcfg, s_services_startupactivation_after_verify_inertials, eobool_true); 
//    }
//    else
//    {   // nothing else to verify ..
//        s_eo_theservices.startupactivationstate.allactivated = eobool_true;
//    }
//    
//    return(eores_OK);
//}
// 
// 
//static eOresult_t s_services_startupactivation_after_verify_mais(EOaService* p, eObool_t operationisok)
//{
//    if(eobool_false == operationisok)
//    {
//        s_eo_theservices.startupactivationstate.failedservice = eo_service_mais;
//        return(eores_OK);
//    }
//         
//    const eOmn_serv_configuration_t * servcfg = NULL;
//    
//    if(NULL != (servcfg = eoboardconfig_code2strain_serv_configuration(s_eo_theservices.board)))
//    {
//        eo_strain_Verify(eo_strain_GetHandle(), servcfg, s_services_startupactivation_after_verify_strain, eobool_true); 
//    }
//    else if(NULL != (servcfg = eoboardconfig_code2skin_serv_configuration(s_eo_theservices.board)))
//    {
//        eo_skin_Verify(eo_skin_GetHandle(), servcfg, s_services_startupactivation_after_verify_skin, eobool_true); 
//    }
//    else if(NULL != (servcfg = eoboardconfig_code2inertials_serv_configuration(s_eo_theservices.board)))
//    {
//        eo_inertials2_Verify(eo_inertials2_GetHandle(), servcfg, s_services_startupactivation_after_verify_inertials, eobool_true); 
//    }              
//    else
//    {   // nothing else to verify ..
//        s_eo_theservices.startupactivationstate.allactivated = eobool_true;
//    }    
//   
//    return(eores_OK);
//}
// 
// 
//static eOresult_t s_services_startupactivation_after_verify_strain(EOaService* p, eObool_t operationisok)
//{
//    if(eobool_false == operationisok)
//    {
//        s_eo_theservices.startupactivationstate.failedservice = eo_service_strain;
//        return(eores_OK);
//    }
//    
//    
//    const eOmn_serv_configuration_t * servcfg = NULL;
//    
//    if(NULL != (servcfg = eoboardconfig_code2skin_serv_configuration(s_eo_theservices.board)))
//    {
//        eo_skin_Verify(eo_skin_GetHandle(), servcfg, s_services_startupactivation_after_verify_skin, eobool_true); 
//    }
//    else if(NULL != (servcfg = eoboardconfig_code2inertials_serv_configuration(s_eo_theservices.board)))
//    {
//        eo_inertials2_Verify(eo_inertials2_GetHandle(), servcfg, s_services_startupactivation_after_verify_inertials, eobool_true); 
//    }              
//    else
//    {   // nothing else to verify ..
//        s_eo_theservices.startupactivationstate.allactivated = eobool_true;
//    }    
//   
//    return(eores_OK);
//}
// 
// 
//static eOresult_t s_services_startupactivation_after_verify_skin(EOaService* p, eObool_t operationisok)
//{
//    if(eobool_false == operationisok)
//    {
//        s_eo_theservices.startupactivationstate.failedservice = eo_service_skin;
//        return(eores_OK);
//    }
//    
//    const eOmn_serv_configuration_t * servcfg = NULL;
//    
//    if(NULL != (servcfg = eoboardconfig_code2inertials_serv_configuration(s_eo_theservices.board)))
//    {
//        eo_inertials2_Verify(eo_inertials2_GetHandle(), servcfg, s_services_startupactivation_after_verify_inertials, eobool_true); 
//    }              
//    else
//    {   // nothing else to verify .. 
//        s_eo_theservices.startupactivationstate.allactivated = eobool_true;
//    }        
// 
//    return(eores_OK);
//}
// 
// 
//static eOresult_t s_services_startupactivation_after_verify_inertials(EOaService* p, eObool_t operationisok)
//{
//    if(eobool_false == operationisok)
//    {
//        s_eo_theservices.startupactivationstate.failedservice = eo_service_inertial;
//        return(eores_OK);
//    }
//    
//    // nothing else to verify .. 
//    s_eo_theservices.startupactivationstate.allactivated = eobool_true;
//   
//   
//    return(eores_OK);
//}
// 
//#endif

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



