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
#define I2C1_SCL_Pin GPIO_PIN_8
#define I2C1_SCL_GPIO_Port GPIOB
#define JTDO_SWO_Pin GPIO_PIN_3
#define JTDO_SWO_GPIO_Port GPIOB
#define SPI1_SCLK_Pin GPIO_PIN_11
#define SPI1_SCLK_GPIO_Port GPIOG
#define SPI1_MISO_Pin GPIO_PIN_9
#define SPI1_MISO_GPIO_Port GPIOG
#define SPI2_SCLK_Pin GPIO_PIN_3
#define SPI2_SCLK_GPIO_Port GPIOD
#define JTDI_Pin GPIO_PIN_15
#define JTDI_GPIO_Port GPIOA
#define JTCK_SWCK_Pin GPIO_PIN_14
#define JTCK_SWCK_GPIO_Port GPIOA
#define TRACED1_Pin GPIO_PIN_4
#define TRACED1_GPIO_Port GPIOE
#define TRACED0_Pin GPIO_PIN_3
#define TRACED0_GPIO_Port GPIOE
#define SPI2_nSEL_Pin GPIO_PIN_9
#define SPI2_nSEL_GPIO_Port GPIOB
#define I2C1_SDA_Pin GPIO_PIN_7
#define I2C1_SDA_GPIO_Port GPIOB
#define SPI1_MOSI_Pin GPIO_PIN_7
#define SPI1_MOSI_GPIO_Port GPIOD
#define TRACED2_Pin GPIO_PIN_2
#define TRACED2_GPIO_Port GPIOD
#define TRACED3_Pin GPIO_PIN_12
#define TRACED3_GPIO_Port GPIOC
#define JTMS_SWDIO_Pin GPIO_PIN_13
#define JTMS_SWDIO_GPIO_Port GPIOA
#define TP2_Pin GPIO_PIN_8
#define TP2_GPIO_Port GPIOA
#define TP4_Pin GPIO_PIN_13
#define TP4_GPIO_Port GPIOC
#define TRACECLK_Pin GPIO_PIN_2
#define TRACECLK_GPIO_Port GPIOE
#define SPI1_nSEL_Pin GPIO_PIN_10
#define SPI1_nSEL_GPIO_Port GPIOG
#define SPI3_MISO_Pin GPIO_PIN_11
#define SPI3_MISO_GPIO_Port GPIOC
#define SPI3_SCLK_Pin GPIO_PIN_10
#define SPI3_SCLK_GPIO_Port GPIOC
#define nLED1_Pin GPIO_PIN_13
#define nLED1_GPIO_Port GPIOH
#define MOT_PWM3_Pin GPIO_PIN_10
#define MOT_PWM3_GPIO_Port GPIOA
#define OSS32_OUT_Pin GPIO_PIN_15
#define OSS32_OUT_GPIO_Port GPIOC
#define OS32_IN_Pin GPIO_PIN_14
#define OS32_IN_GPIO_Port GPIOC
#define SPI3_MOSI_Pin GPIO_PIN_6
#define SPI3_MOSI_GPIO_Port GPIOD
#define nLED2_Pin GPIO_PIN_15
#define nLED2_GPIO_Port GPIOH
#define MOT_PWM2_Pin GPIO_PIN_9
#define MOT_PWM2_GPIO_Port GPIOA
#define MOT_EN3_Pin GPIO_PIN_8
#define MOT_EN3_GPIO_Port GPIOC
#define MOT_EN2_Pin GPIO_PIN_7
#define MOT_EN2_GPIO_Port GPIOC
#define MOT_EN4_Pin GPIO_PIN_9
#define MOT_EN4_GPIO_Port GPIOC
#define MOT_EN1_Pin GPIO_PIN_6
#define MOT_EN1_GPIO_Port GPIOC
#define I2C2_SCL_Pin GPIO_PIN_1
#define I2C2_SCL_GPIO_Port GPIOF
#define I2C2_SDA_Pin GPIO_PIN_0
#define I2C2_SDA_GPIO_Port GPIOF
#define VER1_Pin GPIO_PIN_6
#define VER1_GPIO_Port GPIOG
#define VER0_Pin GPIO_PIN_5
#define VER0_GPIO_Port GPIOG
#define PWR_VCCOK_Pin GPIO_PIN_2
#define PWR_VCCOK_GPIO_Port GPIOF
#define VER2_Pin GPIO_PIN_7
#define VER2_GPIO_Port GPIOG
#define MOT_nFAULT_Pin GPIO_PIN_4
#define MOT_nFAULT_GPIO_Port GPIOG
#define I2C3_XEN_Pin GPIO_PIN_2
#define I2C3_XEN_GPIO_Port GPIOG
#define PWR_VAUXOK_Pin GPIO_PIN_4
#define PWR_VAUXOK_GPIO_Port GPIOF
#define PWR_VAUXEN_Pin GPIO_PIN_5
#define PWR_VAUXEN_GPIO_Port GPIOF
#define PWR_VCOREOK_Pin GPIO_PIN_3
#define PWR_VCOREOK_GPIO_Port GPIOF
#define I2C4_XEN_Pin GPIO_PIN_3
#define I2C4_XEN_GPIO_Port GPIOG
#define MOT_HALL3_Pin GPIO_PIN_14
#define MOT_HALL3_GPIO_Port GPIOD
#define MOT_HALL2_Pin GPIO_PIN_13
#define MOT_HALL2_GPIO_Port GPIOD
#define OSC_IN_Pin GPIO_PIN_0
#define OSC_IN_GPIO_Port GPIOH
#define MOT_HALL1_Pin GPIO_PIN_12
#define MOT_HALL1_GPIO_Port GPIOD
#define SPI2_nXEN_Pin GPIO_PIN_9
#define SPI2_nXEN_GPIO_Port GPIOD
#define SPI2_MOSI_Pin GPIO_PIN_15
#define SPI2_MOSI_GPIO_Port GPIOB
#define SPI2_MISO_Pin GPIO_PIN_14
#define SPI2_MISO_GPIO_Port GPIOB
#define TP3_Pin GPIO_PIN_0
#define TP3_GPIO_Port GPIOC
#define SPI3_nXEN_Pin GPIO_PIN_10
#define SPI3_nXEN_GPIO_Port GPIOD
#define SPI1_nXEN_Pin GPIO_PIN_8
#define SPI1_nXEN_GPIO_Port GPIOD
#define MOT_aVPH2_Pin GPIO_PIN_2
#define MOT_aVPH2_GPIO_Port GPIOC
#define MOT_aVPH3_Pin GPIO_PIN_3
#define MOT_aVPH3_GPIO_Port GPIOC
#define I2C3_SCL_Pin GPIO_PIN_7
#define I2C3_SCL_GPIO_Port GPIOH
#define MOT_ENCB_Pin GPIO_PIN_11
#define MOT_ENCB_GPIO_Port GPIOH
#define nLED3_Pin GPIO_PIN_2
#define nLED3_GPIO_Port GPIOH
#define MOT_nRESET_Pin GPIO_PIN_8
#define MOT_nRESET_GPIO_Port GPIOE
#define I2C2_XEN_Pin GPIO_PIN_1
#define I2C2_XEN_GPIO_Port GPIOG
#define I2C4_SDA_Pin GPIO_PIN_15
#define I2C4_SDA_GPIO_Port GPIOF
#define I2C3_SDA_Pin GPIO_PIN_8
#define I2C3_SDA_GPIO_Port GPIOH
#define MOT_ENCA_Pin GPIO_PIN_10
#define MOT_ENCA_GPIO_Port GPIOH
#define MOT_ENCZ_Pin GPIO_PIN_12
#define MOT_ENCZ_GPIO_Port GPIOH
#define MOT_aCPH2_Pin GPIO_PIN_1
#define MOT_aCPH2_GPIO_Port GPIOA
#define nLED5_Pin GPIO_PIN_4
#define nLED5_GPIO_Port GPIOH
#define SPI3_nSEL_Pin GPIO_PIN_4
#define SPI3_nSEL_GPIO_Port GPIOA
#define TP1_Pin GPIO_PIN_5
#define TP1_GPIO_Port GPIOA
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define I2C1_XEN_Pin GPIO_PIN_0
#define I2C1_XEN_GPIO_Port GPIOG
#define MOT_nSLEEP_Pin GPIO_PIN_7
#define MOT_nSLEEP_GPIO_Port GPIOE
#define MOT_BREAK_Pin GPIO_PIN_15
#define MOT_BREAK_GPIO_Port GPIOE
#define MOT_aCPH1_Pin GPIO_PIN_0
#define MOT_aCPH1_GPIO_Port GPIOA
#define nLED4_Pin GPIO_PIN_3
#define nLED4_GPIO_Port GPIOH
#define nLED6_Pin GPIO_PIN_5
#define nLED6_GPIO_Port GPIOH
#define MOT_aCPH3_Pin GPIO_PIN_6
#define MOT_aCPH3_GPIO_Port GPIOA
#define MOT_aVPH1_Pin GPIO_PIN_0
#define MOT_aVPH1_GPIO_Port GPIOB
#define I2C4_SCL_Pin GPIO_PIN_14
#define I2C4_SCL_GPIO_Port GPIOF
#define MOT_PWM1_Pin GPIO_PIN_9
#define MOT_PWM1_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
