
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_mtx_bsp_amc.h"

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

#include "embot_hw_bsp_amc_config.h"


// --------------------------------------------------------------------------------------------------------------------
// - support maps
// --------------------------------------------------------------------------------------------------------------------


// - support map: begin of embot::hw::mtx

#include "embot_hw_mtx_bsp.h"
#include "embot_hw_mtx.h"

#if !defined(EMBOT_ENABLE_hw_mtx)

namespace embot::hw::mtx::bsp {
             
    constexpr BSP thebsp {};
    void BSP::init(embot::hw::MTX h) const {}
    const BSP & getBSP() { return thebsp; }     
    
}

#elif defined(EMBOT_ENABLE_hw_mtx)

namespace embot::hw::mtx::bsp {
            
    volatile bool clkinitted {false};
    volatile bool irqinitted {false};   
    
    // HSEM values are [0, 31].
    // i dont use 0 because it is reserved to booting mechanism. So far ... 
    
    constexpr PROP m01 = { 1 };
    constexpr PROP m02 = { 2 };
    constexpr PROP m03 = { 3 };
    constexpr PROP m04 = { 4 };
    constexpr PROP m05 = { 5 };
    constexpr PROP m06 = { 6 };
    constexpr PROP m07 = { 7 };
    constexpr PROP m08 = { 8 };
    constexpr PROP m09 = { 9 };
    constexpr PROP m10 = {10 };
    constexpr PROP m11 = {11 };
    constexpr PROP m12 = {12 };
    constexpr PROP m13 = {13 };
    constexpr PROP m14 = {14 };
    constexpr PROP m15 = {15 };
    constexpr PROP m16 = {16 };
    constexpr PROP m17 = {17 };
    constexpr PROP m18 = {18 };
    constexpr PROP m19 = {19 };
    constexpr PROP m20 = {20 };
    constexpr PROP m21 = {21 };
    constexpr PROP m22 = {22 };
    constexpr PROP m23 = {23 };
    constexpr PROP m24 = {24 };
       
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(MTX::one) | mask::pos2mask<uint32_t>(MTX::two) | 
        mask::pos2mask<uint32_t>(MTX::three) | mask::pos2mask<uint32_t>(MTX::four) |
        mask::pos2mask<uint32_t>(MTX::five) | mask::pos2mask<uint32_t>(MTX::six) |
        mask::pos2mask<uint32_t>(MTX::seven) | mask::pos2mask<uint32_t>(MTX::eight) |
        mask::pos2mask<uint32_t>(MTX::nine) | mask::pos2mask<uint32_t>(MTX::ten) |
        mask::pos2mask<uint32_t>(MTX::eleven) |  mask::pos2mask<uint32_t>(MTX::twelve) |
        mask::pos2mask<uint32_t>(MTX::thirteen) | mask::pos2mask<uint32_t>(MTX::fourteen) |
        mask::pos2mask<uint32_t>(MTX::fifteen) | mask::pos2mask<uint32_t>(MTX::sixteen) |
        mask::pos2mask<uint32_t>(MTX::seventeen) | mask::pos2mask<uint32_t>(MTX::eighteen) |
        mask::pos2mask<uint32_t>(MTX::nineteen) | mask::pos2mask<uint32_t>(MTX::twenty) |
        mask::pos2mask<uint32_t>(MTX::twentyone) | mask::pos2mask<uint32_t>(MTX::twentytwo) |
        mask::pos2mask<uint32_t>(MTX::twentythree) | mask::pos2mask<uint32_t>(MTX::twentyfour),        
        // properties
        {{
            &m01, &m02, &m03, &m04, &m05, &m06, &m07, &m08, &m09, &m10, 
            &m11, &m12, &m13, &m14, &m15, &m16, &m17, &m18, &m19, &m20,
            &m21, &m22, &m23, &m24            
        }}        
    };
    

    void BSP::init(embot::hw::MTX h) const
    {
        if(false == clkinitted)
        {
            __HAL_RCC_HSEM_CLK_ENABLE();
            clkinitted = true;
        }
        
        if(false == irqinitted)
        {
            HAL_NVIC_SetPriority(HSEM1_IRQn, 10, 0);
            HAL_NVIC_EnableIRQ(HSEM1_IRQn);
            HAL_NVIC_SetPriority(HSEM2_IRQn, 10, 0);
            HAL_NVIC_EnableIRQ(HSEM2_IRQn);            
            irqinitted = true;
        }           
    }    
        
    const BSP & getBSP() 
    {
        return thebsp;
    }    

} // namespace embot::hw::mtx::bsp {


#endif // #elif defined(EMBOT_ENABLE_hw_mtx)

// in here the required C functions

extern "C" {
    
    // the IRQ handler used by embot::hw::mtx
    
    void HSEM2_IRQHandler(void)
    {
        HAL_HSEM_IRQHandler();	  
    }    

    void HSEM1_IRQHandler(void)
    {
        HAL_HSEM_IRQHandler();	  
    }  
        
    // important note:
    // void HAL_HSEM_FreeCallback(uint32_t SemMask) is inside the driver embot_hw_mtx.cpp file    
}


// - support map: end of embot::hw::mtx


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

