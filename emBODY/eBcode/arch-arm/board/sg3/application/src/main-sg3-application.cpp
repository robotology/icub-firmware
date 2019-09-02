
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_code_application_evntskcan.h"

// --------------------------------------------------------------------------------------------------------------------
// config start

constexpr embot::app::theCANboardInfo::applicationInfo applInfo 
{ 
    embot::app::canprotocol::versionOfAPPLICATION {0, 0, 4},    
    embot::app::canprotocol::versionOfCANPROTOCOL {2, 0}    
};

constexpr std::uint16_t taskIDLEstacksize = 512;
constexpr std::uint16_t taskINITstacksize = 2048;
constexpr std::uint16_t taskEVNTstacksize = 4096;
constexpr std::uint8_t maxINPcanframes = 16;
constexpr std::uint8_t maxOUTcanframes = 48;
constexpr embot::common::relTime taskEVNTtimeout = 50*embot::common::time1millisec;

static void *paramINIT = nullptr;
static void *paramIDLE = nullptr;
static void *paramERR = nullptr;
static void *paramEVNT = nullptr;

constexpr embot::sys::theTimerManager::Config tmcfg {};
constexpr embot::sys::theCallbackManager::Config cmcfg {};
    
    
static const embot::code::application::core::sysConfig syscfg { taskINITstacksize, paramINIT, taskIDLEstacksize, paramIDLE, paramERR, tmcfg, cmcfg};

static const embot::code::application::evntskcan::evtConfig evtcfg { taskEVNTstacksize, paramEVNT, taskEVNTtimeout};

static const embot::code::application::evntskcan::canConfig cancfg { maxINPcanframes, maxOUTcanframes };

// config end
// --------------------------------------------------------------------------------------------------------------------


class mySYS final : public embot::code::application::evntskcan::SYSTEMevtcan
{
public:
    mySYS(const embot::code::application::core::sysConfig &cfg) 
        : SYSTEMevtcan(cfg) {}
        
    void userdefOnIdle(embot::sys::Task *t, void* idleparam) const override;
    void userdefonOSerror(void *errparam) const override;
    void userdefInit_Extra(embot::sys::EventTask* evtsk, void *initparam) const override;
};


class myEVT final : public embot::code::application::evntskcan::EVNTSKcan
{
public:
    myEVT(const embot::code::application::evntskcan::evtConfig& ecfg, const embot::code::application::evntskcan::canConfig& ccfg, const embot::app::theCANboardInfo::applicationInfo& a) 
        : EVNTSKcan(ecfg, ccfg, a) {}
        
    void userdefStartup(embot::sys::Task *t, void *param) const override;
    void userdefOnTimeout(embot::sys::Task *t, embot::common::EventMask eventmask, void *param) const override;
    void userdefOnEventRXcanframe(embot::sys::Task *t, embot::common::EventMask eventmask, void *param, const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &outframes) const override;
    void userdefOnEventANYother(embot::sys::Task *t, embot::common::EventMask eventmask, void *param, std::vector<embot::hw::can::Frame> &outframes) const override;                   
};


static const mySYS mysys { syscfg };
static const myEVT myevt { evtcfg, cancfg, applInfo };
constexpr embot::code::application::evntskcan::CFG cfg{ &mysys, &myevt };

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    embot::code::application::evntskcan::run(cfg);
    for(;;);    
}


// - here is the tailoring of the board: strain2, mtb4, psc, etc.


#include "embot_sys_theScheduler.h"
#include "embot_app_theLEDmanager.h"
#include "embot_app_application_theCANparserBasic.h"

void mySYS::userdefOnIdle(embot::sys::Task *t, void* idleparam) const
{
    static int a = 0;
    a++;        
}

void mySYS::userdefonOSerror(void *errparam) const
{
    static int code = 0;
    embot::sys::theScheduler::getInstance().getOSerror(code);
    for(;;);    
}


void mySYS::userdefInit_Extra(embot::sys::EventTask* evtsk, void *initparam) const
{
    // inside the init task: put the init of many things ...  
    
    // led manager
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(embot::common::time1second);    

    // init of can basic paser
    embot::app::application::theCANparserBasic::getInstance().initialise({});    

    // ...
}

void myEVT::userdefStartup(embot::sys::Task *t, void *param) const
{
    // inside startup of evnt task: put the init of many things ... 

    // ...
}


void myEVT::userdefOnTimeout(embot::sys::Task *t, embot::common::EventMask eventmask, void *param) const
{
    static uint32_t cnt = 0;
    cnt++;    
}


void myEVT::userdefOnEventRXcanframe(embot::sys::Task *t, embot::common::EventMask eventmask, void *param, const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &outframes) const
{    
    // process w/ the basic parser, if not recognised call the parse specific of the board
    if(true == embot::app::application::theCANparserBasic::getInstance().process(frame, outframes))
    {                   
    }
//    else if(true == otherparser.process(frame, outframes))
//    {
//    }
    // ...
}

void myEVT::userdefOnEventANYother(embot::sys::Task *t, embot::common::EventMask eventmask, void *param, std::vector<embot::hw::can::Frame> &outframes) const
{
    constexpr embot::common::Event evXXX = 0x00000001 << 31;

    if(true == embot::binary::mask::check(eventmask, evXXX))
    {        
        // do things      
    }
    
    // ...    
}
   
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




