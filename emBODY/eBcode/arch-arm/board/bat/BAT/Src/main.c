/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
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
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "define.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// ----------------------------------------------------------------------------
// Firmware Version
// ----------------------------------------------------------------------------
char Firmware_vers = 1;
char Revision_vers = 1;
char Build_number  = 0;

/* Private variables ---------------------------------------------------------*/
typedef struct 
{
  uint32_t V_VINPUT;
  uint32_t V_EXTPS;
  uint32_t V_BATTERY;
  uint32_t V_V12board;
  uint32_t V_V12motor;
  int32_t I_V12board;
  int32_t I_V12motor;
  int32_t I_HSM;
  int32_t Temp_sensor;
}adc_measure_t;

adc_measure_t adc_measure = {0};  // initialize all adc values to 0
adc_measure_t mean = {0};         // initialize all average values to 0
uint32_t adc_values[9];           // contains all ADC channels conversion

#define nr_adc_sample (uint16_t) 400 // number of samples for the average measure
uint16_t adc_sample = 0;

typedef struct 
{
  uint32_t V_VINPUT[nr_adc_sample];
  uint32_t V_EXTPS[nr_adc_sample];
  uint32_t V_BATTERY[nr_adc_sample];
  uint32_t V_V12board[nr_adc_sample];
  uint32_t V_V12motor[nr_adc_sample];
  int32_t I_V12board[nr_adc_sample];
  int32_t I_V12motor[nr_adc_sample];
  int32_t I_HSM[nr_adc_sample];
}fifo;

fifo adc_samples = {0};

#ifdef BAT_R1
uint32_t VTH[7]={23000, 24000, 25000, 26000, 27000, 28000, 29000};   // threshold in mV R1 Battery
uint16_t Battery_high=4200*7;   // 7s5p battery
uint16_t Battery_low=3300*7;    // 7s5p battery
#endif 

#ifdef BAT_iCub3
uint32_t VTH[7]={32000, 34000, 36000, 38000, 40000, 42000, 44000};   // threshold in mV iCub 2.5 Battery
uint16_t Battery_high=4200*10;   // 10s3p battery
uint16_t Battery_low=3300*10;    // 10s3p battery
#endif 

uint32_t vtol=100;  // voltage tolerance for hysteresis
uint32_t vhyst=0;    // voltage hysteresis
uint16_t Battery_charge;

uint8_t PB1_debounce    = 0;
uint8_t PB1_restart     = 0;
uint8_t PB1_status      = 1;
uint8_t PB1_counter_debounce;

uint8_t PB2_debounce    = 0;
uint8_t PB2_restart     = 0;
uint8_t PB2_status      = 1;
uint8_t PB2_counter_debounce;

uint8_t int_fault         = 0;
uint8_t filter_fault_gpio = 0;
uint8_t fault_button      = 0;

uint8_t timer_debounce  = 10;     // 10x (10ms timer) = 100ms
uint8_t timer_restart   = 100;    // 100x (10ms timer) = 1s
uint16_t timer_delay_dcdc  = 0;           // delay between switching on V12board and then V12motor
uint16_t timer_delay_board = 0;
uint16_t timer_delay_motor = 0;
uint16_t timer_delay_board_max = 500;
uint16_t timer_delay_motor_max = 10000;
uint16_t timer_delay_dcdc_max  = 10000;   // delay between switching on regulator

uint32_t blink_ds=100;
uint8_t toggle_1s = 0;
uint8_t toggle_100ms = 0;

uint16_t I_V12board_MAX      = 9000;     // threshold in mA
uint16_t I_V12motor_MAX      = 6500;     // threshold in mA
uint16_t I_HSM_MAX           = 20000;
uint16_t timer_fault_board   = 0;
uint16_t timer_fault_motors  = 0;
uint16_t timer_fault_HSM     = 0;
uint16_t timer_mosfet_broken = 0;
uint16_t timer_rearmHSM      = 0;
uint16_t rearmHSM_timevalue  = 20000;    // (value in second) = rearmHSM_timevalue * (loop in ms where is "timer_rearmHSM++")

