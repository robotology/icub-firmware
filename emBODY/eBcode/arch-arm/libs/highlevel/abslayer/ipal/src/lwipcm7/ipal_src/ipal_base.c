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

/* @file       ipal_base.c
	@brief      This file implements internal implementation of the ipal base module
	@author     valentina.gaggero@iit.it
    @date       01/07/2013
**/

// - modules to be built: contains the IPAL_USE_* macros --------------------------------------------------------------
#include "ipal_modules.h"

// - sizes of modules: contains the IPAL_NUMBEROF_* macros ----------------------------------------------------------------
#include "ipal_numberof.h"

#ifdef IPAL_USE_BASE

#include "stdlib.h"
#include "string.h"
#include "time.h"

/* Friend functions */
#include "ipal_hid.h"

/* Storage for IPAL */
//#include "ipal_ipiit_storage.h"

/* API interface */
#include "ipal.h"

/* ====================================================================================================================
 * GLOBAL (HIDDEN) VARIABLES
 * ==================================================================================================================== */

/* IPAL BASE Configuration. 
	 This is a copy, used within the library context of the cfg passed as parameter to Initialise function 
 */ 
 

ipal_cfg2_support_t ipal_base_hid_cfg2_supp = { 0 };
ipal_cfg2_system_t ipal_base_hid_cfg2_sys = { 0 };
ipal_cfg2_eth_t ipal_base_hid_cfg2_eth = { 0 };
ipal_cfg2_arp_t ipal_base_hid_cfg2_arp = { 0 };
ipal_cfg2_udp_t ipal_base_hid_cfg2_udp = { 0 };
ipal_cfg2_dhcp_t ipal_base_hid_cfg2_dhcp = { 0 };
ipal_cfg2_igmp_t ipal_base_hid_cfg2_igmp = { 0 };
ipal_cfg2_extfn_t ipal_base_hid_cfg2_extfn2 = { 0 };

ipal_cfg2_t ipal_base_hid_cfg2 = { 0 };

const uint16_t ipal_cfg2_supportedflags_basic = ipal_cfg2_supportedflag_eth | ipal_cfg2_supportedflag_arp |
                                                ipal_cfg2_supportedflag_udp | ipal_cfg2_supportedflag_igmp |
                                                ipal_cfg2_supportedflag_extfn2;

#warning set dummy functions so that it is never full of NULL pointers

/* IPAL BASE mutex */
void *ipal_base_hid_mutex = NULL;

/* ====================================================================================================================
 * PRIVATE FUNCTIONS DECLARATIONS
 * They are defined at the end of this file
 * ==================================================================================================================== */

static void s_ipal_base_clear_internal_data(void);

static ipal_result_t s_ipal_base_verify_config(const ipal_cfg2_t *cfg2);

/*
 * Initializes internal vars of modules specified in cfg
 */
static ipal_result_t s_ipal_base_vars_init(const ipal_cfg2_t *cfg2);

/*
 * On the base of te given configuration, returns the required memory,
 * as as sum of memory required by each of the enabled modules
 * cfg - IPAL configuration
 */
static uint32_t s_ipal_base_global_getsize(const ipal_cfg2_t *cfg2);

/*
 * Gives memory to the single modules.
 * cfg - IPAL configuration
 * data04aligned - Memory bunch entry point. This memory bunch must be big enough to contain all 
 *								 the memory required by the single modules
 */
static ipal_result_t s_ipal_base_global_setmem(const ipal_cfg2_t *cfg2, uint32_t *data04aligned);

/*
 * Initializes all modules
 * cfg - IPAL configuration
 */
static ipal_result_t s_ipal_base_global_init(ipal_cfg2_t *cfg2);

/*
 * Resets internal variables
 */
static void s_ipal_base_internal_ram_reset(const ipal_cfg2_t *cfg2);

/*
 * Initializes internal variables
 */
static void s_ipal_base_internal_ram_init(const ipal_cfg2_t *cfg2);

/*
 * On the base of configuration, checks wether the requested services are coherent to
 * supported ones
 */
