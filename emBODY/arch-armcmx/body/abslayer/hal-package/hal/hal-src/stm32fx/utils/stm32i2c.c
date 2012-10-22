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

#include "stm32gpio_hid.h"

//#include "hal_brdcfg.h"
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "stm32i2c.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 

#include "stm32i2c_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --- dma support: begin

#define STM32I2C_USE_DMA_ON_I2C1

#if     defined(USE_STM32F1)
#define          STM32I2C_dma_dir_MEMORY2PERIPHERAL      DMA_DIR_PeripheralDST
#define          STM32I2C_dma_dir_PERIPHERAL2MEMORY      DMA_DIR_PeripheralSRC
#elif   defined(USE_STM32F4)
#define          STM32I2C_dma_dir_MEMORY2PERIPHERAL      DMA_DIR_MemoryToPeripheral
#define          STM32I2C_dma_dir_PERIPHERAL2MEMORY      DMA_DIR_PeripheralToMemory
#endif

// --- dma support: end


#define stm32i2c_BASICTIMEOUT                ((uint32_t)0x00111000)
#define stm32i2c_MAXTRIALS                   (300)

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

// --- dma support: begin

typedef struct
{
    uint32_t* data;
    uint32_t* size;
    uint32_t* dir;
} stm32i2c_dma_cfg_mirror_t;

// --- dma support: end

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
    uint8_t                     use_dma[3];
    DMA_InitTypeDef             dma_cfg;
    stm32i2c_dma_cfg_mirror_t   dma_cfg_mirror;  
    volatile uint32_t           timeout;
    volatile uint16_t           bytes2read;
    volatile uint16_t           bytes2write;    
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

// --- dma support: begin
static void s_stm32i2c_dma_init(void);
static void s_stm32i2c_dma_i2c_enable(void);
static void s_stm32i2c_dma_deinit(void);
static void s_stm32i2c_dma_i2c_disable(void);
static void s_stm32i2c_dma_config_tx(uint32_t pBuffer, uint32_t BufferSize);
static void s_stm32i2c_dma_config_rx(uint32_t pBuffer, uint32_t BufferSize);
// --- dma support: end

static stm32i2c_result_t s_i2c4hal_timeoutexpired(void);

static void s_stm32i2c_read_bytes(I2C_TypeDef* i2cx, uint8_t* data, uint16_t size);
static void s_stm32i2c_read_withdma(I2C_TypeDef* i2cx, uint8_t* data, uint16_t size);

static void s_stm32i2c_write_bytes(I2C_TypeDef* i2cx, uint8_t* data, uint16_t size);
static void s_stm32i2c_write_withdma(I2C_TypeDef* i2cx, uint8_t* data, uint16_t size);

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
            .port                           = stm32gpio_portNONE,
            .pin                            = stm32gpio_pinNONE
        },
        .sda                            =
        {
            .port                           = stm32gpio_portNONE,
            .pin                            = stm32gpio_pinNONE        
        },
        .usedma             = 0,
        .ontimeout          = NULL
    },
    .i2cx                   = NULL,
    .i2cx_cfg               =
    {
        .clock                          = 0,
        .gpio_remap                     = 0,
        .gpio_remap_clock               = 0
    },
    .i2c_gpio_scl           = {0},
    .i2c_gpio_sda           = {0},
    .use_dma                = {0, 0, 0},
    .dma_cfg                = {0},
    .dma_cfg_mirror         =
    {
        .data                           = NULL,
        .size                           = NULL,
        .dir                            = NULL
    },
   .timeout                 = 0,
   .bytes2read              = 0,
   .bytes2write             = 0    
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

static const uint8_t s_stm32i2c_maxports                                = 2;

static I2C_TypeDef* const       s_stm32i2c_i2cx_port[3]                 = {I2C1, I2C2, NULL};
static const uint32_t           s_stm32i2c_i2cx_clock[3]                = {RCC_APB1Periph_I2C1, RCC_APB1Periph_I2C2, 0};  
static const uint32_t           s_stm32i2c_i2cx_gpio_remap_clock[3]     = {RCC_APB2Periph_AFIO, 1, 1}; // 1 means that i dont know if it is correct     
static const uint32_t           s_stm32i2c_i2cx_gpio_remap[3]           = {GPIO_Remap_I2C1, GPIO_Remap_I2C1, 0};
                                                                
 
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

static const uint8_t s_stm32i2c_maxports                                = 3;


static  I2C_TypeDef* const      s_stm32i2c_i2cx_port[3]                 = {I2C1, I2C2, I2C3};
static const uint32_t           s_stm32i2c_i2cx_clock[3]                = {RCC_APB1Periph_I2C1, RCC_APB1Periph_I2C2, RCC_APB1Periph_I2C3};  
static const uint32_t           s_stm32i2c_i2cx_gpio_remap_clock[3]     = {RCC_APB2Periph_SYSCFG, 1, 1}; // 1 means that i dont know if it is correct     
static const uint32_t           s_stm32i2c_i2cx_gpio_remap[3]           = {GPIO_AF_I2C1, GPIO_AF_I2C2, GPIO_AF_I2C3};
                                                               
 
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


// --- dma support: begin

#if     defined(USE_STM32F1)

const NVIC_InitTypeDef  stm32i2c_hid_dma_nvic_tx_enable  =
{
    .NVIC_IRQChannel                    = DMA1_Channel6_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = ENABLE
};

const NVIC_InitTypeDef  stm32i2c_hid_dma_nvic_rx_enable  =
{
    .NVIC_IRQChannel                    = DMA1_Channel7_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = ENABLE
};

const NVIC_InitTypeDef  stm32i2c_hid_dma_nvic_tx_disable =
{
    .NVIC_IRQChannel                    = DMA1_Channel6_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = DISABLE
};

