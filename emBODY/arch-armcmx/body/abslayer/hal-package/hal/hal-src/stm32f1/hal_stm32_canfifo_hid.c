/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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

/* @file       hal_stm32_can_fifo_optimized_hid.c
	@brief      This file keeps implementation fifo used by can module to collect 
                frame in tx and rx. the fifo is implemented in optimized way.
	@author     valentina.gaggero@iit.it
    @date       02/16/2012
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_CAN

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stdlib.h"
#include "string.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_stm32_canfifo_hid.h"


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


extern void hal_canfifo_hid_init(hal_canfifo_t *fifo, uint8_t capacity, uint8_t *dataRam)
{
    // avoid checks .... be careful in calling
    uint8_t i;
        
    fifo->capacity = capacity;
    fifo->canFrame_list = (hal_canfifo_item_t*)dataRam;

    for(i=0; i<capacity; i++)
    {
        fifo->canFrame_list[i].IDE   = CAN_ID_STD;     //only stdid are managed
        fifo->canFrame_list[i].ExtId = 0;              // since frame-id is std it is not used by stm32lib
        fifo->canFrame_list[i].RTR   = CAN_RTR_DATA;   //only data frame are managed
    }

    hal_canfifo_hid_reset(fifo);
}


__inline void hal_canfifo_hid_reset(hal_canfifo_t *fifo)
{
    // avoid checks .... be careful in calling
    fifo->size = 0;
    fifo->index = 0;
}


extern hal_result_t hal_canfifo_hid_put(hal_canfifo_t *fifo, uint32_t id, uint8_t datasize, uint8_t *data)
{
    hal_canfifo_item_t *first_free_item = NULL;
    // avoid checks .... be careful in calling
    
    if(fifo->size == fifo->capacity)
    {
        return(hal_res_NOK_busy);
    }
    /*ATTERNZIONE: dato che gli item di questa coda sono di tipo CanRxMsg la cui size e' 16 byte, invece di fare la moltiplicazione faccio <<4 */
    first_free_item = fifo->canFrame_list +(fifo->index << 4);
    first_free_item->StdId = id & 0x7FF;
    first_free_item->DLC = datasize;
 
    /* Qui suppongo che al di la della size, data e' cmq un puntatore ad un arry di 8, in quanto 
    hal_can_frame_t ha la parte data pre-allocata ci 8 byte.*/
    *(uint64_t*)first_free_item->Data = *((uint64_t*)data);

    fifo->size ++;
    fifo->index ++;
    if(fifo->index == fifo->capacity)
    {
        fifo->index = 0;
    }
    
     return(hal_res_OK);
}


extern  hal_canfifo_item_t* hal_canfifo_hid_getFirstFree(hal_canfifo_t *fifo)
{
    hal_canfifo_item_t *first_free_item = NULL;
    // avoid checks .... be careful in calling
    
    if(fifo->size == fifo->capacity)
    {
        return(NULL);
    }

    first_free_item = fifo->canFrame_list +(fifo->index << 4);

    fifo->size ++;
    fifo->index ++;

    if(fifo->index == fifo->capacity)
    {
        fifo->index = 0;
    }
    return(first_free_item);
}


extern hal_canfifo_item_t* hal_canfifo_hid_front(hal_canfifo_t *fifo)
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
    
    return((hal_canfifo_item_t*)(fifo->canFrame_list + (start << 4)));
}


__inline void hal_canfifo_hid_pop(hal_canfifo_t *fifo)
{
    // avoid checks .... be careful in calling

    if(0 != fifo->size)
    {
        fifo->size --;
    }
    
    return;
}



__inline uint8_t hal_canfifo_hid_size(hal_canfifo_t *fifo)
{
    return(fifo->size);
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



#endif//HAL_USE_BASE

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



