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

/* @file       ipal_udp.c
	@brief      This file implements internal implementation of the ipal udp module
	@author     marco.accame@iit.it
    @date       12/12/2011
**/

// - modules to be built: contains the IPAL_USE_* macros --------------------------------------------------------------
#include "ipal_modules.h"

// - sizes of modules: contains the IPAL_NUMBEROF_* macros ----------------------------------------------------------------
#include "ipal_numberof.h"

#ifdef IPAL_USE_UDP

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


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "ipal_udp_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

struct ipal_udpsocket_opaque_t 
{
    uint8_t             id;
    uint16_t            locport;
    uint32_t            locaddr;
    uint16_t            remport;
    uint32_t            remaddr;
    ipal_udp_recv_fn    recv;
    void*               arg;
};


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_ipal_udp_internal_ram_reset(void);
static void s_ipal_udp_internal_ram_init(void);

static uint16_t s_ipal_udp_ipiit_listener(uint8_t id, uint8_t* ip, uint16_t po, uint8_t *da, uint16_t le);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static ipal_udpsocket_t s_ipal_udp_udpsockets[IPAL_NUMBEROF_UDPMAXSOCKETS] = {0};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern ipal_udpsocket_t* ipal_udpsocket_new(ipal_tos_t tos)
{
    uint8_t n = 0;
    U8 t = ((tos.precedence << 5) | (tos.lowdelay << 4) | (tos.highthroughput << 3) | (tos.highreliability << 2));

//    #define IPAL_UDP_TOS    0xac
    #define IPAL_UDP_OPT    UDP_OPT_SEND_CS

    if(0 == ipal_sys_hid_started)
    {
        return(NULL);
    }
    
    ipal_base_hid_threadsafety_lock();

    n = udp_get_socket(t, IPAL_UDP_OPT, s_ipal_udp_ipiit_listener);

    if(0 == n)
    {
        ipal_base_hid_threadsafety_unlock();
        return(NULL);
    }

    s_ipal_udp_udpsockets[n-1].id        = n;
    s_ipal_udp_udpsockets[n-1].locport   = 0;
    s_ipal_udp_udpsockets[n-1].locaddr   = 0;
    s_ipal_udp_udpsockets[n-1].remport   = 0;
    s_ipal_udp_udpsockets[n-1].remaddr   = 0;
    s_ipal_udp_udpsockets[n-1].recv      = NULL;
    s_ipal_udp_udpsockets[n-1].arg       = NULL;

    ipal_base_hid_threadsafety_unlock();

    return(&s_ipal_udp_udpsockets[n-1]);
}


extern ipal_result_t ipal_udpsocket_delete(ipal_udpsocket_t* skt)
{
    BOOL r;

    if((NULL == skt) || (0 == skt->id))
    {
        return(ipal_res_NOK_generic);
    }


    ipal_base_hid_threadsafety_lock();

    if(0 != skt->locport)
    {   // need to close the socket first
        udp_close(skt->id);
        skt->locport = 0;   
    }

    r = udp_release_socket(skt->id);

    s_ipal_udp_udpsockets[skt->id-1].id  = 0;

    skt->id         = 0;
    skt->locport    = 0;
    skt->locaddr    = 0;
    skt->remport  = 0;
    skt->remaddr  = 0;
    skt->recv       = NULL;
    skt->arg        = NULL;

    ipal_base_hid_threadsafety_unlock();
    
    return((__FALSE == r) ? (ipal_res_NOK_generic) : (ipal_res_OK));
} 


extern ipal_result_t ipal_udpsocket_bind(ipal_udpsocket_t* skt, ipal_ipv4addr_t locaddr, ipal_port_t locport)
{
    uint8_t i;

    if((NULL == skt) || (0 == skt->id) || (0 == locport))
    {
        return(ipal_res_NOK_generic);
    }

    ipal_base_hid_threadsafety_lock();

    // if the socket is already bound
    if(0 != skt->locport)
    {   // need to close the socket first
        udp_close(skt->id);
        skt->locport = 0;   
    }

    // now check that no upd socket has already occupied the same locport
    for(i=0; i<IPAL_NUMBEROF_UDPMAXSOCKETS; i++)
    {
        if(locport == s_ipal_udp_udpsockets[i].locport)
        {
            ipal_base_hid_threadsafety_unlock();
            return(ipal_res_NOK_generic);
        }
    }


    // ok, locport is available: calls tcpnet

    i = udp_open(skt->id, locport);

    if(__FALSE == i)
    {
        ipal_base_hid_threadsafety_unlock();
        return(ipal_res_NOK_generic);
    }

    // also tcpnet thinks it is available: i assign locport and locaddr
    skt->locport = locport;
    skt->locaddr = locaddr;

    ipal_base_hid_threadsafety_unlock();
    
    return(ipal_res_OK);
} 


extern ipal_result_t ipal_udpsocket_connect(ipal_udpsocket_t* skt, ipal_ipv4addr_t remaddr, ipal_port_t remport)
{
    if((NULL == skt) || (0 == skt->id))
    {   // fails also if no bind has been done before.
        return(ipal_res_NOK_generic);
    }

    ipal_base_hid_threadsafety_lock();

    // if not bound yet ... do binding
    if(0 == skt->locport)
    {
        if(ipal_res_NOK_generic == ipal_udpsocket_bind(skt, IPAL_ipv4addr_INADDR_ANY, remport))
        {
            ipal_base_hid_threadsafety_unlock();
            return(ipal_res_NOK_generic);
        }
    }

    
    skt->remport = remport;
    skt->remaddr = remaddr;

    ipal_base_hid_threadsafety_unlock();
    
    return(ipal_res_OK);
} 


