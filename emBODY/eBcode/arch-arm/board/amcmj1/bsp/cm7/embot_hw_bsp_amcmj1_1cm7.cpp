
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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
// - support maps
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - specialize the bsp
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_amcmj1_1cm7.h"


namespace embot::hw::bsp::amcfoc::cm7 {
        
    embot::hw::BTN EXTFAULTbutton()
    {
        return embot::hw::BTN::one;
    }
    
    embot::hw::LED EXTFAULTled() 
    {
        return embot::hw::LED::two;
    }

}


#if     !defined(EMBOT_ENABLE_hw_bsp_specialize)
bool embot::hw::bsp::specialize() { return true; }
#else   

    bool embot::hw::bsp::specialize()
    {
        // all the rest
        // nothing for now
//        __HAL_RCC_GPIOB_CLK_ENABLE();
//        __HAL_RCC_GPIOG_CLK_ENABLE();
//        __HAL_RCC_GPIOD_CLK_ENABLE();
//        __HAL_RCC_GPIOC_CLK_ENABLE();
//        __HAL_RCC_GPIOH_CLK_ENABLE();
//        __HAL_RCC_GPIOA_CLK_ENABLE();
//        __HAL_RCC_GPIOF_CLK_ENABLE();
//        __HAL_RCC_GPIOE_CLK_ENABLE();        
        
        
        
//        /** Enable the VREF clock
//        */
//        __HAL_RCC_VREF_CLK_ENABLE();

//        /** Configure the internal voltage reference buffer voltage scale
//        */
//        HAL_SYSCFG_VREFBUF_VoltageScalingConfig(SYSCFG_VREFBUF_VOLTAGE_SCALE0);

//        /** Enable the Internal Voltage Reference buffer
//        */
//        HAL_SYSCFG_EnableVREFBUF();

//        /** Configure the internal voltage reference buffer high impedance mode
//        */
//        HAL_SYSCFG_VREFBUF_HighImpedanceConfig(SYSCFG_VREFBUF_HIGH_IMPEDANCE_DISABLE);         
        
        return true;
    }

#endif  //EMBOT_ENABLE_hw_bsp_specialize
    
    
    
//extern "C"
//{

//    void HAL_MspInit(void)
//    {
////        /** Enable the VREF clock
////        */
////        __HAL_RCC_VREF_CLK_ENABLE();

////        /** Configure the internal voltage reference buffer voltage scale
////        */
////        HAL_SYSCFG_VREFBUF_VoltageScalingConfig(SYSCFG_VREFBUF_VOLTAGE_SCALE0);

////        /** Enable the Internal Voltage Reference buffer
////        */
////        HAL_SYSCFG_EnableVREFBUF();

////        /** Configure the internal voltage reference buffer high impedance mode
////        */
////        HAL_SYSCFG_VREFBUF_HighImpedanceConfig(SYSCFG_VREFBUF_HIGH_IMPEDANCE_DISABLE);        
//    }

//}    




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

