
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_hw_dualcore.h"
#include "stm32hal.h"
   


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


embot::app::scope::Signal *signal {nullptr};
constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
constexpr embot::core::relTime tickperiod = 1000*embot::core::time1millisec;

//#define ENABLE_SPEED_TESTS

#if defined(ENABLE_SPEED_TESTS) 

    enum class SpeedTest { none, delay, one, two }; 
    constexpr SpeedTest speedtest {SpeedTest::delay};
    constexpr embot::core::Time SpeedTestDelay { 40*embot::core::time1microsec };

    void test_SPEED_init(embot::os::Thread *t, void *param);
    void test_SPEED_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param);

#endif

// comment to disable the hw tests
#define ENABLE_HW_TESTS

#if !defined(ENABLE_HW_TESTS)

#else

// place in here what we enable
    
#define ENABLE_HW_TEST_can
//#define ENABLE_HW_TEST_eeprom


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

	}
    
    #if defined(ENABLE_HW_TESTS)   
    test_HW_onevent(t, eventmask, param);        
    #endif  
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
        constexpr embot::hw::dualcore::Config dualcoreconfig {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::activate};
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


#if defined(ENABLE_HW_TEST_can)

void test_can_init(embot::os::Thread *t, void *param);
void test_can_tick(embot::os::Thread *t, embot::os::EventMask eventmask, void *param);

#endif

#if defined(ENABLE_HW_TEST_eeprom)

void test_eeprom_init(embot::os::Thread *t, void *param);
void test_eeprom_tick(embot::os::Thread *t, embot::os::EventMask eventmask, void *param);

#endif

void test_HW_init(embot::os::Thread *t, void *param)
{
    
#if defined(ENABLE_HW_TEST_can)
    test_can_init(t, param);
#endif
    
#if defined(ENABLE_HW_TEST_eeprom)
    test_eeprom_init(t, param);
#endif    
    
}

void test_HW_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{
    
#if defined(ENABLE_HW_TEST_can)
    test_can_tick(t, eventmask, param);
#endif
    
#if defined(ENABLE_HW_TEST_eeprom)
    test_eeprom_tick(t, eventmask, param);
#endif    
    
}





#if defined(ENABLE_HW_TEST_can)

#include "embot_hw_can.h"


constexpr embot::os::Event evtCAN1tx = embot::core::binary::mask::pos2mask<embot::os::Event>(2);
constexpr embot::os::Event evtCAN1rx = embot::core::binary::mask::pos2mask<embot::os::Event>(3);

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
    embot::hw::can::setfilters(embot::hw::CAN::one, 1);   
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
//#define TEST_EMBOT_HW_CAN_ONE

#define TEST_EMBOT_HW_CAN_REPLY2RX

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

#endif // #if defined(TEST_EMBOT_HW_CAN_BURST)
        
#if defined(TEST_EMBOT_HW_CAN_ONE)


#endif  // #if defined(TEST_EMBOT_HW_CAN_ONE)
             
    } // if(true == embot::core::binary::mask::check(eventmask, evtCAN1tx))
    
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

#if defined(TEST_EMBOT_HW_CAN_REPLY2RX)
            embot::core::print("reply");
            hwframe.data[0]++;
            embot::hw::can::put(embot::hw::CAN::one, hwframe); 
            embot::hw::can::transmit(embot::hw::CAN::one);  
#endif              
                                  
            if(remainingINrx > 0)
            {
                t->setEvent(evtCAN1rx);                 
            }
        }         
    
    } // if(true == embot::core::binary::mask::check(eventmask, evtCAN1rx))   
    
}


#endif




#if defined(ENABLE_HW_TEST_eeprom)

#include "embot_hw_eeprom.h"

volatile uint8_t stophere = 0;
constexpr embot::hw::EEPROM eeprom2test {embot::hw::EEPROM::one};


//constexpr embot::os::Event evtEEPROMdo = embot::core::binary::mask::pos2mask<embot::os::Event>(4);

void test_eeprom_init(embot::os::Thread *t, void *param)
{
    volatile uint32_t c = embot::hw::sys::clock(embot::hw::CLOCK::syscore);
    c = c;

    embot::hw::eeprom::init(eeprom2test, {});
    embot::hw::eeprom::erase(eeprom2test, 0, 8*1024, 100000);    
}

constexpr size_t capacity {2048};
uint8_t dd[capacity] = {0};
//constexpr size_t adr2use {128 - 8};
constexpr size_t adr2use {0};

void test_eeprom_tick(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }
    
    embot::core::TimeFormatter tf(embot::core::now());
    
    
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
    
    embot::hw::eeprom::read(eeprom2test, adr2use, data, 3*embot::core::time1millisec);
    
    
    std::memset(dd, cnt, sizeof(dd));
    embot::hw::eeprom::write(eeprom2test, adr2use, data, 3*embot::core::time1millisec);    
    
    
    std::memset(dd, 0, sizeof(dd));
    embot::hw::eeprom::read(eeprom2test, adr2use, data, 3*embot::core::time1millisec);
     
    stophere++;      

//    if(true == embot::core::binary::mask::check(eventmask, evtEEPROMdo)) 
//    { 
//             
//    } // if(true == embot::core::binary::mask::check(eventmask, evtEEPROMdo))
    
    
}


#endif

#endif // #if defined(ENABLE_HW_TESTS)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
