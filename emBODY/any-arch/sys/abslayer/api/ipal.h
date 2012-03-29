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
#ifndef _IPAL_H_
#define _IPAL_H_

/** @file       ipal.h
	@brief      This header file implements public interface to the ip abstraction layer 
	@author     marco.accame@iit.it
	@date       12/12/2011
**/

/* @defgroup ipal IP abstraction layer
    cercecvecve.
    
    @{		
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "ipal_arp.h"
#include "ipal_base.h"
#include "ipal_dhcp.h"
#include "ipal_ftp.h"
#include "ipal_igmp.h"
#include "ipal_sys.h"
#include "ipal_tcpsocket.h"
#include "ipal_telnet.h"
#include "ipal_tftp.h"
#include "ipal_udpsocket.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section

/* @}            
    end of group ipal  
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