static ipal_boolval_t s_ipal_base_requiredservicescheck(const ipal_cfg2_t *cfg2);

    
static void s_ipal_base_default_on_fatal_error(ipal_fatalerror_t errorcode, const char * errormsg)
{
    for(;;)
    {
        errorcode = errorcode;
    }
}

static ipal_result_t s_dummy_hal_eth_init(void *cfg) { return ipal_res_NOK_generic; }
static ipal_result_t s_dummy_hal_eth_enable(void) { return ipal_res_NOK_generic; }
static ipal_result_t s_dummy_hal_eth_disable() { return ipal_res_NOK_generic; }
static ipal_result_t s_dummy_hal_eth_sendframe(void *frame) { return ipal_res_NOK_generic; }
static uint16_t s_dummy_get_frame_size(void) { return 0; }
static void s_dummy_get_frame(uint16_t length, uint8_t* frame) { }    
static void s_dummy_signal_rx_frame(void) {}
static void s_dummy_signal_tx_frame(void) {}

/* ====================================================================================================================
 * PUBLIC FUNCTIONS (APIs) 
 * ==================================================================================================================== */

/*
 * ipal_base_memory_getsize
 * See header file for details
 */
uint32_t ipal_base_memory_getsize(const ipal_cfg_t *cfg, uint32_t *size04aligned)
{
#if 1
    return(0);
#else    
#endif    
}

                                    
/*
 * ipal_base_initialise
 * See header file for details
 */
ipal_result_t ipal_base_initialise(const ipal_cfg_t *cfg, uint32_t *data04aligned)
{
#if 1
    return(ipal_res_NOK_generic);
#else
#endif    
}

uint32_t ipal_base_memory_getsize2(const ipal_cfg2_t *cfg2, uint32_t *size04aligned)
{
	uint32_t retval = 0;
    
    if(ipal_res_OK != s_ipal_base_verify_config(cfg2))
    {
        ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_base_memory_getsize2(): called w/ an invalid cfg2");
        return 0;
    }    

	/* IPAL base size is the sum of all enabled modules size */
	retval = s_ipal_base_global_getsize(cfg2);
    

	if(NULL != size04aligned)
	{
		*size04aligned = retval;
	}
	
	return(retval);
}

ipal_result_t ipal_base_initialise2(const ipal_cfg2_t *cfg2, uint32_t *data04aligned)
{    
    // error if IPAL is already started. 
	if(ipal_sys_hid_started == 1)
	{   
		ipal_base_hid_on_fatalerror(ipal_error_generic, "ipal_base_initialise(): already started");
		return(ipal_res_NOK_generic);
	}
    
    if(ipal_res_OK != s_ipal_base_verify_config(cfg2))
    {
        ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_base_initialise2(): called w/ an invalid cfg2");
        return ipal_res_NOK_generic;
    }
        
    /* Checks whether the configuration is coherent with supported services */ 
    // marco.accame: review it because s_ipal_base_requiredservicescheck() may be redundant or overlapped 
    // w/ s_ipal_base_verify_config()
	if(!s_ipal_base_requiredservicescheck(cfg2))
	{
		return(ipal_res_NOK_generic);
	}

	// initialize internal modules variables. 
    // inside here we reset ram and ... we also copy cfg2 into local copies
    // from after that we can use ipal_base_hid_cfg2 and its pointers
	if(s_ipal_base_vars_init(cfg2) != ipal_res_OK)
	{
		ipal_base_hid_on_fatalerror(ipal_error_incorrectparameter, "ipal_base_initialise(): tcpnet resources are not enough for ipal");
		return(ipal_res_NOK_generic);
	}
    
    /* Gives memory to internal modules */
	if(s_ipal_base_global_setmem(cfg2, data04aligned) != ipal_res_OK)
	{
		return(ipal_res_NOK_generic);
	}
    
    /* Calls modules initialization */
	if(ipal_res_OK != s_ipal_base_global_init(&ipal_base_hid_cfg2))
	{
		return(ipal_res_NOK_generic);
	}    
	
	return(ipal_res_OK);
}

/* ====================================================================================================================
 * FRIEND FUNCTIONS
 * ==================================================================================================================== */

/*
 * ipal_base_hid_getsize
 * See header file for details
 */
uint32_t ipal_base_hid_getsize(const ipal_cfg2_t *cfg2)
{
    // ne memory needed
	return(0);
}

