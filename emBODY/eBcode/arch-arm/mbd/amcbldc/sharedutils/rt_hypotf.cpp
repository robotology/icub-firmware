//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: rt_hypotf.cpp
//
// Code generated for Simulink model 'estimation_velocity'.
//
// Model version                  : 8.11
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Thu Oct  9 17:31:03 2025
//
#include "rtwtypes.h"
#include "rt_hypotf.h"
#include <cmath>

real32_T rt_hypotf(real32_T u0, real32_T u1)
{
  real32_T a;
  real32_T b;
  real32_T y;
  a = std::abs(u0);
  b = std::abs(u1);
  if (a < b) {
    a /= b;
    y = std::sqrt(a * a + 1.0F) * b;
  } else if (a > b) {
    b /= a;
    y = std::sqrt(b * b + 1.0F) * a;
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
