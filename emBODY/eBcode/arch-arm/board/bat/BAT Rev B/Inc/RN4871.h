/****************************************************
*  iCub Facility - 2019                             *
*  uOLED Display Utility                            *
*                                                   *
*  Written by Andrea Mura                           *
*  <andrea.mura@iit.it>                             *
****************************************************/

#include "stm32l4xx_hal.h"
#include "define.h"
#include "global_var.h"

#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#define MESSAGE_SIZE 50

void RN4871_Config(void);
void Bluetooth(void);
void WaitUartResponse(char* response);
static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

extern uint8_t RN4871_CMD[];
extern uint8_t RN4871_Reset[];
extern uint8_t RN4871_Reboot[];
extern uint8_t RN4871_Setting_A[];
//extern uint8_t RN4871_Setting_A[];
extern uint8_t RN4871_Setting_B[];
//extern uint8_t RN4871_Setting_B[];
//extern uint8_t RN4871_Setting_B[];
extern uint8_t RN4871_Setting_C[];
extern uint8_t RN4871_Advertise[];
extern uint8_t aRxBuffer[MESSAGE_SIZE];
extern char *RN_start;
extern char *RN_reboot;
extern char *RN_ok;

#endif
