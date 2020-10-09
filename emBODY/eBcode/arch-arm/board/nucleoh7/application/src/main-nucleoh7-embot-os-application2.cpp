
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

#include "embot_os_rtos.h"
#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"
#include "embot_tools.h"

#include "embot_hw_bno055.h"
#include "embot_hw_ads122c04.h"
#include "embot_hw_ad7147.h"

#include <array>


// macro definition. we keep some behaviours in the same code.

// this cannot be umdefined
#define enableACQUISITION

// it tells to use the serial port to print data on it
#undef  enableSERIAL  
// if enableSERIAL is defined, it prints in human readable format
#undef  enableSERIAL_string
// if enableSERIAL is defined, it prints in compact binary format
#undef  enableSERIAL_binary
// if defined acquisitionPeriod is = 1 sec else = 10 ms 
#undef  enableACQUISITION_slow
// if defined we print values on the trace port
#undef  enableTRACE
// if defined, the periodic transmission is enabled.
#undef enablePERIODICtransmit
// if enabled, the readings of IMU, ADC and skins are printed on trace port at each transmission
#undef enableTRACE_readings


#define enableSERIAL
#if defined(enableSERIAL)
//#define enableSERIAL_string
#define enableSERIAL_binary
#endif

//#define enableTRACE
#if defined(enableTRACE)
//#define enableTRACE_timeofusart
//#define enableTRACE_histograms
//#define enableTRACE_activations
#define enableTRACE_readings
#endif

//#define enablePERIODICtransmit

constexpr embot::core::relTime osTickPeriod = 500*embot::core::time1microsec;

// used to count times...
struct durationCount
{
    embot::core::Time t {0};
    embot::core::Time d {0};
    durationCount() = default;
    
    void start() 
    {
        t = embot::core::now();
    }
    
    void stop()
    {
        d = embot::core::now() - t;
    }
    
    std::string print()
    {
        return embot::core::TimeFormatter(d).to_string();
    }
    
    embot::core::relTime get() const
    {
        return d;
    }
    
};

durationCount durationIMU {};
durationCount durationADC1prepare {};
durationCount durationADC1 {};
durationCount durationADC2prepare {};
durationCount durationADC2 {};
durationCount durationCDC1 {};
durationCount durationCDC2 {};    
    

static void s_chips_init();
static void s_transmit();
    
static void s_imu_histogram_tick(embot::core::relTime duration);


constexpr embot::os::Event evtDATAtransmit = embot::core::binary::mask::pos2mask<embot::os::Event>(30);

// new events much more separated

constexpr embot::os::Event evtAcquisitionStart = embot::core::binary::mask::pos2mask<embot::os::Event>(0);

constexpr embot::os::Event evtIMUstart = embot::core::binary::mask::pos2mask<embot::os::Event>(8);
constexpr embot::os::Event evtIMUretrieve = embot::core::binary::mask::pos2mask<embot::os::Event>(9);

constexpr embot::os::Event evtADC1commute = embot::core::binary::mask::pos2mask<embot::os::Event>(10);
constexpr embot::os::Event evtADC1ready = embot::core::binary::mask::pos2mask<embot::os::Event>(11);
constexpr embot::os::Event evtADC1start = embot::core::binary::mask::pos2mask<embot::os::Event>(12);
constexpr embot::os::Event evtADC1retrieve = embot::core::binary::mask::pos2mask<embot::os::Event>(13);

constexpr embot::os::Event evtADC2commute = embot::core::binary::mask::pos2mask<embot::os::Event>(14);
constexpr embot::os::Event evtADC2ready = embot::core::binary::mask::pos2mask<embot::os::Event>(15);
constexpr embot::os::Event evtADC2start = embot::core::binary::mask::pos2mask<embot::os::Event>(16);
constexpr embot::os::Event evtADC2retrieve = embot::core::binary::mask::pos2mask<embot::os::Event>(17);


