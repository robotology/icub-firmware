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
#ifndef _HL_MPU2CMSIS_H_
#define _HL_MPU2CMSIS_H_


/** @file       hl_mpu2cmsis.h
	@brief      This file converts the hl MPU macros to the macros used by stm32 cmsis 
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
    #define STM32F10X_MD
#elif   defined(HL_USE_MPU_NAME_STM32F107VC)
    #define STM32F10X_CL
#elif   defined(HL_USE_MPU_NAME_STM32F407IG)
    #define STM32F40_41xxx 
    #define __CORTEX_M4    
#else
    #error HL-ERR: unrecognised HL_USE_MPU_NAME_xxxx
#endif//converter

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

