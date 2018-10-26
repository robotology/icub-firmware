 /****************************************************
 *  BFE board - iCub Facility - 2016                 *
 *  TLC59711 configuration                           *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/
 
 // DL1     DL4       DL9     DL16    DL13
 //     DL3                       DL15
 // DL2     DL7   DL10  DL11  DL19    DL14
 //     DL6                       DL18
 // DL5     DL8       DL12    DL20    DL17
 
#ifndef BFE_Mouth_H
#define BFE_Mouth_H

#include "stm32l4xx_hal.h"
#include "tlc59711.h"

// ----------------------------------------------------------------------------
// Define
// ----------------------------------------------------------------------------
#define RFE_Expression_NrDriverTLC59711 5*3
#define RFE_Expression_StreamLength 140*3

void RFE_Expression(uint32_t *Expression, uint32_t color, uint8_t *RFE_Stream, uint16_t size);

#endif
