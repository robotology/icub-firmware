/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
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
  
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _HAL_BRDCFG_EMS001_H_
#define _HAL_BRDCFG_EMS001_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/* @file       hal_brdcfg_ems001.h
    @brief      This header file defines Interface Pins, eval-board depend macro and low level function prototypes.
    @author     valentina.gaggero@iit.it
    @date       11/16/2010
 **/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"

#ifdef HAL_USE_CAN
    #include "hal_can.h"
#endif//HAL_USE_CAN

#ifdef HAL_USE_GPIO
    #include "hal_gpio.h"
#endif//HAL_USE_GPIO

#ifdef HAL_USE_LED
    #include "hal_led.h"
#endif//HAL_USE_LED

#ifdef HAL_USE_SPI4ENCODER
    #include "hal_spi4encoder.h" 
#endif//HAL_USE_SPI4ENCODER

#ifdef HAL_USE_ETH
     #include "hal_eth.h" 
#endif //HAL_USE_ETH


#include "stm32f10x_i2c.h"



// - public #define  --------------------------------------------------------------------------------------------------

//#ifdef HAL_USE_CAN
//    // empty-section
//#endif//HAL_USE_CAN
//
//#ifdef HAL_USE_DISPLAY
//    // empty-section
//#endif//HAL_USE_DISPLAY  

#ifdef HAL_USE_I2C4HAL
    #define HAL_BRDCFG_I2C4HAL__PERIPHERAL                       I2C1    // i2c peripheral used by eeprom 01 and ...
#endif//HAL_USE_I2C4HAL  


#ifdef HAL_USE_SPI4ENCODER

    #define HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_PORT_CS_CLOCK			RCC_APB2Periph_GPIOD
    #define HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_PORT_CS_CLOCK			RCC_APB2Periph_GPIOD
    #define HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_PORT_CS_CLOCK			RCC_APB2Periph_GPIOD  

#endif//HAL_USE_SPI4ENCODER



#ifdef HAL_USE_ETH

    #define HAL_BRDCFG_ETH__PHY_DEVICE_NUM     2

#endif//HAL_USE_ETH




// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

#ifdef HAL_USE_CAN
    extern const uint8_t hal_brdcfg_can__supported_mask; //           = 0x03;
#endif//HAL_USE_CAN

#ifdef HAL_USE_CRC
    extern const uint8_t hal_brdcfg_crc__supported_mask; //           = 0x01;
#endif//HAL_USE_CRC

#ifdef HAL_USE_DISPLAY
    extern const uint8_t hal_brdcfg_display__supported_mask; //         = 0x00;
#endif//HAL_USE_DISPLAY

#ifdef HAL_USE_EEPROM
    extern const uint8_t hal_brdcfg_eeprom__supported_mask; //         = (1 << hal_eeprom_emulatedflash)||(1 << hal_eeprom_i2c_01);
    extern const uint32_t hal_brdcfg_eeprom__emflash_baseaddress;//    = 0x08000000;
    extern const uint32_t hal_brdcfg_eeprom__emflash_totalsize;//      = 256*1024;
    extern const uint32_t hal_brdcfg_eeprom__i2c_01_baseaddress;//     = 0;
    extern const uint32_t hal_brdcfg_eeprom__i2c_01_totalsize;//       = 64*1024;
    extern const uint16_t hal_brdcfg_eeprom__i2c_01_address_i2c;//     = 0xA0;
#endif//HAL_USE_EEPROM

#ifdef HAL_USE_I2C4HAL
    extern const uint8_t hal_brdcfg_i2c4hal__supported_mask;
#endif//HAL_USE_I2C4HAL


#ifdef HAL_USE_ENCODER
    extern const uint32_t hal_brdcfg_encoder__supported_mask; //         = 0x01ff;
#endif//HAL_USE_ENCODER

#ifdef HAL_USE_SPI4ENCODER
    extern const uint8_t hal_brdcfg_spi4encoder__supported_mask; 
