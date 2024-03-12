

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
    
    // we manage: pwm, current, position, hall status. so far we use raw integer values  w/out a specific measurement unit
    using Position = std::int32_t;
    using Pwm = std::int32_t;
    
    // HallStatus uses bits 2, 1, 0 to keep status of {HALL1, HALL2, HALL3}
    using HallStatus = std::uint8_t;
    
    // so far, we keep int32_t as it is more general. even if lower levels may propagate them as int16_t
    // u, v, w are expressed as mA
    struct Currents
    {
        int32_t u {0};
        int32_t v {0};
        int32_t w {0};
        constexpr Currents() = default;
        constexpr Currents(int32_t c0, int32_t c1, int32_t c2) : u(c0), v(c1), w(c2) {}
    };  
        
    using fpOnCurrents = void (*)(void *owner, const Currents * const currents);
    
    // so far Config is an aggregate of the configuration values we need. 
    // we may need to add more along the way, so better having them organised in a struct    
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
           
    std::string to_string(embot::hw::MOTOR id);
            
    bool supported(embot::hw::MOTOR h);   
    bool faulted(MOTOR h);    
    bool initialised(embot::hw::MOTOR h);
    bool enabled(MOTOR h);  
    
    // it prepares the low level HW and if config.isvalid() it also configure the HW w/ its content
    result_t init(embot::hw::MOTOR h, const Config &config); 
    
    // is to be called after init() and if config.isvalid() it configures the HW w/ its content.
    result_t configure(embot::hw::MOTOR h, const Config &config); 
    
    // deprecated in favour for configure()
    result_t config(MOTOR h, bool has_quad_enc, int16_t enc_resolution, 
                    uint8_t pwm_num_polar_couples, bool pwm_has_hall_sens,
                    bool pwm_swapBC, uint16_t pwm_hall_offset);
    
    // this function is effective only if initialised(h) is true  
    result_t enable(MOTOR h, bool on);
       
    // this function is effective also before the call of init() as long as supported(h) is true                  
    result_t fault(MOTOR h, bool on);
                           
    // the pwm can be effectively applied only if(true == enabled(h)), so it also must be: (false == faulted(h))
    result_t setpwm(MOTOR h, Pwm u, Pwm v, Pwm w);
    
    // values of encoders and hall sensors can be read if(true == initialised(h))
    result_t getencoder(embot::hw::MOTOR h, Position &position);         
    result_t gethallstatus(embot::hw::MOTOR h, HallStatus &hs);
     
    // not used so far, so we keep it out                     
    // result_t gethallcounter(embot::hw::MOTOR h, Position &position);   
          
    // imposes a callback on end of measurement of the currents
    result_t setCallbackOnCurrents(MOTOR h, fpOnCurrents callback, void *owner);
     
    struct PWMperc 
    {
        float a {0.0};
        float b {0.0};
        float c {0.0};
        constexpr PWMperc() = default;
        constexpr PWMperc(float u, float v, float w) : a(u), b(v), c(w) {}
    };        
    result_t setPWM(MOTOR h, const PWMperc &p);
    
    // Return Vin in Volts
    float getVIN();

    // Return Cin in Ampere (not yet implemented)
    float getCIN();
    
}}} // namespace embot { namespace hw { namespace motor {
    

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


