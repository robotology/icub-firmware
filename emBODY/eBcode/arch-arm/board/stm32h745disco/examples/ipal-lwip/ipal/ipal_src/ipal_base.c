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
ipal_cfg_t ipal_base_hid_cfg;

/* IPAL BASE mutex */
void *ipal_base_hid_mutex = NULL;

/* ====================================================================================================================
 * PRIVATE FUNCTIONS DECLARATIONS
 * They are defined at the end of this file
 * ==================================================================================================================== */

/*
 * Initializes internal vars of modules specified in cfg
 */
static ipal_result_t s_ipal_base_vars_init(const ipal_cfg_t *cfg);

/*
 * On the base of te given configuration, returns the required memory,
 * as as sum of memory required by each of the enabled modules
 * cfg - IPAL configuration
 */
static uint32_t s_ipal_base_global_getsize(const ipal_cfg_t *cfg);

/*
 * Gives memory to the single modules.
 * cfg - IPAL configuration
 * data04aligned - Memory bunch entry point. This memory bunch must be big enough to contain all 
 *								 the memory required by the single modules
 */
static ipal_result_t s_ipal_base_global_setmem(const ipal_cfg_t *cfg, uint32_t *data04aligned);

/*
 * Initializes all modules
 * cfg - IPAL configuration
 */
static ipal_result_t s_ipal_base_global_init(ipal_cfg_t *cfg);

/*
 * Resets internal variables
 */
static void s_ipal_base_internal_ram_reset(void);

/*
 * Initializes internal variables
 */
static void s_ipal_base_internal_ram_init(void);

/*
 * On the base of configuration, checks wether the requested services are coherent to
 * supported ones
 */
static ipal_boolval_t s_ipal_base_requiredservicescheck(const ipal_cfg_t *cfg);

/* ====================================================================================================================
 * PUBLIC FUNCTIONS (APIs) 
 * ==================================================================================================================== */

/*
 * ipal_base_memory_getsize
 * See header file for details
 */
uint32_t ipal_base_memory_getsize(const ipal_cfg_t *cfg, uint32_t *size04aligned)
{
	uint32_t retval = 0;

	/* Param coherency check */
	if(NULL == cfg)
	{
		ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_base_memory_getsize() needs a cfg");
		return(0);
	}

	/* IPAL base size is the sum of all enabled modules size */
	retval = s_ipal_base_global_getsize(cfg);

	/* MARTINO - Redundant 
		 Required size is already passed as return values, it makes no sense to pass it also as reference parameter */
	if(NULL != size04aligned)
	{
		*size04aligned = retval;
	}
	
	return(retval);
}
                                    
/*
 * ipal_base_initialise
 * See header file for details
 */
