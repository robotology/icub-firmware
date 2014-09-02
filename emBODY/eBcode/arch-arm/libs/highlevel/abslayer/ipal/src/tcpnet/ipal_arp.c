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

/* @file       ipal_arp.c
	@brief      This file implements internal implementation of the ipal arpmodule
	@author     marco.accame@iit.it
    @date       12/12/2011
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
#include "time.h"

#include "ipiit.h"

#include "ipal_base_hid.h"
#include "ipal_sys_hid.h"

// - storage for ipal
#include "ipal_ipiit_storage.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "ipal.h"

extern uint8_t      own_hw_adr[6];


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

typedef struct
{
    uint8_t     macdestination[6];
    uint8_t     macsource[6];
    uint16_t    type;           // arp is 0x0806 network order or 0x0608 in little endian
    uint16_t    hwtype;         // eth is 0x0001 network order or 0x0100 in little endian
    uint16_t    protocol;       // ip is 0x0800 network order or 0x0008 in little endian
    uint8_t     hwsize;         // 6
    uint8_t     ipsize;         // 4
    uint16_t    opc;            // rqst 0x0001, reply 0x0002 network order or 0x0100 and 0x0200 in little endian
    uint8_t     macsender[6];
    uint8_t     ipsender[4];   
    uint8_t     mactarget[6];
    uint8_t     iptarget[4];   
    uint8_t     padding[22];      
} ipal_arp_pkt_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_ipal_arp_internal_ram_reset(void);
static void s_ipal_arp_internal_ram_init(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static ipal_arp_pkt_t s_ipal_arp_query = {0};
static struct os_frame *s_ipal_arp_frame = NULL;

static uint8_t s_ipal_arp_framebuffer[sizeof(ipal_arp_pkt_t)+4] = {0};




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern ipal_result_t ipal_arp_request(ipal_ipv4addr_t ip, ipal_arp_cachemode_t cm)
{   // legacy function
    return(ipal_arp_resolve(ip, cm, 0));
}

extern ipal_result_t ipal_arp_resolve(ipal_ipv4addr_t ip, ipal_arp_cachemode_t cm, uint8_t forcearpframe)
{
    uint8_t *iparray = (uint8_t*)(&ip);
    uint8_t aa = (ipal_arp_cache_permanently == cm) ? (ARP_FIXED_IP)  : (ARP_TEMP_IP);
    uint8_t r = __FALSE;

    if(0 == ipal_sys_hid_started)
    {
        return(ipal_res_NOK_generic);
    }

    ipal_base_hid_threadsafety_lock();
    
    r = arp_cache_ip(iparray, aa);

    if(1 == forcearpframe)
    {
        memcpy(&s_ipal_arp_query.iptarget, iparray, sizeof(s_ipal_arp_query.iptarget));
        memcpy(s_ipal_arp_frame->data, &s_ipal_arp_query, sizeof(s_ipal_arp_query)); 
        send_frame(s_ipal_arp_frame);
    }

    ipal_base_hid_threadsafety_unlock();

    return( (__TRUE == r) ? (ipal_res_OK) : (ipal_res_NOK_generic) );
}

extern ARP_INFO     arp_table[];

//IPAL_NUMBEROF_ARPCACHETABLEMAXSIZE
extern uint16_t     arp_TabSize;

#include "string.h"

extern ipal_result_t ipal_arp_getmac(ipal_ipv4addr_t ip, uint8_t* mac)
{
    uint8_t *iparray = (uint8_t*)(&ip);
    uint8_t r = 0;
    uint8_t* rr = NULL;
    uint8_t i;
    ipal_result_t res = ipal_res_NOK_generic;

    if(0 == ipal_sys_hid_started)
    {
        return(ipal_res_NOK_generic);
    }
    
    if(0 == arp_TabSize)
    {
        return(ipal_res_NOK_generic);
    }
    
    
    ipal_base_hid_threadsafety_lock();    
    
    for(i=0; i<arp_TabSize; i++)
    {
        if(memcmp(arp_table[i].IpAdr, iparray, IP_ADRLEN) == 0)
        {
            r = 1;
            rr = (uint8_t*)&arp_table[i].HwAdr;
            break;
        }
    }

    ipal_base_hid_threadsafety_unlock();
    
    if(1 == r)
    {
        res = ipal_res_OK;
        if(NULL != mac)
        {
            memcpy(mac, rr, ETH_ADRLEN);
        }
    }

    return(res);
}

extern ipal_result_t ipal_arp_isresolved(ipal_ipv4addr_t ip)
{
    uint8_t *iparray = (uint8_t*)(&ip);
    uint8_t r = 0;
    uint8_t* rr = NULL;
    uint8_t i;
    ipal_result_t res = ipal_res_NOK_generic;
    static const uint8_t sixzeroes[6] = {0, 0, 0, 0, 0, 0};
    static const uint8_t sixfs[6] = {255, 255, 255, 255, 255, 255};

    if(0 == ipal_sys_hid_started)
    {
        return(ipal_res_NOK_generic);
    }
    
    if(0 == arp_TabSize)
    {
        return(ipal_res_NOK_generic);
    }
    
    
    ipal_base_hid_threadsafety_lock();    
    
    for(i=0; i<arp_TabSize; i++)
    {
        if(memcmp(arp_table[i].IpAdr, iparray, IP_ADRLEN) == 0)
        {
            r = 1;
            rr = (uint8_t*)&arp_table[i].HwAdr;
            break;
        }
    }

    ipal_base_hid_threadsafety_unlock();
    
    if(1 == r)
    {
        if((memcmp(rr, &sixzeroes, 6) == 0) || (memcmp(rr, &sixfs, 6) == 0))
        {
            res = ipal_res_NOK_generic;
        }
        else
        {
            res = ipal_res_OK;
        }
        
    }

    return(res);
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

    memset(&s_ipal_arp_query.macdestination[0], 0xff, sizeof(s_ipal_arp_query.macdestination));
    memcpy(&s_ipal_arp_query.macsource[0], &ipal_base_hid_cfg.eth_mac, sizeof(s_ipal_arp_query.macsource));
    s_ipal_arp_query.type       = 0x0608;
    s_ipal_arp_query.hwtype     = 0x0100;
    s_ipal_arp_query.protocol   = 0x0008;
    s_ipal_arp_query.hwsize     = 6;
    s_ipal_arp_query.ipsize     = 4;
    s_ipal_arp_query.opc        = 0x0100;
    memcpy(&s_ipal_arp_query.macsender[0], &ipal_base_hid_cfg.eth_mac, sizeof(s_ipal_arp_query.macsender));
    memcpy(&s_ipal_arp_query.ipsender[0], &ipal_base_hid_cfg.eth_ip, sizeof(s_ipal_arp_query.ipsender)); 
    memset(&s_ipal_arp_query.mactarget[0], 0x00, sizeof(s_ipal_arp_query.mactarget));
    memset(&s_ipal_arp_query.iptarget[0], 0x00, sizeof(s_ipal_arp_query.iptarget));  
    memset(&s_ipal_arp_query.padding[0], 0x00, sizeof(s_ipal_arp_query.padding));  
      
    
    s_ipal_arp_frame            = (struct os_frame*)&s_ipal_arp_framebuffer;
    s_ipal_arp_frame->length    = sizeof(ipal_arp_pkt_t)-sizeof(s_ipal_arp_query.padding);
    s_ipal_arp_frame->index     = 0;

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




