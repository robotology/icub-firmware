//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: wrist_decoupler.cpp
//
// Code generated for Simulink model 'wrist_decoupler'.
//
// Model version                  : 2.3
// Simulink Coder version         : 9.4 (R2020b) 29-Jul-2020
// C/C++ source code generated on : Tue Nov 17 12:30:21 2020
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#include "wrist_decoupler.h"
#define NumBitsPerChar                 8U

extern real_T rt_atan2d_snf(real_T u0, real_T u1);
extern real_T rt_remd_snf(real_T u0, real_T u1);

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

extern "C" {
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
    IEEESingle nanF = { { 0 } };

    nanF.wordL.wordLuint = 0xFFC00000U;
    return nanF.wordL.wordLreal;
  }
}

extern "C" {
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

real_T rt_atan2d_snf(real_T u0, real_T u1)
{
  real_T y;
  int32_T u0_0;
  int32_T u1_0;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = (rtNaN);
  } else if (rtIsInf(u0) && rtIsInf(u1)) {
    if (u0 > 0.0) {
      u0_0 = 1;
    } else {
      u0_0 = -1;
    }

    if (u1 > 0.0) {
      u1_0 = 1;
    } else {
      u1_0 = -1;
    }

    y = std::atan2(static_cast<real_T>(u0_0), static_cast<real_T>(u1_0));
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

// Function for MATLAB Function: '<S2>/motors2quat'
real_T wrist_decouplerModelClass::maximum(const real_T x[3])
{
  real_T ex;
  int32_T idx;
  int32_T k;
  boolean_T exitg1;
  if (!rtIsNaN(x[0])) {
    idx = 1;
  } else {
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
    while (idx + 1 <= 3) {
      if (ex < x[idx]) {
        ex = x[idx];
      }

      idx++;
    }
  }

  return ex;
}

// Function for MATLAB Function: '<S2>/motors2quat'
real_T wrist_decouplerModelClass::minimum(const real_T x[3])
{
  real_T ex;
  int32_T idx;
  int32_T k;
  boolean_T exitg1;
  if (!rtIsNaN(x[0])) {
    idx = 1;
  } else {
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
    while (idx + 1 <= 3) {
      if (ex > x[idx]) {
        ex = x[idx];
      }

      idx++;
    }
  }

  return ex;
}

real_T rt_remd_snf(real_T u0, real_T u1)
{
  real_T u1_0;
  real_T y;
  if (rtIsNaN(u0) || rtIsNaN(u1) || rtIsInf(u0)) {
    y = (rtNaN);
  } else if (rtIsInf(u1)) {
    y = u0;
  } else {
    if (u1 < 0.0) {
      u1_0 = std::ceil(u1);
    } else {
      u1_0 = std::floor(u1);
    }

    if ((u1 != 0.0) && (u1 != u1_0)) {
      u1_0 = std::abs(u0 / u1);
      if (!(std::abs(u1_0 - std::floor(u1_0 + 0.5)) > DBL_EPSILON * u1_0)) {
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

// Function for MATLAB Function: '<S2>/motors2quat'
void wrist_decouplerModelClass::sind(real_T x[3])
{
  real_T absx;
  real_T c_x;
  int8_T n;
  if (rtIsInf(x[0]) || rtIsNaN(x[0])) {
    c_x = (rtNaN);
  } else {
    c_x = rt_remd_snf(x[0], 360.0);
    absx = std::abs(c_x);
    if (absx > 180.0) {
      if (c_x > 0.0) {
        c_x -= 360.0;
      } else {
        c_x += 360.0;
      }

      absx = std::abs(c_x);
    }

    if (absx <= 45.0) {
      c_x *= 0.017453292519943295;
      n = 0;
    } else if (absx <= 135.0) {
      if (c_x > 0.0) {
        c_x = (c_x - 90.0) * 0.017453292519943295;
        n = 1;
      } else {
        c_x = (c_x + 90.0) * 0.017453292519943295;
        n = -1;
      }
    } else if (c_x > 0.0) {
      c_x = (c_x - 180.0) * 0.017453292519943295;
      n = 2;
    } else {
      c_x = (c_x + 180.0) * 0.017453292519943295;
      n = -2;
    }

    switch (n) {
     case 0:
      c_x = std::sin(c_x);
      break;

     case 1:
      c_x = std::cos(c_x);
      break;

     case -1:
      c_x = -std::cos(c_x);
      break;

     default:
      c_x = -std::sin(c_x);
      break;
    }
  }

  x[0] = c_x;
  if (rtIsInf(x[1]) || rtIsNaN(x[1])) {
    c_x = (rtNaN);
  } else {
    c_x = rt_remd_snf(x[1], 360.0);
    absx = std::abs(c_x);
    if (absx > 180.0) {
      if (c_x > 0.0) {
        c_x -= 360.0;
      } else {
        c_x += 360.0;
      }

      absx = std::abs(c_x);
    }

    if (absx <= 45.0) {
      c_x *= 0.017453292519943295;
      n = 0;
    } else if (absx <= 135.0) {
      if (c_x > 0.0) {
        c_x = (c_x - 90.0) * 0.017453292519943295;
        n = 1;
      } else {
        c_x = (c_x + 90.0) * 0.017453292519943295;
        n = -1;
      }
    } else if (c_x > 0.0) {
      c_x = (c_x - 180.0) * 0.017453292519943295;
      n = 2;
    } else {
      c_x = (c_x + 180.0) * 0.017453292519943295;
      n = -2;
    }

    switch (n) {
     case 0:
      c_x = std::sin(c_x);
      break;

     case 1:
      c_x = std::cos(c_x);
      break;

     case -1:
      c_x = -std::cos(c_x);
      break;

     default:
      c_x = -std::sin(c_x);
      break;
    }
  }

  x[1] = c_x;
  if (rtIsInf(x[2]) || rtIsNaN(x[2])) {
    c_x = (rtNaN);
  } else {
    c_x = rt_remd_snf(x[2], 360.0);
    absx = std::abs(c_x);
    if (absx > 180.0) {
      if (c_x > 0.0) {
        c_x -= 360.0;
      } else {
        c_x += 360.0;
      }

      absx = std::abs(c_x);
    }

    if (absx <= 45.0) {
      c_x *= 0.017453292519943295;
      n = 0;
    } else if (absx <= 135.0) {
      if (c_x > 0.0) {
        c_x = (c_x - 90.0) * 0.017453292519943295;
        n = 1;
      } else {
        c_x = (c_x + 90.0) * 0.017453292519943295;
        n = -1;
      }
    } else if (c_x > 0.0) {
      c_x = (c_x - 180.0) * 0.017453292519943295;
      n = 2;
    } else {
      c_x = (c_x + 180.0) * 0.017453292519943295;
      n = -2;
    }

    switch (n) {
     case 0:
      c_x = std::sin(c_x);
      break;

     case 1:
      c_x = std::cos(c_x);
      break;

     case -1:
      c_x = -std::cos(c_x);
      break;

     default:
      c_x = -std::sin(c_x);
      break;
    }
  }

  x[2] = c_x;
}

// Function for MATLAB Function: '<S2>/motors2quat'
real_T wrist_decouplerModelClass::det(const real_T x[9])
{
  real_T A[9];
  real_T b_y;
  real_T smax;
  real_T y;
  int32_T c;
  int32_T c_ix;
  int32_T c_k;
  int32_T d;
  int32_T ijA;
  int32_T ix;
  int32_T iy;
  int32_T j;
  int8_T ipiv[3];
  boolean_T isodd;
  std::memcpy(&A[0], &x[0], 9U * sizeof(real_T));
  ipiv[0] = 1;
  ipiv[1] = 2;
  for (j = 0; j < 2; j++) {
    c = j << 2;
    iy = 0;
    ix = c;
    smax = std::abs(A[c]);
    for (c_k = 2; c_k <= 3 - j; c_k++) {
      ix++;
      b_y = std::abs(A[ix]);
      if (b_y > smax) {
        iy = c_k - 1;
        smax = b_y;
      }
    }

    if (A[c + iy] != 0.0) {
      if (iy != 0) {
        iy += j;
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

      iy = (c - j) + 3;
      for (ix = c + 1; ix < iy; ix++) {
        A[ix] /= A[c];
      }
    }

    iy = c;
    ix = c + 3;
    for (c_k = 0; c_k <= 1 - j; c_k++) {
      if (A[ix] != 0.0) {
        smax = -A[ix];
        c_ix = c + 1;
        d = (iy - j) + 6;
        for (ijA = iy + 4; ijA < d; ijA++) {
          A[ijA] += A[c_ix] * smax;
          c_ix++;
        }
      }

      ix += 3;
      iy += 3;
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

// Function for MATLAB Function: '<S2>/motors2quat'
void wrist_decouplerModelClass::mldivide(const real_T A[9], const real_T B_0[3],
  real_T Y[3])
{
  real_T b_A[9];
  real_T maxval;
  real_T y;
  int32_T r1;
  int32_T r2;
  int32_T r3;
  int32_T rtemp;
  std::memcpy(&b_A[0], &A[0], 9U * sizeof(real_T));
  r1 = 0;
  r2 = 1;
  r3 = 2;
  maxval = std::abs(A[0]);
  y = std::abs(A[1]);
  if (y > maxval) {
    maxval = y;
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
void wrist_decouplerModelClass::step()
{
  real_T oa[12];
  real_T up[12];
  real_T arg_0[9];
  real_T p[9];
  real_T q[9];
  real_T arg[3];
  real_T ct[3];
  real_T st[3];
  real_T absx;
  real_T b_oc_idx_0;
  real_T b_oc_idx_1;
  real_T b_oc_idx_2;
  real_T c_oc;
  real_T oa_idx_0;
  real_T od;
  real_T rtb_Product1;
  real_T rtb_Product2;
  real_T rtb_Product3;
  real_T st_0;
  real_T theta;
  int32_T b_cycles;
  int32_T b_k;
  int32_T exitg2;
  int32_T idx;
  int32_T q_tmp;
  int8_T b_n;
  boolean_T exitg1;
  boolean_T guard1 = false;

  // Outputs for Atomic SubSystem: '<Root>/wrist_decoupler'
  // Outport: '<Root>/theta_star' incorporates:
  //   MATLAB Function: '<S1>/ypr2motors'

  // :  theta = [0;0;0];
  // :  q = Q*eul2rotm(deg2rad(ypr'))';
  rtY.theta_star[0] = 0.0;

  // MATLAB Function: '<S1>/ypr2motors' incorporates:
  //   Inport: '<Root>/ypr_star'

  st_0 = 0.017453292519943295 * rtU.ypr[0];
  ct[0] = std::cos(st_0);
  st[0] = std::sin(st_0);

  // Outport: '<Root>/theta_star' incorporates:
  //   MATLAB Function: '<S1>/ypr2motors'

  rtY.theta_star[1] = 0.0;

  // MATLAB Function: '<S1>/ypr2motors' incorporates:
  //   Inport: '<Root>/ypr_star'

  st_0 = 0.017453292519943295 * rtU.ypr[1];
  ct[1] = std::cos(st_0);
  st[1] = std::sin(st_0);

  // Outport: '<Root>/theta_star' incorporates:
  //   MATLAB Function: '<S1>/ypr2motors'

  rtY.theta_star[2] = 0.0;

  // MATLAB Function: '<S1>/ypr2motors' incorporates:
  //   Inport: '<Root>/ypr_star'

  st_0 = 0.017453292519943295 * rtU.ypr[2];
  c_oc = std::cos(st_0);
  st_0 = std::sin(st_0);
  p[0] = ct[1] * ct[0];
  rtb_Product3 = st_0 * st[1];
  p[3] = rtb_Product3 * ct[0] - c_oc * st[0];
  absx = c_oc * st[1];
  p[6] = absx * ct[0] + st_0 * st[0];
  p[1] = ct[1] * st[0];
  p[4] = rtb_Product3 * st[0] + c_oc * ct[0];
  p[7] = absx * st[0] - st_0 * ct[0];
  p[2] = -st[1];
  p[5] = st_0 * ct[1];
  p[8] = c_oc * ct[1];
  for (idx = 0; idx < 3; idx++) {
    for (b_k = 0; b_k < 3; b_k++) {
      q_tmp = idx + 3 * b_k;
      q[q_tmp] = 0.0;
      q[q_tmp] += rtConstP.pooled4[idx] * p[b_k];
      q[q_tmp] += rtConstP.pooled4[idx + 3] * p[b_k + 3];
      q[q_tmp] += rtConstP.pooled4[idx + 6] * p[b_k + 6];
    }
  }

  // :  m = [sqrt(q(1,1)^2+q(1,2)^2) sqrt(q(2,1)^2+q(2,2)^2) sqrt(q(3,1)^2+q(3,2)^2)]; 
  absx = std::sqrt(q[0] * q[0] + q[3] * q[3]);
  c_oc = std::sqrt(q[1] * q[1] + q[4] * q[4]);
  b_oc_idx_1 = std::sqrt(q[2] * q[2] + q[5] * q[5]);

  // :  if min(abs(m)) == 0
  st[0] = std::abs(absx);
  st[1] = std::abs(c_oc);
  st[2] = std::abs(b_oc_idx_1);
  if (!rtIsNaN(st[0])) {
    idx = 1;
  } else {
    idx = 0;
    b_k = 2;
    exitg1 = false;
    while ((!exitg1) && (b_k < 4)) {
      if (!rtIsNaN(st[b_k - 1])) {
        idx = b_k;
        exitg1 = true;
      } else {
        b_k++;
      }
    }
  }

  if (idx == 0) {
    rtb_Product3 = st[0];
  } else {
    rtb_Product3 = st[idx - 1];
    while (idx + 1 <= 3) {
      if (rtb_Product3 > st[idx]) {
        rtb_Product3 = st[idx];
      }

      idx++;
    }
  }

  if (rtb_Product3 == 0.0) {
    // Outport: '<Root>/out_of_range'
    // :  out_of_range = 1;
    rtY.out_of_range = 1.0;
  } else {
    // :  arg = [(PQ(1)-Pz(1)*q(1,3))/m(1); (PQ(2)-Pz(2)*q(2,3))/m(2); (PQ(3)-Pz(3)*q(3,3))/m(3)]; 
    arg[0] = (0.5 - -0.57735026918962573 * q[6]) / absx;
    arg[1] = (0.087155742747658166 - -0.57735026918962573 * q[7]) / c_oc;
    arg[2] = (-0.49999999999999994 - -0.57735026918962573 * q[8]) / b_oc_idx_1;

    // :  if max(abs(arg)) > 1
    st[0] = std::abs(arg[0]);
    st[1] = std::abs(arg[1]);
    st[2] = std::abs(arg[2]);
    if (!rtIsNaN(st[0])) {
      idx = 1;
    } else {
      idx = 0;
      b_k = 2;
      exitg1 = false;
      while ((!exitg1) && (b_k < 4)) {
        if (!rtIsNaN(st[b_k - 1])) {
          idx = b_k;
          exitg1 = true;
        } else {
          b_k++;
        }
      }
    }

    if (idx == 0) {
      rtb_Product3 = st[0];
    } else {
      rtb_Product3 = st[idx - 1];
      while (idx + 1 <= 3) {
        if (rtb_Product3 < st[idx]) {
          rtb_Product3 = st[idx];
        }

        idx++;
      }
    }

    if (rtb_Product3 > 1.0) {
      // Outport: '<Root>/out_of_range'
      // :  out_of_range = 1;
      rtY.out_of_range = 1.0;
    } else {
      // Outport: '<Root>/theta_star'
      // :  theta = atan2d(q(:,2),q(:,1))+[90; 90; 90;]-asind(arg);
      rtY.theta_star[0] = (57.295779513082323 * rt_atan2d_snf(q[3], q[0]) + 90.0)
        - 57.295779513082323 * std::asin(arg[0]);
      rtY.theta_star[1] = (57.295779513082323 * rt_atan2d_snf(q[4], q[1]) + 90.0)
        - 57.295779513082323 * std::asin(arg[1]);
      rtY.theta_star[2] = (57.295779513082323 * rt_atan2d_snf(q[5], q[2]) + 90.0)
        - 57.295779513082323 * std::asin(arg[2]);

      // Outport: '<Root>/out_of_range'
      // :  out_of_range = 0;
      rtY.out_of_range = 0.0;
    }
  }

  // Outputs for Atomic SubSystem: '<S1>/motors2ypr'
  // MATLAB Function: '<S2>/motors2quat' incorporates:
  //   Inport: '<Root>/theta_meas'

  // :  if isempty(T)
  // :  if isempty(singularity_reg)
  // :  theta_diff = theta_meas - [145; -90; 40];
  // :  for k=1:3
  // :  if max(abs(theta_diff)) < 5.0
  arg[0] = rtU.theta_meas[0] - 145.0;

  // :  while theta_diff(k) > 180.0
  while (arg[0] > 180.0) {
    // :  theta_diff(k) = theta_diff(k) - 360.0;
    arg[0] -= 360.0;
  }

  // :  while theta_diff(k) < -180.0
  while (arg[0] < -180.0) {
    // :  theta_diff(k) = theta_diff(k) + 360.0;
    arg[0] += 360.0;
  }

  st[0] = std::abs(arg[0]);
  arg[1] = rtU.theta_meas[1] - -90.0;

  // :  while theta_diff(k) > 180.0
  while (arg[1] > 180.0) {
    // :  theta_diff(k) = theta_diff(k) - 360.0;
    arg[1] -= 360.0;
  }

  // :  while theta_diff(k) < -180.0
  while (arg[1] < -180.0) {
    // :  theta_diff(k) = theta_diff(k) + 360.0;
    arg[1] += 360.0;
  }

  st[1] = std::abs(arg[1]);
  arg[2] = rtU.theta_meas[2] - 40.0;

  // :  while theta_diff(k) > 180.0
  while (arg[2] > 180.0) {
    // :  theta_diff(k) = theta_diff(k) - 360.0;
    arg[2] -= 360.0;
  }

  // :  while theta_diff(k) < -180.0
  while (arg[2] < -180.0) {
    // :  theta_diff(k) = theta_diff(k) + 360.0;
    arg[2] += 360.0;
  }

  st[2] = std::abs(arg[2]);
  if (maximum(st) < 5.0) {
    // :  singularity_reg = 0;
    rtDW.singularity_reg = 0.0;
  }

  // :  if singularity_reg
  if (rtDW.singularity_reg != 0.0) {
    // :  attitude = compact(T);
    absx = rtDW.T.a;
    theta = rtDW.T.b;
    b_oc_idx_1 = rtDW.T.c;
    c_oc = rtDW.T.d;

    // Outport: '<Root>/singularity'
    // :  singularity = singularity_reg;
    rtY.singularity = rtDW.singularity_reg;

    // :  cycles = 0;
    idx = 0;
  } else {
    // :  for cycles=1:200
    idx = 1;
    b_cycles = 0;
    guard1 = false;
    do {
      exitg2 = 0;
      if (b_cycles < 200) {
        idx = b_cycles + 1;

        // :  q = rotatepoint(T,Q);
        theta = std::sqrt(((rtDW.T.a * rtDW.T.a + rtDW.T.b * rtDW.T.b) +
                           rtDW.T.c * rtDW.T.c) + rtDW.T.d * rtDW.T.d);
        rtb_Product3 = rtDW.T.a / theta;
        absx = rtDW.T.b / theta;
        rtb_Product1 = rtDW.T.c / theta;
        rtb_Product2 = rtDW.T.d / theta;
        up[0] = 0.0;
        up[1] = 0.0;
        up[2] = 0.0;
        std::memcpy(&up[3], &rtConstP.pooled4[0], 9U * sizeof(real_T));
        oa_idx_0 = ((rtb_Product3 * up[0] - absx * up[3]) - rtb_Product1 * up[6])
          - rtb_Product2 * up[9];
        c_oc = ((rtb_Product3 * up[3] + absx * up[0]) + rtb_Product1 * up[9]) -
          rtb_Product2 * up[6];
        st_0 = ((rtb_Product3 * up[6] - absx * up[9]) + rtb_Product1 * up[0]) +
          rtb_Product2 * up[3];
        od = ((rtb_Product3 * up[9] + absx * up[6]) - rtb_Product1 * up[3]) +
          rtb_Product2 * up[0];
        oa[0] = ((oa_idx_0 * rtb_Product3 - c_oc * -absx) - st_0 * -rtb_Product1)
          - od * -rtb_Product2;
        st[0] = ((oa_idx_0 * -absx + c_oc * rtb_Product3) + st_0 * -rtb_Product2)
          - od * -rtb_Product1;
        b_oc_idx_0 = ((oa_idx_0 * -rtb_Product1 - c_oc * -rtb_Product2) + st_0 *
                      rtb_Product3) + od * -absx;
        arg[0] = ((oa_idx_0 * -rtb_Product2 + c_oc * -rtb_Product1) - st_0 *
                  -absx) + od * rtb_Product3;
        oa_idx_0 = ((rtb_Product3 * up[1] - absx * up[4]) - rtb_Product1 * up[7])
          - rtb_Product2 * up[10];
        c_oc = ((rtb_Product3 * up[4] + absx * up[1]) + rtb_Product1 * up[10]) -
          rtb_Product2 * up[7];
        st_0 = ((rtb_Product3 * up[7] - absx * up[10]) + rtb_Product1 * up[1]) +
          rtb_Product2 * up[4];
        od = ((rtb_Product3 * up[10] + absx * up[7]) - rtb_Product1 * up[4]) +
          rtb_Product2 * up[1];
        oa[1] = ((oa_idx_0 * rtb_Product3 - c_oc * -absx) - st_0 * -rtb_Product1)
          - od * -rtb_Product2;
        st[1] = ((oa_idx_0 * -absx + c_oc * rtb_Product3) + st_0 * -rtb_Product2)
          - od * -rtb_Product1;
        b_oc_idx_1 = ((oa_idx_0 * -rtb_Product1 - c_oc * -rtb_Product2) + st_0 *
                      rtb_Product3) + od * -absx;
        arg[1] = ((oa_idx_0 * -rtb_Product2 + c_oc * -rtb_Product1) - st_0 *
                  -absx) + od * rtb_Product3;
        oa_idx_0 = ((rtb_Product3 * up[2] - absx * up[5]) - rtb_Product1 * up[8])
          - rtb_Product2 * up[11];
        c_oc = ((rtb_Product3 * up[5] + absx * up[2]) + rtb_Product1 * up[11]) -
          rtb_Product2 * up[8];
        st_0 = ((rtb_Product3 * up[8] - absx * up[11]) + rtb_Product1 * up[2]) +
          rtb_Product2 * up[5];
        od = ((rtb_Product3 * up[11] + absx * up[8]) - rtb_Product1 * up[5]) +
          rtb_Product2 * up[2];
        oa[2] = ((oa_idx_0 * rtb_Product3 - c_oc * -absx) - st_0 * -rtb_Product1)
          - od * -rtb_Product2;
        st[2] = ((oa_idx_0 * -absx + c_oc * rtb_Product3) + st_0 * -rtb_Product2)
          - od * -rtb_Product1;
        b_oc_idx_2 = ((oa_idx_0 * -rtb_Product1 - c_oc * -rtb_Product2) + st_0 *
                      rtb_Product3) + od * -absx;
        oa_idx_0 = ((oa_idx_0 * -rtb_Product2 + c_oc * -rtb_Product1) - st_0 *
                    -absx) + od * rtb_Product3;
        up[3] = st[0];
        up[9] = arg[0];
        oa[3] = st[0];
        oa[6] = b_oc_idx_0;
        oa[9] = arg[0];
        up[4] = st[1];
        up[10] = arg[1];
        oa[4] = st[1];
        oa[7] = b_oc_idx_1;
        oa[10] = arg[1];
        st_0 = st[2];
        oa[5] = st[2];
        oa[8] = b_oc_idx_2;
        oa[11] = oa_idx_0;

        // :  m = [sqrt(q(1,1)^2+q(1,2)^2) sqrt(q(2,1)^2+q(2,2)^2) sqrt(q(3,1)^2+q(3,2)^2)]; 
        c_oc = b_oc_idx_0 * b_oc_idx_0;
        arg[0] = std::sqrt(st[0] * st[0] + c_oc);
        b_oc_idx_1 *= b_oc_idx_1;
        arg[1] = std::sqrt(st[1] * st[1] + b_oc_idx_1);
        arg[2] = std::sqrt(st[2] * st[2] + b_oc_idx_2 * b_oc_idx_2);

        // :  if min(abs(m)) == 0
        for (b_k = 0; b_k < 3; b_k++) {
          q_tmp = (b_k + 1) * 3;
          q[3 * b_k] = oa[q_tmp];
          q[3 * b_k + 1] = oa[q_tmp + 1];
          q[3 * b_k + 2] = oa[q_tmp + 2];
          st[b_k] = std::abs(arg[b_k]);
        }

        if (minimum(st) == 0.0) {
          // :  cycles = -1;
          idx = -1;
          guard1 = true;
          exitg2 = 1;
        } else {
          // :  arg = [(PQ(1)-Pz(1)*q(1,3))/m(1); (PQ(2)-Pz(2)*q(2,3))/m(2); (PQ(3)-Pz(3)*q(3,3))/m(3)]; 
          ct[0] = (0.5 - -0.57735026918962573 * up[9]) / std::sqrt(up[3] * up[3]
            + c_oc);
          ct[1] = (0.087155742747658166 - -0.57735026918962573 * up[10]) / std::
            sqrt(up[4] * up[4] + b_oc_idx_1);
          ct[2] = (-0.49999999999999994 - -0.57735026918962573 * oa_idx_0) / std::
            sqrt(st_0 * st_0 + b_oc_idx_2 * b_oc_idx_2);

          // :  if max(abs(arg)) > 1.0
          st[0] = std::abs(ct[0]);
          st[1] = std::abs(ct[1]);
          st[2] = std::abs(ct[2]);
          if (maximum(st) > 1.0) {
            // :  cycles = -2;
            idx = -2;
            guard1 = true;
            exitg2 = 1;
          } else {
            // :  theta = atan2d(q(:,2),q(:,1))+[90; 90; 90;]-asind(arg);
            // :  theta_err = theta_meas-theta;
            // :  for k=1:3
            // :  if max(abs(theta_err)) < 0.01
            ct[0] = 57.295779513082323 * std::asin(ct[0]);
            arg[0] = (57.295779513082323 * rt_atan2d_snf(q[3], q[0]) + 90.0) -
              ct[0];

            // :  while theta_err(k) > 180.0
            for (rtb_Product1 = rtU.theta_meas[0] - arg[0]; rtb_Product1 > 180.0;
                 rtb_Product1 -= 360.0) {
              // :  theta_err(k) = theta_err(k) - 360.0;
            }

            // :  while theta_err(k) < -180.0
            while (rtb_Product1 < -180.0) {
              // :  theta_err(k) = theta_err(k) + 360.0;
              rtb_Product1 += 360.0;
            }

            st[0] = std::abs(rtb_Product1);
            arg[1] = 57.295779513082323 * rt_atan2d_snf(q[4], q[1]);
            ct[1] = 57.295779513082323 * std::asin(ct[1]);
            arg[1] = (arg[1] + 90.0) - ct[1];

            // :  while theta_err(k) > 180.0
            for (rtb_Product2 = rtU.theta_meas[1] - arg[1]; rtb_Product2 > 180.0;
                 rtb_Product2 -= 360.0) {
              // :  theta_err(k) = theta_err(k) - 360.0;
            }

            // :  while theta_err(k) < -180.0
            while (rtb_Product2 < -180.0) {
              // :  theta_err(k) = theta_err(k) + 360.0;
              rtb_Product2 += 360.0;
            }

            st[1] = std::abs(rtb_Product2);
            arg[2] = 57.295779513082323 * rt_atan2d_snf(q[5], q[2]);
            ct[2] = 57.295779513082323 * std::asin(ct[2]);
            arg[2] = (arg[2] + 90.0) - ct[2];

            // :  while theta_err(k) > 180.0
            for (b_oc_idx_2 = rtU.theta_meas[2] - arg[2]; b_oc_idx_2 > 180.0;
                 b_oc_idx_2 -= 360.0) {
              // :  theta_err(k) = theta_err(k) - 360.0;
            }

            // :  while theta_err(k) < -180.0
            while (b_oc_idx_2 < -180.0) {
              // :  theta_err(k) = theta_err(k) + 360.0;
              b_oc_idx_2 += 360.0;
            }

            st[2] = std::abs(b_oc_idx_2);
            if (maximum(st) < 0.01) {
              // :  T = normalize(T);
              rtDW.T.a /= theta;
              rtDW.T.b /= theta;
              rtDW.T.c /= theta;
              rtDW.T.d /= theta;

              // :  attitude = compact(T);
              absx = rtDW.T.a;
              theta = rtDW.T.b;
              b_oc_idx_1 = rtDW.T.c;
              c_oc = rtDW.T.d;

              // :  singularity = singularity_reg;
              rtY.singularity = rtDW.singularity_reg;
              exitg2 = 1;
            } else {
              // :  p = [cosd(theta) sind(theta) Pz];
              if (rtIsInf(arg[0]) || rtIsNaN(arg[0])) {
                oa_idx_0 = (rtNaN);
              } else {
                rtb_Product3 = rt_remd_snf(arg[0], 360.0);
                absx = std::abs(rtb_Product3);
                if (absx > 180.0) {
                  if (rtb_Product3 > 0.0) {
                    rtb_Product3 -= 360.0;
                  } else {
                    rtb_Product3 += 360.0;
                  }

                  absx = std::abs(rtb_Product3);
                }

                if (absx <= 45.0) {
                  rtb_Product3 *= 0.017453292519943295;
                  b_n = 0;
                } else if (absx <= 135.0) {
                  if (rtb_Product3 > 0.0) {
                    rtb_Product3 = (rtb_Product3 - 90.0) * 0.017453292519943295;
                    b_n = 1;
                  } else {
                    rtb_Product3 = (rtb_Product3 + 90.0) * 0.017453292519943295;
                    b_n = -1;
                  }
                } else if (rtb_Product3 > 0.0) {
                  rtb_Product3 = (rtb_Product3 - 180.0) * 0.017453292519943295;
                  b_n = 2;
                } else {
                  rtb_Product3 = (rtb_Product3 + 180.0) * 0.017453292519943295;
                  b_n = -2;
                }

                switch (b_n) {
                 case 0:
                  oa_idx_0 = std::cos(rtb_Product3);
                  break;

                 case 1:
                  oa_idx_0 = -std::sin(rtb_Product3);
                  break;

                 case -1:
                  oa_idx_0 = std::sin(rtb_Product3);
                  break;

                 default:
                  oa_idx_0 = -std::cos(rtb_Product3);
                  break;
                }
              }

              if (rtIsInf(arg[1]) || rtIsNaN(arg[1])) {
                theta = (rtNaN);
              } else {
                rtb_Product3 = rt_remd_snf(arg[1], 360.0);
                absx = std::abs(rtb_Product3);
                if (absx > 180.0) {
                  if (rtb_Product3 > 0.0) {
                    rtb_Product3 -= 360.0;
                  } else {
                    rtb_Product3 += 360.0;
                  }

                  absx = std::abs(rtb_Product3);
                }

                if (absx <= 45.0) {
                  rtb_Product3 *= 0.017453292519943295;
                  b_n = 0;
                } else if (absx <= 135.0) {
                  if (rtb_Product3 > 0.0) {
                    rtb_Product3 = (rtb_Product3 - 90.0) * 0.017453292519943295;
                    b_n = 1;
                  } else {
                    rtb_Product3 = (rtb_Product3 + 90.0) * 0.017453292519943295;
                    b_n = -1;
                  }
                } else if (rtb_Product3 > 0.0) {
                  rtb_Product3 = (rtb_Product3 - 180.0) * 0.017453292519943295;
                  b_n = 2;
                } else {
                  rtb_Product3 = (rtb_Product3 + 180.0) * 0.017453292519943295;
                  b_n = -2;
                }

                switch (b_n) {
                 case 0:
                  theta = std::cos(rtb_Product3);
                  break;

                 case 1:
                  theta = -std::sin(rtb_Product3);
                  break;

                 case -1:
                  theta = std::sin(rtb_Product3);
                  break;

                 default:
                  theta = -std::cos(rtb_Product3);
                  break;
                }
              }

              if (rtIsInf(arg[2]) || rtIsNaN(arg[2])) {
                rtb_Product3 = (rtNaN);
              } else {
                rtb_Product3 = rt_remd_snf(arg[2], 360.0);
                absx = std::abs(rtb_Product3);
                if (absx > 180.0) {
                  if (rtb_Product3 > 0.0) {
                    rtb_Product3 -= 360.0;
                  } else {
                    rtb_Product3 += 360.0;
                  }

                  absx = std::abs(rtb_Product3);
                }

                if (absx <= 45.0) {
                  rtb_Product3 *= 0.017453292519943295;
                  b_n = 0;
                } else if (absx <= 135.0) {
                  if (rtb_Product3 > 0.0) {
                    rtb_Product3 = (rtb_Product3 - 90.0) * 0.017453292519943295;
                    b_n = 1;
                  } else {
                    rtb_Product3 = (rtb_Product3 + 90.0) * 0.017453292519943295;
                    b_n = -1;
                  }
                } else if (rtb_Product3 > 0.0) {
                  rtb_Product3 = (rtb_Product3 - 180.0) * 0.017453292519943295;
                  b_n = 2;
                } else {
                  rtb_Product3 = (rtb_Product3 + 180.0) * 0.017453292519943295;
                  b_n = -2;
                }

                switch (b_n) {
                 case 0:
                  rtb_Product3 = std::cos(rtb_Product3);
                  break;

                 case 1:
                  rtb_Product3 = -std::sin(rtb_Product3);
                  break;

                 case -1:
                  rtb_Product3 = std::sin(rtb_Product3);
                  break;

                 default:
                  rtb_Product3 = -std::cos(rtb_Product3);
                  break;
                }
              }

              sind(arg);
              p[3] = arg[0];
              p[4] = arg[1];
              p[5] = arg[2];

              // :  q1xp1 = cross(q(1,:),p(1,:));
              arg[2] = q[0] * p[3] - q[3] * oa_idx_0;

              // :  q2xp2 = cross(q(2,:),p(2,:));
              ct[2] = q[1] * p[4] - q[4] * theta;

              // :  q3xp3 = cross(q(3,:),p(3,:));
              absx = q[2] * p[5] - q[5] * rtb_Product3;

              // :  if min(abs([q1xp1(3) q2xp2(3) q3xp3(3)])) == 0.0
              st[0] = std::abs(arg[2]);
              st[1] = std::abs(ct[2]);
              st[2] = std::abs(absx);
              if (minimum(st) == 0.0) {
                // :  cycles = -3;
                idx = -3;
                guard1 = true;
                exitg2 = 1;
              } else {
                // :  if det([q1xp1/q1xp1(3); q2xp2/q2xp2(3); q3xp3/q3xp3(3)]) == 0.0 
                c_oc = (q[3] * -0.57735026918962573 - q[6] * p[3]) / arg[2];
                arg_0[0] = c_oc;
                b_oc_idx_1 = (q[4] * -0.57735026918962573 - q[7] * p[4]) / ct[2];
                arg_0[1] = b_oc_idx_1;
                st_0 = (q[5] * -0.57735026918962573 - q[8] * p[5]) / absx;
                arg_0[2] = st_0;
                oa_idx_0 = (q[6] * oa_idx_0 - q[0] * -0.57735026918962573) /
                  arg[2];
                arg_0[3] = oa_idx_0;
                theta = (q[7] * theta - q[1] * -0.57735026918962573) / ct[2];
                arg_0[4] = theta;
                rtb_Product3 = (q[8] * rtb_Product3 - q[2] *
                                -0.57735026918962573) / absx;
                arg_0[5] = rtb_Product3;
                b_oc_idx_0 = arg[2] / arg[2];
                arg_0[6] = b_oc_idx_0;
                od = ct[2] / ct[2];
                arg_0[7] = od;
                absx /= absx;
                arg_0[8] = absx;
                if (det(arg_0) == 0.0) {
                  // :  cycles = -4;
                  idx = -4;
                  guard1 = true;
                  exitg2 = 1;
                } else {
                  // :  T = quaternion(([q1xp1/q1xp1(3); q2xp2/q2xp2(3); q3xp3/q3xp3(3)]\(0.02*theta_err))','rotvec')*T; 
                  arg_0[0] = c_oc;
                  arg_0[1] = b_oc_idx_1;
                  arg_0[2] = st_0;
                  st[0] = 0.02 * rtb_Product1;
                  arg_0[3] = oa_idx_0;
                  arg_0[4] = theta;
                  arg_0[5] = rtb_Product3;
                  st[1] = 0.02 * rtb_Product2;
                  arg_0[6] = b_oc_idx_0;
                  arg_0[7] = od;
                  arg_0[8] = absx;
                  st[2] = 0.02 * b_oc_idx_2;
                  mldivide(arg_0, st, ct);
                  rtb_Product3 = 1.0;
                  absx = 0.0;
                  rtb_Product1 = 0.0;
                  rtb_Product2 = 0.0;
                  theta = std::sqrt((ct[0] * ct[0] + ct[1] * ct[1]) + ct[2] *
                                    ct[2]);
                  st_0 = std::sin(theta / 2.0);
                  if (theta != 0.0) {
                    ct[0] = ct[0] / theta * st_0;
                    ct[1] = ct[1] / theta * st_0;
                    rtb_Product3 = std::cos(theta / 2.0);
                    absx = ct[0];
                    rtb_Product1 = ct[1];
                    rtb_Product2 = ct[2] / theta * st_0;
                  }

                  theta = ((rtb_Product3 * rtDW.T.a - absx * rtDW.T.b) -
                           rtb_Product1 * rtDW.T.c) - rtb_Product2 * rtDW.T.d;
                  st_0 = ((rtb_Product3 * rtDW.T.b + absx * rtDW.T.a) +
                          rtb_Product1 * rtDW.T.d) - rtb_Product2 * rtDW.T.c;
                  c_oc = ((rtb_Product3 * rtDW.T.c - absx * rtDW.T.d) +
                          rtb_Product1 * rtDW.T.a) + rtb_Product2 * rtDW.T.b;
                  rtDW.T.d = ((rtb_Product3 * rtDW.T.d + absx * rtDW.T.c) -
                              rtb_Product1 * rtDW.T.b) + rtb_Product2 * rtDW.T.a;
                  rtDW.T.a = theta;
                  rtDW.T.b = st_0;
                  rtDW.T.c = c_oc;
                  b_cycles++;
                  guard1 = false;
                }
              }
            }
          }
        }
      } else {
        guard1 = true;
        exitg2 = 1;
      }
    } while (exitg2 == 0);

    if (guard1) {
      // :  T = quaternion(1,0,0,0);
      rtDW.T.a = 1.0;
      rtDW.T.b = 0.0;
      rtDW.T.c = 0.0;
      rtDW.T.d = 0.0;

      // :  attitude = compact(T);
      absx = rtDW.T.a;
      theta = rtDW.T.b;
      b_oc_idx_1 = rtDW.T.c;
      c_oc = rtDW.T.d;

      // :  singularity_reg = 1;
      rtDW.singularity_reg = 1.0;

      // :  singularity = singularity_reg;
      rtY.singularity = rtDW.singularity_reg;
    }
  }

  // Sqrt: '<S12>/sqrt' incorporates:
  //   Product: '<S13>/Product'
  //   Product: '<S13>/Product1'
  //   Product: '<S13>/Product2'
  //   Product: '<S13>/Product3'
  //   Sum: '<S13>/Sum'

  rtb_Product3 = std::sqrt(((absx * absx + theta * theta) + b_oc_idx_1 *
    b_oc_idx_1) + c_oc * c_oc);

  // Product: '<S7>/Product'
  absx /= rtb_Product3;

  // Product: '<S7>/Product1'
  rtb_Product1 = theta / rtb_Product3;

  // Product: '<S7>/Product2'
  rtb_Product2 = b_oc_idx_1 / rtb_Product3;

  // Product: '<S7>/Product3'
  rtb_Product3 = c_oc / rtb_Product3;

  // Fcn: '<S4>/fcn2' incorporates:
  //   Fcn: '<S4>/fcn5'

  c_oc = absx * absx;
  b_oc_idx_1 = rtb_Product1 * rtb_Product1;
  st_0 = rtb_Product2 * rtb_Product2;
  oa_idx_0 = rtb_Product3 * rtb_Product3;

  // Fcn: '<S4>/fcn3'
  theta = (rtb_Product1 * rtb_Product3 - absx * rtb_Product2) * -2.0;

  // If: '<S8>/If' incorporates:
  //   Constant: '<S10>/Constant'
  //   Constant: '<S9>/Constant'

  if (theta > 1.0) {
    // Outputs for IfAction SubSystem: '<S8>/If Action Subsystem' incorporates:
    //   ActionPort: '<S9>/Action Port'

    theta = 1.0;

    // End of Outputs for SubSystem: '<S8>/If Action Subsystem'
  } else {
    if (theta < -1.0) {
      // Outputs for IfAction SubSystem: '<S8>/If Action Subsystem1' incorporates:
      //   ActionPort: '<S10>/Action Port'

      theta = 1.0;

      // End of Outputs for SubSystem: '<S8>/If Action Subsystem1'
    }
  }

  // End of If: '<S8>/If'

  // Outport: '<Root>/ypr_meas' incorporates:
  //   Fcn: '<S4>/fcn1'
  //   Fcn: '<S4>/fcn2'
  //   Fcn: '<S4>/fcn4'
  //   Fcn: '<S4>/fcn5'
  //   Gain: '<S2>/Gain3'
  //   Trigonometry: '<S6>/Trigonometric Function1'
  //   Trigonometry: '<S6>/Trigonometric Function3'
  //   Trigonometry: '<S6>/trigFcn'

  rtY.ypr_meas[0] = 57.295779513082323 * rt_atan2d_snf((rtb_Product1 *
    rtb_Product2 + absx * rtb_Product3) * 2.0, ((c_oc + b_oc_idx_1) - st_0) -
    oa_idx_0);
  rtY.ypr_meas[1] = 57.295779513082323 * std::asin(theta);
  rtY.ypr_meas[2] = 57.295779513082323 * rt_atan2d_snf((rtb_Product2 *
    rtb_Product3 + absx * rtb_Product1) * 2.0, ((c_oc - b_oc_idx_1) - st_0) +
    oa_idx_0);

  // Outport: '<Root>/cycles' incorporates:
  //   MATLAB Function: '<S2>/motors2quat'

  rtY.cycles = idx;

  // End of Outputs for SubSystem: '<S1>/motors2ypr'
  // End of Outputs for SubSystem: '<Root>/wrist_decoupler'
}

// Model initialize function
void wrist_decouplerModelClass::initialize()
{
  // Registration code

  // initialize non-finites
  rt_InitInfAndNaN(sizeof(real_T));

  // SystemInitialize for Atomic SubSystem: '<Root>/wrist_decoupler'
  // SystemInitialize for Atomic SubSystem: '<S1>/motors2ypr'
  // SystemInitialize for MATLAB Function: '<S2>/motors2quat'
  // :  T = quaternion(1,0,0,0);
  rtDW.T.a = 1.0;
  rtDW.T.b = 0.0;
  rtDW.T.c = 0.0;
  rtDW.T.d = 0.0;

  // End of SystemInitialize for SubSystem: '<S1>/motors2ypr'
  // End of SystemInitialize for SubSystem: '<Root>/wrist_decoupler'
  // :  singularity_reg = 0;
}

// Constructor
wrist_decouplerModelClass::wrist_decouplerModelClass() :
  rtU(),
  rtY(),
  rtDW(),
  rtM()
{
  // Currently there is no constructor body generated.
}

// Destructor
wrist_decouplerModelClass::~wrist_decouplerModelClass()
{
  // Currently there is no destructor body generated.
}

// Real-Time Model get method
wrist_decouplerModelClass::RT_MODEL * wrist_decouplerModelClass::getRTM()
{
  return (&rtM);
}

//
// File trailer for generated code.
//
// [EOF]
//
