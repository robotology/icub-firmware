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
    
/* @file       hal_brdcfg_ems004.c
	@brief      This file implements low level functions which depend on board.
	@author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       10/04/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"
  
// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "hal_mpu_stm32xx_include.h"

#include "hal.h"
#include "stdlib.h"
#include "string.h"
#include "hal_base.h"
#include "hal_base_hid.h"
#include "hal_mpu_eth_hid.h"
#include "hal_eeprom.h"
#include "hal_timer.h"
#include "hal_i2c.h"
#include "hal_watchdog.h"

#include "hal_switch.h"
#include "hal_device_ethtransceiver_hid.h"

#include "hal_chip_xx_eeprom_hid.h"
#include "hal_chip_st_l3g4200d_hid.h"
#include "hal_chip_st_lis3dh_hid.h"
#include "hal_chip_micrel_ks8893_hid.h"
#include "hal_chip_generic_ethtransceiver_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_brdcfg_ems004.h"


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


#ifdef HAL_USE_SPI4ENCODER
    //- SPI1 chip selecet
    #define HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_PORT_CS			 	GPIOB
//    #define HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_PORT_CS_CLOCK			RCC_APB2Periph_GPIOB
    #define HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_ENA_ENCODER_0			GPIO_Pin_10
    #define HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_ENA_ENCODER_1			GPIO_Pin_1
    //- SPI2 chip selecet
    #define HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_PORT_CS			 	GPIOB
//    #define HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_PORT_CS_CLOCK			RCC_APB2Periph_GPIOB
    #define HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_ENA_ENCODER_0			GPIO_Pin_10
    #define HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_ENA_ENCODER_1			GPIO_Pin_1
    //- SPI3 chip selecet
    #define HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_PORT_CS			 	GPIOB
//    #define HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_PORT_CS_CLOCK			RCC_APB2Periph_GPIOB
    #define HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_ENA_ENCODER_0			GPIO_Pin_10
    #define HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_ENA_ENCODER_1			GPIO_Pin_1
    // SPI commnds for enable/disable chipselect
    #define HAL_BRDCFG_SPI4ENCODER__SPI_CS_ENA(spix, e)	 	((hal_SPI4ENCODER_ENCDATA_GET(spix)).encoder_gpio_port->BRR |= hal_SPI4ENCODER_ENCDATA_CS_GET(spix, e).BRR_reg);
    #define HAL_BRDCFG_SPI4ENCODER__SPI_CS_DISA(spix, e)	((hal_SPI4ENCODER_ENCDATA_GET(spix)).encoder_gpio_port->BSRR  |= hal_SPI4ENCODER_ENCDATA_CS_GET(spix, e).BSRR_reg);   
#endif//HAL_USE_SPI4ENCODER



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


#ifdef  HAL_USE_CAN
    extern const hal_can_hid_brdcfg_t hal_brdcfg_can__theconfig =
    {
        .supported_mask     = (1 << hal_can_port1) | (1 << hal_can_port2),
        .gpio_rx            =
        {
            {   // hal_can_port1
                .port     = hal_gpio_portD,
                .pin      = hal_gpio_pin0,        
                .dir      = hal_gpio_dirALT,
                .speed    = hal_gpio_speed_default
            },
            {   // hal_can_port2
                .port     = hal_gpio_portB, 
                .pin      = hal_gpio_pin5,        
                .dir      = hal_gpio_dirALT,
                .speed    = hal_gpio_speed_default        
            } 
        },
        .gpio_tx            =
        {   
            {   // hal_can_port1
                .port     = hal_gpio_portD, 
                .pin      = hal_gpio_pin1,        
                .dir      = hal_gpio_dirALT,
                .speed    = hal_gpio_speed_default
            },           
            {   // hal_can_port2
                .port     = hal_gpio_portB, 
                .pin      = hal_gpio_pin6,        
                .dir      = hal_gpio_dirALT,
                .speed    = hal_gpio_speed_default        
            }
        }         
    };
#endif//HAL_USE_CAN

#ifdef  HAL_USE_CRC
    extern const hal_crc_hid_brdcfg_t hal_brdcfg_crc__theconfig =
    {
        .supported_mask     = (1 << hal_crc0) | (1 << hal_crc1),
    };
#endif//HAL_USE_CRC


#ifdef  HAL_USE_ETH    
    extern const hal_eth_hid_brdcfg_t hal_brdcfg_eth__theconfig     =
    {
        .supported          = hal_true,
        .mif                = hal_eth_mif_rmii,
        .gpio_mif.rmii      =
        {
            .ETH_RMII_REF_CLK   = { .port = hal_gpio_portA, .pin = hal_gpio_pin1,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max },
            .ETH_RMII_TX_EN     = { .port = hal_gpio_portB, .pin = hal_gpio_pin11,  .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max },
            .ETH_RMII_TXD0      = { .port = hal_gpio_portB, .pin = hal_gpio_pin12,  .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max },
            .ETH_RMII_TXD1      = { .port = hal_gpio_portB, .pin = hal_gpio_pin13,  .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max },
            .ETH_RMII_CRS_DV    = { .port = hal_gpio_portA, .pin = hal_gpio_pin7,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max },
            .ETH_RMII_RXD0      = { .port = hal_gpio_portC, .pin = hal_gpio_pin4,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max },
            .ETH_RMII_RXD1      = { .port = hal_gpio_portC, .pin = hal_gpio_pin5,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max }       
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
            0xffff,     // port h
            0xffff      // port i
        }
    };
#endif//HAL_USE_GPIO

#ifdef  HAL_USE_I2C
    extern const hal_i2c_hid_brdcfg_t hal_brdcfg_i2c__theconfig =
    {
        .supported_mask         = (1 << hal_i2c_port1),
        .gpio_scl               =
        {
            { .port = hal_gpio_portB,    .pin = hal_gpio_pin8,    .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default } 
        },
        .gpio_sda               =
        {
            { .port = hal_gpio_portB,    .pin = hal_gpio_pin9,    .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default },
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_default }         
        }
    };
#endif//HAL_USE_I2C


#ifdef  HAL_USE_SPI

    extern const hal_spi_hid_brdcfg_t hal_brdcfg_spi__theconfig =
    {
        .tbd = 1234567890ciao;
    };
    
    extern const uint8_t hal_brdcfg_spi__supported_mask             = (1 << hal_spi_port1) | (1 << hal_spi_port2) | (1 << hal_i2c_port3);

    extern const hal_gpio_cfg_t hal_brdcfg_spi__miso[]      =
    {
        {   // spi1
            .port     = hal_gpio_portA, 
            .pin      = hal_gpio_pin6,        
            .dir      = hal_gpio_dirALT,
            .speed    = hal_gpio_speed_default
        },
        
        {   // spi2
            .port     = hal_gpio_portC, 
            .pin      = hal_gpio_pin2,        
            .dir      = hal_gpio_dirALT,
            .speed    = hal_gpio_speed_default        
        },
        {   // spi3
            .port     = hal_gpio_portB, 
            .pin      = hal_gpio_pin4,        
            .dir      = hal_gpio_dirALT,
            .speed    = hal_gpio_speed_default        
        }        
    };
    extern const hal_gpio_cfg_t hal_brdcfg_spi__mosi[]      =
    {
        {   // spi1
            .port     = hal_gpio_portB, 
            .pin      = hal_gpio_pin5,        
            .dir      = hal_gpio_dirALT,
            .speed    = hal_gpio_speed_default
        },
        
        {   // spi2
            .port     = hal_gpio_portC, 
            .pin      = hal_gpio_pin3,        
            .dir      = hal_gpio_dirALT,
            .speed    = hal_gpio_speed_default        
        },
        {   // spi3
            .port     = hal_gpio_portC, 
            .pin      = hal_gpio_pin12,        
            .dir      = hal_gpio_dirALT,
            .speed    = hal_gpio_speed_default        
        }     
    };
    extern const hal_gpio_cfg_t hal_brdcfg_spi__sck[]      =
    {
        {   // spi1
            .port     = hal_gpio_portA, 
            .pin      = hal_gpio_pin5,        
            .dir      = hal_gpio_dirALT,
            .speed    = hal_gpio_speed_default
        },
        
        {   // spi2
            .port     = hal_gpio_portB, 
            .pin      = hal_gpio_pin10,        
            .dir      = hal_gpio_dirALT,
            .speed    = hal_gpio_speed_default        
        },
        {   // spi3
            .port     = hal_gpio_portC, 
            .pin      = hal_gpio_pin10,        
            .dir      = hal_gpio_dirALT,
            .speed    = hal_gpio_speed_default        
        }     
    };    
#endif//HAL_USE_SPI

#ifdef  HAL_USE_SYS
    extern const hal_sys_hid_brdcfg_t hal_brdcfg_sys__theconfig  =
    {
        .keepinternalclockatstartup  = hal_false,      // false is the preferred mode
        .clock                      =
        {
            .refclock           = hal_sys_refclock_pll_on_external_osc,  
            .intclockspeed      =  16000000,
            .extclockspeed      =  50000000,
            .targetspeeds       =
            {   
                .cpu                = 168000000,
                .fastbus            =  84000000,
                .slowbus            =  42000000
            },
            .pllcfg             =
            {   // pllfreq = n*(source/m), speedcpu = pllfreq/p, 48mhz = pllfreq/q
                .m                  =  25, 
                .n                  = 168,
                .p                  =   2,
                .q                  =   7
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




#ifdef  HAL_USE_WATCHDOG
    extern const hal_watchdog_hid_brdcfg_t hal_brdcfg_watchdog__theconfig =
    {
        .supported_mask     = (1 << hal_watchdog_normal) | (1 << hal_watchdog_window)
    };       
#endif//HAL_USE_WATCHDOG



#ifdef HAL_USE_DEVICE_LED
    extern const hal_device_led_hid_brdcfg_t hal_brdcfg_device_led__theconfig =
    {
        .supported_mask             = 0x3F, // only 6 leds ...
        .value_on                   = hal_gpio_valLOW,
        .value_off                  = hal_gpio_valHIGH,
        .gpiocfg                    =
        {
            { .port = hal_gpio_portE,    .pin = hal_gpio_pin8,    .dir = hal_gpio_dirOUT,  .speed = hal_gpio_speed_low }, // hal_led0 - red
            { .port = hal_gpio_portE,    .pin = hal_gpio_pin10,   .dir = hal_gpio_dirOUT,  .speed = hal_gpio_speed_low }, // hal_led1 - green
            { .port = hal_gpio_portE,    .pin = hal_gpio_pin12,   .dir = hal_gpio_dirOUT,  .speed = hal_gpio_speed_low }, // hal_led2 - yellow
            { .port = hal_gpio_portE,    .pin = hal_gpio_pin15,   .dir = hal_gpio_dirOUT,  .speed = hal_gpio_speed_low }, // hal_led3 - orange
            { .port = hal_gpio_portD,    .pin = hal_gpio_pin11,   .dir = hal_gpio_dirOUT,  .speed = hal_gpio_speed_low }, // hal_led4 - can1
            { .port = hal_gpio_portB,    .pin = hal_gpio_pin7,    .dir = hal_gpio_dirOUT,  .speed = hal_gpio_speed_low }, // hal_led5 - can2
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirNONE, .speed = hal_gpio_speed_low }, // hal_led6
            { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirNONE, .speed = hal_gpio_speed_low }  // hal_led7
          //{ .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE, .dir = hal_gpio_dirNONE, .speed = hal_gpio_speed_low }
        }
    };
#endif//HAL_USE_DEVICE_LED


#ifdef  HAL_USE_DEVICE_CANTRANSCEIVER    
    static hal_result_t s_hal_device_cantransceiver_simple_init(hal_can_port_t port, void* initpar);
    static hal_result_t s_hal_device_cantransceiver_simple_enable(hal_can_port_t port);
    static hal_result_t s_hal_device_cantransceiver_simple_disable(hal_can_port_t port);
    
    static const hal_gpio_cfg_t s_hal_device_cantransceiver_enable_pin[2] = 
    {
            { .port = hal_gpio_portB,    .pin = hal_gpio_pin14,   .dir = hal_gpio_dirOUT,  .speed = hal_gpio_speed_low },
            { .port = hal_gpio_portB,    .pin = hal_gpio_pin15,   .dir = hal_gpio_dirOUT,  .speed = hal_gpio_speed_low }
    };
    
    
    static hal_result_t s_hal_device_cantransceiver_simple_init(hal_can_port_t port, void* initpar)
    {
        hal_gpio_configure(s_hal_device_cantransceiver_enable_pin[(uint8_t)port], NULL);       
        s_hal_device_cantransceiver_simple_disable(port);        
        return(hal_res_OK);
    }

    static hal_result_t s_hal_device_cantransceiver_simple_enable(hal_can_port_t port)
    {  
        const hal_gpio_cfg_t* pincfg = &s_hal_device_cantransceiver_enable_pin[(uint8_t)port];       
        hal_gpio_setval(pincfg->port, pincfg->pin, hal_gpio_valLOW);
        return(hal_res_OK);
    }

    static hal_result_t s_hal_device_cantransceiver_simple_disable(hal_can_port_t port)
    {   
        const hal_gpio_cfg_t* pincfg = &s_hal_device_cantransceiver_enable_pin[(uint8_t)port];       
        hal_gpio_setval(pincfg->port, pincfg->pin, hal_gpio_valHIGH);
        return(hal_res_OK);
    }
    
    extern const hal_device_cantransceiver_hid_brdcfg_t hal_brdcfg_device_cantransceiver__theconfig =
    {
        .supported_mask     =  (1 << hal_can_port1) | (1 << hal_can_port2),
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
    extern const hal_device_display_hid_brdcfg_t hal_brdcfg_device_encoder__theconfig =
    {
        .supported      = hal_false,
        .somethingelse  = is-yet-to-be-done-in-here;
    };
#endif//HAL_USE_DEVICE_DISPLAY

  
#ifdef  HAL_USE_DEVICE_EEPROM
    static const hal_gpio_val_t hal_brdcfg_device_eeprom__wp_gpio_enableval    = hal_gpio_valHIGH;      // write protection is on value high                          
    static const hal_gpio_val_t hal_brdcfg_device_eeprom__wp_gpio_disableval   = hal_gpio_valLOW;       // write protection is on value high
    static const hal_gpio_cfg_t hal_brdcfg_device_eeprom__wp_gpio_cfg          =
    {
        .port               = hal_gpio_portD,
        .pin                = hal_gpio_pin10,
        .dir                = hal_gpio_dirOUT,
        .speed              = hal_gpio_speed_high
    };

    static hal_result_t s_hal_brdcfg_device_eeprom__wp_init(void)
    {
        return(hal_gpio_configure(hal_brdcfg_device_eeprom__wp_gpio_cfg, NULL));
    }

    static hal_result_t s_hal_brdcfg_device_eeprom__wp_enable(void)
    {
        return(hal_gpio_setval(hal_brdcfg_device_eeprom__wp_gpio_cfg.port, hal_brdcfg_device_eeprom__wp_gpio_cfg.pin, hal_brdcfg_device_eeprom__wp_gpio_enableval));
    }

    static hal_result_t s_hal_brdcfg_device_eeprom__wp_disable(void)
    {
        return(hal_gpio_setval(hal_brdcfg_device_eeprom__wp_gpio_cfg.port, hal_brdcfg_device_eeprom__wp_gpio_cfg.pin, hal_brdcfg_device_eeprom__wp_gpio_disableval));
    }

    
    // used by hal_brdcfg_device_eeprom__theconfig
    static const hal_chip_xx_eeprom_cfg_t s_initpar_eeprom_i2c_01 =
    {
        .chipcfg    =
        {
            .chip           = hal_chip_xx_eeprom_chip_atmel_at24c1024b,
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
        .supported_mask     =  (0 << hal_eeprom_emulatedflash) | (1 << hal_eeprom_i2c_01) | (0 << hal_eeprom_i2c_02),
        .devcfg             =
        {            
            {   // hal_device_eeprom_emulatedflash 
                .flashemul  =
                {
                    .baseaddress    = 0x08000000,
                    .totalsize      = 0
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
                    {   // use the chip xx_eeprom
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


#ifdef  HAL_USE_CHIP_MICREL_KS8893     
    static hal_result_t s_hal_brdcfg_device_switch__extclock_init(void);    
    static const hal_chip_micrel_ks8893_cfg_t s_micrel_ks8893_cfg =
    {
        .i2cport        = hal_i2c_port1,
        .resetpin       =
        {
            .port = hal_gpio_portB, 
            .pin = hal_gpio_pin2,   
            .dir = hal_gpio_dirOUT, 
            .speed = hal_gpio_speed_low 
        },
        .extclockinit   = s_hal_brdcfg_device_switch__extclock_init
    }; 
#endif//HAL_USE_CHIP_MICREL_KS8893
    
    
#ifdef HAL_USE_DEVICE_ETHTRANSCEIVER
    extern const hal_device_ethtransceiver_hid_brdcfg_t hal_brdcfg_device_ethtransceiver__theconfig =
    {
        .supported      = hal_true,
        .devcfg         =
        {
            .targetphymode      = HAL_ETH_PHYMODE_THEONE2USE,
            .chipif             =
            {   // use the micrel 
                .init           = (hal_res_fp_voidp_t)hal_chip_micrel_ks8893_init,
                .initpar        = (void*)&s_micrel_ks8893_cfg,
                .config         = hal_chip_micrel_ks8893_configure, 
                .getphymode     = hal_chip_micrel_ks8893_mii_getphymode                
            }
         }
    };   
#endif//HAL_USE_DEVICE_ETHTRANSCEIVER




#ifdef HAL_USE_DEVICE_SWITCH
    extern const hal_device_switch_hid_brdcfg_t hal_brdcfg_device_switch__theconfig =
    {
        .supported      = hal_true,
        .devcfg         =
        {
            .targetphymode  = HAL_ETH_PHYMODE_THEONE2USE,
            .chipif         =
            {
                .init       = (hal_res_fp_voidp_t)hal_chip_micrel_ks8893_init,
                .initpar    = (void*)&s_micrel_ks8893_cfg,
                .config     = hal_chip_micrel_ks8893_configure,          
            }
        }
    };
#endif//HAL_USE_DEVICE_SWITCH


 
#ifdef  HAL_USE_DEVICE_ACCELEROMETER   
    extern const hal_device_accelerometer_hid_brdcfg_t hal_brdcfg_device_accelerometer__theconfig =
    {
        .supported_mask         = 0x1,
        .devcfg         =
        {
            {   // hal_accelerometer_port1
                .chipif             =      
                {   // use the lis3dh chip by st
                    .init       = (hal_res_fp_voidp_t)hal_chip_st_lis3dh_init,
                    .initpar    = NULL,
                    .read       = hal_chip_st_lis3dh_accel_get  
                }                    
            }
         }
    };       
#endif//HAL_USE_DEVICE_ACCELEROMETER
    


#ifdef  HAL_USE_SENSOR_ENCODER
    extern const hal_sensor_encoder_hid_brdcfg_t hal_brdcfg_sensor_encoder__theconfig =
    {
        .supported_mask     = 0,
        .something          = yet-to-be-done-in-here
    };
#endif//HAL_USE_SENSOR_ENCODER
  

#ifdef  HAL_USE_DEVICE_GYROSCOPE
    extern const hal_device_gyroscope_hid_brdcfg_t hal_brdcfg_device_gyroscope__theconfig =
    {
        .supported_mask         = 0x1,
        .devcfg         =
        {
            {   // hal_gyroscope_port1
                .chipif             =      
                {   // use the l3g4200d chip by st
                    .init       = (hal_res_fp_voidp_t)hal_chip_st_l3g4200d_init,
                    .initpar    = NULL,
                    .read       = hal_chip_st_l3g4200d_angrate_get
                }                    
            }
         }
    };          
#endif//HAL_USE_DEVICE_GYROSCOPE
    
    
#ifdef  HAL_USE_DEVICE_TERMOMETER 
    extern const hal_device_termometer_hid_brdcfg_t hal_brdcfg_device_termometer__theconfig =
    {
        .supported_mask         = 0x1,
        .devcfg         =
        {
            {   // hal_termometer_port1
                .chipif             =      
                {   // use the l3g4200d chip by st
                    .init       = (hal_res_fp_voidp_t)hal_chip_st_l3g4200d_init,
                    .initpar    = NULL,
                    .read       = hal_chip_st_l3g4200d_temp_get
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
    // no memory needed
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



#ifdef HAL_USE_SPI4ENCODER

/*Note: if there will be some speed problems, you can try to transform  hal_brdcfg_spi4encoder__encoder_enable and  hal_brdcfg_spi4encoder__encoder_disable as inline function,
but pay attention with the project with scatter file.probably it will give you trouble*/

