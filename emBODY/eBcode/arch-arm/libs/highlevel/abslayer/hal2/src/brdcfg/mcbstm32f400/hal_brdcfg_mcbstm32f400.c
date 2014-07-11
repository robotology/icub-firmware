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

#if     0
// qui ci deve stare qualosa del transceiver 

    #warning --> using micrel as automatic un-mananaged device

    // the micrel is hw configured in autonegotiation for its port1 and port2. 
    // the mode for the rmii cannot be read from the micrel, thus we assume it is fd100
    #define HAL_ETHTRANSCEIVER_PHYMODE_THEONE2USE       hal_ethtransceiver_phymode_auto
    #define HAL_ETH_PHYMODE_THEONEUSED                  hal_ethtransceiver_phymode_fullduplex100mbps

    static hal_result_t s_switch_dummy_init(int32_t id, void* param)
    {
        // the HAL_ETHTRANSCEIVER_PHYMODE_THEONE2USE mode is done in HW
        return(hal_res_OK);
    }   
    static hal_result_t s_switch_dummy_start(int32_t id, void* p)
    {
        hal_ethtransceiver_phymode_t* usedphymode = (hal_ethtransceiver_phymode_t*)p;
        // the mode of the rmii is surely this one
        *usedphymode = HAL_ETH_PHYMODE_THEONEUSED;
        return(hal_res_OK);
    }   
    static hal_result_t s_switch_dummy_getmiiphymode(int32_t id, void* p)
    {
        hal_ethtransceiver_phymode_t* usedphymode = (hal_ethtransceiver_phymode_t*)p;
        // the mode of the rmii is surely this one
        *usedphymode = HAL_ETH_PHYMODE_THEONEUSED;
        return(hal_res_OK);
    }  
    static hal_result_t s_switch_dummy_getlinkupmask(int32_t id, void* p)
    {
        //uint8_t *mask = (uint8_t*)p;
        return(hal_res_NOK_generic);
    }
    static hal_result_t s_switch_dummy_getphystatus(int32_t id, void* p, uint8_t n)
    {
        return(hal_res_NOK_generic);
    }

    static hal_result_t s_switch_dummy_getphyerrorinfo(int32_t id, uint8_t n, uint32_t t, void* p)
    {
        return(hal_res_NOK_generic);
    } 
    
#elif 0
    

// lo tengo perche' forse mi serve per il hl_chip_xx_ethphy

    #warning --> using micrel as managed device w/ EMS4RD_USE_MICREL_AS_MANAGED_DEVICE

    #include "hl_chip_micrel_ks8893.h"
    #include "hal_i2c.h"
    
    // the micrel must be sw configured. we set it in autonegotiation for its port1 and port2. 
    // the mode for the rmii is read via sw from the micrel
    // for debug you can also use: hl_ethtrans_phymode_fullduplex10mbps // hl_ethtrans_phymode_halfduplex10mbps //hl_ethtrans_phymode_auto
    #define HL_ETHTRANS_PHYMODE_THEONE2USE              hl_ethtrans_phymode_auto
    
    // we also need to define a target phy mode for the hal-ethtransceiver, but that is dummy. i use auto
    #define HAL_ETHTRANSCEIVER_PHYMODE_THEONE2USE       hal_ethtransceiver_phymode_auto
    
    static hl_result_t s_switch_micrel_extclock_init(void)
    {   // the external clock is not managed by a mpu peripheral, thus i do nothing
        return(hl_res_OK);    
    }    

    extern const hl_chip_micrel_ks8893_cfg_t ks8893_config = 
    {
        .i2cid              = hl_i2c3,
        .resetpin           = { .port = hl_gpio_portB,     .pin = hl_gpio_pin2 },
        .resetval           = hl_gpio_valRESET,
        .extclockinit       = s_switch_micrel_extclock_init,
        .targetphymode      = HL_ETHTRANS_PHYMODE_THEONE2USE
    };    
    
    static hal_result_t s_switch_micrel_init(int32_t id, void* param)
    {
        // i2c3 must be initted.
        hal_i2c_init((hal_i2c_t)ks8893_config.i2cid, NULL);        
        hl_result_t r = hl_chip_micrel_ks8893_init((const hl_chip_micrel_ks8893_cfg_t*)param);
        return((hal_result_t)r);
    }  

    static hal_result_t s_switch_micrel_start(int32_t id, void* p)
    {
        //hal_ethtransceiver_phymode_t* usedphymode = (hal_ethtransceiver_phymode_t*);
        return((hal_result_t)hl_chip_micrel_ks8893_start((hl_ethtrans_phymode_t*)p));
    }

    static hal_result_t s_switch_micrel_getmiiphymode(int32_t id, void* p)
    {
        //hal_ethtransceiver_phymode_t* usedphymode = (hal_ethtransceiver_phymode_t*)p;
        return((hal_result_t)hl_chip_micrel_ks8893_mii_getphymode((hl_ethtrans_phymode_t*)p));        
    }  

    static hal_result_t s_switch_micrel_getlinkupmask(int32_t id, void* p)
    {
        //uint8_t *mask = (uint8_t*)p;
        return((hal_result_t)hl_chip_micrel_ks8893_linkupmask((uint8_t*)p));
    }

    static hal_result_t s_switch_micrel_getphystatus(int32_t id, void* p, uint8_t n)
    {
        return((hal_result_t)hl_chip_micrel_ks8893_phy_status((hl_ethtrans_phystatus_t*)p, (uint8_t)n));
    }

    static hal_result_t s_switch_micrel_getphyerrorinfo(int32_t id, uint8_t n, uint32_t t, void* p)
    {
        return((hal_result_t)hl_chip_micrel_ks8893_phy_errorinfo((uint8_t)n, (hl_ethtrans_phyerror_t)t, (hl_ethtrans_phyerrorinfo_t*)p));
    } 
    
    
