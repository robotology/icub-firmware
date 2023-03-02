

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __THEAPPLICATION_CONFIG_H_
#define __THEAPPLICATION_CONFIG_H_

#include "embot_app_eth_theApplication.h"

#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_eth_theErrorManager.h"
#include "embot_app_theLEDmanager.h"

#include "ipal_cfg2.h"

namespace embot { namespace app { namespace eth {

    void theApplication_Config_onIdle(embot::os::Thread *t, void* idleparam);
    
    // user defined worker
    void theApplication_Config_inituserdefined(void *p);
    
    // errors
    void theApplication_Config_errorman_onemit(theErrorManager::Severity sev, const theErrorManager::Caller &caller, const theErrorManager::Descriptor &des, const std::string &str);
    
    // NOTE: theApplication_Config.property MUST be used in theEnvironment_config.h to as follow
    //       constexpr theEnvironment::Config theEnvironment_Config {theApplication_Config.property};
    
    constexpr theApplication::Config theApplication_Config
    {
        .property =
        {
            Process::eApplication,
#if defined(WRIST_MK2)
    #if defined(WRIST_MK2_RIGHT)
            {101, 0},
    #else
            {102, 0},
    #endif            
#else            
            {103, 0},  
#endif            
            {2023, Month::Mar, Day::two, 15, 20}
        },
        .OStick = 1000*embot::core::time1microsec,
        .OSstacksizeinit = 10*1024,
        .OSuserdefinit = {theApplication_Config_inituserdefined, nullptr},
        .OSstacksizeidle = 8*1024,
        .OSonidle = theApplication_Config_onIdle,
        .TMRMANconfig = {1024, 8, embot::os::Priority::system50},
        .CBKMANconfig = {2*1024, 8, embot::os::Priority::system49},
        .ERRMANconfig = { theApplication_Config_errorman_onemit },
        .allLEDs =
        {
            embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three, 
            embot::hw::LED::four
        },
        .pulseLED = embot::hw::LED::three,
        .pulseFREQ = embot::core::time1second,
        .IPNETconfig =
        {
            {&ipal_cfg2, true},                     // ipal
            {embot::os::Priority::high44, 4*1024},  // proc
            {embot::os::Priority::high43, 4*1024},  // tick
            25*embot::core::time1millisec,          // maxidletime
            {4, 4}                                  // sockets ... {numberofsockets, maxdatagramenqueuedintx} 
        },
        .LISTENERconfig = 
        {
            { embot::os::Priority::belownorm22, 4*1024 },   // thread
            {{2, 32, 1, 128}, 3333},                        // socket.size, socket.localport
            {embot::app::eth::IPlocalhost, 3333}            // hostaddress
        }    
    };
            
}}}



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
