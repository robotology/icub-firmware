/****************************************************
*  iCub Facility - 2018                             *
*  TLV493D configuration                            *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/

#include "stm32l4xx_hal.h"
#include "i2c.h"
#include "TLV493D.h"

extern void I2C_TX(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);

uint8_t TLV493D_TxBuffer[TLV493D_TxBuffer_SIZE] = {0};
uint8_t TLV493D_RxBuffer[2][TLV493D_RxBuffer_SIZE] = {0};
uint8_t TLV493D_RegisterValue[2][TLV493D_RxBuffer_SIZE] = {0};

TLV493D_REGISTER TLV493D_REG[2] = {0};

typedef struct 
{
  uint16_t mXdata;
  uint16_t mYdata;
  uint16_t mZdata;
  uint16_t mTempdata;
  uint8_t mExpectedFrameCount;
}tlv493d_t;

tlv493d_t tlv493d = {0};  // initialize all values to 0


// -----------------------------------------------------------------------------------------------------------------------------
// TLV493D Store the registers
// -----------------------------------------------------------------------------------------------------------------------------
void TLV493D_RegisterMask(uint8_t U){
  TLV493D_REG[U].R_Bx             = TLV493D_RegisterValue[U][0];
  TLV493D_REG[U].R_By             = TLV493D_RegisterValue[U][1];
  TLV493D_REG[U].R_Bz             = TLV493D_RegisterValue[U][2];
  TLV493D_REG[U].R_Temp           = TLV493D_RegisterValue[U][3];
  TLV493D_REG[U].R_Bx2            = TLV493D_RegisterValue[U][4];
  TLV493D_REG[U].R_Bz2            = TLV493D_RegisterValue[U][5];
  TLV493D_REG[U].R_Temp2          = TLV493D_RegisterValue[U][6];
  TLV493D_REG[U].R_FactSet1       = TLV493D_RegisterValue[U][7];
  TLV493D_REG[U].R_FactSet2       = TLV493D_RegisterValue[U][8];
  TLV493D_REG[U].R_FactSet3       = TLV493D_RegisterValue[U][9];
  
  TLV493D_REG[U].FRAMECOUNTER     = TLV493D_REG[U].R_Temp & 0x0c; 
  TLV493D_REG[U].CHANNEL          = TLV493D_REG[U].R_Temp & 0x03;
  TLV493D_REG[U].TESTMODEFLAG     = TLV493D_REG[U].R_Bz2 & 0x40;
  TLV493D_REG[U].PARITYFUSEFLAG   = TLV493D_REG[U].R_Bz2 & 0x20;
  TLV493D_REG[U].POWERDOWNFLAG    = TLV493D_REG[U].R_Bz2 & 0x10;

  TLV493D_REG[U].Bx               = (TLV493D_REG[U].R_Bx << 4) + (TLV493D_REG[U].R_Bx2 >>4);
  TLV493D_REG[U].By               = (TLV493D_REG[U].R_By << 4) + (TLV493D_REG[U].R_Bx2 & 0x0f);
  TLV493D_REG[U].Bz               = (TLV493D_REG[U].R_Bz << 4) + (TLV493D_REG[U].R_Bz2 & 0x0f);
  TLV493D_REG[U].Temp             = ((TLV493D_REG[U].R_Temp & 0xf0) << 8) + (TLV493D_REG[U].R_Temp2);  
}

// -----------------------------------------------------------------------------------------------------------------------------
// TLV493D Access mode
// -----------------------------------------------------------------------------------------------------------------------------
void TLV493D_AccessMode(uint8_t U, uint8_t PARITYBIT, uint8_t IICADDR, uint8_t INT, uint8_t FAST, uint8_t LOWPOWER, uint8_t TEMPERATURE, uint8_t LP_PERIOD, uint8_t PARITYTEST_EN){
  TLV493D_REG[U].W_Res0           = TLV493D_W_RES0;
  TLV493D_REG[U].W_MOD1           = PARITYBIT<<7 | IICADDR<<5 | (TLV493D_REG[U].R_FactSet1 & 0x18) | INT<<2 | FAST<<1 | LOWPOWER;
  TLV493D_REG[U].W_Res2           = TLV493D_REG[U].R_FactSet2;;
  TLV493D_REG[U].W_MOD2           = TEMPERATURE<<7 | LP_PERIOD<<6 | PARITYTEST_EN<<5 | (TLV493D_REG[U2].R_FactSet3 & 0x1F);  
}


// -----------------------------------------------------------------------------------------------------------------------------
// TLV493D Initialization
// -----------------------------------------------------------------------------------------------------------------------------
void TLV493D_Init(uint8_t U){
  
  HAL_Delay(10);       // Wait Vdd stable
  
  TLV493D_SendRecovery(U);
  HAL_Delay(10);
  
  TLV493D_GeneralReset(U);
  HAL_Delay(10);
  
  TLV493D_ReadRegisters(U);
  TLV493D_RegisterMask(U);
  TLV493D_AccessMode(U, 1 , 0, 0, 1, 0, 0, 0, 0);
  HAL_Delay(10);
  
  TLV493D_WriteRegisters(U);
  HAL_Delay(10);
}

// -----------------------------------------------------------------------------------------------------------------------------
// TLV493D Read the registers
// -----------------------------------------------------------------------------------------------------------------------------
void TLV493D(void){
  TLV493D_ReadRegisters(U1);
  TLV493D_ReadRegisters(U2);
}

// -----------------------------------------------------------------------------------------------------------------------------
// TLV493D Write the registers
// -----------------------------------------------------------------------------------------------------------------------------
void TLV493D_WriteRegisters(uint8_t U){
  TLV493D_TxBuffer[0] = TLV493D_REG[U].W_Res0;
  TLV493D_TxBuffer[1] = TLV493D_REG[U].W_MOD1;
  TLV493D_TxBuffer[2] = TLV493D_REG[U].W_Res2;
  TLV493D_TxBuffer[3] = TLV493D_REG[U].W_MOD2;
  if(U == U1)       {I2C_TX(&hi2c1, (uint16_t)TLV493D_I2C_ADDRESS, (uint8_t*)TLV493D_TxBuffer, sizeof(TLV493D_TxBuffer));}
  else if(U == U2)  {I2C_TX(&hi2c3, (uint16_t)TLV493D_I2C_ADDRESS, (uint8_t*)TLV493D_TxBuffer, sizeof(TLV493D_TxBuffer));}
}

// -----------------------------------------------------------------------------------------------------------------------------
// TLV493D Read the registers
// -----------------------------------------------------------------------------------------------------------------------------
void TLV493D_ReadRegisters(uint8_t U){
  TLV493D_TxBuffer[0] = 0x00;
  if(U == U1)       {I2C_TX(&hi2c1, (uint16_t)TLV493D_I2C_ADDRESS, (uint8_t*)TLV493D_TxBuffer, 1);}
  else if(U == U2)  {I2C_TX(&hi2c3, (uint16_t)TLV493D_I2C_ADDRESS, (uint8_t*)TLV493D_TxBuffer, 1);}
}

// -----------------------------------------------------------------------------------------------------------------------------
// TLV493D Send Recovery
// Optional. The recovery frame could free the sensor in case of an aborded communication
// -----------------------------------------------------------------------------------------------------------------------------
void TLV493D_SendRecovery(uint8_t U){
  TLV493D_TxBuffer[0] = 0x00;
  if(U == U1)       {I2C_TX(&hi2c1, (uint16_t)TLV493D_SEND_RECOVERY, (uint8_t*)TLV493D_TxBuffer, 0);}
  else if(U == U2)  {I2C_TX(&hi2c3, (uint16_t)TLV493D_SEND_RECOVERY, (uint8_t*)TLV493D_TxBuffer, 0);}
}

// -----------------------------------------------------------------------------------------------------------------------------
// TLV493D General reset
// This generates an internal reset, refreshes the fuse register settings and re-reads the SDA/ADR line to initialize the I2C bus address
// -----------------------------------------------------------------------------------------------------------------------------
void TLV493D_GeneralReset(uint8_t U){
  TLV493D_TxBuffer[0] = 0x00;
  if(U == U1)       {I2C_TX(&hi2c1, (uint16_t)TLV493D_GENERAL_RESET, (uint8_t*)TLV493D_TxBuffer, 1);}
  else if(U == U2)  {I2C_TX(&hi2c3, (uint16_t)TLV493D_GENERAL_RESET, (uint8_t*)TLV493D_TxBuffer, 1);}
}

