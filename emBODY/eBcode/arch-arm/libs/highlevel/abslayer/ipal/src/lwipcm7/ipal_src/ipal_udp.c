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

#include "stdlib.h"
#include "string.h"
#include "time.h"

#include "lwip_interface.h"

/* Friend functions */
#include "ipal_hid.h"

/* IPAL APIs */
#include "ipal.h"

/* ===================================================================================================
 * MACROS
 * =================================================================================================== */

#define SOCKET_ID_NULL      255

#if (IPAL_NUMBEROF_UDPMAXSOCKETS == 255)
	#error IPAL_NUMBEROF_UDPMAXSOCKETS max 254
#endif

/* ===================================================================================================
 * TYPE DEFINITIONS
 * =================================================================================================== */

/**
 * Definition of opaque structure ipal_udpsocket_opaque_t
 * It is the LWIP-specific implementation of "generic" ipal_udpsocket_t UDP Socket type
 */
struct ipal_udpsocket_opaque_t 
{
	/* Socket ID */
	uint8_t             id;
	
	/* Local Address (lwip type) */
	ip4_addr_t   				locaddr;
	
	/* Loacl Port */
	uint16_t            locport;
	
	/* Remote Address (lwip type) */
	ip4_addr_t        	remaddr;
	
	/* Remote Port */
	uint16_t            remport;
	
	/* Receive callback */
	ipal_udp_recv_fn    recv;
	
	/* Arguments to be passed to the receive callback */
	void*               arg;
	
	/* Pointer to the UDP Protocol Control Block */
	struct udp_pcb      *pcb_ptr;
};

#if (0)
	struct  s_ipal_arrayofudpsocket
	{
		uint8_t capacity;
		uint8_t size;
		struct ipal_udpsocket_opaque_t sockets[IPAL_NUMBEROF_UDPMAXSOCKETS];
	};
#endif 

/* =======================================================================================================================
 * STATIC FUNCTIONS DECLARATION
 * ======================================================================================================================= */

/*
 * Gets the first free socket ID
 */
static uint8_t s_ipal_udpsocket_getfirstfreesocketid(void);

/*
 * Resets internal RAM
 */
static void s_ipal_udp_internal_ram_reset(void);

/*
 * Initializes internal RAM
 */
static void s_ipal_udp_internal_ram_init(void);

/*
 * LWIP IDP Receive callback
 */
static void s_ipal_udpsocket_lwip_recvfn(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip4_addr_t *addr, u16_t port);

/*
 * Copies (before sending) packet (IPAL) to lwip_buf
 *
 * @param pkt 							- IPAL packet. Contains the pointer to the payload and its size
 * @param internal_buffer		- Reference to internal buffer
 */
static ipal_result_t s_ipal_copydata_2_lwip_buf(ipal_packet_t *pkt, struct pbuf *lwip_buf);

/*
 * Copies (before receiving) packet from internal buffer (LWIP) to IPAL buffer
 *
 * @param pkt 							- IPAL packet. Contains the pointer to the payload and its size
 * @param internal_buffer		- Reference to internal buffer
 */
static ipal_result_t s_ipal_copydata_from_lwip_buf(struct pbuf *lwip_buf, ipal_packet_t *pkt);

/*
 * Sets the UDP PCB tos according to given IPAL TOS
 */
static void s_set_lwip_tos(struct udp_pcb *upcb, ipal_tos_t tos);

/* -----------------------------------------------------------------------------------------------------------------------
 * CALLBACKS
 * ----------------------------------------------------------------------------------------------------------------------- */
 
/*
 * Callback to be registered for packet receival
 */
static void s_ipal_udpsocket_lwip_recvfn(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip4_addr_t *addr, u16_t port);

/* =======================================================================================================================
 * STATIC VARIABLES
 * ======================================================================================================================= */

/* MARTINO UDP max payload size */
#define IPAL_UDP_PAYLOAD_SIZE 1512

