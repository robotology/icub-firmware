/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       EOtheBoardConfig.c
    @brief      This file keeps ...
    @author     marco.accame@iit.it
    @date       Nov 10 2014
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"
#include "EOconstvector_hid.h"
#include "EOtheCANmapping.h"
#include "EOappEncodersReader.h"

#include "EOmcController.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheBoardConfig.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const eOmn_serv_configuration_t s_serv_config_as_strain_eb1_eb3 = 
{   // eb1 or eb3
    .type       = eomn_serv_AS_strain,
    .filler     = {0},
    .data.as.strain = 
    {
        .version    =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 1, .minor = 0 }
        },
        .canloc         =
        {
            .port           = eOcanport2,
            .addr           = 13,
            .insideindex    = eomn_serv_caninsideindex_none                   
        }
    }    
};

static const eOmn_serv_configuration_t s_serv_config_as_strain_eb6_eb8 = 
{   // eb6 or eb8
    .type       = eomn_serv_AS_strain,
    .filler     = {0},
    .data.as.strain = 
    {
        .version    =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 1, .minor = 0 }
        },
        .canloc         =
        {
            .port           = eOcanport2,
            .addr           = 13,
            .insideindex    = eomn_serv_caninsideindex_none                   
        }
    }    
};

#if     defined(ICUB_MEC_V2)  
static const eOmn_serv_configuration_t s_serv_config_as_strain_eb7_eb9 =
{   // eb7 or eb9
    .type       = eomn_serv_AS_strain,
    .filler     = {0},
    .data.as.strain = 
    {
        .version    =
        {
            .firmware   = {0, 0},
            .protocol   = { .major = 1, .minor = 0 }
        },
        .canloc         =
        {
            .port           = eOcanport2,
            .addr           = 1,
            .insideindex    = eomn_serv_caninsideindex_none                   
        }
    }    
};
#endif

static const eOmn_serv_configuration_t s_serv_config_mc_eb1_eb3 =
{   // eb1 / eb3
    .type       = eomn_serv_MC_foc,
    .filler     = {0},
    .data.mc.foc_based = 
    {
        .boardtype4mccontroller = emscontroller_board_SHOULDER,
        .version   =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 1, .minor = 3 }
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
                        .insideindex    = eomn_serv_caninsideindex_first                             
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
                        .insideindex    = eomn_serv_caninsideindex_first                             
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
                        .insideindex    = eomn_serv_caninsideindex_first                             
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
                        .insideindex    = eomn_serv_caninsideindex_first                             
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
        .couplingjoint2motor    = 
        {   // zero matrix: use matrix embedded in controller and seecetd by boardtype4mccontroller
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) }        
        },
        .couplingjoint2encoder  = 
        {   // identical matrix
            { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) } 
        }        
    }
};


static const eOmn_serv_configuration_t s_serv_config_mc_eb2_eb4 =
{   // eb2 / eb4
    .type       = eomn_serv_MC_mc4,
    .filler     = {0},
    .data.mc.mc4_based  = 
    {
        .mc4version =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 1, .minor = 6 }                 
        },
        .mc4shifts =
        {
            0  
        },
        .mc4joints =
        {
            {   // jomo 0   
                .port           = eOcanport1,
                .addr           = 3,
                .insideindex    = eocanmap_insideindex_first
            },      
            {   // jomo 1
                .port           = eOcanport1,
                .addr           = 3,
                .insideindex    = eocanmap_insideindex_second
            },
            {   // jomo 2
                .port           = eOcanport1,
                .addr           = 4,
                .insideindex    = eocanmap_insideindex_first
            },
            {   // jomo 3
                .port           = eOcanport1,
                .addr           = 4,
                .insideindex    = eocanmap_insideindex_second
            },
            {   // jomo 4   
                .port           = eOcanport1,
                .addr           = 5,
                .insideindex    = eocanmap_insideindex_first
            },
            {   // jomo 
                .port           = eOcanport1,
                .addr           = 5,
                .insideindex    = eocanmap_insideindex_second
            },
            {   // jomo 6
                .port           = eOcanport1,
                .addr           = 6,
                .insideindex    = eocanmap_insideindex_first
            },
            {   // jomo 7
                .port           = eOcanport1,
                .addr           = 6,
                .insideindex    = eocanmap_insideindex_second
            },
            {   // jomo 8   
                .port           = eOcanport1,
                .addr           = 7,
                .insideindex    = eocanmap_insideindex_first
            },
            {   // jomo 9
                .port           = eOcanport1,
                .addr           = 7,
                .insideindex    = eocanmap_insideindex_second
            },
            {   // jomo 10
                .port           = eOcanport1,
                .addr           = 8,
                .insideindex    = eocanmap_insideindex_first
            },
            {   // jomo 11
                .port           = eOcanport1,
                .addr           = 8,
                .insideindex    = eocanmap_insideindex_second
            }
        },
        .mais = 
        {
            .version    =
            {
                .firmware   = { .major = 0, .minor = 0, .build = 0 },
                .protocol   = { .major = 1, .minor = 0 }    
            },
            .canloc = 
            {
                .port           = eOcanport1,
                .addr           = 14,
                .insideindex    = eocanmap_insideindex_none                    
            }              
        }
    }    
};    

