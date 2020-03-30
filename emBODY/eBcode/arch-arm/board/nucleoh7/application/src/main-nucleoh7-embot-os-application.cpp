
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_core.h"
#include "embot_core_binary.h"

#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_led.h"
#include "embot_hw_button.h"
#include "embot_hw_sys.h"

#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"


// macro definition. we keep some behaviours in the same code.

// it tells to use the serial port to print data on it
#undef  enableSERIAL  
// if enableSERIAL is defined, it prints in human readable format
#undef  enableSERIAL_string
// if enableSERIAL is defined, it prints in compact binary format
#undef  enableSERIAL_binary
// if defined tickperiod is < 10 ms, else it is 1 sec 
#undef  enableTICK_fast
// if defined, the values are asked to the chips bno055 and the adc, if undefined we tx fake values
#undef  enableACQUISITION
// if defined we print values on the trace port
#undef  enableTRACE
// enableTRACE, it enable print of everything, even in the fast ticked function. else it prints only basic info
#undef  enableTRACE_all



#undef enableTICK_fast
#define enableTRACE_all
#define enableSERIAL
#undef enableACQUISITION

#if defined(enableSERIAL)
#define enableSERIAL_string
#endif

static void s_chips_init();
static void s_chips_tick();



void doit(void *p)
{
    static uint32_t a = 0;
    a++;    
}

constexpr embot::os::Event evtTick = 0x01;

#if defined(enableTICK_fast) 
constexpr embot::core::relTime tickperiod = 10*embot::core::time1millisec;
#else
constexpr embot::core::relTime tickperiod = embot::core::time1second;
#endif

void eventbasedthread_startup(embot::os::Thread *t, void *param)
{
    s_chips_init(); 
    embot::hw::sys::puts("evthread-startup: chips initted" );
    
    embot::os::Timer *tmr = new embot::os::Timer;
    
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);
    embot::hw::sys::puts("evthread-startup: started timer which sends evtTick to evthread every us = " + std::to_string(tickperiod));
}



void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }

    if(true == embot::core::binary::mask::check(eventmask, evtTick))
    {
#if defined(enableTRACE_all)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::hw::sys::puts("evthread-onevent: evtTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif        
        s_chips_tick();        
    }
                 
}


void onIdle(embot::os::Thread *t, void* idleparam)
{
    static uint32_t i = 0;
    i++;
}

void initSystem(embot::os::Thread *t, void* initparam)
{
    
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
    
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(2*embot::core::time1second); 
       
    
    embot::os::EventThread::Config configEV { 
        4*1024, 
        embot::os::Priority::high200, 
        eventbasedthread_startup,
        nullptr,
        50*embot::core::time1millisec,
        eventbasedthread_onevent
    };
        
    // create the main thread 
    embot::os::EventThread* thr = new embot::os::EventThread;          
    // and start it
    thr->start(configEV);
    
}



// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    // steps:
    // 1. i init the embot::os
    // 2 i start the scheduler
    
    
    constexpr embot::os::InitThread::Config initcfg = { 1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 512, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = { };
    constexpr embot::os::Config osconfig {embot::core::time1millisec, initcfg, idlecfg, onOSerror};
    
    // embot::os::init() internally calls embot::hw::bsp::init() which also calls embot::core::init()
    embot::os::init(osconfig);
    
    // now i start the os    
    embot::os::start();

    // just because i am paranoid (thescheduler.start() never returns)
    for(;;);    
}



#include "embot_hw_bno055.h"

#include <cstring>

static void s_chips_init()
{
#if !defined(enableACQUISITION)
    
    // nothing. we just simulate the sensors
    
#else
 
    constexpr embot::core::relTime timeout = 5*embot::core::time1millisec;
    
    embot::hw::bno055::Config bno055config { embot::hw::i2c::Descriptor { embot::hw::I2C::one, 400000 } };
    embot::hw::bno055::init(embot::hw::BNO055::one, bno055config); 
    
    // constexpr embot::hw::bno055::Mode mode = embot::hw::bno055::Mode::ACCGYRO;
    constexpr embot::hw::bno055::Mode mode = embot::hw::bno055::Mode::NDOF;
    embot::hw::bno055::set(embot::hw::BNO055::one, mode, timeout);      
    
#endif
    
}

volatile HAL_StatusTypeDef r = HAL_ERROR;

