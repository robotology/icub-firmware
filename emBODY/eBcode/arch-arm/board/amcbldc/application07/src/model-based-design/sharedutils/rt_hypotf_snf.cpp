//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: rt_hypotf_snf.cpp
//
// Code generated for Simulink model 'estimation_velocity'.
//
// Model version                  : 3.3
// Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
// C/C++ source code generated on : Tue Jun  7 15:42:35 2022
//
#include "rtwtypes.h"
#include "rt_hypotf_snf.h"
#include <cmath>

extern "C" {

#include "rt_nonfinite.h"

}
#include "mw_cmsis.h"

real32_T rt_hypotf_snf(real32_T u0, real32_T u1)
{
  real32_T a;
  real32_T tmp;
  real32_T y;
  a = std::abs(u0);
  y = std::abs(u1);
  if (a < y) {
    a /= y;
    mw_arm_sqrt_f32(a * a + 1.0F, &tmp);
    y *= tmp;
  } else if (a > y) {
    y /= a;
    mw_arm_sqrt_f32(y * y + 1.0F, &tmp);
    y = tmp * a;
  } else if (!rtIsNaNF(y)) {
    y = a * 1.41421354F;
  }

  return y;
}

//
// File trailer for generated code.
//
// [EOF]
//
