
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_ANALOG_BSP_H_
#define __EMBOT_HW_ANALOG_BSP_H_

#include "embot_core.h"
#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_analog.h"

namespace embot::hw::analog::bsp {    
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::analog::Value::maxnumberof);
        constexpr BSP(std::uint32_t msk) : SUPP(msk) {}
        constexpr BSP() : SUPP(0) {}            
            
        bool supported(const Value v, embot::hw::MOTOR m = embot::hw::MOTOR::none) const;    
        bool init(const Configuration &config) const;
        bool deinit() const;
        float get(const Value v, embot::hw::MOTOR m = embot::hw::MOTOR::none) const;            
    };
    
    const BSP& getBSP();                

} // namespace embot::hw::analog::bsp

#if 0

the analog surely uses an ADC and maybe it owns a TIM that triggers its acquisition.
we can surely embed inside embot::hw::analog::bsp {} those values and their init functions
but the HAL of stm32 call a common MSP function for all the adc or for all the timers. 
so in here we put the specific funtions for it. they must be used in the relevant common 
function.
Example

extern "C" {

    void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
    {
        embot::hw::analog::bsp::stm32msp::HAL_ADC_MspInit(adcHandle);
        embot::hw::MOTOR::bldc::adc::bsp::stm32msp::HAL_ADC_MspInit(adcHandle);
        // etc for any adc used ....  
    }

}

#endif


namespace embot::hw::analog::bsp::stm32 {
        
    void HAL_ADC_MspInit(void* p);          // ADC_HandleTypeDef*
    void HAL_ADC_MspDeInit(void* p);        // ADC_HandleTypeDef*
    
    void HAL_TIM_Base_MspInit(void* p);     // TIM_HandleTypeDef*  
    void HAL_TIM_Base_MspDeInit(void* p);   // TIM_HandleTypeDef*    
    
} // namespace embot::hw::analog::bsp::stm32 {

#warning .... maybe move FactoryCalibration to some embot_hw_adc_bsp.h

namespace embot::hw::adc::bsp {
    
    struct FactoryCalibration
    {
        uint16_t vrefint_cal {0};           // Raw VREFINT manufacturer calibration 
        uint16_t vrefint_cal_vref {0};      // VREF for VREFINT calibration (mV)    
        uint16_t ts_cal1 {0};               // Raw TEMP1 manufacturer measurement   
        uint16_t ts_cal2 {0};               // Raw TEMP2 manufacturer measurement   
        uint16_t ts_cal1_temp {0};          // Nominal TEMP1 value (celsius deg.)   
        uint16_t ts_cal2_temp {0};          // Nominal TEMP2 value (celsius deg.)   
        uint16_t ts_cal_vref {0};           // Nominal VREF value (mV) for TEMP1 and TEMP2 measurement 
    };    
    
    const FactoryCalibration & getfactorycalibration();
} // namespace embot::hw::adc::bsp {

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

