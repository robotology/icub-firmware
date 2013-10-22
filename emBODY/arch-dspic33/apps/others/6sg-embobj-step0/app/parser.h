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

#ifndef _PARSER_H_
#define _PARSER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       parser.h
    @brief      This file provides functions parse can message concurring with iCub can protocol
    @author     valentina.gaggero@iit.it
    @date       07/14/2011
**/

// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"
#include "can_protocol.h"
#include "6sg_config.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern hal_result_t parser_init(SIXsg_config_data_t *cfg_ptr);
    @brief      initialises the parser. This function must be called befor using a parser's service.
    @param      cfg_ptr         pointer to SIXsg_config_data_t struct.
    @return     hal_res_NOK_nullpointer if cfg_ptr is NULL.(in this case parser may crash.)
                hal_res_OK otherwise
 **/
extern hal_result_t parser_init(SIXsg_config_data_t *cfg_ptr);


/** @fn         extern hal_result_t parse_message(void);
    @brief      Gets a message from CAN's received message queue and parses it.
                If received message is unknown or malformed, parser discards it and returns positive feedbak
                the returned errors are about CAN peripheral only.
    @param      
    @return     hal_res_NOK_nodata if queue is empty
                hal_res_NOK_unsupported if received frame is a remote frame or has extID
                hal_res_NOK_hwerror if received frame's payload's length is bigger the 8 
                hal_res_OK otherwise
 **/
extern hal_result_t parse_message(void);


extern hal_result_t parse_test(void);


// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


