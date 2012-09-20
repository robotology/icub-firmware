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


/* @file       stm32ee.c
	@brief      This file implements management of some i2c EEPROMs using stm32 apis.
	@author     marco.accame@iit.it
    @date       09/14/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
//#include "hal_brdcfg_modules.h"

//#ifdef HAL_USE_EEPROM

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

#include "stm32ee.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 

#include "stm32ee_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// -- here are those used as link between this code and stm's example

#define sEE_PAGESIZE            (s_stm32ee_generics.pagesize)
#define sEEAddress              (s_stm32ee_generics.hwaddress)
#define sEEDataReadPointer      (s_stm32ee_generics.prt2bytes2read)
#define sEEDataWritePointer     (s_stm32ee_generics.prt2bytes2write)
#define sEEDataNum              (s_stm32ee_generics.numberofbyte2writeinpage)
#define sEETimeout              (s_stm32ee_generics.timeout)



#define sEE_DIRECTION_TX 1
#define sEE_DIRECTION_RX 0



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const stm32ee_cfg_t stm32_cfg_default = 
{ 
    .device             = stm32ee_device_st_m24lr64, 
    .i2cspeed           = 400000,
    .usedmatransfer     = 0,
    .functioni2cinit    = NULL, 
    .parameteri2cinit   = NULL, 
    .functioni2cdeinit  = NULL 
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    uint8_t                 hwaddress;                 // EEPROM HW address (typically 0xA0 ... but some bits may change .....
    uint16_t                pagesize;
    uint32_t                totalsize;
    stm32ee_cfg_t           cfg;
    volatile uint16_t*      prt2bytes2read;
    volatile uint16_t*      prt2bytes2write;
    volatile uint16_t       numberofbyte2writeinpage;
    volatile uint32_t       timeout;
} stm32ee_generic_container_t;


typedef struct
{
    uint32_t* data;
    uint32_t* size;
    uint32_t* dir;
} stm32ee_dma_extracfg_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_stm32ee_i2c_gpio_init(void);
static void s_stm32ee_dma_init(void);
static void s_stm32ee_i2c_enable(void);
static void s_stm32ee_dma_i2c_enable(void);

static void s_stm32ee_i2c_disable(void);
static void s_stm32ee_i2c_gpio_deinit(void);
static void s_stm32ee_dma_deinit(void);


static stm32ee_result_t s_stm32ee_verify_rw_bounds(uint32_t address, uint32_t *size, uint8_t* buffer);

static stm32ee_result_t s_stm32_readbuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t* NumByteToRead);
static stm32ee_result_t s_stm32ee_writepage(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t* NumByteToWrite);

static stm32ee_result_t s_stm32ee_writebuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
static stm32ee_result_t s_stm32ee_timeoutexpired(void);
static stm32ee_result_t s_stm32ee_waiteepromstandbystate(void);

static void s_stm32ee_dma_config_tx(uint32_t pBuffer, uint32_t BufferSize);

static void s_stm32ee_dma_config_rx(uint32_t pBuffer, uint32_t BufferSize);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static stm32ee_generic_container_t s_stm32ee_generics = 
{
    .hwaddress                  = 0xA0,
    .pagesize                   = 4,
    .totalsize                  = 0,
    .cfg                        =
    {
        .device                     = stm32ee_device_none, 
        .i2cspeed                   = 400000,
        .usedmatransfer             = 0,
        .functioni2cinit            = NULL, 
        .parameteri2cinit           = NULL, 
        .functioni2cdeinit          = NULL,
        .functionontimeout          = NULL
    },
    .prt2bytes2read             = NULL,
    .prt2bytes2write            = NULL,
    .numberofbyte2writeinpage   = 0,
    .timeout                    = 0    
};


static DMA_InitTypeDef          sEEDMA_InitStructure;

static stm32ee_dma_extracfg_t   s_stm32ee_dma_extracfg = {NULL, NULL, NULL};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern stm32ee_result_t stm32ee_init(const stm32ee_cfg_t *cfg)
{
//    stm32ee_result_t res = stm32ee_res_NOK;
    
    if(NULL == cfg)
    {
        cfg = &stm32_cfg_default;
    }
    
    if((stm32ee_device_none == cfg->device) || (stm32ee_device_none != s_stm32ee_generics.cfg.device))
    {
        return(stm32ee_res_NOK);
    }
    
    
    // do other controls over cfg ....
    
    
    // ok, cfg is good. i copy it into ...
    
    memcpy(&s_stm32ee_generics.cfg, cfg, sizeof(stm32ee_cfg_t));
    
    // per ora uso le funzioni interne. poi penseremo a passarle dall'esterno (se e' il caso).
    s_stm32ee_i2c_gpio_init();
    
    if(1 == s_stm32ee_generics.cfg.usedmatransfer)
    {
        s_stm32ee_dma_init();
    }
    
    s_stm32ee_i2c_enable();
    
    if(1 == s_stm32ee_generics.cfg.usedmatransfer)
    {
        s_stm32ee_dma_i2c_enable();
    }
    
    // init the generics according to the device
    
    switch(s_stm32ee_generics.cfg.device)
    {
        case stm32ee_device_st_m24lr64:
        {
            s_stm32ee_generics.hwaddress     = 0xA0;    // poi si devono mettere a posto i bit meno significativi a seconda dei casi
            s_stm32ee_generics.pagesize      = 4;   
            s_stm32ee_generics.totalsize     = 8*1024;
        } break;
        
        case stm32ee_device_st_m24c64:
        {
            s_stm32ee_generics.hwaddress     = 0xA0;    // poi si devono mettere a posto i bit meno significativi a seconda dei casi
            s_stm32ee_generics.pagesize      = 32; 
            s_stm32ee_generics.totalsize     = 8*1024;
        } break;
        
        case stm32ee_device_st_m24512:
        {
            s_stm32ee_generics.hwaddress     = 0xA0;    // poi si devono mettere a posto i bit meno significativi a seconda dei casi
            s_stm32ee_generics.pagesize      = 128;
            s_stm32ee_generics.totalsize     = 64*1024;            
        } break;
        
        case stm32ee_device_atmel_at24c512b:
        {
            s_stm32ee_generics.hwaddress     = 0xA0;    // poi si devono mettere a posto i bit meno significativi a seconda dei casi
            s_stm32ee_generics.pagesize      = 128;  
            s_stm32ee_generics.totalsize     = 64*1024;
        } break;
        
        case stm32ee_device_atmel_at24c1024b:
        {
            s_stm32ee_generics.hwaddress     = 0xA0;    // poi si devono mettere a posto i bit meno significativi a seconda dei casi
            s_stm32ee_generics.pagesize      = 256;
            s_stm32ee_generics.totalsize     = 128*1024;            
        } break;
        
        default:
        {
            s_stm32ee_generics.hwaddress     = 0xA0;    // poi si devono mettere a posto i bit meno significativi a seconda dei casi
            s_stm32ee_generics.pagesize      = 4;  
            s_stm32ee_generics.totalsize     = 8*1024;            
        } break;
    }
    
    // apply extra bits
    s_stm32ee_generics.hwaddress |= stm32ee_hid_hwaddressbits1and2;
    
    
    
    return(stm32ee_res_OK);
}


extern stm32ee_result_t stm32ee_deinit(void)
{
//    stm32ee_result_t res = stm32ee_res_NOK;
    
    s_stm32ee_i2c_disable();
    s_stm32ee_i2c_gpio_deinit();
    if(1 == s_stm32ee_generics.cfg.usedmatransfer)
    {
        s_stm32ee_dma_deinit();
    }

    return(stm32ee_res_OK);
}


extern stm32ee_result_t stm32ee_read(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* readbytes)
{   // read and block until done
    stm32ee_result_t res = stm32ee_res_NOK;
    
    // verify bounds and ... clip size to maximum possible if address+size is out of bound
    if(stm32ee_res_NOK == s_stm32ee_verify_rw_bounds(address, &size, buffer))
    {
        return(stm32ee_res_NOK);
    }
    
    
    uint16_t ReadAddr = (uint16_t) address;
    uint16_t NumByteToRead = (uint16_t) size;
    res = s_stm32_readbuffer(buffer, ReadAddr, &NumByteToRead);
    
    if(NULL != readbytes)
    {
        *readbytes = (stm32ee_res_OK == res) ? (size) : (0);
    }
    
    return(res);
}


extern stm32ee_result_t stm32ee_write(uint32_t address, uint32_t size, uint8_t* buffer, uint32_t* writtenbytes)
{   // read and block until done
    stm32ee_result_t res = stm32ee_res_NOK;
    
    // verify bounds and ... clip size to maximum possible if address+size is out of bound
    if(stm32ee_res_NOK == s_stm32ee_verify_rw_bounds(address, &size, buffer))
    {
        return(stm32ee_res_NOK);
    } 

    uint16_t WriteAddr = (uint16_t) address;
    uint16_t NumByteToWrite = (uint16_t) size;
    res = s_stm32ee_writebuffer(buffer, WriteAddr, NumByteToWrite);

    if(NULL != writtenbytes)
    {
        *writtenbytes = (stm32ee_res_OK == res) ? (size) : (0);
    }
    
    return(res);   
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----

#if     defined(USE_STM32F4)
void stm32ee_hid_dma_I2C_DMA_TX_IRQHandler(void)
{
    /* Check if the DMA transfer is complete */
    if(DMA_GetFlagStatus(stm32ee_hid_dma_stream_tx, stm32ee_hid_dma_flags_tx_completed) != RESET)
    {  
        /* Disable the DMA Tx Stream and Clear TC flag */  
        DMA_Cmd(stm32ee_hid_dma_stream_tx, DISABLE);
        DMA_ClearFlag(stm32ee_hid_dma_stream_tx, stm32ee_hid_dma_flags_tx_completed);

        /*!< Wait till all data have been physically transferred on the bus */
        sEETimeout = stm32ee_hid_timeout_long;
        while(!I2C_GetFlagStatus(stm32ee_hid_i2c_port, I2C_FLAG_BTF))
        {
        if((sEETimeout--) == 0) s_stm32ee_timeoutexpired();
        }
    
        /*!< Send STOP condition */
        I2C_GenerateSTOP(stm32ee_hid_i2c_port, ENABLE);
        
        /* Perform a read on SR1 and SR2 register to clear eventually pending flags */
        (void)stm32ee_hid_i2c_port->SR1;
        (void)stm32ee_hid_i2c_port->SR2;
    
        /* Reset the variable holding the number of data to be written */
        *sEEDataWritePointer = 0;  
    }
}

