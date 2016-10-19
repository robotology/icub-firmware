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

/* @file       hal_gyroscope.c
	@brief      This file implements internals of the gyroscope module.
	@author     marco.accame@iit.it
    @date       10/24/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"
// - middleware interface: contains hl, stm32 etc. --------------------------------------------------------------------
//#include "hal_middleware_interface.h"

#if !defined(HAL_USE_GYROSCOPE)

#include "hal_gyroscope.h"

extern hal_bool_t hal_gyroscope_supported_is(hal_gyroscope_t id) { return hal_false; }
extern hal_result_t hal_gyroscope_init(hal_gyroscope_t id, const hal_gyroscope_cfg_t *cfg) { return hal_res_NOK_generic; }
extern hal_result_t hal_gyroscope_read(hal_gyroscope_t id, hal_gyroscope_angularrate_t* angrate) { return hal_res_NOK_generic; }
extern hal_result_t hal_gyroscope_readraw(hal_gyroscope_t id, int16_t *x, int16_t *y, int16_t *z) { return hal_res_NOK_generic; }

#endif


#ifdef HAL_USE_GYROSCOPE

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#include "hal_heap.h"

#include "hl_bits.h"

#include "hl_chip_st_l3g4200d.h"
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_gyroscope_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_gyroscope_id2index(t)              ((uint8_t)((t)))



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_gyroscope_cfg_t hal_gyroscope_cfg_default  = 
{ 
    .range = hal_gyroscope_range_250dps
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    hal_gyroscope_cfg_t           config;
} hal_gyroscope_internal_item_t;

typedef struct
{
    uint32_t                            inittedmask;
    hal_gyroscope_internal_item_t*      items[hal_gyroscopes_number];   
} hal_gyroscope_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_gyroscope_supported_is(hal_gyroscope_t id);
static void s_hal_gyroscope_initted_set(hal_gyroscope_t id);
static void s_hal_gyroscope_initted_clr(hal_gyroscope_t id);
static hal_bool_t s_hal_gyroscope_initted_is(hal_gyroscope_t id);

static hal_result_t s_hal_gyroscope_hw_deinit(hal_gyroscope_t id);
static hal_result_t s_hal_gyroscope_hw_init(hal_gyroscope_t id, const hal_gyroscope_cfg_t *cfg);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static hal_gyroscope_theinternals_t s_hal_gyroscope_theinternals =
{
    .inittedmask        = 0,
    .items              = { NULL }   
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hal_bool_t hal_gyroscope_supported_is(hal_gyroscope_t id)
{
    return(s_hal_gyroscope_supported_is(id));
}


extern hal_result_t hal_gyroscope_deinit(hal_gyroscope_t id)
{
    hal_gyroscope_internal_item_t *intitem = s_hal_gyroscope_theinternals.items[HAL_gyroscope_id2index(id)];

    if(hal_false == s_hal_gyroscope_supported_is(id))
    {
        return(hal_res_NOK_generic);
    }

    if(hal_false == s_hal_gyroscope_initted_is(id))
    {   // already deinitted
        return(hal_res_OK);
    }  
    
    if(NULL == intitem)
    {
        intitem = s_hal_gyroscope_theinternals.items[HAL_gyroscope_id2index(id)] = hal_heap_new(sizeof(hal_gyroscope_internal_item_t));
        // minimal initialisation of the internal item
        // nothing to init.      
    }  

    if(hal_res_OK != s_hal_gyroscope_hw_deinit(id))
    {
        return(hal_res_NOK_generic);
    }
    
    s_hal_gyroscope_initted_clr(id);

    return(hal_res_OK);    
    
}

extern hal_result_t hal_gyroscope_init(hal_gyroscope_t id, const hal_gyroscope_cfg_t *cfg)
{
    hal_gyroscope_internal_item_t *intitem = s_hal_gyroscope_theinternals.items[HAL_gyroscope_id2index(id)];

    if(hal_false == s_hal_gyroscope_supported_is(id))
    {
        return(hal_res_NOK_generic);
    }
     
    if(NULL == cfg)
    {
        cfg  = &hal_gyroscope_cfg_default;
    }

    if(hal_true == s_hal_gyroscope_initted_is(id))
    {
        if(0 == memcmp(cfg, &intitem->config, sizeof(hal_gyroscope_cfg_t)))
        {   // ok only if the previously used config is the same as the current one
            return(hal_res_OK);
        }
        else
        {
            return(hal_res_NOK_generic);
        }
    }  

    if(NULL == intitem)
    {
        intitem = s_hal_gyroscope_theinternals.items[HAL_gyroscope_id2index(id)] = hal_heap_new(sizeof(hal_gyroscope_internal_item_t));
        // minimal initialisation of the internal item
        // nothing to init.      
    }    
    memcpy(&intitem->config, cfg, sizeof(hal_gyroscope_cfg_t));
    
    
    if(hal_res_OK != s_hal_gyroscope_hw_init(id, cfg))
    {
        return(hal_res_NOK_generic);
    }
    
    s_hal_gyroscope_initted_set(id);

    return(hal_res_OK);
}


// static int32_t s_hal_gyroscope_convert(int32_t v)
// {
//     // the range is +-250 dps. it means that 32k is mapped into 250 -> 32k/250 = 131.072
//     // if i have 1 degree i must read 131.072 ...
//     // if i want to transform in milli-degree, i must read 131.072 / 1000 = 0.131072 = G
//     // to have the measure in milli-degree i must multiply the read for G.
//     // G = 8590 / (64*1024) = 0.131072998046875 .... i multiply and then i shift 16 times

//     uint8_t neg = (v < 0) ? (1) : (0);
//     int32_t r = (0 == neg) ? (8590*v) : (8590*(-v));
//     // now r is positive
//     r >>= 16;
//     r = (0 == neg) ? (r) : (-r);
//     
//     return(r);  
// }

extern hal_result_t hal_gyroscope_readraw(hal_gyroscope_t id, int16_t *x, int16_t *y, int16_t *z)
{
    hal_result_t res = hal_res_NOK_generic; 

#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)     
    if(hal_true != s_hal_gyroscope_initted_is(id))
    {
        return(hal_res_NOK_generic);
    } 
#endif
    
#if     !defined(HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)    
    if((NULL == x) || (NULL == y) || (NULL == z))
    {
        return(hal_res_NOK_generic);
    }
#endif
    
    *x = 0;
    *y = 0;
    *z = 0;
    int16_t xar = 0;
    int16_t yar = 0;
    int16_t zar = 0;
       
    res = hal_gyroscope__theboardconfig.driver[HAL_gyroscope_id2index(id)].fun.readraw(id, &xar, &yar, &zar);
    
    if(hal_res_OK == res)
    {
        *x = xar; //  factor is about 8.75 or 35/4
        *y = yar;
        *z = zar;        
    }
    
    return(res);
}
    

extern hal_result_t hal_gyroscope_read(hal_gyroscope_t id, hal_gyroscope_angularrate_t* angrate)
{
    hal_result_t res = hal_res_NOK_generic; 

#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)     
    if(hal_true != s_hal_gyroscope_initted_is(id))
    {
        return(hal_res_NOK_generic);
    } 
#endif
    
#if     !defined(HAL_BEH_REMOVE_RUNTIME_PARAMETER_CHECK)    
    if(NULL == angrate)
    {
        return(hal_res_NOK_generic);
    }
#endif
    
    angrate->xar = 0;
    angrate->yar = 0;
    angrate->zar = 0;
    int32_t xar = 0;
    int32_t yar = 0;
    int32_t zar = 0;
       
    res = hal_gyroscope__theboardconfig.driver[HAL_gyroscope_id2index(id)].fun.read(id, &xar, &yar, &zar);
    
    if(hal_res_OK == res)
    {
        angrate->xar = xar; //  factor is about 8.75 or 35/4
        angrate->yar = yar;
        angrate->zar = zar;        
    }
    
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


static hal_bool_t s_hal_gyroscope_supported_is(hal_gyroscope_t id)
{
    return((hal_bool_t)hl_bits_word_bitcheck(hal_gyroscope__theboardconfig.supportedmask, HAL_gyroscope_id2index(id)) );
}

static void s_hal_gyroscope_initted_set(hal_gyroscope_t id)
{
    hl_bits_word_bitset(&s_hal_gyroscope_theinternals.inittedmask, HAL_gyroscope_id2index(id));
}

static void s_hal_gyroscope_initted_clr(hal_gyroscope_t id)
{
    hl_bits_word_bitclear(&s_hal_gyroscope_theinternals.inittedmask, HAL_gyroscope_id2index(id));
}

static hal_bool_t s_hal_gyroscope_initted_is(hal_gyroscope_t id)
{
    return((hal_bool_t)hl_bits_word_bitcheck(s_hal_gyroscope_theinternals.inittedmask, HAL_gyroscope_id2index(id)));
}


static hal_result_t s_hal_gyroscope_hw_deinit(hal_gyroscope_t id)
{    

    if((NULL != hal_gyroscope__theboardconfig.driver[HAL_gyroscope_id2index(id)].fun.deinit))
    {
        return(hal_gyroscope__theboardconfig.driver[HAL_gyroscope_id2index(id)].fun.deinit(id));
    }
    else
    {
        return(hal_res_NOK_generic);
    }
}


static hal_result_t s_hal_gyroscope_hw_init(hal_gyroscope_t id, const hal_gyroscope_cfg_t *cfg)
{
    const hl_chip_st_l3g4200d_cfg_t *cc = (const hl_chip_st_l3g4200d_cfg_t *)hal_gyroscope__theboardconfig.driver[HAL_gyroscope_id2index(id)].cfg.initpar;
    
    hl_chip_st_l3g4200d_cfg_t config;
    config.i2cid = cc->i2cid;
    switch(cfg->range)
    {
        default: config.range = cc->range; break;
        case hal_gyroscope_range_250dps: config.range = hl_chip_st_l3g4200d_range_250dps; break;
        case hal_gyroscope_range_500dps: config.range = hl_chip_st_l3g4200d_range_500dps; break;
        case hal_gyroscope_range_2000dps: config.range = hl_chip_st_l3g4200d_range_2000dps; break;
    }
    
//    if((NULL != hal_gyroscope__theboardconfig.driver[HAL_gyroscope_id2index(id)].fun.init) && (NULL != hal_gyroscope__theboardconfig.driver[HAL_gyroscope_id2index(id)].fun.read))
//    {
//        return(hal_gyroscope__theboardconfig.driver[HAL_gyroscope_id2index(id)].fun.init(id, hal_gyroscope__theboardconfig.driver[HAL_gyroscope_id2index(id)].cfg.initpar));
//    }
    if((NULL != hal_gyroscope__theboardconfig.driver[HAL_gyroscope_id2index(id)].fun.init) && (NULL != hal_gyroscope__theboardconfig.driver[HAL_gyroscope_id2index(id)].fun.read))
    {
        return(hal_gyroscope__theboardconfig.driver[HAL_gyroscope_id2index(id)].fun.init(id, &config));
    }
    else
    {
        return(hal_res_NOK_generic);
    }
}


#endif//HAL_USE_GYROSCOPE


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



