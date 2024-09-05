
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#define TEST_EMBOT_OS_OSAL

//#define TEST_EMBOT_HW_I2CE
#define TEST_EMBOT_HW_TLV
#undef TEST_EMBOT_HISTO

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

#if defined(TEST_EMBOT_HW_I2CE)
#include "embot_hw_i2ce.h"
#endif

//#define enableTRACE_all

constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
constexpr embot::os::Event evtBTNreleased = embot::core::binary::mask::pos2mask<embot::os::Event>(1);
constexpr embot::os::Event evtRead = embot::core::binary::mask::pos2mask<embot::os::Event>(2);

constexpr embot::core::relTime tickperiod = 1000*embot::core::time1millisec;

constexpr embot::hw::BTN buttonBLUE = embot::hw::BTN::one;

//embot::tools::Histogram *histoFOCperiod {nullptr};
//embot::tools::PeriodValidator *validFOCperiod {nullptr};

#if defined(TEST_EMBOT_HISTO)    
embot::app::scope::SignalHisto *signalhisto {nullptr};
#endif

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
    
//    constexpr std::uint64_t focperiodlimitLOW {1000*1000-50};          // from 0 usec
//    constexpr std::uint64_t focperiodlimitHIGH {1000*1000+50};        // up to 48 usec
//    constexpr std::uint64_t focperiodSTEP {1};          // in steps of 1 usec
    
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
    
#if defined(TEST_EMBOT_HW_I2CE)
    embot::hw::i2ce::init(embot::hw::I2CE::one, {});
#endif


#if defined(TEST_EMBOT_HISTO)    
    embot::app::scope::SignalHisto::Config shcfg {0, 100*embot::core::time1microsec, embot::core::time1microsec};
    signalhisto = new embot::app::scope::SignalHisto(shcfg);  
#endif        
    
#if defined(TEST_EMBOT_HW_TLV)    
    embot::core::print("mainthread-startup: initted driver for tlv493d");  
    // init the tlv493d
    embot::hw::tlv493d::init(embot::hw::TLV493D::one, {embot::hw::tlv493d::Config::startupMODE::resetCHIP});

#endif
            
    
//    embot::core::print("mainthread-startup: started timer which sends evtTick to evthread every = " + embot::core::TimeFormatter(tickperiod).to_string());

    
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


#if defined(TEST_EMBOT_HISTO)
const embot::tools::Histogram::Values * vv {nullptr};
std::array<uint64_t, 100> vals {};
#endif

#if defined(TEST_EMBOT_HW_I2CE)

void testI2Cdiscover()
{

    static volatile uint32_t therearesome = 0;
    static volatile uint32_t unluckyyou = 0;
    std::vector<embot::hw::i2ce::ADR> adrs {};
    volatile bool foundany = embot::hw::i2ce::discover(embot::hw::I2CE::one, adrs);
    volatile size_t ss = adrs.size();
    if(foundany)
    {
        therearesome = ss;
        volatile embot::hw::i2ce::ADR a {0};
        for(size_t i=0; i<ss; i++)
        {
            a = adrs[i];
            a = a;
        }
    }
    else
    {
        unluckyyou++;
    }
  
}

constexpr size_t datasize {10};
uint8_t data2read[datasize] = {0};
void testI2Cread()
{
//#if defined(TEST_I2C_EMULATED_embot)
    static volatile embot::core::Time t {0};
    static volatile embot::core::Time duration {0};
    
    volatile embot::hw::result_t r {embot::hw::resNOK};
    volatile uint8_t nn {0};
    memset(data2read, 0, sizeof(data2read));
    embot::core::Data da {data2read, sizeof(data2read)};
    t = embot::core::now();
    r = embot::hw::i2ce::receive(embot::hw::I2CE::one, 0xbc, da, 5*embot::core::time1millisec);
    duration = embot::core::now() - t;
    if(embot::hw::resNOK == r)
    {
        nn++;
    }
//#else    
//    static volatile uint32_t n {0};
//    volatile bool ok {false};
//    memset(data2read, 0, sizeof(data2read));
//    ok = I2C_read(0xbc, sizeof(data2read), data2read);
//    
//    if(ok)
//    {
//        n++;
//    }
//#endif    
}




