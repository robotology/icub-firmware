//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: estimation_velocity.cpp
//
// Code generated for Simulink model 'estimation_velocity'.
//
// Model version                  : 6.21
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Wed Mar  6 16:39:21 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "estimation_velocity.h"
#include "estimation_velocity_types.h"
#include "rtwtypes.h"
#include <cmath>
#include <cstring>
#include "rt_hypotf.h"
#include "mw_cmsis.h"
#include "estimation_velocity_private.h"

// Forward declaration for local functions
static real32_T estimation_velocity_xnrm2(int32_T n, const real32_T x[32],
  int32_T ix0);
static void estimation_velocity_xgeqp3(const real32_T A[32], real32_T b_A[32],
  real32_T tau[2], int32_T jpvt[2]);
static void estimation_velocity_mldivide(const real32_T A_data[], const int32_T
  A_size[2], const real32_T B_data[], const int32_T *B_size, real32_T Y_data[],
  int32_T *Y_size);
static real32_T estimation_velocity_xnrm2(int32_T n, const real32_T x[32],
  int32_T ix0)
{
  int32_T kend;
  real32_T absxk;
  real32_T scale;
  real32_T y;
  y = 0.0F;
  scale = 1.29246971E-26F;

  // Start for MATLABSystem: '<S1>/QR Solver'
  kend = ix0 + n;
  for (int32_T k = ix0; k < kend; k++) {
    absxk = std::abs(x[k - 1]);
    if (absxk > scale) {
      real32_T t;
      t = scale / absxk;
      y = y * t * t + 1.0F;
      scale = absxk;
    } else {
      real32_T t;
      t = absxk / scale;
      y += t * t;
    }
  }

  // End of Start for MATLABSystem: '<S1>/QR Solver'
  mw_arm_sqrt_f32(y, &absxk);
  return scale * absxk;
}

