//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: FOCInnerLoop.h
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 2.96
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Fri Apr  1 11:36:16 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_FOCInnerLoop_h_
#define RTW_HEADER_FOCInnerLoop_h_
#include <cmath>
#include "rtwtypes.h"
#include "zero_crossing_types.h"
#include "control_foc_types.h"
#include "rtGetNaN.h"
#include "rt_nonfinite.h"
#include "arm_math.h"
#ifndef control_foc_MDLREF_HIDE_CHILD_

extern void FOCInnerLoop_Init(void);
extern void FOCInnerLoop(const Flags *rtu_Flags, const ConfigurationParameters
  *rtu_ConfigurationParameters, const SensorsData *rtu_Sensors, const
  EstimatedData *rtu_Estimates, const Targets *rtu_Targets, const
  ControlOuterOutputs *rtu_OuterOutputs, ControlOutputs *rty_FOCOutputs);

#endif                                 //control_foc_MDLREF_HIDE_CHILD_
#endif                                 // RTW_HEADER_FOCInnerLoop_h_

//
// File trailer for generated code.
//
// [EOF]
//
