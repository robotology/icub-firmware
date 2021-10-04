/**
 * @file ethernetif_IPAL.h
 *
 * This is the glue between LWIP low level hardware dependent interface and IPAL libary.
 * To access the low level, LWIP invokes hardware specific functions.
 * In this case these functions just call external functions of IPAL library to access the hardware.
 *
 * So IPAL external functions 			<<<< depends on Hardware
 * LWIP low level access functions 	<<<< uses generic IPAL external functions
 */
#ifndef ETHERNETIF_IPAL_H
#define ETHERNETIF_IPAL_H

/* LWIP specific includes */
#include "lwip/pbuf.h"

#include "ipal.h"
#include "ethernetif_use_ipal.h"
#include "ipal_hal_eth_types.h"

/* ==============================================================================================
 * FUNCTION PROTOTYPES
 * =============================================================================================== */

/* -----------------------------------------------------------------------------------------------
 * DRIVER FUNCTIONS
 * ----------------------------------------------------------------------------------------------- */
 
/**
 * IPAL glue for low level initialization
 * Initializes MAC and PHY
 */
void ethernetif_IPAL_low_level_init(void);

/**
 * IPAL glue for low level output (send frame)
 */
ipal_result_t ethernetif_IPAL_low_level_output(ipal_hal_eth_frame_t *frame);

/* -----------------------------------------------------------------------------------------------
 * CALLBACKS
 * ----------------------------------------------------------------------------------------------- */
 
/**
 * This function is the "equivalent" of ethernetif_input provided by LWIP stack.
 * On the other hand, it is mounted as callback as movetohigherlevel
 */
#if (0)
	uint32_t ethernetif_IPAL_ethernetif_input(uint8_t *frame, uint32_t len);
#else
	void ethernetif_IPAL_ethernetif_input(ipal_hal_eth_frame_t *frame);
#endif

#endif