/*
 * Pool of UDP Sockets
 */
static ipal_udpsocket_t s_ipal_udp_udpsockets[IPAL_NUMBEROF_UDPMAXSOCKETS] = {0};

/*
 * Receive buffer
 */
static uint8_t receive_buf[IPAL_UDP_PAYLOAD_SIZE];

/* =======================================================================================================================
 * PUBLIC FUNCTIONS (API)
 * ======================================================================================================================= */

/*
 * ipal_udpsocket_new
 * See header file for details
 */
ipal_udpsocket_t* ipal_udpsocket_new(ipal_tos_t tos)
{
	struct udp_pcb *pcb = NULL;
	uint8_t id;
	
	/* Just a check */
	if(0 == ipal_sys_hid_started)
	{
		return(NULL);
	}

	/* Resources lock */
	ipal_base_hid_threadsafety_lock();

	/* Just get an ID for the next socket -- if available */
	id = s_ipal_udpsocket_getfirstfreesocketid();
	
	/* If not free ID, return */
	if(SOCKET_ID_NULL == id)
	{
		ipal_base_hid_threadsafety_unlock();
		return(NULL);
	}
	
	/* Gets a new UDP Protocol Control Block */
	pcb = udp_new();
	
	/* Impossible to allocate a PCB. Unlock and return */
	if(NULL == pcb)
	{
		ipal_base_hid_threadsafety_unlock();
		return(NULL);
	}
	
	/* Sets type of service */
	s_set_lwip_tos(pcb, tos);
	
	/* Compile the pool of udp sockets */
	s_ipal_udp_udpsockets[id].id        		= id;
	s_ipal_udp_udpsockets[id].locaddr.addr 	= 0;
	s_ipal_udp_udpsockets[id].locport   		= 0;
	s_ipal_udp_udpsockets[id].remaddr.addr  = 0;
	s_ipal_udp_udpsockets[id].remport   		= 0;
	s_ipal_udp_udpsockets[id].recv      		= NULL;
	s_ipal_udp_udpsockets[id].arg       		= NULL;
	s_ipal_udp_udpsockets[id].pcb_ptr   		= pcb;
	
	/* Unlock */
	ipal_base_hid_threadsafety_unlock();
	
	/* Returns the pointer to the UDP Socket */
	return(&s_ipal_udp_udpsockets[id]);
}

/*
 * ipal_udpsocket_delete
 * See header file for details
 */
ipal_result_t ipal_udpsocket_delete(ipal_udpsocket_t* skt)
{
	/* Checks wether we are trying to delete an unexisting socket */
	if((NULL == skt) || (SOCKET_ID_NULL == skt->id))
	{
		return(ipal_res_NOK_generic);
	}

	/* Generic lock */
	ipal_base_hid_threadsafety_lock();

	/* Remove the PCB */
	udp_remove(skt->pcb_ptr);

	/* Set the ID to NULL - It frees the the UDP Socket */
	skt->id         		= SOCKET_ID_NULL;
	skt->locport    		= 0;
	skt->locaddr.addr   = 0;
	skt->remport    		= 0;
	skt->remaddr.addr   = 0;
	skt->recv       		= NULL;
	skt->arg        		= NULL;
	skt->pcb_ptr    		= NULL;

	/* Generic unlock */
	ipal_base_hid_threadsafety_unlock();
	
	return(ipal_res_OK);
} 

/*
 * ipal_udpsocket_bind
 * See header file for details
 */
