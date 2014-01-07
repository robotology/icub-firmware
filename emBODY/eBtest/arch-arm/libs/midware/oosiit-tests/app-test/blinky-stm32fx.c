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

// include the cmsis api file which is relevant to the used mpu
#if     defined(HL_USE_BRD_MCBSTM32)
    #include "cmsis_stm32f1.h"
#elif   defined(HL_USE_BRD_MCBSTM32_C)
    #include "cmsis_stm32f1.h"
#elif   defined(HL_USE_BRD_EMS001)
    #include "cmsis_stm32f1.h"
#elif   defined(HL_USE_BRD_MCBSTM32_F200)
    #include "cmsis_stm32f2.h"
#elif   defined(HL_USE_BRD_MCBSTM32_F400)   
    #include "cmsis_stm32f4.h" 
#else
    #error --> define a MCBSTM32 board
#endif


#include "board.h"  

#include "oosiit.h"

  

#include "stdint.h"
#include "stdlib.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

// the idle task as seen in uv4
extern void oosiit_idle(void* p);

// the init task as seen in uv4: after initialisation of all oosiit entities it enters in periodic mode and
// blinks led 7 every 5 seconds  
extern void oosiit_init(void* p);

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


static void s_callback_1sec(void* tmr, void* arg);


static void s_gotoerrormode(void);

static void s_init(void* p);
static void s_stay(void* p);
static void s_away(void* p);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static oosiit_objptr_t s_tmr1sec  = NULL;


static oosiit_objptr_t s_mailbox1 = NULL;

static oosiit_tskptr_t s_tskid_evtbased = 0;                        
static oosiit_tskptr_t s_tskid_msgbased = 0;                        


static const uint32_t s_event_trigger = 0x00000010;

static const uint8_t countfive[] = {0, 1, 2, 3, 4}; 
    
// --------------------------------------------------------------------------------------------------------------------
// - declaration of externally defined functions or variable which dont have a .h file
// --------------------------------------------------------------------------------------------------------------------

// from the configuration
extern const oosiit_cfg_t *oosiit_cfg_USERptr;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

