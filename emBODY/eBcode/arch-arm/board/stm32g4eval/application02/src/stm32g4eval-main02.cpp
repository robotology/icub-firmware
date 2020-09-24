
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_app_skeleton_os_evthreadcan.h"

#include "tCOMM.h"

// --------------------------------------------------------------------------------------------------------------------
// config start

constexpr embot::app::theCANboardInfo::applicationInfo applInfo 
{ 
    embot::prot::can::versionOfAPPLICATION {1, 0, 0},    
    embot::prot::can::versionOfCANPROTOCOL {2, 0}    
};

constexpr std::uint16_t threadIDLEstacksize = 2048;
constexpr std::uint16_t threadINITstacksize = 2048;
constexpr std::uint16_t threadCOMMstacksize = 4096;
constexpr std::uint8_t maxINPcanframes = 16;
constexpr std::uint8_t maxOUTcanframes = 48;
constexpr embot::core::relTime threadCOMMtimeout = 50*embot::core::time1millisec;

static void *paramINIT = nullptr;
static void *paramIDLE = nullptr;
static void *paramERR = nullptr;
static void *paramCOMM = nullptr;

constexpr embot::os::theTimerManager::Config tmcfg {};
constexpr embot::os::theCallbackManager::Config cmcfg {};
    
    
static const embot::app::skeleton::os::basic::sysConfig syscfg { threadINITstacksize, paramINIT, threadIDLEstacksize, paramIDLE, paramERR, tmcfg, cmcfg};

static const embot::app::skeleton::os::evthreadcan::evtConfig COMMthreadConfig { threadCOMMstacksize, paramCOMM, threadCOMMtimeout};

static const embot::app::skeleton::os::evthreadcan::canConfig COMMcancfg { maxINPcanframes, maxOUTcanframes };

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


static const mySYS mysys { syscfg };
static const embot::app::ctrl::tCOMM t_comm { COMMthreadConfig, COMMcancfg, applInfo };
constexpr embot::app::skeleton::os::evthreadcan::CFG cfg{ &mysys, &t_comm };

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    embot::app::skeleton::os::evthreadcan::run(cfg);
    for(;;);    
}


// - here is the tailoring of the board.
//   the init thread will start:
//   - the COMM thread is the one defined in the embot::app::skeleton::os::evthreadcan::evThreadCAN object
//     it will deal only with RX of CAN frames, passing them to parser, obtaining from them possible replies,
//     transmitting them back.
//     it will also accept TX of CAN frames as other thread may request.

#define enableTRACE_all


#include "embot_os_theScheduler.h"
#include "embot_app_theLEDmanager.h"
#include "embot_app_application_theCANparserBasic.h"
#include "embot_app_application_theCANtracer.h"


#include "embot_hw_bsp.h"

#include "tSNSR.h"
#include "tCOMM.h"
#include "tCTRL.h"

//constexpr embot::os::Event evtTick = embot::core::binary::mask::pos2mask<embot::os::Event>(0);
//constexpr embot::core::relTime tickperiod = 2000*embot::core::time1millisec;

//constexpr embot::os::Event evtReadFAP = embot::core::binary::mask::pos2mask<embot::os::Event>(1);
//embot::hw::tlv493d::Position positionFAP = 0;


//void alertFAPdataisready(void *p)
//{
//    embot::os::Thread *t = reinterpret_cast<embot::os::Thread*>(p);    
//    t->setEvent(evtReadFAP);     
//}


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


embot::app::ctrl::tSNSR *t_snsr {nullptr};
embot::app::ctrl::tCTRL *t_ctrl {nullptr};

void mySYS::userdefInit_Extra(embot::os::EventThread* evthr, void *initparam) const
{
    // inside the init thread: put the init of many things ...  
    // led manager
    static const std::initializer_list<embot::hw::LED> alltheleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(alltheleds);    
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second); 

    
    // the communication thread is already running. 
    
    
    #warning TODO: add the start of other two threads (tCTRL + tSNSR)
    
    t_snsr = new embot::app::ctrl::tSNSR;    
    t_snsr->start({});
 
    t_ctrl = new embot::app::ctrl::tCTRL;    
    t_ctrl->start({});        
    
                   
//    // init agent of imu
//    embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
//    embot::app::application::theIMU::Config configimu(embot::hw::bsp::strain2::imuBOSCH, embot::hw::bsp::strain2::imuBOSCHconfig, evIMUtick, evIMUdataready, evthr);
//    theimu.initialise(configimu);

//    // init canparser imu and link it to its agent
//    embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
//    embot::app::application::theCANparserIMU::Config configparserimu { &theimu };
//    canparserimu.initialise(configparserimu);             
    
}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



