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

/* @file       eom_applbasic_specialise.c
	@brief      This file keeps the vcececew
	@author     marco.accame@iit.it
    @date       01/11/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EOVtheSystem.h"
#include "EOMtask.h"
#include "EOaction_hid.h"
#include "EOtheErrorManager.h"

#include "osal.h"
#include "hal.h"

#include "stdlib.h"
#include "stdio.h"



#include "eventviewer.h"

#include "EOtimer.h"
#include "EOaction.h"
#include "EOMtheCallbackmanager.h"

#include "EOtheLEDpulser.h"

#include "EOtheMemoryPool.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eom_applbasic_specialise.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

extern void task_eventdriven(void *p);
extern void task_periodic(void *p);
extern void task_messagedriven(void *p);


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

//#define MINIMAL_BEHAVIOUR


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static void s_specialise_blink(void *param);

static void s_eom_applbasic_specialise_tasks_start(void);

static void s_eom_applbasic_specialise_leds(void);

static void s_periodic_startup(EOMtask *p, uint32_t t);
static void s_periodic_run(EOMtask *p, uint32_t t);

static void s_eventdriven_startup(EOMtask *p, uint32_t t);
static void s_eventdriven_run(EOMtask *p, uint32_t t);

//static void s_messagedriven_startup(EOMtask *p, uint32_t t);
static void s_messagedriven_run(EOMtask *p, uint32_t t);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static EOtimer*     s_timer_blink = NULL;
static EOaction*    s_action_blink = NULL;


static EOMtask* s_task_periodic = NULL;
static EOMtask* s_task_eventdriven = NULL;
static EOMtask*  s_task_messagedriven = NULL;

static const eOevent_t s_event_from_task_periodic  = 0x00000004;
static const eOmessage_t s_message_from_task_eventbased  = 0x00000001;

#if     defined(MINIMAL_BEHAVIOUR)
static const char s_eobj_ownname[] = "EOMapplBASICminimal";
#else
static const char s_eobj_ownname[] = "EOMapplBASIC";
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

#include "EOvector.h"
#include "EOlist.h"

typedef struct
{
    uint32_t v1;
    uint16_t v2;   
    uint64_t v3;
} samplestruct; EO_VERIFYsizeof(samplestruct, 16);

eOresult_t item_init(void* p, uint32_t u)
{
    samplestruct* ss = (samplestruct*)p;
    ss->v1 = u+1;
    ss->v2 = u+2;
    ss->v3 = u+3; 
    return(eores_OK);
}

eOresult_t item_copy(void* p, void* e)
{
    samplestruct* ss = (samplestruct*)p;
    samplestruct* ee = (samplestruct*)e;
    ss->v1 = ee->v1;
    ss->v2 = ee->v2;
    ss->v3 = ee->v3;  
    return(eores_OK);
}

eOresult_t item_clear(void* p)
{
    samplestruct* ss = (samplestruct*)p;
    ss->v1 = 0;
    ss->v2 = 0;
    ss->v3 = 0;   
    return(eores_OK);
}

extern void eom_applbasic_specialise_system(void)
{
#if 0    
    // tests
    void* p01 = eo_mempool_New(eo_mempool_GetHandle(), 1);
    void* p02 = eo_mempool_New(eo_mempool_GetHandle(), 2);
    void* p03 = eo_mempool_New(eo_mempool_GetHandle(), 3);
    void* p04 = eo_mempool_New(eo_mempool_GetHandle(), 4);
    void* p07 = eo_mempool_New(eo_mempool_GetHandle(), 7);
    void* pxx = eo_mempool_New(eo_mempool_GetHandle(), 12);
    pxx = eo_mempool_Realloc(eo_mempool_GetHandle(), pxx, 10);
    pxx = eo_mempool_Realloc(eo_mempool_GetHandle(), pxx, 12);
    pxx = eo_mempool_Realloc(eo_mempool_GetHandle(), pxx, 15);
    pxx = eo_mempool_Realloc(eo_mempool_GetHandle(), pxx, 10);
   
    eo_mempool_Delete(eo_mempool_GetHandle(), p01);
    eo_mempool_Delete(eo_mempool_GetHandle(), p02);
    eo_mempool_Delete(eo_mempool_GetHandle(), p07);
    p01 = eo_mempool_New(eo_mempool_GetHandle(), 1);
#endif 
    
//    void* p04 = eo_mempool_New(eo_mempool_GetHandle(), 4);
//    p04 = eo_mempool_Realloc(eo_mempool_GetHandle(), p04, 7);
//    p04 = eo_mempool_Realloc(eo_mempool_GetHandle(), p04, 0);
//    p04 = p04;
//    eo_mempool_Delete(eo_mempool_GetHandle(), p04);
//    p04 = p04;


    EOvector *v = eo_vector_New(sizeof(samplestruct), eo_vectorcapacity_dynamic, item_init, 1, item_copy, item_clear);
    
    samplestruct s;
    s.v1 = 11;
    s.v2 = 12;
    s.v3 = 13;
    eo_vector_PushBack(v, &s);
    s.v1 = 21;
    s.v2 = 22;
    eo_vector_PushBack(v, &s);
    
    samplestruct* sp = NULL;
    sp = (samplestruct*) eo_vector_Back(v);
    eo_vector_PopBack(v);
    sp = (samplestruct*) eo_vector_Back(v);
    
    samplestruct items[2] =  {0};
    items[0].v1 = 30;
    items[1].v1 = 31;
    
    eo_vector_Assign(v, 4, items, 2);
    uint16_t size = eo_vector_Size(v);
    
    sp = (samplestruct*) eo_vector_Back(v);
    sp = sp;
    
    eo_vector_Resize(v, 10);
    size = eo_vector_Size(v);
    
    sp = (samplestruct*) eo_vector_Back(v);
    sp = sp;
    
    eo_vector_Delete(v);
    

    uint16_t cap = eo_vectorcapacity_dynamic;
    EOlist *l = eo_list_New(sizeof(samplestruct), cap, item_init, 1, item_copy, item_clear);
    
    //samplestruct s;
    s.v1 = 11;
    s.v2 = 12;
    s.v3 = 13;
    eo_list_PushBack(l, &s);
    s.v1 = 21;
    s.v2 = 22;
    eo_list_PushBack(l, &s);
    
    //samplestruct* sp = NULL;
    sp = (samplestruct*) eo_list_Back(l);
    eo_list_PopFront(l);
    sp = (samplestruct*) eo_list_Front(l);
    
    size = eo_list_Size(l);
    
//    samplestruct items[2] =  {0};
    items[0].v1 = 30;
    items[1].v1 = 31;
    
    eo_list_Insert(l, eo_list_Begin(l), &items[0]);
    //uint16_t 
    size = eo_list_Size(l);
    
    sp = (samplestruct*) eo_list_Front(l);
    sp = sp;
    
    sp = (samplestruct*) eo_list_Back(l);
    sp = sp;
    
    
    sp = (samplestruct*) eo_list_Back(l);
    sp = sp;
    
    eo_list_Delete(l);    
    
    
    
    // -- service 1:    init the leds.
    //                  led0, led1, led2 with the LEDpulser. led3, led4, and led5 manually
    //                  led0 will blink forever at 1Hz
    //                  led1 will pulse 10 times at 20Hz inside periodic task
    //                  led2 will pulse 5 times at 10Hz inside teh event-driven task
    //                  led3, led4, and led5 will ...
    
    s_eom_applbasic_specialise_leds();
    
    
    // -- service 2:    starts two tasks: one periodic andm one event-driven. 
    //                  the periodic pulses led1, prints with the errormanager and wakes up the event-driven
    //                  the event driven pulses led2 and prints with the error manager.

    s_eom_applbasic_specialise_tasks_start();  
    
    char str[64] = "";
    snprintf(str, sizeof(str), "EOtheMemoryPool has used %d bytes so far", eo_mempool_SizeOfAllocated(eo_mempool_GetHandle()));
    eo_errman_Info(eo_errman_GetHandle(),  "init task", str);

}



static void s_eom_applbasic_specialise_leds(void)
{
    
#if     defined(MINIMAL_BEHAVIOUR)

    // init all leds
    hal_led_init(hal_led0, NULL);
    hal_led_init(hal_led1, NULL);
    hal_led_init(hal_led2, NULL);
    hal_led_init(hal_led3, NULL);
    hal_led_init(hal_led4, NULL);
    hal_led_init(hal_led5, NULL);
    hal_led_init(hal_led6, NULL);
    hal_led_init(hal_led7, NULL);

#else    
   
    // init the ledpulser with leds 0 and 1 and 2 and 3
    
    eOledpulser_cfg_t ledpulsercfg = 
    {
        .led_enable_mask    = (1 << eo_ledpulser_led_zero) | 
                              (1 << eo_ledpulser_led_one)  | 
                              (1 << eo_ledpulser_led_two)  |
                              (1 << eo_ledpulser_led_three),
        .led_init           = (eOint8_fp_uint8_cvoidp_t)hal_led_init,
        .led_on             = (eOint8_fp_uint8_t)hal_led_on,
        .led_off            = (eOint8_fp_uint8_t)hal_led_off,
        .led_toggle         = (eOint8_fp_uint8_t)hal_led_toggle
    };
    
    // each active led will have at timer. we can set it on, off, pulse. 
    eo_ledpulser_Initialise(&ledpulsercfg);  
        
    // start a pulse on led0: infinite with 1 Hz period
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero, 1*eok_reltime1sec, 0);
    
    // led1 will be driven by the periodic task 
    // led2 will be driven by teh event driven task
           
    // init the other leds
    hal_led_init(hal_led4, NULL);
    hal_led_init(hal_led5, NULL);
    hal_led_init(hal_led6, NULL);
    hal_led_init(hal_led7, NULL);
    
    // use leds 4, 5 with a single timer expirying every 2 sec
    
    s_timer_blink = eo_timer_New();
    s_action_blink = eo_action_New();
    eo_action_SetCallback(s_action_blink, 
                          s_specialise_blink, NULL,
                          eom_callbackman_GetTask(eom_callbackman_GetHandle())
                         );
    
    eo_timer_Start(s_timer_blink, eok_abstimeNOW, 2*eok_reltime1sec, eo_tmrmode_FOREVER, s_action_blink);    
    
#endif        
}



static void s_eom_applbasic_specialise_tasks_start(void)
{

    s_task_periodic = eom_task_New(     eom_mtask_Periodic,                 // the type of task: periodic
                                        101,                                // its priority. higher is more priority
                                        2*1024,                             // its dedicated stack size
                                        s_periodic_startup,                 // function executed only once at startup
                                        s_periodic_run,                     // functione executed periodically  
                                        EOK_uint32dummy,                    // not relevant for periodic tasks
                                        5*eok_reltime1sec+33*eok_reltime1ms,// the period
                                        NULL,                               // some memory associated to the task: nothing in our application
                                        task_periodic, "periodicdriven"     // used by microvision
                                        );

    s_task_periodic = s_task_periodic;
    
    
    s_task_eventdriven = eom_task_New(  eom_mtask_EventDriven,              // the type of task: event driven
                                        100, 
                                        2*1024, 
                                        s_eventdriven_startup,              // function executed only once at startup
                                        s_eventdriven_run,  
                                        EOK_uint32dummy,                    // not relevant for eventdriven tasks
                                        eok_reltimeINFINITE,                // the timeout on waiting for an event
                                        NULL, 
                                        task_eventdriven, "eventdriven"
                                        );

    s_task_eventdriven = s_task_eventdriven;    

}  



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


extern void task_periodic(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 

extern void task_eventdriven(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 

extern void task_messagedriven(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------





static void s_periodic_startup(EOMtask *p, uint32_t t)
{
       
}

static void s_periodic_run(EOMtask *p, uint32_t t)
{   
    static uint32_t num = 0;
    char str[64];
    
    snprintf(str, sizeof(str), "periodic task executed %d times", ++num);
    eo_errman_Info(eo_errman_GetHandle(), s_eobj_ownname, str);

#if     defined(MINIMAL_BEHAVIOUR)
    hal_led_toggle(hal_led1);
#else    
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_one, 50*eok_reltime1ms, 10);
#endif
        
    eom_task_SetEvent(s_task_eventdriven, s_event_from_task_periodic);
}


static void s_eventdriven_startup(EOMtask *p, uint32_t t)
{
    // tasks can be created also inside other tasks after that the init task has expired
    
    s_task_messagedriven = eom_task_New(eom_mtask_MessageDriven,          // the type of task: message driven
                                        99, 
                                        2*1024, 
                                        NULL,                               // function executed only once at startup: YES IT CAN BE NULL
                                        s_messagedriven_run,  
                                        4,                                  // size of internal receiving message queue
                                        eok_reltimeINFINITE,                // the timeout on waiting for a message
                                        NULL, 
                                        task_messagedriven, "messagedriven"
                                        );

    s_task_messagedriven = s_task_messagedriven;      
       
}

static void s_eventdriven_run(EOMtask *p, uint32_t t)
{
    // the event that we have received
    eOevent_t evt = (eOevent_t)t;

    if(eobool_true == eo_common_event_check(evt, s_event_from_task_periodic))
    {
#if     defined(MINIMAL_BEHAVIOUR)
        hal_led_toggle(hal_led2);
#else         
        eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_two,  100*eok_reltime1ms, 5);
#endif
        static uint32_t num = 0;
        char str[64];
        
        snprintf(str, sizeof(str), "event-driven task executed %d times", ++num);
        eo_errman_Info(eo_errman_GetHandle(), s_eobj_ownname, str);
        
        // now send a message to task message driven
        eom_task_SendMessage(s_task_messagedriven, s_message_from_task_eventbased, eok_reltimeINFINITE);
    }
    
}



static void s_messagedriven_run(EOMtask *p, uint32_t t)
{
    // the message that we have received
    eOmessage_t msg = (eOmessage_t)t;

    if(s_message_from_task_eventbased == msg)
    {
#if     defined(MINIMAL_BEHAVIOUR)
        hal_led_toggle(hal_led3);
#else         
        eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three,  50*eok_reltime1ms, 10);
#endif
        static uint32_t num = 0;
        char str[64];
        
        snprintf(str, sizeof(str), "message-driven task executed %d times", ++num);
        eo_errman_Info(eo_errman_GetHandle(), s_eobj_ownname, str);
    }
    
}


static void s_specialise_blink(void *param)
{
    hal_led_toggle(hal_led4); 
    hal_led_toggle(hal_led5); 
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



