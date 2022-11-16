
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
#include "embot_hw_can.h"
#include "embot_hw_timer.h"


#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"
#include "embot_app_scope.h"

#include <vector>


constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);

constexpr embot::core::relTime tickperiod = 5*1000*embot::core::time1millisec;


#if defined(TEST_EMBOT_HW)


//#define TEST_EMBOT_HW_SPI123

//#define TEST_EMBOT_HW_EEPROM
//#define TEST_EMBOT_HW_CHIP_M95512DF

//#define TEST_EMBOT_HW_ENCODER
//#define TEST_EMBOT_HW_CHIP_AS5045
//#define TEST_EMBOT_HW_CHIP_MB049

#define TEST_EMBOT_HW_FLASH
#if defined(TEST_EMBOT_HW_FLASH)

#define TEST_EMBOT_HW_FLASH_api

#define TEST_EMBOT_HW_FLASH_SAME_BANK_AS_CODESPACE
#define TEST_EMBOT_HW_FLASH_READ
#define TEST_EMBOT_HW_FLASH_ERASE
#define TEST_EMBOT_HW_FLASH_WRITE
#define TEST_EMBOT_HW_FLASH_WRITEandREADandCHECK


//#define TEST_EMBOT_HW_FLASH_EXECUTIONDURINGOPERATION
#if defined(TEST_EMBOT_HW_FLASH_EXECUTIONDURINGOPERATION)
//#define TEST_EMBOT_HW_FLASH_EXECUTIONDURINGOPERATION_tick1milli
#define TEST_EMBOT_HW_FLASH_EXECUTIONDURINGOPERATION_tick100micro
//#define TEST_EMBOT_HW_FLASH_EXECUTIONDURINGOPERATION_tick10micro
#endif

//constexpr size_t blocksizeKB {129};
constexpr size_t blocksizeKB {1};



#endif

//#define TEST_EMBOT_HW_CAN
#if defined(TEST_EMBOT_HW_CAN)
//#define TEST_EMBOT_HW_CAN_loopback_can1_to_can2_to_can1
//#define TEST_EMBOT_HW_CAN_loopback_can1_to_can2_to_can1_BURST
//#define TEST_EMBOT_HW_CAN_gateway_CAN2toCAN1
//#define TEST_EMBOT_HW_CAN_gateway_CAN1toCAN2
//#define TEST_EMBOT_HW_CAN_BURST
#endif

//#define TEST_EMBOT_HW_TIMER
//#define TEST_EMBOT_HW_TIMER_ONESHOT

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

#if defined(TEST_EMBOT_HW)    
    test_embot_hw_init();   
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
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("mainthread-onevent: evtTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));   

#if defined(TEST_EMBOT_HW)
        test_embot_hw_tick();
#endif
    }
    

}

#if defined(TEST_EMBOT_HW_CAN)

constexpr embot::core::relTime canTXperiod = 3*1000*embot::core::time1millisec;

constexpr embot::os::Event evtCAN1tx = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
constexpr embot::os::Event evtCAN1rx = embot::core::binary::mask::pos2mask<embot::os::Event>(1);
constexpr embot::os::Event evtCAN2rx = evtCAN1rx;

void alerteventbasedthread(void *arg)
{
    embot::os::EventThread* evthr = reinterpret_cast<embot::os::EventThread*>(arg);
    if(nullptr != evthr)
    {
        evthr->setEvent(evtCAN1rx);
    }
}

void can1_startup(embot::os::Thread *t, void *param)
{   
    volatile uint32_t c = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    c = c;
    
    // start can1 driver
    
    embot::hw::can::Config canconfig {};  
    canconfig.txcapacity = 32;  
    canconfig.rxcapacity = 32;        
    canconfig.onrxframe = embot::core::Callback(alerteventbasedthread, t); 
    embot::hw::can::init(embot::hw::CAN::one, canconfig);
//    embot::hw::can::setfilters(embot::hw::CAN::one, 1);   
    embot::hw::can::enable(embot::hw::CAN::one);        
    
    // start a command to periodically tx a frame
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtCAN1tx, t));
    embot::os::Timer::Config cfg{canTXperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);
    
    embot::core::print("tCAN1: started timer triggers CAN communication every = " + embot::core::TimeFormatter(canTXperiod).to_string()); 
    
    embot::core::print("tCAN1: started CAN1 driver");    
      
}