static const eOmn_serv_configuration_t s_serv_config_mc_eb5 =
{   // eb5
    .type       = eomn_serv_MC_foc,
    .filler     = {0},
    .data.mc.foc_based = 
    {
        .boardtype4mccontroller = emscontroller_board_WAIST,
        .version   =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 1, .minor = 3 }
        },
        .filler                 = {0},
        .arrayofjomodescriptors =
        {
            .head   = 
            {
                .capacity       = 4,
                .itemsize       = sizeof(eOmn_serv_jomo_descriptor_t),
                .size           = 3,
                .internalmem    = 0                    
            },
            .data   =
            {
                { // joint 0
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 3,
                        .insideindex    = eomn_serv_caninsideindex_first                             
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
                        .addr           = 4,
                        .insideindex    = eomn_serv_caninsideindex_first                             
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
                        .addr           = 1,
                        .insideindex    = eomn_serv_caninsideindex_first                             
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
                {   // there is not a joint 3 
                    0 
                }                   
            }  
        },
        .couplingjoint2motor    = 
        {   // zero matrix: use matrix embedded in controller and seecetd by boardtype4mccontroller
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) }        
        },
        .couplingjoint2encoder  = 
        {   // identical matrix
            { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) } 
        }            
    }
}; 

static const eOmn_serv_configuration_t s_serv_config_mc_eb6_eb8 =
{   // eb6 / eb8
    .type       = eomn_serv_MC_foc,
    .filler     = {0},
    .data.mc.foc_based = 
    {
        .boardtype4mccontroller = emscontroller_board_UPPERLEG,
        .version   =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 1, .minor = 3 }
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
                        .addr           = 3,
                        .insideindex    = eomn_serv_caninsideindex_first                             
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
                        .addr           = 4,
                        .insideindex    = eomn_serv_caninsideindex_first                             
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
                        .addr           = 1,
                        .insideindex    = eomn_serv_caninsideindex_first                             
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
                        .addr           = 2,
                        .insideindex    = eomn_serv_caninsideindex_first                             
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
        .couplingjoint2motor    = 
        {   // zero matrix: use matrix embedded in controller and seecetd by boardtype4mccontroller
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) }        
        },
        .couplingjoint2encoder  = 
        {   // identical matrix
            { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) } 
        }             
    }
};

static const eOmn_serv_configuration_t s_serv_config_mc_eb7_eb9 =
{   // eb7 / eb9
    .type       = eomn_serv_MC_foc,
    .filler     = {0},
    .data.mc.foc_based = 
    {
        .boardtype4mccontroller = emscontroller_board_ANKLE,
        .version   =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 1, .minor = 3 }
        },
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
                { // joint 0
                    .actuator.foc.canloc    =
                    {
                        .port           = eOcanport1,
                        .addr           = 1,
                        .insideindex    = eomn_serv_caninsideindex_first                             
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
                        .insideindex    = eomn_serv_caninsideindex_first                             
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
                { // no joint 2
                    0
                },               
                { // no joint 3
                    0
                }                    
            }
        },
        .couplingjoint2motor    = 
        {   // zero matrix: use matrix embedded in controller and seecetd by boardtype4mccontroller
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) }        
        },
        .couplingjoint2encoder  = 
        {   // identical matrix
            { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) } 
        }            
    }
};



