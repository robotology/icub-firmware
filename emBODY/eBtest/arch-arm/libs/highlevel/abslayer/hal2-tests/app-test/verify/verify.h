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

#ifndef _VERIFY_H_
#define _VERIFY_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       verify.h
    @brief      this header file gives public interface to a circular buffer
    @author     marco.accame@iit.it
    @date       03/07/2013
**/

/** @defgroup arm_hal_utility_circular HAL tools

    The HAL ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "circular.h"

// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct
{
    int32_t     pos;
    int32_t     vel;   
} verify_pair_t;

typedef struct verify_hid_t verify_t;

struct verify_hid_t
{
    uint8_t synched;                            // equal to 1 only if we have the latest input positions equal to teh reference    
    uint8_t capacity;                           /**< the max possible number of items which can be store inside the circular buffer */
    const verify_pair_t *reference;             /**< the data of the buffer: capacity * sizeofitem bytes which must be passed at construction */
    circular_t received;
    circular_t synchref;
    verify_pair_t *tmp;
};  

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

extern void verify_init(verify_t *verif, uint8_t capacity, const verify_pair_t *reference);

// returns 0 if latest capacity positions are not equal to the reference, return 1 if they are, return 255 if still synch
extern uint8_t verify_add(verify_t *verif, verify_pair_t pair);



/** @}            
    end of group arm_hal_utility_circular  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



