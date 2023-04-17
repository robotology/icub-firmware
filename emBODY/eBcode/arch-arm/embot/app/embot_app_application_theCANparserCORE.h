
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_APPLICATION_THECANPARSERCORE_H_
#define __EMBOT_APP_APPLICATION_THECANPARSERCORE_H_

#include <vector>
#include <memory>

#include "embot_app_application_CANagentCORE.h"

namespace embot { namespace app { namespace application {
    
               
    class theCANparserCORE
    {
    public:
        
        static theCANparserCORE& getInstance();
                
    public:
        
        struct Config
        {
            CANagentCORE* agent {nullptr};  
            
            constexpr Config() = default;
            constexpr Config(CANagentCORE *ag) : agent(ag) {}
            bool isvalid() const { return (nullptr == agent) ? false : true; }
        }; 
        
        
        bool initialise(const Config &config); 
        
        // returns true if the canframe has been recognised. if so, any reply is sent if replies.size() > 0
        bool process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);

    private:
        theCANparserCORE();
        ~theCANparserCORE();    

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;         
    };       


}}} // namespace embot { namespace app { namespace application


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
