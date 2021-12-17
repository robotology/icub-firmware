
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#define TEST_EMBOT_OS_OSAL

#include "embot_core.h"
#include "embot_core_binary.h"

#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_led.h"
#include "embot_hw_button.h"
#include "embot_hw_tlv493d.h"
#include "embot_hw_tlv493d_bsp.h"
#include "embot_hw_sys.h"

#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"
#include "embot_tools.h"
#include "embot_app_scope.h"

#define enableTRACE_all

constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
constexpr embot::os::Event evtBTNreleased = embot::core::binary::mask::pos2mask<embot::os::Event>(1);
constexpr embot::os::Event evtRead = embot::core::binary::mask::pos2mask<embot::os::Event>(2);

constexpr embot::core::relTime tickperiod = 1000*embot::core::time1millisec;

constexpr embot::hw::BTN buttonBLUE = embot::hw::BTN::one;

//embot::tools::Histogram *histoFOCperiod {nullptr};
//embot::tools::PeriodValidator *validFOCperiod {nullptr};

embot::app::scope::SignalHisto *signalhisto {nullptr};

void btncallback(void *p)
{
    embot::os::Thread *t = reinterpret_cast<embot::os::Thread *>(p);
    t->setEvent(evtBTNreleased);
}

void eventbasedthread_startup(embot::os::Thread *t, void *param)
{          
    volatile uint32_t c = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    c = c;
    
//    constexpr std::uint64_t focperiodlimitLOW {0};      // from 0 usec
//    constexpr std::uint64_t focperiodlimitHIGH {48};    // up to 48 usec
//    constexpr std::uint64_t focperiodSTEP {1};          // in steps of 1 usec
    
    constexpr std::uint64_t focperiodlimitLOW {1000*1000-50};          // from 0 usec
    constexpr std::uint64_t focperiodlimitHIGH {1000*1000+50};        // up to 48 usec
    constexpr std::uint64_t focperiodSTEP {1};          // in steps of 1 usec
    
//    histoFOCperiod = new embot::tools::Histogram;
//    histoFOCperiod->init({focperiodlimitLOW, focperiodlimitHIGH, focperiodSTEP});
//    
//    constexpr embot::core::Time period2validate { embot::core::time1second };
//    constexpr embot::core::Time periodlimitlow { embot::core::time1second - 50 };
//    constexpr embot::core::Time periodlimithigh { embot::core::time1second + 50 };
//    embot::tools::PeriodValidator::Config pvc1sec { period2validate, 3*period2validate, 0, {periodlimitlow, periodlimithigh, 1} };
//    
//    constexpr embot::core::Time period2validate1 { 40 * embot::core::time1microsec };
//    constexpr embot::core::Time periodlimitlow1 { 0 };
//    constexpr embot::core::Time periodlimithigh1 { 48 * embot::core::time1microsec };
//    embot::tools::PeriodValidator::Config pvcfoc { period2validate1, 3*period2validate1, 0, { periodlimitlow1, periodlimithigh1, embot::core::time1microsec } };
    
//    validFOCperiod = new embot::tools::PeriodValidator;
//    
//    validFOCperiod->init(pvc1sec);
//    validFOCperiod->init(pvcfoc);
    
    
    embot::app::scope::SignalHisto::Config shcfg {0, 100*embot::core::time1microsec, embot::core::time1microsec};
    signalhisto = new embot::app::scope::SignalHisto(shcfg);    
    
    embot::core::print("mainthread-startup: initted driver for tlv493d");  
    // init the tlv493d
    embot::hw::tlv493d::init(embot::hw::TLV493D::one, {embot::hw::tlv493d::Config::startupMODE::resetCHIP});
            
    
    embot::core::print("mainthread-startup: started timer which sends evtTick to evthread every = " + embot::core::TimeFormatter(tickperiod).to_string());
    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);
    
    // init the ext interrupt button
    embot::hw::button::init(buttonBLUE, {embot::hw::button::Mode::TriggeredOnRelease, {btncallback, t}, 0});
    
}

void alertdataisready00(void *p)
{
    embot::os::Thread *t = reinterpret_cast<embot::os::Thread*>(p);    
    t->setEvent(evtRead);     
}

embot::hw::tlv493d::Position position = 0;

const embot::tools::Histogram::Values * vv {nullptr};

std::array<uint64_t, 100> vals {};

