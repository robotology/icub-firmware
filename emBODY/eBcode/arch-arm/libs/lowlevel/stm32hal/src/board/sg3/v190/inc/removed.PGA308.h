/****************************************************
*  3SG board - iCub Facility - 2019                 *
*  PGA308 configuration                             *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/

// IIT-EXT
#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
#include "stm32l4xx_hal.h"
#endif


#ifndef PGA308_H
#define PGA308_H

#define LOW   0
#define HIGH  1

#define RAM_ZDAC  0x00
#define RAM_GDAC  0x01
#define RAM_CFG0  0x02
#define RAM_CFG1  0x03
#define RAM_CFG2  0x04
#define RAM_CHKS  0x05
#define RAM_SFTC  0x07

#define ONEW_WR			0x00
#define ONEW_RD			0x01
#define	ONEW_ACK		0
#define	ONEW_NACK		1

#define	W_STRAIN1_HIGH    HAL_GPIO_WritePin(W_STRAIN1_GPIO_Port, W_STRAIN1_Pin, GPIO_PIN_SET);
#define	W_STRAIN2_HIGH    HAL_GPIO_WritePin(W_STRAIN2_GPIO_Port, W_STRAIN2_Pin, GPIO_PIN_SET);
#define	W_STRAIN3_HIGH    HAL_GPIO_WritePin(W_STRAIN3_GPIO_Port, W_STRAIN3_Pin, GPIO_PIN_SET);

#define	W_STRAIN1_LOW     HAL_GPIO_WritePin(W_STRAIN1_GPIO_Port, W_STRAIN1_Pin, GPIO_PIN_RESET);
#define	W_STRAIN2_LOW     HAL_GPIO_WritePin(W_STRAIN2_GPIO_Port, W_STRAIN2_Pin, GPIO_PIN_RESET);
#define	W_STRAIN3_LOW     HAL_GPIO_WritePin(W_STRAIN3_GPIO_Port, W_STRAIN3_Pin, GPIO_PIN_RESET);

#define W_STRAIN1_input		LL_GPIO_SetPinMode(W_STRAIN1_GPIO_Port, W_STRAIN1_Pin, LL_GPIO_MODE_INPUT);			LL_GPIO_SetPinOutputType(W_STRAIN1_GPIO_Port, W_STRAIN1_Pin, LL_GPIO_PULL_NO);
#define W_STRAIN2_input		LL_GPIO_SetPinMode(W_STRAIN2_GPIO_Port, W_STRAIN2_Pin, LL_GPIO_MODE_INPUT);			LL_GPIO_SetPinOutputType(W_STRAIN2_GPIO_Port, W_STRAIN2_Pin, LL_GPIO_PULL_NO);
#define W_STRAIN3_input		LL_GPIO_SetPinMode(W_STRAIN3_GPIO_Port, W_STRAIN3_Pin, LL_GPIO_MODE_INPUT);			LL_GPIO_SetPinOutputType(W_STRAIN3_GPIO_Port, W_STRAIN3_Pin, LL_GPIO_PULL_NO);

#define W_STRAIN1_output	LL_GPIO_SetPinMode(W_STRAIN1_GPIO_Port, W_STRAIN1_Pin, LL_GPIO_MODE_OUTPUT);		LL_GPIO_SetPinOutputType(W_STRAIN1_GPIO_Port, W_STRAIN1_Pin, LL_GPIO_OUTPUT_OPENDRAIN);
#define W_STRAIN2_output	LL_GPIO_SetPinMode(W_STRAIN2_GPIO_Port, W_STRAIN2_Pin, LL_GPIO_MODE_OUTPUT);		LL_GPIO_SetPinOutputType(W_STRAIN2_GPIO_Port, W_STRAIN2_Pin, LL_GPIO_OUTPUT_OPENDRAIN);
#define W_STRAIN3_output	LL_GPIO_SetPinMode(W_STRAIN3_GPIO_Port, W_STRAIN3_Pin, LL_GPIO_MODE_OUTPUT);		LL_GPIO_SetPinOutputType(W_STRAIN3_GPIO_Port, W_STRAIN3_Pin, LL_GPIO_OUTPUT_OPENDRAIN);

#define STRAIN_CHANNELS 3
#define PGA308_DimBufferTX 4

void PGA308_init(void);
void PGA308_DefaultConfig(void);
void PGA308_WriteRegister(uint8_t CHANNEL, uint8_t REGISTER, uint16_t VALUE);
void PGA308_ReadRegister(uint8_t CHANNEL, uint8_t REGISTER);
void PGA308_OneWire(uint16_t GPIO, uint8_t IO);
void PGA308_BitValue(uint8_t channel, uint8_t value);

#define nr_adc_sample (uint16_t) 400      // number of samples for the average measure
#define nr_PGA308_register (uint8_t) 6    // number of PGA308 registers
extern uint16_t adc_values[STRAIN_CHANNELS];            // contains all ADC channels conversion
extern uint16_t adc_sample;
extern uint8_t PGA308_channel;

extern uint8_t PGA308_BufferTx[PGA308_DimBufferTX];
extern volatile uint8_t PGA308_StartComm;

extern uint8_t REG_count;
extern uint8_t REG_GO;
extern uint8_t REG_tran;
extern uint8_t REG_OK;
extern uint8_t REG_Value;
#define SAMPLES 10
extern uint8_t REG_max_count;     
extern uint16_t REG_value_tran[SAMPLES];
//extern uint8_t CHANNEL;
extern uint8_t REGISTER;
extern uint8_t PGA308_StartCalib;

typedef struct 
{
  uint32_t STRAIN1;
  uint32_t STRAIN2;
  uint32_t STRAIN3;
}adc_measure_t;
extern adc_measure_t adc_measure;
extern adc_measure_t mean;

typedef struct 
{
  uint32_t STRAIN1[nr_adc_sample];
  uint32_t STRAIN2[nr_adc_sample];
  uint32_t STRAIN3[nr_adc_sample];
}fifo;
extern fifo adc_samples;

typedef struct 
{
  uint16_t SFTC;
  uint16_t ZDAC;
  uint16_t GDAC;
  uint16_t CFG0;
  uint16_t CFG1;
  uint16_t CFG2;
  uint16_t CFG0_GO;
  uint16_t CFG0_MUX;
  uint16_t CFG0_GI;
  uint16_t CFG0_OS;
}PGA308_Register;
extern PGA308_Register PGA308[STRAIN_CHANNELS+1];    // PGA308[0] is used for default configuration

#endif
