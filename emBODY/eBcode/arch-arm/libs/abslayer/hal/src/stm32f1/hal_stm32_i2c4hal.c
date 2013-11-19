/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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


/* @file       hal_stm32_i2c.c
	@brief      This file implements internal implementation of the hal i2c module.
	@author     valentina.gaggero@iit.it
    @date       28/02/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_I2C4HAL

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_brdcfg.h"
#include "stm32_utility/stm32_eval_i2c_ee.h"
#include "hal_stm32_base_hid.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_i2c4hal.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_stm32_i2c4hal_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_i2c_port2index(p)       ((uint8_t)(p))


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

const hal_i2c_cfg_t hal_i2c_cfg_default = 
{ 
    .dummy = 0 
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


/* @typedef    typedef enum hal_i2c_dutycycle_t 
    @brief      hal_i2c_dutycycle_t contains the duty cycles values for i2c.
 **/
typedef enum
{
    hal_i2c_dutycycle_16_9  = 0,
    hal_i2c_dutycycle_2     = 1  
} hal_i2c_dutycycle_t;


/* @typedef    typedef enum hal_i2c_ackaddr_t 
    @brief      hal_i2c_ackaddr_t contains the address types in ack for i2c.
 **/
typedef enum
{
    hal_i2c_ackaddr_7bit = 0,
    hal_i2c_ackaddr_10bit = 1 
} hal_i2c_ackaddr_t;


/* @typedef    typedef enum hal_i2c_4eeprom_cfg_t 
    @brief      hal_i2c_4eeprom_cfg_t contains the configuation for i2c used for the eeprom and ?
 **/
typedef struct
{
    uint32_t                    clock_speed;    /**< The clock frequency. This parameter must be set to a value lower than 400kHz */
    hal_i2c_dutycycle_t         duty_cycle;     /**< The I2C fast mode duty cycle */
    uint16_t                    own_address;    /**< The first device own address. This parameter can be a 7-bit or 10-bit address. */
    uint8_t                     ack_enabled;    /**< Enables or disables the acknowledgement. If its value is 0, then ack is disabled, otherwise is enabled */
    hal_i2c_ackaddr_t           ack_addr;       /**< Specifies if 7-bit or 10-bit address is acknowledged. */
} hal_i2c_4eeprom_cfg_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_i2c4hal_supported_is(hal_i2c_port_t port);
static void s_hal_i2c4hal_initted_set(hal_i2c_port_t port);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_i2c4hal_initted[hal_i2c_ports_number] = { hal_false };

static const hal_i2c_4eeprom_cfg_t hal_i2c_some_cfg_default =
{
    .clock_speed    = I2C_SPEED, /*200kHz*/             // clock_speed  (defined in stm32_eval_i2c_ee.h) 
    .duty_cycle     = hal_i2c_dutycycle_16_9,           // duty_cycle     
    .own_address    = I2C_SLAVE_ADDRESS7,               // own_address   (defined in stm32_eval_i2c_ee.h) 
    .ack_enabled    = 1,                                // ack_enabled
    .ack_addr       = hal_i2c_ackaddr_7bit              // ack_addr
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

/* Note: since the i2c configuration is the same for all device, hal_i2c_init function use 
   constant configuration defined in this file and not the parameter.
   But we contemplete the parameter in order to configur i2c in different way. */

extern hal_result_t hal_i2c4hal_init(hal_i2c_port_t port, const hal_i2c_cfg_t *cfg)
{
    I2C_InitTypeDef  I2C_InitStructure;

    const hal_i2c_4eeprom_cfg_t *somecfg = &hal_i2c_some_cfg_default;

    if(hal_true != s_hal_i2c4hal_supported_is(port))
    {
        return(hal_res_NOK_unsupported);
    }


    if(hal_true == hal_i2c4hal_hid_initted_is(port))
    {
        return(hal_res_OK);
    }

    if (hal_i2c_port1 == port)
    {
        I2C_DeInit(I2C1);
    }
    else if(hal_i2c_port2 == port)
    {
        I2C_DeInit(I2C2);
    }
    else
    {
        return(hal_res_NOK_generic);
    }


    hal_brdcfg_i2c4hal__LowLevel_Init();

    if(hal_i2c_dutycycle_16_9 == somecfg->duty_cycle)
    {
        I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;
    }
    else
    {
        I2C_InitStructure.I2C_DutyCycle  = I2C_DutyCycle_2;
    }

    if(somecfg->ack_enabled)
    {
        I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; 
    }
    else
    {
       I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
    }

    if(hal_i2c_ackaddr_7bit == somecfg->ack_addr)
    {
        I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    }
    else
    {
        I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;
    }

    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_OwnAddress1 = somecfg->own_address;
    I2C_InitStructure.I2C_ClockSpeed = somecfg->clock_speed;
    
    /* sEE_I2C Peripheral Enable and apply configuration after enabling it */
    
    if (hal_i2c_port1 == port)
    {
        I2C_Init(I2C1, &I2C_InitStructure);
        I2C_Cmd(I2C1, ENABLE);

    }
    else if(hal_i2c_port2 == port)
    {
        I2C_Init(I2C2, &I2C_InitStructure);
        I2C_Cmd(I2C2, ENABLE);
    }
    else
    {
        return(hal_res_NOK_generic);
    }

    s_hal_i2c4hal_initted_set(port);

    return(hal_res_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern uint32_t hal_i2c4hal_hid_getsize(const hal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern hal_result_t hal_i2c4hal_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }

    // removed dependancy from nzi data
    memset(s_hal_i2c4hal_initted, hal_false, sizeof(s_hal_i2c4hal_initted));

    return(hal_res_OK);
}

extern hal_boolval_t hal_i2c4hal_hid_initted_is(hal_i2c_port_t port)
{

    I2C_TypeDef* I2Cx = NULL;


    if(hal_true == s_hal_i2c4hal_initted[HAL_i2c_port2index(port)]) 
    {
        return(hal_true);
    }


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
    //return(s_hal_i2c4hal_initted[HAL_i2c_port2index(port)]);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_i2c4hal_supported_is(hal_i2c_port_t port)
{
    return(hal_base_hid_byte_bitcheck(hal_brdcfg_i2c4hal__supported_mask, HAL_i2c_port2index(port)) );
}

static void s_hal_i2c4hal_initted_set(hal_i2c_port_t port)
{
    s_hal_i2c4hal_initted[HAL_i2c_port2index(port)] = hal_true;
}



#endif//HAL_USE_I2C4HAL

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