// from the tlv registers
struct RegisterMap
{
    static constexpr std::uint8_t readsize = 10;
    static constexpr std::uint8_t writesize = 4;
    std::uint8_t readmemory[readsize];
    std::uint8_t writememory[writesize];
    bool isvalid() const 
    {
        if((0b00000000 != (readmemory[3] & 0b00000011))) { return false; }  // CH must  be 00 else a conversion is ongoing
        if((0b00110000 != (readmemory[5] & 0b01110000))) { return false; }     // T+FF+PD (pos 4, 5, 6) must be 011 else ...         
        return true;
    }
    constexpr std::uint16_t getXU() const { return static_cast<std::uint16_t>( (static_cast<std::uint16_t>(readmemory[0]) << 4) | static_cast<std::uint16_t>(readmemory[4]>>4) ); }  
    constexpr std::uint16_t getYU() const { return static_cast<std::int16_t>( (static_cast<std::uint16_t>(readmemory[1]) << 4) | static_cast<std::uint16_t>(readmemory[4]&0x0F) ); }             
    constexpr std::uint16_t getZU() const { return static_cast<std::uint16_t>( (static_cast<std::uint16_t>(readmemory[2]) << 4) | static_cast<std::uint16_t>(readmemory[5]&0x0F) ); }
    constexpr std::uint16_t getTU() const { return static_cast<std::uint16_t>( (static_cast<std::uint16_t>(readmemory[3]&0xF0) << 4) | static_cast<std::uint16_t>(readmemory[6]) ); }
    static constexpr std::int16_t toi16(std::uint16_t u) { return ( (0==(u&0x0800)) ? u : ((0x07ff&u)-2048) ); } 
    constexpr std::int16_t getX() const { return toi16(getXU()); }
    constexpr std::int16_t getY() const { return toi16(getYU()); }
    constexpr std::int16_t getZ() const { return toi16(getZU()); }
    
    constexpr std::int16_t getT() const { return toi16(getTU())-340; }
    
    constexpr std::uint8_t getInitialWRITE0() const { return 0; }
    constexpr std::uint8_t getInitialWRITE1() const { return readmemory[7] & 0x18; }
    constexpr std::uint8_t getInitialWRITE2() const { return readmemory[8]; }
    constexpr std::uint8_t getInitialWRITE3() const { return readmemory[9] & 0x1f; }
// ok     void setWRITE(uint8_t paritybit = 0b1, uint8_t i2caddr = 0b00, uint8_t intena = 0b0, uint8_t fastena = 0b1, uint8_t lowena = 0b0, 
//                      uint8_t tempena = 0b0, uint8_t lpperiod = 0b0, uint8_t parena = 0b0)
    struct config_t
    {
        enum class adr : uint8_t { zero = 0, one = 1, two = 2, three = 3 }; 
        uint32_t    paritybit   : 1;
        uint32_t    i2caddr     : 2;
        uint32_t    intena      : 1; 
        uint32_t    fastena     : 1;
        uint32_t    lowena      : 1;
        uint32_t    tempena     : 1;
        uint32_t    lpperiod    : 1;
        uint32_t    parena      : 1;
        constexpr config_t(uint8_t _paritybit, uint8_t _i2caddr, uint8_t _intena, uint8_t _fastena, uint8_t _lowena, 
            uint8_t _tempena, uint8_t _lpperiod, uint8_t _parena) :
            paritybit(_paritybit), i2caddr(_i2caddr), intena(_intena), fastena(_fastena), lowena(_lowena), tempena(_tempena), lpperiod(_lpperiod), parena(_parena)
        {
        }
        constexpr config_t() :
            paritybit(0b1), i2caddr(0b00), intena(0b0), fastena(0b1), lowena(0b0), tempena(0b0), lpperiod(0b0), parena(0b0)
        {
        }
        constexpr config_t(adr a) :
            paritybit(0b1), i2caddr(embot::core::tointegral(a)), intena(0b0), fastena(0b1), lowena(0b0), tempena(0b0), lpperiod(0b0), parena(0b0)
        {
        }
    };
   

