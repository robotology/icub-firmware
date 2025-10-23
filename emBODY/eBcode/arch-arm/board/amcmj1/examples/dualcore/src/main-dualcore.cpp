
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_hw_dualcore.h"
#include "stm32hal.h"
    
    
//#if defined(STM32HAL_dualcore_BOOT_cm4master)

//    #if defined(STM32HAL_CORE_CM4)
//    
//    #warning this application runs on a cm4 in an MPU where cm4 is master. in here we enable the other core inside bsp::specialize() 
//    
////    constexpr embot::hw::dualcore::Config dualcoreconfig {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::activate};
//    #define RUNNING_ON_MASTER

//    #elif defined(STM32HAL_CORE_CM7)
//   
//    #warning this application runs on a cm7 in an MPU where cm4 is master. we must wait for activation 
////    constexpr embot::hw::dualcore::Config dualcoreconfig {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::donothing};
//    
//    #endif
//    // or: {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::donothing}    

//#elif defined(STM32HAL_dualcore_BOOT_cm7master)

//    #if defined(STM32HAL_CORE_CM7)
//    
//    #warning this application runs on a cm7 in an MPU where cm7 is master. in here we enable the other core inside bsp::specialize() 
//    
//    #define RUNNING_ON_MASTER
////    constexpr embot::hw::dualcore::Config dualcoreconfig {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::activate};

//    #elif defined(STM32HAL_CORE_CM4)
//   
//    #warning this application runs on a cm4 in an MPU where cm7 is master. we must wait for activation 
//    constexpr embot::hw::dualcore::Config dualcoreconfig {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::donothing};
//    
//    #endif


//#else
//    #error
//#endif    


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

#include "embot_app_scope.h"

#include <vector>

//#include "embot_hw_bsp_amcfoc_1cm7.h"




embot::app::scope::Signal *signal {nullptr};
constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
constexpr embot::core::relTime tickperiod = 1000*embot::core::time1millisec;

#define ENABLE_SPEED_TESTS

#if defined(ENABLE_SPEED_TESTS) 

    enum class SpeedTest { none, delay, one, two }; 
    constexpr SpeedTest speedtest {SpeedTest::delay};
    constexpr embot::core::Time SpeedTestDelay { 40*embot::core::time1microsec };

    void test_SPEED_init(embot::os::Thread *t, void *param);
    void test_SPEED_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param);

#endif


#undef ENABLE_HW_TESTS  
//#define ENABLE_HW_TESTS

#if !defined(ENABLE_HW_TESTS)
    #undef TEST_EEPROM
    //#define ERASE_EEPROM


    #undef TEST_EMBOT_HW_TIMER
    #undef TEST_EMBOT_HW_TIMER_ONESHOT
#else
    // place in here what we enable

    #warning marco.accame on 24 march 2025 says: paye attention because the HW tests may not work

#endif


#if defined(ENABLE_HW_TESTS)

void test_HW_init(embot::os::Thread *t, void *param);
void test_HW_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param);

#endif




#include "embot_hw_led.h"
#include "embot_hw_sys.h"

void ON(){};

void eventbasedthread_startup(embot::os::Thread *t, void *param)
{   
    volatile uint32_t c = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    c = c;
   
   
    embot::core::print("mainthread-startup: started timer which sends evtTick to evthread every = " + embot::core::TimeFormatter(tickperiod).to_string());    

    constexpr embot::app::scope::SignalEViewer::Config cc{ON, embot::app::scope::SignalEViewer::Config::LABEL::one};
    signal = new embot::app::scope::SignalEViewer(cc);    

#if defined(ENABLE_SPEED_TESTS) 
    test_SPEED_init(t, param);
#endif

#if defined(ENABLE_HW_TESTS)   
    test_HW_init(t, param);
#endif    
    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);
    
}


uint64_t uid {0};

