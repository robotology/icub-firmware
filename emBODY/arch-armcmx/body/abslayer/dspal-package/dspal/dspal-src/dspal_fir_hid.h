/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#ifndef _DSPAL_FIR_HID_H_
#define _DSPAL_FIR_HID_H_


/* @file       dspal_fir_hid.h
    @brief      This header file implements hidden interface to parts of hal specifics of architecture
    @author     marco.accame@iit.it
    @date       09/19/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "dspal_base.h"
#include "dspal_base_hid.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "dspal_fir.h"


// - #define used with hidden structs ---------------------------------------------------------------------------------


// - definition of hidden structs -------------------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden variables ---------------------------------------------------------------------------



// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t dspal_fir_hid_getsize(const dspal_cfg_t *cfg);
extern dspal_result_t dspal_fir_hid_setmem(const dspal_cfg_t *cfg, uint32_t *memory);


// - definition of extern hidden inline functions ---------------------------------------------------------------------
// empty-section

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




