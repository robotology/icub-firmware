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

#include "EoProtocolMN.h"

#include "EOtheBoardConfig.h"

//#include "EOmcController.h"

#include "EOtheServices.h"

#include "EOtimer.h"

#include "EOMtheEMSconfigurator.h"

#include "EOVtheCallbackManager.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "testRTC.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


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

#if defined(TESTRTC_IS_ACTIVE)

static void s_eo_services_test_initialise(void);



static void s_services_test_verifyactivate(const eOmn_serv_configuration_t* cfg);
static void s_services_test_start(void *arg);
static void s_services_test_stop(void *arg);
static void s_services_test_stop_everything(void *arg);


static void s_services_test_mc_init(void);
static void s_services_test_mc_multiplesteps(void *arg);
static void s_services_test_mc_stop(void *par);



static void s_services_test_inertials_init(void);
static void s_services_test_inertials_multiplesteps(void *arg);
static void s_services_test_inertials_stop(void *par);

#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

#if defined(TESTRTC_IS_ACTIVE)

// in here we decide if we want to test mc or inertials ...
//static const eOmn_serv_category_t s_service_under_test = eomn_serv_category_mc; 
static const eOmn_serv_category_t s_service_under_test = eomn_serv_category_inertials;


// the rest are service variables

static EOtimer *s_timer = NULL;

static eOmn_service_cmmnds_command_t s_command = {0};


static volatile uint8_t services_stop_ANY_service_now = 0;


static eOcallback_t s_service_tick = NULL;

static const eOmn_serv_configuration_t* s_test_config_ok = NULL;
static const eOmn_serv_configuration_t* s_test_config_ko = NULL;

static EOaction_strg s_astrg = {0};
static EOaction *s_act = (EOaction*)&s_astrg;

#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern void testRTC_init(void)
{
#if defined(TESTRTC_IS_ACTIVE)
    
    if(NULL == s_timer)
    {
        s_timer = eo_timer_New();
    }
    
    services_stop_ANY_service_now = 0;
    
    s_eo_services_test_initialise();
       
#endif    
}

extern void testRTC_RUN_tick(void)
{
#if defined(TESTRTC_IS_ACTIVE)

    if(1 == services_stop_ANY_service_now)
    {
        services_stop_ANY_service_now = 0;
        s_services_test_stop(NULL);
    }
    
#endif    
}

extern void testRTC_CFG_tick(void)
{
#if defined(TESTRTC_IS_ACTIVE)
    
    if(NULL != s_service_tick)
    {
        s_service_tick(NULL);
    }
    
#endif    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


#if defined(TESTRTC_IS_ACTIVE) 
extern void eom_emsconfigurator_hid_userdef_ProcessUserdef03Event(EOMtheEMSconfigurator* p)
{
    testRTC_CFG_tick();
}
#endif



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


#if defined(TESTRTC_IS_ACTIVE)

static void s_eo_services_test_initialise(void)
{
    
    switch(s_service_under_test)
    {
        default:
        case eomn_serv_category_mc:
        {                         
            s_services_test_mc_init();             
        } break;
        
        case eomn_serv_category_inertials:
        {
            s_services_test_inertials_init();            
        } break;

    }       
    
    eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
    eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);     
}


static void s_services_test_verifyactivate(const eOmn_serv_configuration_t* cfg)
{       
    s_command.category = s_service_under_test;
    s_command.operation = eomn_serv_operation_verifyactivate;
    memcpy(&s_command.parameter.configuration, cfg, sizeof(eOmn_serv_configuration_t));
        
    // ok, we have received a command for a given service. we ask the object theservices to manage the thing
    
    eo_services_ProcessCommand(eo_services_GetHandle(), &s_command);    
}


static void s_services_test_start(void *arg)
{        
    s_command.category = s_service_under_test;
    s_command.operation = eomn_serv_operation_start;
    memset(&s_command.parameter.configuration, 0, sizeof(eOmn_serv_configuration_t));
        
    // ok, we have received a command for a given service. we ask the object theservices to manage the thing
    
    eo_services_ProcessCommand(eo_services_GetHandle(), &s_command);    
}


static void s_services_test_stop(void *arg)
{        
    s_command.category = s_service_under_test;
    s_command.operation = eomn_serv_operation_stop;
    memset(&s_command.parameter.configuration, 0, sizeof(eOmn_serv_configuration_t));
        
    // ok, we have received a command for a given service. we ask the object theservices to manage the thing
    
    eo_services_ProcessCommand(eo_services_GetHandle(), &s_command);    
}

