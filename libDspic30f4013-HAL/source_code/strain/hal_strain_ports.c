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
// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------
/*	@file 	hal_strain_ports.c
	@brief 	This file implements functions to initilise strain board pin.
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
#include "hal_strain_ports.h"


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


void hal_strain_ports_init(void)
{
  // TRISA = 0x0800;  // all inputs
  TRISB = 0x1FFF;  // all inputs
  TRISC = 0xE000;  // all inputs
  TRISD = 0x030F;  // all inputs
  // TRISE = 0x013F;  // all inputs
  TRISF = 0x007F;  // all inputs

  // - - - 

  TRISBbits.TRISB12 = 0; // set B12 as out (LED)

//@@@@
// For test puropuses
  TRISBbits.TRISB6 = 0; // set sampling trigger
  TRISBbits.TRISB7 = 0; // set mux CH1 trigger
  LATBbits.LATB6=0; 
  LATBbits.LATB7=0;
//@@@@

  TRISBbits.TRISB5  = 0;  // set B5 as out (MUXA0)
  TRISBbits.TRISB4  = 0;  // set B4 as out (MUXA1)
  TRISBbits.TRISB3  = 0;  // set B3 as out (MUXA2)
  
  TRISAbits.TRISA11 = 0; // set A11 as out (STARTCONV ADC)
  LATAbits.LATA11   = 0; // Start conversion inactive   

  TRISDbits.TRISD9  = 0; // set D9 as out (\SS1 = \SS DAC)
  LATDbits.LATD9    = 1; // SS DAC inactive
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


