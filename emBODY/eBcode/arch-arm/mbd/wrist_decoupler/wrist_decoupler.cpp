//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: wrist_decoupler.cpp
//
// Code generated for Simulink model 'wrist_decoupler'.
//
// Model version                  : 8.5
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Wed Feb 14 13:47:53 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#include "wrist_decoupler.h"
#include "rtwtypes.h"
#include <cmath>
#include <cstring>
#include <stddef.h>
#include <cfloat>
#define NumBitsPerChar                 8U

extern real_T rt_remd_snf(real_T u0, real_T u1);
extern real_T rt_atan2d_snf(real_T u0, real_T u1);

//===========*
//  Constants *
// ===========
#define RT_PI                          3.14159265358979323846
#define RT_PIF                         3.1415927F
#define RT_LN_10                       2.30258509299404568402
#define RT_LN_10F                      2.3025851F
#define RT_LOG10E                      0.43429448190325182765
#define RT_LOG10EF                     0.43429449F
#define RT_E                           2.7182818284590452354
#define RT_EF                          2.7182817F

//
//  UNUSED_PARAMETER(x)
//    Used to specify that a function parameter (argument) is required but not
//    accessed by the function body.

#ifndef UNUSED_PARAMETER
#if defined(__LCC__)
#define UNUSED_PARAMETER(x)                                      // do nothing
#else

//
//  This is the semi-ANSI standard way of indicating that an
//  unused function parameter is required.

#define UNUSED_PARAMETER(x)            (void) (x)
#endif
#endif

extern "C"
{
  real_T rtInf;
  real_T rtMinusInf;
  real_T rtNaN;
  real32_T rtInfF;
  real32_T rtMinusInfF;
  real32_T rtNaNF;
}

extern "C"
{
  //
  // Initialize rtNaN needed by the generated code.
  // NaN is initialized as non-signaling. Assumes IEEE.
  //
  static real_T rtGetNaN(void)
  {
    size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
    real_T nan = 0.0;
    if (bitsPerReal == 32U) {
      nan = rtGetNaNF();
    } else {
      union {
        LittleEndianIEEEDouble bitVal;
        real_T fltVal;
      } tmpVal;

      tmpVal.bitVal.words.wordH = 0xFFF80000U;
      tmpVal.bitVal.words.wordL = 0x00000000U;
      nan = tmpVal.fltVal;
    }

    return nan;
  }

  //
  // Initialize rtNaNF needed by the generated code.
  // NaN is initialized as non-signaling. Assumes IEEE.
  //
  static real32_T rtGetNaNF(void)
  {
    IEEESingle nanF = { { 0.0F } };

    nanF.wordL.wordLuint = 0xFFC00000U;
    return nanF.wordL.wordLreal;
  }
}

extern "C"
{
  //
  // Initialize the rtInf, rtMinusInf, and rtNaN needed by the
  // generated code. NaN is initialized as non-signaling. Assumes IEEE.
  //
  static void rt_InitInfAndNaN(size_t realSize)
  {
    (void) (realSize);
    rtNaN = rtGetNaN();
    rtNaNF = rtGetNaNF();
    rtInf = rtGetInf();
    rtInfF = rtGetInfF();
    rtMinusInf = rtGetMinusInf();
    rtMinusInfF = rtGetMinusInfF();
  }

  // Test if value is infinite
  static boolean_T rtIsInf(real_T value)
  {
    return (boolean_T)((value==rtInf || value==rtMinusInf) ? 1U : 0U);
  }

  // Test if single-precision value is infinite
  static boolean_T rtIsInfF(real32_T value)
  {
    return (boolean_T)(((value)==rtInfF || (value)==rtMinusInfF) ? 1U : 0U);
  }

  // Test if value is not a number
  static boolean_T rtIsNaN(real_T value)
  {
    boolean_T result = (boolean_T) 0;
    size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
    if (bitsPerReal == 32U) {
      result = rtIsNaNF((real32_T)value);
    } else {
      union {
        LittleEndianIEEEDouble bitVal;
        real_T fltVal;
      } tmpVal;

      tmpVal.fltVal = value;
      result = (boolean_T)((tmpVal.bitVal.words.wordH & 0x7FF00000) ==
                           0x7FF00000 &&
                           ( (tmpVal.bitVal.words.wordH & 0x000FFFFF) != 0 ||
                            (tmpVal.bitVal.words.wordL != 0) ));
    }

    return result;
  }

  // Test if single-precision value is not a number
  static boolean_T rtIsNaNF(real32_T value)
  {
    IEEESingle tmp;
    tmp.wordL.wordLreal = value;
    return (boolean_T)( (tmp.wordL.wordLuint & 0x7F800000) == 0x7F800000 &&
                       (tmp.wordL.wordLuint & 0x007FFFFF) != 0 );
  }
}

extern "C"
{
  //
  // Initialize rtInf needed by the generated code.
  // Inf is initialized as non-signaling. Assumes IEEE.
  //
  static real_T rtGetInf(void)
  {
    size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
    real_T inf = 0.0;
    if (bitsPerReal == 32U) {
      inf = rtGetInfF();
    } else {
      union {
        LittleEndianIEEEDouble bitVal;
        real_T fltVal;
      } tmpVal;

      tmpVal.bitVal.words.wordH = 0x7FF00000U;
      tmpVal.bitVal.words.wordL = 0x00000000U;
      inf = tmpVal.fltVal;
    }

    return inf;
  }

  //
  // Initialize rtInfF needed by the generated code.
  // Inf is initialized as non-signaling. Assumes IEEE.
  //
  static real32_T rtGetInfF(void)
  {
    IEEESingle infF;
    infF.wordL.wordLuint = 0x7F800000U;
    return infF.wordL.wordLreal;
  }

  //
  // Initialize rtMinusInf needed by the generated code.
  // Inf is initialized as non-signaling. Assumes IEEE.
  //
  static real_T rtGetMinusInf(void)
  {
    size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
    real_T minf = 0.0;
    if (bitsPerReal == 32U) {
      minf = rtGetMinusInfF();
    } else {
      union {
        LittleEndianIEEEDouble bitVal;
        real_T fltVal;
      } tmpVal;

      tmpVal.bitVal.words.wordH = 0xFFF00000U;
      tmpVal.bitVal.words.wordL = 0x00000000U;
      minf = tmpVal.fltVal;
    }

    return minf;
  }

  //
  // Initialize rtMinusInfF needed by the generated code.
  // Inf is initialized as non-signaling. Assumes IEEE.
  //
  static real32_T rtGetMinusInfF(void)
  {
    IEEESingle minfF;
    minfF.wordL.wordLuint = 0xFF800000U;
    return minfF.wordL.wordLreal;
  }
}

real_T rt_remd_snf(real_T u0, real_T u1)
{
  real_T y;
  if (rtIsNaN(u0) || rtIsNaN(u1) || rtIsInf(u0)) {
    y = (rtNaN);
  } else if (rtIsInf(u1)) {
    y = u0;
  } else {
    real_T q;
    if (u1 < 0.0) {
      q = std::ceil(u1);
    } else {
      q = std::floor(u1);
    }

    if ((u1 != 0.0) && (u1 != q)) {
      q = std::abs(u0 / u1);
      if (!(std::abs(q - std::floor(q + 0.5)) > DBL_EPSILON * q)) {
        y = 0.0 * u0;
      } else {
        y = std::fmod(u0, u1);
      }
    } else {
      y = std::fmod(u0, u1);
    }
  }

  return y;
}

