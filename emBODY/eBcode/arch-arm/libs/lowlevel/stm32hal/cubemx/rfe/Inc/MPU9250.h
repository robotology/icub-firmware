/****************************************************
*  RFE_MASTER - iCub Facility - 2018                *
*  MPU9250 configuration                            *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/

#include "stm32l4xx_hal.h"

#ifndef MPU9250_H
#define MPU9250_H

// Define MPU9250 Registers -------------------------
#define MPU9250_CONFIG          0x1C
#define MPU9250_GYRO_CONFIG     0x1B
#define MPU9250_ACCEL_CONFIG    0x1C
#define MPU9250_ACCEL_CONFIG_2  0x1D
#define MPU9250_LP_ACCEL_ODR    0x1E
#define MPU9250_WOM_THR         0x1F
#define MPU9250_ACCEL_XOUT      0x3B
#define MPU9250_ACCEL_YOUT      0x3D
#define MPU9250_ACCEL_ZOUT      0x3F
#define MPU9250_TEMP_OUT        0x41
#define MPU9250_GYRO_XOUT       0x43
#define MPU9250_GYRO_YOUT       0x45
#define MPU9250_GYRO_ZOUT       0x47

#define MPU9250_PWR_MGMT_1      0x6B
#define MPU9250_PWR_MGMT_2      0x6C
#define MPU9250_WHO_AM_I        0x75
// --------------------------------------------------

#define MPU9250_RxBuffer_SIZE   6

extern uint8_t MPU9250_I2C_ADDRESS;
extern uint8_t MPU9250_RxBuffer[MPU9250_RxBuffer_SIZE];

void MPU9250_init(void);
void MPU9250_deinit(void);
void MPU9250_Read(uint8_t REGISTER_ADDRESS);
void MPU9250_Registers(uint8_t REGISTER_ADDRESS, uint8_t VALUE, uint8_t DIM);

#endif
