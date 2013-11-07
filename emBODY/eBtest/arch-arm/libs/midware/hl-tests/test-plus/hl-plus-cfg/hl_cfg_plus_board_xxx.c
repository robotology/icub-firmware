/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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


/* @file       hl_cfg_plus_board_xxx.c
	@brief      This file contains configuration variables for the hl utilities which are specific for a given board.
	@author     marco.accame@iit.it
    @date       10/21/2013
**/


// - modules to be built: contains the HL_USE_* macros ----------------------------------------------------------------

#include "hl_cfg_plus_modules.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "hl_common.h"
#include "hl_core.h"        // contains the required stm32f10x_*.h or stm32f4xx*.h header files

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 

//#include "hl_common_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

#if     defined(HL_USE_UTIL_I2C)

#include "hl_i2c_hid.h"

#if     defined(HL_USE_BRD_MCBSTM32_C)
extern const hl_i2c_hid_brdcfg_t hl_brdcfg_i2c__theconfig =
{
    .supported_mask     = (1 << hl_i2c1) | (0 << hl_i2c2) | (0 << hl_i2c3),
    .gpio_scl           =
    {
        {   // hl_i2c1
            .port   = hl_gpio_portB,
            .pin    = hl_gpio_pin8   
        },
        {   // hl_i2c2
            .port   = hl_gpio_portNONE,
            .pin    = hl_gpio_pinNONE   
        },
        {   // hl_i2c3
            .port   = hl_gpio_portNONE,
            .pin    = hl_gpio_pinNONE   
        }        
    },
    .gpio_sda           =
    {
        {   // hl_i2c1
            .port   = hl_gpio_portB,
            .pin    = hl_gpio_pin9   
        },
        {   // hl_i2c2
            .port   = hl_gpio_portNONE,
            .pin    = hl_gpio_pinNONE   
        },
        {   // hl_i2c3
            .port   = hl_gpio_portNONE,
            .pin    = hl_gpio_pinNONE   
        }            
    },
    .altf_scl           =
    {
        {   // hl_i2c1
            .f1.gpio_remap  = GPIO_Remap_I2C1 
        },
        {   // hl_i2c2
            .f1.gpio_remap  = hl_NA32 
        },
        {   // hl_i2c3
            .f1.gpio_remap  = hl_NA32 
        }           
    },
    .altf_sda           =
    {
        {   // hl_i2c1
            .f1.gpio_remap  = GPIO_Remap_I2C1 
        },
        {   // hl_i2c2
            .f1.gpio_remap  = hl_NA32 
        },
        {   // hl_i2c3
            .f1.gpio_remap  = hl_NA32 
        }           
    }          
};
#elif   defined(HL_USE_BRD_MCBSTM32)
extern const hl_i2c_hid_brdcfg_t hl_brdcfg_i2c__theconfig =
{
    .supported_mask     = (1 << hl_i2c1) | (0 << hl_i2c2) | (0 << hl_i2c3),
    .gpio_scl           =
    {
        {   // hl_i2c1
            .port   = hl_gpio_portB,
            .pin    = hl_gpio_pin8   
        },
        {   // hl_i2c2
            .port   = hl_gpio_portNONE,
            .pin    = hl_gpio_pinNONE   
        },
        {   // hl_i2c3
            .port   = hl_gpio_portNONE,
            .pin    = hl_gpio_pinNONE   
        }        
    },
    .gpio_sda           =
    {
        {   // hl_i2c1
            .port   = hl_gpio_portB,
            .pin    = hl_gpio_pin9   
        },
        {   // hl_i2c2
            .port   = hl_gpio_portNONE,
            .pin    = hl_gpio_pinNONE   
        },
        {   // hl_i2c3
            .port   = hl_gpio_portNONE,
            .pin    = hl_gpio_pinNONE   
        }            
    },
    .altf_scl           =
    {
        {   // hl_i2c1
            .f1.gpio_remap  = GPIO_Remap_I2C1 
        },
        {   // hl_i2c2
            .f1.gpio_remap  = hl_NA32 
        },
        {   // hl_i2c3
            .f1.gpio_remap  = hl_NA32 
        }           
    },
    .altf_sda           =
    {
        {   // hl_i2c1
            .f1.gpio_remap  = GPIO_Remap_I2C1 
        },
        {   // hl_i2c2
            .f1.gpio_remap  = hl_NA32 
        },
        {   // hl_i2c3
            .f1.gpio_remap  = hl_NA32 
        }           
    }          
};

#elif   defined(HL_USE_BRD_EMS001)
extern const hl_i2c_hid_brdcfg_t hl_brdcfg_i2c__theconfig =
{
    .supported_mask     = (1 << hl_i2c1) | (0 << hl_i2c2) | (0 << hl_i2c3),
    .gpio_scl           =
    {
        {   // hl_i2c1
            .port   = hl_gpio_portB,
            .pin    = hl_gpio_pin8   
        },
        {   // hl_i2c2
            .port   = hl_gpio_portNONE,
            .pin    = hl_gpio_pinNONE   
        },
        {   // hl_i2c3
            .port   = hl_gpio_portNONE,
            .pin    = hl_gpio_pinNONE   
        }        
    },
    .gpio_sda           =
    {
        {   // hl_i2c1
            .port   = hl_gpio_portB,
            .pin    = hl_gpio_pin9   
        },
        {   // hl_i2c2
            .port   = hl_gpio_portNONE,
            .pin    = hl_gpio_pinNONE   
        },
        {   // hl_i2c3
            .port   = hl_gpio_portNONE,
            .pin    = hl_gpio_pinNONE   
        }            
    },
    .altf_scl           =
    {
        {   // hl_i2c1
            .f1.gpio_remap  = GPIO_Remap_I2C1 
        },
        {   // hl_i2c2
            .f1.gpio_remap  = hl_NA32 
        },
        {   // hl_i2c3
            .f1.gpio_remap  = hl_NA32 
        }           
    },
    .altf_sda           =
    {
        {   // hl_i2c1
            .f1.gpio_remap  = GPIO_Remap_I2C1 
        },
        {   // hl_i2c2
            .f1.gpio_remap  = hl_NA32 
        },
        {   // hl_i2c3
            .f1.gpio_remap  = hl_NA32 
        }           
    }          
};
#endif

#endif//defined(HL_USE_UTIL_I2C)


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-session




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