#endif//HAL_USE_SPI4ENCODER

#ifdef HAL_USE_ETH
    extern const uint8_t hal_brdcfg_eth__supported_mask; //         = 0x01;
    extern const uint16_t hal_brdcfg_eth__phy_device_list[];
#endif//HAL_USE_ETH

#ifdef HAL_USE_GPIO
    extern const uint16_t hal_brdcfg_gpio__supported_mask[];
#endif//HAL_USE_GPIO

#ifdef HAL_USE_LED
    extern const hal_gpio_val_t hal_brdcfg_led__value_on;
    extern const hal_gpio_val_t hal_brdcfg_led__value_off;
    extern const uint8_t hal_brdcfg_led__supported_mask;
    extern const hal_gpio_cfg_t hal_brdcfg_led__cfg[];    
#endif//HAL_USE_LED 

#ifdef HAL_USE_SWITCH
    extern const hal_boolval_t hal_brdcfg_switch__supported;
#endif//HAL_USE_SWITCH

#ifdef HAL_USE_TIMER
    extern const uint8_t hal_brdcfg_timer__supported_mask;
#endif//HAL_USE_TIMER

#ifdef HAL_USE_WATCHDOG
    extern const uint8_t hal_brdcfg_watchdog__supported_mask;
#endif//HAL_USE_WATCHDOG



// - declaration of extern public functions ---------------------------------------------------------------------------
#ifdef HAL_USE_SYS
    extern void hal_brdcfg_sys__clock_config(void);
    extern void hal_brdcfg_sys__gpio_default_init(void);
#endif


#ifdef HAL_USE_CAN
    extern void hal_brdcfg_can__phydevices_enable(hal_can_port_t port);
    extern void hal_brdcfg_can__phydevices_disable(hal_can_port_t port);
#endif//HAL_USE_CAN


#ifdef HAL_USE_I2C4HAL
    extern void hal_brdcfg_i2c4hal__LowLevel_Init(void); 
    extern void hal_brdcfg_i2c4hal__LowLevel_DeInit(void);  
#endif//HAL_USE_I2C4HAL 

#ifdef HAL_USE_EEPROM
    extern void hal_brdcfg_eeprom__writeprotection_init(void);
    extern void hal_brdcfg_eeprom__writeprotection_disable(void);
    extern void hal_brdcfg_eeprom__writeprotection_enable(void);
#endif//HAL_USE_EEPROM   


#ifdef HAL_USE_SPI4ENCODER

    extern void hal_brdcfg_spi4encoder__chipSelect_init(hal_spi_port_t spix);
    extern void hal_brdcfg_spi4encoder__encoder_enable(hal_spi_port_t spix, hal_spi_mux_t e);
    extern void hal_brdcfg_spi4encoder__encoder_disable(hal_spi_port_t spix, hal_spi_mux_t e);  
#endif//HAL_USE_SPI4ENCODER


#ifdef HAL_USE_ETH
    extern void hal_brdcfg_eth__phy_start(void);
    extern hal_result_t hal_brdcfg_eth__check_links(uint8_t *linkst_mask, uint8_t *links_num);
    extern hal_result_t hal_brdcfg_eth__get_links_status(hal_eth_phy_status_t** link_list, uint8_t *links_num);
    extern hal_result_t hal_brdcfg_eth__get_errors_info(uint8_t phynum, hal_eth_phy_errors_info_type_t errortype, hal_eth_phy_errorsinfo_t *result);
#endif


#ifdef HAL_USE_SWITCH  
    
    extern void hal_brdcfg_switch__MCO_config(void);
    extern void hal_brdcfg_switch__reg_read_byI2C(uint8_t* pBuffer, uint16_t ReadAddr);
    extern void hal_brdcfg_switch__reg_write_byI2C(uint8_t* pBuffer, uint16_t WriteAddr);
#endif//HAL_USE_SWITCH


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------





