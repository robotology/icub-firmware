
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
    embot::app::canprotocol::versionOfAPPLICATION {2, 0, 3},    
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


// - here is the tailoring of the board.


#undef DEBUG_atstartup_tx_FTdata

#include "embot_hw_bsp_strain2.h"

#include "embot_sys_theScheduler.h"
#include "embot_app_theLEDmanager.h"
#include "embot_app_application_theCANparserBasic.h"

#include "embot_app_application_theCANparserSTRAIN.h"
#include "embot_app_application_theCANparserIMU.h"
#include "embot_app_application_theCANparserTHERMO.h"

#include "embot_app_application_theSTRAIN.h"
#include "embot_app_application_theIMU.h"
#include "embot_app_application_theTHERMO.h"


constexpr embot::common::Event evSTRAINtick = 0x00000001 << 1;
constexpr embot::common::Event evSTRAINdataready = 0x00000001 << 2;
constexpr embot::common::Event evIMUtick = 0x00000001 << 3;
constexpr embot::common::Event evIMUdataready = 0x00000001 << 4;
constexpr embot::common::Event evTHERMOtick = 0x00000001 << 5;
constexpr embot::common::Event evTHERMOdataready = 0x00000001 << 6;


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
        
    // init agent of strain
    embot::app::application::theSTRAIN &thestrain = embot::app::application::theSTRAIN::getInstance();
    embot::app::application::theSTRAIN::Config configstrain(evSTRAINtick, evSTRAINdataready, evtsk);
    thestrain.initialise(configstrain); 
    
    // init canparser strain and link it to its agent
    embot::app::application::theCANparserSTRAIN &canparserstrain = embot::app::application::theCANparserSTRAIN::getInstance();
    embot::app::application::theCANparserSTRAIN::Config configparserstrain { &thestrain };
    canparserstrain.initialise(configparserstrain);  
                   
    // init agent of imu
    embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
    embot::app::application::theIMU::Config configimu(embot::hw::bsp::strain2::imuBOSCH, embot::hw::bsp::strain2::imuBOSCHconfig, evIMUtick, evIMUdataready, evtsk);
    theimu.initialise(configimu);

    // init canparser imu and link it to its agent
    embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
    embot::app::application::theCANparserIMU::Config configparserimu { &theimu };
    canparserimu.initialise(configparserimu);   
    
    // init agent of thermo
    embot::app::application::theTHERMO &thethermo = embot::app::application::theTHERMO::getInstance();
    embot::app::application::theTHERMO::Config configthermo(embot::hw::bsp::strain2::thermometerSGAUGES, embot::hw::bsp::strain2::thermometerSGAUGESconfig, evTHERMOtick, evTHERMOdataready, evtsk);
    thethermo.initialise(configthermo);  

    // init canparser thermo and link it to its agent
    embot::app::application::theCANparserTHERMO &canparserthermo = embot::app::application::theCANparserTHERMO::getInstance();
    embot::app::application::theCANparserTHERMO::Config configparserthermo { &thethermo };
    canparserthermo.initialise(configparserthermo);       
        
}

void myEVT::userdefStartup(embot::sys::Task *t, void *param) const
{
    // inside startup of evnt task: put the init of many things ... 
 
    // maybe we start the tx of ft data straight away
#if defined(DEBUG_atstartup_tx_FTdata)
    embot::app::application::theSTRAIN &thestrain = embot::app::application::theSTRAIN::getInstance();
    thestrain.setTXperiod(10*embot::common::time1millisec);
    thestrain.start(embot::app::canprotocol::analog::polling::Message_SET_TXMODE::StrainMode::txUncalibrated);
#endif      
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

void myEVT::userdefOnEventANYother(embot::sys::Task *t, embot::common::EventMask eventmask, void *param, std::vector<embot::hw::can::Frame> &outframes) const
{
    if(true == embot::binary::mask::check(eventmask, evSTRAINtick))
    {        
        embot::app::application::theSTRAIN &thestrain = embot::app::application::theSTRAIN::getInstance();
        thestrain.tick(outframes);        
    }
            
    if(true == embot::binary::mask::check(eventmask, evSTRAINdataready))
    {        
        embot::app::application::theSTRAIN &thestrain = embot::app::application::theSTRAIN::getInstance();
        thestrain.processdata(outframes);        
    }
    
    if(true == embot::binary::mask::check(eventmask, evIMUtick))
    {        
        embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
        theimu.tick(outframes);        
    }   
    
    if(true == embot::binary::mask::check(eventmask, evIMUdataready))
    {        
        embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
        theimu.processdata(outframes);        
    }
     
    if(true == embot::binary::mask::check(eventmask, evTHERMOtick))
    {        
        embot::app::application::theTHERMO &thethermo = embot::app::application::theTHERMO::getInstance();
        thethermo.tick(outframes);        
    }   
    
    if(true == embot::binary::mask::check(eventmask, evTHERMOdataready))
    {        
        embot::app::application::theTHERMO &thethermo = embot::app::application::theTHERMO::getInstance();
        thethermo.processdata(outframes);        
    }  
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



