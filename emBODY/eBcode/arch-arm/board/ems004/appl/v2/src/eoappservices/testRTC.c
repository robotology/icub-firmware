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

#include "EOmcController.h"

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

static void s_services_test_mc_multiplesteps(void *arg);

static void s_eo_services_test(void);

static void s_services_test_stop(void *arg);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtimer *s_timer = NULL;

static volatile uint8_t services_stop_MC_service_now = 0;


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
    
    services_stop_MC_service_now = 0;
    
    s_eo_services_test();
    
#endif    
}

extern void testRTC_RUN_tick(void)
{
#if defined(TESTRTC_IS_ACTIVE)

    if(1 == services_stop_MC_service_now)
    {
        services_stop_MC_service_now = 0;
        s_services_test_stop(NULL);
    }
    
#endif    
}

extern void testRTC_CFG_tick(void)
{
#if defined(TESTRTC_IS_ACTIVE)
    
    s_services_test_mc_multiplesteps(NULL);  
    
#endif    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static const eOmn_serv_configuration_t s_serv_config_mc_eb1_eb3_zeroprotocol =
{   // eb1 / eb3
    .type       = eomn_serv_MC_foc,
    .filler     = {0},
    .data.mc.foc_based = 
    {
        .boardtype4mccontroller = emscontroller_board_SHOULDER,
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
                .itemsize       = sizeof(eOmn_serv_jomo_descriptor_t),
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
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_aea,
                        .port   = eomn_serv_mc_port_ems_spiP6, // hal_encoder1   
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
                    }
                },
                { // joint 1
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 2,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_aea,
                        .port   = eomn_serv_mc_port_ems_spiP7, // hal_encoder4
                        .pos    = eomn_serv_mc_sensor_pos_atjoint                            
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
                    }
                },                    
                { // joint 2
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 3,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_aea,
                        .port   = eomn_serv_mc_port_ems_spiP8, // hal_encoder2  
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
                    }
                },               
                { // joint 3
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 4,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_aea,
                        .port   = eomn_serv_mc_port_ems_spiP9,    // hal_encoder5 
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
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

static const eOmn_serv_configuration_t s_serv_config_mc_eb1_fake_aea =
{   // eb1 / eb3
    .type       = eomn_serv_MC_foc,
    .filler     = {0},
    .data.mc.foc_based = 
    {
        .boardtype4mccontroller = emscontroller_board_SHOULDER,
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
                .itemsize       = sizeof(eOmn_serv_jomo_descriptor_t),
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
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_aea,
                        .port   = eomn_serv_mc_port_ems_spiP10,    
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
                    }
                },
                { // joint 1
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 2,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_aea,
                        .port   = eomn_serv_mc_port_ems_spiP7, // hal_encoder4
                        .pos    = eomn_serv_mc_sensor_pos_atjoint                            
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
                    }
                },                    
                { // joint 2
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 3,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_aea,
                        .port   = eomn_serv_mc_port_ems_spiP8, // hal_encoder2  
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
                    }
                },               
                { // joint 3
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 4,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_aea,
                        .port   = eomn_serv_mc_port_ems_spiP9,    // hal_encoder5 
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
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


static const eOmn_serv_configuration_t s_serv_config_mc_eb1_fake_amo =
{   // eb1 / eb3
    .type       = eomn_serv_MC_foc,
    .filler     = {0},
    .data.mc.foc_based = 
    {
        .boardtype4mccontroller = emscontroller_board_SHOULDER,
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
                .itemsize       = sizeof(eOmn_serv_jomo_descriptor_t),
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
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_amo,
                        .port   = eomn_serv_mc_port_ems_spiP6,    
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
                    }
                },
                { // joint 1
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 2,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_amo,
                        .port   = eomn_serv_mc_port_ems_spiP7, // hal_encoder4
                        .pos    = eomn_serv_mc_sensor_pos_atjoint                            
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
                    }
                },                    
                { // joint 2
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 3,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_amo,
                        .port   = eomn_serv_mc_port_ems_spiP8, // hal_encoder2  
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
                    }
                },               
                { // joint 3
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 4,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_amo,
                        .port   = eomn_serv_mc_port_ems_spiP9,    // hal_encoder5 
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
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


static const eOmn_serv_configuration_t s_serv_config_mc_eb1_fake_inc =
{   // eb1 / eb3
    .type       = eomn_serv_MC_foc,
    .filler     = {0},
    .data.mc.foc_based = 
    {
        .boardtype4mccontroller = emscontroller_board_SHOULDER,
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
                .itemsize       = sizeof(eOmn_serv_jomo_descriptor_t),
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
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = eomn_serv_mc_port_mc4plus_qencP2,    
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
                    }
                },
                { // joint 1
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 2,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = eomn_serv_mc_port_mc4plus_qencP3, // hal_encoder4
                        .pos    = eomn_serv_mc_sensor_pos_atjoint                            
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
                    }
                },                    
                { // joint 2
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 3,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = eomn_serv_mc_port_mc4plus_qencP4, // hal_encoder2  
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
                    }
                },               
                { // joint 3
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 4,
                        .insideindex    = eobrd_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = eomn_serv_mc_port_mc4plus_qencP5,    // hal_encoder5 
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
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

static eOmn_service_cmmnds_command_t s_command = {0};

static const eOmn_serv_configuration_t* s_test_config_ok = NULL;
static const eOmn_serv_configuration_t* s_test_config_ko = &s_serv_config_mc_eb1_fake_inc;


static void s_services_test_activate(const eOmn_serv_configuration_t* cfg)
{
        
    s_command.category = eomn_serv_category_mc;
    s_command.operation = eomn_serv_operation_verifyactivate;
    memcpy(&s_command.parameter.configuration, cfg, sizeof(eOmn_serv_configuration_t));
        
    // ok, we have received a command for a given service. we ask the object theservices to manage the thing
    
    eo_services_ProcessCommand(eo_services_GetHandle(), &s_command);    
}

static void s_services_test_start(void *arg)
{
        
    s_command.category = eomn_serv_category_mc;
    s_command.operation = eomn_serv_operation_start;
    memset(&s_command.parameter.configuration, 0, sizeof(eOmn_serv_configuration_t));
        
    // ok, we have received a command for a given service. we ask the object theservices to manage the thing
    
    eo_services_ProcessCommand(eo_services_GetHandle(), &s_command);    
}


static void s_services_test_stop(void *arg)
{
        
    s_command.category = eomn_serv_category_mc;
    s_command.operation = eomn_serv_operation_stop;
    memset(&s_command.parameter.configuration, 0, sizeof(eOmn_serv_configuration_t));
        
    // ok, we have received a command for a given service. we ask the object theservices to manage the thing
    
    eo_services_ProcessCommand(eo_services_GetHandle(), &s_command);    
}


static EOaction_strg s_astrg = {0};
static EOaction *s_act = (EOaction*)&s_astrg;


static void s_services_test_mc_stop(void *par)
{
    services_stop_MC_service_now = 1;
    eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
    eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);    
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
        s_services_test_activate(s_test_config_ko);
        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);          
    }
    else if(3 == step)
    {        
        s_services_test_start(arg);
        
        // prepare to stop it ... by setting a flag which the runner will process and call 
        services_stop_MC_service_now = 0;
        eo_action_SetCallback(s_act, s_services_test_mc_stop, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
        eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act); 
    }
    else if(4 == step)
    {
        services_stop_MC_service_now = 0;
        s_services_test_activate(s_test_config_ok);
        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);    
    }
    else if(5 == step)
    {
        services_stop_MC_service_now = 0;
        s_services_test_start(arg);
    }

