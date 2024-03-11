
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
    
    
    struct Rounder
    {
    private:

        float value {0.0f};
        float rounded_value {0.0f};
        float multiplier {0.0f};
        uint8_t decimals {0};

        float clipValue(float value) {
            return std::roundf(value * multiplier) / multiplier;
        }

    public:

        Rounder() = default;

        void init(float value, uint8_t decimals)
        {
            this->value = value;
            this->decimals = decimals;
            multiplier = std::pow(10.0f, decimals);
        }

        float getRoundedValueOf(const float &value)
        {
            if(this->value == value)
            {
                return rounded_value;
            }

            this->value = value;
            this->rounded_value = clipValue(value);
            return rounded_value;
        }
    };
    
} // namespace embot::app::bldc {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
