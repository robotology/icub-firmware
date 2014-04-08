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
#ifndef _IPAL_SYS_H_
#define _IPAL_SYS_H_


/** @file       ipal_sys.h
	@brief      This header file implements public interface to the ip abstraction layer: sys 
	@author     marco.accame@iit.it
	@date       12/12/2011
**/

/* @defgroup ipal_sys IP abstraction layer: sys
    cercecvecve.
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "ipal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern ipal_result_t ipal_sys_start(void)
    @brief      Function which starts the IPAL.
    @return     Always ipal_res_OK.  If the configuration is missing (as for ipal_base_initialise() never called), 
                then it calls usr_on_fatal_error().
 **/
extern ipal_result_t ipal_sys_start(void);


/** @fn         extern void ipal_sys_process_communication(void)
    @brief      Function which processes received frames and also effectively transmit frames programmed to be
                sent by calls to other functions.
    @details    As an example, when a UDP packet is received on a socket, this function  calls the registered 
                callback and also effectively send a UDP packet that the user previously requested to send with 
                a call to ipal_udpsocket_send().
                As this function effectively sends and receives frames, it should be called frequently enough
                or at least it should be triggered by some rx or tx events from ISR and sender tasks.
 **/
extern void ipal_sys_process_communication(void);


/** @fn         extern void ipal_sys_timetick_increment(void)
    @brief      Function which increments the time internal to IPAL. It shall be called at exactly the frequency
                specified in field sys_timetick of ipal_cfg_t, also retrievable by function ipal_timetick_get()
 **/
extern void ipal_sys_timetick_increment(void);


/** @fn         extern ipal_reltime_t ipal_timetick_get(void)
    @brief      Function which returns the field sys_timetick of the internally stored configuration of type 
                ipal_cfg_t.
    @return     The time tick value
 **/
extern ipal_reltime_t ipal_sys_timetick_get(void);


/* @}            
    end of group ipal_sys  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

