
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
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


#if defined(EMBOT_REDEFINE_hw_bsp_DRIVER_init)
    
#include "embot_hw_dualcore.h"     

uint32_t _get1millitick()
{
    return embot::core::now() / 1000;        
}
    
bool embot::hw::bsp::DRIVER::init(const embot::hw::Config &config)
{
    // configure the chosen hal
    stm32hal_config_t cfg = {0};
    cfg.tick1ms_init = config.initmicrotime;
    cfg.tick1ms_get = _get1millitick;       
    stm32hal_config(&cfg);
    
    
    // and then, in here we do what we must
    embot::hw::dualcore::init();
    
    return true;    
}


#endif

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

