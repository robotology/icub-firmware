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
#elif   defined(HL_USE_BRD_MCBSTM32F400)   
    #include "cmsis_stm32f4.h" 
#else
    #error --> define a MCBSTM32 board
#endif


#include "board.h"  
  

#include "stdint.h"
#include "stdlib.h"

#include "eventviewer.h"

#include "osal.h"

#include "test-class.h"
#include "test-singleton.h"
#include "test-namespaces.h"

#include <new>

#include <cstring>


#ifdef __cplusplus
extern "C" {
#endif
    extern const osal_cfg_t *osal_cfgMINEX;
#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------



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

static void s_osal_init(void);

static void s_osal_starter(void);

void task_led(void *);

void task_wrk(void *param);

void wedobusinessinhere(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static osal_task_t * s_taskLED = NULL;
static osal_task_t * s_taskWRK = NULL;

static const osal_eventflag_t s_event_TICK = 0x00000001;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of externally defined functions or variable which dont have a .h file
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

int main(void) 
{

    // cmsis init
    SystemInit(); 
    // configuration of priority: not needed
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
    // init the leds
    board_led_init();
     
    // init the event viewer.
    eventviewer_init(); 
    
    s_osal_init();
        
    
    osal_system_start(s_osal_starter);
   
    
    // never in here .... unless an error
    for(;;);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// -------------------------------------------------------------------------------------------------------------------- 

static void s_osal_init(void)
{
    uint32_t ram08size = 0;
    uint64_t *ram08data = NULL;
    
    osal_base_memory_getsize(osal_cfgMINEX, &ram08size);
    
    if(0 != ram08size)
    {
        // what about osal_base_memory_new() ???? yes, we can use it
        ram08data = (uint64_t*)(uint64_t*)osal_base_memory_new(ram08size);
    }

    osal_base_initialise(osal_cfgMINEX, ram08data);      
  
}


static void s_osal_starter(void)
{
    // launch some tasks.
    osal_task_properties_t tskprop = {0};
    
    // 1. blink a led and send a tick event to task worker
    
    tskprop.function = task_led;
    tskprop.param = NULL;
    tskprop.priority = 50;
    tskprop.stacksize = 1024;
    
    s_taskLED = osal_task_new1(&tskprop);
    s_taskLED = s_taskLED;
    
    // 2. start task worker
    
    tskprop.function = task_wrk;
    tskprop.param = NULL;
    tskprop.priority = 60;
    tskprop.stacksize = 2*1024;
    
    s_taskWRK = osal_task_new1(&tskprop); 
    s_taskWRK = s_taskWRK;    

    return;
}


void task_led(void *param)
{    
    // do init only once.
    
    // set periodic task at 2 hz
    osal_task_period_set(500*OSAL_reltime1ms);
    
    static uint8_t led0ON = 0;
    
    
    // enter forever loop
    for(;;)
    {
        // wait ...        
        osal_task_period_wait();      

        // toggle led
        if(0 == led0ON)
        {
            led0ON = 1;
            board_led_on(board_led_0);
        }
        else
        {
            led0ON = 0;
            board_led_off(board_led_0);
        }
        
        // alert another task
        osal_eventflag_set(s_event_TICK, s_taskWRK, osal_callerAUTOdetect);        
    }
       
}


void task_wrk(void *param)
{    
    // do init only once.
    
    const osal_reltime_t tout = 10*OSAL_reltime1sec;
    osal_eventflag_t flags = 0;
    static uint8_t led1ON = 0;
    
    // enter forever loop
    for(;;)
    {
        // wait an event        
        flags = osal_eventflag_getany(tout);      

        if(s_event_TICK == (s_event_TICK & flags))
        {
            // do it ....
            // toggle led
            if(0 == led1ON)
            {
                led1ON = 1;
                board_led_on(board_led_1);
            }
            else
            {
                led1ON = 0;
                board_led_off(board_led_1);
            }  

            wedobusinessinhere();
            
        }
    }
       
}



void wedobusinessinhere(void)
{
    uint32_t xx = 0;
    
    Widget1 w1;
    
    w1.initialise();
    w1.increment();
    xx = w1.read();
    xx = xx;
    
    
    Widget2 w2;
    
    w2.initialise();
    w2.increment();
    w2.increment();
    xx = w2.read();
    xx = xx;
    
    Widget1 *pw1 = NULL;
    
    pw1 = new Widget1;
    
    pw1->initialise();
    
    delete pw1;
    pw1 = NULL;
    
    pw1 = new (std::nothrow) Widget1;
    
    pw1->initialise();
    
    delete pw1;    
    pw1 = NULL;
    
    pw1 = new Widget1[3];
    
    pw1->initialise();
    
    delete[] pw1;
    
    Singleton2& s = Singleton2::getInstance();
    Singleton2 *ps = &s;
    
    Singleton2& ss1 = Singleton2::getInstance();
    Singleton2 *ps1 = &ss1;
    ss1.decrement();
    
    s.initialise();
    
    s.increment();
    s.increment();
    xx = s.read();
    xx = xx;
    
    
    embot::sys::theEnvironment *env = new embot::sys::theEnvironment;
    env->init();
    bool started = env->start();
    
    delete env;
    
    
    
    embot::hw::EEPROM *eeprom = new embot::hw::EEPROM;
    
    embot::sys::theStorage& storage = embot::sys::theStorage::getInstance();
    storage.load(*eeprom);
    
    uint32_t address = 7;
    uint32_t size = 8;
    uint8_t data[32] = {0};
    bool res = storage.read(address, size, data);
    res = res;
    
    delete eeprom;
    
    
    
    embot::hw::emulEEPROM *emee = new embot::hw::emulEEPROM;
    
    embot::hw::emulEEPROM::Config conf(embot::hw::emulEEPROM::Config::emulPlace::inRAM, 0x10000000, 64, 64);
    //conf.load(embot::hw::emulEEPROM::inRAM, 0x10000000, 64, 64);

    emee->init(conf);
    uint8_t buffer[64] = {0};
    uint8_t bbb[64] = {0};
    res = emee->read(0, 8, buffer);   
    
    bbb[0] = 1; bbb[1] = 2; bbb[2] = 3;
    res = emee->write(0, 8, bbb);
    
    res = emee->read(0, 8, buffer);   
    res = res;
    
    embot::sys::theStorage& storage1 = embot::sys::theStorage::getInstance();
    storage1.load(*emee);
    
    std::memset(buffer, 0, sizeof(buffer));
    res = storage1.read(0, 8, buffer);
    
    bbb[0] = 7;
    res = storage1.write(0, 16, bbb);
    res = storage1.read(0, 16, buffer);
    res =  res;
    
    delete emee;       
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






