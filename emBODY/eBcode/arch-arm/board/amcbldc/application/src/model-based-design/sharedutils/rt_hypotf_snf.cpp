//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: rt_hypotf_snf.cpp
//
// Code generated for Simulink model 'estimation_velocity'.
//
// Model version                  : 4.0
// Simulink Coder version         : 9.8 (R2022b) 13-May-2022
// C/C++ source code generated on : Fri Feb 10 13:58:07 2023
//
#include "rtwtypes.h"
#include "rt_hypotf_snf.h"
#include <cmath>

extern "C"
{

#include "rt_nonfinite.h"

}

#include "mw_cmsis.h"
#include "rtGetNaN.h"

real32_T rt_hypotf_snf(real32_T u0, real32_T u1)
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
  } else if (rtIsNaNF(b)) {
    y = (rtNaNF);
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
