//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: estimation_velocity.cpp
//
// Code generated for Simulink model 'estimation_velocity'.
//
// Model version                  : 7.2
// Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
// C/C++ source code generated on : Fri Aug 23 11:51:51 2024
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
#include "estimation_velocity_private.h"

// Forward declaration for local functions
static real32_T estimation_velocity_xnrm2(int32_T n, const real32_T x[32],
  int32_T ix0);
static void estimation_velocity_xgeqp3(const real32_T A[32], real32_T b_A[32],
  real32_T tau[2], int32_T jpvt[2]);
static void estimation_velocity_xtrsm(int32_T m, const real32_T A[32], real32_T
  B[2]);
static real32_T estimation_velocity_xnrm2_p(int32_T n, const real32_T x[32],
  int32_T ix0);
static real32_T estimation_velocity_xnrm2(int32_T n, const real32_T x[32],
  int32_T ix0)
{
  real32_T y;

  // Start for MATLABSystem: '<S1>/QR Solver'
  y = 0.0F;
  if (n >= 1) {
    if (n == 1) {
      y = std::abs(x[ix0 - 1]);
    } else {
      int32_T kend;
      real32_T scale;
      scale = 1.29246971E-26F;
      kend = ix0 + n;
      for (int32_T k = ix0; k < kend; k++) {
        real32_T absxk;
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

      y = scale * std::sqrt(y);
    }
  }

  // End of Start for MATLABSystem: '<S1>/QR Solver'
  return y;
}

static void estimation_velocity_xgeqp3(const real32_T A[32], real32_T b_A[32],
  real32_T tau[2], int32_T jpvt[2])
{
  int32_T b_A_tmp;
  int32_T b_j;
  int32_T d;
  int32_T exitg1;
  int32_T i;
  int32_T ii;
  int32_T itemp;
  int32_T ix;
  int32_T knt;
  int32_T nmip1;
  int32_T temp_tmp;
  real32_T vn1[2];
  real32_T vn2[2];
  real32_T work[2];
  real32_T absxk;
  real32_T scale;
  real32_T smax;
  real32_T t;
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
    nmip1 = b_j << 4;
    smax = 0.0F;
    scale = 1.29246971E-26F;
    for (itemp = nmip1 + 1; itemp <= nmip1 + 16; itemp++) {
      absxk = std::abs(A[itemp - 1]);
      if (absxk > scale) {
        t = scale / absxk;
        smax = smax * t * t + 1.0F;
        scale = absxk;
      } else {
        t = absxk / scale;
        smax += t * t;
      }
    }

    smax = scale * std::sqrt(smax);
    vn1[b_j] = smax;
    vn2[b_j] = smax;
  }

  for (b_j = 0; b_j < 2; b_j++) {
    i = b_j + 1;
    knt = b_j << 4;
    ii = knt + b_j;
    nmip1 = 2 - b_j;
    ix = 0;
    if (2 - b_j > 1) {
      smax = vn1[b_j];
      for (itemp = 2; itemp <= nmip1; itemp++) {
        scale = vn1[b_j + 1];
        if (scale > smax) {
          ix = 1;
          smax = scale;
        }
      }
    }

    nmip1 = b_j + ix;
    if (nmip1 != b_j) {
      ix = nmip1 << 4;
      for (itemp = 0; itemp < 16; itemp++) {
        temp_tmp = ix + itemp;
        scale = b_A[temp_tmp];
        b_A_tmp = knt + itemp;
        b_A[temp_tmp] = b_A[b_A_tmp];
        b_A[b_A_tmp] = scale;
      }

      itemp = jpvt[nmip1];
      jpvt[nmip1] = jpvt[b_j];
      jpvt[b_j] = itemp;
      vn1[nmip1] = vn1[b_j];
      vn2[nmip1] = vn2[b_j];
    }

    nmip1 = ii + 2;
    smax = b_A[ii];
    tau[b_j] = 0.0F;
    absxk = estimation_velocity_xnrm2(15 - b_j, b_A, ii + 2);
    if (absxk != 0.0F) {
      scale = b_A[ii];
      absxk = rt_hypotf(scale, absxk);
      if (scale >= 0.0F) {
        absxk = -absxk;
      }

      if (std::abs(absxk) < 9.86076132E-32F) {
        knt = -1;
        do {
          knt++;
          ix = (ii - b_j) - 1;
          for (itemp = nmip1; itemp <= ix + 17; itemp++) {
            b_A[itemp - 1] *= 1.01412048E+31F;
          }

          absxk *= 1.01412048E+31F;
          smax *= 1.01412048E+31F;
        } while ((std::abs(absxk) < 9.86076132E-32F) && (knt + 1 < 20));

        absxk = rt_hypotf(smax, estimation_velocity_xnrm2(15 - b_j, b_A, ii + 2));
        if (smax >= 0.0F) {
          absxk = -absxk;
        }

        tau[b_j] = (absxk - smax) / absxk;
        smax = 1.0F / (smax - absxk);
        for (itemp = nmip1; itemp <= ix + 17; itemp++) {
          b_A[itemp - 1] *= smax;
        }

        for (itemp = 0; itemp <= knt; itemp++) {
          absxk *= 9.86076132E-32F;
        }

        smax = absxk;
      } else {
        tau[b_j] = (absxk - scale) / absxk;
        smax = 1.0F / (scale - absxk);
        knt = (ii - b_j) - 1;
        for (itemp = nmip1; itemp <= knt + 17; itemp++) {
          b_A[itemp - 1] *= smax;
        }

        smax = absxk;
      }
    }

    b_A[ii] = smax;
    if (b_j + 1 < 2) {
      b_A[ii] = 1.0F;
      ix = ii + 17;
      if (tau[0] != 0.0F) {
        itemp = 16;
        knt = ii - 1;
        while ((itemp > 0) && (b_A[knt + 16] == 0.0F)) {
          itemp--;
          knt--;
        }

        knt = 1;
        temp_tmp = ii + 17;
        do {
          exitg1 = 0;
          if (temp_tmp <= (ii + itemp) + 16) {
            if (b_A[temp_tmp - 1] != 0.0F) {
              exitg1 = 1;
            } else {
              temp_tmp++;
            }
          } else {
            knt = 0;
            exitg1 = 1;
          }
        } while (exitg1 == 0);

        nmip1 = knt - 1;
      } else {
        itemp = 0;
        nmip1 = -1;
      }

      if (itemp > 0) {
        if (nmip1 + 1 != 0) {
          if (nmip1 >= 0) {
            work[0] = 0.0F;
          }

          knt = ((nmip1 << 4) + ii) + 17;
          for (b_A_tmp = ix; b_A_tmp <= knt; b_A_tmp += 16) {
            scale = 0.0F;
            d = b_A_tmp + itemp;
            for (temp_tmp = b_A_tmp; temp_tmp < d; temp_tmp++) {
              scale += b_A[(ii + temp_tmp) - b_A_tmp] * b_A[temp_tmp - 1];
            }

            temp_tmp = ((b_A_tmp - ii) - 17) >> 4;
            work[temp_tmp] += scale;
          }
        }

        if (-tau[0] != 0.0F) {
          ix = ii;
          for (temp_tmp = 0; temp_tmp <= nmip1; temp_tmp++) {
            if (work[0] != 0.0F) {
              scale = work[0] * -tau[0];
              knt = ix + 17;
              b_A_tmp = (itemp + ix) + 16;
              for (d = knt; d <= b_A_tmp; d++) {
                b_A[d - 1] += b_A[((ii + d) - ix) - 17] * scale;
              }
            }

            ix += 16;
          }
        }
      }

      b_A[ii] = smax;
    }

    for (ii = i + 1; ii < 3; ii++) {
      knt = b_j + 17;
      if (vn1[1] != 0.0F) {
        smax = std::abs(b_A[b_j + 16]) / vn1[1];
        smax = 1.0F - smax * smax;
        if (smax < 0.0F) {
          smax = 0.0F;
        }

        scale = vn1[1] / vn2[1];
        scale = scale * scale * smax;
        if (scale <= 0.000345266977F) {
          smax = 0.0F;
          scale = 1.29246971E-26F;
          for (itemp = knt + 1; itemp < 33; itemp++) {
            absxk = std::abs(b_A[itemp - 1]);
            if (absxk > scale) {
              t = scale / absxk;
              smax = smax * t * t + 1.0F;
              scale = absxk;
            } else {
              t = absxk / scale;
              smax += t * t;
            }
          }

          smax = scale * std::sqrt(smax);
          vn1[1] = smax;
          vn2[1] = smax;
        } else {
          vn1[1] *= std::sqrt(smax);
        }
      }
    }
  }
}