/*
 * ipal_base_hid_setmem
 * See header file for details
 */
ipal_result_t ipal_base_hid_setmem(const ipal_cfg2_t *cfg2, uint32_t *memory)
{
	return(ipal_res_OK); 
}


/*
 * ipal_base_hid_init
 * See header file for details
 */
ipal_result_t ipal_base_hid_init(ipal_cfg2_t *cfg2)
{  
	/* Initialize locking system */	
	if(NULL == cfg2->extfn2->osal_mutex_new)
	{
		// put others to NULL as well
		cfg2->extfn2->osal_mutex_take           = NULL;
		cfg2->extfn2->osal_mutex_release        = NULL;
		cfg2->extfn2->osal_param_tout_forever   = 0;
		ipal_base_hid_mutex                     = NULL;
	}
	else if((NULL == cfg2->extfn2->osal_mutex_take) || (NULL == cfg2->extfn2->osal_mutex_release) || (0 == cfg2->extfn2->osal_param_tout_forever))
	{
		ipal_base_hid_on_fatalerror(ipal_error_incorrectparameter, "ipal_base_initialise2() has incomplete osal functions");
		return(ipal_res_NOK_generic);
	}
	else
	{
		ipal_base_hid_threadsafety_init();
	}   
    
  return(ipal_res_OK); 
}

/*
 * ipal_base_hid_vars_init
 * See header file for details
 */
ipal_result_t ipal_base_hid_vars_init(const ipal_cfg2_t *cfg2)
{
	// we reset the internally allocated memory and we init internal variables
	s_ipal_base_internal_ram_reset(cfg2);
	s_ipal_base_internal_ram_init(cfg2);
	
	return(ipal_res_OK);
}

/*
 * ipal_base_hid_on_fatalerror
 * See header file for details
 */
void ipal_base_hid_on_fatalerror(ipal_fatalerror_t errorcode, const char * errormsg)
{
	if(NULL != ipal_base_hid_cfg2_extfn2.usr_on_fatal_error)
	{
		ipal_base_hid_cfg2_extfn2.usr_on_fatal_error(errorcode, errormsg);
	}
	else
	{
		s_ipal_base_default_on_fatal_error(errorcode, errormsg);
	}
}

/*
 * ipal_base_hid_threadsafety_init
 * See header file for details
 */
void ipal_base_hid_threadsafety_init(void)
{
	if(NULL != ipal_base_hid_cfg2_extfn2.osal_mutex_new)
	{
		ipal_base_hid_mutex = ipal_base_hid_cfg2_extfn2.osal_mutex_new();
		if(NULL == ipal_base_hid_mutex)
		{
			ipal_base_hid_on_fatalerror(ipal_error_generic, "ipal_base_hid_threadsafety_init(): cannot allocate the mutex");
		}
	}
}

/*
 * ipal_base_hid_threadsafety_lock
 * See header file for details
 */
void ipal_base_hid_threadsafety_lock(void)
{
	if(NULL != ipal_base_hid_mutex)
	{
		ipal_base_hid_cfg2_extfn2.osal_mutex_take(ipal_base_hid_mutex, ipal_base_hid_cfg2_extfn2.osal_param_tout_forever);
	}
}

/*
 * ipal_base_hid_threadsafety_unlock
 * See header file for details
 */
void ipal_base_hid_threadsafety_unlock(void)
{
	if(NULL != ipal_base_hid_mutex)
	{
		ipal_base_hid_cfg2_extfn2.osal_mutex_release(ipal_base_hid_mutex);
	}
}

/*
 * ipal_base_hid_addr_array_to_ipv4
 * See header file for details
 */
ipal_ipv4addr_t ipal_base_hid_addr_array_to_ipv4(ipal_arrayaddr_t iparray)
{
	return(*((uint32_t*)iparray));
}

/* ====================================================================================================================
 * PRIVATE FUNCTIONS DEFINITIONS
 * ==================================================================================================================== */

