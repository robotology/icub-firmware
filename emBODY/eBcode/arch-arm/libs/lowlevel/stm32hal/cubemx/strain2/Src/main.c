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
  * COPYRIGHT(c) 2018 STMicroelectronics
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
#include "PGA308.h"

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

#define LED_ON    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
#define LED_OFF   HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

uint8_t can_message = 0;
uint16_t count = 0;
uint8_t REG_count = 0;
uint8_t REG_GO = 0;
uint8_t REG_tran = 0;
uint8_t REG_OK = 0;
uint8_t REG_Value = 0;
#define SAMPLES 10
uint8_t REG_max_count = SAMPLES;     
uint16_t REG_value_tran[SAMPLES];
uint8_t CHANNEL=1;
uint8_t REGISTER=RAM_CFG0;
uint8_t PGA308_StartCalib=1;

uint16_t Temperature_TOPcode_prev=0;
uint16_t Temperature_TOPcode;
uint32_t Temperature_TOPboard;
uint16_t Temperature_BOTcode;
uint32_t Temperature_BOTboard;
uint8_t Temp_int;
uint8_t Temp_dec;

uint8_t PGA308_DimBufferTXbit=8*PGA308_DimBufferTX;
uint8_t bit_count=0;
uint8_t byte_count=0;
uint8_t bit_count_rel=0;
uint8_t PGA308_channel;

extern uint8_t PGA308_BufferTx[PGA308_DimBufferTX];
extern uint16_t adc_values[6];           // contains all ADC channels conversion
extern uint8_t PGA308_StartComm;

//uint16_t PGA308register = 0;
fifo PGA308_CoarseOffset = {0};

uint16_t Gain;

