
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BLDC_ADC_BSP_H_
#define __EMBOT_HW_MOTOR_BLDC_ADC_BSP_H_

#include "embot_core.h"
#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_motor_bldc_adc.h"
 
namespace embot::hw::motor::bldc::adc::bsp {
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::MOTOR::maxnumberof);
        constexpr BSP(std::uint32_t msk) : SUPP(msk) {}
        constexpr BSP() : SUPP(0) {}            
   
        bool init(embot::hw::MOTOR m) const;
        bool deinit(embot::hw::MOTOR m) const; 

        bool set(embot::hw::MOTOR m, const embot::hw::motor::bldc::OnCurrents &cbk) const;             
    };
    
    const BSP& getBSP();   
                      
} // namespace embot::hw::motor::bldc::adc::bsp {

namespace embot::hw::motor::bldc::adc::bsp::stm32 {
        
    void HAL_ADC_MspInit(void* p);          // ADC_HandleTypeDef*
    void HAL_ADC_MspDeInit(void* p);        // ADC_HandleTypeDef*
    
    void HAL_TIM_Base_MspInit(void* p);     // TIM_HandleTypeDef*  
    void HAL_TIM_Base_MspDeInit(void* p);   // TIM_HandleTypeDef*   
    
} // namespace embot::hw::motor::bldc::adc::bsp::stm32 {

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

