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

/* @file       hal_timer.c
	@brief      This file implements internal implementation of the hal timer module.
	@author     marco.accame@iit.it
    @date       09/12/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_PERIPH_TIMER

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "string.h"
#include "stdlib.h"
#include "hal_base_hid.h" 
#include "hal_brdcfg.h"
#include "hl_bits.h" 
#include "hal_heap.h"


#include "hal_middleware_interface.h" 


 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_timer.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_periph_timer_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_timer_id2index(t)       ((uint8_t)((t)))


#if     defined(HAL_USE_CPU_FAM_STM32F1)
    #define TIM1_IRQn           TIM1_UP_IRQn
    #define TIM1_IRQHandler     TIM1_UP_IRQHandler
    #define TIM6_IRQn           TIM6_IRQn
    #define TIM6_IRQHandler     TIM6_IRQHandler
#elif   defined(HAL_USE_CPU_FAM_STM32F4)
    #define TIM1_IRQn           TIM1_UP_TIM10_IRQn
    #define TIM1_IRQHandler     TIM1_UP_TIM10_IRQHandler
    #define TIM6_IRQn           TIM6_DAC_IRQn
    #define TIM6_IRQHandler     TIM6_DAC_IRQHandler
    #define TIM8_IRQHandler     TIM8_UP_TIM13_IRQHandler
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// emty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef enum 
{
    hal_timer_refspeed_cpu          = 0,
    hal_timer_refspeed_fastbus      = 1,
    hal_timer_refspeed_slowbus      = 2,
    hal_timer_refspeed_none         = 255,
} hal_timer_reference_speed_bus_t;


#if     defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)    
typedef enum
{
    hal_timer_stm32_periphclock_apb1    = 1,
    hal_timer_stm32_periphclock_apb2    = 2,
    hal_timer_stm32_periphclock_none    = 255
} hal_timer_stm32_periphclock_t;
#else //defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 

typedef struct
{
    hal_timer_reference_speed_bus_t referencespeedbus;
#if     defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)    
    hal_timer_stm32_periphclock_t   periphclock;
    TIM_TypeDef*                    TIMx;
    uint32_t                        RCC_APBxPeriph_TIMx;
    IRQn_Type                       TIMx_IRQn;
#else //defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif    
} hal_timer_cpufamily_regs_t;



typedef struct
{
    hal_timer_cfg_t             config;     // fixed for every architecture
    hal_timer_status_t          status;     // fixed for every architecture
    uint16_t                    period;     // fixed for every architecture
    uint16_t                    prescaler;  // fixed for every architecture
    uint32_t                    tick_ns;    
} hal_timer_internal_item_t;


typedef struct
{
    hal_timer_internal_item_t*    items[hal_timers_number];   
} hal_timer_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_timer_supported_is(hal_timer_t id);
static void s_hal_timer_initted_set(hal_timer_t id);
static hal_bool_t s_hal_timer_initted_is(hal_timer_t id);

static void s_hal_timer_status_set(hal_timer_t id, hal_timer_status_t status);
static hal_timer_status_t s_hal_timer_status_get(hal_timer_t id);

static hal_result_t s_hal_timer_prepare(hal_timer_t id, const hal_timer_cfg_t *cfg);

static void s_hal_timer_cpufamily_start(hal_timer_t id);
static void s_hal_timer_cpufamily_stop(hal_timer_t id);



static hal_reltime_t s_hal_timer_get_period(hal_timer_t id);

static void s_hal_timer_callback(hal_timer_t id);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

#if     defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)
static const hal_timer_cpufamily_regs_t s_hal_timer_cpufamily_regs[hal_timers_number] =
{
    {   // timer1         
        .referencespeedbus      = hal_timer_refspeed_cpu, 
        .periphclock            = hal_timer_stm32_periphclock_apb2,        
        .TIMx                   = TIM1,
        .RCC_APBxPeriph_TIMx    = RCC_APB2Periph_TIM1, 
        .TIMx_IRQn              = TIM1_IRQn      
    },
    {   // timer2  
        .referencespeedbus      = hal_timer_refspeed_fastbus,
        .periphclock            = hal_timer_stm32_periphclock_apb1,
        .TIMx                   = TIM2,
        .RCC_APBxPeriph_TIMx    = RCC_APB1Periph_TIM2, 
        .TIMx_IRQn              = TIM2_IRQn     
    },
    {   // timer3 
        .referencespeedbus      = hal_timer_refspeed_fastbus, 
        .periphclock            = hal_timer_stm32_periphclock_apb1,        
        .TIMx                   = TIM3,
        .RCC_APBxPeriph_TIMx    = RCC_APB1Periph_TIM3, 
        .TIMx_IRQn              = TIM3_IRQn 
    },
    {   // timer4   
        .referencespeedbus      = hal_timer_refspeed_fastbus,
        .periphclock            = hal_timer_stm32_periphclock_apb1,  
        .TIMx                   = TIM4,
        .RCC_APBxPeriph_TIMx    = RCC_APB1Periph_TIM4, 
        .TIMx_IRQn              = TIM4_IRQn
    },
    {   // timer5   
        .referencespeedbus      = hal_timer_refspeed_fastbus,
        .periphclock            = hal_timer_stm32_periphclock_apb1,
        .TIMx                   = TIM5,
        .RCC_APBxPeriph_TIMx    = RCC_APB1Periph_TIM5, 
        .TIMx_IRQn              = TIM5_IRQn 
    },
    {   // timer6          
        .referencespeedbus      = hal_timer_refspeed_fastbus,
        .periphclock            = hal_timer_stm32_periphclock_apb1,
        .TIMx                   = TIM6,
        .RCC_APBxPeriph_TIMx    = RCC_APB1Periph_TIM6, 
        .TIMx_IRQn              = TIM6_IRQn         
    },
    {   // timer7   
        .referencespeedbus      = hal_timer_refspeed_fastbus,
        .periphclock            = hal_timer_stm32_periphclock_apb1,
        .TIMx                   = TIM7,
        .RCC_APBxPeriph_TIMx    = RCC_APB1Periph_TIM7, 
        .TIMx_IRQn              = TIM7_IRQn 
    },
    {   // timer8  
#if     defined(HAL_USE_CPU_NAM_STM32F107)        
        .referencespeedbus      = hal_timer_refspeed_none,
        .periphclock            = hal_timer_stm32_periphclock_none,
        .TIMx                   = NULL,
        .RCC_APBxPeriph_TIMx    = 0, 
        .TIMx_IRQn              = TIM7_IRQn  // we use this value just to remove a warning raised by the compiler
#elif   defined(HAL_USE_CPU_NAM_STM32F407)
        .referencespeedbus      = hal_timer_refspeed_cpu,
        .periphclock            = hal_timer_stm32_periphclock_apb2,
        .TIMx                   = TIM8,
        .RCC_APBxPeriph_TIMx    = RCC_APB2Periph_TIM8, 
        .TIMx_IRQn              = TIM8_UP_TIM13_IRQn 
#else //defined(HAL_USE_CPU_NAM_STM32F407)
    #error ERR --> choose a HAL_USE_CPU_NAM_*
#endif
    },
    {   // timer9    
        .referencespeedbus      = hal_timer_refspeed_none,
        .periphclock            = hal_timer_stm32_periphclock_none,
        .TIMx                   = NULL,
        .RCC_APBxPeriph_TIMx    = 0, 
        .TIMx_IRQn              = TIM7_IRQn  // we use this value just to remove a warning raised by the compiler
    }, 
    {   // timer10    
        .referencespeedbus      = hal_timer_refspeed_none,
        .periphclock            = hal_timer_stm32_periphclock_none,
        .TIMx                   = NULL,
        .RCC_APBxPeriph_TIMx    = 0, 
        .TIMx_IRQn              = TIM7_IRQn  // we use this value just to remove a warning raised by the compiler
    },  
    {   // timer11    
        .referencespeedbus      = hal_timer_refspeed_none,
        .periphclock            = hal_timer_stm32_periphclock_none,
        .TIMx                   = NULL,
        .RCC_APBxPeriph_TIMx    = 0, 
        .TIMx_IRQn              = TIM7_IRQn  // we use this value just to remove a warning raised by the compiler
    },
    {   // timer12    
        .referencespeedbus      = hal_timer_refspeed_none,
        .periphclock            = hal_timer_stm32_periphclock_none,
        .TIMx                   = NULL,
        .RCC_APBxPeriph_TIMx    = 0, 
        .TIMx_IRQn              = TIM7_IRQn  // we use this value just to remove a warning raised by the compiler
    }     
};
#else //defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of variables
// --------------------------------------------------------------------------------------------------------------------


static hal_timer_theinternals_t s_hal_timer_theinternals =
{
    .items              = { NULL }   
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_timer_init(hal_timer_t id, const hal_timer_cfg_t *cfg, hal_reltime_t *error)
{
    hal_timer_internal_item_t *intitem = s_hal_timer_theinternals.items[HAL_timer_id2index(id)];    

    if(hal_false == s_hal_timer_supported_is(id))
    {
        return(hal_res_NOK_generic);
    }

    if(NULL == cfg)
    {
        return(hal_res_NOK_nullpointer);
    }

    if(0 == cfg->countdown)
    {
        return(hal_res_NOK_wrongparam);
    }

    if((hal_timer_mode_oneshot == cfg->mode) && (hal_int_priorityNONE == cfg->priority))
    {
        return(hal_res_NOK_wrongparam);
    }

    if(hal_timer_prescalerAUTO != cfg->prescaler)
    {
        return(hal_res_NOK_wrongparam);
    }
    
    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        intitem = s_hal_timer_theinternals.items[HAL_timer_id2index(id)] = hal_heap_new(sizeof(hal_timer_internal_item_t));
        // minimal initialisation of the internal item
        // initialise at least the status.
        intitem->status = hal_timer_status_none;        
    }    
     
    // if it is running, then stop it.
    if(hal_timer_status_running == hal_timer_status_get(id))
    {
        hal_timer_stop(id);
    }

    // marker: save the cfg and init registers

    // computes the values to be put in registers and compute the error in microsec
    s_hal_timer_prepare(id, cfg);
    

    // marker: calculate error
    if(NULL != error)
    {
        hal_reltime_t period = s_hal_timer_get_period(id);
        if(period > cfg->countdown)
        {
            *error = period - cfg->countdown; 
        }
        else
        {
            *error = cfg->countdown - period;
        }
    }
    
    s_hal_timer_initted_set(id);

    return(hal_res_OK);
}


extern hal_result_t hal_timer_start(hal_timer_t id)
{
    if(hal_false == s_hal_timer_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    if(hal_timer_status_running == s_hal_timer_status_get(id))
    {
        hal_timer_stop(id);
    }

    s_hal_timer_status_set(id, hal_timer_status_running);
    s_hal_timer_cpufamily_start(id); 

    return(hal_res_OK);
}



extern hal_result_t hal_timer_stop(hal_timer_t id)
{
    if(hal_false == s_hal_timer_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    if(hal_timer_status_idle == s_hal_timer_status_get(id))
    {
        return(hal_res_OK);
    }
    // marker: stop and set status

    s_hal_timer_cpufamily_stop(id);
    s_hal_timer_status_set(id, hal_timer_status_idle);

    return(hal_res_OK);
}



extern hal_result_t hal_timer_countdown_set(hal_timer_t id, hal_reltime_t countdown, hal_reltime_t *error)
{                                                                            
    hal_timer_cfg_t *curcfg = NULL;
    hal_timer_cfg_t newcfg;
    uint8_t wasrunning = 0;

    if(hal_false == s_hal_timer_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    // if running stop.
    if(hal_timer_status_running == hal_timer_status_get(id))
    {
        wasrunning = 1;
    }

    // computes the values to be put in registers
    curcfg = &s_hal_timer_theinternals.items[HAL_timer_id2index(id)]->config;
    memcpy(&newcfg, curcfg, sizeof(hal_timer_cfg_t));
    newcfg.countdown = countdown;

    hal_timer_init(id, &newcfg, error);

    if(1 == wasrunning)
    {
        hal_timer_start(id);
    }

    return(hal_res_OK);
}


extern hal_result_t hal_timer_priority_set(hal_timer_t id, hal_interrupt_priority_t prio)
{
    if(hal_false == s_hal_timer_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    // do something 

    return(hal_res_NOK_unsupported);
}


extern hal_result_t hal_timer_interrupt_enable(hal_timer_t id)
{
    if(hal_false == s_hal_timer_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    // do something 

    return(hal_res_NOK_unsupported);
}



extern hal_result_t hal_timer_interrupt_disable(hal_timer_t id)
{
    if(hal_false == s_hal_timer_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    // do something 

    return(hal_res_NOK_unsupported);
}



extern hal_result_t hal_timer_remainingtime_get(hal_timer_t id, hal_reltime_t *remaining_time)
{
    hal_timer_internal_item_t *intitem = s_hal_timer_theinternals.items[HAL_timer_id2index(id)];
    TIM_TypeDef* TIMx               = s_hal_timer_cpufamily_regs[HAL_timer_id2index(id)].TIMx;

    if(hal_false == s_hal_timer_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    *remaining_time = intitem->tick_ns * TIM_GetCounter(TIMx) / 1000;

    return(hal_res_OK);
}


extern hal_result_t hal_timer_offset_write(hal_timer_t id, hal_nanotime_t offset)
{
    if(hal_false == s_hal_timer_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }

    // do something 

    // not supported
    return(hal_res_NOK_unsupported);
}


extern hal_timer_status_t hal_timer_status_get(hal_timer_t id)
{

    if(hal_false == s_hal_timer_initted_is(id))
    {
        return(hal_timer_status_none);
    }

    return(s_hal_timer_status_get(id));
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----

#if     defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)

void TIM1_IRQHandler(void)
{
    // Clear TIMx update interrupt 
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    s_hal_timer_callback(hal_timer1);
}

void TIM2_IRQHandler(void)
{
    // Clear TIMx update interrupt 
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    s_hal_timer_callback(hal_timer2);
}


void TIM3_IRQHandler(void)
{
    // Clear TIMx update interrupt 
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    s_hal_timer_callback(hal_timer3);
}


void TIM4_IRQHandler(void)
{
    // Clear TIMx update interrupt 
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    s_hal_timer_callback(hal_timer4);
}

void TIM5_IRQHandler(void)
{
    // Clear TIMx update interrupt 
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    s_hal_timer_callback(hal_timer5);
}

void TIM6_IRQHandler(void)
{
    // Clear TIMx update interrupt 
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    s_hal_timer_callback(hal_timer6);
}

void TIM7_IRQHandler(void)
{
    // Clear TIMx update interrupt 
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    s_hal_timer_callback(hal_timer7);
}

void TIM8_IRQHandler(void)
{
    // Clear TIMx update interrupt 
    TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
    s_hal_timer_callback(hal_timer8);
}

#else //defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif

// ---- isr of the module: end ------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_timer_supported_is(hal_timer_t id)
{
    if(hal_timer_refspeed_none == s_hal_timer_cpufamily_regs[HAL_timer_id2index(id)].referencespeedbus)
    {
            return(hal_false);
    }
    
    return((hal_boolval_t)hl_bits_hlfword_bitcheck(hal_brdcfg_timer__theconfig.supported_mask, HAL_timer_id2index(id)));
}

static void s_hal_timer_initted_set(hal_timer_t id)
{
    s_hal_timer_status_set(id, hal_timer_status_idle);
}

static hal_bool_t s_hal_timer_initted_is(hal_timer_t id)
{
    return((hal_timer_status_none != s_hal_timer_status_get(id)) ? (hal_true) : (hal_false));
}

static void s_hal_timer_status_set(hal_timer_t id, hal_timer_status_t status)
{
    s_hal_timer_theinternals.items[HAL_timer_id2index(id)]->status = status;
}

static hal_timer_status_t s_hal_timer_status_get(hal_timer_t id)
{
    hal_timer_internal_item_t *intitem = s_hal_timer_theinternals.items[HAL_timer_id2index(id)];
    return( (NULL == intitem) ? (hal_timer_status_none) : (intitem->status) );
}


static hal_result_t s_hal_timer_prepare(hal_timer_t id, const hal_timer_cfg_t *cfg)
{
    hal_timer_internal_item_t *intitem = s_hal_timer_theinternals.items[HAL_timer_id2index(id)];
    // we use SystemCoreClock instead of hal_brdcfg_cpu__theconfig.speeds.fastbus, which should be the same because ...
    //volatile uint32_t referencespeed = hal_brdcfg_cpu__theconfig.speeds.fastbus;  
    hal_timer_reference_speed_bus_t referencespeedbus = s_hal_timer_cpufamily_regs[HAL_timer_id2index(id)].referencespeedbus;   
    volatile uint32_t referencespeed = 0; 

    switch(referencespeedbus)
    {
        case hal_timer_refspeed_slowbus:    { referencespeed = hal_brdcfg_cpu__theconfig.speeds.slowbus;    } break;
        case hal_timer_refspeed_fastbus:    { referencespeed = hal_brdcfg_cpu__theconfig.speeds.fastbus;    } break;
        case hal_timer_refspeed_cpu:        { referencespeed = hal_brdcfg_cpu__theconfig.speeds.cpu;        } break;
        default:                            { referencespeed = 0;                                           } break;
    } 
    
    if(0 == referencespeed)
    {
        return(hal_res_NOK_unsupported);
    }

    
    memcpy(&intitem->config, cfg, sizeof(hal_timer_cfg_t));

    // use prescaler = ((referencespeed/a/1000) )

    if(0 == (intitem->config.countdown % 1000))
    {   // multiple of 1 ms: use 10 khz, thus a = 10. 1 tick is 100us, max countdown is 6400 msec = 6.4 s

        if(intitem->config.countdown > 64000*100) // tick is 100
        {
            intitem->config.countdown = 64000*100; // tick is 100
        }

        intitem->prescaler   = ((referencespeed/10/1000) );  // a is 10. the value is 7200 (for stm332f1): ok, lower than 65k
        intitem->period      = intitem->config.countdown / 100; // tick is 100
        intitem->tick_ns     = 100*1000; // tick is 100

    }
    else if(0 == (intitem->config.countdown % 100))
    {   // multiple of 100 us: use 100 khz, thus a = 100. 1 tick is 10us, max countdown is 640 msec
        
        if(intitem->config.countdown > 64000*10) // tick is 10
        {
            intitem->config.countdown = 64000*10; // tick is 10
        }

        intitem->prescaler   = ((referencespeed/100/1000) );  // a is 100. the value is 720: ok, lower than 65k
        intitem->period      = intitem->config.countdown / 10; // tick is 10
        intitem->tick_ns     = 10*1000; // tick is 10
    }
    else if(0 == (intitem->config.countdown % 10))
    {   // multiple of 10 us: use 1000 khz, thus a = 1000. 1 tick is 1us, max countdown is 64 msec
        
        if(intitem->config.countdown > 64000*1) // tick is 1
        {
            intitem->config.countdown = 64000*1; // tick is 1
        }

        intitem->prescaler   = ((referencespeed/1000/1000) );  // a is 1000. the value is 72: ok, lower than 65k
        intitem->period      = intitem->config.countdown / 1; // tick is 1
        intitem->tick_ns     = 1*1000; // tick is 1
    }
    else
    {   // multiple of 1 us: use 8000 khz, thus a = 8000. 1 tick is 0.125us, max countdown is 8 msec
        
        if(intitem->config.countdown > 8000) // tick is 0.125
        {
            intitem->config.countdown = 8000; // tick is 0.125
        }

        intitem->prescaler   = ((referencespeed/8000/1000) );  // a is 8000. the value is 9: ok, lower than 65k
        intitem->period      = intitem->config.countdown * 8; // tick is 0.125
        intitem->tick_ns     = 125; // tick is 0.125 micro
    }
    
    return(hal_res_OK);

}



static void s_hal_timer_cpufamily_start(hal_timer_t id)
{
#if     defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)
    
    hal_timer_internal_item_t *intitem = s_hal_timer_theinternals.items[HAL_timer_id2index(id)];
    TIM_TypeDef* TIMx               = s_hal_timer_cpufamily_regs[HAL_timer_id2index(id)].TIMx;
    uint32_t RCC_APBxPeriph_TIMx    = s_hal_timer_cpufamily_regs[HAL_timer_id2index(id)].RCC_APBxPeriph_TIMx;
    IRQn_Type TIMx_IRQn             = s_hal_timer_cpufamily_regs[HAL_timer_id2index(id)].TIMx_IRQn;
    hal_timer_stm32_periphclock_t periphclock = s_hal_timer_cpufamily_regs[HAL_timer_id2index(id)].periphclock;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   // NVIC_InitTypeDef NVIC_InitStructure;
   


    // Enable TIMx clock 
    if(hal_timer_stm32_periphclock_apb1 == periphclock)
    {
        RCC_APB1PeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE);
    }
    else if(hal_timer_stm32_periphclock_apb2 == periphclock)
    {
        RCC_APB2PeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE);
    }

    // registers of TIMx

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = intitem->period - 1;          
    TIM_TimeBaseStructure.TIM_Prescaler = intitem->prescaler - 1; 
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);


    //* enable counter 
    TIM_Cmd(TIMx, ENABLE);

    // Immediate load of  Prescaler value */
    TIM_PrescalerConfig(TIMx, intitem->prescaler - 1, TIM_PSCReloadMode_Immediate);
    //TIM_PrescalerConfig(TIMx, intitem->prescaler - 1, TIM_PSCReloadMode_Update);
    

    // Clear  update pending flag */
    TIM_ClearFlag(TIMx, TIM_FLAG_Update);

    if(hal_int_priorityNONE != intitem->config.priority)
    {
        // Enable TIM2 Update interrupt */
        TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
    
        // enable irqs in nvic
        hal_sys_irqn_priority_set(TIMx_IRQn, intitem->config.priority);
        hal_sys_irqn_enable(TIMx_IRQn);
    }
    
