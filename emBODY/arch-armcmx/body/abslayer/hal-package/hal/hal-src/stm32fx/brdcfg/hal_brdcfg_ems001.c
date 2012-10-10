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

/* @file       hal_brdcfg_ems001.c
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
#include "hal_base.h"
#include "hal_sys.h"
#include "hal_stm32_base_hid.h"
#include "hal_stm32_spi4encoder_hid.h"
#include "hal_stm32_eth_hid.h"
#include "hal_stm32_eth_def.h"
#include "hal_eeprom.h"
#include "hal_timer.h"
#include "hal_i2c4hal.h"
#include "hal_watchdog.h"
#include "hal_switch.h"
#include "utils/stm32ee.h"
#include "utils/stm32gpio.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_brdcfg_ems001.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


#ifdef HAL_USE_SPI4ENCODER
    //- SPI1 chip selecet
    #define HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_PORT_CS			 	GPIOD
//    #define HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_PORT_CS_CLOCK			RCC_APB2Periph_GPIOD
    #define SPI1_GPIO_SEL_ENCODER_PIN_1	  	GPIO_Pin_2
    #define SPI1_GPIO_SEL_ENCODER_PIN_2	  	GPIO_Pin_3
    #define SPI1_GPIO_ENA_SLAVE		  		GPIO_Pin_4
//    #define HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_ENA_ENCODER_0			GPIO_Pin_10
//    #define HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_ENA_ENCODER_1			GPIO_Pin_1
#define SPI1_GPIO_SEL_LOW_ENCODER_0  	SPI1_GPIO_SEL_ENCODER_PIN_1 | SPI1_GPIO_SEL_ENCODER_PIN_2
#define SPI1_GPIO_SEL_HIGH_ENCODER_0 	0
#define SPI1_GPIO_SEL_LOW_ENCODER_1    	SPI1_GPIO_SEL_ENCODER_PIN_2
#define SPI1_GPIO_SEL_HIGH_ENCODER_1  	SPI1_GPIO_SEL_ENCODER_PIN_1
#define SPI1_GPIO_SEL_LOW_ENCODER_2    	SPI1_GPIO_SEL_ENCODER_PIN_1
#define SPI1_GPIO_SEL_HIGH_ENCODER_2  	SPI1_GPIO_SEL_ENCODER_PIN_2
    //- SPI2 chip selecet
    #define HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_PORT_CS			 	GPIOD
//    #define HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_PORT_CS_CLOCK			RCC_APB2Periph_GPIOD
    #define SPI2_GPIO_SEL_ENCODER_PIN_1	  	GPIO_Pin_2
    #define SPI2_GPIO_SEL_ENCODER_PIN_2	  	GPIO_Pin_3
    #define SPI2_GPIO_ENA_SLAVE		  		GPIO_Pin_4 
//    #define HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_ENA_ENCODER_0			GPIO_Pin_10
//    #define HAL_BRDCFG_SPI4ENCODER__SPI2_GPIO_ENA_ENCODER_1			GPIO_Pin_1

#define SPI2_GPIO_SEL_LOW_ENCODER_0  	GPIO_Pin_2 | GPIO_Pin_3
#define SPI2_GPIO_SEL_HIGH_ENCODER_0 	0

#define SPI2_GPIO_SEL_LOW_ENCODER_1    	GPIO_Pin_3
#define SPI2_GPIO_SEL_HIGH_ENCODER_1  	GPIO_Pin_2

#define SPI2_GPIO_SEL_LOW_ENCODER_2    	GPIO_Pin_2
#define SPI2_GPIO_SEL_HIGH_ENCODER_2  	GPIO_Pin_3

#define SPI2_GPIO_ENA_ENCODER_ALL		GPIO_Pin_4
#define SPI2_GPIO_DESELECT_ENCODER_ALL	GPIO_Pin_2 | GPIO_Pin_3
   
    //- SPI3 chip selecet
    #define HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_PORT_CS			 	GPIOD
//    #define HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_PORT_CS_CLOCK			RCC_APB2Periph_GPIOD

    #define  SPI3_GPIO_SEL_ENCODER_PIN_1	  	GPIO_Pin_5
    #define  SPI3_GPIO_SEL_ENCODER_PIN_2	  	GPIO_Pin_6
    #define  SPI3_GPIO_ENA_SLAVE		  		GPIO_Pin_7
    
//    #define HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_ENA_ENCODER_0			GPIO_Pin_10
//    #define HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_ENA_ENCODER_1			GPIO_Pin_1

#define SPI3_GPIO_SEL_LOW_ENCODER_0  	SPI3_GPIO_SEL_ENCODER_PIN_1 | SPI3_GPIO_SEL_ENCODER_PIN_2
#define SPI3_GPIO_SEL_HIGH_ENCODER_0 	0

#define SPI3_GPIO_SEL_LOW_ENCODER_1    	SPI3_GPIO_SEL_ENCODER_PIN_2
#define SPI3_GPIO_SEL_HIGH_ENCODER_1  	SPI3_GPIO_SEL_ENCODER_PIN_1

#define SPI3_GPIO_SEL_LOW_ENCODER_2    	SPI3_GPIO_SEL_ENCODER_PIN_1
#define SPI3_GPIO_SEL_HIGH_ENCODER_2  	SPI3_GPIO_SEL_ENCODER_PIN_2

//    // SPI commnds for enable/disable chipselect
//    #define HAL_BRDCFG_SPI4ENCODER__SPI_CS_ENA(spix, e)	 	((hal_SPI4ENCODER_ENCDATA_GET(spix)).encoder_gpio_port->BRR |= hal_SPI4ENCODER_ENCDATA_CS_GET(spix, e).BRR_reg);
//    #define HAL_BRDCFG_SPI4ENCODER__SPI_CS_DISA(spix, e)	((hal_SPI4ENCODER_ENCDATA_GET(spix)).encoder_gpio_port->BSRR  |= hal_SPI4ENCODER_ENCDATA_CS_GET(spix, e).BSRR_reg);

// SPI commnds for enable/disable chipselect
						
#define HAL_BRDCFG_SPI4ENCODER__SPI_CS_ENA(spix)    (hal_SPI4ENCODER_ENCDATA_GET(spix)).encoder_gpio_port->BRR |=  ((hal_SPI4ENCODER_ENCDATA_GET(spix)).cs_pin_ena_slave);	//Livello basso del segnale
#define HAL_BRDCFG_SPI4ENCODER__SPI_CS_DISA(spix)   (hal_SPI4ENCODER_ENCDATA_GET(spix)).encoder_gpio_port->BSRR |=  ((hal_SPI4ENCODER_ENCDATA_GET(spix)).cs_pin_ena_slave);	//Livello alto


//SPI commands for select/deselct encoder "e"  connected on "spix"
#define HAL_BRDCFG_SPI4ENCODER__SPI_CS_SEL(spix, e)	 ((hal_SPI4ENCODER_ENCDATA_GET(spix)).encoder_gpio_port->BRR |= hal_SPI4ENCODER_ENCDATA_CS_GET(spix, e).BRR_reg);\
							 ((hal_SPI4ENCODER_ENCDATA_GET(spix)).encoder_gpio_port->BSRR |= hal_SPI4ENCODER_ENCDATA_CS_GET(spix, e).BSRR_reg);
#define HAL_BRDCFG_SPI4ENCODER__SPI_CS_DESEL(spix)	 ((hal_SPI4ENCODER_ENCDATA_GET(spix)).encoder_gpio_port->BSRR  |= hal_SPI4ENCODER_ENCDATA_CS_GET(spix, 0).BRR_reg);	//Livello alto

    
#endif//HAL_USE_SPI4ENCODER



#if defined(HAL_USE_ETH)
#if 0   
    //#define HAL_BRDCFG_ETH__ETH_IS_IN_RESET_STATE(x)	    (x & 0x8800)
    #define HAL_BRDCFG_ETH_I2C_ADDRESS_WRITE_OP 0xBE        /* eth switch(micrel) device's address on I2C bus  for write operation */
    #define HAL_BRDCFG_ETH_I2C_ADDRESS_READ_OP  0xBF        /* eth switch(micrel) device's address on I2C bus  for read operation */
    // acemor on 16 sept 2011: the following are un-used ...
    #define HAL_BRDCFG_ETH__PHY_REG_ANER        0x06        /* Auto-Neg. Expansion Register      */
    #define HAL_BRDCFG_ETH__PHY_REG_ANNPTR      0x07        /* Auto-Neg. Next Page TX            */
    /* PHY Extended Registers */
    #define HAL_BRDCFG_ETH__PHY_REG_STS         0x10        /* Status Register                   */
    #define HAL_BRDCFG_ETH__PHY_REG_MICR        0x11        /* MII Interrupt Control Register    */
    #define HAL_BRDCFG_ETH__PHY_REG_MISR        0x12        /* MII Interrupt Status Register     */
    #define HAL_BRDCFG_ETH__PHY_REG_FCSCR       0x14        /* False Carrier Sense Counter       */
    #define HAL_BRDCFG_ETH__PHY_REG_RECR        0x15        /* Receive Error Counter             */
    #define HAL_BRDCFG_ETH__PHY_REG_PCSR        0x16        /* PCS Sublayer Config. and Status   */
    #define HAL_BRDCFG_ETH__PHY_REG_RBR         0x17        /* RMII and Bypass Register          */
    #define HAL_BRDCFG_ETH__PHY_REG_LEDCR       0x18        /* LED Direct Control Register       */
    #define HAL_BRDCFG_ETH__PHY_REG_PHYCR       0x19        /* PHY Control Register              */
    #define HAL_BRDCFG_ETH__PHY_REG_10BTSCR     0x1A        /* 10Base-T Status/Control Register  */
    #define HAL_BRDCFG_ETH__PHY_REG_CDCTRL1     0x1B        /* CD Test Control and BIST Extens.  */
    #define HAL_BRDCFG_ETH__PHY_REG_EDCR        0x1D        /* Energy Detect Control Register    */
    #define AL_BRDCFG_ETH__DP83848C_DEF_ADR    0x01        /* Default PHY device address        */
    #define AL_BRDCFG_ETH__DP83848C_ID         0x20005C90  /* PHY Identifier                    */
    //#define PHY_address 		DP83848C_DEF_ADR
    #define AL_BRDCFG_ETH__HAL_ETH_RESET_STATE_MASK		0x77FF//0x8800 DA SISTEMARE!!!!
    // acemor on 16 sept 2011: the previous are un-used ...
