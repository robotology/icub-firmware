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
#ifndef _IPAL_IGMP_H_
#define _IPAL_IGMP_H_


/** @file       ipal_igmp.h
	@brief      This header file implements public interface to the ip abstraction layer: base services and types 
	@author     marco.accame@iit.it
	@date       12/12/2011
**/

/* @defgroup ipal_igmp IP abstraction layer: igmp
    cercecvecve.
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "ipal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------

/** @def        IPAL_ipv4addr_igmp_firstgroup
    @brief      The first igmp group.
 **/ 
#define IPAL_ipv4addr_igmp_firstgroup     IPAL_ipv4addr(239, 0, 0, 0)


/** @def        IPAL_ipv4addr_igmp_lastgroup
    @brief      The last igmp group.
 **/ 
#define IPAL_ipv4addr_igmp_last     IPAL_ipv4addr(239, 0, 0, 255)


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern ipal_result_t ipal_igmp_join(ipal_ipv4addr_t group)
    @brief      Subscribes the local IP address to a given multicast group (only if the IGMP is enabled and its table is not full)
    @param      group           The multicast group beloing to 239.0.0.0/8
    @return     ipal_res_OK on success or ipal_res_NOK_generic upon failure.
    @warning    The group must belong to 239.0.0.0/8.
 **/                                         
extern ipal_result_t ipal_igmp_join(ipal_ipv4addr_t group);                                         


/** @fn         extern ipal_result_t ipal_igmp_leave(ipal_ipv4addr_t group)
    @brief      Unsubscribes the local IP address to a given multicast group.
    @param      group           The multicast group beloing to 239.0.0.0/8
    @return     ipal_res_OK on success or ipal_res_NOK_generic upon failure.
    @warning    The group must belong to 239.0.0.0/8.
 **/  
extern ipal_result_t ipal_igmp_leave(ipal_ipv4addr_t group); 



/* @}            
    end of group ipal_igmp  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

