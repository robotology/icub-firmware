/* 
 * Copyright (C) 2011 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
#ifndef _UTILS_H_
#define _UTILS_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
/** @file    hal_utils.h
    @brief   In this file there are functions that provide general services.
    @author  valentina.gaggero@iit.it
    @date
*/

// - external dependencies --------------------------------------------------------------------------------------------
#include "dspic30_stdint.h"
#include <libpic30.h>

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


//used by strain to convert to 1.15 fixed point
#define HEX_VALC 0x8000


// - declaration of public user-defined types ------------------------------------------------------------------------- 



// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

/**
  * @fn	inline extern void hal_delay(uint32_t cycle_num)
  * @brief	performs delay of cycles denoted by its argument.The minimum delay is 12 cycles
  *         including call and return statements.  With this function only the time
  *         required for argument transmission is not accounted for.  Requests for
  *         less than 12 cycles of delay will cause an 12 cycle delay.
  * @param  cycle_num: number of cycles
  * @retval
  */
inline extern void hal_delay(uint32_t cycle_num)
{
	__delay32(cycle_num);
}
/**
  * @fn	extern void hal_atchdog_reset();
  * @brief	resets watchdog
  * @param	none
  * @retval	none
  */
extern void hal_watchdog_reset();


/**
  * @fn	extern void hal_atchdog_enable();
  * @brief	enable watchdog by software.
			You must be sure you have turn off wd in configuration bits
  * @param	none
  * @retval	none
  */
extern void hal_watchdog_enable();


/**
  * @fn	extern void hal_atchdog_disable();
  * @brief	enable watchdog by software.
			You must be sure you have turn off wd in configuration bits
  * @param	none
  * @retval	none
  */
extern void hal_watchdog_disable(); 

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
