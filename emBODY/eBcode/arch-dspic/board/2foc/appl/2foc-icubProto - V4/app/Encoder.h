//   
//  Encoder related rouines:
//  
#ifndef __ENCODER_H
#define __ENCODER_H

#include "UserTypes.h"
#include "UserParms.h"
#include "system.h"

extern void EncoderInit();

extern inline unsigned int EncoderPositionQE();
extern inline int EncoderPositionDHS();

#endif
