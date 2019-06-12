/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include "stm32l4xx_hal.h"

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
