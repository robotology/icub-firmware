/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
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
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/12/2011
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_TIMER

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "hal_dspic_base_hid.h" 
#include "hal_brdcfg.h"
#include "hal_sys.h"
#include "hal_arch_dspic.h" 

#include <string.h>
#include <p33fxxxx.h>
#include <timer.h>  // contains specific api for the dspic timers.

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_timer.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_dspic_timer_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_timer_t2index(t)           ((uint8_t)((t)))


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// emty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
     uint16_t *timerRegAddr;
     uint16_t *timerHolRegAddr;
     uint16_t *periodRegAddr;
     uint16_t *confRegAddr; 
} hal_timer_arch_registers_t;


//this data struct rappresents register map in memory.
// x ==> even timer number
// y ==> odd timer_number.
// the timers are grouped together in two, in order to provide timer with period on 32 bits.
typedef struct
{
    uint16_t TMRx_reg;
    uint16_t TMRyHLD_reg;
    uint16_t TMRy_reg;
    uint16_t PRx_reg;
    uint16_t PRy_reg;
    uint16_t TxCON_reg;
    uint16_t TyCON_reg;
} hal_timer_arch_register_map_t;


typedef struct
{
    hal_timer_cfg_t             cfg;        // fixed for every architecture
    hal_timer_status_t          status;     // fixed for every architecture
    uint16_t                    period;
    uint16_t                    prescaler;
    hal_timer_arch_registers_t  reg;        
} hal_timer_datastructure_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_timer_supported_is(hal_timer_t timer);
static void s_hal_timer_initted_set(hal_timer_t timer);
static hal_boolval_t s_hal_timer_initted_is(hal_timer_t timer);

static void s_hal_timer_status_set(hal_timer_t timer, hal_timer_status_t status);
static hal_timer_status_t s_hal_timer_status_get(hal_timer_t timer);

static hal_result_t s_hal_timer_config(hal_timer_t timer);
static void s_hal_timer_priority_config(hal_timer_t timer);
static void s_hal_timer_callback(hal_timer_t timer);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hal_timer_datastructure_t *s_hal_timer_info[HAL_BRDCFG_TIMER__maxnumber] = {NULL, NULL, NULL, NULL, NULL};

//size of a timers register group in PC uint = 2 byte 
static const uint8_t s_timer_arch_reg_group_size = (sizeof(hal_timer_arch_register_map_t)/2); 


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hal_result_t hal_timer_init(hal_timer_t timer, const hal_timer_cfg_t *cfg, hal_time_t *error)
{
    hal_timer_datastructure_t *info = s_hal_timer_info[HAL_timer_t2index(timer)];
    hal_result_t res;
    uint16_t remainder;

    if(hal_false == s_hal_timer_supported_is(timer))
    {
        return(hal_res_NOK_generic);
    }

    if(NULL == info)
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

    if(hal_timer_prescalerAUTO == cfg->prescaler)
    {
        return(hal_res_NOK_wrongparam);
    } 

    // if running stop.
    if(hal_timer_status_running == hal_timer_status_get(timer))
    {
        hal_timer_stop(timer);
    } 
   
    
//    if(hal_timer_status_running == s_hal_timer_info[HAL_timer_t2index(timer)]->status)
//    {
//        *(s_hal_timer_info[HAL_timer_t2index(timer)]->reg.confRegAddr) &=0x7FFF; //stop timer (reset TON bit)
//    }
//    s_hal_timer_info[HAL_timer_t2index(timer)]->status = hal_timer_status_idle;


    // marker: save the cfg and init registers

    //save configuration in working data of timer
    memcpy(&(s_hal_timer_info[HAL_timer_t2index(timer)]->cfg), cfg, sizeof(hal_timer_cfg_t));


    //config timer's registers 
    res = s_hal_timer_config(timer);
    
    if(hal_res_OK != res)
    {
        return(res); // in case the chosen prescaler is not supported
    }

    // config timer's priority and enable interrupt
    s_hal_timer_priority_config(timer);

    
    // marker: calculate error
    if(NULL != error)
    {
        remainder = (((s_hal_timer_info[HAL_timer_t2index(timer)]->cfg.countdown)*1000L)%(s_hal_timer_info[HAL_timer_t2index(timer)]->period));
        if(0 == remainder)
        {
            *error = 0;
        }
        else
        {
            *error = remainder/1000L;
        }    
    }

        
    s_hal_timer_initted_set(timer);


    return(hal_res_OK);
}


