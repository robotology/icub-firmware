//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: FOCInnerLoop.cpp
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 1.128
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Mon Sep 20 12:43:59 2021
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
  *rtu_RotorAngle, const real32_T rtu_Iabc_fbk[3], const real32_T
  *rtu_MotorConfig, const real32_T *rtu_MotorConfig_e, const real32_T
  *rtu_MotorConfig_i, const real32_T *rtu_MotorConfig_ig, const real32_T
  *rtu_MotorConfig_a, const real32_T *rtu_MotorConfig_c, const real32_T
  *rtu_RotorSpeed, uint16_T rty_Vabc[3], real32_T *rty_Iq_fbk)
{
  int32_T i;
  real32_T rtb_IaIbIc0[2];
  real32_T rtb_Gain2;
  real32_T rtb_Product1;
  real32_T rtb_Product2;
  real32_T rtb_SumFdbk;
  real32_T rtb_Switch2;
  real32_T rtb_Unary_Minus;
  real32_T rtb_algDD_o1_p;
  real32_T rtb_algDD_o2_n;
  real32_T rtb_deg2rad;
  real32_T rtb_sum_beta;
  real32_T tmp;
  int8_T Integrator_PrevResetState_tmp;
  boolean_T rtb_u_GTE_up;

  // Switch: '<S1>/Switch3'
  if (*rtu_Velocityctrlen) {
    rtb_Unary_Minus = *rtu_Iref;
  } else {
    rtb_Unary_Minus = *rtu_TargetCurrent;
  }

  // End of Switch: '<S1>/Switch3'

  // Product: '<S1>/Product2'
  rtb_Product2 = rtb_Unary_Minus * *rtu_MotorConfig;

  // Product: '<S1>/Product1'
  rtb_Product1 = *rtu_RotorSpeed * *rtu_MotorConfig_e;

  // Sum: '<S1>/Add'
  rtb_Product2 += rtb_Product1;

  // Gain: '<S1>/Ia+Ib+Ic=0'
  for (i = 0; i < 2; i++) {
    rtb_IaIbIc0[i] = 0.0F;
    rtb_IaIbIc0[i] += rtCP_IaIbIc0_Gain[i] * rtu_Iabc_fbk[0];
    rtb_IaIbIc0[i] += rtCP_IaIbIc0_Gain[i + 2] * rtu_Iabc_fbk[1];
    rtb_IaIbIc0[i] += rtCP_IaIbIc0_Gain[i + 4] * rtu_Iabc_fbk[2];
  }

  // End of Gain: '<S1>/Ia+Ib+Ic=0'

  // Outputs for Atomic SubSystem: '<S1>/Clarke Transform'
  // AlgorithmDescriptorDelegate generated from: '<S2>/a16'
  arm_clarke_f32(rtb_IaIbIc0[0], rtb_IaIbIc0[1], &rtb_algDD_o1_p,
                 &rtb_algDD_o2_n);

  // End of Outputs for SubSystem: '<S1>/Clarke Transform'

  // Gain: '<S1>/deg2rad'
  rtb_deg2rad = 0.0174532924F * *rtu_RotorAngle;

  // Trigonometry: '<S1>/SinCos'
  rtb_Product1 = arm_sin_f32(rtb_deg2rad);
  rtb_deg2rad = arm_cos_f32(rtb_deg2rad);

  // Outputs for Atomic SubSystem: '<S1>/Park Transform'
  // Switch: '<S110>/Switch' incorporates:
  //   Product: '<S7>/asin'
  //   Product: '<S7>/bcos'
  //   Sum: '<S7>/sum_Qs'
  //   UnaryMinus: '<S110>/Unary_Minus'

  rtb_IaIbIc0[0] = -(rtb_algDD_o2_n * rtb_deg2rad - rtb_algDD_o1_p *
                     rtb_Product1);

  // AlgorithmDescriptorDelegate generated from: '<S7>/a16' incorporates:
  //   Product: '<S7>/acos'
  //   Product: '<S7>/bsin'
  //   Sum: '<S7>/sum_Ds'

  *rty_Iq_fbk = rtb_algDD_o1_p * rtb_deg2rad + rtb_algDD_o2_n * rtb_Product1;

  // Product: '<S48>/PProd Out' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S7>/a16'
  //   Gain: '<S1>/Gain'

  rtb_sum_beta = -rtb_IaIbIc0[0] * *rtu_MotorConfig_i;

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // Sum: '<S54>/Sum Fdbk'
  rtb_SumFdbk = rtb_sum_beta + control_foc_DW.Integrator_DSTATE;

  // RelationalOperator: '<S36>/u_GTE_up'
  rtb_u_GTE_up = (rtb_SumFdbk >= *rtu_MotorConfig_ig);

  // Gain: '<S1>/Gain2'
  rtb_Gain2 = -*rtu_MotorConfig_ig;

  // Switch: '<S36>/Switch' incorporates:
  //   RelationalOperator: '<S36>/u_GT_lo'
  //   Switch: '<S36>/Switch1'

  if (rtb_u_GTE_up) {
    rtb_algDD_o1_p = *rtu_MotorConfig_ig;
  } else if (rtb_SumFdbk > rtb_Gain2) {
    // Switch: '<S36>/Switch1'
    rtb_algDD_o1_p = rtb_SumFdbk;
  } else {
    rtb_algDD_o1_p = rtb_Gain2;
  }

  // End of Switch: '<S36>/Switch'

  // Sum: '<S36>/Diff'
  rtb_algDD_o1_p = rtb_SumFdbk - rtb_algDD_o1_p;

  // Outputs for Atomic SubSystem: '<S1>/Park Transform'
  // Product: '<S40>/IProd Out' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S7>/a16'
  //   Gain: '<S1>/Gain'

  rtb_algDD_o2_n = -rtb_IaIbIc0[0] * *rtu_MotorConfig_a;

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // Signum: '<S33>/SignPreSat'
  if (rtb_algDD_o1_p < 0.0F) {
    // DataTypeConversion: '<S33>/DataTypeConv1'
    rtb_Switch2 = -1.0F;
  } else if (rtb_algDD_o1_p > 0.0F) {
    // DataTypeConversion: '<S33>/DataTypeConv1'
    rtb_Switch2 = 1.0F;
  } else if (rtb_algDD_o1_p == 0.0F) {
    // DataTypeConversion: '<S33>/DataTypeConv1'
    rtb_Switch2 = 0.0F;
  } else {
    // DataTypeConversion: '<S33>/DataTypeConv1'
    rtb_Switch2 = (rtNaNF);
  }

  // End of Signum: '<S33>/SignPreSat'

  // DataTypeConversion: '<S33>/DataTypeConv1'
  if (rtIsNaNF(rtb_Switch2)) {
    rtb_Switch2 = 0.0F;
  } else {
    rtb_Switch2 = std::fmod(rtb_Switch2, 256.0F);
  }

  // Signum: '<S33>/SignPreIntegrator'
  if (rtb_algDD_o2_n < 0.0F) {
    // DataTypeConversion: '<S33>/DataTypeConv2'
    tmp = -1.0F;
  } else if (rtb_algDD_o2_n > 0.0F) {
    // DataTypeConversion: '<S33>/DataTypeConv2'
    tmp = 1.0F;
  } else if (rtb_algDD_o2_n == 0.0F) {
    // DataTypeConversion: '<S33>/DataTypeConv2'
    tmp = 0.0F;
  } else {
    // DataTypeConversion: '<S33>/DataTypeConv2'
    tmp = (rtNaNF);
  }

  // End of Signum: '<S33>/SignPreIntegrator'

  // DataTypeConversion: '<S33>/DataTypeConv2'
  if (rtIsNaNF(tmp)) {
    tmp = 0.0F;
  } else {
    tmp = std::fmod(tmp, 256.0F);
  }

  // Switch: '<S33>/Switch' incorporates:
  //   Constant: '<S33>/Constant1'
  //   DataTypeConversion: '<S33>/DataTypeConv1'
  //   DataTypeConversion: '<S33>/DataTypeConv2'
  //   Gain: '<S33>/ZeroGain'
  //   Logic: '<S33>/AND3'
  //   RelationalOperator: '<S33>/Equal1'
  //   RelationalOperator: '<S33>/NotEqual'

  if ((0.0F * rtb_SumFdbk != rtb_algDD_o1_p) && ((rtb_Switch2 < 0.0F ?
        static_cast<int32_T>(static_cast<int8_T>(-static_cast<int8_T>(
           static_cast<uint8_T>(-rtb_Switch2)))) : static_cast<int32_T>(
         static_cast<int8_T>(static_cast<uint8_T>(rtb_Switch2)))) == (tmp < 0.0F
        ? static_cast<int32_T>(static_cast<int8_T>(-static_cast<int8_T>(
           static_cast<uint8_T>(-tmp)))) : static_cast<int32_T>
        (static_cast<int8_T>(static_cast<uint8_T>(tmp)))))) {
    rtb_algDD_o2_n = 0.0F;
  }

  // End of Switch: '<S33>/Switch'

  // DiscreteIntegrator: '<S43>/Integrator'
  if ((*rtu_Reset) || (control_foc_DW.Integrator_PrevResetState != 0)) {
    control_foc_DW.Integrator_DSTATE = 0.0F;
  }

  // DiscreteIntegrator: '<S43>/Integrator'
  rtb_SumFdbk = 1.78571427E-5F * rtb_algDD_o2_n +
    control_foc_DW.Integrator_DSTATE;

  // Sum: '<S53>/Sum'
  rtb_algDD_o1_p = rtb_sum_beta + rtb_SumFdbk;

  // RelationalOperator: '<S51>/LowerRelop1'
  rtb_u_GTE_up = (rtb_algDD_o1_p > *rtu_MotorConfig_ig);

  // Switch: '<S51>/Switch2' incorporates:
  //   RelationalOperator: '<S51>/UpperRelop'
  //   Switch: '<S51>/Switch'

  if (rtb_u_GTE_up) {
    rtb_Switch2 = *rtu_MotorConfig_ig;
  } else if (rtb_algDD_o1_p < rtb_Gain2) {
    // Switch: '<S51>/Switch'
    rtb_Switch2 = rtb_Gain2;
  } else {
    rtb_Switch2 = rtb_algDD_o1_p;
  }

  // End of Switch: '<S51>/Switch2'

  // Sum: '<S1>/Sum'
  rtb_Unary_Minus -= *rty_Iq_fbk;

  // Product: '<S98>/PProd Out'
  rtb_algDD_o1_p = rtb_Unary_Minus * *rtu_MotorConfig_i;

  // Product: '<S90>/IProd Out'
  rtb_Unary_Minus *= *rtu_MotorConfig_a;

  // Sum: '<S105>/SumI1' incorporates:
  //   Sum: '<S103>/Sum Fdbk'
  //   Sum: '<S104>/SumI3'
  //   UnitDelay: '<S1>/Unit Delay'

  rtb_Unary_Minus += control_foc_DW.UnitDelay_DSTATE - (rtb_algDD_o1_p +
    control_foc_DW.Integrator_DSTATE_m);

  // DiscreteIntegrator: '<S93>/Integrator'
  if ((*rtu_Reset) || (control_foc_DW.Integrator_PrevResetState_k != 0)) {
    control_foc_DW.Integrator_DSTATE_m = 0.0F;
  }

  // DiscreteIntegrator: '<S93>/Integrator'
  rtb_sum_beta = 1.78571427E-5F * rtb_Unary_Minus +
    control_foc_DW.Integrator_DSTATE_m;

  // Sum: '<S1>/Sum2' incorporates:
  //   Sum: '<S102>/Sum'

  rtb_algDD_o1_p = (rtb_algDD_o1_p + rtb_sum_beta) + rtb_Product2;

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
    rtb_Gain2 = rtb_algDD_o1_p;
  } else {
    rtb_Gain2 = *rtu_Vref;
  }

  // End of Switch: '<S1>/Switch1'

  // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
  // Switch: '<S61>/Switch' incorporates:
  //   Product: '<S5>/dsin'
  //   Product: '<S5>/qcos'
  //   Sum: '<S5>/sum_beta'

  rtb_IaIbIc0[0] = rtb_Gain2 * rtb_deg2rad + rtb_Switch2 * rtb_Product1;

  // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'

  // Switch: '<S1>/Switch2' incorporates:
  //   Constant: '<S1>/Constant1'
  //   Gain: '<S1>/Gain1'

  if (*rtu_Outputen) {
    // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
    // Gain: '<S4>/sqrt3_by_two' incorporates:
    //   Product: '<S5>/dcos'
    //   Product: '<S5>/qsin'
    //   Sum: '<S5>/sum_alpha'
    //   UnaryMinus: '<S61>/Unary_Minus'

    rtb_Product1 = -(rtb_Switch2 * rtb_deg2rad - rtb_Gain2 * rtb_Product1) *
      0.866025388F;

    // Gain: '<S4>/one_by_two' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S5>/a16'

    rtb_deg2rad = 0.5F * rtb_IaIbIc0[0];

    // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'

    // Sum: '<S4>/add_c'
    rtb_Gain2 = (0.0F - rtb_deg2rad) - rtb_Product1;

    // Sum: '<S4>/add_b'
    rtb_deg2rad = rtb_Product1 - rtb_deg2rad;

    // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
    // MinMax: '<S1>/Min' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S5>/a16'

    if ((rtb_IaIbIc0[0] < rtb_deg2rad) || rtIsNaNF(rtb_deg2rad)) {
      rtb_Product1 = rtb_IaIbIc0[0];
    } else {
      rtb_Product1 = rtb_deg2rad;
    }

    // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'
    if ((!(rtb_Product1 < rtb_Gain2)) && (!rtIsNaNF(rtb_Gain2))) {
      rtb_Product1 = rtb_Gain2;
    }

    // End of MinMax: '<S1>/Min'

    // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
    // Product: '<S1>/Divide' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S5>/a16'
    //   Sum: '<S1>/Sum1'

    rtb_Switch2 = (rtb_IaIbIc0[0] - rtb_Product1) / *rtu_MotorConfig_c;

    // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'
    rtb_deg2rad = (rtb_deg2rad - rtb_Product1) / *rtu_MotorConfig_c;
    rtb_Product1 = (rtb_Gain2 - rtb_Product1) / *rtu_MotorConfig_c;

    // Gain: '<S1>/Gain1'
    rtb_Switch2 = std::floor(16383.0F * rtb_Switch2);
    if (rtIsNaNF(rtb_Switch2) || rtIsInfF(rtb_Switch2)) {
      rtb_Switch2 = 0.0F;
    } else {
      rtb_Switch2 = std::fmod(rtb_Switch2, 65536.0F);
    }

    rty_Vabc[0] = static_cast<uint16_T>(rtb_Switch2 < 0.0F ? static_cast<int32_T>
      (static_cast<uint16_T>(-static_cast<int16_T>(static_cast<uint16_T>
      (-rtb_Switch2)))) : static_cast<int32_T>(static_cast<uint16_T>(rtb_Switch2)));

    // Gain: '<S1>/Gain1'
    rtb_Switch2 = std::floor(16383.0F * rtb_deg2rad);
    if (rtIsNaNF(rtb_Switch2) || rtIsInfF(rtb_Switch2)) {
      rtb_Switch2 = 0.0F;
    } else {
      rtb_Switch2 = std::fmod(rtb_Switch2, 65536.0F);
    }

    rty_Vabc[1] = static_cast<uint16_T>(rtb_Switch2 < 0.0F ? static_cast<int32_T>
      (static_cast<uint16_T>(-static_cast<int16_T>(static_cast<uint16_T>
      (-rtb_Switch2)))) : static_cast<int32_T>(static_cast<uint16_T>(rtb_Switch2)));

    // Gain: '<S1>/Gain1'
    rtb_Switch2 = std::floor(16383.0F * rtb_Product1);
    if (rtIsNaNF(rtb_Switch2) || rtIsInfF(rtb_Switch2)) {
      rtb_Switch2 = 0.0F;
    } else {
      rtb_Switch2 = std::fmod(rtb_Switch2, 65536.0F);
    }

    rty_Vabc[2] = static_cast<uint16_T>(rtb_Switch2 < 0.0F ? static_cast<int32_T>
      (static_cast<uint16_T>(-static_cast<int16_T>(static_cast<uint16_T>
      (-rtb_Switch2)))) : static_cast<int32_T>(static_cast<uint16_T>(rtb_Switch2)));
  } else {
    rty_Vabc[0] = 0U;
    rty_Vabc[1] = 0U;
    rty_Vabc[2] = 0U;
  }

  // End of Switch: '<S1>/Switch2'

  // Update for DiscreteIntegrator: '<S43>/Integrator'
  control_foc_DW.Integrator_DSTATE = 1.78571427E-5F * rtb_algDD_o2_n +
    rtb_SumFdbk;
  Integrator_PrevResetState_tmp = static_cast<int8_T>(*rtu_Reset);

  // Update for DiscreteIntegrator: '<S43>/Integrator'
  control_foc_DW.Integrator_PrevResetState = Integrator_PrevResetState_tmp;

  // Update for UnitDelay: '<S1>/Unit Delay' incorporates:
  //   Sum: '<S1>/Sum3'

  control_foc_DW.UnitDelay_DSTATE = rtb_algDD_o1_p - rtb_Product2;

  // Update for DiscreteIntegrator: '<S93>/Integrator'
  control_foc_DW.Integrator_DSTATE_m = 1.78571427E-5F * rtb_Unary_Minus +
    rtb_sum_beta;
  control_foc_DW.Integrator_PrevResetState_k = Integrator_PrevResetState_tmp;
}

//
// File trailer for generated code.
//
// [EOF]
//
