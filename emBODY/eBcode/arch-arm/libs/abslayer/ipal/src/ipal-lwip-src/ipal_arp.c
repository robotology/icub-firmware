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


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"


#include "lwip_interface.h"

#include "ipal_base_hid.h"
#include "ipal_sys_hid.h"
#include "ipal_eth_hid.h"


// - storage for ipal
#include "ipal_ipiit_storage.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "ipal.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "ipal_arp_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


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

static void s_ipal_arp_internal_ram_reset(void);
static void s_ipal_arp_internal_ram_init(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern ipal_result_t ipal_arp_request(ipal_ipv4addr_t ip, ipal_arp_cachemode_t cm)
{
    int8_t ret;
    err_t err;
    ipal_eth_hid_netif * netif_ptr;
    struct ip_addr ipaddr;
    struct eth_addr *eth_ret;
    struct ip_addr  *ip_ret;
    
    
#warning: arp cache mode useless
    //in lwip i can not found something similar to cachemode.
    
    if(0 == ipal_sys_hid_started)
    {
        return(ipal_res_NOK_generic);
    }

    ipaddr.addr = ipal2lwip_ipv4addr(ip);
    
    ipal_base_hid_threadsafety_lock();

    netif_ptr = ipal_eth_hid_getnetif();
    ret = etharp_find_addr(&(netif_ptr->netif), &ipaddr, &eth_ret, &ip_ret);
    if(ret != -1 )
    {
        //ip addr presente in arp tbl, allora ritorno con sucesso
        ipal_base_hid_threadsafety_unlock();
        return(ipal_res_OK);
    }
      
    //se sopno qui ==> ip addr non presente in tbl arp ==> faccio una request
    err = etharp_request(&(netif_ptr->netif), &ipaddr);
    
    ipal_base_hid_threadsafety_unlock();

    //return( (ERR_OK == err) ? (ipal_res_OK) : (ipal_res_NOK_generic) );
    //se sono qui vuol dire che devoi fare una arp request e che quindi la mia arp table non contiene ancora nessuna entry relatina a @ip
    return(ipal_res_NOK_generic);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern uint32_t ipal_arp_hid_getsize(const ipal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern ipal_result_t ipal_arp_hid_setmem(const ipal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
    return(ipal_res_OK); 
}

extern ipal_result_t ipal_arp_hid_init(ipal_cfg_t *cfg)
{
    // nothing is required
    return(ipal_res_OK); 
}

extern ipal_result_t ipal_arp_hid_vars_init(const ipal_cfg_t *cfg)
{
    if( (IPAL_NUMBEROF_ARPCACHETABLEMAXSIZE < cfg->arp_cachesize) )
    {
        return(ipal_res_NOK_generic);
    }
    
    // we reset the static memory and we init internal variables
    s_ipal_arp_internal_ram_reset();
    s_ipal_arp_internal_ram_init();
    
 
    // we initialise ipiit storage
    ipal_ipiit_storage_arp_init(cfg);
    
    return(ipal_res_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_ipal_arp_internal_ram_reset(void)
{
    // nothing to do
}

static void s_ipal_arp_internal_ram_init(void)
{
    // nothing to do
}



#endif//IPAL_USE_ARP

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