#endif
#endif//HAL_USE_ETH

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

#ifdef HAL_USE_CAN
    extern const uint8_t hal_brdcfg_can__supported_mask             = 0x03;
    #warning --> mettere qui solo i gpio del can1rx, can1tx, can2rx, can2tx ... non tutto il resto
    extern const uint32_t hal_brdcfg_can__gpio_clock_canx_rx[]      = {RCC_APB2Periph_GPIOD, RCC_APB2Periph_GPIOB};
    extern const uint32_t hal_brdcfg_can__gpio_clock_canx_tx[]      = {RCC_APB2Periph_GPIOD, RCC_APB2Periph_GPIOB};
    extern const uint8_t hal_brdcfg_can__gpio_pinsource_canx_rx[]   = {0, 5};
    extern const uint8_t hal_brdcfg_can__gpio_pinsource_canx_tx[]   = {1, 6};
    extern const uint16_t hal_brdcfg_can__gpio_pin_canx_rx[]        = {GPIO_Pin_0, GPIO_Pin_5};
    extern const uint16_t hal_brdcfg_can__gpio_pin_canx_tx[]        = {GPIO_Pin_1, GPIO_Pin_6};
    extern GPIO_TypeDef* const hal_brdcfg_can__gpio_port_canx_rx[]  = {GPIOD, GPIOB};
    extern GPIO_TypeDef* const hal_brdcfg_can__gpio_port_canx_tx[]  = {GPIOD, GPIOB};     
