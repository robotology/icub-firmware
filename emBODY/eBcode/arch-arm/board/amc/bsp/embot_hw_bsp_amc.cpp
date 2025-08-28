/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:    Marco Accame,        Kevin Sangalli
 * email:     marco.accame@iit.it, kevin.sangalli@iit.it
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


// --------------------------------------------------------------------------------------------------------------------
// - Inclusion of the bsp cpp files
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_amc_config.h"

#include "embot_hw_can_bsp_amc.cpp"
#include "embot_hw_eeprom_bsp_amc.cpp"
#include "embot_hw_encoder_bsp_amc.cpp"
#include "embot_hw_flash_bsp_amc.cpp"
#include "embot_hw_gpio_bsp_amc.cpp"
#include "embot_hw_i2c_bsp_amc.cpp"
#include "embot_hw_led_bsp_amc.cpp"
#include "embot_hw_spi_bsp_amc.cpp"
#include "embot_hw_specialize_bsp_amc.cpp"
#include "embot_hw_timer_bsp_amc.cpp"


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


