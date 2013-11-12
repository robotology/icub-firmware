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


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

const hl_i2c_cfg_t hl_i2c_cfg_default = 
{ 
    .mode           = hl_i2c_mode_master, 
    .behaviour      = hl_i2c_behaviour_polling,
    .speed          = hl_i2c_speed_400kbps, 
    .ownaddress     = 0
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

static void s_hl_i2c_hw_init(hl_i2c_t id);
static void s_hl_i2c_hw_gpio_init(hl_i2c_t id);
static void s_hl_i2c_hw_enable(hl_i2c_t id, const hl_i2c_cfg_t* cfg);

static void s_hl_i2c_fill_gpio_init_altf(hl_i2c_t id, hl_gpio_init_t* sclinit, hl_gpio_altf_t* sclaltf, hl_gpio_init_t* sdainit, hl_gpio_altf_t* sdaaltf);

static hl_result_t s_i2c_read(hl_i2c_t id, hl_i2c_devaddr_t devaddr, hl_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);
static void s_hl_i2c_read_bytes_with_polling(I2C_TypeDef* I2Cx, uint8_t* data, uint16_t size);


static hl_result_t s_hl_i2c_write(hl_i2c_t id, hl_i2c_devaddr_t devaddr, hl_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);
static void s_hl_i2c_write_bytes_with_polling(I2C_TypeDef* I2Cx, uint8_t* data, uint16_t size);

static hl_result_t s_hl_i2c_ping(hl_i2c_t id, hl_i2c_devaddr_t devaddr);

static hl_result_t s_hl_i2c_standby(hl_i2c_t id, hl_i2c_devaddr_t devaddr);

static hl_result_t s_hl_i2c_timeoutexpired(char *fname);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static const uint32_t s_hl_i2c_timeout_flag = 0x00010000;
static const uint32_t s_hl_i2c_timeout_long = 0x01000000;
static const uint32_t s_hl_i2c_ackaddress_maxtrials = 1000;

static const I2C_InitTypeDef   s_hl_i2c_stm32_cfg =
{
    .I2C_ClockSpeed             = 400000,                       // PARAMETER
    .I2C_Mode                   = I2C_Mode_I2C,
    .I2C_DutyCycle              = I2C_DutyCycle_2,
    .I2C_OwnAddress1            = 0,                            // PARAMETER
    .I2C_Ack                    = I2C_Ack_Enable,
    .I2C_AcknowledgedAddress    = I2C_AcknowledgedAddress_7bit
};

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
    
    if(hl_res_OK == res)
    {
        s_hl_i2c_standby(id, devaddr);
    }
    else
    {
        
    }
       
    return(res);            
}


extern hl_result_t hl_i2c_standby(hl_i2c_t id, hl_i2c_devaddr_t devaddr)      
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
           
    hl_result_t res = s_hl_i2c_standby(id, devaddr);
    
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
    s_hl_i2c_hw_init(id);
    s_hl_i2c_hw_enable(id, cfg);
        
    
    return(hl_res_OK);
}


static void s_hl_i2c_hw_init(hl_i2c_t id)
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


static void s_hl_i2c_hw_enable(hl_i2c_t id, const hl_i2c_cfg_t* cfg)
{
#if     defined(HL_USE_MPU_ARCH_STM32F1) || defined(HL_USE_MPU_ARCH_STM32F4)
    
    I2C_TypeDef* I2Cx = HL_i2c_id2stmI2C(id);
    
    I2C_InitTypeDef i2c_cfg;
    
    memcpy(&i2c_cfg, &s_hl_i2c_stm32_cfg, sizeof(I2C_InitTypeDef));
    // apply the clockspeed 
    i2c_cfg.I2C_ClockSpeed  = (uint32_t)cfg->speed * 100 * 1000;
    i2c_cfg.I2C_OwnAddress1 = cfg->ownaddress;
    // i2c peripheral enable
    I2C_Cmd(I2Cx, ENABLE);
    // apply configuration
    I2C_Init(I2Cx, &i2c_cfg);
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERR --> choose a HL_USE_MPU_ARCH_*
#endif 
}

    

