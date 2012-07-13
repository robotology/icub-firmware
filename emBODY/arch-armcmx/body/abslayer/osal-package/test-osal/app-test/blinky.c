/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

#include "osal.h"
#include "stm32f1.h"      

   


#include <stdint.h>

#include "stdlib.h"
#include "string.h"



static osal_task_t *tsk200 = NULL;
static osal_task_t *tsk201 = NULL;
static osal_task_t *tsk202 = NULL;
static osal_task_t *tsk203 = NULL;


static osal_task_t *mytsk = NULL;
static osal_task_t *yotsk = NULL;
static osal_task_t *wo0tsk = NULL;
static osal_task_t *wo1tsk = NULL;
static osal_task_t *sitsk = NULL;
static osal_mutex_t *mtx = NULL;

static osal_semaphore_t *sem = NULL;



void mytask(void *p);
void yotask(void *p);
void sitask(void *p);
void wo0task(void *p);
void wo1task(void *p);
void wotask(void *p);

void f200task(void *p);
void f201task(void *p);
void f202task(void *p);
void f203task(void* p);

const uint16_t *myused = NULL;
const uint16_t *myfree = NULL;

uint16_t osal_free[7] = {0};
uint16_t osal_used[7] = {0};





extern void SetupLED(void);
extern void LED_On(uint16_t led);
extern void LED_Off(uint16_t led);
extern void LED_toggle(void);

void init(void);

void mytask(void *p);
void yotask(void *p);
void wotask(void *p);
void wo0task(void *p);
void wo1task(void *p);
void sitask(void *p);






int main(void) 
{

    extern const osal_cfg_t *osal_cfgMINE;

 
    uint32_t ram64size;
    uint64_t *ram64data = NULL;





    SystemInit(); 
  
    SetupLED(); 
  
    osal_base_memory_getsize(osal_cfgMINE, &ram64size);

    ram64data = (uint64_t*)calloc(ram64size/8, sizeof(uint64_t));


    osal_base_initialise(osal_cfgMINE, ram64data);

    osal_info_entities_get_stats(&myused, &myfree);
    memcpy(osal_used, myused, sizeof(osal_used));
    memcpy(osal_free, myfree, sizeof(osal_free));

  

    osal_system_start(init);                  
}


void init(void) 
{
    uint8_t run_test_4_bug_scheduling_disable_testnumber = 0;

    sem = osal_semaphore_new(5, 0);

	osal_task_wait(5000);

    mytsk = osal_task_new(mytask, (void*)0x12345678, 20, 128);
    mytsk = mytsk;
    yotsk = osal_task_new(yotask, (void*)0x0, 30, 128);
    yotsk = yotsk;
    sitsk = osal_task_new(sitask, (void*)0x0, 40, 128);
    sitsk = sitsk;
    wo0tsk = osal_task_new(wo0task, (void*)0x00000000, 5, 128);
    wo0tsk = wo0tsk;
    wo1tsk = osal_task_new(wo1task, (void*)0x10000000, 5, 128);
    wo1tsk = wo1tsk;

    //osal_timer_new(1*100, tmr_period, s_toggle_led_cbk, (void*)&s_gpiotmr);


    run_test_4_bug_scheduling_disable_testnumber = 3;
    if(1 == run_test_4_bug_scheduling_disable_testnumber)
    {
        tsk200 = osal_task_new(f200task, (void*)0x0, 200, 128);
        tsk201 = osal_task_new(f201task, (void*)0x0, 201, 128);
    }
    else if(2 == run_test_4_bug_scheduling_disable_testnumber)
    {
        tsk200 = osal_task_new(f200task, (void*)0x0, 205, 128);
        tsk202 = osal_task_new(f202task, (void*)0x0, 202, 128);
    }
    else if(3 == run_test_4_bug_scheduling_disable_testnumber)
    {
        tsk200 = osal_task_new(f200task, (void*)0x0, 205, 128);
        tsk203 = osal_task_new(f203task, (void*)0x0, 202, 128);
    }
    mtx = osal_mutex_new();

    osal_info_entities_get_stats(&myused, &myfree);
    memcpy(osal_used, myused, sizeof(osal_used));
    memcpy(osal_free, myfree, sizeof(osal_free));
}

void f200task(void* p)
{
    static uint8_t mmm = 0;
    static osal_eventflag_t evt = 0;

    for(;;)
    {
        evt = osal_eventflag_getany(osal_reltimeINFINITE);

        mmm ++;
        mmm = mmm;
    }    


}


