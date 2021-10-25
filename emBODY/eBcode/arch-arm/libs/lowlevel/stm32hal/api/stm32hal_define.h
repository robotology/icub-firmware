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


// c++-11 makes deprecated keyword volatile. c++-17 makes it not valid anymore
// i have two options: (a) use  -Wno-deprecated-register (b) remove it to avoid armclang complains.
//#if __ARMCOMPILER_VERSION > 6000000
//    #define register 
//#endif
// i choose the -Wno-deprecated-register solution in projects to avoid interference w/ stm32 lib. but with c++17 i must remove it

#if (__cplusplus >= 201703L)
    //#warning c++-17 features enabled: disabling keyword register in stm32hal
    #define register 
#endif
    
#if     defined(STM32HAL_BOARD_NUCLEO64)

    // it has a STM32L476xx mpu
    #if !defined(STM32L476xx)
    #define STM32L476xx
    #endif
    
    // of family STM32L4
    #if !defined(STM32L4)
    #define STM32L4
    #endif  

    // of family STM32L4 but in STM32HAL_ format   
    #if !defined(STM32HAL_STM32L4)
    #define STM32HAL_STM32L4
    #endif
    
//    #if !defined(ARM_MATH_CM4)
//    #define ARM_MATH_CM4
//    #endif
      
#elif   defined(STM32HAL_BOARD_MTB4)
    
    // it has a STM32L443xx mpu
    #if !defined(STM32L443xx)
    #define STM32L443xx
    #endif	
    
    // of family STM32L4
    #if !defined(STM32L4)
    #define STM32L4
    #endif  

    // of family STM32L4 but in STM32HAL_ format    
    #if !defined(STM32HAL_STM32L4)
    #define STM32HAL_STM32L4
    #endif    
    				
#elif   defined(STM32HAL_BOARD_STRAIN2)

    // it has a STM32L443xx mpu
    #if !defined(STM32L443xx)
    #define STM32L443xx
    #endif	
    
    // of family STM32L4
    #if !defined(STM32L4)
    #define STM32L4
    #endif  

    // of family STM32L4 but in STM32HAL_ format
    #if !defined(STM32HAL_STM32L4)
    #define STM32HAL_STM32L4
    #endif  
     
#elif   defined(STM32HAL_BOARD_RFE)

    // it has a STM32L452xx mpu
    #if !defined(STM32L452xx)
    #define STM32L452xx
    #endif  

    // of family STM32L4
    #if !defined(STM32L4)
    #define STM32L4
    #endif  

    // of family STM32L4 but in STM32HAL_ format  
    #if !defined(STM32HAL_STM32L4)
    #define STM32HAL_STM32L4
    #endif  
    
#elif   defined(STM32HAL_BOARD_PSC)

    // it has a STM32L431xx mpu
    #if !defined(STM32L431xx)
    #define STM32L431xx
    #endif	
    
    // of family STM32L4
    #if !defined(STM32L4)
    #define STM32L4
    #endif  

    // of family STM32L4 but in STM32HAL_ format
    #if !defined(STM32HAL_STM32L4)
    #define STM32HAL_STM32L4
    #endif 
       
#elif   defined(STM32HAL_BOARD_SG3)

    // it has a STM32L431xx mpu
    #if !defined(STM32L431xx)
    #define STM32L431xx
    #endif  

    // of family STM32L4
    #if !defined(STM32L4)
    #define STM32L4
    #endif  

    // of family STM32L4 but in STM32HAL_ format  
    #if !defined(STM32HAL_STM32L4)
    #define STM32HAL_STM32L4
    #endif  

#elif   defined(STM32HAL_BOARD_NUCLEOH7)

    // it has a STM32H743xx mpu
    #if !defined(STM32H743xx)
    #define STM32H743xx
    #endif  

    // of family STM32H7
    #if !defined(STM32H7)
    #define STM32H7
    #endif  

    // of family STM32H7 but in STM32HAL_ format  
    #if !defined(STM32HAL_STM32H7)
    #define STM32HAL_STM32H7
    #endif  

#elif   defined(STM32HAL_BOARD_STM32G4EVAL)

    // it has a STM32G474xx mpu
    #if !defined(STM32G474xx)
    #define STM32G474xx
    #endif  

    // of family STM32G4
    #if !defined(STM32G4)
    #define STM32G4
    #endif  

    // of family STM32H7 but in STM32HAL_ format  
    #if !defined(STM32HAL_STM32G4)
    #define STM32HAL_STM32G4
    #endif  

#elif   defined(STM32HAL_BOARD_PMC)

    // it has a STM32G474xx mpu
    #if !defined(STM32G474xx)
    #define STM32G474xx
    #endif  

    // of family STM32G4
    #if !defined(STM32G4)
    #define STM32G4
    #endif  

    // of family STM32H7 but in STM32HAL_ format  
    #if !defined(STM32HAL_STM32G4)
    #define STM32HAL_STM32G4
    #endif  

#elif   defined(STM32HAL_BOARD_AMCBLDC)

    // it has a STM32G474xx mpu
    #if !defined(STM32G474xx)
    #define STM32G474xx
    #endif  

    // of family STM32G4
    #if !defined(STM32G4)
    #define STM32G4
    #endif  

    // of family STM32H7 but in STM32HAL_ format  
    #if !defined(STM32HAL_STM32G4)
    #define STM32HAL_STM32G4
    #endif  

#elif   defined(STM32HAL_BOARD_STM32H745DISCO)

    // it has a STM32H745xx mpu
    #if !defined(STM32H745xx)
    #define STM32H745xx
    #endif  

    // of family STM32H7
    #if !defined(STM32H7)
    #define STM32H7
    #endif  

    // of family STM32H7 but in STM32HAL_ format  
    #if !defined(STM32HAL_STM32H7)
    #define STM32HAL_STM32H7
    #endif  
	
	// so far we use only the core CM7 for it
    #if !defined(CORE_CM7)
    #define CORE_CM7
    #endif  	

#elif   defined(STM32HAL_BOARD_AMC)

    // it has a STM32H745xx mpu
    #if !defined(STM32H745xx)
    #define STM32H745xx
    #endif  

    // of family STM32H7
    #if !defined(STM32H7)
    #define STM32H7
    #endif  

    // of family STM32H7 but in STM32HAL_ format  
    #if !defined(STM32HAL_STM32H7)
    #define STM32HAL_STM32H7
    #endif  
	
	// so far we use only the core CM7 for it
    #if !defined(CORE_CM7)
    #define CORE_CM7
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


