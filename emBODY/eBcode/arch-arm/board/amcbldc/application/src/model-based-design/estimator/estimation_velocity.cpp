//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: estimation_velocity.cpp
//
// Code generated for Simulink model 'estimation_velocity'.
//
// Model version                  : 6.3
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Thu Dec 14 10:44:05 2023
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
#include "rt_hypotf_snf.h"
#include "mw_cmsis.h"
#include "estimation_velocity_private.h"

extern "C"
{

#include "rt_nonfinite.h"

}

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
  int32_T b_k;
  int32_T exitg1;
  int32_T ia;
  int32_T ix;
  int32_T iy;
  int32_T kend;
  int32_T knt;
  int32_T lastv;
  int32_T pvt;
  int32_T scale_tmp;
  real32_T vn1[2];
  real32_T vn2[2];
  real32_T work[2];
  real32_T absxk;
  real32_T scale;
  real32_T smax;
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
  for (b_k = 0; b_k < 2; b_k++) {
    ix = b_k << 4;
    smax = 0.0F;
    scale = 1.29246971E-26F;

    // Start for MATLABSystem: '<S1>/QR Solver'
    for (lastv = ix + 1; lastv <= ix + 16; lastv++) {
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

    mw_arm_sqrt_f32(smax, &smax);
    smax *= scale;
    vn1[b_k] = smax;
    vn2[b_k] = smax;
  }

  for (b_k = 0; b_k < 2; b_k++) {
    // Start for MATLABSystem: '<S1>/QR Solver'
    knt = b_k << 4;
    kend = knt + b_k;
    pvt = 1;

    // Start for MATLABSystem: '<S1>/QR Solver'
    if ((2 - b_k > 1) && (vn1[1] > vn1[b_k])) {
      pvt = 2;
    }

    pvt = (b_k + pvt) - 1;
    if (pvt != b_k) {
      // Start for MATLABSystem: '<S1>/QR Solver'
      ix = pvt << 4;
      for (lastv = 0; lastv < 16; lastv++) {
        scale_tmp = ix + lastv;
        scale = b_A[scale_tmp];
        iy = knt + lastv;
        b_A[scale_tmp] = b_A[iy];
        b_A[iy] = scale;
      }

      ix = jpvt[pvt];
      jpvt[pvt] = jpvt[b_k];
      jpvt[b_k] = ix;
      vn1[pvt] = vn1[b_k];
      vn2[pvt] = vn2[b_k];
    }

    // Start for MATLABSystem: '<S1>/QR Solver'
    ix = kend + 2;
    smax = b_A[kend];
    tau[b_k] = 0.0F;

    // Start for MATLABSystem: '<S1>/QR Solver'
    scale = estimation_velocity_xnrm2(15 - b_k, b_A, kend + 2);
    if (scale != 0.0F) {
      // Start for MATLABSystem: '<S1>/QR Solver'
      absxk = b_A[kend];
      scale = rt_hypotf_snf(absxk, scale);
      if (absxk >= 0.0F) {
        scale = -scale;
      }

      if (std::abs(scale) < 9.86076132E-32F) {
        knt = -1;
        do {
          knt++;
          pvt = kend - b_k;
          for (lastv = ix; lastv <= pvt + 16; lastv++) {
            b_A[lastv - 1] *= 1.01412048E+31F;
          }

          scale *= 1.01412048E+31F;
          smax *= 1.01412048E+31F;
        } while ((std::abs(scale) < 9.86076132E-32F) && (knt + 1 < 20));

        scale = rt_hypotf_snf(smax, estimation_velocity_xnrm2(15 - b_k, b_A,
          kend + 2));
        if (smax >= 0.0F) {
          scale = -scale;
        }

        tau[b_k] = (scale - smax) / scale;
        smax = 1.0F / (smax - scale);
        for (lastv = ix; lastv <= pvt + 16; lastv++) {
          b_A[lastv - 1] *= smax;
        }

        for (lastv = 0; lastv <= knt; lastv++) {
          scale *= 9.86076132E-32F;
        }

        smax = scale;
      } else {
        tau[b_k] = (scale - absxk) / scale;
        smax = 1.0F / (absxk - scale);
        pvt = kend - b_k;
        for (lastv = ix; lastv <= pvt + 16; lastv++) {
          b_A[lastv - 1] *= smax;
        }

        smax = scale;
      }
    }

    b_A[kend] = smax;

    // Start for MATLABSystem: '<S1>/QR Solver'
    if (b_k + 1 < 2) {
      b_A[kend] = 1.0F;
      iy = kend + 17;
      if (tau[0] != 0.0F) {
        lastv = 16 - b_k;
        pvt = kend - b_k;
        while ((lastv > 0) && (b_A[pvt + 15] == 0.0F)) {
          lastv--;
          pvt--;
        }

        ix = 1 - b_k;
        exitg2 = false;
        while ((!exitg2) && (ix > 0)) {
          ia = kend;
          do {
            exitg1 = 0;
            if (ia + 17 <= (kend + lastv) + 16) {
              if (b_A[ia + 16] != 0.0F) {
                exitg1 = 1;
              } else {
                ia++;
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
        lastv = 0;
        ix = 0;
      }

      if (lastv > 0) {
        if (ix != 0) {
          work[0] = 0.0F;
          for (scale_tmp = iy; scale_tmp <= iy; scale_tmp += 16) {
            scale = 0.0F;
            knt = scale_tmp + lastv;
            for (ia = scale_tmp; ia < knt; ia++) {
              scale += b_A[(kend + ia) - scale_tmp] * b_A[ia - 1];
            }

            pvt = ((scale_tmp - kend) - 17) >> 4;
            work[pvt] += scale;
          }
        }

        if (!(-tau[0] == 0.0F)) {
          iy = kend;
          pvt = ix - 1;
          for (ix = 0; ix <= pvt; ix++) {
            if (work[0] != 0.0F) {
              scale = work[0] * -tau[0];
              ia = iy + 17;
              knt = (lastv + iy) + 16;
              for (scale_tmp = ia; scale_tmp <= knt; scale_tmp++) {
                b_A[scale_tmp - 1] += b_A[((kend + scale_tmp) - iy) - 17] *
                  scale;
              }
            }

            iy += 16;
          }
        }
      }

      b_A[kend] = smax;
    }

    for (pvt = b_k + 2; pvt < 3; pvt++) {
      if (vn1[1] != 0.0F) {
        smax = std::abs(b_A[b_k + 16]) / vn1[1];
        smax = 1.0F - smax * smax;
        if (smax < 0.0F) {
          smax = 0.0F;
        }

        scale = vn1[1] / vn2[1];
        scale = scale * scale * smax;
        if (scale <= 0.000345266977F) {
          smax = 0.0F;
          scale = 1.29246971E-26F;
          for (lastv = b_k + 18; lastv < 33; lastv++) {
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

          mw_arm_sqrt_f32(smax, &smax);
          smax *= scale;
          vn1[1] = smax;
          vn2[1] = smax;
        } else {
          mw_arm_sqrt_f32(smax, &smax);
          vn1[1] *= smax;
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
    for (int32_T minmn = 1; minmn >= 1; minmn--) {
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
  // InitializeConditions for S-Function (sdspsreg2): '<Root>/Delay Line'
  for (int32_T i = 0; i < 15; i++) {
    localDW->DelayLine_Buff[i] = 0.0F;
  }

  localDW->DelayLine_BUFF_OFFSET = 0;

  // End of InitializeConditions for S-Function (sdspsreg2): '<Root>/Delay Line' 

  // InitializeConditions for Delay: '<Root>/Delay'
  localDW->CircBufIdx = 0U;

  // Start for MATLABSystem: '<S1>/QR Solver'
  localDW->objisempty = true;
}

// Output and update for referenced model: 'estimation_velocity'
void estimation_velocity(const SensorsData *rtu_SensorsData, const
  ConfigurationParameters *rtu_ConfigurationParameters, JointVelocities
  *rty_EstimatedVelocity, DW_estimation_velocity_f_T *localDW)
{
  int32_T b_R_size[2];
  int32_T b_jpvt[2];
  int32_T bIndx;
  int32_T d;
  int32_T exitg1;
  int32_T ia;
  int32_T iaii;
  int32_T itau;
  int32_T jA;
  int32_T lastc;
  int32_T lastv;
  int32_T yIndx;
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
  real32_T c;
  real32_T tol;

  // S-Function (sdspsreg2): '<Root>/Delay Line'
  for (lastc = 0; lastc < 15 - localDW->DelayLine_BUFF_OFFSET; lastc++) {
    rtb_DelayLine[lastc] = localDW->DelayLine_Buff
      [localDW->DelayLine_BUFF_OFFSET + lastc];
  }

  for (lastc = 0; lastc < localDW->DelayLine_BUFF_OFFSET; lastc++) {
    rtb_DelayLine[(lastc - localDW->DelayLine_BUFF_OFFSET) + 15] =
      localDW->DelayLine_Buff[lastc];
  }

  rtb_DelayLine[15] = rtu_SensorsData->jointpositions.position;

  // MATLABSystem: '<S1>/QR Solver' incorporates:
  //   Constant: '<S1>/Constant'

  estimation_velocity_xgeqp3(rtCP_Constant_Value_c, c_A, xi, b_jpvt);
  for (iaii = 0; iaii < 2; iaii++) {
    for (itau = 0; itau <= iaii; itau++) {
      b_R[itau + (iaii << 1)] = c_A[(iaii << 4) + itau];
    }

    if (iaii + 2 <= 2) {
      b_R[(iaii << 1) + 1] = 0.0F;
    }
  }

  work[0] = 0.0F;
  work[1] = 0.0F;
  for (itau = 1; itau >= 0; itau--) {
    iaii = ((itau << 4) + itau) + 16;
    if (itau + 1 < 2) {
      c_A[iaii - 16] = 1.0F;
      yIndx = iaii + 1;
      if (xi[itau] != 0.0F) {
        lastv = 16;
        bIndx = iaii - 1;
        while ((lastv > 0) && (c_A[bIndx] == 0.0F)) {
          lastv--;
          bIndx--;
        }

        lastc = 1;
        ia = iaii;
        do {
          exitg1 = 0;
          if (ia + 1 <= iaii + lastv) {
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
        lastv = 0;
        lastc = 0;
      }

      if (lastv > 0) {
        if (lastc != 0) {
          work[0] = 0.0F;
          for (jA = yIndx; jA <= iaii + 1; jA += 16) {
            c = 0.0F;
            d = jA + lastv;
            for (ia = jA; ia < d; ia++) {
              c += c_A[((iaii + ia) - jA) - 16] * c_A[ia - 1];
            }

            bIndx = ((jA - iaii) - 1) >> 4;
            work[bIndx] += c;
          }
        }

        if (!(-xi[itau] == 0.0F)) {
          jA = iaii;
          bIndx = lastc - 1;
          for (lastc = 0; lastc <= bIndx; lastc++) {
            if (work[0] != 0.0F) {
              c = work[0] * -xi[itau];
              yIndx = jA + 1;
              d = lastv + jA;
              for (ia = yIndx; ia <= d; ia++) {
                c_A[ia - 1] += c_A[((iaii + ia) - jA) - 17] * c;
              }
            }

            jA += 16;
          }
        }
      }
    }

    bIndx = iaii - itau;
    for (lastc = iaii - 14; lastc <= bIndx; lastc++) {
      c_A[lastc - 1] *= -xi[itau];
    }

    c_A[iaii - 16] = 1.0F - xi[itau];
    if (itau - 1 >= 0) {
      c_A[iaii - 17] = 0.0F;
    }
  }

  // Start for MATLABSystem: '<S1>/QR Solver'
  c = std::abs(b_R[0]);

  // MATLABSystem: '<S1>/QR Solver' incorporates:
  //   S-Function (sdspsreg2): '<Root>/Delay Line'

  tol = 1.90734863E-6F * c;
  lastc = 0;
  while ((lastc < 2) && (!(std::abs(b_R[(lastc << 1) + lastc]) <= tol))) {
    lastc++;
  }

  if (lastc < 1) {
    yIndx = 0;
  } else {
    yIndx = lastc;
  }

  d = yIndx;
  bIndx = yIndx - 1;
  for (itau = 0; itau <= bIndx; itau++) {
    jA = (itau << 4) - 1;
    tol = 0.0F;
    for (iaii = 0; iaii < 16; iaii++) {
      lastv = (jA + iaii) + 1;
      tol += c_A[((lastv / 16) << 4) + lastv % 16] * rtb_DelayLine[iaii];
    }

    Qb_data[itau] = tol;
  }

  if (lastc == 2) {
    if (std::abs(b_R[1]) > c) {
      itau = 1;
      yIndx = 0;
    } else {
      itau = 0;
      yIndx = 1;
    }

    tol = b_R[yIndx] / b_R[itau];
    c = b_R[itau + 2];
    tol = (Qb_data[yIndx] - Qb_data[itau] * tol) / (b_R[yIndx + 2] - c * tol);
    xi[1] = tol;
    xi[0] = (Qb_data[itau] - tol * c) / b_R[itau];
  } else {
    lastc = (lastc >= 1);
    if (lastc - 1 >= 0) {
      xi[0] = b_R[0];
      xi[1] = b_R[2];
    }

    ia = lastc;
    itau = lastc << 1;
    if (itau - 1 >= 0) {
      std::memcpy(&c_A_data[0], &xi[0], static_cast<uint32_T>(itau) * sizeof
                  (real32_T));
    }

    if (lastc - 1 >= 0) {
      b_tau_data = 0.0F;
    }

    if (lastc != 0) {
      b_tau_data = 0.0F;
      for (iaii = 0; iaii < 2; iaii++) {
        c_A_data_0[iaii] = xi[iaii];
      }

      for (itau = 0; itau < 1; itau++) {
        for (iaii = 0; iaii < 2; iaii++) {
          c_A_data[iaii] = c_A_data_0[iaii];
        }

        c = c_A_data_0[0];
        b_tau_data = 0.0F;
        tol = std::abs(c_A_data_0[1]);
        if (tol != 0.0F) {
          tol = rt_hypotf_snf(c_A_data_0[0], tol);
          if (c_A_data_0[0] >= 0.0F) {
            tol = -tol;
          }

          if (std::abs(tol) < 9.86076132E-32F) {
            yIndx = -1;
            do {
              yIndx++;
              c_A_data[1] *= 1.01412048E+31F;
              tol *= 1.01412048E+31F;
              c *= 1.01412048E+31F;
            } while ((std::abs(tol) < 9.86076132E-32F) && (yIndx + 1 < 20));

            tol = rt_hypotf_snf(c, std::abs(c_A_data[1]));
            if (c >= 0.0F) {
              tol = -tol;
            }

            b_tau_data = (tol - c) / tol;
            c_A_data[1] *= 1.0F / (c - tol);
            for (lastc = 0; lastc <= yIndx; lastc++) {
              tol *= 9.86076132E-32F;
            }

            c = tol;
          } else {
            b_tau_data = (tol - c_A_data_0[0]) / tol;
            for (iaii = 0; iaii < 2; iaii++) {
              c_A_data[iaii] = c_A_data_0[iaii];
            }

            c_A_data[1] *= 1.0F / (c_A_data_0[0] - tol);
            c = tol;
          }
        }

        for (iaii = 0; iaii < 2; iaii++) {
          c_A_data_0[iaii] = c_A_data[iaii];
        }

        c_A_data_0[0] = c;
      }

      ia = 1;
      for (iaii = 0; iaii < 2; iaii++) {
        c_A_data[iaii] = c_A_data_0[iaii];
      }
    }

    lastv = ia - 1;
    if (ia - 1 >= 0) {
      b_R_data = c_A_data[0];
    }

    lastc = ia << 1;
    if (lastc - 1 >= 0) {
      std::memcpy(&xi[0], &c_A_data[0], static_cast<uint32_T>(lastc) * sizeof
                  (real32_T));
    }

    if (ia >= 1) {
      for (lastc = ia; lastc < 1; lastc++) {
        xi[1] = 0.0F;
      }

      xi[1] *= -b_tau_data;
      xi[0] = 1.0F - b_tau_data;
    }

    if (ia - 1 >= 0) {
      c_A_data_0[0] = xi[0];
      c_A_data_0[1] = xi[1];
    }

    b_R_size[0] = ia;
    b_R_size[1] = ia;
    estimation_velocity_mldivide(&b_R_data, b_R_size, Qb_data, &d, &b_tau_data,
      &lastc);
    xi[0] = 0.0F;
    xi[1] = 0.0F;
    for (lastc = 0; lastc <= lastv; lastc++) {
      xi[0] += c_A_data_0[0] * b_tau_data;
      xi[1] += b_tau_data * c_A_data_0[1];
    }
  }

  work[0] = 0.0F;
  work[b_jpvt[0] - 1] = xi[0];
  work[b_jpvt[1] - 1] = xi[1];

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
      - localDW->Delay_DSTATE[localDW->CircBufIdx]) * 62.5F;
    break;

   default:
    rty_EstimatedVelocity->velocity = work[0];
    break;
  }

  // End of MultiPortSwitch: '<Root>/Index Vector'

  // Update for S-Function (sdspsreg2): '<Root>/Delay Line'
  localDW->DelayLine_Buff[localDW->DelayLine_BUFF_OFFSET] =
    rtu_SensorsData->jointpositions.position;
  localDW->DelayLine_BUFF_OFFSET++;
  while (localDW->DelayLine_BUFF_OFFSET >= 15) {
    localDW->DelayLine_BUFF_OFFSET -= 15;
  }

  // End of Update for S-Function (sdspsreg2): '<Root>/Delay Line'

  // Update for Delay: '<Root>/Delay' incorporates:
  //   S-Function (sdspsreg2): '<Root>/Delay Line'

  localDW->Delay_DSTATE[localDW->CircBufIdx] =
    rtu_SensorsData->jointpositions.position;
  if (localDW->CircBufIdx < 15U) {
    localDW->CircBufIdx++;
  } else {
    localDW->CircBufIdx = 0U;
  }

  // End of Update for Delay: '<Root>/Delay'
}

// Model initialize function
void estimation_velocity_initialize(const char_T **rt_errorStatus,
  RT_MODEL_estimation_velocity_T *const estimation_velocity_M,
  DW_estimation_velocity_f_T *localDW)
{
  // Registration code

  // initialize non-finites
  rt_InitInfAndNaN(sizeof(real_T));

  // initialize error status
  rtmSetErrorStatusPointer(estimation_velocity_M, rt_errorStatus);

  // states (dwork)
  (void) std::memset(static_cast<void *>(localDW), 0,
                     sizeof(DW_estimation_velocity_f_T));
}

//
// File trailer for generated code.
//
// [EOF]
//
