/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _HAL_BRDCFG_H_
#define _HAL_BRDCFG_H_


/* @file       hal_brdcfg.h
    @brief      This header file includes .h dependent on board
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       02/28/2013
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"

#include "hal_brdcfg_modules.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


extern hal_result_t hal_brdcfg__start(void);

// - other declarations which are specific of the chosen board

#if   defined(HAL_BOARD_MCBSTM32F400) 
    #include "mcbstm32f400/hal_brdcfg_mcbstm32f400.h"
#elif defined(HAL_BOARD_EMS4RD) 
    #include "ems4rd/hal_brdcfg_ems4rd.h"        
#elif defined(HAL_BOARD_MC4PLUS) 
    #include "mc4plus/hal_brdcfg_mc4plus.h"    
#else 
	#error "Please select board XXX w/ HAL_BOARD_XXX"
#endif                      


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

