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
#include "BAT_B.h"

#ifndef Display_uOLED_H
#define Display_uOLED_H

void Display_uOLED_128_G2(void);
void Display_uOLED_init(void);

extern uint16_t uOLED_Parameters[14];
extern uint16_t Display_qualifier;


#endif
