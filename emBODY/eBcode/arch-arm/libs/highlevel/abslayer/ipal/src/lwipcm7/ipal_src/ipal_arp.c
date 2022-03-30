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

/* @file       ipal_arp.c
	@brief      This file implements internal implementation of the ipal arpmodule
	@author     valentina.gaggero@iit.it
    @date       01/07/2013
**/

// - modules to be built: contains the IPAL_USE_* macros --------------------------------------------------------------
#include "ipal_modules.h"

// - sizes of modules: contains the IPAL_NUMBEROF_* macros ----------------------------------------------------------------
#include "ipal_numberof.h"

#ifdef IPAL_USE_ARP

#include "stdlib.h"
#include "string.h"

#include "lwip_interface.h"

/* Friend functions */
#include "ipal_hid.h"

/* IPAL APIs */
#include "ipal.h"

/* ====================================================================================================================
 * STATIC FUNCTIONS DECLARATION
 * ==================================================================================================================== */

/*
 * Resets internal RAM
 */
static void s_ipal_arp_internal_ram_reset(void);

/*
 * Initializes internal RAM
 */
static void s_ipal_arp_internal_ram_init(void);

/* =======================================================================================================================
 * PUBLIC FUNCTIONS (API)
 * ======================================================================================================================= */
 
/*
 * ipal_arp_resolve
 * See header file for details
 */
ipal_result_t ipal_arp_resolve(ipal_ipv4addr_t ip, ipal_arp_cachemode_t cm, uint8_t forcearpframe)
{
	ipal_result_t ret = ipal_res_OK;
	
	/* Cache mode is not used */
	cm = cm;
	
	/* If not resolved yet, or ARP frame is forced, send an ARP request, otherwise return OK (already in ARP table) */
	if ((ipal_arp_isresolved(ip) == ipal_res_NOK_generic)||forcearpframe)
	{
		ipal_arp_request(ip, cm);
        ret = ipal_arp_isresolved(ip);
	}
	
	return ret;
}

/*
 * ipal_arp_isresolved
 * See header file for details
 */
ipal_result_t ipal_arp_isresolved(ipal_ipv4addr_t ip)
{
	int8_t ret;
	ipal_eth_hid_netif_t *netif_ptr;
	ip4_addr_t ipaddr;
	struct eth_addr *eth_ret_addr;
	const ip4_addr_t *ip_ret_addr;
	
	/* Convert IPAL address to LWIP address */
	ipaddr.addr = ipal2lwip_ipv4addr(ip);
	
	/* IPAL lock */
	ipal_base_hid_threadsafety_lock();

	/* Retieve the network interface */
	netif_ptr = ipal_eth_hid_getnetif();
	
	/* Check wether the requested IP address is already into ARP TABLE */
	ret = etharp_find_addr(&(netif_ptr->netif), &ipaddr, &eth_ret_addr, &ip_ret_addr);
	
	/* IPAL uNLOCK */
	ipal_base_hid_threadsafety_unlock();
	
	return (ret != -1)?ipal_res_OK:ipal_res_NOK_generic;
}

/*
 * ipal_arp_getmac
 * See header file for details
 */
ipal_result_t ipal_arp_getmac(ipal_ipv4addr_t ip, uint8_t* mac)
{
	int8_t ret;
	ipal_eth_hid_netif_t *netif_ptr;
	ip4_addr_t ipaddr;
	struct eth_addr *eth_ret_addr;
	const ip4_addr_t *ip_ret_addr;
	
	/* Convert IPAL address to LWIP address */
	ipaddr.addr = ipal2lwip_ipv4addr(ip);
	
	/* IPAL lock */
	ipal_base_hid_threadsafety_lock();

	/* Retieve the network interface */
	netif_ptr = ipal_eth_hid_getnetif();
	
	/* Check wether the requested IP address is already into ARP TABLE */
	ret = etharp_find_addr(&(netif_ptr->netif), &ipaddr, &eth_ret_addr, &ip_ret_addr);
	
	/* IPAL uNLOCK */
	ipal_base_hid_threadsafety_unlock();
	
	if (ret == -1)
	{
		return ipal_res_NOK_generic;
	}
	else 
	{
		memcpy(mac, eth_ret_addr->addr, ETH_HWADDR_LEN);
	}
	
	return ipal_res_OK;
}

/*
 * ipal_arp_request
 * See header file for details
 */
ipal_result_t ipal_arp_request(ipal_ipv4addr_t ip, ipal_arp_cachemode_t cm)
{
	err_t err;
	ipal_eth_hid_netif_t *netif_ptr;
	ip4_addr_t ipaddr;
	
	/* Convert IPAL address to LWIP address */
	ipaddr.addr = ipal2lwip_ipv4addr(ip);
	
	/* IPAL lock */
	ipal_base_hid_threadsafety_lock();
	
	/* Retieve the network interface */
	netif_ptr = ipal_eth_hid_getnetif();
	
	/* IPaddr is not present in ARP table. Launch a request */
	err = etharp_request(&(netif_ptr->netif), &ipaddr);

	ipal_base_hid_threadsafety_unlock();

	return ((ERR_OK == err) ? (ipal_res_OK) : (ipal_res_NOK_generic) );
}	

/* -----------------------------------------------------------------------------------------------------------------------
 * FRIEND "HIDDEN" FUNCTIONS
 * ----------------------------------------------------------------------------------------------------------------------- */

/* 
 * ipal_arp_hid_getsize
 * See header file for details
 */
uint32_t ipal_arp_hid_getsize(const ipal_cfg2_t *cfg2)
{
	/* No memory needed */
	return(0);
}

/* 
 * ipal_arp_hid_setmem
 * See header file for details
 */
ipal_result_t ipal_arp_hid_setmem(const ipal_cfg2_t *cfg2, uint32_t *memory)
{
	/* No memory needed */
	return(ipal_res_OK); 
}

/* 
 * ipal_arp_hid_init
 * See header file for details
 */
ipal_result_t ipal_arp_hid_init(ipal_cfg2_t *cfg2)
{
	/* Nothing is required */
	return(ipal_res_OK); 
}

/* 
 * ipal_arp_hid_vars_init
 * See header file for details
 */
ipal_result_t ipal_arp_hid_vars_init(const ipal_cfg2_t *cfg2)
{
	s_ipal_arp_internal_ram_reset();
	s_ipal_arp_internal_ram_init();
	
	/* ARP table is handled internally */
	return(ipal_res_OK);
}

/* ====================================================================================================================
 * DEFINITIONS OF STATIC FUNCTIONS
 * ==================================================================================================================== */

/* 
 * Reset RAM
 */
static void s_ipal_arp_internal_ram_reset(void)
{
	/* NOthing to do */ 
}

/* 
 * Initialize RAM
 */
static void s_ipal_arp_internal_ram_init(void)
{
	/* Nothing to do */
}


#endif//IPAL_USE_ARP

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




