
/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"

#include "embot_hw_sys.h"
#include "embot_app_scope.h"
#include "embot_hw_bsp_config.h"


constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
constexpr embot::core::relTime tickperiod = 1000*embot::core::time1millisec;


void ONE(){};
constexpr embot::app::scope::SignalEViewer::Config cONE{ONE, embot::app::scope::SignalEViewer::Config::LABEL::one};
embot::app::scope::Signal *signalONE {nullptr};

#if defined(EMBOT_ENABLE_hw_motor_bldc)
    #define TEST_MOTOR_BLDC
#endif

#if defined(TEST_MOTOR_BLDC)
    void test_motor_bldc_init();
    void test_motor_bldc_tick();
#endif


void eventbasedthread_startup(embot::os::Thread *t, void *param)
{   
    volatile uint32_t c = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    c = c;
   
    
    embot::core::print("mainthread-startup: started timer which sends evtTick to evthread every = " + embot::core::TimeFormatter(tickperiod).to_string());    
    
    signalONE = new embot::app::scope::SignalEViewer(cONE);    
    

    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);

#if defined(TEST_MOTOR_BLDC)    
    test_motor_bldc_init();
#endif
    
}


void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }
    

    if(true == embot::core::binary::mask::check(eventmask, evtTick)) 
    {
        
#if defined(TEST_MOTOR_BLDC)    
    test_motor_bldc_tick();
#endif        
        
//        embot::core::TimeFormatter tf(embot::core::now());        
//        embot::core::print("mainthread-onevent: evtTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));   
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
    embot::core::print("this is a demo. clock = " + std::to_string(cpufreq/1000000) + " MHz");    
    
    embot::core::print("starting the INIT thread");
    
    embot::core::print("INIT: creating the system services: timer manager + callback manager");
    
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
     
    embot::core::print("INIT: creating the LED pulser: it will blink a LED at 1 Hz and run a 0.2 Hz waveform on another");
        
    static const std::initializer_list<embot::hw::LED> allleds = 
    { 
        embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three, 
        embot::hw::LED::four, embot::hw::LED::five, embot::hw::LED::six
    };  
    
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    
    theleds.init(allleds);  
    for(const auto &l : allleds)
    {
        theleds.get(l).on();
        theleds.get(l).off();

    }         
    
#if defined(STM32HAL_dualcore_BOOT_cm4master)
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second);
#else
//    embot::app::LEDwaveT<64> ledwave(150*embot::core::time1millisec, 30, std::bitset<64>(0b01010101));
//    theleds.get(embot::hw::LED::one).wave(&ledwave);
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second);
#endif
      
    
    embot::core::print("INIT: creating the main thread. it will receive one periodic tick event");  
    
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

#include "embot_hw_dualcore.h"


// --------------------------------------------------------------------------------------------------------------------

int main(void)
{     
    // steps:
    // 1. i init the embot::os
    // 2 i start the scheduler
        
    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 2*1024, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = { };
    constexpr embot::os::Config osconfig 
    {
        embot::core::time1millisec, initcfg, idlecfg, onOSerror, 
#if defined(STM32HAL_BOARD_AMCFOC_1CM7)        
        embot::hw::FLASHpartitionID::eapplication01
#elif defined(STM32HAL_BOARD_AMCFOC_2CM4)
        embot::hw::FLASHpartitionID::eloader
#else        
        embot::hw::FLASHpartitionID::none // so that it does not add any offset to the flash location
#endif        
    };
    
    // embot::os::init() internally calls embot::hw::bsp::init() which also calls embot::core::init()
    embot::os::init(osconfig);
    
    // now i start the os    
    embot::os::start(); 

}



// - other code

#if defined(TEST_MOTOR_BLDC)

#include "embot_hw_motor_bldc.h"

#include "embot_hw_analog.h"

#include "embot_hw_motor_bldc_pwm.h"
#include "embot_hw_motor_bldc_adc.h"
#include "embot_hw_motor_bldc_qenc.h"
#include "embot_hw_motor_bldc_hall.h"

    std::array<embot::hw::motor::bldc::Currents, 2> currents {};
        
    void onCurrents(embot::hw::MOTOR m, const embot::hw::motor::bldc::Currents * const currs, void *owner)
    {

        if((nullptr == currs) || (false == embot::hw::motor::bldc::initialised(m)))
        {
            return;
        } 
        
        
        // 1. save the currents for the motor
        
        currents[embot::core::tointegral(m)] = *currs;
        
    }


    int32_t curr1ma {0};
    int32_t curr2ma {0};
    int32_t curr3ma {0};
    
    float curr1 {0.0};
    
    void oncurrents(embot::hw::MOTOR m, const embot::hw::motor::bldc::Currents * const currs, void *owner)
    {
        
        curr1 = currs->u;
        
        curr1ma = 1000 * currs->u;
        curr2ma = 1000 * currs->v;
        curr3ma = 1000 * currs->w;        
    }
    
    //#define TEST_QENC_only
    // #define TEST_QENC_only
    
    
    #define TEST_6steps
    
    #if defined(TEST_6steps)
    
    // either TEST_6steps_hall or TEST_6steps_qenc
    #define TEST_6steps_hall
    #endif

 
    
