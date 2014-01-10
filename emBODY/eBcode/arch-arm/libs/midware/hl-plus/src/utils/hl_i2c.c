/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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


/* @file       hl_i2c.c
	@brief      This file contains internal implementation of the hl i2c utilities.
	@author     marco.accame@iit.it
    @date       10/23/2013
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------

#include "hl_cfg_plus_modules.h"


#if     defined(HL_USE_UTIL_I2C)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "hl_core.h"        // contains the required stm32f10x_*.h or stm32f4xx*.h header files

#include "hl_gpio.h"
#include "hl_sys.h"

#include "hl_bits.h" 

#include "hl_arch.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_i2c.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_i2c_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HL_i2c_id2index(p)           ((uint8_t)(p))

#define HL_i2c_id2stmI2C(p)          (s_hl_i2c_stmI2Cmap[HL_i2c_id2index(p)])


// by defining this macro we use a function for master read found in file I2CRoutines.c of AN2824 by stmicroelectonics
#define HL_I2C_USE_CODE_FROM_I2CRoutines_I2C_Master_BufferRead


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

const hl_i2c_cfg_t hl_i2c_cfg_default = 
{ 
    .mode           = hl_i2c_mode_master, 
    .behaviour      = hl_i2c_behaviour_polling,
    .speed          = hl_i2c_speed_400kbps, 
    .ownaddress     = 0,
    .advcfg         = NULL
};


const hl_i2c_advcfg_t hl_i2c_advcfg_default =
{
   .I2C_ClockSpeed             = 400000,                       // changed by cfg->speed
   .I2C_Mode                   = I2C_Mode_I2C,
   .I2C_DutyCycle              = I2C_DutyCycle_2,
   .I2C_OwnAddress1            = 0,                            // changed by cfg->ownaddress
   .I2C_Ack                    = I2C_Ack_Enable,
   .I2C_AcknowledgedAddress    = I2C_AcknowledgedAddress_7bit
}; 


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    uint8_t         initted;
} hl_i2c_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static hl_boolval_t s_hl_i2c_supported_is(hl_i2c_t id);
static void s_hl_i2c_initted_set(hl_i2c_t id);
static hl_boolval_t s_hl_i2c_initted_is(hl_i2c_t id);


static hl_result_t s_hl_i2c_init(hl_i2c_t id, const hl_i2c_cfg_t *cfg);

static void s_hl_i2c_hw_clocks_init(hl_i2c_t id);
static void s_hl_i2c_hw_gpio_init(hl_i2c_t id);
static void s_hl_i2c_hw_periph_init(hl_i2c_t id, const hl_i2c_cfg_t* cfg);

static void s_hl_i2c_fill_gpio_init_altf(hl_i2c_t id, hl_gpio_init_t* sclinit, hl_gpio_altf_t* sclaltf, hl_gpio_init_t* sdainit, hl_gpio_altf_t* sdaaltf);

static hl_result_t s_i2c_read(hl_i2c_t id, hl_i2c_devaddr_t devaddr, hl_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);


static hl_result_t s_hl_i2c_write(hl_i2c_t id, hl_i2c_devaddr_t devaddr, hl_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);


static hl_result_t s_hl_i2c_ping(hl_i2c_t id, hl_i2c_devaddr_t devaddr);

static hl_result_t s_hl_i2c_isdeviceready(hl_i2c_t id, hl_i2c_devaddr_t devaddr);

static hl_result_t s_hl_i2c_timeoutexpired(char *fname);


static hl_result_t s_hl_i2c_wait_if_busy(I2C_TypeDef* I2Cx);
static hl_result_t s_hl_i2c_transaction_begin(I2C_TypeDef* I2Cx, uint8_t devaddr, uint8_t dir);
static hl_result_t s_hl_i2c_write_data_polling(I2C_TypeDef* I2Cx, hl_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);
static hl_result_t s_hl_i2c_transaction_end(I2C_TypeDef* I2Cx, uint8_t dir);

#if     defined(HL_I2C_USE_CODE_FROM_I2CRoutines_I2C_Master_BufferRead)
static hl_result_t s_I2CRoutines_I2C_Master_BufferRead(I2C_TypeDef* I2Cx, uint8_t* pBuffer,  uint32_t NumByteToRead, uint8_t SlaveAddress);
#endif//defined(HL_I2C_USE_CODE_FROM_I2CRoutines_I2C_Master_BufferRead)

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static const uint32_t s_hl_i2c_timeout_flag = 0x00010000;
static const uint32_t s_hl_i2c_timeout_long = 0x01000000;
//static const uint32_t s_hl_i2c_ackaddress_maxtrials = 1000;


#if     defined(HL_USE_MPU_ARCH_STM32F1)
static I2C_TypeDef* const s_hl_i2c_stmI2Cmap[] = { I2C1, I2C2, NULL };
static const uint32_t s_hl_i2c_hw_rcc[] = { RCC_APB1Periph_I2C1, RCC_APB1Periph_I2C2, 0 };
#elif   defined(HL_USE_MPU_ARCH_STM32F4)
static I2C_TypeDef* const s_hl_i2c_stmI2Cmap[] = { I2C1, I2C2, I2C3 };
static const uint32_t s_hl_i2c_hw_rcc[] = { RCC_APB1Periph_I2C1, RCC_APB1Periph_I2C2, RCC_APB1Periph_I2C3 };
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERR --> choose a HL_USE_MPU_ARCH_*
#endif 


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hl_i2c_theinternals_t s_hl_i2c_theinternals = 
{
    .initted = 0    
};


    
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hl_result_t hl_i2c_init(hl_i2c_t id, const hl_i2c_cfg_t *cfg)
{
    if(NULL == cfg)
    {
        cfg = &hl_i2c_cfg_default;
    }

//#if     !defined(HL_BEH_I2C_REMOVE_INIT_PARAM_CHECK)      
    
    if(hl_i2c_mode_slave == cfg->mode)
    {
        return(hl_res_NOK_unsupported);
    }
    
    if(hl_i2c_behaviour_polling != cfg->behaviour)
    {
        return(hl_res_NOK_unsupported);
    }    
    
    if(hl_true != s_hl_i2c_supported_is(id))
    {
        return(hl_res_NOK_unsupported);
    }
    
//#endif//!defined(HL_BEH_I2C_REMOVE_INIT_PARAM_CHECK)  
    
    
    if(hl_true == hl_i2c_initted_is(id))
    {   // must keep the control over initted bus because teh function is supposed to avoid it tiwce
        return(hl_res_OK);
    }  
             
    
    hl_result_t res = s_hl_i2c_init(id, cfg);


    if(hl_res_OK == res)
    {
        s_hl_i2c_initted_set(id);
    }
     
        
    return(res);
}


extern hl_bool_t hl_i2c_initted_is(hl_i2c_t id)
{
    return(s_hl_i2c_initted_is(id));
}


extern hl_result_t hl_i2c_read(hl_i2c_t id, hl_i2c_devaddr_t devaddr, hl_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
    
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)               

    if(hl_false == hl_i2c_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }
    
    if(0 == devaddr)
    {
        return(hl_res_NOK_generic);
    }
    
    if((0 == regaddr.numofbytes) || (regaddr.numofbytes > 3))
    {
        return(hl_res_NOK_unsupported);
    }  
    
    if(NULL == data)
    {
        return(hl_res_NOK_nullpointer);
    }
    
    if(0 == size)
    {
        return(hl_res_OK);
    }

#endif//!defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)  

    // we make sure lsb is zero
    devaddr &= ~0x01;

    // ok. read it
    hl_result_t res = s_i2c_read(id, devaddr, regaddr, data, size);
    
    return(res);
}


extern hl_result_t hl_i2c_write(hl_i2c_t id, hl_i2c_devaddr_t devaddr, hl_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
 
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)              
 
    if(hl_false == hl_i2c_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }

    if(0 == devaddr)
    {
        return(hl_res_NOK_generic);
    }   
  
    
    if((0 == regaddr.numofbytes) || (regaddr.numofbytes > 3))
    {
        return(hl_res_NOK_unsupported);
    }  
    
    if(NULL == data)
    {
        return(hl_res_NOK_nullpointer);
    }
    
    if(0 == size)
    {
        return(hl_res_OK);
    }
    
#endif//!defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK) 

    // we make sure lsb is zero 
    devaddr &= ~0x01;  
    
    // we write
    hl_result_t res = s_hl_i2c_write(id, devaddr, regaddr, data, size);
        
    return(res);    
}


extern hl_result_t hl_i2c_ping(hl_i2c_t id, hl_i2c_devaddr_t devaddr)
{

#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)               
    
    if(hl_false == hl_i2c_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }
    
    if(0 == devaddr)
    {
        return(hl_res_NOK_generic);
    }
    
#endif//!defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)    
 
    // we make sure lsb is zero 
    devaddr &= ~0x01;
      
    hl_result_t res = s_hl_i2c_ping(id, devaddr);
    
//     if(hl_res_OK == res)
//     {
//         s_hl_i2c_isdeviceready(id, devaddr);
//     }
//     else
//     {
//         
//     }
       
    return(res);            
}


extern hl_result_t hl_i2c_is_device_ready(hl_i2c_t id, hl_i2c_devaddr_t devaddr)      
{
 
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)               
 
    if(hl_false == hl_i2c_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }
    
    if(0 == devaddr)
    {
        return(hl_res_NOK_generic);
    }
    
#endif//!defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)     
       
    // we make sure lsb is zero 
    devaddr &= ~0x01;
           
    hl_result_t res = s_hl_i2c_isdeviceready(id, devaddr);
    
    return(res);    
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static hl_boolval_t s_hl_i2c_supported_is(hl_i2c_t id)
{
    return(hl_bits_byte_bitcheck(hl_i2c_mapping.supported_mask, HL_i2c_id2index(id)) );
}

static void s_hl_i2c_initted_set(hl_i2c_t id)
{
    hl_bits_byte_bitset(&s_hl_i2c_theinternals.initted, HL_i2c_id2index(id));
}

static hl_boolval_t s_hl_i2c_initted_is(hl_i2c_t id)
{
    return(hl_bits_byte_bitcheck(s_hl_i2c_theinternals.initted, HL_i2c_id2index(id)));
}


static hl_result_t s_hl_i2c_init(hl_i2c_t id, const hl_i2c_cfg_t *cfg)
{
    // acemor: very important:
    // init the scl and sda gpio before calling hw_init() (or is it hw_enable() ?) 
    // because if the i2c is already initted and it detects sda or scl low it sets
    // register I2C_SR2.BUSY to 1, which makes things hang up.
    s_hl_i2c_hw_gpio_init(id);
    s_hl_i2c_hw_clocks_init(id);
    s_hl_i2c_hw_periph_init(id, cfg);
        
    
    return(hl_res_OK);
}


static void s_hl_i2c_hw_gpio_init(hl_i2c_t id)
{
    hl_gpio_init_t gpio_init_scl;    
    hl_gpio_altf_t gpio_altf_scl;    
    hl_gpio_init_t gpio_init_sda;
    hl_gpio_altf_t gpio_altf_sda;   
    

    s_hl_i2c_fill_gpio_init_altf(id, &gpio_init_scl, &gpio_altf_scl, &gpio_init_sda, &gpio_altf_sda);
    
    hl_gpio_init(&gpio_init_scl);
    hl_gpio_altf(&gpio_altf_scl);
    
    hl_gpio_init(&gpio_init_sda);
    hl_gpio_altf(&gpio_altf_sda);

}


static void s_hl_i2c_hw_clocks_init(hl_i2c_t id)
{
#if     defined(HL_USE_MPU_ARCH_STM32F1) || defined(HL_USE_MPU_ARCH_STM32F4)
    
    uint32_t RCC_APB1Periph_I2Cx = s_hl_i2c_hw_rcc[HL_i2c_id2index(id)];
    
    // i2c periph clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2Cx, ENABLE);
    
    // reset i2c periph
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2Cx, ENABLE);
    
    // release reset 
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2Cx, DISABLE);

#else //defined(HL_USE_MPU_ARCH_*)
    #error ERR --> choose a HL_USE_MPU_ARCH_*
#endif 
}


static void s_hl_i2c_hw_periph_init(hl_i2c_t id, const hl_i2c_cfg_t* cfg)
{
    
    I2C_TypeDef* I2Cx = HL_i2c_id2stmI2C(id);
    
    I2C_InitTypeDef* init2use = NULL;
    I2C_InitTypeDef i2cinit;
    
    if(NULL != cfg->advcfg)
    {
        init2use = (I2C_InitTypeDef*) cfg->advcfg;
    }
    else
    {
        init2use = &i2cinit;
        
        memcpy(init2use, &hl_i2c_advcfg_default, sizeof(I2C_InitTypeDef)); 
        
        // apply what in cfg 
        init2use->I2C_ClockSpeed  = (uint32_t)cfg->speed * 100 * 1000;
        init2use->I2C_OwnAddress1 = cfg->ownaddress;        
    }
     
    // apply configuration (I2C_Init() also enables the periph, thus I2C_Cmd(I2Cx, ENABLE) is not required).
    I2C_Init(I2Cx, init2use);
}



static hl_result_t s_i2c_read(hl_i2c_t id, hl_i2c_devaddr_t devaddr, hl_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
    hl_result_t res = hl_res_OK;    
    I2C_TypeDef* I2Cx = HL_i2c_id2stmI2C(id);
        
    res = s_hl_i2c_wait_if_busy(I2Cx);
    if(hl_res_OK != res)
    {
        s_hl_i2c_timeoutexpired("s_hl_i2c_wait_if_busy()");
        return(res);
    }
    

    
    if(0 != regaddr.numofbytes)
    {
        // write the address of register
        res = s_hl_i2c_transaction_begin(I2Cx, devaddr, I2C_Direction_Transmitter);   
        if(hl_res_OK != res)
        {
            s_hl_i2c_timeoutexpired("s_hl_i2c_transaction_begin()");
            return(res);
        }  
        
        res = s_hl_i2c_write_data_polling(I2Cx, regaddr, NULL, 0);  
        if(hl_res_OK != res)
        {
            s_hl_i2c_timeoutexpired("s_hl_i2c_write_data_polling()");
            return(res);
        }  
        
        res = s_hl_i2c_transaction_end(I2Cx, I2C_Direction_Transmitter);
        if(hl_res_OK != res)
        {
            s_hl_i2c_timeoutexpired("s_hl_i2c_transaction_end()");
            return(res);
        } 
    }    
    
    // read data
    
#if     defined(HL_I2C_USE_CODE_FROM_I2CRoutines_I2C_Master_BufferRead)
    
    res = s_I2CRoutines_I2C_Master_BufferRead(I2Cx, data, size, devaddr);
    
    if(hl_res_OK != res)
    {
        s_hl_i2c_timeoutexpired("s_I2CRoutines_I2C_Master_BufferRead()");
        return(res);
    }
    
#else
    #error HL: s_i2c_read() misses C code
#endif
    
    

    return(hl_res_OK);  
}




static hl_result_t s_hl_i2c_write(hl_i2c_t id, hl_i2c_devaddr_t devaddr, hl_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
    hl_result_t res = hl_res_OK; 
    I2C_TypeDef* I2Cx = HL_i2c_id2stmI2C(id);
        
    res = s_hl_i2c_wait_if_busy(I2Cx);
    if(hl_res_OK != res)
    {
        s_hl_i2c_timeoutexpired("s_hl_i2c_wait_if_busy()");
        return(res);
    }   
    
    res = s_hl_i2c_transaction_begin(I2Cx, devaddr, I2C_Direction_Transmitter);
    if(hl_res_OK != res)
    {
        s_hl_i2c_timeoutexpired("s_hl_i2c_transaction_begin()");
        return(res);
    }      
     
    res = s_hl_i2c_write_data_polling(I2Cx, regaddr, data, size);
    if(hl_res_OK != res)
    {
        s_hl_i2c_timeoutexpired("s_hl_i2c_write_data_polling()");
        return(res);
    }          
    
    res = s_hl_i2c_transaction_end(I2Cx, I2C_Direction_Transmitter);
    if(hl_res_OK != res)
    {
        s_hl_i2c_timeoutexpired("s_hl_i2c_transaction_end()");
        return(res);
    }      
    
    return(hl_res_OK);
}



static hl_result_t s_hl_i2c_ping(hl_i2c_t id, hl_i2c_devaddr_t devaddr)      
{
    hl_result_t res = hl_res_OK; 
    I2C_TypeDef* I2Cx = HL_i2c_id2stmI2C(id);
        
    res = s_hl_i2c_wait_if_busy(I2Cx);
    if(hl_res_OK != res)
    {
        return(res);
    }   
    
    res = s_hl_i2c_transaction_begin(I2Cx, devaddr, I2C_Direction_Transmitter);

    // must terminate the transaction even if s_hl_i2c_transaction_begin() failed to hook the devaddr.
    // i dont care abou the return value of s_hl_i2c_transaction_end().
    s_hl_i2c_transaction_end(I2Cx, I2C_Direction_Transmitter);

    
    return(res);
}


static hl_result_t s_hl_i2c_isdeviceready(hl_i2c_t id, hl_i2c_devaddr_t devaddr)
{   
    // code taken from CPAL_I2C_IsDeviceReady() in file cpal_i2c.c V1.2.0 of 21dec2012
    // refer to: UM1029 Communication peripheral application library (by stmicroelectronics) 
    
    I2C_TypeDef* I2Cx = HL_i2c_id2stmI2C(id);
    volatile uint32_t timeout = 0;
    
    I2C_Cmd(I2Cx, DISABLE);
    I2C_Cmd(I2Cx, ENABLE);
    
    I2C_GenerateSTART(I2Cx, ENABLE);
    
    // test on ev5
    timeout = s_hl_i2c_timeout_flag;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_hl_i2c_isdeviceready2():ev5");
    } 
    
    // send address of device
    I2C_Send7bitAddress(I2Cx, devaddr, I2C_Direction_Transmitter);
    
    // wait until the device acknowledges the address (addr flag is on 0x0002 on sr1) or timeout expires
    timeout = s_hl_i2c_timeout_flag;
    while(((I2Cx->SR1 & ((uint16_t)0x0002)) == 0) && (timeout-- != 0));
    
    if(0 == timeout)
    {   // timeout occurred
        return(hl_res_NOK_generic);
    }
    
    // else 

    I2C_ClearFlag(I2Cx, I2C_FLAG_AF); // clear acknowledge failure ... 
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    // wait until busy flag is reset (bus busy is 0x0002 or sr2)
    timeout = s_hl_i2c_timeout_flag;
    //while(SET == I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    while((I2Cx->SR2 & ((uint16_t)0x0002)))
    {
        if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_hl_i2c_isdeviceready2():I2C_FLAG_BUSY");
    }

    // perform a read on SR1 and SR2 register to clear eventually pending flags 
    timeout = I2C_GetLastEvent(I2Cx);
    
    I2C_Cmd(I2Cx, DISABLE);
    I2C_Cmd(I2Cx, ENABLE);
    
    I2C_AcknowledgeConfig(I2Cx, ENABLE);

    return(hl_res_OK);

}

// old function ...
// static hl_result_t s_hl_i2c_isdeviceready_old(hl_i2c_t id, hl_i2c_devaddr_t devaddr)      
// {
//     volatile uint16_t tmpSR1 = 0;
//     volatile uint32_t trials = 0;
//     volatile uint32_t timeout = 0;
//     
//     I2C_TypeDef* I2Cx = HL_i2c_id2stmI2C(id);
//  
//     // wait until the bus is not busy 
//     timeout = s_hl_i2c_timeout_long;
//     while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
//     {
//         if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_hl_i2c_isdeviceready:flag_busy");
//     }

//     //  keep looping till the slave acknowledge his address or maximum number of trials is reached
//     while(1)
//     {
//         // send START condition
//         I2C_GenerateSTART(I2Cx, ENABLE);

//         // test on ev5
//         timeout = s_hl_i2c_timeout_flag;
//         while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
//         {
//             if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_hl_i2c_isdeviceready:ev5");
//         }    

//         // send address of device
//         I2C_Send7bitAddress(I2Cx, devaddr, I2C_Direction_Transmitter);

//         // Wait for ADDR flag to be set (Slave acknowledged his address) 
//         timeout = s_hl_i2c_timeout_long;
//         do
//         {     
//             // Get the current value of the SR1 register 
//             tmpSR1 = (I2Cx)->SR1;

//             // Update the timeout value and exit if it reach 0
//             if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_hl_i2c_isdeviceready:addrflag_set");
//         }
//         // Keep looping till the Address is acknowledged or the AF (ack failure) flag is 
//         // set (address not acknowledged at time) 
//         while((tmpSR1 & (I2C_SR1_ADDR | I2C_SR1_AF)) == 0);

//         // Check if the ADDR flag has been set
//         if (tmpSR1 & I2C_SR1_ADDR)
//         {
//             // Clear ADDR Flag by reading SR1 then SR2 registers (SR1 have already been read) 
//             (void)(I2Cx)->SR2;

//             // STOP condition   
//             I2C_GenerateSTOP(I2Cx, ENABLE);

//             // Exit the function
//             return hl_res_OK;
//         }
//         else
//         {
//             // Clear AF flag
//             I2C_ClearFlag(I2Cx, I2C_FLAG_AF);                  
//         }

//         // Check if the maximum allowed number of trials has been reached
//         if (s_hl_i2c_ackaddress_maxtrials == trials++)
//         {
//             // If the maximum number of trials has been reached, exit the function 
//             return s_hl_i2c_timeoutexpired("s_hl_i2c_isdeviceready:maxtrials");
//         }
//     }
// }


static hl_result_t s_hl_i2c_timeoutexpired(char *fname)
{ 
    char str[64];
    snprintf(str, sizeof(str), "i2c timeout in function %s", fname);
    hl_sys_on_error(hl_error_runtimefault, str);
    return(hl_res_NOK_generic);
}





    

static void s_hl_i2c_fill_gpio_init_altf(hl_i2c_t id, hl_gpio_init_t* sclinit, hl_gpio_altf_t* sclaltf, hl_gpio_init_t* sdainit, hl_gpio_altf_t* sdaaltf)
{
    static const hl_gpio_init_t s_hl_i2c_sclsda_gpio_init  =
    {
#if     defined(HL_USE_MPU_ARCH_STM32F1)  
        .port           = hl_gpio_portNONE,
        .mode           =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_speed     = GPIO_Speed_50MHz,
            .gpio_mode      = GPIO_Mode_AF_OD        
        }
#elif   defined(HL_USE_MPU_ARCH_STM32F4)  
        .port        = hl_gpio_portNONE,
        .mode        =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_mode      = GPIO_Mode_AF,
            .gpio_speed     = GPIO_Speed_50MHz,
            .gpio_otype     = GPIO_OType_OD,
            .gpio_pupd      = GPIO_PuPd_NOPULL
        }
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERR --> choose a HL_USE_MPU_ARCH_*
#endif         
    }; 
    
    // at first we copy the default configuration of pin scl and sda
    
    memcpy(sclinit, &s_hl_i2c_sclsda_gpio_init, sizeof(hl_gpio_init_t));   
    memcpy(sdainit, &s_hl_i2c_sclsda_gpio_init, sizeof(hl_gpio_init_t));
    
    // then we verify the pin mapping and the altfun ... ok don't do it.
    // but you could put it in here. maybe by calling an external function which depends on the mpu
    
    // then we set the port and pin of scl and sda
    hl_gpio_fill_init(sclinit, &hl_i2c_mapping.gpiomap[HL_i2c_id2index(id)].scl);
    hl_gpio_fill_init(sdainit, &hl_i2c_mapping.gpiomap[HL_i2c_id2index(id)].sda);
    
    // then we set altfun of scl and sda
    hl_gpio_fill_altf(sclaltf, &hl_i2c_mapping.gpiomap[HL_i2c_id2index(id)].scl);
    hl_gpio_fill_altf(sdaaltf, &hl_i2c_mapping.gpiomap[HL_i2c_id2index(id)].sda);    
    
}


static hl_result_t s_hl_i2c_wait_if_busy(I2C_TypeDef* I2Cx)
{
    volatile uint32_t timeout;
    // wait until the bus is not busy  
    timeout = s_hl_i2c_timeout_long;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (timeout--)) return(hl_res_NOK_generic);
    }
    
    return(hl_res_OK);
}
    
static hl_result_t s_hl_i2c_transaction_begin(I2C_TypeDef* I2Cx, uint8_t devaddr, uint8_t dir)
{   // dir is: I2C_Direction_Transmitter or I2C_Direction_Receiver
    volatile uint32_t timeout;
    volatile uint32_t event = 0;
    
    // send START condition
    I2C_GenerateSTART(I2Cx, ENABLE);
    
    // test on ev5
    timeout = s_hl_i2c_timeout_flag;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (timeout--)) return(hl_res_NOK_generic);
    } 

    // send address of device
    I2C_Send7bitAddress(I2Cx, devaddr, dir);
    
    // test on ev6
    timeout = s_hl_i2c_timeout_flag;
    event = (I2C_Direction_Transmitter == dir) ? (I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) : (I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED); 
    while(!I2C_CheckEvent(I2Cx, event))
    {
        if(0 == (timeout--)) return(hl_res_NOK_generic);    
    }
    
    
    return(hl_res_OK);    
}


static hl_result_t s_hl_i2c_transaction_end(I2C_TypeDef* I2Cx, uint8_t dir)
{   // dir is: I2C_Direction_Transmitter or I2C_Direction_Receiver
    // so far it is used only in I2C_Direction_Transmitter because for reception we use a special procedure as specified by datasheet
    volatile uint32_t timeout;
    hl_result_t res = hl_res_OK;
    
    if(I2C_Direction_Receiver == dir)
    {
        return(hl_res_OK);
    }
    
    // send stop condition
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    // wait to make sure that the stop bit has been cleared by hw before writing CR1 again
    // see page 745 of Doc ID 13902 Rev 14 (I2C_CR1, Bit 9 STOP: Stop generation)
    // Note: When the STOP, START or PEC bit is set, the software must not perform any write
    // access to I2C_CR1 before this bit is cleared by hardware. Otherwise there is a risk of
    // setting a second STOP, START or PEC request.
    timeout = s_hl_i2c_timeout_flag;
    while(((I2Cx)->CR1 & I2C_CR1_STOP) == I2C_CR1_STOP)
    {
        if(0 == (timeout--)) { res = hl_res_NOK_generic; break; }
    }  
    
    // perform a read on SR1 and SR2 register to clear eventually pending flags 
    timeout = I2C_GetLastEvent(I2Cx);
    
    return(res);    
}


static hl_result_t s_hl_i2c_write_data_polling(I2C_TypeDef* I2Cx, hl_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
    volatile uint32_t timeout = 0;
    uint8_t adrsize = 0;
    uint8_t *byte = NULL;
    uint8_t *tmpadr = NULL;
    uint8_t adrdata[4] = {0};

    // prepare address of register
    if(1 == regaddr.numofbytes)
    {        
        adrsize = 1;
        adrdata[0] = regaddr.bytes.one;
    }
    else if(2 == regaddr.numofbytes)
    {
        adrsize = 2;
        adrdata[0] = (regaddr.bytes.two >> 8) & 0x00FF;     // msb first
        adrdata[1] = regaddr.bytes.two & 0x00FF;            // then lsb     
    }
    else
    {
        adrsize = 0;
    }
    
    // prepare normal data
    if(NULL == data)
    {
        size = 0;
    }
    
    uint16_t totalsize = size + adrsize;
    tmpadr = adrdata;
    
    // we execute if: (a) we have regadr and data, (b) we have only regadr, (c) we have only data
    while(totalsize)
    {       
        if(0 != adrsize)
        {
            byte = tmpadr;
            tmpadr++;
            adrsize--;
            totalsize --;
        }
        else if(0 != size)
        {
            byte = data;
            data++;
            size--; 
            totalsize --;            
        }      
        
        
        // tx the byte
        I2C_SendData(I2Cx, *byte); 

        // test on ev8 (or ev8_2 = I2C_EVENT_MASTER_BYTE_TRANSMITTED which is slower but more reliable)    
        timeout = s_hl_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            if(0 == (timeout--)) return(hl_res_NOK_generic); 
        }        
        
    }
    
    return(hl_res_OK);
}




#if     defined(HL_I2C_USE_CODE_FROM_I2CRoutines_I2C_Master_BufferRead)

// extracted from I2CRoutines.c w/ some changes

/**
  ******************************************************************************
  * @file OptimizedI2Cexamples/src/I2CRoutines.c
  * @author  MCD Application Team
  * @version  V4.0.0
  * @date  06/18/2010
  * @brief  Contains the I2Cx slave/Master read and write routines.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */
  
  
