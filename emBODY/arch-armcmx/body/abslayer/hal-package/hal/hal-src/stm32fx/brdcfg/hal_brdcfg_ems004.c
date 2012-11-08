
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

#include "hal_stm32xx_include.h"

#include "hal.h"
#include "stdlib.h"
#include "string.h"
#include "hal_base.h"
#include "hal_stm32_base_hid.h"
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

#include "hal_brdcfg_ems004.h"


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



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

#ifdef HAL_USE_CAN
    extern const uint8_t hal_brdcfg_can__supported_mask             = 0x03;
//    #warning --> mettere i gpio ...
//    extern const uint32_t hal_brdcfg_can__gpio_clock_canx_rx[]      = {RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOB};
//    extern const uint32_t hal_brdcfg_can__gpio_clock_canx_tx[]      = {RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOB};
//    extern const uint8_t hal_brdcfg_can__gpio_pinsource_canx_rx[]   = {0, 5};
//    extern const uint8_t hal_brdcfg_can__gpio_pinsource_canx_tx[]   = {1, 13};
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
            .pin      = hal_gpio_pin13,        
            .dir      = hal_gpio_dirALT,
            .speed    = hal_gpio_speed_default        
        }
    };    

#warning --> sarebbe meglio usare quanto in hal_gpio.h per definire i pin dei vari can1rx, can1tx, can1en etc. poi dentro i file .c usare i clock, pin, etc prendendoli dai s_GPIO_CLK[], s_GPIO_PORT[] etc resi globali e messi in hal_base.c    
#endif//HAL_USE_CAN

#ifdef HAL_USE_CRC
    extern const uint8_t hal_brdcfg_crc__supported_mask             = 0x03;
#endif//HAL_USE_CRC

#ifdef HAL_USE_DISPLAY
    extern const uint8_t hal_brdcfg_display__supported_mask         = 0x00;
#endif//HAL_USE_DISPLAY

#ifdef HAL_USE_ETH
    extern const uint8_t hal_brdcfg_eth__supported_mask                 = 0x01;
    
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_RMII_REF_CLK   = { .port = hal_gpio_portA, .pin = hal_gpio_pin1,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max  }; 
    
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_RMII_TX_EN     = { .port = hal_gpio_portB, .pin = hal_gpio_pin11,  .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max };
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_RMII_TXD0      = { .port = hal_gpio_portB, .pin = hal_gpio_pin12,  .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max };
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_RMII_TXD1      = { .port = hal_gpio_portB, .pin = hal_gpio_pin13,  .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max };
    
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_RMII_CRS_DV    = { .port = hal_gpio_portA, .pin = hal_gpio_pin7,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max };
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_RMII_RXD0      = { .port = hal_gpio_portC, .pin = hal_gpio_pin4,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max };
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_RMII_RXD1      = { .port = hal_gpio_portC, .pin = hal_gpio_pin5,  .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max };    
    
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_MDC            = { .port = hal_gpio_portC, .pin = hal_gpio_pin1,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max  };
    extern const hal_gpio_cfg_t hal_brdcfg_eth__gpio_ETH_MDIO           = { .port = hal_gpio_portA, .pin = hal_gpio_pin2,   .dir = hal_gpio_dirALT, .speed = hal_gpio_speed_max  };   
   
    extern const hal_eth_phymode_t hal_brdcfg_eth__phymode              = hal_eth_phymode_fullduplex100mbps;
#endif//HAL_USE_ETH


#ifdef HAL_USE_EEPROM
    extern const uint8_t  hal_brdcfg_eeprom__supported_mask             = (1 << hal_eeprom_i2c_01);
    extern const uint32_t hal_brdcfg_eeprom__emflash_baseaddress        = 0x08000000;
    extern const uint32_t hal_brdcfg_eeprom__emflash_totalsize          = 0;
    extern const uint32_t hal_brdcfg_eeprom__i2c_01_baseaddress         = 0;
    extern const uint32_t hal_brdcfg_eeprom__i2c_01_totalsize           = 8*1024; 
    extern const hal_eeprom_hw_cfg_t hal_brdcfg_eeprom__i2c_01_device   =
    {
        .device             = hal_eeprom_device_atmel_at24c1024b,
        .hwaddra2a1a0       = 0        // a0 = a1 = a2 = 0                                
    };    
#endif//HAL_USE_EEPROM 


#ifdef HAL_USE_ENCODER
    extern const uint32_t hal_brdcfg_encoder__supported_mask = 0x01ff; // tutti e 9 gli encoder ...
