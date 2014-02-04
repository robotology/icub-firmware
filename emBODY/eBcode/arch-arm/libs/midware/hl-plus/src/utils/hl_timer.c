/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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


/* @file       hl_timer.c
	@brief      This file contains internal implementation of the hl timer utilities.
	@author     marco.accame@iit.it
    @date       12/20/2013
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------

#include "hl_cfg_plus_modules.h"


#if     defined(HL_USE_UTIL_TIMER)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "hl_core.h"        // contains the required stm32f10x_*.h or stm32f4xx*.h header files

#include "hl_sys.h"

#include "hl_bits.h" 

#include "hl_arch.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_timer.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_timer_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#if     defined(HL_USE_MPU_NAME_STM32F103RB)
    #define HL_TIMER_INTERNAL_HAS_TIMER01
    #define HL_TIMER_INTERNAL_HAS_TIMER02
    #define HL_TIMER_INTERNAL_HAS_TIMER03
    #define HL_TIMER_INTERNAL_HAS_TIMER04
#elif   defined(HL_USE_MPU_NAME_STM32F107VC)
    #define HL_TIMER_INTERNAL_HAS_TIMER01
    #define HL_TIMER_INTERNAL_HAS_TIMER02
    #define HL_TIMER_INTERNAL_HAS_TIMER03
    #define HL_TIMER_INTERNAL_HAS_TIMER04
    #define HL_TIMER_INTERNAL_HAS_TIMER05
    #define HL_TIMER_INTERNAL_HAS_TIMER06
    #define HL_TIMER_INTERNAL_HAS_TIMER07
#elif   defined(HL_USE_MPU_NAME_STM32F407IG)
    #define HL_TIMER_INTERNAL_HAS_TIMER01
    #define HL_TIMER_INTERNAL_HAS_TIMER02
    #define HL_TIMER_INTERNAL_HAS_TIMER03
    #define HL_TIMER_INTERNAL_HAS_TIMER04
    #define HL_TIMER_INTERNAL_HAS_TIMER05
    #define HL_TIMER_INTERNAL_HAS_TIMER06
    #define HL_TIMER_INTERNAL_HAS_TIMER07
    #define HL_TIMER_INTERNAL_HAS_TIMER08
#else
    #error ERR: unrecognised MPU_NAME
#endif


#define HL_timer_id2index(t)       ((uint8_t)((t)))


#if     defined(HL_USE_MPU_ARCH_STM32F1)
    #define TIM1_IRQn           TIM1_UP_IRQn
    #define TIM1_IRQHandler     TIM1_UP_IRQHandler
    #define TIM6_IRQn           TIM6_IRQn
    #define TIM6_IRQHandler     TIM6_IRQHandler
#elif   defined(HL_USE_MPU_ARCH_STM32F4)
    #define TIM1_IRQn           TIM1_UP_TIM10_IRQn
    #define TIM1_IRQHandler     TIM1_UP_TIM10_IRQHandler
    #define TIM6_IRQn           TIM6_DAC_IRQn
    #define TIM6_IRQHandler     TIM6_DAC_IRQHandler
    #define TIM8_IRQn           TIM8_UP_TIM13_IRQn
    #define TIM8_IRQHandler     TIM8_UP_TIM13_IRQHandler
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERR --> choose a HL_USE_MPU_ARCH_*
#endif 


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

const hl_timer_cfg_t hl_timer_cfg_default = 
{ 
    0
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef enum 
{
    hl_timer_refspeed_cpu           = 0,
    hl_timer_refspeed_fastbus       = 1,
    hl_timer_refspeed_slowbus       = 2,
    hl_timer_refspeed_none          = 255,
} hl_timer_reference_speed_bus_t;


typedef enum
{
    hl_timer_stm32_periphclock_apb1    = 1,
    hl_timer_stm32_periphclock_apb2    = 2,
    hl_timer_stm32_periphclock_none    = 255
} hl_timer_stm32_periphclock_t;


typedef struct
{
    hl_timer_reference_speed_bus_t  referencespeedbus;
    hl_timer_stm32_periphclock_t    periphclock;
    int8_t                          TIMx_IRQn;
    uint8_t                         dummy;
    TIM_TypeDef*                    TIMx;
    uint32_t                        RCC_APBxPeriph_TIMx;
} hl_timer_stm32_properties_t;      //1+1+1+1+4+4= 12


typedef struct
{
    hl_timer_cfg_t              config;     
    hl_timer_status_t           status;     
    uint16_t                    period;     
    uint16_t                    prescaler;  
    uint32_t                    tick_ns;    
} hl_timer_internal_item_t;


typedef struct
{
    uint16_t                        initted;
    hl_timer_internal_item_t*       items[hl_timers_number];   
} hl_timer_theinternals_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static hl_boolval_t s_hl_timer_supported_is(hl_timer_t id);
static void s_hl_timer_initted_set(hl_timer_t id);
static hl_boolval_t s_hl_timer_initted_is(hl_timer_t id);
static void s_hl_timer_status_set(hl_timer_t id, hl_timer_status_t status);
static hl_timer_status_t s_hl_timer_status_get(hl_timer_t id);
static hl_result_t s_hl_timer_prepare(hl_timer_t id, const hl_timer_cfg_t *cfg);
static void s_hl_timer_stm32_start(hl_timer_t id);
static void s_hl_timer_stm32_stop(hl_timer_t id);
static hl_reltime_t s_hl_timer_get_period(hl_timer_t id);
static void s_hl_timer_callback(hl_timer_t id);




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

// map of all timer properties


#if     defined(HL_TIMER_INTERNAL_HAS_TIMER01)
static const hl_timer_stm32_properties_t s_hl_timer_props_t01 =
{
    .referencespeedbus      = hl_timer_refspeed_cpu,
    .periphclock            = hl_timer_stm32_periphclock_apb2,
    .TIMx_IRQn              = TIM1_IRQn,
    .dummy                  = 0,
    .TIMx                   = TIM1,
    .RCC_APBxPeriph_TIMx    = RCC_APB2Periph_TIM1
};
#endif

#if     defined(HL_TIMER_INTERNAL_HAS_TIMER02)
static const hl_timer_stm32_properties_t s_hl_timer_props_t02 =
{
    .referencespeedbus      = hl_timer_refspeed_fastbus,
    .periphclock            = hl_timer_stm32_periphclock_apb1,
    .TIMx_IRQn              = TIM2_IRQn,
    .dummy                  = 0,
    .TIMx                   = TIM2,
    .RCC_APBxPeriph_TIMx    = RCC_APB1Periph_TIM2
};
#endif

#if     defined(HL_TIMER_INTERNAL_HAS_TIMER03)
static const hl_timer_stm32_properties_t s_hl_timer_props_t03 =
{
    .referencespeedbus      = hl_timer_refspeed_fastbus,
    .periphclock            = hl_timer_stm32_periphclock_apb1,
    .TIMx_IRQn              = TIM3_IRQn,
    .dummy                  = 0,
    .TIMx                   = TIM3,
    .RCC_APBxPeriph_TIMx    = RCC_APB1Periph_TIM3
};
#endif

#if     defined(HL_TIMER_INTERNAL_HAS_TIMER04)
static const hl_timer_stm32_properties_t s_hl_timer_props_t04 =
{
    .referencespeedbus      = hl_timer_refspeed_fastbus,
    .periphclock            = hl_timer_stm32_periphclock_apb1,
    .TIMx_IRQn              = TIM4_IRQn,
    .dummy                  = 0,
    .TIMx                   = TIM4,
    .RCC_APBxPeriph_TIMx    = RCC_APB1Periph_TIM4
};
#endif

#if     defined(HL_TIMER_INTERNAL_HAS_TIMER05)
static const hl_timer_stm32_properties_t s_hl_timer_props_t05 =
{
    .referencespeedbus      = hl_timer_refspeed_fastbus,
    .periphclock            = hl_timer_stm32_periphclock_apb1,
    .TIMx_IRQn              = TIM5_IRQn,
    .dummy                  = 0,
    .TIMx                   = TIM5,
    .RCC_APBxPeriph_TIMx    = RCC_APB1Periph_TIM5
};
#endif

#if     defined(HL_TIMER_INTERNAL_HAS_TIMER06)
static const hl_timer_stm32_properties_t s_hl_timer_props_t06 =
{
    .referencespeedbus      = hl_timer_refspeed_fastbus,
    .periphclock            = hl_timer_stm32_periphclock_apb1,
    .TIMx_IRQn              = TIM6_IRQn,
    .dummy                  = 0,
    .TIMx                   = TIM6,
    .RCC_APBxPeriph_TIMx    = RCC_APB1Periph_TIM6
};
#endif

#if     defined(HL_TIMER_INTERNAL_HAS_TIMER07)
static const hl_timer_stm32_properties_t s_hl_timer_props_t07 =
{
    .referencespeedbus      = hl_timer_refspeed_fastbus,
    .periphclock            = hl_timer_stm32_periphclock_apb1,
    .TIMx_IRQn              = TIM7_IRQn,
    .dummy                  = 0,
    .TIMx                   = TIM7,
    .RCC_APBxPeriph_TIMx    = RCC_APB1Periph_TIM7
};
#endif

#if     defined(HL_TIMER_INTERNAL_HAS_TIMER08)
static const hl_timer_stm32_properties_t s_hl_timer_props_t08 =
{
    .referencespeedbus      = hl_timer_refspeed_cpu,
    .periphclock            = hl_timer_stm32_periphclock_apb2,
    .TIMx_IRQn              = TIM8_IRQn,
    .dummy                  = 0,
    .TIMx                   = TIM8,
    .RCC_APBxPeriph_TIMx    = RCC_APB2Periph_TIM8
};
#endif



static const hl_timer_stm32_properties_t * s_hl_timer_stm32_props[hl_timers_number] =
{
#if     defined(HL_TIMER_INTERNAL_HAS_TIMER01)
    &s_hl_timer_props_t01,
#else
    NULL,
#endif
#if     defined(HL_TIMER_INTERNAL_HAS_TIMER02)
    &s_hl_timer_props_t02,
#else
    NULL,
#endif
#if     defined(HL_TIMER_INTERNAL_HAS_TIMER03)
    &s_hl_timer_props_t03,
#else
    NULL,
#endif
#if     defined(HL_TIMER_INTERNAL_HAS_TIMER04)
    &s_hl_timer_props_t04,
#else
    NULL,
#endif
#if     defined(HL_TIMER_INTERNAL_HAS_TIMER05)
    &s_hl_timer_props_t05,
#else
    NULL,
#endif
#if     defined(HL_TIMER_INTERNAL_HAS_TIMER06)
    &s_hl_timer_props_t06,
#else
    NULL,
#endif
#if     defined(HL_TIMER_INTERNAL_HAS_TIMER07)
    &s_hl_timer_props_t07,
#else
    NULL,
#endif
#if     defined(HL_TIMER_INTERNAL_HAS_TIMER08)
    &s_hl_timer_props_t08,
#else
    NULL,
#endif
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hl_timer_theinternals_t s_hl_timer_theinternals = 
{
    .initted        = 0,
    .items          = { NULL }    
};


    
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hl_result_t hl_timer_init(hl_timer_t id, const hl_timer_cfg_t *cfg, hl_reltime_t *error)
{
    hl_timer_internal_item_t *intitem = s_hl_timer_theinternals.items[HL_timer_id2index(id)];    

    if(hl_false == s_hl_timer_supported_is(id))
    {
        return(hl_res_NOK_generic);
    }

    if(NULL == cfg)
    {
        return(hl_res_NOK_nullpointer);
    }

    if(0 == cfg->countdown)
    {
        return(hl_res_NOK_generic);
    }

    if((hl_timer_mode_oneshot == cfg->mode) && (hl_irqpriorityNONE == cfg->priority))
    {
        return(hl_res_NOK_generic);
    }
    
    
    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        intitem = s_hl_timer_theinternals.items[HL_timer_id2index(id)] = hl_sys_heap_new(sizeof(hl_timer_internal_item_t));
        // minimal initialisation of the internal item
        // initialise at least the status.
        intitem->status = hl_timer_status_none;        
    }    
     
    // if it is running, then stop it.
    if(hl_timer_status_running == hl_timer_status_get(id))
    {
        hl_timer_stop(id);
    }

    // compute the values to be put in registers
    s_hl_timer_prepare(id, cfg);
    

    // calculate error
    if(NULL != error)
    {
        if(NULL != cfg->advcfg)
        {
            *error = hl_NA32;
        }
        else
        {
            hl_reltime_t period = s_hl_timer_get_period(id);
            if(period > cfg->countdown)
            {
                *error = period - cfg->countdown; 
            }
            else
            {
                *error = cfg->countdown - period;
            }
        }
    }
    
    s_hl_timer_initted_set(id);

    return(hl_res_OK);
}


extern hl_bool_t hl_timer_initted_is(hl_timer_t id)
{
    return(s_hl_timer_initted_is(id));
}


extern hl_result_t hl_timer_start(hl_timer_t id)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)    
    if(hl_false == s_hl_timer_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }
#endif//!defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK) 

    if(hl_timer_status_running == s_hl_timer_status_get(id))
    {
        hl_timer_stop(id);
    }

    s_hl_timer_status_set(id, hl_timer_status_running);
    s_hl_timer_stm32_start(id); 

    return(hl_res_OK);
}



extern hl_result_t hl_timer_stop(hl_timer_t id)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK) 
    if(hl_false == s_hl_timer_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }
#endif//!defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK) 

    if(hl_timer_status_idle == s_hl_timer_status_get(id))
    {
        return(hl_res_OK);
    }


    s_hl_timer_stm32_stop(id);
    s_hl_timer_status_set(id, hl_timer_status_idle);

    return(hl_res_OK);
}

extern hl_timer_status_t hl_timer_status_get(hl_timer_t id)
{

    if(hl_false == s_hl_timer_initted_is(id))
    {
        return(hl_timer_status_none);
    }

    return(s_hl_timer_status_get(id));
}


extern hl_result_t hl_timer_remainingtime_get(hl_timer_t id, hl_reltime_t *remaining_time)
{
    hl_timer_internal_item_t *intitem = s_hl_timer_theinternals.items[HL_timer_id2index(id)];
    const hl_timer_stm32_properties_t* props = s_hl_timer_stm32_props[HL_timer_id2index(id)];

#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)    
    if(NULL == props)
    {
        return(hl_res_NOK_unsupported);
    }

    if(NULL == remaining_time)
    {
        return(hl_res_NOK_nullpointer);
    }    
#endif//!defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)
    
    if(hl_false == s_hl_timer_initted_is(id))
    {
        return(hl_res_NOK_generic);
    }
    
    if(NULL != intitem->config.advcfg)
    {
        *remaining_time = hl_NA32;
        return(hl_res_NOK_generic);
    }

    *remaining_time = intitem->tick_ns * TIM_GetCounter(props->TIMx) / 1000;

    return(hl_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----

#if     defined(HL_TIMER_INTERNAL_HAS_TIMER01) &&  !defined(HL_CFG_UTIL_TIMER_DONTUSE_TIMER01)
// use the isr only if there is in the mpu and if the user does not prefer using it for another purpose 
void TIM1_IRQHandler(void)
{
    // Clear TIMx update interrupt 
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    s_hl_timer_callback(hl_timer1);
}
#endif

#if     defined(HL_TIMER_INTERNAL_HAS_TIMER02) &&  !defined(HL_CFG_UTIL_TIMER_DONTUSE_TIMER02)
// use the isr only if there is in the mpu and if the user does not prefer using it for another purpose 
void TIM2_IRQHandler(void)
{
    // Clear TIMx update interrupt 
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    s_hl_timer_callback(hl_timer2);
}
#endif

#if     defined(HL_TIMER_INTERNAL_HAS_TIMER03) &&  !defined(HL_CFG_UTIL_TIMER_DONTUSE_TIMER03)
// use the isr only if there is in the mpu and if the user does not prefer using it for another purpose 
void TIM3_IRQHandler(void)
{
    // Clear TIMx update interrupt 
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    s_hl_timer_callback(hl_timer3);
}
#endif

#if     defined(HL_TIMER_INTERNAL_HAS_TIMER04) &&  !defined(HL_CFG_UTIL_TIMER_DONTUSE_TIMER04)
// use the isr only if there is in the mpu and if the user does not prefer using it for another purpose 
void TIM4_IRQHandler(void)
{
    // Clear TIMx update interrupt 
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    s_hl_timer_callback(hl_timer4);
}
#endif

#if     defined(HL_TIMER_INTERNAL_HAS_TIMER05) &&  !defined(HL_CFG_UTIL_TIMER_DONTUSE_TIMER05)
// use the isr only if there is in the mpu and if the user does not prefer using it for another purpose 
void TIM5_IRQHandler(void)
{
    // Clear TIMx update interrupt 
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    s_hl_timer_callback(hl_timer5);
}
#endif

#if     defined(HL_TIMER_INTERNAL_HAS_TIMER06) &&  !defined(HL_CFG_UTIL_TIMER_DONTUSE_TIMER06)
// use the isr only if there is in the mpu and if the user does not prefer using it for another purpose 
void TIM6_IRQHandler(void)
{
    // Clear TIMx update interrupt 
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    s_hl_timer_callback(hl_timer6);
}
#endif

#if     defined(HL_TIMER_INTERNAL_HAS_TIMER07) &&  !defined(HL_CFG_UTIL_TIMER_DONTUSE_TIMER07)
// use the isr only if there is in the mpu and if the user does not prefer using it for another purpose 
void TIM7_IRQHandler(void)
{
    // Clear TIMx update interrupt 
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    s_hl_timer_callback(hl_timer7);
}
#endif

#if     defined(HL_TIMER_INTERNAL_HAS_TIMER08) &&  !defined(HL_CFG_UTIL_TIMER_DONTUSE_TIMER08)
// use the isr only if there is in the mpu and if the user does not prefer using it for another purpose 
void TIM8_IRQHandler(void)
{ 
    // Clear TIMx update interrupt 
    if(SET == TIM_GetFlagStatus(TIM8, TIM_FLAG_Update))
    {
        TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
        s_hl_timer_callback(hl_timer8);
    }   
}
#endif



// ---- isr of the module: end ------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static hl_boolval_t s_hl_timer_supported_is(hl_timer_t id)
{
    if(NULL == s_hl_timer_stm32_props[HL_timer_id2index(id)])
    {
        return(hl_false);
    }
    if(NULL == hl_timer_map)
    {
        return(hl_false);
    }
    return(hl_bits_hlfword_bitcheck(hl_timer_map->supported_mask, HL_timer_id2index(id)) );
}

static void s_hl_timer_initted_set(hl_timer_t id)
{
    hl_bits_hlfword_bitset(&s_hl_timer_theinternals.initted, HL_timer_id2index(id));
}

static hl_boolval_t s_hl_timer_initted_is(hl_timer_t id)
{
    return(hl_bits_hlfword_bitcheck(s_hl_timer_theinternals.initted, HL_timer_id2index(id)));
}

static void s_hl_timer_status_set(hl_timer_t id, hl_timer_status_t status)
{
    s_hl_timer_theinternals.items[HL_timer_id2index(id)]->status = status;
}

static hl_timer_status_t s_hl_timer_status_get(hl_timer_t id)
{
    hl_timer_internal_item_t *intitem = s_hl_timer_theinternals.items[HL_timer_id2index(id)];
    return( (NULL == intitem) ? (hl_timer_status_none) : (intitem->status) );
}

static hl_result_t s_hl_timer_prepare(hl_timer_t id, const hl_timer_cfg_t *cfg)
{
    hl_timer_internal_item_t *intitem = s_hl_timer_theinternals.items[HL_timer_id2index(id)];
    const hl_timer_stm32_properties_t* props = s_hl_timer_stm32_props[HL_timer_id2index(id)];
    
    if(NULL == props)
    {
        return(hl_res_NOK_unsupported);
    }
    
    if(NULL != cfg->advcfg)
    {
        
    }
    else
    {
        volatile uint32_t referencespeed = 0; 
        RCC_ClocksTypeDef clocks;
        RCC_GetClocksFreq(&clocks);  

        switch(props->referencespeedbus)
        {
            case hl_timer_refspeed_slowbus:     { referencespeed = clocks.PCLK1_Frequency;                      } break;
            case hl_timer_refspeed_fastbus:     { referencespeed = clocks.PCLK2_Frequency;                      } break;
            case hl_timer_refspeed_cpu:         { referencespeed = clocks.SYSCLK_Frequency;                     } break;
            default:                            { referencespeed = 0;                                           } break;
        } 
        
        if(0 == referencespeed)
        {
            return(hl_res_NOK_unsupported);
        }

        
        memcpy(&intitem->config, cfg, sizeof(hl_timer_cfg_t));

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
        
    }
    
    return(hl_res_OK);
}


static void s_hl_timer_stm32_start(hl_timer_t id)
{
    hl_timer_internal_item_t *intitem = s_hl_timer_theinternals.items[HL_timer_id2index(id)];
    const hl_timer_stm32_properties_t* props = s_hl_timer_stm32_props[HL_timer_id2index(id)];
    
    TIM_TypeDef* TIMx                           = props->TIMx;
    uint32_t RCC_APBxPeriph_TIMx                = props->RCC_APBxPeriph_TIMx;
    IRQn_Type TIMx_IRQn                         = (IRQn_Type)props->TIMx_IRQn;
    hl_timer_stm32_periphclock_t periphclock    = props->periphclock;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  


    // Enable TIMx clock 
    if(hl_timer_stm32_periphclock_apb1 == periphclock)
    {
        RCC_APB1PeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE);
    }
    else if(hl_timer_stm32_periphclock_apb2 == periphclock)
    {
        RCC_APB2PeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE);
    }

    // registers of TIMx
    
    if(NULL == intitem->config.advcfg)
    {
        TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
        TIM_TimeBaseStructure.TIM_Period = intitem->period - 1;          
        TIM_TimeBaseStructure.TIM_Prescaler = intitem->prescaler - 1; 
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  
        TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;
    }
    else
    {
        memcpy(&TIM_TimeBaseStructure, intitem->config.advcfg, sizeof(TIM_TimeBaseInitTypeDef));
    }
        
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);

    //* enable counter 
    TIM_Cmd(TIMx, ENABLE);

    // Immediate load of Prescaler value */
    TIM_PrescalerConfig(TIMx, TIM_TimeBaseStructure.TIM_Prescaler, TIM_PSCReloadMode_Immediate);
        

    // Clear  update pending flag */
    TIM_ClearFlag(TIMx, TIM_FLAG_Update);

    if(hl_irqpriorityNONE != intitem->config.priority)
    {
        // Enable TIMx Update interrupt */
        TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
    
        // enable irqs in nvic
        hl_sys_irqn_priority_set(TIMx_IRQn, intitem->config.priority);
        hl_sys_irqn_enable(TIMx_IRQn);
    }

}

