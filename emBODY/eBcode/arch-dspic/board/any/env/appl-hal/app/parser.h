/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
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

#ifndef _PARSER_H_
#define _PARSER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       parser.h
    @brief      this file ...
    @author     valentina.gaggero@iit.it
    @date       27/01/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "hal.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef enum
{
    cmd_none              =  0,
    cmd_sending_enable    =  1,
    cmd_sending_disable   =  2,
    cmd_privdata_get      =  3,
    cmd_privdata_set      =  4,
    cmd_erase_applflash   =  5
} ap_parser_cmd_opcode;

typedef struct
{
    ap_parser_cmd_opcode opc;
    uint8_t data[8];
} ap_parser_cmd_t;
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern hal_result_t ap_parse_init(void);

extern hal_result_t ap_parse_canFrame(hal_can_frame_t *recframe, ap_parser_cmd_t **cmd);

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

