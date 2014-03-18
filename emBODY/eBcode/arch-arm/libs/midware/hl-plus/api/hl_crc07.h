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

#ifndef _HL_CRC07_H_
#define _HL_CRC07_H_


// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_crc07.h
    @brief      This header file implements public interface to hl_crc07 using a LUT, as described by Sven Reifegerste
                at http://zorc.breitbandkatze.de/crc.html
    @author     marco.accame@iit.it
    @date       02/18/2013
**/

/** @defgroup hl_crc07
 
    
    @todo put documentation proper to the entity.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------

// rom-mapped crc7 table for polynomial 0x09
extern const uint8_t hl_crc07_table_0x09[256];


// - declaration of extern public functions ---------------------------------------------------------------------------
 
/** @fn         extern void hl_crc07_table_get(uint8_t polynomial, uint8_t *hl_crc07table)
    @brief      initialises the crctable for a given quotient polynomial. 
    @param      polynomial      the polynomial.
    @param      hl_crc07table   the crctable which must point to 256 bytes.
 **/
extern void hl_crc07_table_get(uint8_t polynomial, uint8_t *hl_crc07table);


/** @fn         extern uint8_t hl_crc07_compute(const uint8_t *hl_crc07table, uint8_t initvalue, const uint8_t *data, uint32_t len)
    @brief      computes the crc. 
    @param      hl_crc07table   the crc table to use. it cannot be NULL.
    @param      initvalue       the initial value.
    @param      data            the data upon which compute the crc.
    @param      len             its size, at least four
    @return     the crc value, or zero if any of hl_crc07table or data is NULL or len is zero.
 **/
extern uint8_t hl_crc07_compute(const uint8_t *hl_crc07table, uint8_t initvalue, const uint8_t *data, uint32_t len);


// - doxy end ---------------------------------------------------------------------------------------------------------

/** @}            
    end of group hl_crc07  
 **/

 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


