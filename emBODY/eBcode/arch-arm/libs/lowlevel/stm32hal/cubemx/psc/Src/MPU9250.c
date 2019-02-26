/****************************************************
*  PSC board - iCub Facility - 2019                 *
*  MPU9250 configuration                            *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/


#include "stm32l4xx_hal.h"
#include "i2c.h"
#include "MPU9250.h"

extern void I2C_TX(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);

uint8_t MPU9250_SensorValue[MPU9250_RxBuffer_SIZE] = {0};
uint8_t MPU9250_RegisterValue;
uint8_t MPU9250_I2C_ADDRESS=0x68<<1;    // The slave address of the MPU-9250 is b110100X which is 7 bits long. The LSB bit of the 7 bit address is determined by the logic level on pin AD0.
uint8_t MPU9250_TxBuffer[2] = {0};
uint8_t MPU9250_RxBuffer[MPU9250_RxBuffer_SIZE] = {0};

// -----------------------------------------------------------------------------------------------------------------------------
// Initialize the MPU9250
// -----------------------------------------------------------------------------------------------------------------------------
void MPU9250_Init(void){
  MPU9250_Registers(MPU9250_WHO_AM_I, 0, 1);   // TEST
  HAL_Delay(10);
  MPU9250_Registers(MPU9250_GYRO_CONFIG, 0x10, 1);   // 1000dps
  HAL_Delay(10);
  MPU9250_Registers(MPU9250_ACCEL_CONFIG, 0x08, 1);   // 4g
  HAL_Delay(10);
  MPU9250_Registers(MPU9250_ACCEL_CONFIG_2, 0x06, 1);   // Set accelerometers low pass filter at 5Hz
  HAL_Delay(10);
  MPU9250_Registers(MPU9250_CONFIG, 0x06, 1);   // Set gyroscope low pass filter at 5Hz
  HAL_Delay(10);
  
}

// -----------------------------------------------------------------------------------------------------------------------------
// De-initialize the MPU9250
// -----------------------------------------------------------------------------------------------------------------------------
void MPU9250_deinit(void){
  
}

// -----------------------------------------------------------------------------------------------------------------------------
// I2C Read the BNO055 registers
// -----------------------------------------------------------------------------------------------------------------------------
void MPU9250_Read(uint8_t REGISTER_ADDRESS){
  MPU9250_TxBuffer[0] = REGISTER_ADDRESS;
  I2C_TX(&hi2c1, (uint16_t)MPU9250_I2C_ADDRESS, (uint8_t*)MPU9250_TxBuffer, sizeof(MPU9250_TxBuffer));
}

// -----------------------------------------------------------------------------------------------------------------------------
// I2C Read or Write the MPU9250 registers
// -----------------------------------------------------------------------------------------------------------------------------
void MPU9250_Registers(uint8_t REGISTER_ADDRESS, uint8_t VALUE, uint8_t DIM){
  MPU9250_TxBuffer[0] = REGISTER_ADDRESS;
  MPU9250_TxBuffer[1] = VALUE;
  I2C_TX(&hi2c1, (uint16_t)MPU9250_I2C_ADDRESS, (uint8_t*)MPU9250_TxBuffer, DIM);
}

// -----------------------------------------------------------------------------------------------------------------------------
// Read the IMU MPU9250 I2C
// -----------------------------------------------------------------------------------------------------------------------------
void MPU9250(void){
  MPU9250_Registers(MPU9250_ACCEL_XOUT, 0, 1);
  //MPU9250_Registers(MPU9250_GYRO_XOUT, 0, 1);
  //MPU9250_Registers(MPU9250_TEMP_OUT, 0, 2);
}


//// Set accelerometers low pass filter at 5Hz
//I2CwriteByte(MPU9250_ADDRESS,29,0x06);
//// Set gyroscope low pass filter at 5Hz
//I2CwriteByte(MPU9250_ADDRESS,26,0x06);
// 
// 
//// Configure gyroscope range
//I2CwriteByte(MPU9250_ADDRESS,27,GYRO_FULL_SCALE_1000_DPS);
//// Configure accelerometers range
//I2CwriteByte(MPU9250_ADDRESS,28,ACC_FULL_SCALE_4_G);
//// Set by pass mode for the magnetometers
//I2CwriteByte(MPU9250_ADDRESS,0x37,0x02);
// 
//// Request continuous magnetometer measurements in 16 bits
//I2CwriteByte(MAG_ADDRESS,0x0A,0x16);