ipal_result_t ipal_udpsocket_bind(ipal_udpsocket_t* skt, ipal_ipv4addr_t locaddr, ipal_port_t locport)
{
	uint8_t i;
	err_t err;
	ip4_addr_t ipaddr;

	/* Checks whether the  socket has been correctly allocated */
	if((NULL == skt) || (SOCKET_ID_NULL == skt->id) || (0 == locport))
	{
		return(ipal_res_NOK_generic);
	}

	/* Generic lock */
	ipal_base_hid_threadsafety_lock();
	
	/* Transform IP address in LWIP format */
	ipaddr.addr = ipal2lwip_ipv4addr(locaddr);

	/* Checks whether the local port has not been already occupied */
	for(i=0; i<IPAL_NUMBEROF_UDPMAXSOCKETS; i++)
	{
		if((ipaddr.addr == s_ipal_udp_udpsockets[i].locaddr.addr) && (locport == s_ipal_udp_udpsockets[i].locport))
		{
			ipal_base_hid_threadsafety_unlock();
			return(ipal_res_NOK_generic);
		}
	}
	
	skt->locaddr = ipaddr;
	skt->locport = locport;
	
	/* Call LWIP udp bind */
	err = udp_bind(skt->pcb_ptr, &(skt->locaddr), skt->locport);
	
	/* Error occurred */
	if(ERR_OK != err)
	{
		skt->locaddr.addr = 0;
		skt->locport 			= 0;
		
		ipal_base_hid_threadsafety_unlock();
		return(ipal_res_NOK_generic);
	}
	
	ipal_base_hid_threadsafety_unlock();
	
	return(ipal_res_OK);
} 

/*
 * ipal_udpsocket_connect
 * See header file for details
 */
ipal_result_t ipal_udpsocket_connect(ipal_udpsocket_t* skt, ipal_ipv4addr_t remaddr, ipal_port_t remport)
{
	err_t err;
	ip4_addr_t ipaddr;
	
	if((NULL == skt) || (SOCKET_ID_NULL == skt->id) || (skt->locaddr.addr == 0) || (skt->locport == 0))
	{  
		return(ipal_res_NOK_generic);
	}

	ipal_base_hid_threadsafety_lock();

	/* If not bound yet, return ERROR. Do not use the "random binding" feature of lwip */
	if(skt->locport == 0)
	{
		ipal_base_hid_threadsafety_unlock();
		return(ipal_res_NOK_generic);
	}

	ipaddr.addr = ipal2lwip_ipv4addr(remaddr);
	
	skt->remaddr = ipaddr;
	skt->remport = remport;
	
	/* LWIP connect */
	err = udp_connect(skt->pcb_ptr, &(skt->remaddr), skt->remport);
	
	/* Error occurred */
	if(ERR_OK != err)
	{
		skt->remaddr.addr = 0;
		skt->remport 			= 0;
		
		ipal_base_hid_threadsafety_unlock();
		
		return(ipal_res_NOK_generic);
	}
	
	ipal_base_hid_threadsafety_unlock();
	
	return(ipal_res_OK);
} 

/*
 * ipal_udpsocket_disconnect
 * See header file for details
 */
ipal_result_t ipal_udpsocket_disconnect(ipal_udpsocket_t* skt)
{
	/* Coherency check */
	if((NULL == skt) || (SOCKET_ID_NULL == skt->id))
	{   
		return(ipal_res_NOK_generic);
	}

	ipal_base_hid_threadsafety_lock();
	
	/* Disconnect */
	udp_disconnect(skt->pcb_ptr);
	
	/* Reset remote ports */
	skt->remport = 0;
	skt->remaddr.addr = 0;

	ipal_base_hid_threadsafety_unlock();
			
	return(ipal_res_OK);
} 

/*
 * ipal_udpsocket_send
 * See header file for details
 */
ipal_result_t ipal_udpsocket_send(ipal_udpsocket_t* skt, ipal_packet_t *pckt)
{
	ipal_result_t res;

	/* Check coherency */
	if((NULL == skt) || (0 == skt->locport) || (0 == skt->remaddr.addr) || (0 == skt->remport))
	{
		return(ipal_res_NOK_generic);
	}

	/* Generic lock */
	ipal_base_hid_threadsafety_lock();
	
	/* Wrap the IPAL sendto */
	res = ipal_udpsocket_sendto(skt, pckt, skt->remaddr.addr, skt->remport);

	/* Generic unlock */
	ipal_base_hid_threadsafety_unlock();

	return(res);
} 

