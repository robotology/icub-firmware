//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: Double2MultiWord.cpp
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 1.280
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Mon Sep 20 12:43:37 2021
//
#include "rtwtypes.h"
#include "div_s32.h"
#include <cmath>
#include <math.h>
#include "Double2MultiWord.h"

void Double2MultiWord(real_T u1, uint32_T y[], int32_T n)
{
  real_T yd;
  real_T yn;
  int32_T currExp;
  int32_T i;
  int32_T msl;
  int32_T prevExp;
  uint32_T cb;
  uint32_T u1i;
  boolean_T isNegative;
  isNegative = (u1 < 0.0);
  yn = frexp(u1, &currExp);
  msl = currExp <= 0 ? -1 : div_s32(currExp - 1, 32);
  cb = 1U;
  for (i = msl + 1; i < n; i++) {
    y[i] = 0U;
  }

  yn = isNegative ? -yn : yn;
  prevExp = 32 * msl;
  for (i = msl; i >= 0; i--) {
    yn = std::ldexp(yn, currExp - prevExp);
    yd = std::floor(yn);
    yn -= yd;
    if (i < n) {
      y[i] = static_cast<uint32_T>(yd);
    }

    currExp = prevExp;
    prevExp -= 32;
  }

  if (isNegative) {
    for (i = 0; i < n; i++) {
      u1i = ~y[i];
      cb += u1i;
      y[i] = cb;
      cb = (cb < u1i);
    }
  }
}

//
// File trailer for generated code.
//
// [EOF]
//
