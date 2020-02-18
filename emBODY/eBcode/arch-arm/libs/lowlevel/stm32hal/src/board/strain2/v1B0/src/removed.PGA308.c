/****************************************************
*  STRAIN2 board - iCub Facility - 2017             *
*  PGA308 configuration                             *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/

/* 

All communication transactions start with an initialization byte transmitted by the external controller. This byte (55h) senses
the baud rate used for the communication transaction. Baud rates of 4.8k bits/second to 114k bits/second are supported.

The second byte is the command byte. The MSB of the command byte is the R/W bit with a '0' indicating a write command and
a '1' indicating a read command. Additional data transfer occurs after the command/address byte. The number of bytes and
direction of data transfer depend on the command byte.

A timeout mechanism is implemented to allow for resynchronization of the One-Wire interface if the synchronization between the controller
and the PGA308 be lost for any reason. The timeout period is set to approximately 28ms (typical).

*/

#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_gpio.h"
#include "gpio.h"
#include "tim.h"
#include "PGA308.h"

uint8_t PGA308_InitByte = 0x55;
uint8_t PGA308_BufferTx[PGA308_DimBufferTX] = {0};
GPIO_InitTypeDef GPIO_InitStruct;
extern uint8_t PGA308_channel;

PGA308_Register PGA308[STRAIN2_CHANNELS+1];

adc_measure_t adc_measure = {0};  // initialize all adc values to 0
adc_measure_t mean = {0};         // initialize all average values to 0
uint16_t adc_values[6];           // contains all ADC channels conversion

uint16_t adc_sample = 0;
fifo adc_samples = {0};
volatile uint8_t PGA308_StartComm;

// -----------------------------------------------------------------------------------------------------------------------------
// Initialize the PGA308
// -----------------------------------------------------------------------------------------------------------------------------
void PGA308_init(void){
	W_STRAIN1_output;		W_STRAIN1_HIGH;     // The 1W pin must always be high when no communication is in progress.
	W_STRAIN2_output;		W_STRAIN2_HIGH;
	W_STRAIN3_output;		W_STRAIN3_HIGH;
	W_STRAIN4_output;		W_STRAIN4_HIGH;
	W_STRAIN5_output;		W_STRAIN5_HIGH;
	W_STRAIN6_output;		W_STRAIN6_HIGH;
	HAL_GPIO_WritePin(EN_2V8_GPIO_Port, EN_2V8_Pin, GPIO_PIN_SET);
    
  HAL_Delay(25);      // da rimuovere
}

