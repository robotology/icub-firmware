/**
  ******************************************************************************
  * File Name          : SDMMC.c
  * Description        : This file provides code for the configuration
  *                      of the SDMMC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "sdmmc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

MMC_HandleTypeDef hmmc1;

/* SDMMC1 init function */

void MX_SDMMC1_MMC_Init(void)
{

  hmmc1.Instance = SDMMC1;
  hmmc1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hmmc1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hmmc1.Init.BusWide = SDMMC_BUS_WIDE_8B;
  hmmc1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hmmc1.Init.ClockDiv = 0;
  hmmc1.Init.TranceiverPresent = SDMMC_TRANSCEIVER_NOT_PRESENT;
  if (HAL_MMC_Init(&hmmc1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_MMC_MspInit(MMC_HandleTypeDef* mmcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(mmcHandle->Instance==SDMMC1)
  {
  /* USER CODE BEGIN SDMMC1_MspInit 0 */

  /* USER CODE END SDMMC1_MspInit 0 */
    /* SDMMC1 clock enable */
    __HAL_RCC_SDMMC1_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SDMMC1 GPIO Configuration
    PC10     ------> SDMMC1_D2
    PC11     ------> SDMMC1_D3
    PC12     ------> SDMMC1_CK
    PB9     ------> SDMMC1_D5
    PB8     ------> SDMMC1_D4
    PD2     ------> SDMMC1_CMD
    PC8     ------> SDMMC1_D0
    PC9     ------> SDMMC1_D1
    PC7     ------> SDMMC1_D7
    PC6     ------> SDMMC1_D6
    */
    GPIO_InitStruct.Pin = SDIO1_D2_Pin|SDIO1_D3_Pin|SDIO1_CK_Pin|SDIO1_D0_Pin
                          |SDIO1_D1_Pin|SDIO1_D7_Pin|SDIO1_D6_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SDIO1_D5_Pin|SDIO1_D4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SDIO1_CMD_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(SDIO1_CMD_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN SDMMC1_MspInit 1 */

  /* USER CODE END SDMMC1_MspInit 1 */
  }
}

void HAL_MMC_MspDeInit(MMC_HandleTypeDef* mmcHandle)
{

  if(mmcHandle->Instance==SDMMC1)
  {
  /* USER CODE BEGIN SDMMC1_MspDeInit 0 */

  /* USER CODE END SDMMC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDMMC1_CLK_DISABLE();

    /**SDMMC1 GPIO Configuration
    PC10     ------> SDMMC1_D2
    PC11     ------> SDMMC1_D3
    PC12     ------> SDMMC1_CK
    PB9     ------> SDMMC1_D5
    PB8     ------> SDMMC1_D4
    PD2     ------> SDMMC1_CMD
    PC8     ------> SDMMC1_D0
    PC9     ------> SDMMC1_D1
    PC7     ------> SDMMC1_D7
    PC6     ------> SDMMC1_D6
    */
    HAL_GPIO_DeInit(GPIOC, SDIO1_D2_Pin|SDIO1_D3_Pin|SDIO1_CK_Pin|SDIO1_D0_Pin
                          |SDIO1_D1_Pin|SDIO1_D7_Pin|SDIO1_D6_Pin);

    HAL_GPIO_DeInit(GPIOB, SDIO1_D5_Pin|SDIO1_D4_Pin);

    HAL_GPIO_DeInit(SDIO1_CMD_GPIO_Port, SDIO1_CMD_Pin);

  /* USER CODE BEGIN SDMMC1_MspDeInit 1 */

  /* USER CODE END SDMMC1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