static void s_services_test_stop_everything(void *arg)
{
    services_stop_ANY_service_now = 1;
    eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
    eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);    
}




// -----------------------------------------------------------------------------------------------------------
// motion control ....




static const eOmn_serv_configuration_t s_serv_config_mc_eb1_eb3_zeroprotocol =
{   // eb1 / eb3
    .type       = eomn_serv_MC_foc,
    .filler     = {0},
    .data.mc.foc_based = 
    {
        .boardtype4mccontroller = eomc_ctrlboard_SHOULDER,
        .version   =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 0, .minor = 0 }
        },
        .filler                 = {0},
        .arrayofjomodescriptors =
        {
            .head   = 
            {
                .capacity       = 4,
                .itemsize       = sizeof(eOmc_jomo_descriptor_t),
                .size           = 4,
                .internalmem    = 0                    
            },
            .data   =
            {
                { // joint 0
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 1,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_aea,
                        .port   = eobrd_port_emsP6, // hal_encoder1   
                        .pos    = eomc_pos_atjoint
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
                    }
                },
                { // joint 1
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 2,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_aea,
                        .port   = eobrd_port_emsP7, // hal_encoder4
                        .pos    = eomc_pos_atjoint                            
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
                    }
                },                    
                { // joint 2
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 3,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_aea,
                        .port   = eobrd_port_emsP8, // hal_encoder2  
                        .pos    = eomc_pos_atjoint
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
                    }
                },               
                { // joint 3
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 4,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_aea,
                        .port   = eobrd_port_emsP9,    // hal_encoder5 
                        .pos    = eomc_pos_atjoint
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
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
            .encoder2joint  = 
            {   // identical matrix
                { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) } 
            }  
        }        
    }
};

static const eOmn_serv_configuration_t s_serv_config_mc_eb1_fake_aea =
{   // eb1 / eb3
    .type       = eomn_serv_MC_foc,
    .filler     = {0},
    .data.mc.foc_based = 
    {
        .boardtype4mccontroller = eomc_ctrlboard_SHOULDER,
        .version   =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 0, .minor = 0 }
        },
        .filler                 = {0},
        .arrayofjomodescriptors =
        {
            .head   = 
            {
                .capacity       = 4,
                .itemsize       = sizeof(eOmc_jomo_descriptor_t),
                .size           = 4,
                .internalmem    = 0                    
            },
            .data   =
            {
                { // joint 0
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 1,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_aea,
                        .port   = eobrd_port_emsP10,    
                        .pos    = eomc_pos_atjoint
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
                    }
                },
                { // joint 1
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 2,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_aea,
                        .port   = eobrd_port_emsP7, // hal_encoder4
                        .pos    = eomc_pos_atjoint                            
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
                    }
                },                    
                { // joint 2
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 3,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_aea,
                        .port   = eobrd_port_emsP8, // hal_encoder2  
                        .pos    = eomc_pos_atjoint
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
                    }
                },               
                { // joint 3
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 4,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_aea,
                        .port   = eobrd_port_emsP9,    // hal_encoder5 
                        .pos    = eomc_pos_atjoint
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
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
            .encoder2joint  = 
            {   // identical matrix
                { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) } 
            }  
        }        
    }
};


static const eOmn_serv_configuration_t s_serv_config_mc_eb1_fake_amo =
{   // eb1 / eb3
    .type       = eomn_serv_MC_foc,
    .filler     = {0},
    .data.mc.foc_based = 
    {
        .boardtype4mccontroller = eomc_ctrlboard_SHOULDER,
        .version   =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 0, .minor = 0 }
        },
        .filler                 = {0},
        .arrayofjomodescriptors =
        {
            .head   = 
            {
                .capacity       = 4,
                .itemsize       = sizeof(eOmc_jomo_descriptor_t),
                .size           = 4,
                .internalmem    = 0                    
            },
            .data   =
            {
                { // joint 0
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 1,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_amo,
                        .port   = eobrd_port_emsP6,    
                        .pos    = eomc_pos_atjoint
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
                    }
                },
                { // joint 1
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 2,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_amo,
                        .port   = eobrd_port_emsP7, // hal_encoder4
                        .pos    = eomc_pos_atjoint                            
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
                    }
                },                    
                { // joint 2
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 3,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_amo,
                        .port   = eobrd_port_emsP8, // hal_encoder2  
                        .pos    = eomc_pos_atjoint
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
                    }
                },               
                { // joint 3
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 4,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_amo,
                        .port   = eobrd_port_emsP9,    // hal_encoder5 
                        .pos    = eomc_pos_atjoint
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
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
            .encoder2joint  = 
            {   // identical matrix
                { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) } 
            }  
        }        
    }
};


