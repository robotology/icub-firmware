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
#ifndef _IPAL_FIFO_HID_H_
#define _IPAL_FIFO_HID_H_


/* @file       ipal_fifo_hid.h
    @brief      This header file implements hidden interface ipal-fifo.
    @author     valentina.gaggero@iit.it
    @date       01/10/2013
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "stdlib.h"
//#include "stm32f1.h"




// - declaration of extern public interface ---------------------------------------------------------------------------
//this module is not public


// - #define used with hidden structs ---------------------------------------------------------------------------------
#define ipal_fifo__itemsize sizeof(void*)

// - definition of hidden structs -------------------------------------------------------------------------------------
typedef   void*     ipal_fifo_item_t;

typedef struct
{
    uint8_t                 capacity;
    uint8_t                 size;
    uint8_t                 index;
    ipal_fifo_item_t        *item_list;
} ipal_fifo_t;

// - declaration of extern hidden variables ---------------------------------------------------------------------------



// - declaration of extern hidden functions ---------------------------------------------------------------------------
/* init fifo with pre-allocated data ram */
extern void ipal_fifo_hid_init(ipal_fifo_t *fifo, uint8_t capacity, uint8_t *dataRam);
extern void ipal_fifo_hid_reset(ipal_fifo_t *fifo);
extern ipal_result_t ipal_fifo_hid_put(ipal_fifo_t *fifo, ipal_fifo_item_t *item);
//extern ipal_result_t ipal_fifo_hid_get(hal_genericfifo_t *gf, ipal_fifo_item_t *data, uint8_t *size);
extern ipal_fifo_item_t * ipal_fifo_hid_front(ipal_fifo_t*fifo);
extern void ipal_fifo_hid_pop(ipal_fifo_t*fifo);
extern uint8_t ipal_fifo_hid_size(ipal_fifo_t*gf);

/* return the pointer to the first free item and increm the size of fifo,
because it supposes that, at one the user has the poiter, the user fills item's field.*/
extern ipal_fifo_item_t* ipal_fifo_hid_getFirstFree(ipal_fifo_t *fifo);


// - definition of extern hidden inline functions ---------------------------------------------------------------------

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




