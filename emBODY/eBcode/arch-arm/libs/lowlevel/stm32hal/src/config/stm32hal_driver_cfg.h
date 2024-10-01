

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _STM32HAL_DRIVER_CFG_H_
#define _STM32HAL_DRIVER_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

// path in this file is referred to stm32hal/api

#include "stm32hal_define.h" 
#include "stm32hal_driver.h" 

#if     defined(STM32HAL_BOARD_NUCLEOL4)

    #if (STM32HAL_DRIVER_VERSION == 0x1D3)
        #include "../src/config/stm32hal_driver_cfg_of_nucleol4_v1D3.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32l4-v1D3/inc/stm32l4xx_ll_gpio.h"
    #else
        #error unknown driver version
    #endif

    
#elif   defined(STM32HAL_BOARD_MTB4)

    #if     (STM32HAL_DRIVER_VERSION == 0x1D3)
        #include "../src/config/stm32hal_driver_cfg_of_mtb4_v1D3.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32l4-v1D3/inc/stm32l4xx_ll_gpio.h"
    #elif   (STM32HAL_DRIVER_VERSION == 0x190)
        #include "../src/config/stm32hal_driver_cfg_of_mtb4_v190.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32l4-v190/inc/stm32l4xx_ll_gpio.h"
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
    #elif   (STM32HAL_DRIVER_VERSION == 0x1D3)
        #include "../src/config/stm32hal_driver_cfg_of_strain2_v1D3.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32l4-v1D3/inc/stm32l4xx_ll_gpio.h"
    #else
        #error unknown driver version
    #endif

#elif   defined(STM32HAL_BOARD_RFE)

    #if     (STM32HAL_DRIVER_VERSION == 0x183)
        #include "../src/config/stm32hal_driver_cfg_of_rfe.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32l4-v183/inc/stm32l4xx_ll_gpio.h"
     #else
        #error unknown driver version
    #endif       
    
    
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
    #elif (STM32HAL_DRIVER_VERSION == 0x122)
        #define USE_FULL_LL_DRIVER
        #include "../src/config/stm32hal_driver_cfg_of_amcbldc_v122.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32g4-v122/inc/stm32g4xx_ll_gpio.h"
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
        //#define STM32HAL_removeWEAK_ETH
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
    #elif (STM32HAL_DRIVER_VERSION == 0x1B1)
        #define USE_FULL_LL_DRIVER
        // keep out of the compilation the weak functions of HAL_ETH
        // they will be defined inside EMAC_STM32H7xx.c
        #define STM32HAL_removeWEAK_ETH
        #define STM32HAL_improveHANDLER_ETH
        #include "../src/config/stm32hal_driver_cfg_of_amc_v1B1.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32h7-v1B1/inc/stm32h7xx_ll_gpio.h"        
    #else
        #error unknown driver version
    #endif

#elif   defined(STM32HAL_BOARD_AMC2C)
    
    #if (STM32HAL_DRIVER_VERSION == 0x1A0)
        #define USE_FULL_LL_DRIVER
        // keep out of the compilation the weak functions of HAL_ETH
        // they will be defined inside EMAC_STM32H7xx.c
        #define STM32HAL_removeWEAK_ETH
        #define STM32HAL_improveHANDLER_ETH
        #include "../src/config/stm32hal_driver_cfg_of_amc2c_v1A0.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32h7-v1A0/inc/stm32h7xx_ll_gpio.h"
    #elif (STM32HAL_DRIVER_VERSION == 0x1B1)
        #define USE_FULL_LL_DRIVER
        // keep out of the compilation the weak functions of HAL_ETH
        // they will be defined inside EMAC_STM32H7xx.c
        #define STM32HAL_removeWEAK_ETH
        #define STM32HAL_improveHANDLER_ETH
        #include "../src/config/stm32hal_driver_cfg_of_amc2c_v1B1.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32h7-v1B1/inc/stm32h7xx_ll_gpio.h"    
    #else
        #error unknown driver version
    #endif

#elif   defined(STM32HAL_BOARD_AMC_1CM7)
    
    #if (STM32HAL_DRIVER_VERSION == 0x1A0)
        #define USE_FULL_LL_DRIVER
        // keep out of the compilation the weak functions of HAL_ETH
        // they will be defined inside EMAC_STM32H7xx.c
        #define STM32HAL_removeWEAK_ETH
        #define STM32HAL_improveHANDLER_ETH
        #include "../src/config/stm32hal_driver_cfg_of_amc_1cm7_v1A0.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32h7-v1A0/inc/stm32h7xx_ll_gpio.h"
    #elif (STM32HAL_DRIVER_VERSION == 0x1B1)
        #define USE_FULL_LL_DRIVER
        // keep out of the compilation the weak functions of HAL_ETH
        // they will be defined inside EMAC_STM32H7xx.c
        #define STM32HAL_removeWEAK_ETH
        #define STM32HAL_improveHANDLER_ETH
        #include "../src/config/stm32hal_driver_cfg_of_amc_1cm7_v1B1.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32h7-v1B1/inc/stm32h7xx_ll_gpio.h"        
    #else
        #error unknown driver version
    #endif

