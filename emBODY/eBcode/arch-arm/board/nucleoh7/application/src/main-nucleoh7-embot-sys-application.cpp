
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_led.h"
#include "embot_hw_button.h"
#include "embot_hw_sys.h"
#include "embot_hw_sys.h"

#include "embot_sys_theScheduler.h"
#include "embot_sys_theTimerManager.h"
#include "embot_sys_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"
#include "embot_binary.h"


static void s_chips_init();
static void s_chips_tick();



void doit(void *p)
{
    static uint32_t a = 0;
    a++;    
}

constexpr embot::common::EventMask evtTick = 0x01;


void eventbasedtask_startup(embot::sys::Task *t, void *param)
{
    s_chips_init(); 
    embot::hw::sys::puts("evtask-startup: chips initted" );
    
    embot::sys::Timer *tmr = new embot::sys::Timer;
    
    embot::sys::Action act(embot::sys::EventToTask(evtTick, t));
    embot::sys::Timer::Config cfg{embot::common::time1second, act, embot::sys::Timer::Mode::forever, 0};
    tmr->start(cfg);
    embot::hw::sys::puts("evtask-startup: started 1 sec timer which sends evtTick to evtask" );
}



void eventbasedtask_onevent(embot::sys::Task *t, embot::common::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }


    if(true == embot::binary::mask::check(eventmask, evtTick))
    {
        embot::common::Date d(embot::sys::now());        
        embot::hw::sys::puts("evtask-onevent: evtTick received @ time = " + d.to_string());     
        s_chips_tick();        
    }
              
    
}


void onIdle(embot::sys::Task *t, void* idleparam)
{
    static uint32_t i = 0;
    i++;
}

void initSystem(embot::sys::Task *t, void* initparam)
{
    
    embot::sys::theTimerManager::getInstance().start({});     
    embot::sys::theCallbackManager::getInstance().start({});  
    
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(2*embot::common::time1second); 
       
    
    embot::sys::EventTask::Config configEV { 
        4*1024, 
        embot::sys::Priority::high200, 
        eventbasedtask_startup,
        nullptr,
        50*embot::common::time1millisec,
        eventbasedtask_onevent
    };
        
    // create the main task 
    embot::sys::EventTask* tsk = new embot::sys::EventTask;          
    // and start it
    tsk->start(configEV);
    
}



// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    embot::hw::bsp::Config cc(nullptr, embot::sys::now);
    embot::hw::bsp::init(cc);
    
    embot::sys::InitTask::Config initcfg = { 1024, initSystem, nullptr };
    embot::sys::IdleTask::Config idlecfg = { 512, nullptr, nullptr, onIdle };
    embot::common::Callback onOSerror = { };

    embot::sys::theScheduler::Config cfg { embot::sys::theScheduler::Timing(embot::hw::sys::clock(embot::hw::CLOCK::syscore),  embot::common::time1millisec), {initcfg, idlecfg, onOSerror} };    
    embot::sys::theScheduler &thescheduler = embot::sys::theScheduler::getInstance();
    thescheduler.start(cfg);    
       
    // just because i am paranoid
    for(;;);    
}


#if 0

static void s_chips_init() {}
static void s_chips_tick() {}

#else

#include "embot_hw_bno055.h"

#include <cstring>

static void s_chips_init()
{
    constexpr embot::common::relTime timeout = 5*embot::common::time1millisec;
    
    embot::hw::bno055::Config bno055config { embot::hw::i2c::Descriptor { embot::hw::I2C::one, 400000 } };
    embot::hw::bno055::init(embot::hw::BNO055::one, bno055config); 
    
    constexpr embot::hw::bno055::Mode nonFusionMode = embot::hw::bno055::Mode::ACCGYRO;
    constexpr embot::hw::bno055::Mode fusionMode = embot::hw::bno055::Mode::NDOF;
    embot::hw::bno055::set(embot::hw::BNO055::one, nonFusionMode, timeout);   
    
}

volatile HAL_StatusTypeDef r = HAL_ERROR;

static void s_chips_tick()
{
    embot::hw::bno055::Data data;
    
    data.clear();
    
    embot::utils::Triple<float> a {0, 0, 0};
    
    
    if(embot::hw::result_t::OK == embot::hw::bno055::acquisition(embot::hw::BNO055::one, embot::hw::bno055::Set::AMG))
    {
        embot::hw::bno055::read(embot::hw::BNO055::one, data);
        data.getACC(a);
        embot::hw::sys::puts("BNO055: acc = (" + std::to_string(a.x) + ", " + std::to_string(a.y) + ", " + std::to_string(a.z) + ") m/(s*s)" );
    }
    else
    {  
        std::string txtout = "BNO055: invalid acc = (" + std::to_string(a.x) + ", " + std::to_string(a.y) + ", " + std::to_string(a.z) + ") m/(s*s)";        
        embot::hw::sys::puts(txtout);
        char text[32] = {'h', 'E', 'l', 'l', 'o', '!', ' '}; 
        // 'hello! " is transmited in about 600 usec @ 115200 w/ 1 bit stop 
        // (9 bits per char) -> 1/(115200/9) = 0.000078125 sec / byte. For 7 bytes -> 546 usec
        embot::common::Time t0 = embot::hw::bsp::now();
        //r = HAL_UART_Transmit(&huart3, const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(txtout.c_str())), std::strlen(txtout.c_str()), 0xFFFF);
        r = HAL_UART_Transmit(&huart3, const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(text)), std::strlen(text), 0xFFFF);
        embot::common::relTime delta =  embot::hw::bsp::now() - t0;
        embot::hw::sys::puts("uart tx @ 115200 lasted " + std::to_string(delta) + " us"); 
        r = r;
    }
    
}

#endif

#if 0
#include "embot_hw_si7051.h"

volatile uint32_t temperatureISready = 0; 
static void setflag(void*)
{
    temperatureISready = 1;    
}


static void s_chips_init()
{
    embot::hw::si7051::init(embot::hw::SI7051::one, {embot::hw::I2C::one, 400000});
    
    embot::hw::si7051::Temperature temperature = 0;
    
    if(true == embot::hw::si7051::isalive(embot::hw::SI7051::one))
    {
        embot::common::Callback cbk { setflag, nullptr};
        temperatureISready =  0;
        embot::hw::si7051::acquisition(embot::hw::SI7051::one, cbk);
        for(;;)
        {
            if(1 == temperatureISready)
            {
                break;
            }
        }
        embot::hw::si7051::read(embot::hw::SI7051::one, temperature);
        
    }
    
}

#endif



// required events: IMUacquisition + IMUvalueisready + ADCacquisition + ADCvalueisready + TXframe
// one thread which processes them all + ledmanager + 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