constexpr embot::os::Event evtCDC1start = embot::core::binary::mask::pos2mask<embot::os::Event>(18);
constexpr embot::os::Event evtCDC1retrieve = embot::core::binary::mask::pos2mask<embot::os::Event>(19);

constexpr embot::os::Event evtCDC2start = embot::core::binary::mask::pos2mask<embot::os::Event>(20);
constexpr embot::os::Event evtCDC2retrieve = embot::core::binary::mask::pos2mask<embot::os::Event>(21);

constexpr embot::os::Event evtDATAtrace = embot::core::binary::mask::pos2mask<embot::os::Event>(22);

embot::os::Timer *tmrADCok = nullptr;

 
#if defined(enableACQUISITION_slow) 
constexpr embot::core::relTime acquisitionPeriod = embot::core::time1second;
#else
constexpr embot::core::relTime acquisitionPeriod = 10*embot::core::time1millisec;
#endif


constexpr embot::hw::BTN buttonBLUE = embot::hw::BTN::one;
constexpr embot::hw::BTN buttonPB8 = embot::hw::BTN::two;
    
constexpr embot::hw::BTN buttonTX = buttonPB8; // but later on: buttonPB8 ore buttonBLUE

#if defined(enableTRACE_histograms) 
embot::tools::Histogram *histoIMU {nullptr};
#endif

void txrequest(void *p)
{
    embot::os::Thread *t = reinterpret_cast<embot::os::Thread *>(p);
    t->setEvent(evtDATAtransmit);
}

void tMAIN_startup(embot::os::Thread *t, void *param)
{       
    // init imu + adc
    s_chips_init(); 
    
    // init the ext interrupt button
    embot::hw::button::init(buttonTX, {embot::hw::button::Mode::TriggeredOnRelease, {txrequest, t}, 0});
    
    embot::core::print("evthread-startup: chips + exti initted" );
    
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtAcquisitionStart, t));
    embot::os::Timer::Config cfg{acquisitionPeriod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);
    
    embot::core::print("evthread-startup: started timer which sends evtAcquisitionStart to evthread every us = " + std::to_string(acquisitionPeriod));
      
#if defined(enablePERIODICtransmit)    
    constexpr embot::core::relTime txperiod = 2000*embot::core::time1millisec;
    embot::os::Timer *tmrTX = new embot::os::Timer;   
    embot::os::Action actTX(embot::os::EventToThread(evtDATAtransmit, t));
    embot::os::Timer::Config cfgTX{txperiod, actTX, embot::os::Timer::Mode::forever, 0};
    tmrTX->start(cfgTX);
#endif 

    tmrADCok = new embot::os::Timer;    
}


// the buffers ...
struct BufferedData
{
    embot::hw::bno055::Data imu {};
    std::tuple<int16_t, int16_t, int16_t> acc2transmit {0, 0, 0};
    std::pair<uint32_t, uint32_t> adc2transmit {0, 0};
    std::array<uint16_t, 24> cdc2transmit = {0};
    
    BufferedData() = default; 
};

BufferedData buffer {};

using namespace embot::os;

