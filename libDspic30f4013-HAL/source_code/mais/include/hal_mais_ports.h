/* 
 * Copyright (C) 2007 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author:	Valentina Gaggero
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
#ifndef _MAIS_PORTS_H_
#define _MAIS_PORTS_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
/*	@file 	hal_mais_ports.c
	@brief 	implements i/o ports functionalities.
	@author valentina.gaggero@iit.it
	@date	31/12/2010
*/


// - external dependencies --------------------------------------------------------------------------------------------
// empty-section

// - public #define  --------------------------------------------------------------------------------------------------

//led yellow
#define ledY   LATFbits.LATF4 
#define ledY_ON   ledY = 1; //led yellow
#define ledY_OFF  ledY = 0;

//led red
#define ledR   LATDbits.LATD1 
#define ledR_ON   ledR = 1;
#define ledR_OFF  ledR = 0;
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

/**
  * @fn     extern void hal_mais_ports_init();
  * @brief  init alla I/O ports. Actually only leds'ports
  * @param  -
  * @retval none
  */
extern void hal_mais_ports_init();

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


