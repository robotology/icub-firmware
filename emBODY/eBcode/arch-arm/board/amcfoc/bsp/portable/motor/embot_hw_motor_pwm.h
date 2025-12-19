/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_PWM_H_
#define __EMBOT_HW_MOTOR_PWM_H_

#include "embot_core.h"
#include "embot_hw.h"

#include "embot_hw_motor_bldc.h"

#if 0

#endif


namespace embot::hw::motor::pwm {
    
    struct Configuration
    {
        enum class START { immediate };        
        START start { START::immediate };            
        constexpr Configuration() = default;
        constexpr bool isvalid() const { return true; }
    };

    void init(embot::hw::MOTOR m, const Configuration &config);
    void deinit(embot::hw::MOTOR m);
    void enable(embot::hw::MOTOR m, bool on);
    void set(embot::hw::MOTOR m, uint16_t u, uint16_t v, uint16_t w); 
    void setperc(embot::hw::MOTOR m, float u, float v, float w);
        
} // namespace embot::hw::motor::pwm {

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

