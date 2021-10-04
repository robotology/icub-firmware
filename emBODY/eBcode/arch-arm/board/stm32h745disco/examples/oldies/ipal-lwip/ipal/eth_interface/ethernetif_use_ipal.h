/**
* @file ethernetif_use_ipal.h
 *
 * Common type definitions
 */
#ifndef ETHERNETIF_USE_IPAL_H
#define ETHERNETIF_USE_IPAL_H

#include "arch/sys_arch.h"
#include <stdbool.h>
#include <stdint.h>

/* ================================================================================================
 * TYPE DEFINITIONS
 * ================================================================================================ */
 
/**
 * Frame (to be sent) passed to lower level
 * Compatible <CASTABLE TO> hl_eth_frame_t
 */
typedef struct
{
	/* Payload length */
	uint16_t  len;
	
	/* Dummuy field */
	uint16_t dummy;
	
	/* The payload */
	void *payload;
	
} eth_frame_t;


/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 */
typedef struct  
{    
	/* PHY initialized successfully  */
	bool               phy_ok;

	/* PHY initialized successfully  */
	bool							 link_status;
	
	/* True when a new rx event is available */
	uint8_t						 n_rx_events_signaled;
	uint8_t						 n_rx_events_consumed;							
	
	/* True when a tx event must be triggered */
	bool						   tx_event;
	bool							 tx_in_course;
	
	#if (NO_SYS)
		eth_frame_t 		 tx_frame_to_send;
		struct pbuf      *p;
	#endif
			
	#if (!NO_SYS)
		/* Semaphore to access this structure */
		sys_sem_t         sem;
	#else
		/* NO_SYS */	
	#endif

} eth_status_t;

#endif