uint8_t V12board      = 0;		// DCDC board control
uint8_t V12board_F    = 0;		// fault
uint8_t V12motor      = 0;		// DCDC motor control
uint8_t V12motor_F    = 0;		// fault
uint8_t HSM_broken    = 0;		// HSM transistors broken
uint8_t HSM           = 0;		// HSM control
uint8_t DCrestart     = 0;

uint8_t V12board_bdc  = 1;    // +++++++++++++++++++++++++++++++++++++++++++++ da cambiare logica ++++++++++++++++++
uint8_t V12motor_bdc  = 1;    // +++++++++++++++++++++++++++++++++++++++++++++ da cambiare logica ++++++++++++++++++

uint8_t state_fsm        = START;
uint8_t state_fsm_boards = START;
uint8_t state_fsm_motors = START;
uint8_t state_fsm_prev;
uint8_t state_fsm_boards_prev;
uint8_t state_fsm_motors_prev;
uint8_t timerFSM      = 0;
uint8_t timerFSM_boards = 0;
uint8_t timerFSM_motors = 0;


uint8_t DCDC_status_A = 0;
uint8_t DCDC_status_B = 0;
uint8_t DCDC_ctrl     = 0;
uint8_t HSM_PG        = 0;
uint8_t HSM_F         = 0;

__IO ITStatus UartReady = RESET;
uint16_t timeout    = 0;
uint16_t time_delay = 500;

uint8_t can_message = 0;

uint8_t RN4020_Reset[] = "SF,1,2\r\n";
uint8_t RN4020_Reboot[] = "R,1\r\n";
uint8_t RN4020_Setting_A[] = "SS,C0000000\r\n";		// Support Device Info and Battery as server
//uint8_t RN4020_Setting_A[] = "SS,30000000\r\n";		// Support Heart Rate and Health Thermometer services as server. (Page 93 datasheet)
uint8_t RN4020_Setting_B[] = "SR,00000000\r\n";
//uint8_t RN4020_Setting_B[] = "SR,92000000\r\n";		// Set device as central, support MLDP and enable, UART flow control
//uint8_t RN4020_Setting_B[] = "SR,32000000\r\n";		// Set device as peripheral with automatic advertisement, and support for MLDP and flow control features
uint8_t RN4020_Setting_C[] = "SN,bat_board\r\n";
uint8_t RN4020_Advertise[] = "A\r\n";
uint8_t aRxBuffer[50]={0};
char *RN_start = "CMD\r\n";
char *RN_reboot = "Reboot\r\n";
char *RN_ok = "AOK\r\n";

uint16_t uOLED_Parameters[14] = {0};
uint16_t Display_qualifier = 0x01;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static void Error_Handler(void);
static void CAN_Config(void);
void CANBUS(void);
void Display_uOLED_128_G2(void);
void Bluetooth(void);
void WaitUartResponse(uint8_t* response);
static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
void displayCharge(void);
void calcMean(void);
void lightupLed(uint16_t lightup);
void checkHSM(void);
void dcdc_management(void);
void FAULT_CHECK(void);
	
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_CAN1_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_TIM13_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  SystemClock_Config();

  /* USER CODE BEGIN 2 */
//  HAL_TIM_Base_Start_IT(&htim6);    // 100us
  HAL_TIM_Base_Start_IT(&htim7);    // 1ms
  HAL_TIM_Base_Start_IT(&htim10);   // 10ms
  HAL_TIM_Base_Start_IT(&htim11);   // 100ms
  HAL_TIM_Base_Start_IT(&htim13);   // 1s
  HAL_TIM_IRQHandler(&htim1);
  HAL_TIM_IRQHandler(&htim2);
//  HAL_TIM_IRQHandler(&htim6);
  HAL_TIM_IRQHandler(&htim7);  
  HAL_TIM_IRQHandler(&htim10);
  HAL_TIM_IRQHandler(&htim11);
  HAL_TIM_IRQHandler(&htim13);
  
  //HAL_ADC_Start(&hadc1);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_values, 9);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //Bluetooth();
  CAN_Config(); 
  V5_EN_ON;
  RST_DSPL_ON;
  FAULT_REM_ON;
  LED_BT_OFF;
  
  uOLED_Parameters[0]  = ((sizeof(uOLED_Parameters))<<8) +  Display_qualifier; 
  uOLED_Parameters[1] = (Firmware_vers<<8) + (Revision_vers<<4) + (Build_number);
  
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */


