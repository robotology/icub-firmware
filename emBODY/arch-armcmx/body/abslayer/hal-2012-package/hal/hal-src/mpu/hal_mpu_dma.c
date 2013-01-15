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

/* @file       hal_mpu_dma.c
	@brief      This file implements the hal dma module.
	@author     marco.accame@iit.it
    @date       12/21/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_DMA

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"

#include "hal_mpu_stm32xx_include.h"

#include "string.h"

#include "hal_brdcfg.h"
#include "hal_base_hid.h" 
#include "hal_mpu_gpio_hid.h" 
#include "hal_mpu_sys_hid.h"
#include "hal_utility_fifo.h"
#include "hal_utility_bits.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "hal_dma.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_mpu_dma_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_dma_port2index(p)           ((uint8_t)((p)))
#define HAL_dma_index2port(i)           ((hal_dma_port_t)((i)))
#define HAL_dma_port2peripheral(p)      (s_hal_dma_memory_mapping_of_ports[HAL_dma_port2index(p)])


#if     defined(USE_STM32F1)

#elif    defined(USE_STM32F4)

#endif





// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_dma_cfg_t           cfg;
    DMA_InitTypeDef         stm32dmainit;
#if     defined(USE_STM32F1)    
    DMA_Channel_TypeDef*    stm32dmaperiph;
#elif   defined(USE_STM32F4)   
    tipodellostream*        stm32dmaperiph;
#endif    
//    void*                   source;
//    void*                   destin;
    hal_bool_t              enabled;
    hal_bool_t              stopit;
} hal_dma_port_datastructure_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_dma_supported_is(hal_dma_port_t port);
static void s_hal_dma_initted_set(hal_dma_port_t port);
static hal_boolval_t s_hal_dma_initted_is(hal_dma_port_t port);


// hw related initialisation whcih may change with different versions of stm32fx mpus
static void s_hal_dma_hw_nvic_init(hal_dma_port_t port);
static void s_hal_dma_isr_enable(hal_dma_port_t port);
static void s_hal_dma_isr_disable(hal_dma_port_t port);


static void s_hal_dma_isr_portx(hal_dma_port_t port);

static void s_hal_dma_isr_clear_flag(hal_dma_port_t port);


// enable or disab


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


#if     defined(USE_STM32F1)
static DMA_Channel_TypeDef* const s_hal_dma_memory_mapping_of_ports[hal_dma_ports_num] = 
{
    DMA1_Channel1, DMA1_Channel2, DMA1_Channel3, DMA1_Channel4, DMA1_Channel5, DMA1_Channel6, DMA1_Channel7,
    DMA2_Channel1, DMA2_Channel2, DMA2_Channel3, DMA2_Channel4, DMA2_Channel5,
    NULL, NULL, NULL, NULL
}; 

static const uint32_t s_hal_dma_periphclocks[hal_dma_ports_num] = 
{
    RCC_AHBPeriph_DMA1, RCC_AHBPeriph_DMA1, RCC_AHBPeriph_DMA1, RCC_AHBPeriph_DMA1, RCC_AHBPeriph_DMA1, RCC_AHBPeriph_DMA1, RCC_AHBPeriph_DMA1,
    RCC_AHBPeriph_DMA2, RCC_AHBPeriph_DMA2, RCC_AHBPeriph_DMA2, RCC_AHBPeriph_DMA2, RCC_AHBPeriph_DMA2,
    0, 0, 0, 0    
};

static const uint32_t s_hal_dma_irqflag_gl[hal_dma_ports_num] =
{
    DMA1_FLAG_GL1, DMA1_FLAG_GL2, DMA1_FLAG_GL3, DMA1_FLAG_GL4, DMA1_FLAG_GL5, DMA1_FLAG_GL6, DMA1_FLAG_GL7,
    DMA2_FLAG_GL1, DMA2_FLAG_GL2, DMA2_FLAG_GL3, DMA2_FLAG_GL4, DMA2_FLAG_GL5,   
    0, 0, 0, 0
};

static const IRQn_Type s_hal_dma_irqnumber[hal_dma_ports_num] =
{
    DMA1_Channel1_IRQn, DMA1_Channel2_IRQn, DMA1_Channel3_IRQn, DMA1_Channel4_IRQn, DMA1_Channel5_IRQn, DMA1_Channel6_IRQn, DMA1_Channel7_IRQn,
    DMA2_Channel1_IRQn, DMA2_Channel2_IRQn, DMA2_Channel3_IRQn, DMA2_Channel4_IRQn, DMA2_Channel5_IRQn,   
    UsageFault_IRQn, UsageFault_IRQn, UsageFault_IRQn, UsageFault_IRQn // use UsageFault_IRQn when there is no dma port available
};
#elif   defined(USE_STM32F4)
    #error --> define s_hal_dma_memory_mapping_of_ports[] for stm32f4
#endif



static hal_dma_port_datastructure_t *s_hal_dma_port_datastruct_ptr[hal_dma_ports_num] = {NULL};

static uint16_t s_hal_dma_initted = 0;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hal_result_t hal_dma_init(hal_dma_port_t port, const hal_dma_cfg_t *cfg)
{
	hal_dma_port_datastructure_t *dmaportdata = s_hal_dma_port_datastruct_ptr[HAL_dma_port2index(port)];

    if((hal_false == s_hal_dma_supported_is(port)) || (NULL == dmaportdata))
    {
        return(hal_res_NOK_generic);
    }

    if(NULL == cfg)
    {
        return(hal_res_NOK_generic);
    }

   
    // set config
    memcpy(&dmaportdata->cfg, cfg, sizeof(hal_dma_cfg_t));
    
    // set the stm32dma
    dmaportdata->stm32dmaperiph = HAL_dma_port2peripheral(port);
    
    
    // enable the peripheral clock
    RCC_AHBPeriphClockCmd(s_hal_dma_periphclocks[HAL_dma_port2index(port)], ENABLE);

    
    // set stm32dmainit
    dmaportdata->stm32dmainit.DMA_PeripheralBaseAddr    = (uint32_t)cfg->source;
    dmaportdata->stm32dmainit.DMA_MemoryBaseAddr        = (uint32_t)cfg->destin;
    dmaportdata->stm32dmainit.DMA_DIR                   = (hal_dma_transfer_per2mem == cfg->transfer) ? (DMA_DIR_PeripheralSRC) : (DMA_DIR_PeripheralDST);
    dmaportdata->stm32dmainit.DMA_BufferSize            = (uint32_t)cfg->datasize;
    dmaportdata->stm32dmainit.DMA_PeripheralInc         = (hal_dma_transfer_per2mem == cfg->transfer) ? (DMA_PeripheralInc_Disable) : (DMA_PeripheralInc_Disable);
    dmaportdata->stm32dmainit.DMA_MemoryInc             = (hal_dma_transfer_per2mem == cfg->transfer) ? (DMA_MemoryInc_Enable) : (DMA_MemoryInc_Enable);
    dmaportdata->stm32dmainit.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte; // decide granularity
    dmaportdata->stm32dmainit.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte; // decide granularity
    dmaportdata->stm32dmainit.DMA_Mode                  = DMA_Mode_Normal;
    dmaportdata->stm32dmainit.DMA_Priority              = DMA_Priority_VeryHigh; // or decide which priority is given
    dmaportdata->stm32dmainit.DMA_M2M                   = DMA_M2M_Disable; // or DMA_M2M_Enable in memory2memory
    
    DMA_DeInit(dmaportdata->stm32dmaperiph);
    DMA_Init(dmaportdata->stm32dmaperiph, &dmaportdata->stm32dmainit);
 
    dmaportdata->enabled = hal_false;
    dmaportdata->stopit = hal_false;

    s_hal_dma_initted_set(port);
    
    s_hal_dma_hw_nvic_init(port);

    return(hal_res_OK);
}


extern hal_result_t hal_dma_enable(hal_dma_port_t port)
{
	hal_dma_port_datastructure_t *dmaportdata = s_hal_dma_port_datastruct_ptr[HAL_dma_port2index(port)];

    if(hal_false == s_hal_dma_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }

    dmaportdata->enabled = hal_true;
         
    DMA_ITConfig(HAL_dma_port2peripheral(port), DMA_IT_TC, ENABLE);
    s_hal_dma_isr_enable(port);
    
    DMA_Cmd(dmaportdata->stm32dmaperiph, ENABLE);
    
	return(hal_res_OK);
}


extern hal_result_t hal_dma_disable(hal_dma_port_t port) 
{
	hal_dma_port_datastructure_t *dmaportdata = s_hal_dma_port_datastruct_ptr[HAL_dma_port2index(port)];

    if(hal_false == s_hal_dma_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }

    dmaportdata->enabled = hal_false;
    
    DMA_Cmd(dmaportdata->stm32dmaperiph, DISABLE);
    
    s_hal_dma_isr_disable(port);
    DMA_ITConfig(HAL_dma_port2peripheral(port), DMA_IT_TC, DISABLE);
    
    
	return(hal_res_OK);
}


extern hal_result_t hal_dma_retrigger(hal_dma_port_t port)
{
	hal_dma_port_datastructure_t *dmaportdata = s_hal_dma_port_datastruct_ptr[HAL_dma_port2index(port)];

    if(hal_false == s_hal_dma_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    dmaportdata->stopit = hal_false;
    
    DMA_Cmd(dmaportdata->stm32dmaperiph, DISABLE);
    dmaportdata->stm32dmaperiph->CNDTR  = dmaportdata->stm32dmainit.DMA_BufferSize;
    DMA_Cmd(dmaportdata->stm32dmaperiph, ENABLE);
    
    return(hal_res_OK);
}

extern hal_result_t hal_dma_dontdisable(hal_dma_port_t port)
{
	hal_dma_port_datastructure_t *dmaportdata = s_hal_dma_port_datastruct_ptr[HAL_dma_port2index(port)];

    if(hal_false == s_hal_dma_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    dmaportdata->stopit = hal_false;
        
    return(hal_res_OK);
}




extern hal_result_t hal_dma_source_set(hal_dma_port_t port, void* source)
{
	hal_dma_port_datastructure_t *dmaportdata = s_hal_dma_port_datastruct_ptr[HAL_dma_port2index(port)];

    if(hal_false == s_hal_dma_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    if(NULL != source)
    {
        dmaportdata->cfg.source                             = source;
        dmaportdata->stm32dmainit.DMA_PeripheralBaseAddr    = (uint32_t)dmaportdata->cfg.source;   
    }
    
    dmaportdata->stopit = hal_false;
    
    DMA_Cmd(dmaportdata->stm32dmaperiph, DISABLE);
    dmaportdata->stm32dmaperiph->CPAR   = dmaportdata->stm32dmainit.DMA_PeripheralBaseAddr;
    dmaportdata->stm32dmaperiph->CNDTR  = dmaportdata->stm32dmainit.DMA_BufferSize;
    DMA_Cmd(dmaportdata->stm32dmaperiph, ENABLE);
    
    return(hal_res_OK);
}


extern hal_result_t hal_dma_destin_set(hal_dma_port_t port, void* destin)
{
	hal_dma_port_datastructure_t *dmaportdata = s_hal_dma_port_datastruct_ptr[HAL_dma_port2index(port)];

    if(hal_false == s_hal_dma_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    if(NULL != destin)
    {
        dmaportdata->cfg.destin                         = destin;
        dmaportdata->stm32dmainit.DMA_MemoryBaseAddr    = (uint32_t)dmaportdata->cfg.destin;   
    }  

    dmaportdata->stopit = hal_false;    
    
    DMA_Cmd(dmaportdata->stm32dmaperiph, DISABLE);
    dmaportdata->stm32dmaperiph->CMAR   = dmaportdata->stm32dmainit.DMA_MemoryBaseAddr;
    dmaportdata->stm32dmaperiph->CNDTR  = dmaportdata->stm32dmainit.DMA_BufferSize;
    DMA_Cmd(dmaportdata->stm32dmaperiph, ENABLE);
    
    return(hal_res_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----

void DMA1_Channel1_IRQHandler(void)
{
    s_hal_dma_isr_portx(hal_dma_port1);
}

void DMA1_Channel2_IRQHandler(void)
{
    s_hal_dma_isr_portx(hal_dma_port2);  
}

void DMA1_Channel3_IRQHandler(void)
{
    s_hal_dma_isr_portx(hal_dma_port3);
}

void DMA1_Channel4_IRQHandler(void)
{
    s_hal_dma_isr_portx(hal_dma_port4);
}

void DMA1_Channel5_IRQHandler(void)
{
    s_hal_dma_isr_portx(hal_dma_port5);
}

void DMA1_Channel6_IRQHandler(void)
{
    s_hal_dma_isr_portx(hal_dma_port6);
}

void DMA1_Channel7_IRQHandler(void)
{
    s_hal_dma_isr_portx(hal_dma_port7);
}

void DMA2_Channel1_IRQHandler(void)
{
    s_hal_dma_isr_portx(hal_dma_port8);
}

void DMA2_Channel2_IRQHandler(void)
{
    s_hal_dma_isr_portx(hal_dma_port9);
}

void DMA2_Channel3_IRQHandler(void)
{
    s_hal_dma_isr_portx(hal_dma_port10);
}

void DMA2_Channel4_IRQHandler(void)
{
    s_hal_dma_isr_portx(hal_dma_port11);
}

void DMA2_Channel5_IRQHandler(void)
{
    s_hal_dma_isr_portx(hal_dma_port12);
}

// ---- isr of the module: end ------

 
extern uint32_t hal_dma_hid_getsize(const hal_cfg_t *cfg)
{
    uint32_t size = 0;
    uint8_t i;
    
    for(i=0; i<hal_dma_ports_num; i++)
    {
        if(hal_true == s_hal_dma_supported_is(HAL_dma_index2port(i)))
        {
            size += sizeof(hal_dma_port_datastructure_t);
        }
    }
    
    return(size);
}


extern hal_result_t hal_dma_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    uint8_t *ram08 = (uint8_t*)memory;
    uint8_t i;
    

    for(i=0; i<hal_dma_ports_num; i++)
    {
        if(hal_true == s_hal_dma_supported_is(HAL_dma_index2port(i)))
        {
            s_hal_dma_port_datastruct_ptr[i] = (hal_dma_port_datastructure_t*)ram08;
            ram08 += sizeof(hal_dma_port_datastructure_t);
        }
        else
        {
            s_hal_dma_port_datastruct_ptr[i] = NULL;
        }
    }
   
    return(hal_res_OK);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_dma_supported_is(hal_dma_port_t port)
{
    return(hal_utility_bits_halfword_bitcheck(hal_brdcfg_dma__theconfig.supported_mask, HAL_dma_port2index(port)));
}

static void s_hal_dma_initted_set(hal_dma_port_t port)
{
    hal_utility_bits_halfword_bitset(&s_hal_dma_initted, HAL_dma_port2index(port));
}

static hal_boolval_t s_hal_dma_initted_is(hal_dma_port_t port)
{
    return(hal_utility_bits_halfword_bitcheck(s_hal_dma_initted, HAL_dma_port2index(port)));
}


static void s_hal_dma_isr_portx(hal_dma_port_t port)
{   
    hal_dma_port_datastructure_t *dmaportdata = s_hal_dma_port_datastruct_ptr[HAL_dma_port2index(port)];
    
    s_hal_dma_isr_clear_flag(port);
   
    
    if(hal_dma_mode_oneshot == dmaportdata->cfg.mode)
    {
        dmaportdata->stopit = hal_true;
    }
    
    if(NULL != dmaportdata->cfg.cbk_on_transfer_done)
    {
        dmaportdata->cfg.cbk_on_transfer_done(dmaportdata->cfg.arg_on_transfer_done);
    }      
    

    if(hal_dma_mode_oneshot == dmaportdata->cfg.mode)
    {
        // if ... inside dmaportdata->cfg.cbk_on_transfer_done() we call any retrigger function, then  we dont disable
        if(hal_true == dmaportdata->stopit)
        {
            hal_dma_disable(port);
        }
        
        dmaportdata->stopit = hal_false;
    }  
       
}






static void s_hal_dma_hw_nvic_init(hal_dma_port_t port)
{
    hal_dma_port_datastructure_t *dmaportdata = s_hal_dma_port_datastruct_ptr[HAL_dma_port2index(port)];   
    IRQn_Type IRQn = s_hal_dma_irqnumber[HAL_dma_port2index(port)];


    if(hal_int_priorityNONE != dmaportdata->cfg.intpriority)
    {
        // enable rx irq in nvic
        hal_sys_irqn_priority_set(IRQn, dmaportdata->cfg.intpriority);
        hal_sys_irqn_disable(IRQn);
    }

}


static void s_hal_dma_isr_enable(hal_dma_port_t port)
{
    hal_dma_port_datastructure_t *dmaportdata = s_hal_dma_port_datastruct_ptr[HAL_dma_port2index(port)];
    IRQn_Type IRQn = s_hal_dma_irqnumber[HAL_dma_port2index(port)];
    if(hal_int_priorityNONE != dmaportdata->cfg.intpriority)
    {
        hal_sys_irqn_enable(IRQn);
    }
}

static void s_hal_dma_isr_disable(hal_dma_port_t port)
{
    hal_dma_port_datastructure_t *dmaportdata = s_hal_dma_port_datastruct_ptr[HAL_dma_port2index(port)];
    IRQn_Type IRQn = s_hal_dma_irqnumber[HAL_dma_port2index(port)];
    if(hal_int_priorityNONE != dmaportdata->cfg.intpriority)
    {
        hal_sys_irqn_disable(IRQn);
    }
}

static void s_hal_dma_isr_clear_flag(hal_dma_port_t port) 
{
    DMA_ClearFlag(s_hal_dma_irqflag_gl[HAL_dma_port2index(port)]);
}


#endif//HAL_USE_DMA

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


