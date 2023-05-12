//   
//  Hes management routines
//

#ifndef __DHES_H__
#define __DHES_H__

#include <p33FJ128MC802.h>
#include <DSP.h>

extern void DHESInit(int num_poles, int offset);
extern unsigned char DHESRead();
extern char DHESSector();
extern long DHESPosition();
extern int DHESVelocity();
extern int DHESAngle();

#endif
