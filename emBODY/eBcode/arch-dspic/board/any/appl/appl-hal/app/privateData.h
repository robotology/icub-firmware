/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
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

#ifndef _PRIVATEDATA_H_
#define _PRIVATEDATA_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       privateData.h
    @brief      this file ...
    @author     valentina.gaggero@iit.it
    @date       27/01/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct
{
    uint8_t dummy1[6];
    uint8_t dummy2[6];
    uint8_t dummy3[6];
    uint8_t dummy4[6];
} ap_private_datastruct_t;


    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern hal_result_t ap_privateData_init(void);

extern hal_result_t ap_privateData_get(ap_private_datastruct_t *privdata);

extern hal_result_t ap_privateData_set(ap_private_datastruct_t *privdata);


// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

