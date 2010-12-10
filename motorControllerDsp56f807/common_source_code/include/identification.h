#ifndef __identification_h__
#define __identification_h__

#include "controller.h"
#include "controller.h"

//exports the following variables
extern Int16 sine_ampl[JN];
extern float sine_freq[JN];
extern float wt[JN];
extern bool openloop_identif;

double sin					(double rad);
void compute_identif_wt		(int j);
void compute_sweep_wt		(int j);
void reset_identif			(int j, int s_max, int s_freq_init, int s_fstep);



#endif