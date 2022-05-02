

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __THEERRORMANAGER_CONFIG_H_
#define __THEERRORMANAGER_CONFIG_H_

#include "embot_app_eth_theErrorManager.h"

#include "theApplication_config.h"


namespace embot { namespace app { namespace eth {
    
//    void onemit(theErrorManager::Severity sev, const theErrorManager::Descriptor &des, const std::string &str)
//    {        
//        embot::core::print("HI: "+ str);
//        
//        if(theErrorManager::Severity::fatal == sev)
//        {
//            for(;;);
//        }
//    }

    constexpr theErrorManager::Config theErrorManager_Config = theApplication_Config.ERRMANconfig;
//    {
//        theApplication_Config.ERRMANconfig.onemit        
//    };
            
}}}



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
