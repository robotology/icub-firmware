/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "rng.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "can_utility.h"
#include "Si705x.h"
#include "BNO055.h"
//#include "EOsensor_BNO055.h"
#include "AD7147.h"
#include "AD7147RegMap.h"
#include "I2C_Multi_SDA.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

// ----------------------------------------------------------------------------
// Firmware Version
// ----------------------------------------------------------------------------
char Firmware_vers = 0;
char Revision_vers = 0;
char Build_number  = 0;

uint8_t can_message = 0;
uint8_t count = 0;

uint16_t Temperature_code;
uint32_t Temperature_board;
uint8_t Temp_int;
uint8_t Temp_dec;

uint8_t text[5] = {0};
uint8_t UART_Rx_buffer[5];



unsigned int channel=0;
extern unsigned int AD7147Registers[2][16][12]; // il primo campo rappresenta il numero dei canali
extern unsigned int CapOffset[2][16][12];       // il primo campo rappresenta il numero dei canali
extern error_cap err[16];
extern unsigned char AD7147_ADD[4];

extern triangle_cfg_t triangle_cfg_list[16];
unsigned char TEMP_COMPENSATION=1;              // if 1 means internal temperature drift compensation
unsigned int taxel[2][16][12];               // il primo campo rappresenta il numero dei canali
int16_t skin_value[2][16];                      // il primo campo rappresenta il numero dei canali
int TOUCH_THREASHOLD=(2450-140);
int SKIN_UPPER_THRESHOLD=2480;
int ERROR_COUNTER=0;

int8_t SKIN_STATE_2T=-1;    // bottom 2 triangles skin state
int8_t SKIN_STATE_10T=-1;   // upper 10 triangles skin state
int8_t SKIN_STATE=-1;
int8_t SKIN_OVER_THRESHOLD=0;
uint8_t WAVEFORM=1;

int16_t triangle[2][16] = {0};
int16_t triangle_old[2][16] = {0};


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void LED_Init(void);
void TIMER_Init(void);
void TEMPERATURE(void);
void BNO055(void);
void BNO055_UART(void);

void FillMessages8bit(unsigned char Channel,unsigned char triangleN);
void UpdateTouchMap(void);
void TouchMapHistoryCheck(void);
void SkinRawData(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void initOSC()
{  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);   

    #warning dont re-init EN_OSC_Pin  
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

    // must use it to init the ext oscillator before clock configuration
    initOSC();
    
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM6_Init();
  MX_CAN1_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_RNG_Init();
  /* USER CODE BEGIN 2 */
#if 0
// marco.accame: it does not compile anymore w/ the most recent versions of HAL
  TIMER_Init();  
  LED_Init();
  CAN_Config();
  Si705x_init();
  BNO055_init();
#endif  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //HAL_UART_Receive_DMA(&huart1,(uint8_t*)UART_Rx_buffer, sizeof(UART_Rx_buffer));
#if 0 
// marco.accame: it does not compile anymore w/ the most recent versions of HAL
  /*##-2- Start the Reception process and enable reception interrupt #########*/
  if (HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler();
  }
#endif  
  while(1)
  {    
    //CANBUS();
    TEMPERATURE();
    BNO055();
    //BNO055_UART();
    
    HAL_Delay(1000);
    text[0] = Temperature_board>>8;
    text[1] = Temperature_board;
    //HAL_UART_Transmit_DMA(&huart1, (uint8_t*)text, sizeof(text));
    //HAL_UART_Transmit(&huart1, (uint8_t*)text, 5,100);
    
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */
void HAL_SYSTICK_Callback(void)
{
	static uint32_t count=0;
	if(count==1000){
		//HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
		count=0;
	}
	count++;
}



// -----------------------------------------------------------------------------------------------------------------------------
// Timers initialization
// -----------------------------------------------------------------------------------------------------------------------------
void TIMER_Init(void){
  //HAL_TIM_Base_Start_IT(&htim6);    // 100us
  //HAL_TIM_IRQHandler(&htim6);
}

// -----------------------------------------------------------------------------------------------------------------------------
// LEDs initialization
// -----------------------------------------------------------------------------------------------------------------------------
void LED_Init(void){
  HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);
}
#if 0 
// marco.accame: it does not compile anymore w/ the most recent versions of H
// -----------------------------------------------------------------------------------------------------------------------------
// CAN messages management
// -----------------------------------------------------------------------------------------------------------------------------
void CANBUS(void){
	switch(can_message){
    case 0x00:  
    {
      hcan1.pTxMsg->StdId   = 0x621;              // Polling
      hcan1.pTxMsg->Data[0] = 0xFF;               // Firmware version message
      hcan1.pTxMsg->Data[1] = 0x02;               // Board type
      hcan1.pTxMsg->Data[2] = Firmware_vers;      // Firmware version
      hcan1.pTxMsg->Data[3] = Revision_vers;      // Revision version
      hcan1.pTxMsg->Data[4] = Build_number;       // Build number
      hcan1.pTxMsg->Data[5] = 0;
      hcan1.pTxMsg->Data[6] = Temperature_board>>8;
      hcan1.pTxMsg->Data[7] = Temperature_board;
      HAL_CAN_Transmit_IT(&hcan1);
      can_message = 0;
    }
    break;
                
    default:    
    {
    }
    break;
  }
}
#endif
// -----------------------------------------------------------------------------------------------------------------------------
// Read the Sensor Temperature
// -----------------------------------------------------------------------------------------------------------------------------
void TEMPERATURE(void){
  Si705x_ReadTemperature();
}

// -----------------------------------------------------------------------------------------------------------------------------
// Read the IMU BNO055 I2C
// -----------------------------------------------------------------------------------------------------------------------------
void BNO055(void){
  BNO055_Read();
}

// -----------------------------------------------------------------------------------------------------------------------------
// Read the IMU BNO055 UART
// -----------------------------------------------------------------------------------------------------------------------------
void BNO055_UART(void){
  BNO055_UART_Read();
}



void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
  if(I2cHandle->Instance==I2C1){
    HAL_I2C_Master_Receive_DMA(&hi2c1, (uint16_t)Si705x_I2C_ADDRESS, (uint8_t*)Si705x_I2C_RxBuffer, sizeof(Si705x_I2C_RxBuffer));
  }
  if(I2cHandle->Instance==I2C2){
    HAL_I2C_Master_Receive_DMA(&hi2c2, (uint16_t)BNO055_I2C_ADDRESS, (uint8_t*)BNO055_RxBuffer, sizeof(BNO055_RxBuffer));
    
  }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
  if(I2cHandle->Instance==I2C1){
    Temperature_code = (Si705x_I2C_RxBuffer[0]<<8) + Si705x_I2C_RxBuffer[1];
    Temperature_board = ((17572*Temperature_code)/65536) - 4685;  // HEX temperature in degrees Celsius (x100)
  }
  if(I2cHandle->Instance==I2C2){
    
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
  HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
  HAL_UART_Receive_DMA(&huart1,(uint8_t*)UART_Rx_buffer, sizeof(UART_Rx_buffer));
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
  HAL_UART_Transmit_DMA(&huart1,(uint8_t*)UART_Rx_buffer, sizeof(UART_Rx_buffer));
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
