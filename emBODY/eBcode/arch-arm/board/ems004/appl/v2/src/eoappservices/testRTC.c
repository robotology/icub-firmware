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

//#include "EOtheBoardConfig.h"

//#include "EOmcController.h"

#include "EOtheServices.h"

#include "EOtimer.h"

#include "EOMtheEMSconfigurator.h"

#include "EOVtheCallbackManager.h"

#include "EOarray.h"

#include "EoProtocolAS.h"
#include "EOMmutex.h"

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
// empty section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

typedef enum { actor_none = 0, actor_cfg = 1, actor_run = 2 } actor_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

#if defined(TESTRTC_IS_ACTIVE)

static void s_eo_services_test_initialise(void);


static void s_services_test_verifyactivate(const eOmn_serv_configuration_t* cfg);
static void s_services_test_start(void *arg);
static void s_services_test_stop(void *arg);
static void s_services_test_stop_everything(void *arg);



#if defined(TESTRTC_MC)

static void s_services_test_mc_init(void);
static void s_services_test_mc_multiplesteps(void *arg);
static void s_services_test_mc_stop(void *par);

#else
static void s_services_test_mc_init(void) {}
static void s_services_test_mc_multiplesteps(void *arg) {}
static void s_services_test_mc_stop(void *par) {}
#endif // #if defined(TESTRTC_MC)


//static void s_services_test_inertials_init();
//static void s_services_test_inertials_multiplesteps(void *arg);
//static void s_services_test_inertials_stop(void *par);



#if defined(TESTRTC_POS)

static void s_services_test_pos_init(void);
static void s_services_test_pos_multiplesteps(void *arg);
static void s_services_test_pos_stop(void *par);

#else
static void s_services_test_pos_init(void) {}
static void s_services_test_pos_multiplesteps(void *arg) {}
static void s_services_test_pos_stop(void *par) {}    
#endif // #if defined(TESTRTC_POS)


#if defined(TESTRTC_FT)

static void s_services_test_ft_init(void);
static void s_services_test_ft_multiplesteps(void *arg);
static void s_services_test_ft_stop(void *par);

#else
static void s_services_test_ft_init(void) {}
static void s_services_test_ft_multiplesteps(void *arg) {}
static void s_services_test_ft_stop(void *par) {}    
#endif // #if defined(TESTRTC_FT)
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

#if defined(TESTRTC_IS_ACTIVE)

// in here we decide if we want to test mc or pos or else ...
#if defined(TESTRTC_MC)
static const eOmn_serv_category_t s_service_under_test = eomn_serv_category_mc; 
#elif defined(TESTRTC_POS)
static const eOmn_serv_category_t s_service_under_test = eomn_serv_category_pos;
#elif defined(TESTRTC_FT)
static const eOmn_serv_category_t s_service_under_test = eomn_serv_category_ft;
#endif


// the rest are service variables

static EOtimer *s_timer = NULL;

static eOmn_service_cmmnds_command_t s_command = {0};


static volatile uint8_t services_stop_ANY_service_now = 0;


static eOcallback_t s_service_tick = NULL;

static const eOmn_serv_configuration_t* s_test_config_ok = NULL;
static const eOmn_serv_configuration_t* s_test_config_ko = NULL;

static EOaction_strg s_astrg = {0};
static EOaction *s_act = (EOaction*)&s_astrg;

#if defined(TESTRTC_FT) 
void s_services_test_ft_tick(actor_t a);
#endif

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

#if defined(TESTRTC_FT) 
        s_services_test_ft_tick(actor_run);
#else  
    if(1 == services_stop_ANY_service_now)
    {
        services_stop_ANY_service_now = 0;
        s_services_test_stop(NULL);
    }
#endif    
#endif    
}