// -----------------------------------------------------------------------------------------------------------------------------
// Finite state machine - DCDC power management - Target BAT_iCub3
// -----------------------------------------------------------------------------------------------------------------------------
#ifdef BAT_iCub3
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
				state_fsm_boards = READY;
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
      if(timer_delay_board < timer_delay_board_max){
        timer_delay_board++;
      }
      else{
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
      if(timer_delay_motor < timer_delay_motor_max){
        timer_delay_motor++;
      }
      else{
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
      else                {PB2_LED_GREEN;}
		}
    break;
		
		case FAULT_MOTORS:
		{
      EN_V12motor_OFF;
      HSM_EN_OFF;
      V12motor=0;
			HSM=0;
      if(toggle_100ms)  {PB2_LED_RED;}
			else              {PB2_LED_OFF;}
		}
    break;
    
    default:
		{
      
    }
		break;
	}

  DCDC_ctrl = ((V12board		& 0x01) << 7) +
              ((V12motor		& 0x01)	<< 5) +
              ((HSM           & 0x01)	<< 3) +
              (((PB1_restart || PB2_restart) & 0x01)	<< 0);
	
  if(DCrestart){
    PB1_restart=0;
    PB2_restart=0;
  }
}
#endif 


// -----------------------------------------------------------------------------------------------------------------------------
// Finite state machine - DCDC power management - Target BAT_R1
// -----------------------------------------------------------------------------------------------------------------------------
#ifdef BAT_R1
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
			if      (HSM)                         	{state_fsm = ON_BOARD;}             // if HSM is on, switch off
			else if (!HSM && V12board && V12motor)	{state_fsm = ON_BOARD_MOTORS;}      // else if V12motor is off and V12board is on, switch on
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
      EN_V12board_ON;                             // switch on V12board
      EN_V12motor_ON;                             // switch on V12motor
      HSM_EN_ON;                                  // switch on HSM
      
      V12board=1;             // switch on V12board
			V12motor=1;             // switch on V12motor
			HSM=1;                  // switch on HSM
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

  DCDC_ctrl = ((V12board		& 0x01) << 7) +
              ((V12motor		& 0x01)	<< 5) +
              ((HSM           & 0x01)	<< 3) +
              (((PB1_restart || PB2_restart) & 0x01)	<< 0);
    
  if(DCrestart){
    PB1_restart=0;
    PB2_restart=0;
  }
}
#endif 





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
  }
  if(htim->Instance==TIM7){         // timer 1ms
    CANBUS();
    dcdc_management();
    //HAL_TIM_Base_Stop_IT(&htim10);
    //timeout=1;
  }
  if(htim->Instance==TIM10){        // timer 10ms
    displayCharge();
    checkHSM();
    #ifdef BAT_iCub3
      FAULT_CHECK();
    #endif
  }
  if(htim->Instance==TIM11){        // timer 100ms
    toggle_100ms = toggle_100ms^1;
  }
  if(htim->Instance==TIM13){        // timer 1s
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
    if(HAL_GPIO_ReadPin(HSM_nFLT) == 0)   {V48motor_FLT_ON;     HSM_F=1;}
    else                                  {V48motor_FLT_OFF;    HSM_F=0;}
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
}

