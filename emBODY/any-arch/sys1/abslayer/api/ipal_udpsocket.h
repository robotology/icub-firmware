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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _IPAL_UDPSOCKET_H_
#define _IPAL_UDPSOCKET_H_


/** @file       ipal_udpsocket.h
	@brief      This header file implements public interface to the ip abstraction layer: base services and types 
	@author     marco.accame@iit.it
	@date       12/12/2011
**/

/* @defgroup ipal_udpsocket IP abstraction layer: udp socket
    cercecvecve.
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "ipal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct ipal_udpsocket_opaque_t ipal_udpsocket_t 
    @brief      UDP socket type used in IPAL
 **/
typedef struct ipal_udpsocket_opaque_t ipal_udpsocket_t;



/** @typedef    typedef enum ipal_precedence_t 
    @brief      ipal_precedence_t contains values of the precedence TOS field in UDP traffic.
                See http://tools.ietf.org/html/rfc791 for further details
 **/ 
typedef enum
{
    ipal_prec_routine               = 0,
    ipal_prec_priority              = 1,
    ipal_prec_immediate             = 2,
    ipal_prec_flash                 = 3,
    ipal_prec_flashoverride         = 4,
    ipal_prec_criticepc             = 5,
    ipal_prec_internetworkcontrol   = 6,
    ipal_prec_networkcontrol        = 7
} ipal_precedence_t;


/** @typedef    typedef struct ipal_tos_t 
    @brief      ipal_resources_t contains the type of service made available by IPAL for UDP packets.
                See http://tools.ietf.org/html/rfc791 for further details
 **/ 
typedef struct
{
    uint8_t     precedence      : 3;        /**< use a value from ipal_precedence_t */
    uint8_t     lowdelay        : 1;        /**< boolean 0 or 1 */
    uint8_t     highthroughput  : 1;        /**< boolean 0 or 1 */
    uint8_t     highreliability : 1;        /**< boolean 0 or 1 */
    uint8_t     unused          : 2;        /**< field not used */
} ipal_tos_t;


/** @typedef    typedef ipal_udp_recv_fn 
    @brief      ipal_udp_recv_fn is a function type used for reception of a UDP packet.
    @param      arg         A parameter expressed as a void*
    @param      skt         Pointer to a UDP socket
    @param      pkt         Pointer to the received packet
    @param      adr         The IP address of the sender
    @param      por         The port of the sender
 **/ 
typedef void (*ipal_udp_recv_fn)(void *arg, ipal_udpsocket_t *skt, ipal_packet_t *pkt, ipal_ipv4addr_t adr, ipal_port_t por);

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section



// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern ipal_udpsocket_t* ipal_udpsocket_new(ipal_tos_t tos)
    @brief      Creates a new raw UDP socket.
    @param      tos         Specifies the type of service of the IP packet as defined in rfc791.
    @return     A valid pointer to a UDP socket or NULL upon failure.
 **/
extern ipal_udpsocket_t* ipal_udpsocket_new(ipal_tos_t tos);


/** @fn         extern ipal_result_t ipal_udpsocket_delete(ipal_udpsocket_t* skt)
    @brief      Deletes a raw UDP socket.
    @param      skt         The UDP socket.
    @return     ipal_res_OK on success or ipal_res_NOK_generic upon failure.
 **/
extern ipal_result_t ipal_udpsocket_delete(ipal_udpsocket_t* skt);


/** @fn         extern ipal_result_t ipal_udpsocket_bind(ipal_udpsocket_t* skt, ipal_ipv4addr_t locaddr, ipal_port_t locport)
    @brief      It binds a UDP socket to a local IP address and port. This operation is required to make the socket able to send 
                or receive datagrams.  
    @param      skt         The UDP socket.
    @param      locaddr     Specify the network interface to use in case there are many. It can be ipal_ipv4addr_INADDR_ANY to specify that
                            the socket is bound to any local address.
    @param      locport     Specifies the port that is used to send or received datagrams.
    @return     ipal_res_OK on success or ipal_res_NOK_generic upon failure (for instance if there is a socket already bound to
                the same @e locport.
    @warning    If the socket is not bound with ipal_udpsocket_bind() then it will not be possible to send or receive any datagram
 **/
extern ipal_result_t ipal_udpsocket_bind(ipal_udpsocket_t* skt, ipal_ipv4addr_t locaddr, ipal_port_t locport);


