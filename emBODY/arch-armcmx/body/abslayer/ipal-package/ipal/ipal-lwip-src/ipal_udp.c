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

/* @file       ipal_udp.c
	@brief      This file implements internal implementation of the ipal udp module
	@author     valentina.gaggero@iit.it
    @date       01/07/2013
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

//6#include "hal_trace.h"
//#include "ipiit.h"
#include "lwip_interface.h"

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
#define SOCKET_ID_NULL      255

#if (IPAL_NUMBEROF_UDPMAXSOCKETS == 255)
#error IPAL_NUMBEROF_UDPMAXSOCKETS max 254
#endif



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
    struct udp_pcb      *pcb_ptr;
};

struct  s_ipal_arrayofudpsocket
{
    uint8_t capacity;
    uint8_t size;
    struct ipal_udpsocket_opaque_t sockets[IPAL_NUMBEROF_UDPMAXSOCKETS];
};
// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_ipal_udp_internal_ram_reset(void);
static void s_ipal_udp_internal_ram_init(void);

static uint16_t s_ipal_udp_ipiit_listener(uint8_t id, uint8_t* ip, uint16_t po, uint8_t *da, uint16_t le);


//nuove
static ipal_result_t s_ipal_copydata2internalbuf(ipal_packet_t *pkt, struct pbuf *internalbuffer);
static ipal_result_t s_ipal_copydatafrominternalbuf(struct pbuf *internalbuffer, ipal_packet_t *pkt) ;
static uint8_t s_ipal_udpsocket_getfirstfreesocketid(void);
static void s_ipal_udpsocket_lwiprecvfn(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static ipal_udpsocket_t s_ipal_udp_udpsockets[IPAL_NUMBEROF_UDPMAXSOCKETS] = {0};
#define IPAL_UDP_PAYLOAD_SIZE 1500
#warning PAY ATTENTION: udp payload size max = 1500 dynamic alloc
#warning VALE -->remove receive_buff and add api 4 copy data!!!!!
//static uint8_t receive_buff[IPAL_UDP_PAYLOAD_SIZE];
static uint8_t *receive_buff;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern ipal_udpsocket_t* ipal_udpsocket_new(ipal_tos_t tos)
{

    struct udp_pcb *pcb = NULL;
    uint8_t id;
    
    uint8_t t = ((tos.precedence << 5) | (tos.lowdelay << 4) | (tos.highthroughput << 3) | (tos.highreliability << 2));
   
    
// //    #define IPAL_UDP_TOS    0xac
//     #define IPAL_UDP_OPT    UDP_OPT_SEND_CS

    if(0 == ipal_sys_hid_started)
    {
        return(NULL);
    }
    
    ipal_base_hid_threadsafety_lock();

    id = s_ipal_udpsocket_getfirstfreesocketid();

    if(SOCKET_ID_NULL == id)
    {
        ipal_base_hid_threadsafety_unlock();
        return(NULL);
    }
    
    pcb = udp_new();
    if(NULL == pcb)
    {
        ipal_base_hid_threadsafety_unlock();
        return(NULL);
    }
    
    pcb->tos = t;
    
    s_ipal_udp_udpsockets[id].id        = id;
    s_ipal_udp_udpsockets[id].locport   = 0;
    s_ipal_udp_udpsockets[id].locaddr   = 0;
    s_ipal_udp_udpsockets[id].remport   = 0;
    s_ipal_udp_udpsockets[id].remaddr   = 0;
    s_ipal_udp_udpsockets[id].recv      = NULL;
    s_ipal_udp_udpsockets[id].arg       = NULL;
    s_ipal_udp_udpsockets[id].pcb_ptr   = pcb;
    
    
    if(NULL == receive_buff)
    {
        receive_buff= (uint8_t*)malloc(IPAL_UDP_PAYLOAD_SIZE);
    }
    ipal_base_hid_threadsafety_unlock();
    
   
    return(&s_ipal_udp_udpsockets[id]);
}


extern ipal_result_t ipal_udpsocket_delete(ipal_udpsocket_t* skt)
{

    if((NULL == skt) || (SOCKET_ID_NULL == skt->id))
    {
        return(ipal_res_NOK_generic);
    }


    ipal_base_hid_threadsafety_lock();

    udp_remove(skt->pcb_ptr);
    

    skt->id         = SOCKET_ID_NULL;
    skt->locport    = 0;
    skt->locaddr    = 0;
    skt->remport    = 0;
    skt->remaddr    = 0;
    skt->recv       = NULL;
    skt->arg        = NULL;
    skt->pcb_ptr    = NULL;

    ipal_base_hid_threadsafety_unlock();
    
    return(ipal_res_OK);
} 


extern ipal_result_t ipal_udpsocket_bind(ipal_udpsocket_t* skt, ipal_ipv4addr_t locaddr, ipal_port_t locport)
{
    uint8_t i;
    err_t err;
    struct ip_addr ipaddr;
    
    if((NULL == skt) || (SOCKET_ID_NULL == skt->id) || (0 == locport))
    {
        return(ipal_res_NOK_generic);
    }

    ipal_base_hid_threadsafety_lock();

    // if the socket is already bound ==> no problem
   
    
    // now check that no upd socket has already occupied the same locport
    /* NOTE: currently lwip doesn't check if couple <addr,port> is already used; this check is under macro LWIP_UDP_TODO.
             Maybe in next release it will be present  */
    for(i=0; i<IPAL_NUMBEROF_UDPMAXSOCKETS; i++)
    {
        if((locaddr == s_ipal_udp_udpsockets[i].locaddr) && (locport == s_ipal_udp_udpsockets[i].locport))
        {
            ipal_base_hid_threadsafety_unlock();
            return(ipal_res_NOK_generic);
        }
    }


    // ok, locport is available: calls lwip

    ipaddr.addr = ipal2lwip_ipv4addr(locaddr);
    
    err = udp_bind(skt->pcb_ptr, &ipaddr, locport);
    if(ERR_OK != err)
    {
        ipal_base_hid_threadsafety_unlock();
        return(ipal_res_NOK_generic);
    }
    
    // also lwip thinks it is available: i assign locport and locaddr
    skt->locport = locport;
    skt->locaddr = locaddr;

    ipal_base_hid_threadsafety_unlock();
    
    return(ipal_res_OK);
} 


