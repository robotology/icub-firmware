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


/* @file       hl_cfg_plus_board_stm32f4.c
	@brief      This file contains configuration variables for the hl utilities which are specific for a given board.
	@author     marco.accame@iit.it
    @date       10/21/2013
**/


// - modules to be built: contains the HL_USE_* macros ----------------------------------------------------------------

#include "hl_cfg_plus_modules.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "hl_common.h"
#include "hl_core.h"        // contains the required stm32f10x_*.h or stm32f4xx*.h header files
#include "hl_arch.h"
 
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


#if     defined(HL_USE_BRD_MCBSTM32_F400)


#elif   defined(HL_USE_BRD_EMS004)



#endif//defined(HL_USE_BRD_EMS004)

#endif//defined(HL_USE_UTIL_SYS)



#if     defined(HL_USE_UTIL_I2C)

#include "hl_i2c.h"

#if     defined(HL_USE_BRD_MCBSTM32_F400)

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
                .af32       = GPIO_AF_I2C1          
            }, 
            .sda = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portB, 
                    .pin    = hl_gpio_pin9
                },
                .af32       = GPIO_AF_I2C1          
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


#elif   defined(HL_USE_BRD_EMS004)

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
                .af32       = GPIO_AF_I2C1          
            }, 
            .sda = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portB, 
                    .pin    = hl_gpio_pin9
                },
                .af32       = GPIO_AF_I2C1          
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


#if     defined(HL_USE_BRD_MCBSTM32_F400)

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
                .af32   = GPIO_AF_ETH
            },  
            .ETH_RMII_TX_EN     =            
            {   
                .gpio   = { .port = hl_gpio_portG,     .pin = hl_gpio_pin11 }, 
                .af32   = GPIO_AF_ETH
            },
            .ETH_RMII_TXD0      =            
            {   
                .gpio   = { .port = hl_gpio_portG,     .pin = hl_gpio_pin13 }, 
                .af32   = GPIO_AF_ETH
            },
            .ETH_RMII_TXD1      =            
            {   
                .gpio   = { .port = hl_gpio_portG,     .pin = hl_gpio_pin14 }, 
                .af32   = GPIO_AF_ETH
            },
            .ETH_RMII_CRS_DV    =            
            {   
                .gpio   = { .port = hl_gpio_portA,     .pin = hl_gpio_pin7 }, 
                .af32   = GPIO_AF_ETH
            },
            .ETH_RMII_RXD0      =            
            {   
                .gpio   = { .port = hl_gpio_portC,     .pin = hl_gpio_pin4 }, 
                .af32   = GPIO_AF_ETH
            },
            .ETH_RMII_RXD1      =            
            {   
                .gpio   = { .port = hl_gpio_portC,     .pin = hl_gpio_pin5 }, 
                .af32   = GPIO_AF_ETH
            } 
        },
        .gpio_smi       =
        {
            .ETH_MDC        =            
            {   
                .gpio   = { .port = hl_gpio_portC,     .pin = hl_gpio_pin1 }, 
                .af32   = GPIO_AF_ETH
            },     
            .ETH_MDIO       =            
            {   
                .gpio   = { .port = hl_gpio_portA,     .pin = hl_gpio_pin2 }, 
                .af32   = GPIO_AF_ETH
            }     
        }      
    }
    
};