uint8_t text[5] = {0};
uint8_t UART_Rx_buffer[5];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void TIMER_Init(void);
void TIMER_PROG(uint16_t value);
void TEMPERATURE(void);
void BNO055(void);
void BNO055_UART(void);
void SelfCalibration(uint8_t channel, uint8_t reg);
void PGA_CoarseOffset_Calib(uint8_t channel, uint16_t DAC_OUT);
void PGA_FineOffset_Calib(uint8_t channel, uint16_t DAC_OUT);
void PGA_StableRegister(uint16_t REGISTER);
//void calcMean();

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART2_UART_Init();
  MX_TIM7_Init();
  MX_TIM16_Init();
  MX_TIM15_Init();
  MX_RNG_Init();
  /* USER CODE BEGIN 2 */
	TIMER_Init();  
  CAN_Config();
	Si705x_init(1);
  BNO055_init();
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  /*##-2- Start the Reception process and enable reception interrupt #########*/
  if (HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler();
  }
  
  PGA308_init();
  //PGA308_DefaultConfig();
  
  HAL_Delay(100);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_values, 6);
  
  HAL_Delay(1000);
  
  REGISTER=RAM_CFG0;
  //REGISTER=RAM_GDAC;
  
  while (1)
  {
    //CANBUS();
    TEMPERATURE();
    BNO055();
    //BNO055_UART();
    
    if(PGA308_StartCalib){
      TIMER_PROG(1000);
      PGA308_StartCalib=0;
      PGA308_DefaultConfig();
    }
    else
    {
      if(REG_OK==0) {SelfCalibration(CHANNEL, REGISTER);}
      else{
        if(CHANNEL<6) {
          REG_OK=0;
          CHANNEL++;
        }
        else{
          if(REGISTER==RAM_CFG0){
            HAL_Delay(100);
            REGISTER=RAM_ZDAC;
            REG_OK=0;
            CHANNEL=1;
          }
          else if(REGISTER==RAM_ZDAC){
            TIMER_PROG(10000);
            REGISTER=RAM_GDAC;
          }
        }
      }
    }
    HAL_Delay(10);
    
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C2
                              |RCC_PERIPHCLK_RNG|RCC_PERIPHCLK_ADC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
  PeriphClkInit.RngClockSelection = RCC_RNGCLKSOURCE_HSI48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure the main internal regulator output voltage 
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_SYSTICK_Callback(void)
{
//	static uint32_t count=0;
//	if(count==1000){
//		//HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
//		count=0;
//	}
//	count++;
}

// -----------------------------------------------------------------------------------------------------------------------------
// Timers initialization
// -----------------------------------------------------------------------------------------------------------------------------
void TIMER_Init(void){
  __HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
  __HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
  __HAL_TIM_CLEAR_IT(&htim15, TIM_IT_UPDATE);
  __HAL_TIM_CLEAR_IT(&htim16, TIM_IT_UPDATE);
  
	HAL_TIM_Base_Start_IT(&htim16);
}

// -----------------------------------------------------------------------------------------------------------------------------
// FT Sensor Self-Calibration for any channel, by using coarse and fine offset tuning
// -----------------------------------------------------------------------------------------------------------------------------
void SelfCalibration(uint8_t channel, uint8_t reg){
  uint16_t DAC_OUT;
  
  if      (channel==1)  {DAC_OUT=adc_measure.STRAIN1;}
  else if (channel==2)  {DAC_OUT=adc_measure.STRAIN2;}
  else if (channel==3)  {DAC_OUT=adc_measure.STRAIN3;}
  else if (channel==4)  {DAC_OUT=adc_measure.STRAIN4;}
  else if (channel==5)  {DAC_OUT=adc_measure.STRAIN5;}
  else if (channel==6)  {DAC_OUT=adc_measure.STRAIN6;}
  
  if(reg==RAM_CFG0){
    PGA_CoarseOffset_Calib(channel, DAC_OUT);                         // Coarse calibration using CFG0.OS register
    PGA_StableRegister(PGA308[channel].CFG0_OS);                      // Check if value is stable or not
    if(REG_Value){                                                    // If value is not stable yet, write a better value in CFG0.OS
      PGA308[channel].CFG0 = ((PGA308[channel].CFG0 & 0xFF00) | PGA308[channel].CFG0_OS);
      PGA308_WriteRegister(channel, reg, PGA308[channel].CFG0);
    }
  }
  
  if(reg==RAM_ZDAC){
    PGA_FineOffset_Calib(channel, DAC_OUT);                           // Fine calibration using ZDAC register
    PGA_StableRegister(PGA308[channel].ZDAC);                         // Check if value is stable or not
    if(REG_Value){                                                    // If value is not stable yet, write a better value in ZDAC
      PGA308_WriteRegister(channel, reg, PGA308[channel].ZDAC);
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
// PGA308 Coarse Offset Calibration - Register CFG0.OS
// -----------------------------------------------------------------------------------------------------------------------------
void PGA_CoarseOffset_Calib(uint8_t channel, uint16_t DAC_OUT){
  if(DAC_OUT < 4096){                         // 4096 metà scala dell'ADC, metto diverso per poter poi calibrare finemente col Zero DAC
    if(PGA308[channel].CFG0_OS>>7 == 0) { 
      if(PGA308[channel].CFG0_OS<0x64)  {PGA308[channel].CFG0_OS++;}
    }
    else{
      if( (PGA308[channel].CFG0_OS & 0x7F) > 0)   {PGA308[channel].CFG0_OS--;}
      else                                        {PGA308[channel].CFG0_OS=0x00;}
    }
  }
  else{
    if(PGA308[channel].CFG0_OS>>7 == 0) {
      if( (PGA308[channel].CFG0_OS & 0x7F) > 0)   {PGA308[channel].CFG0_OS--;} 
      else                                        {PGA308[channel].CFG0_OS=0x80;}
    }
    else{
      if( (PGA308[channel].CFG0_OS & 0x7F)<0x64)  {PGA308[channel].CFG0_OS++;}
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
// PGA308 Fine Offset Calibration - Register ZDAC
// -----------------------------------------------------------------------------------------------------------------------------
void PGA_FineOffset_Calib(uint8_t channel, uint16_t DAC_OUT){
  if(DAC_OUT < 4096){                         // 4096 metà scala dell'ADC
    if(PGA308[channel].ZDAC > 0x0000)  {PGA308[channel].ZDAC--;}
  }
  else{
    if(PGA308[channel].ZDAC < 0xFFFF)  {PGA308[channel].ZDAC++;}
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
// PGA308 Check the stability of register values
// -----------------------------------------------------------------------------------------------------------------------------
void PGA_StableRegister(uint16_t REGISTER){
  uint8_t i;
  REG_value_tran[REG_count]=REGISTER;
  if(REG_count < REG_max_count) {REG_count++;}
  else                          {REG_count=0; REG_GO=1;}
  
  for(i=0; i<REG_max_count; i++){
    if(REG_value_tran[i] > REG_value_tran[REG_max_count]){
      if( (REG_value_tran[i] - REG_value_tran[REG_max_count]) <= 1){
        REG_tran++;
      }
    }
    else{
      if( REG_value_tran[REG_max_count] - (REG_value_tran[i]) <= 1){
        REG_tran++;
      }
    }
  }
  
  if(REG_GO){
    if(REG_tran != REG_max_count) {REG_Value=1;}
    else                          {REG_Value=0; REG_OK=1; REG_GO=0;}
  }
  REG_tran=0;
}

// -----------------------------------------------------------------------------------------------------------------------------
// CAN messages management
// -----------------------------------------------------------------------------------------------------------------------------
void CANBUS(void){
	switch(can_message){
    case 0x00:  
    {
      hcan1.pTxMsg->StdId   = 0x621;              // Polling
      hcan1.pTxMsg->Data[0] = 0xFF;               // Firmware version message
      //hcan1.pTxMsg->Data[1] = 0x00;               // Board type
      hcan1.pTxMsg->Data[1] = Firmware_vers;      // Firmware version
      hcan1.pTxMsg->Data[2] = Revision_vers;      // Revision version
      hcan1.pTxMsg->Data[3] = Build_number;       // Build number
      hcan1.pTxMsg->Data[4] = PGA308[1].CFG0_OS>>8;
      hcan1.pTxMsg->Data[5] = PGA308[1].CFG0_OS;
      hcan1.pTxMsg->Data[6] = 0;
      hcan1.pTxMsg->Data[7] = 0;
      HAL_CAN_Transmit_IT(&hcan1);
      can_message = 1;
    }
    break;
    
    case 0x01:  
    {
      hcan1.pTxMsg->StdId   = 0x621;              // Polling
      hcan1.pTxMsg->Data[0] = 0xFF;               // Firmware version message
      hcan1.pTxMsg->Data[1] = 0x01;               // Board type
      hcan1.pTxMsg->Data[2] = ((adc_measure.STRAIN1)>>8) & 0x000000FF;
      hcan1.pTxMsg->Data[3] = adc_measure.STRAIN1 & 0x000000FF;
      hcan1.pTxMsg->Data[4] = ((adc_measure.STRAIN2)>>8) & 0x000000FF;
      hcan1.pTxMsg->Data[5] = adc_measure.STRAIN2 & 0x000000FF;
      hcan1.pTxMsg->Data[6] = ((adc_measure.STRAIN3)>>8) & 0x000000FF;
      hcan1.pTxMsg->Data[7] = adc_measure.STRAIN3 & 0x000000FF;
      HAL_CAN_Transmit_IT(&hcan1);
      can_message = 2;
    }
    break;
    
    case 0x02:  
    {
      hcan1.pTxMsg->StdId   = 0x621;              // Polling
      hcan1.pTxMsg->Data[0] = 0xFF;               // Firmware version message
      hcan1.pTxMsg->Data[1] = 0x02;               // Board type
      hcan1.pTxMsg->Data[2] = ((adc_measure.STRAIN4)>>8) & 0x000000FF;
      hcan1.pTxMsg->Data[3] = adc_measure.STRAIN4 & 0x000000FF;
      hcan1.pTxMsg->Data[4] = ((adc_measure.STRAIN5)>>8) & 0x000000FF;
      hcan1.pTxMsg->Data[5] = adc_measure.STRAIN5 & 0x000000FF;
      hcan1.pTxMsg->Data[6] = ((adc_measure.STRAIN6)>>8) & 0x000000FF;
      hcan1.pTxMsg->Data[7] = adc_measure.STRAIN6 & 0x000000FF;
      HAL_CAN_Transmit_IT(&hcan1);
      can_message = 3;
    }
    break;
    
    case 0x03:  
    {
      hcan1.pTxMsg->StdId   = 0x621;              // Polling
      hcan1.pTxMsg->Data[0] = 0xFF;               // Firmware version message
      hcan1.pTxMsg->Data[1] = 0x03;               // Board type
      hcan1.pTxMsg->Data[2] = Temperature_TOPboard>>8;
      hcan1.pTxMsg->Data[3] = Temperature_TOPboard;
      hcan1.pTxMsg->Data[4] = Temperature_BOTboard>>8;
      hcan1.pTxMsg->Data[5] = Temperature_BOTboard;
      hcan1.pTxMsg->Data[6] = 0;
      hcan1.pTxMsg->Data[7] = 0;
      HAL_CAN_Transmit_IT(&hcan1);
      can_message = 0;
    }
    break;
     
    case 0x10:  
    {
      hcan1.pTxMsg->StdId   = 0x621;              // Polling
      hcan1.pTxMsg->Data[0] = 0xFF;               // Firmware version message
      //hcan1.pTxMsg->Data[1] = 0x00;               // Board type
      hcan1.pTxMsg->Data[1] = Firmware_vers;      // Firmware version
      hcan1.pTxMsg->Data[2] = Revision_vers;      // Revision version
      hcan1.pTxMsg->Data[3] = Build_number;       // Build number
      hcan1.pTxMsg->Data[4] = PGA308[1].CFG0_OS>>8;
      hcan1.pTxMsg->Data[5] = PGA308[1].CFG0_OS;
      hcan1.pTxMsg->Data[6] = 0;
      hcan1.pTxMsg->Data[7] = 0;
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

// -----------------------------------------------------------------------------------------------------------------------------
// Read the Sensor Temperature
// -----------------------------------------------------------------------------------------------------------------------------
void TEMPERATURE(void){
  Si705x_ReadTemperature(1);
}

// -----------------------------------------------------------------------------------------------------------------------------
// Read the IMU BNO055 I2C
// -----------------------------------------------------------------------------------------------------------------------------
void BNO055(void){
  BNO055_Read(0x08);
}

// -----------------------------------------------------------------------------------------------------------------------------
// Read the IMU BNO055 UART
// -----------------------------------------------------------------------------------------------------------------------------
void BNO055_UART(void){
  BNO055_UART_Read();
}


// -----------------------------------------------------------------------------------------------------------------------------
// I2C Callback
// -----------------------------------------------------------------------------------------------------------------------------
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
  if(I2cHandle->Instance==I2C1){
    HAL_I2C_Master_Receive_DMA(&hi2c1, (uint16_t)Si705x_I2C_ADDRESS, (uint8_t*)Si705x_I2C1_RxBuffer, sizeof(Si705x_I2C1_RxBuffer));
  }
  if(I2cHandle->Instance==I2C2){
    HAL_I2C_Master_Receive_DMA(&hi2c2, (uint16_t)Si705x_I2C_ADDRESS, (uint8_t*)Si705x_I2C2_RxBuffer, sizeof(Si705x_I2C2_RxBuffer));
    HAL_I2C_Master_Receive_DMA(&hi2c2, (uint16_t)BNO055_I2C_ADDRESS, (uint8_t*)BNO055_RxBuffer, sizeof(BNO055_RxBuffer));
    
  }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
  if(I2cHandle->Instance==I2C1){
    Temperature_BOTcode = (Si705x_I2C1_RxBuffer[0]<<8) + Si705x_I2C1_RxBuffer[1];
    Temperature_BOTboard = ((17572*Temperature_BOTcode)/65536) - 4685;  // HEX temperature in degrees Celsius (x100)
  }
  if(I2cHandle->Instance==I2C2){
    Temperature_TOPcode = (Si705x_I2C2_RxBuffer[0]<<8) + Si705x_I2C2_RxBuffer[1];
    Temperature_TOPboard = ((17572*Temperature_TOPcode)/65536) - 4685;  // HEX temperature in degrees Celsius (x100)
    
    if(Temperature_TOPcode != Temperature_TOPcode_prev){
      Temperature_TOPcode_prev=Temperature_TOPcode;
      CANBUS();
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
// UART Callback
// -----------------------------------------------------------------------------------------------------------------------------
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
  HAL_UART_Receive_DMA(&huart1,(uint8_t*)UART_Rx_buffer, sizeof(UART_Rx_buffer));
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  HAL_UART_Transmit_DMA(&huart1,(uint8_t*)UART_Rx_buffer, sizeof(UART_Rx_buffer));
}

// -----------------------------------------------------------------------------------------------------------------------------
// Timers Callback
// -----------------------------------------------------------------------------------------------------------------------------
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if(htim->Instance==TIM6){         // timer 10ms
    if(bit_count_rel==0){       // START condition of One-Wire Protocol
      PGA308_BitValue(PGA308_channel, LOW);
      bit_count_rel++;
    }
    else if(bit_count_rel==9){   // STOP condition of One-Wire Protocol
      PGA308_BitValue(PGA308_channel, HIGH);
      bit_count_rel=0;
      byte_count++;
    }
    else{                       // Transmission of One-Wire Protocol
      if(bit_count<PGA308_DimBufferTXbit){
        if(((PGA308_BufferTx[byte_count])>>(bit_count_rel-1)) & 0x01){
          PGA308_BitValue(PGA308_channel, HIGH);
        }
        else{
          PGA308_BitValue(PGA308_channel, LOW);
        }
        bit_count++;
        bit_count_rel++;
      }
      else{
        PGA308_BitValue(PGA308_channel, HIGH);
        HAL_TIM_Base_Stop_IT(&htim6);
        bit_count=0;
        bit_count_rel=0;
        byte_count=0;
        HAL_TIM_Base_Start_IT(&htim15);
        REG_Value=0;
      }
    }
	}
  
	if(htim->Instance==TIM7){        // timer 100ms
    if(bit_count_rel==0){       // START condition of One-Wire Protocol
      PGA308_BitValue(PGA308_channel, LOW);
      bit_count_rel++;
    }
    else if(bit_count_rel==9){   // STOP condition of One-Wire Protocol
      PGA308_BitValue(PGA308_channel, HIGH);
      bit_count_rel=0;
      byte_count++;
    }
    else{                       // Transmission of One-Wire Protocol
      if(bit_count<16){
        if(((PGA308_BufferTx[byte_count])>>(bit_count_rel-1)) & 0x01){
          PGA308_BitValue(PGA308_channel, HIGH);
        }
        else{
          PGA308_BitValue(PGA308_channel, LOW);
        }
        bit_count++;
        bit_count_rel++;
      }
      else{
        PGA308_BitValue(PGA308_channel, HIGH);
        HAL_TIM_Base_Stop_IT(&htim7);
        bit_count=0;
        bit_count_rel=0;
        byte_count=0;
        HAL_TIM_Base_Start_IT(&htim15);
        can_message=0x10;
      }
    }
	}
  
  if(htim->Instance==TIM15){        // timer 20ms
    PGA308_StartComm=0;
    HAL_TIM_Base_Stop_IT(&htim15);
	}
  
  if(htim->Instance==TIM16){        // timer xxx ms 
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	}
}


void TIMER_PROG(uint16_t value){     // example: value=10000 -> 1s, value=1000 -> 100ms, value=100 -> 10ms  
  htim16.Init.Period = value;
  HAL_TIM_Base_Init(&htim16);
}


// -----------------------------------------------------------------------------------------------------------------------------
// ADC
// -----------------------------------------------------------------------------------------------------------------------------
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
  //HAL_ADC_Stop_DMA(&hadc1);
  adc_measure.STRAIN1 = (*(adc_values+0));//*0.687+58.5;     // mV
  adc_measure.STRAIN2 = (*(adc_values+1));//*0.687+58.5;     // mV
  adc_measure.STRAIN3 = (*(adc_values+2));//*0.687+58.5;     // mV
  adc_measure.STRAIN4 = (*(adc_values+3));//*0.687+58.5;     // mV
  adc_measure.STRAIN5 = (*(adc_values+4));//*0.687+58.5;     // mV
  adc_measure.STRAIN6 = (*(adc_values+5));//*0.687+58.5;     // mV
  
  //HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_values, 6);
  //calcMean();
  //CANBUS();
}

void calcMean(){
  adc_samples.STRAIN1[adc_sample] = adc_measure.STRAIN1;
  adc_samples.STRAIN2[adc_sample] = adc_measure.STRAIN2;
  adc_samples.STRAIN3[adc_sample] = adc_measure.STRAIN3;
  adc_samples.STRAIN4[adc_sample] = adc_measure.STRAIN4;
  adc_samples.STRAIN5[adc_sample] = adc_measure.STRAIN5;
  adc_samples.STRAIN6[adc_sample] = adc_measure.STRAIN6;
  
  if(adc_sample < (nr_adc_sample-1))  {adc_sample++;}
  else                                {adc_sample=0;}
  
  mean.STRAIN1=0;
	mean.STRAIN2=0;
	mean.STRAIN3=0;
	mean.STRAIN4=0;
	mean.STRAIN5=0;
	mean.STRAIN6=0;
  
  for(uint16_t i=0; i < nr_adc_sample; i++){
    mean.STRAIN1 += adc_samples.STRAIN1[i];
    mean.STRAIN2 += adc_samples.STRAIN2[i];
    mean.STRAIN3 += adc_samples.STRAIN3[i];
    mean.STRAIN4 += adc_samples.STRAIN4[i];
    mean.STRAIN5 += adc_samples.STRAIN5[i];
    mean.STRAIN6 += adc_samples.STRAIN6[i];
  }
  
  mean.STRAIN1 = mean.STRAIN1 / nr_adc_sample;
  mean.STRAIN2 = mean.STRAIN2 / nr_adc_sample;
  mean.STRAIN3 = mean.STRAIN3 / nr_adc_sample;
  mean.STRAIN4 = mean.STRAIN4 / nr_adc_sample;
  mean.STRAIN5 = mean.STRAIN5 / nr_adc_sample;
  mean.STRAIN6 = mean.STRAIN6 / nr_adc_sample;
}


void CheckTransient(){

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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
