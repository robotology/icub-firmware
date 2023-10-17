
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
    
    
    
    // declaration of MspInit() / MspDeInit() used by motor. it is called by emebot::hw::timer   
    void TIM_base_MspInit(TIM_HandleTypeDef* htim_base);
    void TIM_base_MspDeInit(TIM_HandleTypeDef* htim_base);

    // they are required by motorhal 
    // adm
    extern ADC_HandleTypeDef hADC1;
    extern ADC_HandleTypeDef hADC2;  
    // encoder
    extern TIM_HandleTypeDef hTIM5;   
    // hall   
    extern TIM_HandleTypeDef hTIM4;    
    // pwm
    extern TIM_HandleTypeDef hTIM1;
    
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
    
    // 20000 is 20.0 us
    // 15000 is 15.0 us
    // 12500 is 12.5 us
    // 12190 is ... as amcbldc
    constexpr PWMvalues PWMvals20us {20000};
    constexpr PWMvalues PWMvals15us {15000};
    constexpr PWMvalues PWMvals12dot5us {12500};
    constexpr PWMvalues PWMvals12dot190us {12190}; // 36.57
    
    constexpr PWMvalues PWMvals {PWMvals20us};
    //constexpr PWMvalues PWMvals {PWMvals15us};

    
    
}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