#elif   defined(HL_USE_BRD_EMS004)

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
                .af32   = GPIO_AF_ETH
            },  
            .ETH_RMII_TX_EN     =            
            {   
                .gpio   = { .port = hl_gpio_portB,     .pin = hl_gpio_pin11 }, 
                .af32   = GPIO_AF_ETH
            },
            .ETH_RMII_TXD0      =            
            {   
                .gpio   = { .port = hl_gpio_portB,     .pin = hl_gpio_pin12 }, 
                .af32   = GPIO_AF_ETH
            },
            .ETH_RMII_TXD1      =            
            {   
                .gpio   = { .port = hl_gpio_portB,     .pin = hl_gpio_pin13 }, 
                .af32   = GPIO_AF_ETH
            },
            .ETH_RMII_CRS_DV    =            
            {   
                .gpio   = { .port = hl_gpio_portA,     .pin = hl_gpio_pin7 }, 
                .af32   = GPIO_AF_ETH
            },
            .ETH_RMII_RXD0      =            
            {   
                .gpio   = { .port = hl_gpio_portC,     .pin = hl_gpio_pin4 }, 
                .af32   = GPIO_AF_ETH
            },
            .ETH_RMII_RXD1      =            
            {   
                .gpio   = { .port = hl_gpio_portC,     .pin = hl_gpio_pin5 }, 
                .af32   = GPIO_AF_ETH
            } 
        },
        .gpio_smi       =
        {
            .ETH_MDC        =            
            {   
                .gpio   = { .port = hl_gpio_portC,     .pin = hl_gpio_pin1 }, 
                .af32   = GPIO_AF_ETH
            },     
            .ETH_MDIO       =            
            {   
                .gpio   = { .port = hl_gpio_portA,     .pin = hl_gpio_pin2 }, 
                .af32   = GPIO_AF_ETH
            }     
        }      
    }
    
};

#endif//defined(HL_USE_BRD_EMS004)

#endif//defined(HL_USE_UTIL_ETH)




#if defined(HL_USE_UTIL_ETHTRANS)

#include "hl_ethtrans.h"


#if defined(HL_USE_BRD_MCBSTM32_F400)

#include "hl_chip_xx_ethphy.h"

extern const hl_ethtrans_mapping_t hl_ethtrans_mapping = 
{
    .supported  = hl_true
};

// must redefine the ethtrans functions to use ... HL_USE_CHIP_XX_ETHPHY
extern hl_result_t hl_ethtrans_chip_init(const hl_ethtrans_cfg_t *cfg)
{  
    return(hl_chip_xx_ethphy_init(NULL));
}


extern hl_result_t hl_ethtrans_chip_config(hl_ethtrans_phymode_t targetphymode, hl_ethtrans_phymode_t *usedphymode)
{
    return(hl_chip_xx_ethphy_configure(targetphymode, usedphymode));
}

#elif defined(HL_USE_BRD_EMS004)

#include "hl_chip_micrel_ks8893.h"


extern const hl_ethtrans_mapping_t hl_ethtrans_mapping = 
{
    .supported  = hl_true
};


static void s_hal_brdcfg_device_switch__mco2_init(void)
{
    // --- mco2: PC9
    
    #define SLR100MHZ               0x000C0000
    #define SLR050MHZ               0x00080000
    // configure pc9 af, push pull, 50mhz
    // enable system configuration controller clock
    RCC->APB2ENR    |= (1 << 14);  
    // clocks port port c
    RCC->AHB1ENR    |= 0x00000004;

    GPIOC->MODER    &= ~0x000C0000;              // reset pc9
    GPIOC->MODER    |=  0x00080000;              // alternate function
    GPIOC->OTYPER   &= ~0x00000200;              // output push-pull (reset state)  
    GPIOC->OSPEEDR  |=  SLR100MHZ;               // slew rate as 100MHz pin (0x000C0000) or 50mhz (0x00080000)
    GPIOC->PUPDR    &= ~0x000C0000;              // no pull up, pull down

    GPIOC->AFR[1]   &= ~0x000000F0;
    GPIOC->AFR[1]   |=  0x00000000;              // AF0 (system) 
}



static void s_hal_brdcfg_device_switch__mco_initialise(void)
{    
#define HAL_SWITCH_USE_HSE  
#undef  HAL_SWITCH_USE_PLLI2S    
    
    s_hal_brdcfg_device_switch__mco2_init();

#if     defined(HAL_SWITCH_USE_PLLI2S)    
    RCC_PLLI2SCmd(DISABLE);
    RCC_PLLI2SConfig(200, 2); // 50mhz: 1mhz*200/2 = 100.  then we divide by 2 again
    RCC_PLLI2SCmd(ENABLE);
    // wait until it is ready
    while(RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == RESET);
    // connect mco2 with plli2s divided by 2
    RCC_MCO2Config(RCC_MCO2Source_PLLI2SCLK, RCC_MCO2Div_2);
#elif   defined(HAL_SWITCH_USE_HSE)
 
    // connect mco2 with hse divided by 1
    RCC_MCO2Config(RCC_MCO2Source_HSE, RCC_MCO2Div_1);
#endif    
    
    
    //hal_eth_hid_rmii_refclock_init();
}