ipal_result_t ipal_base_initialise(const ipal_cfg_t *cfg, uint32_t *data04aligned)
{
	/* Param coherency check */
	if(cfg == NULL)
	{
		ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_base_initialise(): missing cfg");
		return(ipal_res_NOK_generic);
	}

  /* Copy error function in advance */  
	ipal_base_hid_cfg.extfn.usr_on_fatal_error = cfg->extfn.usr_on_fatal_error;
  
	/* It must be LWIP */
	if(cfg->arch_ipstack != ipal_ipstack_iitmod_lwip)
	{
		ipal_base_hid_on_fatalerror(ipal_error_unsupportedbehaviour, "ipal_base_initialise(): unsupported tcp/ip stack");
		return(ipal_res_NOK_generic);
	}
    
  /* Checks wether the configuration is coherent with supported services */ 
	if(!s_ipal_base_requiredservicescheck(cfg))
	{
		return(ipal_res_NOK_generic);
	}

	/* Error if IPAL is already started */ 
	if(ipal_sys_hid_started == 1)
	{
		ipal_base_hid_on_fatalerror(ipal_error_generic, "ipal_base_initialise(): already started");
		return(ipal_res_NOK_generic);
	}

	/* Initialize internal modules variables */
	if(s_ipal_base_vars_init(cfg) != ipal_res_OK)
	{
		ipal_base_hid_on_fatalerror(ipal_error_incorrectparameter, "ipal_base_initialise(): tcpnet resources are not enough for ipal");
		return(ipal_res_NOK_generic);
	}
    
  /* Gives memory to internal modules */
	if(s_ipal_base_global_setmem(cfg, data04aligned) != ipal_res_OK)
	{
		return(ipal_res_NOK_generic);
	}

  /* Copy locally the configuration */
	memcpy(&ipal_base_hid_cfg, cfg, sizeof(ipal_base_hid_cfg));
    
  /* Calls modules initialization */
	if(ipal_res_OK != s_ipal_base_global_init(&ipal_base_hid_cfg))
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
uint32_t ipal_base_hid_getsize(const ipal_cfg_t *cfg)
{
	return(0);
}

/*
 * ipal_base_hid_setmem
 * See header file for details
 */
ipal_result_t ipal_base_hid_setmem(const ipal_cfg_t *cfg, uint32_t *memory)
{
	return(ipal_res_OK); 
}


/*
 * ipal_base_hid_init
 * See header file for details
 */
ipal_result_t ipal_base_hid_init(ipal_cfg_t *cfg)
{
  
	/* Initialize locking system */
	
	if(NULL == cfg->extfn.osal_mutex_new)
	{
		// put others to NULL as well
		cfg->extfn.osal_mutex_take          = NULL;
		cfg->extfn.osal_mutex_release       = NULL;
		cfg->extfn.osal_param_tout_forever  = 0;
		ipal_base_hid_mutex                 = NULL;
	}
	else if((NULL == cfg->extfn.osal_mutex_take) || (NULL == cfg->extfn.osal_mutex_release) || (0 == cfg->extfn.osal_param_tout_forever))
	{
		ipal_base_hid_on_fatalerror(ipal_error_incorrectparameter, "ipal_base_initialise() needs osal functions");
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
ipal_result_t ipal_base_hid_vars_init(const ipal_cfg_t *cfg)
{
	// we reset the internally allocated memory and we init internal variables
	s_ipal_base_internal_ram_reset();
	s_ipal_base_internal_ram_init();
	
	return(ipal_res_OK);
}

/*
 * ipal_base_hid_on_fatalerror
 * See header file for details
 */
void ipal_base_hid_on_fatalerror(ipal_fatalerror_t errorcode, const char * errormsg)
{
	if(NULL != ipal_base_hid_cfg.extfn.usr_on_fatal_error)
	{
		ipal_base_hid_cfg.extfn.usr_on_fatal_error(errorcode, errormsg);
	}
	else
	{
		for(;;)
		{
			errorcode = errorcode;
		}
	}
}

/*
 * ipal_base_hid_threadsafety_init
 * See header file for details
 */
void ipal_base_hid_threadsafety_init(void)
{
	if(NULL != ipal_base_hid_cfg.extfn.osal_mutex_new)
	{
		ipal_base_hid_mutex = ipal_base_hid_cfg.extfn.osal_mutex_new();
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
		ipal_base_hid_cfg.extfn.osal_mutex_take(ipal_base_hid_mutex, ipal_base_hid_cfg.extfn.osal_param_tout_forever);
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
		ipal_base_hid_cfg.extfn.osal_mutex_release(ipal_base_hid_mutex);
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

/*
 * s_ipal_base_vars_init
 * See declaration for details
 */
static ipal_result_t s_ipal_base_vars_init(const ipal_cfg_t *cfg)
{
#ifdef IPAL_USE_ARP
	if(ipal_res_OK != ipal_arp_hid_vars_init(cfg))
	{
		return(ipal_res_NOK_generic);
	}
#endif//IPAL_USE_ARP
    
#ifdef IPAL_USE_BASE
	if(ipal_res_OK != ipal_base_hid_vars_init(cfg))
	{
		return(ipal_res_NOK_generic);
	}
#endif//IPAL_USE_BASE

#ifdef IPAL_USE_DHCP
	if(ipal_res_OK != ipal_dhcp_hid_vars_init(cfg))
	{
		return(ipal_res_NOK_generic);
	}
#endif//IPAL_USE_DHCP

#ifdef IPAL_USE_ETH
	if(ipal_res_OK != ipal_eth_hid_vars_init(cfg))
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
	if(ipal_res_OK != ipal_igmp_hid_vars_init(cfg))
	{
		return(ipal_res_NOK_generic);
	}
#endif//IPAL_USE_IGMP

#ifdef IPAL_USE_SYS
	if(ipal_res_OK != ipal_sys_hid_vars_init(cfg))
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
	if(ipal_res_OK != ipal_udp_hid_vars_init(cfg))
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
static uint32_t s_ipal_base_global_getsize(const ipal_cfg_t *cfg)
{
	uint32_t retval = 0;

#ifdef IPAL_USE_ARP
	retval += ipal_arp_hid_getsize(cfg);
#endif//IPAL_USE_ARP

#ifdef IPAL_USE_BASE
	retval += ipal_base_hid_getsize(cfg);
#endif//IPAL_USE_BASE

#ifdef IPAL_USE_DHCP
    retval += ipal_dhcp_hid_getsize(cfg);
#endif//IPAL_USE_DHCP

#ifdef IPAL_USE_ETH
    retval += ipal_eth_hid_getsize(cfg);
#endif//IPAL_USE_ETH

#ifdef IPAL_USE_FTP
    retval += ipal_ftp_hid_getsize(cfg);
#endif//IPAL_USE_FTP

#ifdef IPAL_USE_IGMP
    retval += ipal_igmp_hid_getsize(cfg);
#endif//IPAL_USE_IGMP

#ifdef IPAL_USE_SYS
    retval += ipal_sys_hid_getsize(cfg);
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
    retval += ipal_udp_hid_getsize(cfg);
#endif//IPAL_USE_UDP

    return(retval);
}

/*
 * s_ipal_base_global_setmem
 * See declaration for details
 */
static ipal_result_t s_ipal_base_global_setmem(const ipal_cfg_t *cfg, uint32_t *data04aligned)
{
#ifdef IPAL_USE_ARP
	if(ipal_res_OK != ipal_arp_hid_setmem(cfg, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_arp_hid_getsize(cfg)/4;
#endif//IPAL_USE_ARP

#ifdef IPAL_USE_BASE
	if(ipal_res_OK != ipal_base_hid_setmem(cfg, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_base_hid_getsize(cfg)/4;
#endif//IPAL_USE_BASE

#ifdef IPAL_USE_DHCP
	if(ipal_res_OK != ipal_dhcp_hid_setmem(cfg, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_dhcp_hid_getsize(cfg)/4;
#endif//IPAL_USE_DHCP

#ifdef IPAL_USE_ETH
	if(ipal_res_OK != ipal_eth_hid_setmem(cfg, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_eth_hid_getsize(cfg)/4;
#endif//IPAL_USE_ETH

#ifdef IPAL_USE_FTP
	if(ipal_res_OK != ipal_ftp_hid_setmem(cfg, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_ftp_hid_getsize(cfg)/4;
#endif//IPAL_USE_FTP

#ifdef IPAL_USE_IGMP
	if(ipal_res_OK != ipal_igmp_hid_setmem(cfg, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_igmp_hid_getsize(cfg)/4;
#endif//IPAL_USE_IGMP

#ifdef IPAL_USE_SYS
	if(ipal_res_OK != ipal_sys_hid_setmem(cfg, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_sys_hid_getsize(cfg)/4;
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
	if(ipal_res_OK != ipal_udp_hid_setmem(cfg, data04aligned))
	{
			return(ipal_res_NOK_generic);
	}
	data04aligned += ipal_udp_hid_getsize(cfg)/4;
#endif//IPAL_USE_UDP 

	return(ipal_res_OK);
}


/*
 * s_ipal_base_global_init
 * See declaration for details
 */
static ipal_result_t s_ipal_base_global_init(ipal_cfg_t *cfg)
{

#ifdef IPAL_USE_ARP
    if(ipal_res_OK != ipal_arp_hid_init(cfg))
    {
        return(ipal_res_NOK_generic);
    }
#endif//IPAL_USE_ARP

#ifdef IPAL_USE_BASE
    if(ipal_res_OK != ipal_base_hid_init(cfg))
    {
        return(ipal_res_NOK_generic);
    }
#endif//IPAL_USE_BASE

#ifdef IPAL_USE_DHCP
    if(ipal_res_OK != ipal_dhcp_hid_init(cfg))
    {
        return(ipal_res_NOK_generic);
    }
#endif//IPAL_USE_DHCP

#ifdef IPAL_USE_ETH
    if(ipal_res_OK != ipal_eth_hid_init(cfg))
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
    if(ipal_res_OK != ipal_igmp_hid_init(cfg))
    {
        return(ipal_res_NOK_generic);
    }
#endif//IPAL_USE_IGMP

#ifdef IPAL_USE_SYS
    if(ipal_res_OK != ipal_sys_hid_init(cfg))
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
    if(ipal_res_OK != ipal_udp_hid_init(cfg))
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
static void s_ipal_base_internal_ram_reset(void)
{
	memset(&ipal_base_hid_cfg, 0, sizeof(ipal_base_hid_cfg));
	ipal_base_hid_mutex = NULL;
}

/*
 * s_ipal_base_internal_ram_init
 * See declaration for details
 */
static void s_ipal_base_internal_ram_init(void)
{
	/* Nothing todo */
}

/*
 * s_ipal_base_requiredservicescheck
 * See declaration for details
 */
static ipal_boolval_t s_ipal_base_requiredservicescheck(const ipal_cfg_t *cfg)
{
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
}

#endif

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




