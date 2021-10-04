/**
 * @file ethernetif_use_ipal.c
 *
 * This is the ethernetif version that USE the IPAL external functions to access hardware
 *
 * Public functions:
 * 		err_t ethernetif_init(struct netif *netif);
 * 		void  ethernetif_poll(struct netif *netif);
 * 		void  ethernetif_check_link (struct netif *netif);
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

#include "ethernetif.h"
#include "ethernetif_use_ipal.h"
#include "ethernetif_ipal.h"
#include "ipal_hid.h"
#include "ipal_hal_eth_lwip_aux_functions.h"
#include "ipal_rtos.h"
#include "ipal_debug.h"

#ifdef FRAME_QUEUE
	#include "lwip_frame_queue_handler.h"
#endif

#ifdef IPAL_USE_RTOS_MINIMAL
	#include "lwip_tx_frame_queue_handler.h"
#endif

#include <stdbool.h>
#include <string.h>

/* ===============================================================================================
 * MACROS
 * =============================================================================================== */

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

/* ===============================================================================================
 * GLOBAL VARIABLES
 * =============================================================================================== */

/* The ETH status */
volatile eth_status_t eth_status;
 
/* ===============================================================================================
 * PRIVATE FUNCTIONS
 * =============================================================================================== */
 
/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif)
{
  uint64_t mac_address = ipal_base_hid_cfg2_eth.eth_mac;
	
  /* Driver information */
  eth_status_t *eth_status = (eth_status_t *)netif->state;
  
  /* set MAC hardware address length */
  netif->hwaddr_len = ETH_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] =  ((uint8_t *)&mac_address)[0];
  netif->hwaddr[1] =  ((uint8_t *)&mac_address)[1];
  netif->hwaddr[2] =  ((uint8_t *)&mac_address)[2];
  netif->hwaddr[3] =  ((uint8_t *)&mac_address)[3];
  netif->hwaddr[4] =  ((uint8_t *)&mac_address)[4];
  netif->hwaddr[5] =  ((uint8_t *)&mac_address)[5];

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

	/* Begin IPAL */

	#if (!NO_SYS)
		/* Create semaphore for structure eth0_hwdrv_status  */
		sys_sem_new(&eth_status->sem, 0);
	#else
		/* MARTINO_NO_SYS */
	#endif
	
	/* PHY not ok */
    eth_status->phy_ok = false;
	
	/* Link down */
	eth_status->link_status = false;
	
  /* WRAP to IPAL hardware access function */
	ethernetif_IPAL_low_level_init();
	
	/* PHY ok */
    eth_status->phy_ok = true;
	
	/* LINK up */
	eth_status->link_status = true;
	
	/* Reset events */
	eth_status->tx_event = false;
	eth_status->tx_in_course = false;
	eth_status->n_rx_events_signaled = 0;
	eth_status->n_rx_events_consumed = 0;
	
	/* MARTINO - This should part of checklink function 
		 It will be fixed in the next version, making it available as diagnostics feature */
	netif_set_link_up (netif);
	
	#if (!NO_SYS)
		/* Signal semaphore for eth0_hwdrv_status */
		sys_sem_signal(&eth_status->sem);
	#else
		/* MARTINO_NO_SYS */
	#endif
	
	/* End IPAL */
}