void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }

    if(true == embot::core::binary::mask::check(eventmask, evtTick)) 
    {
               
        signal->on();   
#if defined(ENABLE_SPEED_TESTS) 
    test_SPEED_onevent(t, eventmask, param);
#endif        
        signal->off();   
        
#if defined(ENABLE_HW_TESTS)   
    test_HW_onevent(t, eventmask, param);        
#endif  

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
     
    embot::core::print("INIT: creating the LED pulser");
        
    static const std::initializer_list<embot::hw::LED> allleds = 
    { 
        embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three
    };  
    
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    
    theleds.init(allleds);  
    for(const auto &l : allleds)
    {
        theleds.get(l).on();
        theleds.get(l).off();

    }        

    if(true == embot::hw::dualcore::ismaster())
    {
        embot::core::print(std::string("INIT: we are runnning in master mode w/: ") + 
                                        embot::hw::dualcore::to_string(embot::hw::dualcore::core()) + ", " +
                                        embot::hw::dualcore::to_string(embot::hw::dualcore::boot()) 
                                       );  
        theleds.get(embot::hw::LED::one).pulse(embot::core::time1second);
        embot::core::print("LED::one will blink @ 1 Hz");
    }
    else
    {
        embot::core::print(std::string("INIT: we are runnning in slave mode w/: ") + 
                                        embot::hw::dualcore::to_string(embot::hw::dualcore::core()) + ", " +
                                        embot::hw::dualcore::to_string(embot::hw::dualcore::boot()) 
                                       );  
        embot::app::LEDwaveT<64> ledwave(100*embot::core::time1millisec, 10, std::bitset<64>(0b0101));
        theleds.get(embot::hw::LED::one).wave(&ledwave);
        embot::core::print("LED::one will pulse twice at 1 Hz ");        
    }
      
    
    embot::core::print("INIT: creating the main thread. it will receives one periodic tick event");  
    
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
    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 2*1024, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = { };
    constexpr embot::os::Config osconfig 
    {
        embot::core::time1millisec, initcfg, idlecfg, onOSerror, 
        embot::hw::FLASHpartitionID::eapplication01
    };
    
    bool iamthemaster = embot::hw::dualcore::ismaster();

    if(true == iamthemaster)
    {
        constexpr embot::hw::dualcore::Config dualcoreconfig {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::activateandhold}; //embot::hw::dualcore::Config::CMD::activate};
        embot::hw::dualcore::config(dualcoreconfig);
    }
    
//#if defined(RUNNING_ON_MASTER)   
//    embot::hw::dualcore::config(dualcoreconfig);
////    embot::hw::dualcore::config({embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::donothing});
//#endif    
    
    embot::os::init(osconfig);     
    embot::os::start();
}


// -- all the other code

#if defined(ENABLE_SPEED_TESTS) 

#include <cmath>

void compute()
{
    static constexpr size_t N {10}; 
    static constexpr size_t M {10};
    static float data[N][M] = {0};
    
    std::memset(data, 0, sizeof(data));
    
    for(size_t n=0; n<N; n++)
    {
        for(size_t m=0; m<M; m++)
        {
            data[n][m] = 0.1*static_cast<float>(n+m)*(data[n][m] + 1.0f);
            data[std::max(N, m)][std::max(M, n)] = data[n][m] * data[n][m];
        }        
    }
    
}

void compute2()
{
    static constexpr size_t N {10}; 
    static constexpr size_t M {10};
    static float data[N][M] = {0};
    
    std::memset(data, 0, sizeof(data));
    
    for(size_t n=0; n<N; n++)
    for(size_t m=0; m<M; m++)
        data[n][m] = 0.1 * static_cast<float>(n) * static_cast<float>(m);
    
    for(size_t n=0; n<N; n++)
    {
        for(size_t m=0; m<M; m++)
        {
            data[n][m] = std::sin(0.1*static_cast<float>(n+m)*(data[n][m] + 1.0f));
            data[std::max(N, m)][std::max(M, n)] = data[n][m] * data[n][m];
        }        
    }
    
}


void test_SPEED_init(embot::os::Thread *t, void *param) 
{
    // do nothing actually
}

void test_SPEED_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{
    switch(speedtest)
    {
        case SpeedTest::delay:
        {
            embot::hw::sys::delay(SpeedTestDelay);
        } break;

        case SpeedTest::one:
        {
            compute();
        } break;
        
        case SpeedTest::two:
        {
            compute2();
        } break;

        case SpeedTest::none:
        default: 
        {
        } break;
    
    }

}

#endif // #if defined(ENABLE_SPEED_TESTS)

#if defined(ENABLE_HW_TESTS)

void test_HW_init(embot::os::Thread *t, void *param)
{
    
}

void test_HW_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{
    
}


#endif // #if defined(ENABLE_HW_TESTS)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
