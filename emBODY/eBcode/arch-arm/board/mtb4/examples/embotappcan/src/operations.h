
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
#ifndef _OPERATIONS_H_
#define _OPERATIONS_H_

#ifdef __cplusplus
extern "C" {
#endif
 
 
/** @file       operation.h
    @brief      This header file is just an example of use of a c module. please use this syntax.
    @author     marco.accame@iit.it
    @date       08/03/2011
**/




// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 



typedef enum
{
    oper_NONE   = 0,
    oper_ONE    = 1,
    oper_TWO    = 2   
} operations_Type_t;

enum { oper_numberof = 2 };

typedef struct 
{
    operations_Type_t   type;
    uint32_t            one;
    uint16_t            two;
    uint16_t            three;    
} operations_Data_t;



    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
extern void operations_test00(operations_Data_t *data);




#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