const NVIC_InitTypeDef  stm32i2c_hid_dma_nvic_rx_disable =
{
    .NVIC_IRQChannel                    = DMA1_Channel7_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = DISABLE
};

const uint32_t          stm32i2c_hid_dma_clock       = RCC_AHBPeriph_DMA1; 

//DMA_TypeDef *           stm32i2c_hid_dma             = DMA1;
DMA_Channel_TypeDef * const  stm32i2c_hid_dma_stream_tx   = DMA1_Channel6;
DMA_Channel_TypeDef * const  stm32i2c_hid_dma_stream_rx   = DMA1_Channel7;

#define stm32i2c_hid_dma_I2C_DMA_TX_IRQHandler    DMA1_Channel6_IRQHandler
#define stm32i2c_hid_dma_I2C_DMA_RX_IRQHandler    DMA1_Channel7_IRQHandler   

const DMA_InitTypeDef         stm32i2c_hid_dma_cfg_init    =
{
    .DMA_PeripheralBaseAddr = ((uint32_t)0x40005410),
    .DMA_MemoryBaseAddr     = (uint32_t)0,              /* This parameter will be configured durig communication */
    .DMA_DIR                = DMA_DIR_PeripheralDST,    /* This parameter will be configured durig communication */
    .DMA_BufferSize         = 0xFFFF,                   /* This parameter will be configured durig communication */
    .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
    .DMA_MemoryInc          = DMA_MemoryInc_Enable,
    .DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte,
    .DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
    .DMA_Mode               = DMA_Mode_Normal,
    .DMA_Priority           = DMA_Priority_VeryHigh,
    .DMA_M2M                = DMA_M2M_Disable    
};

const uint32_t          stm32i2c_hid_dma_flags_tx                = 0;
const uint32_t          stm32i2c_hid_dma_flags_rx                = 0;

const uint32_t          stm32i2c_hid_dma_flags_tx_completed      = (DMA1_IT_TC6);
const uint32_t          stm32i2c_hid_dma_flags_rx_completed      = (DMA1_IT_TC7);

const uint32_t          stm32i2c_hid_dma_flags_tx_all            = (DMA1_IT_GL6);
const uint32_t          stm32i2c_hid_dma_flags_rx_all            = (DMA1_IT_GL7);


#elif   defined(USE_STM32F4)


const NVIC_InitTypeDef  stm32i2c_hid_dma_nvic_tx_enable  =
{
    .NVIC_IRQChannel                    = DMA1_Stream6_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = ENABLE
};

const NVIC_InitTypeDef  stm32i2c_hid_dma_nvic_rx_enable  =
{
    .NVIC_IRQChannel                    = DMA1_Stream0_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = ENABLE
};

const NVIC_InitTypeDef  stm32i2c_hid_dma_nvic_tx_disable =
{
    .NVIC_IRQChannel                    = DMA1_Stream6_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = DISABLE
};

const NVIC_InitTypeDef  stm32i2c_hid_dma_nvic_rx_disable =
{
    .NVIC_IRQChannel                    = DMA1_Stream0_IRQn,
    .NVIC_IRQChannelPreemptionPriority  = 0,
    .NVIC_IRQChannelSubPriority         = 0,
    .NVIC_IRQChannelCmd                 = DISABLE
};

const uint32_t          stm32i2c_hid_dma_clock       = RCC_AHB1Periph_DMA1; 

//DMA_TypeDef *           stm32i2c_hid_dma             = DMA1;
DMA_Stream_TypeDef * const   stm32i2c_hid_dma_stream_tx   = DMA1_Stream6;
DMA_Stream_TypeDef * const   stm32i2c_hid_dma_stream_rx   = DMA1_Stream0;

#define stm32i2c_hid_dma_I2C_DMA_TX_IRQHandler    DMA1_Stream6_IRQHandler
#define stm32i2c_hid_dma_I2C_DMA_RX_IRQHandler    DMA1_Stream0_IRQHandler   

const DMA_InitTypeDef         stm32i2c_hid_dma_cfg_init    =
{
    .DMA_Channel            = DMA_Channel_1,
    .DMA_PeripheralBaseAddr = ((uint32_t)0x40005410),
    .DMA_Memory0BaseAddr    = (uint32_t)0,                  // to be configured during communication 
    .DMA_DIR                = DMA_DIR_MemoryToPeripheral,   // to be configured during communication 
    .DMA_BufferSize         = 0xFFFF,                       // to be configured during communication 
    .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
    .DMA_MemoryInc          = DMA_MemoryInc_Enable,
    .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
    .DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
    .DMA_Mode               = DMA_Mode_Normal,
    .DMA_Priority           = DMA_Priority_VeryHigh,
    .DMA_FIFOMode           = DMA_FIFOMode_Enable,
    .DMA_FIFOThreshold      = DMA_FIFOThreshold_Full,
    .DMA_MemoryBurst        = DMA_MemoryBurst_Single,
    .DMA_PeripheralBurst    = DMA_PeripheralBurst_Single
};

const uint32_t          stm32i2c_hid_dma_flags_tx                = (DMA_FLAG_FEIF6 | DMA_FLAG_DMEIF6 | DMA_FLAG_TEIF6 | DMA_FLAG_HTIF6 | DMA_FLAG_TCIF6);
const uint32_t          stm32i2c_hid_dma_flags_rx                = (DMA_FLAG_FEIF0 | DMA_FLAG_DMEIF0 | DMA_FLAG_TEIF0 | DMA_FLAG_HTIF0 | DMA_FLAG_TCIF0);

const uint32_t          stm32i2c_hid_dma_flags_tx_completed      = (DMA_FLAG_TCIF6);
const uint32_t          stm32i2c_hid_dma_flags_rx_completed      = (DMA_FLAG_TCIF0);

