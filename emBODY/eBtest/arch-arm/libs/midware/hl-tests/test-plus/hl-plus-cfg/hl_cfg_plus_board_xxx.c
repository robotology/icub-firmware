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


#if     defined(HL_USE_UTIL_SYS)

#include "hl_sys.h"

#if     defined(HL_USE_BRD_MCBSTM32_C)

extern const hl_sys_speeds_t hl_sys_speeds = 
{
    .cpuspeed   = 72000000,
    .fastbus    = 72000000,
    .slowbus    = 36000000
};

#elif   defined(HL_USE_BRD_MCBSTM32)

extern const hl_sys_speeds_t hl_sys_speeds = 
{
    .cpuspeed   = sss,
    .fastbus    = sss,
    .slowbus    = sss
};

#elif   defined(HL_USE_BRD_EMS001)

extern const hl_sys_speeds_t hl_sys_speeds = 
{
    .cpuspeed   = 72000000,
    .fastbus    = 72000000,
    .slowbus    = 36000000
};

#endif//defined(HL_USE_BRD_EMS001)

#endif//defined(HL_USE_UTIL_SYS)



#if     defined(HL_USE_UTIL_I2C)

#include "hl_i2c.h"

#if     defined(HL_USE_BRD_MCBSTM32_C)

extern const hl_i2c_mapping_t hl_i2c_mapping =
{
    .supported_mask     = (1 << hl_i2c1) | (0 << hl_i2c2) | (0 << hl_i2c3),
    .gpiomap            =
    {   
        {   // hl_i2c1 
            .scl = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portB, 
                    .pin    = hl_gpio_pin8
                },
                .af32       = GPIO_Remap_I2C1          
            }, 
            .sda = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portB, 
                    .pin    = hl_gpio_pin9
                },
                .af32       = GPIO_Remap_I2C1          
            } 
        }, 
        {   // hl_i2c2 
            .scl = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portNONE, 
                    .pin    = hl_gpio_pinNONE
                },
                .af32       = hl_NA32          
            }, 
            .sda = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portNONE, 
                    .pin    = hl_gpio_pinNONE
                },
                .af32       = hl_NA32           
            } 
        },  
        {   // hl_i2c3 
            .scl = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portNONE, 
                    .pin    = hl_gpio_pinNONE
                },
                .af32       = hl_NA32          
            }, 
            .sda = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portNONE, 
                    .pin    = hl_gpio_pinNONE
                },
                .af32       = hl_NA32           
            } 
        }        
    }       
};


#elif   defined(HL_USE_BRD_MCBSTM32)

extern const hl_i2c_mapping_t hl_i2c_mapping =
{
    .supported_mask     = (1 << hl_i2c1) | (0 << hl_i2c2) | (0 << hl_i2c3),
    .gpiomap            =
    {   
        {   // hl_i2c1 
            .scl = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portB, 
                    .pin    = hl_gpio_pin8
                },
                .af32       = GPIO_Remap_I2C1          
            }, 
            .sda = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portB, 
                    .pin    = hl_gpio_pin9
                },
                .af32       = GPIO_Remap_I2C1          
            } 
        }, 
        {   // hl_i2c2 
            .scl = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portNONE, 
                    .pin    = hl_gpio_pinNONE
                },
                .af32       = hl_NA32          
            }, 
            .sda = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portNONE, 
                    .pin    = hl_gpio_pinNONE
                },
                .af32       = hl_NA32           
            } 
        },  
        {   // hl_i2c3 
            .scl = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portNONE, 
                    .pin    = hl_gpio_pinNONE
                },
                .af32       = hl_NA32          
            }, 
            .sda = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portNONE, 
                    .pin    = hl_gpio_pinNONE
                },
                .af32       = hl_NA32           
            } 
        }        
    }       
};

#elif   defined(HL_USE_BRD_EMS001)

extern const hl_i2c_mapping_t hl_i2c_mapping =
{
    .supported_mask     = (1 << hl_i2c1) | (0 << hl_i2c2) | (0 << hl_i2c3),
    .gpiomap            =
    {   
        {   // hl_i2c1 
            .scl = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portB, 
                    .pin    = hl_gpio_pin8
                },
                .af32       = GPIO_Remap_I2C1          
            }, 
            .sda = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portB, 
                    .pin    = hl_gpio_pin9
                },
                .af32       = GPIO_Remap_I2C1          
            } 
        }, 
        {   // hl_i2c2 
            .scl = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portNONE, 
                    .pin    = hl_gpio_pinNONE
                },
                .af32       = hl_NA32          
            }, 
            .sda = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portNONE, 
                    .pin    = hl_gpio_pinNONE
                },
                .af32       = hl_NA32           
            } 
        },  
        {   // hl_i2c3 
            .scl = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portNONE, 
                    .pin    = hl_gpio_pinNONE
                },
                .af32       = hl_NA32          
            }, 
            .sda = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portNONE, 
                    .pin    = hl_gpio_pinNONE
                },
                .af32       = hl_NA32           
            } 
        }        
    }       
};