static const eOmn_serv_configuration_t s_serv_config_mc_cer_eb15 =
{   // eb15
    .type       = eomn_serv_MC_foc,
    .filler     = {0},
    .data.mc.foc_based = 
    {
        .boardtype4mccontroller = emscontroller_board_CER_WAIST,
        .version    =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 1, .minor = 3 }
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
                        .insideindex    = eomn_serv_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
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
                        .insideindex    = eomn_serv_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none                         
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
                        .insideindex    = eomn_serv_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
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
                        .insideindex    = eomn_serv_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
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
        .couplingjoint2motor    = 
        {   // zero matrix: use matrix embedded in controller and seecetd by boardtype4mccontroller
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) }        
        },
        .couplingjoint2encoder  = 
        {   // identical matrix
            { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) } 
        }            
    }
};



static const eOmn_serv_configuration_t s_serv_config_mc_v3_0B0 =
{   // eb12 or 0B0
    .type       = eomn_serv_MC_mc4plus,
    .filler     = {0},
    .data.mc.mc4plus_based = 
    {
        .boardtype4mccontroller = emscontroller_board_HEAD_neckpitch_neckroll,
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
                { // joint 0: neck-pitch
                    .actuator.pwm   =
                    {
                        .port   = 0  // eomn_serv_mc_port_mc4plus_pwmP3 is hal_motor1=0 ?? verify!                          
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_aea,
                        .port   = 1, // eomn_serv_mc_port_mc4plus_spiP11 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 0,  // eomn_serv_mc_port_mc4plus_qencP3 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atmotor
                    }
                },
                { // joint 1: neck-roll
                    .actuator.pwm   =
                    {
                        .port   = 2  // eomn_serv_mc_port_mc4plus_pwmP4 is hal_motor3=2 ?? verify!                          
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_aea,
                        .port   = 0, // eomn_serv_mc_port_mc4plus_spiP10 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 2,  // eomn_serv_mc_port_mc4plus_qencP4 ?? verify!
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
                        .port   = eomn_serv_mc_port_none, 
                        .pos    = eomn_serv_mc_sensor_pos_none
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,  
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
                        .port   = eomn_serv_mc_port_none, 
                        .pos    = eomn_serv_mc_sensor_pos_none
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
        .couplingjoint2motor    = 
        {   // zero matrix: use matrix embedded in controller and seecetd by boardtype4mccontroller
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) }        
        },
        .couplingjoint2encoder  = 
        {   // identical matrix
            { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) } 
        }            
    }
};




static const eOmn_serv_configuration_t s_serv_config_mc_v3_0B1 =
{   // eb13 or 0B1
    .type       = eomn_serv_MC_mc4plus,
    .filler     = {0},
    .data.mc.mc4plus_based = 
    {
        .boardtype4mccontroller = emscontroller_board_HEAD_neckyaw_eyes,
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
                { // joint 0: neck-yaw
                    .actuator.pwm   =
                    {
                        .port   = 0  // eomn_serv_mc_port_mc4plus_pwmP3 is hal_motor1=0 ?? verify!                          
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_aea,
                        .port   = 1, // eomn_serv_mc_port_mc4plus_spiP11 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 0,  // eomn_serv_mc_port_mc4plus_qencP3 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atmotor
                    }
                },
                { // joint 1: eyes-tilt
                    .actuator.pwm   =
                    {
                        .port   = 1  // eomn_serv_mc_port_mc4plus_pwmP2 is hal_motor2=1 ?? verify!                          
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_aea,
                        .port   = 0, // eomn_serv_mc_port_mc4plus_spiP10 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 1,  // eomn_serv_mc_port_mc4plus_qencP2 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atmotor
                    }
                },                
                { // joint 2: right-eye
                    .actuator.pwm   =
                    {
                        .port   = 3  // eomn_serv_mc_port_mc4plus_pwmP5 is hal_motor4=3 ?? verify!                          
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 3, // eomn_serv_mc_port_mc4plus_qencP5 is hal_quad_enc4 = 3 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 3, // eomn_serv_mc_port_mc4plus_qencP5 is hal_quad_enc4 = 3 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atmotor
                    }
                },                               
                { // joint 3: left-eye
                    .actuator.pwm   =
                    {
                        .port   = 2  // eomn_serv_mc_port_mc4plus_pwmP4 is hal_motor3=2 ?? verify!                          
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 2, // eomn_serv_mc_port_mc4plus_qencP4 is hal_quad_enc3 = 2 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 2, // eomn_serv_mc_port_mc4plus_qencP4 is hal_quad_enc3 = 2 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atmotor
                    }
                }            
            }
        },
        .couplingjoint2motor    = 
        {   // zero matrix: use matrix embedded in controller and seecetd by boardtype4mccontroller
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) }        
        },
        .couplingjoint2encoder  = 
        {   // identical matrix
            { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) } 
        }           
    }
};


