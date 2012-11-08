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

#ifndef _HAL_TOOLS_H_
#define _HAL_TOOLS_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_tools.h
    @brief      this header file gives public interface to some sw tools used in hal
    @author     marco.accame@iit.it
    @date       10/29/2012
**/

/** @defgroup arm_hal_tools HAL tools

    The HAL ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "hal_base.h"


// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct hal_tools_fifo_hid_t hal_tools_fifo_t;


typedef uint8_t* (*hal_tools_fifo_fn_itemaddr_t) (void* pool, uint32_t pos);
typedef void (*hal_tools_fifo_fn_itemcopy_t) (void* dst, uint32_t offsetdst, void* src, uint32_t offsetsrc);

/** @typedef    typedef struct hal_tools_fifo_t 
    @brief      implements a fifo able to hold generic data size.
 **/ 
struct hal_tools_fifo_hid_t
{
    uint8_t capacity;                           /**< the max possible number of items which can be store inside the fifo */
    uint8_t size;                               /**< the number of items inside the fifo at a given time */
    uint8_t index;                              /**< internals: keeps the ordinal number of next item that can be put inside */
    uint8_t sizeofitem;                         /**< the size in bytes of the item type stored inside teh fifo */
    uint8_t *data;                              /**< the data of the fifo: capacity * sizeofitem bytes which must be passed at construction */
    hal_tools_fifo_fn_itemaddr_t    itemaddr;
    hal_tools_fifo_fn_itemcopy_t    itemcopy;   /**< internals: function which copies an item in and out from the fifo. */
};  // 16 bytes

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


extern void hal_tools_fifo_init(hal_tools_fifo_t *fifo, uint8_t capacity, uint8_t sizeofitem, uint8_t *data, uint8_t *iteminitval);

extern void hal_tools_fifo_reset(hal_tools_fifo_t *fifo);

extern void hal_tools_fifo_clear(hal_tools_fifo_t *fifo);

extern hal_result_t hal_tools_fifo_put(hal_tools_fifo_t *fifo, uint8_t *data);

extern hal_result_t hal_tools_fifo_get(hal_tools_fifo_t *fifo, uint8_t *data, uint8_t *size);

extern uint8_t * hal_tools_fifo_front(hal_tools_fifo_t *fifo);

extern void hal_tools_fifo_pop(hal_tools_fifo_t *fifo);

extern uint8_t hal_tools_fifo_size(hal_tools_fifo_t *fifo);

extern hal_bool_t hal_tools_fifo_full(hal_tools_fifo_t *fifo);


extern uint8_t * hal_tools_fifo_front01(hal_tools_fifo_t *fifo);
extern hal_result_t hal_tools_fifo_get01(hal_tools_fifo_t *fifo, uint8_t *data, uint8_t *size);
extern hal_result_t hal_tools_fifo_put01(hal_tools_fifo_t *fifo, uint8_t *data);

extern uint8_t * hal_tools_fifo_front02(hal_tools_fifo_t *fifo);
extern hal_result_t hal_tools_fifo_get02(hal_tools_fifo_t *fifo, uint8_t *data, uint8_t *size);
extern hal_result_t hal_tools_fifo_put02(hal_tools_fifo_t *fifo, uint8_t *data);

extern uint8_t * hal_tools_fifo_front04(hal_tools_fifo_t *fifo);
extern hal_result_t hal_tools_fifo_get04(hal_tools_fifo_t *fifo, uint8_t *data, uint8_t *size);
extern hal_result_t hal_tools_fifo_put04(hal_tools_fifo_t *fifo, uint8_t *data);

extern uint8_t * hal_tools_fifo_front08(hal_tools_fifo_t *fifo);
extern hal_result_t hal_tools_fifo_get08(hal_tools_fifo_t *fifo, uint8_t *data, uint8_t *size);
extern hal_result_t hal_tools_fifo_put08(hal_tools_fifo_t *fifo, uint8_t *data);

extern uint8_t * hal_tools_fifo_front16(hal_tools_fifo_t *fifo);
extern hal_result_t hal_tools_fifo_get16(hal_tools_fifo_t *fifo, uint8_t *data, uint8_t *size);
extern hal_result_t hal_tools_fifo_put16(hal_tools_fifo_t *fifo, uint8_t *data);



inline extern hal_boolval_t hal_tools_bitoperator_byte_bitcheck(uint8_t byte, uint8_t bit)
{
    if(byte & (1<<bit))
    {
        return(hal_true);
    }
    else
    {
        return(hal_false);
    }  
}

inline extern void hal_tools_bitoperator_byte_bitset(uint8_t* byte, uint8_t bit)
{
    (*byte) |= (1<<bit);    
}

inline extern void hal_tools_bitoperator_byte_bitclear(uint8_t* byte, uint8_t bit)
{
    (*byte) &= (~(1<<bit));    
}

inline extern void hal_tools_bitoperator_byte_bittoggle(uint8_t* byte, uint8_t bit)
{
    (*byte) ^= (1<<bit);    
}

inline extern hal_boolval_t hal_tools_bitoperator_halfword_bitcheck(uint16_t hword, uint8_t bit)
{
    if(hword & (1<<bit))
    {
        return(hal_true);
    }
    else
    {
        return(hal_false);
    }
}

inline extern void hal_tools_bitoperator_halfword_bitset(uint16_t* hword, uint8_t bit)
{
    (*hword) |= (1<<bit);    
}

inline extern void hal_tools_bitoperator_halfword_bitclear(uint16_t* hword, uint8_t bit)
{
    (*hword) &= (~(1<<bit));    
}

inline extern void hal_tools_bitmanip_halfword_bittoggle(uint16_t* hword, uint8_t bit)
{
    (*hword) ^= (1<<bit);    
}

inline extern hal_boolval_t hal_tools_bitoperator_word_bitcheck(uint32_t word, uint8_t bit)
{
    if(word & (1<<bit))
    {
        return(hal_true);
    }
    else
    {
        return(hal_false);
    }
}

inline extern void hal_tools_bitoperator_word_bitset(uint32_t* word, uint8_t bit)
{
    (*word) |= (1<<bit);    
}

inline extern void hal_tools_bitoperator_word_bitclear(uint32_t* word, uint8_t bit)
{
    (*word) &= (~(1<<bit));    
}

inline extern void hal_tools_bitoperator_word_bittoggle(uint32_t* word, uint8_t bit)
{
    (*word) ^= (1<<bit);    
}


/** @}            
    end of group arm_hal_tools  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



