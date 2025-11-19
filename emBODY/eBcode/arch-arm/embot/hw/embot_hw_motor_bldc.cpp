
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_bldc.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"

#include "embot_hw_motor_bldc_bsp.h"

#include <cstring>
#include <vector>

#include "embot_core_binary.h"
#include "embot_hw_sys.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif   
  

using namespace embot::hw;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

std::string embot::hw::motor::bldc::to_string(embot::hw::MOTOR id)
{
    constexpr std::array<const char *, embot::core::tointegral(embot::hw::MOTOR::maxnumberof)> MOTOR_map =
    { 
        "MOTOR::one", "MOTOR::two", "MOTOR::three", "MOTOR::four"
    };
    uint8_t pos = embot::core::tointegral(id);
    return (pos < MOTOR_map.size()) ? MOTOR_map[pos] : "MOTOR::none";    
}


#if !defined(EMBOT_ENABLE_hw_motor_bldc)

namespace embot::hw::motor::bldc {

    bool supported(MOTOR m) { return false; }
    bool initialised(MOTOR m) { return false; }
    
    bool init(MOTOR m, const Config &config) { return false; } 
    bool deinit(MOTOR m) { return false; } 
    bool configure(embot::hw::MOTOR m, const Config &config) { return false; }   
    bool enable(MOTOR m, bool on) { return false; }    
    bool enabled(MOTOR m) { return false; } 
    bool fault(MOTOR m, bool on) { return false; }        
    bool faulted(MOTOR m) { return false; }    
    bool set(MOTOR m, const OnCurrents &oncurrents) { return false; }    
    HallStatus hall(MOTOR m) { return 0; }     
    Angle angle(MOTOR m, AngleType type) { return 0.0f; }     
    bool set(MOTOR m, const PWM3 &pwm) { return false; }     
    Voltage powersupply(MOTOR m) { return 0.0f; } 
    
} // namespace embot::hw::motor::bldc {

#elif defined(EMBOT_ENABLE_hw_motor_bldc)

#if defined(STM32HAL_BOARD_AMC2C)
    #include "motorhal.h"  
#elif defined(STM32HAL_BOARD_AMCFOC_1CM7) || defined(STM32HAL_BOARD_AMCFOC_2CM4)
    #include "embot_hw_motor_adc.h"  
    #include "embot_hw_motor_enc.h"  
    #include "embot_hw_motor_hall.h"  
    #include "embot_hw_motor_pwm.h"  
#endif    




namespace embot::hw::motor::bldc {
        
    
    struct Data
    {
        Config config {};        
    };
        
    struct PrivateData
    {
        // masks
        uint32_t initialisedmask {0};
        uint32_t enabledmask {0}; 
        uint32_t faultedmask {0};   
        // data        
        std::array<Data, embot::hw::motor::bldc::MAXnumber> data;
        PrivateData() = default;
    };
    
    
    PrivateData s_privatedata {};
    
    
    bool supported(MOTOR m)
    {
        return embot::hw::motor::bldc::bsp::getBSP().supported(m);
    }
    
    bool initialised(MOTOR m)
    {
        return embot::core::binary::bit::check(s_privatedata.initialisedmask, embot::core::tointegral(m));
    }    
    
    bool enabled(MOTOR m)
    {
        return embot::core::binary::bit::check(s_privatedata.enabledmask, embot::core::tointegral(m));
    }
        
    bool faulted(MOTOR m)
    {
        return embot::core::binary::bit::check(s_privatedata.faultedmask, embot::core::tointegral(m));
    }
  
    
    bool deinit(MOTOR m)
    {
        bool r {false};
        
        if(false == supported(m))
        {
            return r;
        }
        
        if(false == initialised(m))
        {
            return r;
        }

        embot::hw::motor::bldc::bsp::getBSP().deinit(m);
        
        s_privatedata.data[embot::core::tointegral(m)].config = {};
        embot::core::binary::bit::clear(s_privatedata.initialisedmask, embot::core::tointegral(m));

        r = true;   

        return r;                      
    }
    
