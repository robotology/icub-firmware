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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _HAL_STM32_CAN_FIFO_OPTIMIZED_HID_H_
#define _HAL_STM32_CAN_FIFO_OPTIMIZED_HID_H_


/* @file       hal_stm32_can_fifo_optimezed_hid.h
    @brief      This header file implements hidden interface can-fifo.
    @author     valentina.gaggero@iit.it
    @date       02/16/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "stdlib.h"
#include "stm32f1.h"

#include "hal_base.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
//this module is not public


// - #define used with hidden structs ---------------------------------------------------------------------------------
#define hal_canfifo__itemsize sizeof(CanRxMsg)

// - definition of hidden structs -------------------------------------------------------------------------------------
typedef   CanRxMsg     hal_canfifo_item_t;

typedef struct
{
    uint8_t                 capacity;
    uint8_t                 size;
    uint8_t                 index;
    hal_canfifo_item_t      *canFrame_list;
} hal_canfifo_t;

// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern hal_cfg_t hal_base_hid_params;


// - declaration of extern hidden functions ---------------------------------------------------------------------------
/* init fifo with pre-allocated data ram */
extern void hal_canfifo_hid_init(hal_canfifo_t *fifo, uint8_t capacity, uint8_t *dataRam);
extern void hal_canfifo_hid_reset(hal_canfifo_t *fifo);
extern hal_result_t hal_canfifo_hid_put(hal_canfifo_t *fifo, uint32_t id, uint8_t datasize, uint8_t *data);
//extern hal_result_t hal_canfifo_hid_get(hal_genericfifo_t *gf, hal_canfifo_item_t *data, uint8_t *size);
extern hal_canfifo_item_t * hal_canfifo_hid_front(hal_canfifo_t*fifo);
extern void hal_canfifo_hid_pop(hal_canfifo_t*fifo);
extern uint8_t hal_canfifo_hid_size(hal_canfifo_t*gf);
/* return the pointer to the first free item and increm the size of fifo,
because it supposes that, at one the user has the poiter, the user fills item's field.*/
extern hal_canfifo_item_t* hal_canfifo_hid_getFirstFree(hal_canfifo_t *fifo);


// - definition of extern hidden inline functions ---------------------------------------------------------------------

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




