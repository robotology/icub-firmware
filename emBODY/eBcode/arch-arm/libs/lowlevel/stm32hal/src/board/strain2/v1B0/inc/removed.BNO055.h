/****************************************************
*  MTB4 board - iCub Facility - 2017                *
*  Si705x configuration                             *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/

#include "stm32l4xx_hal.h"

#ifndef BNO055_H
#define BNO055_H

extern uint8_t BNO055_I2C_ADDRESS;
extern uint8_t BNO055_RxBuffer[8];

void BNO055_init(void);
void BNO055_deinit(void);
void BNO055_Read(uint8_t REGISTER_ADDRESS);
void BNO055_UART_Read(void);

#endif
