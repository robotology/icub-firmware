
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


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

#include "embot_hw_bsp_amcfoc_2cm4.h"

embot::app::scope::Signal *signal {nullptr};

constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);


constexpr embot::core::relTime tickperiod = 1000*embot::core::time1millisec;


#define TEST_EEPROM
void test_eeprom_init();
void test_eeprom_tick();

#undef TEST_CAN
void test_can_init(embot::os::Thread *t, void *param);
void test_can_tick(embot::os::Thread *t, embot::os::EventMask eventmask, void *param);

constexpr embot::os::Event evtCAN1tx = embot::core::binary::mask::pos2mask<embot::os::Event>(2);
constexpr embot::os::Event evtCAN1rx = embot::core::binary::mask::pos2mask<embot::os::Event>(3);


#undef TEST_ETH
void test_eth_init();
void test_eth_tick();


#define TEST_EMBOT_HW_TIMER
#define TEST_EMBOT_HW_TIMER_ONESHOT
void test_timer();



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
    
	
#if defined(TEST_EEPROM)    
    test_eeprom_init();
#endif

	
#if defined(TEST_CAN)    
    test_can_init(t, param);
#endif

    

#if defined(TEST_ETH)    
    test_eth_init();
#endif
    
    
#if defined(TEST_EMBOT_HW_TIMER) ||  defined(TEST_EMBOT_HW_TIMER_ONESHOT)  
    test_timer();
#endif    
    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);
    
}

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

void testduration()
{
    //embot::hw::sys::delay(50);
    compute2();
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
        
//        uid = embot::hw::sys::uniqueid();

//        signal->on();   
//        testduration();   
//        signal->off();   
		
#if defined(TEST_ETH)    
		test_eth_tick();
#endif
		
#if defined(TEST_EEPROM)    
		test_eeprom_tick();
#endif		
        
//        embot::core::TimeFormatter tf(embot::core::now());        
//        embot::core::print("mainthread-onevent: evtTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));   
    
#if defined(TEST_CAN)    
		test_can_tick(t, eventmask, param);
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
//    theleds.get(embot::hw::LED::one).on(); 
//    theleds.get(embot::hw::LED::two).on(); 
//    theleds.get(embot::hw::LED::three).on(); 
//    theleds.get(embot::hw::LED::one).off(); 
//    theleds.get(embot::hw::LED::two).off(); 
//    theleds.get(embot::hw::LED::three).off();   
    
#if defined(STM32HAL_dualcore_BOOT_cm4master)
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second);
#else
//    embot::app::LEDwaveT<64> ledwave(150*embot::core::time1millisec, 30, std::bitset<64>(0b01010101));
//    theleds.get(embot::hw::LED::one).wave(&ledwave);
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second);
#endif
   

    
    
    embot::core::print("INIT: creating the main thread. it will reveives one periodic tick event");  
    
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
        embot::hw::FLASHpartitionID::eapplication01
    };
    
#if defined(STM32HAL_dualcore_BOOT_cm4master)    
    embot::hw::dualcore::config({embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::activate});
//    embot::hw::dualcore::config({embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::donothing});
#endif    
    
    // embot::os::init() internally calls embot::hw::bsp::init() which also calls embot::core::init()
    embot::os::init(osconfig);
    
    // now i start the os    
    embot::os::start();

    // just because i am paranoid (thescheduler.start() never returns)
    for(;;);    
}



// - other code

volatile uint8_t stophere = 0;
constexpr embot::hw::EEPROM eeprom2test {embot::hw::EEPROM::one};

#include "embot_hw_eeprom.h"

void test_eeprom_init()
{
    embot::hw::eeprom::init(eeprom2test, {});
    //embot::hw::eeprom::erase(eeprom2test, 0, 8*1024, 100000);
}

constexpr size_t capacity {2048};
uint8_t dd[capacity] = {0};
//constexpr size_t adr2use {128 - 8};
constexpr size_t adr2use {0};

void test_eeprom_tick()
{
    
//    return;
    
    static size_t cnt = 0;
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
        
    std::memset(dd, 0, sizeof(dd));
    embot::core::Data data {dd, numberofbytes};
    
    embot::hw::eeprom::read(eeprom2test, adr2use, data, 5*embot::core::time1millisec);
	stophere++;
    
    
    std::memset(dd, cnt, sizeof(dd));
	stophere++;
    embot::hw::eeprom::write(eeprom2test, adr2use, data, 5*embot::core::time1millisec);    
    

    std::memset(dd, 0, sizeof(dd));
    embot::hw::eeprom::read(eeprom2test, adr2use, data, 5*embot::core::time1millisec);
    
    stophere++;    
    
}


