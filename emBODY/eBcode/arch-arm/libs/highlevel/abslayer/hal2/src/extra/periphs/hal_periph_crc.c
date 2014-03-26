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

/* @file       hal_periph_crc.c
	@brief      This file implements internal implementation of the hal crc module.
	@author     marco.accame@iit.it
    @date       09/16/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_CRC

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_base_hid.h" 
#include "hal_brdcfg.h"

#include "hal_heap.h"


#include "hl_bits.h" 

#if defined(HAL_USE_UTILITY_CRC07)
#include "hal_utility_crc07.h"
#endif

#if defined(HAL_USE_UTILITY_CRC16)
#include "hal_utility_crc16.h"
#endif

#if defined(HAL_USE_UTILITY_CRC32)
#include "hal_utility_crc32.h"
#endif


#include "hal_trace.h"
#include "stdio.h"

#include "hal_middleware_interface.h"



 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_crc.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_periph_crc_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_crc_id2index(t)              ((uint8_t)((t)))



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_crc_cfg_t hal_crc_cfg_default = 
{   
    .order      = hal_crc_order_32,
    .polynomial = 0x04C11DB7,
    .crctblram  = NULL
}; // the crc-32 polynomial used for eth

const uint32_t hal_crc_poly_crc07 = 0x00000009;
const uint32_t hal_crc_poly_crc16_ccitt = 0x00001021; 
const uint32_t hal_crc_poly_crc32 = 0x04C11DB7; 


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


typedef struct
{
    hal_crc_cfg_t       config;
    uint32_t            initialvalue;
} hal_crc_internal_item_t;

typedef struct
{
    uint8_t                     initted;
    hal_crc_internal_item_t*    items[hal_crcs_number];   
} hal_crc_theinternals_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_crc_supported_is(hal_crc_t id);
static void s_hal_crc_initted_set(hal_crc_t id);
static hal_boolval_t s_hal_crc_initted_is(hal_crc_t id);

static hal_result_t s_hal_crc_hw_init(hal_crc_internal_item_t *intitem);
static uint32_t s_hal_crc32_hw_compute(hal_crc_internal_item_t *intitem, const void *data, uint32_t size);
static uint32_t s_hal_crc32_sw_compute(hal_crc_internal_item_t *intitem, const void *data, uint32_t size);
static uint32_t s_hal_crc16_sw_compute(hal_crc_internal_item_t *intitem, const void *data, uint32_t size);
static uint32_t s_hal_crc07_sw_compute(hal_crc_internal_item_t *intitem, const void *data, uint32_t size);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section 

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_crc_theinternals_t s_hal_crc_theinternals =
{
    .initted            = 0,
    .items              = { NULL }   
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_crc_init(hal_crc_t id, const hal_crc_cfg_t *cfg)
{
    hal_result_t res = hal_res_NOK_generic; // dont remove ...
    //hal_crc_internals_t *intitem = s_hal_crc_internals[HAL_crc_id2index(id)];
    hal_crc_internal_item_t* intitem = s_hal_crc_theinternals.items[HAL_crc_id2index(id)];

    if(hal_false == s_hal_crc_supported_is(id))
    {
        return(hal_res_NOK_generic);
    }
     
    if(NULL == cfg)
    {
        cfg  = &hal_crc_cfg_default;
    }
    
    
    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        intitem = s_hal_crc_theinternals.items[HAL_crc_id2index(id)] = hal_heap_new(sizeof(hal_crc_internal_item_t));
        // minimal initialisation of the internal item
        // nothing to init.      
    }        


    memcpy(&intitem->config, cfg, sizeof(hal_crc_cfg_t));
    intitem->initialvalue = 0xffffffff;

    switch(intitem->config.order)
    {
        case hal_crc_order_07:
        {   // on arm always use sw emulation
            
#if defined(HAL_USE_UTILITY_CRC07)
            if(hal_crc_poly_crc07 == intitem->config.polynomial)
            {   // for crc07 use rom-ed table
                intitem->config.crctblram = (void*)hal_utility_crc07_table_0x09;
                res = hal_res_OK;
            } 
            else if(NULL != intitem->config.crctblram)
            {   // else compute it.
                //hal_utility_crc07_table_get(intitem->config.polynomial, intitem->config.crctblram);
                //res = hal_res_OK;
                // unfortunately the utility hal_utility_crc07_table_get() is not verified yet
                #warning WIP --> the function hal_utility_crc07_table_get() seems not to behave correctly, thus i dont use it.
                res = hal_res_NOK_generic;
            }
#else
            res = hal_res_NOK_generic;
#endif            
        } break;
        
        case hal_crc_order_16:
        {   // on arm always use sw emulation
#if defined(HAL_USE_UTILITY_CRC16)
            if(hal_crc_poly_crc16_ccitt == intitem->config.polynomial)
            {   // for crc16-ccitt use rom-ed table
                intitem->config.crctblram = (void*)hal_utility_crc16_table_0x1021;
                res = hal_res_OK;
            } 
            else if(NULL != intitem->config.crctblram)
            {   // else compute it.
                hal_utility_crc16_table_get(intitem->config.polynomial, intitem->config.crctblram);
                res = hal_res_OK;
            } 
#else
            res = hal_res_NOK_generic;
#endif   
        } break;

        case hal_crc_order_32:
        {   // on arm use sw emulation except for crc-32 

            if(hal_crc_poly_crc32 == intitem->config.polynomial)
            {   // use hw emulation
                res = s_hal_crc_hw_init(intitem);
            }
            else if(NULL != intitem->config.crctblram)
            {                
#if defined(HAL_USE_UTILITY_CRC32)
                hal_utility_crc32_table_get(intitem->config.polynomial, intitem->config.crctblram);
                res = hal_res_OK; 
#else
            res = hal_res_NOK_generic;
#endif                   
            }
        } break;

        default:
        {
            res = hal_res_NOK_generic;
        } break;
    }

    if(hal_res_OK != res)
    {
        return(res);
    }


    s_hal_crc_initted_set(id);

    return(hal_res_OK);
}

extern hal_result_t hal_crc_compute(hal_crc_t id, hal_crc_compute_mode_t mode, const void *data, uint32_t size, uint32_t *out)
{
    //hal_crc_internals_t *intitem = s_hal_crc_internals[HAL_crc_id2index(id)];
    hal_crc_internal_item_t* intitem = s_hal_crc_theinternals.items[HAL_crc_id2index(id)];
 
    if((NULL == data) || (NULL == out) || (0 == size))
    {
        return(hal_res_NOK_generic);
    }
    
    if(hal_false == s_hal_crc_initted_is(id))
    {
        *out = 0;
        return(hal_res_NOK_generic);
    }

    // do something 
    
    if(hal_crc_mode_clear == mode)
    {
        intitem->initialvalue = 0xffffffff;
    }

    // first the case of hw support
    if(hal_crc_poly_crc32 == intitem->config.polynomial)
    {
        *out = intitem->initialvalue = s_hal_crc32_hw_compute(intitem, data, size);
    }
    else if(hal_crc_order_16 == intitem->config.order)
    {
        *out = intitem->initialvalue = s_hal_crc16_sw_compute(intitem, data, size);
    }
    else if(hal_crc_order_32 == intitem->config.order)
    {
        *out = intitem->initialvalue = s_hal_crc32_sw_compute(intitem, data, size);
    }
    else if(hal_crc_order_07 == intitem->config.order)
    {
        *out = intitem->initialvalue = s_hal_crc07_sw_compute(intitem, data, size);
    }    
    else
    {
        *out = 0;
        return(hal_res_NOK_generic);
    }
           
    return(hal_res_OK);
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

static hal_boolval_t s_hal_crc_supported_is(hal_crc_t id)
{
    return((hal_boolval_t)hl_bits_byte_bitcheck(hal_brdcfg_crc__theconfig.supported_mask, HAL_crc_id2index(id)) );
}

static void s_hal_crc_initted_set(hal_crc_t id)
{
    hl_bits_byte_bitset(&s_hal_crc_theinternals.initted, HAL_crc_id2index(id));
}

static hal_boolval_t s_hal_crc_initted_is(hal_crc_t id)
{
    return((hal_boolval_t)hl_bits_byte_bitcheck(s_hal_crc_theinternals.initted, HAL_crc_id2index(id)));
}


static hal_result_t s_hal_crc_hw_init(hal_crc_internal_item_t *intitem)
{
    intitem->initialvalue = 0xffffffff;

#if     defined(HAL_USE_CPU_FAM_STM32F1)
    // nothing
#elif   defined(HAL_USE_CPU_FAM_STM32F4)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif    
    
    return(hal_res_OK);   
}


static uint32_t s_hal_crc32_hw_compute(hal_crc_internal_item_t *intitem, const void *data, uint32_t size)
{
    uint8_t tailsize = 0;

    if(0xffffffff == intitem->initialvalue)
    {
        CRC_ResetDR();
    }

    intitem->initialvalue = CRC_CalcBlockCRC_networkorder((uint32_t*)data, size/4);
   
    tailsize = size%4; 

    if(0 != (tailsize))
    {
        uint8_t tail[4];
        memset(tail, 0, 4);
        memcpy(tail, ((uint8_t*)data) + size-tailsize, tailsize);
        intitem->initialvalue = CRC_CalcBlockCRC((uint32_t*)tail, 1); 
    }

    return(intitem->initialvalue);
}

static uint32_t s_hal_crc32_sw_compute(hal_crc_internal_item_t *intitem, const void *data, uint32_t size)
{
#if defined(HAL_USE_UTILITY_CRC32)
     
    uint8_t tailsize = size%4;

    uint32_t size4 = size-tailsize;

    intitem->initialvalue = hal_utility_crc32_compute(intitem->config.polynomial, intitem->config.crctblram, intitem->initialvalue, (uint8_t*)data, size4);
   
    if(0 != (tailsize))
    {
        uint8_t tail[4];
        memset(tail, 0, 4);
        memcpy(tail, ((uint8_t*)data) + size-tailsize, tailsize);
        intitem->initialvalue = hal_utility_crc32_compute(intitem->config.polynomial, intitem->config.crctblram, intitem->initialvalue, tail, 4); 
    }

    return(intitem->initialvalue);

#else
    return(0);
#endif       
}


static uint32_t s_hal_crc16_sw_compute(hal_crc_internal_item_t *intitem, const void *data, uint32_t size)
{
#if defined(HAL_USE_UTILITY_CRC16)
    
    uint8_t tailsize = size%4;

    uint32_t size4 = size-tailsize;

    intitem->initialvalue = hal_utility_crc16_compute(intitem->config.polynomial, intitem->config.crctblram, intitem->initialvalue, (uint8_t*)data, size4);
   
    if(0 != (tailsize))
    {
        uint8_t tail[4];
        memset(tail, 0, 4);
        memcpy(tail, ((uint8_t*)data) + size-tailsize, tailsize);
        intitem->initialvalue = hal_utility_crc16_compute(intitem->config.polynomial, intitem->config.crctblram, intitem->initialvalue, tail, 4); 
    }

    return(intitem->initialvalue);
    
#else
    return(0);
#endif      
}


static uint32_t s_hal_crc07_sw_compute(hal_crc_internal_item_t *intitem, const void *data, uint32_t size)
{
#if defined(HAL_USE_UTILITY_CRC07)
    
    uint8_t tailsize = size%4;

    uint32_t size4 = size-tailsize;

    intitem->initialvalue = hal_utility_crc07_compute(intitem->config.polynomial, intitem->config.crctblram, intitem->initialvalue, (uint8_t*)data, size4);
   
    if(0 != (tailsize))
    {
        uint8_t tail[4];
        memset(tail, 0, 4);
        memcpy(tail, ((uint8_t*)data) + size-tailsize, tailsize);
        intitem->initialvalue = hal_utility_crc07_compute(intitem->config.polynomial, intitem->config.crctblram, intitem->initialvalue, tail, 4); 
    }

    return(intitem->initialvalue);
 
#else
    return(0);
#endif    
}

#endif//HAL_USE_CRC


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