static void estimation_velocity_xgeqp3(const real32_T A[32], real32_T b_A[32],
  real32_T tau[2], int32_T jpvt[2])
{
  int32_T b_A_tmp;
  int32_T b_j;
  int32_T exitg1;
  int32_T i;
  int32_T ii;
  int32_T ijA;
  int32_T itemp;
  int32_T ix;
  int32_T jA;
  int32_T pvt;
  real32_T vn1[2];
  real32_T vn2[2];
  real32_T work[2];
  real32_T absxk;
  real32_T beta1;
  real32_T scale;
  real32_T t;
  boolean_T exitg2;
  jpvt[0] = 1;

  // Start for MATLABSystem: '<S1>/QR Solver'
  tau[0] = 0.0F;
  jpvt[1] = 2;

  // Start for MATLABSystem: '<S1>/QR Solver'
  tau[1] = 0.0F;
  std::memcpy(&b_A[0], &A[0], sizeof(real32_T) << 5U);
  work[0] = 0.0F;
  work[1] = 0.0F;
  for (b_j = 0; b_j < 2; b_j++) {
    i = b_j << 4;
    beta1 = 0.0F;
    scale = 1.29246971E-26F;

    // Start for MATLABSystem: '<S1>/QR Solver'
    for (itemp = i + 1; itemp <= i + 16; itemp++) {
      absxk = std::abs(A[itemp - 1]);
      if (absxk > scale) {
        t = scale / absxk;
        beta1 = beta1 * t * t + 1.0F;
        scale = absxk;
      } else {
        t = absxk / scale;
        beta1 += t * t;
      }
    }

    mw_arm_sqrt_f32(beta1, &beta1);
    scale *= beta1;
    vn1[b_j] = scale;
    vn2[b_j] = scale;
  }

  for (b_j = 0; b_j < 2; b_j++) {
    // Start for MATLABSystem: '<S1>/QR Solver'
    i = b_j << 4;
    ii = i + b_j;
    itemp = 1;

    // Start for MATLABSystem: '<S1>/QR Solver'
    if ((2 - b_j > 1) && (vn1[1] > vn1[b_j])) {
      itemp = 2;
    }

    pvt = (b_j + itemp) - 1;
    if (pvt != b_j) {
      // Start for MATLABSystem: '<S1>/QR Solver'
      ix = pvt << 4;
      for (itemp = 0; itemp < 16; itemp++) {
        jA = ix + itemp;
        scale = b_A[jA];
        b_A_tmp = i + itemp;
        b_A[jA] = b_A[b_A_tmp];
        b_A[b_A_tmp] = scale;
      }

      itemp = jpvt[pvt];
      jpvt[pvt] = jpvt[b_j];
      jpvt[b_j] = itemp;
      vn1[pvt] = vn1[b_j];
      vn2[pvt] = vn2[b_j];
    }

    // Start for MATLABSystem: '<S1>/QR Solver'
    i = ii + 2;
    absxk = b_A[ii];
    tau[b_j] = 0.0F;

    // Start for MATLABSystem: '<S1>/QR Solver'
    beta1 = estimation_velocity_xnrm2(15 - b_j, b_A, ii + 2);
    if (beta1 != 0.0F) {
      // Start for MATLABSystem: '<S1>/QR Solver'
      scale = b_A[ii];
      beta1 = rt_hypotf(scale, beta1);
      if (scale >= 0.0F) {
        beta1 = -beta1;
      }

      if (std::abs(beta1) < 9.86076132E-32F) {
        pvt = -1;
        do {
          pvt++;
          ix = ii - b_j;
          for (itemp = i; itemp <= ix + 16; itemp++) {
            b_A[itemp - 1] *= 1.01412048E+31F;
          }

          beta1 *= 1.01412048E+31F;
          absxk *= 1.01412048E+31F;
        } while ((std::abs(beta1) < 9.86076132E-32F) && (pvt + 1 < 20));

        beta1 = rt_hypotf(absxk, estimation_velocity_xnrm2(15 - b_j, b_A, ii + 2));
        if (absxk >= 0.0F) {
          beta1 = -beta1;
        }

        tau[b_j] = (beta1 - absxk) / beta1;
        absxk = 1.0F / (absxk - beta1);
        for (itemp = i; itemp <= ix + 16; itemp++) {
          b_A[itemp - 1] *= absxk;
        }

        for (itemp = 0; itemp <= pvt; itemp++) {
          beta1 *= 9.86076132E-32F;
        }

        absxk = beta1;
      } else {
        tau[b_j] = (beta1 - scale) / beta1;
        absxk = 1.0F / (scale - beta1);
        pvt = ii - b_j;
        for (itemp = i; itemp <= pvt + 16; itemp++) {
          b_A[itemp - 1] *= absxk;
        }

        absxk = beta1;
      }
    }

    b_A[ii] = absxk;

    // Start for MATLABSystem: '<S1>/QR Solver'
    if (b_j + 1 < 2) {
      b_A[ii] = 1.0F;
      pvt = ii + 17;
      if (tau[0] != 0.0F) {
        itemp = 16 - b_j;
        i = ii - b_j;
        while ((itemp > 0) && (b_A[i + 15] == 0.0F)) {
          itemp--;
          i--;
        }

        ix = 1 - b_j;
        exitg2 = false;
        while ((!exitg2) && (ix > 0)) {
          b_A_tmp = ii;
          do {
            exitg1 = 0;
            if (b_A_tmp + 17 <= (ii + itemp) + 16) {
              if (b_A[b_A_tmp + 16] != 0.0F) {
                exitg1 = 1;
              } else {
                b_A_tmp++;
              }
            } else {
              ix = 0;
              exitg1 = 2;
            }
          } while (exitg1 == 0);

          if (exitg1 == 1) {
            exitg2 = true;
          }
        }
      } else {
        itemp = 0;
        ix = 0;
      }

      if (itemp > 0) {
        if (ix != 0) {
          work[0] = 0.0F;
          for (jA = pvt; jA <= pvt; jA += 16) {
            scale = 0.0F;
            i = jA + itemp;
            for (b_A_tmp = jA; b_A_tmp < i; b_A_tmp++) {
              scale += b_A[(ii + b_A_tmp) - jA] * b_A[b_A_tmp - 1];
            }

            i = ((jA - ii) - 17) >> 4;
            work[i] += scale;
          }
        }

        if (-tau[0] != 0.0F) {
          jA = ii;
          pvt = ix - 1;
          for (ix = 0; ix <= pvt; ix++) {
            if (work[0] != 0.0F) {
              scale = work[0] * -tau[0];
              b_A_tmp = jA + 17;
              i = (itemp + jA) + 16;
              for (ijA = b_A_tmp; ijA <= i; ijA++) {
                b_A[ijA - 1] += b_A[((ii + ijA) - jA) - 17] * scale;
              }
            }

            jA += 16;
          }
        }
      }

      b_A[ii] = absxk;
    }

    for (ii = b_j + 2; ii < 3; ii++) {
      if (vn1[1] != 0.0F) {
        absxk = std::abs(b_A[b_j + 16]) / vn1[1];
        absxk = 1.0F - absxk * absxk;
        if (absxk < 0.0F) {
          absxk = 0.0F;
        }

        scale = vn1[1] / vn2[1];
        scale = scale * scale * absxk;
        if (scale <= 0.000345266977F) {
          beta1 = 0.0F;
          scale = 1.29246971E-26F;
          for (itemp = b_j + 18; itemp < 33; itemp++) {
            absxk = std::abs(b_A[itemp - 1]);
            if (absxk > scale) {
              t = scale / absxk;
              beta1 = beta1 * t * t + 1.0F;
              scale = absxk;
            } else {
              t = absxk / scale;
              beta1 += t * t;
            }
          }

          mw_arm_sqrt_f32(beta1, &beta1);
          beta1 *= scale;
          vn1[1] = beta1;
          vn2[1] = beta1;
        } else {
          mw_arm_sqrt_f32(absxk, &beta1);
          vn1[1] *= beta1;
        }
      }
    }
  }
}

