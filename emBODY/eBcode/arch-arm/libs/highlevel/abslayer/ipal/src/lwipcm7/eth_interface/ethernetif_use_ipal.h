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
 
#warning marco.accame: removed eth_frame_t because it is already defined a similar type ipal_eth_frame_t
// which has 
//typedef struct 
//{
//    uint16_t            length;
//    uint16_t            dummy;
//    void*               payload;	
//} ipal_eth_frame_t; __ipalVERIFYsizeof(ipal_eth_frame_t, 8)

#include "ipal_base.h"
#include "ipal_hal_eth.h"

#if 0
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
#endif

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 */
typedef struct  
{    
	/* PHY initialized successfully  */
	bool                        phy_ok;

	/* PHY initialized successfully  */
	bool                        link_status;
	
	/* True when a new rx event is available */
	uint8_t						 n_rx_events_signaled;
	uint8_t						 n_rx_events_consumed;							
	
	/* True when a tx event must be triggered */
	bool                        tx_event;
	bool                        tx_in_course;
	
	#if (NO_SYS)
		ipal_hal_eth_frame_t    tx_frame_to_send;
		struct pbuf             *p;
	#endif
			
	#if (!NO_SYS)
		/* Semaphore to access this structure */
		sys_sem_t         sem;
	#else
		/* NO_SYS */	
	#endif

} eth_status_t;

#endif
