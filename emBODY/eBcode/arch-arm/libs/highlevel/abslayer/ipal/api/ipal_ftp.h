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
#ifndef _IPAL_FTP_H_
#define _IPAL_FTP_H_


/** @file       ipal_ftp.h
	@brief      This header file implements public interface to the ip abstraction layer: ftp 
	@author     marco.accame@iit.it
	@date       12/12/2011
**/

/* @defgroup ipal_ftp IP abstraction layer: ftp
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


/** @fn         extern ipal_bool_t ipal_ftp_isenabled(void)
    @brief      tells if FTP service is enabled.
    @return     ipal_false or ipal_true.
 **/
extern ipal_bool_t ipal_ftp_isenabled(void); 


/** @fn         extern ipal_bool_t ipal_ftp_isactive(void)
    @brief      tells if FTP service is running.
    @return     ipal_false or ipal_true.
 **/
extern ipal_bool_t ipal_ftp_isactive(void); 

                                
/** @fn         extern ipal_result_t ipal_ftp_stop(void)
    @brief      It stops the FTP service (if enabled).
    @return     ipal_res_OK on success or ipal_res_NOK_generic upon failure.
 **/
extern ipal_result_t ipal_ftp_stop(void); 


/** @fn         extern ipal_result_t ipal_ftp_restart(void)
    @brief      It restarts the FTP service (if enabled). If already running it does nothing
    @return     ipal_res_OK on success or ipal_res_NOK_generic upon failure.
 **/
extern ipal_result_t ipal_ftp_restart(void); 



/* @}            
    end of group ipal_ftp  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

