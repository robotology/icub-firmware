
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BLDC_PWM_H_
#define __EMBOT_HW_MOTOR_BLDC_PWM_H_

#include "embot_hw_motor_bldc.h"

namespace embot::hw::motor::bldc::pwm {
    
    struct Configuration
    {
        enum class START { immediate };        
        START start { START::immediate };            
        constexpr Configuration() = default;
        constexpr bool isvalid() const { return true; }
    };

    bool supported(embot::hw::MOTOR m);
    bool initialised(embot::hw::MOTOR m);
    
    void init(embot::hw::MOTOR m, const Configuration &config);
    void deinit(embot::hw::MOTOR m);
    void enable(embot::hw::MOTOR m, bool on);
    void set(embot::hw::MOTOR m, const PWM3 &pwm); 
    void setperc(embot::hw::MOTOR m, float u, float v, float w);    
        
} // namespace embot::hw::motor::bldc::pwm {

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

