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


// marco.accame on 17jun14: 
// mdk5 environment automatically adds -DSTM32F40_41xxx when using the stm32f407.
// it adds similar define with other mpus.
// as a result of this action, there is a redefinition of STM32F40_41xxx or of other macros.
// to maintain the control of the code vs mdk5, we must undefine all possible macros used by cmsis
// macros used for stm32f1:
#undef STM32F10X_LD
#undef STM32F10X_LD_VL
#undef STM32F10X_MD
#undef STM32F10X_MD_VL
#undef STM32F10X_HD 
#undef STM32F10X_HD_VL
#undef STM32F10X_XL
#undef STM32F10X_CL
// macros used for stm32f4:
#undef STM32F40_41xxx
#undef STM32F427_437xx 
#undef STM32F429_439xx
#undef STM32F401xx
#undef STM32F40XX
#undef STM32F427X

// ok, now i can convert HL macros into CMSIS macros

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

