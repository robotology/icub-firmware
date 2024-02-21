
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_APP_BOARD_AMCBLDC_THEMBD_H_
#define _EMBOT_APP_BOARD_AMCBLDC_THEMBD_H_


#include <memory>

#include "embot_core.h"
#include "embot_app_bldc.h"

namespace embot::app::board::amcbldc {
           
    class theMBD 
    {
    public:
        static theMBD& getInstance();
    
        struct Config
        {
            embot::app::msg::ADR adr {1};            
            constexpr Config() = default;
            constexpr Config(embot::app::msg::ADR a) : adr(a) {}
        };
                
        bool initialise(const Config &config);   
        bool tick(const std::vector<embot::app::bldc::MSG> &inpframes, std::vector<embot::app::bldc::MSG> &outframes);
         
        void on();
        void off();
        
    private:
        theMBD(); 
        ~theMBD(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;    
    };       
    
} // namespace


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
