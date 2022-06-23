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
#ifndef _IF2HW_COMMON_H_
#define _IF2HW_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif


  

// - external dependencies --------------------------------------------------------------------------------------------

#include "stm32hal.h"


// - public #define  --------------------------------------------------------------------------------------------------

// it forces AD7147 and I2C_Multi_SDA to use U32 or U16 data. 
// the logical choice is U16 as everywhere the code uses only 2 bytes.
#define if2hw_common_SKIN_USE_U16
  
// it shapes code to use either internal or externally allocated buffers ...    
#define if2hw_common_AD7147_USE_EXTERNALDATA
    
// it shapes the code to use or not the data structure for controllling config of triangles as defined in AD7147
#define if2hw_common_AD7147_USE_EXTERNALCONTROLTYPES    


// - declaration of public user-defined types -------------------------------------------------------------------------


#if defined(if2hw_common_SKIN_USE_U32)

typedef uint32_t if2hw_data_ad7147_t;
typedef uint32_t if2hw_data_i2cmultisda_t;

#elif defined(if2hw_common_SKIN_USE_U16)
    
typedef uint16_t if2hw_data_ad7147_t;
typedef uint16_t if2hw_data_i2cmultisda_t;

#else
    
#error YOU MUST define either if2hw_common_SKIN_USE_U32 or if2hw_common_SKIN_USE_U16

#endif


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------


// - declaration of extern public functions ---------------------------------------------------------------------------

//extern void if2hw_common_delay(uint64_t usec);



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