extern void hal_brdcfg_spi4encoder__encoder_enable(hal_spi_port_t spix, hal_spi_mux_t e)
{
	HAL_BRDCFG_SPI4ENCODER__SPI_CS_ENA(spix, e)
}

extern void hal_brdcfg_spi4encoder__encoder_disable(hal_spi_port_t spix, hal_spi_mux_t e)
{
	HAL_BRDCFG_SPI4ENCODER__SPI_CS_DISA(spix, e)
}

extern void hal_brdcfg_spi4encoder__chipSelect_init(hal_spi_port_t spix )
{
    GPIO_InitTypeDef  GPIO_InitStructure;

	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 //Output push-pull mode
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;


//1) init data struct and GPIO regiter
	switch(spix)
	{
		case hal_spi_port1:
		{
			hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[0].BRR_reg = HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_ENA_ENCODER_0; //pin che devono valere zero
			hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[0].BSRR_reg = HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_ENA_ENCODER_0;	//pin che devono valere 1
			
			hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[1].BRR_reg = HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_ENA_ENCODER_1;
			hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[1].BSRR_reg = HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_ENA_ENCODER_1;
			hal_SPI4ENCODER_ENCDATA_GET(spix).encoder_gpio_port = HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_PORT_CS;
			
		
			GPIO_InitStructure.GPIO_Pin = HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_ENA_ENCODER_0 | HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_ENA_ENCODER_1;
			GPIO_Init(HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_PORT_CS, &GPIO_InitStructure);
		
		} break;

		case hal_spi_port2:
		{
			hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[0].BRR_reg = HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_ENA_ENCODER_0; //pin che devono valere zero
			hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[0].BSRR_reg = HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_ENA_ENCODER_0;	//pin che devono valere 1
			
			hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[1].BRR_reg = HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_ENA_ENCODER_1;
			hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[1].BSRR_reg = HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_ENA_ENCODER_1;
			hal_SPI4ENCODER_ENCDATA_GET(spix).encoder_gpio_port = HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_PORT_CS;
			
		
			GPIO_InitStructure.GPIO_Pin = HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_ENA_ENCODER_0 | HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_ENA_ENCODER_1;
			GPIO_Init(HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_PORT_CS, &GPIO_InitStructure);
		
		} break;
        
		case hal_spi_port3:
		{
			
			hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[0].BRR_reg = HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_ENA_ENCODER_0; //pin che devono valere zero
			hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[0].BSRR_reg = HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_ENA_ENCODER_0;	//pin che devono valere 1
			
			hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[1].BRR_reg = HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_ENA_ENCODER_1;
			hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[1].BSRR_reg = HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_ENA_ENCODER_1;
			hal_SPI4ENCODER_ENCDATA_GET(spix).encoder_gpio_port = HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_PORT_CS;
			
		
			GPIO_InitStructure.GPIO_Pin = HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_ENA_ENCODER_0 | HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_ENA_ENCODER_1;
			GPIO_Init(HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_PORT_CS, &GPIO_InitStructure);
		
		} break;
	}	

//2) init unused data struct fields
	hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[2].BRR_reg = 0;
	hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[2].BSRR_reg = 0;
	
	hal_SPI4ENCODER_ENCDATA_GET(spix).cs_pin_ena_slave = 0;	//non usato	 per questa scheda


//3) set pin high value
	hal_brdcfg_spi4encoder__encoder_disable(spix, (hal_spi_mux_t)0);
	hal_brdcfg_spi4encoder__encoder_disable(spix, (hal_spi_mux_t)1);

}

