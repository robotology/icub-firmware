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
#ifndef _HAL_BRDCFG_H_
#define _HAL_BRDCFG_H_


/* @file       hal_brdcfg.h
    @brief      This header file includes .h dependent on board
    @author     valentina.gaggero@iit.it
    @date       16/11/2010
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

extern uint32_t hal_brdcfg__getsize(const hal_base_cfg_t *cfg);
extern hal_result_t hal_brdcfg__setmem(const hal_base_cfg_t *cfg, uint32_t *memory);

// extern uint32_t hal_brdcfg_chips__getsize(const hal_base_cfg_t *cfg);
// extern hal_result_t hal_brdcfg_chips__setmem(const hal_base_cfg_t *cfg, uint32_t *memory);

// extern uint32_t hal_brdcfg_extbrds__getsize(const hal_base_cfg_t *cfg);
// extern hal_result_t hal_brdcfg_extbrds__setmem(const hal_base_cfg_t *cfg, uint32_t *memory);



// - other declarations which are specific to the chosen board

#if   defined(HAL_BOARD_MCBSTM32C)
	#include "mcbstm32c/hal_brdcfg_mcbstm32c.h"
#elif defined(HAL_BOARD_STM3210CEVAL)
	#include "stm3210ceval/hal_brdcfg_stm3210ceval.h"     
#elif defined(HAL_BOARD_EMS001)
	#include "ems001/hal_brdcfg_ems001.h"
#elif defined(HAL_BOARD_MCBSTM32F400) 
    #include "mcbstm32f400/hal_brdcfg_mcbstm32f400.h"
#elif defined(HAL_BOARD_EMS004) 
    #include "ems004/hal_brdcfg_ems004.h"    
#else 
	#error "Please select board XXX w/ HAL_BOARD_XXX"
#endif                      


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

