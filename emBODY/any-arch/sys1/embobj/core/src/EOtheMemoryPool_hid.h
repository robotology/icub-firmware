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
#ifndef _THEMEMORYPOOL_HID_H_
#define _THEMEMORYPOOL_HID_H_


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOtheMemoryPool.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

struct EOtheMemoryPool_hid 
{
    const eOmempool_cfg_t   *cfg;
    EOVmutex                *mutex;
    eOreltime_t             tout;
    eOmempool_allocmode_t   allocmode;
    uint8_t                 staticmask;
    uint8_t                 initted;
    uint16_t                uint08s_num;
    uint16_t                uint16s_num;
    uint16_t                uint32s_num;
    uint16_t                uint64s_num;
    uint32_t                usedbytes;
}; 


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