#endif//HAL_USE_SPI4ENCODER



#ifdef  HAL_USE_DEVICE_SWITCH

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


static hal_result_t s_hal_brdcfg_device_switch__extclock_init(void)
{
    s_hal_brdcfg_device_switch__mco_initialise();  
    return(hal_res_OK);    
}

#endif//HAL_USE_DEVICE_SWITCH


//#ifdef HAL_USE_DEVICE_SWITCH  
//
// #if 0
// static void hal_brdcfg_device_switch__mco1_init(void)
// {
//     // configure pa8 af, push pull, 50mhz
//     // enable system configuration controller clock
//     RCC->APB2ENR    |= (1 << 14);  
//     // clocks port port a
//     RCC->AHB1ENR    |= 0x00000001;

//     GPIOA->MODER   &= ~0x00030000;              // reset pa8
//     GPIOA->MODER   |=  0x00020000;              // alternate function
//     GPIOA->OTYPER  &= ~0x00000100;              // output push-pull (reset state)  
//     GPIOA->OSPEEDR |=  0x00030000;              // slew rate as 100MHz pin (3) or 50mhz (2)
//     GPIOA->PUPDR   &= ~0x00030000;              // no pull up, pull down

//     GPIOA->AFR[1]  &= ~0x0000000F;
//     GPIOA->AFR[1]  |=  0x00000000;              // AF0 (system) 

