// 
// Definitions for PID routines and assembler PID.
// Different implementations from Microchip DSP library
// or developed in EDL can be enabled by userparm defines
// 
#ifndef __PID_H
#define __PID_H

#include <dsp.h>
#include <libq.h>

#include "UserTypes.h"
#include "pi.h"
#include "pid2.h"
#include "park.h"

#include "Velocity.h"
#include "UserParms.h"


extern void ControllerCurrentDPIDClear();
extern void ControllerCurrentQPIDClear();
extern void ControllerSetCurrentDPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d, SFRAC16 max);
extern void ControllerSetCurrentQPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d, SFRAC16 max);

extern void ControllerGetCurrentDPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d, SFRAC16 *max);
extern void ControllerGetCurrentQPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d, SFRAC16 *max);

extern void ControllerSetWPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d, SFRAC16 max);
extern void ControllerGetWPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d, SFRAC16 *max);


extern tParkParm ParkParm;
extern tCtrlReferences CtrlReferences;
extern volatile tVelocityParm VelocityParm;


extern int TorqueGain;


extern SFRAC16 WPIDError;
extern SFRAC16 QPIDError;
extern SFRAC16 DPIDError;


typedef int fractional;


// Clear the PID state variables and output sample
extern void PIDInit ( tPID* controller ); // pointer to PID data structure

// Derive A, B and C coefficients using PID gain values-Kp, Ki & Kd
extern void PIDCoeffCalc( fractional* kCoeffs, tPID* controller );

extern void OmegaControl(void);
extern void SetupControlParameters(void);
extern void IdIqControl(void);
extern void ZeroRegulators(void);

#endif
