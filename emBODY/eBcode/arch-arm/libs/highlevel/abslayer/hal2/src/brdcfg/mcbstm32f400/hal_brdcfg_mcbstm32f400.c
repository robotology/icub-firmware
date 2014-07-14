/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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

/* @file       hal_brdcfg_mcbstm32f400.c
	@brief      This file implements support for board mcbstm32f400
	@author     marco.accame@iit.it
    @date       11/14/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "hal_middleware_interface.h"


#include "hal_base.h"

#include "hl_cfg_plus_target.h"
#include "hl_cfg_plus_modules.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_brdcfg_mcbstm32f400.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

// -- shared by many



#if defined(HAL_USE_ETH) || defined(HAL_USE_ETHTRANSCEIVER)

    #include "hal_ethtransceiver.h"


    #warning --> using chip_xx ethphy 

    #include "hl_chip_xx_ethphy.h"
   

    extern const hl_chip_xx_ethphy_cfg_t ethphymode_config = 
    {
        .chip               = hl_chip_xx_ethphy_chip_autodetect,
        .targetphymode      = hl_ethtrans_phymode_fullduplex100mbps
    };    
    
    static hal_result_t s_ethphy_init(int32_t id, void* param)
    {        
        hl_result_t r = hl_chip_xx_ethphy_init((const hl_chip_xx_ethphy_cfg_t *)param);
        return((hal_result_t)r);
    }  

    static hal_result_t s_ethphy_start(int32_t id, void* p)
    {
        //hal_ethtransceiver_phymode_t* usedphymode = (hal_ethtransceiver_phymode_t*);
        return((hal_result_t)hl_chip_xx_ethphy_start((hl_ethtrans_phymode_t*)p));
    }

    static hal_result_t s_ethphy_getmiiphymode(int32_t id, void* p)
    {
        //hal_ethtransceiver_phymode_t* usedphymode = (hal_ethtransceiver_phymode_t*)p;
        return((hal_result_t)hl_chip_xx_ethphy_getphymode((hl_ethtrans_phymode_t*)p));        
    }  

    static hal_result_t s_ethphy_getlinkupmask(int32_t id, void* p)
    {
        uint8_t *mask = (uint8_t*)p;
        *mask = 1;
        return(hal_res_OK);
        //return(hal_res_NOK_generic);
    }

    static hal_result_t s_ethphy_getphystatus(int32_t id, void* p, uint8_t n)
    {
        hl_ethtrans_phystatus_t *ps = (hl_ethtrans_phystatus_t*)p;
        
        ps->linkisup        = 1;
        ps->autoNeg_done    = 1;
        ps->linkisgood      = 1;
        ps->linkspeed       = 1;
        ps->linkduplex      = 1;
        ps->dummy           = 0;
        return(hal_res_OK);
        //return(hal_res_NOK_generic);
    }

    static hal_result_t s_ethphy_getphyerrorinfo(int32_t id, uint8_t n, uint32_t t, void* p)
    {
        hl_ethtrans_phyerrorinfo_t* e = (hl_ethtrans_phyerrorinfo_t*)p;
        e->value                    = 0;
        e->counteroverflow          = 0;
        e->validvalue               = 1;
        e->dummy                    = 0;
        return(hal_res_OK);
        //return(hal_res_NOK_generic);
    } 
    
    
#endif




// -- core

#ifdef  HAL_USE_BASE

    #include "hal_base_hid.h"
    
    extern const hal_base_boardconfig_t hal_base__theboardconfig  =
    {
        .dummy  = 0
    };  
    
#endif//HAL_USE_BASE  
    
    
#ifdef  HAL_USE_MPU
    
    #include "hal_mpu_hid.h"
    
    extern const hal_mpu_boardconfig_t hal_mpu__theboardconfig  =
    {
        .architecture       = hal_mpu_arch_armcm4,
        .type               = hal_mpu_type_stm32f4,
        .name               = hal_mpu_name_stm32f407ig,
        .speeds             =
        {   // these values must be coherent w/ HL_CFG_MPUSPEED_STM32xxx valued defined in hl_cfg_plus_target.h 
            .max                = 168000000,
            .cpu                = 168000000,
            .fastbus            =  84000000,
            .slowbus            =  42000000
        } 
    }; 
    
#endif//HAL_USE_MPU  

    
#ifdef  HAL_USE_FLASH
    
    #include "hal_flash_hid.h"
    
    extern const hal_flash_boardconfig_t hal_flash__theboardconfig =
    {
        .baseaddress    =   0x08000000,                 // on every stm32
        .totalsize      =   
        #if     defined(HAL_USE_MPU_NAME_STM32F107VC)
                            256*1024
        #elif   defined(HAL_USE_MPU_NAME_STM32F407IG)
                            1024*1024
        #else
                            #error -> must specify a MPU_NAME
        #endif
    };
#endif//HAL_USE_FLASH
 

#ifdef  HAL_USE_HEAP
    
    #include "hal_heap_hid.h"
    
    extern const hal_heap_boardconfig_t hal_heap__theboardconfig =
    {
        .nothingsofar   = 0
    };
    
#endif//HAL_USE_HEAP    
   
    
#ifdef  HAL_USE_SYS
    
    #include "hal_sys_hid.h"
    
    extern const hal_sys_boardconfig_t hal_sys__theboardconfig  =
    {
        .dummy             = 0
    };   
    
#endif//HAL_USE_SYS   
   

// --  extra: mpu peripherals


#ifdef  HAL_USE_ETH
    
    #include "hal_eth_hid.h"
    extern const hal_eth_boardconfig_t hal_eth__theboardconfig =
    {
        .supported          = hal_true,
        .rxdmafifocapacity  = 4, //hal_NA08, //4, //hal_NA08
        .txdmafifocapacity  = 2, //hal_NA08, //2, //hal_NA08
        .gpiomap            =
        {
            .mif            = hal_eth_mif_rmii,
            .gpio_mif.rmii  =
            {   // if ETH_RMII_CRS_DV is on portD <-> use GPIO_Remap_ETH on rx-rmii
                .ETH_RMII_REF_CLK   =            
                {   
                    .gpio   = { .port = hal_gpio_portA,     .pin = hal_gpio_pin1 }, 
                    .af32   = GPIO_AF_ETH
                },  
                .ETH_RMII_TX_EN     =            
                {   
                    .gpio   = { .port = hal_gpio_portG,     .pin = hal_gpio_pin11 }, 
                    .af32   = GPIO_AF_ETH
                },
                .ETH_RMII_TXD0      =            
                {   
                    .gpio   = { .port = hal_gpio_portG,     .pin = hal_gpio_pin13 }, 
                    .af32   = GPIO_AF_ETH
                },
                .ETH_RMII_TXD1      =            
                {   
                    .gpio   = { .port = hal_gpio_portG,     .pin = hal_gpio_pin14 }, 
                    .af32   = GPIO_AF_ETH
                },
                .ETH_RMII_CRS_DV    =            
                {   
                    .gpio   = { .port = hal_gpio_portA,     .pin = hal_gpio_pin7 }, 
                    .af32   = GPIO_AF_ETH
                },
                .ETH_RMII_RXD0      =            
                {   
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin4 }, 
                    .af32   = GPIO_AF_ETH
                },
                .ETH_RMII_RXD1      =            
                {   
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin5 }, 
                    .af32   = GPIO_AF_ETH
                } 
            },
            .gpio_smi       =
            {
                .ETH_MDC        =            
                {   
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin1 }, 
                    .af32   = GPIO_AF_ETH
                },     
                .ETH_MDIO       =            
                {   
                    .gpio   = { .port = hal_gpio_portA,     .pin = hal_gpio_pin2 }, 
                    .af32   = GPIO_AF_ETH
                }     
            }      
        }
    };
    
#endif//HAL_USE_ETH



#ifdef HAL_USE_GPIO
    
    #include "hal_gpio_hid.h"
    
    extern const hal_gpio_boardconfig_t hal_gpio__theboardconfig =
    {
        .supportedmask_byport    =
        {   // ok, i enable every pin of every port.
            hal_gpio_pinALL,     // port a
            hal_gpio_pinALL,     // port b
            hal_gpio_pinALL,     // port c
            hal_gpio_pinALL,     // port d
            hal_gpio_pinALL,     // port e
            hal_gpio_pinALL,     // port f
            hal_gpio_pinALL,     // port g
            hal_gpio_pinALL,     // port h
            hal_gpio_pinALL      // port i
        }
    };
    
#endif//HAL_USE_GPIO


#ifdef  HAL_USE_I2C
    
    #include "hal_i2c_hid.h"
    
    extern const hal_i2c_boardconfig_t hal_i2c__theboardconfig =
    {
        .supportedmask          = (1 << hal_i2c1) | (0 << hal_i2c2) | (0 << hal_i2c3),
        .gpiomap                =
        {
            {   // hal_i2c1 
                .scl    =
                {
                    .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin8 }, 
                    .af32   = GPIO_AF_I2C1
                },
                 .sda   = 
                {    
                    .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin9 }, 
                    .af32   = GPIO_AF_I2C1
                }   
            },   

            {   // hal_i2c2 
                .scl    =
                {
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32
                },
                 .sda   = 
                {    
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32
                }   
            }, 
            
            {   // hal_i2c3 
                .scl    =
                {
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32
                },
                 .sda   = 
                {    
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32
                }   
            }     
        }       
    }; 
    
#endif//HAL_USE_I2C
      
#ifdef  HAL_USE_TIMER
    
    #include "hal_timer_hid.h"
    
    extern const hal_timer_boardconfig_t hal_timer__theboardconfig =
    {
        .supportedmask  = (1 << hal_timer1) | (1 << hal_timer2) | (1 << hal_timer3) | (1 << hal_timer4) | (1 << hal_timer5) | (1 << hal_timer6) | (1 << hal_timer7) | (1 << hal_timer8)
    }; 
    
#endif//HAL_USE_TIMER

    
#ifdef  HAL_USE_TRACE
    
    #include "hal_trace_hid.h"
    
    extern const hal_trace_boardconfig_t hal_trace__theboardconfig =
    {
        .supported          = hal_true
    }; 
    
#endif//HAL_USE_TRACE
  

#ifdef  HAL_USE_UNIQUEID
    
    #include "hal_uniqueid_hid.h"
    
    extern const hal_uniqueid_boardconfig_t hal_uniqueid__theboardconfig =
    {
        .supportedmask      = (1 << hal_uniqueid_id64bit) | (0 << hal_uniqueid_macaddr),
        .macoui             = 0
    };  
    
#endif//HAL_USE_UNIQUEID
    

#ifdef  HAL_USE_WATCHDOG
    
    #include "hal_watchdog_hid.h" 
    
    extern const hal_watchdog_boardconfig_t hal_watchdog__theboardconfig =
    {
        .supportedmask      = (1 << hal_watchdog1_normal) | (1 << hal_watchdog2_window)
    };   
    
#endif//HAL_USE_WATCHDOG


// -- devices




#ifdef  HAL_USE_EEPROM
    
    #include "hal_eeprom_hid.h" 
    #include "hl_chip_xx_eeprom.h"
    
    const hl_chip_xx_eeprom_cfg_t hl_eepromcfg = 
    {
        .chip           = hl_chip_xx_eeprom_chip_st_m24lr64, 
        .i2cid          = hl_i2c1,
        .hwaddra2a1a0   = 0,
        .wp_val         = hl_gpio_valUNDEF,
        .wp_gpio        = { .port = hl_gpio_portNONE, .pin = hl_gpio_pinNONE }           
    };        
    static hal_result_t s_chip_xx_eeprom_init(int32_t id, const hl_chip_xx_eeprom_cfg_t * cfg)
    {
        return((hal_result_t)hl_chip_xx_eeprom_init(cfg));
    }
    static hal_result_t s_chip_xx_eeprom_read(int32_t id, uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* writtenbytes)
    {
        return((hal_result_t)hl_chip_xx_eeprom_read(address, size, buffer, writtenbytes));        
    }   
    static hal_result_t s_chip_xx_eeprom_write(int32_t id, uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* writtenbytes)
    {
        return((hal_result_t)hl_chip_xx_eeprom_write(address, size, buffer, writtenbytes));        
    } 
    


    extern const hal_eeprom_boardconfig_t hal_eeprom__theboardconfig =
    {
        .supportedmask      =  (0 << hal_eeprom_emulatedflash) | (1 << hal_eeprom_i2c_01) | (0 << hal_eeprom_i2c_02),
        .driver             =
        {            
            {   // hal_eeprom1_emulatedflash 
                .cfg.fls    =
                {
                    .baseaddress    = 0x08000000,
                    .totalsize      = 0
                },
                .fun        =
                {   // emulated flash does not use user-defined functions
                    .init           = NULL, 
                    .read           = NULL,
                    .write          = NULL                    
                }
            },
            {   // hal_eeprom2_i2c_01
                .cfg.i2c    =
                {
                    .baseaddress    = 0,
                    .totalsize      = 8*1024,
                    .i2cid          = hal_i2c1,
                    .initpar        = (void*)&hl_eepromcfg
                },
                .fun        =
                {   // use the chip xx_eeprom
                    .init           = (hal_res_fp_int32_voidp_t)s_chip_xx_eeprom_init,
                    .read           = (hal_res_fp_int32_uint32_uint32_voidp_voidp_t)s_chip_xx_eeprom_read, 
                    .write          = (hal_res_fp_int32_uint32_uint32_voidp_voidp_t)s_chip_xx_eeprom_write,                             
                }                              
            },
            {   // hal_eeprom3_i2c_02
                .cfg.i2c    =
                {
                    .baseaddress    = 0,
                    .totalsize      = 0,
                    .i2cid          = hal_i2c1,
                    .initpar        = NULL
                },
                .fun        =
                {   // use ... nothing
                    .init           = NULL,
                    .read           = NULL, 
                    .write          = NULL,                             
                }
            }                
        }
    
    };    
#endif//HAL_USE_EEPROM 
  


#ifdef  HAL_USE_ETHTRANSCEIVER
    
    #include "hal_ethtransceiver_hid.h" 
             
    extern const hal_ethtransceiver_boardconfig_t hal_ethtransceiver__theboardconfig =
    {
        .supportedmask          = (1 << hal_ethtransceiver1),
        .driver                 =
        {
            {   // hal_ethtransceiver1
                .cfg    =
                {
                    .numofphysicals     = 1,
                    .targetphymode      = hal_ethtransceiver_phymode_auto,  
                    .initpar            = (void*)&ethphymode_config,                    
                },
                .fun    =
                {
                    .init           = s_ethphy_init,
                    .start          = s_ethphy_start, 
                    .getmiiphymode  = s_ethphy_getmiiphymode,                
                    .phylinkupmask  = s_ethphy_getlinkupmask,
                    .phystatus      = s_ethphy_getphystatus,
                    .phyerrorinfo   = s_ethphy_getphyerrorinfo                    
                }  
            } 
        }
    };
    
 
#endif//HAL_USE_ETHTRANSCEIVER




#ifdef HAL_USE_LED
    
    #include "hal_led_hid.h"
    
    extern const hal_led_boardconfig_t hal_led__theboardconfig =
    {
        .supportedmask      = (1 << hal_led0) | (1 << hal_led1) | (1 << hal_led2) | (1 << hal_led3) | (1 << hal_led4) | (1 << hal_led5), // = 0x3F: only first 6 leds ...
        .boardcommon        =
        {
            .value_on       = hal_gpio_valHIGH,
            .value_off      = hal_gpio_valLOW
        },    
        .gpiomap            =
        {
            {   // hal_led0 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portH,     .pin = hal_gpio_pin2 }, 
                    .af32   = hal_NA32
                }
            },
            {   // hal_led1 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portG,     .pin = hal_gpio_pin8}, 
                    .af32   = hal_NA32
                }
            },            
            {   // hal_led2 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portG,     .pin = hal_gpio_pin7}, 
                    .af32   = hal_NA32
                }
            },            
            {   // hal_led3 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portG,     .pin = hal_gpio_pin6}, 
                    .af32   = hal_NA32
                }
            },            
            {   // hal_led4 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portI,     .pin = hal_gpio_pin10}, 
                    .af32   = hal_NA32
                }
            },            
            {   // hal_led5 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portH,     .pin = hal_gpio_pin7}, 
                    .af32   = hal_NA32
                }
            },                      
            {   // hal_led6 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portH,  .pin = hal_gpio_pin6}, 
                    .af32   = hal_NA32
                }
            },              
            {   // hal_led7 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portH,  .pin = hal_gpio_pin3}, 
                    .af32   = hal_NA32
                }
            }  
        }
    };
    
#endif//HAL_USE_LED
    
    
    


// -- chips
// none

// -- extbrds
// none
    


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_brdcfg__start(void)
{
    return(hal_res_OK);    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


