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
#ifndef _IPAL_ARP_H_
#define _IPAL_ARPH_


/** @file       ipal_arp.h
	@brief      This header file implements public interface to the ip abstraction layer: arp 
	@author     marco.accame@iit.it
	@date       12/12/2011
**/

/* @defgroup ipal_arp IP abstraction layer: arp
    cercecvecve.
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "ipal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum ipal_arp_cachemode_t 
    @brief      ipal_arp_cachemode_t contains the modes with which the pair ipaddress-mac is stored on
                the arp cache table.  If the mode is ipal_arp_cache_removeontout, then the pair is
                removed upon expiry of the timeout specified by ipal_params_cfg_t::arp_cachetimeout
 **/ 
typedef enum
{
    ipal_arp_cache_permanently  = 0,
    ipal_arp_cache_removeontout = 1
} ipal_arp_cachemode_t;

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern ipal_result_t ipal_arp_resolve(ipal_ipv4addr_t ip, ipal_arp_cachemode_t cm, uint8_t forcearpframe)
    @brief      Issues an ARP request to map the IP address with the proper MAC address.
    @param      ip              The IP address
    @param      cm              The mode with which the IP-MAC pair is stored.
    @param      forcearpframe   if 1: every call always sends an ARP request and in some cases it may send two frames (it sends an ARP 
                                frame even if the pair IP-MAC is already cached); else if not 1: it sends an ARP frame only if
                                the pair IP-MAC is not resolved yet and if the underlying IP stack has not recently sent a previous
                                APR frame (the way the IP stack decides is implementation-dependent).
    @return     ipal_res_OK on success or ipal_res_NOK_generic if the IPAL has not been started yet or if the ARP request fails.
 **/
extern ipal_result_t ipal_arp_resolve(ipal_ipv4addr_t ip, ipal_arp_cachemode_t cm, uint8_t forcearpframe);


/** @fn         extern ipal_result_t ipal_arp_isresolved(ipal_ipv4addr_t ip)
    @brief      tells the caller is the IP address in argument is already paired to a valid MAC address.
    @param      ip              The IP address
    @return     ipal_res_OK on success or ipal_res_NOK_generic if the IPAL has not been started yet or if the IP-MAC is not paired yet.
 **/
extern ipal_result_t ipal_arp_isresolved(ipal_ipv4addr_t ip);


/** @fn         extern ipal_result_t ipal_arp_getmac(ipal_ipv4addr_t ip, uint8_t* mac)
    @brief      gives to the caller the MAC address associated to the IP in argument. 
    @param      ip              The IP address
    @param      mac             The pointer where is it copied the 6-byte MAC address. It can be NULL.
    @return     ipal_res_OK on success or ipal_res_NOK_generic if the IPAL has not been started yet or if IP-MAC is not paired yet.
 **/
extern ipal_result_t ipal_arp_getmac(ipal_ipv4addr_t ip, uint8_t* mac);


// legacy:

extern ipal_result_t ipal_arp_request(ipal_ipv4addr_t ip, ipal_arp_cachemode_t cm);



/* @}            
    end of group ipal_arp  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