// }
// #endif

// static void s_hal_brdcfg_device_switch__mco2_init(void)
// {
//     #define SLR100MHZ               0x000C0000
//     #define SLR050MHZ               0x00080000
//     // configure pc9 af, push pull, 50mhz
//     // enable system configuration controller clock
//     RCC->APB2ENR    |= (1 << 14);  
//     // clocks port port c
//     RCC->AHB1ENR    |= 0x00000004;

//     GPIOC->MODER    &= ~0x000C0000;              // reset pc9
//     GPIOC->MODER    |=  0x00080000;              // alternate function
//     GPIOC->OTYPER   &= ~0x00000200;              // output push-pull (reset state)  
//     GPIOC->OSPEEDR  |=  SLR100MHZ;               // slew rate as 100MHz pin (0x000C0000) or 50mhz (0x00080000)
//     GPIOC->PUPDR    &= ~0x000C0000;              // no pull up, pull down

//     GPIOC->AFR[1]   &= ~0x000000F0;
//     GPIOC->AFR[1]   |=  0x00000000;              // AF0 (system) 
// }



// static void s_hal_brdcfg_device_switch__mco_initialise(void)
// {    
// #define HAL_SWITCH_USE_HSE   
//     
//     s_hal_brdcfg_device_switch__mco2_init();

