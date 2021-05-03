/**
 * @file ipal_hal_eth_stm32h7.h
 * 
 * IPAL HAL ETH
 */
#ifndef IPAL_HAL_ETH_STM32H7_H
#define IPAL_HAL_ETH_STM32H7_H

#include "ipal.h"
 
/* ============================================================================================
 * FUNCTION PROTOTYPES
 * Mounted in ipal_cfg_t
 * ============================================================================================ */
	
/** 
 * Actual implementation of hal_eth_init, mounted on .hal_eth_init field
 * in ipal_cfg.
 * @param cfg  
 */
ipal_result_t ipal_hal_eth_stm32h7_init(void *cfg);     
	
/** 
 *
 */
ipal_result_t ipal_hal_eth_stm32h7_enable(void);
	
/** 
 *
 */
ipal_result_t ipal_hal_eth_stm32h7_disable(void);
	
/** 
 *
 */
ipal_result_t ipal_hal_eth_stm32h7_sendframe(void *frame); 

/* -------------------------------------------------------------------------------------------
 * AUXILIARY FUNCTIONS
 * Mounted as external functions in ipal_hal_eth_aux_function
 * ------------------------------------------------------------------------------------------- */

/** 
 *
 */
uint16_t ipal_hal_eth_stm32h7_get_frame_size(void);
	
/**
 *
 */
void ipal_hal_eth_stm32h7_get_frame(uint16_t length, uint8_t* frame);

/**
 *
 */
void ipal_signal_rx_frame(void);

/**
 *
 */
void ipal_signal_tx_frame(void);


#endif