extern hal_result_t hal_timer_start(hal_timer_t timer)
{
    if(hal_false == s_hal_timer_initted_is(timer))
    {
        return(hal_res_NOK_generic);
    }

    if(hal_timer_status_running == s_hal_timer_status_get(timer))
    {
        hal_timer_stop(timer);
    }
    
//    if(hal_timer_status_running == s_hal_timer_info[timer]->status)
//    {
//        *(s_hal_timer_info[HAL_timer_t2index(timer)]->reg.confRegAddr) &=0x7FFF; //stop timer
//    }
//    s_hal_timer_info[HAL_timer_t2index(timer)]->status = hal_timer_status_running;  

    s_hal_timer_status_set(timer, hal_timer_status_running);

    
    *(s_hal_timer_info[HAL_timer_t2index(timer)]->reg.timerRegAddr) = 0; //reset counting register
    *(s_hal_timer_info[HAL_timer_t2index(timer)]->reg.confRegAddr) |= 0x8000; //set TON bit

    return(hal_res_OK);
}



extern hal_result_t hal_timer_stop(hal_timer_t timer)
{
    if(hal_false == s_hal_timer_initted_is(timer))
    {
        return(hal_res_NOK_generic);
    }

    if(hal_timer_status_idle == s_hal_timer_status_get(timer))
    {
        return(hal_res_OK);
    }
    
//    if(hal_timer_status_idle == s_hal_timer_info[HAL_timer_t2index(timer)]->status)
//    {
//        return(hal_res_OK);
//    }
    
    // marker: stop and set status

    *(s_hal_timer_info[HAL_timer_t2index(timer)]->reg.confRegAddr) &=0x7FFF; //reset TON bit
//    s_hal_timer_info[HAL_timer_t2index(timer)]->status = hal_timer_status_idle;
  
    s_hal_timer_status_set(timer, hal_timer_status_idle);
  
    return(hal_res_OK);
}



extern hal_result_t hal_timer_countdown_set(hal_timer_t timer, hal_time_t countdown, hal_time_t *error)
{                                                                            
    hal_timer_cfg_t *curcfg = NULL;
    hal_timer_cfg_t newcfg;
    uint8_t wasrunning = 0;

    if(hal_false == s_hal_timer_initted_is(timer))
    {
        return(hal_res_NOK_generic);
    }

    // if running stop.
    if(hal_timer_status_running == hal_timer_status_get(timer))
    {
        wasrunning = 1;
    }

    // computes the values to be put in registers
    curcfg = &s_hal_timer_info[HAL_timer_t2index(timer)]->cfg;
    memcpy(&newcfg, curcfg, sizeof(hal_timer_cfg_t));
    newcfg.countdown = countdown;

    hal_timer_init(timer, &newcfg, error);

    if(1 == wasrunning)
    {
        hal_timer_start(timer);
    }

    return(hal_res_OK);

#if 0  
    if(hal_false == s_hal_timer_initted_is(timer))
    {
        return(hal_res_NOK_generic);
    }

    if(hal_timer_status_running == s_hal_timer_info[timer]->status)
    {
        *(s_hal_timer_info[HAL_timer_t2index(timer)]->reg.confRegAddr) &=0x7FFF; //stop timer (reset TON bit)
    } 


    *(s_hal_timer_info[HAL_timer_t2index(timer)]->reg.periodRegAddr) = (countdown*1000L)/s_hal_timer_info[timer]->period;
    *(s_hal_timer_info[HAL_timer_t2index(timer)]->reg.timerRegAddr) = 0; //reset counting register

    return(hal_res_OK);
#endif    
}


