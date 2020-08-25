/****************************************************
*  iCub Facility - 2019                             *
*  Bluetooth Utility                                *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/

#include "stm32l4xx_hal.h"
#include "RN4871.h"
#include "usart.h"

uint8_t RN4871_CMD[] =  "$$$";
uint8_t RN4871_Reset[] = "SF,1,2\r\n";
uint8_t RN4871_Reboot[] = "R,1\r\n";
uint8_t RN4871_Setting_A[] = "SS,C0000000\r\n";		// Support Device Info and Battery as server
//uint8_t RN4871_Setting_A[] = "SS,30000000\r\n";		// Support Heart Rate and Health Thermometer services as server. (Page 93 datasheet)
uint8_t RN4871_Setting_B[] = "SR,00000000\r\n";
//uint8_t RN4871_Setting_B[] = "SR,92000000\r\n";		// Set device as central, support MLDP and enable, UART flow control
//uint8_t RN4871_Setting_B[] = "SR,32000000\r\n";		// Set device as peripheral with automatic advertisement, and support for MLDP and flow control features
uint8_t RN4871_Setting_C[] = "S-,BAT_board\r\n";
uint8_t RN4871_Advertise[] = "A\r\n";
uint8_t aRxBuffer[MESSAGE_SIZE]={0};
char *RN_start = "CMD\r\n";
char *RN_reboot = "Reboot\r\n";
char *RN_ok = "AOK\r\n";

uint8_t RN4871_RX[6] = {0};
uint8_t sendBT = 0;
uint8_t rx = 0;

// -----------------------------------------------------------------------------------------------------------------------------
// Bluetooth configuration
// -----------------------------------------------------------------------------------------------------------------------------
void RN4871_Config(){
  
  BT_CONFIG_ON;
  HAL_Delay(1000);
  
  //HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
	//HAL_GPIO_WritePin(BT_SW_WAKE, GPIO_PIN_SET);
	//WaitUartResponse("CMD");		// CMD

  
  HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4871_CMD, sizeof(RN4871_CMD)-1);
  WaitUartResponse("CMD");		// AOK
  
//	HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 28);
//	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4871_Reset, sizeof(RN4871_Reset)-1);
//	WaitUartResponse("Reboot after Factory Reset");		// AOK
  
//  HAL_Delay(500);
  
//  HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
//	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4871_CMD, sizeof(RN4871_CMD)-1);
//  WaitUartResponse("CMD");		// AOK
  
  //HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 8);
	//WaitUartResponse("%REBOOT%");		// CMD
  
  HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 10);
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4871_Reboot, sizeof(RN4871_Reboot)-1);
	WaitUartResponse("Rebooting");		// REBOOT
  
  HAL_Delay(1000);
  
  HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4871_CMD, sizeof(RN4871_CMD)-1);
  WaitUartResponse("CMD");		// AOK
  
  
//	HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
//	WaitUartResponse("CMD");		// CMD

//	HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
//	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4871_Setting_A, sizeof(RN4871_Setting_A)-1);
//	WaitUartResponse("AOK");		// AOK

//	HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
//	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4871_Setting_B, sizeof(RN4871_Setting_B)-1);
//	WaitUartResponse("AOK");		// AOK

//	HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
//	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4871_Setting_C, sizeof(RN4871_Setting_C)-1);
//	//WaitUartResponse("AOK");		// AOK

//	HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 8);
//	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4871_Reboot, sizeof(RN4871_Reboot)-1);
//	WaitUartResponse("%REBOOT%");		// REBOOT

//	HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
//	WaitUartResponse("CMD");		// CMD

//	HAL_UART_Receive_DMA(&huart2, (uint8_t*)aRxBuffer, 5);
//	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4871_Advertise, sizeof(RN4871_Advertise)-1);
//	WaitUartResponse("AOK");		// AOK
  
  //HAL_GPIO_WritePin(BT_SW_WAKE, GPIO_PIN_RESET);
}

// ----------------------------------------------------------------------------
// Bluetooth commands
// ----------------------------------------------------------------------------
void Bluetooth(void){
    
    if(aRxBuffer[0]==0x00){           // Disable Bluetooth data transmission
        sendBT=0;
    }
    else if(aRxBuffer[0]==0x01){      // Enable Bluetooth data transmission
        sendBT=1;
    }
    else if(aRxBuffer[0]==0x10){      // V12board power on
        if(state_fsm == READY ||
           state_fsm == OFF_BOARD_MOTORS)    {PB1_debounce=1;}
        
    }
    else if(aRxBuffer[0]==0x11){      // V12board power off
        if(state_fsm == ON_BOARD ||
           state_fsm == ON_BOARD_MOTORS)    {PB1_debounce=1;}
        
    }
    else if(aRxBuffer[0]==0x20){      // V12board power on
        if(state_fsm == ON_BOARD)    {PB2_debounce=1;}
    } 
    else if(aRxBuffer[0]==0x21){      // V12board power on
        if(state_fsm == ON_BOARD_MOTORS)    {PB2_debounce=1;}
    }
    
    else if(aRxBuffer[0]==0xFF){
        RN4871_RX[5] = 0xFF;
        RN4871_RX[4] = 0xFF;
        RN4871_RX[3] = 0xFF;
        RN4871_RX[2] = Firmware_vers;      // Firmware version
        RN4871_RX[1] = Revision_vers;      // Revision version
        RN4871_RX[0] = Build_number;       // Build number                    
    }
               
    if(sendBT || rx==0xFF){
//      SendChar2(0x00);
//      SendUart2(RN4871_RX, sizeof(RN4871_RX));
//      SendChar2(DCDC_status);
//      SendChar2(0x0D);        // send \r
//      SendChar2(0x0A);        // send \n
//      //SendChar2(DCDC_ctrl);
      
      HAL_UART_Transmit_DMA(&huart2, (uint8_t*)RN4871_RX, sizeof(RN4871_RX));
      
      
    }
}

// -----------------------------------------------------------------------------------------------------------------------------
// Bluetooth response
// -----------------------------------------------------------------------------------------------------------------------------
void WaitUartResponse(char* response)
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



