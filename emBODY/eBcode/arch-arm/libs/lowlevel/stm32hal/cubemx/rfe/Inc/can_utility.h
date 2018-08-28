/****************************************************
*  RFE_MASTER - iCub Facility - 2018                *
*  CAN Utility                                      *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/

#include "stm32l4xx_hal.h"

#ifndef CAN_UTILITY_H
#define CAN_UTILITY_H

void CAN_Config(void);

extern uint8_t ubKeyNumber; // = 0x0;

extern CAN_TxHeaderTypeDef   TxHeader;
extern CAN_RxHeaderTypeDef   RxHeader;
extern uint8_t               TxData[8];
extern uint8_t               RxData[8];
extern uint32_t              TxMailbox;

#endif
