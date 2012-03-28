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
#ifndef _IPAL_TCPSOCKET_H_
#define _IPAL_TCPSOCKET_H_


/** @file       ipal_tcpsocket.h
	@brief      This header file implements public interface to the ip abstraction layer: tcp socket 
	@author     marco.accame@iit.it
	@date       12/12/2011
**/

/* @defgroup ipal_tcpsocket IP abstraction layer: tcp socket
    the tcp sockets are not implemented.
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "ipal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct ipal_tcpsocket_opaque_t ipal_tcpsocket_t 
    @brief      TCP socket type used in IPAL
 **/
typedef struct ipal_tcpsocket_opaque_t ipal_tcpsocket_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section



// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section

// warning: tcp sockets are not implemented



/* @}            
    end of group ipal_tcpsocket  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

