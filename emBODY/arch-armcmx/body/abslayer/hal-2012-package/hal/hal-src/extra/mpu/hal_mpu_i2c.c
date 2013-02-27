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


/* @file       hal_mpu_i2c.c
	@brief      This file implements internal implementation of the hal i2c module.
	@author     valentina.gaggero@iit.it
    @date       28/02/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_I2C

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_brdcfg.h"
#include "hal_base_hid.h"
#include "hal_mpu_gpio_hid.h"


#include "hal_utility_bits.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_i2c.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_mpu_i2c_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_i2c_port2index(p)           ((uint8_t)(p))

#define HAL_i2c_port2stmI2C(p)          (s_hal_i2c_stmI2Cmap[HAL_i2c_port2index(p)])


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

const hal_i2c_cfg_t hal_i2c_cfg_default = 
{ 
    .mode           = hal_i2c_mode_master, 
    .speed          = hal_i2c_speed_400kbps, //hal_i2c_speed_400kbps, 
    .ownaddress     = 0, 
    .usedma         = hal_false   
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

typedef struct
{
    hal_bool_t          locked;
    hal_i2c_devaddr_t   devaddr;   
} hal_i2c_status_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hal_i2c_scheduling_suspend(void);
static void s_hal_i2c_scheduling_restart(void);

static hal_boolval_t s_hal_i2c_supported_is(hal_i2c_port_t port);
static void s_hal_i2c_initted_set(hal_i2c_port_t port);

static void s_hal_i2c_status_set(hal_i2c_port_t port, hal_bool_t locked, hal_i2c_devaddr_t devaddr);

static hal_bool_t s_hal_i2c_is_status_locked(hal_i2c_port_t port);

static hal_i2c_devaddr_t s_hal_i2c_status_devaddr_get(hal_i2c_port_t port);

static hal_result_t s_hal_i2c_init(hal_i2c_port_t port, const hal_i2c_cfg_t *cfg);

static void s_hal_i2c_hw_init(hal_i2c_port_t port);
static void s_hal_i2c_hw_gpio_init(hal_i2c_port_t port);
static void s_hal_i2c_hw_enable(hal_i2c_port_t port, const hal_i2c_cfg_t* cfg);


static hal_result_t s_hal_i2c_transaction_begin(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr);

static hal_result_t s_hal_i2c_transaction_tx(hal_i2c_port_t port,  hal_i2c_devaddr_t devaddr, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendstop);
static hal_result_t s_hal_i2c_transaction_rx(hal_i2c_port_t port,  hal_i2c_devaddr_t devaddr, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendnack, hal_bool_t sendstop);

static hal_result_t s_i2c_read(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);
static void s_hal_i2c_read_bytes(I2C_TypeDef* I2Cx, uint8_t* data, uint16_t size);


static hal_result_t s_hal_i2c_write(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size);
static void s_hal_i2c_write_bytes(I2C_TypeDef* I2Cx, uint8_t* data, uint16_t size);

static hal_result_t s_hal_i2c_ping(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr);

static hal_result_t s_hal_i2c_standby(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr);

static hal_result_t s_hal_i2c_timeoutexpired(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_i2c_initted[hal_i2c_ports_number] = { hal_false, hal_false, hal_false };

static hal_i2c_status_t s_hal_i2c_status[hal_i2c_ports_number] = { {.locked = hal_false, .devaddr = 0 }, {.locked = hal_false, .devaddr = 0 }, {.locked = hal_false, .devaddr = 0 } };

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
#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_i2c_init(hal_i2c_port_t port, const hal_i2c_cfg_t *cfg)
{
    return(s_hal_i2c_init(port, cfg));
}


extern hal_bool_t hal_i2c_initted_is(hal_i2c_port_t port)
{

    if(hal_true == s_hal_i2c_initted[HAL_i2c_port2index(port)]) 
    {
        return(hal_true);
    }
    
    return(hal_false);

    
#if 0    
    I2C_TypeDef* I2Cx = NULL;
    // this code allows to understand if the registers are already initted. useful to avoid another init in a jump from bootloader .... but bettere to re-init anyway. 
    if (hal_i2c_port1 == port)
    {
        I2Cx = I2C1;
    }
    else if(hal_i2c_port2 == port)
    {
        I2Cx = I2C2;
    }

    if((I2Cx->CR1 & 0x0001) == 0x0001) 
    {
        //if i'm here it means the periph. has been initted, but i lost status variable in ram (for example i jumped from loader to appl)
        return(hal_true); 
    }

    return(hal_false);
#endif

}

extern hal_result_t hal_i2c_transaction_begin(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr)
{

    if(hal_false == hal_i2c_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    if(0 == devaddr)
    {
        return(hal_res_NOK_generic);
    }
    
    devaddr &= ~0x01;

    s_hal_i2c_scheduling_suspend();    
    if(hal_true == s_hal_i2c_is_status_locked(port))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_i2c_status_set(port, hal_true, devaddr);
    s_hal_i2c_scheduling_restart();
    
    return(s_hal_i2c_transaction_begin(port, devaddr));

}


extern hal_result_t hal_i2c_transaction_end(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr)
{
    if(hal_false == hal_i2c_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    devaddr &= ~0x01;
    
    s_hal_i2c_scheduling_suspend();
    if((hal_false == s_hal_i2c_is_status_locked(port)) || (devaddr != s_hal_i2c_status_devaddr_get(port)))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    
    s_hal_i2c_status_set(port, hal_false, 0);
    s_hal_i2c_scheduling_restart();
    
    return(hal_res_OK);    

}


extern hal_result_t hal_i2c_transaction_transmit(hal_i2c_port_t port, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendstop)
{
 
    if(hal_false == hal_i2c_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    s_hal_i2c_scheduling_suspend();
    if(hal_false == s_hal_i2c_is_status_locked(port))
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


    return(s_hal_i2c_transaction_tx(port, s_hal_i2c_status_devaddr_get(port), data, size, sendstart, sendstop));
}

   

    
extern hal_result_t hal_i2c_transaction_receive(hal_i2c_port_t port, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendnack, hal_bool_t sendstop)
{
    if(hal_false == hal_i2c_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    s_hal_i2c_scheduling_suspend();
    if(hal_false == s_hal_i2c_is_status_locked(port))
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


    return(s_hal_i2c_transaction_rx(port, s_hal_i2c_status_devaddr_get(port), data, size, sendstart, sendnack, sendstop));
}




extern hal_result_t hal_i2c_read(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
    hal_result_t res;

    if(hal_false == hal_i2c_initted_is(port))
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
    if(hal_true == s_hal_i2c_is_status_locked(port))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_i2c_status_set(port, hal_true, devaddr);
    s_hal_i2c_scheduling_restart();
    

    res = s_i2c_read(port, devaddr, regaddr, data, size);
    
    
    s_hal_i2c_scheduling_suspend();
    s_hal_i2c_status_set(port, hal_false, 0);
    s_hal_i2c_scheduling_restart();    
    
    return(res);
}


extern hal_result_t hal_i2c_write(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
    hal_result_t res;
    
    if(hal_false == hal_i2c_initted_is(port))
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
    if(hal_true == s_hal_i2c_is_status_locked(port))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_i2c_status_set(port, hal_true, devaddr);
    s_hal_i2c_scheduling_restart();
    

    res = s_hal_i2c_write(port, devaddr, regaddr, data, size);
    
    
    s_hal_i2c_scheduling_suspend();
    s_hal_i2c_status_set(port, hal_false, 0);
    s_hal_i2c_scheduling_restart();    
    
    return(res);    
}

extern hal_result_t hal_i2c_ping(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr)
{
   hal_result_t res;
    
    if(hal_false == hal_i2c_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    if(0 == devaddr)
    {
        return(hal_res_NOK_generic);
    }
    
    devaddr &= ~0x01;
    
    
    s_hal_i2c_scheduling_suspend();    
    if(hal_true == s_hal_i2c_is_status_locked(port))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_i2c_status_set(port, hal_true, devaddr);
    s_hal_i2c_scheduling_restart();
    

    res = s_hal_i2c_ping(port, devaddr);
    
    if(hal_res_OK == res)
    {
        s_hal_i2c_standby(port, devaddr);
    }
    else
    {
        
    }
    
    
    s_hal_i2c_scheduling_suspend();
    s_hal_i2c_status_set(port, hal_false, 0);
    s_hal_i2c_scheduling_restart();    
    
    return(res);            
}


extern hal_result_t hal_i2c_standby(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr)      
{
    hal_result_t res;
    
    if(hal_false == hal_i2c_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    if(0 == devaddr)
    {
        return(hal_res_NOK_generic);
    }
    
    devaddr &= ~0x01;
    
    
    s_hal_i2c_scheduling_suspend();    
    if(hal_true == s_hal_i2c_is_status_locked(port))
    {
        s_hal_i2c_scheduling_restart();
        return(hal_res_NOK_generic);
    }
    s_hal_i2c_status_set(port, hal_true, devaddr);
    s_hal_i2c_scheduling_restart();
    

    res = s_hal_i2c_standby(port, devaddr);
    
    
    s_hal_i2c_scheduling_suspend();
    s_hal_i2c_status_set(port, hal_false, 0);
    s_hal_i2c_scheduling_restart();    
    
    return(res);    
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern uint32_t hal_i2c_hid_getsize(const hal_base_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern hal_result_t hal_i2c_hid_setmem(const hal_base_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }

    // removed dependancy from nzi data
    memset(&s_hal_i2c_initted, hal_false, sizeof(s_hal_i2c_initted));   
    memset(&s_hal_i2c_status, 0, sizeof(s_hal_i2c_status));

    return(hal_res_OK);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_i2c_supported_is(hal_i2c_port_t port)
{
    return(hal_utility_bits_byte_bitcheck(hal_brdcfg_i2c__theconfig.supported_mask, HAL_i2c_port2index(port)) );
}

static void s_hal_i2c_initted_set(hal_i2c_port_t port)
{
    s_hal_i2c_initted[HAL_i2c_port2index(port)] = hal_true;
}

static void s_hal_i2c_status_set(hal_i2c_port_t port, hal_bool_t locked, hal_i2c_devaddr_t devaddr)
{
//    s_hal_i2c_scheduling_suspend();
    s_hal_i2c_status[HAL_i2c_port2index(port)].locked   = locked;
    s_hal_i2c_status[HAL_i2c_port2index(port)].devaddr  = devaddr;
//    s_hal_i2c_scheduling_restart();
}

static hal_bool_t s_hal_i2c_is_status_locked(hal_i2c_port_t port)
{
    return(s_hal_i2c_status[HAL_i2c_port2index(port)].locked);
}

static hal_i2c_devaddr_t s_hal_i2c_status_devaddr_get(hal_i2c_port_t port)
{
    return(s_hal_i2c_status[HAL_i2c_port2index(port)].devaddr);
}



static hal_result_t s_hal_i2c_init(hal_i2c_port_t port, const hal_i2c_cfg_t *cfg)
{
    if(NULL == cfg)
    {
        cfg = &hal_i2c_cfg_default;
    }
    
    if(hal_i2c_mode_slave == cfg->mode)
    {
        return(hal_res_NOK_unsupported);
    }
    
    if(hal_true != s_hal_i2c_supported_is(port))
    {
        return(hal_res_NOK_unsupported);
    }

    if(hal_true == hal_i2c_initted_is(port))
    {
        return(hal_res_OK);
    }    
    

    // acemor: very important info.
    // init the scl and sda gpio before calling hw_init. 
    // because if the i2c is already initted and it detects sda or scl low it sets
    // register I2C_SR2.BUSY to 1, which makes things hang up.
    s_hal_i2c_hw_gpio_init(port);
    s_hal_i2c_hw_init(port);
    s_hal_i2c_hw_enable(port, cfg);
        
    
    s_hal_i2c_initted_set(port);
    
    s_hal_i2c_status_set(port, hal_false, 0);
    
    
    return(hal_res_OK);
}


static void s_hal_i2c_hw_init(hal_i2c_port_t port)
{
#if     defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)
    
    uint32_t RCC_APB1Periph_I2Cx = s_hal_i2c_hw_rcc[HAL_i2c_port2index(port)];
    
//    // system configuration controller clock
//    #warning HAL-INFO --> in stm32f4 removed "RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);" from i2c_hw_init() and it still works....
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    // i2c periph clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2Cx, ENABLE);
    
    // reset i2c periph
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2Cx, ENABLE);
    
    // release reset 
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2Cx, DISABLE);

#endif
}


static void s_hal_i2c_hw_gpio_init(hal_i2c_port_t port)
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

    
    hal_gpio_port_t portscl = hal_brdcfg_i2c__theconfig.gpio_scl[HAL_i2c_port2index(port)].port;
    hal_gpio_pin_t  pinscl  = hal_brdcfg_i2c__theconfig.gpio_scl[HAL_i2c_port2index(port)].pin;
    hal_gpio_port_t portsda = hal_brdcfg_i2c__theconfig.gpio_sda[HAL_i2c_port2index(port)].port;
    hal_gpio_pin_t  pinsda  = hal_brdcfg_i2c__theconfig.gpio_sda[HAL_i2c_port2index(port)].pin;    
    
    if(hal_i2c_port1 == port)
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
    else if(hal_i2c_port2 == port)
    {
        if((hal_gpio_portB == portscl) && (hal_gpio_portB == portsda) && (hal_gpio_pin10 == pinscl) && (hal_gpio_pin11 == pinsda))
        {
            afname = HAL_GPIO_AFNAME_NONE;  afmode = HAL_GPIO_AFMODE_NONE;      found = hal_true;
        }  
    }
    
    if(hal_false == found)
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_i2c_init(): incorrect pin mapping");
    }

    hal_gpio_altcfg_t hal_i2c_scl_altcfg;
    hal_gpio_altcfg_t hal_i2c_sda_altcfg;
    
    // prepare the altcfg for scl and sda pins
    memcpy(&hal_i2c_scl_altcfg, &s_hal_i2c_sclsda_altcfg, sizeof(hal_gpio_altcfg_t));
    memcpy(&hal_i2c_sda_altcfg, &s_hal_i2c_sclsda_altcfg, sizeof(hal_gpio_altcfg_t));
    hal_i2c_scl_altcfg.afname = hal_i2c_sda_altcfg.afname = afname;
    hal_i2c_scl_altcfg.afmode = hal_i2c_sda_altcfg.afmode = afmode;
    
    // configure scl and sda pins
    hal_gpio_configure(hal_brdcfg_i2c__theconfig.gpio_scl[HAL_i2c_port2index(port)], &hal_i2c_scl_altcfg);    
    hal_gpio_configure(hal_brdcfg_i2c__theconfig.gpio_sda[HAL_i2c_port2index(port)], &hal_i2c_sda_altcfg);


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

    
    hal_gpio_port_t portscl = hal_brdcfg_i2c__theconfig.gpio_scl[HAL_i2c_port2index(port)].port;
    hal_gpio_pin_t  pinscl  = hal_brdcfg_i2c__theconfig.gpio_scl[HAL_i2c_port2index(port)].pin;
    hal_gpio_port_t portsda = hal_brdcfg_i2c__theconfig.gpio_sda[HAL_i2c_port2index(port)].port;
    hal_gpio_pin_t  pinsda  = hal_brdcfg_i2c__theconfig.gpio_sda[HAL_i2c_port2index(port)].pin;       
    
    if(hal_i2c_port1 == port)
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
    else if(hal_i2c_port2 == port)
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
    else if(hal_i2c_port3 == port)
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
        hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_i2c_init(): incorrect pin mapping");
    }
    
   
    hal_gpio_altcfg_t hal_i2c_scl_altcfg;
    hal_gpio_altcfg_t hal_i2c_sda_altcfg;
    
    // prepare the altcfg for scl and sda pins
    memcpy(&hal_i2c_scl_altcfg, &s_hal_i2c_sclsda_altcfg, sizeof(hal_gpio_altcfg_t));
    memcpy(&hal_i2c_sda_altcfg, &s_hal_i2c_sclsda_altcfg, sizeof(hal_gpio_altcfg_t));
    hal_i2c_scl_altcfg.afname = hal_i2c_sda_altcfg.afname = afname;
    hal_i2c_scl_altcfg.afmode = hal_i2c_sda_altcfg.afmode = afmode;
    
    // configure scl and sda pins
    hal_gpio_configure(hal_brdcfg_i2c__theconfig.gpio_scl[HAL_i2c_port2index(port)], &hal_i2c_scl_altcfg);    
    hal_gpio_configure(hal_brdcfg_i2c__theconfig.gpio_sda[HAL_i2c_port2index(port)], &hal_i2c_sda_altcfg);    

    
#endif
}

static void s_hal_i2c_hw_enable(hal_i2c_port_t port, const hal_i2c_cfg_t* cfg)
{
#if     defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)
    
//    #define HAL_i2c_port2stmI2C(p)          (s_hal_i2c_stmI2Cmap[HAL_i2c_port2index(p)])

    I2C_TypeDef* I2Cx = HAL_i2c_port2stmI2C(port);
    
    I2C_InitTypeDef i2c_cfg;
    memcpy(&i2c_cfg, &s_hal_i2c_stm32_cfg, sizeof(I2C_InitTypeDef));
    // apply the clockspeed 
    i2c_cfg.I2C_ClockSpeed  = (uint32_t)cfg->speed * 100000;
    i2c_cfg.I2C_OwnAddress1 = cfg->ownaddress;
    // i2c peripheral enable
    I2C_Cmd(I2Cx, ENABLE);
    // apply configuration
    I2C_Init(I2Cx, &i2c_cfg);
#endif
}


static hal_result_t s_hal_i2c_transaction_begin(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr)    
{
    volatile uint32_t timeout = 0;
    
    I2C_TypeDef* I2Cx = HAL_i2c_port2stmI2C(port);
   
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


static hal_result_t s_hal_i2c_transaction_tx(hal_i2c_port_t port,  hal_i2c_devaddr_t devaddr, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendstop)
{
    volatile uint32_t timeout = 0;    
    I2C_TypeDef* I2Cx = HAL_i2c_port2stmI2C(port);
    
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

static hal_result_t s_hal_i2c_transaction_rx(hal_i2c_port_t port,  hal_i2c_devaddr_t devaddr, uint8_t* data, uint16_t size, hal_bool_t sendstart, hal_bool_t sendnack, hal_bool_t sendstop)
{
    volatile uint32_t timeout = 0;    
    I2C_TypeDef* I2Cx = HAL_i2c_port2stmI2C(port);
    
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
    
    

static hal_result_t s_i2c_read(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
#if 0
    uint8_t addr[3] = {0};

    hal_i2c_transaction_begin(port, devaddr);
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
    hal_i2c_transaction_transmit(port, &addr[0], regaddr.numofbytes,  hal_false, hal_false);
    hal_i2c_transaction_receive(port, data, size,  hal_true, hal_true, hal_true);
    hal_i2c_transaction_end(port, devaddr);
    return(hal_res_OK);
#else    

    //hal_result_t res = hal_res_NOK_generic;
    uint8_t reg1byteadr = 0;
    volatile uint32_t timeout = 0;
    
    I2C_TypeDef* I2Cx = HAL_i2c_port2stmI2C(port);
//    uint8_t usedma = s_stm32i2c_generics.use_dma[port-1];
    
    
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



static hal_result_t s_hal_i2c_write(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
#if 0
    uint8_t addr[3] = {0};
    hal_i2c_transaction_begin(port, devaddr);
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
    hal_i2c_transaction_transmit(port, &addr[0], regaddr.numofbytes,  hal_false, hal_false);
    hal_i2c_transaction_transmit(port, data, size,  hal_false, hal_true);
    hal_i2c_transaction_end(port, devaddr);
    
    return(hal_res_OK);
#else    
    //hal_result_t res = hal_res_NOK_generic;
    uint8_t reg1byteadr = 0;
    volatile uint32_t timeout = 0;
    
    I2C_TypeDef* I2Cx = HAL_i2c_port2stmI2C(port);

       
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

    //s_stm32i2c_i2c_waitdevicestandbystate(port, devaddr);
    
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


static hal_result_t s_hal_i2c_ping(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr)      
{
    hal_result_t res = hal_res_OK;

    volatile uint32_t timeout = 0;
    
    I2C_TypeDef* I2Cx = HAL_i2c_port2stmI2C(port);
 
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


static hal_result_t s_hal_i2c_standby(hal_i2c_port_t port, hal_i2c_devaddr_t devaddr)      
{
    volatile uint16_t tmpSR1 = 0;
    volatile uint32_t trials = 0;
    volatile uint32_t timeout = 0;
    
    I2C_TypeDef* I2Cx = HAL_i2c_port2stmI2C(port);
 
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
    hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "timeout error in i2c operations");
    return(hal_res_NOK_generic);
}

static void s_hal_i2c_scheduling_suspend(void)
{
    hal_base_hid_osal_scheduling_suspend();
}

static void s_hal_i2c_scheduling_restart(void)
{
    hal_base_hid_osal_scheduling_restart();
}



#endif//HAL_USE_I2C

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



