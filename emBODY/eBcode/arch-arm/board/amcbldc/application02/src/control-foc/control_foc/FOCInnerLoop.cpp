//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: FOCInnerLoop.cpp
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 1.112
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Wed Jul 14 16:38:29 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "FOCInnerLoop.h"

// Include model header file for global data
#include "control_foc.h"
#include "control_foc_private.h"

// Output and update for function-call system: '<Root>/FOC inner loop'
void control_focModelClass::FOCInnerLoop(const real32_T *rtu_Iref, const
  real32_T *rtu_TargetCurrent, const real32_T *rtu_Vref, const boolean_T
  *rtu_Reset, const boolean_T *rtu_Currentctrlen, const boolean_T
  *rtu_Velocityctrlen, const boolean_T *rtu_Outputen, const real32_T
  rtu_Iabc_fbk[3], const real32_T *rtu_MotorConfig, const real32_T
  *rtu_MotorConfig_e, const real32_T *rtu_MotorConfig_i, const real32_T
  *rtu_MotorConfig_ig, const real32_T *rtu_MotorConfig_a, const real32_T
  *rtu_MotorConfig_c, const real32_T *rtu_RotorSpeed, const boolean_T
  rtu_hallABC[3], real32_T rty_Vabc[3])
{
  real32_T rtb_Gain2;
  real32_T rtb_Product1;
  real32_T rtb_Product2;
  real32_T rtb_SinCos_o2;
  real32_T rtb_Sum_g;
  real32_T rtb_Switch1;
  real32_T rtb_algDD_o1_p;
  real32_T rtb_algDD_o2;
  real32_T rtb_algDD_o2_m;
  real32_T rtb_algDD_o2_n;
  real32_T rtb_sum_alpha;
  real32_T rtb_sum_beta;
  int8_T Integrator_PrevResetState_tmp;
  boolean_T rtb_u_GTE_up;

  // Switch: '<S1>/Switch3'
  if (*rtu_Velocityctrlen) {
    rtb_Sum_g = *rtu_Iref;
  } else {
    rtb_Sum_g = *rtu_TargetCurrent;
  }

  // End of Switch: '<S1>/Switch3'

  // Product: '<S1>/Product2'
  rtb_Product2 = rtb_Sum_g * *rtu_MotorConfig;

  // Product: '<S1>/Product1'
  rtb_Product1 = *rtu_RotorSpeed * *rtu_MotorConfig_e;

  // Sum: '<S1>/Add'
  rtb_Product2 += rtb_Product1;

  // Outputs for Atomic SubSystem: '<S1>/Clarke Transform'
  // AlgorithmDescriptorDelegate generated from: '<S2>/a16'
  arm_clarke_f32(rtu_Iabc_fbk[0], rtu_Iabc_fbk[1], &rtb_algDD_o1_p,
                 &rtb_algDD_o2_n);

  // End of Outputs for SubSystem: '<S1>/Clarke Transform'

  // Switch: '<S1>/Switch4' incorporates:
  //   Gain: '<S1>/Multiply'
  //   LookupNDDirect: '<S1>/Direct Lookup Table (n-D)'
  //   S-Function (scominttobit): '<S1>/Bit to Integer Converter'
  //
  //  About '<S1>/Direct Lookup Table (n-D)':
  //   1-dimensional Direct Look-Up returning a Scalar,
  //
  //      Remove protection against out-of-range input in generated code: 'on'

  // Bit to Integer Conversion
  // Input bit order is MSB first
  rtb_sum_alpha = static_cast<real32_T>(rtCP_DirectLookupTablenD_table[
    static_cast<uint8_T>((static_cast<uint32_T>(rtu_hallABC[0]) << 1U |
    rtu_hallABC[1]) << 1U | rtu_hallABC[2])]) * 120.0F * 0.5F;

  // Trigonometry: '<S1>/SinCos'
  rtb_Product1 = arm_sin_f32(rtb_sum_alpha);
  rtb_SinCos_o2 = arm_cos_f32(rtb_sum_alpha);

  // Outputs for Atomic SubSystem: '<S1>/Park Transform'
  // AlgorithmDescriptorDelegate generated from: '<S7>/a16'
  arm_park_f32(rtb_algDD_o1_p, rtb_algDD_o2_n, &rtb_sum_alpha, &rtb_algDD_o2,
               rtb_Product1, rtb_SinCos_o2);

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // Product: '<S48>/PProd Out' incorporates:
  //   Gain: '<S1>/Gain'

  rtb_sum_beta = -rtb_sum_alpha * *rtu_MotorConfig_i;

  // Sum: '<S54>/Sum Fdbk'
  rtb_algDD_o2_n = rtb_sum_beta + control_foc_DW.Integrator_DSTATE;

  // RelationalOperator: '<S36>/u_GTE_up'
  rtb_u_GTE_up = (rtb_algDD_o2_n >= *rtu_MotorConfig_ig);

  // Gain: '<S1>/Gain2'
  rtb_Gain2 = -*rtu_MotorConfig_ig;

  // Switch: '<S36>/Switch' incorporates:
  //   RelationalOperator: '<S36>/u_GT_lo'
  //   Switch: '<S36>/Switch1'

  if (rtb_u_GTE_up) {
    rtb_algDD_o1_p = *rtu_MotorConfig_ig;
  } else if (rtb_algDD_o2_n > rtb_Gain2) {
    // Switch: '<S36>/Switch1'
    rtb_algDD_o1_p = rtb_algDD_o2_n;
  } else {
    rtb_algDD_o1_p = rtb_Gain2;
  }

  // End of Switch: '<S36>/Switch'

  // Sum: '<S36>/Diff'
  rtb_algDD_o1_p = rtb_algDD_o2_n - rtb_algDD_o1_p;

  // Product: '<S40>/IProd Out' incorporates:
  //   Gain: '<S1>/Gain'

  rtb_sum_alpha = -rtb_sum_alpha * *rtu_MotorConfig_a;

  // Signum: '<S33>/SignPreSat'
  if (rtb_algDD_o1_p < 0.0F) {
    // DataTypeConversion: '<S33>/DataTypeConv1'
    rtb_Switch1 = -1.0F;
  } else if (rtb_algDD_o1_p > 0.0F) {
    // DataTypeConversion: '<S33>/DataTypeConv1'
    rtb_Switch1 = 1.0F;
  } else if (rtb_algDD_o1_p == 0.0F) {
    // DataTypeConversion: '<S33>/DataTypeConv1'
    rtb_Switch1 = 0.0F;
  } else {
    // DataTypeConversion: '<S33>/DataTypeConv1'
    rtb_Switch1 = (rtNaNF);
  }

  // End of Signum: '<S33>/SignPreSat'

  // DataTypeConversion: '<S33>/DataTypeConv1'
  if (rtIsNaNF(rtb_Switch1)) {
    rtb_Switch1 = 0.0F;
  } else {
    rtb_Switch1 = std::fmod(rtb_Switch1, 256.0F);
  }

  // Signum: '<S33>/SignPreIntegrator'
  if (rtb_sum_alpha < 0.0F) {
    // DataTypeConversion: '<S33>/DataTypeConv2'
    rtb_algDD_o2_m = -1.0F;
  } else if (rtb_sum_alpha > 0.0F) {
    // DataTypeConversion: '<S33>/DataTypeConv2'
    rtb_algDD_o2_m = 1.0F;
  } else if (rtb_sum_alpha == 0.0F) {
    // DataTypeConversion: '<S33>/DataTypeConv2'
    rtb_algDD_o2_m = 0.0F;
  } else {
    // DataTypeConversion: '<S33>/DataTypeConv2'
    rtb_algDD_o2_m = (rtNaNF);
  }

  // End of Signum: '<S33>/SignPreIntegrator'

  // DataTypeConversion: '<S33>/DataTypeConv2'
  if (rtIsNaNF(rtb_algDD_o2_m)) {
    rtb_algDD_o2_m = 0.0F;
  } else {
    rtb_algDD_o2_m = std::fmod(rtb_algDD_o2_m, 256.0F);
  }

  // Switch: '<S33>/Switch' incorporates:
  //   Constant: '<S33>/Constant1'
  //   DataTypeConversion: '<S33>/DataTypeConv1'
  //   DataTypeConversion: '<S33>/DataTypeConv2'
  //   Gain: '<S33>/ZeroGain'
  //   Logic: '<S33>/AND3'
  //   RelationalOperator: '<S33>/Equal1'
  //   RelationalOperator: '<S33>/NotEqual'

  if ((0.0F * rtb_algDD_o2_n != rtb_algDD_o1_p) && ((rtb_Switch1 < 0.0F ?
        static_cast<int32_T>(static_cast<int8_T>(-static_cast<int8_T>(
           static_cast<uint8_T>(-rtb_Switch1)))) : static_cast<int32_T>(
         static_cast<int8_T>(static_cast<uint8_T>(rtb_Switch1)))) ==
       (rtb_algDD_o2_m < 0.0F ? static_cast<int32_T>(static_cast<int8_T>(-
          static_cast<int8_T>(static_cast<uint8_T>(-rtb_algDD_o2_m)))) :
        static_cast<int32_T>(static_cast<int8_T>(static_cast<uint8_T>
          (rtb_algDD_o2_m)))))) {
    rtb_sum_alpha = 0.0F;
  }

  // End of Switch: '<S33>/Switch'

  // DiscreteIntegrator: '<S43>/Integrator'
  if ((*rtu_Reset) || (control_foc_DW.Integrator_PrevResetState != 0)) {
    control_foc_DW.Integrator_DSTATE = 0.0F;
  }

  // DiscreteIntegrator: '<S43>/Integrator'
  rtb_algDD_o2_n = 1.875E-5F * rtb_sum_alpha + control_foc_DW.Integrator_DSTATE;

  // Sum: '<S53>/Sum'
  rtb_algDD_o1_p = rtb_sum_beta + rtb_algDD_o2_n;

  // RelationalOperator: '<S51>/LowerRelop1'
  rtb_u_GTE_up = (rtb_algDD_o1_p > *rtu_MotorConfig_ig);

  // Switch: '<S51>/Switch2' incorporates:
  //   RelationalOperator: '<S51>/UpperRelop'
  //   Switch: '<S51>/Switch'

  if (rtb_u_GTE_up) {
    rtb_sum_beta = *rtu_MotorConfig_ig;
  } else if (rtb_algDD_o1_p < rtb_Gain2) {
    // Switch: '<S51>/Switch'
    rtb_sum_beta = rtb_Gain2;
  } else {
    rtb_sum_beta = rtb_algDD_o1_p;
  }

  // End of Switch: '<S51>/Switch2'

  // Sum: '<S1>/Sum'
  rtb_Sum_g -= rtb_algDD_o2;

  // Product: '<S98>/PProd Out'
  rtb_algDD_o1_p = rtb_Sum_g * *rtu_MotorConfig_i;

  // Product: '<S90>/IProd Out'
  rtb_Sum_g *= *rtu_MotorConfig_a;

  // Sum: '<S105>/SumI1' incorporates:
  //   Sum: '<S103>/Sum Fdbk'
  //   Sum: '<S104>/SumI3'
  //   UnitDelay: '<S1>/Unit Delay'

  rtb_Sum_g += control_foc_DW.UnitDelay_DSTATE - (rtb_algDD_o1_p +
    control_foc_DW.Integrator_DSTATE_m);

  // DiscreteIntegrator: '<S93>/Integrator'
  if ((*rtu_Reset) || (control_foc_DW.Integrator_PrevResetState_k != 0)) {
    control_foc_DW.Integrator_DSTATE_m = 0.0F;
  }

  // DiscreteIntegrator: '<S93>/Integrator'
  rtb_algDD_o2 = 1.875E-5F * rtb_Sum_g + control_foc_DW.Integrator_DSTATE_m;

  // Sum: '<S1>/Sum2' incorporates:
  //   Sum: '<S102>/Sum'

  rtb_algDD_o1_p = (rtb_algDD_o1_p + rtb_algDD_o2) + rtb_Product2;

  // RelationalOperator: '<S8>/LowerRelop1'
  rtb_u_GTE_up = (rtb_algDD_o1_p > *rtu_MotorConfig_ig);

  // Switch: '<S8>/Switch2' incorporates:
  //   RelationalOperator: '<S8>/UpperRelop'
  //   Switch: '<S8>/Switch'

  if (rtb_u_GTE_up) {
    rtb_algDD_o1_p = *rtu_MotorConfig_ig;
  } else if (rtb_algDD_o1_p < rtb_Gain2) {
    // Switch: '<S8>/Switch'
    rtb_algDD_o1_p = rtb_Gain2;
  }

  // End of Switch: '<S8>/Switch2'

  // Switch: '<S1>/Switch1'
  if (*rtu_Currentctrlen) {
    rtb_Switch1 = rtb_algDD_o1_p;
  } else {
    rtb_Switch1 = *rtu_Vref;
  }

  // End of Switch: '<S1>/Switch1'

  // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform1'
  // AlgorithmDescriptorDelegate generated from: '<S5>/a16'
  arm_inv_park_f32(rtb_sum_beta, rtb_Switch1, &rtb_Gain2, &rtb_algDD_o2_m,
                   rtb_Product1, rtb_SinCos_o2);

  // End of Outputs for SubSystem: '<S1>/Inverse Park Transform1'

  // Switch: '<S1>/Switch2' incorporates:
  //   Constant: '<S1>/Constant1'
  //   Gain: '<S1>/Gain1'

  if (*rtu_Outputen) {
    // Gain: '<S4>/sqrt3_by_two'
    rtb_Product1 = 0.866025388F * rtb_algDD_o2_m;

    // Gain: '<S4>/one_by_two'
    rtb_SinCos_o2 = 0.5F * rtb_Gain2;

    // Sum: '<S4>/add_c'
    rtb_sum_beta = (0.0F - rtb_SinCos_o2) - rtb_Product1;

    // Sum: '<S4>/add_b'
    rtb_SinCos_o2 = rtb_Product1 - rtb_SinCos_o2;

    // MinMax: '<S1>/Min'
    if ((rtb_Gain2 < rtb_SinCos_o2) || rtIsNaNF(rtb_SinCos_o2)) {
      rtb_Product1 = rtb_Gain2;
    } else {
      rtb_Product1 = rtb_SinCos_o2;
    }

    if ((!(rtb_Product1 < rtb_sum_beta)) && (!rtIsNaNF(rtb_sum_beta))) {
      rtb_Product1 = rtb_sum_beta;
    }

    // End of MinMax: '<S1>/Min'

    // Product: '<S1>/Divide' incorporates:
    //   Sum: '<S1>/Sum1'

    rty_Vabc[0] = (rtb_Gain2 - rtb_Product1) / *rtu_MotorConfig_c;
    rty_Vabc[1] = (rtb_SinCos_o2 - rtb_Product1) / *rtu_MotorConfig_c;
    rty_Vabc[2] = (rtb_sum_beta - rtb_Product1) / *rtu_MotorConfig_c;
    rty_Vabc[0] *= 1000.0F;
    rty_Vabc[1] *= 1000.0F;
    rty_Vabc[2] *= 1000.0F;
  } else {
    rty_Vabc[0] = 0.0F;
    rty_Vabc[1] = 0.0F;
    rty_Vabc[2] = 0.0F;
  }

  // End of Switch: '<S1>/Switch2'

  // Update for DiscreteIntegrator: '<S43>/Integrator'
  control_foc_DW.Integrator_DSTATE = 1.875E-5F * rtb_sum_alpha + rtb_algDD_o2_n;
  Integrator_PrevResetState_tmp = static_cast<int8_T>(*rtu_Reset);

  // Update for DiscreteIntegrator: '<S43>/Integrator'
  control_foc_DW.Integrator_PrevResetState = Integrator_PrevResetState_tmp;

  // Update for UnitDelay: '<S1>/Unit Delay' incorporates:
  //   Sum: '<S1>/Sum3'

  control_foc_DW.UnitDelay_DSTATE = rtb_algDD_o1_p - rtb_Product2;

  // Update for DiscreteIntegrator: '<S93>/Integrator'
  control_foc_DW.Integrator_DSTATE_m = 1.875E-5F * rtb_Sum_g + rtb_algDD_o2;
  control_foc_DW.Integrator_PrevResetState_k = Integrator_PrevResetState_tmp;
}

//
// File trailer for generated code.
//
// [EOF]
//
