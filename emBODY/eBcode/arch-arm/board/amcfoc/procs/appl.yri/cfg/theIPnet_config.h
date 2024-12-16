

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __THEIPNET_CONFIG_H_
#define __THEIPNET_CONFIG_H_

#include "embot_app_eth_theIPnet.h"

#include "theApplication_config.h"

// for the ipal config we use (variable ipal_cfg2) we need:
//#include "ipal_cfg2.h"

namespace embot { namespace app { namespace eth {

    constexpr theIPnet::Config theIPnet_Config = theApplication_Config.IPNETconfig;
//    {
////        {&ipal_cfg2, true},                     // ipal
////        {embot::os::Priority::high44, 4*1024},  // proc
////        {embot::os::Priority::high43, 4*1024},  // tick
////        25*embot::core::time1millisec,          // maxidletime
////        {4, 4}                                  // sockets   
//        theApplication_Config.IPNETconfig
//    };
            
}}}



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
