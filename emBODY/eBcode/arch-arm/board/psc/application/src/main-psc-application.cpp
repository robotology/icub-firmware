

/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_code_application_evntskcan.h"

// --------------------------------------------------------------------------------------------------------------------
// config start

constexpr embot::app::theCANboardInfo::applicationInfo applInfo 
{ 
    embot::app::canprotocol::versionOfAPPLICATION {1, 2, 2},    
    embot::app::canprotocol::versionOfCANPROTOCOL {2, 0}    
};

constexpr std::uint16_t taskIDLEstacksize = 512;
constexpr std::uint16_t taskINITstacksize = 2048;
constexpr std::uint16_t taskEVNTstacksize = 5*1024;
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


// - here is the tailoring of the board.


#include "embot_sys_theScheduler.h"
#include "embot_app_theLEDmanager.h"
#include "embot_app_application_theCANparserBasic.h"

#include "embot_app_application_theCANparserPOS.h"
#include "embot_app_application_thePOSreader.h"

#include "embot_app_application_theCANparserSkin.h"
#include "embot_app_application_theSkin.h"


constexpr embot::common::Event evPOS0Xacquire = 0x00000001 << 1;
constexpr embot::common::Event evPOS01dataready = 0x00000001 << 2;
constexpr embot::common::Event evPOS02dataready = 0x00000001 << 3;
constexpr embot::common::Event evSKINprocess = 0x00000001 << 4;

constexpr std::array<embot::app::application::thePOSreader::Sensor, embot::app::application::thePOSreader::numberofpositions> POSsensors = 
{{
    {
        embot::hw::TLV493D::one,
        { embot::hw::I2C::one, 400000 }        
    },
    {
        embot::hw::TLV493D::two,
        { embot::hw::I2C::three, 400000 }   
    }   
}};

constexpr embot::app::application::thePOSreader::Events POSevents = { evPOS0Xacquire, {{ evPOS01dataready, evPOS02dataready }} };


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
        
    // init agent of POSreader
    embot::app::application::thePOSreader &thepos = embot::app::application::thePOSreader::getInstance();
    embot::app::application::thePOSreader::Config configpos { evtsk, POSsensors, POSevents };
    thepos.initialise(configpos); 
    
    // init parser of POS and link it to its agent: thePOSreader
    embot::app::application::theCANparserPOS &canparserpos = embot::app::application::theCANparserPOS::getInstance();
    embot::app::application::theCANparserPOS::Config configparserpos { &thepos };
    canparserpos.initialise(configparserpos);    

    // init agent of skin 
    embot::app::application::theSkin &theskin = embot::app::application::theSkin::getInstance();
    embot::app::application::theSkin::Config configskin;
    configskin.tickevent = evSKINprocess;
    configskin.totask = evtsk;
    theskin.initialise(configskin);   
    
    // init canparser skin and link it to its agent
    embot::app::application::theCANparserSkin &canparserskin = embot::app::application::theCANparserSkin::getInstance();
    embot::app::application::theCANparserSkin::Config configparserskin { &theskin };
    canparserskin.initialise(configparserskin);             
}


void myEVT::userdefStartup(embot::sys::Task *t, void *param) const
{
    // inside startup of evnt task: put the init of many things ... 
    
}


void myEVT::userdefOnTimeout(embot::sys::Task *t, embot::common::EventMask eventmask, void *param) const
{
    static uint32_t cnt = 0;
    cnt++;    
}


void myEVT::userdefOnEventRXcanframe(embot::sys::Task *t, embot::common::EventMask eventmask, void *param, const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &outframes) const
{        
    // process w/ the basic parser. if not recognised call the parsers specific of the board
    if(true == embot::app::application::theCANparserBasic::getInstance().process(frame, outframes))
    {                   
    }
    if(true == embot::app::application::theCANparserPOS::getInstance().process(frame, outframes))
    {               
    }
    else if(true == embot::app::application::theCANparserSkin::getInstance().process(frame, outframes))
    {               
    }
 
}

void myEVT::userdefOnEventANYother(embot::sys::Task *t, embot::common::EventMask eventmask, void *param, std::vector<embot::hw::can::Frame> &outframes) const
{
    if(true == embot::binary::mask::check(eventmask, evPOS0Xacquire))
    {        
        embot::app::application::thePOSreader &thepos = embot::app::application::thePOSreader::getInstance();
        thepos.process(evPOS0Xacquire, outframes);        
    }
    
    if(true == embot::binary::mask::check(eventmask, evPOS01dataready))
    {        
        embot::app::application::thePOSreader &thepos = embot::app::application::thePOSreader::getInstance();
        thepos.process(evPOS01dataready, outframes);        
    }
    
    if(true == embot::binary::mask::check(eventmask, evPOS02dataready))
    {        
        embot::app::application::thePOSreader &thepos = embot::app::application::thePOSreader::getInstance();
        thepos.process(evPOS02dataready, outframes);        
    }    
            
    if(true == embot::binary::mask::check(eventmask, evSKINprocess))
    {
        embot::app::application::theSkin &theskin = embot::app::application::theSkin::getInstance();
        theskin.tick(outframes);        
    }  
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------






