/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------
/*	@file 	hal_timers.c
	@brief 	This file implements timer's functionalities.
	@author valentina.gaggero@iit.it
	@date	-
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include <p30f4013.h>
#include <timer.h>


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "hal_timers.h"
#include <string.h>


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//#include "entity_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define TIMER_MAX_NUM       5 //DSPIC30F4013 provides 5 timers only 


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
    void(*call_bkp_fn)(void);
}timer_datastruct_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static timer_datastruct_t timer_list[TIMER_MAX_NUM];


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hal_result_t hal_timer_config(hal_timer_t timer, hal_timer_config_datastruct_t *config_ptr)
{
    //I used T1's macros because they are eual to other timers' macros.

	uint16_t flags = T1_OFF; 
	uint16_t prescaler = 0; 
	
    if(NULL == config_ptr)
    {
        return(HAL_RES_NOK);
    }

    if(NULL != config_ptr->call_bkp_fn)
    {
        timer_list[timer -1].call_bkp_fn = config_ptr->call_bkp_fn;
    }
    else
    {
        timer_list[timer -1].call_bkp_fn = NULL;
    }

	if(config_ptr->gate_op)
	{
		flags &= T1_GATE_ON;
	}
	else
	{
		flags &= T1_GATE_OFF;
	}

	if(config_ptr->idle)
	{
		flags &= T1_IDLE_CON;
	}
	else
	{
		flags &= T1_IDLE_STOP;
	}

	if(TIMER_SOURCE_INTERNAL == config_ptr->source)
	{
		flags &= T1_SOURCE_INT;
	}
	else
	{
		flags &= T1_SOURCE_EXT;
	}

	//convert prescaler form HAL value to DSPIC register value
	prescaler = (config_ptr->prescaler * 0x10) + T1_PS_1_1;
	//prescaler value is set in the register like as other values.
	flags &= prescaler;
	
	switch(timer)
	{
		case hal_timerT1:
		{
			WriteTimer1(0);
			OpenTimer1(flags & T1_SYNC_EXT_OFF, config_ptr->match_value);
			break;
		}	
	
		case hal_timerT2:
		{
			WriteTimer2(0);
 			OpenTimer2(flags, config_ptr->match_value);
			break;
		}


		case hal_timerT3:
		{
			WriteTimer3(0);
 			OpenTimer3(flags, config_ptr->match_value);	
			break;
		}
	
		case hal_timerT4:
		{
			WriteTimer4(0);
 			OpenTimer4(flags, config_ptr->match_value);
			break;
		}


		case hal_timerT5:
		{
			WriteTimer5(0);
 			OpenTimer5(flags, config_ptr->match_value);	
			break;
		}
	}
    return(HAL_RES_OK);
}




extern void hal_timer_start(hal_timer_t timer)
{
	
	switch(timer)
	{
		case hal_timerT1:
		{
			T1CONbits.TON = 1;	
			break;	
		}

		case hal_timerT2:
		{
			T2CONbits.TON = 1;
			break;	
		}

		case hal_timerT3:
		{
			T3CONbits.TON = 1;
			break;	
		}
		case hal_timerT4:
		{
			T4CONbits.TON = 1;
			break;	
		}

		case hal_timerT5:
		{
			T5CONbits.TON = 1;
			break;	
		}
	}
}



extern void hal_timer_stop(hal_timer_t timer)
{
	
	switch(timer)
	{
		case hal_timerT1:
		{
			T1CONbits.TON = 0;	
			break;	
		}

		case hal_timerT2:
		{
			T2CONbits.TON = 0;
			break;	
		}

		case hal_timerT3:
		{
			T3CONbits.TON = 0;
			break;	
		}
		case hal_timerT4:
		{
			T4CONbits.TON = 0;
			break;	
		}

		case hal_timerT5:
		{
			T5CONbits.TON = 0;
			break;	
		}
	}
}



extern void hal_timer_matchValue_set(hal_timer_t timer, hal_timer_prescalerValue_t  prescaler, uint16_t match_value)
{

	switch(timer)
	{
		case hal_timerT1:
		{
			T1CONbits.TCKPS = prescaler;
			PR1 = match_value;	
			break;	
		}

		case hal_timerT2:
		{
			T2CONbits.TCKPS = prescaler;
			PR2 = match_value;	
			break;	
		}

		case hal_timerT3:
		{
			T3CONbits.TCKPS = prescaler;
			PR3 = match_value;	
			break;	
		}
		case hal_timerT4:
		{
			T4CONbits.TCKPS = prescaler;
			PR4 = match_value;	
			break;	
		}

		case hal_timerT5:
		{
			T5CONbits.TCKPS = prescaler;
			PR5 = match_value;	
			break;	
		}
	}
}



extern void hal_timer_interrupt_config(hal_timer_t timer, hal_timer_intPriorityValue_t prio)
{
	switch(timer)
	{
		case hal_timerT1:
		{
			SetPriorityIntT1(prio+T1_INT_PRIOR_0);
			break;
		}
		case hal_timerT2:
		{
			SetPriorityIntT2(prio+T1_INT_PRIOR_0);
			break;
		}
		case hal_timerT3:
		{
			SetPriorityIntT3(prio+T1_INT_PRIOR_0);
			break;
		}
		case hal_timerT4:
		{
			SetPriorityIntT4(prio+T1_INT_PRIOR_0);
			break;
		}
		case hal_timerT5:
		{
			SetPriorityIntT5(prio+T1_INT_PRIOR_0);
			break;
		}
		
	}

}



extern void hal_timer_interrupt_ena(hal_timer_t timer)
{
	switch(timer)
	{
		case hal_timerT1:
		{
			EnableIntT1;
			break;
		}

		case hal_timerT2:
		{
			EnableIntT2;
			break;
		}

		case hal_timerT3:
		{
			EnableIntT3;
			break;
		}
		case hal_timerT4:
		{
			EnableIntT4;
			break;
		}

		case hal_timerT5:
		{
			EnableIntT5;
			break;
		}

	}
}


extern void hal_timer_interrupt_disa(hal_timer_t timer)
{
	switch(timer)
	{
		case hal_timerT1:
		{
			DisableIntT1;
			break;
		}

		case hal_timerT2:
		{
			DisableIntT2;
			break;
		}

		case hal_timerT3:
		{
			DisableIntT3;
			break;
		}
		case hal_timerT4:
		{
			DisableIntT4;
			break;
		}

		case hal_timerT5:
		{
			DisableIntT5;
			break;
		}

	}
}


void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    if(NULL != timer_list[hal_timerT1 -1].call_bkp_fn)
    {
        timer_list[hal_timerT1 -1].call_bkp_fn();
    }
    
    
    IFS0bits.T1IF = 0; // clear flag  
}
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
{
    if(NULL != timer_list[hal_timerT2 -1].call_bkp_fn)
    {
        timer_list[hal_timerT2 -1].call_bkp_fn();
    }

  IFS0bits.T2IF = 0; // clear flag  
}




void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void)
{
    if(NULL != timer_list[hal_timerT3 -1].call_bkp_fn)
        {
            timer_list[hal_timerT3 -1].call_bkp_fn();
        }
  

  IFS0bits.T3IF = 0; // clear flag  
}



void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void)
{
   if(NULL != timer_list[hal_timerT4 -1].call_bkp_fn)
    {
        timer_list[hal_timerT4 -1].call_bkp_fn();
    }

  IFS1bits.T4IF = 0; // clear flag  
}




void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(void)
{
   if(NULL != timer_list[hal_timerT5 -1].call_bkp_fn)
    {
        timer_list[hal_timerT5 -1].call_bkp_fn();
    }
  

  IFS1bits.T5IF = 0; // clear flag  
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