/*
 * ipal_udpsocket_sendto
 * See header file for details
 */
ipal_result_t ipal_udpsocket_sendto(ipal_udpsocket_t* skt, ipal_packet_t *pckt, ipal_ipv4addr_t remaddr, ipal_port_t remport)
{
	struct pbuf *lwip_buf;
	ip4_addr_t ipaddr;
	err_t err;
	ipal_result_t res;
	
	/* Coherency check */
	if((NULL == skt) || (SOCKET_ID_NULL == skt->id) || (0 == skt->locport) || 
		 (NULL == pckt) || (NULL == pckt->data) || (0 == pckt->size))
	{   
		return(ipal_res_NOK_generic);
	}

	/* Generic lock */
	ipal_base_hid_threadsafety_lock();

	/* Alloc buffer in PBUF_RAM buffer */
	lwip_buf = pbuf_alloc(PBUF_TRANSPORT, pckt->size, PBUF_RAM);
	
	/* No more memory */
	if(lwip_buf == NULL)
	{
		ipal_base_hid_on_fatalerror(ipal_error_internal0, "pbuf_alloc returns NULL in ipal_udpsocket_sendto");
		
		/* Unlock */
		ipal_base_hid_threadsafety_unlock();
		return(ipal_res_NOK_generic);
	}
	
	/* Value is internally copied to allocated pbuf */
	res = s_ipal_copydata_2_lwip_buf(pckt, lwip_buf); 
	
	/* Check if any error occurs */
	if(ipal_res_OK != res)
	{
		ipal_base_hid_on_fatalerror(ipal_error_internal0, "error in coping data from user_pkt to pbuf");
	}
	
	/* Gets remote address */
	ipaddr.addr = ipal2lwip_ipv4addr(remaddr);
	
	/* Send to remote system */
	err = udp_sendto(skt->pcb_ptr, lwip_buf, &ipaddr, remport);
	
	#ifndef IPAL_USE_RTOS_MINIMAL
		/* Free the pbuf buffer */
		pbuf_free(lwip_buf);
	#endif
	
	/* Unlock */
	ipal_base_hid_threadsafety_unlock();

	/* Return */
	return( (ERR_OK != err) ? ipal_res_NOK_generic : ipal_res_OK );
} 

/*
 * ipal_udpsocket_recv
 * See header file for details
 */
ipal_result_t ipal_udpsocket_recv(ipal_udpsocket_t* skt, ipal_udp_recv_fn recv, void *arg)
{
	/* Coherency check */
	if((NULL == skt) || (NULL == recv))
	{   
		return(ipal_res_NOK_generic);
	}

	/* Generic lock */
	ipal_base_hid_threadsafety_lock();

	/* Sets receive function and args */
	skt->recv = recv;
	skt->arg  = arg;
	
	/* Calls udp_rcv to mount the callback - The pointer to the IPAL socket will be passed as argument */
	udp_recv(skt->pcb_ptr, s_ipal_udpsocket_lwip_recvfn, (void*)skt);

	/* Generic unlock */
	ipal_base_hid_threadsafety_unlock();
			
	return(ipal_res_OK); 
} 

/* =======================================================================================================================
 * PUBLIC FUNCTIONS (APIs)
 * ======================================================================================================================= */

/*
 * ipal_udp_hid_getsize
 * See header file for details
 */
uint32_t ipal_udp_hid_getsize(const ipal_cfg2_t *cfg2)
{
	/* No specific memory is required, to be allocated outside from the module */
	return(0);
}

/*
 * ipal_udp_hid_setmem
 * See header file for details
 */
ipal_result_t ipal_udp_hid_setmem(const ipal_cfg2_t *cfg2, uint32_t *memory)
{
	/* No needed */
	return(ipal_res_OK); 
}

/*
 *
 * See header file for details
 */
