// 
// Definitions for assembler PID2 algo (copied from Microchip DSP library)
// 
#ifndef PID2_H
#define PID2_H

typedef SFRAC16 fractional;


typedef struct 
{
  fractional* IDPMCoefficients;  // Pointer to I, D, P, MAX_I coefficients located in X-space
  fractional* controlState;  // Pointer to 2 SFRAC16 temps samples located in Y-space (prev error and I)
  fractional controlOutput;  // PID Controller Output
  fractional measuredOutput;  // Measured Output sample
  fractional controlReference;  // Reference Input sample
} tPID2;


// Clear the PID2 state variables and output sample
extern void PID2Init (tPID2* controller); // pointer to PID data structure
// Run the PID2 algo iteration
extern fractional* PID2 (tPID2* controller);      


#endif