#else //defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif    
}

static void s_hal_timer_cpufamily_stop(hal_timer_t id)
{
#if     defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)
    
    hal_timer_internal_item_t *intitem = s_hal_timer_theinternals.items[HAL_timer_id2index(id)];
//    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TypeDef* TIMx               = s_hal_timer_cpufamily_regs[HAL_timer_id2index(id)].TIMx;
//    uint32_t RCC_APBxPeriph_TIMx    = s_hal_timer_cpufamily_regs[HAL_timer_id2index(id)].RCC_APBxPeriph_TIMx;
    IRQn_Type TIMx_IRQn             = s_hal_timer_cpufamily_regs[HAL_timer_id2index(id)].TIMx_IRQn;
//    hal_timer_reference_speed_bus_t referencespeedbus = s_hal_timer_cpufamily_regs[HAL_timer_id2index(id)].referencespeedbus;
  
    
#define TIMER_USE_IT
#if     defined(TIMER_USE_IT)

    TIM_DeInit(TIMx);
    TIM_Cmd(TIMx, DISABLE);

    if(hal_int_priorityNONE != intitem->config.priority)
    {
        TIM_ITConfig(TIMx, TIM_IT_Update, DISABLE);
        hal_sys_irqn_disable(TIMx_IRQn);
    }

#else

    // disable TIMx clock 
    if(hal_timer_stm32_periphclock_apb1 == periphclock)
    {
        RCC_APB1PeriphClockCmd(RCC_APBxPeriph_TIMx, DISABLE);
    }
    else if(hal_timer_stm32_periphclock_apb2 == periphclock)
    {
        RCC_APB2PeriphClockCmd(RCC_APBxPeriph_TIMx, DISABLE);
    }

    // register of TIMx
 
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);


    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);


    //* disable counter 
    TIM_Cmd(TIMx, DISABLE);

    // Immediate load of  Precaler value */
    TIM_PrescalerConfig(TIMx, intitem->prescaler - 1, TIM_PSCReloadMode_Immediate);

    // Clear  update pending flag */
    TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