bool s_get_values(std::tuple<int16_t, int16_t, int16_t> &acc, std::pair<uint32_t, uint32_t> &adc);
bool s_print_values(const std::tuple<int16_t, int16_t, int16_t> &acc, const std::pair<uint32_t, uint32_t> &adc);

static void s_chips_tick()
{
    
    std::tuple<int16_t, int16_t, int16_t> acc {0, 0, 0};
    std::pair<uint32_t, uint32_t> adc {0, 0};
        
    s_get_values(acc, adc);
    
    s_print_values(acc, adc);
      
}

bool s_get_values(std::tuple<int16_t, int16_t, int16_t> &acc, std::pair<uint32_t, uint32_t> &adc)
{
#if !defined(enableACQUISITION)
    
    constexpr uint32_t min = 0;
    constexpr uint32_t max = 16;
    
    static int16_t accx = min;
    static int16_t accy = min;
    static int16_t accz = min;
    static uint32_t adc1 = min;
    static uint32_t adc2 = min;    
    
    acc = {accx, accy, accz};
    adc = {adc1, adc2};
    
    // implements a triangular waveform
    accx++; accy++; accz++; adc1++; adc2++;
    if(accx >= 1024) accx = min;
    if(accy >= 1024) accy = min;
    if(accz >= 1024) accz = min;
    if(adc1 >= 1024) adc1 = min;
    if(adc2 >= 1024) adc2 = min;    

#else

    acc = {0, 0, 0};
    adc = {1, 1};
    
    // so far only bno055
    #if 1
    embot::hw::bno055::Data data;        
    if(embot::hw::result_t::OK == embot::hw::bno055::acquisition(embot::hw::BNO055::one, embot::hw::bno055::Set::AMG, data))
    {
        for(;;)
        {
            if(true == embot::hw::bno055::operationdone(embot::hw::BNO055::one))
            {
                break;
            }
        }
        
        // ok, we have the values in data        
        acc = {data.acc.x, data.acc.y, data.acc.z};        
    }
    #else
    // better mode because it gives a timeout BUT to be tested
    embot::hw::bno055::Data data; 
    constexpr embot::core::relTime timeout = 10*embot::core::time1millisec;   
    if(embot::hw::result_t::OK == embot::hw::bno055::acquisition(embot::hw::BNO055::one, embot::hw::bno055::Set::AMG, data, timeout))
    {      
        // ok, we have the values in data        
        acc = {data.acc.x, data.acc.y, data.acc.z};        
    }        
    #endif

#endif    
       
    return true;    
}


bool s_print_values(const std::tuple<int16_t, int16_t, int16_t> &acc, const std::pair<uint32_t, uint32_t> &adc)
{   
    char text[64] = {0};
    
#if defined(enableSERIAL) && defined(enableSERIAL_string)
     
    // this prints in hex the entire range of values
    //snprintf(text, sizeof(text), "%04x %04x %04x %08x %08x\n", std::get<0>(acc), std::get<1>(acc), std::get<2>(acc), adc.first, adc.second);

    // JUST FOR TEST: this instead prints in hex but only 1 byte (the least significant byte)
    snprintf(text, sizeof(text), "%02x %02x %02x %02x %02x\n", std::get<0>(acc), std::get<1>(acc), std::get<2>(acc), adc.first, adc.second);
        
    HAL_UART_Transmit(&huart3, reinterpret_cast<uint8_t*>(text), std::strlen(text), 0xFFFF);
    
#elif defined(enableSERIAL) && defined(enableSERIAL_binary)   
    
    #warning TOBEDONE
    char text[64] = {0};
    snprintf(text, sizeof(text), "BINARYisTObeDONE\n");        
    HAL_UART_Transmit(&huart3, reinterpret_cast<uint8_t*>(text), std::strlen(text), 0xFFFF);
    
#endif
 

#if defined(enableTRACE_all)  
    std::tuple<float, float, float> a {0.01 * std::get<0>(acc), 0.01 * std::get<1>(acc), 0.01 * std::get<2>(acc)};
    embot::hw::sys::puts("BNO055: acc = (" + std::to_string(std::get<0>(a)) + ", " + std::to_string(std::get<1>(a)) + ", " + std::to_string(std::get<2>(a)) + ") m/(s*s)" );  
#endif    
    
    return true;
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



