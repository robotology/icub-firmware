
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

#include "embot_hw_bsp_amcfoc_2cm4.h"


namespace embot::hw::bsp::amcfoc2cm4 {
    

}

#if     !defined(EMBOT_ENABLE_hw_bsp_specialize)
bool embot::hw::bsp::specialize() { return true; }
#else   

bool embot::hw::bsp::specialize()
{
    // all the rest
    // nothing for now

    // ETM trace-4
//        RCC->AHB4ENR |= 0x0000001C;
//        
//        GPIOE->MODER = 0x000002A0;
//        GPIOE->OSPEEDR = 0x000003F0;
//        GPIOE->PUPDR = 0x00000000;
//        GPIOE->AFR[0] = 0x00000000;
//        
//        GPIOD->MODER = 0x00000020;
//        GPIOD->OSPEEDR = 0x00000030;
//        GPIOD->PUPDR = 0x00000000;
//        GPIOD->AFR[0] = 0x00000000;
//        
//        GPIOC->MODER = 0x02000000;
//        GPIOC->OSPEEDR = 0x03000000;
//        GPIOC->PUPDR = 0x00000000;
//        GPIOC->AFR[1] = 0x00000000;    
    
    return true;  

}    
    
#endif

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

