/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
/* Includes ------------------------------------------------------------------*/
// IIT-EXT
#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #include "stm32l4xx_hal.h"
#endif

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
#define BNO055_BOOT_Pin GPIO_PIN_13
#define BNO055_BOOT_GPIO_Port GPIOC
#define BNO055_INT_Pin GPIO_PIN_10
#define BNO055_INT_GPIO_Port GPIOC
#define BNO055_RESET_Pin GPIO_PIN_12
#define BNO055_RESET_GPIO_Port GPIOC
#define EN_OSC_Pin GPIO_PIN_11
#define EN_OSC_GPIO_Port GPIOA
#define SDA0_Pin GPIO_PIN_8
#define SDA0_GPIO_Port GPIOA
#define LED_RED_Pin GPIO_PIN_9
#define LED_RED_GPIO_Port GPIOC
#define ANALOG_0_Pin GPIO_PIN_0
#define ANALOG_0_GPIO_Port GPIOC
#define LED_BLUE_Pin GPIO_PIN_8
#define LED_BLUE_GPIO_Port GPIOC
#define SDA3_Pin GPIO_PIN_5
#define SDA3_GPIO_Port GPIOA
#define CAN1_SHDN_Pin GPIO_PIN_0
#define CAN1_SHDN_GPIO_Port GPIOB
#define SDA2_Pin GPIO_PIN_6
#define SDA2_GPIO_Port GPIOA
#define CAN1_STB_Pin GPIO_PIN_1
#define CAN1_STB_GPIO_Port GPIOB
#define PG_3V3_Pin GPIO_PIN_13
#define PG_3V3_GPIO_Port GPIOB
#define SCK0_Pin GPIO_PIN_4
#define SCK0_GPIO_Port GPIOA
#define SDA1_Pin GPIO_PIN_7
#define SDA1_GPIO_Port GPIOA
#define POWER_TSENSOR1_Pin GPIO_PIN_11
#define POWER_TSENSOR1_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
