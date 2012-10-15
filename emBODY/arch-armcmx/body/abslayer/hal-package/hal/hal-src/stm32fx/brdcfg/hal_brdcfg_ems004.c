
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



#ifdef HAL_USE_ETH

    #define HAL_BRDCFG_ETH__ETH_IS_IN_RESET_STATE(x)	    (x & 0x8800)

//     // acemor on 16 sept 2011: the following are un-used ...
//     #define HAL_BRDCFG_ETH__PHY_REG_ANER        0x06        /* Auto-Neg. Expansion Register      */
//     #define HAL_BRDCFG_ETH__PHY_REG_ANNPTR      0x07        /* Auto-Neg. Next Page TX            */
//     /* PHY Extended Registers */
//     #define HAL_BRDCFG_ETH__PHY_REG_STS         0x10        /* Status Register                   */
//     #define HAL_BRDCFG_ETH__PHY_REG_MICR        0x11        /* MII Interrupt Control Register    */
//     #define HAL_BRDCFG_ETH__PHY_REG_MISR        0x12        /* MII Interrupt Status Register     */
//     #define HAL_BRDCFG_ETH__PHY_REG_FCSCR       0x14        /* False Carrier Sense Counter       */
//     #define HAL_BRDCFG_ETH__PHY_REG_RECR        0x15        /* Receive Error Counter             */
//     #define HAL_BRDCFG_ETH__PHY_REG_PCSR        0x16        /* PCS Sublayer Config. and Status   */
//     #define HAL_BRDCFG_ETH__PHY_REG_RBR         0x17        /* RMII and Bypass Register          */
//     #define HAL_BRDCFG_ETH__PHY_REG_LEDCR       0x18        /* LED Direct Control Register       */
//     #define HAL_BRDCFG_ETH__PHY_REG_PHYCR       0x19        /* PHY Control Register              */
//     #define HAL_BRDCFG_ETH__PHY_REG_10BTSCR     0x1A        /* 10Base-T Status/Control Register  */
//     #define HAL_BRDCFG_ETH__PHY_REG_CDCTRL1     0x1B        /* CD Test Control and BIST Extens.  */
//     #define HAL_BRDCFG_ETH__PHY_REG_EDCR        0x1D        /* Energy Detect Control Register    */
//     #define AL_BRDCFG_ETH__DP83848C_DEF_ADR    0x01        /* Default PHY device address        */
//     #define AL_BRDCFG_ETH__DP83848C_ID         0x20005C90  /* PHY Identifier                    */
//     //#define PHY_address 		DP83848C_DEF_ADR
//     #define AL_BRDCFG_ETH__HAL_ETH_RESET_STATE_MASK		0x77FF//0x8800 DA SISTEMARE!!!!
//     // acemor on 16 sept 2011: the previous are un-used ...

#endif//HAL_USE_ETH

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

