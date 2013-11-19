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
#ifndef _IPAL_TELNET_H_
#define _IPAL_TELNET_H_


/** @file       ipal_telnet.h
	@brief      This header file implements public interface to the ip abstraction layer: telnet 
	@author     marco.accame@iit.it
	@date       12/12/2011
**/

/* @defgroup ipal_telnet IP abstraction layer: telnet
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


/** @fn         extern ipal_bool_t ipal_telnet_isenabled(void)
    @brief      tells if TELNET service is enabled.
    @return     ipal_false or ipal_true.
 **/
extern ipal_bool_t ipal_telnet_isenabled(void); 


/** @fn         extern ipal_bool_t ipal_telnet_isactive(void)
    @brief      tells if TELNET service is running.
    @return     ipal_false or ipal_true.
 **/
extern ipal_bool_t ipal_telnet_isactive(void); 

                                
/** @fn         extern ipal_result_t ipal_telnet_stop(void)
    @brief      It stops the TELNET service (if enabled).
    @return     ipal_res_OK on success or ipal_res_NOK_generic upon failure.
 **/
extern ipal_result_t ipal_telnet_stop(void); 


/** @fn         extern ipal_result_t ipal_telnet_restart(void)
    @brief      It restarts the TELNET service (if enabled). If already running it does nothing
    @return     ipal_res_OK on success or ipal_res_NOK_generic upon failure.
 **/
extern ipal_result_t ipal_telnet_restart(void); 



/* @}            
    end of group ipal_telnet  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

