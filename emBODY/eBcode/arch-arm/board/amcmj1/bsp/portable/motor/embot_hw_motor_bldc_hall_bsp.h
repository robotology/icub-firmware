
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BLDC_HALL_BSP_H_
#define __EMBOT_HW_MOTOR_BLDC_HALL_BSP_H_


#include "embot_hw_motor_bldc_hall.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_gpio.h"
#include <array>

namespace embot::hw::motor::bldc::hall::bsp {
    
    static const size_t numofhallsensors {3};

#warning -> verify and decide the order of the pins    
    struct PROP
    {
        enum index { H1 = 0, H2 = 1, H3 = 2};         
        std::array<embot::hw::GPIO, numofhallsensors> hhh {};
        constexpr PROP() = default;
        constexpr PROP(const embot::hw::GPIO &h1,const embot::hw::GPIO &h2, const embot::hw::GPIO &h3) : hhh ({h1, h2, h3}) {}
    };    
            
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::MOTOR::maxnumberof);
        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) : SUPP(msk), properties(pro) {}
        constexpr BSP() : SUPP(0), properties({0}) {}            

        std::array<const PROP*, maxnumberof> properties;    
        constexpr const PROP * getPROP(embot::hw::MOTOR m) const { return supported(m) ? properties[embot::core::tointegral(m)] : nullptr; }
            
        bool init(embot::hw::MOTOR m) const;
        bool deinit(embot::hw::MOTOR m) const; 
        bool start(embot::hw::MOTOR m) const;        
        bool stop(embot::hw::MOTOR m) const;
        
        // the following two should be called in pair by the user or the IRQ handler as:
        // capture(embot::hw::MOTOR::one, triple(embot::hw::MOTOR::one));
        
        // use this to get the triple 
        uint8_t triple(embot::hw::MOTOR m) const;       
        // but then execute capture to process the triple logically
        void capture(embot::hw::MOTOR m, uint8_t triple) const;      
    };
    
    const BSP& getBSP();   
    
                      
} // namespace embot::hw::motor::bldc::hall::bsp {


namespace embot::hw::motor::bldc::hall::bsp::stm32 {
 
    void HAL_TIMEx_HallSensor_MspInit(void *p);     // TIM_HandleTypeDef*  
    void HAL_TIMEx_HallSensor_MspDeInit(void *p);   // TIM_HandleTypeDef*  
    
} // namespace embot::hw::motor::bldc::adc::hall::stm32 {




#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