#endif//HAL_USE_ENCODER


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


    
#ifdef HAL_USE_I2C
    extern const uint8_t hal_brdcfg_i2c__supported_mask = (1 << hal_i2c_port1); 
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
    extern const hal_gpio_val_t hal_brdcfg_led__value_on          = hal_gpio_valLOW;
    extern const hal_gpio_val_t hal_brdcfg_led__value_off         = hal_gpio_valHIGH;
    extern const uint8_t hal_brdcfg_led__supported_mask           = 0x3F;
    extern const hal_gpio_cfg_t hal_brdcfg_led__cfg[hal_leds_num] = 
    {
        {   // hal_led0 - red
            .port     = hal_gpio_portE,
            .pin      = hal_gpio_pin8,        
            .dir      = hal_gpio_dirOUT,
            .speed    = hal_gpio_speed_low
        },
        {   // hal_led1 - green
            .port     = hal_gpio_portE,
            .pin      = hal_gpio_pin10,        
            .dir      = hal_gpio_dirOUT,
            .speed    = hal_gpio_speed_low
        },
        {   // hal_led2 - yellow
            .port     = hal_gpio_portE,
            .pin      = hal_gpio_pin12,        
            .dir      = hal_gpio_dirOUT,
            .speed    = hal_gpio_speed_low
        },
        {   // hal_led3 - orange
            .port     = hal_gpio_portE,
            .pin      = hal_gpio_pin15,        
            .dir      = hal_gpio_dirOUT,
            .speed    = hal_gpio_speed_low
        },
        {   // hal_led4 - can1
            .port     = hal_gpio_portD,
            .pin      = hal_gpio_pin11,        
            .dir      = hal_gpio_dirOUT,
            .speed    = hal_gpio_speed_low
        },
        {   // hal_led5 - can2 
            .port     = hal_gpio_portB,
            .pin      = hal_gpio_pin7,        
            .dir      = hal_gpio_dirOUT,
            .speed    = hal_gpio_speed_low
        }        
    };
#endif//HAL_USE_LED

#ifdef HAL_USE_SPI4ENCODER
    extern const uint8_t hal_brdcfg_spi4encoder__supported_mask = 0x05;  // spi1 and spi3
#endif//HAL_USE_SPI4ENCODER


#ifdef HAL_USE_SWITCH
    extern const hal_boolval_t hal_brdcfg_switch__supported         = hal_true;
    extern const hal_gpio_cfg_t hal_brdcfg_switch__gpio_reset       = { .port = hal_gpio_portB, .pin = hal_gpio_pin2,   .dir = hal_gpio_dirOUT, .speed = hal_gpio_speed_low  }; 
#endif//HAL_USE_SWITCH
    

#ifdef  HAL_USE_SYS
    extern const hal_sys_hid_clock_cfg_t hal_brdcfg_sys__clockcfg =
    {
        .sourceclock        = hal_sys_refclock_pll_on_external_osc, 
        .intclockspeed      = 16000000,
        .extclockspeed      = 50000000,
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
            .p                  = 2,
            .q                  = 7
        }
    };        
#endif//HAL_USE_SYS    

    
#ifdef HAL_USE_TIMER
    extern const uint8_t hal_brdcfg_timer__supported_mask           = 
    (0 << hal_timer1) | (1 << hal_timer2) | (1 << hal_timer3) | (1 << hal_timer4) |
    (1 << hal_timer5) | (1 << hal_timer6) | (1 << hal_timer7);
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

#ifdef HAL_USE_CAN