static void estimation_velocity_xtrsm(int32_T m, const real32_T A[32], real32_T
  B[2])
{
  // Start for MATLABSystem: '<S1>/QR Solver'
  for (int32_T k = m; k >= 1; k--) {
    int32_T kAcol;
    real32_T B_0;
    kAcol = ((k - 1) << 4) - 1;
    B_0 = B[k - 1];
    if (B_0 != 0.0F) {
      int32_T b;
      B[k - 1] = B_0 / A[k + kAcol];
      b = k - 2;
      for (int32_T b_i = 0; b_i <= b; b_i++) {
        B[b_i] -= A[(b_i + kAcol) + 1] * B[k - 1];
      }
    }
  }

  // End of Start for MATLABSystem: '<S1>/QR Solver'
}

static real32_T estimation_velocity_xnrm2_p(int32_T n, const real32_T x[32],
  int32_T ix0)
{
  real32_T y;

  // Start for MATLABSystem: '<S1>/QR Solver'
  y = 0.0F;
  if (n >= 1) {
    if (n == 1) {
      y = std::abs(x[ix0 - 1]);
    } else {
      int32_T kend;
      real32_T scale;
      scale = 1.29246971E-26F;
      kend = ((n - 1) << 4) + ix0;
      for (int32_T k = ix0; k <= kend; k += 16) {
        real32_T absxk;
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

      y = scale * std::sqrt(y);
    }
  }

  // End of Start for MATLABSystem: '<S1>/QR Solver'
  return y;
}

// System initialize for referenced model: 'estimation_velocity'
void estimation_velocity_Init(DW_estimation_velocity_f_T *localDW)
{
  // Start for MATLABSystem: '<S1>/QR Solver'
  localDW->objisempty = true;
  localDW->obj.isInitialized = 1;
}

// Output and update for referenced model: 'estimation_velocity'
void estimation_velocity(const EstimationVelocityModes *rtu_EstimationConfig,
  const real32_T *rtu_position, real32_T *rty_EstimatedVelocity,
  DW_estimation_velocity_f_T *localDW)
{
  int32_T jpvt[2];
  int32_T cbeg_tmp;
  int32_T d;
  int32_T i;
  int32_T ia0;
  int32_T k;
  int32_T knt;
  int32_T rank;
  real32_T b_A[32];
  real32_T rtb_DelayLine[16];
  real32_T rtb_QRSolver_0[2];
  real32_T work[2];
  real32_T b_A_0;
  real32_T beta1;
  real32_T wj;

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

  estimation_velocity_xgeqp3(rtCP_Constant_Value_c, b_A, work, jpvt);
  for (rank = 0; rank < 2; rank++) {
    if (work[rank] != 0.0F) {
      wj = rtb_DelayLine[rank];
      for (i = rank + 2; i < 17; i++) {
        wj += b_A[((rank << 4) + i) - 1] * rtb_DelayLine[i - 1];
      }

      wj *= work[rank];
      if (wj != 0.0F) {
        rtb_DelayLine[rank] -= wj;
        for (i = rank + 2; i < 17; i++) {
          rtb_DelayLine[i - 1] -= b_A[((rank << 4) + i) - 1] * wj;
        }
      }
    }
  }

  rank = 0;
  wj = 1.90734863E-6F * std::abs(b_A[0]);
  while ((rank < 2) && (std::abs(b_A[(rank << 4) + rank]) > wj)) {
    rank++;
  }

  rtb_QRSolver_0[0] = 0.0F;
  rtb_QRSolver_0[1] = 0.0F;
  if (rank - 1 >= 0) {
    std::memcpy(&rtb_QRSolver_0[0], &rtb_DelayLine[0], static_cast<uint32_T>
                (rank) * sizeof(real32_T));
  }

  if (rank < 2) {
    work[0] = 0.0F;
    if (rank != 0) {
      for (i = 1; i >= 1; i--) {
        wj = b_A[0];
        work[0] = 0.0F;
        beta1 = estimation_velocity_xnrm2_p(1, b_A, 17);
        if (beta1 != 0.0F) {
          beta1 = rt_hypotf(b_A[0], beta1);
          if (b_A[0] >= 0.0F) {
            beta1 = -beta1;
          }

          if (std::abs(beta1) < 9.86076132E-32F) {
            knt = -1;
            do {
              knt++;
              b_A_0 = b_A[16];
              for (k = 17; k <= 17; k += 16) {
                b_A_0 *= 1.01412048E+31F;
              }

              b_A[16] = b_A_0;
              beta1 *= 1.01412048E+31F;
              wj *= 1.01412048E+31F;
            } while ((std::abs(beta1) < 9.86076132E-32F) && (knt + 1 < 20));

            beta1 = rt_hypotf(wj, estimation_velocity_xnrm2_p(1, b_A, 17));
            if (wj >= 0.0F) {
              beta1 = -beta1;
            }

            work[0] = (beta1 - wj) / beta1;
            wj = 1.0F / (wj - beta1);
            for (k = 17; k <= 17; k += 16) {
              b_A_0 *= wj;
            }

            b_A[16] = b_A_0;
            for (k = 0; k <= knt; k++) {
              beta1 *= 9.86076132E-32F;
            }

            wj = beta1;
          } else {
            work[0] = (beta1 - b_A[0]) / beta1;
            wj = 1.0F / (b_A[0] - beta1);
            b_A_0 = b_A[16];
            for (k = 17; k <= 17; k += 16) {
              b_A_0 *= wj;
            }

            b_A[16] = b_A_0;
            wj = beta1;
          }
        }

        b_A[0] = wj;
      }
    }

    estimation_velocity_xtrsm(rank, b_A, rtb_QRSolver_0);
    for (i = 0; i < rank; i++) {
      ia0 = (rank << 4) + 1;
      if (work[0] != 0.0F) {
        wj = rtb_QRSolver_0[0];
        cbeg_tmp = rank + 1;
        for (k = cbeg_tmp; k <= cbeg_tmp; k += 2) {
          beta1 = 0.0F;
          d = k - rank;
          for (knt = k; knt <= d + 1; knt++) {
            beta1 += b_A[(((knt - k) << 4) + ia0) - 1] * rtb_QRSolver_0[knt - 1];
          }

          wj += beta1;
        }

        if (-work[0] != 0.0F) {
          beta1 = -work[0] * wj;
          rtb_QRSolver_0[0] += beta1;
          if (wj != 0.0F) {
            for (k = cbeg_tmp; k < 3; k++) {
              rtb_QRSolver_0[k - 1] += b_A[((((k - rank) - 1) << 4) + ia0) - 1] *
                beta1;
            }
          }
        }
      }
    }
  } else {
    estimation_velocity_xtrsm(2, b_A, rtb_QRSolver_0);
  }

  work[jpvt[0] - 1] = rtb_QRSolver_0[0];
  work[jpvt[1] - 1] = rtb_QRSolver_0[1];

  // MultiPortSwitch: '<Root>/Index Vector' incorporates:
  //   Constant: '<Root>/Constant'
  //   Gain: '<Root>/Gain'
  //   MATLABSystem: '<S1>/QR Solver'
  //
  switch (*rtu_EstimationConfig) {
   case EstimationVelocityModes_Disabled:
    *rty_EstimatedVelocity = 0.0F;
    break;

   case EstimationVelocityModes_MovingAverage:
    // Sum: '<Root>/Sum' incorporates:
    //   Delay: '<Root>/Delay'

    wj = *rtu_position - localDW->Delay_DSTATE[localDW->CircBufIdx];
    *rty_EstimatedVelocity = 62.5F * wj;
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