void stm32ee_hid_dma_I2C_DMA_RX_IRQHandler(void)
{
    /* Check if the DMA transfer is complete */
    if(DMA_GetFlagStatus(stm32ee_hid_dma_stream_rx, stm32ee_hid_dma_flags_rx_completed) != RESET)
    {      
        /*!< Send STOP Condition */
        I2C_GenerateSTOP(stm32ee_hid_i2c_port, ENABLE);    
    
        /* Disable the DMA Rx Stream and Clear TC Flag */  
        DMA_Cmd(stm32ee_hid_dma_stream_rx, DISABLE);
        DMA_ClearFlag(stm32ee_hid_dma_stream_rx, stm32ee_hid_dma_flags_rx_completed);
    
        /* Reset the variable holding the number of data to be read */
        *sEEDataReadPointer = 0;
    }
}
#elif   defined(USE_STM32F1)
void stm32ee_hid_dma_I2C_DMA_TX_IRQHandler(void)
{
  /* Check if the DMA transfer is complete */ 
  if(DMA_GetFlagStatus(stm32ee_hid_dma_flags_tx_completed) != RESET)
  {  
    /* Disable the DMA Tx Channel and Clear all its Flags */  
    DMA_Cmd(stm32ee_hid_dma_stream_tx, DISABLE);
    DMA_ClearFlag(stm32ee_hid_dma_flags_tx_all);

    /*!< Wait till all data have been physically transferred on the bus */
    sEETimeout = stm32ee_hid_timeout_long;
    while(!I2C_GetFlagStatus(stm32ee_hid_i2c_port, I2C_FLAG_BTF))
    {
      if((sEETimeout--) == 0) s_stm32ee_timeoutexpired();
    }
    
    /*!< Send STOP condition */
    I2C_GenerateSTOP(stm32ee_hid_i2c_port, ENABLE);
    
    /* Perform a read on SR1 and SR2 register to clear eventualaly pending flags */
    (void)stm32ee_hid_i2c_port->SR1;
    (void)stm32ee_hid_i2c_port->SR2;
    
    /* Reset the variable holding the number of data to be written */
    *sEEDataWritePointer = 0;  
  }
}

void stm32ee_hid_dma_I2C_DMA_RX_IRQHandler(void)
{
  /* Check if the DMA transfer is complete */
  if(DMA_GetFlagStatus(stm32ee_hid_dma_flags_rx_completed) != RESET)
  {      
    /*!< Send STOP Condition */
    I2C_GenerateSTOP(stm32ee_hid_i2c_port, ENABLE);    
    
    /* Disable the DMA Rx Channel and Clear all its Flags */  
    DMA_Cmd(stm32ee_hid_dma_stream_rx, DISABLE);
    DMA_ClearFlag(stm32ee_hid_dma_flags_rx_all);
    
    /* Reset the variable holding the number of data to be read */
    *sEEDataReadPointer = 0;
  }
}
#endif 

// ---- isr of the module: end ------


//extern uint32_t stm32ee_hid_myfunction(void)
//{
//}





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

//static uint32_t s_stm32ee_dosomething(void)
//{
//    return(0x12345678);
//}

static void hal_delay(uint32_t microsecs)
{
    volatile uint32_t i;
    uint32_t cnt = 72*microsecs;

    for(i = cnt; i; i--);
}
    
    

static void s_stm32ee_i2c_software_reset(void)
{
    // implementare la sequenza di sw reset.
    
    // forse puo' essere utile usare I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) per verificare qsa.
    // vedi: https://my.st.com/public/STe2ecommunities/mcu/Lists/cortex_mx_stm32/Flat.aspx?RootFolder=%2Fpublic%2FSTe2ecommunities%2Fmcu%2FLists%2Fcortex_mx_stm32%2FI2C%20Unable%20to%20receive%20start%20state%20%28EV5%29&FolderCTID=0x01200200770978C69A1141439FE559EB459D7580009C4E14902C3CDE46A77F0FFD06506F5B&currentviews=2461
    
    
}


