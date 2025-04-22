
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BLDC_BSP_AMCFOC_1CM7_H_
#define __EMBOT_HW_MOTOR_BLDC_BSP_AMCFOC_1CM7_H_

#include "embot_hw_bsp.h"
#include "embot_hw_motor_bldc_bsp.h"
#include "embot_hw_timer_bsp.h"

namespace embot::hw::motor::bldc::bsp::amcfoc::cm7 {
    
    // this is the one required by the bsp. it does not init the timer TIM5 for the encoder
    void Init_MOTOR(embot::hw::MOTOR m);     
    
//    // the timer for the encoder must be initted later
//    void DeInit_TIM5();
//    void Init_TIM5(int16_t _resolution, uint8_t _num_polar_couples);
//    
//    
    
    extern TIM_HandleTypeDef &htimHALL1;
    extern TIM_HandleTypeDef htim4;
    
    extern TIM_HandleTypeDef &htimHALL2;
    extern TIM_HandleTypeDef htim3;
    
    
    extern TIM_HandleTypeDef &htimMOT1;
    extern TIM_HandleTypeDef &htimMOT2;    
    
    extern TIM_HandleTypeDef &htimTriggerOfadcOTHERS;
    
    extern ADC_HandleTypeDef &hadcMOT1;     
    extern ADC_HandleTypeDef &hadcMOT2;  
    extern ADC_HandleTypeDef &hadcOTHERS; 
    
    
//    extern TIM_HandleTypeDef htim5;
//    extern TIM_HandleTypeDef htim2;
    extern TIM_HandleTypeDef &hTimEnc1;        // qenc
    extern TIM_HandleTypeDef &hTimEnc2;        // qenc
    
    
    
    struct PWMvalues
    {   // the default is for pwm @ 10.240 us ....

        constexpr PWMvalues() = default;
        constexpr PWMvalues(uint32_t pwmnanosec) : period(pwmnanosec/10) 
        { 
        };
        
        constexpr uint32_t valueofTIMperiod() const 
        {
           return period; 
        }
        
        constexpr uint32_t valueofLOWsampleoffset() const
        {
            return sampleoffset;
        }
 
        constexpr uint32_t valueofHIGHsampleoffset() const
        {
            return period-sampleoffset;
        }
        
    private:
        static constexpr uint32_t sampleoffset {2};// {2} ; //{ 24 };     
        static constexpr uint32_t timclockspeed {200*1000*1000};         
        uint32_t period { 1024 }; 
        
    };
    
    // the amcbldc uses 82 kHz
    constexpr PWMvalues pwm020000Hz {50000};    // pwm @  20.000 kHz [pwm 50.000us, foc150.000us]
    constexpr PWMvalues pwm025000Hz {40000};    // pwm @  25.000 kHz [pwm 40.000us, foc120.000us]
    constexpr PWMvalues pwm050000Hz {20000};    // pwm @  50.000 kHz [pwm 20.000us, foc 60.000us]
    constexpr PWMvalues pwm066666Hz {15000};    // pwm @  66.666 kHz [pwm 15.000us, foc 45.000us] 
    constexpr PWMvalues pwm070000Hz {14285};    // pwm @  70.000 kHz [pwm 14.285us, foc 42.857us]
    constexpr PWMvalues pwm075000Hz {13333};    // pwm @  75.000 kHz [pwm 13.333us, foc 40.000us]
    constexpr PWMvalues pwm080000Hz {12500};    // pwm @  80.000 kHz [pwm 12.500us, foc 37.500us]
    constexpr PWMvalues PWM082000Hz {12195};    // pwm @  82.000 kHz [pwm 12.195us, foc 36.585us]
    constexpr PWMvalues PWM100000Hz {10000};    // pwm @ 100.000 kHz [pwm 10.000us, foc 30.000us]
    
    constexpr PWMvalues pwmZINI {10240};        // pwm @  97.656 kHz [pwm 10.240us, foc 30.720us]
    
    constexpr PWMvalues PWMvals {pwm066666Hz}; // {pwm066666Hz}; //{pwmZINI}; // {pwm020000Hz}
        
//    constexpr PWMvalues PWMvals {pwm066666Hz};
//    constexpr PWMvalues PWMvals {PWM100000Hz};
    
    
    constexpr uint8_t QEncoder1Mode TIM_ENCODERMODE_TI12;
    constexpr uint8_t QEncoder2Mode TIM_ENCODERMODE_TI12;
    
}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