#endif//HAL_USE_CAN

#ifdef HAL_USE_CRC
    extern const uint8_t hal_brdcfg_crc__supported_mask             = 0x01;
#endif//HAL_USE_CRC

#ifdef HAL_USE_DISPLAY
    extern const uint8_t hal_brdcfg_display__supported_mask         = 0x00;
#endif//HAL_USE_DISPLAY

#ifdef HAL_USE_ETH
    extern const uint8_t hal_brdcfg_eth__supported_mask             = 0x01;
#endif//HAL_USE_ETH


#ifdef HAL_USE_EEPROM
    extern const uint8_t hal_brdcfg_eeprom__supported_mask         = (1 << hal_eeprom_emulatedflash) | (1 << hal_eeprom_i2c_01);
    extern const uint32_t hal_brdcfg_eeprom__emflash_baseaddress    = 0x08000000;
    extern const uint32_t hal_brdcfg_eeprom__emflash_totalsize      = 0;
    extern const uint32_t hal_brdcfg_eeprom__i2c_01_baseaddress     = 0;
    extern const uint32_t hal_brdcfg_eeprom__i2c_01_totalsize       = 8*1024;
    extern const stm32ee_cfg_t hal_brdcfg_eeprom__stm32eecfg =
    {
        .devcfg             =
        {
            .device             = stm32ee_device_atmel_at24c512b, //stm32ee_device_atmel_at24c1024b,
            .i2cport            = 1,        // is equivalent to hal_i2c_port1
            .hwaddra2a1a0       = 0,        // a0 = a1 = a2 = 0
            .wpval              = stm32gpio_valHIGH,        // write protection is on value high       
            .wppin              =
            {
                .port               = stm32gpio_portB,
                .pin                = stm32gpio_pin10
            }            
        },

        .i2cext             =
        {   // does not init/deinit. only use read/write/standby
            .i2cinit            = NULL,         
            .i2cdeinit          = NULL,
            .i2cpar             = NULL,
            .i2cread            = (stm32ee_int8_fp_uint8_uint8_regaddr_uint8p_uint16_t)stm32i2c_read,
            .i2cwrite           = (stm32ee_int8_fp_uint8_uint8_regaddr_uint8p_uint16_t)stm32i2c_write,
            .i2cstandby         = (stm32ee_int8_fp_uint8_uint8_t)stm32i2c_standby            
        }    
    };
    
