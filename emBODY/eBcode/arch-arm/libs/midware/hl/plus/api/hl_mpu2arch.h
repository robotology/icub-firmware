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
#ifndef _HL_MPU2ARCH_H_
#define _HL_MPU2ARCH_H_


/** @file       hl_mpu2arch.h
	@brief      This file converts the hl MPU name macros to the hl MPU arch macros 
	@author     marco.accame@iit.it
	@date       10/30/2013
**/

//#if     (HL_TARGET_MPU_NAME == HL_MPU_NAME_STM32F103RB)
//    #define STM32F10X_MD
//#elif   (HL_TARGET_MPU_NAME == HL_MPU_NAME_STM32F107VC)
//    #define STM32F10X_CL
//#else
//    #error HL-ERR: unrecognised HL_TARGET_MPU_NAME
//#endif

#if     defined(HL_USE_MPU_NAME_STM32F103RB)
    #if !defined(HL_USE_MPU_ARCH_STM32F1)
        #define HL_USE_MPU_ARCH_STM32F1
    #endif
#elif   defined(HL_USE_MPU_NAME_STM32F107VC)
    #if !defined(HL_USE_MPU_ARCH_STM32F1)
        #define HL_USE_MPU_ARCH_STM32F1
    #endif
#elif   defined(HL_USE_MPU_NAME_STM32F407IG)
    #if !defined(HL_USE_MPU_ARCH_STM32F4)
        #define HL_USE_MPU_ARCH_STM32F4
    #endif  
#else
    #error HL-ERR: unrecognised HL_USE_MPU_NAME_xxxx
#endif//converter

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