extern hal_result_t hal_timer_priority_set(hal_timer_t timer, hal_interrupt_priority_t prio)
{

    hal_timer_status_t actual_status;

    if(hal_false == s_hal_timer_initted_is(timer))
    {
        return(hal_res_NOK_generic);
    }

    if((hal_timer_mode_oneshot == s_hal_timer_info[HAL_timer_t2index(timer)]->cfg.mode) && 
       (hal_int_priorityNONE == prio) )
    {
        return(hal_res_NOK_wrongparam);
    }
    
    actual_status = s_hal_timer_info[HAL_timer_t2index(timer)]->status;

    if(hal_timer_status_running == actual_status)
    {
        *(s_hal_timer_info[HAL_timer_t2index(timer)]->reg.confRegAddr) &=0x7FFF; //stop timer (reset TON bit)   
    }

   
    switch(timer)
    {
        case hal_timer1:
        {
            hal_sys_irqn_priority_set(hal_arch_dspic_TIM1_IRQn, prio);
        } break;
        
        case hal_timer2:
        {
            hal_sys_irqn_priority_set(hal_arch_dspic_TIM2_IRQn, prio);
        } break;
        
        case hal_timer3:
        {
            hal_sys_irqn_priority_set(hal_arch_dspic_TIM3_IRQn, prio);
        } break;
        
        case hal_timer4:
        {
            hal_sys_irqn_priority_set(hal_arch_dspic_TIM4_IRQn, prio);
        } break;
        
        case hal_timer5:
        {
            hal_sys_irqn_priority_set(hal_arch_dspic_TIM5_IRQn, prio);
        } break;
        
        //I insert the default case, unless it never can be reached to avoid warning.
        //The default case never can be reached, because if timer is unsupperted, it is check by s_hal_timer_initted_is()
        default:
        {
            return(hal_res_NOK_generic);
        } break;
        
    }

    if(hal_timer_status_running == actual_status)
    {
        *(s_hal_timer_info[HAL_timer_t2index(timer)]->reg.confRegAddr) |= 0x8000; //start timer (set TON bit)   
    }

    return(hal_res_OK);
}


extern hal_result_t hal_timer_interrupt_enable(hal_timer_t timer)
{
    if(hal_false == s_hal_timer_initted_is(timer))
    {
        return(hal_res_NOK_generic);
    }

    switch(timer)
    {
        case hal_timer1:
        {
            hal_sys_irqn_enable(hal_arch_dspic_TIM1_IRQn);
        } break;

        case hal_timer2:
        {
            hal_sys_irqn_enable(hal_arch_dspic_TIM2_IRQn);
        } break;

        case hal_timer3:
        {
            hal_sys_irqn_enable(hal_arch_dspic_TIM3_IRQn);
        } break;
        
        case hal_timer4:
        {
            hal_sys_irqn_enable(hal_arch_dspic_TIM4_IRQn);
        } break;
        
        case hal_timer5:
        {
            hal_sys_irqn_enable(hal_arch_dspic_TIM5_IRQn);
        } break;

        //I insert the default case, unless it never can be reached to evaoid warning.
        //The default case never can be reached, because if timer is unsupperted, it is check by s_hal_timer_initted_is()
        default:
        {
            return(hal_res_NOK_generic);
        } break;
    }

    return(hal_res_OK);
}



extern hal_result_t hal_timer_interrupt_disable(hal_timer_t timer)
{
    if(hal_false == s_hal_timer_initted_is(timer))
    {
        return(hal_res_NOK_generic);
    }

        switch(timer)
    {
        case hal_timer1:
        {
            hal_sys_irqn_disable(hal_arch_dspic_TIM1_IRQn);
        } break;

        case hal_timer2:
        {
            hal_sys_irqn_disable(hal_arch_dspic_TIM2_IRQn);
        } break;

        case hal_timer3:
        {
            hal_sys_irqn_disable(hal_arch_dspic_TIM3_IRQn);
        } break;
        
        case hal_timer4:
        {
            hal_sys_irqn_disable(hal_arch_dspic_TIM4_IRQn);
        } break;

        case hal_timer5:
        {
             hal_sys_irqn_disable(hal_arch_dspic_TIM5_IRQn);
        } break;

        //I insert the default case, unless it never can be reached to evaoid warning.
        //The default case never can be reached, because if timer is unsupperted, it is check by s_hal_timer_initted_is()
        default:
        {
            return(hal_res_NOK_generic);
        } break;
    }
 

    return(hal_res_OK);
}