extern ipal_result_t ipal_udpsocket_disconnect(ipal_udpsocket_t* skt)
{
    if((NULL == skt) || (0 == skt->id))
    {   
        return(ipal_res_NOK_generic);
    }

    ipal_base_hid_threadsafety_lock();
    
    skt->remport = 0;
    skt->remaddr = 0;

    ipal_base_hid_threadsafety_unlock();
        
    return(ipal_res_OK);
} 


extern ipal_result_t ipal_udpsocket_send(ipal_udpsocket_t* skt, ipal_packet_t *pckt)
{

    ipal_result_t res;

    if((NULL == skt) || (0 == skt->locport) || (0 == skt->remaddr) || (0 == skt->remport))
    {
        return(ipal_res_NOK_generic);
    }

    // ok, it seems bound and connected ... i can attempt to send it w/ ipal_udpsocket_sendto()

    ipal_base_hid_threadsafety_lock();
    
    res = ipal_udpsocket_sendto(skt, pckt, skt->remaddr, skt->remport);

    ipal_base_hid_threadsafety_unlock();

    return(res);

} 

extern ipal_result_t ipal_udpsocket_sendto(ipal_udpsocket_t* skt, ipal_packet_t *pckt, ipal_ipv4addr_t remaddr, ipal_port_t remport)
{
    uint8_t *internalbuffer = NULL;
    uint8_t *toiparray = (uint8_t*)&remaddr;
    ipal_result_t res;

    if((NULL == skt) || (0 == skt->id) || (0 == skt->locport) || (NULL == pckt) || (NULL == pckt->data) || (0 == pckt->size))
    {   // fails also if no bind has been done before.
        return(ipal_res_NOK_generic);
    }

    ipal_base_hid_threadsafety_lock();

    // get internal buffer    
    internalbuffer = udp_get_buf(pckt->size);
    
    if(NULL == internalbuffer)
    {
        ipal_base_hid_threadsafety_unlock();
        return(ipal_res_NOK_generic);
    }
    
    memcpy(internalbuffer, pckt->data, pckt->size);
    
    res = (__FALSE == udp_send(skt->id, toiparray, remport, internalbuffer, pckt->size)) ? (ipal_res_NOK_generic) : (ipal_res_OK); 

    ipal_base_hid_threadsafety_unlock();

    return(res);
} 


extern ipal_result_t ipal_udpsocket_recv(ipal_udpsocket_t* skt, ipal_udp_recv_fn recv, void *arg)
{
    if((NULL == skt) || (NULL == recv))
    {   // argv can be NULL.
        return(ipal_res_NOK_generic);
    }

    ipal_base_hid_threadsafety_lock();

    skt->recv = recv;
    skt->arg  = arg;

    ipal_base_hid_threadsafety_unlock();
        
    return(ipal_res_OK); 
} 


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern uint32_t ipal_udp_hid_getsize(const ipal_cfg_t *cfg)
{
    // no memory needed
    return(0);
}

extern ipal_result_t ipal_udp_hid_setmem(const ipal_cfg_t *cfg, uint32_t *memory)
{
    // no memory needed
    return(ipal_res_OK); 
}

extern ipal_result_t ipal_udp_hid_init(ipal_cfg_t *cfg)
{
    // nothing is required
    return(ipal_res_OK); 
}


extern ipal_result_t ipal_udp_hid_vars_init(const ipal_cfg_t *cfg)
{
    if( (IPAL_NUMBEROF_UDPMAXSOCKETS < cfg->udp_socketnum) )
    {
        return(ipal_res_NOK_generic);
    }
 
    // we reset the internally allocated memory and we init internal variables
    s_ipal_udp_internal_ram_reset();
    s_ipal_udp_internal_ram_init();
    
    // we initialise ipiit storage
    ipal_ipiit_storage_udp_init(cfg);

 
    return(ipal_res_OK);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


//typedef void (*ipal_udp_recv_fn)(void *arg, ipal_udpsocket_t *skt, ipal_packet_t *pkt, ipal_ipv4addr_t adr, ipal_port_t por);
static uint16_t s_ipal_udp_ipiit_listener(uint8_t id, uint8_t* ip, uint16_t po, uint8_t *da, uint16_t le)
{   // must be a reentrant function
    ipal_udpsocket_t *skt;
    ipal_packet_t pkt;
    ipal_ipv4addr_t ipaddr;

    // look for the udp-socket

    if((0==id) ||(id>IPAL_NUMBEROF_UDPMAXSOCKETS))
    {
        return(0);
    }

    skt = &s_ipal_udp_udpsockets[id-1];


    if(NULL != skt->recv)
    {
        // filter vs incoming ip and port (but only if we are in connected state ...)
        ipaddr = ipal_base_hid_addr_array_to_ipv4(ip);
        if((0 == skt->remaddr) || ((ipaddr == skt->remaddr) && (po == skt->remport)))
        {
            // ok ... call the used-defined function
            pkt.data = da;
            pkt.size = le;
            skt->recv(skt->arg, skt, &pkt, ipaddr, po);
        }
    }

    return(0);
}


static void s_ipal_udp_internal_ram_reset(void)
{
    memset(&s_ipal_udp_udpsockets[0], 0, sizeof(s_ipal_udp_udpsockets));
}

static void s_ipal_udp_internal_ram_init(void)
{
    // nothing to do
}

#endif//IPAL_USE_UDP

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




