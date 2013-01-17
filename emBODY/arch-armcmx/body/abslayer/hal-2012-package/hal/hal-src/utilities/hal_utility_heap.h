/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _HAL_UTILITY_HEAP_H_
#define _HAL_UTILITY_HEAP_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_utility_heap.h
    @brief      this header file gives public interface to some sw tools used in hal
    @author     marco.accame@iit.it
    @date       01/17/2013
**/

/** @defgroup arm_hal_utility_heap HAL tools

    The HAL ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "hal_base.h"


// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


extern void* hal_utility_heap_new(uint32_t size);

extern void hal_utility_heap_delete(void** p);


/** @}            
    end of group arm_hal_utility_heap  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



