
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
    

    
}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



