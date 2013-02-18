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

#ifndef _HAL_UTILITY_CRC32_H_
#define _HAL_UTILITY_CRC32_H_


// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_utility_crc32.h
    @brief      This header file implements public interface to hal_utility_crc32 using lut, as described by Sven Reifegerste
                at http://zorc.breitbandkatze.de/crc.html
    @author     marco.accame@iit.it
    @date       12/02/2011
**/

/** @defgroup hal_utility_crc32
 
    
    @todo put documentation proper to the entity.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "hal_base.h"


// - public #define  --------------------------------------------------------------------------------------------------

// uncomment to save 1024 bytes of rom
//#define _USE_HAL_UTILITY_CRC32TABLE_IN_ROM_

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empoty-section

    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------

extern const uint32_t hal_utility_crc32_table_0x04c11db7[256];


// - declaration of extern public functions ---------------------------------------------------------------------------
 
/** @fn         extern void hal_utility_crc32_table_get(uint32_t polymomial, uint32_t *hal_utility_crc32table)
    @brief      initialise the crctable for a given quotient polynomial. 
    @param      quotient        teh poly.
    @param      crctab          the crctab whcih must point to 256 words of 4 bytes each.
 **/
extern void hal_utility_crc32_table_get(uint32_t polymomial, uint32_t *hal_utility_crc32table);


/** @fn         extern uint32_t hal_utility_crc32_compute(const uint32_t polynom, const uint32_t *hal_utility_crc32table, uint32_t initvalue, uint8_t *data, uint32_t len)
    @brief      computes the crc. 
    @param      crctab          the table.
    @param      data            data to be used.
    @param      len             its size, at least four
 **/
extern uint32_t hal_utility_crc32_compute(const uint32_t polynom, const uint32_t *hal_utility_crc32table, uint32_t initvalue, uint8_t *data, uint32_t len);


// - doxy end ---------------------------------------------------------------------------------------------------------

/** @}            
    end of group hal_utility_crc32  
 **/

 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
