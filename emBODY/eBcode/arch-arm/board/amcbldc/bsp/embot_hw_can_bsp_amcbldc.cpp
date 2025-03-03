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

// - support map: begin of embot::hw::can

#include "embot_hw_can_bsp.h"

#if   !defined(EMBOT_ENABLE_hw_can)

namespace embot::hw::can {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::CAN h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}

#else

namespace embot::hw::can {
                       
    constexpr PROP can1p = { .handle = &hfdcan2 };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(CAN::one),        
        // properties
        {{
            &can1p           
        }}        
    };
    
    
    void BSP::init(embot::hw::CAN h) const 
    {
        if(h == CAN::one)
        { 
//            // at first make sure these pins are reset
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
            HAL_Delay(1);

            // yes: CAN::one is mapped into FDCAN2
            MX_FDCAN2_Init();
                        
//            // for 80 mhz (from a project by st)
//            hfdcan2.Instance = FDCAN2;
//            hfdcan2.Init.ClockDivider = FDCAN_CLOCK_DIV2;
//            hfdcan2.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
//            hfdcan2.Init.Mode = FDCAN_MODE_NORMAL;
//            hfdcan2.Init.AutoRetransmission = ENABLE;
//            hfdcan2.Init.TransmitPause = ENABLE;
//            hfdcan2.Init.ProtocolException = DISABLE;
//            hfdcan2.Init.NominalPrescaler = 1;
//            hfdcan2.Init.NominalSyncJumpWidth = 16;
//            hfdcan2.Init.NominalTimeSeg1 = 63;
//            hfdcan2.Init.NominalTimeSeg2 = 16;
//            hfdcan2.Init.DataPrescaler = 1;
//            hfdcan2.Init.DataSyncJumpWidth = 4;
//            hfdcan2.Init.DataTimeSeg1 = 5;
//            hfdcan2.Init.DataTimeSeg2 = 4;
//            hfdcan2.Init.StdFiltersNbr = 1;
//            hfdcan2.Init.ExtFiltersNbr = 0;
//            hfdcan2.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
//            if(HAL_FDCAN_Init(&hfdcan2) != HAL_OK)
//            {
//                Error_Handler();
//            }

        }
    }

    const BSP& getBSP() 
    {
        return thebsp;
    }
              
} // namespace embotembot::hw::can {
    

#if defined(HAL_FDCAN_MODULE_ENABLED)

extern "C"
{    
    void FDCAN2_IT0_IRQHandler(void)
    {
        HAL_FDCAN_IRQHandler(&hfdcan2);
    }  
    
    void FDCAN2_IT1_IRQHandler(void)
    {
        HAL_FDCAN_IRQHandler(&hfdcan2);
    }  
}
        

#endif // HAL_FDCAN_MODULE_ENABLED

#endif // EMBOT_ENABLE_hw_can


// - support map: end of embot::hw::can


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

    
    