// -----------------------------------------------------------------------------------------------------------------------------
// ADC
// -----------------------------------------------------------------------------------------------------------------------------
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
  //HAL_GPIO_TogglePin(LED_BT);
  //HAL_ADC_Stop_DMA(&hadc1);
  adc_measure.V_VINPUT    = (*(adc_values+0))*13.31;       	// mV
  adc_measure.V_EXTPS     = (*(adc_values+1))*13.31;       	// mV
  adc_measure.V_BATTERY   = (*(adc_values+2))*13.31;       	// mV
  adc_measure.V_V12board  = (*(adc_values+3))*3.75;       	// mV
  adc_measure.V_V12motor  = (*(adc_values+4))*3.75;         // mV
  adc_measure.I_V12board  = ((*(adc_values+5))-2060)*9;   // mA
  adc_measure.I_V12motor  = ((*(adc_values+6))-2040)*9;   // mA
  adc_measure.I_HSM       = ((*(adc_values+7))*8.5);       // mA
  
  //adc_measure.I_V12board  = (*(adc_values+5));
  //adc_measure.I_V12motor  = (*(adc_values+6));
  //adc_measure.I_HSM       = (*(adc_values+7));
  adc_measure.Temp_sensor = ((((*(adc_values+8))*3300)/4096)-760)/2.5+25;       	// Temperature (in °C) = {(VSENSE – V25) / Avg_Slope} + 25 (page 441)
  
  if(adc_measure.I_V12board > I_V12board_MAX){                    // Check for board overcurrent
    if(timer_fault_board < 10)  {timer_fault_board++;}
    else                        {V12board_F = 1;}
  }else{
    if(timer_fault_board > 0)   {timer_fault_board--;}
  }
        
  if(adc_measure.I_V12motor > I_V12motor_MAX){                    // Check for motors overcurrent
    if(timer_fault_motors < 10) {timer_fault_motors++;}
    else                        {V12motor_F = 1;}
  }else{
    if(timer_fault_motors > 0)  {timer_fault_motors--;}
  }
  
  if(adc_measure.I_HSM > I_HSM_MAX){                              // Check for HSM overcurrent
    if(timer_fault_HSM < 10)    {timer_fault_HSM++;}
    else                        {HSM_F = 1;}
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
// Display_uOLED_128_G2
// -----------------------------------------------------------------------------------------------------------------------------
void Display_uOLED_128_G2(){
//  uOLED_Voltages[0]=0x0001;
//  uOLED_Voltages[1]=mean.V_VINPUT;
//  uOLED_Voltages[2]=mean.V_EXTPS;
//  uOLED_Voltages[3]=mean.V_BATTERY;
//  uOLED_Voltages[4]=mean.V_V12board;
//  uOLED_Voltages[5]=mean.V_V12motor;
  
  //((bat_voltage-b)/a);
  
  if(mean.V_BATTERY > Battery_high)       {Battery_charge=100;}
  else if(mean.V_BATTERY < Battery_low)   {Battery_charge=0;}
  else                                    {Battery_charge = 100*(mean.V_BATTERY-Battery_low)/(Battery_high-Battery_low);}
  
  //uOLED_Parameters[0] 
  //uOLED_Parameters[1] 
  uOLED_Parameters[2] = (state_fsm<<8) + (state_fsm_boards<<4) + (state_fsm_motors);
  uOLED_Parameters[3]  = Battery_charge;
  uOLED_Parameters[4]  = mean.V_VINPUT;
  uOLED_Parameters[5]  = mean.V_EXTPS;
  uOLED_Parameters[6]  = mean.V_BATTERY;
  uOLED_Parameters[7]  = mean.V_V12board;
  uOLED_Parameters[8]  = mean.V_V12motor;
  uOLED_Parameters[9]  = mean.I_V12board;
  uOLED_Parameters[10] = mean.I_V12motor;
  uOLED_Parameters[11] = mean.I_HSM;
  
  uOLED_Parameters[12] = (100*HSM_PG) + (10*V12motor) + (V12board);
  uOLED_Parameters[13] = (100*HSM_F) + (10*V12motor_F) + (V12board_F);
  
  HAL_UART_Transmit_DMA(&huart1, (uint8_t*)uOLED_Parameters, sizeof(uOLED_Parameters));
  //HAL_UART_Transmit_IT(&huart1, (uint8_t*)voltages, 12);
}



// -----------------------------------------------------------------------------------------------------------------------------
// Bluetooth configuration
// -----------------------------------------------------------------------------------------------------------------------------
void Bluetooth(){
  HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
	HAL_GPIO_WritePin(BT_SW_WAKE, GPIO_PIN_SET);
	WaitUartResponse("CMD");		// CMD

	HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4020_Reset, sizeof(RN4020_Reset)-1);
	WaitUartResponse("AOK");		// AOK
  
  HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 8);
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4020_Reboot, sizeof(RN4020_Reboot)-1);
	WaitUartResponse("Reboot");		// REBOOT

	HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
	WaitUartResponse("CMD");		// CMD

	HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4020_Setting_A, sizeof(RN4020_Setting_A)-1);
	WaitUartResponse("AOK");		// AOK

	HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4020_Setting_B, sizeof(RN4020_Setting_B)-1);
	WaitUartResponse("AOK");		// AOK

	HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4020_Setting_C, sizeof(RN4020_Setting_C)-1);
	WaitUartResponse("AOK");		// AOK

	HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 8);
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4020_Reboot, sizeof(RN4020_Reboot)-1);
	WaitUartResponse("Reboot");		// REBOOT

	HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
	WaitUartResponse("CMD");		// CMD

	HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4020_Advertise, sizeof(RN4020_Advertise)-1);
	WaitUartResponse("AOK");		// AOK
  
  HAL_GPIO_WritePin(BT_SW_WAKE, GPIO_PIN_RESET);
}

