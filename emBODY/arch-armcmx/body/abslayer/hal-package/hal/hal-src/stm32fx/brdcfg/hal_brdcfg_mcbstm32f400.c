
/* @file       hal_brdcfg_mcbstmf400.c
	@brief      This file implements low level functions which depend on board.
	@author     valentina.gaggero@iit.it
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

// the phy used by this board is the ST802RT1x. of this phy we just use the Basic Mode Control Register (BMCR) and impose 100mbps full duplex

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
    extern const uint32_t hal_brdcfg_can__gpio_clock_canx_rx[]      = {RCC_AHB1Periph_GPIOH, RCC_AHB1Periph_GPIOB};
    extern const uint32_t hal_brdcfg_can__gpio_clock_canx_tx[]      = {RCC_AHB1Periph_GPIOI, RCC_AHB1Periph_GPIOB};
    extern const uint8_t hal_brdcfg_can__gpio_pinsource_canx_rx[]   = {13, 5};
    extern const uint8_t hal_brdcfg_can__gpio_pinsource_canx_tx[]   = {9, 13};
    extern const uint16_t hal_brdcfg_can__gpio_pin_canx_rx[]        = {GPIO_Pin_13, GPIO_Pin_5};
    extern const uint16_t hal_brdcfg_can__gpio_pin_canx_tx[]        = {GPIO_Pin_9, GPIO_Pin_13};
    extern GPIO_TypeDef* const hal_brdcfg_can__gpio_port_canx_rx[]  = {GPIOH, GPIOB};
    extern GPIO_TypeDef* const hal_brdcfg_can__gpio_port_canx_tx[]  = {GPIOI, GPIOB};                
#endif//HAL_USE_CAN

#ifdef HAL_USE_CRC
    extern const uint8_t hal_brdcfg_crc__supported_mask             = 0x03;
#endif//HAL_USE_CRC

#ifdef HAL_USE_DISPLAY
    extern const uint8_t hal_brdcfg_display__supported_mask         = 0x00;
#endif//HAL_USE_DISPLAY

#ifdef HAL_USE_ETH
    extern const uint8_t hal_brdcfg_eth__supported_mask             = 0x01;
    
    extern const stm32gpio_gpio_t hal_brdcfg_eth__gpio_ETH_RMII_REF_CLK = { .port = stm32gpio_portA, .pin = stm32gpio_pin1  }; 
    
    extern const stm32gpio_gpio_t hal_brdcfg_eth__gpio_ETH_RMII_TX_EN   = { .port = stm32gpio_portG, .pin = stm32gpio_pin11 };
    extern const stm32gpio_gpio_t hal_brdcfg_eth__gpio_ETH_RMII_TXD0    = { .port = stm32gpio_portG, .pin = stm32gpio_pin13 };
    extern const stm32gpio_gpio_t hal_brdcfg_eth__gpio_ETH_RMII_TXD1    = { .port = stm32gpio_portG, .pin = stm32gpio_pin14 };
    
    extern const stm32gpio_gpio_t hal_brdcfg_eth__gpio_ETH_RMII_CRS_DV  = { .port = stm32gpio_portA, .pin = stm32gpio_pin7  };
    extern const stm32gpio_gpio_t hal_brdcfg_eth__gpio_ETH_RMII_RXD0    = { .port = stm32gpio_portC, .pin = stm32gpio_pin4  };
    extern const stm32gpio_gpio_t hal_brdcfg_eth__gpio_ETH_RMII_RXD1    = { .port = stm32gpio_portC, .pin = stm32gpio_pin5  };    
    
    extern const stm32gpio_gpio_t hal_brdcfg_eth__gpio_ETH_MDC          = { .port = stm32gpio_portC, .pin = stm32gpio_pin1  };
    extern const stm32gpio_gpio_t hal_brdcfg_eth__gpio_ETH_MDIO         = { .port = stm32gpio_portA, .pin = stm32gpio_pin2  };   
    
#endif//HAL_USE_ETH


#ifdef HAL_USE_EEPROM
    extern const uint8_t  hal_brdcfg_eeprom__supported_mask         = (1 << hal_eeprom_i2c_01);
    extern const uint32_t hal_brdcfg_eeprom__emflash_baseaddress    = 0x08000000;
    extern const uint32_t hal_brdcfg_eeprom__emflash_totalsize      = 0;
    extern const uint32_t hal_brdcfg_eeprom__i2c_01_baseaddress     = 0;
    extern const uint32_t hal_brdcfg_eeprom__i2c_01_totalsize       = 8*1024; 
    extern const stm32ee_cfg_t hal_brdcfg_eeprom__stm32eecfg =
    {
        .devcfg             =
        {
            .device             = stm32ee_device_st_m24lr64,
            .i2cport            = 1,        // is equivalent to hal_i2c_port1
            .hwaddra2a1a0       = 0,        // a0 = a1 = a2 = 0
            .wpval              = stm32gpio_valNONE,      // no write protection
            .wppin              =
            {
                .port               = stm32gpio_portNONE,
                .pin                = stm32gpio_pinNONE
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
    extern const uint8_t hal_brdcfg_i2c4hal__supported_mask = (1 << hal_i2c_port1); 
    extern const stm32i2c_cfg_t hal_brdcfg_i2c4hal__i2ccfg = 
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
    extern const uint32_t hal_brdcfg_encoder__supported_mask = 0x01ff; // tutti e 9 gli encoder ...
#endif//HAL_USE_ENCODER

#ifdef HAL_USE_SPI4ENCODER
    extern const uint8_t hal_brdcfg_spi4encoder__supported_mask = 0x07;  // tutte e 3 le spi 
#endif//HAL_USE_SPI4ENCODER

#ifdef HAL_USE_GPIO
    extern const uint16_t hal_brdcfg_gpio__supported_mask[hal_gpio_ports_number]    =
    {   // ok, i enable every pin of every port. however i should enable only those used a true gpio
        0xffff,     // port a
        0xffff,     // port b
        0xffff,     // port c
        0xffff,     // port d
        0xffff,     // port e
        0xffff,     // port f
        0xffff,     // port g
        0xffff,     // port h
        0xffff      // port i
    };
#endif//HAL_USE_GPIO

#warning --> the leds are alright

#ifdef HAL_USE_LED
    extern const hal_gpio_val_t hal_brdcfg_led__value_on          = hal_gpio_valHIGH;
    extern const hal_gpio_val_t hal_brdcfg_led__value_off         = hal_gpio_valLOW;
    extern const uint8_t hal_brdcfg_led__supported_mask           = 0xFF;
    extern const hal_gpio_cfg_t hal_brdcfg_led__cfg[hal_leds_num] = 
    {
        {   // hal_led0 
            .port     = hal_gpio_portH,
            .pin      = hal_gpio_pin2,        
            .speed    = hal_gpio_speed_low,
            .dir      = hal_gpio_dirOUT
        },
        {   // hal_led1 
            .port     = hal_gpio_portG,
            .pin      = hal_gpio_pin8,        
            .speed    = hal_gpio_speed_low,
            .dir      = hal_gpio_dirOUT
        },
        {   // hal_led2 
            .port     = hal_gpio_portG,
            .pin      = hal_gpio_pin7,        
            .speed    = hal_gpio_speed_low,
            .dir      = hal_gpio_dirOUT
        },
        {   // hal_led3 
            .port     = hal_gpio_portG,
            .pin      = hal_gpio_pin6,        
            .speed    = hal_gpio_speed_low,
            .dir      = hal_gpio_dirOUT
        },
        {   // hal_led4 
            .port     = hal_gpio_portI,
            .pin      = hal_gpio_pin10,        
            .speed    = hal_gpio_speed_low,
            .dir      = hal_gpio_dirOUT
        },
        {   // hal_led5 
            .port     = hal_gpio_portH,
            .pin      = hal_gpio_pin7,        
            .speed    = hal_gpio_speed_low,
            .dir      = hal_gpio_dirOUT
        },
        {   // hal_led6 
            .port     = hal_gpio_portH,
            .pin      = hal_gpio_pin6,        
            .speed    = hal_gpio_speed_low,
            .dir      = hal_gpio_dirOUT
        }, 
        {   // hal_led7 
            .port     = hal_gpio_portH,
            .pin      = hal_gpio_pin3,        
            .speed    = hal_gpio_speed_low,
            .dir      = hal_gpio_dirOUT
        }  
    };
#endif//HAL_USE_LED

#ifdef HAL_USE_TIMER
    extern const uint8_t hal_brdcfg_timer__supported_mask           = 
    (0 << hal_timer1) | (1 << hal_timer2) | (1 << hal_timer3) | (1 << hal_timer4) |
    (1 << hal_timer5) | (1 << hal_timer6) | (1 << hal_timer7); // timer1 is not enabled.
    #warning --> on stm32f4 there are more timers ...
#endif//HAL_USE_TIMER

#ifdef HAL_USE_SWITCH
    extern const hal_boolval_t hal_brdcfg_switch__supported         = hal_false;
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


#ifdef HAL_USE_I2C4HAL


// extern void hal_brdcfg_i2c4hal__LowLevel_DeInit(void)
// {
//   GPIO_InitTypeDef  GPIO_InitStructure; 
//    
//   /* sEE_I2C Peripheral Disable */
//   I2C_Cmd(HAL_BRDCFG_I2C4HAL__PERIPHERAL, DISABLE);
//  
//   /* sEE_I2C DeInit */
//   I2C_DeInit(HAL_BRDCFG_I2C4HAL__PERIPHERAL);

