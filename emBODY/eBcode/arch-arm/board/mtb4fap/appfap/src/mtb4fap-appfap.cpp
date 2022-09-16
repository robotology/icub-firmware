

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
    embot::prot::can::versionOfAPPLICATION {10, 0, 0},
    embot::prot::can::versionOfCANPROTOCOL {2, 0}
};

constexpr std::uint16_t threadIDLEstacksize = 512;
constexpr std::uint16_t threadINITstacksize = 2048;
constexpr std::uint16_t threadEVNTstacksize = 5*1024;
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
    void userdefInit_Extra(embot::os::EventThread* evtsk, void *initparam) const override;
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


#include "embot_hw_si7051.h"
#include "embot_hw_bno055.h"

namespace embot { namespace hw { namespace bsp { namespace mtb4 {

    constexpr embot::hw::SI7051 thermometer = embot::hw::SI7051::one;
    constexpr embot::hw::si7051::Config thermometerconfig = {};

    constexpr embot::hw::BNO055 imuBOSCH = embot::hw::BNO055::one;
    constexpr embot::hw::bno055::Config imuBOSCHconfig = {};

}}}} // namespace embot { namespace hw { namespace bsp { namespace mtb4 {

#include "embot_os_theScheduler.h"
#include "embot_app_theLEDmanager.h"
#include "embot_app_application_theCANparserBasic.h"

#include "embot_app_application_theFAPreader2.h"
#include "embot_app_application_theCANparserPOS.h"
#include "embot_app_application_theCANparserIMU.h"
#include "embot_app_application_theIMU.h"
#include "embot_app_application_theCANparserTHERMO.h"
#include "embot_app_application_theTHERMO.h"

constexpr embot::os::Event evPOSprocess = 0x00000001 << 1;
constexpr embot::os::Event evIMUtick = 0x00000001 << 3;
constexpr embot::os::Event evIMUdataready = 0x00000001 << 4;
constexpr embot::os::Event evTHERMOtick = 0x00000001 << 5;
constexpr embot::os::Event evTHERMOdataready = 0x00000001 << 6;


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


void mySYS::userdefInit_Extra(embot::os::EventThread* evtsk, void *initparam) const
{
    // inside the init thread: put the init of many things ...

    // led manager
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();
    theleds.init(allleds);
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second);

    // init of can basic paser
    embot::app::application::theCANparserBasic::getInstance().initialise({});

    // init agent of pos
    embot::app::application::theFAPreader2 &thefapreader2 = embot::app::application::theFAPreader2::getInstance();
    embot::app::application::theFAPreader2::Config configfapreader2;
#warning TODO init properly        
//    configthefapreader2.tickevent = evPOSprocess;
//    configthefapreader2.totask = evtsk;
    thefapreader2.initialise(configfapreader2);

    // init canparser pos and link it to its agent
    embot::app::application::theCANparserPOS &canparserpos = embot::app::application::theCANparserPOS::getInstance();
    embot::app::application::theCANparserPOS::Config configparserpos { &thefapreader2 };
    canparserpos.initialise(configparserpos);

    // init agent of imu
    embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
    embot::app::application::theIMU::Config configimu(embot::hw::bsp::mtb4::imuBOSCH, embot::hw::bsp::mtb4::imuBOSCHconfig, evIMUtick, evIMUdataready, evtsk);
    theimu.initialise(configimu);

    // init canparser imu and link it to its agent
    embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
    embot::app::application::theCANparserIMU::Config configparserimu { &theimu };
    canparserimu.initialise(configparserimu);

    // init agent of thermo
    embot::app::application::theTHERMO &thethermo = embot::app::application::theTHERMO::getInstance();
    embot::app::application::theTHERMO::Config configthermo(embot::hw::bsp::mtb4::thermometer, embot::hw::bsp::mtb4::thermometerconfig, evTHERMOtick, evTHERMOdataready, evtsk);
    thethermo.initialise(configthermo);

    // init canparser thermo and link it to its agent
    embot::app::application::theCANparserTHERMO &canparserthermo = embot::app::application::theCANparserTHERMO::getInstance();
    embot::app::application::theCANparserTHERMO::Config configparserthermo { &thethermo };
    canparserthermo.initialise(configparserthermo);

}

void myEVT::userdefStartup(embot::os::Thread *t, void *param) const
{
    // inside startup of evnt thread: put the init of many things ...
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
    if(true == embot::app::application::theCANparserPOS::getInstance().process(frame, outframes))
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
    embot::app::application::theFAPreader2 &thefapreader2 = embot::app::application::theFAPreader2::getInstance();
    if(true == thefapreader2.isvalid(eventmask))
    {
        thefapreader2.process(eventmask);
        #warning TODO: add a way to retrieve the outframes if any ...
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
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------





