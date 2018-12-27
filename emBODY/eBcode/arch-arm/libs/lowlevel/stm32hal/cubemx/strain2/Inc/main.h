/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define BNO055_BOOT_Pin GPIO_PIN_13
#define BNO055_BOOT_GPIO_Port GPIOC
#define BNO055_INT_Pin GPIO_PIN_10
#define BNO055_INT_GPIO_Port GPIOC
#define BNO055_RESET_Pin GPIO_PIN_12
#define BNO055_RESET_GPIO_Port GPIOC
#define EN_OSC_Pin GPIO_PIN_11
#define EN_OSC_GPIO_Port GPIOA
#define W_STRAIN6_Pin GPIO_PIN_9
#define W_STRAIN6_GPIO_Port GPIOC
#define STRAIN2_Pin GPIO_PIN_1
#define STRAIN2_GPIO_Port GPIOC
#define STRAIN1_Pin GPIO_PIN_0
#define STRAIN1_GPIO_Port GPIOC
#define W_STRAIN4_Pin GPIO_PIN_7
#define W_STRAIN4_GPIO_Port GPIOC
#define W_STRAIN5_Pin GPIO_PIN_8
#define W_STRAIN5_GPIO_Port GPIOC
#define STRAIN3_Pin GPIO_PIN_2
#define STRAIN3_GPIO_Port GPIOC
#define CAN1_SHDN_Pin GPIO_PIN_0
#define CAN1_SHDN_GPIO_Port GPIOB
#define W_STRAIN3_Pin GPIO_PIN_6
#define W_STRAIN3_GPIO_Port GPIOC
#define EN_2V8_Pin GPIO_PIN_15
#define EN_2V8_GPIO_Port GPIOB
#define STRAIN4_Pin GPIO_PIN_3
#define STRAIN4_GPIO_Port GPIOC
#define STRAIN5_Pin GPIO_PIN_0
#define STRAIN5_GPIO_Port GPIOA
#define CAN1_STB_Pin GPIO_PIN_1
#define CAN1_STB_GPIO_Port GPIOB
#define PG_3V3_Pin GPIO_PIN_13
#define PG_3V3_GPIO_Port GPIOB
#define STRAIN6_Pin GPIO_PIN_1
#define STRAIN6_GPIO_Port GPIOA
#define W_STRAIN1_Pin GPIO_PIN_4
#define W_STRAIN1_GPIO_Port GPIOC
#define W_STRAIN2_Pin GPIO_PIN_5
#define W_STRAIN2_GPIO_Port GPIOC
#define LED_Pin GPIO_PIN_11
#define LED_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
