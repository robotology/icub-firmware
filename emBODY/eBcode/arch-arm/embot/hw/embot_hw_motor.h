

/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_H_
#define __EMBOT_HW_MOTOR_H_

#include "embot_core.h"
#include "embot_hw.h"
#include "embot_hw_types.h"


namespace embot { namespace hw { namespace motor {
           
    struct Config
    { 
        uint32_t tbd {0};        
        constexpr Config() = default;         
        constexpr Config(uint32_t t) : tbd(t) {} 
        constexpr bool isvalid() const { return true; }
    };
       
    
    std::string to_string(embot::hw::MOTOR id);
            
    bool supported(embot::hw::MOTOR h);    
    bool initialised(embot::hw::MOTOR h);    
    result_t init(embot::hw::MOTOR h, const Config &config);        
    
    bool enabled(MOTOR h);
    result_t enable(MOTOR h, bool on);
        
    // whatever else we shall need
    
    // Position and Pwm are to be defined yet.
    using Position = std::int32_t;
    using Pwm = std::int32_t;
    
    result_t getencoder(embot::hw::MOTOR h, Position &position);
    result_t gethallcounter(embot::hw::MOTOR h, Position &position);
    
    result_t setpwm(MOTOR h, Pwm v);
        
    result_t gethallstatus(embot::hw::MOTOR h, uint8_t &hs);
    result_t setpwmUVW(MOTOR h, Pwm u, Pwm v, Pwm w);

    result_t motorEnable(MOTOR h);
    result_t motorDisable(MOTOR h);
    
    
    // so far, we keep int32_t as it is more general. even if lower levels may propagate them as int16_t
    struct Currents
    {
        std::int32_t u {0};
        std::int32_t v {0};
        std::int32_t w {0};
        constexpr Currents() = default;
    };
    
    using fpOnCurrents = void (*)(void *owner, const Currents * const currents);
    
    result_t setCallbackOnCurrents(MOTOR h, fpOnCurrents callback, void *owner);
    
}}} // namespace embot { namespace hw { namespace motor {
    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


