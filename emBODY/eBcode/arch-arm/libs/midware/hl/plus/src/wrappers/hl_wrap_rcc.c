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


/* @file       hl_wrap_rcc.c
	@brief      This file wraps a stm32fx file.
	@author     marco.accame@iit.it
    @date       10/21/2013
**/


#if     defined(HL_CFG_PLUS_TARGET_BY_FILE)
    #include "hl_cfg_plus_target.h"
#endif


#include "hl_cfg_plus_modules.h"

#if     defined(HL_USE_CORE_STM32)


#if     defined(HL_USE_MPU_ARCH_STM32F1)
    #include "../../../core/stm32f1/src/stm32f10x_rcc.c"
#elif   defined(HL_USE_MPU_ARCH_STM32F2)
    #include "../../../core/stm32f4/src/stm32f2xx_rcc.c"
#elif   defined(HL_USE_MPU_ARCH_STM32F4)
    #include "../../../core/stm32f4/src/stm32f4xx_rcc.c"
#else
    #error HL-PLUS: specify a HL_USE_MPU_ARCH_*
#endif


#endif//defined(HL_USE_CORE_STM32)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