static hl_result_t s_i2c_read(hl_i2c_t id, hl_i2c_devaddr_t devaddr, hl_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
    volatile uint32_t timeout = 0;
    
    I2C_TypeDef* I2Cx = HL_i2c_id2stmI2C(id);
      
    // wait until the bus is not busy 
    timeout = s_hl_i2c_timeout_long;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_i2c_read:flag_busy");
    }
    
    // send START condition
    I2C_GenerateSTART(I2Cx, ENABLE);
 
    // test on ev5
    timeout = s_hl_i2c_timeout_flag;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_i2c_read:ev5");
    }
    
    // send address of device
    I2C_Send7bitAddress(I2Cx, devaddr, I2C_Direction_Transmitter);
    
    // test on ev6
    timeout = s_hl_i2c_timeout_flag;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_i2c_read:ev6");
    }
    
    // end of transaction_begin
    
           
    // send address of register to read inside the device
    if(1 == regaddr.numofbytes)
    {
        uint8_t reg1byteadr = regaddr.bytes.one;
        I2C_SendData(I2Cx, reg1byteadr);                   
        timeout = s_hl_i2c_timeout_flag;
#if 1  
        // test on ev8_2 and clear it (as ev8 but with BTF: I2C_EVENT_MASTER_BYTE_TRANSMITTED) is slower but more reliable       
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
#else
        // test on ev8 and clear it (ev8 is I2C_EVENT_MASTER_BYTE_TRANSMITTING ...)
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
#endif        
        {
            if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_i2c_read:ev8");
        }  
    
    }
    else if(2 == regaddr.numofbytes)
    {   
        uint8_t reg1byteadr = (regaddr.bytes.two >> 8 ) & 0x00FF;               // msb first
        I2C_SendData(I2Cx, reg1byteadr);           
        timeout = s_hl_i2c_timeout_flag;
#if 1  
        // test on ev8_2 and clear it (as ev8 but with BTF: I2C_EVENT_MASTER_BYTE_TRANSMITTED) is slower but more reliable       
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
#else
        // test on ev8 and clear it (ev8 is I2C_EVENT_MASTER_BYTE_TRANSMITTING ...)
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
#endif 
        {
            if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_i2c_read:evt8");
        }  
        
        reg1byteadr = regaddr.bytes.two & 0x00FF;                               // then lsb
        I2C_SendData(I2Cx, reg1byteadr);           
        timeout = s_hl_i2c_timeout_flag;
#if 1  
        // test on ev8_2 and clear it (as ev8 but with BTF: I2C_EVENT_MASTER_BYTE_TRANSMITTED) is slower but more reliable       
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
#else
        // test on ev8 and clear it (ev8 is I2C_EVENT_MASTER_BYTE_TRANSMITTING ...)
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
#endif  
        {
            if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_i2c_read:ev8");
        }               
    }
    
    // now read data: XXX
    
    // send START condition a second time 
    I2C_GenerateSTART(I2Cx, ENABLE);
 
    // test on ev5
    timeout = s_hl_i2c_timeout_flag;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_i2c_read:ev5");
    }

   
    // send address for read
    I2C_Send7bitAddress(I2Cx, devaddr, I2C_Direction_Receiver);
     
    // test on ev6
    timeout = s_hl_i2c_timeout_flag;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_i2c_read:ev6");
    }
  
    
//    if(1 == size)
//    {
//        usedma = 0;
//    }
    
//    if(0 == usedma)
//    {
        s_hl_i2c_read_bytes_with_polling(I2Cx, data, size);
//    }
//    else
//    {
//        s_stm32i2c_read_withdma(I2Cx, data, size);
//    }
    


    return(hl_res_OK);  
}

// see page 732
// considera di mettere dentro questa funzione quanto sta dopo // now read data: XXX
static void s_hl_i2c_read_bytes_with_polling(I2C_TypeDef* I2Cx, uint8_t* data, uint16_t size)
{
    volatile uint32_t timeout = 0;
    
    while(size > 0)
    {    
        // wait for the byte to be received
        timeout = s_hl_i2c_timeout_flag;
        //while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET)
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))
        {                    
            if(0 == (timeout--)) {s_hl_i2c_timeoutexpired("s_hl_i2c_read_bytes_with_polling:evt_MASTER_BYTE_RECEIVED"); return;}
        }
        
        // read received byte
        *data = I2C_ReceiveData(I2Cx);
        
        // increment received data and decrement the size (bytes to be received yet)
        data++;
        size--;          
    }

 
    // disable acknowledgement
    I2C_AcknowledgeConfig(I2Cx, DISABLE);   

    // clear addr by reading sr1 and then sr2
    timeout = I2Cx->SR1;
    timeout = I2Cx->SR2; 
    timeout = timeout;
    
    // send stop condition
    I2C_GenerateSTOP(I2Cx, ENABLE);

    
    // wait to make sure that the stop bit has been cleared by hw before writing CR1 again
    timeout = s_hl_i2c_timeout_flag;
    while(((I2Cx)->CR1 & I2C_CR1_STOP) == I2C_CR1_STOP)
    {
        if(0 == (timeout--)) {s_hl_i2c_timeoutexpired("s_hl_i2c_read_bytes_with_polling:stopbitcleared"); return;}
    }  

    // re-enable acknowledgement to be ready for another reception 
    I2C_AcknowledgeConfig(I2Cx, ENABLE);    
}

