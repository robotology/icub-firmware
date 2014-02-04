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
#ifndef _HL_MPUSPEED_H_
#define _HL_MPUSPEED_H_


/** @file       hl_mpuspeed.h
	@brief      This file prepares the macros used by cmsis for configuring the proper mpu speed 
	@author     marco.accame@iit.it
	@date       1/14/2014
**/



#if     defined(HL_CFG_MPUSPEED_INTclockspeed)
    #define HSI_VALUE       HL_CFG_MPUSPEED_INTclockspeed
#endif

#if     defined(HL_CFG_MPUSPEED_EXTclockspeed)
    #define HSE_VALUE       HL_CFG_MPUSPEED_EXTclockspeed
#endif


#include "hl_mpu2arch.h"

#if     defined(HL_USE_MPU_ARCH_STM32F1)

        // to fill with ... nothing
        
#elif   defined(HL_USE_MPU_ARCH_STM32F4)

    #if     defined(HL_CFG_MPUSPEED_STM32F4_PLL_m)
        #define PLL_M           HL_CFG_MPUSPEED_STM32F4_PLL_m
    #endif

    #if     defined(HL_CFG_MPUSPEED_STM32F4_PLL_n)
        #define PLL_N           HL_CFG_MPUSPEED_STM32F4_PLL_n
    #endif

    #if     defined(HL_CFG_MPUSPEED_STM32F4_PLL_p)
        #define PLL_P           HL_CFG_MPUSPEED_STM32F4_PLL_p
    #endif


    #if     defined(HL_CFG_MPUSPEED_STM32F4_PLL_q)
        #define PLL_Q           HL_CFG_MPUSPEED_STM32F4_PLL_q
    #endif

#else
    #error -> specify arch
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

