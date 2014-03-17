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
#ifndef _HAL_UTILITY_CRC32_HID_H_
#define _HAL_UTILITY_CRC32_HID_H_


/* @file       hal_utility_crc32_hid.h
    @brief      this header file gives hidden interface to hal tools.
    @author     marco.accame@iit.it
    @date       10/29/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------



// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_utility_crc32.h"


// - #define used with hidden structs ---------------------------------------------------------------------------------
// empty-section

// - definition of hidden structs -------------------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section


// - declaration of extern hidden functions ---------------------------------------------------------------------------


extern hal_result_t hal_utility_crc32_hid_static_memory_init(void);

// - definition of extern hidden inline functions ---------------------------------------------------------------------



#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




