/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _HAL_MPUNAME2MACROS_H_
#define _HAL_MPUNAME2MACROS_H_


/** @file       hal_mpuname2macros.h
	@brief      This file prepares the macros used by hal starting from the defined HAL_USE_MPU_NAME_ macro 
	@author     marco.accame@iit.it
	@date       03/24/2014
**/


#if     defined(HAL_USE_MPU_NAME_STM32F103RB)
    // HAL macros
    #if !defined(HAL_USE_MPU_ARCH_ARMCM3)
        #define HAL_USE_MPU_ARCH_ARMCM3
    #endif
    #if !defined(HAL_USE_MPU_TYPE_STM32F1)
        #define HAL_USE_MPU_TYPE_STM32F1
    #endif
    // HL macros
    #if !defined(HL_USE_MPU_NAME_STM32F103RB)
        #define HL_USE_MPU_NAME_STM32F103RB
    #endif    
#elif   defined(HAL_USE_MPU_NAME_STM32F107VC)
    // HAL macros
    #if !defined(HAL_USE_MPU_ARCH_ARMCM3)
        #define HAL_USE_MPU_ARCH_ARMCM3
    #endif
    #if !defined(HAL_USE_MPU_TYPE_STM32F1)
        #define HAL_USE_MPU_TYPE_STM32F1
    #endif
    // HL macros
    #if !defined(HL_USE_MPU_NAME_STM32F107VC)
        #define HL_USE_MPU_NAME_STM32F107VC
    #endif    
#elif   defined(HAL_USE_MPU_NAME_STM32F407IG)
    // HAL macros
    #if !defined(HAL_USE_MPU_ARCH_ARMCM4)
        #define HAL_USE_MPU_ARCH_ARMCM4
    #endif
    #if !defined(HAL_USE_MPU_TYPE_STM32F4)
        #define HAL_USE_MPU_TYPE_STM32F4
    #endif  
    // HL macros
    #if !defined(HL_USE_MPU_NAME_STM32F407IG)
        #define HL_USE_MPU_NAME_STM32F407IG
    #endif    
#else
    #error HAL-ERR: unrecognised HAL_USE_MPU_NAME_xxxx. please use STM32F103RB, STM32F107VC, STM32F407IG
#endif//converter

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

