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

/* @file        hl_dma.c
	@brief      This file contains internals for hl DMA utilities.
	@author     marco.accame@iit.it
    @date       02/12/2014
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------
#if     defined(HL_USE_UTIL_DMA)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "hl_core.h"
#include "hl_sys.h"

#include "hl_bits.h" 

#include "hl_arch.h"
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_dma.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_dma_hid.h" 

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HL_dma_id2index(p)                  ((uint8_t)((p)))

#define HL_dma_port2peripheral(p)           (s_hl_dma_stmDMAmap[HL_dma_id2index(p)] )

#define HL_dma_stream2peripheral(id, p)     (s_hl_dma_stream_stmDMAmap[(id+1)*HL_dma_id2index(p)] )

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hl_dma_cfg_t hl_dma_cfg_default =
{
    .stream         = hl_dma_stream0,  
    .advcfg         = NULL
};

const hl_dma_advcfg_t hl_dma_advcfg_default =
{   
   .DMA_Channel = DMA_CHANNEL0;  
   .DMA_PeripheralBaseAddr = NULL;
   .DMA_Memory0BaseAddr = NULL;
   .DMA_DIR = DMA_DIR_PeripheralToMemory;
   .DMA_BufferSize = 0;
   .DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   .DMA_MemoryInc = DMA_MemoryInc_Enable;
   .DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
   .DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
   .DMA_Mode = DMA_Mode_Circular;
   .DMA_Priority = DMA_Priority_High;
   .DMA_FIFOMode = DMA_FIFOMode_Enable;         
   .DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
   .DMA_MemoryBurst = DMA_MemoryBurst_Single;
   .DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hl_dma_cfg_t                config;
} hl_dma_internal_item_t;


typedef struct
{
    uint32_t                    inittedmask;
    hl_dma_internal_item_t*     items[hl_dmas_number];   
} hl_dma_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hl_boolval_t s_hl_dma_supported_is(hl_dma_t id);
static void s_hl_dma_initted_set(hl_dma_t id);
static void s_hl_dma_initted_reset(hl_dma_t id);
static hl_boolval_t s_hl_dma_initted_is(hl_dma_t id);


// hw related initialisation which may change with different versions of stm32fx mpus
static void s_hl_dma_hw_clock_init(hl_dma_t id);
static hl_result_t s_hl_dma_hw_registers_init(hl_dma_t id);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static DMA_TypeDef* const s_hl_dma_stmDMAmap[] = {DMA1, DMA2};
static DMA_Stream_TypeDef* const s_hl_dma_stream_ stmDMAmap[] = {DMA1_Stream0, DMA1_Stream1, DMA1_Stream2, DMA1_Stream3, DMA1_Stream4
                                                                 DMA1_Stream5, DMA1_Stream6, DMA1_Stream7, DMA2_Stream0, DMA2_Stream1,
                                                                 DMA2_Stream2, DMA2_Stream3, DMA2_Stream4, DMA2_Stream5, DMA2_Stream6,
                                                                 DMA2_Stream7};

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hl_dma_theinternals_t s_hl_dma_theinternals =
{
    .inittedmask        = 0,
    .items              = { NULL }   
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hl_result_t hl_dma_init(hl_dma_t id, const hl_dma_cfg_t *cfg)
{
    hl_dma_internal_item_t* intitem = s_hl_dma_theinternals.items[HL_dma_id2index(id)];

    if(hl_false == s_hl_dma_supported_is(id))
    {
        return(hl_res_NOK_generic);
    }
    
    if(hl_true == s_hl_dma_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }    

    if(NULL == cfg)
    {
        cfg  = &hl_dma_cfg_default;
    }
    
    // give memory to DMA internal item for this id ...   
    if(NULL == intitem)
    {
        // the internal item
        intitem = s_hl_dma_theinternals.items[HL_dma_id2index(id)] = hl_sys_heap_new(sizeof(hl_dma_internal_item_t));   
    }
    
    // set config
    memcpy(&intitem->config, cfg, sizeof(hl_dma_cfg_t));
                 
    // init clock
    s_hl_dma_hw_clock_init(id);
    
    // set the flag    
    s_hl_spi_initted_set(id);

    return(hl_res_OK);
}


extern hl_boolval_t hl_dma_supported_is(hl_dma_t id)
{
    return(s_hl_dma_supported_is(id));
}


extern hl_boolval_t hl_dma_initted_is(hl_dma_t id)
{
    return(s_hl_dma_initted_is(id));
}

extern hl_result_t hl_dma_enable(hl_dma_t id)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)
    if(hl_false == s_hl_spi_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }
#endif
    
    hl_result_t res = s_hl_dma_hw_registers_init(id); // hardware setup

    if(res != hl_res_OK)
    {
    	return(hl_res_NOK_generic);
    }
    
	return(hl_res_OK);
}


extern hl_result_t hl_dma_disable(hl_dma_t id, hl_dma_stream_t id) 
{
    DMA_Stream_TypeDef* DMA_Streamx = HL_dma_stream2peripheral(id, p); 

#if     !defined(HL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)    
    if(hl_false == s_hl_spi_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }
#endif
   
    DMA_DeInit(DMA_Streamx);

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

static hl_boolval_t s_hl_dma_supported_is(hl_dma_t id)
{
    if(NULL == hl_dma_map)
    {
        return(hl_false);
    }
    return(hl_bits_word_bitcheck(hl_dma_map->supportedmask, HL_dma_id2index(id)) );
}

static void s_hl_dma_initted_set(hl_dma_t id)
{
    hl_bits_word_bitset(&s_hl_dma_theinternals.inittedmask, HL_dma_id2index(id));
}

static void s_hl_dma_initted_reset(hl_dma_t id)
{
    hl_bits_word_bitclear(&s_hl_dma_theinternals.inittedmask, HL_dma_id2index(id));
}

static hl_boolval_t s_hl_dma_initted_is(hl_dma_t id)
{
    return(hl_bits_word_bitcheck(s_hl_dma_theinternals.inittedmask, HL_dma_id2index(id)));
}
static void s_hl_dma_hw_clock_init(hl_dma_t id)
{
    DMA_TypeDef* DMAx = HL_dma_port2peripheral(id);       
    switch(id)
    {
        case hl_dma1:
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_DMA1, ENABLE);       
        } break;

        case hl_dma2:
        {
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_DMA2, ENABLE);
        } break;  
    }
    
    //DMA_DeInit(dmax);    
}



static hl_result_t s_hl_dma_hw_registers_init(hl_dma_t id)
{  
    hl_dma_internal_item_t* intitem = s_hl_dma_theinternals.items[HL_dma_id2index(id)];
    hl_dma_cfg_t *cfg = &intitem->config;
    
    dma_TypeDef* dmax = HL_dma_port2peripheral(id);
    
    DMA_InitTypeDef* init2use = NULL;
        
    DMA_InitTypeDef SPI_InitStructure;

    if(NULL != cfg->advcfg)
    {
        // use advcfg. it has the same layout as DMA_InitTypeDef
        init2use = (DMA_InitTypeDef*) cfg->advcfg;      
    }
    else
    {
        // use SPI_InitStructure.
        init2use = &DMA_InitStructure;       
        
        // which i must init according to some cfg params and other constant choices        
        DMA_StructInit(init2use);      
        
        // copy the default configuration
        memcpy(init2use, &hl_dma_advcfg_default, sizeof(DMA_InitTypeDef));
        
        // and now change it         
        // apply only stream       
        //DO it here    
    }
    
    sadasdas
    // ok, we init the SPI
    
    DMA_DeInit(DMA_Streamx);

    DMA_Init(DMAx, init2use);
    
	return(hl_res_OK);
}
#endif//defined(HL_USE_UTIL_DMA)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