// #if     defined(HAL_SWITCH_USE_PLLI2S)    
//     RCC_PLLI2SCmd(DISABLE);
//     RCC_PLLI2SConfig(200, 2); // 50mhz: 1mhz*200/2 = 100.  then we divide by 2 again
//     RCC_PLLI2SCmd(ENABLE);
//     // wait until it is ready
//     while(RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == RESET);
//     // connect mco2 with plli2s divided by 2
//     RCC_MCO2Config(RCC_MCO2Source_PLLI2SCLK, RCC_MCO2Div_2);
// #elif   defined(HAL_SWITCH_USE_HSE)
//  
//     // connect mco2 with hse divided by 1
//     RCC_MCO2Config(RCC_MCO2Source_HSE, RCC_MCO2Div_1);
// #endif    
//     
//     
//     //hal_eth_hid_rmii_refclock_init();
// }

// static void hal_brdcfg_device_switch__rmii_init(void)
// {
//     hal_eth_hid_rmii_prepare();
//     hal_eth_hid_rmii_rx_init(); 
//     hal_eth_hid_rmii_tx_init(); 
//     hal_eth_hid_rmii_refclock_init();  
//     hal_eth_hid_smi_init();    
// }

// #ifdef  HAL_USE_CHIP_MICREL_KS8893
// static hal_result_t s_hal_brdcfg_device_switch__extclock_init(void)
// {
//     // --- mco2: PC9
//     s_hal_brdcfg_device_switch__mco_initialise();  
//     return(hal_res_OK);    
// }
// #endif//#ifdef  HAL_USE_CHIP_MICREL_KS8893