/** @fn         extern ipal_result_t ipal_udpsocket_connect(ipal_udpsocket_t* skt, ipal_ipv4addr_t remaddr, ipal_port_t remport)
    @brief      It connects a UDP socket to a remote host. Any calls of ipal_udpsocket_send() shall use the IP address and port
                here specified until there is a call to ipal_udpsocket_unconnect(). However, the user can still use 
                ipal_udpsocket_sendto() to send a packet to any host. When connected, any reception of packets shall be filtered
                vs the remote IP address and port specified in ipal_udpsocket_connect().
                If the socket is not already bound to a local address, then @e ipal_udpsocket_connect() shall also call 
                @e ipal_udpsocket_bind() with locaddr equal to ipal_ipv4addr_INADDR_ANY and @e locport equal to @e remport.
    @param      skt         The UDP socket.
    @param      locaddr     Specify the network interface to use in case there are many. It can be ipal_ipv4addr_INADDR_ANY to specify that
                            the socket is bound to any local address.
    @param      locport     Specifies the port that is used to send or received datagrams.
    @return     ipal_res_OK on success or ipal_res_NOK_generic upon failure (for instance if there is not an available local port 
                equal to @e remport).
    @warning    Connected sockets only receive data from the connected remote address, while unconnected sockets receive datagrams 
                from any remote address. Connected sockets can use a local port different from the remote port to which they are
                connected. It is used the same in case ipal_udpsocket_connect() requires to make an internal call to 
                ipal_udpsocket_bind() 
 **/
extern ipal_result_t ipal_udpsocket_connect(ipal_udpsocket_t* skt, ipal_ipv4addr_t remaddr, ipal_port_t remport);


/** @fn         extern ipal_result_t ipal_udpsocket_disconnect(ipal_udpsocket_t* skt)
    @brief      It disconnects a UDP socket from the remote host to whcih it is connected. 
    @param      skt         The UDP socket.

    @return     ipal_res_OK on success or ipal_res_NOK_generic upon failure.
 **/
extern ipal_result_t ipal_udpsocket_disconnect(ipal_udpsocket_t* skt);


/** @fn         extern ipal_result_t ipal_udpsocket_sendto(ipal_udpsocket_t* skt, ipal_packet_t *pckt, 
                                                               ipal_ipv4addr_t remaddr, ipal_port_t remport)
    @brief      Sends data to a remote host via a UDP socket already bound to a local address.
    @param      skt             The pointer to a valid UDP socket.
    @param      data            Pointer to a ip packet containing size and data
    @param      remaddr         The destination IP address.
    @param      remport         The destination IP port.
    @return     ipal_res_OK on success or ipal_res_NOK_generic upon failure.
    @warning    The transmission may fail if the MAC address associated to the remote IP has not been discovered yet
                with a previous call to ipal_arp_request().
 **/
extern ipal_result_t ipal_udpsocket_sendto(ipal_udpsocket_t* skt, ipal_packet_t *pckt, ipal_ipv4addr_t remaddr, ipal_port_t remport);


/** @fn         extern ipal_result_t ipal_udpsocket_send(ipal_udpsocket_t* skt, ipal_packet_t *pckt) 
    @brief      Sends data to a remote host via a UDP socket already bound to a local address and connected to a remote host
    @param      skt             The pointer to a valid UDP socket.
    @param      data            Pointer to a ip packet containing size and data
    @return     ipal_res_OK on success or ipal_res_NOK_generic upon failure.
    @warning    The transmission may fail if the MAC address associated to the remote IP has not been discovered yet
                with a previous call to ipal_arp_request().
 **/
extern ipal_result_t ipal_udpsocket_send(ipal_udpsocket_t* skt, ipal_packet_t *pckt);


/** @fn         extern ipal_result_t ipal_udpsocket_recv(ipal_udpsocket_t* skt, ipal_udp_recv_fn recv, void *arg) 
    @brief      It specifies a callback function to be used upon reception of a packet on the a UDP socket bound to a
                local IP address and port. The callback is internally called by IPAL only if the received packet is 
                addressed to the local IP address and port specified by ipal_udpsocket_bind(). If the socket is 
                also connected to a remote host, the callback is called only if the IP address and port of the sender
                match thos specified with ipal_udpsocket_connect(). If it is not connected, then the callback
                is called for any remote sender.
    @param      skt             The pointer to a valid UDP socket.
    @param      recv            Pointer to the callback function:
                                void recv(void *arg, ipal_udpsocket_t *skt, ipal_packet_t *pkt, ipal_ipv4addr_t adr, ipal_port_t por).
                                The arguments are: @e arg is a user-defined parameter, @e skt is the receiving UDP socket, @e pkt is
                                a pointer to the received packet (which must be copied as it will be freed after the callback is called),
                                and finally @e adr and @e por specify the sending host.
    @param      arg             A user-defined parameter which will be passed by the callback as an argument. 
    @return     ipal_res_OK on success or ipal_res_NOK_generic upon failure.

 **/
extern ipal_result_t ipal_udpsocket_recv(ipal_udpsocket_t* skt, ipal_udp_recv_fn recv, void *arg);



/* @}            
    end of group ipal_udpsocket  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

