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

/* @file       hal_brdcfg_mcbstm32c.c
	@brief      This file implements low level functions which depend on board.
	@author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       11/16/2010
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "hal_stm32xx_include.h"

#include "stdlib.h"
#include "string.h"
#include "hal_base.h"
#include "hal_sys.h"
#include "hal_stm32_base_hid.h"
//#include "hal_stm32_spi4encoder_hid.h"
#include "hal_stm32_eth_hid.h"
#include "hal_stm32_eth_def.h"
#include "hal_eeprom.h"
#include "hal_timer.h"
#include "hal_i2c.h"
#include "hal_watchdog.h"
#include "hal_switch.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_brdcfg_mcbstm32c.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



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



#ifdef HAL_USE_ETH

// the phy used by this board is the DP83848x. of this phy we just use the Basic Mode Control Register (BMCR) and impose 100mbps full duplex

#define HAL_BRDCFG_ETH__PHYDEV_ADR                              0x01
    
#define HAL_BRDCFG_ETH__PHYREG_BMRC_ADR                         0x00

#define HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_RESET                   (1 << 15)
#define HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_100M                    (1 << 13)
#define HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_POWERDOWN               (1 << 11)
#define HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_FULLDUPLEX              (1 << 8)


#define HAL_BRDCFG_ETH__PHYDEV_IS_IN_RESET_STATE(bmrcval)	    ((bmrcval) & (HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_RESET | HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_POWERDOWN))

#define HAL_BRDCFG_ETH__PHYREG_WRITE_TIMEOUT                    0x10000

#endif//HAL_USE_ETH

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

#ifdef HAL_USE_CAN
    extern const uint8_t hal_brdcfg_can__supported_mask             = 0x03;
    #warning --> mettere qui solo i gpio del can1rx, can1tx, can2rx, can2tx ... non tutto il resto
//    extern const uint32_t hal_brdcfg_can__gpio_clock_canx_rx[]      = {RCC_APB2Periph_GPIOD, RCC_APB2Periph_GPIOB};
//    extern const uint32_t hal_brdcfg_can__gpio_clock_canx_tx[]      = {RCC_APB2Periph_GPIOD, RCC_APB2Periph_GPIOB};
//    extern const uint8_t hal_brdcfg_can__gpio_pinsource_canx_rx[]   = {0, 5};
//    extern const uint8_t hal_brdcfg_can__gpio_pinsource_canx_tx[]   = {1, 6};
//    extern const uint16_t hal_brdcfg_can__gpio_pin_canx_rx[]        = {GPIO_Pin_0, GPIO_Pin_5};
//    extern const uint16_t hal_brdcfg_can__gpio_pin_canx_tx[]        = {GPIO_Pin_1, GPIO_Pin_6};
//    extern GPIO_TypeDef* const hal_brdcfg_can__gpio_port_canx_rx[]  = {GPIOD, GPIOB};
//    extern GPIO_TypeDef* const hal_brdcfg_can__gpio_port_canx_tx[]  = {GPIOD, GPIOB}; 

    extern const hal_gpio_cfg_t hal_brdcfg_can__gpio_canx_rx[]      =
    {
        {   // can1
            .port     = hal_gpio_portD,
            .pin      = hal_gpio_pin0,        
            .dir      = hal_gpio_dirALT,
            .speed    = hal_gpio_speed_default
        },
        
        {   // can2
            .port     = hal_gpio_portB,
            .pin      = hal_gpio_pin5,        
            .dir      = hal_gpio_dirALT,
            .speed    = hal_gpio_speed_default        
        }
    };
    extern const hal_gpio_cfg_t hal_brdcfg_can__gpio_canx_tx[]      =
    {
        {   // can1
            .port     = hal_gpio_portD,
            .pin      = hal_gpio_pin1,        
            .dir      = hal_gpio_dirALT,
            .speed    = hal_gpio_speed_default
        },
        
        {   // can2
            .port     = hal_gpio_portB,
            .pin      = hal_gpio_pin6,        
            .dir      = hal_gpio_dirALT,
            .speed    = hal_gpio_speed_default        
        }
    };
    
#endif//HAL_USE_CAN

#ifdef HAL_USE_CRC
    extern const uint8_t hal_brdcfg_crc__supported_mask             = 0x03;
#endif//HAL_USE_CRC

#ifdef HAL_USE_DISPLAY
    extern const uint8_t hal_brdcfg_display__supported_mask         = 0x01;
#endif//HAL_USE_DISPLAY

#ifdef HAL_USE_ETH
    extern const uint8_t hal_brdcfg_eth__supported_mask                 = 0x01;
    
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_RMII_REF_CLK   = { .port = hal_gpio_portA, .pin = hal_gpio_pin1,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max  }; 
    
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_RMII_TX_EN     = { .port = hal_gpio_portB, .pin = hal_gpio_pin11,  .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max };
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_RMII_TXD0      = { .port = hal_gpio_portB, .pin = hal_gpio_pin12,  .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max };
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_RMII_TXD1      = { .port = hal_gpio_portB, .pin = hal_gpio_pin13,  .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max };
    
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_RMII_CRS_DV    = { .port = hal_gpio_portD, .pin = hal_gpio_pin8,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max };
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_RMII_RXD0      = { .port = hal_gpio_portD, .pin = hal_gpio_pin9,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max };
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_RMII_RXD1      = { .port = hal_gpio_portD, .pin = hal_gpio_pin10,  .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max };    
    
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_MDC            = { .port = hal_gpio_portC, .pin = hal_gpio_pin1,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max  };
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_MDIO           = { .port = hal_gpio_portA, .pin = hal_gpio_pin2,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max };     

    extern const hal_eth_phymode_t hal_brdcfg_eth__phymode              = hal_eth_phymode_fullduplex100mbps;    
#endif//HAL_USE_ETH


#ifdef HAL_USE_EEPROM
    extern const uint8_t hal_brdcfg_eeprom__supported_mask              = (1 << hal_eeprom_emulatedflash) | (1 << hal_eeprom_i2c_01);
    extern const uint32_t hal_brdcfg_eeprom__emflash_baseaddress        = 0x08000000;
    extern const uint32_t hal_brdcfg_eeprom__emflash_totalsize          = 256*1024;
    extern const hal_i2c_port_t hal_brdcfg_eeprom__i2c_01_i2cport       = hal_i2c_port1;
    extern const uint32_t hal_brdcfg_eeprom__i2c_01_baseaddress         = 0;
    extern const uint32_t hal_brdcfg_eeprom__i2c_01_totalsize           = 8*1024;    
    extern const hal_eeprom_hw_cfg_t hal_brdcfg_eeprom__i2c_01_device   =
    {
        .device             = hal_eeprom_device_st_m24c64,
        .hwaddra2a1a0       = 0        // a0 = a1 = a2 = 0                                 
    };
#endif//HAL_USE_EEPROM 



#ifdef HAL_USE_ENCODER
    extern const uint32_t hal_brdcfg_encoder__supported_mask = 0x01ff; // tutti e 9 gli encoder ...
#endif//HAL_USE_ENCODER


#ifdef HAL_USE_GPIO
    extern const uint16_t hal_brdcfg_gpio__supported_mask[hal_gpio_ports_number]        =
    {   // ok, i enable every pin of every port. however i should enable only those used as true gpios
        0xffff,     // port a
        0xffff,     // port b
        0xffff,     // port c
        0xffff,     // port d
        0xffff,     // port e
        0xffff,     // port f
        0xffff,     // port g
        0x0000,     // port h
        0x0000      // port i
    };
#endif//HAL_USE_GPIO
    
    
#ifdef HAL_USE_I2C
    extern const uint8_t hal_brdcfg_i2c__supported_mask                 = (1 << hal_i2c_port1); 
    extern const hal_gpio_cfg_t hal_brdcfg_i2c__scl[]                   =
    {
       {
            .port       = hal_gpio_portB,
            .pin        = hal_gpio_pin8,        
            .dir        = hal_gpio_dirALT,
            .speed      = hal_gpio_speed_default
        },
        {
            .port       = hal_gpio_portNONE,
            .pin        = hal_gpio_pinNONE,        
            .dir        = hal_gpio_dirALT,
            .speed      = hal_gpio_speed_default
        }            
    };       
    extern const hal_gpio_cfg_t hal_brdcfg_i2c__sda[]                   =
    {
        {
            .port       = hal_gpio_portB,
            .pin        = hal_gpio_pin9,        
            .dir        = hal_gpio_dirALT,
            .speed      = hal_gpio_speed_default
        }, 
        {
            .port       = hal_gpio_portNONE,
            .pin        = hal_gpio_pinNONE,        
            .dir        = hal_gpio_dirALT,
            .speed      = hal_gpio_speed_default
        }        
    };
#endif//HAL_USE_I2C



#ifdef HAL_USE_LED
    extern const hal_gpio_val_t hal_brdcfg_led__value_on          = hal_gpio_valHIGH;
    extern const hal_gpio_val_t hal_brdcfg_led__value_off         = hal_gpio_valLOW;
    extern const uint8_t hal_brdcfg_led__supported_mask           = 0xFF;
    extern const hal_gpio_cfg_t hal_brdcfg_led__cfg[hal_leds_num] = 
    {
        {   // hal_led0 
            .port     = hal_gpio_portE,
            .pin      = hal_gpio_pin8,        
            .dir      = hal_gpio_dirOUT,
            .speed    = hal_gpio_speed_low
        },
        {   // hal_led1 
            .port     = hal_gpio_portE,
            .pin      = hal_gpio_pin9,        
            .dir      = hal_gpio_dirOUT,
            .speed    = hal_gpio_speed_low
        },
        {   // hal_led2 
            .port     = hal_gpio_portE,
            .pin      = hal_gpio_pin10,        
            .dir      = hal_gpio_dirOUT,
            .speed    = hal_gpio_speed_low
        },
        {   // hal_led3 
            .port     = hal_gpio_portE,
            .pin      = hal_gpio_pin11,        
            .dir      = hal_gpio_dirOUT,
            .speed    = hal_gpio_speed_low
        },
        {   // hal_led4 
            .port     = hal_gpio_portE,
            .pin      = hal_gpio_pin12,        
            .dir      = hal_gpio_dirOUT,
            .speed    = hal_gpio_speed_low
        },
        {   // hal_led5 
            .port     = hal_gpio_portE,
            .pin      = hal_gpio_pin13,        
            .dir      = hal_gpio_dirOUT,
            .speed    = hal_gpio_speed_low
        },
        {   // hal_led6 
            .port     = hal_gpio_portE,
            .pin      = hal_gpio_pin14,        
            .dir      = hal_gpio_dirOUT,
            .speed    = hal_gpio_speed_low
        }, 
        {   // hal_led7 
            .port     = hal_gpio_portE,
            .pin      = hal_gpio_pin15,        
            .dir      = hal_gpio_dirOUT,
            .speed    = hal_gpio_speed_low
        }  
    };
#endif//HAL_USE_LED

    
#ifdef HAL_USE_SPI4ENCODER
    extern const uint8_t hal_brdcfg_spi4encoder__supported_mask = 0x07;  // tutte e 3 le spi 
#endif//HAL_USE_SPI4ENCODER    

#ifdef HAL_USE_SWITCH
    extern const hal_boolval_t hal_brdcfg_switch__supported         = hal_false;
    extern const hal_gpio_cfg_t hal_brdcfg_switch__gpio_reset       = { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE,   .dir = hal_gpio_dirNONE, .speed = hal_gpio_speed_default  };
#endif//HAL_USE_SWITCH

#ifdef  HAL_USE_SYS
    extern const hal_sys_hid_clock_cfg_t hal_brdcfg_sys__clockcfg =
    {
        .sourceclock        = hal_sys_refclock_pll_on_external_xtl, 
        .intclockspeed      =  8000000,
        .extclockspeed      = 25000000,
        .targetspeeds       =
        {   
            .cpu                = 72000000,
            .fastbus            = 72000000,
            .slowbus            = 36000000
        },
        .pllcfg             =
        {   // pll2clock = pll2mul*(hse/prediv2), speedcpu = pllmul*(pll2clock/prediv1) 
            .prediv2            = 5, 
            .pll2mul            = 8,
            .prediv1            = 5,
            .pllmul             = 9
        }
    };        
#endif//HAL_USE_SYS    
    
#ifdef HAL_USE_TIMER
    extern const uint8_t hal_brdcfg_timer__supported_mask           = 
    (0 << hal_timer1) | (1 << hal_timer2) | (1 << hal_timer3) | (1 << hal_timer4) |
    (1 << hal_timer5) | (1 << hal_timer6) | (1 << hal_timer7); // timer1 is not enabled.
#endif//HAL_USE_TIMER
    
#ifdef HAL_USE_WATCHDOG
    extern const uint8_t hal_brdcfg_watchdog__supported_mask        = (1 << hal_watchdog_normal) | (1 << hal_watchdog_window);
#endif//HAL_USE_WATCHDOG

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


extern uint32_t hal_brdcfg_devices__getsize(const hal_cfg_t *cfg)
{
    uint32_t ret = 0;
    
    // no memory needed 
    // ...
       
    // or calls ret += all the devices we need
    return(ret);
}

extern hal_result_t hal_brdcfg_devices__setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
   if(NULL == memory)
   {
       hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_brdcfg_devices__setmem(): memory missing");
       return(hal_res_NOK_generic);
   }
 
   // no memory needed
   // ...
   
   // or calls all the devices we need
   
   return(hal_res_OK);
}


extern hal_result_t hal_device_display_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory);

#ifdef HAL_USE_CAN

extern void hal_brdcfg_can__phydevices_init(hal_can_port_t port)
{
    // nothing to do
    hal_brdcfg_can__phydevices_disable(port);
}

extern void hal_brdcfg_can__phydevices_enable(hal_can_port_t port)
{   
    // nothing to do
}

extern void hal_brdcfg_can__phydevices_disable(hal_can_port_t port)
{
    // nothing to do
}

#endif//HAL_USE_CAN


#ifdef HAL_USE_I2C

extern void hal_brdcfg_i2c__ontimeouterror(void)
{
    hal_base_hid_on_fatalerror(hal_fatalerror_runtimefault, "timeout in i2c");
}

#endif//HAL_USE_I2C


#ifdef HAL_USE_EEPROM

extern hal_result_t hal_brdcfg_eeprom__wp_init(void)
{
    return(hal_res_OK);
}

extern hal_result_t hal_brdcfg_eeprom__wp_enable(void)
{
    return(hal_res_OK);
}

extern hal_result_t hal_brdcfg_eeprom__wp_disable(void)
{
    return(hal_res_OK);
}


#endif//HAL_USE_EEPROM





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



#ifdef HAL_USE_SWITCH  
    #error -> mcbstm32c does not have a switch
#endif//HAL_USE_SWITCH



#ifdef HAL_USE_ETH

extern hal_bool_t hal_brdcfg_eth__phy_initialise(void)
{
    uint16_t regv;
    uint32_t tout;
    
    // 1. initialises smi
    hal_eth_hid_smi_init();
    
	// reset the phy device (address is 1)
    hal_eth_hid_smi_write(HAL_BRDCFG_ETH__PHYDEV_ADR, HAL_BRDCFG_ETH__PHYREG_BMRC_ADR, HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_RESET);

	// wait for hardware reset to end 
    for(tout = 0; tout<HAL_BRDCFG_ETH__PHYREG_WRITE_TIMEOUT; tout++) 
    {
        regv = hal_eth_hid_smi_read(HAL_BRDCFG_ETH__PHYDEV_ADR, HAL_BRDCFG_ETH__PHYREG_BMRC_ADR);
        if (!HAL_BRDCFG_ETH__PHYDEV_IS_IN_RESET_STATE(regv))
        {
            // reset complete
            break;
        }
    }
    
    
    if(HAL_BRDCFG_ETH__PHYREG_WRITE_TIMEOUT == tout) //ethernet is still in reset state 
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_runtimefault, "hal_brdcfg_eth__phy_start(): PHY is still in reset state");
        return(hal_false);
    }  

    return(hal_true);
}

extern void hal_brdcfg_eth__phy_configure(hal_eth_phymode_t *phymode)
{
#if 0
    // configure phy in full duplex and 100MB
    hal_eth_hid_smi_write(HAL_BRDCFG_ETH__PHYDEV_ADR, HAL_BRDCFG_ETH__PHYREG_BMRC_ADR, HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_FULLDUPLEX | HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_100M); 
    
    *phymode = hal_eth_phymode_fullduplex100mbps;;
 
//     uint16_t regv;
//     uint32_t tout;
//     static uint16_t aaa = 0;    
//     regv = hal_eth_hid_smi_read(HAL_BRDCFG_ETH__PHYDEV_ADR, 2);    
//     aaa = regv;
//     for(;;);
#else
    
    hal_eth_phymode_t target = hal_brdcfg_eth__phymode;
    
    if((hal_eth_phymode_auto == target) || (hal_eth_phymode_none == target))
    {
        if(NULL != phymode)
        {
            *phymode = hal_eth_phymode_none;
        }   
        return;        
    }
    
    // configure phy according to the target mux and speed
    uint16_t val = 0x0000;
    switch(target)
    {
        case hal_eth_phymode_halfduplex10mbps:      val = 0x0000; break;
        case hal_eth_phymode_halfduplex100mbps:     val = HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_100M; break;
        case hal_eth_phymode_fullduplex10mbps:      val = HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_FULLDUPLEX; break;
        case hal_eth_phymode_fullduplex100mbps:     val = HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_FULLDUPLEX | HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_100M; break;
        default:                                    val = 0x0000; break;
    }
 
    hal_eth_hid_smi_write(HAL_BRDCFG_ETH__PHYDEV_ADR, HAL_BRDCFG_ETH__PHYREG_BMRC_ADR, val); 
    
    val = 0;
    val = hal_eth_hid_smi_read(HAL_BRDCFG_ETH__PHYDEV_ADR, HAL_BRDCFG_ETH__PHYREG_BMRC_ADR); 
    val = val;
    
    // gives back the used mux and speed
    if(NULL != phymode)
    {
        uint8_t speed  = ((val & HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_100M) == HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_100M) ? 1 : 0;
        uint8_t mux    = ((val & HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_FULLDUPLEX) == HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_FULLDUPLEX) ? 1 : 0; 
        if((0==mux)&&(0==speed))        *phymode = hal_eth_phymode_halfduplex10mbps;
        else if((0==mux)&&(1==speed))   *phymode = hal_eth_phymode_halfduplex100mbps; 
        else if((1==mux)&&(0==speed))   *phymode = hal_eth_phymode_fullduplex10mbps; 
        else if((1==mux)&&(1==speed))   *phymode = hal_eth_phymode_fullduplex100mbps;         
    }

#endif
}



#endif//HAL_USE_ETH

#ifdef HAL_USE_SYS

extern void hal_brdcfg_sys__clock_config(void)
{
   ; /* On this board this function is dummy, because 
       is not necessary configure MCO ar something else for thsi board */
}


extern void hal_brdcfg_sys__gpio_default_init(void)
{
   ; /* On this board this function is dummy */
}

#endif

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


