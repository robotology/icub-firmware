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


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "osal.h"

#include "brd_mcbstm32x.h"      

#include "stdint.h"
#include "stdlib.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------



// the evt based task: when triggered by the periodic timer it blinks led 0 and sends a message to the msgbased task
extern void tsk_evtbased(void* p);

// the msg based task: when triggered by the evtbased task it blinks led 1 with time on whcih depends on teh received message
extern void tsk_evtbased(void* p);


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static void s_callback_1sec(osal_timer_t* tmr, void* arg);


//static void s_gotoerrormode(void);

static void s_launcher(void);
static void s_init(void);
static void s_stay(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static osal_timer_t* s_tmr1sec  = NULL;


static osal_messagequeue_t* s_mq1 = NULL;

static osal_task_t* s_tsk_evtbased = NULL;                        
static osal_task_t* s_tsk_msgbased = NULL;                        


static const uint32_t s_event_trigger = 0x00000010;

static const uint8_t countfive[] = {0, 1, 2, 3, 4}; 
    
// --------------------------------------------------------------------------------------------------------------------
// - declaration of externally defined functions or variable which dont have a .h file
// --------------------------------------------------------------------------------------------------------------------

// from the configuration
extern const osal_cfg_t *osal_cfg_USERptr;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

int main(void) 
{
    uint32_t ram64size;
    uint64_t *ram64data = NULL;
    


    // cmsis init
    SystemInit(); 
    // configuration of priority: not needed
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
    // init the leds
    brd_mcbstm32x_led_init();
    
    osal_base_memory_getsize(osal_cfg_USERptr, &ram64size);

    if(NULL != ram64data)
    {
        ram64data = (uint64_t*)osal_base_memory_new(ram64size);
    }
    
 
    osal_base_initialise(osal_cfg_USERptr, ram64data);

 
    osal_system_start(s_launcher);

    
    // never in here .... unless an error
    for(;;);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_launcher(void) 
{
    s_init();
 
    s_stay();
}





extern void tsk_evtbased(void* p) 
{
    uint32_t avail = 0;
    uint8_t i = 0;
    osal_result_t res;
    osal_eventflag_t rxevents = 0;
    for(;;) 
    {
        res = osal_eventflag_get(s_event_trigger, osal_waitALLflags, &rxevents, OSAL_reltimeINFINITE); 
        
 
        if(osal_res_OK == res)
        {
            rxevents = rxevents;
        }            
     
        brd_mcbstm32x_led_on(brd_mcbstm32x_led_0);
        osal_task_wait(50*OSAL_reltime1ms); 
        brd_mcbstm32x_led_off(brd_mcbstm32x_led_0);

    
        avail = osal_messagequeue_available(s_mq1, osal_callerTSK); //osal_callerAUTOdetect);
        if(avail > 0)
        {
            osal_messagequeue_put(s_mq1, (osal_message_t) &countfive[i], OSAL_reltimeZERO, osal_callerTSK); //osal_callerAUTOdetect);
            i++;
            if(i >= (sizeof(countfive) / sizeof(countfive[0])))
            {
                i = 0;
            }
        }
            
    }
}

extern void tsk_msgbased(void* p)
{
    volatile osal_message_t vv0 = NULL;
    volatile osal_result_t res;
    volatile uint8_t number = 1;

    for(;;)
    {
        res = osal_messagequeue_get(s_mq1, (osal_message_t *)&vv0, OSAL_reltimeINFINITE, osal_callerTSK); //osal_callerAUTOdetect);
        
        if(osal_res_OK == res)
        {
            number = (uint8_t) *((uint32_t*)vv0);
            number = number;
        }
        
        brd_mcbstm32x_led_on(brd_mcbstm32x_led_1);
        osal_task_wait((number+1)*50*OSAL_reltime1ms); 
        brd_mcbstm32x_led_off(brd_mcbstm32x_led_1);        
 
    }

}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// -------------------------------------------------------------------------------------------------------------------- 



static void s_callback_1sec(osal_timer_t* tmr, void* arg)
{
    osal_eventflag_set(s_event_trigger, s_tsk_evtbased, osal_callerTMRMAN); //osal_callerAUTOdetect); //osal_callerTMRMAN
}


// static void s_gotoerrormode(void)
// {
//     static volatile uint8_t aa = 0;
//     
//     for(;;)
//     {
//         aa++;
//         if(200 == aa)
//         {
//             aa = 0;
//         }
//     }
// }


static void s_init(void) 
{    
    osal_task_properties_t tskprop;
    

    osal_timer_timing_t   timing;
    osal_timer_onexpiry_t onexpiry;
    
    // create the evt based task. it will run when the scheduler allows it
    
    tskprop.function    = tsk_evtbased;
    tskprop.param       = NULL;
    tskprop.priority    = 50;
    tskprop.stacksize   = 256;
    s_tsk_evtbased      = osal_task_new1(&tskprop);    
    
    
    
    // create the mailbox used by the msg based task
 
    s_mq1 = osal_messagequeue_new(12);
    
    // create the msg based task. it will run when the scheduler allows it
    

    tskprop.function    = tsk_msgbased;
    tskprop.param       = NULL;
    tskprop.priority    = 60;
    tskprop.stacksize   = 256;
    s_tsk_msgbased      = osal_task_new1(&tskprop);   
    s_tsk_msgbased      = s_tsk_msgbased;    

    
    // create the timer 
    
    timing.startat = OSAL_abstimeNONE; timing.count = OSAL_reltime1sec; timing.mode = osal_tmrmodeFOREVER;
    onexpiry.cbk = s_callback_1sec; onexpiry.par = (void*)0x1000;
    s_tmr1sec = osal_timer_new();
    osal_timer_start(s_tmr1sec, &timing, &onexpiry, osal_callerTSK); //osal_callerAUTOdetect);

   
}

static void s_stay(void)
{
    uint32_t count  = 0;  
    
    osal_task_priority_set(osal_task_get(osal_callerTSK), 10);

    osal_task_period_set(5*OSAL_reltime1sec);

    for(;;)
    {
        osal_task_period_wait();
 
        if(0 == (++count % 2)) 
        {
            brd_mcbstm32x_led_off(brd_mcbstm32x_led_7);
        }
        else
        {
            brd_mcbstm32x_led_on(brd_mcbstm32x_led_7);
        }
        
    }

}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