#endif//HAL_USE_EEPROM     


#ifdef HAL_USE_I2C4HAL
    extern const uint8_t hal_brdcfg_i2c4hal__supported_mask         = (1 << hal_i2c_port1); 
    extern const stm32i2c_cfg_t hal_brdcfg_i2c4hal__i2ccfg          = 
    {
        .speed              = 400000,        // 400 mhz
        .scl                =
        {
            .port               = stm32gpio_portB,
            .pin                = stm32gpio_pin8
        }, 
        .sda                =
        {
            .port               = stm32gpio_portB,
            .pin                = stm32gpio_pin9
        },
        .usedma             = 0,
        .ontimeout          = NULL
    };
#endif//HAL_USE_I2C4HAL


#ifdef HAL_USE_ENCODER
    #warning --> tutti e 9 gli encoder anche se abbiamo solo spi1 ed spi3 ?
    extern const uint32_t hal_brdcfg_encoder__supported_mask = 0x01ff; // tutti e 9 gli encoder ...
#endif//HAL_USE_ENCODER

#ifdef HAL_USE_SPI4ENCODER
    extern const uint8_t hal_brdcfg_spi4encoder__supported_mask = 0x05;  // spi1 and spi3
#endif//HAL_USE_SPI4ENCODER

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

#ifdef HAL_USE_LED

    extern const hal_gpio_val_t hal_brdcfg_led__value_on          = hal_gpio_valLOW;
    extern const hal_gpio_val_t hal_brdcfg_led__value_off         = hal_gpio_valHIGH;
    extern const uint8_t hal_brdcfg_led__supported_mask           = 0x3F;
    extern const hal_gpio_cfg_t hal_brdcfg_led__cfg[hal_leds_num] = 
    {
        {   // hal_led0 - red
            .port     = hal_gpio_portE,
            .pin      = hal_gpio_pin8,        
            .speed    = hal_gpio_speed_low,
            .dir      = hal_gpio_dirOUT
        },
        {   // hal_led1 - green
            .port     = hal_gpio_portE,
            .pin      = hal_gpio_pin10,        
            .speed    = hal_gpio_speed_low,
            .dir      = hal_gpio_dirOUT
        },
        {   // hal_led2 - yellow
            .port     = hal_gpio_portE,
            .pin      = hal_gpio_pin12,        
            .speed    = hal_gpio_speed_low,
            .dir      = hal_gpio_dirOUT
        },
        {   // hal_led3 - orange
            .port     = hal_gpio_portE,
            .pin      = hal_gpio_pin15,        
            .speed    = hal_gpio_speed_low,
            .dir      = hal_gpio_dirOUT
        },
        {   // hal_led4 - can1
            .port     = hal_gpio_portD,
            .pin      = hal_gpio_pin11,        
            .speed    = hal_gpio_speed_low,
            .dir      = hal_gpio_dirOUT
        },
        {   // hal_led5 - can2 
            .port     = hal_gpio_portB,
            .pin      = hal_gpio_pin7,        
            .speed    = hal_gpio_speed_low,
            .dir      = hal_gpio_dirOUT
        }        
    };

#endif//HAL_USE_LED

#ifdef HAL_USE_TIMER
    extern const uint8_t hal_brdcfg_timer__supported_mask           = 
    (0 << hal_timer1) | (1 << hal_timer2) | (1 << hal_timer3) | (1 << hal_timer4) |
    (1 << hal_timer5) | (1 << hal_timer6) | (1 << hal_timer7);
#endif//HAL_USE_TIMER

#ifdef HAL_USE_SWITCH
    extern const hal_boolval_t hal_brdcfg_switch__supported         = hal_true;
#endif//HAL_USE_SWITCH

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