void tMAIN_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{
    static bool ADC1isSTABLE {false}; 
    static bool ADC2isSTABLE {false}; 
    
    static embot::core::Time startOfCycle {0};
    
    if(0 == eventmask)
    {   // timeout ...          
        return;
    }
    
    volatile embot::os::EventMask internaltriggers {0};

    if(true == embot::core::binary::mask::check(eventmask, evtAcquisitionStart)) 
    {
#if defined(enableTRACE_activations)
        startOfCycle = embot::core::now();       
        embot::core::TimeFormatter tf(startOfCycle);        
        embot::core::print("evthread-onevent: evtAcquisitionStart received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif     
        internaltriggers |= evtADC1start;
        //t->setEvent(evtADC1commute);          
        //t->setEvent(evtIMUstart);
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evtIMUstart)) 
    {
#if defined(enableTRACE_activations)  
        embot::core::Time n = embot::core::now();        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtIMUstart received after " + 
                embot::core::TimeFormatter(n-startOfCycle).to_string() +
                " @ " + embot::core::TimeFormatter(n).to_string());    
#endif
        durationIMU.start();        
        embot::hw::bno055::acquisition( embot::hw::BNO055::one, 
                                        embot::hw::bno055::Set::A, 
                                        buffer.imu,
                                        { [](void* p){ Thread *th = reinterpret_cast<Thread*>(p); th->setEvent(evtIMUretrieve);}, t }
                                      );
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evtIMUretrieve))
    {
#if defined(enableTRACE_activations)        
        embot::core::Time n = embot::core::now();        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtIMUretrieve received after " + 
                embot::core::TimeFormatter(n-startOfCycle).to_string() +
                " @ " + embot::core::TimeFormatter(n).to_string());   
#endif
        durationIMU.stop(); 
        buffer.acc2transmit = {buffer.imu.acc.x, buffer.imu.acc.y, buffer.imu.acc.z};  
        s_imu_histogram_tick(durationIMU.get());
        internaltriggers |= evtCDC1start;
        //t->setEvent(evtCDC1start);               
    }

    if(true == embot::core::binary::mask::check(eventmask, evtCDC1start))
    {
#if defined(enableTRACE_activations)        
        embot::core::Time n = embot::core::now();        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtCDC1start received after " + 
                embot::core::TimeFormatter(n-startOfCycle).to_string() +
                " @ " + embot::core::TimeFormatter(n).to_string());     
#endif

        durationCDC1.start();         
        embot::hw::ad7147::acquisition(embot::hw::AD7147::one, 
                                        { [](void* p){ Thread *th = reinterpret_cast<Thread*>(p); th->setEvent(evtCDC1retrieve);}, t }
                                      );        
    }

    if(true == embot::core::binary::mask::check(eventmask, evtCDC1retrieve))
    {
#if defined(enableTRACE_activations)        
        embot::core::Time n = embot::core::now();        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtCDC1retrieve received after " + 
                embot::core::TimeFormatter(n-startOfCycle).to_string() +
                " @ " + embot::core::TimeFormatter(n).to_string());    
#endif

        embot::hw::ad7147::Values v {};    
        embot::hw::ad7147::read(embot::hw::AD7147::one, v);
        durationCDC1.stop();
            
        size_t offset = 0; //(embot::hw::AD7147::one == cdc) ? 0 : 12;

        for(size_t i=0; i<v.size(); i++)
        {
            buffer.cdc2transmit[i+offset] = v[i];
        }        
        //internaltriggers |= evtCDC2start;
        //t->setEvent(evtCDC2start); 

        internaltriggers |= evtADC2start;        
    }

    if(true == embot::core::binary::mask::check(eventmask, evtCDC2start))
    {
#if defined(enableTRACE_activations)        
        embot::core::Time n = embot::core::now();        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtCDC2start received after " + 
                embot::core::TimeFormatter(n-startOfCycle).to_string() +
                " @ " + embot::core::TimeFormatter(n).to_string());     
#endif 
        durationCDC2.start();        
        embot::hw::ad7147::acquisition(embot::hw::AD7147::two, 
                                        { [](void* p){ Thread *th = reinterpret_cast<Thread*>(p); th->setEvent(evtCDC2retrieve);}, t }
                                      );        
    }

    if(true == embot::core::binary::mask::check(eventmask, evtCDC2retrieve))
    {
#if defined(enableTRACE_activations)        
        embot::core::Time n = embot::core::now();        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtCDC2retrieve received after " + 
                embot::core::TimeFormatter(n-startOfCycle).to_string() +
                " @ " + embot::core::TimeFormatter(n).to_string());   