extern hal_result_t hal_timer_remainingtime_get(hal_timer_t timer, hal_time_t *remaining_time)
{
    /* Note: the timers in DSPIC count in ascending order. */

    uint32_t remaining_tick_num;

    if(hal_false == s_hal_timer_initted_is(timer))
    {
        return(hal_res_NOK_generic);
    }

    if(NULL == remaining_time)
    {
        return(hal_res_NOK_generic);
    }

    remaining_tick_num =  *(s_hal_timer_info[HAL_timer_t2index(timer)]->reg.periodRegAddr) -  
                          *(s_hal_timer_info[HAL_timer_t2index(timer)]->reg.timerRegAddr);
    
    *remaining_time = ((remaining_tick_num)*(s_hal_timer_info[HAL_timer_t2index(timer)]->period)) / 1000L;
    
    return(hal_res_OK);
}


extern hal_result_t hal_timer_offset_write(hal_timer_t timer, hal_nanotime_t offset)
{
    if(hal_false == s_hal_timer_initted_is(timer))
    {
        return(hal_res_NOK_generic);
    }

    if(offset >= (s_hal_timer_info[HAL_timer_t2index(timer)]->cfg.countdown*1000L))
    {
        return(hal_res_NOK_wrongparam);
    }

    *(s_hal_timer_info[HAL_timer_t2index(timer)]->reg.timerRegAddr) = offset/s_hal_timer_info[HAL_timer_t2index(timer)]->period;  

    
    return(hal_res_OK);
}


extern hal_timer_status_t hal_timer_status_get(hal_timer_t timer)
{

    if(hal_false == s_hal_timer_initted_is(timer))
    {
        return(hal_timer_status_none);
    }

    return(s_hal_timer_status_get(timer));
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----

// isr for timer1
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    s_hal_timer_callback(hal_timer1);
    
    IFS0bits.T1IF = 0; // clear flag  
}

// isr for timer2
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
{
    s_hal_timer_callback(hal_timer2);

    IFS0bits.T2IF = 0; // clear flag  
}

// isr for timer3
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void)
{
    s_hal_timer_callback(hal_timer3);
  
    IFS0bits.T3IF = 0; // clear flag  
}

// isr for timer4
void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void)
{
    s_hal_timer_callback(hal_timer4);

    IFS1bits.T4IF = 0; // clear flag  
}

// isr for timer5
void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(void)
{
    s_hal_timer_callback(hal_timer5);
  
    IFS1bits.T5IF = 0; // clear flag  
}

// ---- isr of the module: end ------


extern uint32_t hal_timer_hid_getsize(const hal_cfg_t *cfg)
{
    uint32_t size = 0;
      
    if(cfg->timers_num > HAL_BRDCFG_TIMER__maxnumber)
    {
        size = (HAL_BRDCFG_TIMER__maxnumber * sizeof(hal_timer_datastructure_t));
    }
    else
    {
        size = (cfg->timers_num * sizeof(hal_timer_datastructure_t));
    }
    return(size);   
}

extern hal_result_t hal_timer_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    uint8_t i, timers_num;
    

    if(NULL == memory)
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_xxx_hid_setmem(): memory missing");
        return(hal_res_NOK_generic);
    }

    if(cfg->timers_num > HAL_BRDCFG_TIMER__maxnumber)
    {
        timers_num = HAL_BRDCFG_TIMER__maxnumber;
    }
    else
    {
        timers_num = cfg->timers_num;
    }

    for(i=0; i<timers_num; i++)
    {
        s_hal_timer_info[i] = (hal_timer_datastructure_t*)memory;
        memset(s_hal_timer_info[i], 0, sizeof(hal_timer_datastructure_t));
        memory += sizeof(hal_timer_datastructure_t);
    }
    return(hal_res_OK);  
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_timer_supported_is(hal_timer_t timer)
{
    return(hal_base_hid_byte_bitcheck(hal_brdcfg_timer__supported_mask, HAL_timer_t2index(timer)) );
}

