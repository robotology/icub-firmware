// IIT-EXT: don't compile it: keep it only for reference
#error DON'T COMPILE ME

/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_hal.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "can_utility.h"
#include "RFE_Expression.h"
#include "RFE_gif.h"
#include "BNO055.h"
#include "MPU9250.h"
#include "usbd_cdc_if.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
char Firmware_vers = 0;
char Revision_vers = 0;
char Build_number  = 0;

uint8_t aTxBuffer[] = "PROVA";
uint8_t Tx[RFE_Expression_StreamLength] = {0};
uint8_t *Data_TMP;
uint32_t color=0;
uint8_t SPI_NR_MSG=0;
uint8_t SPI_TX_OK=1;
//uint8_t *DataStream;
uint8_t DataStream[RFE_Expression_StreamLength];
uint32_t *Expression;
uint8_t BrightnessPercent=0;
uint16_t BrightnessStep;
uint8_t fade=0;
uint16_t timefade=500;    // ms
uint16_t time_period=0;   // ms
uint16_t delay;
uint8_t can_message = 0;
uint8_t *BNO055_SensorValue;
uint8_t *MPU9250_SensorValue;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void FadeEffectSet(uint8_t type, uint16_t time);
void DisplayExpression(uint16_t period);
void CANBUS(void);
void BNO055(void);
void MPU9250(void);

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_CAN1_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_I2C3_Init();
  MX_I2C4_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TIM2_Init();
  MX_USB_DEVICE_Init();
  MX_TIM6_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  HAL_GPIO_WritePin(EN_5V_GPIO_Port, EN_5V_Pin, GPIO_PIN_SET);
  
  CAN_Config();
  BNO055_init();
  MPU9250_init();
  
  BrightnessPercent=90;
  TLC59711_BrightnessSet(BrightnessPercent);
  //TLC59711_BrightnessReset();
  delay=850;
  timefade=3000;
  time_period=200;

  //HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim16);
  
  uint8_t i=0;
  uint32_t color=0;
  
  
  while (1)
  {
    //HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
    //HAL_Delay(200);
    
    // USB Test
    //uint8_t HiMsg[] = "Hello! ";
    //CDC_Transmit_FS(HiMsg, strlen(HiMsg));
    //CDC_Transmit_FS(received_data, received_data_size);
    //CDC_Transmit_FS(receive_total, receive_total);
    //HAL_Delay(500);
      
//    for(i=0; i<60;i++){
//      FACE_MASK[i] = color;
//    }
//    color++;
//    Expression=FACE_MASK;
//    //FadeEffectSet(FadeUP, timefade);
//    //FadeEffectSet(FadeDOWN, timefade);
//    DisplayExpression(time_period);
//    HAL_Delay(10);    
    
//    Expression=FACE0;
//    FadeEffectSet(FadeUP, timefade);
//    FadeEffectSet(FadeDOWN, timefade);
//    HAL_Delay(time_period);    
//    
//    Expression=FACE1;
//    FadeEffectSet(FadeUP, timefade);
//    FadeEffectSet(FadeDOWN, timefade);
//    DisplayExpression(time_period);
//    HAL_Delay(time_period);
//    
//    Expression=FACE2;
//    FadeEffectSet(FadeUP, timefade);
//    DisplayExpression(time_period);
//    HAL_Delay(time_period);
//    Expression=FACE3;
//    DisplayExpression(time_period);
//    HAL_Delay(time_period);
//    Expression=FACE4;
//    DisplayExpression(time_period);
//    HAL_Delay(time_period);
//    Expression=FACE5;
//    FadeEffectSet(FadeDOWN, timefade);
//    DisplayExpression(time_period);
//    HAL_Delay(time_period);

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C2
                              |RCC_PERIPHCLK_I2C4|RCC_PERIPHCLK_I2C3
                              |RCC_PERIPHCLK_USB|RCC_PERIPHCLK_ADC;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
  PeriphClkInit.I2c4ClockSelection = RCC_I2C4CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 12;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV4;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV4;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK|RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the main internal regulator output voltage 
    */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

// -----------------------------------------------------------------------------------------------------------------------------
// Timers
// -----------------------------------------------------------------------------------------------------------------------------

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  SPI_TX_OK=1;
  HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  /* Turn LED2 on: Transfer error in reception/transmission process */
  //HAL_GPIO_TogglePin(GPIOE, LD9_Pin);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if(htim->Instance==TIM6){         // timer 10ms
    //HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
    DisplayExpression(0);
  }
  
  if(htim->Instance==TIM16){         // timer 10ms
   BNO055();
   MPU9250();
   //HAL_Delay(100);
   CANBUS();
   //HAL_Delay(20);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
// I2C Callback
// -----------------------------------------------------------------------------------------------------------------------------
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
  if(I2cHandle->Instance==I2C1){
    HAL_I2C_Master_Receive_DMA(&hi2c1, (uint16_t)BNO055_I2C_ADDRESS, (uint8_t*)BNO055_RxBuffer, sizeof(BNO055_RxBuffer));
  }
  if(I2cHandle->Instance==I2C2){
    HAL_I2C_Master_Receive_DMA(&hi2c2, (uint16_t)MPU9250_I2C_ADDRESS, (uint8_t*)MPU9250_RxBuffer, sizeof(MPU9250_RxBuffer));
  }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
  if(I2cHandle->Instance==I2C1){
    BNO055_SensorValue = BNO055_RxBuffer;
  }
  if(I2cHandle->Instance==I2C2){
    MPU9250_SensorValue = MPU9250_RxBuffer;
  }
}