const uint32_t          stm32i2c_hid_dma_flags_tx_all            = (0);
const uint32_t          stm32i2c_hid_dma_flags_rx_all            = (0);

#endif

// --- dma support: end

const uint32_t          stm32i2c_hid_timeout_flag            = stm32i2c_BASICTIMEOUT;
const uint32_t          stm32i2c_hid_timeout_long            = 10 * stm32i2c_BASICTIMEOUT;
const uint32_t          stm32i2c_hid_ackaddress_maxtrials    = stm32i2c_MAXTRIALS;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern stm32i2c_result_t stm32i2c_init(uint8_t port, const stm32i2c_cfg_t *cfg)
{
    stm32i2c_result_t res = stm32i2c_res_NOK;
    
    if((port >= s_stm32i2c_maxports) || (NULL == cfg))
    {
        return(stm32i2c_res_NOK);
    }
 
    
    if((100000 != cfg->speed) && (200000 != cfg->speed) && (400000 != cfg->speed))
    {
        return(stm32i2c_res_NOK);
    }
    
    if((stm32gpio_portNONE == cfg->scl.port) || (stm32gpio_pinNONE == cfg->scl.pin) || (stm32gpio_portNONE == cfg->sda.port) || (stm32gpio_pinNONE == cfg->sda.pin))
    {
        return(stm32i2c_res_NOK);
    }
      
    
    // do other controls over cfg ....
          
    if(1 == s_stm32i2c_generics.initted[port])
    {
        return(stm32i2c_res_NOK);
    }
    
    
    // ok, cfg is good and can initialise... i copy it into ...
    
    memcpy(&s_stm32i2c_generics.cfg, cfg, sizeof(stm32i2c_cfg_t));
    
    
    // init some data
    s_stm32i2c_generics.i2cx                         = (I2C_TypeDef*)s_stm32i2c_i2cx_port[port];
    s_stm32i2c_generics.i2cx_cfg.clock               = s_stm32i2c_i2cx_clock[port];
    s_stm32i2c_generics.i2cx_cfg.gpio_remap          = s_stm32i2c_i2cx_gpio_remap[port];
    s_stm32i2c_generics.i2cx_cfg.gpio_remap_clock    = s_stm32i2c_i2cx_gpio_remap_clock[port];
    
    

    
    // init the generics according to the device
    
    res = s_stm32i2c_i2c_common_init(&s_stm32i2c_generics.cfg);
    
#if defined(STM32I2C_USE_DMA_ON_I2C1)
    
    if(0 == port)
    {
        s_stm32i2c_generics.use_dma[port] = cfg->usedma;        
    }
    else
    {
        s_stm32i2c_generics.use_dma[port] = 0;
    }

#else
    s_stm32i2c_generics.use_dma[port] = 0;
#endif
    
    // init dma if needed
    if(1 == s_stm32i2c_generics.use_dma[port])
    {
        s_stm32i2c_dma_init();
        s_stm32i2c_dma_i2c_enable();
    }       
    
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
    if((port >= s_stm32i2c_maxports) || (NULL == cfg))
    {
        return(stm32i2c_res_NOK);
    }
 
    
    if((100000 != cfg->speed) && (200000 != cfg->speed) && (400000 != cfg->speed))
    {
        return(stm32i2c_res_NOK);
    }
    
    if((stm32gpio_portNONE == cfg->scl.port) || (stm32gpio_pinNONE == cfg->scl.pin) || (stm32gpio_portNONE == cfg->sda.port) || (stm32gpio_pinNONE == cfg->sda.pin))
    {
        return(stm32i2c_res_NOK);
    }


    // init some data
    s_stm32i2c_generics.i2cx                         = (I2C_TypeDef*)s_stm32i2c_i2cx_port[port];
    s_stm32i2c_generics.i2cx_cfg.clock               = s_stm32i2c_i2cx_clock[port];
    s_stm32i2c_generics.i2cx_cfg.gpio_remap          = s_stm32i2c_i2cx_gpio_remap[port];
    s_stm32i2c_generics.i2cx_cfg.gpio_remap_clock    = s_stm32i2c_i2cx_gpio_remap_clock[port];
    
    s_stm32i2c_i2c_common_deinit(cfg);

    if(1 == s_stm32i2c_generics.use_dma[port])
    {
        s_stm32i2c_dma_deinit();        
        s_stm32i2c_dma_i2c_disable();    
        s_stm32i2c_generics.use_dma[port] = 0;
    }
    
    s_stm32i2c_generics.initted[port]  = 0;
    
    return(stm32i2c_res_OK);
}



