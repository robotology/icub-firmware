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

#ifndef _SENDER_OUTMSG_H_
#define _SENDER_OUTMSG_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       sender_outMsg.h
    @brief      The sender, if started, informs periodically the application that it is necessary to send outgoing message(s).
                the period is can_msg_datarate value configured in SIXsg_config_data_t.
    @author     valentina.gaggero@iit.it
    @date       07/14/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "6sg_config.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern void sender_init(SIXsg_config_data_t *cfg_ptr);
    @brief      initialises sender
    @param      cfg_ptr         The cfg
    @return     hal_res_NOK_nullpointer if cfg_ptr is NULL.(in this case sender may crash.)
                hal_res_OK otherwise
**/
extern hal_result_t sender_init(SIXsg_config_data_t *cfg_ptr);

/** @fn         extern void sender_start(void);
    @brief      starts sender, so periodic messages start going out.
                Sender will start only if in 6SG configuration tx_outMsg_mode was set not equal to tx_outMsg_off
**/
extern void sender_start(void);


/** @fn         extern void sender_stop(void);
    @brief      stops sender, so no periodic message is going out.
**/
extern void sender_stop(void);


/** @fn         extern int16_t sender_is_running(void);
    @brief      returns sender status.
    @param
    @return     1 if sender is running
                0 otherwise
**/
extern int16_t sender_is_running(void);


// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


