

/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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
#ifndef _HAL_MPU_STM32XX_INCLUDE_H_
#define _HAL_MPU_STM32XX_INCLUDE_H_


// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------

#include "hal_brdcfg_modules.h"


#if   defined(HAL_USE_CPU_NAM_STM32F107)
    // we need to adapt the stm32f1.h and cmsis_stm32f1.h for a stm32f107 by defining the macro STM32F10X_CL
    #define STM32F10X_CL
	#include "stm32f1.h"
    #include "cmsis_stm32f1.h"
    //#define USE_STM32F1        
#elif defined(HAL_USE_CPU_NAM_STM32F407)

	#include "stm32f4.h"
    #include "cmsis_stm32f4.h"   
    //#define USE_STM32F4
    
	#error --> put file and define macro
#else
	#error ERR --> Please select board XXX w/ HAL_BOARD_XXX
#endif                        


// #if   defined(HAL_BOARD_MCBSTM32C) || defined(HAL_BOARD_STM3210CEVAL) || defined(HAL_BOARD_EMS001)
// 	#include "stm32f1.h"
//     #include "cmsis_stm32f1.h"
//     //#define cmsis_stm32xx_hid_set_system_core_clock     cmsis_stm32f1_hid_set_system_core_clock
//     #define USE_STM32F1
// //#elif defined(HAL_BOARD_EMS001)
// //	#include "stm32f1.h"
// //    #include "cmsis_stm32f1.h"
// //    //#define cmsis_stm32xx_hid_set_system_core_clock     cmsis_stm32f1_hid_set_system_core_clock
// //    #define USE_STM32F1
// #elif defined(HAL_BOARD_MCBSTM32F400) || defined(HAL_BOARD_EMS004)
// 	#include "stm32f4.h"
//     #include "cmsis_stm32f4.h"   
//     //#define cmsis_stm32xx_hid_set_system_core_clock     cmsis_stm32f4_hid_set_system_core_clock
//     #define USE_STM32F4
// //#elif defined(HAL_BOARD_EMS004)
// //	#include "stm32f4.h"
// //    #include "cmsis_stm32f4.h"   
// //    //#define cmsis_stm32xx_hid_set_system_core_clock     cmsis_stm32f4_hid_set_system_core_clock
// //    #define USE_STM32F4    
// #else 
// 	#error "Please select board XXX w/ HAL_BOARD_XXX"
// #endif        



#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

