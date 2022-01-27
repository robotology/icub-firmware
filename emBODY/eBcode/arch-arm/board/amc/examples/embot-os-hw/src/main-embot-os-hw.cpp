
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#define TEST_EMBOT_HW


#include "embot_core.h"
#include "embot_core_binary.h"

#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_led.h"
#include "embot_hw_sys.h"


#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"

#include <vector>


constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);

constexpr embot::core::relTime tickperiod = 1000*embot::core::time1millisec;


#if defined(TEST_EMBOT_HW)

#define TEST_EMBOT_HW_EEPROM
#define TEST_EMBOT_HW_CHIP_M95512DF

void test_embot_hw_init();
void test_embot_hw_tick();
#endif


void eventbasedthread_startup(embot::os::Thread *t, void *param)
{   
    volatile uint32_t c = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    c = c;
   
    
    embot::core::print("mainthread-startup: started timer which sends evtTick to evthread every = " + embot::core::TimeFormatter(tickperiod).to_string());    
    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);
    
    test_embot_hw_init();    
}


void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }

    if(true == embot::core::binary::mask::check(eventmask, evtTick)) 
    {      
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("mainthread-onevent: evtTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));   
    
        test_embot_hw_tick();
    }
    

}


void onIdle(embot::os::Thread *t, void* idleparam)
{
    static uint32_t i = 0;
    i++;
}

void tMAIN(void *p)
{
    embot::os::Thread* t = reinterpret_cast<embot::os::Thread*>(p);
    t->run();
}

void initSystem(embot::os::Thread *t, void* initparam)
{
    volatile uint32_t cpufreq = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    cpufreq = cpufreq;
    embot::core::print("this is a demo which shows that this code can run on a dev board. clock = " + std::to_string(cpufreq/1000000) + " MHz");    
    
    embot::core::print("starting the INIT thread");
    
    embot::core::print("INIT: creating the system services: timer manager + callback manager");
    
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
                    
    embot::core::print("INIT: creating the LED pulser: it will blink a LED at 1 Hz and run a 0.2 Hz waveform on another");
        
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one, embot::hw::LED::two};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second); 
    embot::app::LEDwaveT<64> ledwave(100*embot::core::time1millisec, 50, std::bitset<64>(0b010101));
    theleds.get(embot::hw::LED::two).wave(&ledwave); 
    
    
    embot::core::print("INIT: creating the main thread. it will reveives one periodic tick event and one upon pressure of the blue button");  
    
    embot::os::EventThread::Config configEV { 
        6*1024, 
        embot::os::Priority::high40, 
        eventbasedthread_startup,
        nullptr,
        50*embot::core::time1millisec,
        eventbasedthread_onevent,
        "mainThreadEvt"
    };
       
        
    // create the main thread 
    embot::os::EventThread *thr {nullptr};
    thr = new embot::os::EventThread;          
    // and start it. w/ osal it will be displayed w/ label tMAIN
    thr->start(configEV, tMAIN); 
    
    embot::core::print("quitting the INIT thread. Normal scheduling starts");    
}


// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    // steps:
    // 1. i init the embot::os
    // 2 i start the scheduler
        
    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 2*1024, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = { };
    constexpr embot::os::Config osconfig {embot::core::time1millisec, initcfg, idlecfg, onOSerror};
    
    // embot::os::init() internally calls embot::hw::bsp::init() which also calls embot::core::init()
    embot::os::init(osconfig);
    
    // now i start the os    
    embot::os::start();

    // just because i am paranoid (thescheduler.start() never returns)
    for(;;);    
}



// - other code


#if defined(TEST_EMBOT_HW)

#if defined(TEST_EMBOT_HW_EEPROM)
#include "embot_hw_eeprom.h"

//constexpr embot::hw::EEPROM eeprom2test {embot::hw::EEPROM::one};
constexpr embot::hw::EEPROM eeprom2test {embot::hw::EEPROM::two};

#endif

#if defined(TEST_EMBOT_HW_CHIP_M95512DF)

    #include "embot_hw_chip_M95512DF.h"

#endif

void test_embot_hw_init()
{

#if defined(TEST_EMBOT_HW_CHIP_M95512DF)

    embot::hw::chip::testof_M95512DF();    

#endif
    
#if defined(TEST_EMBOT_HW_EEPROM)
    
//    if(embot::hw::resOK == embot::hw::eeprom::init(eeprom2test, {});

    embot::hw::eeprom::init(eeprom2test, {});

#endif
    
    
}

constexpr size_t capacity {2048};
uint8_t dd[capacity] = {0};

volatile uint8_t stophere = 0;

embot::core::Time startread {0};
embot::core::Time readtime {0};
embot::core::Time startwrite {0};
embot::core::Time writetime {0};

void test_embot_hw_tick()
{
    static uint8_t cnt = 0;
    cnt++;
    
    static uint8_t shift = 0;
    size_t numberofbytes = capacity >> shift;
    
    if(shift>8)
    {
        shift = 0;
    }
    else
    {
        shift++;
    }
    
#if defined(TEST_EMBOT_HW_EEPROM)
    
    std::memset(dd, 0, sizeof(dd));
    embot::core::Data data {dd, numberofbytes};
    constexpr embot::core::relTime tout {3*embot::core::time1millisec};
    
    startread = embot::core::now(); 
    embot::hw::eeprom::read(eeprom2test, 0, data, 3*embot::core::time1millisec);
    readtime = embot::core::now() - startread;
    stophere++;
    
    std::memset(dd, cnt, sizeof(dd));
    startwrite = embot::core::now(); 
    embot::hw::eeprom::write(eeprom2test, 0, data, 3*embot::core::time1millisec);
    writetime = embot::core::now() - startwrite;
    stophere++;
    
    std::memset(dd, 0, sizeof(dd));
    embot::hw::eeprom::read(eeprom2test, 0, data, 3*embot::core::time1millisec);
     
    stophere++;
    
    embot::core::print(std::string("numberofbytes = ") + std::to_string(numberofbytes) + ", read time = " + embot::core::TimeFormatter(readtime).to_string() + ", write time = " + embot::core::TimeFormatter(writetime).to_string());
    
    #endif    
}

#endif



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
