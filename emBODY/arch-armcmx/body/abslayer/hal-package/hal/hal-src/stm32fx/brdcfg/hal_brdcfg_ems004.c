
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
    extern const uint8_t hal_brdcfg_eth__supported_mask             = 0x01;
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
            .wpval              = 1,        // write protection is on value high       
            .wppin              =
            {
                .port               = stm32ee_gpio_portD,
                .pin                = stm32ee_gpio_pin10
            }, 
            .functionontimeout  = hal_brdcfg_eeprom__ontimeouterror //NULL                        
        },

        .i2ccfg             =
        {
            .i2cinit            = NULL,
            .i2cdeinit          = NULL,
            .i2cpar             = NULL
        },
        .dmacfg             =
        {
            .dontuse            = 0
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
            .port               = stm32i2c_gpio_portB,
            .pin                = stm32i2c_gpio_pin8
        }, 
        .sda                =
        {
            .port               = stm32i2c_gpio_portB,
            .pin                = stm32i2c_gpio_pin9
        }                    
    
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

extern void hal_brdcfg_eth__phy_start(void)
{
    uint8_t i;
    uint16_t regv;
    uint32_t tout;

//     // questo e' per la stm32f1
// //    // configure pin PA1 (Reference clock for switch)
// //    GPIOA->CRL   &= 0xFFFFFF0F;
// //    GPIOA->CRL   |= 0x00000040;    /* set pin 1 in reset state and 2 in alternFunc 50MHz */

//     
//     // init the ETH_RMII_REF_CLK (PA1)
//     GPIOA->MODER    &= ~0x0000000C;              // reset pa1
//     GPIOA->MODER    |=  0x00000008;              /* Pin to alternate function */
//     GPIOA->OTYPER   &= ~0x00000002;              /* Pins in push-pull mode     */
//     GPIOA->OSPEEDR  |=  0x0000000C;              /* Slew rate as 100MHz pin    */
//     GPIOA->PUPDR    &= ~0x0000000C;              /* No pull up, no pull down   */

//     GPIOA->AFR[0]   &= ~0x000000F0;
//     GPIOA->AFR[0]   |=  0x000000B0;              /* Pin to AF11 (Ethernet)    */
    

#if defined(ONEWANTS2_INIT_REFCLOCK_IN_HERE)
    hal_eth_hid_rmii_refclock_init();   
#endif

#if defined(ONEWANTS2_INIT_SMI_IN_HERE)
    hal_eth_hid_smi_init();
#endif  

	// reset the phy device (address is 1)
    hal_eth_hid_smi_write(0x01, PHY_REG_BMCR, PHY_Reset);

	// wait for hardware reset to end 
    for(tout = 0; tout<HAL_ETH_PHY_WR_TIMEOUT; tout++) 
    {
        regv = hal_eth_hid_smi_read(0x01, PHY_REG_BMCR);
        if (!HAL_BRDCFG_ETH__ETH_IS_IN_RESET_STATE(regv))
        {
            // reset complete
            break;
        }
    }
    
    if(HAL_ETH_PHY_WR_TIMEOUT == tout) //ethernet is still in reset state 
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_runtimefault, "hal_brdcfg_eth__phy_start(): PHY is still in reset state");
    }


    // configure phy in full duplex and 100MB
    hal_eth_hid_smi_write(0x01, PHY_REG_BMCR, PHY_FULLD_100M); 
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