#include "embot_hw_can.h"

void alerteventbasedthread(void *arg)
{
    embot::os::EventThread* evthr = reinterpret_cast<embot::os::EventThread*>(arg);
    if(nullptr != evthr)
    {
        evthr->setEvent(evtCAN1rx);
    }
}

void test_can_init(embot::os::Thread *t, void *param)
{
    volatile uint32_t c = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    c = c;
    
    // start can1 driver
    
    constexpr embot::core::relTime canTXperiod {embot::core::time1second};
    
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

//#define TEST_EMBOT_HW_CAN_BURST
void test_can_tick(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
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
        constexpr buprrstmode bm {burstmode::loop_put_transmit_eachtime};
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
        
//#if defined(TEST_EMBOT_HW_CAN_loopback_can1_to_can2_to_can1)  
//        embot::core::print(" ");
//        embot::core::print("-------------------------------------------------------------------------");
//        embot::core::print("tCAN1 -> START OF transmissions from CAN1 to CAN2 and back to CAN1");
//        embot::core::print("-------------------------------------------------------------------------");        
//        embot::core::print("tCAN1: evtCAN1tx received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));       
//        uint8_t payload[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
//        static uint8_t cnt {0};
//        cnt++;
//        for(auto &i : payload) i+=cnt;
//        
//        constexpr uint8_t burstsize = 2;

//        if(burstsize < 2)
//        {    
//            embot::core::print("tCAN1: will now transmit on CAN1 1 frame w/ data[0] = " + std::to_string(payload[0])); 
//            embot::hw::can::put(embot::hw::CAN::one, {2, 8, payload}); 
//        }
//        else
//        {
//            embot::core::print("tCAN1: will now transmit on CAN1 a burst of " + std::to_string(burstsize) + " frames w/ data[0] = " + std::to_string(payload[0]) + " and decreasing sizes"); 
//            embot::hw::can::Frame hwtxframe {2, 8, payload};
//            for(uint8_t n=0; n<burstsize; n++)
//            {
//                hwtxframe.size = 8-n;
//                embot::core::print("tCAN1: tx frame w/ size = " + std::to_string(hwtxframe.size));
//                embot::hw::can::put(embot::hw::CAN::one, hwtxframe);  
//            }   
//        }            
//            
//        embot::hw::can::transmit(embot::hw::CAN::one);
//        embot::core::print(" ");
//#endif        
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

//#if defined(TEST_EMBOT_HW_CAN_gateway_CAN1toCAN2)
//            embot::core::print("fwd to can2");
//            embot::hw::can::put(embot::hw::CAN::two, hwframe); 
//            embot::hw::can::transmit(embot::hw::CAN::two);  
//#endif              
                                  
            if(remainingINrx > 0)
            {
                t->setEvent(evtCAN1rx);                 
            }
        }         
    
    }    
    
}


#include "embot_hw_eth.h"


void test_eth_init()
{
#if defined(TEST_ETH)       
    ipal_hal_eth_cfg_t c {};
    embot::hw::eth::init(&c);
#endif        
}

void test_eth_tick()
{
#if defined(TEST_ETH)       
    static bool prevlink1isup = false;    
    static bool prevlink2isup = false;
    
    bool link1isup = false;    
    bool link2isup = false;
    
    
    link1isup = embot::hw::eth::islinkup(embot::hw::PHY::one);
    link2isup = embot::hw::eth::islinkup(embot::hw::PHY::two);  


    if((prevlink1isup != link1isup) || ((prevlink2isup != link2isup)))
    {            
        std::string msg = std::string("ETH link 1 (J6) is ") + (link1isup ? "UP" : "DOWN") + " ETH link 2 (J7) is " + (link2isup ? "UP" : "DOWN");
        embot::core::print(msg + " @ " + embot::core::TimeFormatter(embot::core::now()).to_string()); 
    }    
    
    
    prevlink1isup = link1isup;
    prevlink2isup = link2isup;  
#endif    
}










//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//TIMER
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


#include "embot_hw_timer.h"

#if defined(TEST_EMBOT_HW_TIMER_ONESHOT)
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
#endif

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


    
void test_timer()
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
   
}











// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
