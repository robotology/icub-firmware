
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BLDC_H_
#define __EMBOT_APP_BLDC_H_



#include "embot_app_msg.h"
#include "embot_prot_can.h"


namespace embot::app::bldc {
    
    struct MSG
    {
        embot::app::msg::Location location {};
        embot::prot::can::Frame frame {};  
        constexpr MSG() = default; 
        constexpr MSG(const embot::app::msg::Location& l, const embot::prot::can::Frame& f) : location(l), frame(f) {}  
        constexpr MSG(const embot::prot::can::Frame& f) : location({}), frame(f) {}            
    };    
    
} // namespace embot::app::bldc {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
