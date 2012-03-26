// 
// Definitions for PID routines and assembler PID from Microchip DSP library
// 
#ifndef __PID_H
#define __PID_H

#include <dsp.h>
#include <libq.h>

#include "UserTypes.h"
#include "pi.h"
//#include "pid.h"
#include "pid2.h"
#include "park.h"

#include "Velocity.h"
#include "UserParms.h"


extern void ControllerCurrentDPIDClear();
extern void ControllerCurrentQPIDClear();
extern void ControllerSetCurrentDPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d);
extern void ControllerSetCurrentQPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d);

extern void ControllerGetCurrentDPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d);
extern void ControllerGetCurrentQPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d);

extern void ControllerSetWPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d);
extern void ControllerGetWPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d);


extern tParkParm ParkParm;
extern tCtrlReferences CtrlReferences;
extern volatile tVelocityParm VelocityParm;


extern int TorqueGain;


extern SFRAC16 WPIDError;
extern SFRAC16 QPIDError;
extern SFRAC16 DPIDError;


typedef int fractional;

/*
typedef struct 
{
  fractional* abcCoefficients;  // Pointer to A, B & C coefficients located in X-space
    // These coefficients are derived from  the PID gain values - Kp, Ki and Kd 
  fractional* controlHistory;  // Pointer to 3 delay-line samples located in Y-space
    // with the first sample being the most recent
  fractional controlOutput;  // PID Controller Output
  fractional measuredOutput;  // Measured Output sample
  fractional controlReference;  // Reference Input sample
} tPID;
*/

// Clear the PID state variables and output sample
extern void PIDInit ( tPID* controller ); // pointer to PID data structure
/* Derive A, B and C coefficients using PID gain values-Kp, Ki & Kd*/
extern void PIDCoeffCalc( fractional* kCoeffs, tPID* controller );

extern void OmegaControl(void);
extern void SetupControlParameters(void);
extern void IdIqControl(void);
extern void ZeroRegulators(void);

#endif