#endif

        embot::hw::ad7147::Values v {};    
        embot::hw::ad7147::read(embot::hw::AD7147::two, v);
        durationCDC2.stop();
            
        size_t offset = 12; //(embot::hw::AD7147::one == cdc) ? 0 : 12;

        for(size_t i=0; i<v.size(); i++)
        {
            buffer.cdc2transmit[i+offset] = v[i];
        } 
        //internaltriggers |= evtADC1start;        
        //t->setEvent(evtADC1start);        
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evtADC1ready))
    {
#if defined(enableTRACE_activations)        
        embot::core::Time n = embot::core::now();        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtADC1ready received after " + 
                embot::core::TimeFormatter(n-startOfCycle).to_string() +
                " @ " + embot::core::TimeFormatter(n).to_string());     
#endif        
        durationADC1prepare.stop();
        ADC1isSTABLE = true;       
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evtADC2ready))
    {
#if defined(enableTRACE_activations)        
        embot::core::Time n = embot::core::now();        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtADC2ready received after " + 
                embot::core::TimeFormatter(n-startOfCycle).to_string() +
                " @ " + embot::core::TimeFormatter(n).to_string());    
#endif  
        durationADC2prepare.stop();
        ADC2isSTABLE = true; 
        //internaltriggers |= evtADC2start;        
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evtADC1commute))
    {
#if defined(enableTRACE_activations)        
        embot::core::Time n = embot::core::now();        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtADC1commute received after " + 
                embot::core::TimeFormatter(n-startOfCycle).to_string() +
                " @ " + embot::core::TimeFormatter(n).to_string());  
#endif  

        ADC1isSTABLE = false;
        durationADC1prepare.start();
        embot::hw::ads122c04::commute(embot::hw::ADS122C04::one, embot::hw::ads122c04::Channel::one, {});
                
        volatile embot::core::relTime tt = embot::hw::ads122c04::conversiontime(embot::hw::ADS122C04::one);
        // round to osTickPeriod + 1 (so that we are sure that we have passed the time.
        tt = tt / osTickPeriod;
        tt = osTickPeriod*(tt+1);

        // ok, now start a timer which exec a setEvent(evtADC1start) after some millisecs.
        
        //embot::os::Action act(embot::os::EventToThread(evtADC1start, t));
        embot::os::Action act(embot::os::EventToThread(evtADC1ready, t));
        embot::os::Timer::Config cfg{tt, act, embot::os::Timer::Mode::oneshot, 1};
        tmrADCok->stop();
        volatile bool res = tmrADCok->start(cfg);
        res = res;
        internaltriggers |= evtCDC2start;
        
        //t->setEvent(evtIMUstart);
    }    

    if(true == embot::core::binary::mask::check(eventmask, evtADC1start))
    {
#if defined(enableTRACE_activations)        
        embot::core::Time n = embot::core::now();        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtADC1start received after " + 
                embot::core::TimeFormatter(n-startOfCycle).to_string() +
                " @ " + embot::core::TimeFormatter(n).to_string());     
#endif  
        durationADC1.start();
        embot::hw::ads122c04::acquire(embot::hw::ADS122C04::one, embot::hw::ads122c04::Channel::one, 
                                      { [](void* p){ Thread *th = reinterpret_cast<Thread*>(p); th->setEvent(evtADC1retrieve);}, t }
                                      );   
    }      


    if(true == embot::core::binary::mask::check(eventmask, evtADC1retrieve))
    {
#if defined(enableTRACE_activations)        
        embot::core::Time n = embot::core::now();        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtADC1retrieve received after " + 
                embot::core::TimeFormatter(n-startOfCycle).to_string() +
                " @ " + embot::core::TimeFormatter(n).to_string());    
