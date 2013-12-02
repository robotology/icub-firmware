//
// data structure for I2T protection 
//

#ifndef __I2T_H
#define __I2T_H

#include "UserParms.h"
#include "Park.h"

#ifdef I2T_FILTER_IMPLEMENTATION
#define I2T_PARAM I2T_TAU2048
#define I2T_THRESHOLD I2T_CURRENT_THRESHOLD
#define I2T I2TFilter
#elif defined(I2T_INTEGRATOR1_IMPLEMENTATION)
#define I2T_PARAM I2T_NOMINAL_IMAX
#define I2T_THRESHOLD I2T_ERROR_THRESHOLD
#define I2T I2TIntegral
#else   //I2T_INTEGRATOR2_IMPLEMENTATION
#define I2T_PARAM I2T_NOMINAL_IMAX
#define I2T_THRESHOLD I2T_ERROR_THRESHOLD
#define I2T I2TIntegral2

#endif

typedef struct 
{
    // Word 0

    SFRAC16 Param; // max nominal current. For implementation 1 
                    // filter K * 2048. For implementation 2

    SFRAC16 IThreshold; // Error limit for I2T (implementation 1) or
                        // filtered current threshold for implementation 2
 
    unsigned int Acc[3]; // accumulator state (ACCAL, ACCAH, ACCAU)
	SFRAC16 IQMeasured; // measued current value 
	SFRAC16 IDMeasured; // measued current value 
    
} tI2T;

extern int I2T(tI2T*);
extern void I2TInit(tI2T*);

extern tI2T I2Tdata;


#endif
