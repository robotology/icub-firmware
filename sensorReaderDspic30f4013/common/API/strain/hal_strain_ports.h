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
#ifndef _STRAIN_PORTS_H_
#define _STRAIN_PORTS_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------
/**	@file 	hal_strain_ports.h
	@brief 	This file define functions to initilise strain board pin.
	@author claudio.lorini@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
	@date	-
*/

// - external dependencies --------------------------------------------------------------------------------------------
// empty-section

// - public #define  --------------------------------------------------------------------------------------------------
#define LED 	LATBbits.LATB12
#define LED_ON  LATBbits.LATB12 = 1;
#define LED_OFF LATBbits.LATB12 = 0;

#define TEST_PIN 		LATBbits.LATB7
#define TEST_PIN_ON 	LATBbits.LATB7 = 1;
#define TEST_PIN_OFF 	LATBbits.LATB7 = 0;


 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
/**
  * @fn  extern void hal_strain_ports_init(void);
  * @brief init all I/O ports.
  * @param -
  * @retval none
  */
extern void hal_strain_ports_init(void);


// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


