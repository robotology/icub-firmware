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

/* @file       hal_device_eeprom.h
	@brief      This file implements internals for management of hal device eeprom
	@author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/09/2010
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_DEVICE_EEPROM_dummy

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "string.h"

//#include "hal_middleware_interface.h" // to see middleware (if needed)


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_eeprom.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_eeprom_cfg_t hal_device_eeprom_cfg_default =
{
    .flashpagebuffer    = NULL, 
    .flashpagesize      = 0
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define MYEEPROMDUMMYSIZE 100

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static uint8_t myeepromdummy[MYEEPROMDUMMYSIZE] = {0}; 


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_eeprom_init(hal_eeprom_t id, const hal_eeprom_cfg_t *cfg)
{
//    GPIO_InitTypeDef gpioinit;
//    GPIO_StructInit(&gpioinit);
    return(hal_res_OK);
}


extern hal_result_t hal_eeprom_erase(hal_eeprom_t id, uint32_t addr, uint32_t size)
{   
    memset(myeepromdummy, 0xff, MYEEPROMDUMMYSIZE);
    return(hal_res_OK);
}



extern hal_result_t hal_eeprom_write(hal_eeprom_t id, uint32_t addr, uint32_t size, void *data)
{
    if(size > MYEEPROMDUMMYSIZE)
    {
        size = MYEEPROMDUMMYSIZE;
    }
    memcpy(myeepromdummy, data, size);
    return(hal_res_OK);
}


extern hal_result_t hal_eeprom_read(hal_eeprom_t id, uint32_t addr, uint32_t size, void *data)
{
    if(size > MYEEPROMDUMMYSIZE)
    {
        size = MYEEPROMDUMMYSIZE;
    }
    memcpy(data, myeepromdummy, size);
	return(hal_res_OK);
}



extern uint32_t hal_eeprom_get_baseaddress(hal_eeprom_t id)
{
    uint32_t val = 0;
    return(val);
}


extern uint32_t hal_eeprom_get_totalsize(hal_eeprom_t id)
{
    uint32_t val = MYEEPROMDUMMYSIZE;
 
    return(val);
}


extern hal_bool_t hal_eeprom_address_is_valid(hal_eeprom_t id, uint32_t addr)
{
    return(hal_true);
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


#endif//HAL_USE_DEVICE_EEPROM_dummy

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



