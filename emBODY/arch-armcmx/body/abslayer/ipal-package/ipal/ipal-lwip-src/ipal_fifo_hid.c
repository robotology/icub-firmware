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


/* @file       ipal_fifo_hid.c
	@brief      This file keeps implementation fifo used by ipal to collect 
                packet in rx. the fifo is implemented in optimized way.
	@author     valentina.gaggero@iit.it
    @date       01/10/2013
**/

// - modules to be built: contains the IPAL_USE_* macros --------------------------------------------------------------
#include "ipal_modules.h"

// - sizes of modules: contains the IPAL_NUMBEROF_* macros ----------------------------------------------------------------
#include "ipal_numberof.h"

#ifdef IPAL_USE_FIFO

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stdlib.h"
#include "string.h"

#include "ipal_base_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "ipal_fifo_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------


extern void ipal_fifo_hid_init(ipal_fifo_t *fifo, uint8_t capacity, uint8_t *dataRam)
{
    // avoid checks .... be careful in calling
    
    fifo->capacity = capacity;
    fifo->item_list = (ipal_fifo_item_t*)dataRam;
    memset(fifo->item_list, 0, (capacity * ipal_fifo__itemsize));
    ipal_fifo_hid_reset(fifo);
}


__inline void ipal_fifo_hid_reset(ipal_fifo_t *fifo)
{
    // avoid checks .... be careful in calling
    fifo->size = 0;
    fifo->index = 0;
}


extern ipal_result_t ipal_fifo_hid_put(ipal_fifo_t *fifo, ipal_fifo_item_t *item)
{
    ipal_fifo_item_t *first_free_item = NULL;

    // avoid checks .... be careful in calling
    
    if(fifo->size == fifo->capacity)
    {
        return(ipal_res_NOK_generic);
    }
    /* Since ipal_fifo_item_t has size 4Bytes, we shift of 2 pos, instead of multiply.
    Before shifting, it's important to cast list pointer to uint8_t */ 
    first_free_item = (ipal_fifo_item_t *)(((uint8_t*)fifo->item_list) +(fifo->index << 2));
    
 
    /* Qui suppongo che al di la della size, data e' cmq un puntatore ad un arry di 8, in quanto 
    hal_can_frame_t ha la parte data pre-allocata ci 8 byte.*/
    *(uint32_t*)first_free_item = *((uint32_t*)item);


    fifo->size ++;
    fifo->index ++;
    if(fifo->index == fifo->capacity)
    {
        fifo->index = 0;
    }
    
     return(ipal_res_OK);
}


extern  ipal_fifo_item_t* ipal_fifo_hid_getFirstFree(ipal_fifo_t *fifo)
{
    ipal_fifo_item_t *first_free_item = NULL;

    // avoid checks .... be careful in calling
    
    if(fifo->size == fifo->capacity)
    {
        return(NULL);
    }
    /* Since ipal_fifo_item_t has size 4Bytes, we shift of 2 pos, instead of multiply.
    Before shifting, it's important to cast list pointer to uint8_t */
    first_free_item = (ipal_fifo_item_t*)(((uint8_t*)fifo->item_list) +(fifo->index << 2));

    fifo->size ++;
    fifo->index ++;

    if(fifo->index == fifo->capacity)
    {
        fifo->index = 0;
    }
    return(first_free_item);
}


extern ipal_fifo_item_t* ipal_fifo_hid_front(ipal_fifo_t *fifo)
{
    // avoid checks .... be careful in calling
    
    uint8_t start = NULL;
    
    if(0 == fifo->size)
    {
        return(NULL);
    }
    
    start = (fifo->capacity + fifo->index - fifo->size);
    if(start >= fifo->capacity)
    {
        start -= fifo->capacity;
    }
    /* Since ipal_fifo_item_t has size 4Bytes, we shift of 2 pos, instead of multiply.
    Before shifting, it's important to cast list pointer to uint8_t */   
    return((ipal_fifo_item_t*)(((uint8_t*)fifo->item_list) + (start << 2)));
}


__inline void ipal_fifo_hid_pop(ipal_fifo_t *fifo)
{
    // avoid checks .... be careful in calling

    if(0 != fifo->size)
    {
        fifo->size --;
    }
    
    return;
}



__inline uint8_t ipal_fifo_hid_size(ipal_fifo_t *fifo)
{
    return(fifo->size);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



#endif//IPAL_USE_FIFO

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



