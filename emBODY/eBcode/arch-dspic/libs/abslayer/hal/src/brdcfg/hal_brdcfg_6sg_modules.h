  
// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _HAL_BRDCFG_6SG_MODULES_H_
#define _HAL_BRDCFG_6SG_MODULES_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/* @file       hal_brdcfg_6sg_modules.h
    @brief      This header file defines the modules to be compiled in hal
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       11/18/2011
 **/


// - modules to be built ----------------------------------------------------------------------------------------------

#define     HAL_USE_ARCH
#define     HAL_USE_BASE
#define     HAL_USE_CAN
#define     HAL_USE_CRC
#define     HAL_USE_EEPROM
#define     HAL_USE_FLASH
#define     HAL_USE_GPIO
#define     HAL_USE_LED
#define     HAL_USE_SYS
#define     HAL_USE_TIMER


// - exceptions -------------------------------------------------------------------------------------------------------


#ifdef  HAL_SLIM_MODE
    #error slim mode is not supported
    #undef  HAL_USE_choose_a_module
 #endif//HAL_SLIM_MODE

// - cross dependencies -----------------------------------------------------------------------------------------------

#ifdef  HAL_USE_EEPROM
    #define HAL_USE_FLASH
#endif//HAL_USE_EEPROM      

#ifdef  HAL_USE_LED
    #define HAL_USE_GPIO
#endif//HAL_USE_LED 



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


