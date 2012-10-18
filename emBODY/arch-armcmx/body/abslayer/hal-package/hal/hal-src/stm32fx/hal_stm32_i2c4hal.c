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
#include "utils/stm32i2c.h"
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
// empty-section

//sto mettendo nuove variabili di inizializzazione al i2c: hal_brdcfg_i2c4hal_hwcfg

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_i2c4hal_supported_is(hal_i2c_port_t port);
static void s_hal_i2c4hal_initted_set(hal_i2c_port_t port);

static void s_hal_i2c4hal_ontimeout(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_i2c4hal_initted[hal_i2c_ports_number] = { hal_false, hal_false, hal_false };


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

/* Note: since the i2c configuration is the same for all device, hal_i2c_init function use 
   constant configuration defined in this file and not the parameter.
   But we contemplete the parameter in order to configur i2c in different way. */

extern hal_result_t hal_i2c4hal_init(hal_i2c_port_t port, const hal_i2c_cfg_t *cfg)
{
    hal_result_t res = hal_res_NOK_generic;
    
    stm32i2c_cfg_t i2ccfg          = 
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
        .ontimeout          = s_hal_i2c4hal_ontimeout                          
    };

    //const hal_i2c_4eeprom_cfg_t *somecfg = &hal_i2c_some_cfg_default;

    if(hal_true != s_hal_i2c4hal_supported_is(port))
    {
        return(hal_res_NOK_unsupported);
    }


    if(hal_true == hal_i2c4hal_hid_initted_is(port))
    {
        return(hal_res_OK);
    }

    i2ccfg.speed        = 100 * 1000 * (uint32_t)hal_brdcfg_i2c4hal__hwcfg.speed;
    i2ccfg.scl.port     = (stm32gpio_port_t)hal_brdcfg_i2c4hal__hwcfg.scl.port;
    i2ccfg.scl.pin      = (stm32gpio_pin_t)hal_brdcfg_i2c4hal__hwcfg.scl.pin;
    i2ccfg.sda.port     = (stm32gpio_port_t)hal_brdcfg_i2c4hal__hwcfg.sda.port;
    i2ccfg.sda.pin      = (stm32gpio_pin_t)hal_brdcfg_i2c4hal__hwcfg.sda.pin;  
    i2ccfg.usedma       = (hal_true == hal_brdcfg_i2c4hal__hwcfg.usedma) ? 1 : 0;
    i2ccfg.ontimeout    = (NULL == hal_brdcfg_i2c4hal__hwcfg.ontimeout) ? (s_hal_i2c4hal_ontimeout) : (hal_brdcfg_i2c4hal__hwcfg.ontimeout);
    res = (hal_result_t) stm32i2c_init(HAL_i2c_port2index(port)+1, &i2ccfg);
    //res = (hal_result_t) stm32i2c_init(HAL_i2c_port2index(port)+1, &hal_brdcfg_i2c4hal__i2ccfg);

    if(hal_res_OK != res)
    {
        return(res);
    }

    s_hal_i2c4hal_initted_set(port);

    return(hal_res_OK);
}

extern hal_result_t hal_i2c4hal_read(hal_i2c_port_t port, uint8_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
//    hal_result_t res = hal_res_NOK_generic;
//    uint8_t reg1byteadr = 0;

    if(hal_true != s_hal_i2c4hal_supported_is(port))
    {
        return(hal_res_NOK_unsupported);
    }

    if(hal_false == hal_i2c4hal_hid_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    if((0 == regaddr.numofbytes) || (regaddr.numofbytes > 3))
    {
        return(hal_res_NOK_unsupported);
    }  

    stm32i2c_regaddr_t i2cregaddr;
    memcpy(&i2cregaddr, &regaddr, sizeof(stm32i2c_regaddr_t));
    return((hal_result_t) stm32i2c_read(HAL_i2c_port2index(port)+1, devaddr, i2cregaddr, data, size));

}



extern hal_result_t hal_i2c4hal_write(hal_i2c_port_t port, uint8_t devaddr, hal_i2c_regaddr_t regaddr, uint8_t* data, uint16_t size)
{
//    hal_result_t res = hal_res_NOK_generic;
//    uint8_t reg1byteadr = 0;

    if(hal_true != s_hal_i2c4hal_supported_is(port))
    {
        return(hal_res_NOK_unsupported);
    }

    if(hal_false == hal_i2c4hal_hid_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    if((0 == regaddr.numofbytes) || (regaddr.numofbytes > 3))
    {
        return(hal_res_NOK_unsupported);
    }  

    stm32i2c_regaddr_t i2cregaddr;
    memcpy(&i2cregaddr, &regaddr, sizeof(stm32i2c_regaddr_t));
    return((hal_result_t) stm32i2c_write(HAL_i2c_port2index(port)+1, devaddr, i2cregaddr, data, size));

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

    if(hal_true == s_hal_i2c4hal_initted[HAL_i2c_port2index(port)]) 
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


static void s_hal_i2c4hal_ontimeout(void)
{   
    hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "timeout error in i2c operations");
}

#endif//HAL_USE_I2C4HAL

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



