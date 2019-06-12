/****************************************************
*  MTB4 board - iCub Facility - 2017                *
*  CAN Utility                                      *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/

#include "stm32l4xx_hal.h"

#ifndef CAN_UTILITY_H
#define CAN_UTILITY_H

void CAN_Config(void);
void CANBUS(void);
extern uint16_t adc_values[6];           // contains all ADC channels conversion

#endif
