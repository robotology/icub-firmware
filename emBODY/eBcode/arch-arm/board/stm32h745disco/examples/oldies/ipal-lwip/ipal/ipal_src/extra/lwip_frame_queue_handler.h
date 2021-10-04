/**
 * @file lwip_frame_queue_handler.h
 *
 * Auxiliary functions to deal with LWIP FRAME QUEUE
 */
#ifndef LWIP_FRAME_QUEUE_H
#define LWIP_FRAME_QUEUE_H

#include "lwip/pbuf.h"
#include <stdbool.h>

/* =========================================================================
 * MACROS
 * ========================================================================= */

/* RX frame queue size */
#define RX_FRAME_QUEUE_SIZE	(20)

/* =========================================================================
 * TYPE DEFINITIONS
 * ========================================================================= */

/**
 * It contains information of RX frame
 */
typedef struct 
{
	/* Len */
	uint16_t length;
	
	/* Payload */
	uint8_t *payload;
	
	/* Custom pbuf */
	struct pbuf_custom pbuf_cstm;
	
} lwip_rxframe_container_t;
 
/* =========================================================================
 * FUNCTION PROTOTYPES
 * ========================================================================= */
 
/**
 * RX frame queue init
 */
void lwip_frame_queue_RX_INIT(void);

/**
 * Put a newly allocated pbuf into pbuf RX queue
 */
lwip_rxframe_container_t *lwip_frame_queue_RX_NEW(void);

/**
 * Gets a pbuf from pbuf RX queue
 */
lwip_rxframe_container_t *lwip_frame_queue_RX_GET(void);

/**
 * Returns true if a new frame is available
 */
bool lwip_frame_queue_RX_AVAILABLE(void);

#endif
