//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: estimation_velocity.cpp
//
// Code generated for Simulink model 'estimation_velocity'.
//
// Model version                  : 3.3
// Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
// C/C++ source code generated on : Tue Aug  9 15:19:01 2022
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
#include "mw_cmsis.h"
#include "rt_hypotf_snf.h"
#include "estimation_velocity_private.h"

extern "C" {

#include "rt_nonfinite.h"

}
  MdlrefDW_estimation_velocity_T estimation_velocity_MdlrefDW;

// Block states (default storage)
DW_estimation_velocity_f_T estimation_velocity_DW;

// Forward declaration for local functions
static real32_T estimation_velocity_xnrm2(int32_T n, const real32_T x[32],
  int32_T ix0);
static void estimation_velocity_xgeqp3(const real32_T A[32], real32_T b_A[32],
  real32_T tau[2], int32_T jpvt[2]);
static real32_T estimation_velocity_xnrm2(int32_T n, const real32_T x[32],
  int32_T ix0)
{
  real32_T absxk;
  real32_T y;
  y = 0.0F;
  if (n < 1) {
  } else if (n == 1) {
    y = std::abs(x[ix0 - 1]);
  } else {
    int32_T kend;
    real32_T scale;
    scale = 1.29246971E-26F;
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

    mw_arm_sqrt_f32(y, &absxk);
    y = scale * absxk;
  }

  return y;
}

