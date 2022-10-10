
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
#include "embot_tools.h"


constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
constexpr embot::core::relTime tickperiod = 1000*embot::core::time1millisec;
void onTick(embot::os::Thread *t);

#if defined(EMBOT_ENABLE_hw_button)
constexpr embot::os::Event evtBTNreleased = embot::core::binary::mask::pos2mask<embot::os::Event>(1);
void onBTNreleased(embot::os::Thread *t);
constexpr embot::hw::BTN buttonBLUE = embot::hw::BTN::one;
void btncallback(void *p)
{
    embot::os::Thread *t = reinterpret_cast<embot::os::Thread *>(p);
    t->setEvent(evtBTNreleased);
}
#endif

void testHWinit(embot::os::Thread *t);
void testHWtick(embot::os::Thread *t);

void eventbasedthread_startup(embot::os::Thread *t, void *param)
{       
    embot::core::print("evthread-startup: ..." ); 
    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);

#if defined(EMBOT_ENABLE_hw_button)    
    // init the ext interrupt button
    embot::hw::button::init(buttonBLUE, {embot::hw::button::Mode::TriggeredOnRelease, {btncallback, t}, 0});    
    embot::core::print("evthread-startup: started timer which sends evtTick to evthread every = " + embot::core::TimeFormatter(tickperiod).to_string());
#endif
    
    testHWinit(t);
    
}


void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }

    if(true == embot::core::binary::mask::check(eventmask, evtTick)) 
    {
        onTick(t);      
    }
 
#if defined(EMBOT_ENABLE_hw_button)    
    if(true == embot::core::binary::mask::check(eventmask, evtBTNreleased)) 
    {  
        onBTNreleased(t);         
    }
#endif
}


void onIdle(embot::os::Thread *t, void* idleparam)
{
    static uint32_t i = 0;
    i++;
}

embot::os::EventThread* thr {nullptr};

void initSystem(embot::os::Thread *t, void* initparam)
{
    
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
    
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(2*embot::core::time1second); 
       
    
    embot::os::EventThread::Config configEV { 
        6*1024, 
        embot::os::Priority::high40, 
        eventbasedthread_startup,
        nullptr,
        50*embot::core::time1millisec,
        eventbasedthread_onevent
    };
        
    // create the main thread 
    thr = new embot::os::EventThread;          
    // and start it
    thr->start(configEV);   
}

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

#if defined(EMBOT_ENABLE_hw_button)
void onBTNreleased(embot::os::Thread *t)
{
    embot::core::TimeFormatter tf(embot::core::now());        
    embot::core::print("evthread-onevent: evtBTNreleased received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));     
}
#endif


void onTick(embot::os::Thread *t)
{
#if defined(enableTRACE_all)        
    embot::core::TimeFormatter tf(embot::core::now());        
    embot::core::print("evthread-onevent: evtTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif  

    testHWtick(t);    
}



#define TEST_U27

#include "embot_hw_tlv493d.h"

constexpr embot::hw::TLV493D tlvU27 {embot::hw::TLV493D::six};

#if defined(EMBOT_ENABLE_hw_tlv493d_emulatedMODE)
const char msgTLV[] = "TLV is emulated";
#else
const char msgTLV[] = "TLV is true";
#endif

void testHWinit(embot::os::Thread *t)
{     

}

//struct WaitCBK
//{
//    embot::core::Callback & get() { init(); return cbk; }
//    void wait() { for(;;) { if(done) return; } }
//    
//    private:  
//        
//    void init() { done = false; }
//    volatile bool done {true};
//    static void stop(void *p) { reinterpret_cast<WaitCBK*>(p)->done =  true; }
//    embot::core::Callback cbk {stop, this};
//    
//};

struct Waiter : public embot::core::Callback
{
    Waiter() : Callback(stop, this) {}
        
    embot::core::Callback & callback() { init(); return *this; }
    
    bool wait(embot::core::relTime timeout = embot::core::reltimeWaitForever) 
    {
        embot::core::Time deadline {embot::core::now() + timeout};        
        for(;;) 
        { 
            volatile std::int64_t rem = deadline - embot::core::now();
            if(rem <= 0) { return false; }
            else if(done) { return true; }
        } 
    }
    
private:  
        
    void init() { done = false; }
    volatile bool done {true};
    static void stop(void *p) { reinterpret_cast<Waiter*>(p)->done =  true; }   
};

void tlv_init()
{
    embot::core::TimeFormatter tf(embot::core::now());
     
    if(embot::hw::resOK != embot::hw::tlv493d::init(tlvU27, {embot::hw::tlv493d::Config::startupMODE::dontresetCHIP}))
    {        
        embot::core::print("ERROR: embot::hw::tlv493d::init() failed @ time = " + tf.to_string());    
    }  
    else
    {
        embot::core::print("INFO: embot::hw::tlv493d::init() OK @ time = " + tf.to_string());
        embot::core::print(std::string("INFO: ") + msgTLV);
    }  
}

void tlv_get()
{
    embot::hw::tlv493d::Position pos {0};
    Waiter waiter;
    embot::core::TimeFormatter tf0(embot::core::now());
    embot::core::print("INFO: embot::hw::tlv493d::acquisition() @ time = " + tf0.to_string());
    
    embot::hw::tlv493d::acquisition(tlvU27, waiter.callback()); 
    
    bool res = waiter.wait();
    if(res)
    {
        embot::hw::tlv493d::read(tlvU27, pos);
    }
    
    embot::core::TimeFormatter tf(embot::core::now());
    embot::core::print("INFO: embot::hw::tlv493d::read() = " + std::to_string(pos) + " @ time = " + tf.to_string());    
}

void tlv_deinit()
{
    embot::core::TimeFormatter tf(embot::core::now());
     
    if(embot::hw::resOK != embot::hw::tlv493d::deinit(tlvU27))
    {        
        embot::core::print("ERROR: embot::hw::tlv493d::deinit() failed @ time = " + tf.to_string());    
    }  
    else
    {
        embot::core::print("INFO: embot::hw::tlv493d::deinit() OK @ time = " + tf.to_string());
        embot::core::print(std::string("INFO: ") + msgTLV);
    }  
}

void testHWtick(embot::os::Thread *t)
{
    static uint32_t cnt {0};
    
    if(0 == cnt)
    {
        tlv_init();
    }
    else if(cnt<4)
    {   
        tlv_get();
    }
    else if(4 == cnt)
    {
        tlv_deinit();
    }
    
    if(++cnt > 4)
    {
        cnt = 0;
    }
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



