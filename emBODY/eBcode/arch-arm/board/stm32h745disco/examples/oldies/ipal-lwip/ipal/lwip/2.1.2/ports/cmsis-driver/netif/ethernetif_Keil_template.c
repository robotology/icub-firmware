/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "lwip/etharp.h"
#include "netif/ppp/pppoe.h"

#include "arch/sys_arch.h"
#include "lwip/sys.h"

#include "ethif_config.h"
#include "ethernetif.h"
#include "Driver_ETH_MAC.h"
#include "Driver_ETH_PHY.h"

/* Ethernet MAC & PHY Driver */
extern ARM_DRIVER_ETH_MAC ARM_Driver_ETH_MAC_(ETH_DRV_NUM);
extern ARM_DRIVER_ETH_PHY ARM_Driver_ETH_PHY_(ETH_DRV_NUM);

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 */
struct ethernetif {
  ARM_DRIVER_ETH_MAC *mac;              // Registered MAC driver
  ARM_DRIVER_ETH_PHY *phy;              // Registered PHY driver
  ARM_ETH_LINK_STATE link;              // Ethernet Link State
  bool               event_rx_frame;    // Callback RX event generated
  bool               phy_ok;            // PHY initialized successfully
  bool               rx_event;          // Frame received
  sys_sem_t          sem;
};

static struct ethernetif eth0_status;

/* Forward declarations. */
static void  ethernetif_input(struct netif *netif);
static void  eth0_notify (uint32_t event);

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif)
{
  struct ethernetif *eth = netif->state;
  ARM_ETH_MAC_CAPABILITIES cap;

  /* set MAC hardware address length */
  netif->hwaddr_len = ETH_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] =  MAC_ADDR0;
  netif->hwaddr[1] =  MAC_ADDR1;
  netif->hwaddr[2] =  MAC_ADDR2;
  netif->hwaddr[3] =  MAC_ADDR3;
  netif->hwaddr[4] =  MAC_ADDR4;
  netif->hwaddr[5] =  MAC_ADDR5;

  /* maximum transfer unit */
  netif->mtu = 1500;

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET;
#if LWIP_IPV4 && LWIP_IGMP
  netif->flags |= NETIF_FLAG_IGMP;
#endif
#if LWIP_IPV6 && LWIP_IPV6_MLD
  netif->flags |= NETIF_FLAG_MLD6;
  /*
   * For hardware/netifs that implement MAC filtering.
   * All-nodes link-local is handled by default, so we must let the hardware know
   * to allow multicast packets in.
   * Should set mld_mac_filter previously. */
  if (netif->mld_mac_filter != NULL) {
    ip6_addr_t ip6_allnodes_ll;
    ip6_addr_set_allnodes_linklocal(&ip6_allnodes_ll);
    netif->mld_mac_filter(netif, &ip6_allnodes_ll, NETIF_ADD_MAC_FILTER);
  }
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */

  /* Do whatever else is needed to initialize interface. */
  eth->phy = &ARM_Driver_ETH_PHY_(ETH_DRV_NUM);
  eth->mac = &ARM_Driver_ETH_MAC_(ETH_DRV_NUM);

  eth->link   = ARM_ETH_LINK_DOWN;
  eth->phy_ok = false;

  sys_sem_new (&eth->sem, 0);
  /* Get MAC capabilities */
  cap = eth->mac->GetCapabilities ();
  eth->event_rx_frame = (cap.event_rx_frame) ? true : false;

  eth->mac->Initialize (eth0_notify);
  eth->mac->PowerControl (ARM_POWER_FULL);
  eth->mac->SetMacAddress ((ARM_ETH_MAC_ADDR *)netif->hwaddr);
  eth->mac->Control (ARM_ETH_MAC_CONTROL_TX, 0);
  eth->mac->Control (ARM_ETH_MAC_CONTROL_RX, 0);

  /* Initialize Physical Media Interface */
  if (eth->phy->Initialize (eth->mac->PHY_Read, eth->mac->PHY_Write) == ARM_DRIVER_OK) {
    eth->phy->PowerControl (ARM_POWER_FULL);
    eth->phy->SetInterface (cap.media_interface);
    eth->phy->SetMode (ARM_ETH_PHY_AUTO_NEGOTIATE);
    eth->phy_ok = true;
  }
	
	
  sys_sem_signal (&eth->sem);
}

static void eth0_notify (uint32_t event) {
  /* Send notification on RX event */
  if (event == ARM_ETH_MAC_EVENT_RX_FRAME) {
    eth0_status.rx_event = true;
  }
}

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
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct ethernetif *eth = netif->state;
  struct pbuf *q;

#if ETH_PAD_SIZE
  pbuf_remove_header(p, ETH_PAD_SIZE); /* drop the padding word */
