/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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


/* @file       stm32i2c.c
	@brief      This file implements init / deinit of i2c using stm32 apis.
	@author     marco.accame@iit.it
    @date       09/24/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
//#include "hal_brdcfg_modules.h"

//#ifdef HAL_USE_I2C4HAL

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_stm32xx_include.h"

//#include "hal_brdcfg.h"
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "stm32i2c.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 

//#include "stm32i2c_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

// extern const stm32i2c_cfg_t stm32i2c_cfg_default = 
// { 
//     0
// };



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



typedef struct
{
    uint32_t    clock;
    uint32_t    gpio_remap;
    uint32_t    gpio_remap_clock;
} stm32i2c_i2cx_cfg_t;

typedef struct
{
    uint32_t            clock;
    GPIO_TypeDef*       port;
    uint16_t            pinnum;
    GPIO_InitTypeDef    pin;
} stm32i2c_i2c_gpio_cfg_t;

typedef struct
{
    uint8_t                     initted[3]; // i2c1, i2c2, i2c3
    stm32i2c_cfg_t              cfg;
    I2C_TypeDef*                i2cx;
    stm32i2c_i2cx_cfg_t         i2cx_cfg;
    stm32i2c_i2c_gpio_cfg_t     i2c_gpio_scl;
    stm32i2c_i2c_gpio_cfg_t     i2c_gpio_sda;
} stm32i2c_generic_container_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static stm32i2c_result_t s_stm32i2c_i2c_common_init(const stm32i2c_cfg_t* i2ccfg);
static stm32i2c_result_t s_stm32i2c_i2c_common_deinit(const stm32i2c_cfg_t* i2ccfg);


static stm32i2c_result_t s_stm32i2c_i2c_prepare_data(const stm32i2c_cfg_t* i2ccfg, const uint8_t usefloatingpins);

static void s_stm32i2c_i2c_gpio_init(void);
static void s_stm32i2c_i2c_enable(void);


static void s_stm32i2c_i2c_disable(void);
static void s_stm32i2c_i2c_gpio_deinit(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static stm32i2c_generic_container_t s_stm32i2c_generics = 
{
    .initted                = {0, 0, 0},
    .cfg                    = 
    {
        .speed                          = 0,
        .scl                            = 
        {
            .port                           = stm32i2c_gpio_portNONE,
            .pin                            = stm32i2c_gpio_pinNONE
        },
        .sda                            =
        {
            .port                           = stm32i2c_gpio_portNONE,
            .pin                            = stm32i2c_gpio_pinNONE        
        }   
    },
    .i2cx                   = NULL,
    .i2cx_cfg               =
    {
        .clock                          = 0,
        .gpio_remap                     = 0,
        .gpio_remap_clock               = 0
    }
};


static const I2C_InitTypeDef   s_stm32i2c_i2c_cfg       =
{
    .I2C_Mode                   = I2C_Mode_I2C,
    .I2C_DutyCycle              = I2C_DutyCycle_2,
    .I2C_OwnAddress1            = 0,
    .I2C_Ack                    = I2C_Ack_Enable,
    .I2C_AcknowledgedAddress    = I2C_AcknowledgedAddress_7bit,
    .I2C_ClockSpeed             = 400000            // PARAMETER
};


#if     defined(USE_STM32F1)

static const uint8_t s_stm32i2c_maxports                    = 2;

static I2C_TypeDef* const       s_stm32i2c_i2cx_port[3]               = {I2C1, I2C2, NULL};
static const uint32_t           s_stm32i2c_i2cx_clock[3]              = {RCC_APB1Periph_I2C1, RCC_APB1Periph_I2C2, 0};  
static const uint32_t           s_stm32i2c_i2cx_gpio_remap_clock[3]   = {RCC_APB2Periph_AFIO, 1, 1}; // 1 means that i dont know if it is correct     
static const uint32_t           s_stm32i2c_i2cx_gpio_remap[3]         = {GPIO_Remap_I2C1, GPIO_Remap_I2C1, GPIO_Remap_I2C1};


static const uint32_t s_stm32i2c_i2c_gpio_thegpioclocks[]    = { RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB, RCC_APB2Periph_GPIOC, 
                                                                RCC_APB2Periph_GPIOD, RCC_APB2Periph_GPIOE, RCC_APB2Periph_GPIOF, 
                                                                RCC_APB2Periph_GPIOG };  
static GPIO_TypeDef* const s_stm32i2c_i2c_gpio_thegpioports[]= { GPIOA, GPIOB, GPIOC, 
                                                                GPIOD, GPIOE, GPIOF, 
                                                                GPIOG };  
static const uint8_t  s_stm32i2c_i2c_gpio_thepinnums[]       = { 0,  1,  2,  3,
                                                                4,  5,  6,  7,
                                                                8,  9,  10, 11,
                                                                12, 13, 14, 15 }; 
static const uint16_t  s_stm32i2c_i2c_gpio_thepins[]         = { GPIO_Pin_0,  GPIO_Pin_1,  GPIO_Pin_2,  GPIO_Pin_3,
                                                                GPIO_Pin_4,  GPIO_Pin_5,  GPIO_Pin_6,  GPIO_Pin_7,
                                                                GPIO_Pin_8,  GPIO_Pin_9,  GPIO_Pin_10, GPIO_Pin_11,
                                                                GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15 }; 
                                                                
 
static const GPIO_InitTypeDef  s_stm32i2c_i2c_gpio_sxx_pin  =
{
    .GPIO_Pin       = GPIO_Pin_All,
    .GPIO_Speed     = GPIO_Speed_50MHz,
    .GPIO_Mode      = GPIO_Mode_AF_OD,
}; 

static const GPIO_InitTypeDef  s_stm32i2c_i2c_gpio_sxx_floatpin  =
{
    .GPIO_Pin       = GPIO_Pin_All,
    .GPIO_Speed     = GPIO_Speed_2MHz,
    .GPIO_Mode      = GPIO_Mode_IN_FLOATING,
}; 
    
#elif   defined(USE_STM32F4)

static const uint8_t s_stm32i2c_maxports                    = 3;



static  I2C_TypeDef* const      s_stm32i2c_i2cx_port[3]               = {I2C1, I2C2, I2C3};
static const uint32_t           s_stm32i2c_i2cx_clock[3]              = {RCC_APB1Periph_I2C1, RCC_APB1Periph_I2C2, RCC_APB1Periph_I2C3};  
static const uint32_t           s_stm32i2c_i2cx_gpio_remap_clock[3]   = {RCC_APB2Periph_SYSCFG, 1, 1}; // 1 means that i dont know if it is correct     
static const uint32_t           s_stm32i2c_i2cx_gpio_remap[3]         = {GPIO_AF_I2C1, GPIO_AF_I2C2, GPIO_AF_I2C3};


static const uint32_t s_stm32i2c_i2c_gpio_thegpioclocks[]    = { RCC_AHB1Periph_GPIOA, RCC_AHB1Periph_GPIOB, RCC_AHB1Periph_GPIOC, 
                                                                RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOE, RCC_AHB1Periph_GPIOF, 
                                                                RCC_AHB1Periph_GPIOG, RCC_AHB1Periph_GPIOH, RCC_AHB1Periph_GPIOI };  
static GPIO_TypeDef* const s_stm32i2c_i2c_gpio_thegpioports[]= { GPIOA, GPIOB, GPIOC, 
                                                                GPIOD, GPIOE, GPIOF, 
                                                                GPIOG, GPIOH, GPIOI };  
static const uint8_t  s_stm32i2c_i2c_gpio_thepinnums[]       = { GPIO_PinSource0,  GPIO_PinSource1,  GPIO_PinSource2,  GPIO_PinSource3,
                                                                GPIO_PinSource4,  GPIO_PinSource5,  GPIO_PinSource6,  GPIO_PinSource7,
                                                                GPIO_PinSource8,  GPIO_PinSource9,  GPIO_PinSource10, GPIO_PinSource11,
                                                                GPIO_PinSource12, GPIO_PinSource13, GPIO_PinSource14, GPIO_PinSource15 }; 
static const uint16_t  s_stm32i2c_i2c_gpio_thepins[]         = { GPIO_Pin_0,  GPIO_Pin_1,  GPIO_Pin_2,  GPIO_Pin_3,
                                                                GPIO_Pin_4,  GPIO_Pin_5,  GPIO_Pin_6,  GPIO_Pin_7,
                                                                GPIO_Pin_8,  GPIO_Pin_9,  GPIO_Pin_10, GPIO_Pin_11,
                                                                GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15 }; 
                                                                
 
static const GPIO_InitTypeDef  s_stm32i2c_i2c_gpio_sxx_pin  =
{
    .GPIO_Pin       = GPIO_Pin_All,
    .GPIO_Mode      = GPIO_Mode_AF,
    .GPIO_Speed     = GPIO_Speed_50MHz,
    .GPIO_OType     = GPIO_OType_OD,
    .GPIO_PuPd      = GPIO_PuPd_NOPULL
}; 

static const GPIO_InitTypeDef  s_stm32i2c_i2c_gpio_sxx_floatpin  =
{
    .GPIO_Pin       = GPIO_Pin_All,
    .GPIO_Mode      = GPIO_Mode_IN,
    .GPIO_Speed     = GPIO_Speed_2MHz,
    .GPIO_OType     = GPIO_OType_PP,
    .GPIO_PuPd      = GPIO_PuPd_NOPULL
}; 

#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern stm32i2c_result_t stm32i2c_init(uint8_t port, const stm32i2c_cfg_t *cfg)
{
    stm32i2c_result_t res = stm32i2c_res_NOK;
    
    if((0 == port) || (port > s_stm32i2c_maxports) || (NULL == cfg))
    {
        return(stm32i2c_res_NOK);
    }
 
    
    if((1 != cfg->speed) && (4 != cfg->speed))
    {
        return(stm32i2c_res_NOK);
    }
    
    if((stm32i2c_gpio_portNONE == cfg->scl.port) || (stm32i2c_gpio_pinNONE == cfg->scl.pin) || (stm32i2c_gpio_portNONE == cfg->sda.port) || (stm32i2c_gpio_pinNONE == cfg->sda.pin))
    {
        return(stm32i2c_res_NOK);
    }
      
    
    // do other controls over cfg ....
          
    if(1 == s_stm32i2c_generics.initted[port-1])
    {
        return(stm32i2c_res_NOK);
    }
    
    
    // ok, cfg is good and can initialise... i copy it into ...
    
    memcpy(&s_stm32i2c_generics.cfg, cfg, sizeof(stm32i2c_cfg_t));
    
    
    // init some data
    s_stm32i2c_generics.i2cx                         = (I2C_TypeDef*)s_stm32i2c_i2cx_port[port-1];
    s_stm32i2c_generics.i2cx_cfg.clock               = s_stm32i2c_i2cx_clock[port-1];
    s_stm32i2c_generics.i2cx_cfg.gpio_remap          = s_stm32i2c_i2cx_gpio_remap[port-1];
    s_stm32i2c_generics.i2cx_cfg.gpio_remap_clock    = s_stm32i2c_i2cx_gpio_remap_clock[port-1];

    
    // init the generics according to the device
    
    res = s_stm32i2c_i2c_common_init(&s_stm32i2c_generics.cfg);
    
    if(stm32i2c_res_NOK == res)
    {
        s_stm32i2c_generics.initted[port]  = 0;
        return(stm32i2c_res_NOK);
    }
    else
    {
        s_stm32i2c_generics.initted[port]  = 1;   
    }        
        
    return(stm32i2c_res_OK);
}


extern stm32i2c_result_t stm32i2c_deinit(uint8_t port, const stm32i2c_cfg_t *cfg)
{
    if((0 == port) || (port > s_stm32i2c_maxports) || (NULL == cfg))
    {
        return(stm32i2c_res_NOK);
    }
 
    
    if((1 != cfg->speed) && (4 != cfg->speed))
    {
        return(stm32i2c_res_NOK);
    }
    
    if((stm32i2c_gpio_portNONE == cfg->scl.port) || (stm32i2c_gpio_pinNONE == cfg->scl.pin) || (stm32i2c_gpio_portNONE == cfg->sda.port) || (stm32i2c_gpio_pinNONE == cfg->sda.pin))
    {
        return(stm32i2c_res_NOK);
    }


    // init some data
    s_stm32i2c_generics.i2cx                         = (I2C_TypeDef*)s_stm32i2c_i2cx_port[port-1];
    s_stm32i2c_generics.i2cx_cfg.clock               = s_stm32i2c_i2cx_clock[port-1];
    s_stm32i2c_generics.i2cx_cfg.gpio_remap          = s_stm32i2c_i2cx_gpio_remap[port-1];
    s_stm32i2c_generics.i2cx_cfg.gpio_remap_clock    = s_stm32i2c_i2cx_gpio_remap_clock[port-1];
    
    s_stm32i2c_i2c_common_deinit(cfg);

    
    return(stm32i2c_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


//extern uint32_t stm32i2c_hid_myfunction(void)
//{
//}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

//static uint32_t s_stm32i2c_dosomething(void)
//{
//    return(0x12345678);
//}

static void hal_delay(uint32_t microsecs)
{
    volatile uint32_t i;
    uint32_t cnt = 72*microsecs;

    for(i = cnt; i; i--);
}
    
    

static void s_stm32i2c_i2c_software_reset(void)
{
    // implementare la sequenza di sw reset
    
    // forse puo' essere utile usare I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) per verificare qsa.
    // vedi: https://my.st.com/public/STe2ecommunities/mcu/Lists/cortex_mx_stm32/Flat.aspx?RootFolder=%2Fpublic%2FSTe2ecommunities%2Fmcu%2FLists%2Fcortex_mx_stm32%2FI2C%20Unable%20to%20receive%20start%20state%20%28EV5%29&FolderCTID=0x01200200770978C69A1141439FE559EB459D7580009C4E14902C3CDE46A77F0FFD06506F5B&currentviews=2461
    
    
}

static stm32i2c_result_t s_stm32i2c_i2c_prepare_data(const stm32i2c_cfg_t* i2ccfg, const uint8_t usefloatingpins)
{

    const GPIO_InitTypeDef* pin = NULL;
    
    s_stm32i2c_generics.i2c_gpio_scl.clock   = s_stm32i2c_i2c_gpio_thegpioclocks[s_stm32i2c_generics.cfg.scl.port];
    s_stm32i2c_generics.i2c_gpio_scl.port    = (GPIO_TypeDef*)s_stm32i2c_i2c_gpio_thegpioports[s_stm32i2c_generics.cfg.scl.port];
    s_stm32i2c_generics.i2c_gpio_scl.pinnum  = s_stm32i2c_i2c_gpio_thepinnums[s_stm32i2c_generics.cfg.scl.pin];
    pin = (0 == usefloatingpins) ? (&s_stm32i2c_i2c_gpio_sxx_pin) : (&s_stm32i2c_i2c_gpio_sxx_floatpin);
    memcpy(&s_stm32i2c_generics.i2c_gpio_scl.pin, pin, sizeof(GPIO_InitTypeDef));
    s_stm32i2c_generics.i2c_gpio_scl.pin.GPIO_Pin = s_stm32i2c_i2c_gpio_thepins[s_stm32i2c_generics.cfg.scl.pin];
        
    s_stm32i2c_generics.i2c_gpio_sda.clock   = s_stm32i2c_i2c_gpio_thegpioclocks[s_stm32i2c_generics.cfg.sda.port];
    s_stm32i2c_generics.i2c_gpio_sda.port    = (GPIO_TypeDef*)s_stm32i2c_i2c_gpio_thegpioports[s_stm32i2c_generics.cfg.sda.port];
    s_stm32i2c_generics.i2c_gpio_sda.pinnum  = s_stm32i2c_i2c_gpio_thepinnums[s_stm32i2c_generics.cfg.sda.pin];
    pin = (0 == usefloatingpins) ? (&s_stm32i2c_i2c_gpio_sxx_pin) : (&s_stm32i2c_i2c_gpio_sxx_floatpin);
    memcpy(&s_stm32i2c_generics.i2c_gpio_sda.pin, pin, sizeof(GPIO_InitTypeDef));
    s_stm32i2c_generics.i2c_gpio_sda.pin.GPIO_Pin = s_stm32i2c_i2c_gpio_thepins[s_stm32i2c_generics.cfg.sda.pin];

  
    return(stm32i2c_res_OK);
}

static stm32i2c_result_t s_stm32i2c_i2c_common_init(const stm32i2c_cfg_t* i2ccfg)
{
    const uint8_t usefloatingpins = 0;
    if(stm32i2c_res_NOK == s_stm32i2c_i2c_prepare_data(i2ccfg, usefloatingpins))
    {
        return(stm32i2c_res_NOK);
    }
    
    s_stm32i2c_i2c_gpio_init();
    s_stm32i2c_i2c_enable();
    
    return(stm32i2c_res_OK);
}


static stm32i2c_result_t s_stm32i2c_i2c_common_deinit(const stm32i2c_cfg_t* i2ccfg)
{

    const uint8_t usefloatingpins = 1;
    if(stm32i2c_res_NOK == s_stm32i2c_i2c_prepare_data(i2ccfg, usefloatingpins))
    {
        return(stm32i2c_res_NOK);
    }
    s_stm32i2c_i2c_disable();
    s_stm32i2c_i2c_gpio_deinit();
    
    return(stm32i2c_res_OK);
}



static void s_stm32i2c_i2c_gpio_init(void)
{   
#if     defined(USE_STM32F1)
    
    // i2c periph clock enable
    RCC_APB1PeriphClockCmd(s_stm32i2c_generics.i2cx_cfg.clock, ENABLE);
    
    // sda and scl periph clock enable
    RCC_APB2PeriphClockCmd(s_stm32i2c_generics.i2c_gpio_scl.clock | s_stm32i2c_generics.i2c_gpio_sda.clock, ENABLE);

    // afio periph clock enable
    RCC_APB2PeriphClockCmd(s_stm32i2c_generics.i2cx_cfg.gpio_remap_clock, ENABLE);
    
    // reset i2c input
    RCC_APB1PeriphResetCmd(s_stm32i2c_generics.i2cx_cfg.clock, ENABLE);
  
    // release reset signal for i2c input
    RCC_APB1PeriphResetCmd(s_stm32i2c_generics.i2cx_cfg.clock, DISABLE);
    
    // i2c remapping   
    GPIO_PinRemapConfig(s_stm32i2c_generics.i2cx_cfg.gpio_remap, ENABLE);    
        
    // configure gpio for scl
    GPIO_Init((GPIO_TypeDef*)s_stm32i2c_generics.i2c_gpio_scl.port, (GPIO_InitTypeDef*)&s_stm32i2c_generics.i2c_gpio_scl.pin);
    
    // configure gpio for sda
    GPIO_Init((GPIO_TypeDef*)s_stm32i2c_generics.i2c_gpio_sda.port, (GPIO_InitTypeDef*)&s_stm32i2c_generics.i2c_gpio_sda.pin);    
    
#elif   defined(USE_STM32F4)    

    // i2c periph clock enable
    RCC_APB1PeriphClockCmd(s_stm32i2c_generics.i2cx_cfg.clock, ENABLE);
    
    // sda and scl periph clock enable
    RCC_AHB1PeriphClockCmd(s_stm32i2c_generics.i2c_gpio_scl.clock | s_stm32i2c_generics.i2c_gpio_sda.clock, ENABLE);

    // afio periph clock enable
    RCC_APB2PeriphClockCmd(s_stm32i2c_generics.i2cx_cfg.gpio_remap_clock, ENABLE);
    
    // reset i2c input
    RCC_APB1PeriphResetCmd(s_stm32i2c_generics.i2cx_cfg.clock, ENABLE);
  
    // release reset signal for i2c input
    RCC_APB1PeriphResetCmd(s_stm32i2c_generics.i2cx_cfg.clock, DISABLE);
    
    // i2c remapping     
    GPIO_PinAFConfig((GPIO_TypeDef*)s_stm32i2c_generics.i2c_gpio_scl.port, s_stm32i2c_generics.i2c_gpio_scl.pinnum, s_stm32i2c_generics.i2cx_cfg.gpio_remap);
    GPIO_PinAFConfig((GPIO_TypeDef*)s_stm32i2c_generics.i2c_gpio_sda.port, s_stm32i2c_generics.i2c_gpio_sda.pinnum, s_stm32i2c_generics.i2cx_cfg.gpio_remap); 
        
    // configure gpio for scl
    GPIO_Init((GPIO_TypeDef*)s_stm32i2c_generics.i2c_gpio_scl.port, (GPIO_InitTypeDef*)&s_stm32i2c_generics.i2c_gpio_scl.pin);
    
    // configure gpio for sda
    GPIO_Init((GPIO_TypeDef*)s_stm32i2c_generics.i2c_gpio_sda.port, (GPIO_InitTypeDef*)&s_stm32i2c_generics.i2c_gpio_sda.pin);    
    
#endif

}

static void s_stm32i2c_i2c_enable(void)
{
#if     defined(USE_STM32F1) || defined(USE_STM32F4)  

    I2C_InitTypeDef i2c_cfg;
    memcpy(&i2c_cfg, &s_stm32i2c_i2c_cfg, sizeof(I2C_InitTypeDef));
    // apply the clockspeed 
    i2c_cfg.I2C_ClockSpeed = 100000 * s_stm32i2c_generics.cfg.speed;
    // i2c peripheral enable
    I2C_Cmd(s_stm32i2c_generics.i2cx, ENABLE);
    // apply configuration
    I2C_Init(s_stm32i2c_generics.i2cx, &i2c_cfg);
     
#endif
}



static void s_stm32i2c_i2c_disable(void)
{   
#if     defined(USE_STM32F1) || defined(USE_STM32F4)    

    // i2c disable
    I2C_Cmd(s_stm32i2c_generics.i2cx, DISABLE);
 
    // i2c deinit
    I2C_DeInit(s_stm32i2c_generics.i2cx);

    // i2c clock disable
    RCC_APB1PeriphClockCmd(s_stm32i2c_generics.i2cx_cfg.clock, DISABLE);

#endif    
}

static void s_stm32i2c_i2c_gpio_deinit(void)
{ 
#if     defined(USE_STM32F1) || defined(USE_STM32F4)
    // configure scl and sda as floating pin using values filled beforehands in i2c_gpio_scl and i2c_gpio_sda
    GPIO_Init((GPIO_TypeDef*)s_stm32i2c_generics.i2c_gpio_scl.port, (GPIO_InitTypeDef*)&s_stm32i2c_generics.i2c_gpio_scl.pin);
    GPIO_Init((GPIO_TypeDef*)s_stm32i2c_generics.i2c_gpio_sda.port, (GPIO_InitTypeDef*)&s_stm32i2c_generics.i2c_gpio_sda.pin); 
#endif    
}




//#endif//HAL_USE_I2C4HAL

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