// only function I2C_Master_BufferRead()

#define Error       hl_res_NOK_generic
#define Success     hl_res_OK

/* I2C START mask */
#define CR1_START_Set           ((uint16_t)0x0100)
#define CR1_START_Reset         ((uint16_t)0xFEFF)

#define CR1_POS_Set           ((uint16_t)0x0800)
#define CR1_POS_Reset         ((uint16_t)0xF7FF)

/* I2C STOP mask */
#define CR1_STOP_Set            ((uint16_t)0x0200)
#define CR1_STOP_Reset          ((uint16_t)0xFDFF)

/* I2C ACK mask */
#define CR1_ACK_Set             ((uint16_t)0x0400)
#define CR1_ACK_Reset           ((uint16_t)0xFBFF)

/* I2C ADD0 mask */
#define OAR1_ADD0_Set           ((uint16_t)0x0001)
#define OAR1_ADD0_Reset         ((uint16_t)0xFFFE)

static __IO uint8_t Address;

//Status I2C_Master_BufferRead(I2C_TypeDef* I2Cx, uint8_t* pBuffer,  uint32_t NumByteToRead, I2C_ProgrammingModel Mode, uint8_t SlaveAddress)
static hl_result_t s_I2CRoutines_I2C_Master_BufferRead(I2C_TypeDef* I2Cx, uint8_t* pBuffer,  uint32_t NumByteToRead, uint8_t SlaveAddress)