// extern hal_result_t hal_brdcfg_device_switch__chip_initialise(void* p)
// {
//     hal_result_t res = hal_res_OK;
//     
//     const hal_gpio_cfg_t hal_brdcfg_device_switch__gpio_reset = 
//     { 
//         .port = hal_gpio_portB, 
//         .pin = hal_gpio_pin2,   
//         .dir = hal_gpio_dirOUT, 
//         .speed = hal_gpio_speed_low 
//     }; 
//     
//     // --- mco2: PC9
//     s_hal_brdcfg_device_switch__mco_initialise();  
//     
//      // --- reset pin: PB2
//     hal_gpio_configure(hal_brdcfg_device_switch__gpio_reset, NULL);
//     hal_gpio_setval(hal_brdcfg_device_switch__gpio_reset.port, hal_brdcfg_device_switch__gpio_reset.pin, hal_gpio_valLOW);
//     hal_sys_delay(10*1000);
//     hal_gpio_setval(hal_brdcfg_device_switch__gpio_reset.port, hal_brdcfg_device_switch__gpio_reset.pin, hal_gpio_valHIGH);
//     hal_sys_delay(100);
//   
//     // proviamo anche questo ....
// //    hal_eth_hid_rmii_refclock_init();
//     
//     // --- mco: PC9
// //    hal_brdcfg_device_switch__mco_initialise();  
//     
//     #warning --> test on it
//     //hal_brdcfg_device_switch__rmii_init();
//     
// //    // --- i2c for communication 
// //    hal_i2c_init(hal_i2c_port1, NULL); // use default configuration    


