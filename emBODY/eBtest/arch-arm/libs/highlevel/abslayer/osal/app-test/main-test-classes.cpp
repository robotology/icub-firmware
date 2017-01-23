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

//#include "EOVtheCallbackManager.h"

#include "EOtheLEDpulser.h"


#ifdef __cplusplus
extern "C" {
#endif
    extern const osal_cfg_t *osal_cfgMINEX;
#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 




// this one is enough for embot namespace embot{ common{}, sys{} }

#include "embot.h"

#include "embot_sys_theJumper.h"

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

//static void s_osal_init(void);

static void s_osal_starter(void);

void task_led(void *);

void task_wrk(void *param);

void wedobusinessinhere(void);

static int8_t jump2address(uint32_t addr);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static osal_task_t * s_taskLED = NULL;
static osal_task_t * s_taskWRK = NULL;

static const osal_eventflag_t s_event_TICK = 0x00000001;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of externally defined functions or variable which dont have a .h file
// --------------------------------------------------------------------------------------------------------------------

void ciao(void*p)
{
    static uint8_t dd = 0;
    
    dd++;
    dd = dd;
}

void onIdle(void)
{
    // called every time inside a for(;;){}
    static volatile uint8_t ii = 0;
    ii++;
    
//    embot::sys::theScheduler & system = embot::sys::theScheduler::getInstance(); 
//    embot::sys::Task *tsk1 = system.getTask();
//    tsk1 = tsk1;
    
    return;
}

void onFatal(void)
{
    uint8_t ciao = 0;
    ciao =  ciao;
    return;
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



// READ.ME very well
// in here we test the use of c++ together with basic embOBJ objects (written in C).
// we want a new 


int main(void) 
{
    embot::sys::theJumper& thejumper = embot::sys::theJumper::getInstance();
    embot::sys::theJumper::Config cfgJumper(NVIC_SystemReset, jump2address);
    thejumper.init(cfgJumper);
    uint32_t address = 0;
    bool found = thejumper.eval(address, false);
    if(found)
    {
        uint8_t ciao = 0;
        address = address;
        // could not jump to address .... why?
        thejumper.jump(address);
    }
    
    thejumper.set(0x1+address);
    
    thejumper.restart();
    
    // cmsis init
    SystemInit(); 
    // configuration of priority: not needed
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
    // init the leds
    board_led_global_init();
     
    // init the event viewer.
    eventviewer_init(); 
    
    
    
//    EOaction_strg strg;
//    EOaction *act = (EOaction*)&strg;
//    eo_action_SetCallback(act, ciao, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle()));  
//    
//    embot::sys::Timer *tmr = new embot::sys::Timer;
//    const embot::relTime countdown = 1000*1000;
//    tmr->start(countdown, embot::sys::Timer::Mode::forever, act);

//    
//    s_osal_init();
        
    
    embot::sys::theScheduler &thesystem = embot::sys::theScheduler::getInstance();
 
#if 0 
    thesystem.setTiming(168*1000*1000, 1000);
    thesystem.setOnIdle(onIdle, 512);
    thesystem.setOnFatalError(onFatal);
    thesystem.setLauncher(s_osal_starter, 2048);
#else
    embot::sys::theScheduler::Config cfg;
    cfg.launcher = s_osal_starter;
    cfg.launcherstacksize = 2048;
    cfg.onidle = onIdle;
    cfg.onidlestacksize = 512;
    cfg.onfatalerror = onFatal;
    
    thesystem.init(cfg);
#endif


    thesystem.start();
    
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

//static void s_osal_init(void)
//{
//    uint32_t ram08size = 0;
//    uint64_t *ram08data = NULL;
//    
//    osal_base_memory_getsize(osal_cfgMINEX, &ram08size);
//    
//    if(0 != ram08size)
//    {
//        // what about osal_base_memory_new() ???? yes, we can use it
//        ram08data = (uint64_t*)(uint64_t*)osal_base_memory_new(ram08size);
//    }

//    osal_base_initialise(osal_cfgMINEX, ram08data);      
//  
//}

embot::sys::EventTask *taskevt = nullptr;
embot::sys::MessageTask *taskmsg = nullptr;

typedef struct
{
    embot::sys::EventTask *taskevt;
    embot::sys::MessageTask *taskmsg;
} helper_t;

helper_t helper = {nullptr, nullptr};

void startup_tskevt(embot::sys::Task *tsk, void *param)
{
    uint8_t ss = tsk->getPriority();
    ss = ss;
    tsk = tsk;
    
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_seven, EOK_reltime1sec, 0);
}

void onevent(embot::sys::Task *tsk, embot::common::Event evt, void *param)
{
    tsk = tsk;
    evt = evt;   
    
    
    static embot::common::Time t1 = 0;
    
    static embot::common::Time prev = 0;
    
    t1 = embot::sys::timeNow();
    t1 = t1;
       
    static uint64_t delta = 0;
    delta = t1 - prev;
    
    delta = delta;
    
    prev = t1;
    
    if(0x10000000 == evt)
    {   
        evt = evt; 
    }
   
    
//    embot::sys::EventTask * t = dynamic_cast<embot::sys::EventTask *>(tsk);    
//    int a = t->getPP();
//    a = a;
}

void onmessage(embot::sys::Task *tsk, embot::common::Message msg, void *param)
{
    tsk = tsk;
    msg = msg;


    static embot::common::Time t1 = 0;
    
    static embot::common::Time prev = 0;
    
    t1 = embot::sys::timeNow();
    t1 = t1;
       
    static uint64_t delta = 0;
    delta = t1 - prev;
    
    delta = delta;
    
    prev = t1;

    embot::sys::Task *tsk1 = embot::sys::taskRunning();
    tsk1 = tsk1;
}


void periodicactivity(embot::sys::Task *tsk, void *param)
{
    tsk = tsk;
    
    static uint32_t times = 0;
    static embot::common::Time tthis = 0;
    static embot::common::Time tmsg = 0;
    static embot::common::Time now = 0;
    
    helper_t *h = static_cast<helper_t*>(param);
    
    times++;  
    //h->taskevt->setEvent(times);    
    taskevt->setEvent(times);
    
    //taskmsg->setMessage(times, 10*1000);
    h->taskmsg->setMessage(times, 10*1000);
    times++;
    //taskmsg->setMessage(times, 10*1000);  
//    h->taskmsg->setMessage(times, 10*1000);  

    tthis = tsk->timeOfTrigger();  
    tthis = tthis;    
    tmsg = h->taskmsg->timeOfTrigger();
    tmsg = tmsg;
    
    now = embot::sys::timeNow();
    now = now;
}

static void s_osal_starter(void)
{
#if 0
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
#else

    embot::sys::theTimerManager& tmrman = embot::sys::theTimerManager::getInstance();

    embot::sys::theTimerManager::Config tmrmanconfig;

    tmrman.init(tmrmanconfig);
    tmrman.start();
    
    embot::sys::theCallbackManager& cbkman = embot::sys::theCallbackManager::getInstance();
    cbkman.start();

    taskevt = new embot::sys::EventTask;
    
    taskevt->init(startup_tskevt, onevent, 1024, 20, 1000*1000);    
    
    taskmsg = new embot::sys::MessageTask(nullptr, onmessage, 1024, 24, 8, 1000*1000);
    
//    embot::sys::Task * t = dynamic_cast<embot::sys::Task *>(task);        
//    delete t;    
//    task = task;


    helper.taskevt = taskevt;
    helper.taskmsg = taskmsg;

    embot::sys::PeriodicTask *taskper = new embot::sys::PeriodicTask;
    
    
    taskper->init(nullptr, periodicactivity, 1024, 30, 100*1000, &helper);
    


    embot::sys::Task *tsk1 = embot::sys::taskRunning();
    tsk1 = tsk1;
    
    embot::sys::Timer *tmr = new embot::sys::Timer;
    
    embot::sys::Action action;
    embot::sys::Action::EventToTask evt2tsk;
    evt2tsk.event = 0x10000000;
    evt2tsk.task = taskevt;
//    evt2tsk.task = NULL;
    action.set(evt2tsk);
    
    tmr->start(1000*1000, embot::sys::Timer::Type::forever, action);
    
    
    
    eOledpulser_cfg_t ledconfig = {0};
    
    ledconfig.led_enable_mask   = (1 << eo_ledpulser_led_zero) | (1 << eo_ledpulser_led_one) | (1 << eo_ledpulser_led_two) | (1 << eo_ledpulser_led_three) | 
                                  (1 << eo_ledpulser_led_four) | (1 << eo_ledpulser_led_five) | (1 << eo_ledpulser_led_six) | (1 << eo_ledpulser_led_seven);
    ledconfig.led_init          = board_led_init;
    ledconfig.led_on            = (eOint8_fp_uint8_t) board_led_on;
    ledconfig.led_off           = (eOint8_fp_uint8_t) board_led_off;
    ledconfig.led_toggle        = (eOint8_fp_uint8_t) board_led_toggle;
    
    eo_ledpulser_Initialise(&ledconfig);
    


#endif

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
    
    
#if 0    
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

#endif
}



static int8_t jump2address(uint32_t addr)
{
#if 1
    return 0;
#else
    volatile uint32_t jumpaddr;
    void (*app_fn)(void) = NULL;

//    if(hl_res_NOK_generic == hl_sys_canjump(addr))
//    {
//        return(hl_res_NOK_generic);
//    }

    // prepare jump address 
    jumpaddr = *(volatile uint32_t*) (addr + 4);
    // prepare jumping function
    app_fn = (void (*)(void)) jumpaddr;
    // initialize user application's stack pointer 
    __set_MSP(*(volatile uint32_t*) addr);
    // jump.
    app_fn(); 
    
    // never in here.
    return(0); 
#endif    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






