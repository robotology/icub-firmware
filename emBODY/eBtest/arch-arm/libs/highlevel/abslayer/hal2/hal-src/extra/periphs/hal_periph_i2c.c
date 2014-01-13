/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
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


/* @file       hal_periph_i2c.c
	@brief      This file implements internal implementation of the hal i2c module.
	@author     valentina.gaggero@iit.it
    @date       28/02/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_PERIPH_I2C

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_brdcfg.h"
#include "hal_base_hid.h"
#include "hal_periph_gpio_hid.h"

#include "hal_heap.h"


#include "hal_utility_bits.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_i2c.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_periph_i2c_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_i2c_id2index(p)           ((uint8_t)(p))

#define HAL_i2c_id2stmI2C(p)          (s_hal_i2c_stmI2Cmap[HAL_i2c_id2index(p)])


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

const hal_i2c_cfg_t hal_i2c_cfg_default = 
{ 
    .mode           = hal_i2c_mode_master, 
    .speed          = hal_i2c_speed_400kbps, //hal_i2c_speed_400kbps, 
    .ownaddress     = 0
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    hal_bool_t          locked;
    hal_i2c_devaddr_t   devaddr;   
} hal_i2c_internal_item_t;

typedef struct
{
    uint8_t                     initted;
    hal_i2c_internal_item_t*    items[hal_i2cs_number];   
} hal_i2c_theinternals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hal_i2c_scheduling_suspend(void);
static void s_hal_i2c_scheduling_restart(void);

static hal_boolval_t s_hal_i2c_supported_is(hal_i2c_t id);
static void s_hal_i2c_initted_set(hal_i2c_t id);
static hal_boolval_t s_hal_i2c_initted_is(hal_i2c_t id);

static void s_hal_i2c_status_set(hal_i2c_t id, hal_bool_t locked, hal_i2c_devaddr_t devaddr);

static hal_bool_t s_hal_i2c_is_status_locked(hal_i2c_t id);

static hal_i2c_devaddr_t s_hal_i2c_status_devaddr_get(hal_i2c_t id);

static hal_result_t s_hal_i2c_init(hal_i2c_t id, const hal_i2c_cfg_t *cfg);

static void s_hal_i2c_hw_init(hal_i2c_t id);
static void s_hal_i2c_hw_gpio_init(hal_i2c_t id);
static void s_hal_i2c_hw_enable(hal_i2c_t id, const hal_i2c_cfg_t* cfg);


static hal_result_t s_hal_i2c_transaction_begin(hal_i2c_t id, hal_i2c_devaddr_t devaddr);

static hal_result_t s_hal_i2c_transaction_tx(hal_i2c_t id,  hal_i2c_devaddr_t devaddr, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendstop);
static hal_result_t s_hal_i2c_transaction_rx(hal_i2c_t id,  hal_i2c_devaddr_t devaddr, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendnack, hal_bool_t sendstop);

static hal_result_t s_i2c_read(hal_i2c_t id, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);
static void s_hal_i2c_read_bytes(I2C_TypeDef* I2Cx, uint8_t* data, uint16_t size);


static hal_result_t s_hal_i2c_write(hal_i2c_t id, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);
static void s_hal_i2c_write_bytes(I2C_TypeDef* I2Cx, uint8_t* data, uint16_t size);

static hal_result_t s_hal_i2c_ping(hal_i2c_t id, hal_i2c_devaddr_t devaddr);

static hal_result_t s_hal_i2c_standby(hal_i2c_t id, hal_i2c_devaddr_t devaddr);

static hal_result_t s_hal_i2c_timeoutexpired(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static const uint32_t s_hal_i2c_timeout_flag = 0x00010000;
static const uint32_t s_hal_i2c_timeout_long = 0x01000000;
static const uint32_t s_hal_i2c_ackaddress_maxtrials = 100000;

static const I2C_InitTypeDef   s_hal_i2c_stm32_cfg =
{
    .I2C_Mode                   = I2C_Mode_I2C,
    .I2C_DutyCycle              = I2C_DutyCycle_2,
    .I2C_OwnAddress1            = 0,                            // PARAMETER
    .I2C_Ack                    = I2C_Ack_Enable,
    .I2C_AcknowledgedAddress    = I2C_AcknowledgedAddress_7bit,
    .I2C_ClockSpeed             = 400000                        // PARAMETER
};

#if     defined(HAL_USE_CPU_FAM_STM32F1)
static I2C_TypeDef* const s_hal_i2c_stmI2Cmap[] = { I2C1, I2C2, NULL };
static const uint32_t s_hal_i2c_hw_rcc[] = { RCC_APB1Periph_I2C1, RCC_APB1Periph_I2C2, 0 };
#elif   defined(HAL_USE_CPU_FAM_STM32F4)
static I2C_TypeDef* const s_hal_i2c_stmI2Cmap[] = { I2C1, I2C2, I2C3 };
static const uint32_t s_hal_i2c_hw_rcc[] = { RCC_APB1Periph_I2C1, RCC_APB1Periph_I2C2, RCC_APB1Periph_I2C3 };
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_i2c_theinternals_t s_hal_i2c_theinternals =
{
    .initted            = 0,
    .items              = { NULL }   
};


    
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_i2c_init(hal_i2c_t id, const hal_i2c_cfg_t *cfg)
{
    return(s_hal_i2c_init(id, cfg));
}


extern hal_bool_t hal_i2c_initted_is(hal_i2c_t id)
{
    return(s_hal_i2c_initted_is(id));
}

extern hal_result_t hal_i2c_transaction_begin(hal_i2c_t id, hal_i2c_devaddr_t devaddr)
{

    if(hal_false == hal_i2c_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    if(0 == devaddr)
    {
        return(hal_res_NOK_generic);
    }
    
    devaddr &= ~0x01;

    s_hal_i2c_scheduling_suspend();    
    if(hal_true == s_hal_i2c_is_status_locked(id))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_i2c_status_set(id, hal_true, devaddr);
    s_hal_i2c_scheduling_restart();
    
    return(s_hal_i2c_transaction_begin(id, devaddr));

}


extern hal_result_t hal_i2c_transaction_end(hal_i2c_t id, hal_i2c_devaddr_t devaddr)
{
    if(hal_false == hal_i2c_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    devaddr &= ~0x01;
    
    s_hal_i2c_scheduling_suspend();
    if((hal_false == s_hal_i2c_is_status_locked(id)) || (devaddr != s_hal_i2c_status_devaddr_get(id)))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    
    s_hal_i2c_status_set(id, hal_false, 0);
    s_hal_i2c_scheduling_restart();
    
    return(hal_res_OK);    

}


extern hal_result_t hal_i2c_transaction_transmit(hal_i2c_t id, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendstop)
{
 
    if(hal_false == hal_i2c_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    s_hal_i2c_scheduling_suspend();
    if(hal_false == s_hal_i2c_is_status_locked(id))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
       
    if(NULL == data)
    {
        return(hal_res_NOK_nullpointer);
    }
    
    if(0 == size)
    {
        return(hal_res_OK);
    }  


    return(s_hal_i2c_transaction_tx(id, s_hal_i2c_status_devaddr_get(id), data, size, sendstart, sendstop));
}

   

    
extern hal_result_t hal_i2c_transaction_receive(hal_i2c_t id, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendnack, hal_bool_t sendstop)
{
    if(hal_false == hal_i2c_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    s_hal_i2c_scheduling_suspend();
    if(hal_false == s_hal_i2c_is_status_locked(id))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
       
    if(NULL == data)
    {
        return(hal_res_NOK_nullpointer);
    }
    
    if(0 == size)
    {
        return(hal_res_OK);
    }  


    return(s_hal_i2c_transaction_rx(id, s_hal_i2c_status_devaddr_get(id), data, size, sendstart, sendnack, sendstop));
}




extern hal_result_t hal_i2c_read(hal_i2c_t id, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
    hal_result_t res;

    if(hal_false == hal_i2c_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    if(0 == devaddr)
    {
        return(hal_res_NOK_generic);
    }
    
    devaddr &= ~0x01;

    if((0 == regaddr.numofbytes) || (regaddr.numofbytes > 3))
    {
        return(hal_res_NOK_unsupported);
    }  
    
    if(NULL == data)
    {
        return(hal_res_NOK_nullpointer);
    }
    
    if(0 == size)
    {
        return(hal_res_OK);
    }
    
    s_hal_i2c_scheduling_suspend();    
    if(hal_true == s_hal_i2c_is_status_locked(id))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_i2c_status_set(id, hal_true, devaddr);
    s_hal_i2c_scheduling_restart();
    

    res = s_i2c_read(id, devaddr, regaddr, data, size);
    
    
    s_hal_i2c_scheduling_suspend();
    s_hal_i2c_status_set(id, hal_false, 0);
    s_hal_i2c_scheduling_restart();    
    
    return(res);
}


extern hal_result_t hal_i2c_write(hal_i2c_t id, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
    hal_result_t res;
    
    if(hal_false == hal_i2c_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    if(0 == devaddr)
    {
        return(hal_res_NOK_generic);
    }   

    devaddr &= ~0x01;    
    
    if((0 == regaddr.numofbytes) || (regaddr.numofbytes > 3))
    {
        return(hal_res_NOK_unsupported);
    }  
    
    if(NULL == data)
    {
        return(hal_res_NOK_nullpointer);
    }
    
    if(0 == size)
    {
        return(hal_res_OK);
    }
    
    
    s_hal_i2c_scheduling_suspend();    
    if(hal_true == s_hal_i2c_is_status_locked(id))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_i2c_status_set(id, hal_true, devaddr);
    s_hal_i2c_scheduling_restart();
    

    res = s_hal_i2c_write(id, devaddr, regaddr, data, size);
    
    
    s_hal_i2c_scheduling_suspend();
    s_hal_i2c_status_set(id, hal_false, 0);
    s_hal_i2c_scheduling_restart();    
    
    return(res);    
}

extern hal_result_t hal_i2c_ping(hal_i2c_t id, hal_i2c_devaddr_t devaddr)
{
   hal_result_t res;
    
    if(hal_false == hal_i2c_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    if(0 == devaddr)
    {
        return(hal_res_NOK_generic);
    }
    
    devaddr &= ~0x01;
    
    
    s_hal_i2c_scheduling_suspend();    
    if(hal_true == s_hal_i2c_is_status_locked(id))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_i2c_status_set(id, hal_true, devaddr);
    s_hal_i2c_scheduling_restart();
    

    res = s_hal_i2c_ping(id, devaddr);
    
    if(hal_res_OK == res)
    {
        s_hal_i2c_standby(id, devaddr);
    }
    else
    {
        
    }
    
    
    s_hal_i2c_scheduling_suspend();
    s_hal_i2c_status_set(id, hal_false, 0);
    s_hal_i2c_scheduling_restart();    
    
    return(res);            
}


extern hal_result_t hal_i2c_standby(hal_i2c_t id, hal_i2c_devaddr_t devaddr)      
{
    hal_result_t res;
    
    if(hal_false == hal_i2c_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    if(0 == devaddr)
    {
        return(hal_res_NOK_generic);
    }
    
    devaddr &= ~0x01;
    
    
    s_hal_i2c_scheduling_suspend();    
    if(hal_true == s_hal_i2c_is_status_locked(id))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_i2c_status_set(id, hal_true, devaddr);
    s_hal_i2c_scheduling_restart();
    

    res = s_hal_i2c_standby(id, devaddr);
    
    
    s_hal_i2c_scheduling_suspend();
    s_hal_i2c_status_set(id, hal_false, 0);
    s_hal_i2c_scheduling_restart();    
    
    return(res);    
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------

extern hal_result_t hal_i2c_hid_static_memory_init(void)
{
    memset(&s_hal_i2c_theinternals, 0, sizeof(s_hal_i2c_theinternals));
    return(hal_res_OK);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_i2c_supported_is(hal_i2c_t id)
{
    return(hal_utility_bits_byte_bitcheck(hal_brdcfg_i2c__theconfig.supported_mask, HAL_i2c_id2index(id)) );
}

static void s_hal_i2c_initted_set(hal_i2c_t id)
{
    hal_utility_bits_byte_bitset(&s_hal_i2c_theinternals.initted, HAL_i2c_id2index(id));
}

static hal_boolval_t s_hal_i2c_initted_is(hal_i2c_t id)
{
    return(hal_utility_bits_byte_bitcheck(s_hal_i2c_theinternals.initted, HAL_i2c_id2index(id)));
}

static void s_hal_i2c_status_set(hal_i2c_t id, hal_bool_t locked, hal_i2c_devaddr_t devaddr)
{
//    s_hal_i2c_scheduling_suspend();
    s_hal_i2c_theinternals.items[HAL_i2c_id2index(id)]->locked   = locked;
    s_hal_i2c_theinternals.items[HAL_i2c_id2index(id)]->devaddr  = devaddr;
//    s_hal_i2c_scheduling_restart();
}

static hal_bool_t s_hal_i2c_is_status_locked(hal_i2c_t id)
{
    return(s_hal_i2c_theinternals.items[HAL_i2c_id2index(id)]->locked);
}

static hal_i2c_devaddr_t s_hal_i2c_status_devaddr_get(hal_i2c_t id)
{
    return(s_hal_i2c_theinternals.items[HAL_i2c_id2index(id)]->devaddr);
}


static hal_result_t s_hal_i2c_init(hal_i2c_t id, const hal_i2c_cfg_t *cfg)
{
    //hal_i2c_internal_item_t *intitem = s_hal_i2c_internals[HAL_i2c_id2index(id)];
    hal_i2c_internal_item_t* intitem = s_hal_i2c_theinternals.items[HAL_i2c_id2index(id)];
    
    if(NULL == cfg)
    {
        cfg = &hal_i2c_cfg_default;
    }
    
    if(hal_i2c_mode_slave == cfg->mode)
    {
        return(hal_res_NOK_unsupported);
    }
    
    if(hal_true != s_hal_i2c_supported_is(id))
    {
        return(hal_res_NOK_unsupported);
    }

    if(hal_true == hal_i2c_initted_is(id))
    {
        return(hal_res_OK);
    }  

   // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        intitem = s_hal_i2c_theinternals.items[HAL_i2c_id2index(id)] = hal_heap_new(sizeof(hal_i2c_internal_item_t));
        // minimal initialisation of the internal item
        s_hal_i2c_status_set(id, hal_false, 0);      
    }           
    

    // acemor: very important info.
    // init the scl and sda gpio before calling hw_init. 
    // because if the i2c is already initted and it detects sda or scl low it sets
    // register I2C_SR2.BUSY to 1, which makes things hang up.
    s_hal_i2c_hw_gpio_init(id);
    s_hal_i2c_hw_init(id);
    s_hal_i2c_hw_enable(id, cfg);
        
    
    s_hal_i2c_initted_set(id);
    
    s_hal_i2c_status_set(id, hal_false, 0);
    
    
    return(hal_res_OK);
}


static void s_hal_i2c_hw_init(hal_i2c_t id)
{
#if     defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)
    
    uint32_t RCC_APB1Periph_I2Cx = s_hal_i2c_hw_rcc[HAL_i2c_id2index(id)];
    
//    // system configuration controller clock
//    #warning HAL-INFO --> in stm32f4 removed "RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);" from i2c_hw_init() and it still works....
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    // i2c periph clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2Cx, ENABLE);
    
    // reset i2c periph
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2Cx, ENABLE);
    
    // release reset 
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2Cx, DISABLE);

#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 
}


static void s_hal_i2c_hw_gpio_init(hal_i2c_t id)
{
    
#if     defined(HAL_USE_CPU_FAM_STM32F1)

    static const GPIO_InitTypeDef  s_hal_i2c_sclsda_altcfg  =
    {
        .GPIO_Pin       = 0,
        .GPIO_Speed     = GPIO_Speed_50MHz,
        .GPIO_Mode      = GPIO_Mode_AF_OD,
    }; 

    // 1. prepare af.
    // for i2c1 (scl, sda): no-remap if it is (PB6, PB7). GPIO_Remap_I2C1 if it is (PB8, PB9).
    // for i2c2 (scl, sda): no remap if it is (PB10, PB11).
    uint32_t afname = HAL_GPIO_AFNAME_NONE;
    uint32_t afmode = HAL_GPIO_AFMODE_NONE;
    hal_bool_t found = hal_false;

    
    hal_gpio_port_t portscl = hal_brdcfg_i2c__theconfig.gpio_scl[HAL_i2c_id2index(id)].gpio.port;
    hal_gpio_pin_t  pinscl  = hal_brdcfg_i2c__theconfig.gpio_scl[HAL_i2c_id2index(id)].gpio.pin;
    hal_gpio_port_t portsda = hal_brdcfg_i2c__theconfig.gpio_sda[HAL_i2c_id2index(id)].gpio.port;
    hal_gpio_pin_t  pinsda  = hal_brdcfg_i2c__theconfig.gpio_sda[HAL_i2c_id2index(id)].gpio.pin;    
    
    if(hal_i2c1 == id)
    {        
        if((hal_gpio_portB == portscl) && (hal_gpio_portB == portsda) && (hal_gpio_pin6 == pinscl) && (hal_gpio_pin7 == pinsda))
        {
            afname = HAL_GPIO_AFNAME_NONE;  afmode = HAL_GPIO_AFMODE_NONE;      found = hal_true;
        }
        if((hal_gpio_portB == portscl) && (hal_gpio_portB == portsda) && (hal_gpio_pin8 == pinscl) && (hal_gpio_pin9 == pinsda))
        {
            afname = GPIO_Remap_I2C1;       afmode = ENABLE;                    found = hal_true;
        }            
    }
    else if(hal_i2c2 == id)
    {
        if((hal_gpio_portB == portscl) && (hal_gpio_portB == portsda) && (hal_gpio_pin10 == pinscl) && (hal_gpio_pin11 == pinsda))
        {
            afname = HAL_GPIO_AFNAME_NONE;  afmode = HAL_GPIO_AFMODE_NONE;      found = hal_true;
        }  
    }
    
    if(hal_false == found)
    {
        hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_i2c_init(): incorrect pin mapping");
    }

    hal_gpio_altcfg_t hal_i2c_scl_altcfg;
    hal_gpio_altcfg_t hal_i2c_sda_altcfg;
    hal_gpio_cfg_t config;
    
    // prepare the altcfg for scl and sda pins
    memcpy(&hal_i2c_scl_altcfg, &s_hal_i2c_sclsda_altcfg, sizeof(hal_gpio_altcfg_t));
    memcpy(&hal_i2c_sda_altcfg, &s_hal_i2c_sclsda_altcfg, sizeof(hal_gpio_altcfg_t));
    hal_i2c_scl_altcfg.afname = hal_i2c_sda_altcfg.afname = afname;
    hal_i2c_scl_altcfg.afmode = hal_i2c_sda_altcfg.afmode = afmode;
    
    // configure scl and sda pins
    memcpy(&config, &hal_brdcfg_i2c__theconfig.gpio_scl[HAL_i2c_id2index(id)].config, sizeof(hal_gpio_cfg_t));
    config.altcfg = &hal_i2c_scl_altcfg;
    hal_gpio_init(hal_brdcfg_i2c__theconfig.gpio_scl[HAL_i2c_id2index(id)].gpio, &config);

    memcpy(&config, &hal_brdcfg_i2c__theconfig.gpio_sda[HAL_i2c_id2index(id)].config, sizeof(hal_gpio_cfg_t));
    config.altcfg = &hal_i2c_sda_altcfg;
    hal_gpio_init(hal_brdcfg_i2c__theconfig.gpio_sda[HAL_i2c_id2index(id)].gpio, &config);    
    
//     hal_gpio_configure(hal_brdcfg_i2c__theconfig.gpio_scl[HAL_i2c_id2index(id)], &hal_i2c_scl_altcfg);    
//     hal_gpio_configure(hal_brdcfg_i2c__theconfig.gpio_sda[HAL_i2c_id2index(id)], &hal_i2c_sda_altcfg);


#elif   defined(HAL_USE_CPU_FAM_STM32F4)    

    static const GPIO_InitTypeDef  s_hal_i2c_sclsda_altcfg  =
    {
        .GPIO_Pin       = GPIO_Pin_All,
        .GPIO_Mode      = GPIO_Mode_AF,
        .GPIO_Speed     = GPIO_Speed_50MHz,
        .GPIO_OType     = GPIO_OType_OD,
        .GPIO_PuPd      = GPIO_PuPd_NOPULL
    }; 
    
    
    // 1. prepare af.
    // for i2c1 (scl, sda): scl -> PB6, PB8. sda -> PB7, PB9
    // for i2c2 (scl, sda): scl -> PF1, PH4, PB10. sda -> PF0, PH5, PB11
    // for i2c3 (scl, sda): scl -> PH7, PA8. sda -> PH8, PC9, PA9

    uint32_t afname = HAL_GPIO_AFNAME_NONE;
    uint32_t afmode = HAL_GPIO_AFMODE_NONE;
    hal_bool_t foundscl = hal_false;
    hal_bool_t foundsda = hal_false;

    
    hal_gpio_port_t portscl = hal_brdcfg_i2c__theconfig.gpio_scl[HAL_i2c_id2index(id)].gpio.port;
    hal_gpio_pin_t  pinscl  = hal_brdcfg_i2c__theconfig.gpio_scl[HAL_i2c_id2index(id)].gpio.pin;
    hal_gpio_port_t portsda = hal_brdcfg_i2c__theconfig.gpio_sda[HAL_i2c_id2index(id)].gpio.port;
    hal_gpio_pin_t  pinsda  = hal_brdcfg_i2c__theconfig.gpio_sda[HAL_i2c_id2index(id)].gpio.pin;       
    
    if(hal_i2c1 == id)
    { 
        afname = GPIO_AF_I2C1;  afmode = ENABLE;
        
        if( ((hal_gpio_portB == portscl) && (hal_gpio_pin6 == pinscl))  ||
            ((hal_gpio_portB == portscl) && (hal_gpio_pin8 == pinscl))  )
        {
            foundscl = hal_true;
        }
        
        if( ((hal_gpio_portB == portsda) && (hal_gpio_pin7 == pinsda))  ||
            ((hal_gpio_portB == portsda) && (hal_gpio_pin9 == pinsda))  )
        {
            foundsda = hal_true;
        }
    }
    else if(hal_i2c2 == id)
    {   
        afname = GPIO_AF_I2C2;  afmode = ENABLE;
        
        if( ((hal_gpio_portF == portscl) && (hal_gpio_pin1 == pinscl))  ||
            ((hal_gpio_portH == portscl) && (hal_gpio_pin4 == pinscl))  ||
            ((hal_gpio_portB == portscl) && (hal_gpio_pin10 == pinscl)) )
        {
            foundscl = hal_true;
        }
        
        if( ((hal_gpio_portF == portsda) && (hal_gpio_pin0 == pinsda))  ||
            ((hal_gpio_portH == portsda) && (hal_gpio_pin5 == pinsda))  ||
            ((hal_gpio_portB == portsda) && (hal_gpio_pin11 == pinsda)) )
        {
            foundsda = hal_true;
        }
    }
    else if(hal_i2c3 == id)
    {
        afname = GPIO_AF_I2C3;  afmode = ENABLE;
        
        if( ((hal_gpio_portH == portscl) && (hal_gpio_pin7 == pinscl))  ||
            ((hal_gpio_portA == portscl) && (hal_gpio_pin8 == pinscl))  )
        {
            foundscl = hal_true;
        }
        
        if( ((hal_gpio_portH == portsda) && (hal_gpio_pin8 == pinsda))  ||
            ((hal_gpio_portC == portsda) && (hal_gpio_pin9 == pinsda))  ||
            ((hal_gpio_portB == portsda) && (hal_gpio_pin9 == pinsda))  )
        {
            foundsda = hal_true;
        }    
    
    }
    
    if((hal_false == foundscl) || (hal_false == foundsda))
    {
        hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_i2c_init(): incorrect pin mapping");
    }
    
   
    hal_gpio_altcfg_t hal_i2c_scl_altcfg;
    hal_gpio_altcfg_t hal_i2c_sda_altcfg;
    hal_gpio_cfg_t config;
    
    // prepare the altcfg for scl and sda pins
    memcpy(&hal_i2c_scl_altcfg, &s_hal_i2c_sclsda_altcfg, sizeof(hal_gpio_altcfg_t));
    memcpy(&hal_i2c_sda_altcfg, &s_hal_i2c_sclsda_altcfg, sizeof(hal_gpio_altcfg_t));
    hal_i2c_scl_altcfg.afname = hal_i2c_sda_altcfg.afname = afname;
    hal_i2c_scl_altcfg.afmode = hal_i2c_sda_altcfg.afmode = afmode;
    
    // configure scl and sda pins
    memcpy(&config, &hal_brdcfg_i2c__theconfig.gpio_scl[HAL_i2c_id2index(id)].config, sizeof(hal_gpio_cfg_t));
    config.altcfg = &hal_i2c_scl_altcfg;
    hal_gpio_init(hal_brdcfg_i2c__theconfig.gpio_scl[HAL_i2c_id2index(id)].gpio, &config);  

    memcpy(&config, &hal_brdcfg_i2c__theconfig.gpio_sda[HAL_i2c_id2index(id)].config, sizeof(hal_gpio_cfg_t));
    config.altcfg = &hal_i2c_sda_altcfg;    
    hal_gpio_init(hal_brdcfg_i2c__theconfig.gpio_sda[HAL_i2c_id2index(id)].gpio, &config);    

    
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 
}

static void s_hal_i2c_hw_enable(hal_i2c_t id, const hal_i2c_cfg_t* cfg)
{
#if     defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)
    
//    #define HAL_i2c_id2stmI2C(p)          (s_hal_i2c_stmI2Cmap[HAL_i2c_id2index(p)])

    I2C_TypeDef* I2Cx = HAL_i2c_id2stmI2C(id);
    
    I2C_InitTypeDef i2c_cfg;
    memcpy(&i2c_cfg, &s_hal_i2c_stm32_cfg, sizeof(I2C_InitTypeDef));
    // apply the clockspeed 
    i2c_cfg.I2C_ClockSpeed  = (uint32_t)cfg->speed * 100000;
    i2c_cfg.I2C_OwnAddress1 = cfg->ownaddress;
    // i2c peripheral enable
    I2C_Cmd(I2Cx, ENABLE);
    // apply configuration
    I2C_Init(I2Cx, &i2c_cfg);
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 
}


static hal_result_t s_hal_i2c_transaction_begin(hal_i2c_t id, hal_i2c_devaddr_t devaddr)    
{
    volatile uint32_t timeout = 0;
    
    I2C_TypeDef* I2Cx = HAL_i2c_id2stmI2C(id);
   
    // wait until the bus is not busy anymore 
    timeout = s_hal_i2c_timeout_long;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
    }
    
    // send START condition
    I2C_GenerateSTART(I2Cx, ENABLE);
 
    // test on ev5
    timeout = s_hal_i2c_timeout_flag;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
    }
    
    // send address of device
    I2C_Send7bitAddress(I2Cx, devaddr, I2C_Direction_Transmitter);
    
    // test on ev6
    timeout = s_hal_i2c_timeout_flag;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
    }    
    
    return(hal_res_OK);   
}


static hal_result_t s_hal_i2c_transaction_tx(hal_i2c_t id,  hal_i2c_devaddr_t devaddr, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendstop)
{
    volatile uint32_t timeout = 0;    
    I2C_TypeDef* I2Cx = HAL_i2c_id2stmI2C(id);
    
//     #warning --> ci vuole il wait del flag busy ?? NO
//     // wait until the bus is not busy anymore 
//     timeout = s_hal_i2c_timeout_long;
//     while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
//     {
//         if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
//     }

    if(hal_true == sendstart)
    {
        // send START condition
        I2C_GenerateSTART(I2Cx, ENABLE);
        
        // test on ev5
        timeout = s_hal_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
        {
            if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
        } 

        // send address of device
        I2C_Send7bitAddress(I2Cx, devaddr, I2C_Direction_Transmitter);
        
        // test on ev6
        timeout = s_hal_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        {
            if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
        }  
    }    

        
    while(size) 
    {
        // send the byte to be written 
        I2C_SendData(I2Cx, *data); 
        
        data++;
        size--;
        
        // test on ev8 (or ev8_2 = I2C_EVENT_MASTER_BYTE_TRANSMITTED which is slower but more reliable)    
        timeout = s_hal_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        {
            if(0 == (timeout--))  return s_hal_i2c_timeoutexpired();
        }
        
    }

    if(hal_true == sendstop)
    {
        // send stop condition
        I2C_GenerateSTOP(I2Cx, ENABLE);
        
        //#warning HAL-INFO --> ci vuole il clear dei flags ???
        // perform a read on SR1 and SR2 register to clear eventually pending flags 
        (void)(I2Cx)->SR1;
        (void)(I2Cx)->SR2; 
    }

    return(hal_res_OK);
}    

static hal_result_t s_hal_i2c_transaction_rx(hal_i2c_t id,  hal_i2c_devaddr_t devaddr, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendnack, hal_bool_t sendstop)
{
    volatile uint32_t timeout = 0;    
    I2C_TypeDef* I2Cx = HAL_i2c_id2stmI2C(id);
    
//    #warning --> ci vuole il wait del flag busy ?? NO
//     // wait until the bus is not busy anymore 
//     timeout = s_hal_i2c_timeout_long;
//     while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
//     {
//         if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
//     }

    if(hal_true == sendstart)
    {
        // send START condition
        I2C_GenerateSTART(I2Cx, ENABLE);
     
        // test on ev5
        timeout = s_hal_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
        {
            if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
        }

        // send address for read
        I2C_Send7bitAddress(I2Cx, devaddr, I2C_Direction_Receiver);
        
        //#warning --> w/ I2C_GetFlagStatus() the eeprom does not work. with I2C_CheckEvent() it works
        // wait on addr flag to be set
        //while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == RESET)
        //{
        //}  
        timeout = s_hal_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        {
            if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
        }    
      
    }

 
 
 
#undef PRE_SEND_STOP

    while(size > 0)
    {
#if defined(PRE_SEND_STOP)    
        if(1 == size)
        {
            if(hal_true == sendnak)
            {
                // disable acknowledgement
                I2C_AcknowledgeConfig(I2Cx, DISABLE);  
            }                

            // clear addr by reading sr1 and then sr2
            (void)(I2Cx)->SR1;
            (void)(I2Cx)->SR2;
            
            if(hal_true == sendstop)
            {
                // send stop condition
                I2C_GenerateSTOP(I2Cx, ENABLE);
            }
        }
#endif        
        // wait for the byte to be received
        timeout = s_hal_i2c_timeout_flag;
        while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET)
        {                    
            if(0 == (timeout--))  return s_hal_i2c_timeoutexpired();
        }
        
        // read received byte
        *data = I2C_ReceiveData(I2Cx);
        
        // increment received data and decrement the size (bytes to be received yet)
        data++;
        size--;          
    }

#if !defined(PRE_SEND_STOP)   
 
    if(hal_true == sendnack)
    {
        // disable acknowledgement
        I2C_AcknowledgeConfig(I2Cx, DISABLE);  
    }                

    // clear addr by reading sr1 and then sr2
    (void)(I2Cx)->SR1;
    (void)(I2Cx)->SR2;
    
    if(hal_true == sendstop)
    {
        // send stop condition
        I2C_GenerateSTOP(I2Cx, ENABLE);
    }
#endif

    if(hal_true == sendstop)
    {
        // wait to make sure that the stop bit has been cleared
        timeout = s_hal_i2c_timeout_flag;
        while((I2Cx)->CR1 & I2C_CR1_STOP)
        {
            if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
        }  
    }

    if(hal_true == sendnack)
    {
        // re-enable acknowledgement to be ready for another reception 
        I2C_AcknowledgeConfig(I2Cx, ENABLE);   
    }        
    

    return(hal_res_OK);
}    
    
    

static hal_result_t s_i2c_read(hal_i2c_t id, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
#if 0
    uint8_t addr[3] = {0};

    hal_i2c_transaction_begin(id, devaddr);
    if(1 == regaddr.numofbytes)
    {
        addr[0] = regaddr.bytes.one;
    }
    else if(2 == regaddr.numofbytes)
    {
        addr[0] = (regaddr.bytes.two >> 8) & 0x00FF;
        addr[1] = (regaddr.bytes.two     ) & 0x00FF;
    }   
    else if(3 == regaddr.numofbytes)
    {
        addr[0] = regaddr.bytes.three[2]; 
        addr[1] = regaddr.bytes.three[1];  
        addr[2] = regaddr.bytes.three[0];          
    }        
    hal_i2c_transaction_transmit(id, &addr[0], regaddr.numofbytes,  hal_false, hal_false);
    hal_i2c_transaction_receive(id, data, size,  hal_true, hal_true, hal_true);
    hal_i2c_transaction_end(id, devaddr);
    return(hal_res_OK);
#else    

    //hal_result_t res = hal_res_NOK_generic;
    uint8_t reg1byteadr = 0;
    volatile uint32_t timeout = 0;
    
    I2C_TypeDef* I2Cx = HAL_i2c_id2stmI2C(id);
//    uint8_t usedma = s_stm32i2c_generics.use_dma[i2c-1];
    
    
    // wait until the bus is not busy anymore 
    timeout = s_hal_i2c_timeout_long;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
    }
    
    // send START condition
    I2C_GenerateSTART(I2Cx, ENABLE);
 
    // test on ev5
    timeout = s_hal_i2c_timeout_flag;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
    }
    
    // send address of device
    I2C_Send7bitAddress(I2Cx, devaddr, I2C_Direction_Transmitter);
    
    // test on ev6
    timeout = s_hal_i2c_timeout_flag;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
    }
    
    
    // end of transaction_begin
    
           
    // send address of register to read inside the device
    if(1 == regaddr.numofbytes)
    {
        reg1byteadr = regaddr.bytes.one;
        I2C_SendData(I2Cx, reg1byteadr);           
        // test on ev8 and clear it (ev8 is I2C_EVENT_MASTER_BYTE_TRANSMITTING ...)
        timeout = s_hal_i2c_timeout_flag;
#if 0                
        while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET)
#else
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
#endif        
        {
            if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
        }  
    
    }
    else if(2 == regaddr.numofbytes)
    {   
        reg1byteadr = (regaddr.bytes.two >> 8 ) & 0xFF00;               // msb first
        I2C_SendData(I2Cx, reg1byteadr);           
        // test on ev8 and clear it
        timeout = s_hal_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        {
            if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
        }  
        
        reg1byteadr = regaddr.bytes.two & 0x00FF;               // then lsb
        I2C_SendData(I2Cx, reg1byteadr);           
        // test on ev8 and clear it (ev8 is I2C_EVENT_MASTER_BYTE_TRANSMITTING ...)       
        timeout = s_hal_i2c_timeout_flag;
#if 0                
        while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET)
#else
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
#endif 
        {
            if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
        }               
    }
    
    // send START condition a second time 
    I2C_GenerateSTART(I2Cx, ENABLE);
 
    // test on ev5
    timeout = s_hal_i2c_timeout_flag;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
    }

   
    // send address for read
    I2C_Send7bitAddress(I2Cx, devaddr, I2C_Direction_Receiver);
    
    //#warning --> w/ I2C_GetFlagStatus() the eeprom does not work. with I2C_CheckEvent() it works
    // wait on addr flag to be set
    //while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == RESET)
    //{
    //}  
    timeout = s_hal_i2c_timeout_flag;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
    }
  
    
//    if(1 == size)
//    {
//        usedma = 0;
//    }
    
//    if(0 == usedma)
//    {
        s_hal_i2c_read_bytes(I2Cx, data, size);
//    }
//    else
//    {
//        s_stm32i2c_read_withdma(I2Cx, data, size);
//    }
    


    return(hal_res_OK);
#endif    
}

static void s_hal_i2c_read_bytes(I2C_TypeDef* I2Cx, uint8_t* data, uint16_t size)
{
    #undef PRE_SEND_STOP
    volatile uint32_t timeout = 0;
    
    while(size > 0)
    {
#if defined(PRE_SEND_STOP)    
        if(1 == size)
        {
            // disable acknowledgement
            I2C_AcknowledgeConfig(I2Cx, DISABLE);   

            // clear addr by reading sr1 and then sr2
            (void)(I2Cx)->SR1;
            (void)(I2Cx)->SR2;
            
            // send stop condition
            I2C_GenerateSTOP(I2Cx, ENABLE);
        }
#endif        
        // wait for the byte to be received
        timeout = s_hal_i2c_timeout_flag;
        while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET)
        {                    
            if(0 == (timeout--)) {s_hal_i2c_timeoutexpired(); return;}
        }
        
        // read received byte
        *data = I2C_ReceiveData(I2Cx);
        
        // increment received data and decrement the size (bytes to be received yet)
        data++;
        size--;          
    }

#if !defined(PRE_SEND_STOP)    
    // disable acknowledgement
    I2C_AcknowledgeConfig(I2Cx, DISABLE);   

    // clear addr by reading sr1 and then sr2
    (void)(I2Cx)->SR1;
    (void)(I2Cx)->SR2;
    
    // send stop condition
    I2C_GenerateSTOP(I2Cx, ENABLE);
#endif
    
    // wait to make sure that the stop bit has been cleared
    timeout = s_hal_i2c_timeout_flag;
    while((I2Cx)->CR1 & I2C_CR1_STOP)
    {
        if(0 == (timeout--)) {s_hal_i2c_timeoutexpired(); return;}
    }  

    // re-enable acknowledgement to be ready for another reception 
    I2C_AcknowledgeConfig(I2Cx, ENABLE);    
}



static hal_result_t s_hal_i2c_write(hal_i2c_t id, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
#if 0
    uint8_t addr[3] = {0};
    hal_i2c_transaction_begin(id, devaddr);
    if(1 == regaddr.numofbytes)
    {
        addr[0] = regaddr.bytes.one;
    }
    else if(2 == regaddr.numofbytes)
    {
        addr[0] = (regaddr.bytes.two >> 8) & 0x00FF;
        addr[1] = (regaddr.bytes.two     ) & 0x00FF;
    }
    else if(3 == regaddr.numofbytes)
    {
        addr[0] = regaddr.bytes.three[2]; 
        addr[1] = regaddr.bytes.three[1];  
        addr[2] = regaddr.bytes.three[0];          
    }      
    hal_i2c_transaction_transmit(id, &addr[0], regaddr.numofbytes,  hal_false, hal_false);
    hal_i2c_transaction_transmit(id, data, size,  hal_false, hal_true);
    hal_i2c_transaction_end(id, devaddr);
    
    return(hal_res_OK);
#else    
    //hal_result_t res = hal_res_NOK_generic;
    uint8_t reg1byteadr = 0;
    volatile uint32_t timeout = 0;
    
    I2C_TypeDef* I2Cx = HAL_i2c_id2stmI2C(id);

       
 ///////////////////////////////////////   
    
    
    
    // wait until the bus is not busy anymore 
    timeout = s_hal_i2c_timeout_long;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
    }

    // send START condition
    I2C_GenerateSTART(I2Cx, ENABLE);
    
    // test on ev5
    timeout = s_hal_i2c_timeout_flag;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
    } 

    // send address of device
    I2C_Send7bitAddress(I2Cx, devaddr, I2C_Direction_Transmitter);
    
    // test on ev6
    timeout = s_hal_i2c_timeout_flag;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
    } 

    // end of transaction_begin
    

    // send address of register inside the device
    if(1 == regaddr.numofbytes)
    {
        reg1byteadr = regaddr.bytes.one;
        I2C_SendData(I2Cx, reg1byteadr);    
     
        // test on ev8
        timeout = s_hal_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        {
            if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
        } 
    }
    else if(2 == regaddr.numofbytes)
    {   
        reg1byteadr = (regaddr.bytes.two >> 8) & 0xFF00;           // msb first
        I2C_SendData(I2Cx, reg1byteadr);    
     
        // test on ev8
        timeout = s_hal_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        {
            if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
        }  

        reg1byteadr = regaddr.bytes.two & 0x00FF;           // then lsb
        I2C_SendData(I2Cx, reg1byteadr);    
     
        // test on ev8
        timeout = s_hal_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        {
            if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
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
        s_hal_i2c_write_bytes(I2Cx, data, size);
//    }
//    else
//    {
//        s_stm32i2c_write_withdma(I2Cx, data, size);
//    }    
    
    
    
/////////////////////////////////////////////////////////////

    //s_stm32i2c_i2c_waitdevicestandbystate(id, devaddr);
    
    return(hal_res_OK);
#endif
}




static void s_hal_i2c_write_bytes(I2C_TypeDef* I2Cx, uint8_t* data, uint16_t size)
{
    volatile uint32_t timeout = 0;
    
    while(size) 
    {
 
        // send the byte to be written 
        I2C_SendData(I2Cx, *data); 
        
        data++;
        size--;
        
        // test on ev8 (or ev8_2 = I2C_EVENT_MASTER_BYTE_TRANSMITTED which is slower but more reliable)    
        timeout = s_hal_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        {
            if(0 == (timeout--)) {s_hal_i2c_timeoutexpired(); return;}
        }
    
    }
    
    // send stop condition
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
     // perform a read on SR1 and SR2 register to clear eventually pending flags 
    (void)(I2Cx)->SR1;
    (void)(I2Cx)->SR2; 
}


static hal_result_t s_hal_i2c_ping(hal_i2c_t id, hal_i2c_devaddr_t devaddr)      
{
    hal_result_t res = hal_res_OK;

    volatile uint32_t timeout = 0;
    
    I2C_TypeDef* I2Cx = HAL_i2c_id2stmI2C(id);
 
    // wait until the bus is not busy anymore 
    timeout = s_hal_i2c_timeout_long;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (timeout--))    { res = hal_res_NOK_generic; break; }
    }
    
    if(hal_res_NOK_generic != res)
    {
        // send START condition
        I2C_GenerateSTART(I2Cx, ENABLE);

        // test on ev5
        timeout = s_hal_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
        {
            if(0 == (timeout--))    { res = hal_res_NOK_generic; break; }
        }
    }
    
    if(hal_res_NOK_generic != res)
    {
        // send address of device
        I2C_Send7bitAddress(I2Cx, devaddr, I2C_Direction_Transmitter);
        
        // test on ev6
        timeout = s_hal_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        {
            if(0 == (timeout--))    { res = hal_res_NOK_generic; break; }
        }
    }
    

    // send stop condition
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
     // perform a read on SR1 and SR2 register to clear eventually pending flags 
    (void)(I2Cx)->SR1;
    (void)(I2Cx)->SR2;     
    
    return(res);
}


static hal_result_t s_hal_i2c_standby(hal_i2c_t id, hal_i2c_devaddr_t devaddr)      
{
    volatile uint16_t tmpSR1 = 0;
    volatile uint32_t trials = 0;
    volatile uint32_t timeout = 0;
    
    I2C_TypeDef* I2Cx = HAL_i2c_id2stmI2C(id);
 
    // While the bus is busy 
    timeout = s_hal_i2c_timeout_long;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
    }

    //  keep looping till the slave acknowledge his address or maximum number of trials is reached
    while(1)
    {
        /*!< Send START condition */
        I2C_GenerateSTART(I2Cx, ENABLE);

        /*!< Test on EV5 and clear it */
        timeout = s_hal_i2c_timeout_flag;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
        {
            if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
        }    

        /*!< Send EEPROM address for write */
        I2C_Send7bitAddress(I2Cx, devaddr, I2C_Direction_Transmitter);

        /* Wait for ADDR flag to be set (Slave acknowledged his address) */
        timeout = s_hal_i2c_timeout_long;
        do
        {     
            /* Get the current value of the SR1 register */
            tmpSR1 = (I2Cx)->SR1;

            /* Update the timeout value and exit if it reach 0 */
            if(0 == (timeout--)) return s_hal_i2c_timeoutexpired();
        }
        /* Keep looping till the Address is acknowledged or the AF (ack failure) flag is 
        set (address not acknowledged at time) */
        while((tmpSR1 & (I2C_SR1_ADDR | I2C_SR1_AF)) == 0);

        /* Check if the ADDR flag has been set */
        if (tmpSR1 & I2C_SR1_ADDR)
        {
            /* Clear ADDR Flag by reading SR1 then SR2 registers (SR1 have already 
            been read) */
            (void)(I2Cx)->SR2;

            /*!< STOP condition */    
            I2C_GenerateSTOP(I2Cx, ENABLE);

            /* Exit the function */
            return hal_res_OK;
        }
        else
        {
            /*!< Clear AF flag */
            I2C_ClearFlag(I2Cx, I2C_FLAG_AF);                  
        }

        /* Check if the maximum allowed number of trials has been reached */
        if (s_hal_i2c_ackaddress_maxtrials == trials++)
        {
            /* If the maximum number of trials has been reached, exit the function */
            return s_hal_i2c_timeoutexpired();
        }
    }
}


static hal_result_t s_hal_i2c_timeoutexpired(void)
{ 
    hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "timeout error in i2c operations");
    return(hal_res_NOK_generic);
}

static void s_hal_i2c_scheduling_suspend(void)
{
    hal_base_osal_scheduling_suspend();
}

static void s_hal_i2c_scheduling_restart(void)
{
    hal_base_osal_scheduling_restart();
}



#endif//HAL_USE_PERIPH_I2C

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



