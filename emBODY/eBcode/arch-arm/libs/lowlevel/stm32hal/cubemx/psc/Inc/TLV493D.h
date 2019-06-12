/****************************************************
*  iCub Facility - 2018                             *
*  TLV493D configuration                            *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/

#include "stm32l4xx_hal.h"

#ifndef TLV493D_H
#define TLV493D_H

#define TLV493D_U1              &hi2c1
#define TLV493D_U2              &hi2c3

#define U1                      0
#define U2                      1

#define TLV493D_I2C_ADDRESS     0x3E
#define TLV493D_RES1_ADDRESS    0x00
#define TLV493D_MOD1_ADDRESS    0x01
#define TLV493D_RES2_ADDRESS    0x02
#define TLV493D_MOD2_ADDRESS    0x03

#define TLV493D_SEND_RECOVERY   0xFF
#define TLV493D_GENERAL_RESET   0x00

#define TLV493D_W_RES0          0x00
#define TLV493D_W_P             0
#define TLV493D_W_IICADDR       0
#define TLV493D_W_INT           0
#define TLV493D_W_FAST          1
#define TLV493D_W_LOW           0
#define TLV493D_W_T             0
#define TLV493D_W_LP            0
#define TLV493D_W_TP            0

#define TLV493D_RxBuffer_SIZE   10
#define TLV493D_TxBuffer_SIZE   4
#define TLV493D_FirstValue      0x00

#define TLV493D_DEFAULTMODE			    POWERDOWNMODE
#define TLV493D_STARTUPDELAY		    40
#define TLV493D_RESETDELAY			    60
#define TLV493D_NUM_OF_REGMASKS		  25
#define TLV493D_NUM_OF_ACCMODES     5
#define TLV493D_MEASUREMENT_READOUT 7
#define TLV493D_FAST_READOUT		    3
#define TLV493D_B_MULT 				      0.098
#define TLV493D_TEMP_MULT 		    	1.1
#define TLV493D_TEMP_OFFSET 		    315

extern uint8_t TLV493D_TxBuffer[TLV493D_TxBuffer_SIZE];
extern uint8_t TLV493D_RxBuffer[2][TLV493D_RxBuffer_SIZE];
extern uint8_t TLV493D_RegisterValue[2][TLV493D_RxBuffer_SIZE];

typedef struct
{
  uint8_t R_Bx;
  uint8_t R_By;
  uint8_t R_Bz;
  uint8_t R_Temp;
  uint8_t R_Bx2;
  uint8_t R_Bz2;
  uint8_t R_Temp2;
  uint8_t R_FactSet1;
  uint8_t R_FactSet2;
  uint8_t R_FactSet3;
  
  uint8_t W_Res0;
  uint8_t W_MOD1;
  uint8_t W_Res2;
  uint8_t W_MOD2;
  
  uint16_t Bx;
  uint16_t By;
  uint16_t Bz;
  uint16_t Temp;
  
  uint8_t FRAMECOUNTER;
  uint8_t CHANNEL;
  uint8_t TESTMODEFLAG;
  uint8_t PARITYFUSEFLAG;
  uint8_t POWERDOWNFLAG;
  
  uint8_t PARITYBIT;
  uint8_t IICADDR;
  uint8_t INT;
  uint8_t FAST;
  uint8_t LOWPOWER;
  uint8_t TEMPERATURE;
  uint8_t LP_PERIOD;
  uint8_t PARITYTEST_EN;
}TLV493D_REGISTER;

extern TLV493D_REGISTER TLV493D_REG[2];

void TLV493D_RegisterMask(uint8_t U);
void TLV493D(void);
void TLV493D_WriteRegisters(uint8_t U);
void TLV493D_ReadRegisters(uint8_t U);
void TLV493D_Init(uint8_t U);
void TLV493D_SendRecovery(uint8_t U);
void TLV493D_GeneralReset(uint8_t U);


#endif
