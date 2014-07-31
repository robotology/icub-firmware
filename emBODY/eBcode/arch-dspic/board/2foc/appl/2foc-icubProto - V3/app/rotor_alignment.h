#ifndef __ROTOR_ALIGNMENT_H__
#define __ROTOR_ALIGNMENT_H__


#include "UserTypes.h"

extern void RotorAlignStart();
extern void RotorAlignStep(SFRAC16 reference_encoder_position, SFRAC16 rotor_alignment_encoder_position);
extern volatile short IraqVd;
extern volatile short IraqVq; 
extern volatile short IraqVd_max;
extern volatile short IraqVq_max; 
extern volatile short Ira_direction;
extern volatile short Ira_ramp_steepness;
#endif
