/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Coauthor:  Kevin Sangalli
 * email:     kevin.sangalli@iit.it
*/
// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_SPI_BSP_AMC_H_
#define __EMBOT_HW_SPI_BSP_AMC_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_spi.h"
#include "embot_hw_spi_bsp.h"


#if !defined(EMBOT_ENABLE_hw_spi)

namespace embot::hw::spi::bsp {


}

#else

namespace embot::hw::spi::bsp {
    
    #if defined(STM32HAL_BOARD_AMC)
    
    void s_J5_SPIpinout(embot::hw::SPI h, bool enable);
    
    #else
        #error embot::hw::bsp::spi::thebsp must be defined    
    #endif

}


#endif // spi


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