#ifdef HAL_USE_CAN
#if 1
extern void hal_brdcfg_can__phydevices_init(hal_can_port_t port)
{   // the phy are connected to the can bus pin using an enable circuit driven by a gpio. then just init the pin
   #warning --> i could instead use hal_gpio_init(hal_gpio_portB, hal_gpio_pinxx, hal_gpio_speed_low); 

    GPIO_InitTypeDef hal_brdcfg_can__gpio_canx_enable =
    {   // output low speed
        .GPIO_Pin       = 0,
        .GPIO_Mode      = GPIO_Mode_Out_PP,
        .GPIO_Speed     = GPIO_Speed_2MHz
    };
    
	if(hal_can_port1 == port)
	{   // PB14
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        hal_brdcfg_can__gpio_canx_enable.GPIO_Pin = GPIO_Pin_14;
        GPIO_Init(GPIOB, &hal_brdcfg_can__gpio_canx_enable);
	}
	else if(hal_can_port2 == port) 
	{   // PB15
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        hal_brdcfg_can__gpio_canx_enable.GPIO_Pin = GPIO_Pin_15;
        GPIO_Init(GPIOB, &hal_brdcfg_can__gpio_canx_enable);
	}   

    hal_brdcfg_can__phydevices_disable(port);
}


extern void hal_brdcfg_can__phydevices_enable(hal_can_port_t port)
{   
    // i could instead use hal_gpio_init(hal_gpio_portB, hal_gpio_pinxx, hal_gpio_valLOW);

	if(hal_can_port1 == port)
	{
        GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
	}
	else 
	{
        GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
	}    
}

extern void hal_brdcfg_can__phydevices_disable(hal_can_port_t port)
{
    // i could instead use hal_gpio_init(hal_gpio_portB, hal_gpio_pinxx, hal_gpio_valHIGH);
    
	if(hal_can_port1 == port)
	{
        GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
	}
	else if(hal_can_port2 == port)
	{
        GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
	}
}
#endif
#endif//HAL_USE_CAN


#ifdef HAL_USE_I2C4HAL

extern void hal_brdcfg_i2c__ontimeouterror(void)
{
    hal_base_hid_on_fatalerror(hal_fatalerror_runtimefault, "timeout in eeprom");
}

#endif//HAL_USE_I2C4HAL


#ifdef HAL_USE_EEPROM

#endif//HAL_USE_EEPROM




#ifdef HAL_USE_SPI4ENCODER

/*Note: if there will be some speed problem try to transform  hal_brdcfg_spi4encoder__encoder_enable and  hal_brdcfg_spi4encoder__encoder_disable as inline function,
but pay attention with the project with scatter file.probably it will give you trouble*/

extern void  hal_brdcfg_spi4encoder__encoder_enable(hal_spi_port_t spix,  hal_spi_mux_t e)
{
    HAL_BRDCFG_SPI4ENCODER__SPI_CS_SEL(spix, e);
    HAL_BRDCFG_SPI4ENCODER__SPI_CS_ENA(spix);	
}

extern void  hal_brdcfg_spi4encoder__encoder_disable(hal_spi_port_t spix,  hal_spi_mux_t e)
{
    HAL_BRDCFG_SPI4ENCODER__SPI_CS_DESEL(spix);
    HAL_BRDCFG_SPI4ENCODER__SPI_CS_DISA(spix);	
}

void  hal_brdcfg_spi4encoder__chipSelect_init(hal_spi_port_t spix)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //output push-pull mode
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

