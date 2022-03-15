

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_SERVICETESTER_H_
#define __EMBOT_APP_ETH_SERVICETESTER_H_

#include "embot_core.h"
#include "EoManagement.h"


namespace embot { namespace app { namespace eth {

    class ServiceTester
    {
    public:
    
        enum class Caller : std::uint8_t { none, threadCFG, threadRUN };
        
        // interface        
        virtual eOmn_serv_category_t category() const = 0;
        virtual eOmn_serv_state_t state() const = 0; 
        virtual const eOmn_serv_configuration_t & configuration() const = 0;        
        virtual bool initialise() = 0;        
        virtual bool tick(Caller caller) = 0;
        
    public:
        virtual ~ServiceTester() {}; 
    }; 
       
}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
