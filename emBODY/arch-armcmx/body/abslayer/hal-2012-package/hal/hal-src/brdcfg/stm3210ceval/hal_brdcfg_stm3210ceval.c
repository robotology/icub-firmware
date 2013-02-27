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

/* @file       hal_brdcfg_stm3210ceval.c
	@brief      This header file defines support for board stm3210ceval
	@author     marco.accame@iit.it
    @date       12/18/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "hal_mpu_stm32xx_include.h"

#include "stdlib.h"
#include "hal_base.h"
#include "string.h"
#include "hal_base_hid.h"
#include "hal_mpu_can_hid.h"
#include "hal_mpu_spi_hid.h"
#include "hal_mpu_eth_hid.h"
#include "hal_mpu_trace_hid.h"
#include "hal_eeprom.h"
#include "hal_timer.h"
#include "hal_i2c.h"
#include "hal_watchdog.h"

#include "hal_switch.h"
#include "hal_device_ethtransceiver_hid.h"

#include "hal_chip_xx_eeprom_hid.h"
#include "hal_chip_st_l3g4200d_hid.h"
#include "hal_chip_st_lis3dh_hid.h"
#include "hal_chip_generic_ethtransceiver_hid.h"


#include "hal_extbrd_keil_mcbqvga_hid.h" 




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_brdcfg_stm3210ceval.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#if defined(HAL_USE_ETH) || defined(HAL_USE_DEVICE_ETHTRANSCEIVER) || defined(HAL_USE_DEVICE_SWITCH)
    #define HAL_ETH_PHYMODE_THEONE2USE     hal_eth_phymode_fullduplex100mbps
#endif



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

// --  mpu peripherals

#ifdef  HAL_USE_CAN
    extern const hal_can_hid_brdcfg_t hal_brdcfg_can__theconfig =
    {
        .supported_mask     = (0 << hal_can_port1) | (0 << hal_can_port2),
        .gpio_rx            =
        {
            {   // hal_can_port1
                .port     = hal_gpio_portNONE, 
                .pin      = hal_gpio_pinNONE,        
                .dir      = hal_gpio_dirALT,
                .speed    = hal_gpio_speed_default
            },
            {   // hal_can_port2
                .port     = hal_gpio_portNONE, 
                .pin      = hal_gpio_pinNONE,        
                .dir      = hal_gpio_dirALT,
                .speed    = hal_gpio_speed_default        
            } 
        },
        .gpio_tx            =
        {   
            {   // hal_can_port1
                .port     = hal_gpio_portNONE,
                .pin      = hal_gpio_pinNONE,        
                .dir      = hal_gpio_dirALT,
                .speed    = hal_gpio_speed_default
            },           
            {   // hal_can_port2
                .port     = hal_gpio_portNONE,
                .pin      = hal_gpio_pinNONE,        
                .dir      = hal_gpio_dirALT,
                .speed    = hal_gpio_speed_default        
            }
        }         
    };
#endif//HAL_USE_CAN
   
    
#ifdef  HAL_USE_CRC
    extern const hal_crc_hid_brdcfg_t hal_brdcfg_crc__theconfig =
    {
        .supported_mask     = (1 << hal_crc0) | (1 << hal_crc1)
    };
#endif//HAL_USE_CRC   

#ifdef  HAL_USE_DMA
    extern const hal_dma_hid_brdcfg_t hal_brdcfg_dma__theconfig =
    {
        .supported_mask     = (1 << hal_dma_port1) | (1 << hal_dma_port2) | (1 << hal_dma_port3) | (1 << hal_dma_port4) | (1 << hal_dma_port5) |
                              (1 << hal_dma_port6) | (1 << hal_dma_port7) | (1 << hal_dma_port8) | (1 << hal_dma_port9) | (1 << hal_dma_port10)|
                              (1 << hal_dma_port11)| (1 << hal_dma_port12)

    };
#endif//HAL_USE_DMA    

    
#ifdef  HAL_USE_ETH    
    extern const hal_eth_hid_brdcfg_t hal_brdcfg_eth__theconfig     =
    {
        .supported          = hal_false,
        .mif                = hal_eth_mif_rmii,
        .gpio_mif.rmii      =
        {
            .ETH_RMII_REF_CLK   = { .port = hal_gpio_portA, .pin = hal_gpio_pin1,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max },
            .ETH_RMII_TX_EN     = { .port = hal_gpio_portB, .pin = hal_gpio_pin11,  .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max },
            .ETH_RMII_TXD0      = { .port = hal_gpio_portB, .pin = hal_gpio_pin12,  .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max },
            .ETH_RMII_TXD1      = { .port = hal_gpio_portB, .pin = hal_gpio_pin13,  .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max },
            .ETH_RMII_CRS_DV    = { .port = hal_gpio_portD, .pin = hal_gpio_pin8,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max },
            .ETH_RMII_RXD0      = { .port = hal_gpio_portD, .pin = hal_gpio_pin9,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max },
            .ETH_RMII_RXD1      = { .port = hal_gpio_portD, .pin = hal_gpio_pin10,  .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max }       
        },
        .gpio_smi           =
        {
            .ETH_MDC            = { .port = hal_gpio_portC, .pin = hal_gpio_pin1,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max },
            .ETH_MDIO           = { .port = hal_gpio_portA, .pin = hal_gpio_pin2,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max }       
        }
    };

#endif//HAL_USE_ETH

#ifdef  HAL_USE_FLASH
    extern const hal_flash_hid_brdcfg_t hal_brdcfg_flash__theconfig =
    {
        .nonesofar  = 0
    };
#endif//HAL_USE_FLASH


#ifdef HAL_USE_GPIO
    extern const hal_gpio_hid_brdcfg_t hal_brdcfg_gpio__theconfig =
    {
        .supported_mask_byport    =
        {   // ok, i enable every pin of every port. 
            0xffff,     // port a
            0xffff,     // port b
            0xffff,     // port c
            0xffff,     // port d
            0xffff,     // port e
            0xffff,     // port f
            0xffff,     // port g
            0x0000,     // port h
            0x0000      // port i
        }
    };
#endif//HAL_USE_GPIO


#ifdef  HAL_USE_I2C
    extern const hal_i2c_hid_brdcfg_t hal_brdcfg_i2c__theconfig =
    {
        .supported_mask         = (0 << hal_i2c_port1),
        .gpio_scl               =
        {
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default } 
        },
        .gpio_sda               =
        {
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default }         
        }
    };
#endif//HAL_USE_I2C


#ifdef  HAL_USE_SPI
    extern const hal_spi_hid_brdcfg_t hal_brdcfg_spi__theconfig =
    {
        .supported_mask         = (0 << hal_spi_port1) | (1 << hal_spi_port2) | (1 << hal_i2c_port3),
        .gpio_sck               =
        {
            { .port = hal_gpio_portA,    .pin = hal_gpio_pin5,    .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portB,    .pin = hal_gpio_pin13,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portC,    .pin = hal_gpio_pin10,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default } 
        },
        .gpio_miso              =
        {
            { .port = hal_gpio_portA,    .pin = hal_gpio_pin6,    .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portB,    .pin = hal_gpio_pin14,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portC,    .pin = hal_gpio_pin11,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default } 
        },
        .gpio_mosi              =
        {
            { .port = hal_gpio_portA,    .pin = hal_gpio_pin7,    .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portB,    .pin = hal_gpio_pin15,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portC,    .pin = hal_gpio_pin12,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default } 
        }      
    };
#endif//HAL_USE_SPI

    
#ifdef  HAL_USE_SYS
    extern const hal_sys_hid_brdcfg_t hal_brdcfg_sys__theconfig  =
    {
        .speeds             =
        {   
            .cpu                =  72000000,
            .fastbus            =  72000000,
            .slowbus            =  36000000
        },
        .clockcfg               =
        {
            .keepinternalclockatstartup     = hal_false,      // false is the preferred mode
            .refclock                       = hal_sys_refclock_pll_on_external_xtl,  
            .intclockspeed                  =   8000000,
            .extclockspeed                  =  25000000,
            .pllcfg                         =
            {   // with refclock = hal_sys_refclock_pll_on_external_xtl -> pll2clock = pll2mul*(extclockspeed/prediv2), speeds.cpu = pllmul*(pll2clock/prediv1) 
                .prediv2            = 5, 
                .pll2mul            = 8,
                .prediv1            = 5,
                .pllmul             = 9
            }
        }   
    };    
#endif//HAL_USE_SYS    
    
     
#ifdef  HAL_USE_TIMER
    extern const hal_timer_hid_brdcfg_t hal_brdcfg_timer__theconfig =
    {
        .supported_mask     = (0 << hal_timer1) | (1 << hal_timer2) | (1 << hal_timer3) | (1 << hal_timer4) | (1 << hal_timer5) | (1 << hal_timer6) | (1 << hal_timer7)
    };       
#endif//HAL_USE_TIMER


#ifdef  HAL_USE_TRACE
    extern const hal_trace_hid_brdcfg_t hal_brdcfg_trace__theconfig =
    {
        .dummy              = 0
    };       
#endif//HAL_USE_TRACE
  
    
#ifdef  HAL_USE_WATCHDOG
    extern const hal_watchdog_hid_brdcfg_t hal_brdcfg_watchdog__theconfig =
    {
        .supported_mask     = (1 << hal_watchdog_normal) | (1 << hal_watchdog_window)
    };       
#endif//HAL_USE_WATCHDOG


// -- devices

#ifdef  HAL_USE_DEVICE_ACCELEROMETER   
    extern const hal_device_accelerometer_hid_brdcfg_t hal_brdcfg_device_accelerometer__theconfig =
    {
        .supported_mask         = 0x0,
        .devcfg         =
        {
            {   // hal_accelerometer_port1
                .chipif             =      
                {   // use the lis3dh chip by st
                    .init       = NULL,
                    .initpar    = NULL,
                    .read       = NULL  
                }                    
            }
         }
    };       
#endif//HAL_USE_DEVICE_ACCELEROMETER
    

#ifdef  HAL_USE_DEVICE_CANTRANSCEIVER

    static hal_result_t s_hal_device_cantransceiver_simple_init(hal_can_port_t port, void* initpar);
    static hal_result_t s_hal_device_cantransceiver_simple_enable(hal_can_port_t port);
    static hal_result_t s_hal_device_cantransceiver_simple_disable(hal_can_port_t port);
    
    static hal_result_t s_hal_device_cantransceiver_simple_init(hal_can_port_t port, void* initpar)
    {
        //#warning WIP --> in here, if required, .... init the gpio
        return(hal_res_OK);
    }

    static hal_result_t s_hal_device_cantransceiver_simple_enable(hal_can_port_t port)
    {   
        //#warning WIP --> in here, if required, .... put gpio low (or high)
        return(hal_res_OK);
    }

    static hal_result_t s_hal_device_cantransceiver_simple_disable(hal_can_port_t port)
    {   
        //#warning WIP --> in here, if required, .... put gpio high (or low)
        return(hal_res_OK);
    }
    
    extern const hal_device_cantransceiver_hid_brdcfg_t hal_brdcfg_device_cantransceiver__theconfig =
    {
        .supported_mask     =  (0 << hal_can_port1) | (0 << hal_can_port2),
        .devcfg             =
        {            
            .chipif             =
            {
                .init               = s_hal_device_cantransceiver_simple_init,
                .initpar            = NULL,
                .enable             = s_hal_device_cantransceiver_simple_enable,
                .disable            = s_hal_device_cantransceiver_simple_disable
            }
        }   
    };    
#endif//HAL_USE_DEVICE_CANTRANSCEIVER     
    

    
#ifdef HAL_USE_DEVICE_DISPLAY  
    extern const hal_device_display_hid_brdcfg_t hal_brdcfg_device_display__theconfig =
    {
        .supported_mask     = (0 << hal_display_dev1),
        .devcfg             =
        {  
            {   // hal_display_dev1
                .chipif             =
                {
                    .init               = NULL,
                    .initpar            = NULL,
                    .clear              = NULL,
                    .settextproperties  = NULL,
                    .clearline          = NULL,
                    .putchar            = NULL,
                    .putstring          = NULL
                }
            }
        }   
    };
#endif//HAL_USE_DEVICE_DISPLAY


#ifdef  HAL_USE_DEVICE_EEPROM
    static hal_result_t s_hal_brdcfg_device_eeprom__wp_init(void)
    {
        return(hal_res_OK);
    }

    static hal_result_t s_hal_brdcfg_device_eeprom__wp_enable(void)
    {
        return(hal_res_OK);
    }

    static hal_result_t s_hal_brdcfg_device_eeprom__wp_disable(void)
    {
        return(hal_res_OK);
    }
    
    // used by hal_brdcfg_device_eeprom__theconfig
    static const hal_chip_xx_eeprom_cfg_t s_initpar_eeprom_i2c_01 =
    {
        .chipcfg    =
        {
            .chip           = hal_chip_xx_eeprom_chip_st_m24c64,
            .i2cport        = hal_i2c_port1,
            .hwaddra2a1a0   = 0 // a0 = a1 = a2 = 0
        },
        .wrprcfg    =
        {
            .wrpr_init              = s_hal_brdcfg_device_eeprom__wp_init,
            .wrpr_deinit            = NULL,
            .wrpr_enable            = s_hal_brdcfg_device_eeprom__wp_enable,
            .wrpr_disable           = s_hal_brdcfg_device_eeprom__wp_disable            
        }
    };    

    extern const hal_device_eeprom_hid_brdcfg_t hal_brdcfg_device_eeprom__theconfig =
    {
        .supported_mask     =  (1 << hal_eeprom_emulatedflash) | (1 << hal_eeprom_i2c_01) | (0 << hal_eeprom_i2c_02),
        .devcfg             =
        {            
            {   // hal_device_eeprom_emulatedflash 
                .flashemul  =
                {
                    .baseaddress    = 0x08000000,
                    .totalsize      = 256*1024
                }
            },
            {   // hal_device_eeprom_i2c_01
                .i2cbased   =
                {
                    .baseaddress    = 0,
                    .totalsize      = 8*1024,
                    .i2cport        = hal_i2c_port1,
                    .chipif         =
                    {   // use the chip xx_eeprom
                        .init           = (hal_res_fp_voidp_t)hal_chip_xx_eeprom_init,
                        .initpar        = (void*)&s_initpar_eeprom_i2c_01,
                        .read           = hal_chip_xx_eeprom_read,
                        .write          = hal_chip_xx_eeprom_write                               
                    }                    
                }           
            },
            {   // hal_device_eeprom_i2c_02
                .i2cbased   =
                {
                    .baseaddress    = 0,
                    .totalsize      = 0,
                    .i2cport        = hal_i2c_port1,
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
#endif//HAL_USE_DEVICE_EEPROM 


#ifdef HAL_USE_DEVICE_ENCODER
    extern const hal_device_encoder_hid_brdcfg_t hal_brdcfg_device_encoder__theconfig =
    {
        .supported_mask             = (0 << hal_encoder1) | (0 << hal_encoder2) | (0 << hal_encoder3) | (0 << hal_encoder4) | (0 << hal_encoder5) | (0 << hal_encoder6), 
        .muxport                    = 
        {   
            hal_mux_port1,  hal_mux_port1,  hal_mux_port1,  hal_mux_port1,  hal_mux_port1,  hal_mux_port1           
        },
        .muxsel                     = 
        {   
            hal_mux_selNONE,hal_mux_selNONE,hal_mux_selNONE,hal_mux_selNONE,hal_mux_selNONE,hal_mux_selNONE           
        },  
        .spiport                     = 
        {   
            hal_spi_port1,  hal_spi_port1,  hal_spi_port1,  hal_spi_port1,  hal_spi_port1,  hal_spi_port1           
        }
    };
#endif//HAL_USE_DEVICE_ENCODER 
    
    
#ifdef HAL_USE_DEVICE_ETHTRANSCEIVER
    extern const hal_device_ethtransceiver_hid_brdcfg_t hal_brdcfg_device_ethtransceiver__theconfig =
    {
        .supported      = hal_true,
        .devcfg         =
        {
            .targetphymode      = HAL_ETH_PHYMODE_THEONE2USE,
            .chipif             =
            {   // use the generic chip 
                .init           = (hal_res_fp_voidp_t)hal_chip_generic_ethtransceiver_init,
                .initpar        = NULL,
                .config         = hal_chip_generic_ethtransceiver_configure, 
                .getphymode     = hal_chip_generic_ethtransceiver_getphymode                
            }
         }
    };   
#endif//HAL_USE_DEVICE_ETHTRANSCEIVER

    
#ifdef  HAL_USE_DEVICE_GYROSCOPE
    extern const hal_device_gyroscope_hid_brdcfg_t hal_brdcfg_device_gyroscope__theconfig =
    {
        .supported_mask         = (0 << hal_gyroscope_port1),
        .devcfg         =
        {
            {   // hal_gyroscope_port1
                .chipif             =      
                {   // use ... nothing
                    .init       = NULL,
                    .initpar    = NULL,
                    .read       = NULL
                }                    
            }
         }
    };          
#endif//HAL_USE_DEVICE_GYROSCOPE

    
#ifdef HAL_USE_DEVICE_LED
    extern const hal_device_led_hid_brdcfg_t hal_brdcfg_device_led__theconfig =
    {
        .supported_mask             = (1 << hal_led0) | (1 << hal_led1) | (1 << hal_led2) | (1 << hal_led3), // only 4 leds
        .value_on                   = hal_gpio_valHIGH,
        .value_off                  = hal_gpio_valLOW,
        .gpiocfg                    =
        {
            { .port = hal_gpio_portD,    .pin = hal_gpio_pin7,    .dir = hal_gpio_dirOUT,  .speed = hal_gpio_speed_low }, // hal_led0
            { .port = hal_gpio_portD,    .pin = hal_gpio_pin13,   .dir = hal_gpio_dirOUT,  .speed = hal_gpio_speed_low }, // hal_led1
            { .port = hal_gpio_portD,    .pin = hal_gpio_pin3,    .dir = hal_gpio_dirOUT,  .speed = hal_gpio_speed_low }, // hal_led2
            { .port = hal_gpio_portD,    .pin = hal_gpio_pin4,    .dir = hal_gpio_dirOUT,  .speed = hal_gpio_speed_low }, // hal_led3
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirNONE, .speed = hal_gpio_speed_low }, // hal_led4
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirNONE, .speed = hal_gpio_speed_low }, // hal_led5
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirNONE, .speed = hal_gpio_speed_low }, // hal_led6
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirNONE, .speed = hal_gpio_speed_low }  // hal_led7
          //{ .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirNONE, .speed = hal_gpio_speed_low }
        }
    };
#endif//HAL_USE_DEVICE_LED
    

#ifdef HAL_USE_DEVICE_MUX
    extern const hal_device_mux_hid_brdcfg_t hal_brdcfg_device_mux__theconfig =
    {
        .supported_mask             = (0 << hal_mux_port1) | (0 << hal_mux_port2) | (0 << hal_mux_port3) | (0 << hal_mux_port4), 
        .gpio_sel0                  =
        {
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default }            
        },
        .gpio_sel1                  =
         {
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default }            
        }, 
        .gpio_enable                =
        {
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default }            
        },    
    };
#endif//HAL_USE_DEVICE_MUX   

    
#ifdef HAL_USE_DEVICE_SWITCH
    extern const hal_device_switch_hid_brdcfg_t hal_brdcfg_device_switch__theconfig =
    {
        .supported      = hal_false,
        .devcfg         =
        {
            .targetphymode  = HAL_ETH_PHYMODE_THEONE2USE,
            .chipif         =
            {
                .init       = NULL,
                .initpar    = NULL,
                .config     = NULL,          
            }
        }
    };
#endif//HAL_USE_DEVICE_SWITCH


#ifdef  HAL_USE_DEVICE_TERMOMETER 
    extern const hal_device_termometer_hid_brdcfg_t hal_brdcfg_device_termometer__theconfig =
    {
        .supported_mask         = 0x00,
        .devcfg         =
        {
            {   // hal_termometer_port1
                .chipif             =      
                {   // use ... nothing
                    .init       = NULL,
                    .initpar    = NULL,
                    .read       = NULL
                }                    
            }
         }
    };           
#endif//HAL_USE_DEVICE_TERMOMETER
  



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern uint32_t hal_brdcfg_chips__getsize(const hal_base_cfg_t *cfg)
{
    uint32_t retval = 0;
    
#ifdef  HAL_USE_CHIP_GENERIC_ETHTRANSCEIVER
    retval += hal_chip_generic_ethtransceiver_hid_getsize(cfg);
#endif//HAL_USE_CHIP_GENERIC_ETHTRANSCEIVER
    
#ifdef  HAL_USE_CHIP_ST_L3G4200D
    retval += hal_chip_st_l3g4200d_hid_getsize(cfg);
#endif//HAL_USE_CHIP_ST_L3G4200D    

#ifdef  HAL_USE_CHIP_ST_LIS3DH
    retval += hal_chip_st_lis3dh_hid_getsize(cfg);
#endif//HAL_USE_CHIP_ST_LIS3DH        

#ifdef  HAL_USE_CHIP_MICREL_KS8893
    retval += hal_chip_micrel_ks8893_hid_getsize(cfg);
#endif//HAL_USE_CHIP_MICREL_KS8893        

#ifdef  HAL_USE_CHIP_XX_EEPROM
    retval += hal_chip_xx_eeprom_hid_getsize(cfg);
#endif//HAL_USE_CHIP_XX_EEPROM          
    
    return(retval);
}


extern hal_result_t hal_brdcfg_chips__setmem(const hal_base_cfg_t *cfg, uint32_t *memory)
{
   if(0 == hal_brdcfg_chips__getsize(cfg))
   {
        return(hal_res_OK);
   }
   
   if(NULL == memory)
   {
       hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
       return(hal_res_NOK_generic);
   }

#ifdef  HAL_USE_CHIP_GENERIC_ETHTRANSCEIVER
    if(hal_res_OK != hal_chip_generic_ethtransceiver_hid_setmem(cfg, memory))
    {
        return(hal_res_NOK_generic);
    }
    memory += hal_chip_generic_ethtransceiver_hid_getsize(cfg)/4;   
#endif//HAL_USE_CHIP_GENERIC_ETHTRANSCEIVER
    
#ifdef  HAL_USE_CHIP_ST_L3G4200D
    if(hal_res_OK != hal_chip_st_l3g4200d_hid_setmem(cfg, memory))
    {
        return(hal_res_NOK_generic);
    }
    memory += hal_chip_st_l3g4200d_hid_getsize(cfg)/4;       
#endif//HAL_USE_CHIP_ST_L3G4200D    

#ifdef  HAL_USE_CHIP_ST_LIS3DH
    if(hal_res_OK != hal_chip_st_lis3dh_hid_setmem(cfg, memory))
    {
        return(hal_res_NOK_generic);
    }
    memory += hal_chip_st_lis3dh_hid_getsize(cfg)/4;       
#endif//HAL_USE_CHIP_ST_LIS3DH        

#ifdef  HAL_USE_CHIP_MICREL_KS8893
    if(hal_res_OK != hal_chip_micrel_ks8893_hid_setmem(cfg, memory))
    {
        return(hal_res_NOK_generic);
    }
    memory += hal_chip_micrel_ks8893_hid_getsize(cfg)/4;       
#endif//HAL_USE_CHIP_MICREL_KS8893        

#ifdef  HAL_USE_CHIP_XX_EEPROM
    if(hal_res_OK != hal_chip_xx_eeprom_hid_setmem(cfg, memory))
    {
        return(hal_res_NOK_generic);
    }
    memory += hal_chip_xx_eeprom_hid_getsize(cfg)/4;       
#endif//HAL_USE_CHIP_XX_EEPROM          
    
    
    return(hal_res_OK);  
}


extern uint32_t hal_brdcfg_extbrds__getsize(const hal_base_cfg_t *cfg)
{
    uint32_t retval = 0;
    
#ifdef  HAL_USE_EXTBRD_KEIL_MCBQVGA
    retval += hal_extbrd_keil_mcbqvga_hid_getsize(cfg);
#endif//HAL_USE_EXTBRD_KEIL_MCBQVGA
    
    return(retval);
}


extern hal_result_t hal_brdcfg_extbrds__setmem(const hal_base_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
   if(NULL == memory)
   {
       hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
       return(hal_res_NOK_generic);
   }

#ifdef  HAL_USE_EXTBRD_KEIL_MCBQVGA
    if(hal_res_OK != hal_extbrd_keil_mcbqvga_hid_setmem(cfg, memory))
    {
        return(hal_res_NOK_generic);
    }
    memory += hal_extbrd_keil_mcbqvga_hid_getsize(cfg)/4;   
#endif//HAL_USE_EXTBRD_KEIL_MCBQVGA
    
    
    return(hal_res_OK);  
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


