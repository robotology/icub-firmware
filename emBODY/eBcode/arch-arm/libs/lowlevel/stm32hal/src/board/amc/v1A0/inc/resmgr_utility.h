/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : resmgr_utility.h
  * Description        : This file provides code for the configuration
  *                      of the resmgr_utility instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __resmgr_utility_H
#define __resmgr_utility_H
#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "res_mgr.h"
#include "main.h"

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/* Global variables ---------------------------------------------------------*/

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */

/* RESMGR_UTILITY init function */
void MX_RESMGR_UTILITY_Init(void);

/* USER CODE BEGIN 2 */
/* USER CODE END 2 */

/* Resource Manager send message function */
void MX_RESMGR_UTILITY_SendMsg(uint32_t id, uint32_t msg);

/* Resource Manager callback function */
void MX_RESMGR_UTILITY_Callback(uint32_t id, uint32_t msg);

#ifdef __cplusplus
}
#endif
#endif /*__resmgr_utility_H */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
