/****************************************************
*  RFE_MASTER board - iCub Facility - 2018          *
*  BNO055 configuration                             *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/

#include "stm32l4xx_hal.h"

#ifndef BNO055_H
#define BNO055_H

//extern uint8_t BNO055_I2C_ADDRESS;
//extern uint8_t BNO055_RxBuffer[8];

//#define BNO055_RxBuffer_SIZE 47
#define BNO055_RxBuffer_SIZE 6

#define BNO055_FirstValue     0x08

#define PWR_MODE              0x3E
#define PWR_MODE_Normal       0x00
#define PWR_MODE_Low          0x01
#define PWR_MODE_Suspend      0x02

#define ACC_Config            0x08

#define OPR_MODE              0x3D
#define OPR_MODE_CONFIGMODE   0x00
#define OPR_MODE_ACCONLY      0x01
#define OPR_MODE_MAGONLY      0x02
#define OPR_MODE_GYROONLY     0x03
#define OPR_MODE_ACCMAG       0x04
#define OPR_MODE_ACCGYRO      0x05
#define OPR_MODE_MAGGYRO      0x06
#define OPR_MODE_AMG          0x07
#define OPR_MODE_IMU          0x08
#define OPR_MODE_COMPASS      0x09
#define OPR_MODE_M4G          0x0A
#define OPR_MODE_NDOF_FMC_OFF 0x0B
#define OPR_MODE_NDOF         0x0C

extern uint8_t BNO055_I2C_ADDRESS;
extern uint8_t BNO055_RxBuffer[BNO055_RxBuffer_SIZE];

void BNO055_init(void);
void BNO055_deinit(void);
void BNO055_Read(uint8_t REGISTER_ADDRESS);
void BNO055_Write(uint8_t REGISTER_ADDRESS, uint8_t VALUE);
void BNO055_UART_Read(void);

#endif
