 /****************************************************
 *  BFE board - iCub Facility - 2016                 *
 *  TLC59711 configuration                           *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/
 
 // BC: Global Brightness Control
 // GS: Grayscale

 
#include "stm32l4xx_hal.h"
#include "tlc59711.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "colors.h"

// ----------------------------------------------------------------------------
// Define
// ----------------------------------------------------------------------------
LED_GS LED[4] = {0};            // initialize to LED off
LED_BC Brightness = {0};        // initialize brightness to off, max 0x7F (7 bits)
#define GlobalBrightness_MAX 0x7F     // maximum value allowed
uint16_t GlobalBrightness=0;

uint8_t WRITE_CMD=0x25;       // 6 bits, 0x25
uint8_t BLANK=0;
uint8_t DSPRPT=1;
uint8_t TMGRST=1;
uint8_t EXTGCK=0;
uint8_t OUTTMG=1;
uint8_t FUNCTION_CTRL;

uint8_t TLC59711_DataPacket[TLC59711_DataPacketByteLength] = {0};
uint8_t DataPacket[TLC59711_DataPacketByteLength] = {0};

extern uint8_t fade;

// ----------------------------------------------------------------------------
// TLC59711 Initialization
// ----------------------------------------------------------------------------
void TLC59711_init(void) {
  uint8_t i=0;
  for(i=0; i<4; i++){
    LED[i].R=0;
    LED[i].G=0;
    LED[i].B=0;
  }
}

// ----------------------------------------------------------------------------
// TLC59711 Color mixer
// ----------------------------------------------------------------------------
void TLC59711_setColor(uint8_t NR_LED, uint32_t color) {
  uint32_t RED, GREEN, BLUE;
  RED   = ((color & 0xFF0000)>>0x10)*0x101;
  GREEN = ((color & 0x00FF00)>>0x08)*0x101;
  BLUE  = ((color & 0x0000FF))*0x101;
  
  LED[NR_LED].R=(uint16_t)RED;
  LED[NR_LED].G=(uint16_t)GREEN;
  LED[NR_LED].B=(uint16_t)BLUE;
}

// ----------------------------------------------------------------------------
// TLC59711 Set % brightness
// ----------------------------------------------------------------------------
void TLC59711_BrightnessSet(uint8_t percent) {
  GlobalBrightness=(GlobalBrightness_MAX*percent)/100;
  
  Brightness.R=GlobalBrightness;
  Brightness.G=GlobalBrightness;
  Brightness.B=GlobalBrightness;
}

// ----------------------------------------------------------------------------
// TLC59711 Get brightness values
// ----------------------------------------------------------------------------
uint8_t TLC59711_BrightnessGet(void) {
  return Brightness.R;    // per ora va bene così perchè i canali R G B hanno lo stesso valore di brightness
}

// ----------------------------------------------------------------------------
// TLC59711 Reset brightness to 0
// ----------------------------------------------------------------------------
void TLC59711_BrightnessReset(void) {
  Brightness.R=0;
  Brightness.G=0;
  Brightness.B=0;
}

// ----------------------------------------------------------------------------
// TLC59711 Brightness Fading
// ----------------------------------------------------------------------------
void TLC59711_Brightness_Fade(uint8_t fade, uint8_t step) {
  if(fade==FadeDOWN){
    if(Brightness.R > step) {Brightness.R-=step;}
    else                    {Brightness.R=0;}
    if(Brightness.G > step) {Brightness.G-=step;}
    else                    {Brightness.G=0;}
    if(Brightness.B > step) {Brightness.B-=step;}
    else                    {Brightness.B=0;}
    
    if(Brightness.R==0 && Brightness.G==0 && Brightness.B==0){
      HAL_TIM_Base_Stop_IT(&htim6);
    }
  }
  else if(fade==FadeUP){
    if(Brightness.R < (GlobalBrightness-step))  {Brightness.R+=step;}
    else                                        {Brightness.R=GlobalBrightness;}
    if(Brightness.G < (GlobalBrightness-step))  {Brightness.G+=step;}
    else                                        {Brightness.G=GlobalBrightness;}
    if(Brightness.B < (GlobalBrightness-step))  {Brightness.B+=step;}
    else                                        {Brightness.B=GlobalBrightness;}
    
    if(Brightness.R == GlobalBrightness && Brightness.G == GlobalBrightness && Brightness.B == GlobalBrightness){
      HAL_TIM_Base_Stop_IT(&htim6);
    }
  }
}

// ----------------------------------------------------------------------------
// TLC59711 Data packet creation
// ----------------------------------------------------------------------------
uint8_t *TLC59711_CreateDataPacket(void) {
  FUNCTION_CTRL=(OUTTMG<<4) + (EXTGCK<<3) + (TMGRST<<2) + (DSPRPT<<1) + BLANK;
  
  TLC59711_DataPacket[ 0] = (WRITE_CMD<<2)     + (FUNCTION_CTRL>>3);
  TLC59711_DataPacket[ 1] = (FUNCTION_CTRL<<5) + (Brightness.B>>2);
  TLC59711_DataPacket[ 2] = (Brightness.B<<6)  + (Brightness.G>>1);
  TLC59711_DataPacket[ 3] = (Brightness.G<<7)  + (Brightness.R);
  
  TLC59711_DataPacket[ 4] = LED[3].B>>8;
  TLC59711_DataPacket[ 5] = LED[3].B;
  TLC59711_DataPacket[ 6] = LED[3].G>>8;
  TLC59711_DataPacket[ 7] = LED[3].G;
  TLC59711_DataPacket[ 8] = LED[3].R>>8;
  TLC59711_DataPacket[ 9] = LED[3].R;
  
  TLC59711_DataPacket[10] = LED[2].B>>8;
  TLC59711_DataPacket[11] = LED[2].B;
  TLC59711_DataPacket[12] = LED[2].G>>8;
  TLC59711_DataPacket[13] = LED[2].G;
  TLC59711_DataPacket[14] = LED[2].R>>8;
  TLC59711_DataPacket[15] = LED[2].R;
  
  TLC59711_DataPacket[16] = LED[1].B>>8;
  TLC59711_DataPacket[17] = LED[1].B;
  TLC59711_DataPacket[18] = LED[1].G>>8;
  TLC59711_DataPacket[19] = LED[1].G;
  TLC59711_DataPacket[20] = LED[1].R>>8;
  TLC59711_DataPacket[21] = LED[1].R;
  
  TLC59711_DataPacket[22] = LED[0].B>>8;
  TLC59711_DataPacket[23] = LED[0].B;
  TLC59711_DataPacket[24] = LED[0].G>>8;
  TLC59711_DataPacket[25] = LED[0].G;
  TLC59711_DataPacket[26] = LED[0].R>>8;
  TLC59711_DataPacket[27] = LED[0].R;
    
  return TLC59711_DataPacket;
}

// ----------------------------------------------------------------------------
// TLC59711 Data stream multipacket creation (for N drivers in daisy chain)
// ----------------------------------------------------------------------------
uint8_t *TLC59711_CreateDataStream(uint8_t *Data, uint8_t length) {
//  for(i=0; i<TLC59711_DataPacketByteLength; i++){
//    BFE_Stream[i+TLC59711_DataPacketByteLength*nr_tlc]=Full_DataPacket[nr_tlc][i];
//  }
  //return TLC59711_DataStream;
  return NULL;
}

// ----------------------------------------------------------------------------
// TLC59711 Data packet transmission
// ----------------------------------------------------------------------------
void TLC59711_SendDataPacket(uint8_t *Data, uint16_t Datasize) {
//  uint8_t i=0;
//  for(i=0; i<TLC59711_DataPacketBitLength; i++){             // inverto gli elementi dell'array solo per la trasmissione
//    DataPacket[i]=Data[TLC59711_DataPacketBitLength-1-i];
//  }
//  HAL_SPI_Transmit_DMA(&hspi3, DataPacket, sizeof(DataPacket));
  HAL_SPI_Transmit_DMA(&hspi1, Data, Datasize);
}



