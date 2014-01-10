/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _HL_FIFO_H_
#define _HL_FIFO_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_fifo.h
    @brief      this header file gives public interface to fifo sw tools used in hl
    @author     marco.accame@iit.it
    @date       10/29/2013
**/

/** @defgroup arm_hl_fifo HL tools

    The HL ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "hl_common.h"


// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types ------------------------------------------------------------------------- 


typedef uint8_t* (*hl_fifo_fn_itemaddr_t) (void* pool, uint32_t pos);
typedef void (*hl_fifo_fn_itemcopy_t) (void* dst, uint32_t offsetdst, void* src, uint32_t offsetsrc);


/** @typedef    typedef struct hl_fifo_hid_t hl_fifo_t 
    @brief      contains the hidden struct able to hold generic items in a fifo
 **/ 
typedef struct hl_fifo_hid_t hl_fifo_t;


/** @typedef    struct hl_fifo_hid_t 
    @brief      contains implementation of data structure able to hold generic items in a fifo with the limitation 
                of maximum 255 items of maximum size 255.   
 **/ 
struct hl_fifo_hid_t
{
    uint8_t capacity;                   /**< the max possible number of items which can be store inside the fifo */
    uint8_t size;                       /**< the number of items inside the fifo at a given time */
    uint8_t index;                      /**< internals: keeps the ordinal number of next item that can be put inside */
    uint8_t sizeofitem;                 /**< the size in bytes of the item type stored inside teh fifo */
    uint8_t *data;                      /**< the data of the fifo: capacity * sizeofitem bytes which must be passed at construction */
    hl_fifo_fn_itemaddr_t itemaddr;
    hl_fifo_fn_itemcopy_t itemcopy;     /**< internals: function which copies an item in and out from the fifo. */
};  // 16 bytes

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern hl_fifo_t* hl_fifo_new(uint8_t capacity, uint8_t sizeofitem, uint8_t *iteminitval)
    @brief      It allocates memory for a fifo and initialises it. This function uses the heap to obtain the required 
                memory. Use hl_fifo_init() if you want use externally defined memory.
    @param      capacity    The capacity of the fifo.
    @param      sizeofitem  The size in bytes of the item contained in the fifo. If it is 1, 2, 4, 8, or 16 it is used
                            a fast implementation.
    @param      iteminitval Pointer to a default value with which the items are initialised.
                            This parameter can be NULL and in such a case no initialisation is done
                            on the items.
    @return     The fifo pointer or NULL in case of failure.
 **/
extern hl_fifo_t* hl_fifo_new(uint8_t capacity, uint8_t sizeofitem, uint8_t *iteminitval);


/** @fn         extern void hl_fifo_delete(hl_fifo_t *fifo)
    @brief      It releases all memory related to the fifo.
    @param      fifo        The fifo
 **/
extern void hl_fifo_delete(hl_fifo_t *fifo);


/** @fn         extern hl_result_t hl_fifo_init(hl_fifo_t *fifo, uint8_t capacity, uint8_t sizeofitem, uint8_t *dataforfifo, uint8_t *iteminitval)
    @brief      It initialises a fifo. This function requires externally allocated memory with size consistent with the arguments capacity
                and sizeofitem.
    @param      fifo        The fifo pointer.           
    @param      capacity    The capacity of the fifo.
    @param      sizeofitem  The size in bytes of the item contained in the fifo. If it is 1, 2, 4, 8, or 16 it is used
                            a fast implementation.
    @param      dataforfifo Pointer to externally allocated memory which is able to contain capacity items of size sizeofitem.
    @param      iteminitval Pointer to a default value with which the items are initialised.
                            This parameter can be NULL and in such a case no initialisation is done
                            on the items.
    @return     hl_res_OK in case of success.
 **/
extern hl_result_t hl_fifo_init(hl_fifo_t *fifo, uint8_t capacity, uint8_t sizeofitem, uint8_t *dataforfifo, uint8_t *iteminitval);


/** @fn         extern void hl_fifo_reset(hl_fifo_t *fifo)
    @brief      It resets the fifo by setting its size to zero.
    @param      fifo        The fifo
 **/
extern void hl_fifo_reset(hl_fifo_t *fifo);


/** @fn         extern void hl_fifo_clear(hl_fifo_t *fifo)
    @brief      It clears the fifo by zeroing its memory buffer and by setting its size to zero.
    @param      fifo        The fifo
 **/
extern void hl_fifo_clear(hl_fifo_t *fifo);


/** @fn         extern hl_result_t hl_fifo_put(hl_fifo_t *fifo, uint8_t *data)
    @brief      It copies inside the fifo the item pointed by data. If data is NULL, it just increments the fifo size 
                without copying anything inside. It can be used after the user has manipulated directly what returned 
                by hl_fifo_end().
    @param      fifo        The fifo
    @param      data        The pointer to item to be copied inside.
    @return     hl_res_OK on success and hl_res_NOK_generic if the fifo is full.
 **/
extern hl_result_t hl_fifo_put(hl_fifo_t *fifo, uint8_t *data);


/** @fn         extern hl_result_t hl_fifo_get(hl_fifo_t *fifo, uint8_t *data, uint8_t *remaining)
    @brief      It copies inside data the first element of the fifo. After the copy it removed the item from the
                fifo and decrements the internal number. 
                by hl_fifo_end().
    @param      fifo        The fifo
    @param      data        The pointer where to copy the item.
    @param      remaining   If not NULL it contains the number of items remaining inside the fifo 
                            at the return of the function.
    @return     hl_res_OK on success and hl_res_NOK_nodata if the fifo is empty.
 **/
extern hl_result_t hl_fifo_get(hl_fifo_t *fifo, uint8_t *data, uint8_t *remaining);


/** @fn         extern uint8_t * hl_fifo_front(hl_fifo_t *fifo)
    @brief      It returns a pointer to the first item of the fifo or NULL if the fifo is empty.
                The first item is NOT removed after a call to hl_fifo_front().
                The pointer must be used to manipulated ONLY ONE item because there is no guarantee
                that the next item is placed in memory adjacent to the front.                
    @param      fifo        The fifo
    @return     pointer to the first item or NULL if the fifo is empty.
 **/
extern uint8_t * hl_fifo_front(hl_fifo_t *fifo);


/** @fn         extern uint8_t * hl_fifo_end(hl_fifo_t *fifo)
    @brief      It returns a pointer to the next position in which data shall be put or NULL 
                if the fifo is full.   
    @param      fifo        The fifo
    @return     pointer to the first item or NULL if the fifo is full.
 **/
extern uint8_t * hl_fifo_end(hl_fifo_t *fifo);


/** @fn         extern void hl_fifo_pop(hl_fifo_t *fifo)
    @brief      It removed first item from fifo.
    @param      fifo        The fifo
 **/
extern void hl_fifo_pop(hl_fifo_t *fifo);


/** @fn         extern uint8_t hl_fifo_size(hl_fifo_t *fifo)
    @brief      It returns the size of the fifo.   
    @param      fifo        The fifo
    @return     size of fifo.
 **/
extern uint8_t hl_fifo_size(hl_fifo_t *fifo);


/** @fn         extern hl_bool_t hl_fifo_full(hl_fifo_t *fifo)
    @brief      It tells if fifo is full.   
    @param      fifo        The fifo
    @return     hl_true if full.
 **/
extern hl_bool_t hl_fifo_full(hl_fifo_t *fifo);




/** @}            
    end of group arm_hl_fifo  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



