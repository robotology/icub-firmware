
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_app_skeleton_os_evthreadcan.h"

// --------------------------------------------------------------------------------------------------------------------
// config start

constexpr embot::app::theCANboardInfo::applicationInfo applInfo 
{ 
    embot::prot::can::versionOfAPPLICATION {2, 0, 10},
    embot::prot::can::versionOfCANPROTOCOL {2, 0}    
};

constexpr std::uint16_t threadIDLEstacksize = 1*512;
constexpr std::uint16_t threadINITstacksize = 2048;
constexpr std::uint16_t threadEVNTstacksize = 4096;
constexpr std::uint8_t maxINPcanframes = 16;
constexpr std::uint8_t maxOUTcanframes = 48;
constexpr embot::core::relTime threadEVNTtimeout = 50*embot::core::time1millisec;

static void *paramINIT = nullptr;
static void *paramIDLE = nullptr;
static void *paramERR = nullptr;
static void *paramEVNT = nullptr;

constexpr embot::os::theTimerManager::Config tmcfg {};
constexpr embot::os::theCallbackManager::Config cmcfg {};
    
    
static const embot::app::skeleton::os::basic::sysConfig syscfg { threadINITstacksize, paramINIT, threadIDLEstacksize, paramIDLE, paramERR, tmcfg, cmcfg};

static const embot::app::skeleton::os::evthreadcan::evtConfig evtcfg { threadEVNTstacksize, paramEVNT, threadEVNTtimeout};

static const embot::app::skeleton::os::evthreadcan::canConfig cancfg { maxINPcanframes, maxOUTcanframes };

// config end
// --------------------------------------------------------------------------------------------------------------------


class mySYS final : public embot::app::skeleton::os::evthreadcan::SYSTEMevtcan
{
public:
    mySYS(const embot::app::skeleton::os::basic::sysConfig &cfg) 
        : SYSTEMevtcan(cfg) {}
        
    void userdefOnIdle(embot::os::Thread *t, void* idleparam) const override;
    void userdefonOSerror(void *errparam) const override;
    void userdefInit_Extra(embot::os::EventThread* evthr, void *initparam) const override;
};


class myEVT final : public embot::app::skeleton::os::evthreadcan::evThreadCAN
{
public:
    myEVT(const embot::app::skeleton::os::evthreadcan::evtConfig& ecfg, const embot::app::skeleton::os::evthreadcan::canConfig& ccfg, const embot::app::theCANboardInfo::applicationInfo& a) 
        : evThreadCAN(ecfg, ccfg, a) {}
        
    void userdefStartup(embot::os::Thread *t, void *param) const override;
    void userdefOnTimeout(embot::os::Thread *t, embot::os::EventMask eventmask, void *param) const override;
    void userdefOnEventRXcanframe(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &outframes) const override;
    void userdefOnEventANYother(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, std::vector<embot::prot::can::Frame> &outframes) const override;                   
};


static const mySYS mysys { syscfg };
static const myEVT myevt { evtcfg, cancfg, applInfo };
constexpr embot::app::skeleton::os::evthreadcan::CFG cfg{ &mysys, &myevt };

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    embot::app::skeleton::os::evthreadcan::run(cfg);
    for(;;);    
}


// - here is the tailoring of the board.


#undef DEBUG_atstartup_tx_FTdata
#undef ENABLE_IHAVEJUSTSTARTED

#include "embot_hw_led.h"
#include "embot_hw_si7051.h"
#include "embot_hw_bno055.h"
#include "embot_hw_pga308.h"

namespace embot { namespace hw { namespace bsp { namespace strain2 {
    const embot::hw::LED ledBLUE = embot::hw::LED::one;      
    const embot::hw::SI7051 thermometerSGAUGES = embot::hw::SI7051::one;
    const embot::hw::si7051::Config thermometerSGAUGESconfig {};    
    const embot::hw::BNO055 imuBOSCH = embot::hw::BNO055::one;
    const embot::hw::bno055::Config imuBOSCHconfig {};         
}}}} // namespace embot { namespace hw { namespace bsp { namespace strain2 {

