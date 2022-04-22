
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


#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"

#include <vector>


constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);

constexpr embot::core::relTime tickperiod = 1000*embot::core::time1millisec;


#if defined(TEST_EMBOT_HW)

//#define TEST_EMBOT_HW_SPI123

#define TEST_EMBOT_HW_EEPROM
//#define TEST_EMBOT_HW_CHIP_M95512DF

//#define TEST_EMBOT_HW_CHIP_AS5045

#define TEST_EMBOT_HW_CAN
#define TEST_EMBOT_HW_CAN_loopback_can1_to_can2_to_can1
#define TEST_EMBOT_HW_CAN_loopback_can1_to_can2_to_can1_BURST
//#define TEST_EMBOT_HW_CAN_gateway_CAN2toCAN1
//#define TEST_EMBOT_HW_CAN_gateway_CAN1toCAN2

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
//        embot::core::print("mainthread-onevent: evtTick received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));   
    
        test_embot_hw_tick();
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


void can1_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }
    
    embot::core::TimeFormatter tf(embot::core::now());

    if(true == embot::core::binary::mask::check(eventmask, evtCAN1tx)) 
    { 
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
    canconfig.onrxframe = embot::core::Callback(alerteventbasedthread, t); 
    embot::hw::can::init(embot::hw::CAN::two, canconfig);
    embot::hw::can::setfilters(embot::hw::CAN::two, 2);   
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

#if defined(TEST_EMBOT_HW_SPI123)
#include "embot_hw_spi.h"
#endif

#if defined(TEST_EMBOT_HW_EEPROM)
#include "embot_hw_eeprom.h"

constexpr embot::hw::EEPROM eeprom2test {embot::hw::EEPROM::one};

#endif

#if defined(TEST_EMBOT_HW_CHIP_M95512DF)
    #include "embot_hw_chip_M95512DF.h"
#endif
#if defined(TEST_EMBOT_HW_CHIP_AS5045)
    #include "embot_hw_chip_AS5045.h"
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

void test_embot_hw_init()
{
    
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

#if defined(TEST_EMBOT_HW_CHIP_AS5045)

    embot::hw::chip::testof_AS5045();    

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

constexpr size_t capacity {2048};
uint8_t dd[capacity] = {0};
//constexpr size_t adr2use {128 - 8};
constexpr size_t adr2use {0};

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



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
