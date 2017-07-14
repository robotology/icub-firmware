/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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
#ifndef _AD7147_CONFIGURATION_H_
#define _AD7147_CONFIGURATION_H_

#ifdef __cplusplus
extern "C" {
#endif


  

// - external dependencies --------------------------------------------------------------------------------------------

#include "if2hw_common.h"



// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types -------------------------------------------------------------------------


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

// the addresses
extern const uint8_t AD7147_ADD[4];   

// - declaration of extern public functions ---------------------------------------------------------------------------

void ConfigAD7147(unsigned char Channel,    unsigned int i, unsigned int pw_control_val, if2hw_data_ad7147_t * convalue); //i is the number of the triangle
void ConfigAD7147_THREE(unsigned char Channel,unsigned int i,unsigned int pw_control_val, if2hw_data_ad7147_t *convalue);
void ConfigAD7147_ALL(unsigned char Channel,unsigned int i, unsigned int pw_control_val, if2hw_data_ad7147_t * convalue); //i is the number of the triangle
void ConfigAD7147_onSdaX(unsigned char Channel, unsigned char setNum, unsigned char indexInSet, unsigned int pw_control_val, if2hw_data_ad7147_t cdcoffset);


// - definition of extern public macros ------------------------------------------------------------------------------

 

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