#ifdef HAL_USE_CAN
    extern const uint8_t hal_brdcfg_can__supported_mask             = 0x03;
    #warning --> mettere i gpio ...
    extern const uint32_t hal_brdcfg_can__gpio_clock_canx_rx[]      = {RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOB};
    extern const uint32_t hal_brdcfg_can__gpio_clock_canx_tx[]      = {RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOB};
    extern const uint8_t hal_brdcfg_can__gpio_pinsource_canx_rx[]   = {0, 5};
    extern const uint8_t hal_brdcfg_can__gpio_pinsource_canx_tx[]   = {1, 13};
    extern const uint16_t hal_brdcfg_can__gpio_pin_canx_rx[]        = {GPIO_Pin_0, GPIO_Pin_5};
    extern const uint16_t hal_brdcfg_can__gpio_pin_canx_tx[]        = {GPIO_Pin_1, GPIO_Pin_6};
    extern GPIO_TypeDef* const hal_brdcfg_can__gpio_port_canx_rx[]  = {GPIOD, GPIOB};
    extern GPIO_TypeDef* const hal_brdcfg_can__gpio_port_canx_tx[]  = {GPIOD, GPIOB};       

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
    
    extern const stm32gpio_gpio_t hal_brdcfg_eth__gpio_ETH_RMII_REF_CLK = { .port = stm32gpio_portA, .pin = stm32gpio_pin1  }; 
    
    extern const stm32gpio_gpio_t hal_brdcfg_eth__gpio_ETH_RMII_TX_EN   = { .port = stm32gpio_portB, .pin = stm32gpio_pin11 };
    extern const stm32gpio_gpio_t hal_brdcfg_eth__gpio_ETH_RMII_TXD0    = { .port = stm32gpio_portB, .pin = stm32gpio_pin12 };
    extern const stm32gpio_gpio_t hal_brdcfg_eth__gpio_ETH_RMII_TXD1    = { .port = stm32gpio_portB, .pin = stm32gpio_pin13 };
    
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
            .device             = stm32ee_device_atmel_at24c1024b,
            .i2cport            = 1,        // is equivalent to hal_i2c_port1
            .hwaddra2a1a0       = 0,        // a0 = a1 = a2 = 0
            .wpval              = stm32gpio_valHIGH,        // write protection is on value high       
            .wppin              =
            {
                .port               = stm32gpio_portD,
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
    extern const uint8_t hal_brdcfg_spi4encoder__supported_mask = 0x05;  // spi1 and spi3
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


#ifdef HAL_USE_I2C4HAL


extern void hal_brdcfg_i2c4hal__ontimeouterror(void)
{
    hal_base_hid_on_fatalerror(hal_fatalerror_runtimefault, "timeout in i2c");
}

#endif//HAL_USE_I2C4HAL


#ifdef HAL_USE_EEPROM


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

static void hal_brdcfg_switch__mco2_init(void)
{
    // configure pc9 af, push pull, 50mhz
    // enable system configuration controller clock
    RCC->APB2ENR    |= (1 << 14);  
    // clocks port port c
    RCC->AHB1ENR    |= 0x00000004;

    GPIOC->MODER    &= ~0x000C0000;              // reset pc9
    GPIOC->MODER    |=  0x00080000;              // alternate function
    GPIOC->OTYPER   &= ~0x00000200;              // output push-pull (reset state)  
    GPIOC->OSPEEDR  |=  0x00080000;              // slew rate as 100MHz pin (0x000C0000) or 50mhz (0x00080000)
    GPIOC->PUPDR    &= ~0x000C0000;              // no pull up, pull down

    GPIOC->AFR[1]   &= ~0x000000F0;
    GPIOC->AFR[1]   |=  0x00000000;              // AF0 (system) 

}

static void hal_brdcfg_switch__mco_initialise(void)
{
#if 0
    // this function initialises MCO in order to provide clock ref to switch.
    // PA8 is MCO. it must be configured = Output mode, max speed 50 MHz + Alternate function output Push-pull (B)
    // also, we connect pll3 at 50mhz to it
    
    // clock gpioa as alternate function
    RCC->APB2ENR    |= 0x00000005;
    // init pa8
    GPIOA->CRH   &= 0xFFFFFFF0;
    GPIOA->CRH   |= 0x0000000B;	


    // set pll3 clock output to 50mhz: (25mhz/5)*10 = 50mhz, thus we use multiplier 10
    RCC_PLL3Config(RCC_PLL3Mul_10);
        
    // enable pll3 
    RCC_PLL3Cmd(ENABLE);
    
    // wait until it is ready
    while(RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) == RESET);
    
    // connect mco on pa8 with pll3
    RCC_MCOConfig(RCC_MCO_PLL3CLK);
#endif  

#if 0
    // configure pc9 af, push pull, 50mhz
    // enable system configuration controller clock
    RCC->APB2ENR    |= (1 << 14);  
    // clocks port port c
    RCC->AHB1ENR    |= 0x00000004;

    GPIOC->MODER    &= ~0x000C0000;              // reset pc9
    GPIOC->MODER    |=  0x00080000;              // alternate function
    GPIOC->OTYPER   &= ~0x00000200;              // output push-pull (reset state)  
    GPIOC->OSPEEDR  |=  0x00080000;              // slew rate as 100MHz pin (C) or 50mhz (8)
    GPIOC->PUPDR    &= ~0x000C0000;              // no pull up, pull down

    GPIOC->AFR[1]   &= ~0x000000F0;
    GPIOC->AFR[1]   |=  0x00000000;              // AF0 (system) 
#endif

    //hal_brdcfg_switch__mco1_init();
   
    hal_brdcfg_switch__mco2_init();
     
    RCC_PLLI2SCmd(DISABLE);
    RCC_PLLI2SConfig(200, 2); // 50mhz: 1mhz*200/10 = 100
    RCC_PLLI2SCmd(ENABLE);
    // wait until it is ready
    while(RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == RESET);
    // connect mco2 with plli2s divided by 2
    RCC_MCO2Config(RCC_MCO2Source_PLLI2SCLK, RCC_MCO2Div_2);

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
//    hal_brdcfg_switch__mco_initialise();  
    
     // --- reset pin: PB2
#if 0    
    // clock gpioa as normal gpio, reset pin 2, and set it as: Output mode, max speed 2 MHz + General purpose output push-pull
    do it ...
    // put in reset state (low) for some time ... 10 ms according to datasheet.
    do it ...
    hal_sys_delay(10*1000);
    // put value high to exit from reset state
    do it ...
    // now wait for 100 usec before using i2c etc.
    hal_sys_delay(100);
#else
    #define F 1
    hal_gpio_init(hal_gpio_portB, hal_gpio_pin2, hal_gpio_dirOUT, hal_gpio_speed_low);
    hal_gpio_setval(hal_gpio_portB, hal_gpio_pin2, hal_gpio_valLOW);
    hal_sys_delay(F*10*1000);
    hal_gpio_setval(hal_gpio_portB, hal_gpio_pin2, hal_gpio_valHIGH);
    hal_sys_delay(F*100);
#endif    
  
    // proviamo anche questo ....
//    hal_eth_hid_rmii_refclock_init();
    
    // --- mco: PC9
    hal_brdcfg_switch__mco_initialise();  
    
    //hal_brdcfg_switch__rmii_init();

    // --- i2c for communication 
    hal_i2c4hal_init(hal_i2c_port1, NULL); // use default configuration    

}

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