static const eOmn_serv_configuration_t s_serv_config_mc_v3_0B7 =
{   // eb14 or 0B7
    .type       = eomn_serv_MC_mc4plus,
    .filler     = {0},
    .data.mc.mc4plus_based = 
    {
        .boardtype4mccontroller = emscontroller_board_FACE_eyelids_jaw,
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
                { // joint 0: jaw
                    .actuator.pwm   =
                    {
                        .port   = 0  // eomn_serv_mc_port_mc4plus_pwmP3 is hal_motor1=0 ?? verify!                          
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_aea,
                        .port   = 1, // eomn_serv_mc_port_mc4plus_spiP11 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,  
                        .pos    = eomn_serv_mc_sensor_pos_none
                    }
                },
                { // joint 1: eyelids
                    .actuator.pwm   =
                    {
                        .port   = 1  // eomn_serv_mc_port_mc4plus_pwmP2 is hal_motor2=1 ?? verify!                          
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_aea,
                        .port   = 0, // eomn_serv_mc_port_mc4plus_spiP10 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 1,  // eomn_serv_mc_port_mc4plus_qencP2 ?? verify!
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
                        .port   = eomn_serv_mc_port_none, 
                        .pos    = eomn_serv_mc_sensor_pos_none
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,  
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
                        .port   = eomn_serv_mc_port_none, 
                        .pos    = eomn_serv_mc_sensor_pos_none
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
        .couplingjoint2motor    = 
        {   // zero matrix: use matrix embedded in controller and seecetd by boardtype4mccontroller
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) }        
        },
        .couplingjoint2encoder  = 
        {   // identical matrix
            { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) } 
        }            
    }
};

static const eOmn_serv_configuration_t s_serv_config_mc_v3_0B9 =
{   // eb15 or 0B9
    .type       = eomn_serv_MC_mc4plus,
    .filler     = {0},
    .data.mc.mc4plus_based = 
    {
        .boardtype4mccontroller = emscontroller_board_FACE_lips,
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
                { // joint 0: lip-left
                    .actuator.pwm   =
                    {
                        .port   = 2  // eomn_serv_mc_port_mc4plus_pwmP4 is hal_motor2=1 ?? verify!                          
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 2, // eomn_serv_mc_port_mc4plus_qencP4 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 2,  // eomn_serv_mc_port_mc4plus_qencP4 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atmotor
                    }
                },
                { // joint 1: lip-high
                    .actuator.pwm   =
                    {
                        .port   = 1  // eomn_serv_mc_port_mc4plus_pwmP2 is hal_motor2=1 ?? verify!                          
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 1, // eomn_serv_mc_port_mc4plus_qencP2 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 1,  // eomn_serv_mc_port_mc4plus_qencP2 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atmotor
                    }
                },                
                { // joint 2: lip-right
                    .actuator.pwm   =
                    {
                        .port   = 0  // eomn_serv_mc_port_mc4plus_pwmP3 is hal_motor4=3 ?? verify!                          
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 0, // eomn_serv_mc_port_mc4plus_qencP3 is hal_quad_enc1 = 0 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 0, // eomn_serv_mc_port_mc4plus_qencP3 is hal_quad_enc1 = 0 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atmotor
                    }
                },                               
                { // joint 3: lip-bottom
                    .actuator.pwm   =
                    {
                        .port   = 3  // eomn_serv_mc_port_mc4plus_pwmP5 is hal_motor3=2 ?? verify!                          
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 3, // eomn_serv_mc_port_mc4plus_qencP5 is hal_quad_enc4 = 3 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 3, // eomn_serv_mc_port_mc4plus_qencP5 is hal_quad_enc4 = 3 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atmotor
                    }
                }            
            }
        },
        .couplingjoint2motor    = 
        {   // zero matrix: use matrix embedded in controller and seecetd by boardtype4mccontroller
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) }        
        },
        .couplingjoint2encoder  = 
        {   // identical matrix
            { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) } 
        }           
    }
};



