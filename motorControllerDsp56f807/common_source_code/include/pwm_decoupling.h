#ifndef __pwm_decoupling_h__
#define __pwm_decoupling_h__

#include "dsp56f807.h"

void decouple_dutycycle_old (Int32 *pwm);
void decouple_dutycycle     (Int32 *pwm);
#endif