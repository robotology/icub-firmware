

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __THEENVIRONMENT_CONFIG_H_
#define __THEENVIRONMENT_CONFIG_H_

#include "embot_app_eth_theEnvironment.h"

namespace embot { namespace app { namespace eth {
   
    constexpr theEnvironment::Config theEnvironment_Config 
    {
        Process::eApplication,
        {100, 99},
        {2023, Month::Dec, Day::one, 10, 55}  
    };        
            
}}}



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
