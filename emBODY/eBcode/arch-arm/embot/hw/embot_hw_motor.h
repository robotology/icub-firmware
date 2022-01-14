

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
    
    // we manage: pwm, current, position. so far we use int32_t w/out a specific measurement unit
    using Position = std::int32_t;
    using Pwm = std::int32_t;
    using HallStatus = std::uint8_t;
    
    // so far, we keep int32_t as it is more general. even if lower levels may propagate them as int16_t
    struct Currents
    {
        std::int32_t u {0};
        std::int32_t v {0};
        std::int32_t w {0};
        constexpr Currents() = default;
    };  
        
    using fpOnCurrents = void (*)(void *owner, const Currents * const currents);
      
    
    struct Config
    { 
        uint32_t tbd {0};        
        constexpr Config() = default;         
        constexpr Config(uint32_t t) : tbd(t) {} 
        constexpr bool isvalid() const { return true; }
    };
           
    std::string to_string(embot::hw::MOTOR id);
            
    bool supported(embot::hw::MOTOR h);   
    bool faulted(MOTOR h);    
    bool initialised(embot::hw::MOTOR h);
    bool enabled(MOTOR h);  
    
    result_t fault(MOTOR h, bool on);
    result_t init(embot::hw::MOTOR h, const Config &config);   
    // enable(h, true) is effective only if(false == faulted(h))    
    result_t enable(MOTOR h, bool on);
    
    // the pwm can be effectively applied only if(true == enabled(h)), so it also must be: (false == faulted(h))
    result_t setpwm(MOTOR h, Pwm v);
    result_t setpwm(MOTOR h, Pwm u, Pwm v, Pwm w);
    
    // values of encoders and hall sensors can be read if(true == initialised(h))
    result_t getencoder(embot::hw::MOTOR h, Position &position);
    result_t gethallcounter(embot::hw::MOTOR h, Position &position);            
    result_t gethallstatus(embot::hw::MOTOR h, HallStatus &hs);
          
    // imposes a callback on end of measurement of the currents
    result_t setCallbackOnCurrents(MOTOR h, fpOnCurrents callback, void *owner);
    
}}} // namespace embot { namespace hw { namespace motor {
    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


