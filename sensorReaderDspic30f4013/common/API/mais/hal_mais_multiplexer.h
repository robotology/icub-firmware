/* 
 * Copyright (C) 2007 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Claudio Lorini, Marco Maggiali, Marco Randazzo, Valentina Gaggero
 * email:   claudio.lorini@iit.it, marco.maggiali@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
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
#ifndef _MAIS_MULTIPLEXER_H_
#define _MAIS_MULTIPLEXER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
/** @file    hal_mais_multiplexer.h
    @brief   This file provides functions to interact with multiplexer for select analogic channel.
    @author  claudio.lorini@iit.it, marco.maggiali@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
    @date
*/


// - external dependencies --------------------------------------------------------------------------------------------
// empty-section

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

/**
  * @fn     extern void  hal_mais_mux_init(unsigned CH);
  * @brief  initialis emultiplexer on mais board
  * @param  -
  * @retval none
  */
extern void hal_mais_mux_init(void);

/**
  * @fn     extern void  hal_mais_mux_channelSelect(unsigned CH);
  * @brief  select the specified analog mux channel and perform phisical channel descrambling
  * @param  	CH: id channel
  * @retval none
  */
extern void hal_mais_mux_channelSelect(unsigned CH);

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


