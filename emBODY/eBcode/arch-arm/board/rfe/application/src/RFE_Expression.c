 /****************************************************
 *  iCub BFE MOUTH board - iCub Facility - 2016      *
 *                                                   *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/
 
#include <stdio.h>
#include <string.h>
#include "stm32l4xx_hal.h"
#include "tim.h"
#include "spi.h"
#include "RFE_Expression.h"


// -----------------------------------------------------------------------------------------------------------------------------
// RFE_Expression creates the data stream for RFE_MASTER board
// -----------------------------------------------------------------------------------------------------------------------------
void RFE_Expression(uint32_t *Expression, uint32_t color, uint8_t *RFE_Stream, uint16_t size){
  uint8_t i;
  uint8_t nr_tlc, nr_led = 0;
  //uint32_t MASK[20]={0};
  uint32_t MASK[60]={0};
  uint8_t Full_DataPacket[RFE_Expression_NrDriverTLC59711][TLC59711_DataPacketByteLength]={0};

  //memcpy(MASK, Expression, 80);     // 80 perchè sono 20 elementi da 4 byte l'uno (uint32_t)
  memcpy(MASK, Expression, 240);     // 240 perchè sono 60 elementi da 4 byte l'uno (uint32_t)
  
  for(nr_tlc=0; nr_tlc<RFE_Expression_NrDriverTLC59711; nr_tlc++){   // board array scan
    TLC59711_init();                                            // LED initialization to 0 (off)
    for(nr_led=0; nr_led<4; nr_led++){                          // single TLC scan (4 LEDs)
      if(color == DontChange){
        TLC59711_setColor(3-nr_led,MASK[nr_led+4*(nr_tlc)]);
      }
      else{
        if(MASK[nr_led+4*(nr_tlc)] != 0){                         // if LED is not off switch on with the choosen color
          TLC59711_setColor(3-nr_led,color);
        }
      } 
    }
    memcpy(&Full_DataPacket[nr_tlc][0], TLC59711_CreateDataPacket(), TLC59711_DataPacketByteLength);

    for(i=0; i<TLC59711_DataPacketByteLength; i++){
      RFE_Stream[i+TLC59711_DataPacketByteLength*(RFE_Expression_NrDriverTLC59711-1-nr_tlc)]=Full_DataPacket[nr_tlc][i];
    }
  }
}