void can1_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }
    
    embot::core::TimeFormatter tf(embot::core::now());

    if(true == embot::core::binary::mask::check(eventmask, evtCAN1tx)) 
    { 
#if defined(TEST_EMBOT_HW_CAN_BURST)
        embot::core::print(" ");
        embot::core::print("-------------------------------------------------------------------------");
        embot::core::print("tCAN1 -> START OF transmissions from CAN1 in burst mode ");
        embot::core::print("-------------------------------------------------------------------------");        
        embot::core::print("tCAN1: evtCAN1tx received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));       
        uint8_t payload[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
        
        constexpr uint8_t burstsize = 12;
        enum class burstmode { 
            putall_then_transmit,               // as we do in can boards such as strain2 or in ems when in run state
            loop_put_transmit_eachtime,         // as we do in ems when in cfg state
            loop_put_transmit_onlyfirsttime     // a new mode i wnat to test 
        };
        constexpr burstmode bm {burstmode::loop_put_transmit_eachtime};
        constexpr bool getoutputsize {true}; // to verify if a irq tx disable / enable gives problems
        constexpr embot::core::relTime delay {0}; // 0 10 200 300 400 500 

        embot::core::print("tCAN1: will now transmit on CAN1 a burst of " + std::to_string(burstsize) + " frames w/ data[0] = " + std::to_string(payload[0])); 
        embot::hw::can::Frame hwtxframe {2, 8, payload};
        for(uint8_t n=0; n<burstsize; n++)
        {
            hwtxframe.data[0]++;
            embot::hw::can::put(embot::hw::CAN::one, hwtxframe); 
            if(burstmode::loop_put_transmit_eachtime == bm)
            {
                embot::hw::can::transmit(embot::hw::CAN::one);                 
                if(delay > 0)
                {
                    embot::hw::sys::delay(delay);
                }
            }  

            if((burstmode::loop_put_transmit_onlyfirsttime == bm) && (0 == n))
            {
                embot::hw::can::transmit(embot::hw::CAN::one); 
                if(delay > 0)
                {
                    embot::hw::sys::delay(delay);
                }
            }

            if(true == getoutputsize)
            {
                embot::hw::can::inputqueuesize(embot::hw::CAN::one);
            }    
        }   

        if(burstmode::putall_then_transmit == bm)
        {
            embot::hw::can::transmit(embot::hw::CAN::one); 
        }   
        
        embot::core::print(" ");

#endif
        
#if defined(TEST_EMBOT_HW_CAN_loopback_can1_to_can2_to_can1)  
        embot::core::print(" ");
        embot::core::print("-------------------------------------------------------------------------");
        embot::core::print("tCAN1 -> START OF transmissions from CAN1 to CAN2 and back to CAN1");
        embot::core::print("-------------------------------------------------------------------------");        
        embot::core::print("tCAN1: evtCAN1tx received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));       
        uint8_t payload[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        static uint8_t cnt {0};
        cnt++;
        for(auto &i : payload) i+=cnt;
        
        constexpr uint8_t burstsize = 2;

        if(burstsize < 2)
        {    
            embot::core::print("tCAN1: will now transmit on CAN1 1 frame w/ data[0] = " + std::to_string(payload[0])); 
            embot::hw::can::put(embot::hw::CAN::one, {2, 8, payload}); 
        }
        else
        {
            embot::core::print("tCAN1: will now transmit on CAN1 a burst of " + std::to_string(burstsize) + " frames w/ data[0] = " + std::to_string(payload[0]) + " and decreasing sizes"); 
            embot::hw::can::Frame hwtxframe {2, 8, payload};
            for(uint8_t n=0; n<burstsize; n++)
            {
                hwtxframe.size = 8-n;
                embot::core::print("tCAN1: tx frame w/ size = " + std::to_string(hwtxframe.size));
                embot::hw::can::put(embot::hw::CAN::one, hwtxframe);  
            }   
        }            
            
        embot::hw::can::transmit(embot::hw::CAN::one);
        embot::core::print(" ");
#endif        
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evtCAN1rx)) 
    {                
        embot::core::print("tCAN1: evtCAN1rx received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));       
        embot::hw::can::Frame hwframe {};
        std::uint8_t remainingINrx = 0;
        if(embot::hw::resOK == embot::hw::can::get(embot::hw::CAN::one, hwframe, remainingINrx))
        {  
            embot::core::print("tCAN1: decoded frame w/ [id size {payload} ] = " + 
                                std::to_string(hwframe.id) + ", " +
                                std::to_string(hwframe.size) + ", [" +
                                std::to_string(hwframe.data[0]) + ", " +
                                std::to_string(hwframe.data[1]) + ", " +
                                std::to_string(hwframe.data[3]) + ", ...} ]"
            );
            embot::core::print(" ");            

#if defined(TEST_EMBOT_HW_CAN_gateway_CAN1toCAN2)
            embot::core::print("fwd to can2");
            embot::hw::can::put(embot::hw::CAN::two, hwframe); 
            embot::hw::can::transmit(embot::hw::CAN::two);  
#endif              
                                  
            if(remainingINrx > 0)
            {
                t->setEvent(evtCAN1rx);                 
            }
        }         
    
    }
    
}


void can2_startup(embot::os::Thread *t, void *param)
{   
    volatile uint32_t c = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    c = c;
    
    // start can2 driver
    
    embot::hw::can::Config canconfig {}; 
    canconfig.txcapacity = 32;  
    canconfig.rxcapacity = 32;         
    canconfig.onrxframe = embot::core::Callback(alerteventbasedthread, t); 
    embot::hw::can::init(embot::hw::CAN::two, canconfig);
    //embot::hw::can::setfilters(embot::hw::CAN::two, 2);   
    embot::hw::can::enable(embot::hw::CAN::two);        
    
    embot::core::print("tCAN2: started CAN2 driver");    
      
}


void can2_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }
    
    embot::core::TimeFormatter tf(embot::core::now());

    if(true == embot::core::binary::mask::check(eventmask, evtCAN2rx)) 
    {       
        embot::core::print("tCAN2: evtCAN2rx received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));       
        embot::hw::can::Frame hwframe {};
        std::uint8_t remainingINrx = 0;
        if(embot::hw::resOK == embot::hw::can::get(embot::hw::CAN::two, hwframe, remainingINrx))
        {  
            embot::core::print("tCAN2: decoded frame w/ [id size {payload} ] = " + 
                                std::to_string(hwframe.id) + ", " +
                                std::to_string(hwframe.size) + ", [" +
                                std::to_string(hwframe.data[0]) + ", " +
                                std::to_string(hwframe.data[1]) + ", " +
                                std::to_string(hwframe.data[3]) + ", ...} ]"
                                ); 



#if defined(TEST_EMBOT_HW_CAN_gateway_CAN2toCAN1)
            embot::core::print("fwd to can1");
            embot::hw::can::put(embot::hw::CAN::one, hwframe); 
            embot::hw::can::transmit(embot::hw::CAN::one);  
#else            
            embot::core::print("tCAN2: and now sending back a short reply on CAN2 w/ payload[0] = size of rx frame");
            embot::core::print(" ");            
            uint8_t payload[1] = { hwframe.size };
            embot::hw::can::put(embot::hw::CAN::two, {1, 1, payload}); 
            embot::hw::can::transmit(embot::hw::CAN::two);     
#endif                                  
            if(remainingINrx > 0)
            {
                t->setEvent(evtCAN2rx);                 
            }
        }         
    
    }
    
}


void tCAN1(void *p)
{
    embot::os::Thread* t = reinterpret_cast<embot::os::Thread*>(p);
    t->run();
}

void tCAN2(void *p)
{
    embot::os::Thread* t = reinterpret_cast<embot::os::Thread*>(p);
    t->run();
}

#endif // #if defined(TEST_EMBOT_HW_CAN)

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

// entry point (first running thread)
void initSystem(embot::os::Thread *t, void* initparam)
{
    volatile uint32_t cpufreq = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    cpufreq = cpufreq;
    embot::core::print("this is a demo which shows that this code can run on a dev board. clock = " + std::to_string(cpufreq/1000000) + " MHz");    
    
    embot::core::print("starting the INIT thread");
    
    embot::core::print("INIT: creating the system services: timer manager + callback manager");
    
    // start the services with default params
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
        
    constexpr embot::core::Time blinktime {250*embot::core::time1millisec};
                    
    embot::core::print("INIT: creating the LED pulser: it will blink a LED @" + embot::core::TimeFormatter(blinktime).to_string());
        
    static const std::initializer_list<embot::hw::LED> allleds = 
    {   
        embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three, embot::hw::LED::four, embot::hw::LED::five, embot::hw::LED::six   
    };  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::two).pulse(blinktime); 
//    embot::app::LEDwaveT<64> ledwave(100*embot::core::time1millisec, 50, std::bitset<64>(0b010101));
//    theleds.get(embot::hw::LED::two).wave(&ledwave); 
    
    
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
 
#if defined(TEST_EMBOT_HW_CAN)
    
    embot::core::print("initting two threads, each one managing a different embot::hw::CAN");    
    
    embot::os::EventThread::Config configEVcan1 { 
        4*1024, 
        embot::os::Priority::normal24, 
        can1_startup,
        nullptr,
        50*embot::core::time1millisec,
        can1_onevent,
        "can1ThreadEvt"
    };
               
    // create the can1 thread 
    embot::os::EventThread *thrcan1 {nullptr};
    thrcan1 = new embot::os::EventThread;          
    // and start it. w/ osal it will be displayed w/ label tMAIN
    thrcan1->start(configEVcan1, tCAN1); 
    
    
    embot::os::EventThread::Config configEVcan2 { 
        4*1024, 
        embot::os::Priority::normal25, 
        can2_startup,
        nullptr,
        50*embot::core::time1millisec,
        can2_onevent,
        "can2ThreadEvt"
    };
               
    // create the can2 thread 
    embot::os::EventThread *thrcan2 {nullptr};
    thrcan2 = new embot::os::EventThread;          
    // and start it. w/ osal it will be displayed w/ label tMAIN
    thrcan2->start(configEVcan2, tCAN2);    
    
#endif // #if defined(TEST_EMBOT_HW_CAN)
    
    embot::core::print("quitting the INIT thread. Normal scheduling starts");    
}


// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    // steps:
    // 1. i init the embot::os
    // 2. i start the scheduler
        
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

#if defined(TEST_EMBOT_HW_FLASH)
#include "embot_hw_flash.h"
//#include "embot_hw_flash_bsp.h"
#if defined(TEST_EMBOT_HW_FLASH_EXECUTIONDURINGOPERATION)
#include "embot_hw_timer.h"
constexpr embot::hw::TIMER flashexec_timer {embot::hw::TIMER::one};
constexpr embot::core::relTime flashexec_period 
#if defined(TEST_EMBOT_HW_FLASH_EXECUTIONDURINGOPERATION_tick1milli)    
{ 1 * embot::core::time1millisec };
#elif defined(TEST_EMBOT_HW_FLASH_EXECUTIONDURINGOPERATION_tick100micro)
{ 100 * embot::core::time1microsec };
#elif defined(TEST_EMBOT_HW_FLASH_EXECUTIONDURINGOPERATION_tick10micro)
{ 10 * embot::core::time1microsec };
#endif
volatile uint32_t flashexec_ticks {0};
void flashexec_onexpiry(void *p)
{ 
    flashexec_ticks++;
};

embot::hw::timer::Config flashexec_timerConfig {
    flashexec_period,
    embot::hw::timer::Mode::periodic, 
    {flashexec_onexpiry, nullptr},
};
#endif
#endif

#if defined(TEST_EMBOT_HW_SPI123)
#include "embot_hw_spi.h"
#endif

#if defined(TEST_EMBOT_HW_EEPROM)
#include "embot_hw_eeprom.h"

constexpr embot::hw::EEPROM eeprom2test {embot::hw::EEPROM::one};

#endif

#if defined(TEST_EMBOT_HW_ENCODER)
    #include "embot_hw_encoder.h"
#endif
#if defined(TEST_EMBOT_HW_CHIP_M95512DF)
    #include "embot_hw_chip_M95512DF.h"
#endif
#if defined(TEST_EMBOT_HW_CHIP_AS5045)
    #include "embot_hw_chip_AS5045.h"
#endif
#if defined(TEST_EMBOT_HW_CHIP_MB049)
    #include "embot_hw_chip_MB049.h"
#endif

void done1(void* p)
{
    embot::core::print("SPI::one cbk called");
}
void done2(void* p)
{
    embot::core::print("SPI::two cbk called");
}
void done3(void* p)
{
    embot::core::print("SPI::three cbk called");
}

// ---------------------------------------------------------------------------------------------

static std::uint8_t on = 0;
constexpr embot::os::Event evtTIM_HW = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
void timer_cbk(void* p)
{
    embot::os::EventThread* thr = reinterpret_cast<embot::os::EventThread*>(p);
    thr->setEvent(evtTIM_HW);
}

void toggleLED(void*)
{
    if(0 == on)
    {
        embot::hw::led::off(embot::hw::LED::five); 
        on = 1;
    }
    else 
    {
        embot::hw::led::on(embot::hw::LED::five);
        on = 0;
    }    
//    embot::hw::chip::testof_AS5045();
}

void tim_hw_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{
    if( eventmask == evtTIM_HW)
    {
        toggleLED(nullptr);
    }
}


void tTIMTEST(void *p)
{
    embot::os::Thread* t = reinterpret_cast<embot::os::Thread*>(p);
    t->run();
}

#if defined(TEST_EMBOT_HW_TIMER_ONESHOT)

constexpr embot::hw::TIMER timeroneshot2test {embot::hw::TIMER::one};
constexpr embot::core::relTime timeoneshot {1400*embot::core::time1microsec};
constexpr embot::core::relTime timeperiodic {1000*embot::core::time1microsec};
constexpr embot::hw::TIMER timerperiodic2test {embot::hw::TIMER::two};

embot::app::scope::SignalEViewer *sigEVstart {nullptr};
embot::app::scope::SignalEViewer *sigEV01oneshot {nullptr};
embot::app::scope::SignalEViewer *sigEV01period {nullptr};
embot::app::scope::SignalEViewer *sigEV02period {nullptr};
embot::app::scope::SignalEViewer *sigEVenc {nullptr};

void timer02_on_period(void *p)
{
    sigEV02period->on();
    sigEV02period->off();     
}


void timer01_on_period(void *p)
{
    sigEV01period->on();
    sigEV01period->off();   
}

void timer01_on_oneshot(void *p)
{
    sigEV01oneshot->on();
    sigEV01oneshot->off();
    
    constexpr embot::hw::timer::Config cfg {
        timeperiodic,
        embot::hw::timer::Mode::periodic, 
        {timer01_on_period, nullptr},
    };
    
    embot::hw::timer::configure(timeroneshot2test, cfg);

    // start the timer again
    embot::hw::timer::start(timeroneshot2test);    
}

void enc_on_read_completion(void *p)
{
    sigEVenc->on();
    sigEVenc->off();
}

void tmrSTART() {}
void tmr01ONESHOT() {}
void tmr01PERIOD() {}
void tmr02PERIOD() {}
void readENC() {}
    
#endif

void test_embot_hw_init()
{
#if defined(TEST_EMBOT_HW_TIMER_ONESHOT)

    // 2. Create and initialize the timer with the callback defined above
//    embot::core::Callback timer_oneshot_cbk { timer_on_oneshot, nullptr };
    
//    constexpr embot::core::relTime timeoneshot {1400*embot::core::time1microsec};
//    constexpr embot::core::relTime period {embot::core::time1microsec  * 50};
//    constexpr embot::core::relTime period {embot::core::time1millisec  * 1000};
    
    sigEVstart = new embot::app::scope::SignalEViewer({tmrSTART, embot::app::scope::SignalEViewer::Config::LABEL::one});
    sigEV01oneshot = new embot::app::scope::SignalEViewer({tmr01ONESHOT, embot::app::scope::SignalEViewer::Config::LABEL::two});
    sigEV01period = new embot::app::scope::SignalEViewer({tmr01PERIOD, embot::app::scope::SignalEViewer::Config::LABEL::three});
    sigEV02period = new embot::app::scope::SignalEViewer({tmr02PERIOD, embot::app::scope::SignalEViewer::Config::LABEL::four});
    sigEVenc = new embot::app::scope::SignalEViewer({readENC, embot::app::scope::SignalEViewer::Config::LABEL::five});


        
    constexpr embot::hw::timer::Config timeroneshotConfig {
        timeoneshot,
        embot::hw::timer::Mode::oneshot, 
        {timer01_on_oneshot, nullptr},
    };
    
    
    //constexpr embot::hw::TIMER timer2test {embot::hw::TIMER::fifteen};
    //constexpr embot::hw::TIMER timer2test {embot::hw::TIMER::sixteen};
    embot::hw::timer::init(timeroneshot2test, timeroneshotConfig);
    embot::hw::timer::init(timerperiodic2test, {1500, embot::hw::timer::Mode::periodic, {timer02_on_period, nullptr}});
    
    sigEVstart->on();
    sigEVstart->off();

    // 3. Start the timer
    embot::hw::timer::start(timeroneshot2test);
    embot::hw::timer::start(timerperiodic2test);
#endif

    
#if defined(TEST_EMBOT_HW_TIMER)
    
    embot::hw::led::init(embot::hw::LED::five);
    
    // 1. Configure and create a thread that will toggle the LED when the event evtTIM_HW is set. 
    embot::core::print("Creating a thread that manages the timer callback.");
    
    embot::os::EventThread::Config configEV { 
        6*1024, 
        embot::os::Priority::high40, 
        nullptr,
        nullptr,
        embot::core::reltimeWaitForever,
        tim_hw_onevent,
        "timThreadEvt"
    };
    
    embot::os::EventThread *thr {nullptr};
    thr = new embot::os::EventThread;          
    thr->start(configEV, tTIMTEST);
    
    // 2. Create and initialize the timer with the callback defined above
    embot::core::Callback tim_hw_cbk { timer_cbk, thr };
    
    constexpr embot::core::relTime period {embot::core::time1millisec  * 1};
//    constexpr embot::core::relTime period {embot::core::time1microsec  * 50};
//    constexpr embot::core::relTime period {embot::core::time1millisec  * 1000};

    embot::hw::timer::Config timerConfig {
        period,
        embot::hw::timer::Mode::periodic, 
        tim_hw_cbk,
    };
    
    constexpr embot::hw::TIMER timer2test {embot::hw::TIMER::thirteen};
    //constexpr embot::hw::TIMER timer2test {embot::hw::TIMER::fifteen};
    //constexpr embot::hw::TIMER timer2test {embot::hw::TIMER::sixteen};
    embot::hw::timer::init(timer2test, timerConfig);

    // 3. Start the timer
    embot::hw::timer::start(timer2test);
#endif
    
#if defined(TEST_EMBOT_HW_FLASH)

#if defined(TEST_EMBOT_HW_FLASH_EXECUTIONDURINGOPERATION)

    embot::hw::timer::init(flashexec_timer, flashexec_timerConfig);
    
#endif

#endif
    
#if defined(TEST_EMBOT_HW_SPI123)
    
    embot::hw::spi::Config spi1cfg =
    { 
        embot::hw::spi::Prescaler::eight, 
        embot::hw::spi::DataSize::eight, 
        embot::hw::spi::Mode::zero,
        { {embot::hw::gpio::Pull::nopull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Pull::none} }
    };
    embot::hw::spi::init(embot::hw::SPI::one, spi1cfg);
    embot::hw::spi::init(embot::hw::SPI::two, spi1cfg);
    embot::hw::spi::init(embot::hw::SPI::three, spi1cfg);
    
    static uint8_t spirxdata[4] = {0};
    embot::core::Data rxdata { spirxdata, sizeof(spirxdata) };   
    embot::hw::result_t r {embot::hw::resNOK};

    memset(spirxdata, 0, sizeof(spirxdata));    
    r = embot::hw::spi::read(embot::hw::SPI::one, rxdata, 5*embot::core::time1millisec);    
    embot::core::print("SPI1 test: " + std::string(r==embot::hw::resNOK ? "KO ":"OK ") + std::to_string(spirxdata[0]) );
    spirxdata[0] = spirxdata[0];
    embot::hw::spi::read(embot::hw::SPI::one, rxdata, {done1, nullptr}); 
    
    memset(spirxdata, 0, sizeof(spirxdata));    
    r = embot::hw::spi::read(embot::hw::SPI::two, rxdata, 5*embot::core::time1millisec);    
    embot::core::print("SPI2 test: " + std::string(r==embot::hw::resNOK ? "KO ":"OK ") + std::to_string(spirxdata[0]) );
    spirxdata[0] = spirxdata[0];  
    embot::hw::spi::read(embot::hw::SPI::two, rxdata, {done2, nullptr});    
    
    
    memset(spirxdata, 0, sizeof(spirxdata));    
    r = embot::hw::spi::read(embot::hw::SPI::three, rxdata, 5*embot::core::time1millisec);    
    embot::core::print("SPI3 test: " + std::string(r==embot::hw::resNOK ? "KO ":"OK ") + std::to_string(spirxdata[0]) );
    spirxdata[0] = spirxdata[0];   
    embot::hw::spi::read(embot::hw::SPI::three, rxdata, {done3, nullptr});     
#endif


#if defined(TEST_EMBOT_HW_ENCODER)

    embot::hw::encoder::Config cfgONE   { .type = embot::hw::encoder::Type::chipAS5045 };
    embot::hw::encoder::Config cfgTWO   { .type = embot::hw::encoder::Type::chipAS5045 };
    embot::hw::encoder::Config cfgTHREE { .type = embot::hw::encoder::Type::chipAS5045 };
    
    embot::hw::ENCODER encoder_ONE = embot::hw::ENCODER::one;
    embot::hw::ENCODER encoder_TWO = embot::hw::ENCODER::two;
    embot::hw::ENCODER encoder_THREE = embot::hw::ENCODER::three;
    
    uint16_t posONE, posTWO, posTHREE = 0;
    
    // init the encoder(s)
    if( embot::hw::resOK == embot::hw::encoder::init(encoder_ONE, cfgONE) &&
        embot::hw::resOK == embot::hw::encoder::init(encoder_TWO, cfgTWO) &&
        embot::hw::resOK == embot::hw::encoder::init(encoder_THREE, cfgTHREE))
    {
        for(;;)
        {
            sigEVenc->on();
            sigEVenc->off();
            
            // start the encoder reading
            embot::hw::encoder::startRead(encoder_ONE);
            embot::hw::encoder::startRead(encoder_TWO);
            embot::hw::encoder::startRead(encoder_THREE);
            
            for(;;)
            {
                // try to get the value read when the data is ready
                if(embot::hw::resOK == embot::hw::encoder::getValue(encoder_ONE, posONE) &&
                   embot::hw::resOK == embot::hw::encoder::getValue(encoder_TWO, posTWO) &&
                   embot::hw::resOK == embot::hw::encoder::getValue(encoder_THREE, posTHREE))
                {
                    //embot::core::print(std::to_string(posONE) + " | " + 
                    //                   std::to_string(posTWO) + " | " +
                    //                   std::to_string(posTHREE));
                    sigEVenc->on();
                    sigEVenc->off();
                    break;
                }
            }
            embot::core::wait(600); // "simulate" DO + TX phase
        }
    }

#elif defined(TEST_EMBOT_HW_CHIP_AS5045)

    embot::hw::chip::testof_AS5045();

#elif defined(TEST_EMBOT_HW_CHIP_MB049)

    embot::hw::chip::testof_MB049();

#endif


#if defined(TEST_EMBOT_HW_CHIP_M95512DF)

    embot::hw::chip::testof_M95512DF();    

#endif    
    
#if defined(TEST_EMBOT_HW_EEPROM)
    
//    if(embot::hw::resOK == embot::hw::eeprom::init(eeprom2test, {});

    embot::hw::eeprom::init(eeprom2test, {});
    
     
    uint32_t ciao[3] = {1, 2, 3};
    void *arg {ciao}; 
    auto lambda = [](void *p){ 
         
        uint32_t *data = reinterpret_cast<uint32_t*>(p);
        data[1] = 7;
    };
    embot::core::Callback cbk1 {lambda, &ciao};
    
    cbk1.execute();
    
    ciao[1] = ciao[1];

#endif
    
    
}

#if defined(TEST_EMBOT_HW_EEPROM)
constexpr size_t capacity {2048};
uint8_t dd[capacity] = {0};
//constexpr size_t adr2use {128 - 8};
constexpr size_t adr2use {0};

volatile uint8_t stophere = 0;

embot::core::Time startread {0};
embot::core::Time readtime {0};
embot::core::Time startwrite {0};
#endif


#if defined(TEST_EMBOT_HW_FLASH)

embot::core::Time writetime {0};
embot::core::Time erasetime {0};
embot::core::Time readtime {0};
embot::core::Time start{0};
embot::core::Time activation{0};

volatile uint32_t flashexec_write_ticks {0};
volatile uint32_t flashexec_erase_ticks {0};
volatile uint32_t flashexec_read_ticks {0};



uint32_t d2flash[blocksizeKB*1024/4] = {0};   // nk KB
uint32_t readback[blocksizeKB*1024/4] = {0};  // nk KB

void fill(uint8_t tmp)
{
#define FILLMODE_eachdifferent_incremental
#if defined(FILLMODE_samevalue)
    std::memset(d2flash, tmp, sizeof(d2flash)); 
#else
    size_t s = sizeof(d2flash) / 4;
    for(size_t i=0; i<s; i++)
    {
#if defined(FILLMODE_eachdifferent_rand)        
        d2flash[i] = rand();
#elif defined(FILLMODE_eachdifferent_incremental)        
        d2flash[i] = i;
#else
        #error choose one FILLMODE_
#endif 
    }
#endif    
}

char debug_print[256] = {0};
void printdifferences()
{
    size_t lines = sizeof(d2flash) / 4 / 8;
    for(uint32_t l=0; l<lines; l++)
    {
        snprintf(debug_print, sizeof(debug_print), "wanted: %08x %08x %08x %08x %08x %08x %08x %08x", 
                    d2flash[8*l  ], d2flash[8*l+1], d2flash[8*l+2], d2flash[8*l+3],
                    d2flash[8*l+4], d2flash[8*l+5], d2flash[8*l+6], d2flash[8*l+7]
        );
        embot::core::print(debug_print);
        snprintf(debug_print, sizeof(debug_print), "read:   %08x %08x %08x %08x %08x %08x %08x %08x", 
                    readback[8*l  ], readback[8*l+1], readback[8*l+2], readback[8*l+3],
                    readback[8*l+4], readback[8*l+5], readback[8*l+6], readback[8*l+7]
        );
        embot::core::print(debug_print);
    }    
    
}

#endif

#if defined(TEST_EMBOT_HW_FLASH_api)
namespace test {
void APIflashBSP();
}
#endif

void test_embot_hw_tick()
{
    static size_t cnt = 0;
    cnt++;
      
#if defined(TEST_EMBOT_HW_FLASH) 
    
    constexpr size_t period {1};     
    static uint8_t tmp_data = 0x20;
    if (0 == (cnt % period))
    {
#if defined(TEST_EMBOT_HW_FLASH_api)        
        test::APIflashBSP();
#endif
        
#if defined(TEST_EMBOT_HW_FLASH_SAME_BANK_AS_CODESPACE)
        constexpr embot::hw::flash::Partition::ID partitionID = embot::hw::flash::Partition::ID::eapplication00;
        std::string flashaddress {"0x08060000"}; 
        // application00 -> 0x08060000
#else
        constexpr embot::hw::flash::Partition::ID partitionID = embot::hw::flash::Partition::ID::eapplication01;
        std::string flashaddress {"0x08120000"};
        // application01 -> 0x08120000
#endif 

       
        size_t partition_address = embot::hw::flash::bsp::partition(partitionID).address;
        size_t partition_size = embot::hw::flash::bsp::partition(partitionID).size;
   
        activation = embot::core::now();
        bool OKcheck {true};
        
        bool resfl {false};
        
#if defined(TEST_EMBOT_HW_FLASH_READ)        
        std::memset(readback, 0, sizeof(readback));
        embot::hw::led::on(embot::hw::LED::six);
        start = embot::core::now();
        embot::hw::flash::read(partition_address, sizeof(readback), readback);
        readtime = embot::core::now() - start;
        embot::hw::led::off(embot::hw::LED::six);
#endif

#if defined(TEST_EMBOT_HW_FLASH_ERASE)
        embot::hw::led::on(embot::hw::LED::five);           
        start = embot::core::now();
        
#if defined(TEST_EMBOT_HW_FLASH_EXECUTIONDURINGOPERATION)
        flashexec_ticks = 0;
        flashexec_erase_ticks = 0;
        embot::hw::timer::start(flashexec_timer);    
#endif         
        resfl = embot::hw::flash::erase(partition_address, partition_size); // whole partition, actually
#if defined(TEST_EMBOT_HW_FLASH_EXECUTIONDURINGOPERATION)
        embot::hw::timer::stop(flashexec_timer); 
        flashexec_erase_ticks = flashexec_ticks;
        flashexec_ticks = 0;        
#endif

        erasetime = embot::core::now() - start;        
        embot::hw::led::off(embot::hw::LED::five);
        
        if(false == resfl)
        {
            embot::core::print("failure of embot::hw::flash::erase()"); 
        }
        
#endif // TEST_EMBOT_HW_FLASH_ERASE
        
#if defined(TEST_EMBOT_HW_FLASH_WRITE)        
//        std::memset(readback, 0, sizeof(readback));
//        embot::hw::flash::read(partition_address, sizeof(readback), readback);
        
        fill(tmp_data++);
        embot::hw::led::on(embot::hw::LED::three); 
        start = embot::core::now();
        
#if defined(TEST_EMBOT_HW_FLASH_EXECUTIONDURINGOPERATION)
        flashexec_ticks = 0;
        flashexec_write_ticks = 0;
        embot::hw::timer::start(flashexec_timer);    
#endif        
        resfl = embot::hw::flash::write(partition_address, sizeof(d2flash), d2flash);        
#if defined(TEST_EMBOT_HW_FLASH_EXECUTIONDURINGOPERATION)
        embot::hw::timer::stop(flashexec_timer); 
        flashexec_write_ticks = flashexec_ticks;
        flashexec_ticks = 0;        
#endif

        writetime = embot::core::now() - start;
        embot::hw::led::off(embot::hw::LED::three);  
        if(false == resfl)
        {
            embot::core::print("failure of embot::hw::flash::write()"); 
        }
        
#if defined(TEST_EMBOT_HW_FLASH_WRITEandREADandCHECK) 

        std::memset(readback, 0, sizeof(readback));  
        start = embot::core::now();    
        
#if defined(TEST_EMBOT_HW_FLASH_EXECUTIONDURINGOPERATION)
        flashexec_ticks = 0;
        flashexec_read_ticks = 0;
        embot::hw::timer::start(flashexec_timer);    
#endif             
        embot::hw::flash::read(partition_address, sizeof(readback), readback);
#if defined(TEST_EMBOT_HW_FLASH_EXECUTIONDURINGOPERATION)
        embot::hw::timer::stop(flashexec_timer); 
        flashexec_read_ticks = flashexec_ticks;
        flashexec_ticks = 0;        
#endif 
        
        readtime = embot::core::now() - start;
        
        OKcheck = (0 == std::memcmp(d2flash, readback, sizeof(readback))); 
        
        if(false == OKcheck)
        {
            embot::hw::led::on(embot::hw::LED::one);
            printdifferences();
        }
#endif // TEST_EMBOT_HW_FLASH_WRITEandREADandCHECK

#endif // TEST_EMBOT_HW_FLASH_WRITE
        

#if defined(TEST_EMBOT_HW_FLASH_READ) | defined(TEST_EMBOT_HW_FLASH_ERASE) | defined(TEST_EMBOT_HW_FLASH_WRITE) | defined(TEST_EMBOT_HW_FLASH_WRITEandREADandCHECK) 
        embot::core::print(
        std::string("FLASH op addr = ") + flashaddress + ": "
//             + embot::core::TimeFormatter(activation).to_string() + ": " +
            "partition size = " + std::to_string(partition_size/1024) + " KB, " +
            "block size = " + std::to_string(sizeof(d2flash)/1024) + " KB, " +
            "erase time of whole partition = " + embot::core::TimeFormatter(erasetime).to_string() + ", " +
            "write time of block = " + embot::core::TimeFormatter(writetime).to_string() + ", " +
            "read time of block = " + embot::core::TimeFormatter(readtime).to_string() + ", " +
            "readback = " + std::string(OKcheck?"OK":"FCUKO")            
            );
#endif

#if defined(TEST_EMBOT_HW_FLASH_EXECUTIONDURINGOPERATION)
        embot::core::print(
            std::string("FLASH concurrency w/ hw timer tick period = ") + embot::core::TimeFormatter(flashexec_period).to_string() + ": "
            "erase timer ticks = " + std::to_string(flashexec_erase_ticks) + ", " +
            "write timer ticks = " + std::to_string(flashexec_write_ticks) + ", " +
            "read timer ticks = " + std::to_string(flashexec_read_ticks)           
            );
#endif
        
    }
    else
    {
        embot::core::print("tick number: " + std::to_string(cnt));
    }
#endif // TEST_EMBOT_HW_FLASH    
    
    
#if defined(TEST_EMBOT_HW_EEPROM)

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
    
    
    std::memset(dd, 0, sizeof(dd));
    embot::core::Data data {dd, numberofbytes};
    constexpr embot::core::relTime tout {3*embot::core::time1millisec};
    
    startread = embot::core::now(); 
    embot::hw::eeprom::read(eeprom2test, adr2use, data, 3*embot::core::time1millisec);
    readtime = embot::core::now() - startread;
    stophere++;
    
    std::memset(dd, cnt, sizeof(dd));
    startwrite = embot::core::now(); 
    embot::hw::eeprom::write(eeprom2test, adr2use, data, 3*embot::core::time1millisec);
    writetime = embot::core::now() - startwrite;
    stophere++;
    
//    embot::hw::eeprom::erase(eeprom2test, adr2use+1, 200, 3*embot::core::time1millisec);
//    embot::hw::eeprom::erase(eeprom2test, 3*embot::core::time1millisec);  
    
    std::memset(dd, 0, sizeof(dd));
    embot::hw::eeprom::read(eeprom2test, adr2use, data, 3*embot::core::time1millisec);
     
    stophere++;
    
    embot::core::print(std::string("numberofbytes = ") + std::to_string(numberofbytes) + ", read time = " + embot::core::TimeFormatter(readtime).to_string() + ", write time = " + embot::core::TimeFormatter(writetime).to_string());
    
    #endif    
}

#endif


#if defined(TEST_EMBOT_HW_FLASH_api)

#include "embot_hw_flash.h"

using namespace embot::hw;

namespace test {
    
constexpr size_t datasize {32};
constexpr uint32_t datawrite[datasize/sizeof(uint32_t)] = {0, 1, 2, 3, 4, 5, 6, 7};
uint32_t dataread[datasize/sizeof(uint32_t)] = {0};

void APIflashBasic()
{
    flash::erase(0x08100000, 128*1024);             // must erase entire page
    flash::write(0x08100000, datasize, datawrite);  // write 32 bytes
    flash::read(0x08100000, datasize, dataread);    // read them back    
}

void APIflashBSP()
{
    embot::core::print("");
    embot::core::print(
            "FLASH test API of embot::hw::flash");
    
    // get the handler of the first Bank and operate on it by pages
    std::vector<embot::hw::flash::Page> thepages {};
    const embot::hw::flash::Bank &bank1 = flash::bsp::bank(flash::Bank::ID::one);
    if(!bank1.isvalid())
    {
        embot::core::print(
            "FLASH (bank): Bank::one is not configured in the BSP");
    }
    else    
    {
        // retrieves info of all the pages inside
        bank1.pages(thepages);
        embot::core::print(
            std::string("FLASH (pages): on Bank::one their number is = ") + 
            std::to_string(thepages.size())); 
           
        // retrieve info of the last page in the bank          
        flash::ADDR endofbank01 {bank1.address+bank1.size-1};
        flash::Page page = bank1.page(endofbank01);
        
        // and operate on it: erase, write, read back
        if(flash::erase(page.address, page.size))
        {
            size_t s = std::min(sizeof(datawrite), page.size);
            if(flash::write(page.address, s, datawrite))
            {
                flash::read(page.address, s, dataread);    
            } 
            const char *rr = (0 == std::memcmp(dataread, datawrite, s)) ? "OK" : "KO";
            embot::core::print(
                std::string("FLASH (erase, write, read): on last page of Bank::one = ") +
                rr);
        }
    }
    
    
    // get the handler of the partion containing the bootloader (CAN based boards)
    const flash::Partition& btl { flash::bsp::partition(flash::Partition::ID::bootloader) };  
    if(!btl.isvalid())
    {
        embot::core::print(
            "FLASH (partition): Partition::ID::bootloader is not configured in the BSP");
    }
    else
    {
        btl.pages(thepages);
        embot::core::print(
        "FLASH (partition): Partition::ID::bootloader has size = " + std::to_string(btl.size) +
            " and " + std::to_string(thepages.size()) + " pages");
        
    }
    
    // get the handler of the partion containing the eLoader (ETH based boards)
    const flash::Partition& ldr { flash::bsp::partition(flash::Partition::ID::eloader) };  
    if(!ldr.isvalid())
    {
        embot::core::print(
            "FLASH (partition): Partition::ID::eloader is not configured in the BSP");
    }
    else
    {
        ldr.pages(thepages);
        embot::core::print(
        "FLASH (partition): Partition::ID::eloader has size = " + std::to_string(ldr.size) +
            " and " + std::to_string(thepages.size()) + " pages");
    }  
    
    embot::core::print("");
}

} // namespace test
#endif
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