    void setWRITE(const config_t &cfg)
    {
        writememory[0] = getInitialWRITE0();
        writememory[1] = getInitialWRITE1() | ((cfg.paritybit&0b1) << 7) | ((cfg.i2caddr&0b11) << 5) | ((cfg.intena&0b1) << 2) | ((cfg.fastena&0b1) << 1) | ((cfg.lowena&0b1));
        writememory[2] = getInitialWRITE2();
        writememory[3] = getInitialWRITE3() | ((cfg.tempena&0b1) << 7) | ((cfg.lpperiod&0b1) << 6) | ((cfg.parena&0b1) << 5);
    }          

    std::int8_t getX08() const { return ( (readmemory[0] & 0x7f) - (128*(readmemory[0]>>7)) ); }  
    std::int8_t getY08() const { return ( (readmemory[1] & 0x7f) - (128*(readmemory[1]>>7)) ); }
    
};

static constexpr RegisterMap::config_t defconfig { };


RegisterMap regmap {};
    
void testI2Creadwriteread()
{
    // at first we read 10 bytes
    
    static volatile embot::core::Time t {0};
    static volatile embot::core::Time duration {0};
    
    volatile embot::hw::result_t r {embot::hw::resNOK};
    volatile uint8_t nn {0};
    memset(regmap.readmemory, 0, sizeof(regmap.readmemory));
    embot::core::Data da {regmap.readmemory, sizeof(regmap.readmemory)};
    t = embot::core::now();
    r = embot::hw::i2ce::receive(embot::hw::I2CE::one, 0xbc, da, 5*embot::core::time1millisec);
    duration = embot::core::now() - t;
    if(embot::hw::resNOK == r)
    {
        nn++;
    }
    
    // then we prepare what to write

    regmap.setWRITE({});
    
    // then we transmit the four bytes

    embot::core::Data da1 {regmap.writememory, sizeof(regmap.writememory)};
    t = embot::core::now();
    r = embot::hw::i2ce::transmit(embot::hw::I2CE::one, 0xbc, da1, 5*embot::core::time1millisec);
    duration = embot::core::now() - t;
    if(embot::hw::resNOK == r)
    {
        nn++;
    }
    
    // now we read back all data and we compare
            
    testI2Cread();
    
}

#endif

void eventbasedthread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{   
    if(0 == eventmask)
    {   // timeout ...         
        return;
    }

#if defined(TEST_EMBOT_HW_I2CE)    
//    testI2Cdiscover();
//    testI2Cread();
    testI2Creadwriteread();

#endif

    
#if defined(TEST_EMBOT_HISTO)  
 
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
    
#endif    

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

#if defined(TEST_EMBOT_HW_TLV)
        embot::core::print("mainthread-onevent: called reading of chip TLV493D");    
        embot::core::Callback cbk00(alertdataisready00, t);
        embot::hw::tlv493d::acquisition(embot::hw::TLV493D::one, cbk00);        
#endif
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evtRead)) 
    {
#if defined(enableTRACE_all)        
        embot::core::TimeFormatter tf(embot::core::now());        
        embot::core::print("evthread-onevent: evtRead received @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full));    
#endif  

#if defined(TEST_EMBOT_HW_TLV)
        if(embot::hw::resOK != embot::hw::tlv493d::read(embot::hw::TLV493D::one, position))
        {
            position = 66666;
        }
               
        embot::core::print("pos = " + std::to_string(0.01 * position) + "deg");
#endif        
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

void thrEVT(void* p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }

void thrPER(void* p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }

embot::os::EventThread* thr {nullptr};

void initSystem(embot::os::Thread *t, void* initparam)
{
//    embot::core::print("this is a demo which shows that this code can run on a dev board and later on the real pmc board");    
    
//    embot::core::print("starting the INIT thread");
    
 //   embot::core::print("creating the system services: timer manager + callback manager");
    
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
    
//    embot::core::print("creating the LED pulser: it will blink a LED at 1 Hz.");
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
    
 //   embot::core::print("creating the main thread");
        
    // create the main thread 
    thr = new embot::os::EventThread;          
    // and start it
    thr->start(configEV, thrEVT); 
    
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
 //           embot::core::print("periodic thread: exec @ time = " + tf.to_string(embot::core::TimeFormatter::Mode::full)); 
            
        }
    };

    embot::os::PeriodicThread *pth = new embot::os::PeriodicThread;
    pth->start(pc, thrPER);    

#endif

//    embot::core::print("quitting the INIT thread. Normal scheduling starts");    
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



