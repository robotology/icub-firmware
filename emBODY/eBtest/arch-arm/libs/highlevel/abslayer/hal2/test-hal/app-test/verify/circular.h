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

#ifndef _CIRCULAR_H_
#define _CIRCULAR_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       utility_circular.h
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


// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct circular_hid_t circular_t;

struct circular_hid_t
{
    uint8_t capacity;                           /**< the max possible number of items which can be store inside the circular buffer */
    uint8_t size;
    uint8_t earliest;                            /**< the earliest */
    uint8_t index;                              /**< internals: keeps the ordinal number of next item that can be put inside */
    uint8_t sizeofitem;                         /**< the size in bytes of the item type stored inside the circular buffer */
    uint8_t *data;                              /**< the data of the buffer: capacity * sizeofitem bytes which must be passed at construction */
};  

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

extern void circular_init(circular_t *circ, uint8_t capacity, uint8_t sizeofitem);

extern void circular_reset(circular_t *circ);

extern void circular_clear(circular_t *circ);

// if data is null, then it just advance without copying
extern void circular_put(circular_t *circ, uint8_t *data);

extern void circular_get_from_earliest(circular_t *circ, uint8_t pos, uint8_t *data);

extern void circular_get_from_latest_minus(circular_t *circ, uint8_t pos, uint8_t *data);

/** @}            
    end of group arm_hal_utility_circular  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



