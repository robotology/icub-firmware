
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
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

#include "embot_hw_gpio.h"
#include "embot_hw_sys.h"


namespace embot::hw::bsp::amcmj1::cm7 {
        
}



#if !defined(EMBOT_ENABLE_hw_bsp_specialize)

bool embot::hw::bsp::specialize() { return true; }

#else

    bool embot::hw::bsp::specialize()
    { 
        
#if defined(EMBOT_ENABLE_hw_can_5V) 
        
        constexpr embot::hw::GPIO candrivergpiovauxen = 
                {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::thirteen};    // PWR_VAUXEN_GPIO_Port, PWR_VAUXEN_Pin
                
        constexpr embot::hw::gpio::Config cfgvauxen {
                embot::hw::gpio::Mode::OUTPUTpushpull, 
                embot::hw::gpio::Pull::pulldown, 
                embot::hw::gpio::Speed::low };
        
        embot::hw::gpio::init(candrivergpiovauxen, cfgvauxen);
        embot::hw::gpio::set(candrivergpiovauxen, embot::hw::gpio::State::SET);
    
#endif        
        
        return true;
    }

#endif  //EMBOT_ENABLE_hw_bsp_specialize

   
    


    
    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