#endif

#endif//defined(HL_USE_UTIL_I2C)


#if     defined(HL_USE_UTIL_ETH)

#include "hl_eth.h"

#if     defined(HL_USE_BRD_EMS001)

extern const hl_eth_mapping_t hl_eth_mapping =
{
    .supported          = hl_true,
    .gpiomap            =
    {
        .mif            = hl_eth_mif_rmii,
        .gpio_mif.rmii  =
        {   // if ETH_RMII_CRS_DV is on portD <-> use GPIO_Remap_ETH on rx-rmii
            .ETH_RMII_REF_CLK   =            
            {   
                .gpio   = { .port = hl_gpio_portA,     .pin = hl_gpio_pin1 }, 
                .af32   = hl_NA32
            },  
            .ETH_RMII_TX_EN     =            
            {   
                .gpio   = { .port = hl_gpio_portB,     .pin = hl_gpio_pin11 }, 
                .af32   = hl_NA32
            },
            .ETH_RMII_TXD0      =            
            {   
                .gpio   = { .port = hl_gpio_portB,     .pin = hl_gpio_pin12 }, 
                .af32   = hl_NA32
            },
            .ETH_RMII_TXD1      =            
            {   
                .gpio   = { .port = hl_gpio_portB,     .pin = hl_gpio_pin13 }, 
                .af32   = hl_NA32
            },
            .ETH_RMII_CRS_DV    =            
            {   
                .gpio   = { .port = hl_gpio_portD,     .pin = hl_gpio_pin8 }, 
                .af32   = GPIO_Remap_ETH
            },
            .ETH_RMII_RXD0      =            
            {   
                .gpio   = { .port = hl_gpio_portD,     .pin = hl_gpio_pin9 }, 
                .af32   = GPIO_Remap_ETH
            },
            .ETH_RMII_RXD1      =            
            {   
                .gpio   = { .port = hl_gpio_portD,     .pin = hl_gpio_pin10 }, 
                .af32   = GPIO_Remap_ETH
            } 
        },
        .gpio_smi       =
        {
            .ETH_MDC        =            
            {   
                .gpio   = { .port = hl_gpio_portC,     .pin = hl_gpio_pin1 }, 
                .af32   = hl_NA32
            },     
            .ETH_MDIO       =            
            {   
                .gpio   = { .port = hl_gpio_portA,     .pin = hl_gpio_pin2 }, 
                .af32   = hl_NA32
            }     
        }
        
        
    }
    
};

#endif//defined(HL_USE_BRD_EMS001)

#endif//defined(HL_USE_UTIL_ETH)




#if defined(HL_USE_UTIL_ETHTRANS)

#include "hl_ethtrans.h"

#if defined(HL_USE_BRD_EMS001)

extern const hl_ethtrans_mapping_t hl_ethtrans_mapping = 
{
    .supported  = hl_true
};

#endif//defined(HL_USE_BRD_EMS001)

#endif//defined(HL_USE_UTIL_ETHTRANSCEIVER)



#if     defined(HL_USE_UTIL_CAN)

#include "hl_can.h"

#if     defined(HL_USE_BRD_MCBSTM32_C)


#elif   defined(HL_USE_BRD_MCBSTM32)


#elif   defined(HL_USE_BRD_EMS001)

extern const hl_can_mapping_t hl_can_mapping =
{
    .supported_mask     = (1 << hl_can1) | (1 << hl_can2),
    .gpiomap            =
    {   
        {   // hl_can1 
            .rx = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portD, 
                    .pin    = hl_gpio_pin0
                },
                .af32       = GPIO_Remap2_CAN1          
            }, 
            .tx = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portD, 
                    .pin    = hl_gpio_pin1
                },
                .af32       = GPIO_Remap2_CAN1          
            } 
        }, 
        {   // hl_can2 
            .rx = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portB, 
                    .pin    = hl_gpio_pin5
                },
                .af32       = GPIO_Remap_CAN2          
            }, 
            .tx = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portB, 
                    .pin    = hl_gpio_pin6
                },
                .af32       = GPIO_Remap_CAN2          
            } 
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