extern ipal_result_t ipal_udpsocket_connect(ipal_udpsocket_t* skt, ipal_ipv4addr_t remaddr, ipal_port_t remport)
{
    err_t err;
    struct ip_addr ipaddr;
    
    if((NULL == skt) || (SOCKET_ID_NULL == skt->id))
    {   // fails also if no bind has been done before.
        return(ipal_res_NOK_generic);
    }

    ipal_base_hid_threadsafety_lock();

    // if not bound yet ... do binding
    /* NOTE: udp_connect func binds pcb to a random local port if not already bound.
       so i can't use this feature because ipal interfarce say that this function bind to a port equal to remport
       and not random port like lwip */

    if(0 == skt->locport)
    {
        if(ipal_res_NOK_generic == ipal_udpsocket_bind(skt, IPAL_ipv4addr_INADDR_ANY, remport))
        {
            ipal_base_hid_threadsafety_unlock();
            return(ipal_res_NOK_generic);
        }
    }
    ipaddr.addr = ipal2lwip_ipv4addr(remaddr);
    err = udp_connect(skt->pcb_ptr, &ipaddr, remport);
    if(ERR_OK != err)
    {
        ipal_base_hid_threadsafety_unlock();
        return(ipal_res_NOK_generic);
    }
    
    skt->remport = remport;
    skt->remaddr = remaddr;

    ipal_base_hid_threadsafety_unlock();
    
    return(ipal_res_OK);
} 


