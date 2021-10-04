/** 
 * @file eth_user_callbacks.c
 *
 * ETH User callbacks
 */
#include "stm32h7xx_hal_conf.h"
#include "stm32h7xx_hal_eth.h"
#include "eth_user_callbacks.h"
#include <stdint.h>

/* ==================================================================================================
 * EXTERNAL SYMBOLS
 * ================================================================================================== */

/* HAL callback: Tx transfer completed (frame sent) */
extern void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth); 

/* HAL callback: Rx transfer completed (frame received) */
extern void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth); 

/* HAL callback: Power management callback (Magic/WOL packet received) */
extern void HAL_ETH_PMTCallback(ETH_HandleTypeDef *heth);

/* ==================================================================================================
 * PRIVATE FUNCTIONS
 * ================================================================================================== */

#if (USE_HAL_ETH_REGISTER_CALLBACKS == 1)

	/**
	 * ETH_TX_Complete_cb
	 */
	static void usr_ETH_TX_Complete_cb(ETH_HandleTypeDef * heth)
	{
		HAL_ETH_TxCpltCallback(heth); 
	}

	/**
	 * ETH_RX_Complete_cb
	 */
	static void usr_ETH_RX_Complete_cb(ETH_HandleTypeDef * heth)
	{
		HAL_ETH_RxCpltCallback(heth); 
	}

	/**
	 * ETH_DMA_Error_cb
	 */
	static void usr_ETH_DMA_Error_cb(ETH_HandleTypeDef * heth)
	{
		HAL_ETH_DMAErrorCallback(heth);
	}

	/**
	 * ETH_MAC_Error_cb
	 */
	static void usr_ETH_MAC_Error_cb(ETH_HandleTypeDef * heth)
	{
		HAL_ETH_MACErrorCallback(heth);
	}

	/**
	 * Power Management Callback
	 */
	static void usr_ETH_PMT_cb(ETH_HandleTypeDef * heth)
	{
		HAL_ETH_PMTCallback(heth);
	}

	/**
	 * EEE callback
	 */
	static void usr_ETH_EEE_cb(ETH_HandleTypeDef * heth)
	{
		HAL_ETH_EEECallback(heth);
	}

	/**
	 * ETH_Wakeup_cb
	 */
	static void usr_ETH_Wakeup_cb(ETH_HandleTypeDef * heth)
	{
		HAL_ETH_WakeUpCallback(heth);
	}
	
#endif

/* ==================================================================================================
 * PUBLIC FUNCTIONS
 * ================================================================================================== */
/* 
 * usr_ETH_register_callbacks
 * See header file for details
 */
void usr_ETH_register_callbacks(ETH_HandleTypeDef *heth)
{
	#if (USE_HAL_ETH_REGISTER_CALLBACKS == 1)	
		HAL_ETH_RegisterCallback(heth, HAL_ETH_TX_COMPLETE_CB_ID, usr_ETH_TX_Complete_cb);
		HAL_ETH_RegisterCallback(heth, HAL_ETH_RX_COMPLETE_CB_ID, usr_ETH_RX_Complete_cb);
		HAL_ETH_RegisterCallback(heth, HAL_ETH_DMA_ERROR_CB_ID, usr_ETH_DMA_Error_cb);
		HAL_ETH_RegisterCallback(heth, HAL_ETH_MAC_ERROR_CB_ID, usr_ETH_MAC_Error_cb);
		HAL_ETH_RegisterCallback(heth, HAL_ETH_PMT_CB_ID, usr_ETH_PMT_cb);
		HAL_ETH_RegisterCallback(heth, HAL_ETH_EEE_CB_ID, usr_ETH_EEE_cb);
		HAL_ETH_RegisterCallback(heth, HAL_ETH_WAKEUP_CB_ID, usr_ETH_Wakeup_cb);
	#endif
}
