/* 
 * Copyright (C) 2007 RobotCub Consortium, European Commission FP6 Project IST-004370
  * Author: Claudio Lorini, Marco Randazzo, Valentina Gaggero
 * email:   claudio.lorini@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
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
#ifndef _STRAIN_MULTIPLEXER_H_
#define _STRAIN_MULTIPLEXER_H_

// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------
/**	@file 	hal_strain_multiplexer.h
	@brief 	This file defines functions to select analogical channel by mutiplexer.
	@author claudio.lorini@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
	@date	-
*/

// - external dependencies --------------------------------------------------------------------------------------------
#include "dspic30_stdint.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
/**
  * @fn     extern void hal_strain_mux_channelSelect(uint8_t ch);
  * @brief  selects the specified analog mux channel and performs phisical channel descrambling
  * @param  	CH: id channel
  * @retval none
  */
extern void hal_strain_mux_channelSelect(uint8_t ch);

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