static const eOmn_serv_configuration_t s_serv_config_mc_eb1_fake_inc =
{   // eb1 / eb3
    .type       = eomn_serv_MC_foc,
    .filler     = {0},
    .data.mc.foc_based = 
    {
        .boardtype4mccontroller = eomc_ctrlboard_SHOULDER,
        .version   =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 0, .minor = 0 }
        },
        .filler                 = {0},
        .arrayofjomodescriptors =
        {
            .head   = 
            {
                .capacity       = 4,
                .itemsize       = sizeof(eOmc_jomo_descriptor_t),
                .size           = 4,
                .internalmem    = 0                    
            },
            .data   =
            {
                { // joint 0
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 1,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_qenc,
                        .port   = eobrd_port_mc4plusP2,    
                        .pos    = eomc_pos_atjoint
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
                    }
                },
                { // joint 1
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 2,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_qenc,
                        .port   = eobrd_port_mc4plusP3, 
                        .pos    = eomc_pos_atjoint                            
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
                    }
                },                    
                { // joint 2
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 3,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_qenc,
                        .port   = eobrd_port_mc4plusP4, 
                        .pos    = eomc_pos_atjoint
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
                    }
                },               
                { // joint 3
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 4,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_qenc,
                        .port   = eobrd_port_mc4plusP5,   
                        .pos    = eomc_pos_atjoint
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
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
            .encoder2joint  = 
            {   // identical matrix
                { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) } 
            }  
        }        
    }
};



static void s_services_test_mc_init(void)
{
    s_test_config_ko = &s_serv_config_mc_eb1_fake_inc;
    s_test_config_ok = &s_serv_config_mc_eb1_eb3_zeroprotocol;               
    s_service_tick = s_services_test_mc_multiplesteps;    
}

static void s_services_test_mc_stop(void *par)
{
    s_services_test_stop_everything(NULL);
}

static void s_services_test_mc_multiplesteps(void *arg)
{
#if 0    
    // this is a test for fully working activate() /  deactivate()
    static uint8_t step = 0;
    
    step++;
    
    if(1 == step)
    {
        s_services_test_stop(arg);
        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);    
    }
    else if(2 == step)
    {
        s_services_test_verifyactivate(s_test_config_ko);
        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);          
    }
    else if(3 == step)
    {        
        s_services_test_start(arg);
        
        // prepare to stop it ... by setting a flag which the runner will process and call 
        services_stop_ANY_service_now = 0;
        eo_action_SetCallback(s_act, s_services_test_mc_stop, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
        eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act); 
    }
    else if(4 == step)
    {
        services_stop_ANY_service_now = 0;
        s_services_test_verifyactivate(s_test_config_ok);
        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);    
    }
    else if(5 == step)
    {
        services_stop_ANY_service_now = 0;
        s_services_test_start(arg);
    }

#else
    
    // this is a test for simple activate() and start()
    static uint8_t step = 0;
    
    step++;
    
    if(1 == step)
    {
        s_services_test_stop(arg);
        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);    
    }
    else if(2 == step)
    {
        s_services_test_verifyactivate(s_test_config_ok);
        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);          
    }
    else if(3 == step)
    {        
        s_services_test_start(arg);
        
        // prepare to stop it ... by setting a flag which the runner will process and call 
        services_stop_ANY_service_now = 0;
        eo_action_SetCallback(s_act, s_services_test_mc_stop, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
        eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act); 
    }
    else if(4 == step)
    {
        services_stop_ANY_service_now = 0;
        s_services_test_verifyactivate(s_test_config_ok);
        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);    
    }
    else if(5 == step)
    {
        services_stop_ANY_service_now = 0;
        s_services_test_start(arg);
    }    
    
#endif
          
}


// --------------------------------------------------------------------------------------------------------------------
// inertials mems gyro