#endif

  sys_sem_wait (&eth->sem);
  for (q = p; q != NULL; q = q->next) {
    /* Send the data from the pbuf to the interface, one pbuf at a
       time. The size of the data in each pbuf is kept in the ->len
       variable. */
    u32_t flags = (q->next) ? ARM_ETH_MAC_TX_FRAME_FRAGMENT : 0;
    eth->mac->SendFrame (q->payload, q->len, flags);
  }
  sys_sem_signal (&eth->sem);

  MIB2_STATS_NETIF_ADD(netif, ifoutoctets, p->tot_len);
  if (((u8_t *)p->payload)[0] & 1) {
    /* broadcast or multicast packet*/
    MIB2_STATS_NETIF_INC(netif, ifoutnucastpkts);
  } else {
    /* unicast packet */
    MIB2_STATS_NETIF_INC(netif, ifoutucastpkts);
  }
  /* increase ifoutdiscards or ifouterrors on error */

#if ETH_PAD_SIZE
  pbuf_add_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

  LINK_STATS_INC(link.xmit);

  return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
  struct ethernetif *eth = netif->state;
  struct pbuf *p;
  u16_t len;

  /* Obtain the size of the packet and put it into the "len"
     variable. */
  len = eth->mac->GetRxFrameSize ();
  if (len == 0) {
    /* No packet available */
    return NULL;
  }
  if (len > 1514) {
    /* Drop oversized packet */
    eth->mac->ReadFrame (NULL, 0);
    return NULL;
  }

#if ETH_PAD_SIZE
  len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

  if (p != NULL) {

#if ETH_PAD_SIZE
    pbuf_remove_header(p, ETH_PAD_SIZE); /* drop the padding word */
#endif

    /* pbuf payload should be allocated in one piece of contiguous memory. */
    if (p->len < len) {
      len = p->len;
    }
    /* we are assuming the data will fit into this payload.
     * this is documented in pbuf.h
     */
    eth->mac->ReadFrame (p->payload, len);

    MIB2_STATS_NETIF_ADD(netif, ifinoctets, p->tot_len);
    if (((u8_t *)p->payload)[0] & 1) {
      /* broadcast or multicast packet*/
      MIB2_STATS_NETIF_INC(netif, ifinnucastpkts);
    } else {
      /* unicast packet*/
      MIB2_STATS_NETIF_INC(netif, ifinucastpkts);
    }
#if ETH_PAD_SIZE
    pbuf_add_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.recv);
  } else {
    /* Drop packet */
    eth->mac->ReadFrame (NULL, 0);
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
    MIB2_STATS_NETIF_INC(netif, ifindiscards);
  }

  return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
static void
ethernetif_input(struct netif *netif)
{
  struct ethernetif *eth = netif->state;
  struct pbuf *p;

  /* move received packet into a new pbuf */
  sys_sem_wait (&eth->sem);
  p = low_level_input(netif);
  sys_sem_signal (&eth->sem);
  /* if no packet could be read, silently ignore this */
  if (p != NULL) {
    /* pass all packets to ethernet_input, which decides what packets it supports */
    if (netif->input(p, netif) != ERR_OK) {
      LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
      pbuf_free(p);
      p = NULL;
    }
    return;
  }
  eth->rx_event = false;
}

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
err_t
ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, 100000000);

  netif->state = &eth0_status;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
#if LWIP_IPV4
  netif->output = etharp_output;
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
  netif->linkoutput = low_level_output;

  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}

void ethernetif_check_link (struct netif *netif) {
  struct ethernetif *eth = netif->state;
  ARM_ETH_LINK_STATE link;

  if (!eth->phy_ok) {
    return;
  }
  /* Check link status */
  sys_sem_wait (&eth->sem);
  link = eth->phy->GetLinkState ();
  if (link == eth->link) {
    /* No change */
    sys_sem_signal (&eth->sem);
    return;
  }
  eth->link = link;
  if (eth->link == ARM_ETH_LINK_UP) {
    /* Start EMAC DMA */
    ARM_ETH_LINK_INFO info = eth->phy->GetLinkInfo ();
    eth->mac->Control (ARM_ETH_MAC_CONFIGURE,
                       info.speed  << ARM_ETH_MAC_SPEED_Pos  |
                       info.duplex << ARM_ETH_MAC_DUPLEX_Pos |
                       ARM_ETH_MAC_ADDRESS_BROADCAST         );
    eth->mac->Control(ARM_ETH_MAC_CONTROL_TX,1);
    eth->mac->Control(ARM_ETH_MAC_CONTROL_RX,1);
    sys_sem_signal (&eth->sem);
    netif_set_link_up (netif);
  }
  else {
    /* Stop EMAC DMA */
    eth->mac->Control(ARM_ETH_MAC_CONTROL_TX,0);
    eth->mac->Control(ARM_ETH_MAC_CONTROL_RX,0);
    sys_sem_signal (&eth->sem);
    netif_set_link_down (netif);
  }
}

void ethernetif_poll (struct netif *netif) {
  struct ethernetif *eth = netif->state;

  if (!eth->phy_ok || eth->link == ARM_ETH_LINK_DOWN) {
    return;
  }
  if ((eth->rx_event) || (!eth->event_rx_frame)) {
    /* process received ethernet packet */
    ethernetif_input (netif);
  }
}
