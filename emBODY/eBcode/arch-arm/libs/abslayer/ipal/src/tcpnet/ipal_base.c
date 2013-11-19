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

/* @file       ipal_base.c
	@brief      This file implements internal implementation of the ipal base module
	@author     marco.accame@iit.it
    @date       12/12/2011
**/

// - modules to be built: contains the IPAL_USE_* macros --------------------------------------------------------------
#include "ipal_modules.h"

// - sizes of modules: contains the IPAL_NUMBEROF_* macros ----------------------------------------------------------------
#include "ipal_numberof.h"

#ifdef IPAL_USE_BASE


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "time.h"

#include "ipiit.h"

#include "ipal_arp_hid.h"
//#include "ipal_base_hid.h"
#include "ipal_dhcp_hid.h"
#include "ipal_eth_hid.h"
#include "ipal_ftp_hid.h"
#include "ipal_igmp_hid.h"
#include "ipal_sys_hid.h"
#include "ipal_tcp_hid.h"
#include "ipal_telnet_hid.h"
#include "ipal_tftp_hid.h"
#include "ipal_udp_hid.h"

// - storage for ipal
#include "ipal_ipiit_storage.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "ipal.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "ipal_base_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

ipal_cfg_t ipal_base_hid_cfg;
extern void *ipal_base_hid_mutex                                        = NULL;



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static ipal_result_t s_ipal_base_global_tcpnet_vars_init(const ipal_cfg_t *cfg);

static uint32_t s_ipal_base_global_getsize(const ipal_cfg_t *cfg);
static ipal_result_t s_ipal_base_global_setmem(const ipal_cfg_t *cfg, uint32_t *data04aligned);
static ipal_result_t s_ipal_base_global_init(ipal_cfg_t *cfg);

static void s_ipal_base_internal_ram_reset(void);
static void s_ipal_base_internal_ram_init(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern uint32_t ipal_base_memory_getsize(const ipal_cfg_t *cfg, uint32_t *size04aligned)
{
    uint32_t retval = 0;
 
    if(NULL == cfg)
    {
        ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_base_memory_getsize() needs a cfg");
        return(0);
    }

    retval = s_ipal_base_global_getsize(cfg);

    if(NULL != size04aligned)
    {
        *size04aligned = retval;
    }
    
    return(retval);
}
                                    

extern ipal_result_t ipal_base_initialise(const ipal_cfg_t *cfg, uint32_t *data04aligned)
{

    if(NULL == cfg)
    {
        ipal_base_hid_on_fatalerror(ipal_error_missingconfiguration, "ipal_base_initialise(): missing cfg");
        return(ipal_res_NOK_generic);
    }

    // set the usr_on_fatal_error asap ... i do it even if cfg->extfn.usr_on_fatal_error is NULL.

    ipal_base_hid_cfg.extfn.usr_on_fatal_error = cfg->extfn.usr_on_fatal_error;

        
    if(ipal_ipstack_iitmod_tcpnetarm != cfg->arch_ipstack)
    {
        ipal_base_hid_on_fatalerror(ipal_error_unsupportedbehaviour, "ipal_base_initialise(): unsupported tcp/ip stack");
        return(ipal_res_NOK_generic);
    }

    if(1 == ipal_sys_hid_started)
    {
        ipal_base_hid_on_fatalerror(ipal_error_generic, "ipal_base_initialise(): already started");
        return(ipal_res_NOK_generic);
    }
    
    if(ipal_res_OK != s_ipal_base_global_tcpnet_vars_init(cfg))
    {
        ipal_base_hid_on_fatalerror(ipal_error_incorrectparameter, "ipal_base_initialise(): tcpnet resources are not enough for ipal");
        return(ipal_res_NOK_generic);
    }
    
    // gives memory to the various modules
    if(ipal_res_OK != s_ipal_base_global_setmem(cfg, data04aligned))
    {
        return(ipal_res_NOK_generic);
    }

 
    // ... sets used config: ipal_base_hid_cfg
    memcpy(&ipal_base_hid_cfg, cfg, sizeof(ipal_base_hid_cfg));
    
    
    // and calls the initialisations of the modules using ipal_base_hid_cfg
    if(ipal_res_OK != s_ipal_base_global_init(&ipal_base_hid_cfg))
    {
        return(ipal_res_NOK_generic);
    }    
    
    return(ipal_res_OK);

}


extern uint32_t ipal_memory_getsize(const ipal_cfg_t *cfg, uint32_t *size04aligned)
{
    return(ipal_base_memory_getsize(cfg, size04aligned));
}


extern ipal_result_t ipal_initialise(const ipal_cfg_t *cfg, uint32_t *data04aligned)
{
    return(ipal_base_initialise(cfg, data04aligned));
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


extern uint32_t ipal_base_hid_getsize(const ipal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern ipal_result_t ipal_base_hid_setmem(const ipal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
    return(ipal_res_OK); 
}


extern ipal_result_t ipal_base_hid_init(ipal_cfg_t *cfg)
{
    // some things are required
    
     // thread safety
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


extern ipal_result_t ipal_base_hid_vars_init(const ipal_cfg_t *cfg)
{

    // we reset the internally allocated memory and we init internal variables
    s_ipal_base_internal_ram_reset();
    s_ipal_base_internal_ram_init();
    
    // but we dont initialise ipiit storage
    // because there is no ipiit module as base.
    
    return(ipal_res_OK);
}



extern void ipal_base_hid_on_fatalerror(ipal_fatalerror_t errorcode, const char * errormsg)
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


extern void ipal_base_hid_threadsafety_init(void)
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

extern void ipal_base_hid_threadsafety_lock(void)
{
    if(NULL != ipal_base_hid_mutex)
    {
        ipal_base_hid_cfg.extfn.osal_mutex_take(ipal_base_hid_mutex, ipal_base_hid_cfg.extfn.osal_param_tout_forever);
    }
}

extern void ipal_base_hid_threadsafety_unlock(void)
{
    if(NULL != ipal_base_hid_mutex)
    {
        ipal_base_hid_cfg.extfn.osal_mutex_release(ipal_base_hid_mutex);
    }
}


extern ipal_ipv4addr_t ipal_base_hid_addr_array_to_ipv4(ipal_arrayaddr_t iparray)
{
    return(*((uint32_t*)iparray));
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static ipal_result_t s_ipal_base_global_tcpnet_vars_init(const ipal_cfg_t *cfg)
{
    // in here we just calls the init of various modules.

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


static void s_ipal_base_internal_ram_reset(void)
{
    memset(&ipal_base_hid_cfg, 0, sizeof(ipal_base_hid_cfg));
    ipal_base_hid_mutex = NULL;
}

static void s_ipal_base_internal_ram_init(void)
{
    // nothing to do
}


#endif//IPAL_USE_BASE

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




