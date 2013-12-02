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
// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------
/*
    @file   board_config_6sg.c
    @brief  this file implements board-dependent services for 6SG board.
    @author valentina.gaggero@iit.it
    @date   17/06/2011
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

//#include <p33fj128mc802.h>
//#include <libpic30.h>

#include "stdint.h"
#include "hal_base.h"
#include "hal_can.h"
#include "can_protocol.h"
#include "eEcommon.h"
#include "eEmemorymap_dspic33.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "board_func_6SG.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//#include "entity_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static const eEboardInfo_t s_bl_boardinfo_default =                       
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_board,
            .signature  = 0x11,
            .version    = 
            { 
                .major = 1, 
                .minor = 0
            },  
            .builddate  = 
            {
                .year  = 2010,
                .month = 1,
                .day   = 1,
                .hour  = 0,
                .min   = 0
            }
        },
        .rom        = 
        {   
            .addr   = EE_MEMMAP_ROM_ADDR,
            .size   = EE_MEMMAP_ROM_SIZE
        },
        .ram        = 
        {   
            .addr   = EE_MEMMAP_RAM_ADDR,
            .size   = EE_MEMMAP_ROM_SIZE
        },
        .storage    = 
        {
            .type   = ee_strg_emuleeprom,
            .size   = EE_MEMMAP_STRG_SIZE,
            .addr   = EE_MEMMAP_STRG_ADDR
        },
        .communication  = ee_commtype_can1,
        .name           = "6sg"
    },
    .uniqueid       = 0,
    .extra          = {0}
};

const eEboardInfo_t *bl_boardinfo_default_ptr = &s_bl_boardinfo_default;
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern void board_func_can_filter_polling_msg_set(uint8_t mask_num, uint32_t mask_val, uint8_t identifier_num, 
                                                  uint8_t board_addr)
{  
    hal_can_receptionfilter_set(hal_can_port1, mask_num, mask_val, identifier_num, 
                                (CAN_MSG_CLASS_POLLING_SENSOR|board_addr), hal_can_frameID_std);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



