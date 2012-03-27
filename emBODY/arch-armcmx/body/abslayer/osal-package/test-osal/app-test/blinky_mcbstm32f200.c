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
#include "stm32f2.h"  

#include "led_mcbstm32f200.h"    

   


#include <stdint.h>

#include "stdlib.h"
#include "string.h"





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

const uint16_t *myused = NULL;
const uint16_t *myfree = NULL;

uint16_t osal_free[7] = {0};
uint16_t osal_used[7] = {0};






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
  
    led_mcbstm32f200_init(); 
  
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

    sem = osal_semaphore_new(5, 0);

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


    mtx = osal_mutex_new();

    osal_info_entities_get_stats(&myused, &myfree);
    memcpy(osal_used, myused, sizeof(osal_used));
    memcpy(osal_free, myfree, sizeof(osal_free));
}



void mytask(void *p)
{
    static uint32_t mm = 0;
    static u32 aa[20] = {0};
    static volatile osal_result_t res;
    static void *pp = NULL;

    static uint8_t val = 0;


    p = p;

    res = osal_semaphore_increment(sem);

    aa[0] = aa[0];

    pp = (void*)calloc(32, 1);



    for(;;)
    {
        osal_task_wait(1000*1000);
        if(0 == val)
        {
            val = 1;
            led_mcbstm32f200_on(led_mcbstm32f200_0);
        }
        else
        {
            val = 0;
            led_mcbstm32f200_off(led_mcbstm32f200_0);
        }

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



