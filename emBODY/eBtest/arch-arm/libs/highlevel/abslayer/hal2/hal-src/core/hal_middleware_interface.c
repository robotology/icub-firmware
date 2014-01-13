/*
  * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologiaa
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

/* @file       hal_middleware_interface.c
    @brief      This file wraps the relevant system source file from middleware
    @author     marco.accame@iit.it
    @date       02/27/2013
 **/

 
// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------

#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_MIDDLEWARE_INTERFACE

// - the api ----------------------------------------------------------------------------------------------------------

#include "hal_middleware_interface.h"


// - the file ---------------------------------------------------------------------------------------------------------

#if   defined(HAL_USE_CPU_NAM_STM32F107)
    // we need to adapt the filesystem_stm32f10x.c for a stm32f107 by defining the macro STM32F10X_CL
    #define STM32F10X_CL
	#include "..\..\..\..\..\sys\mid\src\system_stm32f10x_weaked.c"
#elif defined(HAL_USE_CPU_NAM_STM32F407)
    // we need to adapt the filesystem_stm32f10x.c for a stm32f107 by defining the macro STM32F10X_CL
    #define STM32F4XX 
    #define __CORTEX_M4
	#include "..\..\..\..\..\sys\mid\src\system_stm32f4xx_weaked.c"
#else
	#error ERR --> Please select board XXX w/ HAL_BOARD_XXX
#endif 

#else

    #warning BEWARE --> you are not using the standard HAL middleware interface. are you sure? 

#endif//HAL_USE_MIDDLEWARE_INTERFACE


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