//     res = hal_chip_micrel_ks8893_init(NULL);
//    
//     return(res);
// }

// static void s_hal_brdcfg_device_switch__mii_phymode_get(hal_eth_phymode_t* phymode)
// {
//     #define MIIHALFD    (1 << 6)
//     #define MII10MBS    (1 << 4)
//     uint8_t mux = 0;
//     uint8_t speed = 0;
//     uint8_t read = 0xFF; 
//     hal_i2c_regaddr_t regadr = {.numofbytes = 1, .bytes.one = 0};
//     regadr.bytes.one = 0x06;
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &read, 1);
//     if( (MIIHALFD & read) == MIIHALFD)
//     {
//         mux = 0;
//     }
//     else
//     {
//         mux = 1;
//     }
//     if( (MII10MBS & read) == MII10MBS)
//     {
//         speed = 0;
//     }
//     else
//     {
//         speed = 1;
//     }
//     if((0==mux)&&(0==speed))        *phymode = hal_eth_phymode_halfduplex10mbps;
//     else if((0==mux)&&(1==speed))   *phymode = hal_eth_phymode_halfduplex100mbps; 
//     else if((1==mux)&&(0==speed))   *phymode = hal_eth_phymode_fullduplex10mbps; 
//     else if((1==mux)&&(1==speed))   *phymode = hal_eth_phymode_fullduplex100mbps;          
//     
// }
 