#include "embot_os_theScheduler.h"
#include "embot_app_theLEDmanager.h"
#include "embot_app_application_theCANparserBasic.h"

#include "embot_app_application_theCANparserSTRAIN.h"
#include "embot_app_application_theCANparserIMU.h"
#include "embot_app_application_theCANparserTHERMO.h"

#include "embot_app_application_theSTRAIN.h"
#include "embot_app_application_theIMU.h"
#include "embot_app_application_theTHERMO.h"

#include "embot_app_application_theCANtracer.h"


constexpr embot::os::Event evSTRAINtick = 0x00000001 << 1;
constexpr embot::os::Event evSTRAINdataready = 0x00000001 << 2;
constexpr embot::os::Event evIMUtick = 0x00000001 << 3;
constexpr embot::os::Event evIMUdataready = 0x00000001 << 4;
constexpr embot::os::Event evTHERMOtick = 0x00000001 << 5;
constexpr embot::os::Event evTHERMOdataready = 0x00000001 << 6;

#if defined(ENABLE_IHAVEJUSTSTARTED)
constexpr embot::os::Event evIHAVEjuststarted = 0x00000001 << 7;
static void ihavejuststarted_init(embot::os::EventThread* evthr);
static void ihavejuststarted_tick(std::vector<embot::prot::can::Frame> &outframes);
#endif

void mySYS::userdefOnIdle(embot::os::Thread *t, void* idleparam) const
{
    static int a = 0;
    a++;        
}

void mySYS::userdefonOSerror(void *errparam) const
{
    static int code = 0;
    embot::os::theScheduler::getInstance().getOSerror(code);
    for(;;);    
}


void mySYS::userdefInit_Extra(embot::os::EventThread* evthr, void *initparam) const
{
    // inside the init thread: put the init of many things ...  
    
    // led manager
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second); 

    // init of can basic paser
    embot::app::application::theCANparserBasic::getInstance().initialise({});
        
    // init agent of strain
    embot::app::application::theSTRAIN &thestrain = embot::app::application::theSTRAIN::getInstance();
    embot::app::application::theSTRAIN::Config configstrain(evSTRAINtick, evSTRAINdataready, evthr);
    thestrain.initialise(configstrain); 
    
    // init canparser strain and link it to its agent
    embot::app::application::theCANparserSTRAIN &canparserstrain = embot::app::application::theCANparserSTRAIN::getInstance();
    embot::app::application::theCANparserSTRAIN::Config configparserstrain { &thestrain };
    canparserstrain.initialise(configparserstrain);  
                   
    // init agent of imu
    embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
    embot::app::application::theIMU::Config configimu(embot::hw::bsp::strain2::imuBOSCH, embot::hw::bsp::strain2::imuBOSCHconfig, evIMUtick, evIMUdataready, evthr);
    theimu.initialise(configimu);

    // init canparser imu and link it to its agent
    embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
    embot::app::application::theCANparserIMU::Config configparserimu { &theimu };
    canparserimu.initialise(configparserimu);   
    
    // init agent of thermo
    embot::app::application::theTHERMO &thethermo = embot::app::application::theTHERMO::getInstance();
    embot::app::application::theTHERMO::Config configthermo(embot::hw::bsp::strain2::thermometerSGAUGES, embot::hw::bsp::strain2::thermometerSGAUGESconfig, evTHERMOtick, evTHERMOdataready, evthr);
    thethermo.initialise(configthermo);  

    // init canparser thermo and link it to its agent
    embot::app::application::theCANparserTHERMO &canparserthermo = embot::app::application::theCANparserTHERMO::getInstance();
    embot::app::application::theCANparserTHERMO::Config configparserthermo { &thethermo };
    canparserthermo.initialise(configparserthermo);       
        
#if defined(ENABLE_IHAVEJUSTSTARTED)
    // init service IHaveJustStarted
    ihavejuststarted_init(evthr); 
#endif       
}

