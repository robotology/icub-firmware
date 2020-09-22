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
#include "stm32g4xx_hal.h"

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
#define J9_27_Pin GPIO_PIN_4
#define J9_27_GPIO_Port GPIOE
#define FIN1_SDA_Pin GPIO_PIN_9
#define FIN1_SDA_GPIO_Port GPIOB
#define FIN1_SCL_Pin GPIO_PIN_8
#define FIN1_SCL_GPIO_Port GPIOB
#define VER0_Pin GPIO_PIN_6
#define VER0_GPIO_Port GPIOB
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_6
#define LED1_GPIO_Port GPIOD
#define CAN1_STDBY_Pin GPIO_PIN_5
#define CAN1_STDBY_GPIO_Port GPIOD
#define MOT2_ENCB_Pin GPIO_PIN_4
#define MOT2_ENCB_GPIO_Port GPIOD
#define CAN1_TX_Pin GPIO_PIN_1
#define CAN1_TX_GPIO_Port GPIOD
#define MOT3_ENCB_Pin GPIO_PIN_12
#define MOT3_ENCB_GPIO_Port GPIOC
#define J9_28_Pin GPIO_PIN_5
#define J9_28_GPIO_Port GPIOE
#define J9_26_Pin GPIO_PIN_3
#define J9_26_GPIO_Port GPIOE
#define J9_24_Pin GPIO_PIN_1
#define J9_24_GPIO_Port GPIOE
#define VER1_Pin GPIO_PIN_7
#define VER1_GPIO_Port GPIOB
#define DAC1_SDO_Pin GPIO_PIN_5
#define DAC1_SDO_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_7
#define LED2_GPIO_Port GPIOD
#define CAN1_SHDN_Pin GPIO_PIN_2
#define CAN1_SHDN_GPIO_Port GPIOD
#define CAN1_RX_Pin GPIO_PIN_0
#define CAN1_RX_GPIO_Port GPIOD
#define MOT2_ENCA_Pin GPIO_PIN_15
#define MOT2_ENCA_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define OSC32_IN_Pin GPIO_PIN_14
#define OSC32_IN_GPIO_Port GPIOC
#define J9_29_Pin GPIO_PIN_6
#define J9_29_GPIO_Port GPIOE
#define J9_25_Pin GPIO_PIN_2
#define J9_25_GPIO_Port GPIOE
#define J9_23_Pin GPIO_PIN_0
#define J9_23_GPIO_Port GPIOE
#define TP8_Pin GPIO_PIN_4
#define TP8_GPIO_Port GPIOB
#define MOT2_ENCZ_Pin GPIO_PIN_3
#define MOT2_ENCZ_GPIO_Port GPIOD
#define DAC3_SDO_Pin GPIO_PIN_11
#define DAC3_SDO_GPIO_Port GPIOC
#define DAC3_CLKI_Pin GPIO_PIN_10
#define DAC3_CLKI_GPIO_Port GPIOC
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define OSC32_OUT_Pin GPIO_PIN_15
#define OSC32_OUT_GPIO_Port GPIOC
#define J9_33_Pin GPIO_PIN_13
#define J9_33_GPIO_Port GPIOC
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define DAC2_SDO_Pin GPIO_PIN_10
#define DAC2_SDO_GPIO_Port GPIOA
#define FIN2_SCL_Pin GPIO_PIN_9
#define FIN2_SCL_GPIO_Port GPIOA
#define OSC_IN_Pin GPIO_PIN_0
#define OSC_IN_GPIO_Port GPIOF
#define OSC_OUT_Pin GPIO_PIN_1
#define OSC_OUT_GPIO_Port GPIOF
#define DAC2_CLKI_Pin GPIO_PIN_9
#define DAC2_CLKI_GPIO_Port GPIOF
#define TP13_Pin GPIO_PIN_10
#define TP13_GPIO_Port GPIOF
#define FIN3_SCL_Pin GPIO_PIN_8
#define FIN3_SCL_GPIO_Port GPIOC
#define FIN3_SDA_Pin GPIO_PIN_9
#define FIN3_SDA_GPIO_Port GPIOC
#define FIN2_SDA_Pin GPIO_PIN_8
#define FIN2_SDA_GPIO_Port GPIOA
#define J9_31_Pin GPIO_PIN_2
#define J9_31_GPIO_Port GPIOC
#define MOT3_CMPN_Pin GPIO_PIN_0
#define MOT3_CMPN_GPIO_Port GPIOC
#define MOT3_CMPP_Pin GPIO_PIN_1
#define MOT3_CMPP_GPIO_Port GPIOC
#define J9_13_Pin GPIO_PIN_14
#define J9_13_GPIO_Port GPIOD
#define FIN4_SCL_Pin GPIO_PIN_6
#define FIN4_SCL_GPIO_Port GPIOC
#define FIN4_SDA_Pin GPIO_PIN_7
#define FIN4_SDA_GPIO_Port GPIOC
#define J9_32_Pin GPIO_PIN_3
#define J9_32_GPIO_Port GPIOC
#define VPP_TEST_Pin GPIO_PIN_1
#define VPP_TEST_GPIO_Port GPIOA
#define TP12_Pin GPIO_PIN_2
#define TP12_GPIO_Port GPIOF
#define MOT1_CMPN_Pin GPIO_PIN_0
#define MOT1_CMPN_GPIO_Port GPIOA
#define J9_30_Pin GPIO_PIN_7
#define J9_30_GPIO_Port GPIOE
#define MOT1_ECLK_Pin GPIO_PIN_12
#define MOT1_ECLK_GPIO_Port GPIOE
#define LED5_Pin GPIO_PIN_10
#define LED5_GPIO_Port GPIOD
#define LED4_Pin GPIO_PIN_9
#define LED4_GPIO_Port GPIOD
#define J9_12_Pin GPIO_PIN_13
#define J9_12_GPIO_Port GPIOD
#define J9_14_Pin GPIO_PIN_15
#define J9_14_GPIO_Port GPIOD
#define MOT2_CMPN_Pin GPIO_PIN_2
#define MOT2_CMPN_GPIO_Port GPIOA
#define DAC1_NSEL_Pin GPIO_PIN_4
#define DAC1_NSEL_GPIO_Port GPIOA
#define VIN_TEST_Pin GPIO_PIN_3
#define VIN_TEST_GPIO_Port GPIOA
#define R_NL_Pin GPIO_PIN_0
#define R_NL_GPIO_Port GPIOB
#define LED6_Pin GPIO_PIN_8
#define LED6_GPIO_Port GPIOE
#define LED7_Pin GPIO_PIN_9
#define LED7_GPIO_Port GPIOE
#define TP11_Pin GPIO_PIN_15
#define TP11_GPIO_Port GPIOE
#define J9_20_Pin GPIO_PIN_11
#define J9_20_GPIO_Port GPIOB
#define CIN_TEST_Pin GPIO_PIN_14
#define CIN_TEST_GPIO_Port GPIOB
#define MOT3_ENCZ_Pin GPIO_PIN_11
#define MOT3_ENCZ_GPIO_Port GPIOD
#define DAC1_CLKO_Pin GPIO_PIN_5
#define DAC1_CLKO_GPIO_Port GPIOA
#define DAC_SYNCEN_Pin GPIO_PIN_6
#define DAC_SYNCEN_GPIO_Port GPIOA
#define NPWF_Pin GPIO_PIN_5
#define NPWF_GPIO_Port GPIOC
#define MOT3_ENCA_Pin GPIO_PIN_2
#define MOT3_ENCA_GPIO_Port GPIOB
#define MAGVCC2_Pin GPIO_PIN_11
#define MAGVCC2_GPIO_Port GPIOE
#define SPI4_MOSI_Pin GPIO_PIN_14
#define SPI4_MOSI_GPIO_Port GPIOE
#define J9_19_Pin GPIO_PIN_10
#define J9_19_GPIO_Port GPIOB
#define J9_22_Pin GPIO_PIN_13
#define J9_22_GPIO_Port GPIOB
#define J9_11_Pin GPIO_PIN_12
#define J9_11_GPIO_Port GPIOD
#define MOT2_CMPP_Pin GPIO_PIN_7
#define MOT2_CMPP_GPIO_Port GPIOA
#define VPPEN_Pin GPIO_PIN_4
#define VPPEN_GPIO_Port GPIOC
#define MOT1_CMPP_Pin GPIO_PIN_1
#define MOT1_CMPP_GPIO_Port GPIOB
#define MAGVCC1_Pin GPIO_PIN_10
#define MAGVCC1_GPIO_Port GPIOE
#define MOT1_EDAT_Pin GPIO_PIN_13
#define MOT1_EDAT_GPIO_Port GPIOE
#define J9_21_Pin GPIO_PIN_12
#define J9_21_GPIO_Port GPIOB
#define TP9_Pin GPIO_PIN_15
#define TP9_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_8
#define LED3_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
