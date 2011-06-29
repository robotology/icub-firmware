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
// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------
/** @file   hal_ mais_internalADC.h
    @brief   This file implements functions to interact with multiplexer of ADC.
    @author  claudio.lorini@iit.it, marco.maggiali@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
    @date
*/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include <p30f4013.h>
#include <libpic30.h>


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "hal_mais_multiplexer.h"
#include "hal_errors.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


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
void hal_mais_mux_init(void)
{
	TRISCbits.TRISC13=0;
	TRISCbits.TRISC14=0;
}	

// select the specified analog nux channel and 
// perform phisical channel descrambling
void hal_mais_mux_channelSelect(unsigned CH)
{
  switch (CH)
  {
  case 0:  // T1 Thumb
  case 1:  // T2
  case 2:  // T3
  case 3:  // I1 Index
  case 4:  // I2
  case 5:  // I3
  case 6:  // M1 Middle
  case 7:  // M2
  case 8:  // M3
  case 9:  // R1 Ring
  case 10: // R2
  case 11: // R3
    LATCbits.LATC14 = 0; // A14 MUXA0
    LATCbits.LATC13 = 0; // A13 MUXA1
  break;

  case 12:  // L1 Little
    LATCbits.LATC14 = 1; // A14 MUXA0
    LATCbits.LATC13 = 0; // A13 MUXA1
  break;

  case 13:  // L2
    LATCbits.LATC14 = 0; // A14 MUXA0
    LATCbits.LATC13 = 1; // A13 MUXA1
  break;

  case 14:  // L3
    LATCbits.LATC14 = 1; // A14 MUXA0
    LATCbits.LATC13 = 1; // A13 MUXA1
  break;
 
 default:
    // channel id not valid
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



