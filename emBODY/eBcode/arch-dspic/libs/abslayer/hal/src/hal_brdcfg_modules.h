// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _HAL_BRDCFG_MODULES_H_
#define _HAL_BRDCFG_MODULES_H_


/* @file       hal_brdcfg.h
    @brief      This header file includes .h dependent on board
    @author     marco.accame@iit.it
    @date       11/18/2011
 **/


#ifdef HAL_BOARD_6SG
	#include "brdcfg/hal_brdcfg_6sg_modules.h"
#elif defined HAL_BOARD_2FOC
	#include "brdcfg/hal_brdcfg_2foc_modules.h"
#elif defined HAL_BOARD_2FOCAPPL
	#include "brdcfg/hal_brdcfg_2focappl_modules.h"
#else 
	#error "Please select board XXX w/ HAL_BOARD_XXX"
#endif                      


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

