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
#ifndef _STM32HAL_DEFINE_H_
#define _STM32HAL_DEFINE_H_

#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       stm32hal_define.h
    @brief      This header file transform the macro STM32HAL_BOARD_xxx int what is required.
    @author     marco.accame@iit.it
    @date       03/21/2017
**/

    
/** @defgroup stm32hal_define STM32HALdefine STM32HALdefine

    The STM32HAL is ..... 
    
    @{        
 **/


#if !defined(USE_STM32HAL)
#define USE_STM32HAL
#endif   

#if !defined(USE_HAL_DRIVER)
#define USE_HAL_DRIVER
#endif   

    
    
#if     defined(STM32HAL_BOARD_NUCLEO64)

    // it has a STM32L476xx mpu
    #if !defined(STM32L476xx)
    #define STM32L476xx
    #endif
    
    #if !defined(STM32L4)
    #define STM32L4
    #endif      
    
    // of type STM32L4    
    #if !defined(STM32HAL_STM32L4xx)
    #define STM32HAL_STM32L4xx
    #endif
    
//    #if !defined(ARM_MATH_CM4)
//    #define ARM_MATH_CM4
//    #endif
      

#elif   defined(STM32HAL_BOARD_MTB4)
    
    // it has a STM32L443xx mpu
    #if !defined(STM32L443xx)
    #define STM32L443xx
    #endif	
    
    #if !defined(STM32L4)
    #define STM32L4
    #endif  

    // of type STM32L4    
    #if !defined(STM32HAL_STM32L4xx)
    #define STM32HAL_STM32L4xx
    #endif    
				
#elif   defined(STM32HAL_BOARD_STRAIN2)

    // it has a STM32L443xx mpu
    #if !defined(STM32L443xx)
    #define STM32L443xx
    #endif	
    
    #if !defined(STM32L4)
    #define STM32L4
    #endif  

    // of type STM32L4    
    #if !defined(STM32HAL_STM32L4xx)
    #define STM32HAL_STM32L4xx
    #endif  
 
#elif   defined(STM32HAL_BOARD_RFE)

    // it has a STM32L452xx mpu
    #if !defined(STM32L452xx)
    #define STM32L452xx
    #endif  

    #if !defined(STM32L4)
    #define STM32L4
    #endif  

    // of type STM32L4    
    #if !defined(STM32HAL_STM32L4xx)
    #define STM32HAL_STM32L4xx
    #endif  
    
    // other important macros
    #if !defined(USE_HAL_CAN_REGISTER_CALLBACKS)
    #define USE_HAL_CAN_REGISTER_CALLBACKS 1
    #endif  

#else
        #error STM32HAL: you must define a STM32HAL_BOARD_${BRD}
#endif

                
/** @}            
    end of group stm32hal_define  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