#endif  
        
        embot::hw::ads122c04::Values v {};
        embot::hw::ads122c04::read(embot::hw::ADS122C04::one, v); 
        buffer.adc2transmit = {v.v1, v.v2};
        durationADC1.stop();
        
        internaltriggers |= evtADC2commute;
        //t->setEvent(evtADC2commute); 
    }


    if(true == embot::core::binary::mask::check(eventmask, evtADC2commute))
    {
#if defined(enableTRACE_activations)        
        embot::core::Time n = embot::core::now();        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtADC2commute received after " + 
                embot::core::TimeFormatter(n-startOfCycle).to_string() +
                " @ " + embot::core::TimeFormatter(n).to_string());  
#endif  
        
        ADC2isSTABLE = false;
        durationADC2prepare.start();

        embot::hw::ads122c04::commute(embot::hw::ADS122C04::one, embot::hw::ads122c04::Channel::two, {});
        
        // ok, now start a timer which exec a setEvent(evtADC2ready) after some millisecs.
        volatile embot::core::relTime tt = embot::hw::ads122c04::conversiontime(embot::hw::ADS122C04::one);
        // round to osTickPeriod + 1 (so that we are sure that we have passed the time.
        tt = tt / osTickPeriod;
        tt = osTickPeriod*(tt+1);
        // ok, now start a timer which exec a setEvent(evtADC1start) after some millisecs.
        embot::os::Action act(embot::os::EventToThread(evtADC2ready, t));
        embot::os::Timer::Config cfg{tt, act, embot::os::Timer::Mode::oneshot, 1};
        tmrADCok->stop();
        volatile bool res = tmrADCok->start(cfg);
        
        internaltriggers |= evtIMUstart;
        res = res;
    }    

    if(true == embot::core::binary::mask::check(eventmask, evtADC2start))
    {
#if defined(enableTRACE_activations)        
        embot::core::Time n = embot::core::now();        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtADC2start received after " + 
                embot::core::TimeFormatter(n-startOfCycle).to_string() +
                " @ " + embot::core::TimeFormatter(n).to_string());    
#endif  
        durationADC2.start();
        embot::hw::ads122c04::acquire(embot::hw::ADS122C04::one, embot::hw::ads122c04::Channel::two, 
                                      { [](void* p){ Thread *th = reinterpret_cast<Thread*>(p); th->setEvent(evtADC2retrieve);}, t }
                                      );   
    }      


    if(true == embot::core::binary::mask::check(eventmask, evtADC2retrieve))
    {
#if defined(enableTRACE_activations)        
        embot::core::Time n = embot::core::now();        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtADC2retrieve received after " + 
                embot::core::TimeFormatter(n-startOfCycle).to_string() +
                " @ " + embot::core::TimeFormatter(n).to_string());  
#endif  
        
        embot::hw::ads122c04::Values v {};
        embot::hw::ads122c04::read(embot::hw::ADS122C04::one, v); 
        durationADC2.stop();
        buffer.adc2transmit = {v.v1, v.v2};
        
        internaltriggers |= evtADC1commute;
        
        // ok, nothing more
    }
    
    
    if(true == embot::core::binary::mask::check(eventmask, evtDATAtransmit))
    {
#if defined(enableTRACE_activations)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtDATAtransmit received @ time = " + tf.to_string());    
#endif        
        s_transmit();
    }
    
    
    // i send the internaltriggers to re-execute this function
    if(0 != internaltriggers)
    {
        //embot::os::rtos::thread_sleep(embot::os::rtos::scheduler_thread_running(), 5*embot::core::time1microsec);
        t->setEvent(internaltriggers);
    }

}




void onIdle(embot::os::Thread *t, void* idleparam)
{
    static uint32_t i = 0;
    i++;
}


embot::os::EventThread* thrMAIN {nullptr};

void tMAIN(void* p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }

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
        tMAIN_startup,
        nullptr,
        50*embot::core::time1millisec,
        tMAIN_onevent
    };
        
    // create the main thread 
    thrMAIN = new embot::os::EventThread;          
    // and start it
    thrMAIN->start(configEV, tMAIN);   
}