extern ipal_result_t ipal_udpsocket_disconnect(ipal_udpsocket_t* skt)
{
    if((NULL == skt) || (SOCKET_ID_NULL == skt->id))
    {   
        return(ipal_res_NOK_generic);
    }

    ipal_base_hid_threadsafety_lock();
    
    udp_disconnect(skt->pcb_ptr);
    
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
    struct pbuf *internalbuffer;
    struct ip_addr ipaddr;
    err_t err;
    ipal_result_t res;
    
    if((NULL == skt) || (SOCKET_ID_NULL == skt->id) || (0 == skt->locport) || (NULL == pckt) || (NULL == pckt->data) || (0 == pckt->size))
    {   // fails also if no bind has been done before.
        return(ipal_res_NOK_generic);
    }

    ipal_base_hid_threadsafety_lock();

    //1) get ineternal buffer
     /*ho messo raw perche' cosi' l'allocazione non mette il payload dopo aver lasciato il buco per heder del pkt.
    intanto nella funzione udp_sendto_if viene aggiunto un pbuf se header non ci sta. */
    #warning ho usato PBUF_TRANSPORT invece di RAW!!!
    internalbuffer = pbuf_alloc(PBUF_TRANSPORT, pckt->size, PBUF_POOL);
    if(NULL == internalbuffer)
    {
        ipal_base_hid_on_fatalerror(ipal_error_internal0, "pbuf_alloc returns NULL in ipal_udpsocket_sendto");
        ipal_base_hid_threadsafety_unlock();
        return(ipal_res_NOK_generic);
    }
    
    res = s_ipal_copydata2internalbuf(pckt, internalbuffer); 
    if(ipal_res_OK != res)
    {
        ipal_base_hid_on_fatalerror(ipal_error_internal0, "error in coping data from user_pkt to pbuf");
    }
    
    ipaddr.addr = ipal2lwip_ipv4addr(remaddr);
    err = udp_sendto(skt->pcb_ptr, internalbuffer, &ipaddr, remport);
    
    ipal_base_hid_threadsafety_unlock();

    return( (ERR_OK != err) ? ipal_res_NOK_generic : ipal_res_OK );

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
    udp_recv(skt->pcb_ptr, s_ipal_udpsocket_lwiprecvfn, (void*)skt);


    ipal_base_hid_threadsafety_unlock();
        
    return(ipal_res_OK); 
} 


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern uint32_t ipal_udp_hid_getsize(const ipal_cfg_t *cfg)
{
    return(0);
}

extern ipal_result_t ipal_udp_hid_setmem(const ipal_cfg_t *cfg, uint32_t *memory)
{
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
    uint8_t id;
    
    for(id=0; id<IPAL_NUMBEROF_UDPMAXSOCKETS; id++)
    {
        s_ipal_udp_udpsockets[id].id = SOCKET_ID_NULL;
    }
}


static void s_ipal_udpsocket_lwiprecvfn(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
    // must be a reentrant function
    ipal_udpsocket_t* skt = (ipal_udpsocket_t*)arg;
    ipal_packet_t pkt;
    ipal_result_t res;
    
    if((NULL != skt) && (SOCKET_ID_NULL != skt->id) && (skt->pcb_ptr == upcb))
    {
        if(NULL != skt->recv)
        {
            res = s_ipal_copydatafrominternalbuf(p, &pkt);
            if(ipal_res_OK != res)
            {
                //error during coping. the error is alredy managed by s_ipal_copydatafrominternalbuf func
                return;
            }
            skt->recv(skt->arg, skt, &pkt, lwip2ipal_ipv4addr(addr->addr), port);
        }
    }
}

static ipal_result_t s_ipal_copydata2internalbuf(ipal_packet_t *pkt, struct pbuf *internalbuffer) 
{
    err_t err;

    //Copy application supplied data into a pbuf
    err = pbuf_take(internalbuffer, (void*)pkt->data, pkt->size);
    return((ERR_OK==err) ? ipal_res_OK : ipal_res_NOK_generic);
}

static ipal_result_t s_ipal_copydatafrominternalbuf(struct pbuf *internalbuffer, ipal_packet_t *pkt) 
{
    uint16_t copiedbytes = 0;
    char str[150];
    
    //fill pkt data
    copiedbytes = pbuf_copy_partial(internalbuffer, (void*)&receive_buff[0], internalbuffer->tot_len, 0/* offset in the pbuf */);
    
    if(copiedbytes != internalbuffer->tot_len)
    {
        snprintf(str, sizeof(str), "errorn in s_ipal_copydatafrominternalbuf copiedbytes=%d, pkt_len=%d", copiedbytes, internalbuffer->tot_len);
        ipal_base_hid_on_fatalerror(ipal_error_internal0, str);
        return(ipal_res_NOK_generic);
    }
    pkt->size = internalbuffer->tot_len;
    pkt->data = &receive_buff[0];

    return(ipal_res_OK);
}


static uint8_t s_ipal_udpsocket_getfirstfreesocketid(void)
{
    uint8_t id;
    
    for(id=0; id<IPAL_NUMBEROF_UDPMAXSOCKETS; id++)
    {
       if(s_ipal_udp_udpsockets[id].id == SOCKET_ID_NULL)
       {
            return(id);
       }           
    }
    
    return(SOCKET_ID_NULL);
}
#endif//IPAL_USE_UDP

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