void f201task(void* p)
{
    // this task shows that if we susepnd scheduling and then we put in hold teh scheduler, then
    // a context swicth may occur that hangs the system as control goes to task 200 and then to
    // task worker without restoring the scheduling.
    // NASTY BEHAVIOUR.
    #warning --> during suspended scheduling never and ever make osal_* calls which put the task on hold.

    static uint8_t mmm = 0;


    osal_system_scheduling_suspend();

    osal_eventflag_set(0x00000001, tsk200, osal_callerTSK);
//    osal_eventflag_set(0x00000001, tsk200, osal_callerTSK);

    for(;;)
    {

        osal_task_wait(1000*1000);

        mmm ++;
        mmm = mmm;
    }    


}


void f202task(void* p)
{
    // this task shows that if we susepnd scheduling and then we put in hold teh scheduler, then
    // a context swicth may occur that hangs the system as control goes to task 200 without restoring
    // the scheduling. NASTY BEHAVIOUR.

    static uint8_t mmm = 0;
    static uint32_t nn = 0;
    uint32_t i = 0;


    osal_system_scheduling_suspend();

    osal_eventflag_set(0x00000001, tsk200, osal_callerISR);

    //osal_system_scheduling_restart();

        for(i=0; i<8000000; i++)
        {
            mmm ++;
            mmm = mmm;
        }

    for(;;)
    {
        
        for(i=0; i<8000000; i++)
        {
            mmm ++;
            mmm = mmm;
        }

        osal_task_wait(1000*1000);
        //osal_task_wait(0);
        nn++;
        nn = nn;

    }    


}

void f203task(void* p)
{
    // this task shows that if we susepnd scheduling and then we put in hold teh scheduler, then
    // a context swicth may occur that hangs the system as control goes to task 200 without restoring
    // the scheduling. NASTY BEHAVIOUR.

    static uint8_t mmm = 0;
    static uint32_t nn = 0;
    uint32_t i = 0;
	osal_abstime_t tim;

	volatile uint32_t cnt0 = 0;
	volatile uint32_t cnt1 = 0;

	tim = osal_system_abstime_get();

	cnt0 = *((volatile uint32_t*)(0xe000e018));


    osal_system_scheduling_suspend();

	cnt0 = *((volatile uint32_t*)(0xe000e018));
	for(;;)
	{
	 	cnt1 = *((volatile uint32_t*)(0xe000e018)); 
		if(cnt1 > cnt0)
		{
		 	break;
		}
	}

	cnt0 = *((volatile uint32_t*)(0xe000e018));
	for(;;)
	{
	 	cnt1 = *((volatile uint32_t*)(0xe000e018)); 
		if(cnt1 > cnt0)
		{
		 	break;
		}
	}

	osal_system_scheduling_restart();


    cnt0 = *((volatile uint32_t*)(0xe000e018));
	for(;;)
	{
	 	cnt1 = *((volatile uint32_t*)(0xe000e018)); 
		if(cnt1 > cnt0)
		{
		 	break;
		}
	}

    for(;;)
    {
        
        for(i=0; i<8000000; i++)
        {
            mmm ++;
            mmm = mmm;
        }

        //osal_task_wait(1000*1000);
        //osal_task_wait(0);
        nn++;
        nn = nn;

    }    


}

void mytask(void *p)
{
    static uint32_t mm = 0;
    static u32 aa[20] = {0};
    static volatile osal_result_t res;
    static void *pp = NULL;


    p = p;

    res = osal_semaphore_increment(sem, osal_callerTSK);

    aa[0] = aa[0];

    pp = (void*)calloc(32, 1);



    for(;;)
    {
        osal_task_wait(1000*1000);
        LED_toggle();

        res = osal_mutex_take(mtx, 0);
        mm ++;
        osal_eventflag_set(0x00010000, sitsk, osal_callerTSK);
    }

}



void yotask(void *p)
{
    static uint32_t nn = 0;
    osal_result_t res;

    p = p;

    res = osal_semaphore_decrement(sem, 0xffffffff);

    for(;;)
    {
       osal_task_wait(500*1000);
        nn ++;
    }

}



void wotask(void *p)
{
    volatile uint32_t nn = 0;
    volatile uint32_t pp = 0;

    p = p;

    pp = (uint32_t)p;
    pp = pp;

    nn = pp;

    for(;;)
    {
       nn ++;
       nn = nn;
    }

}

void wo0task(void *p)
{
    wotask(p);
}


void wo1task(void *p)
{
    wotask(p);
}

void sitask(void *p)
{
    volatile uint32_t mm = 0;
    osal_eventflag_t rxmsk = 0;
    osal_result_t res;

    p = p;

    for(;;)
    {
       res = osal_eventflag_get(0x00111000, osal_waitANYflag, &rxmsk, OSAL_reltimeINFINITE); 
       //res = osal_eventflag_get(0x00010000, osal_waitALLflags, &rxmsk, 550*1000);
       mm ++;
       res = res;
       res = osal_eventflag_get(0x00111000, osal_waitANYflag, &rxmsk, OSAL_reltimeZERO);
       res = res;
       mm = mm;
    }

}

// eof



