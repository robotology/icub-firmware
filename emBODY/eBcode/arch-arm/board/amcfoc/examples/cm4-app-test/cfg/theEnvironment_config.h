

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __THEENVIRONMENT_CONFIG_H_
#define __THEENVIRONMENT_CONFIG_H_

#include "embot_app_eth_theEnvironment.h"
#include "theApplication_config.h"

namespace embot { namespace app { namespace eth {
   
    constexpr theEnvironment::Config theEnvironment_Config {theApplication_Config.property};        
            
}}}



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