ipal_result_t ipal_udp_hid_init(ipal_cfg2_t *cfg2)
{
	/* No specific initialization */
	return(ipal_res_OK); 
}

/*
 *
 * See header file for details
 */
ipal_result_t ipal_udp_hid_vars_init(const ipal_cfg2_t *cfg2)
{
	/* We must have enough available sockets */
	if( (IPAL_NUMBEROF_UDPMAXSOCKETS < cfg2->udp->udp_socketnum) )
	{
		return(ipal_res_NOK_generic);
	}

	/* Resets internal variables */
	s_ipal_udp_internal_ram_reset();
	
	/* Initializes internal variables */
	s_ipal_udp_internal_ram_init();
	
	return(ipal_res_OK);
}

/* =======================================================================================================================
 * PRIVATE FUNCTIONS DEFINITIONS
 * ======================================================================================================================= */

/*
 * Resets UDP sockets (sets all them fields to zero) 
 */
static void s_ipal_udp_internal_ram_reset(void)
{
	memset(&s_ipal_udp_udpsockets[0], 0, sizeof(s_ipal_udp_udpsockets));
}

/*
 * Marks sockets ID as NULL (it means that they are free and available for allocation 
 */
static void s_ipal_udp_internal_ram_init(void)
{
	uint8_t id;
	
	/* Iterates on UDP sockets */
	for(id=0; id<IPAL_NUMBEROF_UDPMAXSOCKETS; id++)
	{
		s_ipal_udp_udpsockets[id].id = SOCKET_ID_NULL;
	}
}

/*
 * s_ipal_udpsocket_lwip_recvfn
 * See declaration for details
 */
static void s_ipal_udpsocket_lwip_recvfn(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip4_addr_t *addr, u16_t port)
{
	/* The UDP Socket is passed as argument */
	ipal_udpsocket_t* skt = (ipal_udpsocket_t*)arg;
	
	/* IPAL packet (it will host the payload of received packet */
	ipal_packet_t pkt;
	
	/* IPAL result */
	ipal_result_t res;
	
	/* Coherency check */
	if((NULL != skt) && (SOCKET_ID_NULL != skt->id) && (skt->pcb_ptr == upcb))
	{
		if(NULL != skt->recv)
		{
			res = s_ipal_copydata_from_lwip_buf(p, &pkt);
			
			if(ipal_res_OK != res)
			{
				return;
			}
			
			/* Call IPAL socket receive callback */
			skt->recv(skt->arg, skt, &pkt, lwip2ipal_ipv4addr(addr->addr), port);
		}
	}
}


/*
 * s_ipal_copydata_2_internalbuf
 * See declaration for details
 */
static ipal_result_t s_ipal_copydata_2_lwip_buf(ipal_packet_t *pkt, struct pbuf *lwip_buf) 
{
	memcpy(lwip_buf->payload, pkt->data, pkt->size);
	
	return (ipal_res_OK);   
}

/*
 * s_ipal_copydata_from_lwip_buf
 * See declaration for details
 */
static ipal_result_t s_ipal_copydata_from_lwip_buf(struct pbuf *lwip_buf, ipal_packet_t *pkt)
{
	memcpy(receive_buf, lwip_buf->payload, lwip_buf->len);
	
	/* Copy in receive data */
	pkt->size = lwip_buf->len;
	pkt->data = receive_buf;
	
	/* Free LWIP buf */
	pbuf_free(lwip_buf);
	
	return (ipal_res_OK);   
}

/*
 * s_set_lwip_tos
 * See declaration for details
 */
static void s_set_lwip_tos(struct udp_pcb *upcb, ipal_tos_t tos)
{	
	upcb->tos = ((tos.precedence << 5) | (tos.lowdelay << 4) | (tos.highthroughput << 3) | (tos.highreliability << 2));
}

/*
 * s_ipal_udpsocket_getfirstfreesocketid
 * See declaration for details
 */
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




