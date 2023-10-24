

/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEMCSERVICEEOTESTERCONFIG_H_
#define __EMBOT_APP_ETH_THEMCSERVICEEOTESTERCONFIG_H_

#include "embot_core.h"
#include "embot_core_binary.h"

#include "EoCommon.h"
#include "EoProtocol.h"

#if defined(USE_EMBOT_theServices)  
#include "embot_app_eth_Service.h"
#else
#include "EOtheServices.h"
#endif

#include "embot_app_eth_theServiceTester.h"

#include <vector>
#include <memory>


namespace embot { namespace app { namespace eth {
      
    
    class theMCserviceEOtesterconfig
    {
    public:
        static theMCserviceEOtesterconfig& getInstance();
                
    public:
    
        enum class Type { ems_3foc, mc4plus_2pwm, amc_3amcbldc, amc_2amcbldc_1amc2c }; 
                
        const theServiceTester::Config & servicetesterconfig(Type type = Type::ems_3foc) const;        
                           
    private:
        theMCserviceEOtesterconfig(); 
        ~theMCserviceEOtesterconfig(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    }; 


}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
