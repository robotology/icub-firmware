#ifndef __pwm_decoupling_h__
#define __pwm_decoupling_h__

#include "dsp56f807.h"

//#define USE_NEW_DECOUPLING 1
//#define USE_PARAMETRIC_DECOUPLING 1

#ifdef USE_NEW_DECOUPLING
	#warning USING POSITION+TORQUE DECOUPLING ALGORITHM 
#else
    #warning USING POSITION DECOUPLING ALGORITHM ONLY
#endif

void decouple_dutycycle (Int32 *);
void decouple_dutycycle_new_joint(Int32 *pwm);

#endif