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
#include "MPU9250.h"
#include "TLV493D.h"
#include "AD7147.h"
#include "AD7147RegMap.h"
#include "I2C_Multi_SDA.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct 
{
  I2C_HandleTypeDef *hi2c;
  uint16_t DevAddress;
  uint8_t *pData;
  uint16_t Size;
  uint8_t Busy;
}I2C_STATUS;

I2C_STATUS I2C_TxStatus = {0};  // initialize all values to 0


typedef struct _t_skin {
    uint16_t triangle;
    uint8_t value[10];
} t_skin;

t_skin skin;

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

#define MODE_SKIN 0
int mode = MODE_SKIN ;

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

void CANBUS(void);
void TIMER_Init(void);
void I2C_TX(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
void UpdateTouchMap(void);
void SkinRawData(void);
void FillMessages8bit(unsigned char Channel,unsigned char triangleN);

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
  MX_CAN1_Init();
  MX_I2C1_Init();
  MX_I2C3_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
  
  TIMER_Init();
  
  CAN_Config();
  MPU9250_AD0_0;
  MPU9250_Init();
  HAL_Delay(10);
  TLV493D_Init(U1);
  TLV493D_Init(U2);
  
  TrianglesInit(0);
  
  HAL_Delay(10);
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    MPU9250();
    HAL_Delay(2);
    
    TLV493D_ReadRegisters(U1);
    HAL_Delay(2);
    
    TLV493D_ReadRegisters(U2);
    HAL_Delay(2);
    
    CANBUS();
    
    ServiceAD7147Isr(0);        // read sensors AD7147 channel 0
    //UpdateTouchMap();
    //TouchMapHistoryCheck();
    //SkinRawData();
    HAL_Delay(500);
    
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C3;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
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
  
      can_message = 0;
      TxData[0] = 0xFF;               //
      TxData[1] = can_message;        //
      TxData[2] = Firmware_vers;      // Firmware version
      TxData[3] = Revision_vers;      // Revision version
      TxData[4] = Build_number;       // Build number
      TxData[5] = 0;
      TxData[6] = 0;
      TxData[7] = 0;
      if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK){
        /* Transmission request Error */
        Error_Handler();
      }  
      
      while(CAN_TxComplete == 0)  {}
      CAN_TxComplete=0;
        
      can_message = 1;
      TxData[0] = 0xFF;
      TxData[1] = can_message;
      TxData[2] = TLV493D_REG[U1].Bx>>8;
      TxData[3] = TLV493D_REG[U1].Bx;
      TxData[4] = TLV493D_REG[U1].By>>8;
      TxData[5] = TLV493D_REG[U1].By;
      TxData[6] = TLV493D_REG[U1].Bz>>8;
      TxData[7] = TLV493D_REG[U1].Bz;
      if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK){
        /* Transmission request Error */
        Error_Handler();
      }
      
      while(CAN_TxComplete == 0)  {}
      CAN_TxComplete=0;
        
      can_message = 2;  
      TxData[0] = 0xFF;
      TxData[1] = can_message;
      TxData[2] = TLV493D_REG[U1].Temp>>8;
      TxData[3] = TLV493D_REG[U1].Temp;
      TxData[4] = 0;
      TxData[5] = 0;
      TxData[6] = 0;
      TxData[7] = 0;
      if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK){
        /* Transmission request Error */
        Error_Handler();
      }
      
      while(CAN_TxComplete == 0)  {}
      CAN_TxComplete=0;
        
      can_message = 3;
      TxData[0] = 0xFF;
      TxData[1] = can_message;
      TxData[2] = TLV493D_REG[U2].Bx>>8;
      TxData[3] = TLV493D_REG[U2].Bx;
      TxData[4] = TLV493D_REG[U2].By>>8;
      TxData[5] = TLV493D_REG[U2].By;
      TxData[6] = TLV493D_REG[U2].Bz>>8;
      TxData[7] = TLV493D_REG[U2].Bz;
      if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK){
        /* Transmission request Error */
        Error_Handler();
      }
      
      while(CAN_TxComplete == 0)  {}
      CAN_TxComplete=0;
        
      can_message = 4;  
      TxData[0] = 0xFF;
      TxData[1] = can_message;
      TxData[2] = TLV493D_REG[U2].Temp>>8;
      TxData[3] = TLV493D_REG[U2].Temp;
      TxData[4] = 0;
      TxData[5] = 0;
      TxData[6] = 0;
      TxData[7] = 0;
      if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK){
        /* Transmission request Error */
        Error_Handler();
      }
      
      while(CAN_TxComplete == 0)  {}
      CAN_TxComplete=0;
        
      can_message = 5;
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
      
      while(CAN_TxComplete == 0)  {}
      CAN_TxComplete=0;

      for(int i=0; i<16; i++){                          // To send the most significative 8 bits of the raw data (16bits) of the all taxels
        for(int j=0; j<2; j++){
          TxData[0] = i;
          TxData[1] = j;
          TxData[2] = ((AD7147Registers[0][i][j*6+0]) >> 8) & 0xff;
          TxData[3] = ((AD7147Registers[0][i][j*6+1]) >> 8) & 0xff;
          TxData[4] = ((AD7147Registers[0][i][j*6+2]) >> 8) & 0xff;
          TxData[5] = ((AD7147Registers[0][i][j*6+3]) >> 8) & 0xff;
          TxData[6] = ((AD7147Registers[0][i][j*6+4]) >> 8) & 0xff;
          TxData[7] = ((AD7147Registers[0][i][j*6+5]) >> 8) & 0xff;
          if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK){
            /* Transmission request Error */
            Error_Handler();
          }
          
          while(CAN_TxComplete == 0)  {}
          CAN_TxComplete=0;
        }
      }
      //   
}