// Function for MATLAB Function: '<S1>/motors2ypr'
void wrist_decoupler::cosd(real_T x[3])
{
  real_T absx;
  real_T b_x;
  int8_T n;
  if (rtIsInf(x[0]) || rtIsNaN(x[0])) {
    x[0] = (rtNaN);
  } else {
    b_x = rt_remd_snf(x[0], 360.0);
    absx = std::abs(b_x);
    if (absx > 180.0) {
      if (b_x > 0.0) {
        b_x -= 360.0;
      } else {
        b_x += 360.0;
      }

      absx = std::abs(b_x);
    }

    if (absx <= 45.0) {
      b_x *= 0.017453292519943295;
      n = 0;
    } else if (absx <= 135.0) {
      if (b_x > 0.0) {
        b_x = (b_x - 90.0) * 0.017453292519943295;
        n = 1;
      } else {
        b_x = (b_x + 90.0) * 0.017453292519943295;
        n = -1;
      }
    } else if (b_x > 0.0) {
      b_x = (b_x - 180.0) * 0.017453292519943295;
      n = 2;
    } else {
      b_x = (b_x + 180.0) * 0.017453292519943295;
      n = -2;
    }

    switch (n) {
     case 0:
      x[0] = std::cos(b_x);
      break;

     case 1:
      x[0] = -std::sin(b_x);
      break;

     case -1:
      x[0] = std::sin(b_x);
      break;

     default:
      x[0] = -std::cos(b_x);
      break;
    }
  }

  if (rtIsInf(x[1]) || rtIsNaN(x[1])) {
    x[1] = (rtNaN);
  } else {
    b_x = rt_remd_snf(x[1], 360.0);
    absx = std::abs(b_x);
    if (absx > 180.0) {
      if (b_x > 0.0) {
        b_x -= 360.0;
      } else {
        b_x += 360.0;
      }

      absx = std::abs(b_x);
    }

    if (absx <= 45.0) {
      b_x *= 0.017453292519943295;
      n = 0;
    } else if (absx <= 135.0) {
      if (b_x > 0.0) {
        b_x = (b_x - 90.0) * 0.017453292519943295;
        n = 1;
      } else {
        b_x = (b_x + 90.0) * 0.017453292519943295;
        n = -1;
      }
    } else if (b_x > 0.0) {
      b_x = (b_x - 180.0) * 0.017453292519943295;
      n = 2;
    } else {
      b_x = (b_x + 180.0) * 0.017453292519943295;
      n = -2;
    }

    switch (n) {
     case 0:
      x[1] = std::cos(b_x);
      break;

     case 1:
      x[1] = -std::sin(b_x);
      break;

     case -1:
      x[1] = std::sin(b_x);
      break;

     default:
      x[1] = -std::cos(b_x);
      break;
    }
  }

  if (rtIsInf(x[2]) || rtIsNaN(x[2])) {
    x[2] = (rtNaN);
  } else {
    b_x = rt_remd_snf(x[2], 360.0);
    absx = std::abs(b_x);
    if (absx > 180.0) {
      if (b_x > 0.0) {
        b_x -= 360.0;
      } else {
        b_x += 360.0;
      }

      absx = std::abs(b_x);
    }

    if (absx <= 45.0) {
      b_x *= 0.017453292519943295;
      n = 0;
    } else if (absx <= 135.0) {
      if (b_x > 0.0) {
        b_x = (b_x - 90.0) * 0.017453292519943295;
        n = 1;
      } else {
        b_x = (b_x + 90.0) * 0.017453292519943295;
        n = -1;
      }
    } else if (b_x > 0.0) {
      b_x = (b_x - 180.0) * 0.017453292519943295;
      n = 2;
    } else {
      b_x = (b_x + 180.0) * 0.017453292519943295;
      n = -2;
    }

    switch (n) {
     case 0:
      x[2] = std::cos(b_x);
      break;

     case 1:
      x[2] = -std::sin(b_x);
      break;

     case -1:
      x[2] = std::sin(b_x);
      break;

     default:
      x[2] = -std::cos(b_x);
      break;
    }
  }
}

// Function for MATLAB Function: '<S1>/motors2ypr'
void wrist_decoupler::sind(real_T x[3])
{
  real_T absx;
  real_T b_x;
  int8_T n;
  if (rtIsInf(x[0]) || rtIsNaN(x[0])) {
    x[0] = (rtNaN);
  } else {
    b_x = rt_remd_snf(x[0], 360.0);
    absx = std::abs(b_x);
    if (absx > 180.0) {
      if (b_x > 0.0) {
        b_x -= 360.0;
      } else {
        b_x += 360.0;
      }

      absx = std::abs(b_x);
    }

    if (absx <= 45.0) {
      b_x *= 0.017453292519943295;
      n = 0;
    } else if (absx <= 135.0) {
      if (b_x > 0.0) {
        b_x = (b_x - 90.0) * 0.017453292519943295;
        n = 1;
      } else {
        b_x = (b_x + 90.0) * 0.017453292519943295;
        n = -1;
      }
    } else if (b_x > 0.0) {
      b_x = (b_x - 180.0) * 0.017453292519943295;
      n = 2;
    } else {
      b_x = (b_x + 180.0) * 0.017453292519943295;
      n = -2;
    }

    switch (n) {
     case 0:
      x[0] = std::sin(b_x);
      break;

     case 1:
      x[0] = std::cos(b_x);
      break;

     case -1:
      x[0] = -std::cos(b_x);
      break;

     default:
      x[0] = -std::sin(b_x);
      break;
    }
  }

  if (rtIsInf(x[1]) || rtIsNaN(x[1])) {
    x[1] = (rtNaN);
  } else {
    b_x = rt_remd_snf(x[1], 360.0);
    absx = std::abs(b_x);
    if (absx > 180.0) {
      if (b_x > 0.0) {
        b_x -= 360.0;
      } else {
        b_x += 360.0;
      }

      absx = std::abs(b_x);
    }

    if (absx <= 45.0) {
      b_x *= 0.017453292519943295;
      n = 0;
    } else if (absx <= 135.0) {
      if (b_x > 0.0) {
        b_x = (b_x - 90.0) * 0.017453292519943295;
        n = 1;
      } else {
        b_x = (b_x + 90.0) * 0.017453292519943295;
        n = -1;
      }
    } else if (b_x > 0.0) {
      b_x = (b_x - 180.0) * 0.017453292519943295;
      n = 2;
    } else {
      b_x = (b_x + 180.0) * 0.017453292519943295;
      n = -2;
    }

    switch (n) {
     case 0:
      x[1] = std::sin(b_x);
      break;

     case 1:
      x[1] = std::cos(b_x);
      break;

     case -1:
      x[1] = -std::cos(b_x);
      break;

     default:
      x[1] = -std::sin(b_x);
      break;
    }
  }

  if (rtIsInf(x[2]) || rtIsNaN(x[2])) {
    x[2] = (rtNaN);
  } else {
    b_x = rt_remd_snf(x[2], 360.0);
    absx = std::abs(b_x);
    if (absx > 180.0) {
      if (b_x > 0.0) {
        b_x -= 360.0;
      } else {
        b_x += 360.0;
      }

      absx = std::abs(b_x);
    }

    if (absx <= 45.0) {
      b_x *= 0.017453292519943295;
      n = 0;
    } else if (absx <= 135.0) {
      if (b_x > 0.0) {
        b_x = (b_x - 90.0) * 0.017453292519943295;
        n = 1;
      } else {
        b_x = (b_x + 90.0) * 0.017453292519943295;
        n = -1;
      }
    } else if (b_x > 0.0) {
      b_x = (b_x - 180.0) * 0.017453292519943295;
      n = 2;
    } else {
      b_x = (b_x + 180.0) * 0.017453292519943295;
      n = -2;
    }

    switch (n) {
     case 0:
      x[2] = std::sin(b_x);
      break;

     case 1:
      x[2] = std::cos(b_x);
      break;

     case -1:
      x[2] = -std::cos(b_x);
      break;

     default:
      x[2] = -std::sin(b_x);
      break;
    }
  }
}

// Function for MATLAB Function: '<S1>/motors2ypr'
void wrist_decoupler::cosd_f(real_T *x)
{
  real_T absx;
  real_T b_x;
  int8_T n;
  if (rtIsInf(*x) || rtIsNaN(*x)) {
    *x = (rtNaN);
  } else {
    b_x = rt_remd_snf(*x, 360.0);
    absx = std::abs(b_x);
    if (absx > 180.0) {
      if (b_x > 0.0) {
        b_x -= 360.0;
      } else {
        b_x += 360.0;
      }

      absx = std::abs(b_x);
    }

    if (absx <= 45.0) {
      b_x *= 0.017453292519943295;
      n = 0;
    } else if (absx <= 135.0) {
      if (b_x > 0.0) {
        b_x = (b_x - 90.0) * 0.017453292519943295;
        n = 1;
      } else {
        b_x = (b_x + 90.0) * 0.017453292519943295;
        n = -1;
      }
    } else if (b_x > 0.0) {
      b_x = (b_x - 180.0) * 0.017453292519943295;
      n = 2;
    } else {
      b_x = (b_x + 180.0) * 0.017453292519943295;
      n = -2;
    }

    switch (n) {
     case 0:
      *x = std::cos(b_x);
      break;

     case 1:
      *x = -std::sin(b_x);
      break;

     case -1:
      *x = std::sin(b_x);
      break;

     default:
      *x = -std::cos(b_x);
      break;
    }
  }
}

// Function for MATLAB Function: '<S1>/motors2ypr'
void wrist_decoupler::sind_c(real_T *x)
{
  real_T absx;
  real_T b_x;
  int8_T n;
  if (rtIsInf(*x) || rtIsNaN(*x)) {
    *x = (rtNaN);
  } else {
    b_x = rt_remd_snf(*x, 360.0);
    absx = std::abs(b_x);
    if (absx > 180.0) {
      if (b_x > 0.0) {
        b_x -= 360.0;
      } else {
        b_x += 360.0;
      }

      absx = std::abs(b_x);
    }

    if (absx <= 45.0) {
      b_x *= 0.017453292519943295;
      n = 0;
    } else if (absx <= 135.0) {
      if (b_x > 0.0) {
        b_x = (b_x - 90.0) * 0.017453292519943295;
        n = 1;
      } else {
        b_x = (b_x + 90.0) * 0.017453292519943295;
        n = -1;
      }
    } else if (b_x > 0.0) {
      b_x = (b_x - 180.0) * 0.017453292519943295;
      n = 2;
    } else {
      b_x = (b_x + 180.0) * 0.017453292519943295;
      n = -2;
    }

    switch (n) {
     case 0:
      *x = std::sin(b_x);
      break;

     case 1:
      *x = std::cos(b_x);
      break;

     case -1:
      *x = -std::cos(b_x);
      break;

     default:
      *x = -std::sin(b_x);
      break;
    }
  }
}