// extern hal_result_t hal_brdcfg_device_switch__configure(hal_eth_phymode_t targetphymode, hal_eth_phymode_t* phymode)
// {
//     hal_result_t res = hal_res_NOK_generic;
//     
//     hal_eth_phymode_t phymode2use = targetphymode; //hal_eth_phymode_fullduplex100mbps;
//     const uint8_t fd100 = 0x60;
//     const uint8_t fd010 = 0x20;
//     uint8_t buff_write = 0x60; // FORCE FULL DUPLEX AND 100T
//     uint8_t buff_read = 0xFF; 
//     volatile uint32_t i = 1;
//     hal_i2c_regaddr_t regadr = {.numofbytes = 1, .bytes.one = 0};
//     
//     if(NULL != phymode)
//     {
//         phymode2use = *phymode;
//     }


//     regadr.bytes.one = 0x01;
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
//     if((buff_read&0x01))
//     {   // already initted. to be initted again must pass through a reset
//         s_hal_brdcfg_device_switch__mii_phymode_get(phymode);
//         return(hal_res_OK);
//     }
//     
//     
//     if(hal_eth_phymode_none == phymode2use)
//     {
//         if(NULL != phymode)
//         {
//             *phymode = hal_eth_phymode_none;    
//         }   
//         return(hal_res_OK);        
//     }


//     

// #if 0    
//     // configure mii
//     regadr.bytes.one = 0x06;
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
//     buff_write  = buff_read;
//     buff_write |= 0x10; 
//     hal_i2c_write(hal_i2c_port1, 0xBE, regadr, &buff_write, 1);
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
// #endif

//     // 1. configure  switch's ports 1 and 2
//     switch(phymode2use)
//     {
//         case hal_eth_phymode_auto:                  buff_write = 0x9F; break;
//         case hal_eth_phymode_halfduplex10mbps:      buff_write = 0x00; break;
//         case hal_eth_phymode_halfduplex100mbps:     buff_write = 0x40; break;
//         case hal_eth_phymode_fullduplex10mbps:      buff_write = 0x20; break;
//         case hal_eth_phymode_fullduplex100mbps:     buff_write = 0x60; break;
//         default:                                    buff_write = 0x00; break;
//     }    
//     
//     
//     // port 1
//     regadr.bytes.one = 0x1C;
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
//     hal_i2c_write(hal_i2c_port1, 0xBE, regadr, &buff_write, 1);
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
//      
//     // port 2 
//     regadr.bytes.one = 0x2C;
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
//     hal_i2c_write(hal_i2c_port1, 0xBE, regadr, &buff_write, 1);   
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);

//     // 2. start the switch
//     buff_write = 0x1;  
//     regadr.bytes.one = 0x01;    
//     hal_i2c_write(hal_i2c_port1, 0xBE, regadr, &buff_write, 1);
//     

//     // 3. read back to verify
//     regadr.bytes.one = 0x01;
//     hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
//     if(!(buff_read&0x01))
//     {
//         hal_base_hid_on_fatalerror(hal_fatalerror_runtimefault, "hal_brdcfg_device_switch__configure(): SWITCH not configured");
//     }
//     
//     s_hal_brdcfg_device_switch__mii_phymode_get(phymode);
//     
//     return(hal_res_OK);
//     
// }
//
//#endif//HAL_USE_DEVICE_SWITCH



#ifdef HAL_USE_ETH

// extern hal_bool_t hal_brdcfg_eth__phy_initialise(void)
// {
//     #warning --> in here we just init the switch ... put in reset mode, exit from reset, do the mco, init the i2c if not initted, etc. 
//     if(hal_false == hal_device_switch_initted_is())
//     {
//         hal_device_switch_init(NULL);
//     }
//     
//     return(hal_true);
// }

// extern void hal_brdcfg_eth__phy_configure(hal_eth_phymode_t *phymode)
// {
//     hal_eth_phymode_t target = hal_brdcfg_eth__phymode;
//  
// //    if((hal_eth_mux_auto == target.mux) || (hal_eth_mux_none == target.mux) || (hal_eth_speed_auto == target.speed) || (hal_eth_speed_none == target.speed))
//     if(hal_eth_phymode_none == target)
//     {
//         if(NULL != phymode)
//         {
//             *phymode  = hal_eth_phymode_none;
//         }   
//         return;        
//     }    
//     
//     hal_device_switch_start(&target);
//     
//     if(NULL != phymode)
//     {
//         *phymode  = target;     
//     }
// }

#endif//HAL_USE_ETH



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


