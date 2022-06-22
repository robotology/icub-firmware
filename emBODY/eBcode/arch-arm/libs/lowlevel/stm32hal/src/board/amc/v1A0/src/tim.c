/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
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
#include "tim.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim17;

/* TIM6 init function */
void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 200;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 1000;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}
/* TIM17 init function */
void MX_TIM17_Init(void)
{

  /* USER CODE BEGIN TIM17_Init 0 */

  /* USER CODE END TIM17_Init 0 */

  /* USER CODE BEGIN TIM17_Init 1 */

  /* USER CODE END TIM17_Init 1 */
  htim17.Instance = TIM17;
  htim17.Init.Prescaler = 199;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 1000;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM17_Init 2 */

  /* USER CODE END TIM17_Init 2 */

}

//void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
//{

//  if(tim_baseHandle->Instance==TIM6)
//  {
//  /* USER CODE BEGIN TIM6_MspInit 0 */

//  /* USER CODE END TIM6_MspInit 0 */
//    /* TIM6 clock enable */
//    __HAL_RCC_TIM6_CLK_ENABLE();
//  /* USER CODE BEGIN TIM6_MspInit 1 */

//  /* USER CODE END TIM6_MspInit 1 */
//  }
//  else if(tim_baseHandle->Instance==TIM17)
//  {
//  /* USER CODE BEGIN TIM17_MspInit 0 */

//  /* USER CODE END TIM17_MspInit 0 */
//    /* TIM17 clock enable */
//    __HAL_RCC_TIM17_CLK_ENABLE();

//    /* TIM17 interrupt Init */
//    HAL_NVIC_SetPriority(TIM17_IRQn, 15, 0);
//    HAL_NVIC_EnableIRQ(TIM17_IRQn);
//  /* USER CODE BEGIN TIM17_MspInit 1 */

//  /* USER CODE END TIM17_MspInit 1 */
//  }
//}

//void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
//{

//  if(tim_baseHandle->Instance==TIM6)
//  {
//  /* USER CODE BEGIN TIM6_MspDeInit 0 */

//  /* USER CODE END TIM6_MspDeInit 0 */
//    /* Peripheral clock disable */
//    __HAL_RCC_TIM6_CLK_DISABLE();
//  /* USER CODE BEGIN TIM6_MspDeInit 1 */

//  /* USER CODE END TIM6_MspDeInit 1 */
//  }
//  else if(tim_baseHandle->Instance==TIM17)
//  {
//  /* USER CODE BEGIN TIM17_MspDeInit 0 */

//  /* USER CODE END TIM17_MspDeInit 0 */
//    /* Peripheral clock disable */
//    __HAL_RCC_TIM17_CLK_DISABLE();

//    /* TIM17 interrupt Deinit */
//    HAL_NVIC_DisableIRQ(TIM17_IRQn);
//  /* USER CODE BEGIN TIM17_MspDeInit 1 */

//  /* USER CODE END TIM17_MspDeInit 1 */
//  }
//}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