static void estimation_velocity_mldivide(const real32_T A_data[], const int32_T
  A_size[2], const real32_T B_data[], const int32_T *B_size, real32_T Y_data[],
  int32_T *Y_size)
{
  // Start for MATLABSystem: '<S1>/QR Solver'
  if ((A_size[0] == 0) || (A_size[1] == 0) || (*B_size == 0)) {
    *Y_size = A_size[1];
    if (A_size[1] - 1 >= 0) {
      Y_data[0] = 0.0F;
    }
  } else {
    *Y_size = *B_size;
    std::memcpy(&Y_data[0], &B_data[0], static_cast<uint32_T>(*B_size) * sizeof
                (real32_T));
    for (int32_T loop_ub = 1; loop_ub >= 1; loop_ub--) {
      if (Y_data[0] != 0.0F) {
        Y_data[0] /= A_data[0];
      }
    }
  }

  // End of Start for MATLABSystem: '<S1>/QR Solver'
}

// System initialize for referenced model: 'estimation_velocity'
void estimation_velocity_Init(DW_estimation_velocity_f_T *localDW)
{
  // Start for MATLABSystem: '<S1>/QR Solver'
  localDW->objisempty = true;
}

// Output and update for referenced model: 'estimation_velocity'
void estimation_velocity(const EstimationVelocityModes *rtu_EstimationConfig,
  const real32_T *rtu_position, real32_T *rty_EstimatedVelocity,
  DW_estimation_velocity_f_T *localDW)
{
  int32_T b_R_size[2];
  int32_T b_jpvt[2];
  int32_T c;
  int32_T d;
  int32_T exitg1;
  int32_T i;
  int32_T i_0;
  int32_T ia;
  int32_T iaii;
  int32_T jA;
  int32_T k;
  int32_T lastc;
  real32_T c_A[32];
  real32_T rtb_DelayLine[16];
  real32_T b_R[4];
  real32_T Qb_data[2];
  real32_T c_A_data[2];
  real32_T c_A_data_0[2];
  real32_T work[2];
  real32_T xi[2];
  real32_T b_R_data;
  real32_T b_tau_data;
  real32_T c_0;
  real32_T tol;

  // S-Function (sdspsreg2): '<Root>/Delay Line'
  for (k = 0; k < 15 - localDW->DelayLine_BUFF_OFFSET; k++) {
    rtb_DelayLine[k] = localDW->DelayLine_Buff[localDW->DelayLine_BUFF_OFFSET +
      k];
  }

  for (k = 0; k < localDW->DelayLine_BUFF_OFFSET; k++) {
    rtb_DelayLine[(k - localDW->DelayLine_BUFF_OFFSET) + 15] =
      localDW->DelayLine_Buff[k];
  }

  rtb_DelayLine[15] = *rtu_position;

  // End of S-Function (sdspsreg2): '<Root>/Delay Line'

  // MATLABSystem: '<S1>/QR Solver' incorporates:
  //   Constant: '<S1>/Constant'

  estimation_velocity_xgeqp3(rtCP_Constant_Value_c, c_A, xi, b_jpvt);
  for (iaii = 0; iaii < 2; iaii++) {
    for (i = 0; i <= iaii; i++) {
      b_R[i + (iaii << 1)] = c_A[(iaii << 4) + i];
    }

    if (iaii + 2 <= 2) {
      b_R[(iaii << 1) + 1] = 0.0F;
    }
  }

  work[0] = 0.0F;
  work[1] = 0.0F;
  for (i = 1; i >= 0; i--) {
    iaii = ((i << 4) + i) + 16;
    if (i + 1 < 2) {
      c_A[iaii - 16] = 1.0F;
      c = iaii + 1;
      if (xi[i] != 0.0F) {
        k = 16;
        i_0 = iaii - 1;
        while ((k > 0) && (c_A[i_0] == 0.0F)) {
          k--;
          i_0--;
        }

        lastc = 1;
        ia = iaii;
        do {
          exitg1 = 0;
          if (ia + 1 <= iaii + k) {
            if (c_A[ia] != 0.0F) {
              exitg1 = 1;
            } else {
              ia++;
            }
          } else {
            lastc = 0;
            exitg1 = 1;
          }
        } while (exitg1 == 0);
      } else {
        k = 0;
        lastc = 0;
      }

      if (k > 0) {
        if (lastc != 0) {
          work[0] = 0.0F;
          for (i_0 = c; i_0 <= iaii + 1; i_0 += 16) {
            c_0 = 0.0F;
            d = i_0 + k;
            for (ia = i_0; ia < d; ia++) {
              c_0 += c_A[((iaii + ia) - i_0) - 16] * c_A[ia - 1];
            }

            d = ((i_0 - iaii) - 1) >> 4;
            work[d] += c_0;
          }
        }

        if (-xi[i] != 0.0F) {
          jA = iaii;
          i_0 = lastc - 1;
          for (lastc = 0; lastc <= i_0; lastc++) {
            if (work[0] != 0.0F) {
              c_0 = work[0] * -xi[i];
              c = jA + 1;
              d = k + jA;
              for (ia = c; ia <= d; ia++) {
                c_A[ia - 1] += c_A[((iaii + ia) - jA) - 17] * c_0;
              }
            }

            jA += 16;
          }
        }
      }
    }

    i_0 = iaii - i;
    for (k = iaii - 14; k <= i_0; k++) {
      c_A[k - 1] *= -xi[i];
    }

    c_A[iaii - 16] = 1.0F - xi[i];
    if (i - 1 >= 0) {
      c_A[iaii - 17] = 0.0F;
    }
  }

  // Start for MATLABSystem: '<S1>/QR Solver'
  c_0 = std::abs(b_R[0]);

  // MATLABSystem: '<S1>/QR Solver' incorporates:
  //   S-Function (sdspsreg2): '<Root>/Delay Line'

  tol = 1.90734863E-6F * c_0;
  k = 0;
  while ((k < 2) && (std::abs(b_R[(k << 1) + k]) > tol)) {
    k++;
  }

  if (k < 1) {
    c = 0;
  } else {
    c = k;
  }

  ia = c;
  i_0 = c - 1;
  for (i = 0; i <= i_0; i++) {
    lastc = (i << 4) - 1;
    tol = 0.0F;
    for (c = 0; c < 16; c++) {
      iaii = (lastc + c) + 1;
      tol += c_A[((iaii / 16) << 4) + iaii % 16] * rtb_DelayLine[c];
    }

    Qb_data[i] = tol;
  }

  if (k == 2) {
    if (std::abs(b_R[1]) > c_0) {
      i = 1;
      iaii = 0;
    } else {
      i = 0;
      iaii = 1;
    }

    tol = b_R[iaii] / b_R[i];
    c_0 = b_R[i + 2];
    tol = (Qb_data[iaii] - Qb_data[i] * tol) / (b_R[iaii + 2] - c_0 * tol);
    xi[1] = tol;
    xi[0] = (Qb_data[i] - tol * c_0) / b_R[i];
  } else {
    c = (k >= 1);
    if (c - 1 >= 0) {
      xi[0] = b_R[0];
      xi[1] = b_R[2];
    }

    k = c;
    i = c << 1;
    if (i - 1 >= 0) {
      std::memcpy(&c_A_data[0], &xi[0], static_cast<uint32_T>(i) * sizeof
                  (real32_T));
    }

    if (c - 1 >= 0) {
      b_tau_data = 0.0F;
    }

    if (c != 0) {
      b_tau_data = 0.0F;
      for (iaii = 0; iaii < 2; iaii++) {
        c_A_data_0[iaii] = xi[iaii];
      }

      for (i = 0; i < 1; i++) {
        for (iaii = 0; iaii < 2; iaii++) {
          c_A_data[iaii] = c_A_data_0[iaii];
        }

        c_0 = c_A_data_0[0];
        b_tau_data = 0.0F;
        tol = std::abs(c_A_data_0[1]);
        if (tol != 0.0F) {
          tol = rt_hypotf(c_A_data_0[0], tol);
          if (c_A_data_0[0] >= 0.0F) {
            tol = -tol;
          }

          if (std::abs(tol) < 9.86076132E-32F) {
            iaii = -1;
            do {
              iaii++;
              c_A_data[1] *= 1.01412048E+31F;
              tol *= 1.01412048E+31F;
              c_0 *= 1.01412048E+31F;
            } while ((std::abs(tol) < 9.86076132E-32F) && (iaii + 1 < 20));

            tol = rt_hypotf(c_0, std::abs(c_A_data[1]));
            if (c_0 >= 0.0F) {
              tol = -tol;
            }

            b_tau_data = (tol - c_0) / tol;
            c_A_data[1] *= 1.0F / (c_0 - tol);
            for (k = 0; k <= iaii; k++) {
              tol *= 9.86076132E-32F;
            }

            c_0 = tol;
          } else {
            b_tau_data = (tol - c_A_data_0[0]) / tol;
            for (iaii = 0; iaii < 2; iaii++) {
              c_A_data[iaii] = c_A_data_0[iaii];
            }

            c_A_data[1] *= 1.0F / (c_A_data_0[0] - tol);
            c_0 = tol;
          }
        }

        for (iaii = 0; iaii < 2; iaii++) {
          c_A_data_0[iaii] = c_A_data[iaii];
        }

        c_A_data_0[0] = c_0;
      }

      k = 1;
      for (iaii = 0; iaii < 2; iaii++) {
        c_A_data[iaii] = c_A_data_0[iaii];
      }
    }

    i = k - 1;
    if (k - 1 >= 0) {
      b_R_data = c_A_data[0];
    }

    c = k << 1;
    if (c - 1 >= 0) {
      std::memcpy(&xi[0], &c_A_data[0], static_cast<uint32_T>(c) * sizeof
                  (real32_T));
    }

    if (k >= 1) {
      for (lastc = k; lastc < 1; lastc++) {
        xi[1] = 0.0F;
      }

      xi[1] *= -b_tau_data;
      xi[0] = 1.0F - b_tau_data;
    }

    if (k - 1 >= 0) {
      c_A_data_0[0] = xi[0];
      c_A_data_0[1] = xi[1];
    }

    b_R_size[0] = k;
    b_R_size[1] = k;
    estimation_velocity_mldivide(&b_R_data, b_R_size, Qb_data, &ia, &b_tau_data,
      &k);
    xi[0] = 0.0F;
    xi[1] = 0.0F;
    for (k = 0; k <= i; k++) {
      xi[0] += c_A_data_0[0] * b_tau_data;
      xi[1] += b_tau_data * c_A_data_0[1];
    }
  }

  work[0] = 0.0F;
  work[b_jpvt[0] - 1] = xi[0];
  work[b_jpvt[1] - 1] = xi[1];

  // MultiPortSwitch: '<Root>/Index Vector' incorporates:
  //   Constant: '<Root>/Constant'
  //   Gain: '<Root>/Gain'
  //   MATLABSystem: '<S1>/QR Solver'

  switch (*rtu_EstimationConfig) {
   case EstimationVelocityModes_Disabled:
    *rty_EstimatedVelocity = 0.0F;
    break;

   case EstimationVelocityModes_MovingAverage:
    // Sum: '<Root>/Sum' incorporates:
    //   Delay: '<Root>/Delay'

    c_0 = *rtu_position - localDW->Delay_DSTATE[localDW->CircBufIdx];
    *rty_EstimatedVelocity = 62.5F * c_0;
    break;

   default:
    *rty_EstimatedVelocity = work[0];
    break;
  }

  // End of MultiPortSwitch: '<Root>/Index Vector'

  // Update for S-Function (sdspsreg2): '<Root>/Delay Line'
  localDW->DelayLine_Buff[localDW->DelayLine_BUFF_OFFSET] = *rtu_position;
  localDW->DelayLine_BUFF_OFFSET++;
  while (localDW->DelayLine_BUFF_OFFSET >= 15) {
    localDW->DelayLine_BUFF_OFFSET -= 15;
  }

  // End of Update for S-Function (sdspsreg2): '<Root>/Delay Line'

  // Update for Delay: '<Root>/Delay'
  localDW->Delay_DSTATE[localDW->CircBufIdx] = *rtu_position;
  if (localDW->CircBufIdx < 15U) {
    localDW->CircBufIdx++;
  } else {
    localDW->CircBufIdx = 0U;
  }

  // End of Update for Delay: '<Root>/Delay'
}

//
// File trailer for generated code.
//
// [EOF]
//
