
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
  
#if     defined(STM32HAL_BOARD_NUCLEO64)

    // only one possible driver
    #if !defined(STM32HAL_DRIVER_V172)
        #define STM32HAL_DRIVER_V172
    #endif
    #define STM32HAL_DRIVER_VERSION 0x172
    
#elif   defined(STM32HAL_BOARD_MTB4)

// two possible drivers. default is the 190 ...
    #if     defined(STM32HAL_DRIVER_V172)    
        #define STM32HAL_DRIVER_VERSION 0x172  
    #else   
        #if !defined(STM32HAL_DRIVER_V190)
            #define STM32HAL_DRIVER_V190
        #endif        
        #define STM32HAL_DRIVER_VERSION 0x190   
    #endif
    
#elif   defined(STM32HAL_BOARD_STRAIN2)

    // three possible drivers. default is the 1B0 ...
    #if     defined(STM32HAL_DRIVER_V172)    
        #define STM32HAL_DRIVER_VERSION 0x172  
    #elif   defined(STM32HAL_DRIVER_V190)
        #define STM32HAL_DRIVER_VERSION 0x190
    #else   
        #if !defined(STM32HAL_DRIVER_V1B0)
            #define STM32HAL_DRIVER_V1B0
        #endif        
        #define STM32HAL_DRIVER_VERSION 0x1B0   
    #endif
    
#elif   defined(STM32HAL_BOARD_RFE)

    // only one possible driver
    #if !defined(STM32HAL_DRIVER_V183)
        #define STM32HAL_DRIVER_V183
    #endif
    #define STM32HAL_DRIVER_VERSION 0x183
    
#elif   defined(STM32HAL_BOARD_PSC)

    // only one possible driver
    #if !defined(STM32HAL_DRIVER_V190)
        #define STM32HAL_DRIVER_V190
    #endif
    #define STM32HAL_DRIVER_VERSION 0x190
      
#elif   defined(STM32HAL_BOARD_SG3)

    // only one possible driver
    #if !defined(STM32HAL_DRIVER_V190)
        #define STM32HAL_DRIVER_V190
    #endif
    #define STM32HAL_DRIVER_VERSION 0x190
      
#elif   defined(STM32HAL_BOARD_NUCLEOH7)

    // only one possible driver
    #if !defined(STM32HAL_DRIVER_V180)
        #define STM32HAL_DRIVER_V180
    #endif
    #define STM32HAL_DRIVER_VERSION 0x180

#elif   defined(STM32HAL_BOARD_STM32G4EVAL)

    // only one possible driver
    #if !defined(STM32HAL_DRIVER_V111)
        #define STM32HAL_DRIVER_V1111
    #endif
    #define STM32HAL_DRIVER_VERSION 0x111

#elif   defined(STM32HAL_BOARD_PMC)

    // only one possible driver
    #if !defined(STM32HAL_DRIVER_V111)
        #define STM32HAL_DRIVER_V1111
    #endif
    #define STM32HAL_DRIVER_VERSION 0x111
    
#else
    #error STM32HAL: the STM32HAL_BOARD_${B} is undefined
#endif


// now extra code-shaping macros which depend on the family and the driver version 
#if defined(STM32HAL_STM32L4)
    #if (STM32HAL_DRIVER_VERSION >= 0x183)
        // there is a new api for can
        #if !defined(USE_HAL_CAN_REGISTER_CALLBACKS)
        #define USE_HAL_CAN_REGISTER_CALLBACKS 1
        #endif  
    #endif
#elif defined(STM32HAL_STM32H7)
    // nothing so far for can on h7
#elif defined(STM32HAL_STM32G4)
    // nothing so far for can on g4
#endif



// and only now ... include the .h which must see the code-shaping macros

#if defined(STM32HAL_STM32L4)

    #if     defined(STM32HAL_DRIVER_V172)    
        #include "../src/driver/stm32l4-v172/inc/stm32l4xx_hal.h"       
    #elif   defined(STM32HAL_DRIVER_V183)        
        #include "../src/driver/stm32l4-v183/inc/stm32l4xx_hal.h"
    #elif   defined(STM32HAL_DRIVER_V190)        
        #include "../src/driver/stm32l4-v190/inc/stm32l4xx_hal.h"
    #elif   defined(STM32HAL_DRIVER_V1B0)        
        #include "../src/driver/stm32l4-v1B0/inc/stm32l4xx_hal.h"
    #else
        #error STM32HAL: the STM32HAL_DRIVER_${V} is not managed
    #endif

#elif defined(STM32HAL_STM32H7)

    #if     defined(STM32HAL_DRIVER_V180)    
        #include "../src/driver/stm32h7-v180/inc/stm32h7xx_hal.h"       
    #else
        #error STM32HAL: the STM32HAL_DRIVER_${V} is not managed
    #endif

#elif defined(STM32HAL_STM32G4)

    #if     defined(STM32HAL_DRIVER_V111)    
        #include "../src/driver/stm32g4-v111/inc/stm32g4xx_hal.h"       
    #else
        #error STM32HAL: the STM32HAL_DRIVER_${V} is not managed
    #endif
    
#else
    #error STM32HAL: the STM32HAL_${SF} is not managed
#endif   

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


