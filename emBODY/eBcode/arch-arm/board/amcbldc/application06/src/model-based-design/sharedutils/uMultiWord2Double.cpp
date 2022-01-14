//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: uMultiWord2Double.cpp
//
// Code generated for Simulink model 'SupervisorFSM_RX'.
//
// Model version                  : 3.139
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Thu Jan 13 14:09:31 2022
//
#include "rtwtypes.h"
#include <cmath>
#include "uMultiWord2Double.h"

real_T uMultiWord2Double(const uint32_T u1[], int32_T n1, int32_T e1)
{
  real_T y;
  int32_T exp_0;
  y = 0.0;
  exp_0 = e1;
  for (int32_T i = 0; i < n1; i++) {
    y += std::ldexp(static_cast<real_T>(u1[i]), exp_0);
    exp_0 += 32;
  }

  return y;
}

//
// File trailer for generated code.
//
// [EOF]
//
