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


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "lwip_interface.h"

#include "ipal_base_hid.h"
#include "ipal_sys_hid.h"
#include "ipal_fifo_hid.h"

// - storage for ipal
#include "ipal_ipiit_storage.h"
#include "ipal_ipiit_storage_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "ipal.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "ipal_eth_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define IPAL_ETH_MTU  IP_FRAG_MAX_MTU

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_ipal_eth_internal_ram_reset(void);
static void s_ipal_eth_internal_ram_init(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static ipal_eth_hid_netif s_ipal_eth_netif;

//rx_pbuf_fifo collects all pkt (transformed in pbuf format) received from phy
static ipal_fifo_t s_rx_pbuf_fifo;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


extern uint32_t ipal_eth_hid_getsize(const ipal_cfg_t *cfg)
{
    uint32_t size = 0;
    
    if((cfg->udp_enable) && (cfg->udp_socketnum>0))
    {
        size = (ipal_fifo__itemsize*cfg->udp_socketnum);
    }
    return(size);
}

extern ipal_result_t ipal_eth_hid_setmem(const ipal_cfg_t *cfg, uint32_t *memory)
{
    uint8_t *ram08 = (uint8_t*)memory;
    
    if((cfg->udp_enable) && (cfg->udp_socketnum>0))
    {    
        if(NULL == memory)
        {
            ipal_base_hid_on_fatalerror(ipal_error_missingmemory, "ipal_eth_hid_setmem(): memory missing");
            return(ipal_res_NOK_generic);
        }
        
        ipal_fifo_hid_init(&s_rx_pbuf_fifo, cfg->udp_socketnum, ram08);
    }
    
    return(ipal_res_OK); 
}


extern ipal_result_t ipal_eth_hid_init(ipal_cfg_t *cfg)
{
    if((NULL == cfg->extfn.hal_eth_init) || (NULL == cfg->extfn.hal_eth_enable) ||
       (NULL == cfg->extfn.hal_eth_disable) || (NULL == cfg->extfn.hal_eth_sendframe) )
    {
        ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_eth_hid_init(): missing eth functions");
        return(ipal_res_NOK_generic);
    }
    
    return(ipal_res_OK);
}

extern ipal_result_t ipal_eth_hid_vars_init(const ipal_cfg_t *cfg)
{
    
    // we reset the internally allocated memory and we init internal variables
    s_ipal_eth_internal_ram_reset();
    s_ipal_eth_internal_ram_init();
    
    // we initialise ipiit storage
    ipal_ipiit_storage_eth_init(cfg);
    
    return(ipal_res_OK);
}

extern ipal_result_t ipal_eth_hid_run(void)
{
    
    struct ip_addr ipaddr, mask, gw_addr;
    struct netif * netif_ptr = NULL;

    ipaddr.addr = ipal2lwip_ipv4addr(ipal_base_hid_cfg.eth_ip);
    mask.addr = ipal2lwip_ipv4addr(ipal_base_hid_cfg.eth_mask);
    gw_addr.addr = 0;

    /* Is important set can addr before netif_add func, becouse netif_add calls hal_eth_init that needs mac addr in cfg.
    As an alternative mac addr cfg can be done in func low_level_init, but so the initialization of netif struct is 
    shared between more functions makeing difficult to mantain code*/
    
    s_ipal_eth_netif.netif.hwaddr_len = ETHARP_HWADDR_LEN;
    memcpy(&s_ipal_eth_netif.netif.hwaddr, &ipal_base_hid_cfg.eth_mac, 6);


    netif_ptr = netif_add(&s_ipal_eth_netif.netif, 
                          &ipaddr,
                          &mask,
                          &gw_addr, 
                          &s_rx_pbuf_fifo, //user param to save
                          &ethernetif_init,
                          &ethernet_input);

    if(NULL == netif_ptr)
    {
        ipal_base_hid_on_fatalerror(ipal_error_generic, "ipal_eth_hid_run: error in netif_add");
        return(ipal_res_NOK_generic);
    }
        
    /* config other device capabilities */
    
    /* maximum transfer unit */
    s_ipal_eth_netif.netif.mtu = IPAL_ETH_MTU;
    
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    s_ipal_eth_netif.netif.flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

    /*  Registers the default network interface.*/
    netif_set_default(&s_ipal_eth_netif.netif);

    /*  When the netif is fully configured this function must be called.*/
    netif_set_up(&s_ipal_eth_netif.netif);
    
    return(ipal_res_OK);
  
}

extern ipal_eth_hid_netif * ipal_eth_hid_getnetif(void)
{
    return(&s_ipal_eth_netif);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_ipal_eth_internal_ram_reset(void)
{
    memset(&s_ipal_eth_netif, 0, sizeof(s_ipal_eth_netif));
}

static void s_ipal_eth_internal_ram_init(void)
{
    //nothing to do
}



#endif//IPAL_USE_ETH

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




