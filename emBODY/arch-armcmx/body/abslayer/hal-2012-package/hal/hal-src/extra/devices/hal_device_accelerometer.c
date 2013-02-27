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

/* @file       hal_device_accelerometer.c
	@brief      This file implements internals of the acceleration port module.
	@author     marco.accame@iit.it
    @date       10/26/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_DEVICE_ACCELEROMETER

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_base_hid.h" 
#include "hal_brdcfg.h"


#include "stdio.h"

//#include "hal_stm32xx_include.h"

#include "hal_utility_bits.h"

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_accelerometer.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_device_accelerometer_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_device_accelerometer_port2index(t)              ((uint8_t)((t)))



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hal_accelerometer_cfg_t hal_device_accelerometer_cfg_default  = 
{ 
    .dummy = 0 
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_accelerometer_cfg_t         cfg;
    uint32_t                        initialvalue;
} hal_device_accelerometer_info_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_accelerometer_supported_is(hal_accelerometer_port_t port);
static void s_hal_device_accelerometer_initted_set(hal_accelerometer_port_t port);
static hal_boolval_t s_hal_device_accelerometer_initted_is(hal_accelerometer_port_t port);

static hal_result_t s_hal_device_accelerometer_hw_init(hal_accelerometer_port_t port, const hal_accelerometer_cfg_t *cfg);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_accelerometer_initted[hal_accelerometer_ports_number] = { hal_false };

static hal_device_accelerometer_info_t s_hal_device_accelerometer_info[hal_accelerometer_ports_number] = { 0 };


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern hal_result_t hal_accelerometer_init(hal_accelerometer_port_t port, const hal_accelerometer_cfg_t *cfg)
{
//     hal_result_t res = hal_res_NOK_generic; // dont remove ...
//     hal_device_accelerometer_info_t *info = NULL;

    if(hal_false == s_hal_device_accelerometer_supported_is(port))
    {
        return(hal_res_NOK_generic);
    }
     
    if(NULL == cfg)
    {
        cfg  = &hal_device_accelerometer_cfg_default;
    }

    if(hal_true == s_hal_device_accelerometer_initted_is(port))
    {
        return(hal_res_OK);
    } 
    
    if(hal_res_OK != s_hal_device_accelerometer_hw_init(port, cfg))
    {
        return(hal_res_NOK_generic);
    }
    
    s_hal_device_accelerometer_initted_set(port);

    return(hal_res_OK);
}

// static int32_t s_hal_convert(int32_t v)
// {
//     // the range is +-2G. it means that 32k is mapped into 2000 mG -> 32k/2000 = 16.384
//     // if i have 1 mG i must read  16.384
//     // to have the measure in milli-G i must multiply the read for F = 1 / 16.384 = 0.06103515625
//     // F = 125 / (2*1024) = 0.06103515625 .... i multiply 125 and then i shift 11 times
//     
//     // if range is +-4G. 32k / 4000 = 8.192 ..... F = 0.1220703125 ....
//     // F = 125 / 1024 
//     
//     // if range is +-8g 
//     // F = 125 / 512
//     
//     // if range is +-16g
//     // F = 125 / 256

//     uint8_t neg = (v < 0) ? (1) : (0);
//     int32_t r = (0 == neg) ? (125*v) : (125*(-v));
//     // now r is positive
//     r >>= 11;
//     r = (0 == neg) ? (r) : (-r);
//     
//     return(r);  
// }
    


extern hal_result_t hal_accelerometer_read(hal_accelerometer_port_t port, hal_accelerometer_acceleration_t* acceler)
{
    hal_result_t res = hal_res_NOK_generic; 
 
    if(NULL == acceler)
    {
        return(hal_res_NOK_generic);
    }
    
    acceler->xac = 0;
    acceler->yac = 0;
    acceler->zac = 0;
    int32_t xac = 0;
    int32_t yac = 0;
    int32_t zac = 0;

    
    res = hal_brdcfg_device_accelerometer__theconfig.devcfg[HAL_device_accelerometer_port2index(port)].chipif.read(&xac, &yac, &zac);
    
    if(hal_res_OK == res)
    {
        acceler->xac = xac; 
        acceler->yac = yac;
        acceler->zac = zac;        
    }
    
    return(res);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern uint32_t hal_device_accelerometer_hid_getsize(const hal_base_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern hal_result_t hal_device_accelerometer_hid_setmem(const hal_base_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }


    memset(s_hal_device_accelerometer_info, 0, sizeof(s_hal_device_accelerometer_info));
    memset(s_hal_device_accelerometer_initted, hal_false, sizeof(s_hal_device_accelerometer_initted));
    return(hal_res_OK);  
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_device_accelerometer_supported_is(hal_accelerometer_port_t port)
{
    return(hal_utility_bits_byte_bitcheck(hal_brdcfg_device_accelerometer__theconfig.supported_mask, HAL_device_accelerometer_port2index(port)) );
}

static void s_hal_device_accelerometer_initted_set(hal_accelerometer_port_t port)
{
    s_hal_device_accelerometer_initted[HAL_device_accelerometer_port2index(port)] = hal_true;
}

static hal_boolval_t s_hal_device_accelerometer_initted_is(hal_accelerometer_port_t port)
{
    return(s_hal_device_accelerometer_initted[HAL_device_accelerometer_port2index(port)]);
}


static hal_result_t s_hal_device_accelerometer_hw_init(hal_accelerometer_port_t port, const hal_accelerometer_cfg_t *cfg)
{
    if((NULL != hal_brdcfg_device_accelerometer__theconfig.devcfg[HAL_device_accelerometer_port2index(port)].chipif.init) && (NULL != hal_brdcfg_device_accelerometer__theconfig.devcfg[HAL_device_accelerometer_port2index(port)].chipif.read))
    {
        return(hal_brdcfg_device_accelerometer__theconfig.devcfg[HAL_device_accelerometer_port2index(port)].chipif.init(hal_brdcfg_device_accelerometer__theconfig.devcfg[HAL_device_accelerometer_port2index(port)].chipif.initpar));
    }
    else
    {
        return(hal_res_NOK_generic);
    }
}


#endif//HAL_USE_SENSOR_ACCEL


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



