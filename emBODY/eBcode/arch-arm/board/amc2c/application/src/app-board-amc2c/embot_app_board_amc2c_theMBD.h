
/*

 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_APP_BOARD_AMC_THEMBD_H_
#define _EMBOT_APP_BOARD_AMC_THEMBD_H_


#include <memory>

#include "embot_core.h"
#include "embot_prot_can.h"

namespace embot::app::board::amc2c {
           
    class theMBD 
    {
    public:
        static theMBD& getInstance();
    
        struct Config
        {
            embot::prot::can::Address adr {1};            
            constexpr Config() = default;
            constexpr Config(embot::prot::can::Address a) : adr(a) {}
        };
                
        bool initialise(const Config &config);   
        bool tick(const std::vector<embot::prot::can::Frame> &inpframes, std::vector<embot::prot::can::Frame> &outframes);
         
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
