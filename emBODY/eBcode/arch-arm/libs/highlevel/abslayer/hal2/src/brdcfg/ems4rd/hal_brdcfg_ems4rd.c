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
#include "hal_base_hid.h"

#include "hal_cpu.h"
#include "hal_cpu_hid.h"



#include "hal_can_hid.h"
#include "hal_spi_hid.h"
#include "hal_eth_hid.h"
#include "hal_trace_hid.h"
#include "hal_eeprom.h"
#include "hal_timer.h"
#include "hal_i2c.h"
#include "hal_watchdog.h"

#include "hal_switch.h"



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

    #include "hal_eth.h"

#if     !defined(EMS4RD_USE_MICREL_AS_MANAGED_DEVICE) 

    #warning --> using micrel as automatic un-mananaged device

    // the micrel is hw configured in autonegotiation for its port1 and port2. 
    // the mode for the rmii cannot be read from the micrel, thus we assume it is fd100
    #define HAL_ETHTRANSCEIVER_PHYMODE_THEONE2USE       hal_ethtransceiver_phymode_auto
    #define HAL_ETH_PHYMODE_THEONEUSED                  hal_ethtransceiver_phymode_fullduplex100mbps

    static hal_result_t s_switch_dummy_init(void* param)
    {
        // the HAL_ETHTRANSCEIVER_PHYMODE_THEONE2USE mode is done in HW
        return(hal_res_OK);
    }   
    static hal_result_t s_switch_dummy_configure(void* p)
    {
        hal_ethtransceiver_phymode_t* usedphymode = (hal_ethtransceiver_phymode_t*)p;
        // the mode of the rmii is surely this one
        *usedphymode = HAL_ETH_PHYMODE_THEONEUSED;
        return(hal_res_OK);
    }   
    static hal_result_t s_switch_dummy_getphymode(void* p)
    {
        hal_ethtransceiver_phymode_t* usedphymode = (hal_ethtransceiver_phymode_t*)p;
        // the mode of the rmii is surely this one
        *usedphymode = HAL_ETH_PHYMODE_THEONEUSED;
        return(hal_res_OK);
    }  
    