//1) init data struct and GPIO regiter
	switch(spix)
	{
		case hal_spi_port1:
		{
    		hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[0].BRR_reg = SPI1_GPIO_SEL_LOW_ENCODER_0; //pin che devono valere zero
    		hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[0].BSRR_reg = SPI1_GPIO_SEL_HIGH_ENCODER_0;	//pin che devono valere 1
    
    		hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[1].BRR_reg = SPI1_GPIO_SEL_LOW_ENCODER_1;
    		hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[1].BSRR_reg = SPI1_GPIO_SEL_HIGH_ENCODER_1;
    
    		hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[2].BRR_reg = SPI1_GPIO_SEL_LOW_ENCODER_2;
    		hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[2].BSRR_reg = SPI1_GPIO_SEL_HIGH_ENCODER_2;
    
    		hal_SPI4ENCODER_ENCDATA_GET(spix).cs_pin_ena_slave = SPI1_GPIO_ENA_SLAVE;
    		hal_SPI4ENCODER_ENCDATA_GET(spix).encoder_gpio_port = HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_PORT_CS;	
    
    		GPIO_InitStructure.GPIO_Pin = SPI1_GPIO_SEL_ENCODER_PIN_1| SPI1_GPIO_SEL_ENCODER_PIN_2 | SPI1_GPIO_ENA_SLAVE; //tutti i pin sono sulla stessa porta.
    		GPIO_Init(HAL_BRDCFG_SPI4ENCODER__SPI1_GPIO_PORT_CS, &GPIO_InitStructure);
		
		} break;
		
		case hal_spi_port3:
		{
    		hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[0].BRR_reg = SPI3_GPIO_SEL_LOW_ENCODER_0; //pin che devono valere zero
    		hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[0].BSRR_reg = SPI3_GPIO_SEL_HIGH_ENCODER_0;	//pin che devono valere 1
    
    		hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[1].BRR_reg = SPI3_GPIO_SEL_LOW_ENCODER_1;
    		hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[1].BSRR_reg = SPI3_GPIO_SEL_HIGH_ENCODER_1;
    
    		hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[2].BRR_reg = SPI3_GPIO_SEL_LOW_ENCODER_2;
    		hal_SPI4ENCODER_ENCDATA_GET(spix).cs_encoder[2].BSRR_reg = SPI3_GPIO_SEL_HIGH_ENCODER_2;
    
    		hal_SPI4ENCODER_ENCDATA_GET(spix).cs_pin_ena_slave = SPI3_GPIO_ENA_SLAVE;
    		hal_SPI4ENCODER_ENCDATA_GET(spix).encoder_gpio_port = HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_PORT_CS;	
    
    		GPIO_InitStructure.GPIO_Pin = SPI3_GPIO_SEL_ENCODER_PIN_1| SPI3_GPIO_SEL_ENCODER_PIN_2 | SPI3_GPIO_ENA_SLAVE; //tutti i pin sono sulla stessa porta.
    		GPIO_Init(HAL_BRDCFG_SPI4ENCODER__SPI3_GPIO_PORT_CS, &GPIO_InitStructure);
		
		} break;

	}


//3) set pin high value
	HAL_BRDCFG_SPI4ENCODER__SPI_CS_DESEL(spix);
	HAL_BRDCFG_SPI4ENCODER__SPI_CS_DISA(spix);
	
//4)  enable powe supply
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); 
	GPIOE->CRL &= 0xFFFFFF0F;  //reset Pin 1
	GPIOE->CRL |= 0x00000020;  //Configure pin 1 as General Purpose output Push Pull 2Mhz 
	GPIOE->BSRR |= 0x0002; //enable power supply
}

#endif//HAL_USE_SPI4ENCODER

                     
#ifdef HAL_USE_SWITCH
 

#endif//HAL_USE_SWITCH


#ifdef HAL_USE_ETH

extern void hal_brdcfg_eth__phy_initialise(void)
{

    #warning --> in here we just init the switch ... put in reset mode, exit from reset, do the mco, init the i2c if not initted, etc. 
    if(hal_false == hal_switch_initted_is())
    {
        hal_switch_init(NULL);
    }

}

extern void hal_brdcfg_eth__phy_configure(void)
{
    #warning --> in here we configure the switch ... in full duplex 100mbps for instance
    hal_switch_start();
}


#endif//HAL_USE_ETH


#ifdef HAL_USE_SYS
extern void hal_brdcfg_sys__clock_config(void)
{
 
}

