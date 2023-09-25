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
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "define.h"
#include "global_var.h"
#include "can_utility.h"
#include "BAT_B.h"
#include "Display_uOLED.h"
#include "RN4871.h"

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


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

//static void Error_Handler(void);
void CANBUS(void);
void displayCharge(void);
void calcMean(void);
void lightupLed(uint16_t lightup);
void checkHSM(void);
void dcdc_management(void);
void FAULT_CHECK(void);
void dcdcStatusUpdate(void);

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
  MX_I2C2_SMBUS_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_TIM15_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */

  //  HAL_TIM_Base_Start_IT(&htim6);    // 100us
      HAL_TIM_Base_Start_IT(&htim7);    // 1ms
      HAL_TIM_Base_Start_IT(&htim15);   // 10ms
      HAL_TIM_Base_Start_IT(&htim16);   // 100ms
      HAL_TIM_Base_Start_IT(&htim17);   // 1s
  HAL_TIM_IRQHandler(&htim1);
  HAL_TIM_IRQHandler(&htim2);
//  HAL_TIM_IRQHandler(&htim6);
  HAL_TIM_IRQHandler(&htim7);  
  HAL_TIM_IRQHandler(&htim15);
  HAL_TIM_IRQHandler(&htim16);
  HAL_TIM_IRQHandler(&htim17);
  
  // ----------------------------------------------------------------------------
  // Initialization
  // ----------------------------------------------------------------------------
  CAN_Config(); 
  Display_uOLED_init();
  //RN4871_Config();
  
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED); 
  HAL_Delay(100);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_values, 9);
  
  V5_EN_ON;
  RST_DSPL_ON;
  FAULT_REM_ON;
  LED_BT_OFF;
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C2;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
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
// Timers
// -----------------------------------------------------------------------------------------------------------------------------
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if(htim->Instance==TIM1){        // timer 10ms
    if(HAL_GPIO_ReadPin(CPU)==0 || HAL_GPIO_ReadPin(GPIO1)==0){
      if(PB1_counter_debounce<timer_debounce){
				PB1_counter_debounce++;
			}
      else if(PB1_counter_debounce==timer_debounce){
        PB1_debounce=1;
        PB1_counter_debounce++;
      }
      else if(PB1_counter_debounce<timer_restart && (V12board_F==1)){
        PB1_counter_debounce++;
      }
      else if(V12board_F==1){
        PB1_status=1;
        HAL_TIM_Base_Stop_IT(&htim1);
				PB1_restart=1;                          // comando valido per il restart dopo il fault
				PB1_debounce=0;
      }
    }
    else{
      PB1_status=1;
      HAL_TIM_Base_Stop_IT(&htim1);
    }
  }
  if(htim->Instance==TIM2){        // timer 10ms
    if(HAL_GPIO_ReadPin(MOTORS)==0 || HAL_GPIO_ReadPin(GPIO2)==0){
      if(PB2_counter_debounce<timer_debounce){
				PB2_counter_debounce++;
			}
      else if(PB2_counter_debounce==timer_debounce){
        PB2_debounce=1;
        PB2_counter_debounce++;
      }
      else if(PB2_counter_debounce<timer_restart && (V12motor_F==1 || HSM_F==1)){
        PB2_counter_debounce++;
      }
      else if(V12motor_F==1 || HSM_F==1){
        PB2_status=1;
        HAL_TIM_Base_Stop_IT(&htim2);
				PB2_restart=1;                          // comando valido per il restart dopo il fault
				PB2_debounce=0;
      }
    }
    else{
      PB2_status=1;
      HAL_TIM_Base_Stop_IT(&htim2);
    }
  }
  if(htim->Instance==TIM6){         // timer 100us
    // do nothing -> interrupt not used
  }
  if(htim->Instance==TIM7){         // timer 1ms
    dcdc_management();
    dcdcStatusUpdate();
    //HAL_TIM_Base_Stop_IT(&htim10);
    //timeout=1;
  }
  if(htim->Instance==TIM15){        // timer 10ms
    displayCharge();
    checkHSM();
    #ifdef BAT_B_iCub3
      FAULT_CHECK();
    #endif
  }
  if(htim->Instance==TIM16){        // timer 100ms
    CANBUS();
  }
  if(htim->Instance==TIM17){        // timer 1s
    toggle_1s = toggle_1s^1;
    Display_uOLED_128_G2();
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
// Push buttons
// -----------------------------------------------------------------------------------------------------------------------------
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if(GPIO_Pin==GPIO_PIN_7 || GPIO_Pin==GPIO_PIN_0){               // PB1 or external button GPIO1
    if(PB1_status==1){
      PB1_status=0;
      PB1_counter_debounce=0;
      HAL_TIM_Base_Start_IT(&htim1);   // 10ms
    }

  }
  if(GPIO_Pin==GPIO_PIN_10 || GPIO_Pin==GPIO_PIN_1){        // PB2 or external button GPIO2
    if(PB2_status==1){
      PB2_status=0;
      PB2_counter_debounce=0;
      HAL_TIM_Base_Start_IT(&htim2);   // 10ms
    }
  }
  if(GPIO_Pin==GPIO_PIN_13){        // HSM_nFLT 
    if(HAL_GPIO_ReadPin(HSM_nFLT) == 0)   
    {
        V48motor_FLT_ON;     
        HSM_F=1;
        HSM_HW_F = 1;
    }
    else
    {
        V48motor_FLT_OFF;    
        HSM_F=0;
    }
  }
  if(GPIO_Pin==GPIO_PIN_14){        // HSM_nPG
    if(HAL_GPIO_ReadPin(HSM_nPG) == 0)    {V48motor_PG_ON;      HSM_PG=1;}
    else                                  {V48motor_PG_OFF;     HSM_PG=0;}
  }
  
  if(GPIO_Pin==GPIO_PIN_6 || GPIO_Pin==GPIO_PIN_8 || GPIO_Pin==GPIO_PIN_9){        // FAULT_ICUB, FAULT_GPIO, JACK
    int_fault = 1;
  }
  
}