#endif

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





  


#ifdef  HAL_USE_ETHTRANSCEIVER
    
    #include "hal_ethtransceiver_hid.h" 
 
    #warning --> to be done   
    
#if     defined(EMS4RD_USE_MICREL_AS_MANAGED_DEVICE)      
            
    extern const hal_ethtransceiver_boardconfig_t hal_ethtransceiver__theboardconfig =
    {
        .supportedmask          = (1 << hal_ethtransceiver1),
        .driver                 =
        {
            {   // hal_ethtransceiver1
                .cfg    =
                {
                    .numofphysicals     = 2,
                    .targetphymode      = HAL_ETHTRANSCEIVER_PHYMODE_THEONE2USE,  
                    .initpar            = (void*)&ks8893_config,                    
                },
                .fun    =
                {
                    .init           = s_switch_micrel_init,
                    .start          = s_switch_micrel_start, 
                    .getmiiphymode  = s_switch_micrel_getmiiphymode,                
                    .phylinkupmask  = s_switch_micrel_getlinkupmask,
                    .phystatus      = s_switch_micrel_getphystatus,
                    .phyerrorinfo   = s_switch_micrel_getphyerrorinfo                    
                }  
            } 
        }
    };
    

#elif 0

    static hal_result_t s_switch_dummy_linkupmask(void* p)
    {
        return(hal_res_NOK_generic);        
    }
    static hal_result_t s_switch_dummy_phy_status(void* p, uint8_t num)
    {
        return(hal_res_NOK_generic);        
    }  
    static hal_result_t s_switch_dummy_phy_errorinfo(uint8_t num, uint32_t e, void* p)
    {
        return(hal_res_NOK_generic);        
    } 


    extern const hal_ethtransceiver_boardconfig_t hal_ethtransceiver__theboardconfig =
    {
        .supportedmask          = (1 << hal_ethtransceiver1),
        .driver                 =
        {
            {   // hal_ethtransceiver1
                .cfg    =
                {
                    .numofphysicals     = 2,
                    .targetphymode      = HAL_ETHTRANSCEIVER_PHYMODE_THEONE2USE,  
                    .initpar            = NULL,                    
                },
                .fun    =
                {
                    .init           = s_switch_dummy_init,
                    .start          = s_switch_dummy_start, 
                    .getmiiphymode  = s_switch_dummy_getmiiphymode,                
                    .phylinkupmask  = s_switch_dummy_getlinkupmask,
                    .phystatus      = s_switch_dummy_getphystatus,
                    .phyerrorinfo   = s_switch_dummy_getphyerrorinfo                    
                }  
            } 
        }
    };
        
#endif
    
#endif//HAL_USE_ETHTRANSCEIVER




#ifdef HAL_USE_LED
    
    #include "hal_led_hid.h"
    
    extern const hal_led_boardconfig_t hal_led__theboardconfig =
    {
        .supportedmask      = (1 << hal_led0) | (1 << hal_led1) | (1 << hal_led2) | (1 << hal_led3) | (1 << hal_led4) | (1 << hal_led5), // = 0x3F: only first 6 leds ...
        .boardcommon        =
        {
            .value_on       = hal_gpio_valLOW,
            .value_off      = hal_gpio_valHIGH
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


