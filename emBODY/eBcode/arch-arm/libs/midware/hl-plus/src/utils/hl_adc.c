/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Davide Pollarolo
 * email:   davide.pollarolo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license SPI be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

/* @file        hl_adc.c
	@brief      This file contains internals for hl ADC utilities.
	@author     marco.accame@iit.it
    @date       02/12/2014
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------
#include "hl_cfg_plus_modules.h"

#if     defined(HL_USE_UTIL_ADC)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "hl_core.h"
#include "hl_sys.h"
#include "hl_bits.h" 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_adc.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_adc_hid.h" 

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HL_adc_id2index(p)              ((uint8_t)((p)))

#define HL_adc2peripheral(p)            (s_hl_adc_stmadcmap[HL_adc_id2index(p)] )

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hl_adc_cfg_t hl_adc_cfg_default =
{
    .dummy          = 0,  
    .advcfg         = NULL
};

const hl_adc_advcfg_t hl_adc_advcfg_default =
{   
    .dummy          = 0
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hl_adc_cfg_t                config;
} hl_adc_internal_item_t;


typedef struct
{
    uint32_t                    initteadcsk;
    hl_adc_internal_item_t*     items[hl_adcs_number];   
} hl_adc_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hl_boolval_t s_hl_adc_supported_is(hl_adc_t id);
static void s_hl_adc_initted_set(hl_adc_t id);
static void s_hl_adc_initted_reset(hl_adc_t id);
static hl_boolval_t s_hl_adc_initted_is(hl_adc_t id);

// hw related initialisation which may change with different versions of stm32fx mpus
static void s_hl_adc_hw_clock_init(hl_adc_t id);
static hl_result_t s_hl_adc_hw_registers_init(hl_adc_t id);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static ADC_TypeDef* const s_hl_adc_stmadcmap[] = {ADC1, ADC2, ADC3};

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hl_adc_theinternals_t s_hl_adc_theinternals =
{
    .initteadcsk        = 0,
    .items              = { NULL }   
};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hl_result_t hl_adc_init(hl_adc_t id, const hl_adc_cfg_t *cfg)
{
    hl_adc_internal_item_t* intitem = s_hl_adc_theinternals.items[HL_adc_id2index(id)];

    if(hl_false == s_hl_adc_supported_is(id))
    {
        return(hl_res_NOK_generic);
    }
    
    if(hl_true == s_hl_adc_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }    

    if(NULL == cfg)
    {
        cfg  = &hl_adc_cfg_default;
    }
    
    // give memory to adc internal item for this id ...   
    if(NULL == intitem)
    {
        // the internal item
        intitem = s_hl_adc_theinternals.items[HL_adc_id2index(id)] = hl_sys_heap_new(sizeof(hl_adc_internal_item_t));   
    }
    
    // set config
    memcpy(&intitem->config, cfg, sizeof(hl_adc_cfg_t));
                 
    // init clock
    s_hl_adc_hw_clock_init(id);
    
    // set the flag    
    s_hl_adc_initted_set(id);

    return(hl_res_OK);
}


extern hl_boolval_t hl_adc_supported_is(hl_adc_t id)
{
    return(s_hl_adc_supported_is(id));
}


extern hl_boolval_t hl_adc_initted_is(hl_adc_t id)
{
    return(s_hl_adc_initted_is(id));
}

extern hl_result_t hl_adc_enable(hl_adc_t id)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)
    if(hl_false == s_hl_adc_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }
#endif
    
    hl_result_t res = s_hl_adc_hw_registers_init(id); // hardware setup

    if(res != hl_res_OK)
    {
    	return(hl_res_NOK_generic);
    }
    
	return(hl_res_OK);
}


extern hl_result_t hl_adc_disable(hl_adc_t id) 
{
    ADC_TypeDef* ADCx = HL_adc2peripheral(id); 

#if     !defined(HL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)    
    if(hl_false == s_hl_adc_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }
#endif
   
    //ADC_DeInit(ADCx);

    return(hl_res_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----


// ---- isr of the module: end ------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hl_boolval_t s_hl_adc_supported_is(hl_adc_t id)
{
    if(NULL == hl_adc_map)
    {
        return(hl_false);
    }
    return(hl_bits_word_bitcheck(hl_adc_map->supportedmask, HL_adc_id2index(id)) );
}

static void s_hl_adc_initted_set(hl_adc_t id)
{
    hl_bits_word_bitset(&s_hl_adc_theinternals.initteadcsk, HL_adc_id2index(id));
}

static void s_hl_adc_initted_reset(hl_adc_t id)
{
    hl_bits_word_bitclear(&s_hl_adc_theinternals.initteadcsk, HL_adc_id2index(id));
}

static hl_boolval_t s_hl_adc_initted_is(hl_adc_t id)
{
    return(hl_bits_word_bitcheck(s_hl_adc_theinternals.initteadcsk, HL_adc_id2index(id)));
}
static void s_hl_adc_hw_clock_init(hl_adc_t id)
{
    //ADC_DeInit();   
    ADC_TypeDef* ADCx = HL_adc2peripheral(id);       
    switch(id)
    {
        case hl_adc1:
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);       
        } break;

        case hl_adc2:
        {
            RCC_APB1PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
        } break;
        
        case hl_adc3:
        {
            RCC_APB1PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
        } break;    
    }
    
}



static hl_result_t s_hl_adc_hw_registers_init(hl_adc_t id)
{  
    hl_adc_internal_item_t* intitem = s_hl_adc_theinternals.items[HL_adc_id2index(id)];
    hl_adc_cfg_t *cfg = &intitem->config;
    
    ADC_TypeDef* ADCx = HL_adc2peripheral(id);
    
    ADC_InitTypeDef* init2use = NULL;
        
    ADC_InitTypeDef ADC_InitStructure;

    if(NULL != cfg->advcfg)
    {
        // use advcfg. it has the same layout as DMA_InitTypeDef
        init2use = (ADC_InitTypeDef*) cfg->advcfg;      
    }
    else
    {
        // use SPI_InitStructure.
        init2use = &ADC_InitStructure;       
        
        // which i must init according to some cfg params and other constant choices        
        ADC_StructInit(init2use);      
        
        // copy the default configuration
        memcpy(init2use, &hl_adc_advcfg_default, sizeof(ADC_InitTypeDef));
        
        // and now change it         
        // apply only stream       
        //DO it here    
    }
    
    // ok, we init the ADC
    
    //DMA_DeInit(DMA_Streamx);

    ADC_Init(ADCx, init2use);
    
	return(hl_res_OK);
}
#endif//defined(HL_USE_UTIL_ADC)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