#if (!NO_SYS)
	/**
	 * The low level output function used when an RTOS is in use 
	 * (It uses semaphores to synchronizes operations)
	 */
	static err_t low_level_output_sys(struct netif *netif, struct pbuf *p)
	{	
		/* The frame to send */
		ipal_hal_eth_frame_t eth_frame_to_send;
	
		/* Driver information */
		eth_status_t *eth_status = (eth_status_t *)netif->state;

		#if ETH_PAD_SIZE
			pbuf_remove_header(p, ETH_PAD_SIZE); /* drop the padding word */
		#endif
	
		/* Begin IPAL */
		
		#if (!NO_SYS)
			/* Acquire lock on structure eth0_hwdrv_status  */
			sys_sem_wait (&eth_status->sem);
		#else
			/* MARTINO_NO_SYS */
		#endif
		
		/* Sets payload and len */
		eth_frame_to_send.payload = p->payload;
		eth_frame_to_send.len = p->len;
		
		/* CALL IPAL low level functions */
		ethernetif_IPAL_low_level_output(&eth_frame_to_send);
		
		#if (!NO_SYS)
			/* Release lock */
			sys_sem_signal (&eth_status->sem);
		#else
			/* MARTINO_NO_SYS: TODO */
		#endif
		
		/* End IPAL */

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
#else
	/**
	 * The low level output function used when an RTOS is in use 
	 * (It uses semaphores to synchronizes operations)
	 */
	static err_t low_level_output_nosys(struct netif *netif, struct pbuf *p)
	{
		/* Driver information */
		eth_status_t *eth_status = (eth_status_t *)netif->state;
		
		err_t ret = ERR_OK;
		
		/* martino: verificare */
		#if(0)
		
		if(eth_status->tx_event)
		{
			ret = ERR_INPROGRESS;
		}
		
		if (ret !=  ERR_OK)
		{
			return ret;
		}
		#endif

		#if ETH_PAD_SIZE
			pbuf_remove_header(p, ETH_PAD_SIZE); /* drop the padding word */
		#endif
		
		lwip_TX_queue_protect();
		
		lwip_TX_frame_enqueue(p);
		
		lwip_TX_queue_unprotect();
		
		
		
		#if (0)
			/* Sets payload and length  */
			eth_status->tx_frame_to_send.payload = p->payload;
			eth_status->tx_frame_to_send.len = p->len;
		#endif
		

		#if ETH_PAD_SIZE
			pbuf_add_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
		#endif
		
		/* Trigger transmit */
		//eth_status->tx_event = true;
		
		return ret;
	}
#endif

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
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
	err_t ret;
	
	#if (!NO_SYS)
		ret = low_level_output_sys(netif, p);
	#else
		ret = low_level_output_nosys(netif, p);
	#endif
	
	if (ret == ERR_OK)
	{
        #warning marco.accame.comment
        // this function does the dual as legacy usr_on_ethframe_received()
        // and signals that we have just transmitted a frame
		/* Signal that a tx frame is ready */
		// ipal_hal_eth_aux_functions.signal_tx_frame();
        if(NULL != ipal_base_hid_cfg2_extfn2.signal_tx_frame)
        {
            ipal_base_hid_cfg2_extfn2.signal_tx_frame();
        }
	}
	
	return ret;
}

#ifndef FRAME_QUEUE
	/**
	 * This version of "low_level_input" leverages on allocation of pbuf and 
	 * transfer of received frame to higher level of the stack
	 *
	 * Should allocate a pbuf and transfer the bytes of the incoming
	 * packet from the interface into the pbuf.
	 *
	 * @param netif the lwip network interface structure for this ethernetif
	 * @return a pbuf filled with the received packet (including MAC header)
	 *         NULL on memory error
	 */
	static struct pbuf *low_level_input_noqueue(struct netif *netif)
	{
		struct pbuf *p;
		u16_t len;
		
		/* Obtain the size of the packet and put it into the "len" variable. */
		// len = ipal_hal_eth_aux_functions.get_frame_size();
        len = ipal_base_hid_cfg2_eth.get_frame_size();
		
		if (len == 0) {
			/* No packet available */
			return NULL;
		}
		if (len > 1514) {
			/* Drop oversized packet */
			//ipal_hal_eth_aux_functions.get_frame(0, NULL);
            ipal_base_hid_cfg2_eth.get_frame(0, NULL);
			return NULL;
		}

		#if ETH_PAD_SIZE
			len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
		#endif

		/* We allocate a pbuf chain of pbufs from the pool. */
		p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

		if (p != NULL) 
		{
			
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
			//ipal_hal_eth_aux_functions.get_frame(len, p->payload);
            ipal_base_hid_cfg2_eth.get_frame(len, p->payload);
			
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
			//ipal_hal_eth_aux_functions.get_frame(0, NULL);
            ipal_base_hid_cfg2_eth.get_frame(0, NULL);
			LINK_STATS_INC(link.memerr);
			LINK_STATS_INC(link.drop);
			MIB2_STATS_NETIF_INC(netif, ifindiscards);
		}

		return p;
	}
