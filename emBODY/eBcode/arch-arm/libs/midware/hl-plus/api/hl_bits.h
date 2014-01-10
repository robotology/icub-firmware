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

#ifndef _HL_BITS_H_
#define _HL_BITS_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_bits.h
    @brief      this header file gives public interface to some bit tools used in hl
    @author     marco.accame@iit.it
    @date       10/29/2013
**/

/** @defgroup arm_hl_bits HL bit tools

    The HL ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"


// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


extern hl_boolval_t hl_bits_byte_bitcheck(uint8_t byte, uint8_t bit);

extern void hl_bits_byte_bitset(uint8_t* byte, uint8_t bit);

extern void hl_bits_byte_bitclear(uint8_t* byte, uint8_t bit);

extern void hl_bits_byte_bittoggle(uint8_t* byte, uint8_t bit);



extern hl_boolval_t hl_bits_hlfword_bitcheck(uint16_t hword, uint8_t bit);

extern void hl_bits_hlfword_bitset(uint16_t* hword, uint8_t bit);

extern void hl_bits_hlfword_bitclear(uint16_t* hword, uint8_t bit);

extern void hl_bits_hlfword_bittoggle(uint16_t* hword, uint8_t bit);



extern hl_boolval_t hl_bits_word_bitcheck(uint32_t word, uint8_t bit);

extern void hl_bits_word_bitset(uint32_t* word, uint8_t bit);

extern void hl_bits_word_bitclear(uint32_t* word, uint8_t bit);

extern void hl_bits_word_bittoggle(uint32_t* word, uint8_t bit);


/** @}            
    end of group arm_hl_bits  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



