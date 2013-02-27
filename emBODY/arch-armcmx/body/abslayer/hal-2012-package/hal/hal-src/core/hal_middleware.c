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

/* @file       hal_middleware.c
    @brief      This file wraps the relevant system source file from middleware
    @author     marco.accame@iit.it
    @date       02/27/2013
 **/

 
// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------

#include "hal_brdcfg_modules.h"


// - the file ---------------------------------------------------------------------------------------------------------

#if   defined(HAL_USE_CPU_NAM_STM32F107)
    // we need to adapt the filesystem_stm32f10x.c for a stm32f107 by defining the macro STM32F10X_CL
    #define STM32F10X_CL
	#include "..\..\..\..\..\sys\mid\src\system_stm32f10x.c"
    //#include "system_stm32f10x.c"
#elif defined(HAL_USE_CPU_NAM_STM32F407)
	#error --> put file and define macro
#else
	#error ERR --> Please select board XXX w/ HAL_BOARD_XXX
#endif                        


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
