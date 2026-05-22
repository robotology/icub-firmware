
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BLDC_BSP_AMCMJ1_H_
#define __EMBOT_HW_MOTOR_BLDC_BSP_AMCMJ1_H_

#include "embot_hw_bsp_config.h"


#if !defined(EMBOT_ENABLE_hw_motor_bldc)
    // none
#elif defined(EMBOT_ENABLE_hw_motor_bldc)

#include "embot_hw_bsp.h"
#include "embot_hw_motor_bldc_bsp.h"
#include "embot_hw_motor_bldc_pwm.h"

namespace embot::hw::motor::bldc::bsp {
    constexpr size_t numberofMOTORs {1};
}


namespace embot::hw::motor::bldc::bsp::amcmj1 {
    
    // this is the one required by the bsp
    void Init_MOTOR(embot::hw::MOTOR m);     
    
    // this way of getting the handles is ... boh 
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
    
    
    // the amcbldc uses 82 kHz
    constexpr embot::hw::motor::bldc::pwm::PWMvalues pwm020000Hz {50000};        // pwm @  20.000 kHz [pwm 50.000us, foc150.000us]
    constexpr embot::hw::motor::bldc::pwm::PWMvalues pwm025000Hz {40000};        // pwm @  25.000 kHz [pwm 40.000us, foc120.000us]
    constexpr embot::hw::motor::bldc::pwm::PWMvalues pwm050000Hz {20000};        // pwm @  50.000 kHz [pwm 20.000us, foc 60.000us]
    constexpr embot::hw::motor::bldc::pwm::PWMvalues pwm066666Hz {15000, 36};    // pwm @  66.666 kHz [pwm 15.000us, foc 45.000us] 
    constexpr embot::hw::motor::bldc::pwm::PWMvalues pwm070000Hz {14285};        // pwm @  70.000 kHz [pwm 14.285us, foc 42.857us]
    constexpr embot::hw::motor::bldc::pwm::PWMvalues pwm075000Hz {13333};        // pwm @  75.000 kHz [pwm 13.333us, foc 40.000us]
    constexpr embot::hw::motor::bldc::pwm::PWMvalues pwm080000Hz {12500};        // pwm @  80.000 kHz [pwm 12.500us, foc 37.500us]
    constexpr embot::hw::motor::bldc::pwm::PWMvalues PWM082000Hz {12195};        // pwm @  82.000 kHz [pwm 12.195us, foc 36.585us]
    constexpr embot::hw::motor::bldc::pwm::PWMvalues PWM100000Hz {10000};        // pwm @ 100.000 kHz [pwm 10.000us, foc 30.000us]

    constexpr embot::hw::motor::bldc::pwm::PWMvalues pwmZINI     {10240, 24};    // pwm @  97.656 kHz [pwm 10.240us, foc 30.720us]

#if defined(MOTOR_BLDC_debug_PWMasZINIcode)
    constexpr embot::hw::motor::bldc::pwm::PWMvalues PWMvals {pwmZINI};
#else    
    constexpr embot::hw::motor::bldc::pwm::PWMvalues PWMvals {pwm066666Hz}; // {pwm066666Hz}; //{pwmZINI}; // {pwm020000Hz}
#endif
    
//    constexpr embot::hw::motor::bldc::pwm::PWMvalues PWMvals {pwm066666Hz};
//    constexpr embot::hw::motor::bldc::pwm::PWMvalues PWMvals {PWM100000Hz};
    
    
    constexpr auto QEncoder1Mode TIM_ENCODERMODE_TI12;
    constexpr auto QEncoder2Mode TIM_ENCODERMODE_TI12;
        
} // namespace embot::hw::motor::bldc::bsp::amcmj1 {

//namespace embot::hw::motor::bldc::pwm {
//    const PWMvalues & getPWMvalues(embot::hw::MOTOR m);// { return bsp::amcmj1::PWMvals; }
//}

#endif // #elif defined(EMBOT_ENABLE_hw_motor_bldc)

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



