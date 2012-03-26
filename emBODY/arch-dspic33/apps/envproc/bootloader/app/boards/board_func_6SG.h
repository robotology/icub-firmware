/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
#ifndef _BOARD_CONFIG_6SG_H_
#define _BOARD_CONFIG_6SG_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
/*
    @file   board_func_6sg.h
    @brief  this file provides board-dependent services for 6SG board.
    @author valentina.gaggero@iit.it
    @date   06/17/2011
**/

// - external dependencies --------------------------------------------------------------------------------------------
#include "stdint.h"

// - public #define  --------------------------------------------------------------------------------------------------
#define BOARD_TYPE_6SG     				0x09
#define TARGET_BOARD 					BOARD_TYPE_6SG
#define BOARD_CAN_ADDRESS_DEFAULT		0xD
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern void board_func_can_filter_polling_msg_set(uint8_t mask_num, uint32_t mask_val, uint8_t identifier_num, 
                                                  uint8_t board_addr);

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


