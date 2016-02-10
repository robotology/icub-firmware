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


/* @file       hl_bits.c
    @brief      this file implements internals of a tool module used in hl
    @author     marco.accame@iit.it
    @date       10/29/2013
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------

#include "hl_cfg_plus_modules.h"

#if     defined(HL_USE_UTIL_BITS)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_bits.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_bits_hid.h" 


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



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

// see http://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer
static const uint8_t s_hl_bits_oneBitsInU8[256] = 
{
//  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F (<- n)
//  =====================================================
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, // 0n
    
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, // 1n (+1)
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, // 2n (+1)
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, // 3n (+2)    
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, // 4n (+1)
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, // 3n (+2)
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, // 6n (+2)
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, // 7n (+3)
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, // 8n (+1)
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, // 9n (+2)
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, // An (+2)
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, // Bn (+3)
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, // Cn (+2)
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, // Dn (+3)
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, // En (+3)  
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8  // Fn (+4)
};

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hl_boolval_t hl_bits_byte_bitcheck(uint8_t byte, uint8_t bit)
{
    if(byte & (1<<bit))
    {
        return(hl_true);
    }
    else
    {
        return(hl_false);
    }  
}


extern void hl_bits_byte_bitset(uint8_t* byte, uint8_t bit)
{
    (*byte) |= (1<<bit);    
}


extern void hl_bits_byte_bitclear(uint8_t* byte, uint8_t bit)
{
    (*byte) &= (~(1<<bit));    
}


extern void hl_bits_byte_bittoggle(uint8_t* byte, uint8_t bit)
{
    (*byte) ^= (1<<bit);    
}


extern hl_boolval_t hl_bits_hlfword_bitcheck(uint16_t hword, uint8_t bit)
{
    if(hword & (1<<bit))
    {
        return(hl_true);
    }
    else
    {
        return(hl_false);
    }
}


extern hl_boolval_t hl_bits_hlfword_maskcheck(uint16_t hword, uint16_t mask)
{
    if(hword & mask)
    {
        return(hl_true);
    }
    else
    {
        return(hl_false);
    }
}


extern void hl_bits_hlfword_bitset(uint16_t* hword, uint8_t bit)
{
    (*hword) |= (1<<bit);    
}


extern void hl_bits_hlfword_maskset(uint16_t* hword, uint16_t mask)
{
    (*hword) |= (mask);    
}


extern void hl_bits_hlfword_bitclear(uint16_t* hword, uint8_t bit)
{
    (*hword) &= (~(1<<bit));    
}

extern void hl_bits_hlfword_maskclear(uint16_t* hword, uint16_t mask)
{
    (*hword) &= (~(mask));    
}


extern void hl_bits_hlfword_bittoggle(uint16_t* hword, uint8_t bit)
{
    (*hword) ^= (1<<bit);    
}


extern hl_boolval_t hl_bits_word_bitcheck(uint32_t word, uint8_t bit)
{
    if(word & (1<<bit))
    {
        return(hl_true);
    }
    else
    {
        return(hl_false);
    }
}


extern void hl_bits_word_bitset(uint32_t* word, uint8_t bit)
{
    (*word) |= (1<<bit);    
}


extern void hl_bits_word_bitclear(uint32_t* word, uint8_t bit)
{
    (*word) &= (~(1<<bit));    
}


extern void hl_bits_word_bittoggle(uint32_t* word, uint8_t bit)
{
    (*word) ^= (1<<bit);    
}


extern uint8_t hl_bits_byte_bitsetcount(uint8_t byte)
{
    return(s_hl_bits_oneBitsInU8[byte & 0xff]);
}


extern uint8_t hl_bits_hlfword_bitsetcount(uint16_t hword)
{
    return(hl_bits_byte_bitsetcount(hword&0xff)+hl_bits_byte_bitsetcount((hword>>8)&0xff));
}


extern uint8_t hl_bits_word_bitsetcount(uint32_t word)
{
    return(hl_bits_hlfword_bitsetcount(word&0xffff) + hl_bits_hlfword_bitsetcount((word>>16)&0xffff));
}


extern uint8_t hl_bits_dword_bitsetcount(uint64_t dword)
{
    return(hl_bits_word_bitsetcount(dword&0xffffffff) + hl_bits_word_bitsetcount((dword>>32)&0xffffffff));
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----
// empty-section
// ---- isr of the module: end ------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




#endif//defined(HL_USE_UTIL_BITS)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