void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }
    
    
    signalhisto->on();
    embot::core::wait(30);
    signalhisto->off();
    
    vv = signalhisto->histogram()->getvalues();
    
    for(int i=0; i<vv->inside.size(); i++)
    {
        if(i < vals.size())
        {
            vals[i] = vv->inside[i];
        }
    }
    

    if(true == embot::core::binary::mask::check(eventmask, evtTick)) 
    {
        
    static uint8_t st = 0;
//    if(0 != st)
//    {
//        embot::core::wait(st*10);
//    }        
    st++;
    st %= 3;
    
//    static volatile std::uint64_t prev {0};
//    std::uint64_t volatile nnow = static_cast<std::uint64_t>(embot::core::now());
//    if(0 != prev)
//    {
//        volatile std::uint64_t delta = nnow - prev;
//        histoFOCperiod->add(delta + st);
//    }
//    prev = nnow;
//    
//    vv = histoFOCperiod->getvalues();
//    for(int i=0; i<vv->inside.size(); i++)
//    {
//        if(i < vals.size())
//        {
//            vals[i] = vv->inside[i];
//        }
//    }
//    
//    embot::core::Time focdelta {0};
//    validFOCperiod->tick(embot::core::now(), focdelta);
//    

//    vv = validFOCperiod->histogram()->getvalues();
//    for(int i=0; i<vv->inside.size(); i++)
//    {
//        if(i < vals.size())
//        {
//            vals[i] = vv->inside[i];
//        }
//    }
    
    
#if defined(enableTRACE_all)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("mainthread-onevent: evtTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif  

        embot::core::print("mainthread-onevent: called a fake reading of chip TLV493D which will be effectvively read when board fap arrives");    
        embot::core::Callback cbk00(alertdataisready00, t);
        embot::hw::tlv493d::acquisition(embot::hw::TLV493D::one, cbk00);        
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evtRead)) 
    {
#if defined(enableTRACE_all)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtRead received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif  

        if(embot::hw::resOK != embot::hw::tlv493d::read(embot::hw::TLV493D::one, position))
        {
            position = 66666;
        }
        
        embot::core::print("pos = " + std::to_string(0.01 * position) + "deg");
        
    }    
    
    if(true == embot::core::binary::mask::check(eventmask, evtBTNreleased)) 
    {    
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtBTNreleased received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));        
    }

}


void onIdle(embot::os::Thread *t, void* idleparam)
{
    static uint32_t i = 0;
    i++;
}

embot::os::EventThread* thr {nullptr};

void initSystem(embot::os::Thread *t, void* initparam)
{
    embot::core::print("this is a demo which shows that this code can run on a dev board and later on the real pmc board");    
    
    embot::core::print("starting the INIT thread");
    
    embot::core::print("creating the system services: timer manager + callback manager");
    
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
    
    embot::core::print("creating the LED pulser: it will blink a LED at 1 Hz.");
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(1*embot::core::time1second); 
       
    
    embot::os::EventThread::Config configEV { 
        6*1024, 
        embot::os::Priority::high40, 
        eventbasedthread_startup,
        nullptr,
        50*embot::core::time1millisec,
        eventbasedthread_onevent
    };
    
    embot::core::print("creating the main thread");
        
    // create the main thread 
    thr = new embot::os::EventThread;          
    // and start it
    thr->start(configEV); 
    
#if defined(TEST_EMBOT_OS_OSAL)    
    
    embot::os::PeriodicThread::Config pc { 
        4*1024, 
        embot::os::Priority::normal24, 
        [](embot::os::Thread *t, void *param) {
            static uint64_t zz{0};
            
            zz = embot::core::now();  
            zz = zz;            
        }, 
        nullptr,
        100*embot::core::time1millisec,
        [](embot::os::Thread *t, void *param) { 
            static uint64_t xx{0};
            
            xx = embot::core::now();
            
            embot::core::TimeFormatter tf(embot::core::now());        
            embot::core::print("periodic thread: exec @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full)); 
            
        }
    };

    embot::os::PeriodicThread *pth = new embot::os::PeriodicThread;
    pth->start(pc);    

#endif

    embot::core::print("quitting the INIT thread. Normal scheduling starts");    
}

// usart 921600

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    // steps:
    // 1. i init the embot::os
    // 2 i start the scheduler
        
    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 1024, nullptr, nullptr, onIdle };
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



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



