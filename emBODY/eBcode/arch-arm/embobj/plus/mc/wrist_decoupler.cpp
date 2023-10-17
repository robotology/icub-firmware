//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: wrist_decoupler.cpp
//
// Code generated for Simulink model 'wrist_decoupler'.
//
// Model version                  : 7.13
// Simulink Coder version         : 9.9 (R2023a) 19-Nov-2022
// C/C++ source code generated on : Tue Jul 25 11:18:40 2023
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
#include <cfloat>
#include <stddef.h>
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

// Model step function
void wrist_decoupler::step()
{
  real_T up[12];
  real_T P_0[9];
  real_T b[9];
  real_T ya_0[9];
  real_T xa[3];
  real_T xb[3];
  real_T ya[3];
  real_T yb[3];
  real_T absx;
  real_T b_n;
  real_T maxval;
  real_T rtb_Gain_o_idx_0;
  real_T rtb_Gain_o_idx_1;
  real_T rtb_Gain_o_idx_2;
  real_T rtb_Product1;
  real_T rtb_attitude_idx_0;
  real_T st;
  real_T theta;
  real_T tmp;
  real_T tmp_0;
  real_T up_tmp;
  real_T up_tmp_0;
  real_T up_tmp_1;
  real_T up_tmp_2;
  real_T up_tmp_3;
  real_T up_tmp_4;
  real_T yc_idx_2;
  int32_T cycles;
  int32_T exitg1;
  int32_T idx;
  int32_T r1;
  int32_T r2;
  int32_T r3;
  int32_T rtemp;
  int8_T n;
  boolean_T exitg2;
  boolean_T guard1 = false;

  // Outputs for Atomic SubSystem: '<Root>/wrist_decoupler'
  // MATLAB Function: '<S1>/motors2ypr' incorporates:
  //   Inport: '<Root>/arm_bend'
  //   Inport: '<Root>/plat_off'
  //   Inport: '<Root>/theta_diff'
  //   Inport: '<Root>/theta_off'

  rtb_Gain_o_idx_0 = rtU.theta_diff[0];
  rtb_Gain_o_idx_1 = rtU.theta_diff[1];
  rtb_Gain_o_idx_2 = rtU.theta_diff[2];

  // :  if isempty(singularity_reg)
  if (!rtDW.singularity_reg_not_empty) {
    // :  Pz = -tand(arm_bend)*[1; 1; 1];
    if (rtIsInf(rtU.arm_bend) || rtIsNaN(rtU.arm_bend)) {
      maxval = (rtNaN);
    } else {
      maxval = rt_remd_snf(rtU.arm_bend, 360.0);
      absx = std::abs(maxval);
      if (absx > 180.0) {
        if (maxval > 0.0) {
          maxval -= 360.0;
        } else {
          maxval += 360.0;
        }

        absx = std::abs(maxval);
      }

      if (absx <= 45.0) {
        maxval *= 0.017453292519943295;
        n = 0;
      } else if (absx <= 135.0) {
        if (maxval > 0.0) {
          maxval = (maxval - 90.0) * 0.017453292519943295;
          n = 1;
        } else {
          maxval = (maxval + 90.0) * 0.017453292519943295;
          n = -1;
        }
      } else if (maxval > 0.0) {
        maxval = (maxval - 180.0) * 0.017453292519943295;
        n = 2;
      } else {
        maxval = (maxval + 180.0) * 0.017453292519943295;
        n = -2;
      }

      maxval = std::tan(maxval);
      if ((n == 1) || (n == -1)) {
        absx = 1.0 / maxval;
        maxval = -(1.0 / maxval);
        if (rtIsInf(maxval) && (n == 1)) {
          maxval = absx;
        }
      }
    }

    // :  P = [cosd(theta_off) sind(theta_off) Pz];
    rtDW.Pz_b[0] = -maxval;
    yb[0] = rtU.theta_off[0];
    rtDW.Pz_b[1] = -maxval;
    yb[1] = rtU.theta_off[1];
    rtDW.Pz_b[2] = -maxval;
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
    rtDW.Q_p[0] = yb[0];
    rtDW.Q_p[3] = ya[0];
    rtDW.Q_p[6] = 0.0;
    rtDW.Q_p[1] = yb[1];
    rtDW.Q_p[4] = ya[1];
    rtDW.Q_p[7] = 0.0;
    rtDW.Q_p[2] = yb[2];
    rtDW.Q_p[5] = ya[2];
    rtDW.Q_p[8] = 0.0;

    // :  PQ = [P(1,:)*Q(1,:)' P(2,:)*Q(2,:)' P(3,:)*Q(3,:)'];
    rtDW.PQ_f[0] = (P_0[0] * rtDW.Q_p[0] + P_0[3] * rtDW.Q_p[3]) + rtDW.Pz_b[0] *
      0.0;
    rtDW.PQ_f[1] = (P_0[1] * rtDW.Q_p[1] + P_0[4] * rtDW.Q_p[4]) + rtDW.Pz_b[1] *
      0.0;
    rtDW.PQ_f[2] = (P_0[2] * rtDW.Q_p[2] + P_0[5] * rtDW.Q_p[5]) + rtDW.Pz_b[2] *
      0.0;

    // :  T = quaternion(1,0,0,0);
    rtDW.T.a = 1.0;
    rtDW.T.b = 0.0;
    rtDW.T.c = 0.0;
    rtDW.T.d = 0.0;

    // :  singularity_reg = 0;
    rtDW.singularity_reg_not_empty = true;
  }

  // :  theta_diff = Wrap180(theta_diff);
  // :  y = x;
  // :  for i=1:3
  // :  if max(abs(theta_diff)) < 5.0
  // :  while y(i) > 180.0
  while (rtb_Gain_o_idx_0 > 180.0) {
    // :  y(i) = y(i) - 360.0;
    rtb_Gain_o_idx_0 -= 360.0;
  }

  // :  while y(i) < -180.0
  while (rtb_Gain_o_idx_0 < -180.0) {
    // :  y(i) = y(i) + 360.0;
    rtb_Gain_o_idx_0 += 360.0;
  }

  xa[0] = std::abs(rtb_Gain_o_idx_0);

  // :  while y(i) > 180.0
  while (rtb_Gain_o_idx_1 > 180.0) {
    // :  y(i) = y(i) - 360.0;
    rtb_Gain_o_idx_1 -= 360.0;
  }

  // :  while y(i) < -180.0
  while (rtb_Gain_o_idx_1 < -180.0) {
    // :  y(i) = y(i) + 360.0;
    rtb_Gain_o_idx_1 += 360.0;
  }

  xa[1] = std::abs(rtb_Gain_o_idx_1);

  // :  while y(i) > 180.0
  while (rtb_Gain_o_idx_2 > 180.0) {
    // :  y(i) = y(i) - 360.0;
    rtb_Gain_o_idx_2 -= 360.0;
  }

  // :  while y(i) < -180.0
  while (rtb_Gain_o_idx_2 < -180.0) {
    // :  y(i) = y(i) + 360.0;
    rtb_Gain_o_idx_2 += 360.0;
  }

  xa[2] = std::abs(rtb_Gain_o_idx_2);
  if (maximum(xa) < 5.0) {
    // :  singularity_reg = 0;
    rtDW.singularity_reg = 0.0;
  }

  // :  if singularity_reg
  if (rtDW.singularity_reg != 0.0) {
    // :  attitude = compact(T);
    rtb_attitude_idx_0 = rtDW.T.a;
    yc_idx_2 = rtDW.T.b;
    b_n = rtDW.T.c;
    st = rtDW.T.d;

    // Outport: '<Root>/singularity'
    // :  singularity = singularity_reg;
    rtY.singularity = rtDW.singularity_reg;

    // :  cycles = 0;
    cycles = 0;
  } else {
    // :  for cycles=1:200
    cycles = 1;
    idx = 0;
    guard1 = false;
    do {
      exitg1 = 0;
      if (idx < 200) {
        cycles = idx + 1;

        // :  q = rotatepoint(T,Q);
        theta = std::sqrt(((rtDW.T.a * rtDW.T.a + rtDW.T.b * rtDW.T.b) +
                           rtDW.T.c * rtDW.T.c) + rtDW.T.d * rtDW.T.d);
        rtb_attitude_idx_0 = rtDW.T.a / theta;
        yc_idx_2 = rtDW.T.b / theta;
        b_n = rtDW.T.c / theta;
        st = rtDW.T.d / theta;
        up[0] = 0.0;
        up[1] = 0.0;
        up[2] = 0.0;
        std::memcpy(&up[3], &rtDW.Q_p[0], 9U * sizeof(real_T));
        yb[0] = ((rtb_attitude_idx_0 * up[0] - yc_idx_2 * up[3]) - b_n * up[6])
          - st * up[9];
        ya[0] = ((rtb_attitude_idx_0 * up[3] + yc_idx_2 * up[0]) + b_n * up[9])
          - st * up[6];
        xa[0] = ((rtb_attitude_idx_0 * up[6] - yc_idx_2 * up[9]) + b_n * up[0])
          + st * up[3];
        xb[0] = ((rtb_attitude_idx_0 * up[9] + yc_idx_2 * up[6]) - b_n * up[3])
          + st * up[0];
        yb[1] = ((rtb_attitude_idx_0 * up[1] - yc_idx_2 * up[4]) - b_n * up[7])
          - st * up[10];
        ya[1] = ((rtb_attitude_idx_0 * up[4] + yc_idx_2 * up[1]) + b_n * up[10])
          - st * up[7];
        xa[1] = ((rtb_attitude_idx_0 * up[7] - yc_idx_2 * up[10]) + b_n * up[1])
          + st * up[4];
        xb[1] = ((rtb_attitude_idx_0 * up[10] + yc_idx_2 * up[7]) - b_n * up[4])
          + st * up[1];
        yb[2] = ((rtb_attitude_idx_0 * up[2] - yc_idx_2 * up[5]) - b_n * up[8])
          - st * up[11];
        ya[2] = ((rtb_attitude_idx_0 * up[5] + yc_idx_2 * up[2]) + b_n * up[11])
          - st * up[8];
        xa[2] = ((rtb_attitude_idx_0 * up[8] - yc_idx_2 * up[11]) + b_n * up[2])
          + st * up[5];
        xb[2] = ((rtb_attitude_idx_0 * up[11] + yc_idx_2 * up[8]) - b_n * up[5])
          + st * up[2];
        up_tmp = ((yb[0] * -yc_idx_2 + ya[0] * rtb_attitude_idx_0) + xa[0] * -st)
          - xb[0] * -b_n;
        up_tmp_0 = ((yb[0] * -b_n - ya[0] * -st) + xa[0] * rtb_attitude_idx_0) +
          xb[0] * -yc_idx_2;
        up_tmp_1 = ((yb[0] * -st + ya[0] * -b_n) - xa[0] * -yc_idx_2) + xb[0] *
          rtb_attitude_idx_0;
        up[0] = ((yb[0] * rtb_attitude_idx_0 - ya[0] * -yc_idx_2) - xa[0] * -b_n)
          - xb[0] * -st;
        up[3] = up_tmp;
        up[6] = up_tmp_0;
        up[9] = up_tmp_1;
        up_tmp_2 = ((yb[1] * -yc_idx_2 + ya[1] * rtb_attitude_idx_0) + xa[1] *
                    -st) - xb[1] * -b_n;
        up_tmp_3 = ((yb[1] * -b_n - ya[1] * -st) + xa[1] * rtb_attitude_idx_0) +
          xb[1] * -yc_idx_2;
        up_tmp_4 = ((yb[1] * -st + ya[1] * -b_n) - xa[1] * -yc_idx_2) + xb[1] *
          rtb_attitude_idx_0;
        up[1] = ((yb[1] * rtb_attitude_idx_0 - ya[1] * -yc_idx_2) - xa[1] * -b_n)
          - xb[1] * -st;
        up[4] = up_tmp_2;
        up[7] = up_tmp_3;
        up[10] = up_tmp_4;
        theta = ((yb[2] * -yc_idx_2 + ya[2] * rtb_attitude_idx_0) + xa[2] * -st)
          - xb[2] * -b_n;
        rtb_Product1 = ((yb[2] * -b_n - ya[2] * -st) + xa[2] *
                        rtb_attitude_idx_0) + xb[2] * -yc_idx_2;
        maxval = ((yb[2] * -st + ya[2] * -b_n) - xa[2] * -yc_idx_2) + xb[2] *
          rtb_attitude_idx_0;
        up[2] = ((yb[2] * rtb_attitude_idx_0 - ya[2] * -yc_idx_2) - xa[2] * -b_n)
          - xb[2] * -st;
        up[5] = theta;
        up[8] = rtb_Product1;
        up[11] = maxval;
        for (r1 = 0; r1 < 3; r1++) {
          r2 = (r1 + 1) * 3;
          P_0[3 * r1] = up[r2];
          P_0[3 * r1 + 1] = up[r2 + 1];
          P_0[3 * r1 + 2] = up[r2 + 2];
        }

        // :  m = [sqrt(q(1,1)^2+q(1,2)^2) sqrt(q(2,1)^2+q(2,2)^2) sqrt(q(3,1)^2+q(3,2)^2)]; 
        absx = std::sqrt(P_0[0] * P_0[0] + P_0[3] * P_0[3]);
        tmp = std::sqrt(P_0[1] * P_0[1] + P_0[4] * P_0[4]);
        tmp_0 = std::sqrt(P_0[2] * P_0[2] + P_0[5] * P_0[5]);

        // :  if min(abs(m)) == 0
        xa[0] = absx;
        xa[1] = tmp;
        xa[2] = tmp_0;
        if (minimum(xa) == 0.0) {
          // :  cycles = -1;
          cycles = -1;
          guard1 = true;
          exitg1 = 1;
        } else {
          // :  arg = [(PQ(1)-Pz(1)*q(1,3))/m(1); (PQ(2)-Pz(2)*q(2,3))/m(2); (PQ(3)-Pz(3)*q(3,3))/m(3)]; 
          yb[0] = (rtDW.PQ_f[0] - rtDW.Pz_b[0] * up_tmp_1) / absx;
          yb[1] = (rtDW.PQ_f[1] - rtDW.Pz_b[1] * up_tmp_4) / tmp;
          yb[2] = (rtDW.PQ_f[2] - rtDW.Pz_b[2] * maxval) / tmp_0;

          // :  if max(abs(arg)) > 1.0
          xa[0] = std::abs(yb[0]);
          xa[1] = std::abs(yb[1]);
          xa[2] = std::abs(yb[2]);
          if (maximum(xa) > 1.0) {
            // :  cycles = -2;
            cycles = -2;
            guard1 = true;
            exitg1 = 1;
          } else {
            // :  theta = atan2d(q(:,2),q(:,1))+[90; 90; 90;]-asind(arg);
            // :  theta_err = Wrap180(theta_diff+theta_off-theta);
            // :  y = x;
            // :  for i=1:3
            // :  if max(abs(theta_err)) < 0.01
            absx = (57.295779513082323 * rt_atan2d_snf(P_0[3], P_0[0]) + 90.0) -
              57.295779513082323 * std::asin(yb[0]);
            ya[0] = absx;
            xb[0] = (rtb_Gain_o_idx_0 + rtU.theta_off[0]) - absx;

            // :  while y(i) > 180.0
            while (xb[0] > 180.0) {
              // :  y(i) = y(i) - 360.0;
              xb[0] -= 360.0;
            }

            // :  while y(i) < -180.0
            while (xb[0] < -180.0) {
              // :  y(i) = y(i) + 360.0;
              xb[0] += 360.0;
            }

            xa[0] = std::abs(xb[0]);
            absx = (57.295779513082323 * rt_atan2d_snf(P_0[4], P_0[1]) + 90.0) -
              57.295779513082323 * std::asin(yb[1]);
            ya[1] = absx;
            xb[1] = (rtb_Gain_o_idx_1 + rtU.theta_off[1]) - absx;

            // :  while y(i) > 180.0
            while (xb[1] > 180.0) {
              // :  y(i) = y(i) - 360.0;
              xb[1] -= 360.0;
            }

            // :  while y(i) < -180.0
            while (xb[1] < -180.0) {
              // :  y(i) = y(i) + 360.0;
              xb[1] += 360.0;
            }

            xa[1] = std::abs(xb[1]);
            absx = (57.295779513082323 * rt_atan2d_snf(P_0[5], P_0[2]) + 90.0) -
              57.295779513082323 * std::asin(yb[2]);
            ya[2] = absx;
            xb[2] = (rtb_Gain_o_idx_2 + rtU.theta_off[2]) - absx;

            // :  while y(i) > 180.0
            while (xb[2] > 180.0) {
              // :  y(i) = y(i) - 360.0;
              xb[2] -= 360.0;
            }

            // :  while y(i) < -180.0
            while (xb[2] < -180.0) {
              // :  y(i) = y(i) + 360.0;
              xb[2] += 360.0;
            }

            xa[2] = std::abs(xb[2]);
            if (maximum(xa) < 0.01) {
              // :  T = normalize(T);
              rtDW.T.a = rtb_attitude_idx_0;
              rtDW.T.b = yc_idx_2;
              rtDW.T.c = b_n;
              rtDW.T.d = st;

              // :  attitude = compact(T);
              // :  singularity = singularity_reg;
              rtY.singularity = rtDW.singularity_reg;
              exitg1 = 1;
            } else {
              // :  p = [cosd(theta) sind(theta) Pz];
              yb[0] = ya[0];
              yb[1] = ya[1];
              yb[2] = absx;
              cosd(yb);
              sind(ya);
              P_0[3] = ya[0];
              P_0[1] = yb[1];
              P_0[4] = ya[1];
              P_0[2] = yb[2];
              P_0[5] = ya[2];

              // :  q1xp1 = cross(q(1,:),p(1,:));
              ya[0] = rtDW.Pz_b[0] * up_tmp_0 - P_0[3] * up_tmp_1;
              ya[1] = yb[0] * up_tmp_1 - rtDW.Pz_b[0] * up_tmp;
              ya[2] = up_tmp * P_0[3] - yb[0] * up_tmp_0;

              // :  q2xp2 = cross(q(2,:),p(2,:));
              yb[0] = rtDW.Pz_b[1] * up_tmp_3 - P_0[4] * up_tmp_4;
              yb[1] = P_0[1] * up_tmp_4 - rtDW.Pz_b[1] * up_tmp_2;
              yb[2] = up_tmp_2 * P_0[4] - P_0[1] * up_tmp_3;

              // :  q3xp3 = cross(q(3,:),p(3,:));
              yc_idx_2 = theta * P_0[5] - P_0[2] * rtb_Product1;

              // :  if min(abs([q1xp1(3) q2xp2(3) q3xp3(3)])) == 0.0
              xa[0] = std::abs(ya[2]);
              xa[1] = std::abs(yb[2]);
              xa[2] = std::abs(yc_idx_2);
              if (minimum(xa) == 0.0) {
                // :  cycles = -3;
                cycles = -3;
                guard1 = true;
                exitg1 = 1;
              } else {
                // :  if det([q1xp1/q1xp1(3); q2xp2/q2xp2(3); q3xp3/q3xp3(3)]) == 0.0 
                st = ya[0] / ya[2];
                ya_0[0] = st;
                absx = yb[0] / yb[2];
                ya_0[1] = absx;
                b_n = (rtDW.Pz_b[2] * rtb_Product1 - P_0[5] * maxval) / yc_idx_2;
                ya_0[2] = b_n;
                rtb_Product1 = ya[1] / ya[2];
                ya_0[3] = rtb_Product1;
                rtb_attitude_idx_0 = yb[1] / yb[2];
                ya_0[4] = rtb_attitude_idx_0;
                theta = (P_0[2] * maxval - rtDW.Pz_b[2] * theta) / yc_idx_2;
                ya_0[5] = theta;
                maxval = ya[2] / ya[2];
                ya_0[6] = maxval;
                up_tmp = yb[2] / yb[2];
                ya_0[7] = up_tmp;
                yc_idx_2 /= yc_idx_2;
                ya_0[8] = yc_idx_2;
                if (det(ya_0) == 0.0) {
                  // :  cycles = -4;
                  cycles = -4;
                  guard1 = true;
                  exitg1 = 1;
                } else {
                  // :  T = quaternion(([q1xp1/q1xp1(3); q2xp2/q2xp2(3); q3xp3/q3xp3(3)]\(0.02*theta_err))','rotvec')*T; 
                  xb[0] *= 0.02;
                  P_0[0] = st;
                  P_0[1] = absx;
                  P_0[2] = b_n;
                  xb[1] *= 0.02;
                  P_0[3] = rtb_Product1;
                  P_0[4] = rtb_attitude_idx_0;
                  P_0[5] = theta;
                  xb[2] *= 0.02;
                  P_0[6] = maxval;
                  P_0[7] = up_tmp;
                  P_0[8] = yc_idx_2;
                  r1 = 0;
                  r2 = 1;
                  r3 = 2;
                  maxval = std::abs(st);
                  absx = std::abs(absx);
                  if (absx > maxval) {
                    maxval = absx;
                    r1 = 1;
                    r2 = 0;
                  }

                  if (std::abs(b_n) > maxval) {
                    r1 = 2;
                    r2 = 1;
                    r3 = 0;
                  }

                  P_0[r2] /= P_0[r1];
                  P_0[r3] /= P_0[r1];
                  P_0[r2 + 3] -= P_0[r1 + 3] * P_0[r2];
                  P_0[r3 + 3] -= P_0[r1 + 3] * P_0[r3];
                  P_0[r2 + 6] -= P_0[r1 + 6] * P_0[r2];
                  P_0[r3 + 6] -= P_0[r1 + 6] * P_0[r3];
                  if (std::abs(P_0[r3 + 3]) > std::abs(P_0[r2 + 3])) {
                    rtemp = r2;
                    r2 = r3;
                    r3 = rtemp;
                  }

                  P_0[r3 + 3] /= P_0[r2 + 3];
                  P_0[r3 + 6] -= P_0[r3 + 3] * P_0[r2 + 6];
                  yb[1] = xb[r2] - xb[r1] * P_0[r2];
                  yb[2] = (xb[r3] - xb[r1] * P_0[r3]) - P_0[r3 + 3] * yb[1];
                  yb[2] /= P_0[r3 + 6];
                  yb[0] = xb[r1] - P_0[r1 + 6] * yb[2];
                  yb[1] -= P_0[r2 + 6] * yb[2];
                  yb[1] /= P_0[r2 + 3];
                  yb[0] -= P_0[r1 + 3] * yb[1];
                  yb[0] /= P_0[r1];
                  maxval = 1.0;
                  absx = 0.0;
                  rtb_Product1 = 0.0;
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
                    maxval = std::cos(theta / 2.0);
                    absx = yb[0];
                    rtb_Product1 = yb[1];
                    b_n = yb[2];
                  }

                  theta = rtDW.T.a;
                  st = rtDW.T.b;
                  yc_idx_2 = rtDW.T.c;
                  rtDW.T.a = ((maxval * rtDW.T.a - absx * rtDW.T.b) -
                              rtb_Product1 * rtDW.T.c) - b_n * rtDW.T.d;
                  rtDW.T.b = ((maxval * rtDW.T.b + absx * theta) + rtb_Product1 *
                              rtDW.T.d) - b_n * rtDW.T.c;
                  rtDW.T.c = ((maxval * rtDW.T.c - absx * rtDW.T.d) +
                              rtb_Product1 * theta) + b_n * st;
                  rtDW.T.d = ((maxval * rtDW.T.d + absx * yc_idx_2) -
                              rtb_Product1 * st) + b_n * theta;
                  idx++;
                  guard1 = false;
                }
              }
            }
          }
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

      // :  attitude = compact(T);
      rtb_attitude_idx_0 = rtDW.T.a;
      yc_idx_2 = rtDW.T.b;
      b_n = rtDW.T.b;
      st = rtDW.T.b;

      // :  singularity_reg = 1;
      rtDW.singularity_reg = 1.0;

      // :  singularity = singularity_reg;
      rtY.singularity = 1.0;
    }
  }

  // Sqrt: '<S12>/sqrt' incorporates:
  //   Product: '<S13>/Product'
  //   Product: '<S13>/Product1'
  //   Product: '<S13>/Product2'
  //   Product: '<S13>/Product3'
  //   Sum: '<S13>/Sum'

  maxval = std::sqrt(((rtb_attitude_idx_0 * rtb_attitude_idx_0 + yc_idx_2 *
                       yc_idx_2) + b_n * b_n) + st * st);

  // Product: '<S7>/Product'
  absx = rtb_attitude_idx_0 / maxval;

  // Product: '<S7>/Product1'
  rtb_Product1 = yc_idx_2 / maxval;

  // Product: '<S7>/Product2'
  b_n /= maxval;

  // Product: '<S7>/Product3'
  maxval = st / maxval;

  // Fcn: '<S2>/fcn3'
  theta = (rtb_Product1 * maxval - absx * b_n) * -2.0;

  // Fcn: '<S2>/fcn2' incorporates:
  //   Fcn: '<S2>/fcn5'

  rtb_Gain_o_idx_2 = absx * absx;
  rtb_Gain_o_idx_1 = rtb_Product1 * rtb_Product1;
  st = b_n * b_n;
  yc_idx_2 = maxval * maxval;

  // Gain: '<S3>/Gain' incorporates:
  //   Fcn: '<S2>/fcn1'
  //   Fcn: '<S2>/fcn2'
  //   Trigonometry: '<S6>/Trigonometric Function1'

  rtb_Gain_o_idx_0 = rt_atan2d_snf((rtb_Product1 * b_n + absx * maxval) * 2.0,
    ((rtb_Gain_o_idx_2 + rtb_Gain_o_idx_1) - st) - yc_idx_2) *
    57.295779513082323;

  // If: '<S8>/If' incorporates:
  //   Constant: '<S10>/Constant'
  //   Constant: '<S9>/Constant'

  if (theta > 1.0) {
    // Outputs for IfAction SubSystem: '<S8>/If Action Subsystem' incorporates:
    //   ActionPort: '<S9>/Action Port'

    theta = 1.0;

    // End of Outputs for SubSystem: '<S8>/If Action Subsystem'
  } else if (theta < -1.0) {
    // Outputs for IfAction SubSystem: '<S8>/If Action Subsystem1' incorporates:
    //   ActionPort: '<S10>/Action Port'

    theta = 1.0;

    // End of Outputs for SubSystem: '<S8>/If Action Subsystem1'
  }

  // Gain: '<S3>/Gain' incorporates:
  //   Fcn: '<S2>/fcn4'
  //   Fcn: '<S2>/fcn5'
  //   Trigonometry: '<S6>/Trigonometric Function3'

  rtb_Gain_o_idx_2 = rt_atan2d_snf((b_n * maxval + absx * rtb_Product1) * 2.0,
    ((rtb_Gain_o_idx_2 - rtb_Gain_o_idx_1) - st) + yc_idx_2) *
    57.295779513082323;

  // MATLAB Function: '<S1>/ypr2motors' incorporates:
  //   Inport: '<Root>/arm_bend'
  //   Inport: '<Root>/plat_off'
  //   Inport: '<Root>/theta_off'

  // :  if isempty(Pz)
  if (!rtDW.Pz_not_empty) {
    // :  Pz = -tand(arm_bend)*[1; 1; 1];
    if (rtIsInf(rtU.arm_bend) || rtIsNaN(rtU.arm_bend)) {
      maxval = (rtNaN);
    } else {
      maxval = rt_remd_snf(rtU.arm_bend, 360.0);
      absx = std::abs(maxval);
      if (absx > 180.0) {
        if (maxval > 0.0) {
          maxval -= 360.0;
        } else {
          maxval += 360.0;
        }

        absx = std::abs(maxval);
      }

      if (absx <= 45.0) {
        maxval *= 0.017453292519943295;
        n = 0;
      } else if (absx <= 135.0) {
        if (maxval > 0.0) {
          maxval = (maxval - 90.0) * 0.017453292519943295;
          n = 1;
        } else {
          maxval = (maxval + 90.0) * 0.017453292519943295;
          n = -1;
        }
      } else if (maxval > 0.0) {
        maxval = (maxval - 180.0) * 0.017453292519943295;
        n = 2;
      } else {
        maxval = (maxval + 180.0) * 0.017453292519943295;
        n = -2;
      }

      maxval = std::tan(maxval);
      if ((n == 1) || (n == -1)) {
        absx = 1.0 / maxval;
        maxval = -(1.0 / maxval);
        if (rtIsInf(maxval) && (n == 1)) {
          maxval = absx;
        }
      }
    }

    rtDW.Pz_not_empty = true;

    // :  P = [cosd(theta_off) sind(theta_off) Pz];
    rtDW.Pz[0] = -maxval;
    yb[0] = rtU.theta_off[0];
    rtDW.Pz[1] = -maxval;
    yb[1] = rtU.theta_off[1];
    rtDW.Pz[2] = -maxval;
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

  absx *= 0.017453292519943295;
  rtb_Gain_o_idx_1 = std::cos(absx);
  maxval = std::sin(absx);
  absx = 0.017453292519943295 * rtU.ypr_star[1];
  b_n = std::cos(absx);
  st = std::sin(absx);

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

  absx *= 0.017453292519943295;
  rtb_Product1 = std::cos(absx);
  absx = std::sin(absx);
  b[0] = rtb_Gain_o_idx_1 * b_n;
  yc_idx_2 = st * absx;
  b[3] = yc_idx_2 * rtb_Gain_o_idx_1 - maxval * rtb_Product1;
  rtb_attitude_idx_0 = st * rtb_Product1;
  b[6] = rtb_attitude_idx_0 * rtb_Gain_o_idx_1 + maxval * absx;
  b[1] = maxval * b_n;
  b[4] = yc_idx_2 * maxval + rtb_Gain_o_idx_1 * rtb_Product1;
  b[7] = rtb_attitude_idx_0 * maxval - rtb_Gain_o_idx_1 * absx;
  b[2] = -st;
  b[5] = b_n * absx;
  b[8] = b_n * rtb_Product1;
  for (r1 = 0; r1 < 3; r1++) {
    rtb_Gain_o_idx_1 = rtDW.Q[r1 + 3];
    maxval = rtDW.Q[r1];
    absx = rtDW.Q[r1 + 6];
    for (idx = 0; idx < 3; idx++) {
      P_0[r1 + 3 * idx] = (b[idx + 3] * rtb_Gain_o_idx_1 + maxval * b[idx]) +
        b[idx + 6] * absx;
    }
  }

  // :  m = [sqrt(q(1,1)^2+q(1,2)^2) sqrt(q(2,1)^2+q(2,2)^2) sqrt(q(3,1)^2+q(3,2)^2)]; 
  absx = std::sqrt(P_0[0] * P_0[0] + P_0[3] * P_0[3]);
  tmp = std::sqrt(P_0[1] * P_0[1] + P_0[4] * P_0[4]);
  tmp_0 = std::sqrt(P_0[2] * P_0[2] + P_0[5] * P_0[5]);

  // :  if min(abs(m)) == 0
  ya[0] = absx;
  ya[1] = tmp;
  ya[2] = tmp_0;
  if (minimum(ya) == 0.0) {
    // :  theta = last_theta_ok;
    xb[0] = rtDW.last_theta_ok[0];
    xb[1] = rtDW.last_theta_ok[1];
    xb[2] = rtDW.last_theta_ok[2];

    // Outport: '<Root>/out_of_range'
    // :  out_of_range = 1;
    rtY.out_of_range = 1.0;
  } else {
    // :  arg = [(PQ(1)-Pz(1)*q(1,3))/m(1); (PQ(2)-Pz(2)*q(2,3))/m(2); (PQ(3)-Pz(3)*q(3,3))/m(3)]; 
    xa[0] = (rtDW.PQ[0] - rtDW.Pz[0] * P_0[6]) / absx;
    xa[1] = (rtDW.PQ[1] - rtDW.Pz[1] * P_0[7]) / tmp;
    xa[2] = (rtDW.PQ[2] - rtDW.Pz[2] * P_0[8]) / tmp_0;

    // :  if max(abs(arg)) > 1
    ya[0] = std::abs(xa[0]);
    ya[1] = std::abs(xa[1]);
    ya[2] = std::abs(xa[2]);
    if (!rtIsNaN(ya[0])) {
      idx = 1;
    } else {
      idx = 0;
      r1 = 2;
      exitg2 = false;
      while ((!exitg2) && (r1 < 4)) {
        if (!rtIsNaN(ya[r1 - 1])) {
          idx = r1;
          exitg2 = true;
        } else {
          r1++;
        }
      }
    }

    if (idx == 0) {
      maxval = ya[0];
    } else {
      maxval = ya[idx - 1];
      for (r1 = idx + 1; r1 < 4; r1++) {
        absx = ya[r1 - 1];
        if (maxval < absx) {
          maxval = absx;
        }
      }
    }

    if (maxval > 1.0) {
      // :  theta = last_theta_ok;
      xb[0] = rtDW.last_theta_ok[0];
      xb[1] = rtDW.last_theta_ok[1];
      xb[2] = rtDW.last_theta_ok[2];

      // Outport: '<Root>/out_of_range'
      // :  out_of_range = 1;
      rtY.out_of_range = 1.0;
    } else {
      // :  theta = atan2d(q(:,2),q(:,1))+[90; 90; 90;]-asind(arg);
      rtb_Gain_o_idx_1 = (57.295779513082323 * rt_atan2d_snf(P_0[3], P_0[0]) +
                          90.0) - 57.295779513082323 * std::asin(xa[0]);
      maxval = (57.295779513082323 * rt_atan2d_snf(P_0[4], P_0[1]) + 90.0) -
        57.295779513082323 * std::asin(xa[1]);
      absx = (57.295779513082323 * rt_atan2d_snf(P_0[5], P_0[2]) + 90.0) -
        57.295779513082323 * std::asin(xa[2]);

      // :  gap = abs(Wrap180([(theta(2)-theta(1)) (theta(3)-theta(2)) (theta(1)-theta(3))])); 
      yb[0] = maxval - rtb_Gain_o_idx_1;
      yb[1] = absx - maxval;
      yb[2] = rtb_Gain_o_idx_1 - absx;

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
        xb[0] = rtDW.last_theta_ok[0];
        xb[1] = rtDW.last_theta_ok[1];
        xb[2] = rtDW.last_theta_ok[2];

        // Outport: '<Root>/out_of_range'
        // :  out_of_range = 1;
        rtY.out_of_range = 1.0;
      } else {
        // :  theta = Wrap180(theta-theta_off);
        // :  y = x;
        // :  for i=1:3
        // :  last_theta_ok = theta;
        xb[0] = rtb_Gain_o_idx_1 - rtU.theta_off[0];

        // :  while y(i) > 180.0
        while (xb[0] > 180.0) {
          // :  y(i) = y(i) - 360.0;
          xb[0] -= 360.0;
        }

        // :  while y(i) < -180.0
        while (xb[0] < -180.0) {
          // :  y(i) = y(i) + 360.0;
          xb[0] += 360.0;
        }

        rtDW.last_theta_ok[0] = xb[0];
        xb[1] = maxval - rtU.theta_off[1];

        // :  while y(i) > 180.0
        while (xb[1] > 180.0) {
          // :  y(i) = y(i) - 360.0;
          xb[1] -= 360.0;
        }

        // :  while y(i) < -180.0
        while (xb[1] < -180.0) {
          // :  y(i) = y(i) + 360.0;
          xb[1] += 360.0;
        }

        rtDW.last_theta_ok[1] = xb[1];
        xb[2] = absx - rtU.theta_off[2];

        // :  while y(i) > 180.0
        while (xb[2] > 180.0) {
          // :  y(i) = y(i) - 360.0;
          xb[2] -= 360.0;
        }

        // :  while y(i) < -180.0
        while (xb[2] < -180.0) {
          // :  y(i) = y(i) + 360.0;
          xb[2] += 360.0;
        }

        rtDW.last_theta_ok[2] = xb[2];

        // Outport: '<Root>/out_of_range' incorporates:
        //   Inport: '<Root>/theta_off'

        // :  out_of_range = 0;
        rtY.out_of_range = 0.0;
      }
    }
  }

  // End of Outputs for SubSystem: '<Root>/wrist_decoupler'

  // Outport: '<Root>/theta_star'
  rtY.theta_star[0] = xb[0];

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
  rtY.theta_star[1] = xb[1];

  // Outputs for Atomic SubSystem: '<Root>/wrist_decoupler'
  // If: '<S8>/If' incorporates:
  //   Gain: '<S3>/Gain'
  //   Outport: '<Root>/ypr_meas'
  //   Switch: '<S1>/Switch1'
  //   Trigonometry: '<S6>/trigFcn'

  rtY.ypr_meas[1] = 57.295779513082323 * std::asin(theta);

  // End of Outputs for SubSystem: '<Root>/wrist_decoupler'

  // Outport: '<Root>/theta_star'
  rtY.theta_star[2] = xb[2];

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
