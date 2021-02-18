
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
    embot::prot::can::versionOfAPPLICATION {1, 0, 0},    
    embot::prot::can::versionOfCANPROTOCOL {2, 0}    
};

constexpr std::uint16_t threadIDLEstacksize = 2048;
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

#define enableTRACE_all


#include "embot_os_theScheduler.h"
#include "embot_app_theLEDmanager.h"
#include "embot_app_application_theCANparserBasic.h"
#include "embot_app_application_theCANtracer.h"

#include "embot_hw_bsp.h"
#include "embot_hw_gpio.h"
#include "embot_hw_can.h"

#include "embot_app_application_theCANparserMC.h"
#include "embot_app_application_theMCagent.h"


constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(2);
constexpr embot::core::relTime tickperiod = 2000*embot::core::time1millisec;



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
    static const std::initializer_list<embot::hw::LED> alltheleds = { 
        embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three
    };  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(alltheleds);   
    
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second); 

       
    embot::app::LEDwaveT<64> ledwave(100*embot::core::time1millisec, 30, std::bitset<64>(0b000001));
    theleds.get(embot::hw::LED::two).wave(&ledwave);     

    
    // init of can basic paser
    embot::app::application::theCANparserBasic::getInstance().initialise({});
        
    
                   
    // init agent of mc
    embot::app::application::theMCagent &themcagent = embot::app::application::theMCagent::getInstance();
    themcagent.initialise({});

    // init canparser mc and link it to its agent
    embot::app::application::theCANparserMC &canparsermc = embot::app::application::theCANparserMC::getInstance();
    embot::app::application::theCANparserMC::Config configparsermc { &themcagent };
    canparsermc.initialise(configparsermc);   
        
    
}


void myEVT::userdefStartup(embot::os::Thread *t, void *param) const
{
    // inside startup of evnt thread: put the init of many things ... 
       
    embot::core::print("userdefStartup(): start a timer which sends a tick event");
       
    // start a timer which sends a tick event
    embot::os::Timer *tmr = new embot::os::Timer;   
    embot::os::Action act(embot::os::EventToThread(evtTick, t));
    embot::os::Timer::Config cfg{tickperiod, act, embot::os::Timer::Mode::forever, 0};
    tmr->start(cfg);     
      
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
    else if(true == embot::app::application::theCANparserMC::getInstance().process(frame, outframes))
    {               
    }

}



void myEVT::userdefOnEventANYother(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, std::vector<embot::prot::can::Frame> &outframes) const
{
    
    static embot::core::Time startOfFAPacquisition = 0;
    
    if(true == embot::core::binary::mask::check(eventmask, evtTick)) 
    {

     
    }   
    
}





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