//   /*!< sEE_I2C Periph clock disable */
//   RCC_APB1PeriphClockCmd(HAL_BRDCFG_I2C4HAL__I2C_CLK, DISABLE);
//     
//   /*!< GPIO configuration */  
//   /*!< Configure sEE_I2C pins: SCL */
// //  GPIO_InitStructure.GPIO_Pin = HAL_BRDCFG_I2C4HAL__I2C_SCL_PIN;
// //  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
// //  GPIO_Init(HAL_BRDCFG_I2C4HAL__I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

//   /*!< Configure sEE_I2C pins: SDA */
// //  GPIO_InitStructure.GPIO_Pin = HAL_BRDCFG_I2C4HAL__I2C_SDA_PIN;
// //  GPIO_Init(HAL_BRDCFG_I2C4HAL__I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
// }

#endif//HAL_USE_I2C4HAL


#ifdef HAL_USE_I2C4HAL

// extern void hal_brdcfg_i2c4hal__LowLevel_Init(void)
// {
//   GPIO_InitTypeDef  GPIO_InitStructure; 
//     
//   /*!< sEE_I2C_SCL_GPIO_CLK and sEE_I2C_SDA_GPIO_CLK Periph clock enable */
//   RCC_APB2PeriphClockCmd(HAL_BRDCFG_I2C4HAL__I2C_SCL_GPIO_CLK | HAL_BRDCFG_I2C4HAL__I2C_SDA_GPIO_CLK, ENABLE);