static void s_ipal_base_clear_internal_data(void)
{
    // clear ram
    memset(&ipal_base_hid_cfg2, 0, sizeof(ipal_cfg2_t));
    memset(&ipal_base_hid_cfg2_supp, 0, sizeof(ipal_cfg2_support_t));
    memset(&ipal_base_hid_cfg2_sys, 0, sizeof(ipal_cfg2_system_t));
    memset(&ipal_base_hid_cfg2_arp, 0, sizeof(ipal_cfg2_arp_t));
    memset(&ipal_base_hid_cfg2_eth, 0, sizeof(ipal_cfg2_eth_t));
    memset(&ipal_base_hid_cfg2_udp, 0, sizeof(ipal_cfg2_udp_t));
    memset(&ipal_base_hid_cfg2_igmp, 0, sizeof(ipal_cfg2_igmp_t));
    memset(&ipal_base_hid_cfg2_dhcp, 0, sizeof(ipal_cfg2_dhcp_t));
    memset(&ipal_base_hid_cfg2_extfn2, 0, sizeof(ipal_cfg2_extfn_t));
    // link all together 
    ipal_base_hid_cfg2.support = &ipal_base_hid_cfg2_supp;
    ipal_base_hid_cfg2.system = &ipal_base_hid_cfg2_sys;
    ipal_base_hid_cfg2.arp = &ipal_base_hid_cfg2_arp;
    ipal_base_hid_cfg2.eth = &ipal_base_hid_cfg2_eth;
    ipal_base_hid_cfg2.udp = &ipal_base_hid_cfg2_udp;
    ipal_base_hid_cfg2.igmp = &ipal_base_hid_cfg2_igmp;
    ipal_base_hid_cfg2.dhcp = &ipal_base_hid_cfg2_dhcp;
    ipal_base_hid_cfg2.extfn2 = &ipal_base_hid_cfg2_extfn2;
    // and perform a basic minimal initialization
    // of at least the fatal error handler
    ipal_base_hid_cfg2_extfn2.usr_on_fatal_error = s_ipal_base_default_on_fatal_error;

    ipal_base_hid_mutex = NULL;
}

static ipal_result_t s_ipal_base_verify_config(const ipal_cfg2_t *cfg2)
{
    // we must have non NULL params
    
	if(cfg2 == NULL)
	{
		ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_base_initialise2(): missing cfg2");
		return(ipal_res_NOK_generic);
	}
     
    if(NULL == cfg2->support)
    {
		ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_base_initialise2(): support config is missing");
		return(ipal_res_NOK_generic);        
    }  
    else
    {
        // it must be LWIP 
        if(ipal_ipstack_iitmod_lwip != cfg2->support->arch_ipstack)
        {
            ipal_base_hid_on_fatalerror(ipal_error_unsupportedbehaviour, "ipal_base_initialise(): unsupported tcp/ip stack");
            return(ipal_res_NOK_generic);
        }    

        if(ipal_memmodel_static != cfg2->support->memorymodel)
        {
            ipal_base_hid_on_fatalerror(ipal_error_unsupportedbehaviour, "ipal_base_initialise(): unsupported memory mode");
            return(ipal_res_NOK_generic);
        }  
        
        if(ipal_cfg2_supportedflags_basic != cfg2->support->supportedmodules)
        {
            ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_base_initialise2(): only basic modules are supported");
            return(ipal_res_NOK_generic);
        } 
    }
    
    if(NULL == cfg2->system)
    {
		ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_base_initialise2(): system config is missing");
		return(ipal_res_NOK_generic);        
    }
    
    if(NULL == cfg2->eth)
    {
		ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_base_initialise2(): eth config is missing");
		return(ipal_res_NOK_generic);        
    }
    else
    {
        if((NULL == cfg2->eth->get_frame) || (NULL == cfg2->eth->get_frame_size) || 
           (NULL == cfg2->eth->hal_eth_disable) || (NULL == cfg2->eth->hal_eth_enable) ||
           (NULL == cfg2->eth->hal_eth_init) || (NULL == cfg2->eth->hal_eth_sendframe)
          )
        {
            ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_base_initialise2(): some eth config functions are missing");
            return(ipal_res_NOK_generic);           
        }
    }
    
    if(NULL == cfg2->arp)
    {
		ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_base_initialise2(): arp config is missing");
		return(ipal_res_NOK_generic);        
    }

    if(NULL == cfg2->udp)
    {
		ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_base_initialise2(): udp config is missing");
		return(ipal_res_NOK_generic);        
    }
 
    if(NULL == cfg2->igmp)
    {
		ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_base_initialise2(): igmp config is missing");
		return(ipal_res_NOK_generic);        
    }

    if(NULL == cfg2->dhcp)
    {
        // it can be NULL
    }
    
    if(NULL == cfg2->extfn2)
    {
		ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_base_initialise2(): extfn2 config is missing");
		return(ipal_res_NOK_generic);        
    } 
    else
    {
        // usr_on_fatal_error can be NULL
        // signal_rx_frame and/or signal_tx_frame can be NULL
        // osal_mutex_new can be NULL. if it is NULL then no rtos protection
    }    
       

    return ipal_res_OK;   
}

