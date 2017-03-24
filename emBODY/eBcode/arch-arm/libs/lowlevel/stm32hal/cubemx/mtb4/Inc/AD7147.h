/*
 * AD7147.h
 *
 *  Created on: Oct 16, 2015
 *      Author: mmaggiali
 */

#ifndef APPLICATION_USER_AD7147_H_
#define APPLICATION_USER_AD7147_H_

void ServiceAD7147Isr(unsigned char Channel);
void TrianglesInit(unsigned char Channel);
void SetCDCoffsetOnAllTriangles(unsigned char Channel, unsigned int cdcOffset);

typedef struct error_cap
{
    unsigned int error_outofrange;
    unsigned int error;
} error_cap;

enum Errors
    {
      error_ok,
      error_noack,
      error_notconnected
    };


typedef struct
{
    unsigned int                    shift;
    unsigned int                    indexInSet; //triangle index in set
    unsigned int                    setNum; //equal to sda num
    unsigned int                    isToUpdate; //if =1 triangle needs to be recalibrated
    unsigned int                   CDCoffset;
}triangle_cfg_t;

#define triangles_max_num   16
#define MAXVAL 255
#define MINVAL   0
#define NOLOAD 245


#endif /* APPLICATION_USER_AD7147_H_ */

