

/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BLDC_H_
#define __EMBOT_HW_MOTOR_BLDC_H_

#include "embot_core.h"
#include "embot_hw.h"
#include "embot_hw_types.h"



namespace embot::hw::motor::bldc {
        
    constexpr uint8_t MAXnumber {2};
    
    using Current = float; // in [A]
    struct Currents 
    {
        Current u {0};
        Current v {0};
        Current w {0};
        constexpr Currents() = default;
        constexpr Currents(float v1, float v2, float v3) : u(v1), v(v2), w(v3) {}            
    };
    
    
    struct PWM3 
    {
        using Percentage = float; // in range [0.0, 100.0]
        Percentage u {0.0};
        Percentage v {0.0};
        Percentage w {0.0};
        constexpr PWM3() = default;
        constexpr PWM3(float v1, float v2, float v3) : u(v1), v(v2), w(v3) {}
        constexpr bool isvalid() const { 
            bool positive = (u>=0) && (v>=0) && (w>=0); bool less100 = (u<=100) && (v<=100) && (w<=100);
            return positive && less100;
        }
    };  

    struct OnCurrents
    {
        using Action = void (*)(MOTOR m, const Currents * const currs, void *owner);
        
        MOTOR motor {MOTOR::none};        
        Action action {nullptr};
        void *owner {nullptr};
        
        constexpr OnCurrents() = default;
        constexpr OnCurrents(MOTOR m, Action a, void *o) : motor(m), action(a), owner(o) {}
            
        void load(MOTOR m, Action a, void *o) { motor = m; action = a; owner = o; } 
       // void load(const OnCurrents &obj) { motor = obj.motor; action = obj.action; owner = obj.owner; }    
        bool isvalid() const { return (nullptr != action); }
        void execute(const Currents * const curs) const
        { 
            if(isvalid()) { action(motor, curs, owner); } 
        }          
    };

    using Voltage = float; // in range [V]  
    
    // HallStatus uses bits 2, 1, 0 to keep status of {HALL1, HALL2, HALL3}
    using HallStatus = std::uint8_t;
    
    using Angle = float; // in [degrees]
    
    enum class AngleType : uint8_t { hall_electrical = 0, hall_mechanical = 2, quadenc = 3 };
    
    struct Config
    {
        int16_t enc_resolution {0}; 
        uint8_t pwm_num_polar_couples {0}; 
        uint16_t pwm_hall_offset {0};         
        bool has_quad_enc {false};
        bool pwm_has_hall_sens {false};
        bool pwm_swapBC {false};
        
        constexpr Config() = default;         
        constexpr Config(int16_t er, uint8_t pc, uint16_t ho, bool hqe, bool hhs, bool spbc) 
            : enc_resolution(er), pwm_num_polar_couples(pc), pwm_hall_offset(ho), 
              has_quad_enc(hqe), pwm_has_hall_sens(hhs), pwm_swapBC(spbc)  {} 
        constexpr bool isvalid() const { 
            bool KO = (0 == enc_resolution) && (0 == pwm_num_polar_couples) && (0 == pwm_hall_offset);
            return !KO; 
        }
    };
    
    // functions
    
    std::string to_string(embot::hw::MOTOR id);

    bool supported(embot::hw::MOTOR m);     
    bool initialised(embot::hw::MOTOR m);
    
    bool deinit(MOTOR m);
    
    // it prepares the low level HW and if config.isvalid() it also configures the HW w/ its content
    bool init(embot::hw::MOTOR m, const Config &config); 
    

    
    // is to be called after init() and if config.isvalid() it configures the HW w/ its content.
    bool configure(embot::hw::MOTOR m, const Config &config);     

    // it enables on/off the motor. this function is effective only if initialised(h) is true  
    bool enable(MOTOR m, bool on);
    bool enabled(MOTOR m); 

    // it faults on/off the motor. this function is effective also before the call of init() as long as supported(h) is true                  
    bool fault(MOTOR m, bool on);
    bool faulted(MOTOR m); 
        
    // it imposes the callback of reception of currents for a given motor.
    // it typically gets the motor position and applies PWMs properly rotated
    bool set(MOTOR m, const embot::hw::motor::bldc::OnCurrents &oncurrents);

    HallStatus hall(MOTOR m);
    
    Angle angle(MOTOR m, AngleType type);
    
    bool set(MOTOR m, const PWM3 &pwm);
    
    Voltage powersupply(MOTOR m);
    
} // namespace embot::hw::motor::bldc {
    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