//    TIM_ClearFlag(TIMx, TIM_FLAG_Update);

    // Enable TIM2 Update interrupt */
    TIM_ITConfig(TIMx, TIM_IT_Update, DISABLE);

    // nvic
    #warning --> acemor removed it to maintain 16 priorities 
    /* Configure two bits for preemption priority */
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
    /* Enable the TIM2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIMx_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);

#endif//defined(TIMER_USE_IT)


#else //defined(HAL_USE_CPU_FAM_STM32F1) || defined(HAL_USE_CPU_FAM_STM32F4)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif
}



//
//
//
//static void s_TIM_Configuration(void)
//{ 
//  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
////  TIM_OCInitTypeDef  TIM_OCInitStructure;
//
//    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
//
////    prescaler = ((SystemCoreClock/a/1000) - 1)
////    per granualrita' 1000 us -> vado a 2khz quindi uso a = 2   max 32000 msec
////    per               100 us ->       20khz            a = 20  max  3200 ms
////                       10 us ->      200khz            a = 200 max   320 ms
////                          1 us         2000khz           a = 2000 max 32 ms
////
////                       oppure 1000 us -> 10khz quindi max 6.4 sec
//
//  /* TIM2 configuration */
//  TIM_TimeBaseStructure.TIM_Period = 2000 - 1;          
////  TIM_TimeBaseStructure.TIM_Prescaler = 36000 - 1;//((SystemCoreClock/1200) - 1);  
////  TIM_TimeBaseStructure.TIM_Prescaler = TIM_TimeBaseStructure.TIM_Prescaler;     
//  TIM_TimeBaseStructure.TIM_Prescaler = ((SystemCoreClock/2/1000) - 1); 
//  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    
//  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  
//  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;
//  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//
//  //TIM_SetClockDivision(TIM2, TIM_CKD_DIV1);
//  
////  TIM_OCStructInit(&TIM_OCInitStructure);
////  /* Output Compare Timing Mode configuration: Channel1 */
////  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
////  TIM_OCInitStructure.TIM_Pulse = 0x0;  
////  TIM_OC1Init(TIM2, &TIM_OCInitStructure);
////  
////  /* TIM3 configuration */
////  TIM_TimeBaseStructure.TIM_Period = 0x95F;
////          
////  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
////  /* Output Compare Timing Mode configuration: Channel1 */
////  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
////  
////  /* TIM4 configuration */
////  TIM_TimeBaseStructure.TIM_Period = 0xE0F;  
////        
////  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
////  /* Output Compare Timing Mode configuration: Channel1 */
////  TIM_OC1Init(TIM4, &TIM_OCInitStructure);
//
//  /* TIM2 enable counter */
//  TIM_Cmd(TIM2, ENABLE);
////  /* TIM3 enable counter */
////  TIM_Cmd(TIM3, ENABLE);
////  /* TIM4 enable counter */
////  TIM_Cmd(TIM4, ENABLE);
//
//  /* Immediate load of TIM2 Precaler value */
////  TIM_PrescalerConfig(TIM2, ((SystemCoreClock/2/1000) - 1), TIM_PSCReloadMode_Immediate);
////  /* Immediate load of TIM3 Precaler value */  
////  TIM_PrescalerConfig(TIM3, ((SystemCoreClock/1200) - 1), TIM_PSCReloadMode_Immediate);
////  /* Immediate load of TIM4 Precaler value */
////  TIM_PrescalerConfig(TIM4, ((SystemCoreClock/1200) - 1), TIM_PSCReloadMode_Immediate);
//
//  /* Clear TIM2 update pending flag */
//  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
////  /* Clear TIM3 update pending flag */
////  TIM_ClearFlag(TIM3, TIM_FLAG_Update);
////  /* Clear TIM4 update pending flag */
////  TIM_ClearFlag(TIM4, TIM_FLAG_Update);
//
//  /* Enable TIM2 Update interrupt */
//  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
////  /* Enable TIM3 Update interrupt */
////  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
////  /* Enable TIM4 Update interrupt */
////  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
//}

static hal_reltime_t s_hal_timer_get_period(hal_timer_t id)
{
    hal_timer_internal_item_t *intitem = s_hal_timer_theinternals.items[HAL_timer_id2index(id)];

    return(intitem->config.countdown);
}


static void s_hal_timer_callback(hal_timer_t id)
{
    hal_timer_internal_item_t *intitem = s_hal_timer_theinternals.items[HAL_timer_id2index(id)];
    
    if(hal_timer_mode_oneshot == intitem->config.mode)
    {
        // stop timer 
        s_hal_timer_cpufamily_stop(id);
              
        s_hal_timer_status_set(id, hal_timer_status_expired);
    }

    if(NULL != intitem->config.callback_on_exp)
    {
        intitem->config.callback_on_exp(intitem->config.arg);
    }
}


#endif//HAL_USE_PERIPH_TIMER

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



