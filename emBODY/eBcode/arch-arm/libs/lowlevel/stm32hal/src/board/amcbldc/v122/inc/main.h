/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
//#include "stm32g4xx_hal.h"
#include "stm32hal.h" // IIT-EXT

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
#define VCCOK_Pin GPIO_PIN_13
#define VCCOK_GPIO_Port GPIOC
#define VCCOK_EXTI_IRQn EXTI15_10_IRQn
#define VAUXOK_Pin GPIO_PIN_14
#define VAUXOK_GPIO_Port GPIOC
#define VAUXOK_EXTI_IRQn EXTI15_10_IRQn
#define VAUXEN_Pin GPIO_PIN_15
#define VAUXEN_GPIO_Port GPIOC
#define OSCIN_Pin GPIO_PIN_0
#define OSCIN_GPIO_Port GPIOF
#define OSCOUT_Pin GPIO_PIN_1
#define OSCOUT_GPIO_Port GPIOF
#define VSENS1_Pin GPIO_PIN_0
#define VSENS1_GPIO_Port GPIOA
#define ENCB_Pin GPIO_PIN_1
#define ENCB_GPIO_Port GPIOA
#define VSENS2_Pin GPIO_PIN_2
#define VSENS2_GPIO_Port GPIOA
#define VSENS3_Pin GPIO_PIN_3
#define VSENS3_GPIO_Port GPIOA
#define HALL2_Pin GPIO_PIN_4
#define HALL2_GPIO_Port GPIOA
#define ENCA_Pin GPIO_PIN_5
#define ENCA_GPIO_Port GPIOA
#define CSENS1_Pin GPIO_PIN_6
#define CSENS1_GPIO_Port GPIOA
#define CSENS2_Pin GPIO_PIN_7
#define CSENS2_GPIO_Port GPIOA
#define CSENS3_Pin GPIO_PIN_4
#define CSENS3_GPIO_Port GPIOC
#define HALL3_Pin GPIO_PIN_0
#define HALL3_GPIO_Port GPIOB
#define VINTST_Pin GPIO_PIN_1
#define VINTST_GPIO_Port GPIOB
#define nLED1_Pin GPIO_PIN_2
#define nLED1_GPIO_Port GPIOB
#define nMFAULT_Pin GPIO_PIN_10
#define nMFAULT_GPIO_Port GPIOB
#define CINTST_Pin GPIO_PIN_11
#define CINTST_GPIO_Port GPIOB
#define CANRX_Pin GPIO_PIN_12
#define CANRX_GPIO_Port GPIOB
#define CANTX_Pin GPIO_PIN_13
#define CANTX_GPIO_Port GPIOB
#define CANSTB_Pin GPIO_PIN_14
#define CANSTB_GPIO_Port GPIOB
#define nBREAK_Pin GPIO_PIN_15
#define nBREAK_GPIO_Port GPIOB
#define nBREAK_EXTI_IRQn EXTI15_10_IRQn
#define I2CSCL_Pin GPIO_PIN_6
#define I2CSCL_GPIO_Port GPIOC
#define PWM1_Pin GPIO_PIN_8
#define PWM1_GPIO_Port GPIOA
#define PWM2_Pin GPIO_PIN_9
#define PWM2_GPIO_Port GPIOA
#define PWM3_Pin GPIO_PIN_10
#define PWM3_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define ENCZ_Pin GPIO_PIN_15
#define ENCZ_GPIO_Port GPIOA
#define nMSLEEP_Pin GPIO_PIN_10
#define nMSLEEP_GPIO_Port GPIOC
#define nMRESET_Pin GPIO_PIN_11
#define nMRESET_GPIO_Port GPIOC
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define HALL1_Pin GPIO_PIN_4
#define HALL1_GPIO_Port GPIOB
#define nLED2_Pin GPIO_PIN_5
#define nLED2_GPIO_Port GPIOB
#define EN1_Pin GPIO_PIN_6
#define EN1_GPIO_Port GPIOB
#define I2CSDA_Pin GPIO_PIN_7
#define I2CSDA_GPIO_Port GPIOB
#define EN2_Pin GPIO_PIN_8
#define EN2_GPIO_Port GPIOB
#define EN3_Pin GPIO_PIN_9
#define EN3_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
