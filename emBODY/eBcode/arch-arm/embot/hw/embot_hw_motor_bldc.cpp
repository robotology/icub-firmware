
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_motor_bsp.h"

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


#if !defined(EMBOT_ENABLE_hw_motor)

namespace embot::hw::motor::bldc {

    bool supported(MOTOR h) { return false; }
    bool initialised(MOTOR h) { return false; }
    
    bool init(MOTOR h, const Config &config) { return false; } 
    bool configure(embot::hw::MOTOR h, const Config &config) { return false; }   
    bool enable(MOTOR h, bool on) { return false; }    
    bool enabled(MOTOR h) { return false; } 
    bool fault(MOTOR h, bool on) { return false; }        
    bool faulted(MOTOR h) { return false; }    
    bool set(MOTOR m, const OnCurrents &oncurrents) { return false; }    
    HallStatus hall(MOTOR m) { return 0; }     
    Angle angle(MOTOR m, Encoder enc) { return false; }     
    bool set(MOTOR m, const PWMs &pwms) { return false; }     
    Voltage powersupply(MOTOR m) { return false; } 
    
} // namespace embot::hw::motor::bldc {

#elif defined(EMBOT_ENABLE_hw_motor)

#if defined(STM32HAL_BOARD_AMC2C) || defined(STM32HAL_BOARD_AMC1CM7) || defined(STM32HAL_BOARD_AMC2CM4)
    #include "motorhal.h"  
#elif defined(STM32HAL_BOARD_AMCFOC_1CM7)
    #include "embot_hw_motor_adc.h"  
    #include "embot_hw_motor_enc.h"  
    #include "embot_hw_motor_hall.h"  
    #include "embot_hw_motor_pwm.h"  
#endif    




namespace embot::hw::motor::bldc {
   
    bool supported(MOTOR m)
    {
        return embot::hw::motor::supported(m);
    }
    
    bool initialised(MOTOR m)
    {
        return embot::hw::motor::initialised(m);
    }    
    
    
    bool init(MOTOR m, const Config &cfg)
    { 
        embot::hw::motor::Config c { 
            cfg.enc_resolution,
            cfg.pwm_num_polar_couples,
            cfg.pwm_hall_offset,         
            cfg.has_quad_enc,
            cfg.pwm_has_hall_sens,
            cfg.pwm_swapBC
        };
        
        return (resOK == embot::hw::motor::init(m, c)) ? true : false;
    }
  
    
    bool configure(MOTOR m, const Config&cfg)
    {
        embot::hw::motor::Config c { 
            cfg.enc_resolution,
            cfg.pwm_num_polar_couples,
            cfg.pwm_hall_offset,         
            cfg.has_quad_enc,
            cfg.pwm_has_hall_sens,
            cfg.pwm_swapBC
        };
        
        return (resOK == embot::hw::motor::configure(m, c)) ? true : false;
    }     
 
       
    bool enable(MOTOR m, bool on)
    {
        return (resOK == embot::hw::motor::enable(m, on)) ? true : false;                     
    }    

    bool enabled(MOTOR m)
    {
        return embot::hw::motor::enabled(m);
    }
        
    
    bool fault(MOTOR m, bool on)
    {        
        return (resOK == embot::hw::motor::fault(m, on)) ? true : false;
    } 

    bool faulted(MOTOR m)
    {
        return embot::hw::motor::faulted(m);
    }
    
    bool set(MOTOR m, const embot::hw::motor::bldc::OnCurrents &oncurrents)
    {
        bool r { false};       
        r = embot::hw::motor::adc::set(m, oncurrents);
        return r;        
    }

    HallStatus hall(MOTOR m)
    {
        HallStatus ha {0};
        embot::hw::motor::gethallstatus(m, ha);
        return ha;
    }

    bool set(MOTOR m, const PWM3 &pwm)
    {
        return (resOK == embot::hw::motor::setPWM(m, {pwm.u, pwm.v, pwm.w})) ? true : false;
    }
  
    
    Angle angle(MOTOR m, Encoder enc)
    {
        Angle a = (enc == Encoder::hall) ? 0 : 1;
        
        //return  embot::hw::motor::hall::angle(h);
        
        return a;        
    }
    
    
    
       
//    result_t setCallbackOnCurrents(MOTOR h, fpOnCurrents callback, void *owner)
//    {
//        return s_hw_setCallbackOnCurrents(h, callback, owner);
//    }
//    
//    bool set1(MOTOR h, OnPhaseCurrents onphasecurrents, void *owner)
//    {
//         return s_hw_setOnPhaseCurrents(h, onphasecurrents, owner);
//    }
//    
    Voltage powersupply(MOTOR m)
    {
        return embot::hw::motor::getVIN();
    }

    
// in here is the part for low level hw of the boards (amc2c or amcbldc)
      
    
#if defined(STM32HAL_BOARD_AMC2C) || defined(STM32HAL_BOARD_AMC1CM7) || defined(STM32HAL_BOARD_AMC2CM4) || defined(STM32HAL_BOARD_AMCFOC_1CM7)
    
   

//    Position s_hw_getencoder(MOTOR h)
//    {
//        Position p {0};
//        if(true == embot::hw::motor::enc::isstarted())
//        {
//           p = embot::hw::motor::enc::getvalue();
//        }
//        else if(true == embot::hw::motor::hall::isstarted())
//        {
//           p = embot::hw::motor::hall::getangle();
//        }
//        return p;
//    }


        
//    HallStatus s_hw_gethallstatus(MOTOR h)
//    {       
//        return  embot::hw::motor::hall::getstatus();
//    }

     

//    result_t s_hw_setpwmUVWperc(MOTOR h, const PWMperc &p)
//    {   
//        embot::hw::motor::pwm::setperc(p.a, p.b, p.c);        
//        return resOK;
//    }  
    
   
   
    
//    bool s_hw_setOnPhaseCurrents(MOTOR h, OnPhaseCurrents onphasecurrents,  void *owner)
//    {
//        if((nullptr == onphasecurrents))
//        {
//            return false;
//        }      
//        
//        //embot::hw::motor::adc::OnMotorCurrents cbk {reinterpret_cast<embot::hw::motor::adc::OnMotorCurrents::Action>(onphasecurrents), owner};
//        embot::hw::motor::adc::OnMotorPhaseCurrents cbk1 {onphasecurrents, owner};
//        embot::hw::motor::adc::set1(h, cbk1);
//        return true;           
//    }
      

#elif defined(STM32HAL_BOARD_AMCBLDC)
    
    #error add implentation for amcbldc  
    
#else
    #error define a board
#endif    
    
    
 
} // namespace embot::hw::motor::bldc {



#endif // #elif defined(EMBOT_ENABLE_hw_motor)
 



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