#else
	/**
	 * This version of "low_level_input" compiles a pbuf and transfer it 
	 * to the higher level of the stack.
	 * 
	 * pbuf is not allocated at this level, the frame is allocated when received, here
	 * it is just adjusted the pointer reference within the pbuf
	 */
	static struct pbuf *low_level_input_queue(struct netif *netif)
	{
		lwip_rxframe_container_t *rx_container;
		struct pbuf *p;
		u16_t len;
		
		/* Get the next available container */
		rx_container = lwip_frame_queue_RX_GET();
		
		/* Set len */
		len = rx_container->length;
		
		/* Alloc custom pbuf - Type ???  */
		p = pbuf_alloced_custom(PBUF_RAW, len, PBUF_POOL, &rx_container->pbuf_cstm, rx_container->payload, len);
		
		#if ETH_PAD_SIZE
			/* Reclaim the padding word */
			p->payload = (uint8_t *)p->payload - ETH_PAD_SIZE;
			p->len += ETH_PAD_SIZE;
			p->tot_len = p->len;
		
			rx_container->payload -= ETH_PAD_SIZE;
		#endif

		return p;
	}
#endif

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *low_level_input(struct netif *netif)
{
	struct pbuf *p;
	
	#ifndef FRAME_QUEUE
	
		/* No frame queue */
		p = low_level_input_noqueue(netif);
	
	#else
	
		/* Get Frame from queue */
		p = low_level_input_queue(netif);
	#endif
	
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
static void ethernetif_input(struct netif *netif)
{	
	/* Driver information */
	eth_status_t *eth_status = (eth_status_t *)netif->state;
	
	#ifdef FRAME_QUEUE
		eth_status = eth_status;
	#endif
		
  struct pbuf *p;
	
	#if (!NO_SYS)
		/* move received packet into a new pbuf */
		sys_sem_wait (&eth_status->sem);
	#else
		/* MARTINO_NO_SYS */
	#endif
		
	p = low_level_input(netif);
  
	#if (!NO_SYS)
		sys_sem_signal (&eth_status->sem);
	#else
		/* MARTINO_NO_SYS */
	#endif
  	
	/* if no packet could be read, silently ignore this */
	if (p != NULL) {
		/* pass all packets to ethernet_input, which decides what packets it supports */
		if (netif->input(p, netif) != ERR_OK) 
		{
			LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
			
			/* Free allocated room for pbuf */
			pbuf_free(p);
			
			p = NULL;
		}
		
		//return;
	}
	
	#ifndef FRAME_QUEUE
		/* Event reset */
		eth_status->n_rx_events_consumed += 1;
	#endif
}
 
/* ===============================================================================================
 * PUBLIC FUNCTIONS
 * =============================================================================================== */
 
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
err_t ethernetif_init(struct netif *netif)
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

	/* Sets reference to the struct which keep hardware-level drivers and status information */
	netif->state = (void *)&eth_status;
	
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

	/* "send frame" function */
	netif->linkoutput = low_level_output;
	
  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}

/**
 * Check link
 * Retrieve diagnostic info
 * Not implemented in this version
 */
void ethernetif_check_link (struct netif *netif) 
{
	/* Left unimplemented - SO FAR */
}

extern bool dbg_rx_event;


/**
 * Polls for incoming packets
 * Not implemented, interrupt-based version is used
 */
void ethernetif_poll (struct netif *netif) 
{
	#ifdef IPAL_USE_RTOS_MINIMAL
		tx_fq_element_t *tx_el;
	#endif
	
	/* Driver information */
  eth_status_t *eth_status = (eth_status_t *)netif->state;

	/* If connection down, return */
  if (!eth_status->phy_ok || !eth_status->link_status) 
	{
    return;
  }
	
	#ifndef FRAME_QUEUE
		if (eth_status->n_rx_events_signaled != eth_status->n_rx_events_consumed)
	#else
		if (lwip_frame_queue_RX_AVAILABLE())
	#endif
		{
			dbg_ipal_rx_frame_detected = true;
			
			/* Process received ethernet packet */
			ethernetif_input (netif);
						
			dbg_ipal_rx_frame_detected = false;
		}
	
	#if (IPAL_USE_RTOS_MINIMAL)
		
		if (lwip_TX_frame_available())
		{
			
			/* Waiting for EOT */
//			if (!eth_status->tx_in_course)
//			{
//				eth_status->tx_in_course = true;
				
				lwip_TX_queue_protect();
				
				tx_el = lwip_TX_frame_dequeue();
				
				lwip_TX_queue_unprotect();
				
				/* CALL IPAL low level functions */
				ethernetif_IPAL_low_level_output(&(tx_el->tx_frame));
				
				/* Free pbuf */
				pbuf_free(tx_el->p);
//			}
		}

	#endif
}