static void s_hl_timer_stm32_stop(hl_timer_t id)
{   
    hl_timer_internal_item_t *intitem = s_hl_timer_theinternals.items[HL_timer_id2index(id)];
    const hl_timer_stm32_properties_t* props = s_hl_timer_stm32_props[HL_timer_id2index(id)];
    
    TIM_TypeDef* TIMx               = props->TIMx;
    IRQn_Type TIMx_IRQn             = (IRQn_Type)props->TIMx_IRQn;
    
#define TIMER_USE_IT
#if     defined(TIMER_USE_IT)

    TIM_DeInit(TIMx);
    TIM_Cmd(TIMx, DISABLE);

    if(hl_irqpriorityNONE != intitem->config.priority)
    {
        TIM_ITConfig(TIMx, TIM_IT_Update, DISABLE);
        hl_sys_irqn_disable(TIMx_IRQn);
    }

#else

    // disable TIMx clock 
    if(hl_timer_stm32_periphclock_apb1 == periphclock)
    {
        RCC_APB1PeriphClockCmd(RCC_APBxPeriph_TIMx, DISABLE);
    }
    else if(hl_timer_stm32_periphclock_apb2 == periphclock)
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

}

static hl_reltime_t s_hl_timer_get_period(hl_timer_t id)
{
    hl_timer_internal_item_t *intitem = s_hl_timer_theinternals.items[HL_timer_id2index(id)];

    return(intitem->config.countdown);
}


static void s_hl_timer_callback(hl_timer_t id)
{
    hl_timer_internal_item_t *intitem = s_hl_timer_theinternals.items[HL_timer_id2index(id)];
    
    if(hl_timer_mode_oneshot == intitem->config.mode)
    {
        // stop timer 
        s_hl_timer_stm32_stop(id);
              
        s_hl_timer_status_set(id, hl_timer_status_expired);
    }

    if(NULL != intitem->config.callback)
    {
        intitem->config.callback(intitem->config.arg);
    }
}

#endif//defined(HL_USE_UTIL_TIMER)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



