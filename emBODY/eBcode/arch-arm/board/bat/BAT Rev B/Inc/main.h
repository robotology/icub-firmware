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
#define PC13_Pin GPIO_PIN_13
#define PC13_GPIO_Port GPIOC
#define PC14_Pin GPIO_PIN_14
#define PC14_GPIO_Port GPIOC
#define PC15_Pin GPIO_PIN_15
#define PC15_GPIO_Port GPIOC
#define BATTERY_voltage_Pin GPIO_PIN_0
#define BATTERY_voltage_GPIO_Port GPIOC
#define V12board_voltage_Pin GPIO_PIN_1
#define V12board_voltage_GPIO_Port GPIOC
#define V12motor_voltage_Pin GPIO_PIN_2
#define V12motor_voltage_GPIO_Port GPIOC
#define V12board_current_Pin GPIO_PIN_3
#define V12board_current_GPIO_Port GPIOC
#define VINPUT_voltage_Pin GPIO_PIN_0
#define VINPUT_voltage_GPIO_Port GPIOA
#define EXTPS_voltage_Pin GPIO_PIN_1
#define EXTPS_voltage_GPIO_Port GPIOA
#define V48motor_PG_Pin GPIO_PIN_2
#define V48motor_PG_GPIO_Port GPIOA
#define V48motor_FLT_Pin GPIO_PIN_3
#define V48motor_FLT_GPIO_Port GPIOA
#define V12motor_current_Pin GPIO_PIN_4
#define V12motor_current_GPIO_Port GPIOC
#define HSM_current_Pin GPIO_PIN_5
#define HSM_current_GPIO_Port GPIOC
#define GPIO1_Pin GPIO_PIN_0
#define GPIO1_GPIO_Port GPIOB
#define GPIO1_EXTI_IRQn EXTI0_IRQn
#define GPIO2_Pin GPIO_PIN_1
#define GPIO2_GPIO_Port GPIOB
#define GPIO2_EXTI_IRQn EXTI1_IRQn
#define CPU_Pin GPIO_PIN_7
#define CPU_GPIO_Port GPIOE
#define CPU_EXTI_IRQn EXTI9_5_IRQn
#define CPU_Green_Pin GPIO_PIN_8
#define CPU_Green_GPIO_Port GPIOE
#define CPU_Red_Pin GPIO_PIN_9
#define CPU_Red_GPIO_Port GPIOE
#define MOTORS_Pin GPIO_PIN_10
#define MOTORS_GPIO_Port GPIOE
#define MOTORS_EXTI_IRQn EXTI15_10_IRQn
#define MOTORS_Green_Pin GPIO_PIN_11
#define MOTORS_Green_GPIO_Port GPIOE
#define MOTORS_Red_Pin GPIO_PIN_12
#define MOTORS_Red_GPIO_Port GPIOE
#define HSM_nFLT_Pin GPIO_PIN_13
#define HSM_nFLT_GPIO_Port GPIOE
#define HSM_nFLT_EXTI_IRQn EXTI15_10_IRQn
#define HSM_nPG_Pin GPIO_PIN_14
#define HSM_nPG_GPIO_Port GPIOE
#define HSM_nPG_EXTI_IRQn EXTI15_10_IRQn
#define LED_BT_Pin GPIO_PIN_13
#define LED_BT_GPIO_Port GPIOB
#define CAN1_LED_Pin GPIO_PIN_14
#define CAN1_LED_GPIO_Port GPIOB
#define HSM_EN_Pin GPIO_PIN_8
#define HSM_EN_GPIO_Port GPIOD
#define V12board_EN_Pin GPIO_PIN_9
#define V12board_EN_GPIO_Port GPIOD
#define V12motor_EN_Pin GPIO_PIN_10
#define V12motor_EN_GPIO_Port GPIOD
#define LED0_BATTERY_Pin GPIO_PIN_11
#define LED0_BATTERY_GPIO_Port GPIOD
#define LED1_BATTERY_Pin GPIO_PIN_12
#define LED1_BATTERY_GPIO_Port GPIOD
#define LED2_BATTERY_Pin GPIO_PIN_13
#define LED2_BATTERY_GPIO_Port GPIOD
#define LED3_BATTERY_Pin GPIO_PIN_14
#define LED3_BATTERY_GPIO_Port GPIOD
#define LED4_BATTERY_Pin GPIO_PIN_15
#define LED4_BATTERY_GPIO_Port GPIOD
#define FAULT_ICUB_Pin GPIO_PIN_6
#define FAULT_ICUB_GPIO_Port GPIOC
#define FAULT_ICUB_EXTI_IRQn EXTI9_5_IRQn
#define FAULT_REM_Pin GPIO_PIN_7
#define FAULT_REM_GPIO_Port GPIOC
#define FAULT_GPIO_Pin GPIO_PIN_8
#define FAULT_GPIO_GPIO_Port GPIOC
#define FAULT_GPIO_EXTI_IRQn EXTI9_5_IRQn
#define JACK_Pin GPIO_PIN_9
#define JACK_GPIO_Port GPIOC
#define JACK_EXTI_IRQn EXTI9_5_IRQn
#define RST_DSPL_Pin GPIO_PIN_8
#define RST_DSPL_GPIO_Port GPIOA
#define TX_DSPL_Pin GPIO_PIN_9
#define TX_DSPL_GPIO_Port GPIOA
#define RX_DSPL_Pin GPIO_PIN_10
#define RX_DSPL_GPIO_Port GPIOA
#define I2C1_WP_Pin GPIO_PIN_12
#define I2C1_WP_GPIO_Port GPIOA
#define PC12_Pin GPIO_PIN_12
#define PC12_GPIO_Port GPIOC
#define BT_TX_Pin GPIO_PIN_5
#define BT_TX_GPIO_Port GPIOD
#define BT_RX_Pin GPIO_PIN_6
#define BT_RX_GPIO_Port GPIOD
#define V5_EN_Pin GPIO_PIN_7
#define V5_EN_GPIO_Port GPIOD
#define CAN1_SHDN_Pin GPIO_PIN_5
#define CAN1_SHDN_GPIO_Port GPIOB
#define BT_CONFIG_Pin GPIO_PIN_0
#define BT_CONFIG_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
