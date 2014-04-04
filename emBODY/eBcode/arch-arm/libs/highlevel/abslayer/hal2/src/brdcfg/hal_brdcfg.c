/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

/* @file       hal_brdcfg.c
    @brief      This header file includes .c dependent on board
    @author     valentina.gaggero@iit.it
    @date       16/11/2010
 **/

 
// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------

#include "hal_brdcfg_modules.h"
#include "hal_middleware_interface.h"

#include "hal_brdcfg.h"

#if     defined(HAL_BUILD_ONLYCORE)
    #warning HAL is being built in mode ONLYCORE
#elif   !defined(HAL_BUILD_ONLYCORE)
    #warning HAL is being built in mode ONLYCORE + EXTRA
#endif//!defined(HAL_BUILD_ONLYCORE)

// - the file ---------------------------------------------------------------------------------------------------------

#if   defined(HAL_BOARD_MCBSTM32C)
	#include "mcbstm32c/hal_brdcfg_mcbstm32c.c"
#elif defined(HAL_BOARD_STM3210CEVAL)
	#include "stm3210ceval/hal_brdcfg_stm3210ceval.c"    
#elif defined(HAL_BOARD_EMS001)
	#include "ems001/hal_brdcfg_ems001.c"
#elif defined(HAL_BOARD_MCBSTM32F400) 
    #include "mcbstm32f400/hal_brdcfg_mcbstm32f400.c"
#elif defined(HAL_BOARD_EMS004) 
    #include "ems004/hal_brdcfg_ems004.c"    
#elif defined(HAL_BOARD_EMS4RD) 
    #include "ems4rd/hal_brdcfg_ems4rd.c"  
#elif defined(HAL_BOARD_MC4PLUS) 
    #include "mc4plus/hal_brdcfg_mc4plus.c"    
#else
	#error "Please select board XXX w/ HAL_BOARD_XXX"
#endif                        


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
