/****************************************************
*  RFE_MASTER board - iCub Facility - 2018          *
*  BNO055 configuration                             *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/


#include "stm32l4xx_hal.h"
#include "i2c.h"
#include "BNO055.h"

uint8_t BNO055_I2C_ADDRESS=0x28<<1;    // left shifted device address 0x28
uint8_t BNO055_TxBuffer[2] = {0};
//uint8_t BNO055_RxBuffer[8] = {0};
uint8_t BNO055_RxBuffer[BNO055_RxBuffer_SIZE] = {0};
extern uint8_t BNO055_RegisterValue;

// -----------------------------------------------------------------------------------------------------------------------------
// Initialize the BNO055
// -----------------------------------------------------------------------------------------------------------------------------
void BNO055_init(void){
  HAL_GPIO_WritePin(BNO055_BOOT_GPIO_Port, BNO055_BOOT_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(BNO055_RESET_GPIO_Port, BNO055_RESET_Pin, GPIO_PIN_SET);
  
  HAL_Delay(800);       // POR time >650ms
  
  BNO055_Write(PWR_MODE, PWR_MODE_Normal);   // BNO055 initialization Power Mode
  HAL_Delay(100);
  BNO055_Write(OPR_MODE, OPR_MODE_ACCONLY);   // BNO055 initialization Operation Mode
  HAL_Delay(100);
  BNO055_Write(ACC_Config, 0x01);   // 4g
  HAL_Delay(100);
}

// -----------------------------------------------------------------------------------------------------------------------------
// De-initialize the BNO055
// -----------------------------------------------------------------------------------------------------------------------------
void BNO055_deinit(void){
  
}

// -----------------------------------------------------------------------------------------------------------------------------
// I2C Read the BNO055 registers
// -----------------------------------------------------------------------------------------------------------------------------
void BNO055_Read(uint8_t REGISTER_ADDRESS){
  BNO055_TxBuffer[0] = REGISTER_ADDRESS;
  HAL_I2C_Master_Transmit_DMA(&hi2c1, (uint16_t)BNO055_I2C_ADDRESS, (uint8_t*)BNO055_TxBuffer, 1);
  //HAL_I2C_Master_Transmit_IT(&hi2c1, (uint16_t)BNO055_I2C_ADDRESS, (uint8_t*)BNO055_TxBuffer, sizeof(BNO055_TxBuffer));
}

// -----------------------------------------------------------------------------------------------------------------------------
// I2C Write the BNO055 registers
// -----------------------------------------------------------------------------------------------------------------------------
void BNO055_Write(uint8_t REGISTER_ADDRESS, uint8_t VALUE){
  BNO055_TxBuffer[0] = REGISTER_ADDRESS;
  BNO055_TxBuffer[1] = VALUE;
  HAL_I2C_Master_Transmit_DMA(&hi2c1, (uint16_t)BNO055_I2C_ADDRESS, (uint8_t*)BNO055_TxBuffer, sizeof(BNO055_TxBuffer));
}
