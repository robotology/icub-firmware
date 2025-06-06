//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: mul_s32_sat.cpp
//
// Code generated for Simulink model 'iterative_motion_controller'.
//
// Model version                  : 3.14
// Simulink Coder version         : 24.2 (R2024b) 21-Jun-2024
// C/C++ source code generated on : Fri May 23 10:49:48 2025
//
#include "mul_s32_sat.h"
#include "mul_wide_s32.h"
#include "rtwtypes.h"

int32_T mul_s32_sat(int32_T a, int32_T b)
{
  int32_T result;
  uint32_T u32_chi;
  uint32_T u32_clo;
  mul_wide_s32(a, b, &u32_chi, &u32_clo);
  if ((static_cast<int32_T>(u32_chi) > 0) || ((u32_chi == 0U) && (u32_clo >=
        2147483648U))) {
    result = MAX_int32_T;
  } else if ((static_cast<int32_T>(u32_chi) < -1) || ((static_cast<int32_T>
               (u32_chi) == -1) && (u32_clo < 2147483648U))) {
    result = MIN_int32_T;
  } else {
    result = static_cast<int32_T>(u32_clo);
  }

  return result;
}

//
// File trailer for generated code.
//
// [EOF]
//