#else
    
    // this is a test for simple activate() and start() in case of not fully working deactivate() of mc, as it is at the data of 16 may 2016
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
        s_services_test_activate(s_test_config_ok);
        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);          
    }
    else if(3 == step)
    {        
        s_services_test_start(arg);
        
        // prepare to stop it ... by setting a flag which the runner will process and call 
        services_stop_MC_service_now = 0;
        eo_action_SetCallback(s_act, s_services_test_mc_stop, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
        eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act); 
    }
    else if(4 == step)
    {
        services_stop_MC_service_now = 0;
        s_services_test_activate(s_test_config_ok);
        eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
        eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);    
    }
    else if(5 == step)
    {
        services_stop_MC_service_now = 0;
        s_services_test_start(arg);
    }    
    
#endif
          
}



static void s_eo_services_test(void)
{
    
    s_test_config_ok = &s_serv_config_mc_eb1_eb3_zeroprotocol; // board eb1, with no check of can boards
    
    // start a timer of 1 seconds which will activate verify a service of motioncontrol for board eb1
    
    
    //eo_action_SetCallback(act, s_services_test_mc_multiplesteps, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
    
    eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));    
    eo_timer_Start(s_timer, eok_abstimeNOW, 3*eok_reltime1sec, eo_tmrmode_ONESHOT, s_act);     
}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