static const eOmn_serv_configuration_t s_serv_config_mc_cer_eb21 =
{   // eb21
    .type       = eomn_serv_MC_foc,
    .filler     = {0},
    .data.mc.foc_based = 
    {
        .boardtype4mccontroller = emscontroller_board_CER_BASE,
        .version    =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 1, .minor = 3 }
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
                        .addr           = 4,
                        .insideindex    = eomn_serv_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
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
                        .insideindex    = eomn_serv_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none                         
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
                        .insideindex    = eomn_serv_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
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
                        .addr           = 1,
                        .insideindex    = eomn_serv_caninsideindex_first                             
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,
                        .pos    = eomn_serv_mc_sensor_pos_none
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
        .couplingjoint2motor    = 
        {   // zero matrix: use matrix embedded in controller and seecetd by boardtype4mccontroller
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) }        
        },
        .couplingjoint2encoder  = 
        {   // identical matrix
            { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) } 
        }            
    }
};

static const eOmn_serv_configuration_t s_serv_config_mc_cer_testOfmc2plus =
{   // use address .1 then change it
    .type       = eomn_serv_MC_mc4plus, // it is ok to use it for now.
    .filler     = {0},
    .data.mc.mc4plus_based = 
    {
        .boardtype4mccontroller = emscontroller_board_FACE_eyelids_jaw, // it uses two independent joints
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
                        .port   = 0  // eomn_serv_mc_port_mc4plus_pwmP3 is hal_motor1=0 ?? verify!                          
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc, // eomn_serv_mc_sensor_encoder_spichainof2, // eomn_serv_mc_sensor_encoder_inc, // or eomn_serv_mc_sensor_encoder_spichainof2
                        .port   = 0, // eomn_serv_mc_port_mc4plus_pwmP3 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc, // or: eomn_serv_mc_sensor_encoder_spichainof2,
                        .port   = 0, // port J2   of mc2plus, which is connected to encoder1 = 0.
                        .pos    = eomn_serv_mc_sensor_pos_atmotor
                    }
                },
                { // joint 1: second
                    .actuator.pwm   =
                    {
                        .port   = 1  // eomn_serv_mc_port_mc4plus_pwmP2 is hal_motor2=1 ?? verify!                          
                    },
                    .sensor         =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 1, // eomn_serv_mc_port_mc4plus_pwmP2 ?? verify!
                        .pos    = eomn_serv_mc_sensor_pos_atjoint
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_encoder_inc,
                        .port   = 1,  
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
                        .port   = eomn_serv_mc_port_none, 
                        .pos    = eomn_serv_mc_sensor_pos_none
                    },
                    .extrasensor    =
                    {
                        .type   = eomn_serv_mc_sensor_none,
                        .port   = eomn_serv_mc_port_none,  
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
                        .port   = eomn_serv_mc_port_none, 
                        .pos    = eomn_serv_mc_sensor_pos_none
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
        .couplingjoint2motor    = 
        {   // zero matrix: use matrix embedded in controller and seecetd by boardtype4mccontroller
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) }        
        },
        .couplingjoint2encoder  = 
        {   // identical matrix
            { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
            { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) } 
        }            
    }
};


static const eOmn_serv_configuration_t s_serv_config_as_mais_eb2_eb4 =  
{   // eb2/2b4
    .type       = eomn_serv_AS_mais,
    .filler     = {0},
    .data.as.mais = 
    {
        .version    =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 1, .minor = 0 }    
        },
        .canloc = 
        {
            .port           = eOcanport1,
            .addr           = 14,
            .insideindex    = eocanmap_insideindex_none                    
        }
    }    
};


static const eOmn_serv_configuration_t s_serv_config_sk_skin_eb2_eb4 =
{   // eb2 / eb4
    .type       = eomn_serv_SK_skin,
    .filler     = {0},
    .data.sk.skin = 
    {
        .version    =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 0, .minor = 0 }  // in case of {0, 0} the can discovery is not done but the verify will be ok. for normal case use: {1, 0}    
        },
        .numofpatches   = 1,
        .canmapskin[0] = { 0x0000, 0x7f00 },
        .canmapskin[1] = {0},
        .canmapskin[2] = {0},
        .canmapskin[3] = {0}
    }    
};

