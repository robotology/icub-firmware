/** 
 * @file ipal_hal_eth_lqip_aux_functions.h
 *
 * This file contains externalization of some functions specifically needed by LWIP to access the
 * low level driver (hardware dependent). 
 */
 
#ifndef IPAL_HAL_ETH_AUX_FUNCTIONS_H
#define IPAL_HAL_ETH_AUX_FUNCTIONS_H

#include <stdint.h>

/* =============================================================================================
 * TYPE DEFINTITIONS
 * ============================================================================================= */

//typedef struct
//{
//	/* Get frame size */
//	uint16_t (*get_frame_size)(void);
//	
//	/* Get frame */
//	void (*get_frame)(uint16_t length, uint8_t* frame);
//	
//	/* Signal to higher layer - This is the last function called by movetohigherlayer. 
//	   For instance, it can be used to send a signal to a waiting thread.
//	 */
//	void (*signal_rx_frame)(void);
//	
//	/* Signal to higher layer - This is the last function called by low_level_output 
//	   For instance, it can be used to send a signal to a waiting thread.
//	 */
//	void (*signal_tx_frame)(void);
//	
//} ipal_hal_eth_aux_functions_t;


/* =============================================================================================
 * SYMBOLS EXTERNALIZATION
 * ============================================================================================= */

/* More driver functions */
//extern ipal_hal_eth_aux_functions_t ipal_hal_eth_aux_functions;
 
#endif