// -----------------------------------------------------------------------------------------------------------------------------
// I2C Callback
// -----------------------------------------------------------------------------------------------------------------------------
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
  if(I2cHandle->Instance==I2C1){
    if(I2C_TxStatus.DevAddress == MPU9250_I2C_ADDRESS){
      HAL_I2C_Master_Receive_DMA(&hi2c1, (uint16_t)MPU9250_I2C_ADDRESS, (uint8_t*)MPU9250_RxBuffer, sizeof(MPU9250_RxBuffer));
    }
    if(I2C_TxStatus.DevAddress == TLV493D_I2C_ADDRESS){
      HAL_I2C_Master_Receive_DMA(&hi2c1, (uint16_t)TLV493D_I2C_ADDRESS, (uint8_t*)TLV493D_RxBuffer[U1], sizeof(TLV493D_RxBuffer[U1]));
    }
    I2C_TxStatus.Busy = 0;
  }
  
  if(I2cHandle->Instance==I2C3){
    HAL_I2C_Master_Receive_DMA(&hi2c3, (uint16_t)TLV493D_I2C_ADDRESS, (uint8_t*)TLV493D_RxBuffer[U2], sizeof(TLV493D_RxBuffer[U2]));
  }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
  if(I2cHandle->Instance==I2C1){
    if(I2C_TxStatus.DevAddress == MPU9250_I2C_ADDRESS){
      for(int i=0; i<MPU9250_RxBuffer_SIZE; i++){
        MPU9250_SensorValue[i] = MPU9250_RxBuffer[i];
      }
    }
    if(I2C_TxStatus.DevAddress == TLV493D_I2C_ADDRESS){
      for(int i=0; i<TLV493D_RxBuffer_SIZE; i++){
        TLV493D_RegisterValue[U1][i] = TLV493D_RxBuffer[U1][i];
      }
      TLV493D_RegisterMask(U1);
    }
    I2C_TxStatus.Busy = 0;
  }
  
  if(I2cHandle->Instance==I2C3){
    for(int i=0; i<TLV493D_RxBuffer_SIZE; i++){
      TLV493D_RegisterValue[U2][i] = TLV493D_RxBuffer[U2][i];
    }
    TLV493D_RegisterMask(U2);
  }
}


// -----------------------------------------------------------------------------------------------------------------------------
// I2C Bus busy function
// -----------------------------------------------------------------------------------------------------------------------------
void I2C_TX(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size){
  
  //while(I2C_TxStatus.Busy == 1) {}
    I2C_TxStatus.Busy = 1;
    I2C_TxStatus.DevAddress = DevAddress;
    
    HAL_I2C_Master_Transmit_DMA(hi2c, DevAddress, pData, Size);
  //}
}



