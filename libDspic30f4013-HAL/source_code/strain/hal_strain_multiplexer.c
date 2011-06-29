/* 
 * Copyright (C)2007 RobotCub Consortium, European Commission FP6 Project IST-004370
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
// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------
/*	@file 	hal_strain_multiplexer.c
	@brief 	This file implements functions to select analogical channel by mutiplexer.
	@author claudio.lorini@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
	@date	-
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include <p30f4013.h>
#include <libpic30.h>


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "hal_strain_multiplexer.h"
#include "hal_errors.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//#include "entity_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

void hal_strain_mux_channelSelect(uint8_t ch)
{
  switch (ch)
  {
  case 0: 
    // CH1 -> (J1-5) -> S5 -> A(1,0,0) -> RB(...x,x,0,0,1,x,x,x)
    LATBbits.LATB3 = 1; // A2
    LATBbits.LATB4 = 0; // A1
    LATBbits.LATB5 = 0; // A0
  break;
  case 1: 
    // CH2 -> (J1-2) -> S7 -> A(1,1,0) -> RB(...x,x,0,1,1,x,x,x)
    LATBbits.LATB3 = 1; // A2
    LATBbits.LATB4 = 1; // A1
    LATBbits.LATB5 = 0; // A0
  break;
  case 2:
    // CH3 -> (J3-2) -> S1 -> A(0,0,0) -> RB(...x,x,0,0,0,x,x,x)
    LATBbits.LATB3 = 0; // A2
    LATBbits.LATB4 = 0; // A1
    LATBbits.LATB5 = 0; // A0
  break;
  case 3: 
    // CH4 -> (J3-5) -> S4 -> A(0,1,1) -> RB(...x,x,1,1,0,x,x,x)
    LATBbits.LATB3 = 0; // A2
    LATBbits.LATB4 = 1; // A1
    LATBbits.LATB5 = 1; // A0
  break;
  case 4: 
    // CH5 -> (J2-5) -> S2 -> A(0,0,1) -> RB(...x,x,1,0,0,x,x,x)
    LATBbits.LATB3 = 0; // A2
    LATBbits.LATB4 = 0; // A1
    LATBbits.LATB5 = 1; // A0
  break;
  case 5: 
    // CH6 -> (J2-2) -> S6 -> A(1,0,1) -> RB(...x,x,1,0,1,x,x,x)
    LATBbits.LATB3 = 1; // A2
    LATBbits.LATB4 = 0; // A1
    LATBbits.LATB5 = 1; // A0
  break;

  default:
    // add an MUXINDEXINGERROR error in cueue
    hal_error_manage(ERR_MUX_INDEXING);
  break;
  }
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



