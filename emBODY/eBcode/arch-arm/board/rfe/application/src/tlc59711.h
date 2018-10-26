 /****************************************************
 *  BFE board - iCub Facility - 2016                 *
 *  TLC59711 configuration                           *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/
 
#ifndef TLC59711_H
#define TLC59711_H

#include "stm32l4xx_hal.h"
#include "colors.h"

#define TLC59711_DataPacketByteLength 28

void TLC59711_init(void);
void TLC59711_setColor(uint8_t NR_LED, uint32_t color);
uint8_t *TLC59711_CreateDataPacket(void);
uint8_t *TLC59711_CreateDataStream(uint8_t *Data, uint8_t length);
void TLC59711_SendDataPacket(uint8_t *Data, uint16_t Datasize) ;
void TLC59711_BrightnessSet(uint8_t percent);
void TLC59711_BrightnessReset(void);
void TLC59711_Brightness_Fade(uint8_t fade, uint8_t step);
uint8_t TLC59711_BrightnessGet(void);


typedef struct 
{
  uint16_t B;    // blue
  uint16_t G;    // green
  uint16_t R;    // red
}LED_GS;

typedef struct 
{
  uint8_t B;    // blue
  uint8_t G;    // green
  uint8_t R;    // red
}LED_BC;

#endif
