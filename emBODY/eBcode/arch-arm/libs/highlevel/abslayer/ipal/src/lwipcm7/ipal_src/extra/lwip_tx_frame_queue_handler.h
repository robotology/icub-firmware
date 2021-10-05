/**
 * @file lwip_tx_frame_queue_handler.h
 */
#ifndef LWIP_TX_FRAME_QUEUE_HANDLER_H
#define LWIP_TX_FRAME_QUEUE_HANDLER_H

#include "ethernetif_use_ipal.h"

/* ================================================================================
 * MACROS
 * ================================================================================ */

#define TX_FRAME_QUEUE_SIZE		(20)

/* ================================================================================
 * TYPE DEFINITIONS
 * ================================================================================ */

/*
 * TX frame queue element
 */
typedef struct 
{
	/* The frame to send */
	ipal_hal_eth_frame_t tx_frame;
	
	/* The pbuf */
	struct pbuf      *p;
	
} tx_fq_element_t;

/* ================================================================================
 * FUNCTION PROTOTYPES
 * ================================================================================ */

/**
 * 
 */
void lwip_TX_frame_queue_init(void);

/**
 * 
 */
void lwip_TX_frame_enqueue(struct pbuf *p);

/**
 * 
 */
tx_fq_element_t *lwip_TX_frame_dequeue(void);

/**
 * 
 */
bool lwip_TX_frame_available(void);

/**
 * 
 */
void lwip_TX_queue_protect(void);


/**
 * 
 */
void lwip_TX_queue_unprotect(void);


#endif