// Function for MATLAB Function: '<S1>/motors2ypr'
real_T wrist_decoupler::minimum(const real_T x[3])
{
  real_T ex;
  int32_T idx;
  int32_T k;
  if (!rtIsNaN(x[0])) {
    idx = 1;
  } else {
    boolean_T exitg1;
    idx = 0;
    k = 2;
    exitg1 = false;
    while ((!exitg1) && (k < 4)) {
      if (!rtIsNaN(x[k - 1])) {
        idx = k;
        exitg1 = true;
      } else {
        k++;
      }
    }
  }

  if (idx == 0) {
    ex = x[0];
  } else {
    ex = x[idx - 1];
    for (k = idx + 1; k < 4; k++) {
      real_T x_0;
      x_0 = x[k - 1];
      if (ex > x_0) {
        ex = x_0;
      }
    }
  }

  return ex;
}

// Function for MATLAB Function: '<S1>/motors2ypr'
real_T wrist_decoupler::maximum(const real_T x[3])
{
  real_T ex;
  int32_T idx;
  int32_T k;
  if (!rtIsNaN(x[0])) {
    idx = 1;
  } else {
    boolean_T exitg1;
    idx = 0;
    k = 2;
    exitg1 = false;
    while ((!exitg1) && (k < 4)) {
      if (!rtIsNaN(x[k - 1])) {
        idx = k;
        exitg1 = true;
      } else {
        k++;
      }
    }
  }

  if (idx == 0) {
    ex = x[0];
  } else {
    ex = x[idx - 1];
    for (k = idx + 1; k < 4; k++) {
      real_T x_0;
      x_0 = x[k - 1];
      if (ex < x_0) {
        ex = x_0;
      }
    }
  }

  return ex;
}

real_T rt_atan2d_snf(real_T u0, real_T u1)
{
  real_T y;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = (rtNaN);
  } else if (rtIsInf(u0) && rtIsInf(u1)) {
    int32_T tmp;
    int32_T tmp_0;
    if (u0 > 0.0) {
      tmp = 1;
    } else {
      tmp = -1;
    }

    if (u1 > 0.0) {
      tmp_0 = 1;
    } else {
      tmp_0 = -1;
    }

    y = std::atan2(static_cast<real_T>(tmp), static_cast<real_T>(tmp_0));
  } else if (u1 == 0.0) {
    if (u0 > 0.0) {
      y = RT_PI / 2.0;
    } else if (u0 < 0.0) {
      y = -(RT_PI / 2.0);
    } else {
      y = 0.0;
    }
  } else {
    y = std::atan2(u0, u1);
  }

  return y;
}

// Function for MATLAB Function: '<S1>/motors2ypr'
real_T wrist_decoupler::det(const real_T x[9])
{
  real_T A[9];
  real_T y;
  int8_T ipiv[3];
  boolean_T isodd;
  std::memcpy(&A[0], &x[0], 9U * sizeof(real_T));
  ipiv[0] = 1;
  ipiv[1] = 2;
  for (int32_T j = 0; j < 2; j++) {
    real_T smax;
    int32_T b_ix;
    int32_T iy;
    int32_T jj;
    jj = j << 2;
    iy = 3 - j;
    b_ix = 0;
    smax = std::abs(A[jj]);
    for (int32_T c_k = 2; c_k <= iy; c_k++) {
      real_T s;
      s = std::abs(A[(jj + c_k) - 1]);
      if (s > smax) {
        b_ix = c_k - 1;
        smax = s;
      }
    }

    if (A[jj + b_ix] != 0.0) {
      if (b_ix != 0) {
        iy = j + b_ix;
        ipiv[j] = static_cast<int8_T>(iy + 1);
        smax = A[j];
        A[j] = A[iy];
        A[iy] = smax;
        smax = A[j + 3];
        A[j + 3] = A[iy + 3];
        A[iy + 3] = smax;
        smax = A[j + 6];
        A[j + 6] = A[iy + 6];
        A[iy + 6] = smax;
      }

      iy = (jj - j) + 3;
      for (b_ix = jj + 2; b_ix <= iy; b_ix++) {
        A[b_ix - 1] /= A[jj];
      }
    }

    iy = 1 - j;
    b_ix = jj + 5;
    for (int32_T c_k = 0; c_k <= iy; c_k++) {
      smax = A[(c_k * 3 + jj) + 3];
      if (smax != 0.0) {
        int32_T c;
        c = (b_ix - j) + 1;
        for (int32_T ijA = b_ix; ijA <= c; ijA++) {
          A[ijA - 1] += A[((jj + ijA) - b_ix) + 1] * -smax;
        }
      }

      b_ix += 3;
    }
  }

  isodd = (ipiv[0] > 1);
  y = A[0] * A[4] * A[8];
  if (ipiv[1] > 2) {
    isodd = !isodd;
  }

  if (isodd) {
    y = -y;
  }

  return y;
}

// Function for MATLAB Function: '<S1>/motors2ypr'
void wrist_decoupler::mldivide(const real_T A[9], const real_T B_0[3], real_T Y
  [3])
{
  real_T b_A[9];
  real_T a21;
  real_T maxval;
  int32_T r1;
  int32_T r2;
  int32_T r3;
  std::memcpy(&b_A[0], &A[0], 9U * sizeof(real_T));
  r1 = 0;
  r2 = 1;
  r3 = 2;
  maxval = std::abs(A[0]);
  a21 = std::abs(A[1]);
  if (a21 > maxval) {
    maxval = a21;
    r1 = 1;
    r2 = 0;
  }

  if (std::abs(A[2]) > maxval) {
    r1 = 2;
    r2 = 1;
    r3 = 0;
  }

  b_A[r2] = A[r2] / A[r1];
  b_A[r3] /= b_A[r1];
  b_A[r2 + 3] -= b_A[r1 + 3] * b_A[r2];
  b_A[r3 + 3] -= b_A[r1 + 3] * b_A[r3];
  b_A[r2 + 6] -= b_A[r1 + 6] * b_A[r2];
  b_A[r3 + 6] -= b_A[r1 + 6] * b_A[r3];
  if (std::abs(b_A[r3 + 3]) > std::abs(b_A[r2 + 3])) {
    int32_T rtemp;
    rtemp = r2;
    r2 = r3;
    r3 = rtemp;
  }

  b_A[r3 + 3] /= b_A[r2 + 3];
  b_A[r3 + 6] -= b_A[r3 + 3] * b_A[r2 + 6];
  Y[1] = B_0[r2] - B_0[r1] * b_A[r2];
  Y[2] = (B_0[r3] - B_0[r1] * b_A[r3]) - b_A[r3 + 3] * Y[1];
  Y[2] /= b_A[r3 + 6];
  Y[0] = B_0[r1] - b_A[r1 + 6] * Y[2];
  Y[1] -= b_A[r2 + 6] * Y[2];
  Y[1] /= b_A[r2 + 3];
  Y[0] -= b_A[r1 + 3] * Y[1];
  Y[0] /= b_A[r1];
}

