//   
//  Encoder related rouines:
//  
//  position, velocity, acceleration functions
//  Initial Phase alinement 
//  
//  Available drivers for:
//    Increental encoder with (AS5040) or without index pulse (AS530x) Avago, Mecapion...
//    Absolute SPI encoders AS5045
//    Absolute+incremental TLE5012
//    DHES 

// TODO: fare tutto l'error checking possibile sull' encoder (counter overflow, missing index...)
//  deve essere possibile capire quando viene tagliato il cavo !!!

#include <p33FJ128MC802.h>
#include <dsp.h>

#include "Encoder.h"

#include "qep.h"
//#include "Faults.h"
#include "DHES.h"

void EncoderInit()
{
    QEPInit();

    //DHESInit();
}

inline unsigned int EncoderPositionQE()  __attribute__((always_inline));
inline unsigned int EncoderPositionQE()
{
    return //__builtin_divud(16384UL*QEPGet(),QE_PPR);
            __builtin_divud(((unsigned long)QEPGet())<<14,QE_PPR);
}

/*
inline int EncoderPositionDHS() __attribute__((always_inline));
inline int EncoderPositionDHS()
{
  return DHESRead();
}
*/