#elif   defined(STM32HAL_BOARD_AMC_2CM4)
    
    #if (STM32HAL_DRIVER_VERSION == 0x1A0)
        #define USE_FULL_LL_DRIVER
        // keep out of the compilation the weak functions of HAL_ETH
        // they will be defined inside EMAC_STM32H7xx.c
        #define STM32HAL_removeWEAK_ETH
        #define STM32HAL_improveHANDLER_ETH
        #include "../src/config/stm32hal_driver_cfg_of_amc_2cm4_v1A0.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32h7-v1A0/inc/stm32h7xx_ll_gpio.h"
    #elif (STM32HAL_DRIVER_VERSION == 0x1B1)
        #define USE_FULL_LL_DRIVER
        // keep out of the compilation the weak functions of HAL_ETH
        // they will be defined inside EMAC_STM32H7xx.c
        #define STM32HAL_removeWEAK_ETH
        #define STM32HAL_improveHANDLER_ETH
        #include "../src/config/stm32hal_driver_cfg_of_amc2c_v1B1.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32h7-v1B1/inc/stm32h7xx_ll_gpio.h"    
    #else
        #error unknown driver version
    #endif
    
#elif   defined(STM32HAL_BOARD_AMCFOCM7)
    
    #if (STM32HAL_DRIVER_VERSION == 0x1A0)
        #define USE_FULL_LL_DRIVER
        // keep out of the compilation the weak functions of HAL_ETH
        // they will be defined inside EMAC_STM32H7xx.c
        #define STM32HAL_removeWEAK_ETH
        #define STM32HAL_improveHANDLER_ETH
        #include "../src/config/stm32hal_driver_cfg_of_amcfocm7_v1A0.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32h7-v1A0/inc/stm32h7xx_ll_gpio.h"
    #else
        #error unknown driver version
    #endif

#elif   defined(STM32HAL_BOARD_AMCFOC_1CM7)
    
    #if (STM32HAL_DRIVER_VERSION == 0x1A0)
        #define USE_FULL_LL_DRIVER
        // keep out of the compilation the weak functions of HAL_ETH
        // they will be defined inside EMAC_STM32H7xx.c
        #define STM32HAL_removeWEAK_ETH
        #define STM32HAL_improveHANDLER_ETH
        #include "../src/config/stm32hal_driver_cfg_of_amcfoc_1cm7_v1A0.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32h7-v1A0/inc/stm32h7xx_ll_gpio.h"
    #else
        #error unknown driver version
    #endif
    
#elif   defined(STM32HAL_BOARD_AMCFOCM4)
    
    #if (STM32HAL_DRIVER_VERSION == 0x1A0)
        #define USE_FULL_LL_DRIVER
        // keep out of the compilation the weak functions of HAL_ETH
        // they will be defined inside EMAC_STM32H7xx.c
        #define STM32HAL_removeWEAK_ETH
        #define STM32HAL_improveHANDLER_ETH
        #include "../src/config/stm32hal_driver_cfg_of_amcfocm4_v1A0.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32h7-v1A0/inc/stm32h7xx_ll_gpio.h"
    #else
        #error unknown driver version
    #endif
 
#elif   defined(STM32HAL_BOARD_AMCFOC_2CM4)
    
    #if (STM32HAL_DRIVER_VERSION == 0x1A0)
        #define USE_FULL_LL_DRIVER
        // keep out of the compilation the weak functions of HAL_ETH
        // they will be defined inside EMAC_STM32H7xx.c
        #define STM32HAL_removeWEAK_ETH
        #define STM32HAL_improveHANDLER_ETH
        #include "../src/config/stm32hal_driver_cfg_of_amcfoc_2cm4_v1A0.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32h7-v1A0/inc/stm32h7xx_ll_gpio.h"
    #else
        #error unknown driver version
    #endif
    
#elif   defined(STM32HAL_BOARD_MTB4C)

    #if     (STM32HAL_DRIVER_VERSION == 0x1D3)
        #include "../src/config/stm32hal_driver_cfg_of_mtb4c_v1D3.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32l4-v1D3/inc/stm32l4xx_ll_gpio.h"
    #elif   (STM32HAL_DRIVER_VERSION == 0x190)
        #include "../src/config/stm32hal_driver_cfg_of_mtb4c_v190.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32l4-v190/inc/stm32l4xx_ll_gpio.h"
    #else
        #error unknown driver version
    #endif
    
#elif   defined(STM32HAL_BOARD_STRAIN2C)

    #if     (STM32HAL_DRIVER_VERSION == 0x1D3)
        #include "../src/config/stm32hal_driver_cfg_of_strain2c_v1D3.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32l4-v1D3/inc/stm32l4xx_ll_gpio.h"
    #elif   (STM32HAL_DRIVER_VERSION == 0x1B0)
        #include "../src/config/stm32hal_driver_cfg_of_strain2c_v1B0.h"   
        // we also need some ll includes contained in the driver section
        #include "../src/driver/stm32l4-v1B0/inc/stm32l4xx_ll_gpio.h"
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