static void estimation_velocity_xgeqp3(const real32_T A[32], real32_T b_A[32],
  real32_T tau[2], int32_T jpvt[2])
{
  int32_T ix;
  int32_T lastv;
  real32_T vn1[2];
  real32_T vn2[2];
  real32_T work[2];
  real32_T absxk;
  real32_T b_atmp;
  real32_T scale;
  real32_T t;
  for (int32_T b_k = 0; b_k < 2; b_k++) {
    jpvt[b_k] = b_k + 1;
  }

  tau[0] = 0.0F;
  tau[1] = 0.0F;
  std::memcpy(&b_A[0], &A[0], sizeof(real32_T) << 5U);
  work[0] = 0.0F;
  work[1] = 0.0F;
  for (int32_T b_k = 0; b_k < 2; b_k++) {
    ix = b_k << 4;
    b_atmp = 0.0F;
    scale = 1.29246971E-26F;
    for (lastv = ix + 1; lastv <= ix + 16; lastv++) {
      absxk = std::abs(A[lastv - 1]);
      if (absxk > scale) {
        t = scale / absxk;
        b_atmp = b_atmp * t * t + 1.0F;
        scale = absxk;
      } else {
        t = absxk / scale;
        b_atmp += t * t;
      }
    }

    mw_arm_sqrt_f32(b_atmp, &b_atmp);
    b_atmp *= scale;
    vn2[b_k] = b_atmp;
    vn1[b_k] = b_atmp;
  }

  for (int32_T b_k = 0; b_k < 2; b_k++) {
    int32_T i;
    int32_T jy;
    int32_T kend;
    int32_T kend_tmp;
    int32_T knt;
    int32_T nmip1;
    i = b_k + 1;
    kend_tmp = b_k << 4;
    kend = kend_tmp + b_k;
    nmip1 = 2 - b_k;
    knt = 0;
    if (2 - b_k > 1) {
      scale = vn1[b_k];
      for (lastv = 2; lastv <= nmip1; lastv++) {
        absxk = vn1[b_k + 1];
        if (absxk > scale) {
          knt = 1;
          scale = absxk;
        }
      }
    }

    nmip1 = b_k + knt;
    if (nmip1 + 1 != b_k + 1) {
      ix = nmip1 << 4;
      for (lastv = 0; lastv < 16; lastv++) {
        knt = ix + lastv;
        scale = b_A[knt];
        jy = kend_tmp + lastv;
        b_A[knt] = b_A[jy];
        b_A[jy] = scale;
      }

      ix = jpvt[nmip1];
      jpvt[nmip1] = jpvt[b_k];
      jpvt[b_k] = ix;
      vn1[nmip1] = vn1[b_k];
      vn2[nmip1] = vn2[b_k];
    }

    ix = kend + 2;
    b_atmp = b_A[kend];
    tau[b_k] = 0.0F;
    scale = estimation_velocity_xnrm2(15 - b_k, b_A, kend + 2);
    if (scale != 0.0F) {
      scale = rt_hypotf_snf(b_A[kend], scale);
      if (b_A[kend] >= 0.0F) {
        scale = -scale;
      }

      if (std::abs(scale) < 9.86076132E-32F) {
        knt = -1;
        do {
          knt++;
          nmip1 = (kend - b_k) - 1;
          for (lastv = ix; lastv <= nmip1 + 17; lastv++) {
            b_A[lastv - 1] *= 1.01412048E+31F;
          }

          scale *= 1.01412048E+31F;
          b_atmp *= 1.01412048E+31F;
        } while ((std::abs(scale) < 9.86076132E-32F) && (knt + 1 < 20));

        scale = estimation_velocity_xnrm2(15 - b_k, b_A, kend + 2);
        scale = rt_hypotf_snf(b_atmp, scale);
        if (b_atmp >= 0.0F) {
          scale = -scale;
        }

        tau[b_k] = (scale - b_atmp) / scale;
        b_atmp = 1.0F / (b_atmp - scale);
        nmip1 = (kend - b_k) - 1;
        for (lastv = ix; lastv <= nmip1 + 17; lastv++) {
          b_A[lastv - 1] *= b_atmp;
        }

        for (lastv = 0; lastv <= knt; lastv++) {
          scale *= 9.86076132E-32F;
        }

        b_atmp = scale;
      } else {
        tau[b_k] = (scale - b_A[kend]) / scale;
        b_atmp = 1.0F / (b_A[kend] - scale);
        nmip1 = (kend - b_k) - 1;
        for (lastv = ix; lastv <= nmip1 + 17; lastv++) {
          b_A[lastv - 1] *= b_atmp;
        }

        b_atmp = scale;
      }
    }

    b_A[kend] = b_atmp;
    if (b_k + 1 < 2) {
      b_atmp = b_A[kend];
      b_A[kend] = 1.0F;
      kend_tmp = kend + 17;
      if (tau[0] != 0.0F) {
        boolean_T exitg2;
        lastv = 16;
        ix = kend - 1;
        while ((lastv > 0) && (b_A[ix + 16] == 0.0F)) {
          lastv--;
          ix--;
        }

        knt = 0;
        exitg2 = false;
        while ((!exitg2) && (knt + 1 > 0)) {
          int32_T exitg1;
          ix = (knt << 4) + kend;
          jy = ix + 17;
          do {
            exitg1 = 0;
            if (jy <= (ix + lastv) + 16) {
              if (b_A[jy - 1] != 0.0F) {
                exitg1 = 1;
              } else {
                jy++;
              }
            } else {
              knt--;
              exitg1 = 2;
            }
          } while (exitg1 == 0);

          if (exitg1 == 1) {
            exitg2 = true;
          }
        }
      } else {
        lastv = 0;
        knt = -1;
      }

      if (lastv > 0) {
        int32_T d;
        if (knt + 1 != 0) {
          if (knt >= 0) {
            std::memset(&work[0], 0, (knt + 1) * sizeof(real32_T));
          }

          nmip1 = (knt << 4) + kend;
          for (ix = kend_tmp; ix <= nmip1 + 17; ix += 16) {
            scale = 0.0F;
            d = ix + lastv;
            for (jy = ix; jy < d; jy++) {
              scale += b_A[(kend + jy) - ix] * b_A[jy - 1];
            }

            jy = ((ix - kend) - 17) >> 4;
            work[jy] += scale;
          }
        }

        if (!(-tau[0] == 0.0F)) {
          jy = kend;
          for (kend_tmp = 0; kend_tmp <= knt; kend_tmp++) {
            scale = work[kend_tmp];
            if (scale != 0.0F) {
              scale *= -tau[0];
              nmip1 = jy + 17;
              ix = (lastv + jy) + 16;
              for (d = nmip1; d <= ix; d++) {
                b_A[d - 1] += b_A[((kend + d) - jy) - 17] * scale;
              }
            }

            jy += 16;
          }
        }
      }

      b_A[kend] = b_atmp;
    }

    for (nmip1 = i + 1; nmip1 < 3; nmip1++) {
      absxk = vn2[1];
      b_atmp = vn1[1];
      if (vn1[1] != 0.0F) {
        b_atmp = std::abs(b_A[b_k + 16]) / vn1[1];
        b_atmp = 1.0F - b_atmp * b_atmp;
        if (b_atmp < 0.0F) {
          b_atmp = 0.0F;
        }

        scale = vn1[1] / vn2[1];
        scale = scale * scale * b_atmp;
        if (scale <= 0.000345266977F) {
          ix = b_k + 17;
          b_atmp = 0.0F;
          scale = 1.29246971E-26F;
          for (lastv = ix + 1; lastv < 33; lastv++) {
            absxk = std::abs(b_A[lastv - 1]);
            if (absxk > scale) {
              t = scale / absxk;
              b_atmp = b_atmp * t * t + 1.0F;
              scale = absxk;
            } else {
              t = absxk / scale;
              b_atmp += t * t;
            }
          }

          mw_arm_sqrt_f32(b_atmp, &b_atmp);
          b_atmp *= scale;
          absxk = b_atmp;
        } else {
          mw_arm_sqrt_f32(b_atmp, &b_atmp);
          b_atmp *= vn1[1];
        }
      }

      vn1[1] = b_atmp;
      vn2[1] = absxk;
    }
  }
}