/*
 * s_ipal_base_vars_init
 * See declaration for details
 */
static ipal_result_t s_ipal_base_vars_init(const ipal_cfg2_t *cfg2)
{
    // marco.accame: ipal_base_hid_vars_init() inits ipal_base_hid_cfg2 
    // and its pointers w/ the content of cfg2
    // SO ... keep it first so that after that all the code can safely use 
    // ipal_base_hid_cfg2
    
#ifdef IPAL_USE_BASE
	if(ipal_res_OK != ipal_base_hid_vars_init(cfg2))
	{
		return(ipal_res_NOK_generic);
	}
#endif//IPAL_USE_BASE
    
#ifdef IPAL_USE_ARP
	if(ipal_res_OK != ipal_arp_hid_vars_init(cfg2))
	{
		return(ipal_res_NOK_generic);
	}
#endif//IPAL_USE_ARP    

#ifdef IPAL_USE_DHCP
	if(ipal_res_OK != ipal_dhcp_hid_vars_init(cfg))
	{
		return(ipal_res_NOK_generic);
	}
#endif//IPAL_USE_DHCP

#ifdef IPAL_USE_ETH
	if(ipal_res_OK != ipal_eth_hid_vars_init(cfg2))
	{
		return(ipal_res_NOK_generic);
	}
#endif//IPAL_USE_ETH

#ifdef IPAL_USE_FTP
	if(ipal_res_OK != ipal_ftp_hid_vars_init(cfg))
	{
		return(ipal_res_NOK_generic);
	}
#endif//IPAL_USE_FTP

#ifdef IPAL_USE_IGMP
	if(ipal_res_OK != ipal_igmp_hid_vars_init(cfg2))
	{
		return(ipal_res_NOK_generic);
	}
#endif//IPAL_USE_IGMP

#ifdef IPAL_USE_SYS
	if(ipal_res_OK != ipal_sys_hid_vars_init(cfg2))
	{
		return(ipal_res_NOK_generic);
	}
#endif//IPAL_USE_SYS

#ifdef IPAL_USE_TCP
	if(ipal_res_OK != ipal_tcp_hid_vars_init(cfg))
	{
		return(ipal_res_NOK_generic);
	}
#endif//IPAL_USE_TCP

#ifdef IPAL_USE_TELNET
	if(ipal_res_OK != ipal_telnet_hid_vars_init(cfg))
	{
		return(ipal_res_NOK_generic);
	}
#endif//IPAL_USE_TELNET

#ifdef IPAL_USE_TFTP
	if(ipal_res_OK != ipal_tftp_hid_vars_init(cfg))
	{
		return(ipal_res_NOK_generic);
	}
#endif//IPAL_USE_TFTP

#ifdef IPAL_USE_UDP
	if(ipal_res_OK != ipal_udp_hid_vars_init(cfg2))
	{
		return(ipal_res_NOK_generic);
	}
#endif//IPAL_USE_UDP

  return(ipal_res_OK);      
}

/*
 * s_ipal_base_global_getsize
 * See declaration for details
 */