static const eOmn_serv_configuration_t s_serv_config_sk_skin_eb10_eb11 =   
{   // eb10 / eb11
    .type       = eomn_serv_SK_skin,
    .filler     = {0},
    .data.sk.skin = 
    {
        .version    =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 0, .minor = 0 }  // in case of {0, 0} the can discovery is not done but the verify will be ok. for normal case use: {1, 0}   
        },
        .numofpatches   = 2,
        .canmapskin[0] = { 0x00fe, 0x0000 },
        .canmapskin[1] = { 0x0000, 0x3f00 },
        .canmapskin[2] = {0},
        .canmapskin[3] = {0}
    }    
};    




static const eOmn_serv_configuration_t s_serv_config_as_inertial_eb2_eb4 =
{   // eb2 / eb4
    .type       = eomn_serv_AS_inertial,
    .filler     = {0},
    .data.as.inertial = 
    {
        .version    =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 0, .minor = 0 }  // in case of {0, 0} the can discovery is not done but the verify will be ok. for normal case use: {1, 0}  
        },
        .canmap = { 0x0000, 0x7f00 }
    }    
};
    

static const eOmn_serv_configuration_t s_serv_config_as_inertial_eb10_eb11 =    
{   // eb10 / eb11
    .type       = eomn_serv_AS_inertial,
    .filler     = {0},
    .data.as.inertial = 
    {
        .version    =
        {
            .firmware   = { .major = 0, .minor = 0, .build = 0 },
            .protocol   = { .major = 0, .minor = 0 }  // in case of {0, 0} the can discovery is not done but the verify will be ok. for normal case use: {1, 0}  
        },
        .canmap = { 0x00fe, 0x3f00 }
    }    
};  


enum {maxboards_V2 = 11};
static const eOmn_serv_configuration_t * const s_serv_config_mc_V2[maxboards_V2] =
{   // there are at most 11 ems boards. only board eb10 and eb11 (pos 9 and 10) dont have mc.
    &s_serv_config_mc_eb1_eb3,
    &s_serv_config_mc_eb2_eb4,
    &s_serv_config_mc_eb1_eb3,
    &s_serv_config_mc_eb2_eb4,    
    &s_serv_config_mc_eb5,
    &s_serv_config_mc_eb6_eb8,
    &s_serv_config_mc_eb7_eb9,    
    &s_serv_config_mc_eb6_eb8,
    &s_serv_config_mc_eb7_eb9,     
    NULL,
    NULL
};

enum {maxboards_V3 = 21};
static const eOmn_serv_configuration_t * const s_serv_config_mc_V3[maxboards_V3] =
{   // there are only ....   
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    &s_serv_config_mc_v3_0B0,   // board ip.12, 0b0, head neck pitch + neck roll
    &s_serv_config_mc_v3_0B1,   // board ip.13, 0b1, head neck yaw + 3 eyes
    &s_serv_config_mc_v3_0B7,   // board ip.14, 0b7, face eyelids + jaw
    &s_serv_config_mc_v3_0B9,   // board ip.15, 0b9, face 4 lips
    NULL,
    NULL,
    NULL,
    NULL,
    &s_serv_config_mc_cer_testOfmc2plus,
    NULL    
};

enum {maxboards_CER = 21};
static const eOmn_serv_configuration_t * const s_serv_config_mc_CER[maxboards_CER] =
{   // there are only eb15 and eb21 .   
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    &s_serv_config_mc_cer_eb15,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    &s_serv_config_mc_cer_eb21    
};



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern const eOemsrunner_timing_t * eoboardconfig_code2ctrlooptiming(uint32_t code)
{
    static const eOemsrunner_timing_t normal = 
    {   // 400-300-300, but maybe better doing 400-250-350 or even 400-200-400
        .period         = 1000,
        .rxstartafter   = 0,
        .dostartafter   = 400,
        .txstartafter   = 700,
        .safetygap      = 50          
    };
    
    static const eOemsrunner_timing_t gateway = 
    {   // 450-050-500 .... or even we dont wait the can tx end in the tx phase.
        .period         = 1000,
        .rxstartafter   = 0,
        .dostartafter   = 450,
        .txstartafter   = 500,
        .safetygap      = 25          
    };
    
    const eOemsrunner_timing_t *ret = &normal;
 
    switch(code)
    {
        case 1:
        case 3:
        case 9:
        case 10:            
        {   // eb2, eb4, eb10, eb11
            ret = &gateway;
        } break;
         
        default:    
        {   // all the others
            ret = &normal;
        } break;
    
    }

    return(ret);   
}
    