#if defined(TEST_6steps)

    volatile bool ishallqencready {false};

    void do6steps(embot::hw::MOTOR m, const embot::hw::motor::bldc::Currents * const currs, void *owner)
    {
        
        if(false == ishallqencready)
        {
            return;
        }
        
        uint8_t nsector = embot::hw::motor::bldc::hall::sector(embot::hw::MOTOR::one);
        
//        if(nsector >= embot::hw::motor::bldc::hall::numsectors)
//        {
//            return;
//        }
        
        // apply the pwm
        #warning after a brief check pls set dutycyclepercent = 5.0
        constexpr float dutycyclepercent {0.0}; // 5%
        constexpr float pwm {dutycyclepercent};
        constexpr float hiz {dutycyclepercent/2.0};
        
        constexpr std::array<embot::hw::motor::bldc::PWM3, 8> thepwms
        {{
            {0.0, 0.0, 0.0}, // 0 not valid
            {0.0, hiz, pwm}, // 1
            {hiz, pwm, 0.0}, // 2
            {0.0, pwm, hiz}, // 3
            {pwm, 0.0, hiz}, // 4
            {hiz, 0.0, pwm}, // 5
            {pwm, hiz, 0.0}, // 6
            {0.0, 0.0, 0.0}, // 7 not valid                       
        }};
        
        uint8_t status = embot::hw::motor::bldc::hall::getstatus(embot::hw::MOTOR::one);
        
//        if((0 == status) || (status >=7))
//        {
//            return;
//        }
        
        embot::hw::motor::bldc::pwm::set(embot::hw::MOTOR::one, thepwms[status & 0b111]);
                       
    }

#endif     
    
    
    
    
    void test_motor_bldc_init()
    {
        
#if defined(TEST_6steps)

        embot::hw::analog::init({});
            
        embot::hw::motor::bldc::hall::init(embot::hw::MOTOR::one, {}); 
        embot::hw::motor::bldc::hall::start(embot::hw::MOTOR::one, {});    // no swap-bc, 0 offset, 1 polar pair (unused)        
            
            
        ishallqencready = true;
       
        embot::hw::motor::bldc::adc::init(embot::hw::MOTOR::one, {});   
        embot::hw::motor::bldc::OnCurrents oncurs {embot::hw::MOTOR::one, do6steps, nullptr};
        embot::hw::motor::bldc::adc::set(embot::hw::MOTOR::one, oncurs);            
        embot::hw::motor::bldc::pwm::init(embot::hw::MOTOR::one, {});

#else        
        
#if defined(TEST_QENC_only)
        
        constexpr embot::hw::motor::bldc::qenc::Mode encmode {
            1024,       // resolution
            {}          // onindex
        };     
        embot::hw::motor::bldc::qenc::init(embot::hw::MOTOR::one, {});
        embot::hw::motor::bldc::qenc::start(embot::hw::MOTOR::one, encmode);
        
#else        
        
        embot::hw::analog::init({});
       
        embot::hw::motor::bldc::adc::init(embot::hw::MOTOR::one, {});   
        embot::hw::motor::bldc::OnCurrents oncurs {embot::hw::MOTOR::one, oncurrents, nullptr};
        embot::hw::motor::bldc::adc::set(embot::hw::MOTOR::one, oncurs);
            
        embot::hw::motor::bldc::pwm::init(embot::hw::MOTOR::one, {});
        
#endif    

#endif

//        embot::hw::motor::bldc::init(embot::hw::MOTOR::one, {});
//        //embot::hw::motor::bldc::init(embot::hw::MOTOR::two, {});    

//        embot::hw::motor::bldc::set(embot::hw::MOTOR::one, {embot::hw::MOTOR::one, onCurrents, nullptr});
//        //embot::hw::motor::bldc::set(embot::hw::MOTOR::two, {embot::hw::MOTOR::two, onCurrents, nullptr});             
        
    }
    
    float angle {0};
    float angleoflastindex {0};
    
    void test_motor_bldc_tick()
    {
        
        // return;
        
        // every second i deinit and then init again
        static embot::core::Time tt = embot::core::now();
        static bool initted {true};
        
        constexpr embot::core::Time delta { embot::core::time1second };
        
        embot::core::Time n = embot::core::now();
        
        
#if defined(TEST_QENC_only)
        angle = embot::hw::motor::bldc::qenc::angle(embot::hw::MOTOR::one, embot::hw::motor::bldc::qenc::AngleQE::current);
        angleoflastindex = embot::hw::motor::bldc::qenc::angle(embot::hw::MOTOR::one, embot::hw::motor::bldc::qenc::AngleQE::oflatestindexcrossing);
#else         
        
        if((n-tt) >= delta)
        {
//            if(initted)
//            {
//                embot::hw::analog::deinit();
//                initted = false;
//            }
//            else
//            {
//                embot::hw::analog::init({});
//                initted = true;
//            }
        }

#endif
        
       // embot::hw::analog::init({});

//        embot::hw::motor::bldc::init(embot::hw::MOTOR::one, {});
//        //embot::hw::motor::bldc::init(embot::hw::MOTOR::two, {});    

//        embot::hw::motor::bldc::set(embot::hw::MOTOR::one, {embot::hw::MOTOR::one, onCurrents, nullptr});
//        //embot::hw::motor::bldc::set(embot::hw::MOTOR::two, {embot::hw::MOTOR::two, onCurrents, nullptr});             
        
    }
    
#endif


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