extern void hal_brdcfg_sys__gpio_default_init(void)
{

#if 0

//
//#define   IN_ANALOG          0x0
//#define   IN_FLOATING        0x04
//#define   OUT_PUSH_PULL_2M   0x2
//#define   IN_PUL_UP          0x8



// 1) abilito il clock per tutte le porte GPIO (A, B, C, D, E) una volta per tutte.
    RCC->APB2ENR |= 0x0000007D;
// 2) configuro i pin di tutte le porte

/*
    PORT A: pin 0 to 7
        - pin 0: lascia cosi' come e' inizializzato (input -floating)
        - pin 1: lascia cosi' come e' inizializzato (input -floating)
        - pin 2: GPOUT 2MHz 
        - pin 3: Analog input 
        - pin 4: Analog input
        - pin 5: GPOUT 2MHz   (valore basso)
        - pin 6: input PullUp
        - pin 7: GPOUT 2MHz    (valore basso)
*/
    GPIOA->CRL   &= 0x000000FF;  //reset
    GPIOA->CRL   |= 0x28288200;  //config
    GPIOA->BRR   |= ((1 << 5) |  (1 << 7)); //set val low


/*
    PORT A: pin 8 to 15
        - pin 8: GPOUT 2MHz
        - pin 9: GPOUT 2MHz (valore alto)
        - pin 10: input PullUp
        - pin 11: input PullUp
        - pin 12:  input PullUp
        - pin 13:  lascia cosi' come e' inizializzato  (Jtag nopn toccare)
        - pin 14:  lascia cosi' come e' inizializzato  (Jtag nopn toccare)
        - pin 15:  lascia cosi' come e' inizializzato   (Jtag nopn toccare)
*/
 GPIOA->CRH   &= 0xFFF00000;  //reset
 GPIOA->CRH   |= 0x00088822;   //config

 GPIOA->BSRR   |= ((1 << 9) ); //set val high




/*
    PORT B: pin 0 to 7
        - pin 0: input PullUp
        - pin 1: input PullUp
        - pin 2: GPOUT 2MHz   (valore basso)
        - pin 3: lascia cosi' come e' inizializzato   (Jtag nopn toccare)
        - pin 4: lascia cosi' come e' inizializzato   (Jtag nopn toccare)
        - pin 5: input PullUp
        - pin 6: GPOUT 2MHz    (valore alto)
        - pin 7: GPOUT 2MHz    (valore alto)
*/
    GPIOB->CRL   &= 0x00000000;  //reset
    GPIOB->CRL   |= 0x22800288;  //config
    GPIOB->BSRR   |= ((1 << 6) |  (1 << 7)); //set val high
    GPIOB->BRR   |= ((1 << 2) ); //set val low


/*
    PORT B pin 8 to 15
        - pin 8: lascia cosi' come e' inizializzato (perche' hanno la resistenza)
        - pin 9: lascia cosi' come e' inizializzato (perche' hanno la resistenza)
        - pin 10: lascia cosi' come e' inizializzato (perche' hanno la resistenza)
        - pin 11: GPOUT 2MHz    (valore basso)
        - pin 12: GPOUT 2MHz    (valore basso)
        - pin 13: GPOUT 2MHz    (valore basso)
        - pin 14:  GPOUT 2MHz    (valore alto)  (anche se hanno la loro resistenza)
        - pin 15: GPOUT 2MHz    (valore alto)   (anche se hanno la loro resistenza)
*/
    GPIOB->CRH   &= 0x00000FFF;  //reset
    GPIOB->CRH   |= 0x22222000;   //config
    
    GPIOB->BSRR   |= ((1 << 14) | (1 << 15) ); //set val high
    GPIOB->BRR   |= ((1 << 11) | (1 << 12) | (1 << 13)); //set val low






/*
    PORT C: pin 0 to 7
        - pin 0: lascia cosi' come e' inizializzato (input -floating)
        - pin 1: GPOUT 2MHz   (valore basso)
        - pin 2: Analog input
        - pin 3: Analog input 
        - pin 4: Analog input
        - pin 5: Analog input
        - pin 6: input PullUp
        - pin 7: input PullUp
*/
    GPIOC->CRL   &= 0x0000000F;  //reset
    GPIOC->CRL   |= 0x88000020;  //config
    GPIOC->BRR   |= ((1 << 1)); //set val low


/*
    PORT C: pin 8 to 15
        - pin 8: input PullUp
        - pin 9: input PullUp (perche' A8 e' output e se no fa cortocircuito)
        - pin 10: GPOUT 2MHz   (valore basso)
        - pin 11: input PullUp
        - pin 12:  GPOUT 2MHz   (valore basso)
        - pin 13:  GPOUT 2MHz  
        - pin 14:  GPOUT 2MHz
        - pin 15:  GPOUT 2MHz
*/
    GPIOC->CRH   &= 0x00000000;  //reset
    GPIOC->CRH   |= 0x22228288;   //config
    
    GPIOC->BSRR   |= ((1 << 10) | (1 << 12)  ); //set val low




/*
    PORT D: pin 0 to 7
        - pin 0: input PullUp
        - pin 1: GPOUT 2MHz   (valore alto)
        - pin 2: lascia cosi' come e' inizializzato (input -floating) (perche' ci sono i pull up)
        - pin 3: lascia cosi' come e' inizializzato (input -floating) (perche' ci sono i pull up)
        - pin 4: GPOUT 2MHz   (valore alto)
        - pin 5: lascia cosi' come e' inizializzato (input -floating) (perche' ci sono i pull up)
        - pin 6: lascia cosi' come e' inizializzato (input -floating) (perche' ci sono i pull up)
        - pin 7: GPOUT 2MHz   (valore alto)
*/
    GPIOD->CRL   &= 0x0FF0FF00;  //reset
    GPIOD->CRL   |= 0x20020028;  //config
    GPIOD->BSRR   |= ((1 << 1) |  (1 << 4) |  (1 << 7)); //set val low


/*
    PORT D: pin 8 to 15
        - pin 8: lascia cosi' come e' inizializzato (input -floating)
        - pin 9: lascia cosi' come e' inizializzato (input -floating)
        - pin 10: lascia cosi' come e' inizializzato (input -floating)
        - pin 11: GPOUT 2MHz   (valore alto)
        - pin 12: lascia cosi' come e' inizializzato (input -floating) (perche' ci sono i pull up)
        - pin 13: lascia cosi' come e' inizializzato (input -floating) (perche' ci sono i pull up)
        - pin 14: lascia cosi' come e' inizializzato (input -floating) (perche' ci sono i pull up)
        - pin 15: lascia cosi' come e' inizializzato (input -floating) (perche' ci sono i pull up)
*/
    GPIOD->CRH   &= 0xFFFF0FFF;  //reset
    GPIOD->CRH   |= 0x00002000;   //config
    
    GPIOD->BSRR   |= ((1 << 11) ); //set val high




/*
    PORT E: pin 0 to 7
        - pin 0: lascia cosi' come e' inizializzato (input -floating)
        - pin 1: GPOUT 2MHz (valore alto)
        - pin 2: lascia cosi' come e' inizializzato   (Jtag nopn toccare)
        - pin 3: lascia cosi' come e' inizializzato   (Jtag nopn toccare)
        - pin 4: lascia cosi' come e' inizializzato   (Jtag nopn toccare)
        - pin 5: lascia cosi' come e' inizializzato   (Jtag nopn toccare)
        - pin 6: lascia cosi' come e' inizializzato   (Jtag nopn toccare)
        - pin 7: input PullUp
*/
    GPIOE->CRL   &= 0x0FFFFF0F;  //reset
    GPIOE->CRL   |= 0x80000020;  //config
    GPIOE->BSRR   |= ((1 << 1) ); //set val high


/*
    PORT E: pin 8 to 15
        - pin 8: GPOUT 2MHz (valore alto)
        - pin 9: input PullUp
        - pin 10: GPOUT 2MHz (valore alto)
        - pin 11: input PullUp
        - pin 12: GPOUT 2MHz (valore alto)
        - pin 13: input PullUp
        - pin 14:  input PullUp
        - pin 15: GPOUT 2MHz (valore alto)
*/
    GPIOE->CRH   &= 0x00000000;  //reset
    GPIOE->CRH   |= 0x28828282;   //config
    
    GPIOE->BSRR   |= ((1 << 8) | (1 << 10) |  (1 << 12) |  (1 << 15) ); //set val high


#endif

}

