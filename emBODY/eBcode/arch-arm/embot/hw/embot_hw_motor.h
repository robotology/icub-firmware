

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
        
 
    // whatever else we shall need
    
    // Position and Pwm are to be defined yet.
    using Position = std::int32_t;
    using Pwm = std::int32_t;
    
    result_t getencoder(embot::hw::MOTOR h, Position &position);
    result_t gethallcounter(embot::hw::MOTOR h, Position &position);
    
    result_t gethallstatus(embot::hw::MOTOR h, uint8_t &hs);
    result_t setpwmUVW(MOTOR h, Pwm u, Pwm v, Pwm w);
    result_t setADCcallback(MOTOR h, void (*fn_cb)(int16_t[3], void*, void*), void* rtu, void* rty);
    result_t motorEnable(MOTOR h);
    result_t motorDisable(MOTOR h);
    
}}} // namespace embot { namespace hw { namespace motor {
    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


