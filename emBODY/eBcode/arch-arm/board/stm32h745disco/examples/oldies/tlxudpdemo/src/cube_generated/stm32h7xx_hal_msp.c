/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : stm32h7xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization
  *                      and de-Initialization codes.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
* @brief ETH MSP Initialization
* This function configures the hardware resources used in this example
* @param heth: ETH handle pointer
* @retval None
*/
void HAL_ETH_MspInit(ETH_HandleTypeDef* heth)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(heth->Instance==ETH)
  {
  /* USER CODE BEGIN ETH_MspInit 0 */

  /* USER CODE END ETH_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_ETH1MAC_CLK_ENABLE();
    __HAL_RCC_ETH1TX_CLK_ENABLE();
    __HAL_RCC_ETH1RX_CLK_ENABLE();

    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**ETH GPIO Configuration
    PG11     ------> ETH_TX_EN
    PE2     ------> ETH_TXD3
    PG12     ------> ETH_TXD1
    PG13     ------> ETH_TXD0
    PI10     ------> ETH_RX_ER
    PC1     ------> ETH_MDC
    PC2     ------> ETH_TXD2
    PC3     ------> ETH_TX_CLK
    PH2     ------> ETH_CRS
    PA2     ------> ETH_MDIO
    PA1     ------> ETH_RX_CLK
    PH3     ------> ETH_COL
    PA7     ------> ETH_RX_DV
    PC4     ------> ETH_RXD0
    PB1     ------> ETH_RXD3
    PC5     ------> ETH_RXD1
    PB0     ------> ETH_RXD2
    */
    GPIO_InitStruct.Pin = MII_TX_EN_Pin|GPIO_PIN_12|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MII_TXD3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(MII_TXD3_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MII_MDC_Pin|MII_TXD2_Pin|MII_TX_CLK_Pin|MII_RXD0_Pin
                          |MII_RXD1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MII_MDIO_Pin|MII_RX_CLK_Pin|MII_RX_DV_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MII_RXD3_Pin|MII_RXD2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* ETH interrupt Init */
    HAL_NVIC_SetPriority(ETH_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ETH_IRQn);
  /* USER CODE BEGIN ETH_MspInit 1 */

  /* USER CODE END ETH_MspInit 1 */
  }

}

/**
* @brief ETH MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param heth: ETH handle pointer
* @retval None
*/
void HAL_ETH_MspDeInit(ETH_HandleTypeDef* heth)
{
  if(heth->Instance==ETH)
  {
  /* USER CODE BEGIN ETH_MspDeInit 0 */

  /* USER CODE END ETH_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ETH1MAC_CLK_DISABLE();
    __HAL_RCC_ETH1TX_CLK_DISABLE();
    __HAL_RCC_ETH1RX_CLK_DISABLE();

    /**ETH GPIO Configuration
    PG11     ------> ETH_TX_EN
    PE2     ------> ETH_TXD3
    PG12     ------> ETH_TXD1
    PG13     ------> ETH_TXD0
    PI10     ------> ETH_RX_ER
    PC1     ------> ETH_MDC
    PC2     ------> ETH_TXD2
    PC3     ------> ETH_TX_CLK
    PH2     ------> ETH_CRS
    PA2     ------> ETH_MDIO
    PA1     ------> ETH_RX_CLK
    PH3     ------> ETH_COL
    PA7     ------> ETH_RX_DV
    PC4     ------> ETH_RXD0
    PB1     ------> ETH_RXD3
    PC5     ------> ETH_RXD1
    PB0     ------> ETH_RXD2
    */
    HAL_GPIO_DeInit(GPIOG, MII_TX_EN_Pin|GPIO_PIN_12|GPIO_PIN_13);

    HAL_GPIO_DeInit(MII_TXD3_GPIO_Port, MII_TXD3_Pin);

    HAL_GPIO_DeInit(GPIOI, GPIO_PIN_10);

    HAL_GPIO_DeInit(GPIOC, MII_MDC_Pin|MII_TXD2_Pin|MII_TX_CLK_Pin|MII_RXD0_Pin
                          |MII_RXD1_Pin);

    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_2|GPIO_PIN_3);

    HAL_GPIO_DeInit(GPIOA, MII_MDIO_Pin|MII_RX_CLK_Pin|MII_RX_DV_Pin);

    HAL_GPIO_DeInit(GPIOB, MII_RXD3_Pin|MII_RXD2_Pin);

    /* ETH interrupt DeInit */
    HAL_NVIC_DisableIRQ(ETH_IRQn);
  /* USER CODE BEGIN ETH_MspDeInit 1 */

  /* USER CODE END ETH_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
