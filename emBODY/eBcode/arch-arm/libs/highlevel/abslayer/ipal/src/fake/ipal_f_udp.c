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

/* @file       ipal_f_udp.c
	@brief      This file implements internal implementation of the fake ipal udp module
	@author     marco.accame@iit.it
    @date       11/03/2014
**/


#ifdef IPAL_USE_UDP

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "time.h"


#include "ipal_f_base_hid.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "ipal.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

//#include "ipal_f_udp_hid.h"

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



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static struct ipal_udpsocket_opaque_t s_ipal_f_udp_socket = {0};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern ipal_udpsocket_t* ipal_udpsocket_new(ipal_tos_t tos)
{
    return(&s_ipal_f_udp_socket);
}


extern ipal_result_t ipal_udpsocket_delete(ipal_udpsocket_t* skt)
{  
    return(ipal_res_OK);
} 


extern ipal_result_t ipal_udpsocket_bind(ipal_udpsocket_t* skt, ipal_ipv4addr_t locaddr, ipal_port_t locport)
{    
    return(ipal_res_OK);
} 


extern ipal_result_t ipal_udpsocket_connect(ipal_udpsocket_t* skt, ipal_ipv4addr_t remaddr, ipal_port_t remport)
{ 
    return(ipal_res_OK);
} 


extern ipal_result_t ipal_udpsocket_disconnect(ipal_udpsocket_t* skt)
{
    return(ipal_res_OK);
} 


extern ipal_result_t ipal_udpsocket_send(ipal_udpsocket_t* skt, ipal_packet_t *pckt)
{
    return(ipal_res_OK);
} 

extern ipal_result_t ipal_udpsocket_sendto(ipal_udpsocket_t* skt, ipal_packet_t *pckt, ipal_ipv4addr_t remaddr, ipal_port_t remport)
{
    return(ipal_res_OK);
} 


extern ipal_result_t ipal_udpsocket_recv(ipal_udpsocket_t* skt, ipal_udp_recv_fn recv, void *arg)
{
    return(ipal_res_OK); 
} 


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



#endif//IPAL_USE_UDP

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




