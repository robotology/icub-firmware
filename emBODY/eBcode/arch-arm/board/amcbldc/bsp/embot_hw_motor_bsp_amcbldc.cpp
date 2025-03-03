/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Kevin Sangalli
 * email:   kevin.sangalli@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>
#include <array>

#include "embot_core_binary.h"
#include "embot_core.h"
#include "embot_hw_sys.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif


using namespace std;
using namespace embot::core::binary;

// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"

// --------------------------------------------------------------------------------------------------------------------
// - support maps for the supported drivers
// --------------------------------------------------------------------------------------------------------------------

// - support map: begin of embot::hw::motor

#include "embot_hw_motor_bsp.h"

#if !defined(EMBOT_ENABLE_hw_motor)

namespace embot::hw::motor::bsp {
    
    constexpr BSP thebsp {};
    void BSP::init(embot::hw::MOTOR h) const {}    
    const BSP& getBSP() { return thebsp; }
    
}

#else

namespace embot::hw::motor::bsp {

    constexpr PROP propM1  { 0 };
    
    constexpr BSP thebsp {     

        // maskofsupported
        mask::pos2mask<uint32_t>(MOTOR::one),        
        // properties
        {{
            &propM1
        }}
  
    };
    
    void BSP::init(embot::hw::MOTOR h) const {
    // step 1: what cube mx does
        MX_ADC1_Init();
        MX_ADC2_Init();
        
        MX_TIM1_Init();
        MX_TIM3_Init();
        MX_TIM2_Init();
        MX_CORDIC_Init();
        MX_FMAC_Init();
        MX_CRC_Init();   
#if defined(STM32HAL_DRIVER_V120)
#else
        MX_TIM15_Init(); 
#endif        
        
        HAL_GPIO_WritePin(VAUXEN_GPIO_Port, VAUXEN_Pin, GPIO_PIN_SET);
        HAL_Delay(10); 
    }

    const BSP& getBSP() 
    {
        return thebsp;
    }
              
} //namespace embot::hw::motor::bsp {

#endif // motor

// - support map: end of embot::hw::motor


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

    
    
