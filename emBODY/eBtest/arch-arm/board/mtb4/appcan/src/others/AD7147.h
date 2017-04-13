/*
 * AD7147.h
 *
 *  Created on: Oct 16, 2015
 *      Author: mmaggiali
 */

#ifndef APPLICATION_USER_AD7147_H_
#define APPLICATION_USER_AD7147_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
    
void ServiceAD7147Isr(unsigned char Channel);
void TrianglesInit(unsigned char Channel);
void SetCDCoffsetOnAllTriangles(unsigned char Channel, uint16_t cdcOffset);
    

extern void ad7147_init(void);
extern void ad7147_acquire(void);
extern uint16_t * ad7147_get12valuesoftriangle(uint8_t trg); 
    

typedef struct error_cap
{
    uint16_t error_outofrange;
    uint16_t error;
} error_cap;

enum Errors
    {
      error_ok,
      error_noack,
      error_notconnected
    };


typedef struct
{
    uint16_t                    shift;
    uint8_t                     indexInSet; //triangle index in set
    uint8_t                     setNum; //equal to sda num
    uint16_t                    isToUpdate; //if =1 triangle needs to be recalibrated
    uint16_t                    CDCoffset;
}triangle_cfg_t;

#define triangles_max_num   16
#define MAXVAL 255
#define MINVAL   0
#define NOLOAD 245

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 


#endif /* APPLICATION_USER_AD7147_H_ */