// System initialize for referenced model: 'estimation_velocity'
void estimation_velocity_Init(void)
{
  // Start for MATLABSystem: '<S1>/QR Solver'
  estimation_velocity_DW.objisempty = true;
}

// Output and update for referenced model: 'estimation_velocity'
void estimation_velocity(const SensorsData *rtu_SensorsData, const
  ConfigurationParameters *rtu_ConfigurationParameters, JointVelocities
  *rty_EstimatedVelocity)
{
  int32_T jpvt[2];
  int32_T i;
  int32_T rankA;
  real32_T b_A[32];
  real32_T rtb_DelayLine[16];
  real32_T rtb_QRSolver_0[2];
  real32_T tau[2];
  real32_T tol;

  // S-Function (sdspsreg2): '<Root>/Delay Line'
  for (rankA = 0; rankA < 15 - estimation_velocity_DW.DelayLine_BUFF_OFFSET;
       rankA++) {
    rtb_DelayLine[rankA] =
      estimation_velocity_DW.DelayLine_Buff[estimation_velocity_DW.DelayLine_BUFF_OFFSET
      + rankA];
  }

  for (rankA = 0; rankA < estimation_velocity_DW.DelayLine_BUFF_OFFSET; rankA++)
  {
    rtb_DelayLine[(rankA - estimation_velocity_DW.DelayLine_BUFF_OFFSET) + 15] =
      estimation_velocity_DW.DelayLine_Buff[rankA];
  }

  rtb_DelayLine[15] = rtu_SensorsData->jointpositions.position;

  // MATLABSystem: '<S1>/QR Solver' incorporates:
  //   Constant: '<S1>/Constant'

  estimation_velocity_xgeqp3(rtCP_Constant_Value_c, b_A, tau, jpvt);
  rankA = 0;
  tol = 1.90734863E-5F * std::abs(b_A[0]);
  while ((rankA < 2) && (!(std::abs(b_A[(rankA << 4) + rankA]) <= tol))) {
    rankA++;
  }

  for (int32_T i_0 = 0; i_0 < 2; i_0++) {
    real32_T tau_0;
    tau_0 = tau[i_0];
    rtb_QRSolver_0[i_0] = 0.0F;
    if (tau_0 != 0.0F) {
      tol = rtb_DelayLine[i_0];
      for (i = i_0 + 2; i < 17; i++) {
        tol += b_A[((i_0 << 4) + i) - 1] * rtb_DelayLine[i - 1];
      }

      tol *= tau_0;
      if (tol != 0.0F) {
        rtb_DelayLine[i_0] -= tol;
        for (i = i_0 + 2; i < 17; i++) {
          rtb_DelayLine[i - 1] -= b_A[((i_0 << 4) + i) - 1] * tol;
        }
      }
    }
  }

  for (i = 0; i < rankA; i++) {
    rtb_QRSolver_0[jpvt[i] - 1] = rtb_DelayLine[i];
  }

  for (int32_T i_0 = rankA; i_0 >= 1; i_0--) {
    int32_T rtb_QRSolver_tmp;
    int32_T rtb_QRSolver_tmp_0;
    rtb_QRSolver_tmp = jpvt[i_0 - 1] - 1;
    rtb_QRSolver_tmp_0 = (i_0 - 1) << 4;
    rtb_QRSolver_0[rtb_QRSolver_tmp] /= b_A[(rtb_QRSolver_tmp_0 + i_0) - 1];
    i = i_0 - 2;
    for (int32_T bIndx = 0; bIndx <= i; bIndx++) {
      rtb_QRSolver_0[jpvt[0] - 1] -= rtb_QRSolver_0[rtb_QRSolver_tmp] *
        b_A[rtb_QRSolver_tmp_0];
    }
  }

  // MultiPortSwitch: '<Root>/Index Vector' incorporates:
  //   Constant: '<Root>/Constant'
  //   Delay: '<Root>/Delay'
  //   Gain: '<Root>/Gain'
  //   MATLABSystem: '<S1>/QR Solver'
  //   S-Function (sdspsreg2): '<Root>/Delay Line'
  //   Sum: '<Root>/Sum'

  switch (rtu_ConfigurationParameters->estimationconfig.velocity_mode) {
   case EstimationVelocityModes_Disabled:
    rty_EstimatedVelocity->velocity = 0.0F;
    break;

   case EstimationVelocityModes_MovingAverage:
    rty_EstimatedVelocity->velocity = (rtu_SensorsData->jointpositions.position
      - estimation_velocity_DW.Delay_DSTATE[estimation_velocity_DW.CircBufIdx]) *
      62.5F;
    break;

   default:
    rty_EstimatedVelocity->velocity = rtb_QRSolver_0[0];
    break;
  }

  // End of MultiPortSwitch: '<Root>/Index Vector'

  // Update for S-Function (sdspsreg2): '<Root>/Delay Line'
  estimation_velocity_DW.DelayLine_Buff[estimation_velocity_DW.DelayLine_BUFF_OFFSET]
    = rtu_SensorsData->jointpositions.position;
  estimation_velocity_DW.DelayLine_BUFF_OFFSET++;
  while (estimation_velocity_DW.DelayLine_BUFF_OFFSET >= 15) {
    estimation_velocity_DW.DelayLine_BUFF_OFFSET -= 15;
  }

  // End of Update for S-Function (sdspsreg2): '<Root>/Delay Line'

  // Update for Delay: '<Root>/Delay' incorporates:
  //   S-Function (sdspsreg2): '<Root>/Delay Line'

  estimation_velocity_DW.Delay_DSTATE[estimation_velocity_DW.CircBufIdx] =
    rtu_SensorsData->jointpositions.position;
  if (estimation_velocity_DW.CircBufIdx < 15U) {
    estimation_velocity_DW.CircBufIdx++;
  } else {
    estimation_velocity_DW.CircBufIdx = 0U;
  }

  // End of Update for Delay: '<Root>/Delay'
}

// Model initialize function
void estimation_velocity_initialize(const char_T **rt_errorStatus)
{
  RT_MODEL_estimation_velocity_T *const estimation_velocity_M =
    &(estimation_velocity_MdlrefDW.rtm);

  // Registration code

  // initialize non-finites
  rt_InitInfAndNaN(sizeof(real_T));

  // initialize error status
  rtmSetErrorStatusPointer(estimation_velocity_M, rt_errorStatus);
}

//
// File trailer for generated code.
//
// [EOF]
//
