
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BLDC_BSP_AMCFOC_H_
#define __EMBOT_HW_MOTOR_BLDC_BSP_AMCFOC_H_

#include "embot_hw_bsp.h"
#include "embot_hw_motor_bldc_bsp.h"
#include "embot_hw_timer_bsp.h"

// macros that shape code for motor::bldc

//#define MOTOR_BLDC_debug_PWMasZINIcode
//#define MOTOR_BLDC_debug_PwmInit_USE_NEWCODE

#if defined(MOTOR_BLDC_debug_PWMasZINIcode)
    #warning MOTOR_BLDC_debug_PWMasZINIcode is defined
    #undef MOTOR_BLDC_debug_PwmInit_USE_NEWCODE
    #undef MOTOR_BLDC_debug_PwmInit_USE_NEWCODE_extraNEW

#endif

#if defined(MOTOR_BLDC_debug_PwmInit_USE_NEWCODE)
    #warning MOTOR_BLDC_debug_PwmInit_USE_NEWCODE is defined
    #undef MOTOR_BLDC_debug_PWMasZINIcode
    // we may or we may not define it
    //#define MOTOR_BLDC_debug_PwmInit_USE_NEWCODE_extraNEW
#endif

namespace embot::hw::motor::bldc::bsp::amcfoc {
    
    // this is the one required by the bsp
    void Init_MOTOR(embot::hw::MOTOR m);     
    
    extern TIM_HandleTypeDef &htimHALL1;
    extern TIM_HandleTypeDef htim4;
    
    extern TIM_HandleTypeDef &htimHALL2;
    extern TIM_HandleTypeDef htim3;
    
    
    extern TIM_HandleTypeDef &htimMOT1;
    extern TIM_HandleTypeDef &htimMOT2;    
    
    
    extern ADC_HandleTypeDef &hadcMOT1;     
    extern ADC_HandleTypeDef &hadcMOT2;  
    extern ADC_HandleTypeDef &hadcOTHERS; 
    
    
    extern TIM_HandleTypeDef &hTimEnc1;        // qenc
    extern TIM_HandleTypeDef &hTimEnc2;        // qenc
    
    
    // this struct keeps the value of the period to be written in the ARR register
    // and also the offsets for triggeringe xternal events.
    // gzini used period = 1024 and 24 for offset
    // but what if we use a period of 1500? should we use the same time displacement?
    // so a ratio = 24/1024 = 0.0234375 aka 1500*ratio = 35.1 =~ 36?
    // we allow different values in here
    
    struct PWMvalues
    {   // the default is for pwm @ 10.240 us ....

        constexpr PWMvalues() = default;
        constexpr PWMvalues(uint32_t pwmnanosec, uint32_t off = defoffset) : period(pwmnanosec/10), offset(off) 
        { 
        };
        
        constexpr uint32_t valueofTIMperiod() const 
        {
           return period; 
        }
        
        constexpr uint32_t valueofLOWsampleoffset() const
        {
            return offset;
        }
 
        constexpr uint32_t valueofHIGHsampleoffset() const
        {
            return period-offset;
        }
        
    private:
        static constexpr uint32_t timclockspeed {200*1000*1000};
        static constexpr uint32_t defoffset {24};
        static constexpr uint32_t defperiod {1024};         
        uint32_t period { defperiod }; 
        uint32_t offset { defoffset };
        
    };
    
    // the amcbldc uses 82 kHz
    constexpr PWMvalues pwm020000Hz {50000};        // pwm @  20.000 kHz [pwm 50.000us, foc150.000us]
    constexpr PWMvalues pwm025000Hz {40000};        // pwm @  25.000 kHz [pwm 40.000us, foc120.000us]
    constexpr PWMvalues pwm050000Hz {20000};        // pwm @  50.000 kHz [pwm 20.000us, foc 60.000us]
    constexpr PWMvalues pwm066666Hz {15000, 36};    // pwm @  66.666 kHz [pwm 15.000us, foc 45.000us] 
    constexpr PWMvalues pwm070000Hz {14285};        // pwm @  70.000 kHz [pwm 14.285us, foc 42.857us]
    constexpr PWMvalues pwm075000Hz {13333};        // pwm @  75.000 kHz [pwm 13.333us, foc 40.000us]
    constexpr PWMvalues pwm080000Hz {12500};        // pwm @  80.000 kHz [pwm 12.500us, foc 37.500us]
    constexpr PWMvalues PWM082000Hz {12195};        // pwm @  82.000 kHz [pwm 12.195us, foc 36.585us]
    constexpr PWMvalues PWM100000Hz {10000};        // pwm @ 100.000 kHz [pwm 10.000us, foc 30.000us]

    constexpr PWMvalues pwmZINI     {10240, 24};    // pwm @  97.656 kHz [pwm 10.240us, foc 30.720us]

#if defined(MOTOR_BLDC_debug_PWMasZINIcode)
    constexpr PWMvalues PWMvals {pwmZINI};
#else    
    constexpr PWMvalues PWMvals {pwm066666Hz}; // {pwm066666Hz}; //{pwmZINI}; // {pwm020000Hz}
#endif
    
//    constexpr PWMvalues PWMvals {pwm066666Hz};
//    constexpr PWMvalues PWMvals {PWM100000Hz};
    
    
    constexpr auto QEncoder1Mode TIM_ENCODERMODE_TI12;
    constexpr auto QEncoder2Mode TIM_ENCODERMODE_TI12;
    
    
    
}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