// -----------------------------------------------------------------------------------------------------------------------------
// Bluetooth response
// -----------------------------------------------------------------------------------------------------------------------------
void WaitUartResponse(uint8_t* response)
{
	int i;
  timeout=0;
  //HAL_TIM_Base_Start_IT(&htim10);
  //HAL_TIM_IRQHandler(&htim10);
  
	while (UartReady != SET){     // && timeout != 1
		}
	UartReady = RESET;

	if(Buffercmp((uint8_t*)aRxBuffer, (uint8_t*)response, sizeof(response)) == 0){
		UartReady = SET;
	}

	while (UartReady != SET){     // && timeout != 1
      //HAL_GPIO_TogglePin(LED_BT);
		}
	UartReady = RESET;

	for(i=0; i<sizeof(aRxBuffer); i++)		{aRxBuffer[i]=0;}
}


// -----------------------------------------------------------------------------------------------------------------------------
// CAN configuration
// -----------------------------------------------------------------------------------------------------------------------------
static void CAN_Config(void)
{
  static CanTxMsgTypeDef        TxMessage;
  static CanRxMsgTypeDef        RxMessage;

  hcan1.pTxMsg = &TxMessage;
  hcan1.pRxMsg = &RxMessage;

  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-3- Configure Transmission process #####################################*/
  hcan1.pTxMsg->StdId = 0x621;
  hcan1.pTxMsg->ExtId = 0x01;
  hcan1.pTxMsg->RTR = CAN_RTR_DATA;
  //hcan1.pTxMsg->IDE = CAN_ID_EXT;
  hcan1.pTxMsg->IDE = CAN_ID_STD;
  hcan1.pTxMsg->DLC = 8;
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
      hcan1.pTxMsg->Data[1] = 0x02;               // Board type
      hcan1.pTxMsg->Data[2] = Firmware_vers;      // Firmware version
      hcan1.pTxMsg->Data[3] = Revision_vers;      // Revision version
      hcan1.pTxMsg->Data[4] = Build_number;       // Build number
      hcan1.pTxMsg->Data[5] = 0;
      hcan1.pTxMsg->Data[6] = 0;
      hcan1.pTxMsg->Data[7] = 0;
      HAL_CAN_Transmit_IT(&hcan1);
      can_message = 1;
    }
    break;
    
    case 0x01:
    {
      hcan1.pTxMsg->StdId   = 0x621;
      hcan1.pTxMsg->Data[0] = 0x80;
      hcan1.pTxMsg->Data[1] = 0x00;
      hcan1.pTxMsg->Data[2] = 0x00;
      hcan1.pTxMsg->Data[3] = (adc_measure.Temp_sensor >> 8) & 0xFF;
      hcan1.pTxMsg->Data[4] = adc_measure.Temp_sensor & 0xFF;
      hcan1.pTxMsg->Data[5] = 0;
      hcan1.pTxMsg->Data[6] = 0;
      hcan1.pTxMsg->Data[7] = 0;
      HAL_CAN_Transmit_IT(&hcan1);
      can_message = 2;
    }
    break;
                        
    case 0x02:
    {
      hcan1.pTxMsg->StdId   = 0x621;
      hcan1.pTxMsg->Data[0] = 0x81;               // Voltage message
      hcan1.pTxMsg->Data[1] = 0x00;
      hcan1.pTxMsg->Data[2] = (mean.V_VINPUT >> 8) & 0xFF;
      hcan1.pTxMsg->Data[3] = mean.V_VINPUT & 0xFF;
      hcan1.pTxMsg->Data[4] = (mean.V_EXTPS >> 8) & 0xFF;
      hcan1.pTxMsg->Data[5] = mean.V_EXTPS & 0xFF;
      hcan1.pTxMsg->Data[6] = (mean.V_BATTERY >> 8) & 0xFF;
      hcan1.pTxMsg->Data[7] = mean.V_BATTERY & 0xFF;
      HAL_CAN_Transmit_IT(&hcan1);
      can_message = 3;
    }
    break;
                        
    case 0x03:
    {
      hcan1.pTxMsg->StdId   = 0x621;
      hcan1.pTxMsg->Data[0] = 0x82;               // Voltage message
      hcan1.pTxMsg->Data[1] = 0x00;
      hcan1.pTxMsg->Data[2] = (mean.V_V12board >> 8) & 0xFF;
      hcan1.pTxMsg->Data[3] = mean.V_V12board & 0xFF;
      hcan1.pTxMsg->Data[4] = (mean.V_V12motor >> 8) & 0xFF;
      hcan1.pTxMsg->Data[5] = mean.V_V12motor & 0xFF;
      hcan1.pTxMsg->Data[6] = 0x00;
      hcan1.pTxMsg->Data[7] = 0x00;
      HAL_CAN_Transmit_IT(&hcan1);
      can_message = 4;
    }
    break;
                            
    case 0x04:
    {
      hcan1.pTxMsg->StdId   = 0x621;
      hcan1.pTxMsg->Data[0] = 0x83;               // Current message
      hcan1.pTxMsg->Data[1] = 0x00;
      hcan1.pTxMsg->Data[2] = (mean.I_V12board >> 8) & 0xFF;
      hcan1.pTxMsg->Data[3] = mean.I_V12board & 0xFF;
      hcan1.pTxMsg->Data[4] = (mean.I_V12motor >> 8) & 0xFF;
      hcan1.pTxMsg->Data[5] = mean.I_V12motor & 0xFF;
      hcan1.pTxMsg->Data[6] = (mean.I_HSM >> 8) & 0xFF;
      hcan1.pTxMsg->Data[7] = mean.I_HSM & 0xFF;
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
// -----------------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Transmission  complete callback in non blocking mode 
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan)
{
  HAL_GPIO_WritePin(CAN1_LED, toggle_100ms);
}





// -----------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */
static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
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
  HAL_GPIO_WritePin(LED0_BATTERY, ((!(lightup    & 0x01)) || blink));
  HAL_GPIO_WritePin(LED1_BATTERY, ((!(lightup>>1 & 0x01)) || blink));
  HAL_GPIO_WritePin(LED2_BATTERY, ((!(lightup>>2 & 0x01)) || blink));
  HAL_GPIO_WritePin(LED3_BATTERY, ((!(lightup>>3 & 0x01)) || blink));
  HAL_GPIO_WritePin(LED4_BATTERY, ((!(lightup>>4 & 0x01)) || blink));
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
            LED_BT_OFF;
            fault_button=0;
            filter_fault_gpio=0;
            int_fault=0;
          }
       }
       else{                // FAULT!!!!
          if(filter_fault_gpio<timer_debounce){             
              filter_fault_gpio++;                    
          }
          else if(filter_fault_gpio==timer_debounce){     // valid command
            LED_BT_ON;
            fault_button=1;
            filter_fault_gpio=0;
            int_fault=0;
          }   
      }
    }
}

// -----------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void){
}

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
