/**
 * @file lwip_frame_queue_handler.c
 *
 * Implementation of function bodies defined in lwip_frame_queue_handler.h
 */
#include "lwip_frame_queue_handler.h"
#include <stdint.h>
#include <string.h>

#define MAX_RX_FRAME_PAYLOAD 			(1520)

/* =========================================================================
 * STATIC VARIABLES
 * ========================================================================= */
 
/* RX frame payloads */
static uint8_t rx_frame_payload[RX_FRAME_QUEUE_SIZE * MAX_RX_FRAME_PAYLOAD];

/* The queue of RX frames */
static lwip_rxframe_container_t rx_frame_queue[RX_FRAME_QUEUE_SIZE];

/* Write index */
volatile uint8_t rx_fq_widx = 0;
	
/* Read index */
volatile uint8_t rx_fq_ridx = 0;

/* =========================================================================
 * PRIVATE FUNCTIONS
 * ========================================================================= */
 
/*
 * Free function
 */
static void custom_pbuf_free_function(struct pbuf *p)
{
	/* Avoid compiler warnings */
	p = p;
	
	/* Do nothing */
}


/* =========================================================================
 * PUBLIC FUNCTIONS
 * ========================================================================= */
 
/*
 * lwip_frame_queue_RX_INIT
 * See header file for details
 */
void lwip_frame_queue_RX_INIT(void)
{
	uint8_t i;
	lwip_rxframe_container_t *container;
	struct pbuf_custom *p;
	
	rx_fq_widx = 0;
	rx_fq_ridx = 0;
	
	for(i=0; i<RX_FRAME_QUEUE_SIZE; i++)
	{
		container = &rx_frame_queue[i];
		p = &(container->pbuf_cstm);
		
		/* Initialize pbuf to zero */
		memset(&p->pbuf, 0, sizeof(struct pbuf));
		
		/* Set custom free function */
		p->custom_free_function = custom_pbuf_free_function;
		
		container->payload = &rx_frame_payload[i*MAX_RX_FRAME_PAYLOAD];
	}
}


/*
 * lwip_frame_queue_RX_NEW
 * See header file for details
 */
lwip_rxframe_container_t *lwip_frame_queue_RX_NEW(void)
{
	lwip_rxframe_container_t *p;
	
	p = &rx_frame_queue[rx_fq_widx];
	
	rx_fq_widx = (rx_fq_widx + 1)%RX_FRAME_QUEUE_SIZE;
	
	return p;
}

/*
 * lwip_frame_queue_RX_GET
 * See header file for details
 */
lwip_rxframe_container_t *lwip_frame_queue_RX_GET(void)
{
  lwip_rxframe_container_t *p;
	
	p = &rx_frame_queue[rx_fq_ridx];
	
	rx_fq_ridx = (rx_fq_ridx + 1)%RX_FRAME_QUEUE_SIZE;
	
	return p;
}

/*
 * lwip_frame_queue_RX_AVAILABLE
 * See header file for details
 */
bool lwip_frame_queue_RX_AVAILABLE(void)
{
	return (rx_fq_ridx != rx_fq_widx);
}
