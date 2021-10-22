

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _STM32HAL_DRIVER_CFG_H_
#define _STM32HAL_DRIVER_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

// path in this file is referred to stm32hal/api

#include "stm32hal_define.h" 
#include "stm32hal_driver.h" 

#if     defined(STM32HAL_BOARD_NUCLEO64)
    
    #include "../src/config/stm32hal_driver_cfg_of_nucleo64.h" 
    
#elif   defined(STM32HAL_BOARD_MTB4)

    #if     (STM32HAL_DRIVER_VERSION == 0x190)
        #include "../src/config/stm32hal_driver_cfg_of_mtb4_v190.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32l4-v190/inc/stm32l4xx_ll_gpio.h"
    #elif   (STM32HAL_DRIVER_VERSION == 0x172)
        #include "../src/config/stm32hal_driver_cfg_of_mtb4.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32l4-v172/inc/stm32l4xx_ll_gpio.h"
    #else
        #error unknown driver version
    #endif

#elif   defined(STM32HAL_BOARD_STRAIN2)
    
    #if     (STM32HAL_DRIVER_VERSION == 0x1B0)
        #include "../src/config/stm32hal_driver_cfg_of_strain2_v1B0.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32l4-v1B0/inc/stm32l4xx_ll_gpio.h"
    #elif   (STM32HAL_DRIVER_VERSION == 0x190)
        #include "../src/config/stm32hal_driver_cfg_of_strain2_v190.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32l4-v190/inc/stm32l4xx_ll_gpio.h"
    #elif   (STM32HAL_DRIVER_VERSION == 0x172)
        #include "../src/config/stm32hal_driver_cfg_of_strain2.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32l4-v172/inc/stm32l4xx_ll_gpio.h"
    #else
        #error unknown driver version
    #endif

#elif   defined(STM32HAL_BOARD_RFE)
    
    #include "../src/config/stm32hal_driver_cfg_of_rfe.h"   
    // we also need some ll includes contained in the driver section
    #include "../src/driver/stm32l4-v183/inc/stm32l4xx_ll_gpio.h"
    
    
#elif   defined(STM32HAL_BOARD_PSC)
    
    #if     (STM32HAL_DRIVER_VERSION == 0x190)
        #include "../src/config/stm32hal_driver_cfg_of_psc_v190.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32l4-v190/inc/stm32l4xx_ll_gpio.h"
    #else
        #error unknown driver version
    #endif
    
#elif   defined(STM32HAL_BOARD_SG3)
    
    #if     (STM32HAL_DRIVER_VERSION == 0x190)
        #include "../src/config/stm32hal_driver_cfg_of_sg3_v190.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32l4-v190/inc/stm32l4xx_ll_gpio.h"
    #else
        #error unknown driver version
    #endif
    

#elif   defined(STM32HAL_BOARD_NUCLEOH7)
    
    #if (STM32HAL_DRIVER_VERSION == 0x180)
        #define USE_FULL_LL_DRIVER
        #include "../src/config/stm32hal_driver_cfg_of_nucleoh7_v180.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32h7-v180/inc/stm32h7xx_ll_gpio.h"
    #elif (STM32HAL_DRIVER_VERSION == 0x190)
        #define USE_FULL_LL_DRIVER
        #include "../src/config/stm32hal_driver_cfg_of_nucleoh7_v190.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32h7-v190/inc/stm32h7xx_ll_gpio.h"        
    #else
        #error unknown driver version
    #endif

#elif   defined(STM32HAL_BOARD_STM32G4EVAL)
    
    #if (STM32HAL_DRIVER_VERSION == 0x111)
        #define USE_FULL_LL_DRIVER
        #include "../src/config/stm32hal_driver_cfg_of_stm32g4eval_v111.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32g4-v111/inc/stm32g4xx_ll_gpio.h"
    #elif (STM32HAL_DRIVER_VERSION == 0x120)
        #define USE_FULL_LL_DRIVER
        #include "../src/config/stm32hal_driver_cfg_of_stm32g4eval_v120.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
    #else    
        #error unknown driver version
    #endif

#elif   defined(STM32HAL_BOARD_PMC)
    
    #if (STM32HAL_DRIVER_VERSION == 0x111)
        #define USE_FULL_LL_DRIVER
        #include "../src/config/stm32hal_driver_cfg_of_pmc_v111.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32g4-v111/inc/stm32g4xx_ll_gpio.h"
    #elif (STM32HAL_DRIVER_VERSION == 0x120)
        #define USE_FULL_LL_DRIVER
        #include "../src/config/stm32hal_driver_cfg_of_pmc_v120.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"        
    #else
        #error unknown driver version
    #endif

#elif   defined(STM32HAL_BOARD_AMCBLDC)
    
    #if (STM32HAL_DRIVER_VERSION == 0x120)
        #define USE_FULL_LL_DRIVER
        #include "../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"        
    #else
        #error unknown driver version
    #endif

#elif   defined(STM32HAL_BOARD_STM32H745DISCO)
    
    #if (STM32HAL_DRIVER_VERSION == 0x190)
        #define USE_FULL_LL_DRIVER
        // keep out of the compilation the weak functions of HAL_ETH
        #define STM32HAL_removeWEAK_ETH
        #include "../src/config/stm32hal_driver_cfg_of_stm32h745disco_v190.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32h7-v190/inc/stm32h7xx_ll_gpio.h"
    #elif (STM32HAL_DRIVER_VERSION == 0x1A0)
        #define USE_FULL_LL_DRIVER
        // keep out of the compilation the weak functions of HAL_ETH
        // they will be defined inside EMAC_STM32H7xx.c
        #define STM32HAL_removeWEAK_ETH
        #define STM32HAL_improveHANDLER_ETH
        #include "../src/config/stm32hal_driver_cfg_of_stm32h745disco_v1A0.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32h7-v1A0/inc/stm32h7xx_ll_gpio.h"
    #else
        #error unknown driver version
    #endif
 
#elif   defined(STM32HAL_BOARD_AMC)
    
    #if (STM32HAL_DRIVER_VERSION == 0x1A0)
        #define USE_FULL_LL_DRIVER
        // keep out of the compilation the weak functions of HAL_ETH
        // they will be defined inside EMAC_STM32H7xx.c
        #define STM32HAL_removeWEAK_ETH
        #define STM32HAL_improveHANDLER_ETH
        #include "../src/config/stm32hal_driver_cfg_of_amc_v1A0.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32h7-v1A0/inc/stm32h7xx_ll_gpio.h"
    #else
        #error unknown driver version
    #endif
    
#else

        #error pls define a STM32HAL_BOARD_${BRD}
    
#endif


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard

