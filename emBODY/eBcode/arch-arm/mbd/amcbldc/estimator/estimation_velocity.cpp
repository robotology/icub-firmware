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
// Simulink Coder version         : 26.1 (R2026a) 20-Nov-2025
// C/C++ source code generated on : Thu Jul  9 10:00:44 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "estimation_velocity.h"
#include "rtwtypes.h"
#include "estimation_velocity_types.h"
#include <cstring>
#include <cmath>
#include "rt_hypotf.h"
#include "estimation_velocity_private.h"

// Forward declaration for local functions
static real32_T estimation_velocity_xnrm2(int32_T n, const real32_T x[32],
  int32_T ix0);
static void estimation_velocity_xgeqp3_d(real32_T A[32], real32_T tau[2],
  int32_T jpvt[2]);
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
      scale = 1.2924697E-26F;
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

static void estimation_velocity_xgeqp3_d(real32_T A[32], real32_T tau[2],
  int32_T jpvt[2])
{
  int32_T b_k;
  int32_T d;
  int32_T exitg1;
  int32_T i;
  int32_T i_0;
  int32_T ii;
  int32_T itemp;
  int32_T ix;
  int32_T jA;
  int32_T nmip1;
  int32_T temp_tmp;
  real32_T vn1[2];
  real32_T vn2[2];
  real32_T work[2];
  real32_T absxk;
  real32_T scale;
  real32_T smax;
  real32_T t;
  for (b_k = 0; b_k < 2; b_k++) {
    jpvt[b_k] = b_k + 1;

    // Start for MATLABSystem: '<S4>/QR Solver'
    work[b_k] = 0.0F;
    nmip1 = b_k << 4;

    // Start for MATLABSystem: '<S4>/QR Solver'
    smax = 0.0F;
    scale = 1.2924697E-26F;

    // Start for MATLABSystem: '<S4>/QR Solver'
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
    vn1[b_k] = smax;
    vn2[b_k] = smax;
  }

  for (b_k = 0; b_k < 2; b_k++) {
    i = b_k + 1;

    // Start for MATLABSystem: '<S4>/QR Solver'
    i_0 = b_k << 4;
    ii = i_0 + b_k;
    nmip1 = 2 - b_k;

    // Start for MATLABSystem: '<S4>/QR Solver'
    ix = 0;
    if (2 - b_k > 1) {
      smax = vn1[b_k];
      for (itemp = 2; itemp <= nmip1; itemp++) {
        scale = vn1[b_k + 1];
        if (scale > smax) {
          ix = 1;
          smax = scale;
        }
      }
    }

    nmip1 = b_k + ix;
    if (nmip1 != b_k) {
      // Start for MATLABSystem: '<S4>/QR Solver'
      ix = nmip1 << 4;
      for (itemp = 0; itemp < 16; itemp++) {
        temp_tmp = ix + itemp;
        scale = A[temp_tmp];
        jA = i_0 + itemp;
        A[temp_tmp] = A[jA];
        A[jA] = scale;
      }

      itemp = jpvt[nmip1];
      jpvt[nmip1] = jpvt[b_k];
      jpvt[b_k] = itemp;
      vn1[nmip1] = vn1[b_k];
      vn2[nmip1] = vn2[b_k];
    }

    // Start for MATLABSystem: '<S4>/QR Solver'
    nmip1 = ii + 2;
    smax = A[ii];
    tau[b_k] = 0.0F;

    // Start for MATLABSystem: '<S4>/QR Solver'
    scale = estimation_velocity_xnrm2(15 - b_k, A, ii + 2);
    if (scale != 0.0F) {
      // Start for MATLABSystem: '<S4>/QR Solver'
      scale = rt_hypotf(A[ii], scale);
      if (A[ii] >= 0.0F) {
        scale = -scale;
      }

      if (std::abs(scale) < 9.8607613E-32F) {
        ix = -1;
        do {
          ix++;
          i_0 = (ii - b_k) - 1;
          for (itemp = nmip1; itemp <= i_0 + 17; itemp++) {
            A[itemp - 1] *= 1.0141205E+31F;
          }

          scale *= 1.0141205E+31F;
          smax *= 1.0141205E+31F;
        } while ((std::abs(scale) < 9.8607613E-32F) && (ix + 1 < 20));

        scale = rt_hypotf(smax, estimation_velocity_xnrm2(15 - b_k, A, ii + 2));
        if (smax >= 0.0F) {
          scale = -scale;
        }

        tau[b_k] = (scale - smax) / scale;
        smax = 1.0F / (smax - scale);
        for (itemp = nmip1; itemp <= i_0 + 17; itemp++) {
          A[itemp - 1] *= smax;
        }

        for (itemp = 0; itemp <= ix; itemp++) {
          scale *= 9.8607613E-32F;
        }

        smax = scale;
      } else {
        tau[b_k] = (scale - A[ii]) / scale;
        smax = 1.0F / (A[ii] - scale);
        i_0 = (ii - b_k) - 1;
        for (itemp = nmip1; itemp <= i_0 + 17; itemp++) {
          A[itemp - 1] *= smax;
        }

        smax = scale;
      }
    }

    A[ii] = smax;

    // Start for MATLABSystem: '<S4>/QR Solver'
    if (b_k + 1 < 2) {
      smax = A[ii];
      A[ii] = 1.0F;
      ix = ii + 17;
      if (tau[0] != 0.0F) {
        itemp = 16;
        i_0 = ii - 1;
        while ((itemp > 0) && (A[i_0 + 16] == 0.0F)) {
          itemp--;
          i_0--;
        }

        i_0 = 1;
        temp_tmp = ii + 17;
        do {
          exitg1 = 0;
          if (temp_tmp <= (ii + itemp) + 16) {
            if (A[temp_tmp - 1] != 0.0F) {
              exitg1 = 1;
            } else {
              temp_tmp++;
            }
          } else {
            i_0 = 0;
            exitg1 = 1;
          }
        } while (exitg1 == 0);

        nmip1 = i_0 - 1;
      } else {
        itemp = 0;
        nmip1 = -1;
      }

      if (itemp > 0) {
        if (nmip1 + 1 != 0) {
          if (nmip1 >= 0) {
            work[0] = 0.0F;
          }

          i_0 = (nmip1 << 4) + ii;
          for (jA = ix; jA <= i_0 + 17; jA += 16) {
            scale = 0.0F;
            d = jA + itemp;
            for (temp_tmp = jA; temp_tmp < d; temp_tmp++) {
              scale += A[(ii + temp_tmp) - jA] * A[temp_tmp - 1];
            }

            temp_tmp = ((jA - ii) - 17) / 16;
            work[temp_tmp] += scale;
          }
        }

        if (-tau[0] != 0.0F) {
          jA = ii;
          for (ix = 0; ix <= nmip1; ix++) {
            if (work[0] != 0.0F) {
              scale = work[0] * -tau[0];
              i_0 = jA + 17;
              d = (itemp + jA) + 16;
              for (temp_tmp = i_0; temp_tmp <= d; temp_tmp++) {
                A[temp_tmp - 1] += A[((ii + temp_tmp) - jA) - 17] * scale;
              }
            }

            jA += 16;
          }
        }
      }

      A[ii] = smax;
    }

    for (ii = i + 1; ii < 3; ii++) {
      i_0 = b_k + 17;
      if (vn1[1] != 0.0F) {
        // Start for MATLABSystem: '<S4>/QR Solver'
        smax = std::abs(A[b_k + 16]) / vn1[1];
        smax = 1.0F - smax * smax;
        if (smax < 0.0F) {
          // Start for MATLABSystem: '<S4>/QR Solver'
          smax = 0.0F;
        }

        // Start for MATLABSystem: '<S4>/QR Solver'
        scale = vn1[1] / vn2[1];
        if (scale * scale * smax <= 0.00034526698F) {
          smax = 0.0F;
          scale = 1.2924697E-26F;
          for (itemp = i_0 + 1; itemp < 33; itemp++) {
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
      scale = 1.2924697E-26F;
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
  //   Constant: '<S4>/Constant'
  //   MATLABSystem: '<S4>/QR Solver'

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
    std::memcpy(&c_A[0], &rtCP_Constant_Value_c[0], sizeof(real32_T) << 5U);

    // MATLABSystem: '<S4>/QR Solver' incorporates:
    //   Constant: '<S4>/Constant'
    //   S-Function (sdspsreg2): '<S2>/Delay Line'

    estimation_velocity_xgeqp3_d(c_A, b_tauqr, b_jpvt);
    rtb_Delay = 1.9073486E-6F * std::abs(c_A[0]);
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

            if (std::abs(xnorm) < 9.8607613E-32F) {
              knt = -1;
              do {
                knt++;
                c_A_0 = c_A[16];
                for (k = 17; k <= 17; k += 16) {
                  c_A_0 *= 1.0141205E+31F;
                }

                c_A[16] = c_A_0;
                xnorm *= 1.0141205E+31F;
                rtb_Delay *= 1.0141205E+31F;
              } while ((std::abs(xnorm) < 9.8607613E-32F) && (knt + 1 < 20));

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
                xnorm *= 9.8607613E-32F;
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