static uint32_t s_ipal_base_global_getsize(const ipal_cfg2_t *cfg2)
{
	uint32_t retval = 0;

#ifdef IPAL_USE_BASE
	retval += ipal_base_hid_getsize(cfg2);
#endif//IPAL_USE_BASE
    
#ifdef IPAL_USE_ARP
	retval += ipal_arp_hid_getsize(cfg2);
#endif//IPAL_USE_ARP

#ifdef IPAL_USE_DHCP
    retval += ipal_dhcp_hid_getsize(cfg);
#endif//IPAL_USE_DHCP

#ifdef IPAL_USE_ETH
    retval += ipal_eth_hid_getsize(cfg2);
#endif//IPAL_USE_ETH

#ifdef IPAL_USE_FTP
    retval += ipal_ftp_hid_getsize(cfg);
#endif//IPAL_USE_FTP

#ifdef IPAL_USE_IGMP
    retval += ipal_igmp_hid_getsize(cfg2);
#endif//IPAL_USE_IGMP

#ifdef IPAL_USE_SYS
    retval += ipal_sys_hid_getsize(cfg2);
#endif//IPAL_USE_SYS

#ifdef IPAL_USE_TCP
    retval += ipal_tcp_hid_getsize(cfg);
#endif//IPAL_USE_TCP

#ifdef IPAL_USE_TELNET
    retval += ipal_telnet_hid_getsize(cfg);
#endif//IPAL_USE_TELNET

#ifdef IPAL_USE_TFTP
    retval += ipal_tftp_hid_getsize(cfg);
#endif//IPAL_USE_TFTP

#ifdef IPAL_USE_UDP
    retval += ipal_udp_hid_getsize(cfg2);
#endif//IPAL_USE_UDP

    return(retval);
}

/*
 * s_ipal_base_global_setmem
 * See declaration for details
 */