#else
    
    #warning --> using micrel as managed device w/ EMS4RD_USE_MICREL_AS_MANAGED_DEVICE

    #include "hl_chip_micrel_ks8893.h"
    
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
    
    static hal_result_t s_switch_micrel_init(void* param)
    {
        // i2c3 must be initted.
        hal_i2c_init((hal_i2c_t)ks8893_config.i2cid, NULL);        
        hl_result_t r = hl_chip_micrel_ks8893_init((const hl_chip_micrel_ks8893_cfg_t*)param);
        return((hal_result_t)r);
    }  

    static hal_result_t s_switch_micrel_configure(void* p)
    {
        //hal_ethtransceiver_phymode_t* usedphymode = (hal_ethtransceiver_phymode_t*);
        return((hal_result_t)hl_chip_micrel_ks8893_configure((hl_ethtrans_phymode_t*)p));
    }

    static hal_result_t s_switch_micrel_getphymode(void* p)
    {
        //hal_ethtransceiver_phymode_t* usedphymode = (hal_ethtransceiver_phymode_t*)p;
        return((hal_result_t)hl_chip_micrel_ks8893_mii_getphymode((hl_ethtrans_phymode_t*)p));        
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
    extern const hal_base_hid_brdcfg_t hal_brdcfg_base__theconfig  =
    {
        .dummy  = 0
    };    
#endif//HAL_USE_BASE  
    
    
#ifdef  HAL_USE_CPU
    extern const hal_cpu_hid_brdcfg_t hal_brdcfg_cpu__theconfig  =
    {
        .architecture       = hal_cpu_arc_armcm4,
        .family             = hal_cpu_fam_stm32f4,
        .name               = hal_cpu_nam_stm32f407,
        .speeds             =
        {   // these values must be coherent w/ HL_CFG_MPUSPEED_STM32xxx valued defined in hl_cfg_plus_target.h 
            .max                = 168000000,
            .cpu                = 168000000,
            .fastbus            =  84000000,
            .slowbus            =  42000000
        } 
    };    
#endif//HAL_USE_CPU  

#ifdef  HAL_USE_FLASH
    extern const hal_flash_hid_brdcfg_t hal_brdcfg_flash__theconfig =
    {
        .baseaddress    =   0x08000000,                 // on every stm32
        .totalsize      =
        #if     defined(HAL_USE_CPU_NAM_STM32F107)
                            256*1024
        #elif   defined(HAL_USE_CPU_NAM_STM32F407)
                            1024*1024
        #endif
    };
#endif//HAL_USE_FLASH
 

#ifdef  HAL_USE_HEAP
    extern const hal_heap_hid_brdcfg_t hal_brdcfg_heap__theconfig =
    {
        .nothingsofar   = 0
    };
#endif//HAL_USE_HEAP    
   
    
#ifdef  HAL_USE_SYS
    extern const hal_sys_hid_brdcfg_t hal_brdcfg_sys__theconfig  =
    {
        .dummy             = 0
    };    
#endif//HAL_USE_SYS   
   

// --  extra: mpu peripherals

#ifdef  HAL_USE_CAN
    
    extern const hal_can_hid_brdcfg_t hal_brdcfg_can__theconfig =
    {
        .supported_mask     = (1 << hal_can1) | (1 << hal_can2),
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
    
    extern const hal_eth_hid_brdcfg_t hal_brdcfg_eth__theconfig     =
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
    extern const hal_gpio_hid_brdcfg_t hal_brdcfg_gpio__theconfig =
    {
        .supported_mask_byport    =
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
    extern const hal_i2c_hid_brdcfg_t hal_brdcfg_i2c__theconfig =
    {
        .supported_mask         = (1 << hal_i2c1) | (0 << hal_i2c2) | (1 << hal_i2c3),
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

    extern const hal_spi_hid_brdcfg_t hal_brdcfg_spi__theconfig =
    {
        .supported_mask         = (0 << hal_spi1) | (1 << hal_spi2) | (1 << hal_spi3),
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
    extern const hal_timer_hid_brdcfg_t hal_brdcfg_timer__theconfig =
    {
        .supported_mask     = (1 << hal_timer1) | (1 << hal_timer2) | (1 << hal_timer3) | (1 << hal_timer4) | (1 << hal_timer5) | (1 << hal_timer6) | (1 << hal_timer7) | (1 << hal_timer8)
    };       
#endif//HAL_USE_TIMER

    
#ifdef  HAL_USE_TRACE
    extern const hal_trace_hid_brdcfg_t hal_brdcfg_trace__theconfig =
    {
        .supported          = hal_true
    };       
#endif//HAL_USE_TRACE
  

#ifdef  HAL_USE_UNIQUEID
    extern const hal_uniqueid_hid_brdcfg_t hal_brdcfg_uniqueid__theconfig =
    {
        .supported_mask     = (1 << hal_uniqueid_id64bit) | (0 << hal_uniqueid_macaddr),
        .macoui             = 0
    };       
#endif//HAL_USE_UNIQUEID
    

#ifdef  HAL_USE_WATCHDOG
    extern const hal_watchdog_hid_brdcfg_t hal_brdcfg_watchdog__theconfig =
    {
        .supported_mask     = (1 << hal_watchdog1_normal) | (1 << hal_watchdog2_window)
    };       
#endif//HAL_USE_WATCHDOG


// -- devices

#ifdef  HAL_USE_ACCELEROMETER   
    
    #include "hl_chip_st_lis3x.h"
    
    static const hl_chip_st_lis3x_cfg_t lis3xcfg =
    {
        .chip       = hl_chip_st_lis3x_chip_autodetect,
        .i2cid      = hl_i2c1,
        .range      = hl_chip_st_lis3x_range_2g
    };
    
    extern const hal_accelerometer_hid_brdcfg_t hal_brdcfg_accelerometer__theconfig =
    {
        .supported_mask         = (1 << hal_accelerometer1),
        .devcfg         =
        {
            {   // hal_accelerometer1
                .chipif             =      
                {   // use the lis3x chip by st
                    .init       = (hal_res_fp_voidp_t)hl_chip_st_lis3x_init,
                    .initpar    = (void*)&lis3xcfg,
                    .read       = (hal_accelerometer_hid_fn_read_t)hl_chip_st_lis3x_accel_get  
                }                    
            }
         }
    };   
    
#endif//HAL_USE_ACCELEROMETER


#ifdef  HAL_USE_CANTRANSCEIVER
    
    static hal_result_t s_hal_cantransceiver_simple_init(hal_cantransceiver_t id, void* initpar);
    static hal_result_t s_hal_cantransceiver_simple_enable(hal_cantransceiver_t id);
    static hal_result_t s_hal_cantransceiver_simple_disable(hal_cantransceiver_t id);
    
    static const hal_gpio_maP_t s_hal_cantransceiver__gpiomapof_enable[hal_cantransceivers_number] = 
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
        const hal_gpio_maP_t* map = &s_hal_cantransceiver__gpiomapof_enable[(uint8_t)id];        
        const hal_gpio_cfg_t* cfg = &s_hal_cantransceiver__gpiocfgof_enable[(uint8_t)id];   
        hal_gpio_init(map->gpio, cfg);        
        s_hal_cantransceiver_simple_disable(id);     
        s_hal_brdcfg_ems4rd_vaux_5v0_init();
        s_hal_brdcfg_ems4rd_vaux_5v0_on();
        return(hal_res_OK);
    }

    static hal_result_t s_hal_cantransceiver_simple_enable(hal_cantransceiver_t id)
    {  
        const hal_gpio_maP_t* map = &s_hal_cantransceiver__gpiomapof_enable[(uint8_t)id];          
        hal_gpio_setval(map->gpio, hal_gpio_valLOW);
        return(hal_res_OK);
    }

    static hal_result_t s_hal_cantransceiver_simple_disable(hal_cantransceiver_t id)
    {   
        const hal_gpio_maP_t* map = &s_hal_cantransceiver__gpiomapof_enable[(uint8_t)id];         
        hal_gpio_setval(map->gpio, hal_gpio_valHIGH);
        return(hal_res_OK);
    }
    
    extern const hal_cantransceiver_hid_brdcfg_t hal_brdcfg_cantransceiver__theconfig =
    {
        .supported_mask     =  (1 << hal_cantransceiver1) | (1 << hal_cantransceiver2),
        .devcfg             =
        {            
            .chipif             =
            {
                .init               = s_hal_cantransceiver_simple_init,
                .initpar            = NULL,
                .enable             = s_hal_cantransceiver_simple_enable,
                .disable            = s_hal_cantransceiver_simple_disable
            }
        }   
    };    
#endif//HAL_USE_CANTRANSCEIVER     
    




#ifdef  HAL_USE_EEPROM
    
    #include "hl_chip_xx_eeprom.h"
    
    const hl_chip_xx_eeprom_cfg_t hl_eepromcfg = 
    {
        .chip           = hl_chip_xx_eeprom_chip_atmel_at24c512b, 
        .i2cid          = hl_i2c1,
        .hwaddra2a1a0   = 0,
        .wp_val         = hl_gpio_valSET,
        .wp_gpio        = { .port = hl_gpio_portD, .pin = hl_gpio_pin10 }           
    };        
    static hal_result_t s_chip_xx_eeprom_init(const hl_chip_xx_eeprom_cfg_t * cfg)
    {
        return((hal_result_t)hl_chip_xx_eeprom_init(cfg));
    }
    static hal_result_t s_chip_xx_eeprom_write(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* writtenbytes)
    {
        return((hal_result_t)hl_chip_xx_eeprom_write(address, size, buffer, writtenbytes));        
    }
    static hal_result_t s_chip_xx_eeprom_read(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* writtenbytes)
    {
        return((hal_result_t)hl_chip_xx_eeprom_read(address, size, buffer, writtenbytes));        
    }   
    
    static const hal_gpio_val_t hal_brdcfg_eeprom__wp_gpio_enableval    = hal_gpio_valHIGH;      // write protection is on value high                          
    static const hal_gpio_val_t hal_brdcfg_eeprom__wp_gpio_disableval   = hal_gpio_valLOW;       // write protection is on value high
    
    static const hal_gpio_maP_t hal_brdcfg_eeprom__gpiomapof_wp  = 
    {    
        .gpio   = { .port = hal_gpio_portD,     .pin = hal_gpio_pin10 }, 
        .af32   = hal_NA32
    };
    static const hal_gpio_cfg_t hal_brdcfg_eeprom__gpiocfgof_wp =
    {
        .dir = hal_gpio_dirOUT,     .speed = hal_gpio_speed_high,       .altcfg = NULL
    };

    

    static hal_result_t s_hal_brdcfg_eeprom__wp_init(void)
    {
        const hal_gpio_maP_t* map = &hal_brdcfg_eeprom__gpiomapof_wp;
        return(hal_gpio_init(map->gpio, &hal_brdcfg_eeprom__gpiocfgof_wp));
    }

    static hal_result_t s_hal_brdcfg_eeprom__wp_enable(void)
    {
        const hal_gpio_maP_t* map = &hal_brdcfg_eeprom__gpiomapof_wp;
        return(hal_gpio_setval(map->gpio, hal_brdcfg_eeprom__wp_gpio_enableval));
    }

    static hal_result_t s_hal_brdcfg_eeprom__wp_disable(void)
    {
        const hal_gpio_maP_t* map = &hal_brdcfg_eeprom__gpiomapof_wp;
        return(hal_gpio_setval(map->gpio, hal_brdcfg_eeprom__wp_gpio_disableval));
    }
   

    extern const hal_eeprom_hid_brdcfg_t hal_brdcfg_eeprom__theconfig =
    {
        .supported_mask     =  (0 << hal_eeprom_emulatedflash) | (1 << hal_eeprom_i2c_01) | (0 << hal_eeprom_i2c_02),
        .devcfg             =
        {            
            {   // hal_eeprom1_emulatedflash 
                .flashemul  =
                {
                    .baseaddress    = 0x08000000,
                    .totalsize      = 0
                }
            },
            {   // hal_eeprom2_i2c_01
                .i2cbased   =
                {
                    .baseaddress    = 0,
                    .totalsize      = 8*1024,
                    .i2cid          = hal_i2c1,
                    .chipif         =
                    {   // use the chip xx_eeprom
                        .init           = (hal_res_fp_voidp_t)s_chip_xx_eeprom_init, //(hal_res_fp_voidp_t)hal_chip_xx_eeprom_init,
                        .initpar        = (void*)&hl_eepromcfg, //(void*)&s_initpar_eeprom_i2c_01,
                        .read           = s_chip_xx_eeprom_read, //hal_chip_xx_eeprom_read,
                        .write          = s_chip_xx_eeprom_write, //hal_chip_xx_eeprom_write                               
                    }                    
                }           
            },
            {   // hal_eeprom3_i2c_02
                .i2cbased   =
                {
                    .baseaddress    = 0,
                    .totalsize      = 0,
                    .i2cid          = hal_i2c1,
                    .chipif         =
                    {   // dont use any chip
                        .init           = NULL,
                        .initpar        = NULL,
                        .read           = NULL,
                        .write          = NULL                               
                    }                    
                }           
            }                
        }
    
    };    
#endif//HAL_USE_EEPROM 


#ifdef HAL_USE_ENCODER

    extern const hal_encoder_hid_brdcfg_t hal_brdcfg_encoder__theconfig =
    {
        .supported_mask             = (1 << hal_encoder1) | (1 << hal_encoder2) | (1 << hal_encoder3) | (1 << hal_encoder4) | (1 << hal_encoder5) | (1 << hal_encoder6), 
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
    
    
#if     defined(EMS4RD_USE_MICREL_AS_MANAGED_DEVICE)      
        
       
    //extern const hal_chip_micrel_ks8893_cfg_t s_micrel_ks8893_cfg;
    extern const hal_ethtransceiver_hid_brdcfg_t hal_brdcfg_ethtransceiver__theconfig =
    {
        .supported      = hal_true,
        .devcfg         =
        {
            .numofphysicals     = 2,
            .targetphymode      = HAL_ETHTRANSCEIVER_PHYMODE_THEONE2USE,
            .chipif             =
            {   // use the micrel 
                .init           = s_switch_micrel_init,
                .initpar        = (void*)&ks8893_config,
                .config         = s_switch_micrel_configure, 
                .getphymode     = (hal_res_fp_voidp_t)hl_chip_micrel_ks8893_mii_getphymode,                
                .phylinkupmask  = (hal_res_fp_voidp_t)hl_chip_micrel_ks8893_linkupmask,
                .phystatus      = (hal_res_fp_voidp_uint8_t)hl_chip_micrel_ks8893_phy_status,
                .phyerrorinfo   = (hal_res_fp_uint8_uint32_voidp_t)hl_chip_micrel_ks8893_phy_errorinfo
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
    extern const hal_ethtransceiver_hid_brdcfg_t hal_brdcfg_ethtransceiver__theconfig =
    {
        .supported      = hal_true,
        .devcfg         =
        {
            .targetphymode      = HAL_ETHTRANSCEIVER_PHYMODE_THEONE2USE,
            .chipif             =
            {   // use the micrel 
                .init           = s_switch_dummy_init,
                .initpar        = NULL,
                .config         = s_switch_dummy_configure, 
                .getphymode     = s_switch_dummy_getphymode,
                .phylinkupmask  = s_switch_dummy_linkupmask,
                .phystatus      = s_switch_dummy_phy_status,
                .phyerrorinfo   = s_switch_dummy_phy_errorinfo                
            }
         }
    }; 
    
#endif
    
#endif//HAL_USE_ETHTRANSCEIVER

    
#ifdef  HAL_USE_GYROSCOPE   
    
    extern const hal_gyroscope_hid_brdcfg_t hal_brdcfg_gyroscope__theconfig =
    {
        .supported_mask         = (1 << hal_gyroscope1),
        .devcfg         =
        {
            {   // hal_gyroscope1
                .chipif             =      
                {   // use the l3g4200d chip by st
                    .init       = (hal_res_fp_voidp_t)hl_chip_st_l3g4200d_init,
                    .initpar    = (void*)&l3g4200dcfg,
                    .read       = (hal_gyroscope_hid_fn_read_t)hl_chip_st_l3g4200d_angrate_get
                }                    
            }
         }
    }; 
    
#endif//HAL_USE_GYROSCOPE
 

#ifdef HAL_USE_LED
    
    extern const hal_led_hid_brdcfg_t hal_brdcfg_led__theconfig =
    {
        .supported_mask             = (1 << hal_led0) | (1 << hal_led1) | (1 << hal_led2) | (1 << hal_led3) | (1 << hal_led4) | (1 << hal_led5), // = 0x3F: only first 6 leds ...
        .value_on                   = hal_gpio_valLOW,
        .value_off                  = hal_gpio_valHIGH,
        .gpiomap                    =
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
    
    extern const hal_mux_hid_brdcfg_t hal_brdcfg_mux__theconfig =
    {
        .supported_mask             = (0 << hal_mux1) | (1 << hal_mux2) | (1 << hal_mux3) | (0 << hal_mux4), 
        .gpiomap                    =
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
    

#if     defined(EMS4RD_USE_MICREL_AS_MANAGED_DEVICE)   

    #include "hl_chip_micrel_ks8893.h"
    

    extern const hal_switch_hid_brdcfg_t hal_brdcfg_switch__theconfig =
    {
        .supported      = hal_true,
        .devcfg         =
        {
            .targetphymode  = HAL_ETHTRANSCEIVER_PHYMODE_THEONE2USE,
            .chipif         =
            {
                .init       = s_switch_micrel_init,
                .initpar    = (void*)&ks8893_config,
                .config     = s_switch_micrel_configure,          
            }
        }
    };
       
    
#else
    
    extern const hal_switch_hid_brdcfg_t hal_brdcfg_switch__theconfig =
    {
        .supported      = hal_true,
        .devcfg         =
        {
            .targetphymode  = HAL_ETHTRANSCEIVER_PHYMODE_THEONE2USE,
            .chipif         =
            {
                .init       = s_switch_dummy_init,
                .initpar    = NULL,
                .config     = s_switch_dummy_configure,          
            }
        }
    };
    
#endif    

#endif//HAL_USE_SWITCH
      
    
    
#ifdef  HAL_USE_TERMOMETER 
    
    extern const hal_termometer_hid_brdcfg_t hal_brdcfg_termometer__theconfig =
    {
        .supported_mask         = (1 << hal_termometer1),
        .devcfg         =
        {
            {   // hal_termometer1
                .chipif             =      
                {   // use the l3g4200d chip by st
                    .init       = (hal_res_fp_voidp_t)hl_chip_st_l3g4200d_init,
                    .initpar    = (void*)&l3g4200dcfg,
                    .read       = (hal_termometer_hid_fn_read_t)hl_chip_st_l3g4200d_temp_get
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




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions
// --------------------------------------------------------------------------------------------------------------------


static const hal_gpio_maP_t s_hal_brdcfg_ems4rd_vaux_5v0_gpiomap = 
{
    .gpio   = { .port = hal_gpio_portE,     .pin = hal_gpio_pin1 }, 
    .af32   = hal_NA32      
};

static const hal_gpio_cfg_t s_hal_brdcfg_ems4rd_vaux_5v0_gpiocfg =
{
    .dir = hal_gpio_dirOUT,     .speed = hal_gpio_speed_low,        .altcfg = NULL
};
    
static void s_hal_brdcfg_ems4rd_vaux_5v0_init(void)
{
    static uint8_t initted = 0;
    if(1 == initted)
    {
        return;
    }
    hal_gpio_init(s_hal_brdcfg_ems4rd_vaux_5v0_gpiomap.gpio, &s_hal_brdcfg_ems4rd_vaux_5v0_gpiocfg); 
    initted = 1;
    s_hal_brdcfg_ems4rd_vaux_5v0_on();    
}

static void s_hal_brdcfg_ems4rd_vaux_5v0_on(void)
{
    hal_gpio_setval(s_hal_brdcfg_ems4rd_vaux_5v0_gpiomap.gpio, hal_gpio_valHIGH);   
}

static void s_hal_brdcfg_ems4rd_vaux_5v0_off(void)
{
    hal_gpio_setval(s_hal_brdcfg_ems4rd_vaux_5v0_gpiomap.gpio, hal_gpio_valLOW);
}  



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


