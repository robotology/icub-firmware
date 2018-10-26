 /****************************************************
 *  RFE gif expression - iCub Facility - 2018        *
 *                                                   *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/

// RFE Leds layout

//                RIGHT EYEBROW                                     LEFT EYEBROW
// +----------------------------------------+       +----------------------------------------+
// |                   DL9                  |       |                   DL9                  |
// | DL17    DL13              DL5     DL1  |       | DL17    DL13              DL5     DL1  |
// |                           DL6     DL2  |       | DL18    DL14                           |
// |                   DL10                 |       |                   DL10                 |
// |         DL14                           |       |                           DL6          |
// | DL18                                   |       |                                   DL2  |
// |                   DL11                 |       |                   DL11                 |
// | DL19    DL15              DL7     DL3  |       | DL19    DL15              DL7     DL3  |
// | DL20    DL16                           |       |                           DL8     DL4  |
// |                   DL12                 |       |                   DL12                 |
// |                           DL8          |       |         DL16                           |
// |                                   DL4  |       | DL20                                   |
// +----------------------------------------+       +----------------------------------------+
//                                            MOUTH
//                         +----------------------------------------+
//                         | DL1     DL4       DL9     DL16    DL13 |
//                         |     DL3                       DL15     |
//                         | DL2     DL7   DL10  DL11  DL19    DL14 |
//                         |     DL6                       DL18     |
//                         | DL5     DL8       DL12    DL20    DL17 |
//                         +----------------------------------------+

 
#ifndef GIF_MOUTH_H
#define GIF_MOUTH_H

#include "stm32l4xx_hal.h"
#include "colors.h"

// NEUTRAL
// TALKING
// HAPPY
// SAD
// SURPRISED
// EVIL
// ANGRY
// SHY
// CUNNING

// {DL1, DL2, DL3, DL4, DL5, DL6, DL7, DL8, DL9, DL10, DL11, DL12, DL13, DL14, DL15, DL16, DL17, DL18, DL19, DL20};

uint32_t REYEB0[20] = {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};     // higher eyebrow
uint32_t REYEB1[20] = {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0};
uint32_t REYEB2[20] = {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0};
uint32_t REYEB3[20] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1};     // lower eyebrow

uint32_t LEYEB0[20] = {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};     // higher eyebrow
uint32_t LEYEB1[20] = {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0};
uint32_t LEYEB2[20] = {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0};
uint32_t LEYEB3[20] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1};     // lower eyebrow

uint32_t MOUTH0[20] = {1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0};     

uint32_t FACE_MASK[60] = {0};

uint32_t FACE0[60] = {Blue,Blue,White,Blue,Black,Blue,White,Blue,Blue,White,White,Blue,Blue,Blue,White,Blue,Black,Blue,White,Blue,
                       Blue,Black,Black,Black,Blue,Black,Black,Black,Blue,Black,Black,Black,Blue,Black,Black,Black,Blue,Black,Black,Black,
                       Blue,Black,Black,Black,Blue,Black,Black,Black,Blue,Black,Black,Black,Blue,Black,Black,Black,Blue,Black,Black,Black};

uint32_t FACE1[60] = {Green,Green,White,Green,Black,Green,White,Green,Green,White,White,Green,Green,Green,White,Green,Black,Green,White,Green,
                       Black,Green,Black,Black,Black,Green,Black,Black,Black,Green,Black,Black,Black,Green,Black,Black,Black,Green,Black,Black,
                       Black,Green,Black,Black,Black,Green,Black,Black,Black,Green,Black,Black,Black,Green,Black,Black,Black,Green,Black,Black};

uint32_t FACE2[60] = {Black, Red, Red, Red, Black, Red, Black, Red, Red, Black, Black, Red, Black, Red, Red, Red, Black, Red, Black, Red,
                       Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,
                       Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple};

uint32_t FACE3[60] = {Black, Red, Red, Red, Black, Red, Black, Red, Red, Black, Black, Red, Black, Red, Red, Red, Black, Red, Black, Red,
                       Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,
                       Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black};

uint32_t FACE4[60] = {Black, Red, Red, Red, Black, Red, Black, Red, Red, Black, Black, Red, Black, Red, Red, Red, Black, Red, Black, Red,
                       Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,
                       Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black};

uint32_t FACE5[60] = {Black, Red, Red, Red, Black, Red, Black, Red, Red, Black, Black, Red, Black, Red, Red, Red, Black, Red, Black, Red,
                       Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,
                       Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black,Purple,Black,Black,Black};
                       
//uint32_t SMILE1[20] = {Black,Black,Cyan,Black,Black,Black,Cyan,Black,Black,Cyan,Cyan,Black,Black,Black,Cyan,Black,Black,Black,Cyan,Black};
//uint32_t SMILE2[20] = {Blue,Blue,Black,Blue,Black,Blue,Black,Blue,Blue,Black,Black,Blue,Blue,Blue,Black,Blue,Black,Blue,Black,Blue};
//uint32_t SMILE3[20] = {Blue,Blue,White,Blue,Black,Blue,White,Blue,Blue,White,White,Blue,Blue,Blue,White,Blue,Black,Blue,White,Blue};
//uint32_t SMILE4[20] = {Purple,Purple,White,Purple,Black,Purple,White,Purple,Purple,White,White,Purple,Purple,Purple,White,Purple,Black,Purple,White,Purple};
//uint32_t SMILE5[20] = {Green,Green,Black,Green,Black,Green,Black,Green,Green,Black,Black,Green,Green,Green,Black,Green,Black,Green,Black,Green};
//uint32_t SMILE6[20] = {Black, Black, Black, Black, Black, Black, White, Black, Black, White, White, Black, Black, Black, Black, Black, Black, Black, White, Black};
//uint32_t SMILE7[20] = {Black, Red, Red, Red, Black, Red, Black, Red, Red, Black, Black, Red, Black, Red, Red, Red, Black, Red, Black, Red};
//uint32_t SMILE8[20] = {Black, Black, Black, Black, Black, Purple, Black, Purple, Black, Black, Black, Purple, Black, Black, Black, Black, Black, Purple, Black, Purple};
//uint32_t SMILE9[20] = {Blue, Blue, White, Blue, Black, White, Black, Blue, Blue, Black, Black, Blue, Blue, Blue, White, Blue, Black, White, Black, Blue};
  
#endif