static ipal_result_t s_ipal_base_global_setmem(const ipal_cfg2_t *cfg2, uint32_t *data04aligned)
{

#ifdef IPAL_USE_BASE
	if(ipal_res_OK != ipal_base_hid_setmem(cfg2, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_base_hid_getsize(cfg2)/4;
#endif//IPAL_USE_BASE

#ifdef IPAL_USE_ARP
	if(ipal_res_OK != ipal_arp_hid_setmem(cfg2, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_arp_hid_getsize(cfg2)/4;
#endif//IPAL_USE_ARP
    
#ifdef IPAL_USE_DHCP
	if(ipal_res_OK != ipal_dhcp_hid_setmem(cfg, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_dhcp_hid_getsize(cfg)/4;
#endif//IPAL_USE_DHCP

#ifdef IPAL_USE_ETH
	if(ipal_res_OK != ipal_eth_hid_setmem(cfg2, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_eth_hid_getsize(cfg2)/4;
#endif//IPAL_USE_ETH

#ifdef IPAL_USE_FTP
	if(ipal_res_OK != ipal_ftp_hid_setmem(cfg, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_ftp_hid_getsize(cfg)/4;
#endif//IPAL_USE_FTP

#ifdef IPAL_USE_IGMP
	if(ipal_res_OK != ipal_igmp_hid_setmem(cfg2, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_igmp_hid_getsize(cfg2)/4;
#endif//IPAL_USE_IGMP

#ifdef IPAL_USE_SYS
	if(ipal_res_OK != ipal_sys_hid_setmem(cfg2, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_sys_hid_getsize(cfg2)/4;
#endif//IPAL_USE_SYS

#ifdef IPAL_USE_TCP
	if(ipal_res_OK != ipal_tcp_hid_setmem(cfg, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_tcp_hid_getsize(cfg)/4;
#endif//IPAL_USE_TCP

#ifdef IPAL_USE_TELNET
	if(ipal_res_OK != ipal_telnet_hid_setmem(cfg, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_telnet_hid_getsize(cfg)/4;
#endif//IPAL_USE_TELNET

#ifdef IPAL_USE_TFTP
	if(ipal_res_OK != ipal_tftp_hid_setmem(cfg, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_tftp_hid_getsize(cfg)/4;
#endif//IPAL_USE_TFTP

#ifdef IPAL_USE_UDP
	if(ipal_res_OK != ipal_udp_hid_setmem(cfg2, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_udp_hid_getsize(cfg2)/4;
#endif//IPAL_USE_UDP 

	return(ipal_res_OK);
}


/*
 * s_ipal_base_global_init
 * See declaration for details
 */
static ipal_result_t s_ipal_base_global_init(ipal_cfg2_t *cfg2)
{
#ifdef IPAL_USE_BASE
    if(ipal_res_OK != ipal_base_hid_init(cfg2))
    {
        return(ipal_res_NOK_generic);
    }
#endif//IPAL_USE_BASE
    
#ifdef IPAL_USE_ARP
    if(ipal_res_OK != ipal_arp_hid_init(cfg2))
    {
        return(ipal_res_NOK_generic);
    }
#endif//IPAL_USE_ARP

#ifdef IPAL_USE_DHCP
    if(ipal_res_OK != ipal_dhcp_hid_init(cfg))
    {
        return(ipal_res_NOK_generic);
    }
#endif//IPAL_USE_DHCP

#ifdef IPAL_USE_ETH
    if(ipal_res_OK != ipal_eth_hid_init(cfg2))
    {
        return(ipal_res_NOK_generic);
    }
#endif//IPAL_USE_ETH

#ifdef IPAL_USE_FTP
    if(ipal_res_OK != ipal_ftp_hid_init(cfg))
    {
        return(ipal_res_NOK_generic);
    }
#endif//IPAL_USE_FTP

#ifdef IPAL_USE_IGMP
    if(ipal_res_OK != ipal_igmp_hid_init(cfg2))
    {
        return(ipal_res_NOK_generic);
    }
#endif//IPAL_USE_IGMP

#ifdef IPAL_USE_SYS
    if(ipal_res_OK != ipal_sys_hid_init(cfg2))
    {
        return(ipal_res_NOK_generic);
    }
#endif//IPAL_USE_SYS

#ifdef IPAL_USE_TCP
    if(ipal_res_OK != ipal_tcp_hid_init(cfg))
    {
        return(ipal_res_NOK_generic);
    }
#endif//IPAL_USE_TCP

#ifdef IPAL_USE_TELNET
    if(ipal_res_OK != ipal_telnet_hid_init(cfg))
    {
        return(ipal_res_NOK_generic);
    }
#endif//IPAL_USE_TELNET

#ifdef IPAL_USE_TFTP
    if(ipal_res_OK != ipal_tftp_hid_init(cfg))
    {
        return(ipal_res_NOK_generic);
    }
#endif//IPAL_USE_TFTP

#ifdef IPAL_USE_UDP
    if(ipal_res_OK != ipal_udp_hid_init(cfg2))
    {
        return(ipal_res_NOK_generic);
    }
#endif//IPAL_USE_UDP 

    return(ipal_res_OK);
}

/*
 * s_ipal_base_internal_ram_reset
 * See declaration for details
 */
static void s_ipal_base_internal_ram_reset(const ipal_cfg2_t *cfg2)
{
    // clear ram
    memset(&ipal_base_hid_cfg2, 0, sizeof(ipal_cfg2_t));
    memset(&ipal_base_hid_cfg2_supp, 0, sizeof(ipal_cfg2_support_t));
    memset(&ipal_base_hid_cfg2_sys, 0, sizeof(ipal_cfg2_system_t));
    memset(&ipal_base_hid_cfg2_arp, 0, sizeof(ipal_cfg2_arp_t));
    memset(&ipal_base_hid_cfg2_eth, 0, sizeof(ipal_cfg2_eth_t));
    memset(&ipal_base_hid_cfg2_udp, 0, sizeof(ipal_cfg2_udp_t));
    memset(&ipal_base_hid_cfg2_igmp, 0, sizeof(ipal_cfg2_igmp_t));
    memset(&ipal_base_hid_cfg2_dhcp, 0, sizeof(ipal_cfg2_dhcp_t));
    memset(&ipal_base_hid_cfg2_extfn2, 0, sizeof(ipal_cfg2_extfn_t));
    // link all together 
    ipal_base_hid_cfg2.support = &ipal_base_hid_cfg2_supp;
    ipal_base_hid_cfg2.system = &ipal_base_hid_cfg2_sys;
    ipal_base_hid_cfg2.arp = &ipal_base_hid_cfg2_arp;
    ipal_base_hid_cfg2.eth = &ipal_base_hid_cfg2_eth;
    ipal_base_hid_cfg2.udp = &ipal_base_hid_cfg2_udp;
    ipal_base_hid_cfg2.igmp = &ipal_base_hid_cfg2_igmp;
    ipal_base_hid_cfg2.dhcp = &ipal_base_hid_cfg2_dhcp;
    ipal_base_hid_cfg2.extfn2 = &ipal_base_hid_cfg2_extfn2;

    ipal_base_hid_mutex = NULL;
}

/*
 * s_ipal_base_internal_ram_init
 * See declaration for details
 */
static void s_ipal_base_internal_ram_init(const ipal_cfg2_t *cfg2)
{
    // copy locally the configuration.
	// dont copy int ipal_base_hid_cfg2 it because it is just a container of pointers
    // memcpy(&ipal_base_hid_cfg2, cfg2, sizeof(ipal_cfg2_t));
    memcpy(&ipal_base_hid_cfg2_supp, cfg2->support, sizeof(ipal_cfg2_support_t));
    memcpy(&ipal_base_hid_cfg2_sys, cfg2->system, sizeof(ipal_cfg2_system_t));
    memcpy(&ipal_base_hid_cfg2_eth, cfg2->eth, sizeof(ipal_cfg2_eth_t));
    memcpy(&ipal_base_hid_cfg2_arp, cfg2->arp, sizeof(ipal_cfg2_arp_t));
    memcpy(&ipal_base_hid_cfg2_udp, cfg2->udp, sizeof(ipal_cfg2_udp_t));
    memcpy(&ipal_base_hid_cfg2_igmp, cfg2->igmp, sizeof(ipal_cfg2_igmp_t));
    memcpy(&ipal_base_hid_cfg2_extfn2, cfg2->extfn2, sizeof(ipal_cfg2_extfn_t));
    // link all together 
    ipal_base_hid_cfg2.support = &ipal_base_hid_cfg2_supp;
    ipal_base_hid_cfg2.system = &ipal_base_hid_cfg2_sys;
    ipal_base_hid_cfg2.arp = &ipal_base_hid_cfg2_arp;
    ipal_base_hid_cfg2.eth = &ipal_base_hid_cfg2_eth;
    ipal_base_hid_cfg2.udp = &ipal_base_hid_cfg2_udp;
    ipal_base_hid_cfg2.igmp = &ipal_base_hid_cfg2_igmp;
    ipal_base_hid_cfg2.dhcp = &ipal_base_hid_cfg2_dhcp;
    ipal_base_hid_cfg2.extfn2 = &ipal_base_hid_cfg2_extfn2;
}

/*
 * s_ipal_base_requiredservicescheck
 * See declaration for details
 */
static ipal_boolval_t s_ipal_base_requiredservicescheck(const ipal_cfg2_t *cfg2)
{
#if 1
    #warning add in here the check
    return(ipal_true);
#else    
	if(cfg->igmp_enable)
	{
		ipal_base_hid_on_fatalerror(ipal_error_unsupportedbehaviour, "ipal_base_initialise(): igmp unsupported");
		return(ipal_false);
	}
    
	if(cfg->dhcp_enable)
	{
		ipal_base_hid_on_fatalerror(ipal_error_unsupportedbehaviour, "ipal_base_initialise(): dhcp unsupported");
		return(ipal_false);
	}
    
	if(cfg->tcp_enable)
	{
		ipal_base_hid_on_fatalerror(ipal_error_unsupportedbehaviour, "ipal_base_initialise(): tcp unsupported");
		return(ipal_false);
	}

	if(cfg->tftp_enable)
	{
		ipal_base_hid_on_fatalerror(ipal_error_unsupportedbehaviour, "ipal_base_initialise(): tftp unsupported");
		return(ipal_false);
	}
    
	if(cfg->ftp_enable)
	{
		ipal_base_hid_on_fatalerror(ipal_error_unsupportedbehaviour, "ipal_base_initialise(): ftp unsupported");
		return(ipal_false);
	}
    
	if(cfg->tel_enable)
	{
		ipal_base_hid_on_fatalerror(ipal_error_unsupportedbehaviour, "ipal_base_initialise(): telnet unsupported");
		return(ipal_false);
	}

	return(ipal_true);
#endif    
}

#endif

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