// -----------------------------------------------------------------------------------------------------------------------------
// USART
// -----------------------------------------------------------------------------------------------------------------------------
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  if(huart->Instance==USART2){
    UartReady = SET;      // Set transmission flag: trasfer complete
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
  if(huart->Instance==USART1){
    
  }
  if(huart->Instance==USART2){
    
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
// ADC
// -----------------------------------------------------------------------------------------------------------------------------
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
  HAL_GPIO_TogglePin(LED_BT);
  //HAL_ADC_Stop_DMA(&hadc1);
  
  adc_measure.V_VINPUT    = (*(adc_values+0))*13.31;       	// mV
  adc_measure.V_EXTPS     = (*(adc_values+1))*13.31;       	// mV
  adc_measure.V_BATTERY   = (*(adc_values+2))*13.31;       	// mV
  adc_measure.V_V12board  = (*(adc_values+3))*3.75;       	// mV
  adc_measure.V_V12motor  = (*(adc_values+4))*3.75;         // mV
  if( (*(adc_values+5)) > 2048) {adc_measure.I_V12board  = ((*(adc_values+5))-2048)*9;}  // mA
  else                          {adc_measure.I_V12board  = 0;}  
  if( (*(adc_values+6)) > 2048) {adc_measure.I_V12motor  = ((*(adc_values+6))-2048)*9;}  // mA
  else                          {adc_measure.I_V12motor  = 0;}  
  adc_measure.I_HSM       = ((*(adc_values+7))*8.5);       // mA
  adc_measure.Temp_sensor = (((*(adc_values+8))*80.5)-76000)/2.5+2500;       	// Temperature (in �C*100) = {(VSENSE � V25) / Avg_Slope} + 25 (page 441)
  //adc_measure.Temp_sensor = ((((*(adc_values+8))*3300)/4096)-760)/2.5+25;       	// Temperature (in �C) = {(VSENSE � V25) / Avg_Slope} + 25 (page 441)
  
//  adc_measure.V_VINPUT    = (*(adc_values+0));
//  adc_measure.V_EXTPS     = (*(adc_values+1));
//  adc_measure.V_BATTERY   = (*(adc_values+2));
//  adc_measure.V_V12board  = (*(adc_values+3));
//  adc_measure.V_V12motor  = (*(adc_values+4));
//  adc_measure.I_V12board  = (*(adc_values+5));
//  adc_measure.I_V12motor  = (*(adc_values+6));
//  adc_measure.I_HSM       = (*(adc_values+7));
//  adc_measure.Temp_sensor = (*(adc_values+8));
    
  if(adc_measure.I_V12board > I_V12board_MAX){                    // Check for board overcurrent
    if(timer_fault_board < 10)  
    {
        timer_fault_board++;
    }
    else                        
    {
        V12board_F = 1;
        Current_board_in_fault = adc_measure.I_V12board;
    }
  }else{
    if(timer_fault_board > 0)   {timer_fault_board--;}
    if(Current_board_in_fault != 0) {Current_board_in_fault = 0;}
  }
        
  if(adc_measure.I_V12motor > I_V12motor_MAX){                    // Check for motors overcurrent
    if(timer_fault_motors < 10) 
    {
        timer_fault_motors++;
    }
    else                        
    {
        V12motor_F = 1;
        Current_motor_in_fault = adc_measure.I_V12motor;
    }
  }else{
    if(timer_fault_motors > 0)  {timer_fault_motors--;}
    if(Current_motor_in_fault != 0) {Current_motor_in_fault = 0;}
  }
  
  if(adc_measure.I_HSM > I_HSM_MAX){                              // Check for HSM overcurrent
    if(timer_fault_HSM < 20)    
    {
        timer_fault_HSM++;
    }
    else                        
    {
        HSM_F = 1;
        HSM_SW_F = 1;
        Current_HSM_in_fault = adc_measure.I_HSM;
    }
  }else{
    if(timer_fault_HSM > 0)     {timer_fault_HSM--;}
  }
    
  calcMean();
}

void calcMean(){
  adc_samples.V_VINPUT[adc_sample]    = adc_measure.V_VINPUT;
  adc_samples.V_EXTPS[adc_sample]     = adc_measure.V_EXTPS;
  adc_samples.V_BATTERY[adc_sample]   = adc_measure.V_BATTERY;
  adc_samples.V_V12board[adc_sample]  = adc_measure.V_V12board;
  adc_samples.V_V12motor[adc_sample]  = adc_measure.V_V12motor;
  adc_samples.I_V12board[adc_sample]  = adc_measure.I_V12board;
  adc_samples.I_V12motor[adc_sample]  = adc_measure.I_V12motor;
  adc_samples.I_HSM[adc_sample]       = adc_measure.I_HSM;
  
  if(adc_sample < (nr_adc_sample-1))  {adc_sample++;}
  else                                {adc_sample=0;}
  
  mean.V_VINPUT=0;
  mean.V_EXTPS=0;
  mean.V_BATTERY=0;
  mean.V_V12board=0;
  mean.V_V12motor=0;
  mean.I_V12board=0;
  mean.I_V12motor=0;
  mean.I_HSM =0;
  
  for(uint16_t i=0; i < nr_adc_sample; i++){
    mean.V_VINPUT   += adc_samples.V_VINPUT[i];
    mean.V_EXTPS    += adc_samples.V_EXTPS[i];
    mean.V_BATTERY  += adc_samples.V_BATTERY[i];
    mean.V_V12board += adc_samples.V_V12board[i];
    mean.V_V12motor += adc_samples.V_V12motor[i];
    mean.I_V12board += adc_samples.I_V12board[i];
    mean.I_V12motor += adc_samples.I_V12motor[i];
    mean.I_HSM      += adc_samples.I_HSM[i];
  }
  
  mean.V_VINPUT   = mean.V_VINPUT   / nr_adc_sample;
  mean.V_EXTPS    = mean.V_EXTPS    / nr_adc_sample;
  mean.V_BATTERY  = mean.V_BATTERY  / nr_adc_sample;  
  mean.V_V12board = mean.V_V12board / nr_adc_sample;
  mean.V_V12motor = mean.V_V12motor / nr_adc_sample;
  mean.I_V12board = mean.I_V12board / nr_adc_sample;
  mean.I_V12motor = mean.I_V12motor / nr_adc_sample;
  mean.I_HSM      = mean.I_HSM      / nr_adc_sample;
}


// -----------------------------------------------------------------------------------------------------------------------------
// CAN messages management
// -----------------------------------------------------------------------------------------------------------------------------
// TODO: this method can be maybe changed by adding a for or a while loop
// messages should be sent from the BMS in polling with a frequence of 1Hz
// This method will just send the 5 CAN messages continuously at every board cycle (1ms)
// As an example, firmware version can be sent just one time or every x minutes
// The rest can be sent with a higher frequency
void CANBUS(void){
    
	switch(can_message){

#ifdef ENABLE_GUI_CUSTOM_MSG  // Define to be added as -DENABLE_GUI_CUSTOM_MSG in `Option for target... >> C/C++ >> Misc controls` for enabling all messages (Also the custom ones not related to protocol)
    case 0x00:  
    {
      TxData[0] = 0xFF;               // Firmware version message
      TxData[1] = 0x02;               // Board type
      TxData[2] = Firmware_vers;      // Firmware version
      TxData[3] = Revision_vers;      // Revision version
      TxData[4] = Build_number;       // Build number
      TxData[5] = 0;
      TxData[6] = 0;
      TxData[7] = 0;
      //HAL_CAN_Transmit_IT(&hcan1);
      if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK){
        /* Transmission request Error */
        Error_Handler();
      }      
      can_message = 1;
    }
    break;
    
    case 0x01:
    {
      TxData[0] = 0x80;
      TxData[1] = 0x00;
      TxData[2] = (adc_measure.Temp_sensor >> 8) & 0xFF;
      TxData[3] = adc_measure.Temp_sensor & 0xFF;
      TxData[4] = 0x00;
      TxData[5] = 0;
      TxData[6] = 0;
      TxData[7] = 0;
      if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK){
        /* Transmission request Error */
        Error_Handler();
      }
      can_message = 2;
    }
    break;
                        
    case 0x02:
    {
      TxData[0] = 0x81;               // Voltage message
      TxData[1] = 0x00;
      TxData[2] = (mean.V_VINPUT >> 8) & 0xFF;   
      TxData[3] = mean.V_VINPUT & 0xFF;
      TxData[4] = (mean.V_EXTPS >> 8) & 0xFF;        
      TxData[5] = mean.V_EXTPS & 0xFF;
      TxData[6] = (mean.V_BATTERY >> 8) & 0xFF;       
      TxData[7] = mean.V_BATTERY & 0xFF;
      if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK){
        /* Transmission request Error */
        Error_Handler();
      }
      can_message = 3;
    }
    break;
                        
    case 0x03:
    {
      TxData[0] = 0x82;               // Voltage message
      TxData[1] = 0x00;
      TxData[2] = (mean.V_V12board >> 8) & 0xFF; 
      TxData[3] = mean.V_V12board & 0xFF;
      TxData[4] = (mean.V_V12motor >> 8) & 0xFF;
      TxData[5] = mean.V_V12motor & 0xFF;
      TxData[6] = 0x00;
      TxData[7] = 0x00;
      if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK){
        /* Transmission request Error */
        Error_Handler();
      }
      can_message = 4;
    }
    break;
                            
    case 0x04:
    {
      TxData[0] = 0x83;               // Current message
      TxData[1] = 0x00;
      TxData[2] = (mean.I_V12board >> 8) & 0xFF;           
      TxData[3] = mean.I_V12board & 0xFF;
      TxData[4] = (mean.I_V12motor >> 8) & 0xFF;            
      TxData[5] = mean.I_V12motor & 0xFF;
      TxData[6] = (mean.I_HSM >> 8) & 0xFF;                 
      TxData[7] = mean.I_HSM & 0xFF;
      if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK){
        /* Transmission request Error */
        Error_Handler();
      }
      can_message = 5;
    }
    break;
    
    case 0x05:
#else
    case 0x00:   
#endif
    {
        // Battery Pack Info message
        vBatterydV = 0.01 * (mean.V_BATTERY);
        TxData_620[0] = vBatterydV & 0xFF;              // b7-b6 Battery pack voltage            
        TxData_620[1] = (vBatterydV >> 8) & 0xFF;
        TxData_620[2] = 0x00;                           // b5-b4 Instant current
        TxData_620[3] = 0x00;
        TxData_620[4] = Battery_charge & 0xFF;          // b3-b2 State of charge of battery
        TxData_620[5] = 0x00;
        TxData_620[6] = 0x00;                           // b1-b0 Average Temperature of the batteries
        TxData_620[7] = 0x00;
        if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader_620, TxData_620, &TxMailbox_620) != HAL_OK){
            /* Transmission request Error */
            Error_Handler();
        }
        
        can_message = 6;
    }
    break;
    
    case 0x06:
    {
        TxData_629[0] = DCDC_status_A & 0xFF;
        TxData_629[1] = DCDC_status_B & 0xFF;
        TxData_629[2] = 0x00;           
        TxData_629[3] = 0x00;
        TxData_629[4] = 0x00;            
        TxData_629[5] = 0x00;
        TxData_629[6] = 0x00;                 
        TxData_629[7] = 0x00;
        if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader_629, TxData_629, &TxMailbox_629) != HAL_OK){
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
// Light up leds to display battery voltage
// -----------------------------------------------------------------------------------------------------------------------------
void displayCharge(void){
    //int adc_buffer_last=adc_buffer;

    if(mean.V_VINPUT < (VTH[0]+vhyst)){
        if((VTH[0]-mean.V_VINPUT) > vtol)   {vhyst=0;}
        else                                {vhyst=vtol;}
        lightupLed(0x81);     // light up LED0
    }
    else if(mean.V_VINPUT < (VTH[1]+vhyst)){
        if((VTH[1]-mean.V_VINPUT) > vtol)   {vhyst=0;}
        else                                {vhyst=vtol;}
        lightupLed(0x81);     // light up LED0
    }
    else if(mean.V_VINPUT < (VTH[2]+vhyst)){
        if((VTH[2]-mean.V_VINPUT) > vtol)   {vhyst=0;}
        else                                {vhyst=vtol;}
        lightupLed(0x01);     // light up LED0
    }
    else if(mean.V_VINPUT < (VTH[3]+vhyst)){
        if((VTH[3]-mean.V_VINPUT) > vtol)   {vhyst=0;}
        else                                {vhyst=vtol;}
        lightupLed(0x03);     // light up LED1
    }
    else if(mean.V_VINPUT < (VTH[4]+vhyst)){
        if((VTH[4]-mean.V_VINPUT) > vtol)   {vhyst=0;}
        else                                {vhyst=vtol;}
        lightupLed(0x07);     // light up LED2
    }
    else if(mean.V_VINPUT < (VTH[5]+vhyst)){
        if((VTH[5]-mean.V_VINPUT) > vtol)   {vhyst=0;}
        else                                {vhyst=vtol;}
        lightupLed(0x0f);     // light up LED3
    }
    else if(mean.V_VINPUT < (VTH[6]+vhyst)){
        if((VTH[6]-mean.V_VINPUT) > vtol)   {vhyst=0;}
        else                                {vhyst=vtol;}
        lightupLed(0x1f);     // light up LED4
    }
    else{
        vhyst=0;
        lightupLed(0x9f);     // light up LED4
    }
}

// -----------------------------------------------------------------------------------------------------------------------------
// Turn on/off and blink V-meter leds
// -----------------------------------------------------------------------------------------------------------------------------
void lightupLed(uint16_t lightup){
  uint16_t blink = toggle_100ms & (lightup>>7 & 0x01);
  HAL_GPIO_WritePin(LED0_BATTERY, (GPIO_PinState)((!(lightup    & 0x01)) || blink));
  HAL_GPIO_WritePin(LED1_BATTERY, (GPIO_PinState)((!(lightup>>1 & 0x01)) || blink));
  HAL_GPIO_WritePin(LED2_BATTERY, (GPIO_PinState)((!(lightup>>2 & 0x01)) || blink));
  HAL_GPIO_WritePin(LED3_BATTERY, (GPIO_PinState)((!(lightup>>3 & 0x01)) || blink));
  HAL_GPIO_WritePin(LED4_BATTERY, (GPIO_PinState)((!(lightup>>4 & 0x01)) || blink));
}

// -----------------------------------------------------------------------------------------------------------------------------
// Check for HSM mosfet broken
// -----------------------------------------------------------------------------------------------------------------------------
void checkHSM(){
  if(HAL_GPIO_ReadPin(HSM_nFLT) == 0)   {V48motor_FLT_ON;}
  else                                  {V48motor_FLT_OFF;}
  if(HAL_GPIO_ReadPin(HSM_nPG) == 0)    {V48motor_PG_ON;}
  else                                  {V48motor_PG_OFF;}
}

// ----------------------------------------------------------------------------
// FAULT check
// ----------------------------------------------------------------------------
void FAULT_CHECK(void){
  if(int_fault){
    
    if(HAL_GPIO_ReadPin(JACK) == 1)   {FAULT_REM_ON;}     // check for the link of the fault push button
    else                              {FAULT_REM_OFF;}
    
    if( HAL_GPIO_ReadPin(FAULT_REM) == 1 && (
      ( HAL_GPIO_ReadPin(JACK) == 0 && HAL_GPIO_ReadPin(FAULT_GPIO) == 1) ||        // there isn't the jack
      ( HAL_GPIO_ReadPin(JACK) == 1 && HAL_GPIO_ReadPin(FAULT_GPIO) == 0) ))        // there is the jack
      {                     // NO FAULT
          if(filter_fault_gpio<timer_debounce){             
            filter_fault_gpio++;                    
          }
          else if(filter_fault_gpio==timer_debounce){     // valid command
            //LED_BT_OFF;
            fault_button=0;
            filter_fault_gpio=0;
            int_fault=0;
          }
       }
       else{// FAULT!!!!
          if(filter_fault_gpio<timer_debounce){             
              filter_fault_gpio++;                    
          }
          else if(filter_fault_gpio==timer_debounce){     // valid command
            //LED_BT_ON;
            fault_button=1;
            filter_fault_gpio=0;
            int_fault=0;
          }   
      }
    }
}




// -----------------------------------------------------------------------------------------------------------------------------
// Finite state machine - DCDC power management - Target BAT_B_iCub3
// -----------------------------------------------------------------------------------------------------------------------------
#ifdef BAT_B_iCub3
void dcdc_management(void){
	
  // ----- EVENTS -------------------------------------------------- //
  if(!V12board_F){
    if(PB1_debounce){
      PB1_debounce=0;
      if(V12board)    {state_fsm_boards = OFF_BOARD;}
      else            {state_fsm_boards = ON_BOARD;}
    }
  }
  else{
    if(PB1_restart){
      state_fsm_boards = ON_BOARD;
      V12board_F=0;
      PB1_restart=0;
    }
    else{
      state_fsm_boards = FAULT_BOARD;
    }
  }
  
  if(!V12motor_F && !HSM_F){
    if(PB2_debounce){
      PB2_debounce=0;
      if(V12motor)    {state_fsm_motors = OFF_MOTORS;}
      else            {state_fsm_motors = ON_MOTORS;}
    }
  }
  else{
    if(PB2_restart){
      state_fsm_motors = ON_MOTORS;
      V12motor_F=0;
      PB2_restart=0;
    }
    else{
      state_fsm_motors = FAULT_MOTORS;
    }
  }	
	
  // ----- FINITE STATE MACHINE ------------------------------------ //
  
	switch(state_fsm_boards){
	
        case START:
        {
            timerFSM_boards++;
            state_fsm_boards_prev = START;

            if(timerFSM_boards >= 10){
                state_fsm_boards = READY;   // after 10ms boards are ready -> are we sure of thise timing?
            }
        }
        break;

        case READY:
        {
            timerFSM_boards = 0;
            PB1_LED_RED;
        }
        break;

        case OFF_BOARD:
        {
            EN_V12board_OFF;
            V12board=0;
            PB1_LED_RED;
        }
        break;

        case ON_BOARD:
        {
            if(timer_delay_board < timer_delay_board_max)
            {
                timer_delay_board++;
            }
            else
            {
                EN_V12board_ON;
                V12board=1;
                timer_delay_board=0;
            }
            if(V12board==0){
                if(toggle_100ms)  {PB1_LED_GREEN;}
                else              {PB1_LED_OFF;}
            }
            else                {PB1_LED_GREEN;}
        }
        break;

        case FAULT_BOARD:
        {
            EN_V12board_OFF;
            V12board=0;
            if(toggle_100ms)  {PB1_LED_RED;}
            else              {PB1_LED_OFF;}
            // send CAN message before the set timer of 100ms if a FAULT happens 
            CANBUS();
        }
        break;

        default:
        {
        }
        break;
    }
  
  
    switch(state_fsm_motors){
	
        case START:
        {
            timerFSM_motors++;
            state_fsm_motors_prev = START;
            
            if(timerFSM_motors >= 10){
                state_fsm_motors = READY;
            }
        }
        break;
            
        case READY:
        {
            timerFSM_motors = 0;
            PB2_LED_RED;			
        }
        break;

        case OFF_MOTORS:
        {
            EN_V12motor_OFF;
            HSM_EN_OFF;
            V12motor=0;
            HSM=0;
            PB2_LED_RED;
        }
        break;

        case ON_MOTORS:
        {
            HSM_HW_F = 0;
            HSM_SW_F = 0;
            
            if(timer_delay_motor < timer_delay_motor_max){
                timer_delay_motor++;
            }
            else
            {   
                EN_V12motor_ON;
                HSM_EN_ON;
                V12motor=1;
                HSM=1;
                timer_delay_motor=0;
            }
            if(HSM_PG==0){
                if(toggle_100ms)  {PB2_LED_GREEN;}
                else              {PB2_LED_OFF;}
            }
            else                  {PB2_LED_GREEN;}
        }
        break;

        case FAULT_MOTORS:
        {
            EN_V12motor_OFF;
            HSM_EN_OFF;
            V12motor=0;
            HSM=0;
            if(toggle_100ms)      {PB2_LED_RED;}
                else              {PB2_LED_OFF;}
            // send CAN message before the set timer of 100ms if a FAULT happens
            CANBUS();
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
// Finite state machine - DCDC power management - Target BAT_B_R1
// -----------------------------------------------------------------------------------------------------------------------------
#ifdef BAT_B_R1
void dcdc_management(void){
	
	// ----- EVENTS -------------------------------------------------- //
  if(V12board_F==0 && V12motor_F==0 && HSM_F==0){
		if(PB1_debounce){
			PB1_debounce=0;
			if(V12board || V12motor)            {state_fsm = OFF_BOARD_MOTORS;}     // if V12board or V12motor is on, switch off
			else                                {state_fsm = ON_BOARD;}             // else if V12board or V12motor is off, switch on
		}
		
		if(PB2_debounce){
			PB2_debounce=0;
			if      (HSM || (state_fsm == ON_BOARD_MOTORS))     {state_fsm = ON_BOARD;}             // if HSM is on, switch off
			else if (!HSM && V12board && V12motor)              {state_fsm = ON_BOARD_MOTORS;}      // else if V12motor is on and V12board is on, switch on
		}	
	}

	else{
		if(V12board_F==0 && V12motor_F==0 && HSM_F==1){
			if(PB1_debounce){
				PB1_debounce=0;
				if(V12board && V12motor)          {state_fsm = OFF_BOARD_MOTORS;}     // if V12board and V12motor are on, switch off
				else                              {state_fsm = ON_BOARD;}             // else if V12board and V12motor are off, switch on
			}
			else{
				if(PB2_restart==0)                {state_fsm = FAULT_MOTORS;}
				else{
					if(V12board && V12motor)				{state_fsm = ON_BOARD_MOTORS;}      // if V12board and V12motor are on, switch off
					else                            {state_fsm = OFF_BOARD_MOTORS;}     // else if V12board and V12motor are off, switch on

          HSM_F=0;
          PB2_restart=0;
				}
			}
		}
		else if(V12board_F==1 || V12motor_F==1){
			if(PB1_restart==0){
				if(HSM_F==1)     {state_fsm = FAULT_BOARD_MOTORS;}
				else             {state_fsm = FAULT_BOARD;}
			}
			else if(PB1_restart==1){
				state_fsm = ON_BOARD;
				V12board_F=0;
				V12motor_F=0;
        PB1_restart=0;
        PB2_debounce=0;
			}
		}
	}
	
  // ----- FINITE STATE MACHINE ------------------------------------ //
	
	switch(state_fsm){
	
		case START:
		{
      timerFSM++;
			state_fsm_prev = START;
		
			if(timerFSM >= 10){
				state_fsm = READY;
			}
    }
		break;
		
		case READY:
		{
      timerFSM = 0;
			PB1_LED_RED;
			PB2_LED_RED;			
		}
    break;
		
		case OFF_BOARD_MOTORS:
		{
      EN_V12board_OFF;        // switch off V12board
      EN_V12motor_OFF;        // switch off V12motor
      HSM_EN_OFF;             // switch off HSM
      
      V12board=0;             // switch off V12board
			V12motor=0;             // switch off V12motor
			HSM=0;                  // switch off HSM
			PB1_LED_RED;
			PB2_LED_RED;
    }
    break;
		
		case ON_BOARD:
    {
			EN_V12board_ON;                             // switch on V12board
      //EN_V12motor_OFF;                            // switch off V12motor  - for iCub!
      if(V12motor==0){
        if(timer_delay_dcdc < timer_delay_dcdc_max){
          timer_delay_dcdc++;
        }
        else{
          EN_V12motor_ON;                            // switch on V12motor     - for CER!
          V12motor=1;             // switch on V12motor       - for CER!
          timer_delay_dcdc=0;
        }
      }
      
      HSM_EN_OFF;                                 // switch off HSM
      V12board=1;             // switch on V12board
			
      // V12motor=0;             // switch off V12motor   - for iCub!
			HSM=0;                  // switch off HSM
      //if(V12board==0){                                  // - for iCub
      if(V12board==0 || V12motor==0){                     // - for CER
        if(toggle_100ms)  {PB1_LED_GREEN;}
        else              {PB1_LED_OFF;}
			}
      else                {PB1_LED_GREEN;}
			PB2_LED_RED;
		}
    break;
		
		case ON_BOARD_MOTORS:
    {
      if(timer_delay_motor < timer_delay_motor_max){
        timer_delay_motor++;
      }
      else{
        EN_V12board_ON;                             // switch on V12board
        EN_V12motor_ON;                             // switch on V12motor
        HSM_EN_ON;                                  // switch on HSM
        
        V12board=1;             // switch on V12board
        V12motor=1;             // switch on V12motor
        
        HSM=1;                  // switch on HSM
        timer_delay_motor=0;
      }
      //if(V12board==0){                          // - for iCub
      if(V12board==0 || V12motor==0){             // - for CER
        if(toggle_100ms)  {PB1_LED_GREEN;}
        else              {PB1_LED_OFF;}
			}
      else                {PB1_LED_GREEN;}
      
      //if(V12motor==0 || HSM_PG==0){             // - for iCub
      if(HSM_PG==0){                              // - for CER
        if(toggle_100ms)  {PB2_LED_GREEN;}
        else              {PB2_LED_OFF;}
			}
      else                {PB2_LED_GREEN;}
		}
    break;
		
		case FAULT_BOARD:
		{
      EN_V12board_OFF;                            // switch off V12board
      EN_V12motor_OFF;                            // switch off V12motor
      HSM_EN_OFF;                                 // switch off HSM
      
      V12board=0;             // switch off V12board
			V12motor=0;             // switch off V12motor
			HSM=0;                  // switch off HSM
			if(toggle_100ms)  {PB1_LED_RED;}
			else              {PB1_LED_OFF;}
			PB2_LED_RED;
		}
    break;
			
		case FAULT_MOTORS:
		{
      //EN_V12board_ON;                             // switch on V12board
      //EN_V12motor_OFF;                            // switch off V12motor
      HSM_EN_OFF;                                 // switch off HSM
    
      //V12board=1;           // switch on V12board
			//V12motor=0;           // switch off V12motor
			HSM=0;                  // switch off HSM
      PB1_LED_GREEN;
			if(toggle_100ms)  {PB2_LED_RED;}
			else              {PB2_LED_OFF;}
		}
    break;
			
		case FAULT_BOARD_MOTORS:
		{
      EN_V12board_OFF;        // switch off V12board
      EN_V12motor_OFF;        // switch off V12motor
      HSM_EN_OFF;             // switch off HSM
      
      V12board=0;             // switch off V12board
			V12motor=0;             // switch off V12motor
			HSM=0;                  // switch off HSM
			if(toggle_100ms)  {PB1_LED_RED;       PB2_LED_RED;}
			else              {PB1_LED_OFF;       PB2_LED_OFF;}
		}
    break;
    
    default:
		{
      
    }
		break;
    
	}
}
#endif 


void dcdcStatusUpdate(void)
{
    DCDC_ctrl = ((V12board		& 0x01) << 7) +
              ((V12motor		& 0x01)	<< 5) +
              ((HSM           & 0x01)	<< 3) +
              (((PB1_restart || PB2_restart) & 0x01)	<< 0);
    
    
    DCDC_status_A = ((V12board      & 0x01) << 7) +
                    ((V12board_F    & 0x01) << 6) +
                    ((V12motor      & 0x01) << 5) +
                    ((V12motor_F    & 0x01) << 4) +
                    ((HSM           & 0x01) << 3) +
                    ((HSM_PG        & 0x01) << 2) +
                    ((HSM_F         & 0x01) << 1) +
                    ((HSM_broken    & 0x01) << 0);
    

    DCDC_status_B = (( HSM_SW_F    & 0x01) << 3) +
                    (( HSM_HW_F    & 0x01) << 2) +
                    (( PB1_restart & 0x01) << 1) + 
                    (( PB2_restart    & 0x01) << 0);

    if(DCrestart){
        PB1_restart=0;
        PB2_restart=0;
    }
}


void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef* hcan1)
{
  HAL_GPIO_WritePin(CAN1_LED, (GPIO_PinState)toggle_100ms);
  //HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_values, 6);
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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