    bool init(MOTOR m, const Config &cfg)
    { 
        bool r {false};
        
        if(false == supported(m))
        {
            return r;
        }
        
        if(true == initialised(m))
        {
            deinit(m);
        }
                        
        // prepare the hw environment. something such as prepare GPIOs, TIMs, ADC, link them together, etc.
        // and something else that embot::hw::motor::init() does inside s_hw_init()
        embot::hw::motor::bldc::bsp::getBSP().init(m);                           
        
        // load config etc
        s_privatedata.data[embot::core::tointegral(m)].config = cfg;
      
        
        // placed in here so that we can call configure()
        embot::core::binary::bit::set(s_privatedata.initialisedmask, embot::core::tointegral(m));
        
        // for now it is ok
        r = true;
        
        // make sure that the ... way we use the HW peripherals is with the correct parameters
        if(true == cfg.isvalid())
        {   // ok i know there is a double check vs validity of cfg
            configure(m, cfg);
        } 
        
        return r;        
    }
  
    
    bool configure(MOTOR m, const Config&cfg)
    {
        bool r {false};
        
        if(false == initialised(m))
        {
            return r;
        }
        
        if(false == cfg.isvalid())
        {
            return r;
        }
        
        s_privatedata.data[embot::core::tointegral(m)].config = cfg;    
        
        r = embot::hw::motor::bldc::bsp::getBSP().configure(m, cfg);
        
        return r;        
    }     
 
       
    bool enable(MOTOR m, bool on)
    {
        bool r {false};
        
        if(false == initialised(m))
        {
            return r;
        }
        
        // if we are faulted we cannot enable, so i force on to false    
        if(true == faulted(m))
        {
            on = false;
        }
        
        // in any case at first we set pwm = {0, 0, 0};
        
        constexpr PWM3 pwm3zero {0.0f, 0.0f, 0.0f};
        set(m, pwm3zero);
               
        if(true == on)
        { 
            embot::core::binary::bit::set(s_privatedata.enabledmask, embot::core::tointegral(m));
            r = embot::hw::motor::bldc::bsp::getBSP().enable(m, true);
        }
        else
        {
            embot::core::binary::bit::clear(s_privatedata.enabledmask, embot::core::tointegral(m));
            r = embot::hw::motor::bldc::bsp::getBSP().enable(m, false);           
        } 

        return r;
    }    

            
    bool fault(MOTOR m, bool on)
    { 
        if(false == initialised(m))
        {
            return false;
        }
        
        // we always must be able to set the fault on / off, also when we are not initialised yet.
        // because the fault(h, true/false) can be called also before init(h, {});
        
        if(true == on)
        {
            embot::core::binary::bit::set(s_privatedata.faultedmask, embot::core::tointegral(m));
            // we disable the action of the motor
//            constexpr PWM3 pwm3zero {0.0f, 0.0f, 0.0f};
//            set(m, pwm3zero);
//            // and we also disable ...
            enable(m, false); // enable(m, false) also sets pwm = 0
            // and we also call the bsp fault
            embot::hw::motor::bldc::bsp::getBSP().fault(m, true);
        }
        else
        {
            embot::core::binary::bit::clear(s_privatedata.faultedmask, embot::core::tointegral(m));  
            // we dont enable the action of the motor. that must be done explicitly
        }

        return true;
    } 

    
    bool set(MOTOR m, const embot::hw::motor::bldc::OnCurrents &oncurrents)
    {
        if(false == initialised(m))
        {
            return false;
        }
        
        bool r { false};       
        r = embot::hw::motor::bldc::bsp::getBSP().set(m, oncurrents);
        return r;        
    }

    HallStatus hall(MOTOR m)
    {        
        if(false == initialised(m))
        {
            return 0;
        } 
        
        return embot::hw::motor::bldc::bsp::getBSP().hall(m);
    }

    bool set(MOTOR m, const PWM3 &pwm)
    {
        if(false == initialised(m))
        {
            return false;
        } 
        
        return embot::hw::motor::bldc::bsp::getBSP().set(m, pwm);
    }
  
    
    Angle angle(MOTOR m, AngleType type)
    {
        if(false == initialised(m))
        {
            return 0.0f;
        }         
        
        return embot::hw::motor::bldc::bsp::getBSP().angle(m, type);     
    }
    
     
    Voltage powersupply(MOTOR m)
    {
        if(false == initialised(m))
        {
            return 0.0f;
        }
        
        return embot::hw::motor::bldc::bsp::getBSP().powersupply(m);        
    }
   
 
} // namespace embot::hw::motor::bldc {



#endif // #elif defined(EMBOT_ENABLE_hw_motor_bldc)
 



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

