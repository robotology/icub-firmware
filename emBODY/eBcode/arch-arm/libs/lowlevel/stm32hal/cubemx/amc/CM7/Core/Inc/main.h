/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "resmgr_utility.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CAN2_TXD_Pin GPIO_PIN_6
#define CAN2_TXD_GPIO_Port GPIOB
#define JTDO_SWO_Pin GPIO_PIN_3
#define JTDO_SWO_GPIO_Port GPIOB
#define CAN1_TXD_Pin GPIO_PIN_1
#define CAN1_TXD_GPIO_Port GPIOD
#define JTDI_Pin GPIO_PIN_15
#define JTDI_GPIO_Port GPIOA
#define JTCK_SWCK_Pin GPIO_PIN_14
#define JTCK_SWCK_GPIO_Port GPIOA
#define TRACED1_Pin GPIO_PIN_4
#define TRACED1_GPIO_Port GPIOE
#define TRACED0_Pin GPIO_PIN_3
#define TRACED0_GPIO_Port GPIOE
#define ETH_nPME_Pin GPIO_PIN_0
#define ETH_nPME_GPIO_Port GPIOE
#define EE_MISO_Pin GPIO_PIN_4
#define EE_MISO_GPIO_Port GPIOB
#define EE_SCLK_Pin GPIO_PIN_13
#define EE_SCLK_GPIO_Port GPIOG
#define CAN2_STBY_Pin GPIO_PIN_5
#define CAN2_STBY_GPIO_Port GPIOD
#define TRACED2_Pin GPIO_PIN_2
#define TRACED2_GPIO_Port GPIOD
#define TRACED3_Pin GPIO_PIN_12
#define TRACED3_GPIO_Port GPIOC
#define CAN1_RXD_Pin GPIO_PIN_14
#define CAN1_RXD_GPIO_Port GPIOH
#define JTMS_SWDIO_Pin GPIO_PIN_13
#define JTMS_SWDIO_GPIO_Port GPIOA
#define TP2_Pin GPIO_PIN_8
#define TP2_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define TRACECLK_Pin GPIO_PIN_2
#define TRACECLK_GPIO_Port GPIOE
#define ETH_nIRQ_Pin GPIO_PIN_1
#define ETH_nIRQ_GPIO_Port GPIOE
#define CAN2_RXD_Pin GPIO_PIN_5
#define CAN2_RXD_GPIO_Port GPIOB
#define EE_MOSI_Pin GPIO_PIN_14
#define EE_MOSI_GPIO_Port GPIOG
#define CAN1_STBY_Pin GPIO_PIN_4
#define CAN1_STBY_GPIO_Port GPIOD
#define CAN_SHDN_Pin GPIO_PIN_0
#define CAN_SHDN_GPIO_Port GPIOD
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define OSS32_OUT_Pin GPIO_PIN_15
#define OSS32_OUT_GPIO_Port GPIOC
#define OS32_IN_Pin GPIO_PIN_14
#define OS32_IN_GPIO_Port GPIOC
#define EE_nSEL_Pin GPIO_PIN_8
#define EE_nSEL_GPIO_Port GPIOG
#define ETH_nSEL_Pin GPIO_PIN_6
#define ETH_nSEL_GPIO_Port GPIOF
#define OSC_IN_Pin GPIO_PIN_0
#define OSC_IN_GPIO_Port GPIOH
#define ETH_MISO_Pin GPIO_PIN_8
#define ETH_MISO_GPIO_Port GPIOF
#define PWR_aVCORE_Pin GPIO_PIN_7
#define PWR_aVCORE_GPIO_Port GPIOF
#define PWR_aVAUX_Pin GPIO_PIN_9
#define PWR_aVAUX_GPIO_Port GPIOF
#define ETH_nRST_Pin GPIO_PIN_13
#define ETH_nRST_GPIO_Port GPIOB
#define PWR_aCIN_Pin GPIO_PIN_2
#define PWR_aCIN_GPIO_Port GPIOC
#define PWR_aVIN_Pin GPIO_PIN_3
#define PWR_aVIN_GPIO_Port GPIOC
#define ETH_MOSI_Pin GPIO_PIN_11
#define ETH_MOSI_GPIO_Port GPIOF
#define EE_nHOLD_Pin GPIO_PIN_13
#define EE_nHOLD_GPIO_Port GPIOF
#define EE_nWP_Pin GPIO_PIN_12
#define EE_nWP_GPIO_Port GPIOF
#define ETH_SCLK_Pin GPIO_PIN_6
#define ETH_SCLK_GPIO_Port GPIOH
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
