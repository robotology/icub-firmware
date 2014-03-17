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

#ifndef _HAL_UTILITY_BITS_H_
#define _HAL_UTILITY_BITS_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_utility_bits.h
    @brief      this header file gives public interface to some bit tools used in hal
    @author     marco.accame@iit.it
    @date       10/29/2012
**/

/** @defgroup arm_hal_utility_bits HAL bit tools

    The HAL ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "hal_base.h"


// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types ------------------------------------------------------------------------- 

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


inline extern hal_boolval_t hal_utility_bits_byte_bitcheck(uint8_t byte, uint8_t bit)
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

inline extern void hal_utility_bits_byte_bitset(uint8_t* byte, uint8_t bit)
{
    (*byte) |= (1<<bit);    
}

inline extern void hal_utility_bits_byte_bitclear(uint8_t* byte, uint8_t bit)
{
    (*byte) &= (~(1<<bit));    
}

inline extern void hal_utility_bits_byte_bittoggle(uint8_t* byte, uint8_t bit)
{
    (*byte) ^= (1<<bit);    
}

inline extern hal_boolval_t hal_utility_bits_halfword_bitcheck(uint16_t hword, uint8_t bit)
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


inline extern hal_boolval_t hal_utility_bits_halfword_maskcheck(uint16_t hword, uint16_t mask)
{
    if(hword & mask)
    {
        return(hal_true);
    }
    else
    {
        return(hal_false);
    }
}

inline extern void hal_utility_bits_halfword_bitset(uint16_t* hword, uint8_t bit)
{
    (*hword) |= (1<<bit);    
}

inline extern void hal_utility_bits_halfword_maskset(uint16_t* hword, uint16_t mask)
{
    (*hword) |= (mask);    
}


inline extern void hal_utility_bits_halfword_bitclear(uint16_t* hword, uint8_t bit)
{
    (*hword) &= (~(1<<bit));    
}

inline extern void hal_utility_bits_halfword_maskclear(uint16_t* hword, uint16_t mask)
{
    (*hword) &= (~(mask));    
}

inline extern void hal_tools_bitmanip_halfword_bittoggle(uint16_t* hword, uint8_t bit)
{
    (*hword) ^= (1<<bit);    
}

inline extern hal_boolval_t hal_utility_bits_word_bitcheck(uint32_t word, uint8_t bit)
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

inline extern void hal_utility_bits_word_bitset(uint32_t* word, uint8_t bit)
{
    (*word) |= (1<<bit);    
}

inline extern void hal_utility_bits_word_bitclear(uint32_t* word, uint8_t bit)
{
    (*word) &= (~(1<<bit));    
}

inline extern void hal_utility_bits_word_bittoggle(uint32_t* word, uint8_t bit)
{
    (*word) ^= (1<<bit);    
}


/** @}            
    end of group arm_hal_utility_bits  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



