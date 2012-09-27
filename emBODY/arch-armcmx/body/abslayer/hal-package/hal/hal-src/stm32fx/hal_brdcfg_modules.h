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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _HAL_BRDCFG_MODULES_H_
#define _HAL_BRDCFG_MODULES_H_


/* @file       hal_brdcfg.h
    @brief      This header file includes .h dependent on board
    @author     marco.accame@iit.it
    @date       11/18/2011
 **/


#if   defined(HAL_BOARD_MCBSTM32C)
	#include "brdcfg/hal_brdcfg_mcbstm32c_modules.h"
#elif defined(HAL_BOARD_EMS001)
	#include "brdcfg/hal_brdcfg_ems001_modules.h"
#elif defined(HAL_BOARD_MCBSTM32F400)
	#include "brdcfg/hal_brdcfg_mcbstm32f400_modules.h"    
#else 
	#error "Please select board XXX w/ HAL_BOARD_XXX"
#endif                      


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

