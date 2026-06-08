
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_bldc_pwm.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if !defined(EMBOT_ENABLE_hw_motor_bldc_pwm)

#warning EMBOT_ENABLE_hw_motor_bldc_pwm is undefined, so we use dummy implementation 

namespace embot::hw::motor::bldc::pwm {
    
    bool supported(embot::hw::MOTOR m) { return false; }
    bool initialised(embot::hw::MOTOR m) { return false; }
    void init(embot::hw::MOTOR m, const Configuration &config) { }
    void deinit(embot::hw::MOTOR m) { }
    void enable(embot::hw::MOTOR m, bool on)  { }
    void setperc(embot::hw::MOTOR m, float u, float v, float w)  { }
    void set(embot::hw::MOTOR m, const PWM3 &pwm) {}
}

#elif defined(EMBOT_ENABLE_hw_motor_bldc_pwm)


#include "embot_hw_motor_bldc_pwm_bsp.h"

namespace embot::hw::motor::bldc::pwm {
    
    struct pwm_Internals
    {        
        struct Item
        {
            bool started {false};
            bool calibrating {false};
            Configuration config {};   
                
            Item() = default;
        };
        
        std::array<Item, embot::hw::motor::bldc::MAXnumber> _items {};
            
        pwm_Internals() = default;   
    };

    pwm_Internals _pwm_internals {};
     
    // we init or deinit all motors together ???     
    bool initted {false};

    void init(embot::hw::MOTOR m, const Configuration &config)
    {
        _pwm_internals._items[embot::core::tointegral(m)].config = config;
        _pwm_internals._items[embot::core::tointegral(m)].calibrating = false;

        if(false == initted)
        {
            embot::hw::motor::bldc::pwm::bsp::getBSP().init(m);
            initted = true;
        }
        
        _pwm_internals._items[embot::core::tointegral(m)].started = true;
        
        set(m, {0, 0, 0});    
    }

    void deinit(embot::hw::MOTOR m)
    {
        embot::hw::motor::bldc::pwm::bsp::getBSP().deinit(m);
        initted = false;
        _pwm_internals._items[embot::core::tointegral(m)].started = false; 
    }

    void set(embot::hw::MOTOR m, const PWM3 &pwm)
    {
        if(true == _pwm_internals._items[embot::core::tointegral(m)].calibrating)
        {        
            embot::hw::motor::bldc::pwm::bsp::getBSP().set(m, {0, 0, 0});
        }
        else
        {
            embot::hw::motor::bldc::pwm::bsp::getBSP().set(m, pwm);  
        }           
    }

    void setperc(embot::hw::MOTOR m, float u, float v, float w)
    {
        set(m, {u, v, w});
    }
        
    extern void enable(embot::hw::MOTOR m, bool on)
    {
        embot::hw::motor::bldc::pwm::bsp::getBSP().enable(m, on);
    }

} // namespace embot::hw::motor::bldc::pwm {


#endif // #elif defined(EMBOT_ENABLE_hw_motor_bldc_pwm)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

