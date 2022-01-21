/**
  ******************************************************************************
  * @file    dma.c
  * @brief   This file provides code for the configuration
  *          of all the requested memory to memory DMA transfers.
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

/* Includes ------------------------------------------------------------------*/
#include "dma.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure DMA                                                              */
/*----------------------------------------------------------------------------*/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Enable DMA controller clock
  */
void MX_DMA_Init(void)
{

  if (ResMgr_Request(RESMGR_ID_DMA1, RESMGR_FLAGS_ACCESS_NORMAL | \
                  RESMGR_FLAGS_CPU1 , 0, NULL) != RESMGR_OK)
  {
    /* USER CODE BEGIN RESMGR_UTILITY_DMA1 */
    Error_Handler();
    /* USER CODE END RESMGR_UTILITY_DMA1 */
  }
  if (ResMgr_Request(RESMGR_ID_DMA1_Stream2, RESMGR_FLAGS_ACCESS_NORMAL | \
                  RESMGR_FLAGS_CPU1 , 0, NULL) != RESMGR_OK)
  {
    /* USER CODE BEGIN RESMGR_UTILITY_DMA1_Stream2 */
    Error_Handler();
    /* USER CODE END RESMGR_UTILITY_DMA1_Stream2 */
  }
  if (ResMgr_Request(RESMGR_ID_DMA1_Stream3, RESMGR_FLAGS_ACCESS_NORMAL | \
                  RESMGR_FLAGS_CPU1 , 0, NULL) != RESMGR_OK)
  {
    /* USER CODE BEGIN RESMGR_UTILITY_DMA1_Stream3 */
    Error_Handler();
    /* USER CODE END RESMGR_UTILITY_DMA1_Stream3 */
  }
  if (ResMgr_Request(RESMGR_ID_DMA2, RESMGR_FLAGS_ACCESS_NORMAL | \
                  RESMGR_FLAGS_CPU1 , 0, NULL) != RESMGR_OK)
  {
    /* USER CODE BEGIN RESMGR_UTILITY_DMA2 */
    Error_Handler();
    /* USER CODE END RESMGR_UTILITY_DMA2 */
  }
  if (ResMgr_Request(RESMGR_ID_DMAMUX1, RESMGR_FLAGS_ACCESS_NORMAL | \
                  RESMGR_FLAGS_CPU1 , 0, NULL) != RESMGR_OK)
  {
    /* USER CODE BEGIN RESMGR_UTILITY_DMAMUX1 */
    Error_Handler();
    /* USER CODE END RESMGR_UTILITY_DMAMUX1 */
  }
  if (ResMgr_Request(RESMGR_ID_DMAMUX1_Channel2, RESMGR_FLAGS_ACCESS_NORMAL | \
                  RESMGR_FLAGS_CPU1 , 0, NULL) != RESMGR_OK)
  {
    /* USER CODE BEGIN RESMGR_UTILITY_DMAMUX1_Channel2 */
    Error_Handler();
    /* USER CODE END RESMGR_UTILITY_DMAMUX1_Channel2 */
  }
  if (ResMgr_Request(RESMGR_ID_DMAMUX1_Channel3, RESMGR_FLAGS_ACCESS_NORMAL | \
                  RESMGR_FLAGS_CPU1 , 0, NULL) != RESMGR_OK)
  {
    /* USER CODE BEGIN RESMGR_UTILITY_DMAMUX1_Channel3 */
    Error_Handler();
    /* USER CODE END RESMGR_UTILITY_DMAMUX1_Channel3 */
  }

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
