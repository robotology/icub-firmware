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
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "define.h"
#include "global_var.h"
#include "can_utility.h"
#include "Si705x.h"
#include "MPU9250.h"
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
char Firmware_vers = 0;
char Revision_vers = 0;
char Build_number  = 0;

uint8_t can_message = 0;
uint32_t Temperature_TOPboard;

uint8_t CHANNEL=1;

uint8_t PGA308_DimBufferTXbit=8*PGA308_DimBufferTX;
uint8_t bit_count=0;
uint8_t byte_count=0;
uint8_t bit_count_rel=0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void TIMER_Init(void);
void CANBUS(void);
void TIMER_PROG(uint16_t value);
void SelfCalibration(uint8_t channel, uint8_t reg);
void PGA_CoarseOffset_Calib(uint8_t channel, uint16_t DAC_OUT);
void PGA_FineOffset_Calib(uint8_t channel, uint16_t DAC_OUT);
void PGA_StableRegister(uint16_t REGISTER);

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
  MX_ADC1_Init();
  MX_CAN1_Init();
  MX_I2C1_Init();
  MX_I2C3_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_TIM15_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */
  
  TIMER_Init();  
  CAN_Config();
  MPU9250_init();
  PGA308_init();
  
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED); 
  HAL_Delay(100);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_values, STRAIN_CHANNELS);
  
  REGISTER=RAM_CFG0;
  
  //PGA308_DefaultConfig();
  
  //HAL_Delay(100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if(PGA308_StartCalib){
      TIMER_PROG(1000);
      PGA308_StartCalib=0;
      PGA308_DefaultConfig();
    }
    else
    {
      if(REG_OK==0) {SelfCalibration(CHANNEL, REGISTER);}
      else{
        if(CHANNEL<STRAIN_CHANNELS) {
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
    
    
    Si705x_ReadTemperature();
    MPU9250();
    HAL_Delay(100);
    CANBUS();

//    LED_ON;
//    //HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
//    //HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
//    HAL_Delay(200);
//    LED_OFF;
//    //HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C3
                              |RCC_PERIPHCLK_ADC;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 10;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
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


// -----------------------------------------------------------------------------------------------------------------------------
// CAN messages management
// -----------------------------------------------------------------------------------------------------------------------------
void CANBUS(void){
	switch(can_message){
    case 0x00:  
    {
      TxData[0] = 0xFF;               //
      TxData[1] = can_message;        //
      TxData[2] = Firmware_vers;      // Firmware version
      TxData[3] = Revision_vers;      // Revision version
      TxData[4] = Build_number;       // Build number
      TxData[5] = PGA308[1].CFG0_OS>>8;
      TxData[6] = PGA308[1].CFG0_OS;
      TxData[7] = 0;
      if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK){
        /* Transmission request Error */
        Error_Handler();
      }      
      can_message = 1;
    }
    break;
    
    case 0x01:  
    {
      TxData[0] = 0xFF;
      TxData[1] = can_message;
      TxData[2] = ((adc_measure.STRAIN1)>>8) & 0x000000FF;
      TxData[3] = adc_measure.STRAIN1 & 0x000000FF;
      TxData[4] = ((adc_measure.STRAIN2)>>8) & 0x000000FF;
      TxData[5] = adc_measure.STRAIN2 & 0x000000FF;
      TxData[6] = ((adc_measure.STRAIN3)>>8) & 0x000000FF;
      TxData[7] = adc_measure.STRAIN3 & 0x000000FF;
      if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK){
        /* Transmission request Error */
        Error_Handler();
      }
      can_message = 2;
    }
    break;
        
    case 0x02:  
    {
      TxData[0] = 0xFF;
      TxData[1] = can_message;
      TxData[2] = Temperature_board>>8;
      TxData[3] = Temperature_board;
      TxData[4] = 0;
      TxData[5] = 0;
      TxData[6] = 0;
      TxData[7] = 0;
      if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK){
        /* Transmission request Error */
        Error_Handler();
      }
      can_message = 3;
    }
    break;
    
    case 0x03:  
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
// I2C Callback
// -----------------------------------------------------------------------------------------------------------------------------
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
  if(I2cHandle->Instance==I2C1){
    HAL_I2C_Master_Receive_DMA(&hi2c1, (uint16_t)MPU9250_I2C_ADDRESS, (uint8_t*)MPU9250_RxBuffer, sizeof(MPU9250_RxBuffer));
  }
  
  if(I2cHandle->Instance==I2C3){
    HAL_I2C_Master_Receive_DMA(&hi2c3, (uint16_t)Si705x_I2C_ADDRESS, (uint8_t*)Si705x_I2C3_RxBuffer, sizeof(Si705x_I2C3_RxBuffer));
  }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
  if(I2cHandle->Instance==I2C1){
    MPU9250_SensorValue = MPU9250_RxBuffer;
  }
  
  if(I2cHandle->Instance==I2C3){
    Temperature_code = (Si705x_I2C3_RxBuffer[0]<<8) + Si705x_I2C3_RxBuffer[1];
    Temperature_board = ((17572*Temperature_code)/65536) - 4685;  // HEX temperature in degrees Celsius (x100)
    if(Temperature_code != Temperature_code_prev){
    Temperature_code_prev=Temperature_code;
    }
  }
}





// -----------------------------------------------------------------------------------------------------------------------------
// ADC
// -----------------------------------------------------------------------------------------------------------------------------
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
  //HAL_ADC_Stop_DMA(&hadc1);
  adc_measure.STRAIN1 = (*(adc_values+0));//*0.687+58.5;     // mV
  adc_measure.STRAIN2 = (*(adc_values+1));//*0.687+58.5;     // mV
  adc_measure.STRAIN3 = (*(adc_values+2));//*0.687+58.5;     // mV
  
  //HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_values, 6);
  //calcMean();
  //CANBUS();
}

void calcMean(){
  adc_samples.STRAIN1[adc_sample] = adc_measure.STRAIN1;
  adc_samples.STRAIN2[adc_sample] = adc_measure.STRAIN2;
  adc_samples.STRAIN3[adc_sample] = adc_measure.STRAIN3;
  
  if(adc_sample < (nr_adc_sample-1))  {adc_sample++;}
  else                                {adc_sample=0;}
  
  mean.STRAIN1=0;
	mean.STRAIN2=0;
	mean.STRAIN3=0;
  
  for(uint16_t i=0; i < nr_adc_sample; i++){
    mean.STRAIN1 += adc_samples.STRAIN1[i];
    mean.STRAIN2 += adc_samples.STRAIN2[i];
    mean.STRAIN3 += adc_samples.STRAIN3[i];
  }
  
  mean.STRAIN1 = mean.STRAIN1 / nr_adc_sample;
  mean.STRAIN2 = mean.STRAIN2 / nr_adc_sample;
  mean.STRAIN3 = mean.STRAIN3 / nr_adc_sample;
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
    //HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	}
  
}

void TIMER_PROG(uint16_t value){     // example: value=10000 -> 1s, value=1000 -> 100ms, value=100 -> 10ms  
  htim16.Init.Period = value;
  HAL_TIM_Base_Init(&htim16);
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
  
  if(REG_count <= REG_max_count){
    REG_value_tran[REG_count]=REGISTER;
    REG_count++;
    if(REG_count > REG_max_count)   {REG_count=0; REG_GO=1;}
  }
  
  for(i=0; i <= REG_max_count; i++){
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
