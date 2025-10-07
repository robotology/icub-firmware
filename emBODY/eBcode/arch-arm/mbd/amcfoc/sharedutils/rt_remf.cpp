//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: rt_remf.cpp
//
// Code generated for Simulink model 'process_sensors'.
//
// Model version                  : 1.13
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Mon Sep 22 10:03:15 2025
//
#include "rtwtypes.h"
#include "rt_remf.h"
#include <cmath>
#include <cfloat>

real32_T rt_remf(real32_T u0, real32_T u1)
{
  real32_T q;
  real32_T y;
  if (u1 < 0.0F) {
    q = std::ceil(u1);
  } else {
    q = std::floor(u1);
  }

  if ((u1 != 0.0F) && (u1 != q)) {
    q = std::abs(u0 / u1);
    if (std::abs(q - std::floor(q + 0.5F)) <= FLT_EPSILON * q) {
      y = 0.0F;
    } else {
      y = std::fmod(u0, u1);
    }
  } else {
    y = std::fmod(u0, u1);
  }

  return y;
}

//
// File trailer for generated code.
//
// [EOF]
//