{
    __IO uint32_t temp = 0;
    __IO uint32_t Timeout = 0;

//rem /* Enable I2C errors interrupts (used in all modes: Polling, DMA and Interrupts */
//rem  I2Cx->CR2 |= I2C_IT_ERR;

//rem  if (Mode == DMA) /* I2Cx Master Reception using DMA */
//     {
//         /* Configure I2Cx DMA channel */
//         I2C_DMAConfig(I2Cx, pBuffer, NumByteToRead, I2C_DIRECTION_RX);
//         /* Set Last bit to have a NACK on the last received byte */
//         I2Cx->CR2 |= CR2_LAST_Set;
//         /* Enable I2C DMA requests */
//         I2Cx->CR2 |= CR2_DMAEN_Set;
//         Timeout = 0xFFFF;
//         /* Send START condition */
//         I2Cx->CR1 |= CR1_START_Set;
//         /* Wait until SB flag is set: EV5  */
//         while ((I2Cx->SR1&0x0001) != 0x0001)
//         {
//             if (Timeout-- == 0)
//                 return Error;
//         }
//         Timeout = 0xFFFF;
//         /* Send slave address */
//         /* Set the address bit0 for read */
//         SlaveAddress |= OAR1_ADD0_Set;
//         Address = SlaveAddress;
//         /* Send the slave address */
//         I2Cx->DR = Address;
//         /* Wait until ADDR is set: EV6 */
//         while ((I2Cx->SR1&0x0002) != 0x0002)
//         {
//             if (Timeout-- == 0)
//                 return Error;
//         }
//         /* Clear ADDR flag by reading SR2 register */
//         temp = I2Cx->SR2;
//         if (I2Cx == I2C1)
//         {
//             /* Wait until DMA end of transfer */
//             while (!DMA_GetFlagStatus(DMA1_FLAG_TC7));
//             /* Disable DMA Channel */
//             DMA_Cmd(I2C1_DMA_CHANNEL_RX, DISABLE);
//             /* Clear the DMA Transfer Complete flag */
//             DMA_ClearFlag(DMA1_FLAG_TC7);
//
//         }
//
//         else /* I2Cx = I2C2*/
//         {
//             /* Wait until DMA end of transfer */
//             while (!DMA_GetFlagStatus(DMA1_FLAG_TC5));
//             /* Disable DMA Channel */
//             DMA_Cmd(I2C2_DMA_CHANNEL_RX, DISABLE);
//             /* Clear the DMA Transfer Complete flag */
//             DMA_ClearFlag(DMA1_FLAG_TC5);
//         }
//         /* Program the STOP */
//         I2Cx->CR1 |= CR1_STOP_Set;
//         /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
//         while ((I2Cx->CR1&0x200) == 0x200);
//     }
//
//rem  else if (Mode == Polling) /* I2Cx Master Reception using Polling */
    {


        if (NumByteToRead == 1)
        {
            Timeout = 0xFFFF;
            /* Send START condition */
            I2Cx->CR1 |= CR1_START_Set;
            /* Wait until SB flag is set: EV5  */
            while ((I2Cx->SR1&0x0001) != 0x0001)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            /* Send slave address */
            /* Reset the address bit0 for read */
            SlaveAddress |= OAR1_ADD0_Set;
            Address = SlaveAddress;
            /* Send the slave address */
            I2Cx->DR = Address;
            /* Wait until ADDR is set: EV6_3, then program ACK = 0, clear ADDR
            and program the STOP just after ADDR is cleared. The EV6_3 
            software sequence must complete before the current byte end of transfer.*/
            /* Wait until ADDR is set */
            Timeout = 0xFFFF;
            while ((I2Cx->SR1&0x0002) != 0x0002)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            /* Clear ACK bit */
            I2Cx->CR1 &= CR1_ACK_Reset;
            /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
            software sequence must complete before the current byte end of transfer */
            __disable_irq();
            /* Clear ADDR flag */
            temp = I2Cx->SR2;
            /* Program the STOP */
            I2Cx->CR1 |= CR1_STOP_Set;
            /* Re-enable IRQs */
            __enable_irq();
            /* Wait until a data is received in DR register (RXNE = 1) EV7 */
            while ((I2Cx->SR1 & 0x00040) != 0x000040);
            /* Read the data */
            *pBuffer = I2Cx->DR;
            /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
            while ((I2Cx->CR1&0x200) == 0x200);
            /* Enable Acknowledgement to be ready for another reception */
            I2Cx->CR1 |= CR1_ACK_Set;

        }

        else if (NumByteToRead == 2)
        {
            /* Set POS bit */
            I2Cx->CR1 |= CR1_POS_Set;
            Timeout = 0xFFFF;
            /* Send START condition */
            I2Cx->CR1 |= CR1_START_Set;
            /* Wait until SB flag is set: EV5 */
            while ((I2Cx->SR1&0x0001) != 0x0001)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            Timeout = 0xFFFF;
            /* Send slave address */
            /* Set the address bit0 for read */
            SlaveAddress |= OAR1_ADD0_Set;
            Address = SlaveAddress;
            /* Send the slave address */
            I2Cx->DR = Address;
            /* Wait until ADDR is set: EV6 */
            while ((I2Cx->SR1&0x0002) != 0x0002)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            /* EV6_1: The acknowledge disable should be done just after EV6,
            that is after ADDR is cleared, so disable all active IRQs around ADDR clearing and 
            ACK clearing */
            __disable_irq();
            /* Clear ADDR by reading SR2 register  */
            temp = I2Cx->SR2;
            /* Clear ACK */
            I2Cx->CR1 &= CR1_ACK_Reset;
            /*Re-enable IRQs */
            __enable_irq();
            /* Wait until BTF is set */
            while ((I2Cx->SR1 & 0x00004) != 0x000004);
            /* Disable IRQs around STOP programming and data reading because of the limitation ?*/
            __disable_irq();
            /* Program the STOP */
            I2C_GenerateSTOP(I2Cx, ENABLE);
            /* Read first data */
            *pBuffer = I2Cx->DR;
            /* Re-enable IRQs */
            __enable_irq();
            /**/
            pBuffer++;
            /* Read second data */
            *pBuffer = I2Cx->DR;
            /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
            while ((I2Cx->CR1&0x200) == 0x200);
            /* Enable Acknowledgement to be ready for another reception */
            I2Cx->CR1  |= CR1_ACK_Set;
            /* Clear POS bit */
            I2Cx->CR1  &= CR1_POS_Reset;

        }

        else

        {

            Timeout = 0xFFFF;
            /* Send START condition */
            I2Cx->CR1 |= CR1_START_Set;
            /* Wait until SB flag is set: EV5 */
            while ((I2Cx->SR1&0x0001) != 0x0001)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            Timeout = 0xFFFF;
            /* Send slave address */
            /* Reset the address bit0 for write */
            SlaveAddress |= OAR1_ADD0_Set;;
            Address = SlaveAddress;
            /* Send the slave address */
            I2Cx->DR = Address;
            /* Wait until ADDR is set: EV6 */
            while ((I2Cx->SR1&0x0002) != 0x0002)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            /* Clear ADDR by reading SR2 status register */
            temp = I2Cx->SR2;
            /* While there is data to be read */
            while (NumByteToRead)
            {
                /* Receive bytes from first byte until byte N-3 */
                if (NumByteToRead != 3)
                {
                    /* Poll on BTF to receive data because in polling mode we can not guarantee the
                    EV7 software sequence is managed before the current byte transfer completes */
                    while ((I2Cx->SR1 & 0x00004) != 0x000004);
                    /* Read data */
                    *pBuffer = I2Cx->DR;
                    /* */
                    pBuffer++;
                    /* Decrement the read bytes counter */
                    NumByteToRead--;
                }

                /* it remains to read three data: data N-2, data N-1, Data N */
                if (NumByteToRead == 3)
                {

                    /* Wait until BTF is set: Data N-2 in DR and data N -1 in shift register */
                    while ((I2Cx->SR1 & 0x00004) != 0x000004);
                    /* Clear ACK */
                    I2Cx->CR1 &= CR1_ACK_Reset;

                    /* Disable IRQs around data reading and STOP programming because of the
                    limitation ? */
                    __disable_irq();
                    /* Read Data N-2 */
                    *pBuffer = I2Cx->DR;
                    /* Increment */
                    pBuffer++;
                    /* Program the STOP */
                    I2Cx->CR1 |= CR1_STOP_Set;
                    /* Read DataN-1 */
                    *pBuffer = I2Cx->DR;
                    /* Re-enable IRQs */
                    __enable_irq();
                    /* Increment */
                    pBuffer++;
                    /* Wait until RXNE is set (DR contains the last data) */
                    while ((I2Cx->SR1 & 0x00040) != 0x000040);
                    /* Read DataN */
                    *pBuffer = I2Cx->DR;
                    /* Reset the number of bytes to be read by master */
                    NumByteToRead = 0;

                }
            }
            /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
            while ((I2Cx->CR1&0x200) == 0x200);
            /* Enable Acknowledgement to be ready for another reception */
            I2Cx->CR1 |= CR1_ACK_Set;

        }

    }

//rem  else /* I2Cx Master Reception using Interrupts with highest priority in an application */
//     {
//         /* Enable EVT IT*/
//         I2Cx->CR2 |= I2C_IT_EVT;
//         /* Enable BUF IT */
//         I2Cx->CR2 |= I2C_IT_BUF;
//         /* Set the I2C direction to reception */
//         I2CDirection = I2C_DIRECTION_RX;
//         SlaveAddress |= OAR1_ADD0_Set;
//         Address = SlaveAddress;
//         if (I2Cx == I2C1)    NumbOfBytes1 = NumByteToRead;
//         else NumbOfBytes2 = NumByteToRead;
//         /* Send START condition */
//         I2Cx->CR1 |= CR1_START_Set;
//         /* Wait until the START condition is generated on the bus: START bit is cleared by hardware */
//         while ((I2Cx->CR1&0x100) == 0x100);
//         /* Wait until BUSY flag is reset (until a STOP is generated) */
//         while ((I2Cx->SR2 &0x0002) == 0x0002);
//         /* Enable Acknowledgement to be ready for another reception */
//         I2Cx->CR1 |= CR1_ACK_Set;
//rem  }

    return Success;
}

#endif//defined(HL_I2C_USE_CODE_FROM_I2CRoutines_I2C_Master_BufferRead)


#endif//defined(HL_USE_UTIL_I2C)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



