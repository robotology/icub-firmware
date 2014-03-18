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

#ifndef _HL_CRC16_H_
#define _HL_CRC16_H_


// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_crc16.h
    @brief      This header file implements public interface to hl_crc16 using lut, as described by Sven Reifegerste
                at http://zorc.breitbandkatze.de/crc.html
    @author     marco.accame@iit.it
    @date       12/02/2011
**/

/** @defgroup hl_crc16
 
    
    @todo put documentation proper to the entity.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"



// - public #define  --------------------------------------------------------------------------------------------------

  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empoty-section

    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------

// rom-mapped crc16 table for polynomial 0x1021
extern const uint16_t hl_crc16_table_0x1021[256];


// - declaration of extern public functions ---------------------------------------------------------------------------
 
/** @fn         extern void hl_crc16_table_get(uint16_t polynomial, uint16_t *hl_crc16table)
    @brief      initialises the crctable for a given quotient polynomial. 
    @param      polynomial      the polynomial.
    @param      hl_crc16table   the crctable which must point to 256 half words.
 **/    
extern void hl_crc16_table_get(uint16_t polynomial, uint16_t *hl_crc16table);


/** @fn         extern uint16_t hl_crc16_compute(const uint16_t *hl_crc16table, uint16_t initvalue, const uint8_t *data, uint32_t len)
    @brief      computes the crc. 
    @param      hl_crc16table   the crc table to use. cannot be NULL.
    @param      initvalue       the initial value.
    @param      data            the data upon which compute the crc.
    @param      len             its size, at least four
    @return     the crc value, or zero if any of hl_crc16table or data is NULL or len is zero.
 **/
extern uint16_t hl_crc16_compute(const uint16_t *hl_crc16table, uint16_t initvalue, const uint8_t *data, uint32_t len);


// - doxy end ---------------------------------------------------------------------------------------------------------

/** @}            
    end of group hl_crc16  
 **/

 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


