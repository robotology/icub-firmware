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

#ifndef _HAL_UTILITY_FIFO_H_
#define _HAL_UTILITY_FIFO_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_utility_fifo.h
    @brief      this header file gives public interface to some sw tools used in hal
    @author     marco.accame@iit.it
    @date       10/29/2012
**/

/** @defgroup arm_hal_utility_fifo HAL tools

    The HAL ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "hal_base.h"


// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct hal_utility_fifo_hid_t hal_utility_fifo_t;


typedef uint8_t* (*hal_utility_fifo_fn_itemaddr_t) (void* pool, uint32_t pos);
typedef void (*hal_utility_fifo_fn_itemcopy_t) (void* dst, uint32_t offsetdst, void* src, uint32_t offsetsrc);

/** @typedef    typedef struct hal_utility_fifo_t 
    @brief      implements a fifo able to hold generic data size.
 **/ 
struct hal_utility_fifo_hid_t
{
    uint8_t capacity;                           /**< the max possible number of items which can be store inside the fifo */
    uint8_t size;                               /**< the number of items inside the fifo at a given time */
    uint8_t index;                              /**< internals: keeps the ordinal number of next item that can be put inside */
    uint8_t sizeofitem;                         /**< the size in bytes of the item type stored inside teh fifo */
    uint8_t *data;                              /**< the data of the fifo: capacity * sizeofitem bytes which must be passed at construction */
    hal_utility_fifo_fn_itemaddr_t    itemaddr;
    hal_utility_fifo_fn_itemcopy_t    itemcopy;   /**< internals: function which copies an item in and out from the fifo. */
};  // 16 bytes

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


extern void hal_utility_fifo_init(hal_utility_fifo_t *fifo, uint8_t capacity, uint8_t sizeofitem, uint8_t *data, uint8_t *iteminitval);

extern void hal_utility_fifo_reset(hal_utility_fifo_t *fifo);

extern void hal_utility_fifo_clear(hal_utility_fifo_t *fifo);

extern hal_result_t hal_utility_fifo_put(hal_utility_fifo_t *fifo, uint8_t *data);

extern hal_result_t hal_utility_fifo_get(hal_utility_fifo_t *fifo, uint8_t *data, uint8_t *size);

extern uint8_t * hal_utility_fifo_front(hal_utility_fifo_t *fifo);

extern void hal_utility_fifo_pop(hal_utility_fifo_t *fifo);

extern uint8_t hal_utility_fifo_size(hal_utility_fifo_t *fifo);

extern hal_bool_t hal_utility_fifo_full(hal_utility_fifo_t *fifo);


extern uint8_t * hal_utility_fifo_front01(hal_utility_fifo_t *fifo);
extern hal_result_t hal_utility_fifo_get01(hal_utility_fifo_t *fifo, uint8_t *data, uint8_t *size);
extern hal_result_t hal_utility_fifo_put01(hal_utility_fifo_t *fifo, uint8_t *data);

extern uint8_t * hal_utility_fifo_front02(hal_utility_fifo_t *fifo);
extern hal_result_t hal_utility_fifo_get02(hal_utility_fifo_t *fifo, uint8_t *data, uint8_t *size);
extern hal_result_t hal_utility_fifo_put02(hal_utility_fifo_t *fifo, uint8_t *data);

extern uint8_t * hal_utility_fifo_front04(hal_utility_fifo_t *fifo);
extern hal_result_t hal_utility_fifo_get04(hal_utility_fifo_t *fifo, uint8_t *data, uint8_t *size);
extern hal_result_t hal_utility_fifo_put04(hal_utility_fifo_t *fifo, uint8_t *data);

extern uint8_t * hal_utility_fifo_front08(hal_utility_fifo_t *fifo);
extern hal_result_t hal_utility_fifo_get08(hal_utility_fifo_t *fifo, uint8_t *data, uint8_t *size);
extern hal_result_t hal_utility_fifo_put08(hal_utility_fifo_t *fifo, uint8_t *data);

extern uint8_t * hal_utility_fifo_front16(hal_utility_fifo_t *fifo);
extern hal_result_t hal_utility_fifo_get16(hal_utility_fifo_t *fifo, uint8_t *data, uint8_t *size);
extern hal_result_t hal_utility_fifo_put16(hal_utility_fifo_t *fifo, uint8_t *data);


/** @}            
    end of group arm_hal_utility_fifo  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



