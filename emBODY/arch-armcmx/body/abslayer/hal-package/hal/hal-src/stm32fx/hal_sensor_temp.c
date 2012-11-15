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

/* @file       hal_stm32_sensor_temp.c
	@brief      This file implements internals of the temperature sensor module.
	@author     marco.accame@iit.it
    @date       10/24/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_SENSOR_TEMP

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_base_hid.h" 
#include "hal_brdcfg.h"


#include "hal_trace.h"
#include "stdio.h"

//#include "hal_stm32xx_include.h"

#include "hal_utility_bits.h"

//#include "utils/hal_chip_st_l3g4200d.h"




 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_sensor_temp.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_sensor_temp_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_sensor_temp_sensor2index(t)              ((uint8_t)((t)))



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const hal_sensor_temp_cfg_t hal_sensor_temp_cfg_default  = 
{ 
    .dummy = 0 
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_sensor_temp_cfg_t       cfg;
    uint32_t                    initialvalue;
} hal_sensor_temp_info_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_sensor_temp_supported_is(hal_sensor_temp_t sensor);
static void s_hal_sensor_temp_initted_set(hal_sensor_temp_t sensor);
static hal_boolval_t s_hal_sensor_temp_initted_is(hal_sensor_temp_t sensor);

static hal_result_t s_hal_sensor_temp_hw_init(hal_sensor_temp_t sensor, const hal_sensor_temp_cfg_t *cfg);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_sensor_temp_initted[hal_sensor_temps_number] = { hal_false };

static hal_sensor_temp_info_t s_hal_sensor_temp_info[hal_sensor_temps_number] = { 0 };


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_sensor_temp_init(hal_sensor_temp_t sensor, const hal_sensor_temp_cfg_t *cfg)
{
//     hal_result_t res = hal_res_NOK_generic; // dont remove ...
//     hal_sensor_temp_info_t *info = NULL;

    if(hal_false == s_hal_sensor_temp_supported_is(sensor))
    {
        return(hal_res_NOK_generic);
    }
     
    if(NULL == cfg)
    {
        cfg  = &hal_sensor_temp_cfg_default;
    }
    
    if(hal_true == s_hal_sensor_temp_initted_is(sensor))
    {
        return(hal_res_OK);
    } 
 
    if(hal_res_OK != s_hal_sensor_temp_hw_init(sensor, cfg))
    {
        return(hal_res_NOK_generic);
    }
    
    s_hal_sensor_temp_initted_set(sensor);

    return(hal_res_OK);
}

extern hal_result_t hal_sensor_temp_read(hal_sensor_temp_t sensor, hal_sensor_temp_degree_t* degrees)
{
    hal_result_t res = hal_res_NOK_generic; 
//    hal_sensor_temp_info_t *info = NULL;
    int8_t data08 = 0;

    if(NULL == degrees)
    {
        return(hal_res_NOK_generic);
    }
    
    if(hal_false == s_hal_sensor_temp_initted_is(sensor))
    {
        *degrees = -128;
        return(hal_res_NOK_generic);
    }


    res = hal_brdcfg_sensor_temp__theconfig.devcfg[HAL_sensor_temp_sensor2index(sensor)].chipif.read(&data08);
    *degrees = (int16_t) data08;  
    
    return(res);    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern uint32_t hal_sensor_temp_hid_getsize(const hal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern hal_result_t hal_sensor_temp_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
//    if(NULL == memory)
//    {
//        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
//        return(hal_res_NOK_generic);
//    }


    memset(s_hal_sensor_temp_info, 0, sizeof(s_hal_sensor_temp_info));
    memset(s_hal_sensor_temp_initted, hal_false, sizeof(s_hal_sensor_temp_initted));
    return(hal_res_OK);  
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static hal_boolval_t s_hal_sensor_temp_supported_is(hal_sensor_temp_t sensor)
{
    return(hal_utility_bits_byte_bitcheck(hal_brdcfg_sensor_temp__theconfig.supported_mask, HAL_sensor_temp_sensor2index(sensor)) );
}

static void s_hal_sensor_temp_initted_set(hal_sensor_temp_t sensor)
{
    s_hal_sensor_temp_initted[HAL_sensor_temp_sensor2index(sensor)] = hal_true;
}

static hal_boolval_t s_hal_sensor_temp_initted_is(hal_sensor_temp_t sensor)
{
    return(s_hal_sensor_temp_initted[HAL_sensor_temp_sensor2index(sensor)]);
}


static hal_result_t s_hal_sensor_temp_hw_init(hal_sensor_temp_t sensor, const hal_sensor_temp_cfg_t *cfg)
{   
    if((NULL != hal_brdcfg_sensor_temp__theconfig.devcfg[HAL_sensor_temp_sensor2index(sensor)].chipif.init) && (NULL != hal_brdcfg_sensor_temp__theconfig.devcfg[HAL_sensor_temp_sensor2index(sensor)].chipif.read))
    {
        return(hal_brdcfg_sensor_temp__theconfig.devcfg[HAL_sensor_temp_sensor2index(sensor)].chipif.init(hal_brdcfg_sensor_temp__theconfig.devcfg[HAL_sensor_temp_sensor2index(sensor)].chipif.initpar));
    }
    else
    {
        return(hal_res_NOK_generic);
    }
}


#endif//HAL_USE_SENSOR_TEMP


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