//static void s_hl_i2c_read_bytes_with_dma(I2C_TypeDef* I2Cx, uint8_t* data, uint16_t size)
//{
//    volatile uint32_t timeout = 0;
//    
//
//    // tbd
//}




static hl_result_t s_hl_i2c_write(hl_i2c_t id, hl_i2c_devaddr_t devaddr, hl_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
    volatile uint32_t timeout = 0;
    
    I2C_TypeDef* I2Cx = HL_i2c_id2stmI2C(id);
        
    // wait until the bus is not busy  
    timeout = s_hl_i2c_timeout_long;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_hl_i2c_write:flag_busy");
    }

    // send START condition
    I2C_GenerateSTART(I2Cx, ENABLE);
    
    // test on ev5
    timeout = s_hl_i2c_timeout_flag;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_hl_i2c_write:ev5");
    } 

    // send address of device
    I2C_Send7bitAddress(I2Cx, devaddr, I2C_Direction_Transmitter);
    
    // test on ev6
    timeout = s_hl_i2c_timeout_flag;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_hl_i2c_write:ev6");
    } 
    
    // end of transaction_begin
    

    // send address of register inside the device
    if(1 == regaddr.numofbytes)
    {
        uint8_t reg1byteadr = regaddr.bytes.one;
        I2C_SendData(I2Cx, reg1byteadr);    
        timeout = s_hl_i2c_timeout_flag;
#if 1  
        // test on ev8_2 and clear it (as ev8 but with BTF: I2C_EVENT_MASTER_BYTE_TRANSMITTED) is slower but more reliable       
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
#else
        // test on ev8 and clear it (ev8 is I2C_EVENT_MASTER_BYTE_TRANSMITTING ...)
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
#endif
        {
            if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_hl_i2c_write:ev8");
        } 
        
    }
    else if(2 == regaddr.numofbytes)
    {   
        uint8_t reg1byteadr = (regaddr.bytes.two >> 8) & 0x00FF;           // msb first
        I2C_SendData(I2Cx, reg1byteadr);    
        timeout = s_hl_i2c_timeout_flag;
#if 1  
        // test on ev8_2 and clear it (as ev8 but with BTF: I2C_EVENT_MASTER_BYTE_TRANSMITTED) is slower but more reliable       
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
#else
        // test on ev8 and clear it (ev8 is I2C_EVENT_MASTER_BYTE_TRANSMITTING ...)
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
#endif
        {
            if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_hl_i2c_write:ev8");
        }  

        reg1byteadr = regaddr.bytes.two & 0x00FF;           // then lsb
        I2C_SendData(I2Cx, reg1byteadr);    
        timeout = s_hl_i2c_timeout_flag;
#if 1  
        // test on ev8_2 and clear it (as ev8 but with BTF: I2C_EVENT_MASTER_BYTE_TRANSMITTED) is slower but more reliable       
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
#else
        // test on ev8 and clear it (ev8 is I2C_EVENT_MASTER_BYTE_TRANSMITTING ...)
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
#endif 
        {
            if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_hl_i2c_write:ev8");
        }           
    }
    
    // now we decide if it is dma or direct send
    
    
//    if(1 == size)
//    {
//        usedma = 0;
//    }
//    
//    if(0 == usedma)
//    {
        s_hl_i2c_write_bytes_with_polling(I2Cx, data, size);
//    }
//    else
//    {
//        s_stm32i2c_write_withdma(I2Cx, data, size);
//    }    
    
    
    
/////////////////////////////////////////////////////////////

    //s_stm32i2c_i2c_waitdevicestandbystate(id, devaddr);
    
    return(hl_res_OK);

}




static void s_hl_i2c_write_bytes_with_polling(I2C_TypeDef* I2Cx, uint8_t* data, uint16_t size)
{
    volatile uint32_t timeout = 0;
    
    while(size) 
    {
 
        // send the byte to be written 
        I2C_SendData(I2Cx, *data); 
        
        data++;
        size--;
        
        // test on ev8 (or ev8_2 = I2C_EVENT_MASTER_BYTE_TRANSMITTED which is slower but more reliable)    
        timeout = s_hl_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            if(0 == (timeout--)) {s_hl_i2c_timeoutexpired("s_hl_i2c_write_bytes_with_polling:ev8_2"); return;}
        }
    
    }
    
    // send stop condition
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    // wait to make sure that the stop bit has been cleared by hw before writing CR1 again
    timeout = s_hl_i2c_timeout_flag;
    while(((I2Cx)->CR1 & I2C_CR1_STOP) == I2C_CR1_STOP)
    {
        if(0 == (timeout--)) {s_hl_i2c_timeoutexpired("s_hl_i2c_write_bytes_with_polling:stopbit_cleared"); return;}
    }  
    
     // perform a read on SR1 and SR2 register to clear eventually pending flags 
    timeout = I2Cx->SR1;
    timeout = I2Cx->SR2; 
    timeout = timeout;
}


