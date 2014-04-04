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

/* @file       hal_watchdog.c
	@brief      This file implements internal implementation of the hal watchdog module.
	@author     marco.accame@iit.it
    @date       10/21/2011
**/


// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"
// - middleware interface: contains hl, stm32 etc. --------------------------------------------------------------------
#include "hal_middleware_interface.h"


#ifdef HAL_USE_WATCHDOG


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "string.h"
#include "stdlib.h"
#include "hl_bits.h" 
#include "hal_heap.h"
#include "hal_sys.h" 

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_watchdog.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_watchdog_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_watchdog_id2index(t)               ((uint8_t)(t))


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_watchdog_cfg_t hal_watchdog_cfg_default =
{
    .countdown                  = 20000,
    .priority                   = hal_int_priorityNONE,
    .onwindowexpiry_cbk         = NULL, 
    .onwindowexpiry_arg         = NULL
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    hal_watchdog_cfg_t  config;
    uint32_t            reload;    
} hal_watchdog_internal_item_t;

typedef struct
{
    uint32_t                        inittedmask;
    uint32_t                        startedmask;
    hal_watchdog_internal_item_t*   items[hal_watchdogs_number];   
} hal_watchdog_theinternals_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_watchdog_supported_is(hal_watchdog_t id);
static void s_hal_watchdog_initted_set(hal_watchdog_t id);
static void s_hal_watchdog_initted_clear(hal_watchdog_t id);

//#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK) 
static hal_boolval_t s_hal_watchdog_initted_is(hal_watchdog_t id);
//#endif

static void s_hal_watchdog_started_set(hal_watchdog_t id);
static hal_boolval_t s_hal_watchdog_started_is(hal_watchdog_t id);


