/** 
 * @file ipal_frame_queue_handler.c
 *
 * IPAL Frame Queue Handler
 */
#include "ipal_frame_queue.h"
#include <stddef.h>
#include <stdint.h>

/* =====================================================================================
 * MACROS
 * ===================================================================================== */

/* Max MTU Size */
#define MAX_FRAME_SIZE 	(1514)

/* Max number of frame in queue */
#define MAX_RX_N_FRAMES	(5)

/* =====================================================================================
 * STATIC VARIABLES
 * ===================================================================================== */

/* The rx frame pool */
static uint8_t rx_frames_pool[MAX_FRAME_SIZE * MAX_RX_N_FRAMES];

/* The rx frame queue */
static ipal_eth_frame_t rx_frame_queue[MAX_RX_N_FRAMES];

/* New frame to write pointer */
static uint8_t w_frame_idx;

/* New frame to read pointer */
static uint8_t r_frame_idx; 

/* =====================================================================================
 * PRIVATE SYMBOLS
 * ===================================================================================== */
 
/*
 * Initializes RX Frame Queue
 */
static void rx_frame_queue_init(void)
{
	uint8_t i;
	
	/* Set payload pointers */
	for (i=0; i<MAX_RX_N_FRAMES; i++)
	{
		rx_frame_queue[i].length = 0;
		rx_frame_queue[i].payload = &rx_frames_pool[i * MAX_FRAME_SIZE];
	}
	
	/* Set read and write pointers */
	w_frame_idx = 0;
	r_frame_idx = 0;
}

/*
 * Returns new room for storing rx frame queue
 */
static ipal_eth_frame_t *rx_frame_queue_alloc(void)
{
	ipal_eth_frame_t *frame = &rx_frame_queue[w_frame_idx];
	
	w_frame_idx = (w_frame_idx + 1)%5;
	
	return frame;
}

/*
 * Gives the reference to a newly available xr frame queue 
 */
static ipal_eth_frame_t *rx_frame_queue_next(void)
{
	ipal_eth_frame_t *frame = NULL;
	
	if (w_frame_idx != r_frame_idx)
	{
		frame = &rx_frame_queue[r_frame_idx];
		
		r_frame_idx = (r_frame_idx + 1)%5;
	}
	
	return frame;	
}

/* =====================================================================================
 * GLOBAL SYMBOLS
 * ===================================================================================== */

/**
 * IPAL rx frame queue
 */
const ipal_frame_queue_t ipal_RX_frame_queue = 
{
	/* Initialize Frame Queue */
	.init = rx_frame_queue_init,
	
	/* Alloc the next frame available */
	.alloc = rx_frame_queue_alloc,
	
	/* If a new frame is available to be read, returns a reference to it */
	.next = rx_frame_queue_next
};
