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

#ifndef _HAL_UTILITY_CRC07_H_
#define _HAL_UTILITY_CRC07_H_


// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_utility_crc07.h
    @brief      This header file implements public interface to hal_utility_crc07 using lut, as described by Sven Reifegerste
                at http://zorc.breitbandkatze.de/crc.html
    @author     marco.accame@iit.it
    @date       02/18/2013
**/

/** @defgroup hal_utility_crc07
 
    
    @todo put documentation proper to the entity.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------

// uncomment to save 256 bytes of rom
#define _USE_HAL_UTILITY_CRC07TABLE_IN_ROM_
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empoty-section

    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------

extern const uint8_t hal_utility_crc07_table_0x09[256];


// - declaration of extern public functions ---------------------------------------------------------------------------
 
/** @fn         extern void hal_utility_crc07_table_get(uint8_t polymomial, uint8_t *hal_utility_crc07table)
    @brief      initialise the crctable for a given quotient polynomial. 
    @param      quotient        the poly.
    @param      crctab          the crctab whcih must point to 256 words of 1 bytes each.
 **/
extern void hal_utility_crc07_table_get(uint8_t polymomial, uint8_t *hal_utility_crc07table);


/** @fn         extern uint8_t hal_utility_crc07_compute(const uint32_t polynom, const uint8_t *hal_utility_crc07table, uint8_t initvalue, uint8_t *data, uint32_t len)
    @brief      computes the crc. 
    @param      crctab          the table.
    @param      data            data to be used.
    @param      len             its size, at least four
 **/
extern uint8_t hal_utility_crc07_compute(const uint32_t polynom, const uint8_t *hal_utility_crc07table, uint8_t initvalue, uint8_t *data, uint32_t len);


// - doxy end ---------------------------------------------------------------------------------------------------------

/** @}            
    end of group hal_utility_crc07  
 **/

 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