static hl_result_t s_hl_i2c_ping(hl_i2c_t id, hl_i2c_devaddr_t devaddr)      
{
    hl_result_t res = hl_res_OK;

    volatile uint32_t timeout = 0;
    
    I2C_TypeDef* I2Cx = HL_i2c_id2stmI2C(id);
 
    // wait until the bus is not busy 
    timeout = s_hl_i2c_timeout_long;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (timeout--))    { res = hl_res_NOK_generic; break; }
    }
    
    if(hl_res_NOK_generic != res)
    {
        // send START condition
        I2C_GenerateSTART(I2Cx, ENABLE);

        // test on ev5
        timeout = s_hl_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
        {
            if(0 == (timeout--))    { res = hl_res_NOK_generic; break; }
        }
    }
    
    if(hl_res_NOK_generic != res)
    {
        // send address of device
        I2C_Send7bitAddress(I2Cx, devaddr, I2C_Direction_Transmitter);
        
        // test on ev6
        timeout = s_hl_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        {
            if(0 == (timeout--))    { res = hl_res_NOK_generic; break; }
        }
    }
    

    // send stop condition
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    
    // wait to make sure that the stop bit has been cleared by hw before writing CR1 again
    timeout = s_hl_i2c_timeout_flag;
    while(((I2Cx)->CR1 & I2C_CR1_STOP) == I2C_CR1_STOP)
    {
        if(0 == (timeout--)) { res = hl_res_NOK_generic; break; }
    }     
    
    // perform a read on SR1 and SR2 register to clear eventually pending flags 
    timeout = I2Cx->SR1;
    timeout = I2Cx->SR2; 
    timeout = timeout;     
    
    return(res);
}


static hl_result_t s_hl_i2c_standby(hl_i2c_t id, hl_i2c_devaddr_t devaddr)      
{
    volatile uint16_t tmpSR1 = 0;
    volatile uint32_t trials = 0;
    volatile uint32_t timeout = 0;
    
    I2C_TypeDef* I2Cx = HL_i2c_id2stmI2C(id);
 
    // wait until the bus is not busy 
    timeout = s_hl_i2c_timeout_long;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_hl_i2c_standby:flag_busy");
    }

    //  keep looping till the slave acknowledge his address or maximum number of trials is reached
    while(1)
    {
        // send START condition
        I2C_GenerateSTART(I2Cx, ENABLE);

        // test on ev5
        timeout = s_hl_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
        {
            if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_hl_i2c_standby:ev5");
        }    

        // send address of device
        I2C_Send7bitAddress(I2Cx, devaddr, I2C_Direction_Transmitter);

        // Wait for ADDR flag to be set (Slave acknowledged his address) 
        timeout = s_hl_i2c_timeout_long;
        do
        {     
            // Get the current value of the SR1 register 
            tmpSR1 = (I2Cx)->SR1;

            // Update the timeout value and exit if it reach 0
            if(0 == (timeout--)) return s_hl_i2c_timeoutexpired("s_hl_i2c_standby:addrflag_set");
        }
        // Keep looping till the Address is acknowledged or the AF (ack failure) flag is 
        // set (address not acknowledged at time) 
        while((tmpSR1 & (I2C_SR1_ADDR | I2C_SR1_AF)) == 0);

        // Check if the ADDR flag has been set
        if (tmpSR1 & I2C_SR1_ADDR)
        {
            // Clear ADDR Flag by reading SR1 then SR2 registers (SR1 have already been read) 
            (void)(I2Cx)->SR2;

            // STOP condition   
            I2C_GenerateSTOP(I2Cx, ENABLE);

            // Exit the function
            return hl_res_OK;
        }
        else
        {
            // Clear AF flag
            I2C_ClearFlag(I2Cx, I2C_FLAG_AF);                  
        }

        // Check if the maximum allowed number of trials has been reached
        if (s_hl_i2c_ackaddress_maxtrials == trials++)
        {
            // If the maximum number of trials has been reached, exit the function 
            return s_hl_i2c_timeoutexpired("s_hl_i2c_standby:maxtrials");
        }
    }
}


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


#endif//defined(HL_USE_UTIL_I2C)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



