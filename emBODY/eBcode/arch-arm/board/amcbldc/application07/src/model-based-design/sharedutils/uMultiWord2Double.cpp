//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: uMultiWord2Double.cpp
//
// Code generated for Simulink model 'SupervisorFSM_RX'.
//
// Model version                  : 4.6
// Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
// C/C++ source code generated on : Tue Jun  7 15:41:38 2022
//
#include "uMultiWord2Double.h"
#include <cmath>
#include "rtwtypes.h"

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
