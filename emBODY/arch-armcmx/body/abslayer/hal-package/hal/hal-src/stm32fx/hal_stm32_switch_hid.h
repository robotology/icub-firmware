/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#ifndef _HAL_STM32_SWITCH_HID_H_
#define _HAL_STM32_SWITCH_HID_H_


/* @file       hal_stm32_switch_hid.h
    @brief      This header file implements hidden interface to ...
    @author     valentina.gaggero@iit.it / marco.accame@iit.it
    @date       09/09/2010
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "hal_stm32xx_include.h"
#include "hal_base.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_switch.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------


// - definition of the hidden struct implementing the object ----------------------------------------------------------
// empty-section




// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section


// - declaration of extern hidden macros ------------------------------------------------------------------------------    
// empty-section


// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t hal_switch_hid_getsize(const hal_cfg_t *cfg);

extern hal_result_t hal_switch_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory);




#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