static void s_hal_timer_initted_set(hal_timer_t timer)
{
    s_hal_timer_status_set(timer, hal_timer_status_idle);
}

static hal_boolval_t s_hal_timer_initted_is(hal_timer_t timer)
{
    return((hal_timer_status_none != s_hal_timer_status_get(timer)) ? (hal_true) : (hal_false));
}

static void s_hal_timer_status_set(hal_timer_t timer, hal_timer_status_t status)
{
    s_hal_timer_info[HAL_timer_t2index(timer)]->status = status;
}

static hal_timer_status_t s_hal_timer_status_get(hal_timer_t timer)
{
    hal_timer_datastructure_t *info = s_hal_timer_info[HAL_timer_t2index(timer)];
    return( (NULL == info) ? (hal_timer_status_none) : (info->status) );
}


static hal_result_t s_hal_timer_config(hal_timer_t timer)
{
    //I used T1's macros because they are equal to other timers' macros.

    uint16_t flags = 0; 
    uint16_t *timerRegAddr, *periodRegAddr, *confRegAddr;
    uint8_t num_group_reg;
    uint8_t odd;
    hal_timer_datastructure_t *t_data_ptr = s_hal_timer_info[HAL_timer_t2index(timer)];  
    
    flags  = T1_OFF & T1_GATE_OFF & T1_IDLE_STOP & T1_SOURCE_INT;

    switch(t_data_ptr->cfg.prescaler)
    {
        case hal_timer_prescaler1:
        {
            flags &= T1_PS_1_1;
            t_data_ptr->period = 25; //expressed in nanosec
        }
        break;

        case hal_timer_prescaler8:
        {
            flags &= T1_PS_1_8;
            t_data_ptr->period = 200; //expressed in nanosec
        }
        break;


        case hal_timer_prescaler64:
        {
            flags &= T1_PS_1_64;
            t_data_ptr->period = 1600; //expressed in nanosec
        }
        break;

        case hal_timer_prescaler256:
        {
            flags &= T1_PS_1_256;
            t_data_ptr->period = 6400; //expressed in nanosec
        }
        break;

        default:
        {
            return(hal_res_NOK_unsupported);
        }
    }


    if(hal_timer1 == timer)
    {
        t_data_ptr->reg.timerRegAddr = (uint16_t *)(&TMR1);
        t_data_ptr->reg.periodRegAddr = (uint16_t *)(&PR1);
        t_data_ptr->reg.confRegAddr = (uint16_t *)(&T1CON);
        flags &= T1_SYNC_EXT_OFF;
    }
    else
    {
        
    switch(timer)
    {
        case hal_timer2:
        case hal_timer4:
        {
            odd = 0;
            num_group_reg = (HAL_timer_t2index(timer) - HAL_timer_t2index(hal_timer2))/2;
            timerRegAddr = (uint16_t *)(&TMR2 + s_timer_arch_reg_group_size *num_group_reg + odd);
        }
        break;

        case hal_timer3:
        case hal_timer5:
        {
            odd = 1;
            num_group_reg = (HAL_timer_t2index(timer) - HAL_timer_t2index(hal_timer2))/2;
            timerRegAddr = (uint16_t *)(&TMR2 + s_timer_arch_reg_group_size *num_group_reg + odd + 1); 
        }
        break;

        default:
            break;
    }

        periodRegAddr = (uint16_t *)(&PR2 + s_timer_arch_reg_group_size*num_group_reg + odd);
        confRegAddr = (uint16_t *)(&T2CON + s_timer_arch_reg_group_size*num_group_reg + odd);
        
        //save register adress in ram.
        t_data_ptr->reg.timerRegAddr = timerRegAddr;
        t_data_ptr->reg.periodRegAddr = periodRegAddr;
        t_data_ptr->reg.confRegAddr = confRegAddr;
    }

    /* config register */

    // Reset timer value to 0x0000
    *(t_data_ptr->reg.timerRegAddr) = 0; 

    // assigning Period to Timer period register
    *(t_data_ptr->reg.periodRegAddr) = ((t_data_ptr->cfg.countdown)*1000L)/t_data_ptr->period;

    // config timer's control regiter
    *(t_data_ptr->reg.confRegAddr) = 0;
    *(t_data_ptr->reg.confRegAddr) = flags;  

    //configure timer to work on 16 bits
    *(t_data_ptr->reg.confRegAddr) &= 0xFFF7; //

    return(hal_res_OK);
}  

