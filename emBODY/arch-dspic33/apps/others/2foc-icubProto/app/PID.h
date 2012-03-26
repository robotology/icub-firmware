// 
// Definitions for assembler PID (copied from Microchip DSP library)
// 
#ifndef PID_H
#define PID_H

typedef SFRAC16 fractional;


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


// Clear the PID state variables and output sample
extern void PIDInit ( tPID* controller ); // pointer to PID data structure

/* Derive A, B and C coefficients using PID gain values-Kp, Ki & Kd*/
extern void PIDCoeffCalc( fractional* kCoeffs, tPID* controller );

#endif