#endif


///// -----------------  OLDIES ---------------------------------------------------------------------------------------

//extern  void hal_brdcfg_eth__clock_config(void)
//{
//    //su questa board il clock tx e rx e' connesso all'MCO.
//    //quindi devo configurare MCO.
//    
//    // il clock sulla porta A e' abilitato all'inizio nella eth init.
//    GPIOA->CRH   &= 0xFFFFFFF0;
//    GPIOA->CRH   |= 0x0000000B;	/*Pin 8 is congigured in AlterFunc 50MHz*/	
//    
//    /* set PLL3 clock output to 50MHz (25MHz /5 *10 =50MHz) */
//    RCC_PLL3Config(RCC_PLL3Mul_10);
//    
//    
//    /* Enable PLL3 */
//    RCC_PLL3Cmd(ENABLE);
//    
//    /* Wait till PLL3 is ready */
//    while (RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) == RESET)
//    {}
//    
//    /* Get clock PLL3 clock on PA8 pin */
//    RCC_MCOConfig(RCC_MCO_PLL3CLK);
//
//}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

#ifdef HAL_USE_I2C4HAL
/*Note: mcu runs @ 72Mhz, 1.25 MIPS/Hz ==> 72 * 1.25 = 90 MIPS
        in 2 microsec about 180 istructions.
*/
static void s_hal_ems001_i2c_wait2micro(void)   
{
#if 0
    volatile uint8_t i, aux = 0;

    for(i = 0; i<180; i++)
    {
       aux++; //istruzione per occupare un po' la cpu
       if(255 == aux)
       {
        aux = 0;
       }
    }
#else
    hal_sys_delay(2);
#endif    
}
#endif//HAL_USE_I2C4HAL

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