static void s_stm32ee_i2c_gpio_init(void)
{   
#if defined(USE_STM32F4)    
#if 1
   // i2c periph clock enable
    RCC_APB1PeriphClockCmd(stm32ee_hid_i2c_clock, ENABLE);
    
    // sda and scl periph clock enable
    RCC_AHB1PeriphClockCmd(stm32ee_hid_i2c_gpio_scl_clock | stm32ee_hid_i2c_gpio_sda_clock, ENABLE);

    // afio periph clock enable
    RCC_APB2PeriphClockCmd(stm32ee_hid_i2c_gpio_remap_clock, ENABLE);
    
    // reset i2c input
    RCC_APB1PeriphResetCmd(stm32ee_hid_i2c_clock, ENABLE);
  
    // release reset signal for i2c input
    RCC_APB1PeriphResetCmd(stm32ee_hid_i2c_clock, DISABLE);
    
    // i2c remapping     
    GPIO_PinAFConfig(stm32ee_hid_i2c_gpio_scl_port, stm32ee_hid_i2c_gpio_scl_pinnum, stm32ee_hid_i2c_gpio_remap);
    GPIO_PinAFConfig(stm32ee_hid_i2c_gpio_sda_port, stm32ee_hid_i2c_gpio_sda_pinnum, stm32ee_hid_i2c_gpio_remap); 
        
    // configure gpio for scl
    GPIO_Init(stm32ee_hid_i2c_gpio_scl_port, (GPIO_InitTypeDef*)&stm32ee_hid_i2c_gpio_scl_pin);
    
    // configure gpio for sda
    GPIO_Init(stm32ee_hid_i2c_gpio_sda_port, (GPIO_InitTypeDef*)&stm32ee_hid_i2c_gpio_sda_pin);    

#else
    
    GPIO_InitTypeDef  GPIO_InitStructure; 
       
    /*!< sEE_I2C Periph clock enable */
    RCC_APB1PeriphClockCmd(sEE_I2C_CLK, ENABLE);
  
    /*!< sEE_I2C_SCL_GPIO_CLK and sEE_I2C_SDA_GPIO_CLK Periph clock enable */
    RCC_AHB1PeriphClockCmd(sEE_I2C_SCL_GPIO_CLK | sEE_I2C_SDA_GPIO_CLK, ENABLE);        // different call and one more flag in stm32f1

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
    /* Reset sEE_I2C IP */
    RCC_APB1PeriphResetCmd(sEE_I2C_CLK, ENABLE);
  
    /* Release reset signal of sEE_I2C IP */
    RCC_APB1PeriphResetCmd(sEE_I2C_CLK, DISABLE);
    
    /*!< GPIO configuration */
    /* Connect PXx to I2C_SCL*/
    GPIO_PinAFConfig(sEE_I2C_SCL_GPIO_PORT, sEE_I2C_SCL_SOURCE, sEE_I2C_SCL_AF);
    /* Connect PXx to I2C_SDA*/
    GPIO_PinAFConfig(sEE_I2C_SDA_GPIO_PORT, sEE_I2C_SDA_SOURCE, sEE_I2C_SDA_AF);  
  
    /*!< Configure sEE_I2C pins: SCL */   
    GPIO_InitStructure.GPIO_Pin = sEE_I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            // as in stm32f1  but diff value
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;          // not in stm32f1
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;       // not in stm32f1
    GPIO_Init(sEE_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure sEE_I2C pins: SDA */
    GPIO_InitStructure.GPIO_Pin = sEE_I2C_SDA_PIN;
    GPIO_Init(sEE_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
#endif
    
#elif   defined(USE_STM32F1)
#if 1
    
    // i2c periph clock enable
    RCC_APB1PeriphClockCmd(stm32ee_hid_i2c_clock, ENABLE);
    
    // sda and scl periph clock enable
    RCC_APB2PeriphClockCmd(stm32ee_hid_i2c_gpio_scl_clock | stm32ee_hid_i2c_gpio_sda_clock, ENABLE);

    // afio periph clock enable
    RCC_APB2PeriphClockCmd(stm32ee_hid_i2c_gpio_remap_clock, ENABLE);
    
    // reset i2c input
    RCC_APB1PeriphResetCmd(stm32ee_hid_i2c_clock, ENABLE);
  
    // release reset signal for i2c input
    RCC_APB1PeriphResetCmd(stm32ee_hid_i2c_clock, DISABLE);
    
    // i2c remapping   
    GPIO_PinRemapConfig(stm32ee_hid_i2c_gpio_remap, ENABLE);    
        
    // configure gpio for scl
    GPIO_Init(stm32ee_hid_i2c_gpio_scl_port, (GPIO_InitTypeDef*)&stm32ee_hid_i2c_gpio_scl_pin);
    
    // configure gpio for sda
    GPIO_Init(stm32ee_hid_i2c_gpio_sda_port, (GPIO_InitTypeDef*)&stm32ee_hid_i2c_gpio_sda_pin);    
    
#else
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    /*!< sEE_I2C Periph clock enable */
    RCC_APB1PeriphClockCmd(sEE_I2C_CLK, ENABLE);
    
    /*!< sEE_I2C_SCL_GPIO_CLK and sEE_I2C_SDA_GPIO_CLK Periph clock enable */
    RCC_APB2PeriphClockCmd(sEE_I2C_SCL_GPIO_CLK | sEE_I2C_SDA_GPIO_CLK, ENABLE);
    // acemor added a RCC_APB2Periph_AFIO 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    

    /* Reset sEE_I2C IP */
    RCC_APB1PeriphResetCmd(sEE_I2C_CLK, ENABLE);
  
    /* Release reset signal of sEE_I2C IP */
    RCC_APB1PeriphResetCmd(sEE_I2C_CLK, DISABLE);
    
    /*!< GPIO configuration */  
    //AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;  /* I2C1 remapping */   
    GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
    
 
 
    /*!< Configure sEE_I2C pins: SCL */
    GPIO_InitStructure.GPIO_Pin = sEE_I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    // GPIO_OType is not used in stm1 lib
    // GPIO_PuPd is not used in stm1 lib
    GPIO_Init(sEE_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure sEE_I2C pins: SDA */
    GPIO_InitStructure.GPIO_Pin = sEE_I2C_SDA_PIN;
    GPIO_Init(sEE_I2C_SDA_GPIO_PORT, &GPIO_InitStructure); 
#endif

#endif
//   GPIO_InitTypeDef  GPIO_InitStructure;

//    // acemor changed and added a RCC_APB2Periph_AFIO 
//   /*!< sEE_I2C_SCL_GPIO_CLK and sEE_I2C_SDA_GPIO_CLK Periph clock enable */
//   RCC_APB2PeriphClockCmd(sEE_I2C_SCL_GPIO_CLK | sEE_I2C_SDA_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);

//   /*!< sEE_I2C Periph clock enable */
//   RCC_APB1PeriphClockCmd(sEE_I2C_CLK, ENABLE);
//     
//   
//     // acemor added:
//     AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;  /* I2C1 remapping */
//     
//   /*!< GPIO configuration */  
//   /*!< Configure sEE_I2C pins: SCL */
//   GPIO_InitStructure.GPIO_Pin = sEE_I2C_SCL_PIN;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
//   GPIO_Init(sEE_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

//   /*!< Configure sEE_I2C pins: SDA */
//   GPIO_InitStructure.GPIO_Pin = sEE_I2C_SDA_PIN;
//   GPIO_Init(sEE_I2C_SDA_GPIO_PORT, &GPIO_InitStructure); 
}


static void s_stm32ee_dma_init(void)
{
#if defined(USE_STM32F4)
#if 1
    // init nvic for dma tx and rx
    NVIC_Init((NVIC_InitTypeDef*)&stm32ee_hid_dma_nvic_tx_enable);
    NVIC_Init((NVIC_InitTypeDef*)&stm32ee_hid_dma_nvic_rx_enable);
    
    // enable dma clock
    RCC_AHB1PeriphClockCmd(stm32ee_hid_dma_clock, ENABLE);
    
    
    // clear pending flags on tx
    DMA_ClearFlag(stm32ee_hid_dma_stream_tx, stm32ee_hid_dma_flags_tx);
    // Disable the EE I2C Tx DMA stream 
    DMA_Cmd(stm32ee_hid_dma_stream_tx, DISABLE);
    
    // Configure the DMA stream for the EE I2C peripheral TX direction 
    DMA_DeInit(stm32ee_hid_dma_stream_tx);
    #warning --> thou shall remove memcpy
    memcpy(&sEEDMA_InitStructure, &stm32ee_hid_dma_cfg_init, sizeof(DMA_InitTypeDef));
    s_stm32ee_dma_extracfg.data         = &sEEDMA_InitStructure.DMA_Memory0BaseAddr;
    s_stm32ee_dma_extracfg.size         = &sEEDMA_InitStructure.DMA_BufferSize;
    s_stm32ee_dma_extracfg.dir          = &sEEDMA_InitStructure.DMA_DIR;
    DMA_Init(stm32ee_hid_dma_stream_tx, (DMA_InitTypeDef*)&stm32ee_hid_dma_cfg_init);    

    
    // clear pending flags on rx
    DMA_ClearFlag(stm32ee_hid_dma_stream_rx, stm32ee_hid_dma_flags_rx);
    /* Disable the EE I2C DMA Rx stream */
    DMA_Cmd(stm32ee_hid_dma_stream_rx, DISABLE);
    
    /* Configure the DMA stream for the EE I2C peripheral RX direction */
    DMA_DeInit(stm32ee_hid_dma_stream_rx);
    DMA_Init(stm32ee_hid_dma_stream_rx, (DMA_InitTypeDef*)&stm32ee_hid_dma_cfg_init);
    
    /* Enable the DMA Channels Interrupts */
    DMA_ITConfig(stm32ee_hid_dma_stream_tx, DMA_IT_TC, ENABLE);
    DMA_ITConfig(stm32ee_hid_dma_stream_rx, DMA_IT_TC, ENABLE);  
    
#else    
    NVIC_InitTypeDef   NVIC_InitStructure;

    /* Configure and enable I2C DMA TX Channel interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = sEE_I2C_DMA_TX_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = sEE_I2C_DMA_PREPRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = sEE_I2C_DMA_SUBPRIO;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure and enable I2C DMA RX Channel interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = sEE_I2C_DMA_RX_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = sEE_I2C_DMA_PREPRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = sEE_I2C_DMA_SUBPRIO;
    NVIC_Init(&NVIC_InitStructure);  


    /*!< I2C DMA TX and RX channels configuration */
    /* Enable the DMA clock */
    RCC_AHB1PeriphClockCmd(sEE_I2C_DMA_CLK, ENABLE);

    /* Clear any pending flag on Tx Stream  */
    DMA_ClearFlag(stm32ee_I2C_DMA_STREAMorCHANNEL_TX, sEE_TX_DMA_FLAG_FEIF | sEE_TX_DMA_FLAG_DMEIF | sEE_TX_DMA_FLAG_TEIF | \
                                       sEE_TX_DMA_FLAG_HTIF | sEE_TX_DMA_FLAG_TCIF);
    /* Disable the EE I2C Tx DMA stream */
    DMA_Cmd(stm32ee_I2C_DMA_STREAMorCHANNEL_TX, DISABLE);
    /* Configure the DMA stream for the EE I2C peripheral TX direction */
    DMA_DeInit(stm32ee_I2C_DMA_STREAMorCHANNEL_TX);
    sEEDMA_InitStructure.DMA_Channel = sEE_I2C_DMA_CHANNEL;
    sEEDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)sEE_I2C_DR_Address;
    sEEDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0;    /* This parameter will be configured durig communication */;
    sEEDMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral; /* This parameter will be configured durig communication */
    sEEDMA_InitStructure.DMA_BufferSize = 0xFFFF;              /* This parameter will be configured durig communication */
    sEEDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    sEEDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    sEEDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    sEEDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    sEEDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    sEEDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    sEEDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
    sEEDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    sEEDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    sEEDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(stm32ee_I2C_DMA_STREAMorCHANNEL_TX, &sEEDMA_InitStructure);

    /* Clear any pending flag on Rx Stream */
    DMA_ClearFlag(stm32ee_I2C_DMA_STREAMorCHANNEL_RX, sEE_RX_DMA_FLAG_FEIF | sEE_RX_DMA_FLAG_DMEIF | sEE_RX_DMA_FLAG_TEIF | \
                                       sEE_RX_DMA_FLAG_HTIF | sEE_RX_DMA_FLAG_TCIF);
    /* Disable the EE I2C DMA Rx stream */
    DMA_Cmd(stm32ee_I2C_DMA_STREAMorCHANNEL_RX, DISABLE);
    /* Configure the DMA stream for the EE I2C peripheral RX direction */
    DMA_DeInit(stm32ee_I2C_DMA_STREAMorCHANNEL_RX);
    DMA_Init(stm32ee_I2C_DMA_STREAMorCHANNEL_RX, &sEEDMA_InitStructure);
    


    /* Enable the DMA Channels Interrupts */
    DMA_ITConfig(stm32ee_I2C_DMA_STREAMorCHANNEL_TX, DMA_IT_TC, ENABLE);
    DMA_ITConfig(stm32ee_I2C_DMA_STREAMorCHANNEL_RX, DMA_IT_TC, ENABLE);  
    
#endif
#elif   defined(USE_STM32F1)

#if 1
    // init nvic for dma tx and rx
    NVIC_Init((NVIC_InitTypeDef*)&stm32ee_hid_dma_nvic_tx_enable);
    NVIC_Init((NVIC_InitTypeDef*)&stm32ee_hid_dma_nvic_rx_enable);
    
    // enable dma clock
    RCC_AHBPeriphClockCmd(stm32ee_hid_dma_clock, ENABLE);
    

//    // clear pending flags on tx
//    DMA_ClearFlag(stm32ee_hid_dma_stream_tx, stm32ee_hid_dma_flags_tx);
//    // Disable the EE I2C Tx DMA stream 
//    DMA_Cmd(stm32ee_hid_dma_stream_tx, DISABLE);
    
    // Configure the DMA stream for the EE I2C peripheral TX direction 
    DMA_DeInit(stm32ee_hid_dma_stream_tx);
    #warning --> thou shall remove memcpy
    memcpy(&sEEDMA_InitStructure, &stm32ee_hid_dma_cfg_init, sizeof(DMA_InitTypeDef));
    s_stm32ee_dma_extracfg.data         = &sEEDMA_InitStructure.DMA_MemoryBaseAddr;
    s_stm32ee_dma_extracfg.size         = &sEEDMA_InitStructure.DMA_BufferSize;
    s_stm32ee_dma_extracfg.dir          = &sEEDMA_InitStructure.DMA_DIR;
    DMA_Init(stm32ee_hid_dma_stream_tx, (DMA_InitTypeDef*)&stm32ee_hid_dma_cfg_init);    

    
//    // clear pending flags on rx
//    DMA_ClearFlag(stm32ee_hid_dma_stream_rx, stm32ee_hid_dma_flags_rx);
//    /* Disable the EE I2C DMA Rx stream */
//    DMA_Cmd(stm32ee_hid_dma_stream_rx, DISABLE);
    
    /* Configure the DMA stream for the EE I2C peripheral RX direction */
    DMA_DeInit(stm32ee_hid_dma_stream_rx);
    DMA_Init(stm32ee_hid_dma_stream_rx, (DMA_InitTypeDef*)&stm32ee_hid_dma_cfg_init);
    
    /* Enable the DMA Channels Interrupts */
    DMA_ITConfig(stm32ee_hid_dma_stream_tx, DMA_IT_TC, ENABLE);
    DMA_ITConfig(stm32ee_hid_dma_stream_rx, DMA_IT_TC, ENABLE);  
        

#else
   NVIC_InitTypeDef   NVIC_InitStructure;

  /* Configure and enable I2C DMA TX Channel interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = sEE_I2C_DMA_TX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = sEE_I2C_DMA_PREPRIO;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = sEE_I2C_DMA_SUBPRIO;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure and enable I2C DMA RX Channel interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = sEE_I2C_DMA_RX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = sEE_I2C_DMA_PREPRIO;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = sEE_I2C_DMA_SUBPRIO;
  NVIC_Init(&NVIC_InitStructure);  

  
  /*!< I2C DMA TX and RX channels configuration */
  /* Enable the DMA clock */
  RCC_AHBPeriphClockCmd(sEE_I2C_DMA_CLK, ENABLE);


  /* I2C TX DMA Channel configuration */
  DMA_DeInit(stm32ee_I2C_DMA_STREAMorCHANNEL_TX);
  sEEDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)sEE_I2C_DR_Address;
  sEEDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;   /* This parameter will be configured durig communication */
  sEEDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;    /* This parameter will be configured durig communication */
  sEEDMA_InitStructure.DMA_BufferSize = 0xFFFF;            /* This parameter will be configured durig communication */
  sEEDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  sEEDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  sEEDMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
  sEEDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  sEEDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  sEEDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  sEEDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(stm32ee_I2C_DMA_STREAMorCHANNEL_TX, &sEEDMA_InitStructure);  
  
  /* I2C RX DMA Channel configuration */
  DMA_DeInit(stm32ee_I2C_DMA_STREAMorCHANNEL_RX);
  DMA_Init(stm32ee_I2C_DMA_STREAMorCHANNEL_RX, &sEEDMA_InitStructure);  
  
  /* Enable the DMA Channels Interrupts */
  DMA_ITConfig(stm32ee_I2C_DMA_STREAMorCHANNEL_TX, DMA_IT_TC, ENABLE);
  DMA_ITConfig(stm32ee_I2C_DMA_STREAMorCHANNEL_RX, DMA_IT_TC, ENABLE);   
  
#endif
#endif
}

static void s_stm32ee_i2c_disable(void)
{   
#if     defined(USE_STM32F1) || defined(USE_STM32F4)    
#if 1
    // i2c disable
    I2C_Cmd(stm32ee_hid_i2c_port, DISABLE);
 
    // i2c deinit
    I2C_DeInit(stm32ee_hid_i2c_port);

    // i2c clock disable
    RCC_APB1PeriphClockCmd(stm32ee_hid_i2c_clock, DISABLE);

#else   
    /* sEE_I2C Peripheral Disable */
    I2C_Cmd(sEE_I2C, DISABLE);
 
    /* sEE_I2C DeInit */
    I2C_DeInit(sEE_I2C);

    /*!< sEE_I2C Periph clock disable */
    RCC_APB1PeriphClockCmd(sEE_I2C_CLK, DISABLE);
#endif

#endif    
}

static void s_stm32ee_i2c_gpio_deinit(void)
{   
#if defined(USE_STM32F4)    
#if 1
    // configure scl and sda as floating pin
    GPIO_Init(stm32ee_hid_i2c_gpio_scl_port, (GPIO_InitTypeDef*)&stm32ee_hid_i2c_gpio_scl_floatingpin);
    GPIO_Init(stm32ee_hid_i2c_gpio_sda_port, (GPIO_InitTypeDef*)&stm32ee_hid_i2c_gpio_sda_floatingpin);
#else
    // for stm32f4
    GPIO_InitTypeDef  GPIO_InitStructure; 

    /*!< GPIO configuration */  
    /*!< Configure sEE_I2C pins: SCL */
    GPIO_InitStructure.GPIO_Pin = sEE_I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(sEE_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure sEE_I2C pins: SDA */
    GPIO_InitStructure.GPIO_Pin = sEE_I2C_SDA_PIN;
    GPIO_Init(sEE_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
#endif

#elif   defined(USE_STM32F1)
#if 1

    // configure scl and sda as floating pin
    GPIO_Init(stm32ee_hid_i2c_gpio_scl_port, (GPIO_InitTypeDef*)&stm32ee_hid_i2c_gpio_scl_floatingpin);
    GPIO_Init(stm32ee_hid_i2c_gpio_sda_port, (GPIO_InitTypeDef*)&stm32ee_hid_i2c_gpio_sda_floatingpin);

#else
     GPIO_InitTypeDef  GPIO_InitStructure;

    /*!< GPIO configuration */  
    /*!< Configure sEE_I2C pins: SCL */
    GPIO_InitStructure.GPIO_Pin = sEE_I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    // GPIO_PuPd is missing in stm32f1
    GPIO_Init(sEE_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure sEE_I2C pins: SDA */
    GPIO_InitStructure.GPIO_Pin = sEE_I2C_SDA_PIN;
    GPIO_Init(sEE_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
#endif
    
#endif
}

static void s_stm32ee_dma_deinit(void)
{   
#if defined(USE_STM32F4)  
#if 1

    // disable tx and rx dma interrupts
    NVIC_Init((NVIC_InitTypeDef*)&stm32ee_hid_dma_nvic_tx_disable);
    NVIC_Init((NVIC_InitTypeDef*)&stm32ee_hid_dma_nvic_rx_disable);

    // disable and deinitialise the dma streams
    DMA_Cmd(stm32ee_hid_dma_stream_tx, DISABLE);
    DMA_Cmd(stm32ee_hid_dma_stream_rx, DISABLE);
    DMA_DeInit(stm32ee_hid_dma_stream_tx);
    DMA_DeInit(stm32ee_hid_dma_stream_rx);
 
#else  
    // for stm32f4
    NVIC_InitTypeDef   NVIC_InitStructure;

    /* Configure and enable I2C DMA TX Stream interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = sEE_I2C_DMA_TX_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = sEE_I2C_DMA_PREPRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = sEE_I2C_DMA_SUBPRIO;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure and enable I2C DMA RX Stream interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = sEE_I2C_DMA_RX_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = sEE_I2C_DMA_PREPRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = sEE_I2C_DMA_SUBPRIO;
    NVIC_Init(&NVIC_InitStructure);   

    /* Disable and Deinitialize the DMA Streams */
    DMA_Cmd(stm32ee_I2C_DMA_STREAMorCHANNEL_TX, DISABLE);
    DMA_Cmd(stm32ee_I2C_DMA_STREAMorCHANNEL_RX, DISABLE);
    DMA_DeInit(stm32ee_I2C_DMA_STREAMorCHANNEL_TX);
    DMA_DeInit(stm32ee_I2C_DMA_STREAMorCHANNEL_RX);
#endif
#elif   defined(USE_STM32F1)
#if 1

    // disable tx and rx dma interrupts
    NVIC_Init((NVIC_InitTypeDef*)&stm32ee_hid_dma_nvic_tx_disable);
    NVIC_Init((NVIC_InitTypeDef*)&stm32ee_hid_dma_nvic_rx_disable);

    // disable and deinitialise the dma streams
    DMA_Cmd(stm32ee_hid_dma_stream_tx, DISABLE);
    DMA_Cmd(stm32ee_hid_dma_stream_rx, DISABLE);
    DMA_DeInit(stm32ee_hid_dma_stream_tx);
    DMA_DeInit(stm32ee_hid_dma_stream_rx);

#else

    NVIC_InitTypeDef   NVIC_InitStructure;
    
  /* Configure and enable I2C DMA TX Channel interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = sEE_I2C_DMA_TX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = sEE_I2C_DMA_PREPRIO;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = sEE_I2C_DMA_SUBPRIO;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure and enable I2C DMA RX Channel interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = sEE_I2C_DMA_RX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = sEE_I2C_DMA_PREPRIO;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = sEE_I2C_DMA_SUBPRIO;
  NVIC_Init(&NVIC_InitStructure);   
  
  /* Disable and Deinitialize the DMA channels */
  DMA_Cmd(stm32ee_I2C_DMA_STREAMorCHANNEL_TX, DISABLE);
  DMA_Cmd(stm32ee_I2C_DMA_STREAMorCHANNEL_RX, DISABLE);
  DMA_DeInit(stm32ee_I2C_DMA_STREAMorCHANNEL_TX);
  DMA_DeInit(stm32ee_I2C_DMA_STREAMorCHANNEL_RX);
#endif

#endif
}


static void s_stm32ee_i2c_enable(void)
{
#if defined(USE_STM32F1) || defined(USE_STM32F4)  
#if 1
    I2C_InitTypeDef i2c_cfg;
    memcpy(&i2c_cfg, &stm32ee_hid_i2c_cfg, sizeof(I2C_InitTypeDef));
    // apply the clockspeed 
    i2c_cfg.I2C_ClockSpeed = s_stm32ee_generics.cfg.i2cspeed;
    // i2c peripheral enable
    I2C_Cmd(stm32ee_hid_i2c_port, ENABLE);
    // apply configuration
    I2C_Init(stm32ee_hid_i2c_port, &i2c_cfg);
     
#else
    
    I2C_InitTypeDef  I2C_InitStructure;

    /*!< I2C configuration */
    /* sEE_I2C configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS7;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;

    /* sEE_I2C Peripheral Enable */
    I2C_Cmd(sEE_I2C, ENABLE);
    /* Apply sEE_I2C configuration after enabling it */
    I2C_Init(sEE_I2C, &I2C_InitStructure);
    
    // il dma e' abilitato solo se ...
    
    /* Enable the sEE_I2C peripheral DMA requests */
    I2C_DMACmd(sEE_I2C, ENABLE);  

#endif 
 
#endif
}

static void s_stm32ee_dma_i2c_enable(void)
{
    I2C_DMACmd(stm32ee_hid_i2c_port, ENABLE);  
}

static void s_stm32ee_dma_config_tx(uint32_t pBuffer, uint32_t BufferSize)
{ 
    *s_stm32ee_dma_extracfg.data    = (uint32_t)pBuffer;
    *s_stm32ee_dma_extracfg.size    = BufferSize;
    *s_stm32ee_dma_extracfg.dir     = stm32ee_hid_dma_dir_MEMORY2PERIPHERAL;
    
    DMA_Init(stm32ee_hid_dma_stream_tx, &sEEDMA_InitStructure); 
}

static void s_stm32ee_dma_config_rx(uint32_t pBuffer, uint32_t BufferSize)
{ 
    *s_stm32ee_dma_extracfg.data    = (uint32_t)pBuffer;
    *s_stm32ee_dma_extracfg.size    = BufferSize;
    *s_stm32ee_dma_extracfg.dir     = stm32ee_hid_dma_dir_PERIPHERAL2MEMORY;
    
    DMA_Init(stm32ee_hid_dma_stream_rx, &sEEDMA_InitStructure); 
}


#if 0
static void s_stm32ee_dma_config(uint32_t pBuffer, uint32_t BufferSize, uint32_t Direction)
{ 


#if defined(USE_STM32F4)      
    
  /* Initialize the DMA with the new parameters */
  if (Direction == sEE_DIRECTION_TX)
  {
    /* Configure the DMA Tx Stream with the buffer address and the buffer size */
    sEEDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)pBuffer;
    sEEDMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;    
    sEEDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;  
    DMA_Init(stm32ee_I2C_DMA_STREAMorCHANNEL_TX, &sEEDMA_InitStructure);  
  }
  else
  { 
    /* Configure the DMA Rx Stream with the buffer address and the buffer size */
    sEEDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)pBuffer;
    sEEDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    sEEDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;      
    DMA_Init(stm32ee_I2C_DMA_STREAMorCHANNEL_RX, &sEEDMA_InitStructure);    
  }
  
#elif   defined(USE_STM32F1)   

  /* Initialize the DMA with the new parameters */
  if (Direction == sEE_DIRECTION_TX)
  {
    /* Configure the DMA Tx Channel with the buffer address and the buffer size */
    sEEDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer;
    sEEDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;    
    sEEDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;  
    DMA_Init(stm32ee_I2C_DMA_STREAMorCHANNEL_TX, &sEEDMA_InitStructure);  
  }
  else
  { 
    /* Configure the DMA Rx Channel with the buffer address and the buffer size */
    sEEDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer;
    sEEDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    sEEDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;      
    DMA_Init(stm32ee_I2C_DMA_STREAMorCHANNEL_RX, &sEEDMA_InitStructure);    
  }  


#endif  
}
#endif

static stm32ee_result_t s_stm32_readbuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t* NumByteToRead)
{  
  /* Set the pointer to the Number of data to be read. This pointer will be used 
      by the DMA Transfer Completer interrupt Handler in order to reset the 
      variable to 0. User should check on this variable in order to know if the 
      DMA transfer has been complete or not. */
  sEEDataReadPointer = NumByteToRead;
  
  /*!< While the bus is busy */
  sEETimeout = stm32ee_hid_timeout_long;
  while(I2C_GetFlagStatus(stm32ee_hid_i2c_port, I2C_FLAG_BUSY))
  {
    if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
  }
  
  /*!< Send START condition */
  I2C_GenerateSTART(stm32ee_hid_i2c_port, ENABLE);
  
  /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
  sEETimeout = stm32ee_hid_timeout_flag;
  while(!I2C_CheckEvent(stm32ee_hid_i2c_port, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
  }
  
  /*!< Send EEPROM address for write */
  I2C_Send7bitAddress(stm32ee_hid_i2c_port, sEEAddress, I2C_Direction_Transmitter);

  /*!< Test on EV6 and clear it */
  sEETimeout = stm32ee_hid_timeout_flag;
  while(!I2C_CheckEvent(stm32ee_hid_i2c_port, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
  } 


//#elif defined (sEE_M24C64_32)

  /*!< Send the EEPROM's internal address to read from: MSB of the address first */
  I2C_SendData(stm32ee_hid_i2c_port, (uint8_t)((ReadAddr & 0xFF00) >> 8));    

  /*!< Test on EV8 and clear it */
  sEETimeout = stm32ee_hid_timeout_flag;
  while(!I2C_CheckEvent(stm32ee_hid_i2c_port, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
  {
    if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
  }

  /*!< Send the EEPROM's internal address to read from: LSB of the address */
  I2C_SendData(stm32ee_hid_i2c_port, (uint8_t)(ReadAddr & 0x00FF));    
  
//#endif /*!< sEE_M24C08 */

  /*!< Test on EV8 and clear it */
  sEETimeout = stm32ee_hid_timeout_flag;
  while(I2C_GetFlagStatus(stm32ee_hid_i2c_port, I2C_FLAG_BTF) == RESET)
  {
    if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
  }
  
  /*!< Send START condition a second time */  
  I2C_GenerateSTART(stm32ee_hid_i2c_port, ENABLE);
  
  /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
  sEETimeout = stm32ee_hid_timeout_flag;
  while(!I2C_CheckEvent(stm32ee_hid_i2c_port, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
  } 
  
  /*!< Send EEPROM address for read */
  I2C_Send7bitAddress(stm32ee_hid_i2c_port, sEEAddress, I2C_Direction_Receiver);  
  
  /* If number of data to be read is 1, then DMA couldn't be used */
  /* One Byte Master Reception procedure (POLLING) ---------------------------*/
  if ((uint16_t)(*NumByteToRead) < 2)
  {
    /* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
    sEETimeout = stm32ee_hid_timeout_flag;
    while(I2C_GetFlagStatus(stm32ee_hid_i2c_port, I2C_FLAG_ADDR) == RESET)
    {
      if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
    }     
    
    /*!< Disable Acknowledgement */
    I2C_AcknowledgeConfig(stm32ee_hid_i2c_port, DISABLE);   
    
    
    #warning --> in stm32f1 they use a sEE_EnterCriticalSection_UserCallback(); it generally disalbe interrupts
    
    /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
    (void)stm32ee_hid_i2c_port->SR2;
    
    /*!< Send STOP Condition */
    I2C_GenerateSTOP(stm32ee_hid_i2c_port, ENABLE);
        
    
        #warning --> in stm32f1 they use a sEE_ExitCriticalSection_UserCallback();
   
    /* Wait for the byte to be received */
    sEETimeout = stm32ee_hid_timeout_flag;
    while(I2C_GetFlagStatus(stm32ee_hid_i2c_port, I2C_FLAG_RXNE) == RESET)
    {
      if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
    }
    
    /*!< Read the byte received from the EEPROM */
    *pBuffer = I2C_ReceiveData(stm32ee_hid_i2c_port);
    
    /*!< Decrement the read bytes counter */
    (uint16_t)(*NumByteToRead)--;        
    
    /* Wait to make sure that STOP control bit has been cleared */
    sEETimeout = stm32ee_hid_timeout_flag;
    while(stm32ee_hid_i2c_port->CR1 & I2C_CR1_STOP)
    {
      if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
    }  
    
    /*!< Re-Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(stm32ee_hid_i2c_port, ENABLE);    
    
    return(stm32ee_res_OK);
  }
  else if(1 == s_stm32ee_generics.cfg.usedmatransfer) /* More than one Byte Master Reception procedure (DMA) -----------------*/
  {
    /*!< Test on EV6 and clear it */
    sEETimeout = stm32ee_hid_timeout_flag;
    while(!I2C_CheckEvent(stm32ee_hid_i2c_port, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
      if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
    }  
    
    /* Configure the DMA Rx Channel with the buffer address and the buffer size */
    //s_stm32ee_dma_config((uint32_t)pBuffer, (uint16_t)(*NumByteToRead), sEE_DIRECTION_RX);
    s_stm32ee_dma_config_rx((uint32_t)pBuffer, (uint16_t)(*NumByteToRead));
    
    /* Inform the DMA that the next End Of Transfer Signal will be the last one */
    I2C_DMALastTransferCmd(stm32ee_hid_i2c_port, ENABLE); 
    
    /* Enable the DMA Rx Stream */
    DMA_Cmd(stm32ee_hid_dma_stream_rx, ENABLE);  

        // and now ... waits for the dma reception
        sEETimeout = stm32ee_hid_timeout_long;
        while ((*sEEDataReadPointer) > 0)
        {
            if((sEETimeout--) == 0) {return s_stm32ee_timeoutexpired();};
        }
    
  }
    else
    {   // must use transfer without dma
        #warning --> to be done....................
        

        /*!< Test on EV6 and clear it */
        sEETimeout = stm32ee_hid_timeout_flag;
        while(!I2C_CheckEvent(stm32ee_hid_i2c_port, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        {
            if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
        }  

        while(*NumByteToRead)
        {
        
            if(1 == (*NumByteToRead))
            {
                /*!< Disable Acknowledgement */
                I2C_AcknowledgeConfig(stm32ee_hid_i2c_port, DISABLE);          
                /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
                (void)stm32ee_hid_i2c_port->SR2;
                /*!< Send STOP Condition */
                I2C_GenerateSTOP(stm32ee_hid_i2c_port, ENABLE);            
            }
        
            /* Wait for the byte to be received */
            sEETimeout = stm32ee_hid_timeout_flag;
            while(I2C_GetFlagStatus(stm32ee_hid_i2c_port, I2C_FLAG_RXNE) == RESET)
            {
              if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
            }
            
            /*!< Read the byte received from the EEPROM */
            *pBuffer = I2C_ReceiveData(stm32ee_hid_i2c_port);
            
            pBuffer++;
            
            /*!< Decrement the read bytes counter */
            (uint16_t)(*NumByteToRead)--;              
        
        }
    
    
        /* Wait to make sure that STOP control bit has been cleared */
        sEETimeout = stm32ee_hid_timeout_flag;
        while(stm32ee_hid_i2c_port->CR1 & I2C_CR1_STOP)
        {
          if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
        }  
        
        /*!< Re-Enable Acknowledgement to be ready for another reception */
        I2C_AcknowledgeConfig(stm32ee_hid_i2c_port, ENABLE);    
   
    
  }
  
  /* If all operations OK, return OK (0) */
  return stm32ee_res_OK;
}

static stm32ee_result_t s_stm32ee_verify_rw_bounds(uint32_t address, uint32_t *size, uint8_t* buffer)
{   
    if((stm32ee_device_none == s_stm32ee_generics.cfg.device) || (NULL == buffer) || (0 == *size) || (address >= s_stm32ee_generics.totalsize))
    {
        return(stm32ee_res_NOK);
    }
    
    // clip address+size to ... s_stm32ee_generics.totalsize
    if((address+(*size)) > s_stm32ee_generics.totalsize)
    {
        uint32_t ss = address+(*size) - s_stm32ee_generics.totalsize;
        (*size) -= ss; 
    }    
 

 
    // so far we manage only .... lower 64k bytes. to manage higher addresses we must split the reading operations or ...
    
    if(address > 64*1024)
    {
        return(stm32ee_res_NOK);
    }
    
    if((address+(*size)) > 64*1024)
    {
        return(stm32ee_res_NOK);
    }
    
    return(stm32ee_res_OK);
    
}


   
static stm32ee_result_t s_stm32ee_writebuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
  uint16_t NumOfPage = 0, NumOfSingle = 0, count = 0;
  uint16_t Addr = 0;

  Addr = WriteAddr % sEE_PAGESIZE;
  count = sEE_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / sEE_PAGESIZE;
  NumOfSingle = NumByteToWrite % sEE_PAGESIZE;
 
  /*!< If WriteAddr is sEE_PAGESIZE aligned  */
  if(Addr == 0) 
  {
    /*!< If NumByteToWrite < sEE_PAGESIZE */
    if(NumOfPage == 0) 
    {
      /* Store the number of data to be written */
      sEEDataNum = NumOfSingle;
      /* Start writing data */
      s_stm32ee_writepage(pBuffer, WriteAddr, (uint16_t*)(&sEEDataNum));
      /* Wait transfer through DMA to be complete */
      sEETimeout = stm32ee_hid_timeout_long;
      while (sEEDataNum > 0)
      {
        if((sEETimeout--) == 0) {return s_stm32ee_timeoutexpired();};
      }
      s_stm32ee_waiteepromstandbystate();
    }
    /*!< If NumByteToWrite > sEE_PAGESIZE */
    else  
    {
      while(NumOfPage--)
      {
        /* Store the number of data to be written */
        sEEDataNum = sEE_PAGESIZE;        
        s_stm32ee_writepage(pBuffer, WriteAddr, (uint16_t*)(&sEEDataNum)); 
        /* Wait transfer to be complete */
        sEETimeout = stm32ee_hid_timeout_long;
        while (sEEDataNum > 0)
        {
          if((sEETimeout--) == 0) {return s_stm32ee_timeoutexpired();};
        }      
        s_stm32ee_waiteepromstandbystate();
        WriteAddr +=  sEE_PAGESIZE;
        pBuffer += sEE_PAGESIZE;
      }

      if(NumOfSingle!=0)
      {
        /* Store the number of data to be written */
        sEEDataNum = NumOfSingle;          
        s_stm32ee_writepage(pBuffer, WriteAddr, (uint16_t*)(&sEEDataNum));
        /* Wait transfer to be complete */
        sEETimeout = stm32ee_hid_timeout_long;
        while (sEEDataNum > 0)
        {
          if((sEETimeout--) == 0) {return s_stm32ee_timeoutexpired();};
        }    
        s_stm32ee_waiteepromstandbystate();
      }
    }
  }
  /*!< If WriteAddr is not sEE_PAGESIZE aligned  */
  else 
  {
    /*!< If NumByteToWrite < sEE_PAGESIZE */
    if(NumOfPage== 0) 
    {
      /*!< If the number of data to be written is more than the remaining space 
      in the current page: */
      if (NumByteToWrite > count)
      {
        /* Store the number of data to be written */
        sEEDataNum = count;        
        /*!< Write the data contained in same page */
        s_stm32ee_writepage(pBuffer, WriteAddr, (uint16_t*)(&sEEDataNum));
        /* Wait transfer to be complete */
        sEETimeout = stm32ee_hid_timeout_long;
        while (sEEDataNum > 0)
        {
          if((sEETimeout--) == 0) {return s_stm32ee_timeoutexpired();};
        }          
        s_stm32ee_waiteepromstandbystate();      
        
        /* Store the number of data to be written */
        sEEDataNum = (NumByteToWrite - count);          
        /*!< Write the remaining data in the following page */
        s_stm32ee_writepage((uint8_t*)(pBuffer + count), (WriteAddr + count), (uint16_t*)(&sEEDataNum));
        /* Wait transfer to be complete */
        sEETimeout = stm32ee_hid_timeout_long;
        while (sEEDataNum > 0)
        {
          if((sEETimeout--) == 0) {return s_stm32ee_timeoutexpired();};
        }     
        s_stm32ee_waiteepromstandbystate();        
      }      
      else      
      {
        /* Store the number of data to be written */
        sEEDataNum = NumOfSingle;         
        s_stm32ee_writepage(pBuffer, WriteAddr, (uint16_t*)(&sEEDataNum));
        /* Wait transfer to be complete */
        sEETimeout = stm32ee_hid_timeout_long;
        while (sEEDataNum > 0)
        {
          if((sEETimeout--) == 0) {return s_stm32ee_timeoutexpired();};
        }          
        s_stm32ee_waiteepromstandbystate();        
      }     
    }
    /*!< If NumByteToWrite > sEE_PAGESIZE */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / sEE_PAGESIZE;
      NumOfSingle = NumByteToWrite % sEE_PAGESIZE;
      
      if(count != 0)
      {  
        /* Store the number of data to be written */
        sEEDataNum = count;         
        s_stm32ee_writepage(pBuffer, WriteAddr, (uint16_t*)(&sEEDataNum));
        /* Wait transfer to be complete */
        sEETimeout = stm32ee_hid_timeout_long;
        while (sEEDataNum > 0)
        {
          if((sEETimeout--) == 0) {return s_stm32ee_timeoutexpired();};
        }     
        s_stm32ee_waiteepromstandbystate();
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        /* Store the number of data to be written */
        sEEDataNum = sEE_PAGESIZE;          
        s_stm32ee_writepage(pBuffer, WriteAddr, (uint16_t*)(&sEEDataNum));
        /* Wait transfer to be complete */
        sEETimeout = stm32ee_hid_timeout_long;
        while (sEEDataNum > 0)
        {
          if((sEETimeout--) == 0) {return s_stm32ee_timeoutexpired();};
        }        
        s_stm32ee_waiteepromstandbystate();
        WriteAddr +=  sEE_PAGESIZE;
        pBuffer += sEE_PAGESIZE;  
      }
      if(NumOfSingle != 0)
      {
        /* Store the number of data to be written */
        sEEDataNum = NumOfSingle;           
        s_stm32ee_writepage(pBuffer, WriteAddr, (uint16_t*)(&sEEDataNum)); 
        /* Wait transfer to be complete */
        sEETimeout = stm32ee_hid_timeout_long;
        while (sEEDataNum > 0)
        {
          if((sEETimeout--) == 0) {return s_stm32ee_timeoutexpired();};
        }         
        s_stm32ee_waiteepromstandbystate();
      }
    }
  }

    return(stm32ee_res_OK);
}



static stm32ee_result_t s_stm32ee_writepage(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t* NumByteToWrite)
{ 
  /* Set the pointer to the Number of data to be written. This pointer will be used 
      by the DMA Transfer Completer interrupt Handler in order to reset the 
      variable to 0. User should check on this variable in order to know if the 
      DMA transfer has been complete or not. */
  sEEDataWritePointer = NumByteToWrite;  
  
  /*!< While the bus is busy */
  sEETimeout = stm32ee_hid_timeout_long;
  while(I2C_GetFlagStatus(stm32ee_hid_i2c_port, I2C_FLAG_BUSY))
  {
    if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
  }
  
  /*!< Send START condition */
  I2C_GenerateSTART(stm32ee_hid_i2c_port, ENABLE);
  
  /*!< Test on EV5 and clear it */
  sEETimeout = stm32ee_hid_timeout_flag;
  while(!I2C_CheckEvent(stm32ee_hid_i2c_port, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
  }
  
  /*!< Send EEPROM address for write */
  sEETimeout = stm32ee_hid_timeout_flag;
  I2C_Send7bitAddress(stm32ee_hid_i2c_port, sEEAddress, I2C_Direction_Transmitter);

  /*!< Test on EV6 and clear it */
  sEETimeout = stm32ee_hid_timeout_flag;
  while(!I2C_CheckEvent(stm32ee_hid_i2c_port, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
  }

//#ifdef sEE_M24C08
//  
//  /*!< Send the EEPROM's internal address to write to : only one byte Address */
//  I2C_SendData(stm32ee_hid_i2c_port, WriteAddr);
//  
//#elif defined(sEE_M24C64_32)
  
  
  /*!< Send the EEPROM's internal address to write to : MSB of the address first */
  I2C_SendData(stm32ee_hid_i2c_port, (uint8_t)((WriteAddr & 0xFF00) >> 8));

  /*!< Test on EV8 and clear it */
  sEETimeout = stm32ee_hid_timeout_flag;  
  while(!I2C_CheckEvent(stm32ee_hid_i2c_port, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
  {
    if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
  }  
  
  /*!< Send the EEPROM's internal address to write to : LSB of the address */
  I2C_SendData(stm32ee_hid_i2c_port, (uint8_t)(WriteAddr & 0x00FF));
  
//#endif /*!< sEE_M24C08 */  
  
  /*!< Test on EV8 and clear it */
  sEETimeout = stm32ee_hid_timeout_flag; 
  while(!I2C_CheckEvent(stm32ee_hid_i2c_port, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
  {
    if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
  } 

    if(1 == s_stm32ee_generics.cfg.usedmatransfer)
    { 
        /* Configure the DMA Tx Channel with the buffer address and the buffer size */
        //s_stm32ee_dma_config((uint32_t)pBuffer, (uint32_t)(*NumByteToWrite), sEE_DIRECTION_TX);
        s_stm32ee_dma_config_tx((uint32_t)pBuffer, (uint32_t)(*NumByteToWrite));

        /* Enable the DMA Tx Stream */
        DMA_Cmd(stm32ee_hid_dma_stream_tx, ENABLE);
    }
    else
    {
        #warning --> the transfer w/out dma is to be done
        
        while(*NumByteToWrite)
        {       
            /*!< Send the byte to be written */
            I2C_SendData(stm32ee_hid_i2c_port, *pBuffer); 
            
            pBuffer++;
            (*NumByteToWrite)--;
            
            /*!< Test on EV8 and clear it */
            sEETimeout = stm32ee_hid_timeout_flag; 
            while(!I2C_CheckEvent(stm32ee_hid_i2c_port, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
            {
                if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
            }  
        }        
 
        /*!< Send STOP condition */
        I2C_GenerateSTOP(stm32ee_hid_i2c_port, ENABLE);
        
        /* Perform a read on SR1 and SR2 register to clear eventualaly pending flags */
        (void)stm32ee_hid_i2c_port->SR1;
        (void)stm32ee_hid_i2c_port->SR2;        
        // end        
    }
  
  /* If all operations OK, return OK (0) */
  return stm32ee_res_OK;
}


static stm32ee_result_t s_stm32ee_waiteepromstandbystate(void)      
{
  volatile uint16_t tmpSR1 = 0;
  volatile uint32_t sEETrials = 0;

  /*!< While the bus is busy */
  sEETimeout = stm32ee_hid_timeout_long;
  while(I2C_GetFlagStatus(stm32ee_hid_i2c_port, I2C_FLAG_BUSY))
  {
    if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
  }

  /* Keep looping till the slave acknowledge his address or maximum number 
     of trials is reached (this number is defined by stm32ee_hid_ackaddress_maxtrials define
     in stm324xg_eval_i2c_ee.h file) */
  while (1)
  {
    /*!< Send START condition */
    I2C_GenerateSTART(stm32ee_hid_i2c_port, ENABLE);

    /*!< Test on EV5 and clear it */
    sEETimeout = stm32ee_hid_timeout_flag;
    while(!I2C_CheckEvent(stm32ee_hid_i2c_port, I2C_EVENT_MASTER_MODE_SELECT))
    {
      if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
    }    

    /*!< Send EEPROM address for write */
    I2C_Send7bitAddress(stm32ee_hid_i2c_port, sEEAddress, I2C_Direction_Transmitter);
    
    /* Wait for ADDR flag to be set (Slave acknowledged his address) */
    sEETimeout = stm32ee_hid_timeout_long;
    do
    {     
      /* Get the current value of the SR1 register */
      tmpSR1 = stm32ee_hid_i2c_port->SR1;
      
      /* Update the timeout value and exit if it reach 0 */
      if((sEETimeout--) == 0) return s_stm32ee_timeoutexpired();
    }
    /* Keep looping till the Address is acknowledged or the AF flag is 
       set (address not acknowledged at time) */
    while((tmpSR1 & (I2C_SR1_ADDR | I2C_SR1_AF)) == 0);
     
    /* Check if the ADDR flag has been set */
    if (tmpSR1 & I2C_SR1_ADDR)
    {
      /* Clear ADDR Flag by reading SR1 then SR2 registers (SR1 have already 
         been read) */
      (void)stm32ee_hid_i2c_port->SR2;
      
      /*!< STOP condition */    
      I2C_GenerateSTOP(stm32ee_hid_i2c_port, ENABLE);
        
      /* Exit the function */
      return stm32ee_res_OK;
    }
    else
    {
      /*!< Clear AF flag */
      I2C_ClearFlag(stm32ee_hid_i2c_port, I2C_FLAG_AF);                  
    }
    
    /* Check if the maximum allowed number of trials has bee reached */
    if (sEETrials++ == stm32ee_hid_ackaddress_maxtrials)
    {
      /* If the maximum number of trials has been reached, exit the function */
      return s_stm32ee_timeoutexpired();
    }
  }
}




static stm32ee_result_t s_stm32ee_timeoutexpired(void) 
{
    if(NULL != s_stm32ee_generics.cfg.functionontimeout)
    {
        s_stm32ee_generics.cfg.functionontimeout();
        return(stm32ee_res_NOK);
    }
    else
    {
        for(;;);
    }
    
    return(stm32ee_res_NOK);
}


//#endif//HAL_USE_EEPROM

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