void myEVT::userdefStartup(embot::os::Thread *t, void *param) const
{
    // inside startup of evnt thread: put the init of many things ... 
 
    // maybe we start the tx of ft data straight away
#if defined(DEBUG_atstartup_tx_FTdata)
    embot::app::application::theSTRAIN &thestrain = embot::app::application::theSTRAIN::getInstance();
    thestrain.setTXperiod(10*embot::core::time1millisec);
    thestrain.start(embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode::txUncalibrated);
#endif      
}


void myEVT::userdefOnTimeout(embot::os::Thread *t, embot::os::EventMask eventmask, void *param) const
{
    static uint32_t cnt = 0;
    cnt++;    
}


void myEVT::userdefOnEventRXcanframe(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &outframes) const
{        
    // process w/ the basic parser. if not recognised call the parsers specific of the board
    if(true == embot::app::application::theCANparserBasic::getInstance().process(frame, outframes))
    {                   
    }
    if(true == embot::app::application::theCANparserSTRAIN::getInstance().process(frame, outframes))
    {               
    }
    else if(true == embot::app::application::theCANparserIMU::getInstance().process(frame, outframes))
    {               
    }
    else if(true == embot::app::application::theCANparserTHERMO::getInstance().process(frame, outframes))
    {               
    }   
}

void myEVT::userdefOnEventANYother(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, std::vector<embot::prot::can::Frame> &outframes) const
{
    if(true == embot::core::binary::mask::check(eventmask, evSTRAINtick))
    {        
        embot::app::application::theSTRAIN &thestrain = embot::app::application::theSTRAIN::getInstance();
        thestrain.tick(outframes);        
    }
            
    if(true == embot::core::binary::mask::check(eventmask, evSTRAINdataready))
    {        
        embot::app::application::theSTRAIN &thestrain = embot::app::application::theSTRAIN::getInstance();
        thestrain.processdata(outframes);        
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evIMUtick))
    {        
        embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
        theimu.tick(outframes);        
    }   
    
    if(true == embot::core::binary::mask::check(eventmask, evIMUdataready))
    {        
        embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
        theimu.processdata(outframes);        
    }
     
    if(true == embot::core::binary::mask::check(eventmask, evTHERMOtick))
    {        
        embot::app::application::theTHERMO &thethermo = embot::app::application::theTHERMO::getInstance();
        thethermo.tick(outframes);        
    }   
    
    if(true == embot::core::binary::mask::check(eventmask, evTHERMOdataready))
    {        
        embot::app::application::theTHERMO &thethermo = embot::app::application::theTHERMO::getInstance();
        thethermo.processdata(outframes);        
    }  

#if defined(ENABLE_IHAVEJUSTSTARTED)    
    if(true == embot::core::binary::mask::check(eventmask, evIHAVEjuststarted))
    {        
        ihavejuststarted_tick(outframes);
    } 
#endif 
    
}


#if defined(ENABLE_IHAVEJUSTSTARTED)

constexpr uint32_t ntimes = 5;
constexpr embot::core::Time deltatime = 3 * embot::core::time1second;
uint32_t times = 0;

embot::os::Timer * _tmr {nullptr};
static void ihavejuststarted_init(embot::os::EventThread* evthr)
{
    times = 0;
    _tmr = new embot::os::Timer;
    _tmr->start({deltatime, {embot::os::EventToThread{evIHAVEjuststarted, evthr}}, embot::os::Timer::Mode::someshots, ntimes});
}

//action.load(embot::os::EventToThread(pImpl->config.tickevent, pImpl->config.tothread));
//evIHAVEjuststarted

static void ihavejuststarted_tick(std::vector<embot::prot::can::Frame> &outframes)
{
    char ss[64] = {0};
    uint64_t tt = embot::core::now();
    uint32_t mm = tt /= 1000;
    uint32_t s = mm/1000;
    uint32_t m = mm - s*1000;
    
    snprintf(ss, sizeof(ss), "i woke up %ds%dm ago", s, m);
    embot::app::theCANtracer &tracer = embot::app::theCANtracer::getInstance();
    tracer.print(ss, outframes);        
}

#endif // #if defined(ENABLE_IHAVEJUSTSTARTED)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