// ----------------------------------------------------------------------------
// Fade effects
// ----------------------------------------------------------------------------
void FadeEffectSet(uint8_t type, uint16_t time) {
  uint8_t BrightnessActual;
  uint16_t timer_counter;
  
  BrightnessActual=TLC59711_BrightnessGet();
  timer_counter=time/10;    // 10 sono i ms del timer6
  
  BrightnessStep = (uint16_t)BrightnessActual / timer_counter;      // incremento o decremento del brightness durante il fade
  BrightnessStep=1;
  if(type==FadeUP){
    TLC59711_BrightnessReset();
    fade=FadeUP;
  }
  else if(type==FadeDOWN){
    fade=FadeDOWN;
  }
  
  HAL_TIM_Base_Start_IT(&htim6);
  
  HAL_Delay(delay);
}

// ----------------------------------------------------------------------------
// Display with fading effect
// ----------------------------------------------------------------------------
void DisplayExpression(uint16_t period){
  TLC59711_Brightness_Fade(fade, BrightnessStep);
  RFE_Expression(Expression, DontChange, DataStream, RFE_Expression_StreamLength);
  TLC59711_SendDataPacket(DataStream, RFE_Expression_StreamLength);
  //HAL_Delay(period);
}


// -----------------------------------------------------------------------------------------------------------------------------
// CAN messages management
// -----------------------------------------------------------------------------------------------------------------------------
void CANBUS(void){
	switch(can_message){
    case 0x00:  
    {
      TxData[0] = 0xff;
      TxData[1] = can_message;
      TxData[2] = *(BNO055_SensorValue+1);
      TxData[3] = *(BNO055_SensorValue+0);
      TxData[4] = *(BNO055_SensorValue+3);
      TxData[5] = *(BNO055_SensorValue+2);
      TxData[6] = *(BNO055_SensorValue+5);
      TxData[7] = *(BNO055_SensorValue+4);
//      TxData[2] = ((*(BNO055_SensorValue+1))<<2) + ((*(BNO055_SensorValue+0))>>6);
//      TxData[3] = (*(BNO055_SensorValue+0))<<2;
//      TxData[4] = ((*(BNO055_SensorValue+3))<<2) + ((*(BNO055_SensorValue+2))>>6);
//      TxData[5] = (*(BNO055_SensorValue+2))<<2;
//      TxData[6] = ((*(BNO055_SensorValue+5))<<2) + ((*(BNO055_SensorValue+4))>>6);
//      TxData[7] = (*(BNO055_SensorValue+4))<<2;
      
      if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK){
        /* Transmission request Error */
        Error_Handler();
      }
      can_message = 1;
    }
    break;
    
    case 0x01:  
    {
      TxData[0] = 0xff;
      TxData[1] = can_message;
      TxData[2] = *(MPU9250_SensorValue+0);
      TxData[3] = *(MPU9250_SensorValue+1);
      TxData[4] = *(MPU9250_SensorValue+2);
      TxData[5] = *(MPU9250_SensorValue+3);
      TxData[6] = *(MPU9250_SensorValue+4);
      TxData[7] = *(MPU9250_SensorValue+5);
      
      if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK){
        /* Transmission request Error */
        Error_Handler();
      }
      can_message = 0;
    }
    break;
    
    default:
    {
    }
    break;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
// Read the IMU BNO055 I2C
// -----------------------------------------------------------------------------------------------------------------------------
void BNO055(void){
  BNO055_Read(BNO055_FirstValue);
}

// -----------------------------------------------------------------------------------------------------------------------------
// Read the IMU MPU9250 I2C
// -----------------------------------------------------------------------------------------------------------------------------
void MPU9250(void){
  MPU9250_Registers(MPU9250_ACCEL_XOUT, 0, 1);
  //MPU9250_Registers(MPU9250_GYRO_XOUT, 0, 1);
  //MPU9250_Registers(MPU9250_TEMP_OUT, 0, 2);
}
    
    

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