static void s_hal_timer_priority_config(hal_timer_t timer)
{

    uint8_t int_on = hal_false;
    hal_interrupt_priority_t prio = s_hal_timer_info[HAL_timer_t2index(timer)]->cfg.priority;

    if(hal_int_priorityNONE != prio)
    {
        int_on = hal_true;
    }
    switch(timer)
    {
        case hal_timer1:
        {
            if(int_on)
            {
                hal_sys_irqn_priority_set(hal_arch_dspic_TIM1_IRQn, prio);
                hal_sys_irqn_enable(hal_arch_dspic_TIM1_IRQn);
            }
            else
            {
                hal_sys_irqn_disable(hal_arch_dspic_TIM1_IRQn);
            }
            break;
        }

        case hal_timer2:
        {
            if(int_on)
            {
                hal_sys_irqn_priority_set(hal_arch_dspic_TIM2_IRQn, prio);
                hal_sys_irqn_enable(hal_arch_dspic_TIM2_IRQn);            
            }
            else
            {
                hal_sys_irqn_disable(hal_arch_dspic_TIM2_IRQn);
            }
            break;
        }

        case hal_timer3:
        {
            if(int_on)
            {
                hal_sys_irqn_priority_set(hal_arch_dspic_TIM3_IRQn, prio);
                hal_sys_irqn_enable(hal_arch_dspic_TIM3_IRQn);
            }
            else
            {
                hal_sys_irqn_disable(hal_arch_dspic_TIM3_IRQn);
            }
            break;
        }
        case hal_timer4:
        {
            if(int_on)
            {
                hal_sys_irqn_priority_set(hal_arch_dspic_TIM4_IRQn, prio);
                hal_sys_irqn_enable(hal_arch_dspic_TIM4_IRQn);
            }
            else
            {
                hal_sys_irqn_disable(hal_arch_dspic_TIM4_IRQn);
             }
            break;
        }

        case hal_timer5:
        {
            if(int_on)
            {
                hal_sys_irqn_priority_set(hal_arch_dspic_TIM5_IRQn, prio);
                hal_sys_irqn_enable(hal_arch_dspic_TIM5_IRQn);            
            }
            else
            {
                hal_sys_irqn_disable(hal_arch_dspic_TIM5_IRQn);
             }
            break;
        }

        //I insert the default case, unless it never can be reached to evaoid warning.
        //The default case never can be reached, because if timer is unsupperted, it is check by s_hal_timer_initted_is()
        default:
        {
            return;
        } 
    } 
}


static void s_hal_timer_callback(hal_timer_t timer)
{
    hal_timer_cfg_t *cfg = &(s_hal_timer_info[HAL_timer_t2index(timer)]->cfg);
    
    if(hal_timer_mode_oneshot == cfg->mode)
    {
        *(s_hal_timer_info[HAL_timer_t2index(timer)]->reg.confRegAddr) &=0x7FFF; //stop timer (reset TON bit)       
        s_hal_timer_info[HAL_timer_t2index(timer)]->status = hal_timer_status_expired;
    }
    if(NULL != cfg->callback_on_exp)
    {
        cfg->callback_on_exp(cfg->arg);
    }
}
#endif//HAL_USE_TIMER

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



