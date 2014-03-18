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

#ifndef _HL_CRC32_H_
#define _HL_CRC32_H_


// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_crc32.h
    @brief      This header file implements public interface to hl_crc32 using lut, as described by Sven Reifegerste
                at http://zorc.breitbandkatze.de/crc.html
    @author     marco.accame@iit.it
    @date       12/02/2011
**/

/** @defgroup hl_crc32
 
    
    @todo put documentation proper to the entity.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"


// - public #define  --------------------------------------------------------------------------------------------------

// uncomment to save 1024 bytes of rom
//#define _USE_HL_CRC32TABLE_IN_ROM_

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empoty-section

    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------

// rom-mapped crc32 table for polynomial 0x04c11db7
extern const uint32_t hl_crc32_table_0x04c11db7[256];


// - declaration of extern public functions ---------------------------------------------------------------------------
 
/** @fn         extern void hl_crc32_table_get(uint32_t polynomial, uint32_t *hl_crc32table)
    @brief      initialises the crctable for a given quotient polynomial. 
    @param      polynomial      the polynomial.
    @param      hl_crc32table   the crctable which must point to 256 words.
 **/
extern void hl_crc32_table_get(uint32_t polynomial, uint32_t *hl_crc32table);


/** @fn         extern uint32_t hl_crc32_compute(const uint32_t *hl_crc32table, uint32_t initvalue, const uint8_t *data, uint32_t len)
    @brief      computes the crc. 
    @param      hl_crc32table   the crc table to use. if NULL, then the CRC is computed in HW using the 0x04c11db7 polynomial.
    @param      initvalue       the initial value.
    @param      data            the data upon which compute the crc.
    @param      len             its size, at least four
    @return     the crc value, or zero if data is NULL or len is zero.
 **/
extern uint32_t hl_crc32_compute(const uint32_t *hl_crc32table, uint32_t initvalue, const uint8_t *data, uint32_t len);


// - doxy end ---------------------------------------------------------------------------------------------------------

/** @}            
    end of group hl_crc32  
 **/

 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