static void s_hal_watchdog_normal_start(hal_watchdog_internal_item_t *intitem);
static void s_hal_watchdog_window_start(hal_watchdog_internal_item_t *intitem);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_watchdog_theinternals_t s_hal_watchdog_theinternals =
{
    .inittedmask        = 0,
    .startedmask        = 0,
    .items              = { NULL }   
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hal_result_t hal_watchdog_init(hal_watchdog_t id, const hal_watchdog_cfg_t *cfg)
{
    hal_watchdog_internal_item_t* intitem = s_hal_watchdog_theinternals.items[HAL_watchdog_id2index(id)];
    hal_result_t res = hal_res_NOK_generic;

    if(hal_false == s_hal_watchdog_supported_is(id))
    {
        return(hal_res_NOK_unsupported);
    }
    
    if(hal_true == s_hal_watchdog_started_is(id))
    {   // if already started cannot init that again
        return(hal_res_NOK_generic);
    }    
    
    if(NULL == cfg)
    {
        cfg = &hal_watchdog_cfg_default;
    }
    
    // if it does not have ram yet, then attempt to allocate it.
    if(NULL == intitem)
    {
        intitem = s_hal_watchdog_theinternals.items[HAL_watchdog_id2index(id)] = hal_heap_new(sizeof(hal_watchdog_internal_item_t));
        // minimal initialisation of the internal item
        // nothing to init.      
    }      

    switch(id)
    {
        case hal_watchdog1_normal:
        {
            memcpy(&intitem->config, cfg, sizeof(hal_watchdog_cfg_t));
            if(intitem->config.countdown > 10*hal_RELTIME_1second)
            {
                intitem->config.countdown = 10*hal_RELTIME_1second;
            }
            else if(intitem->config.countdown < 10*hal_RELTIME_1millisec)
            {
                intitem->config.countdown = 10*hal_RELTIME_1millisec;
            }
        
            intitem->reload = (intitem->config.countdown / 3200);        
            
            res = hal_res_OK;
        } break;

        case hal_watchdog2_window:
        {
            memcpy(&intitem->config, cfg, sizeof(hal_watchdog_cfg_t));
            if(intitem->config.countdown > 50*hal_RELTIME_1millisec)
            {
                intitem->config.countdown = 50*hal_RELTIME_1millisec;
            }
            else if(intitem->config.countdown < 5*hal_RELTIME_1millisec)
            {
                intitem->config.countdown = 5*hal_RELTIME_1millisec;
            }
        
            intitem->reload = 64 + ((intitem->config.countdown+910) / 910);        
            
            res = hal_res_OK;
        } break;

        default:
        {
            res = hal_res_NOK_generic;
        } break;
    }
    
   
    if(hal_res_OK == res)
    {
        s_hal_watchdog_initted_set(id);
    }
    else
    {
        s_hal_watchdog_initted_clear(id);
    }

    return(res);
}


extern hal_result_t hal_watchdog_start(hal_watchdog_t id)
{
    hal_watchdog_internal_item_t* intitem = s_hal_watchdog_theinternals.items[HAL_watchdog_id2index(id)];
    hal_result_t res = hal_res_NOK_generic;

//#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK)     
    if(hal_false == s_hal_watchdog_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    if(hal_true == s_hal_watchdog_started_is(id))
    {   // if already started cannot start it again
        hal_watchdog_refresh(id);
        return(hal_res_OK);
    }
//#endif

    switch(id)
    {
        case hal_watchdog1_normal:
        {
            s_hal_watchdog_normal_start(intitem);
            s_hal_watchdog_started_set(id);
            res = hal_res_OK;
        } break;

        case hal_watchdog2_window:
        {
            s_hal_watchdog_window_start(intitem);
            s_hal_watchdog_started_set(id);
            res = hal_res_OK;
        } break;

        default:
        {
            res = hal_res_NOK_generic;
        } break;
    }

    return(res);
}



extern hal_result_t hal_watchdog_refresh(hal_watchdog_t id)
{
    hal_watchdog_internal_item_t* intitem = s_hal_watchdog_theinternals.items[HAL_watchdog_id2index(id)];
    hal_result_t res = hal_res_NOK_generic;

//#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK) 
    if(hal_false == s_hal_watchdog_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    if(hal_false == s_hal_watchdog_started_is(id))
    {   // if not started i cannot refresh it
        return(hal_res_NOK_generic);
    }    
//#endif

    switch(id)
    {
        case hal_watchdog1_normal:
        {
            IWDG_ReloadCounter();
            res = hal_res_OK;
        } break;

        case hal_watchdog2_window:
        {
            WWDG_SetCounter(intitem->reload);
            res = hal_res_OK;
        } break;

        default:
        {
            res = hal_res_NOK_generic;
        } break;
    }

    return(res);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----

void WWDG_IRQHandler(void)
{
    hal_watchdog_internal_item_t* intitem = s_hal_watchdog_theinternals.items[HAL_watchdog_id2index(hal_watchdog2_window)];

    // Clear EWI flag 
    WWDG_ClearFlag();

    if(NULL !=intitem->config.onwindowexpiry_cbk)
    {
        intitem->config.onwindowexpiry_cbk(intitem->config.onwindowexpiry_arg);
    }
}

// ---- isr of the module: end ------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_watchdog_supported_is(hal_watchdog_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(hal_watchdog__theboardconfig.supportedmask, HAL_watchdog_id2index(id)));
}

static void s_hal_watchdog_initted_set(hal_watchdog_t id)
{
    hl_bits_word_bitset(&s_hal_watchdog_theinternals.inittedmask, HAL_watchdog_id2index(id));
}

static void s_hal_watchdog_initted_clear(hal_watchdog_t id)
{
    hl_bits_word_bitclear(&s_hal_watchdog_theinternals.inittedmask, HAL_watchdog_id2index(id));
}

//#if     !defined(HAL_BEH_REMOVE_RUNTIME_VALIDITY_CHECK) 
static hal_boolval_t s_hal_watchdog_initted_is(hal_watchdog_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(s_hal_watchdog_theinternals.inittedmask, HAL_watchdog_id2index(id)));
}
//#endif

static void s_hal_watchdog_started_set(hal_watchdog_t id)
{
    hl_bits_word_bitset(&s_hal_watchdog_theinternals.startedmask, HAL_watchdog_id2index(id));
}


static hal_boolval_t s_hal_watchdog_started_is(hal_watchdog_t id)
{
    return((hal_boolval_t)hl_bits_word_bitcheck(s_hal_watchdog_theinternals.startedmask, HAL_watchdog_id2index(id)));
}



static void s_hal_watchdog_normal_start(hal_watchdog_internal_item_t *intitem)
{
  /* IWDG timeout equal to xxx ms (the timeout may varies due to LSI frequency
     dispersion) */
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  /* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */
  IWDG_SetPrescaler(IWDG_Prescaler_128);

  /* Set counter reload value to 349 */
  IWDG_SetReload(intitem->reload);

  /* Reload IWDG counter */
  IWDG_ReloadCounter();

  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();

}


static void s_hal_watchdog_window_start(hal_watchdog_internal_item_t *intitem)
{
//    NVIC_InitTypeDef NVIC_InitStructure;
    // do something using s_hal_watchdog_theinternals.items[0].countdown etc.

    // acemor on 21oct11: taken from STM32F10x_StdPeriph_Lib_V3.2.0\Project\STM32F10x_StdPeriph_Examples\WWDG

//    // 1. init nvic
//    /* 1 bits for pre-emption priority and 3 bits for subpriority */
////    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
//
////    NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0,0));
//
//    /* Set WWDG interrupt vector Preemption Priority to 1 */
//    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

    // 1b. wwdg clock
    /* WWDG configuration */
    /* Enable WWDG clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

    // 2. set prescaler
    /* On other devices, WWDG clock counter = (PCLK1(36MHz)/4096)/8 = 1099 Hz (909.918 us)  */
    WWDG_SetPrescaler(WWDG_Prescaler_8);

    // 3. set window
    /* Set Window value to 65 */
    WWDG_SetWindowValue(intitem->reload);
    //WWDG_SetWindowValue(65);


    // 4. set counter value and clear
    /* On Value line devices, Enable WWDG and set counter value to 127, WWDG timeout = ~1366 æs * 64 = 87.42 ms */
    /* On other devices, Enable WWDG and set counter value to 127, WWDG timeout = ~910 æs * 64 = 58.25 ms */
    WWDG_Enable(intitem->reload);
    
    /* Clear EWI flag */
    WWDG_ClearFlag();
    
    // 5. enable interrupt
    if(hal_int_priorityNONE != intitem->config.priority)
    {
        // enable irqs in nvic
        hal_sys_irqn_priority_set(WWDG_IRQn, intitem->config.priority);
        hal_sys_irqn_enable(WWDG_IRQn);

        /* Enable EW interrupt */
        WWDG_EnableIT();
    }

}




#endif//HAL_USE_WATCHDOG

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



