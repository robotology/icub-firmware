
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BSP_AMC2C_H_
#define __EMBOT_HW_MOTOR_BSP_AMC2C_H_

#include "embot_hw_bsp.h"
#include "embot_hw_motor_bsp.h"
#include "embot_hw_timer_bsp.h"

namespace embot::hw::motor::bsp::amc2c {
    
    // this is the one required by the bsp. it does not init the timer TIM5 for the encoder
    void Init_MOTOR(embot::hw::MOTOR h); 
    
    // the timer for the encoder must be initted later
    void DeInit_TIM5();
    void Init_TIM5(int16_t _resolution, uint8_t _num_polar_couples);
    
    // this is the one required to test the motorhal. it inits everything
    void Init_MOTORHAL_testmode(embot::hw::MOTOR h);
    
    
    
    // declaration of MspInit() / MspDeInit() used by motor. it is called by embot::hw::timer   
    void TIM_base_MspInit(TIM_HandleTypeDef* htim_base);
    void TIM_base_MspDeInit(TIM_HandleTypeDef* htim_base);

    // they are required by motorhal 
    // adm
    extern ADC_HandleTypeDef hADC1;
    extern ADC_HandleTypeDef hADC2;
    extern ADC_HandleTypeDef hADC3;
    // encoder
    extern TIM_HandleTypeDef hTIM5;   
    // hall   
    extern TIM_HandleTypeDef hTIM4;    
    // pwm
    extern TIM_HandleTypeDef hTIM1;
    // vcc
    extern TIM_HandleTypeDef hTIM6;
    
    struct PWMvalues
    {   // the default is for pwm @ 20 us (

        uint32_t TIM1_period { 2000 };                                                            // 2000 {1219};
        static constexpr float prop { 0.7345 };
        uint32_t TIM1_ocPulse { static_cast<uint32_t>(prop*static_cast<float>(TIM1_period)) };    // 1469 {895} // actually a proprotion of TIM1_period       
        uint32_t value100perc {TIM1_period-1};
        constexpr PWMvalues() = default;
        constexpr PWMvalues(uint32_t pwmnanosec) : TIM1_period(pwmnanosec/10) 
        { 
            TIM1_ocPulse = static_cast<uint32_t>(prop*static_cast<float>(TIM1_period));
            value100perc = TIM1_period-1;
        };
    };
    
    // the amcbldc uses 82 kHz
    constexpr PWMvalues pwm50000Hz {20000};     // pwm @ 50.000 kHz [pwm 20.000us, foc 60.000us]
    constexpr PWMvalues pwm66666Hz {15000};     // pwm @ 66.666 kHz [pwm 15.000us, foc 45.000us] 
    constexpr PWMvalues pwm70000Hz {14285};     // pwm @ 70.000 kHz [pwm 14.285us, foc 42.857us]
    constexpr PWMvalues pwm75000Hz {13333};     // pwm @ 75.000 kHz [pwm 13.333us, foc 40.000us]
    constexpr PWMvalues pwm80000Hz {12500};     // pwm @ 80.000 kHz [pwm 12.500us, foc 37.500us]
    constexpr PWMvalues PWM82000Hz {12195};     // pwm @ 82.000 kHz [pwm 12.195us, foc 36.585us]
    
    constexpr PWMvalues PWMvals {pwm66666Hz};
    
    
}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