extern void testRTC_CFG_tick(void)
{
#if defined(TESTRTC_IS_ACTIVE)

#if defined(TESTRTC_FT) 
        s_services_test_ft_tick(actor_cfg);
#else    
    
    if(NULL != s_service_tick)
    {
        s_service_tick(NULL);
    }
#endif    
#endif    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


#if defined(TESTRTC_IS_ACTIVE) 
extern "C"
{
    extern void eom_emsconfigurator_hid_userdef_ProcessUserdef03Event(EOMtheEMSconfigurator* p)
    {
        testRTC_CFG_tick();
    }
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
        
//        case eomn_serv_category_inertials:
//        {
//            s_services_test_inertials_init();            
//        } break;

        case eomn_serv_category_pos:
        {
            s_services_test_pos_init();            
        } break;
        
        case eomn_serv_category_ft:
        {
            s_services_test_ft_init();            
        } break;        
    }       

#if !defined(TESTRTC_FT)    
    EOVtaskDerived * t = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());
    eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, t);    
    eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act); 
#endif    
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

#if defined(TESTRTC_MC)

//eo_motcon_mode_mc4pluspmc
static const eOmn_serv_configuration_t s_serv_config_mc_mc4pluspmc =
{   
    .type       = eomn_serv_MC_mc4pluspmc,
    .diagnosticsmode = eomn_serv_diagn_mode_NONE,
    .diagnosticsparam = 0,
    .data.mc.mc4pluspmc = 
    {
        .pos   =
        {
            .version = 
            {
                .firmware = 
                {
                    .major = 0, .minor = 0, .build = 0
                },
                .protocol = 
                {
                    .major = 0, .minor = 0
                }
            },
            .boardInfo = 
            {
                .canloc = 
                {
                    { 
                        .port = eOcanport1, 
                        .addr = 1, 
                        .insideindex = eobrd_caninsideindex_none
                    }        
                }
            }        
            
        },
        .filler                 = {0},
        .arrayof7jomodescriptors =
        {
            .head   = 
            {
                .capacity       = 7,
                .itemsize       = sizeof(eOmc_jomo_descriptor_t),
                .size           = 7,
                .internalmem    = 0                    
            },
            .data   =
            {
                { // joint 0
                    .actuator.pwm    =
                    {
                        .port           = eobrd_port_mc4plusP2,
                        .dummy          = 0                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_pos,
                        .port   = eobrd_portpos_hand_thumb,  
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
                    .actuator.pwm    =
                    {
                        .port           = eobrd_port_mc4plusP3,
                        .dummy          = 0                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_pos,
                        .port   = eobrd_portpos_hand_index, 
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
                    .actuator.pwm    =
                    {
                        .port           = eobrd_port_mc4plusP4,
                        .dummy          = 0                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_pos,
                        .port   = eobrd_portpos_hand_medium,  
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
                    .actuator.pwm    =
                    {
                        .port           = eobrd_port_mc4plusP5,
                        .dummy          = 0                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_pos,
                        .port   = eobrd_portpos_hand_pinky,    
                        .pos    = eomc_pos_atjoint
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
                    }
                },
                { // joint 4
                    .actuator.pmc    =
                    {
                        .canloc = 
                        {
                            .port = eOcanport1, 
                            .addr = 1, 
                            .insideindex = eobrd_caninsideindex_none  
                        }                      
                    },                       
                    .encoder1         =
                    {
                        .type   = eomc_enc_pos,
                        .port   = eobrd_portpos_hand_thumbmetacarpus,  
                        .pos    = eomc_pos_atjoint
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
                    }
                },
                { // joint 5
                    .actuator.pmc    =
                    {
                        .canloc = 
                        {
                            .port = eOcanport1, 
                            .addr = 1, 
                            .insideindex = eobrd_caninsideindex_none  
                        }                      
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_pos,
                        .port   = eobrd_portpos_hand_thumbrotation,  
                        .pos    = eomc_pos_atjoint
                    },
                    .encoder2    =
                    {
                        .type   = eomc_enc_none,
                        .port   = eobrd_port_none,
                        .pos    = eomc_pos_none
                    }
                },
                { // joint 6
                    .actuator.pmc    =
                    {
                        .canloc = 
                        {
                            .port = eOcanport1, 
                            .addr = 1, 
                            .insideindex = eobrd_caninsideindex_none  
                        }                      
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_pos,
                        .port   = eobrd_portpos_hand_indexadduction,  
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
        .dummy = 0,        
        .arrayof7jointsets = 
        {
            .head   = 
            {
                .capacity       = 7,
                .itemsize       = sizeof(eOmc_jointset_configuration_t),
                .size           = 7,
                .internalmem    = 0                    
            },
            .data   =
            {
                { // 0                    
                },
                { // 1
                }, 
                { // 2
                },
                { // 3
                },  
                { // 4
                },
                { // 5
                }, 
                { // 7
                }                 
            }           
        }
    }
};

// eo_motcon_mode_mc4plusfaps
static const eOmn_serv_configuration_t s_serv_config_mc_mc4plusfaps =
{   
    .type       = eomn_serv_MC_mc4plusfaps,
    .diagnosticsmode = eomn_serv_diagn_mode_NONE,
    .diagnosticsparam = 0,
    .data.mc.mc4plusfaps = 
    {
        .pos   =
        {
            .version = 
            {
                .firmware = 
                {
                    .major = 1, .minor = 0, .build = 0
                },
                .protocol = 
                {
                    .major = 2, .minor = 0
                }
            },
            .boardInfo = 
            {
                .canloc = 
                {
                    { 
                        .port = eOcanport1, 
                        .addr = 1, 
                        .insideindex = eobrd_caninsideindex_none
                    }        
                }
            }        
            
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
                    .actuator.pwm    =
                    {
                        .port           = eobrd_port_mc4plusP2,
                        .dummy          = 0                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_pos,
                        .port   = eobrd_portpos_hand_thumb,  
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
                    .actuator.pwm    =
                    {
                        .port           = eobrd_port_mc4plusP3,
                        .dummy          = 0                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_pos,
                        .port   = eobrd_portpos_hand_index, 
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
                    .actuator.pwm    =
                    {
                        .port           = eobrd_port_mc4plusP4,
                        .dummy          = 0                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_pos,
                        .port   = eobrd_portpos_hand_medium,  
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
                    .actuator.pwm    =
                    {
                        .port           = eobrd_port_mc4plusP5,
                        .dummy          = 0                             
                    },
                    .encoder1         =
                    {
                        .type   = eomc_enc_pos,
                        .port   = eobrd_portpos_hand_pinky,    
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
            {   // tbd
                { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) }        
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

static const eOmn_serv_configuration_t s_serv_config_mc_eb1_eb3_zeroprotocol =
{   // eb1 / eb3
    .type       = eomn_serv_MC_foc,
    .diagnosticsmode = eomn_serv_diagn_mode_NONE,
    .diagnosticsparam = 0,
    .data.mc.foc_based = 
    {
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
    .diagnosticsmode = eomn_serv_diagn_mode_NONE,
    .diagnosticsparam = 0,
    .data.mc.foc_based = 
    {
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
    .diagnosticsmode = eomn_serv_diagn_mode_NONE,
    .diagnosticsparam = 0,
    .data.mc.foc_based = 
    {
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
    .diagnosticsmode = eomn_serv_diagn_mode_NONE,
    .diagnosticsparam = 0,
    .data.mc.foc_based = 
    {
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
    //s_test_config_ko = &s_serv_config_mc_eb1_fake_inc;
    //s_test_config_ok = &s_serv_config_mc_eb1_eb3_zeroprotocol; 
     
//    s_test_config_ko = &s_serv_config_mc_mc4plusfaps;
//    s_test_config_ok = &s_serv_config_mc_mc4plusfaps;     
    s_test_config_ko = &s_serv_config_mc_mc4pluspmc;
    s_test_config_ok = &s_serv_config_mc_mc4pluspmc;
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


#endif // #if defined(TESTRTC_MC)

// --------------------------------------------------------------------------------------------------------------------
// pos


#if defined(TESTRTC_POS)

#include "EOthePOS.h"


static const eOmn_serv_configuration_t s_serv_config_as_pos =
{   
    .type = eomn_serv_AS_pos,
    .diagnosticsmode = eomn_serv_diagn_mode_NONE,
    .diagnosticsparam = 0,
    .data.as.pos = 
    {
        .version = 
        {
            .firmware = 
            {
                .major = 1, .minor = 0, .build = 0
            },
            .protocol = 
            {
                .major = 2, .minor = 0
            }
        },
        .boardInfo = 
        {
            .canloc = 
            {
                { 
                    .port = eOcanport1, 
                    .addr = 1, 
                    .insideindex = eobrd_caninsideindex_none
                }        
            }
        }        
        
    }
};



static void s_services_test_pos_init(void)
{
    s_test_config_ok = &s_serv_config_as_pos; 
    s_test_config_ko = &s_serv_config_as_pos;
    s_service_tick = s_services_test_pos_multiplesteps;    
}


static void s_services_test_pos_multiplesteps(void *arg)
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
    {   // pos config  
        
        eOas_pos_config_t config = {0};
        config.datarate = 10; // ms

        eo_pos_Config(eo_pos_GetHandle(), &config);
        
        services_stop_ANY_service_now = 0;
        eo_action_SetCallback(s_act, s_services_test_pos_stop, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
        eo_timer_Start(s_timer, eok_abstimeNOW, 1*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act); 
    }
    else if(4 == step)
    {   // regsig  
        
        s_command.category = s_service_under_test;
        s_command.operation = eomn_serv_operation_regsig_load;
        memset(&s_command.parameter.configuration, 0, sizeof(eOmn_serv_configuration_t));
        
        eOmn_serv_arrayof_id32_t id32s = {};
        EOarray* ar = eo_array_New(eOmn_serv_capacity_arrayof_id32, 4, &s_command.parameter.arrayofid32);   
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_pos, 0, eoprot_tag_as_pos_status);
        eo_array_PushBack(ar, &id32);  
                      
            
        // ok, we have received a command for a given service. we ask the object theservices to manage the thing
        
        eo_services_ProcessCommand(eo_services_GetHandle(), &s_command);  
        
        services_stop_ANY_service_now = 0;
        eo_action_SetCallback(s_act, s_services_test_pos_stop, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
        eo_timer_Start(s_timer, eok_abstimeNOW, 1*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act); 
    }
    else if(5 == step)
    {   // service start        
        s_services_test_start(arg);
        
        services_stop_ANY_service_now = 0;
        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, 1*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);    
    }
    else if(6 == step)
    {   // tx enable        
        eo_pos_Transmission(eo_pos_GetHandle(), eobool_true);
        
        services_stop_ANY_service_now = 0;
        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, 30*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);   
    }  
    else if(7 == step)
    {
        s_services_test_stop_everything(NULL);
    }
}


static void s_services_test_pos_stop(void *par)
{   
    s_services_test_stop_everything(NULL);   
}



#endif // #if defined(TESTRTC_POS)



// --------------------------------------------------------------------------------------------------------------------
// ft


#if defined(TESTRTC_FT)

#include "embot_app_eth_theFTservice.h"

static const eOmn_serv_configuration_t s_serv_config_as_ft =
{   
    .type = eomn_serv_AS_ft,
    .diagnosticsmode = eomn_serv_diagn_mode_NONE,
    .diagnosticsparam = 0,
    .data.as.ft.arrayofsensors = 
    {
        .head   = 
        {
            .capacity       = eOas_ft_sensors_maxnumber,
            .itemsize       = sizeof(eOas_ft_sensordescriptor_t),
            .size           = 1,
            .internalmem    = 0                    
        },
        .data   =
        {
            {   // 0
                .boardinfo =
                {
                    .type = eobrd_strain2, 
                    .firmware = {0, 0, 0},
                    .protocol = {2, 0}                      
                },
                .canloc = 
                {
                    .port = eOcanport1, 
                    .addr = 13, 
                    .insideindex = eobrd_caninsideindex_none                    
                },
                .ffu = 0
            },
            {   // 1
                .boardinfo =
                {
                    .type = eobrd_none, 
                    .firmware = {0, 0, 0},
                    .protocol = {0, 0}                    
                },
                .canloc = 
                {
                    .port = eOcanport1, 
                    .addr = 2, 
                    .insideindex = eobrd_caninsideindex_none                    
                },
                .ffu = 0
            },
            {   // 2
                .boardinfo =
                {
                    .type = eobrd_none, 
                    .firmware = {0, 0, 0},
                    .protocol = {0, 0}                    
                },
                .canloc = 
                {
                    .port = eOcanport1, 
                    .addr = 2, 
                    .insideindex = eobrd_caninsideindex_none                    
                },
                .ffu = 0
            },
            {   // 3
                .boardinfo =
                {
                    .type = eobrd_none, 
                    .firmware = {0, 0, 0},
                    .protocol = {0, 0}                    
                },
                .canloc = 
                {
                    .port = eOcanport1, 
                    .addr = 2, 
                    .insideindex = eobrd_caninsideindex_none                    
                },
                .ffu = 0
            }            
        }
     } 
};

static uint8_t step2use = 0;
static uint8_t action_run = 0;


typedef struct 
{
    EOMmutex *mtx;
    actor_t actor;
    eOvoid_fp_voidp_t action;
    void * par;    
} functor_t;

volatile functor_t functor = { .mtx = NULL, .actor = actor_none, .action = NULL, .par = NULL };


static void s_services_test_ft_step();

static void s_services_test_ft_init(void)
{
    s_test_config_ok = &s_serv_config_as_ft; 
    s_test_config_ko = &s_serv_config_as_ft;
    s_service_tick = NULL; //s_services_test_ft_multiplesteps;   
    functor.actor = actor_none;
    functor.action = NULL;
    functor.par = NULL;  
    functor.mtx = eom_mutex_New();
    
    step2use = 0;
    s_services_test_ft_step();
}

void sendevent2cfgby(eOreltime_t delta)
{
    EOVtaskDerived * t = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());
    eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, t);    
    eo_timer_Start(s_timer, eok_abstimeNOW, delta, eo_tmrmode_ONESHOT, s_act);     
}


void functor_fill(actor_t actor, eOvoid_fp_voidp_t action, void *par)
{
    eom_mutex_Take(functor.mtx, eok_reltimeINFINITE);
    functor.actor = actor;
    functor.action = action;
    functor.par = par;
    eom_mutex_Release(functor.mtx);        
}

void serv_STOP(void *p)
{
    s_command.category = s_service_under_test;
    s_command.operation = eomn_serv_operation_stop;
    memset(&s_command.parameter.configuration, 0, sizeof(eOmn_serv_configuration_t));
    eo_services_ProcessCommand(eo_services_GetHandle(), &s_command);           
}

void serv_VERIFYACTIVATE(void *p)
{
    //const eOmn_serv_configuration_t* cfg = (const eOmn_serv_configuration_t*) p;
    s_command.category = s_service_under_test;
    s_command.operation = eomn_serv_operation_verifyactivate;
    memcpy(&s_command.parameter.configuration, s_test_config_ok, sizeof(eOmn_serv_configuration_t));         
    eo_services_ProcessCommand(eo_services_GetHandle(), &s_command);   
}

void serv_CONFIG(void *p)
{       
    eOas_ft_config_t ftconfig =
    {
        .mode = eoas_ft_mode_calibrated,
        .ftdatarate = 100,
        .calibrationset = 0,
        .tempdatarate = 1
    }; 
    
    eOropdescriptor_t ropdes = {};
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, 0, eoprot_tag_as_ft_config);

    embot::app::eth::fill(ropdes, id32, &ftconfig, sizeof(ftconfig));    

    embot::app::eth::theFTservice::getInstance().process(&ropdes);    
}

void serv_TXstart(void *p)
{       
    uint8_t enable = 1;  
    eOropdescriptor_t ropdes = {};
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, 0, eoprot_tag_as_ft_cmmnds_enable);

    embot::app::eth::fill(ropdes, id32, &enable, sizeof(enable));    

    embot::app::eth::theFTservice::getInstance().process(&ropdes);  
}

void fillfunctorTX(void *p)
{
    functor_fill(actor_run, serv_TXstart, NULL);
}

void fillfunctorSTOP(void *p)
{
    functor_fill(actor_run, serv_STOP, NULL);
}

void serv_REGULARS(void *p)
{       
    s_command.category = s_service_under_test;
    s_command.operation = eomn_serv_operation_regsig_load;
    memset(&s_command.parameter.configuration, 0, sizeof(eOmn_serv_configuration_t));
    
    eOmn_serv_arrayof_id32_t id32s = {};
    EOarray* ar = eo_array_New(eOmn_serv_capacity_arrayof_id32, 4, &s_command.parameter.arrayofid32);   
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, 0, eoprot_tag_as_ft_status_timedvalue);
    eo_array_PushBack(ar, &id32);  
    eo_services_ProcessCommand(eo_services_GetHandle(), &s_command);    
}

void serv_START(void *p)
{       
    s_command.category = s_service_under_test;
    s_command.operation = eomn_serv_operation_start;
    memset(&s_command.parameter.configuration, 0, sizeof(eOmn_serv_configuration_t));        
    eo_services_ProcessCommand(eo_services_GetHandle(), &s_command);          
}




static void s_services_test_ft_step()
{
    eOreltime_t delta = 1*eok_reltime1sec;
    eOreltime_t runTXtime = 10*eok_reltime1sec;
    
    
    
    if(0 == step2use)
    {
        // INIT asks to CFG -> STOP()
        functor_fill(actor_cfg, serv_STOP, NULL);        
        sendevent2cfgby(delta);         
    }
    else if(1 == step2use)
    {
        // CFG ask to CFG -> VERIFY_ACTIVATE(okconfig)
        functor_fill(actor_cfg, serv_VERIFYACTIVATE, NULL);        
        sendevent2cfgby(delta);        
    }
//    else if(2 == step2use)
//    {
//        // CFG asks to CFG -> VERIFY_ACTIVATE(okconfig)
//        functor_fill(actor_cfg, serv_VERIFYACTIVATE, &s_test_config_ok);        
//        sendevent2cfgby(delta);        
//    }    
    else if(2 == step2use)
    {
        // CFG asks to CFG -> CFG()
        functor_fill(actor_cfg, serv_CONFIG, NULL);        
        sendevent2cfgby(delta);        
    }     
    else if(3 == step2use)
    {
        // CFG asks to CFG -> REGULARS()
        functor_fill(actor_cfg, serv_REGULARS, NULL);        
        sendevent2cfgby(delta);        
    }     
    else if(4 == step2use)
    {
        // CFG asks to CFG -> START()
        functor_fill(actor_cfg, serv_START, NULL);        
        sendevent2cfgby(delta);        
    } 
    else if(5 == step2use)
    {
        // we are still inside CFG which has just executed serv_START()
        // but we stay in CFG for short time because the START sends the application in RUN mode
        // so, ... next request will be done to RUN
        // CFG asks to RUN -> TX()
        // we exec a callback by delta time which:
        // - fills the functor for the sake of the run thread
        
        functor_fill(actor_none, NULL, NULL);
        
        // this timer will force the TX enable on CAN
        EOVtaskDerived * t = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());
        eo_action_SetCallback(s_act, fillfunctorTX, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, delta, eo_tmrmode_ONESHOT, s_act);
       
        // next time we execute inside RUN ....         
    } 
    else if(6 == step2use)
    {
        // we have just started TX on CAN
        functor_fill(actor_none, NULL, NULL);
        
        // this timer will force the stop of the service
        EOVtaskDerived * t = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());
        eo_action_SetCallback(s_act, fillfunctorSTOP, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, runTXtime, eo_tmrmode_ONESHOT, s_act);
    }
    
    step2use++;
}

void s_services_test_ft_tick(actor_t a)
{
    // we execute only if .... 
    eom_mutex_Take(functor.mtx, eok_reltimeINFINITE);
    if(a == functor.actor)
    {
        if(NULL != functor.action)
        {
            functor.action(functor.par);
        }
        functor_fill(actor_none, NULL, NULL);
        s_services_test_ft_step();        
    }
    eom_mutex_Release(functor.mtx);        
}


//static void s_services_test_ft_multiplesteps(void *arg)
//{
//    // this is a test for fully working activate() /  deactivate()
//    static uint8_t step = 0;
//    
//    step++;
//    
//    if(1 == step)
//    {
//        s_services_test_stop(arg);
//        
//        services_stop_ANY_service_now = 0;
//        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
//        eo_timer_Start(s_timer, eok_abstimeNOW, 1*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);    
//    }
//    else if(2 == step)
//    {   // verify-activate
//        s_services_test_verifyactivate(s_test_config_ok);
//        
//        services_stop_ANY_service_now = 0;
//        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
//        eo_timer_Start(s_timer, eok_abstimeNOW, 1*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);              
//    }
//    else if(3 == step)
//    {   // pos config  
//        
//        eOas_pos_config_t config = {0};
//        config.datarate = 10; // ms

//        eo_pos_Config(eo_pos_GetHandle(), &config);
//        
//        services_stop_ANY_service_now = 0;
//        eo_action_SetCallback(s_act, s_services_test_pos_stop, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
//        eo_timer_Start(s_timer, eok_abstimeNOW, 1*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act); 
//    }
//    else if(4 == step)
//    {   // regsig  
//        
//        s_command.category = s_service_under_test;
//        s_command.operation = eomn_serv_operation_regsig_load;
//        memset(&s_command.parameter.configuration, 0, sizeof(eOmn_serv_configuration_t));
//        
//        eOmn_serv_arrayof_id32_t id32s = {};
//        EOarray* ar = eo_array_New(eOmn_serv_capacity_arrayof_id32, 4, &s_command.parameter.arrayofid32);   
//        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_pos, 0, eoprot_tag_as_pos_status);
//        eo_array_PushBack(ar, &id32);  
//                      
//            
//        // ok, we have received a command for a given service. we ask the object theservices to manage the thing
//        
//        eo_services_ProcessCommand(eo_services_GetHandle(), &s_command);  
//        
//        services_stop_ANY_service_now = 0;
//        eo_action_SetCallback(s_act, s_services_test_pos_stop, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
//        eo_timer_Start(s_timer, eok_abstimeNOW, 1*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act); 
//    }
//    else if(5 == step)
//    {   // service start        
//        s_services_test_start(arg);
//        
//        services_stop_ANY_service_now = 0;
//        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
//        eo_timer_Start(s_timer, eok_abstimeNOW, 1*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);    
//    }
//    else if(6 == step)
//    {   // tx enable        
//        eo_pos_Transmission(eo_pos_GetHandle(), eobool_true);
//        
//        services_stop_ANY_service_now = 0;
//        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
//        eo_timer_Start(s_timer, eok_abstimeNOW, 30*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);   
//    }  
//    else if(7 == step)
//    {
//        s_services_test_stop_everything(NULL);
//    }
//}


//static void s_services_test_pos_stop(void *par)
//{   
//    s_services_test_stop_everything(NULL);   
//}



#endif // #if defined(TESTRTC_FT)


#endif // TESTRTC_IS_ACTIVE

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