// -----------------------------------------------------------------------------------------------------------------------------
// Timers initialization
// -----------------------------------------------------------------------------------------------------------------------------
void TIMER_Init(void){
  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim7);
}

// -----------------------------------------------------------------------------------------------------------------------------
// Timers Callback
// -----------------------------------------------------------------------------------------------------------------------------
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if(htim->Instance==TIM6){         // timer 
    
  }
  if(htim->Instance==TIM7){         // timer 
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  }
}






























// -----------------------------------------------------------------------------------------------------------------------------
// Update the Touch Map with last values
// -----------------------------------------------------------------------------------------------------------------------------
void UpdateTouchMap(void){
    int i,j;
    for (i=0;i<16;i++)          // patch 16 triangles
        {
            SKIN_OVER_THRESHOLD=0;  // reset del valore precedente
            triangle[0][i]=0;          // reset del valore precedente
            triangle[1][i]=0;          // reset del valore precedente
            FillMessages8bit(0,i);
            skin_value[0][i]=0;
            FillMessages8bit(1,i);
            skin_value[1][i]=0;

            for (j=0;j<12;j++)                      // sarebbero 10 perchè poi ce ne sono due che sono per la temperatura
            {
                if ((j==6) || (j==10))
                {
                    // nothing
                }
                else
                {
                    skin_value[0][i] += taxel[0][i][j];
                    skin_value[1][i] += taxel[1][i][j];
                    if ( (skin_value[0][i] > SKIN_UPPER_THRESHOLD) || (skin_value[1][i] > SKIN_UPPER_THRESHOLD) ) SKIN_OVER_THRESHOLD = 1; // taxel over threshold -> need calibration
                }
            }

            if (SKIN_STATE == i)        SKIN_STATE = -1;        // se lo stato era i, lo resetto
            if (SKIN_STATE_2T == i)     SKIN_STATE_2T = -1;     // se lo stato era i, lo resetto
            if (SKIN_STATE_10T == i)    SKIN_STATE_10T = -1;    // se lo stato era i, lo resetto



            if (mode == MODE_SKIN){             // in SKIN mode there are 2 different thresholds for triangle 5 and triangle 6
                if(i==5){
                    if (skin_value[0][i] < TOUCH_THREASHOLD+10)     // soft pressure
                    {
                        SKIN_STATE_2T=i;
                        triangle[0][i]=1;                           // fill the triangle array
                    }
                }
                if(i==6){
                    if (skin_value[0][i] < TOUCH_THREASHOLD-180)    // higher pressure
                    {
                        SKIN_STATE_2T=i;
                        triangle[0][i]=1;                           // fill the triangle array
                    }
                }
            }
            else{                               // in CMD mode there is only 1 threshold for all triangles
                if (skin_value[0][i] < TOUCH_THREASHOLD)            // event on "2 triangles" occurs
                {
                    SKIN_STATE_2T=i;
                    triangle[0][i]=1;                               // fill the triangle array
                }
            }



            if (skin_value[1][i] < TOUCH_THREASHOLD)                // event on "10 triangles" occurs
            {
                SKIN_STATE_10T=i;
                triangle[1][i]=1;                                   // fill the triangle array
            }
        }
}







    


    
    

// -----------------------------------------------------------------------------------------------------------------------------
// Skin overthreshold recognition and skin raw data sending
// -----------------------------------------------------------------------------------------------------------------------------
void SkinRawData(void){
    //t_skin skin;
    int i;

//    if (NO_TOUCH){
//        GESTURE_STATE=CMD_NO_EVENT;
//        if(SKIN_OVER_THRESHOLD){        // Calibration
//            TrianglesInit(0);
//            TrianglesInit(1);
//        }
//    }
//    else{

        //if( SKIN_VARIATION && ((CONTROL_LEFT && !CONTROL_RIGHT) || (!CONTROL_LEFT && CONTROL_RIGHT)) )
        //{
            for (i=0;i<16;i++)          // patch 16 triangles
            {
                if(triangle[1][i]){     // send only triangle over threshold
                    skin.triangle = i;
                    skin.value[0] = taxel[0][i][0];
                    skin.value[1] = taxel[0][i][1];
                    skin.value[2] = taxel[0][i][2];
                    skin.value[3] = taxel[0][i][3];
                    skin.value[4] = taxel[0][i][4];
                    skin.value[5] = taxel[0][i][5];
                    skin.value[6] = taxel[0][i][7];
                    skin.value[7] = taxel[0][i][8];
                    skin.value[8] = taxel[0][i][9];
                    skin.value[9] = taxel[0][i][11];
                    //rn4020SetSkin(&skin);       // send raw data
                }
            }
        //}
    //}
}













