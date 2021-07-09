
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "amcbldc_codetotest.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"
#include "embot_hw_sys.h"
#include <array>

//#define TEST_FOC
#define TEST_MOTOR

#if defined(TEST_FOC)
// foc code
#include "Current_FOC.h"               // Model's header file
#include "rtwtypes.h"

#elif defined(TEST_MOTOR)

#include "embot_hw_motor.h"
#endif

// --------------------------------------------------------------------------------------------------------------------
// - code
// --------------------------------------------------------------------------------------------------------------------



    
namespace amcbldc { namespace codetotest {
    
#if defined(TEST_FOC)    
    Current_FOCModelClass Current_FOC_Obj;// Instance of model class
#endif
    
    void init() 
    {
#if defined(TEST_FOC)        
        // Initialize model
        Current_FOC_Obj.initialize();        
#elif defined(TEST_MOTOR)
        embot::core::print("amcbldc::codetotest::init(): initting embot::hw::MOTOR::one");
        embot::hw::motor::init(embot::hw::MOTOR::one, {});        
#endif        
    }
        
    void tick()
    {
#if defined(TEST_FOC)
        // Step the model
        Current_FOC_Obj.step();
        
#elif defined(TEST_MOTOR)

#undef TEST_MOTOR_BUT_DONTMOVEIT
        
        static embot::hw::motor::Pwm pwm = 0;
        static uint8_t cnt = 0;
        constexpr embot::hw::motor::Pwm step {500};
        constexpr embot::core::relTime period {1000*embot::core::time1millisec};
        static embot::core::Time prevcommutation = 0;
        
        embot::core::Time now = embot::core::now();
        
        embot::hw::motor::Position hall {0};       
        embot::hw::motor::Position enco {0};            
        embot::hw::motor::getencoder(embot::hw::MOTOR::one, enco); 
        embot::hw::motor::gethallcounter(embot::hw::MOTOR::one, hall); 
        constexpr uint8_t npoles = 7;
        constexpr uint8_t nhall = 6;
        constexpr float resolutionhalltick = 360.0 / (npoles*nhall);        
        float deghall = hall*resolutionhalltick;        
        
        embot::core::print("@ " + embot::core::TimeFormatter(now).to_string() + ": [encmod4, hallcnt, halldeg] = [" + std::to_string(enco) + ", " + std::to_string(hall) + ", " + std::to_string(deghall) + "]");

#if defined(TEST_MOTOR_BUT_DONTMOVEIT)
#else        
        if((now - prevcommutation) >= period)
        {
            prevcommutation = now;
            embot::core::print("@ " + embot::core::TimeFormatter(now).to_string() + ": embot::hw::motor::setpwm(embot::hw::MOTOR::one, " + std::to_string(pwm) + ")");
            
            embot::hw::motor::setpwm(embot::hw::MOTOR::one, pwm);
            
            switch(cnt)
            {
                case 0:     {   cnt = 1;    pwm = step;    } break;
                case 1:     {   cnt = 2;    pwm = 0;       } break;
                case 2:     {   cnt = 3;    pwm = -step;   } break;
                case 4: 
                default:    {   cnt = 0;    pwm = 0;       } break;
            }
        }
#endif        
        
#else        
        static constexpr std::array<embot::core::relTime, 5> usec = 
        {   
            100*embot::core::time1microsec, 
            200*embot::core::time1microsec, 
            300*embot::core::time1microsec, 
            400*embot::core::time1microsec, 
            500*embot::core::time1microsec
        }; 
        static uint8_t index {0};
        
        index = (index+1) % usec.size();
        embot::hw::sys::delay(usec[index]);        
#endif        
    }
    
} } // namespace amcbldc { namespace codetotest {




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


