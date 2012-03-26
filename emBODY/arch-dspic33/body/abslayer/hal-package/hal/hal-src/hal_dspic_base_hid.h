/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
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

#ifndef _HAL_DSPIC_BASE_HID_H_
#define _HAL_DSPIC_BASE_HID_H_


/* @file       hal_dspic_base_hid.h
    @brief      This header file implements hidden interface to hal.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/12/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------



// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_base.h"


// - #define used with hidden structs ---------------------------------------------------------------------------------
// empty-section

// - definition of hidden structs -------------------------------------------------------------------------------------

typedef struct
{
    uint8_t capacity;
    uint8_t size;
    uint8_t index;
    uint8_t sizeofitem;
    uint8_t *data;
} hal_genericfifo_t;

// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern hal_cfg_t hal_base_hid_params;


// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t hal_base_hid_getsize(const hal_cfg_t *cfg);
extern hal_result_t hal_base_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory);

extern void hal_base_hid_on_fatalerror(hal_fatalerror_t errorcode, const char * errormsg);

extern void hal_base_hid_genericfifo_init(hal_genericfifo_t *gf, uint8_t capacity, uint8_t sizeofitem, uint8_t *data);
extern void hal_base_hid_genericfifo_reset(hal_genericfifo_t *gf);
extern hal_result_t hal_base_hid_genericfifo_put(hal_genericfifo_t *gf, uint8_t *data);
extern hal_result_t hal_base_hid_genericfifo_get(hal_genericfifo_t *gf, uint8_t *data, uint8_t *size);
extern uint8_t * hal_base_hid_genericfifo_front(hal_genericfifo_t *gf);
extern void hal_base_hid_genericfifo_pop(hal_genericfifo_t *gf);
extern uint8_t hal_base_hid_genericfifo_size(hal_genericfifo_t *gf);

extern void hal_base_hid_osal_scheduling_suspend(void);
extern void hal_base_hid_osal_scheduling_restart(void);


// - definition of extern hidden inline functions ---------------------------------------------------------------------


inline extern hal_boolval_t hal_base_hid_byte_bitcheck(uint8_t byte, uint8_t bit)
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

inline extern void hal_base_hid_byte_bitset(uint8_t* byte, uint8_t bit)
{
    (*byte) |= (1<<bit);    
}

inline extern void hal_base_hid_byte_bitclear(uint8_t* byte, uint8_t bit)
{
    (*byte) &= (~(1<<bit));    
}

inline extern void hal_base_hid_byte_bittoggle(uint8_t* byte, uint8_t bit)
{
    (*byte) ^= (1<<bit);    
}

inline extern hal_boolval_t hal_base_hid_halfword_bitcheck(uint16_t hword, uint8_t bit)
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

inline extern void hal_base_hid_halfword_bitset(uint16_t* hword, uint8_t bit)
{
    (*hword) |= (1<<bit);    
}

inline extern void hal_base_hid_halfword_bitclear(uint16_t* hword, uint8_t bit)
{
    (*hword) &= (~(1<<bit));    
}

inline extern void hal_base_hid_halfword_bittoggle(uint16_t* hword, uint8_t bit)
{
    (*hword) ^= (1<<bit);    
}

inline extern hal_boolval_t hal_base_hid_word_bitcheck(uint32_t word, uint8_t bit)
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

inline extern void hal_base_hid_word_bitset(uint32_t* word, uint8_t bit)
{
    (*word) |= (1<<bit);    
}

inline extern void hal_base_hid_word_bitclear(uint32_t* word, uint8_t bit)
{
    (*word) &= (~(1<<bit));    
}

inline extern void hal_base_hid_word_bittoggle(uint32_t* word, uint8_t bit)
{
    (*word) ^= (1<<bit);    
}

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