static hl_result_t s_hal_brdcfg_device_switch__extclock_init(void)
{
    s_hal_brdcfg_device_switch__mco_initialise();  
    return(hl_res_OK);    
}


extern const hl_chip_micrel_ks8893_cfg_t ems004_micrel_ks8893_config = 
{
    .i2cid              = hl_i2c1,
    .resetpin           = { .port = hl_gpio_portB,     .pin = hl_gpio_pin2 },
    .resetval           = hl_gpio_valRESET,
    .extclockinit       = s_hal_brdcfg_device_switch__extclock_init    
};


// must redefine the ethtrans functions to use ... HL_USE_CHIP_XX_ETHPHY
extern hl_result_t hl_ethtrans_chip_init(const hl_ethtrans_cfg_t *cfg)
{  
    return(hl_chip_micrel_ks8893_init(&ems004_micrel_ks8893_config));
}


extern hl_result_t hl_ethtrans_chip_config(hl_ethtrans_phymode_t targetphymode, hl_ethtrans_phymode_t *usedphymode)
{
    return(hl_chip_micrel_ks8893_configure(targetphymode, usedphymode));
}

#endif//defined(HL_USE_BRD_EMS004)

#endif//defined(HL_USE_UTIL_ETHTRANSCEIVER)



#if     defined(HL_USE_UTIL_CAN)

#include "hl_can.h"

#if     defined(HL_USE_BRD_MCBSTM32_F400)


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
                    .port   = hl_gpio_portI, 
                    .pin    = hl_gpio_pin9
                },
                .af32       = GPIO_AF_CAN1          
            }, 
            .tx = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portB, 
                    .pin    = hl_gpio_pin5
                },
                .af32       = GPIO_AF_CAN1          
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
                .af32       = GPIO_AF_CAN2          
            }, 
            .tx = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portB, 
                    .pin    = hl_gpio_pin3
                },
                .af32       = GPIO_AF_CAN2          
            } 
        }   
    } 
};

#elif   defined(HL_USE_BRD_EMS004)


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
                .af32       = GPIO_AF_CAN1          
            }, 
            .tx = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portD, 
                    .pin    = hl_gpio_pin1
                },
                .af32       = GPIO_AF_CAN1          
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
                .af32       = GPIO_AF_CAN2          
            }, 
            .tx = 
            {
                .gpio   =
                {
                    .port   = hl_gpio_portB, 
                    .pin    = hl_gpio_pin6
                },
                .af32       = GPIO_AF_CAN2          
            } 
        }   
    } 
};

#endif

#endif//defined(HL_USE_UTIL_CAN)


#if     defined(HL_USE_UTIL_TIMER)

#include "hl_timer.h"

#if     defined(HL_USE_BRD_MCBSTM32_F400)


extern const hl_timer_mapping_t hl_timer_mapping =
{
    .supported_mask     = (1 << hl_timer1) | (1 << hl_timer2) | (1 << hl_timer3) | (1 << hl_timer4) | 
                          (1 << hl_timer5) | (1 << hl_timer6) | (1 << hl_timer7) | (1 << hl_timer8)
};

#elif   defined(HL_USE_BRD_EMS004)

extern const hl_timer_mapping_t hl_timer_mapping =
{
    .supported_mask     = (1 << hl_timer1) | (1 << hl_timer2) | (1 << hl_timer3) | (1 << hl_timer4) | 
                          (1 << hl_timer5) | (1 << hl_timer6) | (1 << hl_timer7) | (1 << hl_timer8),
};

#endif

#endif//defined(HL_USE_UTIL_TIMER)



    

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