extern void hal_brdcfg_can__phydevices_init(hal_can_port_t port)
{
    // i could instead use hal_gpio_init(hal_gpio_portB, hal_gpio_pinxx, hal_gpio_speed_low); 

    GPIO_InitTypeDef hal_brdcfg_can__gpio_canx_enable =
    {   // output low speed
        .GPIO_Pin       = 0,
        .GPIO_Mode      = GPIO_Mode_OUT,
        .GPIO_Speed     = GPIO_Speed_2MHz,
        .GPIO_OType     = GPIO_OType_PP,
        .GPIO_PuPd      = GPIO_PuPd_UP
    };
    
	if(hal_can_port1 == port)
	{   // PB14
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        hal_brdcfg_can__gpio_canx_enable.GPIO_Pin = GPIO_Pin_14;
        GPIO_Init(GPIOB, &hal_brdcfg_can__gpio_canx_enable);
	}
	else if(hal_can_port2 == port) 
	{   // PB15
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
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

#endif//HAL_USE_CAN


#ifdef HAL_USE_EEPROM

static const hal_gpio_val_t hal_brdcfg_eeprom__wp_gpio_enableval    = hal_gpio_valHIGH;      // write protection is on value high                          
static const hal_gpio_cfg_t hal_brdcfg_eeprom__wp_gpio_cfg          =
{
    .port               = hal_gpio_portD,
    .pin                = hal_gpio_pin10,
    .dir                = hal_gpio_dirOUT,
    .speed              = hal_gpio_speed_high
};

extern hal_result_t hal_brdcfg_eeprom__wp_init(void)
{
    return(hal_gpio_configure(hal_brdcfg_eeprom__wp_gpio_cfg, NULL));
}

extern hal_result_t hal_brdcfg_eeprom__wp_enable(void)
{
    hal_gpio_val_t enable = hal_brdcfg_eeprom__wp_gpio_enableval;
    return(hal_gpio_setval(hal_brdcfg_eeprom__wp_gpio_cfg.port, hal_brdcfg_eeprom__wp_gpio_cfg.pin, enable));
}

extern hal_result_t hal_brdcfg_eeprom__wp_disable(void)
{
    hal_gpio_val_t disable = (hal_gpio_valHIGH == hal_brdcfg_eeprom__wp_gpio_enableval) ? hal_gpio_valLOW : hal_gpio_valHIGH;
    return(hal_gpio_setval(hal_brdcfg_eeprom__wp_gpio_cfg.port, hal_brdcfg_eeprom__wp_gpio_cfg.pin, disable));
}

#endif//HAL_USE_EEPROM


#ifdef HAL_USE_I2C

extern void hal_brdcfg_i2c__ontimeouterror(void)
{
    hal_base_hid_on_fatalerror(hal_fatalerror_runtimefault, "timeout in i2c");
}

#endif//HAL_USE_I2C



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

#if 0
static void hal_brdcfg_switch__mco1_init(void)
{
    // configure pa8 af, push pull, 50mhz
    // enable system configuration controller clock
    RCC->APB2ENR    |= (1 << 14);  
    // clocks port port a
    RCC->AHB1ENR    |= 0x00000001;

    GPIOA->MODER   &= ~0x00030000;              // reset pa8
    GPIOA->MODER   |=  0x00020000;              // alternate function
    GPIOA->OTYPER  &= ~0x00000100;              // output push-pull (reset state)  
    GPIOA->OSPEEDR |=  0x00030000;              // slew rate as 100MHz pin (3) or 50mhz (2)
    GPIOA->PUPDR   &= ~0x00030000;              // no pull up, pull down

    GPIOA->AFR[1]  &= ~0x0000000F;
    GPIOA->AFR[1]  |=  0x00000000;              // AF0 (system) 

}
#endif

static void hal_brdcfg_switch__mco2_init(void)
{
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

#define HAL_SWITCH_USE_HSE

static void hal_brdcfg_switch__mco_initialise(void)
{
   
    hal_brdcfg_switch__mco2_init();

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

void hal_brdcfg_switch__rmii_init(void)
{
    hal_eth_hid_rmii_prepare();
    hal_eth_hid_rmii_rx_init(); 
    hal_eth_hid_rmii_tx_init(); 
    hal_eth_hid_rmii_refclock_init();  
    hal_eth_hid_smi_init();    
}

extern void hal_brdcfg_switch__initialise(void)
{
    
    // --- mco2: PC9
    hal_brdcfg_switch__mco_initialise();  
    
     // --- reset pin: PB2
    hal_gpio_configure(hal_brdcfg_switch__gpio_reset, NULL);
    hal_gpio_setval(hal_brdcfg_switch__gpio_reset.port, hal_brdcfg_switch__gpio_reset.pin, hal_gpio_valLOW);
    hal_sys_delay(10*1000);
    hal_gpio_setval(hal_brdcfg_switch__gpio_reset.port, hal_brdcfg_switch__gpio_reset.pin, hal_gpio_valHIGH);
    hal_sys_delay(100);
  
    // proviamo anche questo ....
//    hal_eth_hid_rmii_refclock_init();
    
    // --- mco: PC9
//    hal_brdcfg_switch__mco_initialise();  
    
    hal_brdcfg_switch__rmii_init();

    // --- i2c for communication 
    hal_i2c_init(hal_i2c_port1, NULL); // use default configuration    

}

static void s_hal_brdcfg_switch__mii_phymode_get(hal_eth_phymode_t* phymode)
{
    #define MIIHALFD    (1 << 6)
    #define MII10MBS    (1 << 4)
    uint8_t mux = 0;
    uint8_t speed = 0;
    uint8_t read = 0xFF; 
    hal_i2c_regaddr_t regadr = {.numofbytes = 1, .bytes.one = 0};
    regadr.bytes.one = 0x06;
    hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &read, 1);
    if( (MIIHALFD & read) == MIIHALFD)
    {
        mux = 0;
    }
    else
    {
        mux = 1;
    }
    if( (MII10MBS & read) == MII10MBS)
    {
        speed = 0;
    }
    else
    {
        speed = 1;
    }
    if((0==mux)&&(0==speed))        *phymode = hal_eth_phymode_halfduplex10mbps;
    else if((0==mux)&&(1==speed))   *phymode = hal_eth_phymode_halfduplex100mbps; 
    else if((1==mux)&&(0==speed))   *phymode = hal_eth_phymode_fullduplex10mbps; 
    else if((1==mux)&&(1==speed))   *phymode = hal_eth_phymode_fullduplex100mbps;          
    
}
 
extern void hal_brdcfg_switch__configure(hal_eth_phymode_t* phymode)
{
    hal_eth_phymode_t phymode2use = hal_eth_phymode_fullduplex100mbps;
    const uint8_t fd100 = 0x60;
    const uint8_t fd010 = 0x20;
    uint8_t buff_write = 0x60; // FORCE FULL DUPLEX AND 100T
    uint8_t buff_read = 0xFF; 
    volatile uint32_t i = 1;
    hal_i2c_regaddr_t regadr = {.numofbytes = 1, .bytes.one = 0};
    
    if(NULL != phymode)
    {
        phymode2use = *phymode;
    }


    regadr.bytes.one = 0x01;
    hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
    if((buff_read&0x01))
    {   // already initted. to be initted again must pass through a reset
        s_hal_brdcfg_switch__mii_phymode_get(phymode);
        return;
    }
    
    
    if(hal_eth_phymode_none == phymode2use)
    {
        if(NULL != phymode)
        {
            *phymode = hal_eth_phymode_none;    
        }   
        return;        
    }


    

#if 0    
    // configure mii
    regadr.bytes.one = 0x06;
    hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
    buff_write  = buff_read;
    buff_write |= 0x10; 
    hal_i2c_write(hal_i2c_port1, 0xBE, regadr, &buff_write, 1);
    hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
#endif

    // 1. configure  switch's ports 1 and 2
    switch(phymode2use)
    {
        case hal_eth_phymode_auto:                  buff_write = 0x9F; break;
        case hal_eth_phymode_halfduplex10mbps:      buff_write = 0x00; break;
        case hal_eth_phymode_halfduplex100mbps:     buff_write = 0x40; break;
        case hal_eth_phymode_fullduplex10mbps:      buff_write = 0x20; break;
        case hal_eth_phymode_fullduplex100mbps:     buff_write = 0x60; break;
        default:                                    buff_write = 0x00; break;
    }    
    
    
    // port 1
    regadr.bytes.one = 0x1C;
    hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
    hal_i2c_write(hal_i2c_port1, 0xBE, regadr, &buff_write, 1);
    hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
     
    // port 2 
    regadr.bytes.one = 0x2C;
    hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
    hal_i2c_write(hal_i2c_port1, 0xBE, regadr, &buff_write, 1);   
    hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);

    // 2. start the switch
    buff_write = 0x1;  
    regadr.bytes.one = 0x01;    
    hal_i2c_write(hal_i2c_port1, 0xBE, regadr, &buff_write, 1);
    

    // 3. read back to verify
    regadr.bytes.one = 0x01;
    hal_i2c_read(hal_i2c_port1, 0xBE, regadr, &buff_read, 1);
    if(!(buff_read&0x01))
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_runtimefault, "s_hal_switch_reg_config(): SWITCH not configured");
    }
    
    s_hal_brdcfg_switch__mii_phymode_get(phymode);
    
}