//   /*!< sEE_I2C Periph clock enable */
//   RCC_APB1PeriphClockCmd(HAL_BRDCFG_I2C4HAL__I2C_CLK, ENABLE);

//     #warning --> mettere AFIO
// //  AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;  /* I2C1 remapping */
//     
//     GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
//     GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
//     
//     
//   /*!< GPIO configuration */  
//   /*!< Configure sEE_I2C pins: SCL */
//   GPIO_InitStructure.GPIO_Pin = HAL_BRDCFG_I2C4HAL__I2C_SCL_PIN;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //GPIO_Mode_AF_OD;
//   GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//   GPIO_Init(HAL_BRDCFG_I2C4HAL__I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

//   /*!< Configure sEE_I2C pins: SDA */
//   GPIO_InitStructure.GPIO_Pin = HAL_BRDCFG_I2C4HAL__I2C_SDA_PIN;
//   GPIO_Init(HAL_BRDCFG_I2C4HAL__I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
// }

#endif//HAL_USE_I2C4HAL


#ifdef HAL_USE_EEPROM


extern void hal_brdcfg_eeprom__ontimeouterror(void)
{
    hal_base_hid_on_fatalerror(hal_fatalerror_runtimefault, "timeout in eeprom");
}

// extern void hal_brdcfg_eeprom__writeprotection_init(void)
// {
//     ;
// }

// extern void hal_brdcfg_eeprom__writeprotection_disable(void)
// {
//     ;
// }
// extern void hal_brdcfg_eeprom__writeprotection_enable(void)
// {
//     ;
// }

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

extern void hal_brdcfg_switch__MCO_config(void)
{
    // dummy
    #error -> mcbstm32c does not have a switch
}

extern void hal_brdcfg_switch__reg_read_byI2C(uint8_t* pBuffer, uint16_t ReadAddr)
{
    // dummy
    #error -> mcbstm32c does not have a switch
}

extern void hal_brdcfg_switch__reg_write_byI2C(uint8_t* pBuffer, uint16_t WriteAddr)
{
    // dummy
    #error -> mcbstm32c does not have a switch
}

#endif//HAL_USE_SWITCH



#ifdef HAL_USE_ETH

//#define HAL_BRDCFG_ETH__PHYDEV_IS_IN_RESET_STATE(x)	    (x & 0x8800)

extern void hal_brdcfg_eth__phy_initialise(void)
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
    }    

}

extern void hal_brdcfg_eth__phy_configure(void)
{

    // configure phy in full duplex and 100MB
    hal_eth_hid_smi_write(HAL_BRDCFG_ETH__PHYDEV_ADR, HAL_BRDCFG_ETH__PHYREG_BMRC_ADR, HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_FULLDUPLEX | HAL_BRDCFG_ETH__PHYREG_BMRC_VAL_100M); 
 
//     uint16_t regv;
//     uint32_t tout;
//     static uint16_t aaa = 0;    
//     regv = hal_eth_hid_smi_read(HAL_BRDCFG_ETH__PHYDEV_ADR, 2);    
//     aaa = regv;
//     for(;;);
}


#endif//HAL_USE_ETH

#ifdef HAL_USE_SYS
extern void hal_brdcfg_sys__clock_config(void)
{
    /* On this board this function is dummy, because it is not not necessary 
       configure MCO or something else
     */
    #warning --> a cosa serve il hal_brdcfg_sys__clock_config() ?? il MCO viene configurato in hal_eth_hid_microcontrollerclockoutput_init() 
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