#include "EOtheInertials2.h"


static const eOmn_serv_configuration_t s_serv_config_as_inertial_test_gyro =
{   
    .type       = eomn_serv_AS_inertials,
    .filler     = {0},
    .data.as.inertial = 
    {
        .mtbversion    =
        {
            .firmware   = { .major = 2, .minor = 17, .build = 0 },
            .protocol   = { .major = 1, .minor = 0 }  // in case of {0, 0} the can discovery is not done but the verify will be ok. for normal case use: {1, 0}  
        },
        
        .arrayofsensors =
        {
            .head   = 
            {
                .capacity       = eOas_inertials_maxnumber,
                .itemsize       = sizeof(eOas_inertial_descriptor_t),
                .size           = 2,
                .internalmem    = 0                    
            },
            .data   =
            {
                {   // mtb
                    .type   = eoas_inertial_accel_mtb_int,
                    .on.can = { .place = eobrd_place_can, .port = eOcanport1, .addr = 7 }
                },                                
                {   // gyro ....
                    .type   = eoas_inertial_gyros_ems_st_l3g4200d,
                    .on.eth = { .place = eobrd_place_eth, .id = 0 }
                }              
            }                
        }
    }    
};

static const eOmn_serv_configuration_t s_serv_config_as_inertial_test_accel =
{   
    .type       = eomn_serv_AS_inertials,
    .filler     = {0},
    .data.as.inertial = 
    {
        .mtbversion    =
        {
            .firmware   = { .major = 2, .minor = 17, .build = 0 },
            .protocol   = { .major = 1, .minor = 0 }  // in case of {0, 0} the can discovery is not done but the verify will be ok. for normal case use: {1, 0}  
        },
        
        .arrayofsensors =
        {
            .head   = 
            {
                .capacity       = eOas_inertials_maxnumber,
                .itemsize       = sizeof(eOas_inertial_descriptor_t),
                .size           = 2,
                .internalmem    = 0                    
            },
            .data   =
            {
                {   // mtb
                    .type   = eoas_inertial_accel_mtb_int,
                    .on.can = { .place = eobrd_place_can, .port = eOcanport1, .addr = 7 }
                },                                
                {   // gyro ....
                    .type   = eoas_inertial_accel_ems_st_lis3x,
                    .on.eth = { .place = eobrd_place_eth, .id = 0 }
                }              
            }                
        }
    }    
};

static void s_services_test_inertials_init(void)
{
    s_test_config_ok = &s_serv_config_as_inertial_test_gyro; 
    s_test_config_ko = &s_serv_config_as_inertial_test_accel;
    s_service_tick = s_services_test_inertials_multiplesteps;    
}


static void s_services_test_inertials_multiplesteps(void *arg)
{
    // this is a test for fully working activate() /  deactivate()
    static uint8_t step = 0;
    
    step++;
    
    if(1 == step)
    {
        s_services_test_stop(arg);
        
        services_stop_ANY_service_now = 0;
        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, 1*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);    
    }
    else if(2 == step)
    {   // verify-activate
        s_services_test_verifyactivate(s_test_config_ok);
        
        services_stop_ANY_service_now = 0;
        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, 1*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);              
    }
    else if(3 == step)
    {   // inertials config  
        eOas_inertial_config_t config = {0};
        config.datarate = 10; // ms
        config.enabled = 0x3; // enable the first two
        eo_inertials2_Config(eo_inertials2_GetHandle(), &config);
        
        services_stop_ANY_service_now = 0;
        eo_action_SetCallback(s_act, s_services_test_mc_stop, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
        eo_timer_Start(s_timer, eok_abstimeNOW, 1*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act); 
    }
    else if(4 == step)
    {   // service start        
        s_services_test_start(arg);
        
        services_stop_ANY_service_now = 0;
        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, 1*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);    
    }
    else if(5 == step)
    {   // inertials enable        
        eo_inertials2_Transmission(eo_inertials2_GetHandle(), eobool_true);
        
        services_stop_ANY_service_now = 0;
        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, 30*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);   
    }  
    else if(6 == step)
    {
        s_services_test_stop_everything(NULL);
    }
}


static void s_services_test_inertials_stop(void *par)
{   
    s_services_test_stop_everything(NULL);   
}



#endif // TESTRTC_IS_ACTIVE

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



