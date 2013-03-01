/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
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
#ifndef _HAL_BASE_HID_H_
#define _HAL_BASE_HID_H_


/* @file       hal_base_hid.h
    @brief      This header file implements hidden interface to hal base.
    @author     marco.accame@iit.it
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
    uint8_t     dummy;
} hal_base_hid_brdcfg_t;

// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section


// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern hal_result_t hal_base_hid_static_memory_init(void);
extern hal_bool_t hal_base_hid_initted_is(void);




// - definition of extern hidden inline functions ---------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




