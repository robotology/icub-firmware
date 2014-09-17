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

/* @file       hal_brdcfg_ems4rd.c
	@brief      This file implements support for board ems4rd
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

#include "hal_brdcfg_ems4rd.h"


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



#if defined(HAL_USE_ETH) || defined(HAL_USE_ETHTRANSCEIVER) || defined(HAL_USE_SWITCH)

    #include "hal_ethtransceiver.h"

#if     !defined(EMS4RD_USE_MICREL_AS_MANAGED_DEVICE) 

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
    
#else
    
    #warning --> using micrel as managed device w/ EMS4RD_USE_MICREL_AS_MANAGED_DEVICE

    #include "hl_chip_micrel_ks8893.h"
    #include "hal_i2c.h"
    
    // the micrel must be sw configured. we set it in autonegotiation for its port1 and port2. 
    // the mode for the rmii is read via sw from the micrel
    // for debug you can also use: hl_ethtrans_phymode_fullduplex10mbps // hl_ethtrans_phymode_halfduplex10mbps //hl_ethtrans_phymode_auto
    //#define HL_ETHTRANS_PHYMODE_THEONE2USE              hl_ethtrans_phymode_auto
    #define HL_ETHTRANS_PHYMODE_THEONE2USE              hl_ethtrans_phymode_fullduplex100mbps
    #define HL_ETHTRANS_XCOMODE_THEONE2USE              hl_ethtrans_xcorr_none_port2inverted
    
    // we also need to define a target phy mode for the hal-ethtransceiver, but that is dummy. i use auto
    //#define HAL_ETHTRANSCEIVER_PHYMODE_THEONE2USE       hal_ethtransceiver_phymode_auto
    #define HAL_ETHTRANSCEIVER_PHYMODE_THEONE2USE       hal_ethtransceiver_phymode_fullduplex100mbps
    
    #define HL_CHIP_MICREL_RESET_TIME_USEC              (110*1000)
    
    
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
        .targetphymode      = HL_ETHTRANS_PHYMODE_THEONE2USE,
        .xcorrection        = HL_ETHTRANS_XCOMODE_THEONE2USE
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


#if     defined(HAL_USE_GYROSCOPE) | defined(HAL_USE_TERMOMETER)

#if     defined(HL_USE_CHIP_ST_L3G4200D)
    
    #include "hl_chip_st_l3g4200d.h"
    
    const hl_chip_st_l3g4200d_cfg_t l3g4200dcfg =
    {
        .i2cid      = hl_i2c1,
        .range      = hl_chip_st_l3g4200d_range_250dps       
    };
    
#endif//defined(HL_USE_CHIP_ST_L3G4200D)
    
#endif//defined(HAL_USE_GYROSCOPE)|defined(HAL_USE_TERMOMETER)    
    
    
// - begin: used by HAL_USE_CANTRANSCEIVER and in hal_brdcfg__start()
    

static void s_hal_brdcfg_ems4rd_vaux_5v0_init(void);
static void s_hal_brdcfg_ems4rd_vaux_5v0_on(void);
static void s_hal_brdcfg_ems4rd_vaux_5v0_off(void);

    

// - end:   used by HAL_USE_CANTRANSCEIVER and in hal_brdcfg__start()


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

#ifdef  HAL_USE_CAN
    
    #include "hal_can_hid.h"
    
    extern const hal_can_boardconfig_t hal_can__theboardconfig =
    {
        .supportedmask      = (1 << hal_can1) | (1 << hal_can2),
        .gpiomap            =
        {   
            {   // hal_can1 
                .rx = 
                {
                    .gpio   = { .port = hal_gpio_portD,     .pin = hal_gpio_pin0 },
                    .af32   = GPIO_AF_CAN1          
                }, 
                .tx = 
                {
                    .gpio   = { .port = hal_gpio_portD,     .pin = hal_gpio_pin1 },
                    .af32       = GPIO_AF_CAN1          
                } 
            }, 
            {   // hal_can2 
                .rx = 
                {
                    .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin5 },
                    .af32   = GPIO_AF_CAN2          
                }, 
                .tx = 
                {
                    .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin6 },
                    .af32   = GPIO_AF_CAN2          
                } 
            }   
        } 
    };
    
#endif//HAL_USE_CAN


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
                    .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin11 }, 
                    .af32   = GPIO_AF_ETH
                },
                .ETH_RMII_TXD0      =            
                {   
                    .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin12 }, 
                    .af32   = GPIO_AF_ETH
                },
                .ETH_RMII_TXD1      =            
                {   
                    .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin13 }, 
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
        .supportedmask          = (1 << hal_i2c1) | (0 << hal_i2c2) | (1 << hal_i2c3),
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
                },   
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
                },   
            }, 
            
            {   // hal_i2c3 
                .scl    =
                {
                    .gpio   = { .port = hal_gpio_portA,     .pin = hal_gpio_pin8 }, 
                    .af32   = GPIO_AF_I2C3 
                },
                 .sda   = 
                {    
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin9 }, 
                    .af32   = GPIO_AF_I2C3 
                },   
            }     
        }       
    }; 
    
#endif//HAL_USE_I2C


#if     defined(HAL_USE_SPI) | defined(HAL_USE_SPI_DMA)
    
    #include "hal_spi_hid.h"

    extern const hal_spi_boardconfig_t hal_spi__theboardconfig =
    {
        .supportedmask          = (0 << hal_spi1) | (1 << hal_spi2) | (1 << hal_spi3),
        .gpiomap                =
        {   
            {   // hal_spi1
                .sck    =
                {
                    .gpio   = { .port = hal_gpio_portNONE,     .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32                     
                },
                .miso   =
                {
                    .gpio   = { .port = hal_gpio_portNONE,     .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32                     
                },
                .mosi   =
                {
                    .gpio   = { .port = hal_gpio_portNONE,     .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32                    
                } 
            },
            {   // hal_spi2
                .sck    =
                {
                    .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin10 }, 
                    .af32   = GPIO_AF_SPI2                     
                },
                .miso   =
                {
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin2 }, 
                    .af32   = GPIO_AF_SPI2                     
                },
                .mosi   =
                {
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin3 }, 
                    .af32   = GPIO_AF_SPI2                     
                } 
            },
            {   // hal_spi3
                .sck    =
                {
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin10 }, 
                    .af32   = GPIO_AF_SPI3                     
                },
                .miso   =
                {
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin11 }, 
                    .af32   = GPIO_AF_SPI3                     
                },
                .mosi   =
                {
                    .gpio   = { .port = hal_gpio_portC,     .pin = hal_gpio_pin12 }, 
                    .af32   = GPIO_AF_SPI3                     
                } 
            }                   
        }
           
    };
    
#endif//defined(HAL_USE_SPI)|defined(HAL_USE_SPI_DMA)
    
      
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

#ifdef  HAL_USE_ACCELEROMETER   
    #include "hal_accelerometer_hid.h"
    #include "hl_chip_st_lis3x.h"
    
    static const hl_chip_st_lis3x_cfg_t lis3xcfg =
    {
        .chip       = hl_chip_st_lis3x_chip_autodetect,
        .i2cid      = hl_i2c1,
        .range      = hl_chip_st_lis3x_range_2g
    };
    
    static hal_result_t s_acce_init(int32_t id, void* p)
    {
        return((hal_result_t)hl_chip_st_lis3x_init((const hl_chip_st_lis3x_cfg_t*)p));
    }

    static hal_result_t s_acce_read(int32_t id, void* x, void* y, void* z)
    {
        return((hal_result_t)hl_chip_st_lis3x_accel_get((int32_t*)x, (int32_t*)y, (int32_t*)z));
    }      
    
    extern const hal_accelerometer_boardconfig_t hal_accelerometer__theboardconfig =
    {
        .supportedmask      = (1 << hal_accelerometer1),
        .driver             =
        {
            {   // hal_accelerometer1: use the lis3x chip by st
                .cfg        = 
                {
                    .initpar    = (void*)&lis3xcfg,
                },
                .fun            =      
                {   
                    .init       = s_acce_init,
                    .read       = s_acce_read  
                }                    
            }
         }
    };   
    
#endif//HAL_USE_ACCELEROMETER


    
#ifdef  HAL_USE_CANTRANSCEIVER
    
    #include "hal_cantransceiver_hid.h" 
    
    static hal_result_t s_hal_cantransceiver_simple_init(hal_cantransceiver_t id, void* initpar);
    static hal_result_t s_hal_cantransceiver_simple_enable(hal_cantransceiver_t id);
    static hal_result_t s_hal_cantransceiver_simple_disable(hal_cantransceiver_t id);
    
    static const hal_gpio_map_t s_hal_cantransceiver__gpiomapof_enable[hal_cantransceivers_number] = 
    {
        {   // hal_cantransceiver1 
            .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin14 }, 
            .af32   = hal_NA32
        },        
        {   // hal_cantransceiver2 
            .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin15 }, 
            .af32   = hal_NA32
        }        
    };
    
    static const hal_gpio_cfg_t s_hal_cantransceiver__gpiocfgof_enable[hal_cantransceivers_number] =
    {
        { .dir = hal_gpio_dirOUT,     .speed = hal_gpio_speed_low,        .altcfg = NULL },
        { .dir = hal_gpio_dirOUT,     .speed = hal_gpio_speed_low,        .altcfg = NULL }
    };
       
    static hal_result_t s_hal_cantransceiver_simple_init(hal_cantransceiver_t id, void* initpar)
    {
        initpar = initpar;
        const hal_gpio_map_t* map = &s_hal_cantransceiver__gpiomapof_enable[(uint8_t)id];        
        const hal_gpio_cfg_t* cfg = &s_hal_cantransceiver__gpiocfgof_enable[(uint8_t)id];   
        hal_gpio_init(map->gpio, cfg);        
        s_hal_cantransceiver_simple_disable(id);     
        s_hal_brdcfg_ems4rd_vaux_5v0_init();
        s_hal_brdcfg_ems4rd_vaux_5v0_on();
        return(hal_res_OK);
    }

    static hal_result_t s_hal_cantransceiver_simple_enable(hal_cantransceiver_t id)
    {  
        const hal_gpio_map_t* map = &s_hal_cantransceiver__gpiomapof_enable[(uint8_t)id];          
        hal_gpio_setval(map->gpio, hal_gpio_valLOW);
        return(hal_res_OK);
    }

    static hal_result_t s_hal_cantransceiver_simple_disable(hal_cantransceiver_t id)
    {   
        const hal_gpio_map_t* map = &s_hal_cantransceiver__gpiomapof_enable[(uint8_t)id];         
        hal_gpio_setval(map->gpio, hal_gpio_valHIGH);
        return(hal_res_OK);
    }
    
    extern const hal_cantransceiver_boardconfig_t hal_cantransceiver__theboardconfig =
    {
        .supportedmask      =  (1 << hal_cantransceiver1) | (1 << hal_cantransceiver2),
        .driver             =
        {
            {   // hal_cantransceiver1            
                .cfg            =
                {
                    .initpar    = NULL
                },
                .fun            =
                {
                    .init               = (hal_res_fp_int32_voidp_t)s_hal_cantransceiver_simple_init,
                    .enable             = (hal_res_fp_int32_t)s_hal_cantransceiver_simple_enable,
                    .disable            = (hal_res_fp_int32_t)s_hal_cantransceiver_simple_disable
                }
            },
            {   // hal_cantransceiver2            
                .cfg            =
                {
                    .initpar    = NULL
                },
                .fun            =
                {
                    .init               = (hal_res_fp_int32_voidp_t)s_hal_cantransceiver_simple_init,
                    .enable             = (hal_res_fp_int32_t)s_hal_cantransceiver_simple_enable,
                    .disable            = (hal_res_fp_int32_t)s_hal_cantransceiver_simple_disable
                }
            }             
        }        
    }; 
    
#endif//HAL_USE_CANTRANSCEIVER     
    




#ifdef  HAL_USE_EEPROM
    
    #include "hal_eeprom_hid.h" 
    #include "hl_chip_xx_eeprom.h"
    
    const hl_chip_xx_eeprom_cfg_t hl_eepromcfg = 
    {
        .chip           = hl_chip_xx_eeprom_chip_atmel_at24c512b, 
        .i2cid          = hl_i2c1,
        .hwaddra2a1a0   = 0,
        .wp_val         = hl_gpio_valSET,
        .wp_gpio        = { .port = hl_gpio_portD, .pin = hl_gpio_pin10 }           
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
    
//     static const hal_gpio_val_t hal_brdcfg_eeprom__wp_gpio_enableval    = hal_gpio_valHIGH;      // write protection is on value high                          
//     static const hal_gpio_val_t hal_brdcfg_eeprom__wp_gpio_disableval   = hal_gpio_valLOW;       // write protection is on value high
//     
//     static const hal_gpio_map_t hal_brdcfg_eeprom__gpiomapof_wp  = 
//     {    
//         .gpio   = { .port = hal_gpio_portD,     .pin = hal_gpio_pin10 }, 
//         .af32   = hal_NA32
//     };
//     static const hal_gpio_cfg_t hal_brdcfg_eeprom__gpiocfgof_wp =
//     {
//         .dir = hal_gpio_dirOUT,     .speed = hal_gpio_speed_high,       .altcfg = NULL
//     };
//    
//     static hal_result_t s_hal_brdcfg_eeprom__wp_init(void)
//     {
//         const hal_gpio_map_t* map = &hal_brdcfg_eeprom__gpiomapof_wp;
//         return(hal_gpio_init(map->gpio, &hal_brdcfg_eeprom__gpiocfgof_wp));
//     }

//     static hal_result_t s_hal_brdcfg_eeprom__wp_enable(void)
//     {
//         const hal_gpio_map_t* map = &hal_brdcfg_eeprom__gpiomapof_wp;
//         return(hal_gpio_setval(map->gpio, hal_brdcfg_eeprom__wp_gpio_enableval));
//     }

//     static hal_result_t s_hal_brdcfg_eeprom__wp_disable(void)
//     {
//         const hal_gpio_map_t* map = &hal_brdcfg_eeprom__gpiomapof_wp;
//         return(hal_gpio_setval(map->gpio, hal_brdcfg_eeprom__wp_gpio_disableval));
//     }
   

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


#ifdef HAL_USE_ENCODER
    
    #include "hal_encoder_hid.h"

    extern const hal_encoder_boardconfig_t hal_encoder__theboardconfig =
    {
        .supportedmask              = (1 << hal_encoder1) | (1 << hal_encoder2) | (1 << hal_encoder3) | (1 << hal_encoder4) | (1 << hal_encoder5) | (1 << hal_encoder6), 
        .spimaxspeed                =  1000*1000, // not more than 1 mhz. actually on stm32f4 it is exactly 42m/64 = 0.65625 mhz
        .spimap                     =
        {
            {   // hal_encoder1:    P6 on ems4rd
                .spiid = hal_spi2,  .muxid = hal_mux2,  .muxsel = hal_mux_selA                
            },
            {   // hal_encoder2:    P8 on ems4rd
                .spiid = hal_spi2,  .muxid = hal_mux2,  .muxsel = hal_mux_selB                
            },            
            {   // hal_encoder3:    P10 on ems4rd
                .spiid = hal_spi2,  .muxid = hal_mux2,  .muxsel = hal_mux_selC                
            },
            {   // hal_encoder4:    P7 on ems4rd
                .spiid = hal_spi3,  .muxid = hal_mux3,  .muxsel = hal_mux_selA                
            },
            {   // hal_encoder5:    P9 on ems4rd
                .spiid = hal_spi3,  .muxid = hal_mux3,  .muxsel = hal_mux_selB                
            },            
            {   // hal_encoder6:    P11 on ems4rd
                .spiid = hal_spi3,  .muxid = hal_mux3,  .muxsel = hal_mux_selC                
            }                        
        }

    };
    
#endif//HAL_USE_ENCODER  
  
  

#ifdef  HAL_USE_ETHTRANSCEIVER
    
    #include "hal_ethtransceiver_hid.h" 
    
    
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
    

#else

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

    
#ifdef  HAL_USE_GYROSCOPE   
    
    #include "hal_gyroscope_hid.h"
    #include "hl_chip_st_l3g4200d.h"
       
    static hal_result_t s_gyro_init(int32_t id, void* p)
    {
        return((hal_result_t)hl_chip_st_l3g4200d_init((const hl_chip_st_l3g4200d_cfg_t*)p));
    }

    static hal_result_t s_gyro_read(int32_t id, void* x, void* y, void* z)
    {
        return((hal_result_t)hl_chip_st_l3g4200d_angrate_get((int32_t*)x, (int32_t*)y, (int32_t*)z));
    }    
    
    extern const hal_gyroscope_boardconfig_t hal_gyroscope__theboardconfig =
    {
        .supportedmask          = (1 << hal_gyroscope1),
        .driver                 =
        {   
            {   // hal_gyroscope1
                .cfg    =
                {
                    .initpar    = (void*)&l3g4200dcfg                    
                },
                .fun    =
                {
                    .init       = s_gyro_init,
                    .read       = s_gyro_read                    
                }
            }
        }
    };

    
#endif//HAL_USE_GYROSCOPE
 

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
                    .gpio   = { .port = hal_gpio_portE,     .pin = hal_gpio_pin8 }, 
                    .af32   = hal_NA32
                }
            },
            {   // hal_led1 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portE,     .pin = hal_gpio_pin10}, 
                    .af32   = hal_NA32
                }
            },            
            {   // hal_led2 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portE,     .pin = hal_gpio_pin12}, 
                    .af32   = hal_NA32
                }
            },            
            {   // hal_led3 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portE,     .pin = hal_gpio_pin15}, 
                    .af32   = hal_NA32
                }
            },            
            {   // hal_led4 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portD,     .pin = hal_gpio_pin11}, 
                    .af32   = hal_NA32
                }
            },            
            {   // hal_led5 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portB,     .pin = hal_gpio_pin7}, 
                    .af32   = hal_NA32
                }
            },                      
            {   // hal_led6 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE}, 
                    .af32   = hal_NA32
                }
            },              
            {   // hal_led7 
                .led    = 
                {
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE}, 
                    .af32   = hal_NA32
                }
            }  
        }
    };
    
#endif//HAL_USE_LED
 
    
    
#ifdef HAL_USE_MUX
    
    #include "hal_mux_hid.h"
    
    extern const hal_mux_boardconfig_t hal_mux__theboardconfig =
    {
        .supportedmask      = (0 << hal_mux1) | (1 << hal_mux2) | (1 << hal_mux3) | (0 << hal_mux4), 
        .gpiomap            =
        {
            {   // hal_mux1   
                .gpio_sel0              =                
                {    
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32
                },                      
                .gpio_sel1              =
                {
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32                    
                },
                .gpio_enable            =
                {
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32                    
                }
            }, 
            {   // hal_mux2   
                .gpio_sel0              =                
                {    
                    .gpio   = { .port = hal_gpio_portD,  .pin = hal_gpio_pin2 }, 
                    .af32   = hal_NA32
                },                      
                .gpio_sel1              =
                {
                    .gpio   = { .port = hal_gpio_portD,  .pin = hal_gpio_pin3 }, 
                    .af32   = hal_NA32                    
                },
                .gpio_enable            =
                {
                    .gpio   = { .port = hal_gpio_portD,  .pin = hal_gpio_pin4 }, 
                    .af32   = hal_NA32                    
                }
            },
            {   // hal_mux3   
                .gpio_sel0              =                
                {    
                    .gpio   = { .port = hal_gpio_portD,  .pin = hal_gpio_pin5 }, 
                    .af32   = hal_NA32
                },                      
                .gpio_sel1              =
                {
                    .gpio   = { .port = hal_gpio_portD,  .pin = hal_gpio_pin6 }, 
                    .af32   = hal_NA32                    
                },
                .gpio_enable            =
                {
                    .gpio   = { .port = hal_gpio_portD,  .pin = hal_gpio_pin7 }, 
                    .af32   = hal_NA32                    
                }
            }, 
            {   // hal_mux4   
                .gpio_sel0              =                
                {    
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32
                },                      
                .gpio_sel1              =
                {
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32                    
                },
                .gpio_enable            =
                {
                    .gpio   = { .port = hal_gpio_portNONE,  .pin = hal_gpio_pinNONE }, 
                    .af32   = hal_NA32                    
                }
            }            
        }
    };
    
#endif//HAL_USE_MUX      
    

    
#ifdef HAL_USE_SWITCH
    
    #include "hal_switch_hid.h" 
    

#if     defined(EMS4RD_USE_MICREL_AS_MANAGED_DEVICE)   

    #include "hl_chip_micrel_ks8893.h"
    

    extern const hal_switch_boardconfig_t hal_switch__theboardconfig =
    {
        .supportedmask      = (1 << hal_switch1),
        .driver                 =
        {
            {   // hal_switch1
                .cfg    =
                {
                    .numofphysicals     = 2,
                    .targetphymode      = HAL_ETHTRANSCEIVER_PHYMODE_THEONE2USE,  
                    .initpar            = (void*)&ks8893_config,                    
                },
                .fun    =
                {
                    .init           = s_switch_micrel_init,
                    .start          = s_switch_micrel_start//, 
 //                     .getmiiphymode  = s_switch_micrel_getmiiphymode,                
//                     .phylinkupmask  = s_switch_micrel_getlinkupmask,
//                     .phystatus      = s_switch_micrel_getphystatus,
//                     .phyerrorinfo   = s_switch_micrel_getphyerrorinfo                 
                }  
            } 
        }         
    };
            
   
#else
    
    extern const hal_switch_boardconfig_t hal_switch__theboardconfig =
    {
        .supportedmask      = (1 << hal_switch1),
        .driver                 =
        {
            {   // hal_switch1
                .cfg    =
                {
                    .numofphysicals     = 2,
                    .targetphymode      = HAL_ETHTRANSCEIVER_PHYMODE_THEONE2USE,  
                    .initpar            = NULL,                    
                },
                .fun    =
                {
                    .init           = s_switch_dummy_init,
                    .start          = s_switch_dummy_start//, 
//                     .getmiiphymode  = s_switch_dummy_getmiiphymode,                
//                     .phylinkupmask  = s_switch_dummy_getlinkupmask,
//                     .phystatus      = s_switch_dummy_getphystatus,
//                     .phyerrorinfo   = s_switch_dummy_getphyerrorinfo  
                 
                }  
            } 
        }         
    };    
    
    
#endif    

#endif//HAL_USE_SWITCH
      
    
    
#ifdef  HAL_USE_TERMOMETER 
    
    #include "hal_termometer_hid.h" 
    
    #include "hl_chip_st_l3g4200d.h"
       
    static hal_result_t s_term_init(int32_t id, void* p)
    {
        return((hal_result_t)hl_chip_st_l3g4200d_init((const hl_chip_st_l3g4200d_cfg_t*)p));
    }

    static hal_result_t s_term_read(int32_t id, void* t)
    {
        return((hal_result_t)hl_chip_st_l3g4200d_temp_get((int8_t*)t));
    }        
        
    extern const hal_termometer_boardconfig_t hal_termometer__theboardconfig =
    {
        .supportedmask      = (1 << hal_termometer1),
        .driver             =
        {
            {   // hal_termometer1: use the l3g4200d chip by st (the same as the gyro)
                .cfg    =
                {
                    .initpar    = (void*)&l3g4200dcfg,
                },    
                {   // use the l3g4200d chip by st
                    .init       = s_term_init,
                    .read       = s_term_read
                }                    
            }

        }        
    };
    
   
#endif//HAL_USE_TERMOMETER
    


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
    s_hal_brdcfg_ems4rd_vaux_5v0_init();
    s_hal_brdcfg_ems4rd_vaux_5v0_on();

    return(hal_res_OK);    
}


// hl_system_stm32fx_before_setsysclock() as used inside hl_wrap_system.c
#include "hl_core.h"    // contains declarations in cmsis 
 
#include "hl_cfg_plus_target.h"  
#include "hl_cfg_plus_modules.h"   

#include "hl_gpio.h"
#include "hl_sys.h"


extern void hl_system_stm32fx_before_setsysclock(void)
{   // this function executes inside SystemInit() before SetSysClock(). 
    
#if     !defined(EMS4RD_USE_MICREL_AS_MANAGED_DEVICE) 
    
        #warning --> hl_system_stm32fx_before_setsysclock() is empty
    
#elif   defined(EMS4RD_USE_MICREL_AS_MANAGED_DEVICE)  

    #warning --> hl_system_stm32fx_before_setsysclock() enables the micrel to use the managed mode
    
    // this code forces the micrel to use managed mode, so that the application in runtime can use i2c to configure it.
    // this forcing mode must be done before the mpu initialises the proper hw registers of the clock inside SetSysClock()
    
    SystemCoreClockUpdate();    // required so that the hl_sys_delay(0 function works properly 
    
    const hl_gpio_t ethslv = {.port = hl_gpio_portB, .pin = hl_gpio_pin2};
    hl_gpio_pin_output_init(ethslv);
    
    const hl_gpio_t notethrst = {.port = hl_gpio_portH, .pin = hl_gpio_pin1};
    hl_gpio_pin_output_init(notethrst);

    const hl_reltime_t resettime = HL_CHIP_MICREL_RESET_TIME_USEC;   
    
    hl_gpio_pin_write(ethslv, hl_gpio_valSET);
    hl_gpio_pin_write(notethrst, hl_gpio_valRESET);
    hl_sys_delay(resettime);    
    hl_gpio_pin_write(notethrst, hl_gpio_valSET);
    
//    hl_sys_itm_puts("executed hl_system_stm32fx_before_setsysclock() w/ code for EMS4RD_USE_MICREL_AS_MANAGED_DEVICE"); 
    
#endif//defined(EMS4RD_USE_MICREL_AS_MANAGED_DEVICE)    
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions
// --------------------------------------------------------------------------------------------------------------------


static const hal_gpio_map_t s_hal_brdcfg_ems4rd_vaux_5v0_gpiomap = 
{
    .gpio   = { .port = hal_gpio_portE,     .pin = hal_gpio_pin1 }, 
    .af32   = hal_NA32      
};

static const hal_gpio_cfg_t s_hal_brdcfg_ems4rd_vaux_5v0_gpiocfg =
{
    .dir = hal_gpio_dirOUT,     .speed = hal_gpio_speed_low,        .altcfg = NULL
};

#define NNNEW
//#undef NNNEW

#if defined(NNNEW)
static uint8_t vaux_5v0_initted = 0; 
static uint8_t vaux_5v0_status = 0; // 0 off, 1 on
#endif

static void s_hal_brdcfg_ems4rd_vaux_5v0_init(void)
{
#if !defined(NNNEW)    
    static uint8_t initted = 0;
    if(1 == initted)
    {
        return;
    }
#else
    if(1 == vaux_5v0_initted)
    {
        return;
    }    
#endif    
    hal_gpio_init(s_hal_brdcfg_ems4rd_vaux_5v0_gpiomap.gpio, &s_hal_brdcfg_ems4rd_vaux_5v0_gpiocfg); 
#if !defined(NNNEW)    
    initted = 1;
#else
    vaux_5v0_initted = 1;
#endif    
    s_hal_brdcfg_ems4rd_vaux_5v0_on();    
}


static void s_hal_brdcfg_ems4rd_vaux_5v0_on(void)
{
#if defined(NNNEW) 
    if(0 == vaux_5v0_initted)
    {
        return;
    }
    if(hal_gpio_valHIGH == hal_gpio_getval(s_hal_brdcfg_ems4rd_vaux_5v0_gpiomap.gpio))
    {
        return;
    }
#endif    
    hal_gpio_setval(s_hal_brdcfg_ems4rd_vaux_5v0_gpiomap.gpio, hal_gpio_valHIGH);   
}

static void s_hal_brdcfg_ems4rd_vaux_5v0_off(void)
{
#if defined(NNNEW) 
    if(0 == vaux_5v0_initted)
    {
        return;
    }    
    if(hal_gpio_valLOW == hal_gpio_getval(s_hal_brdcfg_ems4rd_vaux_5v0_gpiomap.gpio))
    {
        return;
    }
#endif    
    hal_gpio_setval(s_hal_brdcfg_ems4rd_vaux_5v0_gpiomap.gpio, hal_gpio_valLOW);
}  



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