// -----------------------------------------------------------------------------------------------------------------------------
// Fill message 8 bit function
// -----------------------------------------------------------------------------------------------------------------------------
void FillMessages8bit(unsigned char Channel,unsigned char triangleN)
{
    int Tpad;
    int Tpad_base;
    unsigned int i;
    int drift=0;
    int value; //difference of the current measurement and the initial value (_pCapOffset)
    unsigned int txdata[12];
    int unconnected=0;
    unsigned int GAIN[12]={70,96,83,38,38,70, 0,45,77,164,0,77}; //this gains are moltiplied by 128 with respect to matlab
    int UP_LIMIT, BOT_LIMIT;

    UP_LIMIT=((int) (MAXVAL-NOLOAD)<<triangle_cfg_list[triangleN].shift);
    BOT_LIMIT=(int)(NOLOAD)<<triangle_cfg_list[triangleN].shift;

    Tpad_base=CapOffset[Channel][triangleN][(ADCRESULT_S6-ADCRESULT_S0)];
    Tpad=AD7147Registers[Channel][triangleN][(ADCRESULT_S6-ADCRESULT_S0)];

    err[triangleN].error=error_ok;
    err[triangleN].error_outofrange=0;
    unconnected=0;
    for (i=0;i<12;i++)
    {
        if (((CapOffset[Channel][triangleN][i]!=0) && ((AD7147Registers[Channel][triangleN][i]==0)))) //reading error
        {
            err[triangleN].error |=error_noack;

        }
        if (TEMP_COMPENSATION==1)
        {
			if (Tpad>Tpad_base)
			{
			drift=(((Tpad-Tpad_base)>>2)*GAIN[i])>>5;
			}
			else
			{
			drift=-((((Tpad_base-Tpad)>>2)*GAIN[i])>>5);
			}
        }
        else drift=0;

        value=(AD7147Registers[Channel][triangleN][i]-CapOffset[Channel][triangleN][i])-drift;

        if (value<=-UP_LIMIT)
        {
            txdata[i]=MAXVAL; // out of range, pressure too low
        }
        if (value>=BOT_LIMIT)
        {
            txdata[i]=MINVAL; // out of range, pressure too high
        }
        if ((value>-UP_LIMIT) && (value<BOT_LIMIT))
        {
                txdata[i]=NOLOAD-(value>>triangle_cfg_list[triangleN].shift);
        }
//check if the sensor is far from the limits -> taxel could be broken;
        if ((value<=-(UP_LIMIT<<1)) || (value>(BOT_LIMIT<<1)))
        {
            err[triangleN].error_outofrange |= 1<<i;
        }
        if (AD7147Registers[Channel][triangleN][i]==0xffff)
        {
            unconnected +=1; //if all the taxels has a 0xFFFF the triangle is most probably unconnected
        }
    }

    if (unconnected==12) //if all the taxels has a 0xFFFF the triangle is most probably unconnected
    {
        err[triangleN].error=error_notconnected;
        for (i=0;i<12;i++)
           {
           	taxel[Channel][triangleN][i]  = NOLOAD;
           }
    }
    else
    {
    	 for (i=0;i<12;i++)
    	    {
    	    	taxel[Channel][triangleN][i]  = txdata[i];
    	    }
    }


    if ((err[triangleN].error!=error_ok)  && (err[triangleN].error != error_notconnected))//do again the configuration since an error has occured
    {
		err[triangleN].error=error_ok;
		ERROR_COUNTER++;
    }
	if (ERROR_COUNTER==5)  //after x errors I do the CALIB of the board
	{
		triangle_cfg_list[triangleN].isToUpdate=1;
	//	board_MODE=CALIB;
		ERROR_COUNTER=0;
	//	return;
	}
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
