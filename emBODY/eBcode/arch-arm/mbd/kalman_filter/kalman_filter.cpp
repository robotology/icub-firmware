//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: kalman_filter.cpp
//
// Code generated for Simulink model 'kalman_filter'.
//
// Model version                  : 1.10
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Mon Apr  4 11:53:10 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "kalman_filter.h"
#include "kalman_filter_private.h"

namespace kalmanfilter
{
  // Model step function
  void KalmanFilter::step(const boolean_T &arg_enable, const real32_T &arg_y1,
    const real32_T (&arg_x0)[3], const real32_T &arg_P0, const real32_T (&arg_Q)
    [3], const real32_T &arg_R, real32_T (&arg_xhat)[3])
  {
    real32_T Abar[9];
    real32_T Abar_0[9];
    real32_T rtb_Product[9];
    real32_T rtb_y_f[9];
    real32_T rtb_y_h[9];
    real32_T rtb_Add_f[3];
    real32_T rtb_M[3];
    real32_T rtb_Product2[3];
    int8_T b_I[9];

    // Outputs for Enabled SubSystem: '<Root>/Kalman Filter' incorporates:
    //   EnablePort: '<S1>/Enable'

    // Inport: '<Root>/enable'
    if (arg_enable) {
      int32_T Abar_tmp;
      int32_T i;
      int32_T i_0;
      int32_T rtb_y_h_tmp;
      real32_T NRinv_idx_1;
      real32_T Product3_idx_2;
      real32_T rtb_Add1;
      real32_T yCov;
      if (!kalman_filter_DW.KalmanFilter_MODE) {
        // InitializeConditions for Delay: '<S2>/MemoryP'
        kalman_filter_DW.icLoad = true;

        // InitializeConditions for Delay: '<S2>/MemoryX'
        kalman_filter_DW.icLoad_g = true;
        kalman_filter_DW.KalmanFilter_MODE = true;
      }

      // Outputs for Atomic SubSystem: '<S2>/ScalarExpansionQ'
      // MATLAB Function: '<S27>/ScalarExpansion' incorporates:
      //   Inport: '<Root>/Q'

      for (i = 0; i < 9; i++) {
        rtb_y_f[i] = 0.0F;
      }

      rtb_y_f[0] = arg_Q[0];
      if (arg_Q[0] <= 0.0F) {
        rtb_y_f[0] = 0.0F;
      }

      rtb_y_f[4] = arg_Q[1];
      if (arg_Q[1] <= 0.0F) {
        rtb_y_f[4] = 0.0F;
      }

      rtb_y_f[8] = arg_Q[2];
      if (arg_Q[2] <= 0.0F) {
        rtb_y_f[8] = 0.0F;
      }

      // End of MATLAB Function: '<S27>/ScalarExpansion'
      // End of Outputs for SubSystem: '<S2>/ScalarExpansionQ'

      // Outputs for Atomic SubSystem: '<S2>/ScalarExpansionR'
      // MATLAB Function: '<S28>/ScalarExpansion' incorporates:
      //   Inport: '<Root>/R'

      yCov = arg_R;
      if (arg_R <= 0.0F) {
        yCov = 1.1920929E-7F;
      }

      // End of Outputs for SubSystem: '<S2>/ScalarExpansionR'

      // Outputs for Atomic SubSystem: '<S2>/ReducedQRN'
      // Product: '<S23>/Product' incorporates:
      //   Math: '<S23>/Transpose1'

      i = 0;
      for (i_0 = 0; i_0 < 3; i_0++) {
        for (Abar_tmp = 0; Abar_tmp < 3; Abar_tmp++) {
          rtb_y_h_tmp = Abar_tmp + i;
          rtb_y_h[rtb_y_h_tmp] = 0.0F;
          rtb_y_h[rtb_y_h_tmp] += kalman_filter_ConstB.Transpose1[i] *
            rtb_y_f[Abar_tmp];
          rtb_y_h[rtb_y_h_tmp] += kalman_filter_ConstB.Transpose1[i + 1] *
            rtb_y_f[Abar_tmp + 3];
          rtb_y_h[rtb_y_h_tmp] += kalman_filter_ConstB.Transpose1[i + 2] *
            rtb_y_f[Abar_tmp + 6];
        }

        i += 3;
      }

      for (i = 0; i < 3; i++) {
        // Sum: '<S23>/Add' incorporates:
        //   Constant: '<S2>/G'
        //   Math: '<S23>/Transpose2'
        //   Product: '<S23>/Product'
        //   Product: '<S23>/Product1'
        //   Sum: '<S29>/Add'

        NRinv_idx_1 = 0.0F;
        i_0 = 0;
        for (Abar_tmp = 0; Abar_tmp < 3; Abar_tmp++) {
          rtb_y_h_tmp = i_0 + i;
          rtb_Product[rtb_y_h_tmp] = 0.0F;
          rtb_Product[rtb_y_h_tmp] += rtb_y_h[i_0] *
            kalman_filter_ConstP.G_Value[i];
          rtb_Product[rtb_y_h_tmp] += rtb_y_h[i_0 + 1] *
            kalman_filter_ConstP.G_Value[i + 3];
          rtb_Product[rtb_y_h_tmp] += rtb_y_h[i_0 + 2] *
            kalman_filter_ConstP.G_Value[i + 6];
          NRinv_idx_1 += rtb_y_f[rtb_y_h_tmp] *
            kalman_filter_ConstB.Transpose2[Abar_tmp];
          i_0 += 3;
        }

        rtb_Add_f[i] = NRinv_idx_1;

        // End of Sum: '<S23>/Add'
      }

      // End of Product: '<S23>/Product'

      // Product: '<S23>/Product3'
      NRinv_idx_1 = 0.0F;
      for (i = 0; i < 3; i++) {
        // Product: '<S23>/Product2' incorporates:
        //   Constant: '<S2>/G'
        //   Sum: '<S29>/Add'

        rtb_Product2[i] = 0.0F;
        rtb_Product2[i] += kalman_filter_ConstP.G_Value[i] * rtb_Add_f[0];
        rtb_Product2[i] += kalman_filter_ConstP.G_Value[i + 3] * rtb_Add_f[1];
        rtb_Product2[i] += kalman_filter_ConstP.G_Value[i + 6] * rtb_Add_f[2];

        // Product: '<S23>/Product3' incorporates:
        //   Constant: '<S2>/G'
        //   Product: '<S23>/Product2'
        //   Sum: '<S29>/Add'

        NRinv_idx_1 += 0.0F * rtb_Add_f[i];
      }

      // Outputs for Atomic SubSystem: '<S2>/ScalarExpansionR'
      // Sum: '<S23>/Add1' incorporates:
      //   MATLAB Function: '<S28>/ScalarExpansion'
      //   Product: '<S23>/Product3'

      rtb_Add1 = (NRinv_idx_1 + kalman_filter_ConstB.Product4) + yCov;

      // End of Outputs for SubSystem: '<S2>/ScalarExpansionR'
      // End of Outputs for SubSystem: '<S2>/ReducedQRN'

      // Outputs for Atomic SubSystem: '<S2>/ScalarExpansionP0'
      // MATLAB Function: '<S26>/ScalarExpansion' incorporates:
      //   Inport: '<Root>/P0'

      yCov = arg_P0;
      for (i = 0; i < 9; i++) {
        rtb_y_f[i] = 0.0F;
      }

      if (arg_P0 <= 0.0F) {
        yCov = 1.1920929E-7F;
      }

      rtb_y_f[0] = yCov;
      rtb_y_f[4] = yCov;
      rtb_y_f[8] = yCov;

      // End of MATLAB Function: '<S26>/ScalarExpansion'
      // End of Outputs for SubSystem: '<S2>/ScalarExpansionP0'

      // Delay: '<S2>/MemoryP'
      if (kalman_filter_DW.icLoad) {
        for (i = 0; i < 9; i++) {
          kalman_filter_DW.MemoryP_DSTATE[i] = rtb_y_f[i];
        }
      }

      // Outputs for Atomic SubSystem: '<S2>/CalculatePL'
      // MATLAB Function: '<S3>/Discrete-Time KF - Calculate PLMZ' incorporates:
      //   Constant: '<S2>/A'
      //   Constant: '<S2>/C'
      //   Delay: '<S2>/MemoryP'
      //   Product: '<S23>/Product'
      //   Product: '<S23>/Product2'

      NRinv_idx_1 = 0.0F;
      i = 0;
      for (i_0 = 0; i_0 < 3; i_0++) {
        rtb_Add_f[i_0] = 0.0F;
        Abar_tmp = 0;
        for (rtb_y_h_tmp = 0; rtb_y_h_tmp < 3; rtb_y_h_tmp++) {
          int32_T tmp;
          rtb_Add_f[i_0] += kalman_filter_DW.MemoryP_DSTATE[rtb_y_h_tmp + i] *
            kalman_filter_ConstP.C_Value[rtb_y_h_tmp];
          tmp = Abar_tmp + i_0;
          rtb_y_h[tmp] = 0.0F;
          rtb_y_h[tmp] += kalman_filter_DW.MemoryP_DSTATE[Abar_tmp] *
            kalman_filter_ConstP.A_Value[i_0];
          rtb_y_h[tmp] += kalman_filter_DW.MemoryP_DSTATE[Abar_tmp + 1] *
            kalman_filter_ConstP.A_Value[i_0 + 3];
          rtb_y_h[tmp] += kalman_filter_DW.MemoryP_DSTATE[Abar_tmp + 2] *
            kalman_filter_ConstP.A_Value[i_0 + 6];
          Abar_tmp += 3;
        }

        NRinv_idx_1 += rtb_Add_f[i_0] * kalman_filter_ConstP.C_Value[i_0];
        i += 3;
      }

      yCov = NRinv_idx_1 + rtb_Add1;
      for (i = 0; i < 3; i++) {
        rtb_Add_f[i] = (((rtb_y_h[i + 3] * 0.0F + rtb_y_h[i]) + rtb_y_h[i + 6] *
                         0.0F) + rtb_Product2[i]) / yCov;
        rtb_M[i] = ((kalman_filter_DW.MemoryP_DSTATE[i + 3] * 0.0F +
                     kalman_filter_DW.MemoryP_DSTATE[i]) +
                    kalman_filter_DW.MemoryP_DSTATE[i + 6] * 0.0F) / yCov;
      }

      for (i = 0; i < 9; i++) {
        b_I[i] = 0;
      }

      b_I[0] = 1;
      b_I[4] = 1;
      b_I[8] = 1;
      i = 0;
      for (i_0 = 0; i_0 < 3; i_0++) {
        Abar[i] = static_cast<real32_T>(b_I[i]) - rtb_M[0] *
          kalman_filter_ConstP.C_Value[i_0];
        Abar[i + 1] = static_cast<real32_T>(b_I[i + 1]) - rtb_M[1] *
          kalman_filter_ConstP.C_Value[i_0];
        Abar[i + 2] = static_cast<real32_T>(b_I[i + 2]) - rtb_M[2] *
          kalman_filter_ConstP.C_Value[i_0];
        i += 3;
      }

      for (i = 0; i < 3; i++) {
        for (i_0 = 0; i_0 < 3; i_0++) {
          Abar_tmp = 3 * i_0 + i;
          Abar_0[Abar_tmp] = 0.0F;
          Abar_0[Abar_tmp] += kalman_filter_DW.MemoryP_DSTATE[3 * i_0] * Abar[i];
          Abar_0[Abar_tmp] += kalman_filter_DW.MemoryP_DSTATE[3 * i_0 + 1] *
            Abar[i + 3];
          Abar_0[Abar_tmp] += kalman_filter_DW.MemoryP_DSTATE[3 * i_0 + 2] *
            Abar[i + 6];
        }

        for (i_0 = 0; i_0 < 3; i_0++) {
          Abar_tmp = 3 * i_0 + i;
          rtb_y_h[Abar_tmp] = 0.0F;
          rtb_y_h[Abar_tmp] += Abar_0[i] * Abar[i_0];
          rtb_y_h[Abar_tmp] += Abar_0[i + 3] * Abar[i_0 + 3];
          rtb_y_h[Abar_tmp] += Abar_0[i + 6] * Abar[i_0 + 6];
        }
      }

      i = 0;
      for (i_0 = 0; i_0 < 3; i_0++) {
        Abar[i] = rtb_M[0] * rtb_Add1 * rtb_M[i_0];
        Abar[i + 1] = rtb_M[1] * rtb_Add1 * rtb_M[i_0];
        Abar[i + 2] = rtb_M[2] * rtb_Add1 * rtb_M[i_0];
        i += 3;
      }

      for (i = 0; i < 9; i++) {
        rtb_y_f[i] = rtb_y_h[i] + Abar[i];
      }

      yCov = rtb_Product2[0] / rtb_Add1;
      NRinv_idx_1 = rtb_Product2[1] / rtb_Add1;
      rtb_Add1 = rtb_Product2[2] / rtb_Add1;
      i = 0;
      for (i_0 = 0; i_0 < 3; i_0++) {
        Abar[i] = kalman_filter_ConstP.A_Value[i] - yCov *
          kalman_filter_ConstP.C_Value[i_0];
        Abar[i + 1] = kalman_filter_ConstP.A_Value[i + 1] - NRinv_idx_1 *
          kalman_filter_ConstP.C_Value[i_0];
        Abar[i + 2] = kalman_filter_ConstP.A_Value[i + 2] - rtb_Add1 *
          kalman_filter_ConstP.C_Value[i_0];
        i += 3;
      }

      for (i = 0; i < 3; i++) {
        for (i_0 = 0; i_0 < 3; i_0++) {
          Abar_tmp = 3 * i_0 + i;
          Abar_0[Abar_tmp] = 0.0F;
          Abar_0[Abar_tmp] += rtb_y_f[3 * i_0] * Abar[i];
          Abar_0[Abar_tmp] += rtb_y_f[3 * i_0 + 1] * Abar[i + 3];
          Abar_0[Abar_tmp] += rtb_y_f[3 * i_0 + 2] * Abar[i + 6];
        }

        for (i_0 = 0; i_0 < 3; i_0++) {
          Abar_tmp = 3 * i_0 + i;
          rtb_y_h[Abar_tmp] = ((Abar_0[i + 3] * Abar[i_0 + 3] + Abar_0[i] *
                                Abar[i_0]) + Abar_0[i + 6] * Abar[i_0 + 6]) +
            rtb_Product[Abar_tmp];
        }
      }

      i = 0;
      for (i_0 = 0; i_0 < 3; i_0++) {
        rtb_Product[i] = yCov * rtb_Product2[i_0];
        rtb_Product[i + 1] = NRinv_idx_1 * rtb_Product2[i_0];
        rtb_Product[i + 2] = rtb_Add1 * rtb_Product2[i_0];
        i += 3;
      }

      for (i = 0; i < 9; i++) {
        rtb_y_f[i] = rtb_y_h[i] - rtb_Product[i];
      }

      // End of MATLAB Function: '<S3>/Discrete-Time KF - Calculate PLMZ'
      // End of Outputs for SubSystem: '<S2>/CalculatePL'

      // Delay: '<S2>/MemoryX' incorporates:
      //   Inport: '<Root>/x0'

      if (kalman_filter_DW.icLoad_g) {
        kalman_filter_DW.MemoryX_DSTATE[0] = arg_x0[0];
        kalman_filter_DW.MemoryX_DSTATE[1] = arg_x0[1];
        kalman_filter_DW.MemoryX_DSTATE[2] = arg_x0[2];
      }

      // Outputs for Enabled SubSystem: '<S22>/MeasurementUpdate' incorporates:
      //   EnablePort: '<S49>/Enable'

      kalman_filter_DW.MeasurementUpdate_MODE = true;

      // Sum: '<S49>/Sum' incorporates:
      //   Constant: '<S2>/C'
      //   Delay: '<S2>/MemoryX'
      //   Inport: '<Root>/y1'
      //   Product: '<S49>/C[k]*xhat[k|k-1]'
      //   Sum: '<S49>/Add1'

      rtb_Add1 = arg_y1 - (((0.0F * kalman_filter_DW.MemoryX_DSTATE[1] +
        kalman_filter_DW.MemoryX_DSTATE[0]) + 0.0F *
                            kalman_filter_DW.MemoryX_DSTATE[2]) +
                           kalman_filter_ConstB.Dkuk);

      // End of Outputs for SubSystem: '<S22>/MeasurementUpdate'

      // Outputs for Enabled SubSystem: '<S29>/Enabled Subsystem' incorporates:
      //   EnablePort: '<S53>/Enable'

      kalman_filter_DW.EnabledSubsystem_MODE = true;

      // End of Outputs for SubSystem: '<S29>/Enabled Subsystem'

      // Outputs for Enabled SubSystem: '<S22>/MeasurementUpdate' incorporates:
      //   EnablePort: '<S49>/Enable'

      // Product: '<S49>/Product3'
      yCov = rtb_Add_f[0] * rtb_Add1;
      NRinv_idx_1 = rtb_Add_f[1] * rtb_Add1;
      Product3_idx_2 = rtb_Add_f[2] * rtb_Add1;

      // End of Outputs for SubSystem: '<S22>/MeasurementUpdate'

      // Outputs for Enabled SubSystem: '<S29>/Enabled Subsystem' incorporates:
      //   EnablePort: '<S53>/Enable'

      // Sum: '<S53>/Add1' incorporates:
      //   Constant: '<S2>/C'
      //   Delay: '<S2>/MemoryX'
      //   Inport: '<Root>/y1'
      //   Product: '<S53>/Product'

      rtb_Add1 = arg_y1 - ((0.0F * kalman_filter_DW.MemoryX_DSTATE[1] +
                            kalman_filter_DW.MemoryX_DSTATE[0]) + 0.0F *
                           kalman_filter_DW.MemoryX_DSTATE[2]);

      // Reshape: '<S2>/Reshapexhat' incorporates:
      //   Delay: '<S2>/MemoryX'
      //   Product: '<S53>/Product2'
      //   Sum: '<S29>/Add'

      kalman_filter_B.Reshapexhat[0] = rtb_M[0] * rtb_Add1 +
        kalman_filter_DW.MemoryX_DSTATE[0];
      kalman_filter_B.Reshapexhat[1] = rtb_M[1] * rtb_Add1 +
        kalman_filter_DW.MemoryX_DSTATE[1];
      kalman_filter_B.Reshapexhat[2] = rtb_M[2] * rtb_Add1 +
        kalman_filter_DW.MemoryX_DSTATE[2];

      // End of Outputs for SubSystem: '<S29>/Enabled Subsystem'

      // Update for Delay: '<S2>/MemoryP'
      kalman_filter_DW.icLoad = false;
      for (i = 0; i < 9; i++) {
        kalman_filter_DW.MemoryP_DSTATE[i] = rtb_y_f[i];
      }

      // End of Update for Delay: '<S2>/MemoryP'

      // Update for Delay: '<S2>/MemoryX'
      kalman_filter_DW.icLoad_g = false;

      // Product: '<S22>/A[k]*xhat[k|k-1]' incorporates:
      //   Constant: '<S2>/A'
      //   Delay: '<S2>/MemoryX'

      for (i = 0; i < 3; i++) {
        rtb_Add_f[i] = 0.0F;
        rtb_Add_f[i] += kalman_filter_ConstP.A_Value[i] *
          kalman_filter_DW.MemoryX_DSTATE[0];
        rtb_Add_f[i] += kalman_filter_ConstP.A_Value[i + 3] *
          kalman_filter_DW.MemoryX_DSTATE[1];
        rtb_Add_f[i] += kalman_filter_ConstP.A_Value[i + 6] *
          kalman_filter_DW.MemoryX_DSTATE[2];
      }

      // End of Product: '<S22>/A[k]*xhat[k|k-1]'

      // Update for Delay: '<S2>/MemoryX' incorporates:
      //   Product: '<S22>/B[k]*u[k]'
      //   Product: '<S49>/Product3'
      //   Sum: '<S22>/Add'

      kalman_filter_DW.MemoryX_DSTATE[0] = (kalman_filter_ConstB.Bkuk[0] +
        rtb_Add_f[0]) + yCov;
      kalman_filter_DW.MemoryX_DSTATE[1] = (kalman_filter_ConstB.Bkuk[1] +
        rtb_Add_f[1]) + NRinv_idx_1;
      kalman_filter_DW.MemoryX_DSTATE[2] = (kalman_filter_ConstB.Bkuk[2] +
        rtb_Add_f[2]) + Product3_idx_2;
    } else if (kalman_filter_DW.KalmanFilter_MODE) {
      // Disable for Enabled SubSystem: '<S22>/MeasurementUpdate'
      kalman_filter_DW.MeasurementUpdate_MODE = false;

      // End of Disable for SubSystem: '<S22>/MeasurementUpdate'

      // Disable for Enabled SubSystem: '<S29>/Enabled Subsystem'
      kalman_filter_DW.EnabledSubsystem_MODE = false;

      // End of Disable for SubSystem: '<S29>/Enabled Subsystem'
      kalman_filter_DW.KalmanFilter_MODE = false;
    }

    // End of Inport: '<Root>/enable'
    // End of Outputs for SubSystem: '<Root>/Kalman Filter'

    // Outport: '<Root>/xhat'
    arg_xhat[0] = kalman_filter_B.Reshapexhat[0];
    arg_xhat[1] = kalman_filter_B.Reshapexhat[1];
    arg_xhat[2] = kalman_filter_B.Reshapexhat[2];
  }

  // Model initialize function
  void KalmanFilter::initialize()
  {
    // SystemInitialize for Enabled SubSystem: '<Root>/Kalman Filter'
    // InitializeConditions for Delay: '<S2>/MemoryP'
    kalman_filter_DW.icLoad = true;

    // InitializeConditions for Delay: '<S2>/MemoryX'
    kalman_filter_DW.icLoad_g = true;

    // End of SystemInitialize for SubSystem: '<Root>/Kalman Filter'
  }

  // Model terminate function
  void KalmanFilter::terminate()
  {
    // (no terminate code required)
  }

  // Constructor
  KalmanFilter::KalmanFilter() :
    kalman_filter_B(),
    kalman_filter_DW(),
    kalman_filter_M()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  KalmanFilter::~KalmanFilter()
  {
    // Currently there is no destructor body generated.
  }

  // Real-Time Model get method
  kalmanfilter::KalmanFilter::RT_MODEL_kalman_filter_T * KalmanFilter::getRTM()
  {
    return (&kalman_filter_M);
  }
}

//
// File trailer for generated code.
//
// [EOF]
//