// usart 921600

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{        
    constexpr embot::os::InitThread::Config initcfg = { 4*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 1024, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = { };
    constexpr embot::os::Config osconfig {osTickPeriod, initcfg, idlecfg, onOSerror};
    
    embot::os::init(osconfig);
      
    embot::os::start(); 
}




#include <cstring>

static void s_chips_init()
{

#if defined(enableTRACE_histograms)     
    histoIMU = new embot::tools::Histogram;
    histoIMU->init({0, 2500, 100});   
#endif
    
#if !defined(enableACQUISITION)    
    // nothing. we just simulate the sensors  
    #error this mode is not enabled anymore
#else
 
    constexpr embot::core::relTime timeout = 5*embot::core::time1millisec;
    constexpr uint32_t i2cspeed400k = 400000;
    constexpr uint32_t i2cspeed1m = 1000000;
    
    constexpr uint32_t i2cspeed = i2cspeed400k;
    
    
    // BNO055
    // init it normally
    
    embot::hw::bno055::Config bno055config { embot::hw::i2c::Descriptor { embot::hw::I2C::one, i2cspeed } };
    embot::hw::bno055::init(embot::hw::BNO055::one, bno055config); 
    
    constexpr embot::hw::bno055::Mode mode = embot::hw::bno055::Mode::ACCGYRO;
    //constexpr embot::hw::bno055::Mode mode = embot::hw::bno055::Mode::NDOF;
    embot::hw::bno055::set(embot::hw::BNO055::one, mode, timeout);   


    // ADS122C04
    // init normally and start acquisition on Channel::one
    embot::hw::ads122c04::Config adsconfig { embot::hw::i2c::Descriptor { embot::hw::I2C::one, i2cspeed } };
    embot::hw::ads122c04::init(embot::hw::ADS122C04::one, adsconfig); 

    embot::hw::ads122c04::commute(embot::hw::ADS122C04::one, embot::hw::ads122c04::Channel::one, {});                
    embot::core::delay(embot::hw::ads122c04::conversiontime(embot::hw::ADS122C04::one));    

    // AD7147
    // init both of them
    embot::hw::ad7147::Config skconfig { embot::hw::i2c::Descriptor { embot::hw::I2C::one, i2cspeed } };
    embot::hw::ad7147::init(embot::hw::AD7147::one, skconfig);
    embot::hw::ad7147::init(embot::hw::AD7147::two, skconfig);      
   
#endif
    
}



static void s_imu_histogram_tick(embot::core::relTime duration)
{

#if defined(enableTRACE_histograms)    
    histoIMU->add(duration);
    static uint32_t cnt = 0;
    cnt++;
    if((cnt%1000) == 0)
    {
//        embot::core::print(std::string("imu read in ") + embot::core::TimeFormatter(imu_acquisitiontime).to_string());
        char str[256] = {0};
        const embot::tools::Histogram::Values *v = histoIMU->getvalues();
        snprintf(str, sizeof(str), "(%d) %f [%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f] %f", 
                                    v->inside.size(),
                                    100.0f*v->below/static_cast<float>(v->total),
                                    100.0f*v->inside[0]/static_cast<float>(v->total),
                                    100.0f*v->inside[1]/static_cast<float>(v->total),
                                    100.0f*v->inside[2]/static_cast<float>(v->total),
                                    100.0f*v->inside[3]/static_cast<float>(v->total),
                                    100.0f*v->inside[4]/static_cast<float>(v->total),  
                                    100.0f*v->inside[5]/static_cast<float>(v->total),
                                    100.0f*v->inside[6]/static_cast<float>(v->total), 
                                    100.0f*v->inside[7]/static_cast<float>(v->total),     
                                    100.0f*v->inside[8]/static_cast<float>(v->total), 
                                    100.0f*v->inside[9]/static_cast<float>(v->total), 
                                    100.0f*v->inside[10]/static_cast<float>(v->total), 
                                    100.0f*v->inside[11]/static_cast<float>(v->total),     
                                    100.0f*v->inside[12]/static_cast<float>(v->total), 
                                    100.0f*v->inside[13]/static_cast<float>(v->total),   
                                    100.0f*v->inside[14]/static_cast<float>(v->total), 
                                    100.0f*v->inside[15]/static_cast<float>(v->total), 
                                    100.0f*v->inside[16]/static_cast<float>(v->total), 
                                    100.0f*v->inside[17]/static_cast<float>(v->total),     
                                    100.0f*v->inside[18]/static_cast<float>(v->total), 
                                    100.0f*v->inside[19]/static_cast<float>(v->total),   
                                    100.0f*v->inside[20]/static_cast<float>(v->total), 
                                    100.0f*v->inside[21]/static_cast<float>(v->total),     
                                    100.0f*v->inside[22]/static_cast<float>(v->total), 
                                    100.0f*v->inside[23]/static_cast<float>(v->total), 
                                    100.0f*v->inside[24]/static_cast<float>(v->total),                                        
                                    100.0f*v->beyond/static_cast<float>(v->total)
                                    );
        embot::core::print(std::string("histo IMU -> ") + str);
                                    
        histoIMU->reset();
    }  
#endif        
}



static void s_transmit()
{ 
    char text[128] = {0};
    
#if defined(enableTRACE_readings)
    
    embot::core::print(" ");  
    
    std::string imutime = durationIMU.print();
    std::tuple<float, float, float> a {0.01 * std::get<0>(buffer.acc2transmit), 0.01 * std::get<1>(buffer.acc2transmit), 0.01 * std::get<2>(buffer.acc2transmit)};    
        
    embot::core::print( "BNO055: acc = (" + std::to_string(std::get<0>(a)) + ", " + std::to_string(std::get<1>(a)) + ", " + std::to_string(std::get<2>(a)) + ") m/(s*s)" +
                        "in " + imutime
        );  
     
    std::string adc1time = durationADC1prepare.print() + "+" + durationADC1.print();
    std::string adc2time = durationADC2prepare.print() + "+" + durationADC2.print();
            
    embot::core::print( "ADS122C04: (" + std::to_string(buffer.adc2transmit.first) + ", " + std::to_string(buffer.adc2transmit.second) + ")" + 
                        " in (" + adc1time + ", " + adc2time + ")"
    );     
        

    std::string skin1time = durationCDC1.print();
    std::string skin2time = durationCDC2.print();        
        
    std::string taxels02to13 {};
    std::string taxels14to24 {};
    for(size_t i=0; i<12; i++)
    {
        taxels02to13 += std::to_string(buffer.cdc2transmit[i]); 
        taxels02to13 += " ";
        taxels14to24 += std::to_string(buffer.cdc2transmit[i+12]);
        taxels14to24 += " ";
    }
    embot::core::print( std::string("P02->P13 = (") + 
                        taxels02to13 +         
                        ")" +
                        "in " + skin1time
    );        
    
    embot::core::print( std::string("P14->P24 = (") + 
                        taxels14to24 +         
                        ")" +
                        "in " + skin2time
    );  
    

#endif    
    
#if defined(enableSERIAL) && defined(enableSERIAL_string)
  
    #warning CAVEAT: the mode enableSERIAL_string is incomplete and does not prints everything
    // this prints in hex the entire range of values
    snprintf(text, sizeof(text), "%04x %04x %04x %04x %04x\n", std::get<0>(buffer.acc2transmit), std::get<1>(buffer.acc2transmit), std::get<2>(buffer.acc2transmit), buffer.adc2transmit.first, buffer.adc2transmit.second);        
    HAL_UART_Transmit(&huart3, reinterpret_cast<uint8_t*>(text), std::strlen(text), 0xFFFF);    
    
#elif defined(enableSERIAL) && defined(enableSERIAL_binary)   

    //snprintf(text, sizeof(text), "%d\n", static_cast<uint8_t>(std::get<0>(buffer.acc2transmit)));   
    // writing into text[] in big endian order all values with the same integer 32 bit type.
    // i use int32_t. 
    // the acceleration values, which are in int16_t, are first transformed into int32_t
    // to keep their sign and then copied in big endian order.
    // the adc values, which have only 24 signisficant bits are just copied in big endian order as they are.

    volatile int32_t tmp = 0;
    volatile uint8_t pos = 0;
    // acc.x
    tmp = std::get<0>(buffer.acc2transmit);
    text[pos++] =  (tmp & 0xff000000) >> 24;
    text[pos++] =  (tmp & 0x00ff0000) >> 16;
    text[pos++] =  (tmp & 0x0000ff00) >> 8;
    text[pos++] =  (tmp & 0x000000ff);
    // acc.y
    tmp = std::get<1>(buffer.acc2transmit);
    text[pos++] =  (tmp & 0xff000000) >> 24;
    text[pos++] =  (tmp & 0x00ff0000) >> 16;
    text[pos++] =  (tmp & 0x0000ff00) >> 8;
    text[pos++] =  (tmp & 0x000000ff);
    // acc.z
    tmp = std::get<2>(buffer.acc2transmit);
    text[pos++] =  (tmp & 0xff000000) >> 24;
    text[pos++] =  (tmp & 0x00ff0000) >> 16;
    text[pos++] =  (tmp & 0x0000ff00) >> 8;
    text[pos++] =  (tmp & 0x000000ff);
    // adc.first
    tmp = buffer.adc2transmit.first;
    text[pos++] =  (tmp & 0xff000000) >> 24;
    text[pos++] =  (tmp & 0x00ff0000) >> 16;
    text[pos++] =  (tmp & 0x0000ff00) >> 8;
    text[pos++] =  (tmp & 0x000000ff);
    // adc.second
    tmp = buffer.adc2transmit.second;
    text[pos++] =  (tmp & 0xff000000) >> 24;
    text[pos++] =  (tmp & 0x00ff0000) >> 16;
    text[pos++] =  (tmp & 0x0000ff00) >> 8;
    text[pos++] =  (tmp & 0x000000ff); 
    
    // so far we have used 20 bytes.
    
    // cdc -> 24 values, each with 2 bytes .... but we use four -> total = 96 bytes
    
    for(int i=0; i<buffer.cdc2transmit.size(); i++)
    {
        tmp = buffer.cdc2transmit[i];
        text[pos++] =  (tmp & 0xff000000) >> 24;
        text[pos++] =  (tmp & 0x00ff0000) >> 16;
        text[pos++] =  (tmp & 0x0000ff00) >> 8;
        text[pos++] =  (tmp & 0x000000ff); 
    }
    // total bytes = 20 + 96 = 116
    // it takes about S0:m1:u327 to transmit the 116 bytes 

    // at the end we add a new line.
    // the new line in binary value is 0x0A ... do we really need to transmit it? 
    // if the receiver expects just a fixed number of bytes then the terminator character '\n' is useless
    // else if it looks for the terminator '\n' to stop the rx, ... there is the possibility that it stops if data contains the byte 0x0A.

// following code detects the presence of a '\n' inside data.
//    volatile bool found = false;
//    volatile int index = -1;
//    for(volatile int i=0; i<20; i++)
//    {
//        if(text[i] == '\n')
//        {
//            found = true;
//            index = i;
//        }
//    }
//    found = found;
//    index = index;
    
//   	text[pos++] = '\n'; 

    embot::core::Time t0 = embot::core::now();
    HAL_UART_Transmit(&huart3, reinterpret_cast<uint8_t*>(text), pos, 0xFFFF);
    
#if defined(enableTRACE_timeofusart)     
    embot::core::print(std::string("USART TX time: ") + embot::core::TimeFormatter(embot::core::now()-t0).to_string());
#endif

#endif
 
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



