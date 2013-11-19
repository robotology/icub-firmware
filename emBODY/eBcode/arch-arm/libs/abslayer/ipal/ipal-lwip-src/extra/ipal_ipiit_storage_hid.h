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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _IPAL_IPIIT_STORAGE_HID_H_
#define _IPAL_IPIIT_STORAGE_HID_H_


/* @file       ipal_ipiit_storage_hid.h
	@brief      This file keeps external hidden reference to the variables needed by lwip
	@author     valentina.gaggero@iit.it
    @date       01/10/2013
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "lwip_interface.h"



// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "ipal_ipiit_storage.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// meoty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------
// empty-section


// - declaration of extern hidden variables ---------------------------------------------------------------------------
// - declaration of extern hidden functions ---------------------------------------------------------------------------


// - arp


// - dhcp



// - eth
/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
extern void low_level_init(struct netif *netif);


/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
extern struct pbuf * low_level_input(struct netif *netif);

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

extern err_t low_level_output(struct netif *netif, struct pbuf *p);

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
extern err_t ethernetif_init(struct netif *netif);


// - ftp


// - igmp


// - sys



// - tcp




// - telnet



// - tftp



// - udp




#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


