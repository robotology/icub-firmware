/**
 * @file ipal_hal_eth_stm32h7.h
 * 
 * IPAL HAL ETH
 */
#ifndef __IPAL_HAL_ETH_STM32H7_H
#define __IPAL_HAL_ETH_STM32H7_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ipal.h"
// even if only ipal_base.h would be enough
 
/* ============================================================================================
 * FUNCTION PROTOTYPES
 * Mounted in ipal_cfg_t
 * ============================================================================================ */
	
/** 
 * Actual implementation of hal_eth_init, mounted on .hal_eth_init field
 * in ipal_cfg.
 * @param cfg  
 */
ipal_result_t ipal_hal_eth_stm32h7_init(ipal_hal_eth_cfg_t *cfg);     
	
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
ipal_result_t ipal_hal_eth_stm32h7_sendframe(ipal_hal_eth_frame_t *frame); 


/** 
 *
 */
uint16_t ipal_hal_eth_stm32h7_get_frame_size(void);
	
/**
 *
 */
void ipal_hal_eth_stm32h7_get_frame(uint16_t length, uint8_t* frame);


#ifdef __cplusplus
}
#endif

#endif //__IPAL_HAL_ETH_STM32H7_H

// eof