extern stm32i2c_result_t stm32i2c_read(uint8_t port, uint8_t devaddr, stm32i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
    stm32i2c_result_t res = stm32i2c_res_NOK;
    uint8_t reg1byteadr = 0;

    if((port >= s_stm32i2c_maxports))
    {
        return(stm32i2c_res_NOK);
    }

   
    if((0 == regaddr.numofbytes) || (regaddr.numofbytes > 3))
    {
        return(stm32i2c_res_NOK);
    } 

    
    I2C_TypeDef* i2cx = s_stm32i2c_i2cx_port[port];
    uint8_t usedma = s_stm32i2c_generics.use_dma[port];
    
    
    // wait until the bus is not busy anymore 
    s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_long;
    while(I2C_GetFlagStatus(i2cx, I2C_FLAG_BUSY))
    {
        if(0 == (s_stm32i2c_generics.timeout--)) return s_i2c4hal_timeoutexpired();
    }
    
    // send START condition
    I2C_GenerateSTART(i2cx, ENABLE);
 
    // test on ev5
    s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_flag;
    while(!I2C_CheckEvent(i2cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (s_stm32i2c_generics.timeout--)) return s_i2c4hal_timeoutexpired();
    }
    
    // send address of device
    I2C_Send7bitAddress(i2cx, devaddr, I2C_Direction_Transmitter);
    
    // test on ev6
    s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_flag;
    while(!I2C_CheckEvent(i2cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(0 == (s_stm32i2c_generics.timeout--)) return s_i2c4hal_timeoutexpired();
    }
    
    
    // send address of register to read inside the device
    if(1 == regaddr.numofbytes)
    {
        reg1byteadr = regaddr.bytes.one;
        I2C_SendData(i2cx, reg1byteadr);           
        // test on ev8 and clear it
        s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_flag;
        while(I2C_GetFlagStatus(i2cx, I2C_FLAG_BTF) == RESET)
        {
            if(0 == (s_stm32i2c_generics.timeout--)) return s_i2c4hal_timeoutexpired();
        }               
    }
    else if(2 == regaddr.numofbytes)
    {   
        reg1byteadr = regaddr.bytes.two & 0xFF00;               // msb first
        I2C_SendData(i2cx, reg1byteadr);           
        // test on ev8 and clear it
        s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_flag;
        while(!I2C_CheckEvent(i2cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        {
            if(0 == (s_stm32i2c_generics.timeout--)) return s_i2c4hal_timeoutexpired();
        }  
        
        reg1byteadr = regaddr.bytes.two & 0x00FF;               // then lsb
        I2C_SendData(i2cx, reg1byteadr);           
        // test on ev8 and clear it
        s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_flag;
        while(I2C_GetFlagStatus(i2cx, I2C_FLAG_BTF) == RESET)
        {
            if(0 == (s_stm32i2c_generics.timeout--)) return s_i2c4hal_timeoutexpired();
        }               
    }
    
    // send START condition a second time 
    I2C_GenerateSTART(i2cx, ENABLE);
 
    // test on ev5
    s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_flag;
    while(!I2C_CheckEvent(i2cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (s_stm32i2c_generics.timeout--)) return s_i2c4hal_timeoutexpired();
    }

   
    // send address for read
    I2C_Send7bitAddress(i2cx, devaddr, I2C_Direction_Receiver);
    
    //#warning --> w/ I2C_GetFlagStatus() the eeprom does not work. with I2C_CheckEvent() it works
    // wait on addr flag to be set
    //while(I2C_GetFlagStatus(i2cx, I2C_FLAG_ADDR) == RESET)
    //{
    //}  
    s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_flag;
    while(!I2C_CheckEvent(i2cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(0 == (s_stm32i2c_generics.timeout--)) return s_i2c4hal_timeoutexpired();
    }
  
    
    if(1 == size)
    {
        usedma = 0;
    }
    
    if(0 == usedma)
    {
        s_stm32i2c_read_bytes(i2cx, data, size);
    }
    else
    {
        s_stm32i2c_read_withdma(i2cx, data, size);
    }
    

    
    return(stm32i2c_res_OK);
}



extern stm32i2c_result_t stm32i2c_write(uint8_t port, uint8_t devaddr, stm32i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
    stm32i2c_result_t res = stm32i2c_res_NOK;
    uint8_t reg1byteadr = 0;

    if((port >= s_stm32i2c_maxports))
    {
        return(stm32i2c_res_NOK);
    }

   
    if((0 == regaddr.numofbytes) || (regaddr.numofbytes > 3))
    {
        return(stm32i2c_res_NOK);
    } 

    
    I2C_TypeDef* i2cx = s_stm32i2c_i2cx_port[port];
    uint8_t usedma = s_stm32i2c_generics.use_dma[port];

       
 ///////////////////////////////////////   
    
    
    
    // wait until the bus is not busy anymore 
    s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_long;
    while(I2C_GetFlagStatus(i2cx, I2C_FLAG_BUSY))
    {
        if(0 == (s_stm32i2c_generics.timeout--)) return s_i2c4hal_timeoutexpired();
    }

    // send START condition
    I2C_GenerateSTART(i2cx, ENABLE);
    
    // test on ev5
    s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_flag;
    while(!I2C_CheckEvent(i2cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (s_stm32i2c_generics.timeout--)) return s_i2c4hal_timeoutexpired();
    } 

    // send address of device
    I2C_Send7bitAddress(i2cx, devaddr, I2C_Direction_Transmitter);
    
    // test on ev6
    s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_flag;
    while(!I2C_CheckEvent(i2cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(0 == (s_stm32i2c_generics.timeout--)) return s_i2c4hal_timeoutexpired();
    }    

    // send address of register inside the device
    if(1 == regaddr.numofbytes)
    {
        reg1byteadr = regaddr.bytes.one;
        I2C_SendData(i2cx, reg1byteadr);    
     
        // test on ev8
        s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_flag;
        while(!I2C_CheckEvent(i2cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        {
            if(0 == (s_stm32i2c_generics.timeout--)) return s_i2c4hal_timeoutexpired();
        } 
    }
    else if(2 == regaddr.numofbytes)
    {   
        reg1byteadr = regaddr.bytes.two & 0xFF00;           // msb first
        I2C_SendData(i2cx, reg1byteadr);    
     
        // test on ev8
        s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_flag;
        while(!I2C_CheckEvent(i2cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        {
            if(0 == (s_stm32i2c_generics.timeout--)) return s_i2c4hal_timeoutexpired();
        }  

        reg1byteadr = regaddr.bytes.two & 0x00FF;           // then lsb
        I2C_SendData(i2cx, reg1byteadr);    
     
        // test on ev8
        s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_flag;
        while(!I2C_CheckEvent(i2cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        {
            if(0 == (s_stm32i2c_generics.timeout--)) return s_i2c4hal_timeoutexpired();
        }           
    }
    
    // now we decide if it is dma or direct send
    
    
    if(1 == size)
    {
        usedma = 0;
    }
    
    if(0 == usedma)
    {
        s_stm32i2c_write_bytes(i2cx, data, size);
    }
    else
    {
        s_stm32i2c_write_withdma(i2cx, data, size);
    }    
    
    
    
/////////////////////////////////////////////////////////////

    //s_stm32i2c_i2c_waitdevicestandbystate(port, devaddr);
    
    return(stm32i2c_res_OK);
}



extern stm32i2c_result_t stm32i2c_standby(uint8_t port, uint8_t devaddr)      
{
    volatile uint16_t tmpSR1 = 0;
    volatile uint32_t trials = 0;
    
    I2C_TypeDef* i2cx = s_stm32i2c_i2cx_port[port];
    
 
    // While the bus is busy 
    s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_long;
    while(I2C_GetFlagStatus(i2cx, I2C_FLAG_BUSY))
    {
        if(0 == (s_stm32i2c_generics.timeout--)) return s_i2c4hal_timeoutexpired();
    }

    /*  Keep looping till the slave acknowledge his address or maximum number 
        of trials is reached (this number is defined by stm32ee_hid_ackaddress_maxtrials define
        in stm324xg_eval_i2c_ee.h file) */
    while(1)
    {
        /*!< Send START condition */
        I2C_GenerateSTART(i2cx, ENABLE);

        /*!< Test on EV5 and clear it */
        s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_flag;
        while(!I2C_CheckEvent(i2cx, I2C_EVENT_MASTER_MODE_SELECT))
        {
            if(0 == (s_stm32i2c_generics.timeout--)) return s_i2c4hal_timeoutexpired();
        }    

        /*!< Send EEPROM address for write */
        I2C_Send7bitAddress(i2cx, devaddr, I2C_Direction_Transmitter);

        /* Wait for ADDR flag to be set (Slave acknowledged his address) */
        s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_long;
        do
        {     
            /* Get the current value of the SR1 register */
            tmpSR1 = (i2cx)->SR1;

            /* Update the timeout value and exit if it reach 0 */
            if(0 == (s_stm32i2c_generics.timeout--)) return s_i2c4hal_timeoutexpired();
        }
        /* Keep looping till the Address is acknowledged or the AF (ack failure) flag is 
        set (address not acknowledged at time) */
        while((tmpSR1 & (I2C_SR1_ADDR | I2C_SR1_AF)) == 0);

        /* Check if the ADDR flag has been set */
        if (tmpSR1 & I2C_SR1_ADDR)
        {
            /* Clear ADDR Flag by reading SR1 then SR2 registers (SR1 have already 
            been read) */
            (void)(i2cx)->SR2;

            /*!< STOP condition */    
            I2C_GenerateSTOP(i2cx, ENABLE);

            /* Exit the function */
            return stm32i2c_res_OK;
        }
        else
        {
            /*!< Clear AF flag */
            I2C_ClearFlag(i2cx, I2C_FLAG_AF);                  
        }

        /* Check if the maximum allowed number of trials has bee reached */
        if (stm32i2c_hid_ackaddress_maxtrials == trials++)
        {
            /* If the maximum number of trials has been reached, exit the function */
            return s_i2c4hal_timeoutexpired();
        }
    }
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----


// --- dma support: begin

#if     defined(USE_STM32F1)


#define stm32i2c_hid_dma_I2C_DMA_TX_IRQHandler    DMA1_Channel6_IRQHandler
#define stm32i2c_hid_dma_I2C_DMA_RX_IRQHandler    DMA1_Channel7_IRQHandler   

void stm32i2c_hid_dma_I2C_DMA_TX_IRQHandler(void)
{
    volatile uint32_t timeout = 0;
    
    /* Check if the DMA transfer is complete */ 
    if(DMA_GetFlagStatus(stm32i2c_hid_dma_flags_tx_completed) != RESET)
    {  
        /* Disable the DMA Tx Channel and Clear all its Flags */  
        DMA_Cmd(stm32i2c_hid_dma_stream_tx, DISABLE);
        DMA_ClearFlag(stm32i2c_hid_dma_flags_tx_all);

        /*!< Wait till all data have been physically transferred on the bus */
        timeout   =   stm32i2c_hid_timeout_long; // timeout must be local
        while(!I2C_GetFlagStatus(s_stm32i2c_generics.i2cx, I2C_FLAG_BTF))
        {
            if(0 == (timeout--)) s_i2c4hal_timeoutexpired();
        }
    
        /*!< Send STOP condition */
        I2C_GenerateSTOP(s_stm32i2c_generics.i2cx, ENABLE);
    
        /* Perform a read on SR1 and SR2 register to clear eventualaly pending flags */
        (void)(s_stm32i2c_generics.i2cx)->SR1;
        (void)(s_stm32i2c_generics.i2cx)->SR2;
    
        /* Reset the variable holding the number of data to be written */
        s_stm32i2c_generics.bytes2write = 0;  
    }
}

void stm32i2c_hid_dma_I2C_DMA_RX_IRQHandler(void)
{
    
    /* Check if the DMA transfer is complete */
    if(DMA_GetFlagStatus(stm32i2c_hid_dma_flags_rx_completed) != RESET)
    {      
        /*!< Send STOP Condition */
        I2C_GenerateSTOP(s_stm32i2c_generics.i2cx, ENABLE);    
    
        /* Disable the DMA Rx Channel and Clear all its Flags */  
        DMA_Cmd(stm32i2c_hid_dma_stream_rx, DISABLE);
        DMA_ClearFlag(stm32i2c_hid_dma_flags_rx_all);
    
        /* Reset the variable holding the number of data to be read */
        s_stm32i2c_generics.bytes2read = 0;
    }
}

#elif   defined(USE_STM32F4)


#define stm32i2c_hid_dma_I2C_DMA_TX_IRQHandler    DMA1_Stream6_IRQHandler
#define stm32i2c_hid_dma_I2C_DMA_RX_IRQHandler    DMA1_Stream0_IRQHandler   

void stm32i2c_hid_dma_I2C_DMA_TX_IRQHandler(void)
{
    volatile uint32_t timeout = 0;
    
    /* Check if the DMA transfer is complete */
    if(DMA_GetFlagStatus(stm32i2c_hid_dma_stream_tx, stm32i2c_hid_dma_flags_tx_completed) != RESET)
    {  
        /* Disable the DMA Tx Stream and Clear TC flag */  
        DMA_Cmd(stm32i2c_hid_dma_stream_tx, DISABLE);
        DMA_ClearFlag(stm32i2c_hid_dma_stream_tx, stm32i2c_hid_dma_flags_tx_completed);

        /*!< Wait till all data have been physically transferred on the bus */
        timeout   =   stm32i2c_hid_timeout_long; // timeout must be local
        while(!I2C_GetFlagStatus(s_stm32i2c_generics.i2cx, I2C_FLAG_BTF))
        {
            if(0 == (timeout--)) s_i2c4hal_timeoutexpired();
        }
    
        /*!< Send STOP condition */
        I2C_GenerateSTOP(s_stm32i2c_generics.i2cx, ENABLE);
        
        /* Perform a read on SR1 and SR2 register to clear eventually pending flags */
        (void)(s_stm32i2c_generics.i2cx)->SR1;
        (void)(s_stm32i2c_generics.i2cx)->SR2;
    
        /* Reset the variable holding the number of data to be written */
        s_stm32i2c_generics.bytes2write = 0;  
    }
}

void stm32i2c_hid_dma_I2C_DMA_RX_IRQHandler(void)
{
    /* Check if the DMA transfer is complete */
    if(DMA_GetFlagStatus(stm32i2c_hid_dma_stream_rx, stm32i2c_hid_dma_flags_rx_completed) != RESET)
    {      
        /*!< Send STOP Condition */
        I2C_GenerateSTOP(s_stm32i2c_generics.i2cx, ENABLE);    
    
        /* Disable the DMA Rx Stream and Clear TC Flag */  
        DMA_Cmd(stm32i2c_hid_dma_stream_rx, DISABLE);
        DMA_ClearFlag(stm32i2c_hid_dma_stream_rx, stm32i2c_hid_dma_flags_rx_completed);
    
        /* Reset the variable holding the number of data to be read */
        s_stm32i2c_generics.bytes2read = 0;
    }
}

#endif 

// --- dma support: end

// ---- isr of the module: end ------

//extern uint32_t stm32i2c_hid_myfunction(void)
//{
//}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


// static void s_stm32i2c_i2c_software_reset(void)
// {
//     // implementare la sequenza di sw reset. tuttavia serve una funzione che dia ritardo in microsec e la abbiamo nella hal
//     // non chiamarla mai....
//     for(;;);   
// }

static stm32i2c_result_t s_stm32i2c_i2c_prepare_data(const stm32i2c_cfg_t* i2ccfg, const uint8_t usefloatingpins)
{

    const GPIO_InitTypeDef* pin = NULL;
    
    s_stm32i2c_generics.i2c_gpio_scl.clock   = stm32gpio_hid_thegpioclocks[s_stm32i2c_generics.cfg.scl.port];
    s_stm32i2c_generics.i2c_gpio_scl.port    = (GPIO_TypeDef*)stm32gpio_hid_thegpioports[s_stm32i2c_generics.cfg.scl.port];
    s_stm32i2c_generics.i2c_gpio_scl.pinnum  = stm32gpio_hid_thepinnums[s_stm32i2c_generics.cfg.scl.pin];
    pin = (0 == usefloatingpins) ? (&s_stm32i2c_i2c_gpio_sxx_pin) : (&s_stm32i2c_i2c_gpio_sxx_floatpin);
    memcpy(&s_stm32i2c_generics.i2c_gpio_scl.pin, pin, sizeof(GPIO_InitTypeDef));
    s_stm32i2c_generics.i2c_gpio_scl.pin.GPIO_Pin = stm32gpio_hid_thepins[s_stm32i2c_generics.cfg.scl.pin];
        
    s_stm32i2c_generics.i2c_gpio_sda.clock   = stm32gpio_hid_thegpioclocks[s_stm32i2c_generics.cfg.sda.port];
    s_stm32i2c_generics.i2c_gpio_sda.port    = (GPIO_TypeDef*)stm32gpio_hid_thegpioports[s_stm32i2c_generics.cfg.sda.port];
    s_stm32i2c_generics.i2c_gpio_sda.pinnum  = stm32gpio_hid_thepinnums[s_stm32i2c_generics.cfg.sda.pin];
    pin = (0 == usefloatingpins) ? (&s_stm32i2c_i2c_gpio_sxx_pin) : (&s_stm32i2c_i2c_gpio_sxx_floatpin);
    memcpy(&s_stm32i2c_generics.i2c_gpio_sda.pin, pin, sizeof(GPIO_InitTypeDef));
    s_stm32i2c_generics.i2c_gpio_sda.pin.GPIO_Pin = stm32gpio_hid_thepins[s_stm32i2c_generics.cfg.sda.pin];

  
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
    i2c_cfg.I2C_ClockSpeed = s_stm32i2c_generics.cfg.speed;
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


// --- dma support: begin

static void s_stm32i2c_dma_init(void)
{
#if defined(USE_STM32F4)

    // init nvic for dma tx and rx
    NVIC_Init((NVIC_InitTypeDef*)&stm32i2c_hid_dma_nvic_tx_enable);
    NVIC_Init((NVIC_InitTypeDef*)&stm32i2c_hid_dma_nvic_rx_enable);
    
    // enable dma clock
    RCC_AHB1PeriphClockCmd(stm32i2c_hid_dma_clock, ENABLE);
    
    
    // clear pending flags on tx
    DMA_ClearFlag(stm32i2c_hid_dma_stream_tx, stm32i2c_hid_dma_flags_tx);
    // Disable the EE I2C Tx DMA stream 
    DMA_Cmd(stm32i2c_hid_dma_stream_tx, DISABLE);
    
    // Configure the DMA stream for the EE I2C peripheral TX direction 
    DMA_DeInit(stm32i2c_hid_dma_stream_tx);
    memcpy(&s_stm32i2c_generics.dma_cfg, &stm32i2c_hid_dma_cfg_init, sizeof(DMA_InitTypeDef));
    s_stm32i2c_generics.dma_cfg_mirror.data         = &s_stm32i2c_generics.dma_cfg.DMA_Memory0BaseAddr;
    s_stm32i2c_generics.dma_cfg_mirror.size         = &s_stm32i2c_generics.dma_cfg.DMA_BufferSize;
    s_stm32i2c_generics.dma_cfg_mirror.dir          = &s_stm32i2c_generics.dma_cfg.DMA_DIR;
    DMA_Init(stm32i2c_hid_dma_stream_tx, (DMA_InitTypeDef*)&stm32i2c_hid_dma_cfg_init);    
//     DMA_InitTypeDef xxx;
//     memcpy(&xxx, &stm32i2c_hid_dma_cfg_init, sizeof(DMA_InitTypeDef));
//     xxx.DMA_PeripheralBaseAddr = (uint32_t)(&(I2C1->DR));    
//     DMA_Init(stm32i2c_hid_dma_stream_tx, &xxx); 

    
    // clear pending flags on rx
    DMA_ClearFlag(stm32i2c_hid_dma_stream_rx, stm32i2c_hid_dma_flags_rx);
    /* Disable the EE I2C DMA Rx stream */
    DMA_Cmd(stm32i2c_hid_dma_stream_rx, DISABLE);
    
    /* Configure the DMA stream for the EE I2C peripheral RX direction */
    DMA_DeInit(stm32i2c_hid_dma_stream_rx);
    DMA_Init(stm32i2c_hid_dma_stream_rx, (DMA_InitTypeDef*)&stm32i2c_hid_dma_cfg_init);
    
    /* Enable the DMA Channels Interrupts */
    DMA_ITConfig(stm32i2c_hid_dma_stream_tx, DMA_IT_TC, ENABLE);
    DMA_ITConfig(stm32i2c_hid_dma_stream_rx, DMA_IT_TC, ENABLE);  
    

#elif   defined(USE_STM32F1)

    // init nvic for dma tx and rx
    NVIC_Init((NVIC_InitTypeDef*)&stm32i2c_hid_dma_nvic_tx_enable);
    NVIC_Init((NVIC_InitTypeDef*)&stm32i2c_hid_dma_nvic_rx_enable);
    
    // enable dma clock
    RCC_AHBPeriphClockCmd(stm32i2c_hid_dma_clock, ENABLE);
    

//    // clear pending flags on tx
//    DMA_ClearFlag(stm32i2c_hid_dma_stream_tx, stm32i2c_hid_dma_flags_tx);
//    // Disable the EE I2C Tx DMA stream 
//    DMA_Cmd(stm32i2c_hid_dma_stream_tx, DISABLE);
    
    // Configure the DMA stream for the EE I2C peripheral TX direction 
    DMA_DeInit(stm32i2c_hid_dma_stream_tx);
    memcpy(&s_stm32i2c_generics.dma_cfg, &stm32i2c_hid_dma_cfg_init, sizeof(DMA_InitTypeDef));
    s_stm32i2c_generics.dma_cfg_mirror.data         = &s_stm32i2c_generics.dma_cfg.DMA_MemoryBaseAddr;
    s_stm32i2c_generics.dma_cfg_mirror.size         = &s_stm32i2c_generics.dma_cfg.DMA_BufferSize;
    s_stm32i2c_generics.dma_cfg_mirror.dir          = &s_stm32i2c_generics.dma_cfg.DMA_DIR;
    DMA_Init(stm32i2c_hid_dma_stream_tx, (DMA_InitTypeDef*)&stm32i2c_hid_dma_cfg_init);    

    
//    // clear pending flags on rx
//    DMA_ClearFlag(stm32i2c_hid_dma_stream_rx, stm32i2c_hid_dma_flags_rx);
//    /* Disable the EE I2C DMA Rx stream */
//    DMA_Cmd(stm32i2c_hid_dma_stream_rx, DISABLE);
    
    /* Configure the DMA stream for the EE I2C peripheral RX direction */
    DMA_DeInit(stm32i2c_hid_dma_stream_rx);
    DMA_Init(stm32i2c_hid_dma_stream_rx, (DMA_InitTypeDef*)&stm32i2c_hid_dma_cfg_init);
    
    /* Enable the DMA Channels Interrupts */
    DMA_ITConfig(stm32i2c_hid_dma_stream_tx, DMA_IT_TC, ENABLE);
    DMA_ITConfig(stm32i2c_hid_dma_stream_rx, DMA_IT_TC, ENABLE);  
        
#endif
}


static void s_stm32i2c_dma_deinit(void)
{   
#if     defined(USE_STM32F1) || defined(USE_STM32F4)  

    // disable tx and rx dma interrupts
    NVIC_Init((NVIC_InitTypeDef*)&stm32i2c_hid_dma_nvic_tx_disable);
    NVIC_Init((NVIC_InitTypeDef*)&stm32i2c_hid_dma_nvic_rx_disable);

    // disable and deinitialise the dma streams
    DMA_Cmd(stm32i2c_hid_dma_stream_tx, DISABLE);
    DMA_Cmd(stm32i2c_hid_dma_stream_rx, DISABLE);
    DMA_DeInit(stm32i2c_hid_dma_stream_tx);
    DMA_DeInit(stm32i2c_hid_dma_stream_rx);

#endif
}


static void s_stm32i2c_dma_i2c_enable(void)
{
    I2C_DMACmd(s_stm32i2c_generics.i2cx, ENABLE);  
}


static void s_stm32i2c_dma_i2c_disable(void)
{
    I2C_DMACmd(s_stm32i2c_generics.i2cx, DISABLE);  
}


static void s_stm32i2c_dma_config_tx(uint32_t pBuffer, uint32_t BufferSize)
{ 
    *s_stm32i2c_generics.dma_cfg_mirror.data    = (uint32_t)pBuffer;
    *s_stm32i2c_generics.dma_cfg_mirror.size    = BufferSize;
    *s_stm32i2c_generics.dma_cfg_mirror.dir     = STM32I2C_dma_dir_MEMORY2PERIPHERAL;
    
    DMA_Init(stm32i2c_hid_dma_stream_tx, &s_stm32i2c_generics.dma_cfg); 
}

static void s_stm32i2c_dma_config_rx(uint32_t pBuffer, uint32_t BufferSize)
{ 
    *s_stm32i2c_generics.dma_cfg_mirror.data    = (uint32_t)pBuffer;
    *s_stm32i2c_generics.dma_cfg_mirror.size    = BufferSize;
    *s_stm32i2c_generics.dma_cfg_mirror.dir     = STM32I2C_dma_dir_PERIPHERAL2MEMORY;
    
    DMA_Init(stm32i2c_hid_dma_stream_rx, &s_stm32i2c_generics.dma_cfg); 
}

// --- dma support: end

static stm32i2c_result_t s_i2c4hal_timeoutexpired(void)
{
     if(NULL != s_stm32i2c_generics.cfg.ontimeout)
     {
         s_stm32i2c_generics.cfg.ontimeout();
     }
     else
     {
        for(;;);
     }
    
    return(stm32i2c_res_NOK);
}



static void s_stm32i2c_read_bytes(I2C_TypeDef* i2cx, uint8_t* data, uint16_t size)
{
    
    while(size)
    {
        if(1 == size)
        {
            // disable acknowledgement
            I2C_AcknowledgeConfig(i2cx, DISABLE);   

            // clear addr by reading sr1 and then sr2
            (void)(i2cx)->SR1;
            (void)(i2cx)->SR2;
            
            // send stop condition
            I2C_GenerateSTOP(i2cx, ENABLE);
        }
        
        // wait for the byte to be received
        s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_flag;
        while(I2C_GetFlagStatus(i2cx, I2C_FLAG_RXNE) == RESET)
        {                    
            if(0 == (s_stm32i2c_generics.timeout--)) {s_i2c4hal_timeoutexpired(); return;}
        }
        
        // read received byte
        *data = I2C_ReceiveData(i2cx);
        
        // increment received data and decrement the size (bytes to be received yet)
        data++;
        size--;          
    }
    
    // wait to make sure that the stop bit has been cleared
    s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_flag;
    while((i2cx)->CR1 & I2C_CR1_STOP)
    {
        if(0 == (s_stm32i2c_generics.timeout--)) {s_i2c4hal_timeoutexpired(); return;}
    }  

    // re-enable acknowledgement to be ready for another reception 
    I2C_AcknowledgeConfig(i2cx, ENABLE);    
}


static void s_stm32i2c_read_withdma(I2C_TypeDef* i2cx, uint8_t* data, uint16_t size)
{

    s_stm32i2c_generics.bytes2read = size;

    s_stm32i2c_dma_config_rx((uint32_t)data, size);

    // Inform the DMA that the next End Of Transfer Signal will be the last one
    I2C_DMALastTransferCmd(i2cx, ENABLE); 

    /* Enable the DMA Rx Stream */
    DMA_Cmd(stm32i2c_hid_dma_stream_rx, ENABLE);  

    // and now ... waits for the dma reception
    s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_long;
    while ((s_stm32i2c_generics.bytes2read) > 0)
    {
        if(0 == (s_stm32i2c_generics.timeout--)) {s_i2c4hal_timeoutexpired(); return;}
    }
    
}

static void s_stm32i2c_write_bytes(I2C_TypeDef* i2cx, uint8_t* data, uint16_t size)
{
    while(size) 
    {
  
        // send the byte to be written 
        I2C_SendData(i2cx, *data); 
        
        data++;
        size--;
        
        // test on ev8 (or ev8_2 = I2C_EVENT_MASTER_BYTE_TRANSMITTED which is slower but more reliable)    
        s_stm32i2c_generics.timeout = stm32i2c_hid_timeout_flag;
        while(!I2C_CheckEvent(i2cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        {
            if(0 == (s_stm32i2c_generics.timeout--)) {s_i2c4hal_timeoutexpired(); return;}
        }
    
    }
    
    // send stop condition
    I2C_GenerateSTOP(i2cx, ENABLE);
    
     // perform a read on SR1 and SR2 register to clear eventually pending flags 
    (void)(i2cx)->SR1;
    (void)(i2cx)->SR2; 
}

static void s_stm32i2c_write_withdma(I2C_TypeDef* i2cx, uint8_t* data, uint16_t size)
{
    s_stm32i2c_generics.bytes2write = size;
    
    /* Configure the DMA Tx Channel with the buffer address and the buffer size */
    //s_stm32ee_dma_config((uint32_t)pBuffer, (uint32_t)(*NumByteToWrite), sEE_DIRECTION_TX);
    s_stm32i2c_dma_config_tx((uint32_t)data, (uint32_t)size);

    /* Enable the DMA Tx Stream */
    DMA_Cmd(stm32i2c_hid_dma_stream_tx, ENABLE);
}



//#endif//HAL_USE_I2C4HAL

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