// Model step function
void wrist_decoupler::step()
{
  quaternion Told;
  real_T up[12];
  real_T P_0[9];
  real_T b[9];
  real_T ya_0[9];
  real_T xb[3];
  real_T ya[3];
  real_T yb[3];
  real_T absx;
  real_T b_n;
  real_T b_n_tmp;
  real_T b_yc;
  real_T rtb_Gain_o_idx_0;
  real_T rtb_Gain_o_idx_1;
  real_T rtb_Gain_o_idx_2;
  real_T rtb_Product1_tmp;
  real_T rtb_Product2_tmp;
  real_T st;
  real_T step_0;
  real_T theta;
  real_T up_tmp;
  real_T up_tmp_0;
  real_T up_tmp_1;
  real_T up_tmp_2;
  real_T up_tmp_3;
  real_T up_tmp_4;
  real_T xa_idx_1;
  real_T xa_idx_2;
  real_T yc_idx_2;
  int32_T P_tmp;
  int32_T cycles;
  int32_T exitg1;
  int32_T idx;
  int32_T k;
  int32_T singularity;
  int8_T n;
  boolean_T exitg2;
  boolean_T first_try;
  boolean_T guard1;
  boolean_T guard11;

  // Outputs for Atomic SubSystem: '<Root>/wrist_decoupler'
  // MATLAB Function: '<S1>/motors2ypr' incorporates:
  //   Inport: '<Root>/arm_bend'
  //   Inport: '<Root>/plat_off'
  //   Inport: '<Root>/theta_diff'
  //   Inport: '<Root>/theta_off'

  // :  theta_diff = Wrap180(theta_diff);
  // :  y = x;
  // :  for i=1:3
  // :  while y(i) >= 180.0
  for (rtb_Gain_o_idx_0 = rtU.theta_diff[0]; rtb_Gain_o_idx_0 >= 180.0;
       rtb_Gain_o_idx_0 -= 360.0) {
    // :  y(i) = y(i) - 360.0;
  }

  // :  while y(i) < -180.0
  while (rtb_Gain_o_idx_0 < -180.0) {
    // :  y(i) = y(i) + 360.0;
    rtb_Gain_o_idx_0 += 360.0;
  }

  // :  while y(i) >= 180.0
  for (rtb_Gain_o_idx_1 = rtU.theta_diff[1]; rtb_Gain_o_idx_1 >= 180.0;
       rtb_Gain_o_idx_1 -= 360.0) {
    // :  y(i) = y(i) - 360.0;
  }

  // :  while y(i) < -180.0
  while (rtb_Gain_o_idx_1 < -180.0) {
    // :  y(i) = y(i) + 360.0;
    rtb_Gain_o_idx_1 += 360.0;
  }

  // :  while y(i) >= 180.0
  for (rtb_Gain_o_idx_2 = rtU.theta_diff[2]; rtb_Gain_o_idx_2 >= 180.0;
       rtb_Gain_o_idx_2 -= 360.0) {
    // :  y(i) = y(i) - 360.0;
  }

  // :  while y(i) < -180.0
  while (rtb_Gain_o_idx_2 < -180.0) {
    // :  y(i) = y(i) + 360.0;
    rtb_Gain_o_idx_2 += 360.0;
  }

  // :  if isempty(Q)
  if (!rtDW.Q_not_empty) {
    // :  Pz = -tand(arm_bend)*[1; 1; 1];
    if (rtIsInf(rtU.arm_bend) || rtIsNaN(rtU.arm_bend)) {
      step_0 = (rtNaN);
    } else {
      step_0 = rt_remd_snf(rtU.arm_bend, 360.0);
      absx = std::abs(step_0);
      if (absx > 180.0) {
        if (step_0 > 0.0) {
          step_0 -= 360.0;
        } else {
          step_0 += 360.0;
        }

        absx = std::abs(step_0);
      }

      if (absx <= 45.0) {
        step_0 *= 0.017453292519943295;
        n = 0;
      } else if (absx <= 135.0) {
        if (step_0 > 0.0) {
          step_0 = (step_0 - 90.0) * 0.017453292519943295;
          n = 1;
        } else {
          step_0 = (step_0 + 90.0) * 0.017453292519943295;
          n = -1;
        }
      } else if (step_0 > 0.0) {
        step_0 = (step_0 - 180.0) * 0.017453292519943295;
        n = 2;
      } else {
        step_0 = (step_0 + 180.0) * 0.017453292519943295;
        n = -2;
      }

      step_0 = std::tan(step_0);
      if ((n == 1) || (n == -1)) {
        absx = 1.0 / step_0;
        step_0 = -(1.0 / step_0);
        if (rtIsInf(step_0) && (n == 1)) {
          step_0 = absx;
        }
      }
    }

    // :  P = [cosd(theta_off) sind(theta_off) Pz];
    rtDW.Pz_i[0] = -step_0;
    yb[0] = rtU.theta_off[0];
    rtDW.Pz_i[1] = -step_0;
    yb[1] = rtU.theta_off[1];
    rtDW.Pz_i[2] = -step_0;
    yb[2] = rtU.theta_off[2];
    cosd(yb);
    ya[0] = rtU.theta_off[0];
    ya[1] = rtU.theta_off[1];
    ya[2] = rtU.theta_off[2];
    sind(ya);

    // :  Q = [cosd(plat_off) sind(plat_off) [0; 0; 0]];
    P_0[0] = yb[0];
    P_0[3] = ya[0];
    yb[0] = rtU.plat_off[0];
    P_0[1] = yb[1];
    P_0[4] = ya[1];
    yb[1] = rtU.plat_off[1];
    P_0[2] = yb[2];
    P_0[5] = ya[2];
    yb[2] = rtU.plat_off[2];
    cosd(yb);
    ya[0] = rtU.plat_off[0];
    ya[1] = rtU.plat_off[1];
    ya[2] = rtU.plat_off[2];
    sind(ya);
    rtDW.Q_a[0] = yb[0];
    rtDW.Q_a[3] = ya[0];
    rtDW.Q_a[6] = 0.0;
    rtDW.Q_a[1] = yb[1];
    rtDW.Q_a[4] = ya[1];
    rtDW.Q_a[7] = 0.0;
    rtDW.Q_a[2] = yb[2];
    rtDW.Q_a[5] = ya[2];
    rtDW.Q_a[8] = 0.0;
    rtDW.Q_not_empty = true;

    // :  PQ = [P(1,:)*Q(1,:)' P(2,:)*Q(2,:)' P(3,:)*Q(3,:)'];
    rtDW.PQ_c[0] = (P_0[0] * rtDW.Q_a[0] + P_0[3] * rtDW.Q_a[3]) + rtDW.Pz_i[0] *
      0.0;
    rtDW.PQ_c[1] = (P_0[1] * rtDW.Q_a[1] + P_0[4] * rtDW.Q_a[4]) + rtDW.Pz_i[1] *
      0.0;
    rtDW.PQ_c[2] = (P_0[2] * rtDW.Q_a[2] + P_0[5] * rtDW.Q_a[5]) + rtDW.Pz_i[2] *
      0.0;

    // :  hyaw = 0.1666*(theta_diff(1)+theta_diff(2)+theta_diff(3));
    absx = ((rtb_Gain_o_idx_0 + rtb_Gain_o_idx_1) + rtb_Gain_o_idx_2) * 0.1666;

    // :  T = quaternion(cosd(hyaw),0,0,sind(0.5*hyaw));
    rtDW.T.a = absx;
    cosd_f(&rtDW.T.a);
    rtDW.T.b = 0.0;
    rtDW.T.c = 0.0;
    rtDW.T.d = 0.5 * absx;
    sind_c(&rtDW.T.d);

    // :  attitude_ok = compact(T);
    rtDW.attitude_ok[0] = rtDW.T.a;
    rtDW.attitude_ok[1] = rtDW.T.b;
    rtDW.attitude_ok[2] = rtDW.T.c;
    rtDW.attitude_ok[3] = rtDW.T.d;
  }

  // :  Told = T;
  Told = rtDW.T;

  // :  singularity = 0;
  singularity = 0;

  // :  step = 0.01;
  step_0 = 0.01;

  // :  first_try = true;
  first_try = true;

  // :  for cycles=1:40
  cycles = 1;
  idx = 0;
  guard1 = false;
  do {
    exitg1 = 0;
    if (idx < 40) {
      cycles = idx + 1;

      // :  q = rotatepoint(T,Q);
      b_n_tmp = std::sqrt(((rtDW.T.a * rtDW.T.a + rtDW.T.b * rtDW.T.b) +
                           rtDW.T.c * rtDW.T.c) + rtDW.T.d * rtDW.T.d);
      yc_idx_2 = rtDW.T.a / b_n_tmp;
      rtb_Product1_tmp = rtDW.T.b / b_n_tmp;
      rtb_Product2_tmp = rtDW.T.c / b_n_tmp;
      b_n_tmp = rtDW.T.d / b_n_tmp;
      up[0] = 0.0;
      up[1] = 0.0;
      up[2] = 0.0;
      std::memcpy(&up[3], &rtDW.Q_a[0], 9U * sizeof(real_T));
      yb[0] = ((yc_idx_2 * up[0] - rtb_Product1_tmp * up[3]) - rtb_Product2_tmp *
               up[6]) - b_n_tmp * up[9];
      ya[0] = ((yc_idx_2 * up[3] + rtb_Product1_tmp * up[0]) + rtb_Product2_tmp *
               up[9]) - b_n_tmp * up[6];
      xb[0] = ((yc_idx_2 * up[6] - rtb_Product1_tmp * up[9]) + rtb_Product2_tmp *
               up[0]) + b_n_tmp * up[3];
      absx = ((yc_idx_2 * up[9] + rtb_Product1_tmp * up[6]) - rtb_Product2_tmp *
              up[3]) + b_n_tmp * up[0];
      yb[1] = ((yc_idx_2 * up[1] - rtb_Product1_tmp * up[4]) - rtb_Product2_tmp *
               up[7]) - b_n_tmp * up[10];
      ya[1] = ((yc_idx_2 * up[4] + rtb_Product1_tmp * up[1]) + rtb_Product2_tmp *
               up[10]) - b_n_tmp * up[7];
      xb[1] = ((yc_idx_2 * up[7] - rtb_Product1_tmp * up[10]) + rtb_Product2_tmp
               * up[1]) + b_n_tmp * up[4];
      xa_idx_1 = ((yc_idx_2 * up[10] + rtb_Product1_tmp * up[7]) -
                  rtb_Product2_tmp * up[4]) + b_n_tmp * up[1];
      yb[2] = ((yc_idx_2 * up[2] - rtb_Product1_tmp * up[5]) - rtb_Product2_tmp *
               up[8]) - b_n_tmp * up[11];
      ya[2] = ((yc_idx_2 * up[5] + rtb_Product1_tmp * up[2]) + rtb_Product2_tmp *
               up[11]) - b_n_tmp * up[8];
      xb[2] = ((yc_idx_2 * up[8] - rtb_Product1_tmp * up[11]) + rtb_Product2_tmp
               * up[2]) + b_n_tmp * up[5];
      xa_idx_2 = ((yc_idx_2 * up[11] + rtb_Product1_tmp * up[8]) -
                  rtb_Product2_tmp * up[5]) + b_n_tmp * up[2];
      up_tmp = ((yb[0] * -rtb_Product1_tmp + ya[0] * yc_idx_2) + xb[0] *
                -b_n_tmp) - absx * -rtb_Product2_tmp;
      up_tmp_0 = ((yb[0] * -rtb_Product2_tmp - ya[0] * -b_n_tmp) + xb[0] *
                  yc_idx_2) + absx * -rtb_Product1_tmp;
      up_tmp_1 = ((yb[0] * -b_n_tmp + ya[0] * -rtb_Product2_tmp) - xb[0] *
                  -rtb_Product1_tmp) + absx * yc_idx_2;
      up[0] = ((yb[0] * yc_idx_2 - ya[0] * -rtb_Product1_tmp) - xb[0] *
               -rtb_Product2_tmp) - absx * -b_n_tmp;
      up[3] = up_tmp;
      up[6] = up_tmp_0;
      up[9] = up_tmp_1;
      up_tmp_2 = ((yb[1] * -rtb_Product1_tmp + ya[1] * yc_idx_2) + xb[1] *
                  -b_n_tmp) - xa_idx_1 * -rtb_Product2_tmp;
      up_tmp_3 = ((yb[1] * -rtb_Product2_tmp - ya[1] * -b_n_tmp) + xb[1] *
                  yc_idx_2) + xa_idx_1 * -rtb_Product1_tmp;
      up_tmp_4 = ((yb[1] * -b_n_tmp + ya[1] * -rtb_Product2_tmp) - xb[1] *
                  -rtb_Product1_tmp) + xa_idx_1 * yc_idx_2;
      up[1] = ((yb[1] * yc_idx_2 - ya[1] * -rtb_Product1_tmp) - xb[1] *
               -rtb_Product2_tmp) - xa_idx_1 * -b_n_tmp;
      up[4] = up_tmp_2;
      up[7] = up_tmp_3;
      up[10] = up_tmp_4;
      b_n = ((yb[2] * -rtb_Product1_tmp + ya[2] * yc_idx_2) + xb[2] * -b_n_tmp)
        - xa_idx_2 * -rtb_Product2_tmp;
      b_yc = ((yb[2] * -rtb_Product2_tmp - ya[2] * -b_n_tmp) + xb[2] * yc_idx_2)
        + xa_idx_2 * -rtb_Product1_tmp;
      theta = ((yb[2] * -b_n_tmp + ya[2] * -rtb_Product2_tmp) - xb[2] *
               -rtb_Product1_tmp) + xa_idx_2 * yc_idx_2;
      up[2] = ((yb[2] * yc_idx_2 - ya[2] * -rtb_Product1_tmp) - xb[2] *
               -rtb_Product2_tmp) - xa_idx_2 * -b_n_tmp;
      up[5] = b_n;
      up[8] = b_yc;
      up[11] = theta;
      for (k = 0; k < 3; k++) {
        P_tmp = (k + 1) * 3;
        P_0[3 * k] = up[P_tmp];
        P_0[3 * k + 1] = up[P_tmp + 1];
        P_0[3 * k + 2] = up[P_tmp + 2];
      }

      // :  m = [sqrt(q(1,1)^2+q(1,2)^2) sqrt(q(2,1)^2+q(2,2)^2) sqrt(q(3,1)^2+q(3,2)^2)]; 
      absx = std::sqrt(P_0[0] * P_0[0] + P_0[3] * P_0[3]);
      xa_idx_1 = std::sqrt(P_0[1] * P_0[1] + P_0[4] * P_0[4]);
      xa_idx_2 = std::sqrt(P_0[2] * P_0[2] + P_0[5] * P_0[5]);

      // :  if min(abs(m)) == 0
      xb[0] = absx;
      xb[1] = xa_idx_1;
      xb[2] = xa_idx_2;
      guard11 = false;
      if (minimum(xb) == 0.0) {
        // :  if step > 0.001
        if (step_0 > 0.001) {
          // :  step = 0.5*step;
          step_0 *= 0.5;

          // :  T = Told;
          rtDW.T = Told;
          guard11 = true;
        } else if (first_try) {
          // :  elseif first_try
          // :  first_try = false;
          first_try = false;

          // :  step = 0.01;
          step_0 = 0.01;

          // :  hyaw = 0.1666*(theta_diff(1)+theta_diff(2)+theta_diff(3));
          absx = ((rtb_Gain_o_idx_0 + rtb_Gain_o_idx_1) + rtb_Gain_o_idx_2) *
            0.1666;

          // :  T = quaternion(cosd(hyaw),0,0,sind(0.5*hyaw));
          rtDW.T.a = absx;
          cosd_f(&rtDW.T.a);
          rtDW.T.b = 0.0;
          rtDW.T.c = 0.0;
          rtDW.T.d = 0.5 * absx;
          sind_c(&rtDW.T.d);
          guard11 = true;
        } else {
          guard1 = true;
          exitg1 = 1;
        }
      } else {
        // :  arg = [(PQ(1)-Pz(1)*q(1,3))/m(1); (PQ(2)-Pz(2)*q(2,3))/m(2); (PQ(3)-Pz(3)*q(3,3))/m(3)]; 
        yb[0] = (rtDW.PQ_c[0] - rtDW.Pz_i[0] * up_tmp_1) / absx;
        yb[1] = (rtDW.PQ_c[1] - rtDW.Pz_i[1] * up_tmp_4) / xa_idx_1;
        yb[2] = (rtDW.PQ_c[2] - rtDW.Pz_i[2] * theta) / xa_idx_2;

        // :  if max(abs(arg)) > 1.0
        xb[0] = std::abs(yb[0]);
        xb[1] = std::abs(yb[1]);
        xb[2] = std::abs(yb[2]);
        if (maximum(xb) > 1.0) {
          // :  if step > 0.001
          if (step_0 > 0.001) {
            // :  step = 0.5*step;
            step_0 *= 0.5;

            // :  T = Told;
            rtDW.T = Told;
            guard11 = true;
          } else if (first_try) {
            // :  elseif first_try
            // :  first_try = false;
            first_try = false;

            // :  step = 0.01;
            step_0 = 0.01;

            // :  hyaw = 0.1666*(theta_diff(1)+theta_diff(2)+theta_diff(3));
            absx = ((rtb_Gain_o_idx_0 + rtb_Gain_o_idx_1) + rtb_Gain_o_idx_2) *
              0.1666;

            // :  T = quaternion(cosd(hyaw),0,0,sind(0.5*hyaw));
            rtDW.T.a = absx;
            cosd_f(&rtDW.T.a);
            rtDW.T.b = 0.0;
            rtDW.T.c = 0.0;
            rtDW.T.d = 0.5 * absx;
            sind_c(&rtDW.T.d);
            guard11 = true;
          } else {
            guard1 = true;
            exitg1 = 1;
          }
        } else {
          // :  theta = atan2d(q(:,2),q(:,1))+[90; 90; 90;]-asind(arg);
          // :  theta_err = Wrap180(theta_diff+theta_off-theta);
          // :  y = x;
          // :  for i=1:3
          // :  if max(abs(theta_err)) < 0.1
          st = (57.295779513082323 * rt_atan2d_snf(P_0[3], P_0[0]) + 90.0) -
            57.295779513082323 * std::asin(yb[0]);
          ya[0] = st;

          // :  while y(i) >= 180.0
          for (absx = (rtb_Gain_o_idx_0 + rtU.theta_off[0]) - st; absx >= 180.0;
               absx -= 360.0) {
            // :  y(i) = y(i) - 360.0;
          }

          // :  while y(i) < -180.0
          while (absx < -180.0) {
            // :  y(i) = y(i) + 360.0;
            absx += 360.0;
          }

          xb[0] = std::abs(absx);
          st = (57.295779513082323 * rt_atan2d_snf(P_0[4], P_0[1]) + 90.0) -
            57.295779513082323 * std::asin(yb[1]);
          ya[1] = st;

          // :  while y(i) >= 180.0
          for (xa_idx_1 = (rtb_Gain_o_idx_1 + rtU.theta_off[1]) - st; xa_idx_1 >=
               180.0; xa_idx_1 -= 360.0) {
            // :  y(i) = y(i) - 360.0;
          }

          // :  while y(i) < -180.0
          while (xa_idx_1 < -180.0) {
            // :  y(i) = y(i) + 360.0;
            xa_idx_1 += 360.0;
          }

          xb[1] = std::abs(xa_idx_1);
          st = (57.295779513082323 * rt_atan2d_snf(P_0[5], P_0[2]) + 90.0) -
            57.295779513082323 * std::asin(yb[2]);
          ya[2] = st;

          // :  while y(i) >= 180.0
          for (xa_idx_2 = (rtb_Gain_o_idx_2 + rtU.theta_off[2]) - st; xa_idx_2 >=
               180.0; xa_idx_2 -= 360.0) {
            // :  y(i) = y(i) - 360.0;
          }

          // :  while y(i) < -180.0
          while (xa_idx_2 < -180.0) {
            // :  y(i) = y(i) + 360.0;
            xa_idx_2 += 360.0;
          }

          xb[2] = std::abs(xa_idx_2);
          if (maximum(xb) < 0.1) {
            // :  T = normalize(T);
            rtDW.T.a = yc_idx_2;
            rtDW.T.b = rtb_Product1_tmp;
            rtDW.T.c = rtb_Product2_tmp;
            rtDW.T.d = b_n_tmp;

            // :  attitude = compact(T);
            rtDW.attitude_ok[0] = rtDW.T.a;
            rtDW.attitude_ok[1] = rtDW.T.b;
            rtDW.attitude_ok[2] = rtDW.T.c;
            rtDW.attitude_ok[3] = rtDW.T.d;

            // :  attitude_ok = attitude;
            exitg1 = 1;
          } else {
            // :  p = [cosd(theta) sind(theta) Pz];
            yb[0] = ya[0];
            yb[1] = ya[1];
            yb[2] = st;
            cosd(yb);
            sind(ya);
            P_0[3] = ya[0];
            P_0[1] = yb[1];
            P_0[4] = ya[1];
            P_0[2] = yb[2];
            P_0[5] = ya[2];

            // :  q1xp1 = cross(q(1,:),p(1,:));
            ya[0] = rtDW.Pz_i[0] * up_tmp_0 - P_0[3] * up_tmp_1;
            ya[1] = yb[0] * up_tmp_1 - rtDW.Pz_i[0] * up_tmp;
            ya[2] = up_tmp * P_0[3] - yb[0] * up_tmp_0;

            // :  q2xp2 = cross(q(2,:),p(2,:));
            yb[0] = rtDW.Pz_i[1] * up_tmp_3 - P_0[4] * up_tmp_4;
            yb[1] = P_0[1] * up_tmp_4 - rtDW.Pz_i[1] * up_tmp_2;
            yb[2] = up_tmp_2 * P_0[4] - P_0[1] * up_tmp_3;

            // :  q3xp3 = cross(q(3,:),p(3,:));
            yc_idx_2 = b_n * P_0[5] - P_0[2] * b_yc;

            // :  if min(abs([q1xp1(3) q2xp2(3) q3xp3(3)])) == 0.0
            xb[0] = std::abs(ya[2]);
            xb[1] = std::abs(yb[2]);
            xb[2] = std::abs(yc_idx_2);
            if (minimum(xb) == 0.0) {
              // :  if step > 0.001
              if (step_0 > 0.001) {
                // :  step = 0.5*step;
                step_0 *= 0.5;

                // :  T = Told;
                rtDW.T = Told;
                guard11 = true;
              } else if (first_try) {
                // :  elseif first_try
                // :  first_try = false;
                first_try = false;

                // :  step = 0.01;
                step_0 = 0.01;

                // :  hyaw = 0.1666*(theta_diff(1)+theta_diff(2)+theta_diff(3)); 
                absx = ((rtb_Gain_o_idx_0 + rtb_Gain_o_idx_1) + rtb_Gain_o_idx_2)
                  * 0.1666;

                // :  T = quaternion(cosd(hyaw),0,0,sind(0.5*hyaw));
                rtDW.T.a = absx;
                cosd_f(&rtDW.T.a);
                rtDW.T.b = 0.0;
                rtDW.T.c = 0.0;
                rtDW.T.d = 0.5 * absx;
                sind_c(&rtDW.T.d);
                guard11 = true;
              } else {
                guard1 = true;
                exitg1 = 1;
              }
            } else {
              // :  if det([q1xp1/q1xp1(3); q2xp2/q2xp2(3); q3xp3/q3xp3(3)]) == 0.0 
              st = ya[0] / ya[2];
              ya_0[0] = st;
              b_n_tmp = yb[0] / yb[2];
              ya_0[1] = b_n_tmp;
              b_yc = (rtDW.Pz_i[2] * b_yc - P_0[5] * theta) / yc_idx_2;
              ya_0[2] = b_yc;
              rtb_Product1_tmp = ya[1] / ya[2];
              ya_0[3] = rtb_Product1_tmp;
              rtb_Product2_tmp = yb[1] / yb[2];
              ya_0[4] = rtb_Product2_tmp;
              b_n = (P_0[2] * theta - rtDW.Pz_i[2] * b_n) / yc_idx_2;
              ya_0[5] = b_n;
              theta = ya[2] / ya[2];
              ya_0[6] = theta;
              up_tmp = yb[2] / yb[2];
              ya_0[7] = up_tmp;
              yc_idx_2 /= yc_idx_2;
              ya_0[8] = yc_idx_2;
              if (det(ya_0) == 0.0) {
                // :  if step > 0.001
                if (step_0 > 0.001) {
                  // :  step = 0.5*step;
                  step_0 *= 0.5;

                  // :  T = Told;
                  rtDW.T = Told;
                  guard11 = true;
                } else if (first_try) {
                  // :  elseif first_try
                  // :  first_try = false;
                  first_try = false;

                  // :  step = 0.01;
                  step_0 = 0.01;

                  // :  hyaw = 0.1666*(theta_diff(1)+theta_diff(2)+theta_diff(3)); 
                  absx = ((rtb_Gain_o_idx_0 + rtb_Gain_o_idx_1) +
                          rtb_Gain_o_idx_2) * 0.1666;

                  // :  T = quaternion(cosd(hyaw),0,0,sind(0.5*hyaw));
                  rtDW.T.a = absx;
                  cosd_f(&rtDW.T.a);
                  rtDW.T.b = 0.0;
                  rtDW.T.c = 0.0;
                  rtDW.T.d = 0.5 * absx;
                  sind_c(&rtDW.T.d);
                  guard11 = true;
                } else {
                  guard1 = true;
                  exitg1 = 1;
                }
              } else {
                // :  Told = T;
                Told = rtDW.T;

                // :  T = quaternion(([q1xp1/q1xp1(3); q2xp2/q2xp2(3); q3xp3/q3xp3(3)]\(step*theta_err))','rotvec')*T; 
                ya_0[0] = st;
                ya_0[1] = b_n_tmp;
                ya_0[2] = b_yc;
                ya[0] = step_0 * absx;
                ya_0[3] = rtb_Product1_tmp;
                ya_0[4] = rtb_Product2_tmp;
                ya_0[5] = b_n;
                ya[1] = step_0 * xa_idx_1;
                ya_0[6] = theta;
                ya_0[7] = up_tmp;
                ya_0[8] = yc_idx_2;
                ya[2] = step_0 * xa_idx_2;
                mldivide(ya_0, ya, yb);
                absx = 1.0;
                xa_idx_1 = 0.0;
                xa_idx_2 = 0.0;
                b_n = 0.0;
                ya[0] = yb[0] * yb[0];
                ya[1] = yb[1] * yb[1];
                ya[2] = yb[2] * yb[2];
                theta = std::sqrt((ya[0] + ya[1]) + ya[2]);
                st = std::sin(theta / 2.0);
                if (theta != 0.0) {
                  yb[0] = yb[0] / theta * st;
                  yb[1] = yb[1] / theta * st;
                  yb[2] = yb[2] / theta * st;
                  absx = std::cos(theta / 2.0);
                  xa_idx_1 = yb[0];
                  xa_idx_2 = yb[1];
                  b_n = yb[2];
                }

                theta = rtDW.T.a;
                st = rtDW.T.b;
                b_yc = rtDW.T.c;
                rtDW.T.a = ((absx * rtDW.T.a - xa_idx_1 * rtDW.T.b) - xa_idx_2 *
                            rtDW.T.c) - b_n * rtDW.T.d;
                rtDW.T.b = ((absx * rtDW.T.b + xa_idx_1 * theta) + xa_idx_2 *
                            rtDW.T.d) - b_n * rtDW.T.c;
                rtDW.T.c = ((absx * rtDW.T.c - xa_idx_1 * rtDW.T.d) + xa_idx_2 *
                            theta) + b_n * st;
                rtDW.T.d = ((absx * rtDW.T.d + xa_idx_1 * b_yc) - xa_idx_2 * st)
                  + b_n * theta;
                guard11 = true;
              }
            }
          }
        }
      }

      if (guard11) {
        idx++;
        guard1 = false;
      }
    } else {
      guard1 = true;
      exitg1 = 1;
    }
  } while (exitg1 == 0);

  if (guard1) {
    // :  T = quaternion(1,0,0,0);
    rtDW.T.a = 1.0;
    rtDW.T.b = 0.0;
    rtDW.T.c = 0.0;
    rtDW.T.d = 0.0;

    // :  attitude = attitude_ok;
    // :  singularity = 1;
    singularity = 1;
  }

  // Sqrt: '<S12>/sqrt' incorporates:
  //   MATLAB Function: '<S1>/motors2ypr'
  //   Product: '<S13>/Product'
  //   Product: '<S13>/Product1'
  //   Product: '<S13>/Product2'
  //   Product: '<S13>/Product3'
  //   Sum: '<S13>/Sum'

  step_0 = std::sqrt(((rtDW.attitude_ok[0] * rtDW.attitude_ok[0] +
                       rtDW.attitude_ok[1] * rtDW.attitude_ok[1]) +
                      rtDW.attitude_ok[2] * rtDW.attitude_ok[2]) +
                     rtDW.attitude_ok[3] * rtDW.attitude_ok[3]);

  // Product: '<S7>/Product' incorporates:
  //   MATLAB Function: '<S1>/motors2ypr'

  absx = rtDW.attitude_ok[0] / step_0;

  // Product: '<S7>/Product1' incorporates:
  //   MATLAB Function: '<S1>/motors2ypr'

  xa_idx_1 = rtDW.attitude_ok[1] / step_0;

  // Product: '<S7>/Product2' incorporates:
  //   MATLAB Function: '<S1>/motors2ypr'

  xa_idx_2 = rtDW.attitude_ok[2] / step_0;

  // Product: '<S7>/Product3' incorporates:
  //   MATLAB Function: '<S1>/motors2ypr'

  step_0 = rtDW.attitude_ok[3] / step_0;

  // Fcn: '<S2>/fcn3'
  b_n = (xa_idx_1 * step_0 - absx * xa_idx_2) * -2.0;

  // Fcn: '<S2>/fcn2' incorporates:
  //   Fcn: '<S2>/fcn5'

  rtb_Gain_o_idx_2 = absx * absx;
  rtb_Gain_o_idx_1 = xa_idx_1 * xa_idx_1;
  theta = xa_idx_2 * xa_idx_2;
  st = step_0 * step_0;

  // Gain: '<S3>/Gain' incorporates:
  //   Fcn: '<S2>/fcn1'
  //   Fcn: '<S2>/fcn2'
  //   Trigonometry: '<S6>/Trigonometric Function1'

  rtb_Gain_o_idx_0 = rt_atan2d_snf((xa_idx_1 * xa_idx_2 + absx * step_0) * 2.0,
    ((rtb_Gain_o_idx_2 + rtb_Gain_o_idx_1) - theta) - st) * 57.295779513082323;

  // If: '<S8>/If' incorporates:
  //   Constant: '<S10>/Constant'
  //   Constant: '<S9>/Constant'

  if (b_n > 1.0) {
    // Outputs for IfAction SubSystem: '<S8>/If Action Subsystem' incorporates:
    //   ActionPort: '<S9>/Action Port'

    b_n = 1.0;

    // End of Outputs for SubSystem: '<S8>/If Action Subsystem'
  } else if (b_n < -1.0) {
    // Outputs for IfAction SubSystem: '<S8>/If Action Subsystem1' incorporates:
    //   ActionPort: '<S10>/Action Port'

    b_n = 1.0;

    // End of Outputs for SubSystem: '<S8>/If Action Subsystem1'
  }

  // Gain: '<S3>/Gain' incorporates:
  //   Fcn: '<S2>/fcn4'
  //   Fcn: '<S2>/fcn5'
  //   Trigonometry: '<S6>/Trigonometric Function3'

  rtb_Gain_o_idx_2 = rt_atan2d_snf((xa_idx_2 * step_0 + absx * xa_idx_1) * 2.0,
    ((rtb_Gain_o_idx_2 - rtb_Gain_o_idx_1) - theta) + st) * 57.295779513082323;

  // MATLAB Function: '<S1>/ypr2motors' incorporates:
  //   Inport: '<Root>/arm_bend'
  //   Inport: '<Root>/plat_off'
  //   Inport: '<Root>/theta_off'

  // :  if isempty(Pz)
  if (!rtDW.Pz_not_empty) {
    // :  Pz = -tand(arm_bend)*[1; 1; 1];
    if (rtIsInf(rtU.arm_bend) || rtIsNaN(rtU.arm_bend)) {
      step_0 = (rtNaN);
    } else {
      step_0 = rt_remd_snf(rtU.arm_bend, 360.0);
      absx = std::abs(step_0);
      if (absx > 180.0) {
        if (step_0 > 0.0) {
          step_0 -= 360.0;
        } else {
          step_0 += 360.0;
        }

        absx = std::abs(step_0);
      }

      if (absx <= 45.0) {
        step_0 *= 0.017453292519943295;
        n = 0;
      } else if (absx <= 135.0) {
        if (step_0 > 0.0) {
          step_0 = (step_0 - 90.0) * 0.017453292519943295;
          n = 1;
        } else {
          step_0 = (step_0 + 90.0) * 0.017453292519943295;
          n = -1;
        }
      } else if (step_0 > 0.0) {
        step_0 = (step_0 - 180.0) * 0.017453292519943295;
        n = 2;
      } else {
        step_0 = (step_0 + 180.0) * 0.017453292519943295;
        n = -2;
      }

      step_0 = std::tan(step_0);
      if ((n == 1) || (n == -1)) {
        absx = 1.0 / step_0;
        step_0 = -(1.0 / step_0);
        if (rtIsInf(step_0) && (n == 1)) {
          step_0 = absx;
        }
      }
    }

    rtDW.Pz_not_empty = true;

    // :  P = [cosd(theta_off) sind(theta_off) Pz];
    rtDW.Pz[0] = -step_0;
    yb[0] = rtU.theta_off[0];
    rtDW.Pz[1] = -step_0;
    yb[1] = rtU.theta_off[1];
    rtDW.Pz[2] = -step_0;
    yb[2] = rtU.theta_off[2];
    cosd(yb);
    ya[0] = rtU.theta_off[0];
    ya[1] = rtU.theta_off[1];
    ya[2] = rtU.theta_off[2];
    sind(ya);
    P_0[0] = yb[0];
    P_0[3] = ya[0];
    P_0[1] = yb[1];
    P_0[4] = ya[1];
    P_0[2] = yb[2];
    P_0[5] = ya[2];

    // :  Q = [cosd(plat_off) sind(plat_off) [0; 0; 0]];
    yb[0] = rtU.plat_off[0];
    yb[1] = rtU.plat_off[1];
    yb[2] = rtU.plat_off[2];
    cosd(yb);
    ya[0] = rtU.plat_off[0];
    ya[1] = rtU.plat_off[1];
    ya[2] = rtU.plat_off[2];
    sind(ya);
    rtDW.Q[0] = yb[0];
    rtDW.Q[3] = ya[0];
    rtDW.Q[6] = 0.0;
    rtDW.Q[1] = yb[1];
    rtDW.Q[4] = ya[1];
    rtDW.Q[7] = 0.0;
    rtDW.Q[2] = yb[2];
    rtDW.Q[5] = ya[2];
    rtDW.Q[8] = 0.0;

    // :  PQ = [P(1,:)*Q(1,:)' P(2,:)*Q(2,:)' P(3,:)*Q(3,:)'];
    rtDW.PQ[0] = (P_0[0] * rtDW.Q[0] + P_0[3] * rtDW.Q[3]) + rtDW.Pz[0] * 0.0;
    rtDW.PQ[1] = (P_0[1] * rtDW.Q[1] + P_0[4] * rtDW.Q[4]) + rtDW.Pz[1] * 0.0;
    rtDW.PQ[2] = (P_0[2] * rtDW.Q[2] + P_0[5] * rtDW.Q[5]) + rtDW.Pz[2] * 0.0;

    // :  last_theta_ok = [0;0;0];
  }

  // Switch: '<S1>/Switch' incorporates:
  //   Gain: '<S1>/Gain'
  //   Inport: '<Root>/RL'
  //   Inport: '<Root>/ypr_star'

  // :  q = Q*eul2rotm(deg2rad(ypr'))';
  if (rtU.RL) {
    absx = -rtU.ypr_star[0];
  } else {
    absx = rtU.ypr_star[0];
  }

  // MATLAB Function: '<S1>/ypr2motors' incorporates:
  //   Gain: '<S1>/Gain'
  //   Inport: '<Root>/ypr_star'
  //   Switch: '<S1>/Switch'

  st = 0.017453292519943295 * absx;
  step_0 = std::cos(st);
  rtb_Gain_o_idx_1 = std::sin(st);
  st = 0.017453292519943295 * rtU.ypr_star[1];
  xa_idx_1 = std::cos(st);
  xa_idx_2 = std::sin(st);

  // Switch: '<S1>/Switch' incorporates:
  //   Gain: '<S1>/Gain'
  //   Inport: '<Root>/RL'
  //   Inport: '<Root>/ypr_star'

  if (rtU.RL) {
    absx = -rtU.ypr_star[2];
  } else {
    absx = rtU.ypr_star[2];
  }

  // MATLAB Function: '<S1>/ypr2motors' incorporates:
  //   Inport: '<Root>/theta_off'
  //   Switch: '<S1>/Switch'

  st = 0.017453292519943295 * absx;
  absx = std::cos(st);
  st = std::sin(st);
  b[0] = step_0 * xa_idx_1;
  theta = xa_idx_2 * st;
  b[3] = theta * step_0 - rtb_Gain_o_idx_1 * absx;
  b_yc = xa_idx_2 * absx;
  b[6] = b_yc * step_0 + rtb_Gain_o_idx_1 * st;
  b[1] = rtb_Gain_o_idx_1 * xa_idx_1;
  b[4] = theta * rtb_Gain_o_idx_1 + step_0 * absx;
  b[7] = b_yc * rtb_Gain_o_idx_1 - step_0 * st;
  b[2] = -xa_idx_2;
  b[5] = xa_idx_1 * st;
  b[8] = xa_idx_1 * absx;
  for (k = 0; k < 3; k++) {
    step_0 = rtDW.Q[k + 3];
    rtb_Gain_o_idx_1 = rtDW.Q[k];
    absx = rtDW.Q[k + 6];
    for (idx = 0; idx < 3; idx++) {
      P_0[k + 3 * idx] = (b[idx + 3] * step_0 + rtb_Gain_o_idx_1 * b[idx]) +
        b[idx + 6] * absx;
    }
  }

  // :  m = [sqrt(q(1,1)^2+q(1,2)^2) sqrt(q(2,1)^2+q(2,2)^2) sqrt(q(3,1)^2+q(3,2)^2)]; 
  absx = std::sqrt(P_0[0] * P_0[0] + P_0[3] * P_0[3]);
  xa_idx_1 = std::sqrt(P_0[1] * P_0[1] + P_0[4] * P_0[4]);
  xa_idx_2 = std::sqrt(P_0[2] * P_0[2] + P_0[5] * P_0[5]);

  // :  if min(abs(m)) == 0
  ya[0] = absx;
  ya[1] = xa_idx_1;
  ya[2] = xa_idx_2;
  if (minimum(ya) == 0.0) {
    // :  theta = last_theta_ok;
    absx = rtDW.last_theta_ok[0];
    xa_idx_1 = rtDW.last_theta_ok[1];
    xa_idx_2 = rtDW.last_theta_ok[2];

    // Outport: '<Root>/out_of_range'
    // :  out_of_range = 1;
    rtY.out_of_range = 1.0;
  } else {
    // :  arg = [(PQ(1)-Pz(1)*q(1,3))/m(1); (PQ(2)-Pz(2)*q(2,3))/m(2); (PQ(3)-Pz(3)*q(3,3))/m(3)]; 
    xb[0] = (rtDW.PQ[0] - rtDW.Pz[0] * P_0[6]) / absx;
    xb[1] = (rtDW.PQ[1] - rtDW.Pz[1] * P_0[7]) / xa_idx_1;
    xb[2] = (rtDW.PQ[2] - rtDW.Pz[2] * P_0[8]) / xa_idx_2;

    // :  if max(abs(arg)) > 0.99
    ya[0] = std::abs(xb[0]);
    ya[1] = std::abs(xb[1]);
    ya[2] = std::abs(xb[2]);
    if (!rtIsNaN(ya[0])) {
      idx = 1;
    } else {
      idx = 0;
      k = 2;
      exitg2 = false;
      while ((!exitg2) && (k < 4)) {
        if (!rtIsNaN(ya[k - 1])) {
          idx = k;
          exitg2 = true;
        } else {
          k++;
        }
      }
    }

    if (idx == 0) {
      step_0 = ya[0];
    } else {
      step_0 = ya[idx - 1];
      for (k = idx + 1; k < 4; k++) {
        st = ya[k - 1];
        if (step_0 < st) {
          step_0 = st;
        }
      }
    }

    if (step_0 > 0.99) {
      // :  theta = last_theta_ok;
      absx = rtDW.last_theta_ok[0];
      xa_idx_1 = rtDW.last_theta_ok[1];
      xa_idx_2 = rtDW.last_theta_ok[2];

      // Outport: '<Root>/out_of_range'
      // :  out_of_range = 2;
      rtY.out_of_range = 2.0;
    } else {
      // :  theta = atan2d(q(:,2),q(:,1))+[90; 90; 90;]-asind(arg);
      absx = (57.295779513082323 * rt_atan2d_snf(P_0[3], P_0[0]) + 90.0) -
        57.295779513082323 * std::asin(xb[0]);
      rtb_Gain_o_idx_1 = (57.295779513082323 * rt_atan2d_snf(P_0[4], P_0[1]) +
                          90.0) - 57.295779513082323 * std::asin(xb[1]);
      step_0 = (57.295779513082323 * rt_atan2d_snf(P_0[5], P_0[2]) + 90.0) -
        57.295779513082323 * std::asin(xb[2]);

      // :  gap = abs(Wrap180([(theta(2)-theta(1)) (theta(3)-theta(2)) (theta(1)-theta(3))])); 
      yb[0] = rtb_Gain_o_idx_1 - absx;
      yb[1] = step_0 - rtb_Gain_o_idx_1;
      yb[2] = absx - step_0;

      // :  y = x;
      // :  for i=1:3
      // :  while y(i) > 180.0
      while (yb[0] > 180.0) {
        // :  y(i) = y(i) - 360.0;
        yb[0] -= 360.0;
      }

      // :  while y(i) < -180.0
      while (yb[0] < -180.0) {
        // :  y(i) = y(i) + 360.0;
        yb[0] += 360.0;
      }

      ya[0] = std::abs(yb[0]);

      // :  while y(i) > 180.0
      while (yb[1] > 180.0) {
        // :  y(i) = y(i) - 360.0;
        yb[1] -= 360.0;
      }

      // :  while y(i) < -180.0
      while (yb[1] < -180.0) {
        // :  y(i) = y(i) + 360.0;
        yb[1] += 360.0;
      }

      ya[1] = std::abs(yb[1]);

      // :  while y(i) > 180.0
      while (yb[2] > 180.0) {
        // :  y(i) = y(i) - 360.0;
        yb[2] -= 360.0;
      }

      // :  while y(i) < -180.0
      while (yb[2] < -180.0) {
        // :  y(i) = y(i) + 360.0;
        yb[2] += 360.0;
      }

      ya[2] = std::abs(yb[2]);

      // :  if min(gap) < 33.0
      if (minimum(ya) < 33.0) {
        // :  theta = last_theta_ok;
        absx = rtDW.last_theta_ok[0];
        xa_idx_1 = rtDW.last_theta_ok[1];
        xa_idx_2 = rtDW.last_theta_ok[2];

        // Outport: '<Root>/out_of_range'
        // :  out_of_range = 3;
        rtY.out_of_range = 3.0;
      } else {
        // :  theta = Wrap180(theta-theta_off);
        // :  y = x;
        // :  for i=1:3
        // :  last_theta_ok = theta;
        // :  while y(i) > 180.0
        for (absx -= rtU.theta_off[0]; absx > 180.0; absx -= 360.0) {
          // :  y(i) = y(i) - 360.0;
        }

        // :  while y(i) < -180.0
        while (absx < -180.0) {
          // :  y(i) = y(i) + 360.0;
          absx += 360.0;
        }

        rtDW.last_theta_ok[0] = absx;

        // :  while y(i) > 180.0
        for (xa_idx_1 = rtb_Gain_o_idx_1 - rtU.theta_off[1]; xa_idx_1 > 180.0;
             xa_idx_1 -= 360.0) {
          // :  y(i) = y(i) - 360.0;
        }

        // :  while y(i) < -180.0
        while (xa_idx_1 < -180.0) {
          // :  y(i) = y(i) + 360.0;
          xa_idx_1 += 360.0;
        }

        rtDW.last_theta_ok[1] = xa_idx_1;

        // :  while y(i) > 180.0
        for (xa_idx_2 = step_0 - rtU.theta_off[2]; xa_idx_2 > 180.0; xa_idx_2 -=
             360.0) {
          // :  y(i) = y(i) - 360.0;
        }

        // :  while y(i) < -180.0
        while (xa_idx_2 < -180.0) {
          // :  y(i) = y(i) + 360.0;
          xa_idx_2 += 360.0;
        }

        rtDW.last_theta_ok[2] = xa_idx_2;

        // Outport: '<Root>/out_of_range' incorporates:
        //   Inport: '<Root>/theta_off'

        // :  out_of_range = 0;
        rtY.out_of_range = 0.0;
      }
    }
  }

  // End of Outputs for SubSystem: '<Root>/wrist_decoupler'

  // Outport: '<Root>/theta_star'
  rtY.theta_star[0] = absx;

  // Outputs for Atomic SubSystem: '<Root>/wrist_decoupler'
  // Switch: '<S1>/Switch1' incorporates:
  //   Inport: '<Root>/RL'

  if (rtU.RL) {
    // Outport: '<Root>/ypr_meas' incorporates:
    //   Gain: '<S1>/Gain1'

    rtY.ypr_meas[0] = -rtb_Gain_o_idx_0;
  } else {
    // Outport: '<Root>/ypr_meas'
    rtY.ypr_meas[0] = rtb_Gain_o_idx_0;
  }

  // End of Outputs for SubSystem: '<Root>/wrist_decoupler'

  // Outport: '<Root>/theta_star'
  rtY.theta_star[1] = xa_idx_1;

  // Outputs for Atomic SubSystem: '<Root>/wrist_decoupler'
  // If: '<S8>/If' incorporates:
  //   Gain: '<S3>/Gain'
  //   Outport: '<Root>/ypr_meas'
  //   Switch: '<S1>/Switch1'
  //   Trigonometry: '<S6>/trigFcn'

  rtY.ypr_meas[1] = 57.295779513082323 * std::asin(b_n);

  // End of Outputs for SubSystem: '<Root>/wrist_decoupler'

  // Outport: '<Root>/theta_star'
  rtY.theta_star[2] = xa_idx_2;

  // Outputs for Atomic SubSystem: '<Root>/wrist_decoupler'
  // Switch: '<S1>/Switch1' incorporates:
  //   Inport: '<Root>/RL'

  if (rtU.RL) {
    // Outport: '<Root>/ypr_meas' incorporates:
    //   Gain: '<S1>/Gain1'

    rtY.ypr_meas[2] = -rtb_Gain_o_idx_2;
  } else {
    // Outport: '<Root>/ypr_meas'
    rtY.ypr_meas[2] = rtb_Gain_o_idx_2;
  }

  // Outport: '<Root>/singularity' incorporates:
  //   MATLAB Function: '<S1>/motors2ypr'

  rtY.singularity = singularity;

  // Outport: '<Root>/cycles' incorporates:
  //   MATLAB Function: '<S1>/motors2ypr'

  rtY.cycles = cycles;

  // End of Outputs for SubSystem: '<Root>/wrist_decoupler'
}

// Model initialize function
void wrist_decoupler::initialize()
{
  // Registration code

  // initialize non-finites
  rt_InitInfAndNaN(sizeof(real_T));
}

// Constructor
wrist_decoupler::wrist_decoupler() :
  rtU(),
  rtY(),
  rtDW(),
  rtM()
{
  // Currently there is no constructor body generated.
}

// Destructor
wrist_decoupler::~wrist_decoupler()
{
  // Currently there is no destructor body generated.
}

// Real-Time Model get method
wrist_decoupler::RT_MODEL * wrist_decoupler::getRTM()
{
  return (&rtM);
}

//
// File trailer for generated code.
//
// [EOF]
//
