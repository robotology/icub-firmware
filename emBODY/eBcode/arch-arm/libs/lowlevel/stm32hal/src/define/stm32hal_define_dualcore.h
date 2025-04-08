/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __STM32HAL_DEFINE_DUALCORE_H_
#define __STM32HAL_DEFINE_DUALCORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#if   defined(STM32HAL_BOARD_AMC_1CM7)

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
    
    #if !defined(STM32HAL_DUALCORE)
    #define STM32HAL_DUALCORE
    #endif   

    #undef STM32HAL_CORE_CM4
    #if !defined(STM32HAL_CORE_CM7)
    #define STM32HAL_CORE_CM7
    #endif  

#elif   defined(STM32HAL_BOARD_AMC_2CM4)

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
	
	// so far we use only the core CM4 for it
    #if !defined(CORE_CM4)
    #define CORE_CM4
    #endif  

    #if !defined(STM32HAL_DUALCORE)
    #define STM32HAL_DUALCORE
    #endif
    
    #undef STM32HAL_CORE_CM7
    #if !defined(STM32HAL_CORE_CM4)
    #define STM32HAL_CORE_CM4
    #endif      

#elif     defined(STM32HAL_BOARD_AMCFOC_1CM7)

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
    
    #if !defined(STM32HAL_DUALCORE)
    #define STM32HAL_DUALCORE
    #endif
    
    #undef STM32HAL_CORE_CM4
    #if !defined(STM32HAL_CORE_CM7)
    #define STM32HAL_CORE_CM7
    #endif         

#elif   defined(STM32HAL_BOARD_AMCFOC_2CM4)

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
	
	// so far we use only the core CM4 for it
    #if !defined(CORE_CM4)
    #define CORE_CM4
    #endif 

    #if !defined(STM32HAL_DUALCORE)
    #define STM32HAL_DUALCORE
    #endif  
    
    #undef STM32HAL_CORE_CM7
    #if !defined(STM32HAL_CORE_CM4)
    #define STM32HAL_CORE_CM4
    #endif  

#elif     defined(STM32HAL_BOARD_AMCMJ1_1CM7)

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
    
    #if !defined(STM32HAL_DUALCORE)
    #define STM32HAL_DUALCORE
    #endif
    
    #undef STM32HAL_CORE_CM4
    #if !defined(STM32HAL_CORE_CM7)
    #define STM32HAL_CORE_CM7
    #endif      
    

#elif   defined(STM32HAL_BOARD_AMCMJ1_2CM4)

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
	
	// so far we use only the core CM4 for it
    #if !defined(CORE_CM4)
    #define CORE_CM4
    #endif 

    #if !defined(STM32HAL_DUALCORE)
    #define STM32HAL_DUALCORE
    #endif  
    
    #undef STM32HAL_CORE_CM7
    #if !defined(STM32HAL_CORE_CM4)
    #define STM32HAL_CORE_CM4
    #endif  
    
#else
    #error STM32HAL: you must define a DUAL_CORE STM32HAL_BOARD_${BRD}
#endif

                
/** @}            
    end of group stm32hal_define  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


