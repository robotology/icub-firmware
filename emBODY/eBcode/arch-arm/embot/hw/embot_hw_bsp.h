
/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BSP_H_
#define _EMBOT_HW_BSP_H_

#include "embot_core.h"
#include "embot_core_binary.h"
#include "embot_hw.h"
#include "embot_hw_types.h"

#include <array>

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning you should use a HAL interface which matches the implementation inside the .cpp files
#endif


namespace embot::hw::bsp {

                
    bool initialised();
    
    void initialised(bool v);
    
    // it is used by embot::hw::init().
    // it calls the proper initialisations for the chosen HAL (e.g., stm32hal) etc. 
    bool init(const embot::hw::Config &config); 

    // it is used by embot::hw::init() and given to embot::core::init() so that embot::core::print() can work
    int print(const std::string &str);   

    // it is called inside embot::bsp::init() after the HAL init to ... specialize what it has done. it must be implemented board by board
    bool specialize();    
    
} // namespace embot::hw::bsp {


// - support maps

namespace embot::hw::bsp {
                    
    // it is the base class used by all other support maps
    struct SUPP
    {
        std::uint32_t supportedmask {0};
        
        constexpr SUPP(std::uint32_t m) : supportedmask(m) {}      
            
        template <typename T>
        constexpr bool supported(T v) const
        {
            return embot::core::binary::bit::check(supportedmask, embot::core::tointegral(v));
        }        
    }; 

    // the other support maps, one per each driver, must be defined board by board    
              
} // namespace embot::hw::bsp {


// in here declaration of code that depends on the DRIVER  
// it can be redefined using macros
// - EMBOT_REDEFINE_hw_bsp_DRIVER_init
// - EMBOT_REDEFINE_hw_bsp_DRIVER_puts
namespace embot::hw::bsp::DRIVER {
    
    // it prepares the HW DRIVER. In short: it calls HAL_Init(), configures clocks etc
    bool init(const embot::hw::Config &config);    
    // it sends a character to the HW. It may be ITM but it can ba also ... usart or else
    int puts(const char* str);   
}



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