int main(void) 
{
    uint16_t ram32num;
    uint16_t ram64num;
    uint32_t *ram32data = NULL;
    uint64_t *ram64data = NULL;
    uint64_t *inittskstackdata = NULL;
    uint16_t inittskstacksize = 0;
    uint64_t *idletskstackdata = NULL;
    uint16_t idletskstacksize = 0;    
    


    // cmsis init
    SystemInit(); 
    // configuration of priority: not needed
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
    // init the leds
    board_led_init();
    
    oosiit_memory_getsize(oosiit_cfg_USERptr, &ram32num, &ram64num);

//     ram32data = (uint32_t*)calloc(ram32num/4, sizeof(uint32_t));
//     ram64data = (uint64_t*)calloc(ram64num/8, sizeof(uint64_t));

    ram32data = (0 == ram32num) ? (NULL) : ((uint32_t*)oosiit_memory_new(ram32num));
    ram64data = (0 == ram64num) ? (NULL) : ((uint64_t*)oosiit_memory_new(ram64num));
    
    oosiit_memory_load(oosiit_cfg_USERptr, ram32data, ram64data);
    

    inittskstacksize = 256;
    inittskstackdata = oosiit_memory_getstack(inittskstacksize);
    if(NULL == inittskstackdata)
    {
        s_gotoerrormode();
    }    

    oosiit_task_properties_t tskinit =
    {
        .function       = oosiit_init,
        .param          = (void*) 0x11, 
        .priority       = 249,
        .stacksize      = inittskstacksize,
        .stackdata      = inittskstackdata
    };
    
    idletskstacksize = 256;
    idletskstackdata = oosiit_memory_getstack(idletskstacksize);
    if(NULL == idletskstackdata)
    {
        s_gotoerrormode();
    }  
    
    oosiit_task_properties_t tskidle =
    {
        .function       = oosiit_idle,
        .param          = (void*) countfive,
        .priority       = 0,
        .stacksize      = 256,
        .stackdata      = idletskstackdata        
    };
    oosiit_sys_start(&tskinit, &tskidle);

    
    // never in here .... unless an error
    for(;;);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void oosiit_idle(void *p) 
{
    uint32_t *arrayof10 = (uint32_t*)p;
    uint32_t i = 0;
    for(;;) 
    {
        // idle
        p = p;
        if(arrayof10[2] == 2)
        {
            i++;
        }
    }
}

extern void oosiit_init(void* p) 
{
    static volatile int i = 0;
    s_init(p);
    
    i++;
 
    s_stay(p);
    s_away(p);
}




extern void *__user_perthread_libspace (void);
extern void tsk_evtbased(void* p) 
{
    volatile uint32_t sig = 0;
    uint32_t avail = 0;
    uint8_t i = 0;
    
    
    static volatile void* pp = NULL;
    
    pp = __user_perthread_libspace();
    
    pp = pp;
    
    pp = __user_perthread_libspace();
    
    pp = pp;    
    
    pp = oosiit_tsk_get_extdata(oosiit_tsk_self());
    
    oosiit_tsk_set_extdata(oosiit_tsk_self(), (void*)0x12345678);
    
    pp = oosiit_tsk_get_extdata(oosiit_tsk_self());
    
    oosiit_tsk_set_extdata(oosiit_tsk_self(), (void*)0x11111111);
    
    pp = oosiit_tsk_get_extdata(oosiit_tsk_self());
    
    for(;;) 
    {
        oosiit_evt_wait(s_event_trigger, OOSIIT_NOTIMEOUT, oosiit_evt_wait_mode_all); 
        sig = oosiit_evt_get();
        sig = sig;
        board_led_on(board_led_0);
        oosiit_dly_wait(50); 
        board_led_off(board_led_0);

        avail = oosiit_mbx_available(s_mailbox1);
        if(avail > 0)
        {
            oosiit_mbx_send(s_mailbox1, (void*) &countfive[i], 0);
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
    volatile uint32_t *vv0 = NULL;
    volatile oosiit_result_t res;
    volatile uint8_t number = 1;

    for(;;)
    {
        res = oosiit_mbx_retrieve(s_mailbox1, (void**)&vv0, OOSIIT_NOTIMEOUT);
        
        if((oosiit_res_OK == res) || (oosiit_res_MBX == res))
        {
            number = (uint8_t) *vv0;
            number = number;
        }
        
        board_led_on(board_led_1);
        oosiit_dly_wait((number+1)*50); 
//        oosiit_dly_wait(50); 
        board_led_off(board_led_1);        
 
    }

}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// -------------------------------------------------------------------------------------------------------------------- 



static void s_callback_1sec(void* tmr, void* arg)
{
    oosiit_evt_set(s_event_trigger, s_tskid_evtbased);
}


static void s_gotoerrormode(void)
{
    static volatile uint8_t aa = 0;
    
    for(;;)
    {
        aa++;
        if(200 == aa)
        {
            aa = 0;
        }
    }
}


static void s_init(void* p) 
{    
    uint16_t  tskstacksize = 256;
    uint64_t* tskstackdata = NULL;
    oosiit_task_properties_t tskprop;
    
    oosiit_advtmr_timing_t timing;
    oosiit_advtmr_action_t action;
    
    // create the evt based task. it will run when the scheduler allows it
    
    tskstacksize = 256;
    tskstackdata = oosiit_memory_getstack(tskstacksize);
    if(NULL == tskstackdata)
    {
        s_gotoerrormode();
    }
    tskprop.function    = tsk_evtbased;
    tskprop.param       = NULL;
    tskprop.priority    = 50;
    tskprop.stacksize   = tskstacksize;
    tskprop.stackdata   = tskstackdata;
    tskprop.extdata     = (void*)0x66667777;
    s_tskid_evtbased    = oosiit_tsk_create(&tskprop);    
    
    
    
    // create the mailbox used by the msg based task
 
    s_mailbox1 = oosiit_mbx_create(12);
    
    // create the msg based task. it will run when the scheduler allows it
    
    tskstacksize = 256;
    tskstackdata = oosiit_memory_getstack(tskstacksize);
    if(NULL == tskstackdata)
    {
        s_gotoerrormode();
    }
    tskprop.function    = tsk_msgbased;
    tskprop.param       = NULL;
    tskprop.priority    = 60;
    tskprop.stacksize   = tskstacksize;
    tskprop.stackdata   = tskstackdata;
    s_tskid_msgbased    = oosiit_tsk_create(&tskprop);   
    s_tskid_msgbased = s_tskid_msgbased;    

    
    // create the timer 
    
    timing.startat = OOSIIT_ASAPTIME; timing.firstshot = 1000; timing.othershots = 1000;
    action.cbk = s_callback_1sec; action.par = (void*)0x1000;
    s_tmr1sec = oosiit_advtmr_new();
    oosiit_advtmr_start(s_tmr1sec, &timing, &action);

   
}


extern void *__user_perthread_libspace (void);
static void s_stay(void* p)
{
    uint32_t count  = 0; 
    uint32_t xxx = 0;
    static volatile void *pp = NULL;    
    
    oosiit_tsk_setprio(oosiit_tsk_self(), 10);

    oosiit_itv_set(2000);
    
    pp = __user_perthread_libspace();
    
    pp = pp;
    
    pp = __user_perthread_libspace();
    
    pp = pp;
    
    oosiit_objptr_t mut = oosiit_mut_create();
    oosiit_mut_wait(mut, 0);

    for(;;)
    {
        oosiit_itv_wait();
        if(oosiit_memmode_dynamic == oosiit_cfg_USERptr->memorymode)
        {   // we can keep on creating mutexes only if we have dynamic mode ...
            if(NULL != mut)
            {
                oosiit_mut_delete(mut);
                //oosiit_mut_wait(mut, 0);
                mut = NULL;
                mut = oosiit_mut_create();
                if(NULL != mut)
                    oosiit_mut_wait(mut, 0);
            }
            else
            {
                xxx++;
            }
        }
 
        if(0 == (++count % 2)) 
        {
            board_led_off(board_led_7);
        }
        else
        {
            board_led_on(board_led_7);
        }
        
    }

}

static void s_away(void* p)
{
    static volatile int i=0;
    
    oosiit_tsk_delete(oosiit_tsk_self());
    i++;
    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






