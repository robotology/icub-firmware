//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: rt_roundd_snf.cpp
//
// Code generated for Simulink model 'SupervisorFSM_RX'.
//
// Model version                  : 5.6
// Simulink Coder version         : 9.8 (R2022b) 13-May-2022
// C/C++ source code generated on : Fri Dec  9 14:59:21 2022
//
#include "rtwtypes.h"
#include "rt_roundd_snf.h"
#include <cmath>

real_T rt_roundd_snf(real_T u)
{
  real_T y;
  if (std::abs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = std::floor(u + 0.5);
    } else if (u > -0.5) {
      y = u * 0.0;
    } else {
      y = std::ceil(u - 0.5);
    }
  } else {
    y = u;
  }

  return y;
}

//
// File trailer for generated code.
//
// [EOF]
//
