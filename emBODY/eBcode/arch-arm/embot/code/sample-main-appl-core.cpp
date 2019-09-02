
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_code_application_core.h"

// --------------------------------------------------------------------------------------------------------------------
// config start



constexpr std::uint16_t taskIDLEstacksize = 512;
constexpr std::uint16_t taskINITstacksize = 2048;
constexpr std::uint16_t taskEVNTstacksize = 2048;

constexpr embot::sys::theTimerManager::Config tmcfg {};
constexpr embot::sys::theCallbackManager::Config cmcfg {};

static void *paramINIT = nullptr;
static void *paramIDLE = nullptr;
static void *paramERR = nullptr;
static void *paramEVNT = nullptr;
    
static const embot::code::application::core::sysConfig syscfg { taskINITstacksize, paramINIT, taskIDLEstacksize, paramIDLE, paramERR, tmcfg, cmcfg};

// config end
// --------------------------------------------------------------------------------------------------------------------

class mySYS final : public embot::code::application::core::SYSTEM
{
public:
    mySYS(const embot::code::application::core::sysConfig &cfg) 
        : SYSTEM(cfg) {}
        
    void userdefInit(void *initparam) const override;
    void userdefOnIdle(embot::sys::Task *t, void* idleparam) const override;
    void userdefonOSerror(void *errparam) const override;
};


static const mySYS mysyscfg { syscfg };
constexpr embot::code::application::core::CFG cfg{ &mysyscfg };

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    embot::code::application::core::run(cfg);
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


void mySYS::userdefInit(void *initparam) const
{
    // put the init of many things ... such as start of services of start of tasks 
    
    // led manager
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(embot::common::time1second);      

    // ...
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



