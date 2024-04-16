//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: rt_hypotf.cpp
//
// Code generated for Simulink model 'estimation_velocity'.
//
// Model version                  : 6.21
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Tue Apr 16 11:31:02 2024
//
#include "rtwtypes.h"
#include "rt_hypotf.h"
#include <cmath>
#include "mw_cmsis.h"

real32_T rt_hypotf(real32_T u0, real32_T u1)
{
  real32_T a;
  real32_T b;
  real32_T tmp;
  real32_T y;
  a = std::abs(u0);
  b = std::abs(u1);
  if (a < b) {
    a /= b;
    mw_arm_sqrt_f32(a * a + 1.0F, &tmp);
    y = tmp * b;
  } else if (a > b) {
    b /= a;
    mw_arm_sqrt_f32(b * b + 1.0F, &tmp);
    y = tmp * a;
  } else {
    y = a * 1.41421354F;
  }

  return y;
}

//
// File trailer for generated code.
//
// [EOF]
//