// -----------------------------------------------------------------------------------------------------------------------------
// Write the default configuration in the PGA308
// -----------------------------------------------------------------------------------------------------------------------------
void PGA308_DefaultConfig(void){
  uint8_t channel;
  
  PGA308[0].CFG0_GO  = 0x06;
  PGA308[0].CFG0_MUX = 0x01;
  PGA308[0].CFG0_GI  = 0x04;
  PGA308[0].CFG0_OS  = 0x00;
  
  PGA308[0].SFTC = 0x0050;
  PGA308[0].ZDAC = 0x4000;
  PGA308[0].GDAC = 0x4000;
  PGA308[0].CFG0 = (PGA308[0].CFG0_GO<<13) + (PGA308[0].CFG0_MUX<<12) + (PGA308[0].CFG0_GI<<8) + (PGA308[0].CFG0_OS);
  PGA308[0].CFG1 = 0x0000;
  PGA308[0].CFG2 = 0x0400;
  
  for(channel=1; channel<=6; channel++){
    PGA308[channel].SFTC = PGA308[0].SFTC;
    PGA308[channel].ZDAC = PGA308[0].ZDAC;
    PGA308[channel].GDAC = PGA308[0].GDAC;
    PGA308[channel].CFG0 = PGA308[0].CFG0;
    PGA308[channel].CFG1 = PGA308[0].CFG1;
    PGA308[channel].CFG2 = PGA308[0].CFG2;
    
    PGA308_WriteRegister(channel, RAM_SFTC, PGA308[0].SFTC);      // SFTC Register - Software Lock Mode  (datasheet page 72)
    while(PGA308_StartComm);
    PGA308_WriteRegister(channel, RAM_ZDAC, PGA308[0].ZDAC);      //
    while(PGA308_StartComm);
    PGA308_WriteRegister(channel, RAM_GDAC, PGA308[0].GDAC);      //
    while(PGA308_StartComm);
    PGA308_WriteRegister(channel, RAM_CFG0, PGA308[0].CFG0);      // 
    while(PGA308_StartComm);
    PGA308_WriteRegister(channel, RAM_CFG1, PGA308[0].CFG1);      //
    while(PGA308_StartComm);
    PGA308_WriteRegister(channel, RAM_CFG2, PGA308[0].CFG2);      //
    while(PGA308_StartComm);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
// Configure the GPIO for One-Wire communication
// -----------------------------------------------------------------------------------------------------------------------------
void PGA308_OneWire(uint16_t GPIO, uint8_t IO){				// da passare W_STRAINx_Pin
	
//	if(IO == 1){
//		/*Configure GPIO pin : PtPin */
//		GPIO_InitStruct.Pin = PROVA_GPIO_INPUT_Pin;
//		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//		GPIO_InitStruct.Pull = GPIO_NOPULL;
//		HAL_GPIO_Init(PROVA_GPIO_INPUT_GPIO_Port, &GPIO_InitStruct);
//	}
//	else if(IO == 0){
//		/*Configure GPIO pins : PAPin PAPin */
//		GPIO_InitStruct.Pin = PROVA_GPIO_OUT_Pin;
//		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//		GPIO_InitStruct.Pull = GPIO_NOPULL;
//		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//		
//		HAL_GPIO_WritePin(GPIOA, PROVA_GPIO_OUT_Pin|EN_OSC_Pin, GPIO_PIN_RESET);
//	}
	
	HAL_TIM_Base_Start_IT(&htim6);    // 100us          Baud rates of 4.8k to 114k bits/second are supported.
	
}

// -----------------------------------------------------------------------------------------------------------------------------
// Configure the GPIO for One-Wire communication
// -----------------------------------------------------------------------------------------------------------------------------
void PGA308_WriteRegister(uint8_t CHANNEL, uint8_t REGISTER, uint16_t VALUE){
  PGA308_StartComm=1;
  PGA308_channel=CHANNEL;
	PGA308_BufferTx[0]=PGA308_InitByte;
	PGA308_BufferTx[1]=REGISTER & 0x0F;       // RAM Write command byte
	PGA308_BufferTx[2]=VALUE & 0xFF;          // Register data 8 LSBs
	PGA308_BufferTx[3]=(VALUE >> 8) & 0xFF;   // Register data 8 MSBs
	
	HAL_TIM_Base_Start_IT(&htim6);    // 100us	
}

void PGA308_ReadRegister(uint8_t CHANNEL, uint8_t REGISTER){
  PGA308_StartComm=1;
  PGA308_channel=CHANNEL;
  PGA308_BufferTx[0]=PGA308_InitByte;
	PGA308_BufferTx[1]=(REGISTER & 0x0F) | 0x80;       // RAM Read command byte
  PGA308_BufferTx[2]=0x00;
  PGA308_BufferTx[3]=0x00;
	
	HAL_TIM_Base_Start_IT(&htim7);    // 100us	
}

void PGA308_BitValue(uint8_t channel, uint8_t value){
  if(value==0){
    if      (channel==1){W_STRAIN1_LOW;}
    else if (channel==2){W_STRAIN2_LOW;}
    else if (channel==3){W_STRAIN3_LOW;}
    else if (channel==4){W_STRAIN4_LOW;}
    else if (channel==5){W_STRAIN5_LOW;}
    else if (channel==6){W_STRAIN6_LOW;}
  }
  else{
    if      (channel==1){W_STRAIN1_HIGH;}
    else if (channel==2){W_STRAIN2_HIGH;}
    else if (channel==3){W_STRAIN3_HIGH;}
    else if (channel==4){W_STRAIN4_HIGH;}
    else if (channel==5){W_STRAIN5_HIGH;}
    else if (channel==6){W_STRAIN6_HIGH;}
  }
}