#endif//HAL_USE_SWITCH



#ifdef HAL_USE_ETH

extern hal_bool_t hal_brdcfg_eth__phy_initialise(void)
{
    #warning --> in here we just init the switch ... put in reset mode, exit from reset, do the mco, init the i2c if not initted, etc. 
    if(hal_false == hal_switch_initted_is())
    {
        hal_switch_init(NULL);
    }
    
    return(hal_true);
}

extern void hal_brdcfg_eth__phy_configure(hal_eth_phymode_t *phymode)
{
    hal_eth_phymode_t target = hal_brdcfg_eth__phymode;
 
//    if((hal_eth_mux_auto == target.mux) || (hal_eth_mux_none == target.mux) || (hal_eth_speed_auto == target.speed) || (hal_eth_speed_none == target.speed))
    if(hal_eth_phymode_none == target)
    {
        if(NULL != phymode)
        {
            *phymode  = hal_eth_phymode_none;
        }   
        return;        
    }    
    
    hal_switch_start(&target);
    
    if(NULL != phymode)
    {
        *phymode  = target;     
    }
}

#endif//HAL_USE_ETH

#ifdef HAL_USE_SYS
extern void hal_brdcfg_sys__clock_config(void)
{

}


extern void hal_brdcfg_sys__gpio_default_init(void)
{

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


