
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
#ifndef _STM32HAL_DRIVER_H_
#define _STM32HAL_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

    
    
// - external dependencies --------------------------------------------------------------------------------------------  
    
#include "stm32hal_define.h"



// - public interface  ------------------------------------------------------------------------------------------------
  
#if     defined(STM32HAL_BOARD_NUCLEO64) || defined(STM32HAL_BOARD_MTB4) 

    // we have three boards which all share the same driver type (stm32l4) and version (v 1.7.2)
    #include "../src/driver/stm32l4-v172/inc/stm32l4xx_hal.h"

#elif   defined(STM32HAL_BOARD_RFE) || defined(STM32HAL_BOARD_STRAIN2)

    // we have one board which needs a different version  (v 1.8.3) of the same driver type (stm32l4)
    #include "../src/driver/stm32l4-v183/inc/stm32l4xx_hal.h"

#else
    #error STM32HAL: the STM32HAL_BOARD_${BRD} is undefined
#endif

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


