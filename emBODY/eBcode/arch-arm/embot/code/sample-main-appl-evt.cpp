
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_code_application_evntsk.h"

// --------------------------------------------------------------------------------------------------------------------
// config start



constexpr std::uint16_t taskIDLEstacksize = 512;
constexpr std::uint16_t taskINITstacksize = 2048;

constexpr embot::sys::theTimerManager::Config tmcfg {};
constexpr embot::sys::theCallbackManager::Config cmcfg {};

static void *paramINIT = nullptr;
static void *paramIDLE = nullptr;
static void *paramERR = nullptr;
    

constexpr std::uint16_t taskEVNTstacksize = 2048;    
static void *paramEVNT = nullptr;
constexpr embot::common::relTime taskEVNTtimeout = 50*embot::common::time1millisec;
    
static const embot::code::application::core::sysConfig syscfg { taskINITstacksize, paramINIT, taskIDLEstacksize, paramIDLE, paramERR, tmcfg, cmcfg};

static const embot::code::application::evntsk::evtConfig evtcfg { taskEVNTstacksize, paramEVNT, taskEVNTtimeout};

// config end
// --------------------------------------------------------------------------------------------------------------------

class mySYS final : public embot::code::application::evntsk::SYSTEMevt
{
public:
    mySYS(const embot::code::application::core::sysConfig &cfg) 
        : SYSTEMevt(cfg) {}
        
    void userdefOnIdle(embot::sys::Task *t, void* idleparam) const override;
    void userdefonOSerror(void *errparam) const override;
    void userdefInit_Extra(embot::sys::EventTask* evtsk, void *initparam) const override;
};


class myEVT final : public embot::code::application::evntsk::EVNTSK
{
public:
    myEVT(const embot::code::application::evntsk::evtConfig& cfg) 
        : EVNTSK(cfg) {}
        
    void userdefStartup(embot::sys::Task *t, void *param) const override;
    void userdefOnTimeout(embot::sys::Task *t, embot::common::EventMask eventmask, void *param) const override;
    void userdefOnEvent(embot::sys::Task *t, embot::common::EventMask eventmask, void *param) const override;
};


static const mySYS mysys { syscfg };
static const myEVT myevt { evtcfg };
constexpr embot::code::application::evntsk::CFG cfg{ &mysys, &myevt };

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    embot::code::application::evntsk::run(cfg);
    for(;;);    
}


// - here is the tailoring of the board: strain2, mtb4, psc, etc.


#include "embot_sys_theScheduler.h"
#include "embot_app_theLEDmanager.h"

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


void myEVT::userdefOnEvent(embot::sys::Task *t, embot::common::EventMask eventmask, void *param) const
{
    constexpr embot::common::Event evXXX = 0x00000001 << 31;

    if(true == embot::binary::mask::check(eventmask, evXXX))
    {        
        // do things      
    }
    
    // ...    
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



