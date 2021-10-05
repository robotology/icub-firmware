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

/* @file       ipal_eth.c
	@brief      This file implements internal implementation of the ipal eth module
	@author     valentina.gaggero@iit.it
    @date       01/07/2013
**/

// - modules to be built: contains the IPAL_USE_* macros --------------------------------------------------------------
#include "ipal_modules.h"

// - sizes of modules: contains the IPAL_NUMBEROF_* macros ----------------------------------------------------------------
#include "ipal_numberof.h"

#ifdef IPAL_USE_ETH

#include "stdlib.h"
#include "string.h"

#include "lwip_interface.h"
#include "netif/ethernet.h"
#include "ethernetif.h"

/* Friends functions */
#include "ipal_hid.h"

#ifdef FRAME_QUEUE
	/* This is equivalent to the "old" storage */
	#include "lwip_frame_queue_handler.h"
#endif

#ifdef IPAL_USE_RTOS_MINIMAL
	#include "lwip_tx_frame_queue_handler.h"
#endif

/* IPAL API */
#include "ipal.h"

/* ===================================================================================================================
 * MACROS
 * =================================================================================================================== */

/* Maximum transfer unit */
#define IPAL_ETH_MTU  (1500)

/* ===================================================================================================================
 * PRIVATE FUNCTIONS DECLARATION
 * =================================================================================================================== */

/*
 * Resets internal variables
 */
static void s_ipal_eth_internal_ram_reset(void);

/*
 * Initializes internal variables
 */
static void s_ipal_eth_internal_ram_init(void);

/* ===================================================================================================================
 * PRIVATE VARIABLES
 * =================================================================================================================== */

/*
 * Network interface
 */
static ipal_eth_hid_netif_t s_ipal_eth_netif;

/* ===================================================================================================================
 * FRIEND FUNCTIONS
 * =================================================================================================================== */

/*
 * ipal_eth_hid_getsize
 * See header file for details
 */
uint32_t ipal_eth_hid_getsize(const ipal_cfg_t *cfg)
{	
	return(0);
}

/*
 * ipal_eth_hid_setmem
 * See header file for details
 */
ipal_result_t ipal_eth_hid_setmem(const ipal_cfg_t *cfg, uint32_t *memory)
{
	uint8_t *ram08 = (uint8_t*)memory;
	
	/* Avoid compilere warnings */
	ram08 = ram08;
	
	return(ipal_res_OK); 
}

/*
 * ipal_eth_hid_init
 * See header file for details
 */
ipal_result_t ipal_eth_hid_init(ipal_cfg_t *cfg)
{
	if((NULL == cfg->extfn.hal_eth_init) || (NULL == cfg->extfn.hal_eth_enable) ||
		 (NULL == cfg->extfn.hal_eth_disable) || (NULL == cfg->extfn.hal_eth_sendframe) )
	{
		ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_eth_hid_init(): missing eth functions");
		return(ipal_res_NOK_generic);
	}
	
	return(ipal_res_OK);
}

/*
 * ipal_eth_hid_vars_init
 * See header file for details
 */
ipal_result_t ipal_eth_hid_vars_init(const ipal_cfg_t *cfg)
{
  /* Initializes internal structures */    
	s_ipal_eth_internal_ram_reset();
	s_ipal_eth_internal_ram_init();
    
	return(ipal_res_OK);
}

/*
 * ipal_eth_hid_run
 * See header file for details
 */
ipal_result_t ipal_eth_hid_run(void)
{
	ip4_addr_t ipaddr, mask, gw_addr;
	struct netif * netif_ptr = NULL;

	ipaddr.addr = ipal2lwip_ipv4addr(ipal_base_hid_cfg.eth_ip);
	mask.addr = ipal2lwip_ipv4addr(ipal_base_hid_cfg.eth_mask);
	gw_addr.addr = 0;

	/* Is important set can addr before netif_add func, becouse netif_add calls hal_eth_init that needs mac addr in cfg.
	As an alternative mac addr cfg can be done in func low_level_init, but so the initialization of netif struct is 
	shared between more functions makeing difficult to mantain code*/
	
	s_ipal_eth_netif.netif.hwaddr_len = ETHARP_HWADDR_LEN;
	memcpy(&s_ipal_eth_netif.netif.hwaddr, &ipal_base_hid_cfg.eth_mac, 6);

	/* Initializes interface */
	netif_ptr = netif_add(&s_ipal_eth_netif.netif, 
												&ipaddr,
												&mask,
												&gw_addr, 
												NULL, 					
												&ethernetif_init,
												&ethernet_input);


	if(NULL == netif_ptr)
	{
			ipal_base_hid_on_fatalerror(ipal_error_generic, "ipal_eth_hid_run: error in netif_add");
			return(ipal_res_NOK_generic);
	}
        
	/* config other device capabilities */

	/*  Registers the default network interface.*/
	netif_set_default(&s_ipal_eth_netif.netif);

	/*  When the netif is fully configured this function must be called.*/
	netif_set_up(&s_ipal_eth_netif.netif);
	
	return(ipal_res_OK);  
}

/*
 * ipal_eth_hid_getnetif
 * See header file for details
 */
ipal_eth_hid_netif_t *ipal_eth_hid_getnetif(void)
{
    return(&s_ipal_eth_netif);
}

/* ===================================================================================================================
 * PRIVATE FUNCTIONS DEFINITIONS
 * =================================================================================================================== */

/*
 * s_ipal_eth_internal_ram_reset
 * See declaration for details
 */
static void s_ipal_eth_internal_ram_reset(void)
{
	memset(&s_ipal_eth_netif, 0, sizeof(s_ipal_eth_netif));
}

/*
 * s_ipal_eth_internal_ram_init
 * See declaration for details
 */
static void s_ipal_eth_internal_ram_init(void)
{
	#ifdef FRAME_QUEUE
		lwip_frame_queue_RX_INIT();
	#endif
	
	#ifdef IPAL_USE_RTOS_MINIMAL
		lwip_TX_frame_queue_init();
	#endif
}

#endif

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