// ok, verified by marco.accame on 07jan16
extern const eOmn_serv_configuration_t * eoboardconfig_code2motion_serv_configuration(uint32_t code)
{
    const eOmn_serv_configuration_t * ret = NULL;

#if     defined(ICUB_MEC_V3)
    if(code < maxboards_V3)
    {   
        ret = s_serv_config_mc_V3[code]; 
    }        
#elif   defined(CER)
    if(code < maxboards_CER)
    {
        ret= s_serv_config_mc_CER[code];
    }
#else
    if(code < maxboards_V2)
    {   // boards from eb1 to eb11
        ret = s_serv_config_mc_V2[code]; 
    }
#endif    

    return(ret);        
}

// ok, verified by marco.accame on 30dec15
extern const eOmn_serv_configuration_t * eoboardconfig_code2strain_serv_configuration(uint32_t code)
{
    const eOmn_serv_configuration_t * ret = NULL;
    
    switch(code)
    {
        case 0:
        case 2:
        {   // board eb1 / eb3 / eb15  
            ret = &s_serv_config_as_strain_eb1_eb3; 
        } break;  
        
        case 5:
        case 7:
        {   // board eb6 / eb8
            ret = &s_serv_config_as_strain_eb6_eb8;           
        } break;        
 
#if     defined(ICUB_MEC_V2)        
        case 6:
        case 8:
        {   // board eb7 / eb9 ... but only in v2-mec
            ret = &s_serv_config_as_strain_eb7_eb9;           
        } break; 
#endif  

//#if     defined(CER)   
//#error -> if CER board 10.0.1.15 has a strain keep code. if not remove it. then remove error pragma, committ and push.     
//        case 14:
//        {   // CER board eb15: has a strain ... 
//            ret = &s_serv_config_as_strain_eb1_eb3; 
//        } break;          
//#endif
        
        default:    
        {   // all the others
            ret = NULL;
        } break;
    
    }

    return(ret);        
}


// ok, verified by marco.accame on 30dec15
extern const eOmn_serv_configuration_t * eoboardconfig_code2mais_serv_configuration(uint32_t code)
{
    const eOmn_serv_configuration_t * ret = NULL;
    
    switch(code)
    {
        case 1:
        case 3:
        {   // board eb2 / eb4   
            ret = &s_serv_config_as_mais_eb2_eb4; 
        } break;  

        default:    
        {   // all the others
            ret = NULL;
        } break;
    
    }

    return(ret);        
}

// ok, verified by marco.accame on 30dec15
extern const eOmn_serv_configuration_t * eoboardconfig_code2skin_serv_configuration(uint32_t code)
{
    const eOmn_serv_configuration_t * ret = NULL;
    
//    #warning --> rimuovo la skin per debug sul viola che ha problemi sul can2 della eb2
//    return(NULL);
    
    switch(code)
    {
        case 1:
        case 3:
        {   // board eb2 / eb4   
            ret = &s_serv_config_sk_skin_eb2_eb4; 
        } break;  

        case 9:
        case 10:
        {   // board eb10 / eb11   
            ret = &s_serv_config_sk_skin_eb10_eb11; 
        } break;         
        
        default:    
        {   // all the others
            ret = NULL;
        } break;
    
    }

    return(ret);        
}

// ok, verified by marco.accame on 07jan16
extern const eOmn_serv_configuration_t * eoboardconfig_code2inertials_serv_configuration(uint32_t code)
{
    const eOmn_serv_configuration_t * ret = NULL;

//    #warning --> rimuovo inerziali per debug sul viola che ha problemi sul can2 della eb2
//    return(NULL);
    
    switch(code)
    {
        case 1:
        case 3:
        {   // board eb2 / eb4   
            ret = &s_serv_config_as_inertial_eb2_eb4; 
        } break;  

        case 9:
        case 10:
        {   // board eb10 / eb11   
            ret = &s_serv_config_as_inertial_eb10_eb11; 
        } break;        
        
        default:    
        {   // all the others
            ret = NULL;
        } break;
    
    }

    return(ret);        
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



