/**
 * @file lwip_tx_frame_queue_handler.c
 */
#include "lwip_tx_frame_queue_handler.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <stdint.h>

/* ================================================================================
 * STATIC VARIABLES
 * ================================================================================ */
 
/* Write index */
static uint8_t tx_fq_widx = 0;

/* Read index */
static uint8_t tx_fq_ridx = 0;

/* The TX frame queue */
static  tx_fq_element_t tx_frame_queue[TX_FRAME_QUEUE_SIZE];

/* ================================================================================
 * PUBLIC FUNCTIONS
 * ================================================================================ */
 
/*
 * 
 */
void lwip_TX_frame_queue_init(void)
{
}

/*
 * 
 */
void lwip_TX_frame_enqueue(struct pbuf *p)
{
	tx_fq_element_t *el = &tx_frame_queue[tx_fq_widx];
	
	el->p = p;
	el->tx_frame.length = p->len;
	el->tx_frame.payload = p->payload;
	
	tx_fq_widx = (tx_fq_widx + 1)%TX_FRAME_QUEUE_SIZE;
}

/*
 * 
 */
tx_fq_element_t *lwip_TX_frame_dequeue(void)
{
	tx_fq_element_t *el = &tx_frame_queue[tx_fq_ridx];
	
	tx_fq_ridx = (tx_fq_ridx + 1)%TX_FRAME_QUEUE_SIZE;
	
	return el;
}

/*
 * 
 */
bool lwip_TX_frame_available(void)
{
	return (tx_fq_widx != tx_fq_ridx);
}

/**
 * 
 */
void lwip_TX_queue_protect(void)
{
	sys_arch_protect();
}


/**
 * 
 */
void lwip_TX_queue_unprotect(void)
{
	sys_arch_unprotect(0);
}


