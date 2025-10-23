//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: estimation_velocity.cpp
//
// Code generated for Simulink model 'estimation_velocity'.
//
// Model version                  : 8.11
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Thu Oct 16 10:36:40 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "estimation_velocity.h"
#include "rtwtypes.h"
#include "estimation_velocity_types.h"
#include <cmath>
#include "rt_hypotf.h"
#include <cstring>
#include "estimation_velocity_private.h"

// Forward declaration for local functions
static real32_T estimation_velocity_xnrm2(int32_T n, const real32_T x[32],
  int32_T ix0);
static void estimation_velocity_xgeqp3(const real32_T A[32], real32_T b_A[32],
  real32_T tau[2], int32_T jpvt[2]);
static void estimation_velocity_xtrsm(int32_T m, const real32_T A[32], real32_T
  B[2]);
static void estimation_velocity_Pagelsqminnorm_solveLinearSystem(const real32_T
  b[16], const real32_T A[32], const real32_T tauqr[2], const int32_T jpvt[2],
  const real32_T taucod[2], int32_T rank, uint8_T solverToUse, real32_T y[2]);
static real32_T estimation_velocity_xnrm2_p(int32_T n, const real32_T x[32],
  int32_T ix0);
static real32_T estimation_velocity_xnrm2(int32_T n, const real32_T x[32],
  int32_T ix0)
{
  real32_T y;

  // Start for MATLABSystem: '<S4>/QR Solver'
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

  // End of Start for MATLABSystem: '<S4>/QR Solver'
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
  int32_T lastv;
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

  // Start for MATLABSystem: '<S4>/QR Solver'
  tau[0] = 0.0F;
  jpvt[1] = 2;

  // Start for MATLABSystem: '<S4>/QR Solver'
  tau[1] = 0.0F;
  std::memcpy(&b_A[0], &A[0], sizeof(real32_T) << 5U);
  for (b_j = 0; b_j < 2; b_j++) {
    work[b_j] = 0.0F;
    vn1[b_j] = 0.0F;
    vn2[b_j] = 0.0F;
    nmip1 = b_j << 4;
    smax = 0.0F;
    scale = 1.29246971E-26F;
    for (lastv = nmip1 + 1; lastv <= nmip1 + 16; lastv++) {
      absxk = std::abs(A[lastv - 1]);
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
    itemp = b_j << 4;
    ii = itemp + b_j;
    nmip1 = 2 - b_j;
    ix = 0;
    if (2 - b_j > 1) {
      smax = vn1[b_j];
      for (lastv = 2; lastv <= nmip1; lastv++) {
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
      for (lastv = 0; lastv < 16; lastv++) {
        temp_tmp = ix + lastv;
        scale = b_A[temp_tmp];
        b_A_tmp = itemp + lastv;
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
        ix = -1;
        do {
          ix++;
          itemp = (ii - b_j) - 1;
          for (lastv = nmip1; lastv <= itemp + 17; lastv++) {
            b_A[lastv - 1] *= 1.01412048E+31F;
          }

          absxk *= 1.01412048E+31F;
          smax *= 1.01412048E+31F;
        } while ((std::abs(absxk) < 9.86076132E-32F) && (ix + 1 < 20));

        absxk = rt_hypotf(smax, estimation_velocity_xnrm2(15 - b_j, b_A, ii + 2));
        if (smax >= 0.0F) {
          absxk = -absxk;
        }

        tau[b_j] = (absxk - smax) / absxk;
        smax = 1.0F / (smax - absxk);
        for (lastv = nmip1; lastv <= itemp + 17; lastv++) {
          b_A[lastv - 1] *= smax;
        }

        for (lastv = 0; lastv <= ix; lastv++) {
          absxk *= 9.86076132E-32F;
        }

        smax = absxk;
      } else {
        tau[b_j] = (absxk - scale) / absxk;
        smax = 1.0F / (scale - absxk);
        itemp = (ii - b_j) - 1;
        for (lastv = nmip1; lastv <= itemp + 17; lastv++) {
          b_A[lastv - 1] *= smax;
        }

        smax = absxk;
      }
    }

    b_A[ii] = smax;
    if (b_j + 1 < 2) {
      b_A[ii] = 1.0F;
      ix = ii + 17;
      if (tau[0] != 0.0F) {
        lastv = 16;
        itemp = ii - 1;
        while ((lastv > 0) && (b_A[itemp + 16] == 0.0F)) {
          lastv--;
          itemp--;
        }

        itemp = 1;
        temp_tmp = ii + 17;
        do {
          exitg1 = 0;
          if (temp_tmp <= (ii + lastv) + 16) {
            if (b_A[temp_tmp - 1] != 0.0F) {
              exitg1 = 1;
            } else {
              temp_tmp++;
            }
          } else {
            itemp = 0;
            exitg1 = 1;
          }
        } while (exitg1 == 0);

        nmip1 = itemp - 1;
      } else {
        lastv = 0;
        nmip1 = -1;
      }

      if (lastv > 0) {
        if (nmip1 + 1 != 0) {
          if (nmip1 >= 0) {
            work[0] = 0.0F;
          }

          itemp = (nmip1 << 4) + ii;
          for (b_A_tmp = ix; b_A_tmp <= itemp + 17; b_A_tmp += 16) {
            scale = 0.0F;
            d = b_A_tmp + lastv;
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
              itemp = ix + 17;
              b_A_tmp = (lastv + ix) + 16;
              for (d = itemp; d <= b_A_tmp; d++) {
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
      itemp = b_j + 17;
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
          for (lastv = itemp + 1; lastv < 33; lastv++) {
            absxk = std::abs(b_A[lastv - 1]);
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
  // Start for MATLABSystem: '<S4>/QR Solver'
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

  // End of Start for MATLABSystem: '<S4>/QR Solver'
}

static void estimation_velocity_Pagelsqminnorm_solveLinearSystem(const real32_T
  b[16], const real32_T A[32], const real32_T tauqr[2], const int32_T jpvt[2],
  const real32_T taucod[2], int32_T rank, uint8_T solverToUse, real32_T y[2])
{
  int32_T cbeg;
  int32_T d;
  int32_T i;
  int32_T ia;
  int32_T ia0;
  int32_T iac;
  real32_T b_0[16];
  real32_T work[2];
  real32_T c;
  real32_T taucod_0;
  real32_T wj;

  // Start for MATLABSystem: '<S4>/QR Solver'
  std::memcpy(&b_0[0], &b[0], sizeof(real32_T) << 4U);
  for (ia0 = 0; ia0 < 2; ia0++) {
    // Start for MATLABSystem: '<S4>/QR Solver'
    if (tauqr[ia0] != 0.0F) {
      wj = b_0[ia0];
      for (i = ia0 + 2; i < 17; i++) {
        wj += A[((ia0 << 4) + i) - 1] * b_0[i - 1];
      }

      wj *= tauqr[ia0];
      if (wj != 0.0F) {
        b_0[ia0] -= wj;
        for (i = ia0 + 2; i < 17; i++) {
          b_0[i - 1] -= A[((ia0 << 4) + i) - 1] * wj;
        }
      }
    }

    y[ia0] = 0.0F;
  }

  // Start for MATLABSystem: '<S4>/QR Solver'
  if (rank - 1 >= 0) {
    std::memcpy(&y[0], &b_0[0], static_cast<uint32_T>(rank) * sizeof(real32_T));
  }

  if (solverToUse == 2) {
    estimation_velocity_xtrsm(rank, A, y);
    for (i = 0; i < rank; i++) {
      ia0 = ((rank << 4) + i) + 1;
      taucod_0 = taucod[i];
      if (taucod_0 != 0.0F) {
        wj = y[i];
        cbeg = rank + 1;
        if (2 - rank != 0) {
          for (iac = cbeg; iac <= cbeg; iac += 2) {
            c = 0.0F;
            d = iac - rank;
            for (ia = iac; ia <= d + 1; ia++) {
              c += A[(((ia - iac) << 4) + ia0) - 1] * y[ia - 1];
            }

            wj += c;
          }
        }

        if (-taucod[i] != 0.0F) {
          taucod_0 = -taucod_0 * wj;
          y[i] += taucod_0;
          cbeg = rank + 1;
          if (wj != 0.0F) {
            for (iac = cbeg; iac < 3; iac++) {
              y[iac - 1] += A[((((iac - rank) - 1) << 4) + ia0) - 1] * taucod_0;
            }
          }
        }
      }
    }
  } else {
    estimation_velocity_xtrsm(rank, A, y);
  }

  work[jpvt[0] - 1] = y[0];
  work[jpvt[1] - 1] = y[1];
  y[0] = work[0];
  y[1] = work[1];
}

static real32_T estimation_velocity_xnrm2_p(int32_T n, const real32_T x[32],
  int32_T ix0)
{
  real32_T y;

  // Start for MATLABSystem: '<S4>/QR Solver'
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

  // End of Start for MATLABSystem: '<S4>/QR Solver'
  return y;
}

// System initialize for referenced model: 'estimation_velocity'
void estimation_velocity_Init(DW_estimation_velocity_f_T *localDW)
{
  // SystemInitialize for IfAction SubSystem: '<Root>/If Action Subsystem1'
  // Start for MATLABSystem: '<S4>/QR Solver'
  localDW->objisempty = true;
  localDW->obj.isInitialized = 1;

  // End of SystemInitialize for SubSystem: '<Root>/If Action Subsystem1'
}

// Output and update for referenced model: 'estimation_velocity'
void estimation_velocity(const EstimationVelocityModes *rtu_EstimationConfig,
  const real32_T *rtu_position, const uint32_T *rtu_estwindow, real32_T
  *rty_EstimatedVelocity, DW_estimation_velocity_f_T *localDW)
{
  int32_T b_jpvt[2];
  int32_T b_rank;
  int32_T i;
  int32_T k;
  int32_T knt;
  real32_T c_A[32];
  real32_T rtb_DelayLine[16];
  real32_T b_tau[2];
  real32_T b_tauqr[2];
  real32_T tmp[2];
  real32_T c_A_0;
  real32_T rtb_Delay;
  real32_T xnorm;
  uint32_T delayLen;
  uint8_T b_solverToUse;

  // SwitchCase: '<Root>/Switch Case' incorporates:
  //   Constant: '<S1>/Constant'

  switch (*rtu_EstimationConfig) {
   case EstimationVelocityModes_Disabled:
    // Outputs for IfAction SubSystem: '<Root>/If Action Subsystem' incorporates:
    //   ActionPort: '<S1>/Action Port'

    *rty_EstimatedVelocity = 0.0F;

    // End of Outputs for SubSystem: '<Root>/If Action Subsystem'
    break;

   case EstimationVelocityModes_LeastSquares:
    // Outputs for IfAction SubSystem: '<Root>/If Action Subsystem1' incorporates:
    //   ActionPort: '<S2>/Action Port'

    // S-Function (sdspsreg2): '<S2>/Delay Line'
    for (k = 0; k < 15 - localDW->DelayLine_BUFF_OFFSET; k++) {
      rtb_DelayLine[k] = localDW->DelayLine_Buff[localDW->DelayLine_BUFF_OFFSET
        + k];
    }

    for (k = 0; k < localDW->DelayLine_BUFF_OFFSET; k++) {
      rtb_DelayLine[(k - localDW->DelayLine_BUFF_OFFSET) + 15] =
        localDW->DelayLine_Buff[k];
    }

    rtb_DelayLine[15] = *rtu_position;

    // End of S-Function (sdspsreg2): '<S2>/Delay Line'

    // MATLABSystem: '<S4>/QR Solver' incorporates:
    //   Constant: '<S4>/Constant'
    //   S-Function (sdspsreg2): '<S2>/Delay Line'

    estimation_velocity_xgeqp3(rtCP_Constant_Value_c, c_A, b_tauqr, b_jpvt);
    rtb_Delay = 1.90734863E-6F * std::abs(c_A[0]);
    b_rank = 0;
    while ((b_rank < 2) && (std::abs(c_A[(b_rank << 4) + b_rank]) > rtb_Delay))
    {
      b_rank++;
    }

    if (b_rank < 2) {
      b_solverToUse = 2U;
      b_tau[0] = 0.0F;
      b_tau[1] = 0.0F;
      if (b_rank != 0) {
        for (i = 1; i >= 1; i--) {
          rtb_Delay = c_A[0];
          b_tau[0] = 0.0F;
          xnorm = estimation_velocity_xnrm2_p(1, c_A, 17);
          if (xnorm != 0.0F) {
            xnorm = rt_hypotf(c_A[0], xnorm);
            if (c_A[0] >= 0.0F) {
              xnorm = -xnorm;
            }

            if (std::abs(xnorm) < 9.86076132E-32F) {
              knt = -1;
              do {
                knt++;
                c_A_0 = c_A[16];
                for (k = 17; k <= 17; k += 16) {
                  c_A_0 *= 1.01412048E+31F;
                }

                c_A[16] = c_A_0;
                xnorm *= 1.01412048E+31F;
                rtb_Delay *= 1.01412048E+31F;
              } while ((std::abs(xnorm) < 9.86076132E-32F) && (knt + 1 < 20));

              xnorm = rt_hypotf(rtb_Delay, estimation_velocity_xnrm2_p(1, c_A,
                17));
              if (rtb_Delay >= 0.0F) {
                xnorm = -xnorm;
              }

              b_tau[0] = (xnorm - rtb_Delay) / xnorm;
              rtb_Delay = 1.0F / (rtb_Delay - xnorm);
              for (k = 17; k <= 17; k += 16) {
                c_A_0 *= rtb_Delay;
              }

              c_A[16] = c_A_0;
              for (k = 0; k <= knt; k++) {
                xnorm *= 9.86076132E-32F;
              }

              rtb_Delay = xnorm;
            } else {
              b_tau[0] = (xnorm - c_A[0]) / xnorm;
              rtb_Delay = 1.0F / (c_A[0] - xnorm);
              c_A_0 = c_A[16];
              for (k = 17; k <= 17; k += 16) {
                c_A_0 *= rtb_Delay;
              }

              c_A[16] = c_A_0;
              rtb_Delay = xnorm;
            }
          }

          c_A[0] = rtb_Delay;
        }
      }
    } else {
      b_solverToUse = 3U;
    }

    estimation_velocity_Pagelsqminnorm_solveLinearSystem(rtb_DelayLine, c_A,
      b_tauqr, b_jpvt, b_tau, b_rank, b_solverToUse, tmp);

    // End of MATLABSystem: '<S4>/QR Solver'

    // SignalConversion generated from: '<S2>/Out1'
    *rty_EstimatedVelocity = tmp[0];

    // Update for S-Function (sdspsreg2): '<S2>/Delay Line'
    localDW->DelayLine_Buff[localDW->DelayLine_BUFF_OFFSET] = *rtu_position;
    localDW->DelayLine_BUFF_OFFSET++;
    while (localDW->DelayLine_BUFF_OFFSET >= 15) {
      localDW->DelayLine_BUFF_OFFSET -= 15;
    }

    // End of Update for S-Function (sdspsreg2): '<S2>/Delay Line'
    // End of Outputs for SubSystem: '<Root>/If Action Subsystem1'
    break;

   default:
    // Outputs for IfAction SubSystem: '<Root>/If Action Subsystem2' incorporates:
    //   ActionPort: '<S3>/Action Port'

    // Delay: '<S3>/Delay'
    if (*rtu_estwindow <= 0U) {
      rtb_Delay = *rtu_position;
    } else {
      if (*rtu_estwindow > 100U) {
        delayLen = 100U;
      } else {
        delayLen = *rtu_estwindow;
      }

      if (delayLen <= localDW->CircBufIdx) {
        delayLen = localDW->CircBufIdx - delayLen;
      } else {
        delayLen = (localDW->CircBufIdx - delayLen) + 100U;
      }

      rtb_Delay = localDW->Delay_DSTATE[delayLen];
    }

    // End of Delay: '<S3>/Delay'

    // Gain: '<S3>/Gain' incorporates:
    //   DataTypeConversion: '<S3>/Cast To Single'
    //   Product: '<S3>/Divide'
    //   Sum: '<S3>/Sum'

    *rty_EstimatedVelocity = (*rtu_position - rtb_Delay) / static_cast<real32_T>
      (*rtu_estwindow) * 1000.0F;

    // Update for Delay: '<S3>/Delay'
    localDW->Delay_DSTATE[localDW->CircBufIdx] = *rtu_position;
    if (localDW->CircBufIdx < 99U) {
      localDW->CircBufIdx++;
    } else {
      localDW->CircBufIdx = 0U;
    }

    // End of Update for Delay: '<S3>/Delay'
    // End of Outputs for SubSystem: '<Root>/If Action Subsystem2'
    break;
  }

  // End of SwitchCase: '<Root>/Switch Case'
}

//
// File trailer for generated code.
//
// [EOF]
//
