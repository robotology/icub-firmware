//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: FOCInnerLoop.cpp
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 2.96
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Fri Apr  1 11:36:16 2022
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

// System initialize for atomic system: '<Root>/FOC inner loop'
void FOCInnerLoop_Init(void)
{
  // InitializeConditions for UnitDelay: '<S3>/Delay Input1'
  //
  //  Block description for '<S3>/Delay Input1':
  //
  //   Store in Global RAM

  control_foc_DW.DelayInput1_DSTATE = ControlModes_Idle;

  // InitializeConditions for DiscreteIntegrator: '<S98>/Integrator'
  control_foc_DW.Integrator_PrevResetState = 2;

  // InitializeConditions for DiscreteIntegrator: '<S46>/Integrator'
  control_foc_DW.Integrator_PrevResetState_k = 2;
}

// Output and update for atomic system: '<Root>/FOC inner loop'
void FOCInnerLoop(const Flags *rtu_Flags, const ConfigurationParameters
                  *rtu_ConfigurationParameters, const SensorsData *rtu_Sensors,
                  const EstimatedData *rtu_Estimates, const Targets *rtu_Targets,
                  const ControlOuterOutputs *rtu_OuterOutputs, ControlOutputs
                  *rty_FOCOutputs)
{
  real32_T rtb_IaIbIc0[2];
  real32_T rtb_Add;
  real32_T rtb_DProdOut;
  real32_T rtb_FilterDifferentiatorTF;
  real32_T rtb_FilterDifferentiatorTF_a;
  real32_T rtb_FilterDifferentiatorTF_f;
  real32_T rtb_PProdOut_k;
  real32_T rtb_Product;
  real32_T rtb_SinCos_o1;
  real32_T rtb_SinCos_o2;
  real32_T rtb_Unary_Minus;
  real32_T rtb_algDD_o1_p;
  real32_T rtb_algDD_o2_n;
  real32_T rtb_sum_alpha;
  real32_T rtb_sum_beta;
  real32_T tmp;
  boolean_T rtb_FixPtRelationalOperator;

  // Sum: '<S1>/Add' incorporates:
  //   Product: '<S1>/Product1'
  //   Product: '<S1>/Product2'

  rtb_Add = rtu_OuterOutputs->motorcurrent.current *
    rtu_ConfigurationParameters->motorconfig.Rphase +
    rtu_Estimates->jointvelocities.velocity *
    rtu_ConfigurationParameters->motorconfig.Kbemf;

  // MinMax: '<S1>/Min'
  if ((rtu_ConfigurationParameters->motorconfig.Vcc <=
       rtu_ConfigurationParameters->motorconfig.Vmax) || rtIsNaNF
      (rtu_ConfigurationParameters->motorconfig.Vmax)) {
    rtb_Product = rtu_ConfigurationParameters->motorconfig.Vcc;
  } else {
    rtb_Product = rtu_ConfigurationParameters->motorconfig.Vmax;
  }

  // End of MinMax: '<S1>/Min'

  // Product: '<S1>/Product' incorporates:
  //   Gain: '<S1>/Gain4'

  rtb_Product = 0.5F * rtb_Product * control_foc_ConstB.Sum5;

  // Gain: '<S1>/Ia+Ib+Ic=0'
  for (int32_T i = 0; i < 2; i++) {
    rtb_IaIbIc0[i] = 0.0F;
    rtb_IaIbIc0[i] += rtCP_IaIbIc0_Gain[i] * rtu_Sensors->motorsensors.Iabc[0];
    rtb_IaIbIc0[i] += rtCP_IaIbIc0_Gain[i + 2] * rtu_Sensors->motorsensors.Iabc
      [1];
    rtb_IaIbIc0[i] += rtCP_IaIbIc0_Gain[i + 4] * rtu_Sensors->motorsensors.Iabc
      [2];
  }

  // End of Gain: '<S1>/Ia+Ib+Ic=0'

  // Outputs for Atomic SubSystem: '<S1>/Clarke Transform'
  // AlgorithmDescriptorDelegate generated from: '<S2>/a16'
  arm_clarke_f32(rtb_IaIbIc0[0], rtb_IaIbIc0[1], &rtb_algDD_o1_p,
                 &rtb_algDD_o2_n);

  // End of Outputs for SubSystem: '<S1>/Clarke Transform'

  // Gain: '<S1>/deg2rad'
  rtb_PProdOut_k = 0.0174532924F * rtu_Sensors->motorsensors.angle;

  // Trigonometry: '<S1>/SinCos'
  rtb_SinCos_o1 = std::sin(rtb_PProdOut_k);
  rtb_SinCos_o2 = std::cos(rtb_PProdOut_k);

  // Outputs for Atomic SubSystem: '<S1>/Park Transform'
  // Switch: '<S115>/Switch' incorporates:
  //   Product: '<S8>/acos'
  //   Product: '<S8>/asin'
  //   Product: '<S8>/bcos'
  //   Product: '<S8>/bsin'
  //   Sum: '<S8>/sum_Ds'
  //   Sum: '<S8>/sum_Qs'
  //   UnaryMinus: '<S115>/Unary_Minus'

  rtb_IaIbIc0[0] = -(rtb_algDD_o2_n * rtb_SinCos_o2 - rtb_algDD_o1_p *
                     rtb_SinCos_o1);
  rtb_IaIbIc0[1] = rtb_algDD_o1_p * rtb_SinCos_o2 + rtb_algDD_o2_n *
    rtb_SinCos_o1;

  // Sum: '<S1>/Sum' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S8>/a16'

  rtb_PProdOut_k = rtu_OuterOutputs->motorcurrent.current - rtb_IaIbIc0[1];

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // Product: '<S103>/PProd Out'
  rtb_algDD_o1_p = rtb_PProdOut_k * rtu_ConfigurationParameters->motorconfig.Kp;

  // RelationalOperator: '<S3>/FixPt Relational Operator' incorporates:
  //   UnitDelay: '<S3>/Delay Input1'
  //
  //  Block description for '<S3>/Delay Input1':
  //
  //   Store in Global RAM

  rtb_FixPtRelationalOperator = (rtu_Flags->control_mode !=
    control_foc_DW.DelayInput1_DSTATE);

  // DiscreteTransferFcn: '<S91>/Filter Differentiator TF' incorporates:
  //   Product: '<S90>/DProd Out'

  if (rtb_FixPtRelationalOperator &&
      (control_foc_PrevZCX.FilterDifferentiatorTF_Reset_ZC != 1)) {
    control_foc_DW.FilterDifferentiatorTF_states = 0.0F;
  }

  control_foc_PrevZCX.FilterDifferentiatorTF_Reset_ZC =
    rtb_FixPtRelationalOperator;
  control_foc_DW.FilterDifferentiatorTF_tmp = rtb_PProdOut_k *
    rtu_ConfigurationParameters->motorconfig.Kd - -0.999634385F *
    control_foc_DW.FilterDifferentiatorTF_states;

  // Product: '<S101>/NProd Out' incorporates:
  //   Constant: '<S1>/Constant'
  //   DiscreteTransferFcn: '<S91>/Filter Differentiator TF'
  //   Product: '<S91>/DenCoefOut'

  rtb_algDD_o2_n = (control_foc_DW.FilterDifferentiatorTF_tmp +
                    -control_foc_DW.FilterDifferentiatorTF_states) *
    0.999817193F * 10.0F;

  // Sum: '<S110>/SumI1' incorporates:
  //   Product: '<S95>/IProd Out'
  //   Sum: '<S108>/Sum Fdbk'
  //   Sum: '<S109>/SumI3'
  //   UnitDelay: '<S1>/Unit Delay'

  rtb_FilterDifferentiatorTF = (control_foc_DW.UnitDelay_DSTATE -
    ((rtb_algDD_o1_p + control_foc_DW.Integrator_DSTATE) + rtb_algDD_o2_n)) +
    rtb_PProdOut_k * rtu_ConfigurationParameters->motorconfig.Ki;

  // DiscreteIntegrator: '<S98>/Integrator'
  if (rtb_FixPtRelationalOperator && (control_foc_DW.Integrator_PrevResetState <=
       0)) {
    control_foc_DW.Integrator_DSTATE = 0.0F;
  }

  // DiscreteIntegrator: '<S98>/Integrator'
  rtb_DProdOut = 1.82857148E-5F * rtb_FilterDifferentiatorTF +
    control_foc_DW.Integrator_DSTATE;

  // Switch: '<S1>/Switch1' incorporates:
  //   Sum: '<S107>/Sum'
  //   Sum: '<S1>/Sum2'

  if (rtu_OuterOutputs->cur_en) {
    rtb_algDD_o2_n = ((rtb_algDD_o1_p + rtb_DProdOut) + rtb_algDD_o2_n) +
      rtb_Add;
  } else {
    rtb_algDD_o2_n = rtu_Targets->motorvoltage.voltage;
  }

  // End of Switch: '<S1>/Switch1'

  // Switch: '<S9>/Switch2' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S9>/LowerRelop1'
  //   RelationalOperator: '<S9>/UpperRelop'
  //   Switch: '<S9>/Switch'

  if (rtb_algDD_o2_n > rtb_Product) {
    rtb_algDD_o1_p = rtb_Product;
  } else if (rtb_algDD_o2_n < -rtb_Product) {
    // Switch: '<S9>/Switch' incorporates:
    //   Gain: '<S1>/Gain2'

    rtb_algDD_o1_p = -rtb_Product;
  } else {
    rtb_algDD_o1_p = rtb_algDD_o2_n;
  }

  // End of Switch: '<S9>/Switch2'

  // Gain: '<S1>/Gain3' incorporates:
  //   Product: '<S1>/Divide1'

  rtb_algDD_o2_n = rtb_algDD_o1_p / rtu_ConfigurationParameters->motorconfig.Vcc
    * 100.0F;

  // Outputs for Atomic SubSystem: '<S1>/Park Transform'
  // Product: '<S51>/PProd Out' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S8>/a16'
  //   Gain: '<S1>/Gain'

  rtb_PProdOut_k = -rtb_IaIbIc0[0] * rtu_ConfigurationParameters->motorconfig.Kp;

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // DiscreteTransferFcn: '<S39>/Filter Differentiator TF' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S8>/a16'
  //   Gain: '<S1>/Gain'
  //   Product: '<S38>/DProd Out'

  if (rtb_FixPtRelationalOperator &&
      (control_foc_PrevZCX.FilterDifferentiatorTF_Reset__o != 1)) {
    control_foc_DW.FilterDifferentiatorTF_states_k = 0.0F;
  }

  control_foc_PrevZCX.FilterDifferentiatorTF_Reset__o =
    rtb_FixPtRelationalOperator;

  // Outputs for Atomic SubSystem: '<S1>/Park Transform'
  control_foc_DW.FilterDifferentiatorTF_tmp_c = -rtb_IaIbIc0[0] *
    rtu_ConfigurationParameters->motorconfig.Kd - -0.999634385F *
    control_foc_DW.FilterDifferentiatorTF_states_k;

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // Product: '<S49>/NProd Out' incorporates:
  //   Constant: '<S1>/Constant'
  //   DiscreteTransferFcn: '<S39>/Filter Differentiator TF'
  //   Product: '<S39>/DenCoefOut'

  rtb_sum_beta = (control_foc_DW.FilterDifferentiatorTF_tmp_c +
                  -control_foc_DW.FilterDifferentiatorTF_states_k) *
    0.999817193F * 10.0F;

  // Sum: '<S57>/Sum Fdbk'
  rtb_FilterDifferentiatorTF_f = (rtb_PProdOut_k +
    control_foc_DW.Integrator_DSTATE_o) + rtb_sum_beta;

  // Switch: '<S37>/Switch' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S37>/u_GTE_up'
  //   RelationalOperator: '<S37>/u_GT_lo'
  //   Switch: '<S37>/Switch1'

  if (rtb_FilterDifferentiatorTF_f >= rtb_Product) {
    rtb_FilterDifferentiatorTF_a = rtb_Product;
  } else if (rtb_FilterDifferentiatorTF_f > -rtb_Product) {
    // Switch: '<S37>/Switch1'
    rtb_FilterDifferentiatorTF_a = rtb_FilterDifferentiatorTF_f;
  } else {
    rtb_FilterDifferentiatorTF_a = -rtb_Product;
  }

  // End of Switch: '<S37>/Switch'

  // Sum: '<S37>/Diff'
  rtb_sum_alpha = rtb_FilterDifferentiatorTF_f - rtb_FilterDifferentiatorTF_a;

  // Outputs for Atomic SubSystem: '<S1>/Park Transform'
  // Product: '<S43>/IProd Out' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S8>/a16'
  //   Gain: '<S1>/Gain'

  rtb_Unary_Minus = -rtb_IaIbIc0[0] *
    rtu_ConfigurationParameters->motorconfig.Ki;

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // Signum: '<S34>/SignPreSat'
  if (rtb_sum_alpha < 0.0F) {
    // DataTypeConversion: '<S34>/DataTypeConv1'
    rtb_FilterDifferentiatorTF_a = -1.0F;
  } else if (rtb_sum_alpha > 0.0F) {
    // DataTypeConversion: '<S34>/DataTypeConv1'
    rtb_FilterDifferentiatorTF_a = 1.0F;
  } else if (rtb_sum_alpha == 0.0F) {
    // DataTypeConversion: '<S34>/DataTypeConv1'
    rtb_FilterDifferentiatorTF_a = 0.0F;
  } else {
    // DataTypeConversion: '<S34>/DataTypeConv1'
    rtb_FilterDifferentiatorTF_a = (rtNaNF);
  }

  // End of Signum: '<S34>/SignPreSat'

  // DataTypeConversion: '<S34>/DataTypeConv1'
  if (rtIsNaNF(rtb_FilterDifferentiatorTF_a)) {
    rtb_FilterDifferentiatorTF_a = 0.0F;
  } else {
    rtb_FilterDifferentiatorTF_a = std::fmod(rtb_FilterDifferentiatorTF_a,
      256.0F);
  }

  // Signum: '<S34>/SignPreIntegrator'
  if (rtb_Unary_Minus < 0.0F) {
    // DataTypeConversion: '<S34>/DataTypeConv2'
    tmp = -1.0F;
  } else if (rtb_Unary_Minus > 0.0F) {
    // DataTypeConversion: '<S34>/DataTypeConv2'
    tmp = 1.0F;
  } else if (rtb_Unary_Minus == 0.0F) {
    // DataTypeConversion: '<S34>/DataTypeConv2'
    tmp = 0.0F;
  } else {
    // DataTypeConversion: '<S34>/DataTypeConv2'
    tmp = (rtNaNF);
  }

  // End of Signum: '<S34>/SignPreIntegrator'

  // DataTypeConversion: '<S34>/DataTypeConv2'
  if (rtIsNaNF(tmp)) {
    tmp = 0.0F;
  } else {
    tmp = std::fmod(tmp, 256.0F);
  }

  // Switch: '<S34>/Switch' incorporates:
  //   Constant: '<S34>/Constant1'
  //   DataTypeConversion: '<S34>/DataTypeConv1'
  //   DataTypeConversion: '<S34>/DataTypeConv2'
  //   Gain: '<S34>/ZeroGain'
  //   Logic: '<S34>/AND3'
  //   RelationalOperator: '<S34>/Equal1'
  //   RelationalOperator: '<S34>/NotEqual'

  if ((0.0F * rtb_FilterDifferentiatorTF_f != rtb_sum_alpha) &&
      ((rtb_FilterDifferentiatorTF_a < 0.0F ? static_cast<int32_T>(static_cast<
         int8_T>(-static_cast<int8_T>(static_cast<uint8_T>
           (-rtb_FilterDifferentiatorTF_a)))) : static_cast<int32_T>(
         static_cast<int8_T>(static_cast<uint8_T>(rtb_FilterDifferentiatorTF_a))))
       == (tmp < 0.0F ? static_cast<int32_T>(static_cast<int8_T>
         (-static_cast<int8_T>(static_cast<uint8_T>(-tmp)))) :
           static_cast<int32_T>(static_cast<int8_T>(static_cast<uint8_T>(tmp))))))
  {
    rtb_FilterDifferentiatorTF_f = 0.0F;
  } else {
    rtb_FilterDifferentiatorTF_f = rtb_Unary_Minus;
  }

  // End of Switch: '<S34>/Switch'

  // DiscreteIntegrator: '<S46>/Integrator'
  if (rtb_FixPtRelationalOperator && (control_foc_DW.Integrator_PrevResetState_k
       <= 0)) {
    control_foc_DW.Integrator_DSTATE_o = 0.0F;
  }

  // DiscreteIntegrator: '<S46>/Integrator'
  rtb_Unary_Minus = 1.82857148E-5F * rtb_FilterDifferentiatorTF_f +
    control_foc_DW.Integrator_DSTATE_o;

  // Sum: '<S56>/Sum'
  rtb_PProdOut_k = (rtb_PProdOut_k + rtb_Unary_Minus) + rtb_sum_beta;

  // Switch: '<S54>/Switch2' incorporates:
  //   RelationalOperator: '<S54>/LowerRelop1'

  if (!(rtb_PProdOut_k > rtb_Product)) {
    // Switch: '<S54>/Switch' incorporates:
    //   Gain: '<S1>/Gain2'
    //   RelationalOperator: '<S54>/UpperRelop'

    if (rtb_PProdOut_k < -rtb_Product) {
      rtb_Product = -rtb_Product;
    } else {
      rtb_Product = rtb_PProdOut_k;
    }

    // End of Switch: '<S54>/Switch'
  }

  // End of Switch: '<S54>/Switch2'

  // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
  // Switch: '<S64>/Switch' incorporates:
  //   Product: '<S6>/dsin'
  //   Product: '<S6>/qcos'
  //   Sum: '<S6>/sum_beta'

  rtb_IaIbIc0[0] = rtb_algDD_o1_p * rtb_SinCos_o2 + rtb_Product * rtb_SinCos_o1;

  // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'

  // Switch: '<S1>/Switch2' incorporates:
  //   Constant: '<S1>/Constant1'

  if (rtu_OuterOutputs->out_en) {
    // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
    // Gain: '<S5>/sqrt3_by_two' incorporates:
    //   Product: '<S6>/dcos'
    //   Product: '<S6>/qsin'
    //   Sum: '<S6>/sum_alpha'
    //   UnaryMinus: '<S64>/Unary_Minus'

    rtb_SinCos_o2 = -(rtb_Product * rtb_SinCos_o2 - rtb_algDD_o1_p *
                      rtb_SinCos_o1) * 0.866025388F;

    // Gain: '<S5>/one_by_two' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S6>/a16'

    rtb_Product = 0.5F * rtb_IaIbIc0[0];

    // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'

    // Sum: '<S5>/add_c'
    rtb_SinCos_o1 = (0.0F - rtb_Product) - rtb_SinCos_o2;

    // Sum: '<S5>/add_b'
    rtb_Product = rtb_SinCos_o2 - rtb_Product;

    // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
    // MinMax: '<S1>/Min1' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S6>/a16'

    if ((rtb_IaIbIc0[0] <= rtb_Product) || rtIsNaNF(rtb_Product)) {
      rtb_SinCos_o2 = rtb_IaIbIc0[0];
    } else {
      rtb_SinCos_o2 = rtb_Product;
    }

    // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'
    if ((!(rtb_SinCos_o2 <= rtb_SinCos_o1)) && (!rtIsNaNF(rtb_SinCos_o1))) {
      rtb_SinCos_o2 = rtb_SinCos_o1;
    }

    // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
    // Sum: '<S1>/Sum1' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S6>/a16'
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_FilterDifferentiatorTF_a = (rtb_IaIbIc0[0] - rtb_SinCos_o2) /
      rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F + 5.0F;

    // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'

    // Saturate: '<S1>/Saturation'
    if (rtb_FilterDifferentiatorTF_a > 100.0F) {
      rty_FOCOutputs->Vabc[0] = 100.0F;
    } else if (rtb_FilterDifferentiatorTF_a < 0.0F) {
      rty_FOCOutputs->Vabc[0] = 0.0F;
    } else {
      rty_FOCOutputs->Vabc[0] = rtb_FilterDifferentiatorTF_a;
    }

    // Sum: '<S1>/Sum1' incorporates:
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_FilterDifferentiatorTF_a = (rtb_Product - rtb_SinCos_o2) /
      rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F + 5.0F;

    // Saturate: '<S1>/Saturation'
    if (rtb_FilterDifferentiatorTF_a > 100.0F) {
      rty_FOCOutputs->Vabc[1] = 100.0F;
    } else if (rtb_FilterDifferentiatorTF_a < 0.0F) {
      rty_FOCOutputs->Vabc[1] = 0.0F;
    } else {
      rty_FOCOutputs->Vabc[1] = rtb_FilterDifferentiatorTF_a;
    }

    // Sum: '<S1>/Sum1' incorporates:
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_FilterDifferentiatorTF_a = (rtb_SinCos_o1 - rtb_SinCos_o2) /
      rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F + 5.0F;

    // Saturate: '<S1>/Saturation'
    if (rtb_FilterDifferentiatorTF_a > 100.0F) {
      rty_FOCOutputs->Vabc[2] = 100.0F;
    } else if (rtb_FilterDifferentiatorTF_a < 0.0F) {
      rty_FOCOutputs->Vabc[2] = 0.0F;
    } else {
      rty_FOCOutputs->Vabc[2] = rtb_FilterDifferentiatorTF_a;
    }
  } else {
    rty_FOCOutputs->Vabc[0] = 0.0F;
    rty_FOCOutputs->Vabc[1] = 0.0F;
    rty_FOCOutputs->Vabc[2] = 0.0F;
  }

  // End of Switch: '<S1>/Switch2'

  // Outputs for Atomic SubSystem: '<S1>/Park Transform'
  // BusCreator: '<S1>/Bus Creator1' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S8>/a16'

  rty_FOCOutputs->Iq_fbk.current = rtb_IaIbIc0[1];

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // Saturate: '<S1>/Saturation1'
  if (rtb_algDD_o2_n > 100.0F) {
    // BusCreator: '<S1>/Bus Creator'
    rty_FOCOutputs->Vq = 100.0F;
  } else if (rtb_algDD_o2_n < -100.0F) {
    // BusCreator: '<S1>/Bus Creator'
    rty_FOCOutputs->Vq = -100.0F;
  } else {
    // BusCreator: '<S1>/Bus Creator'
    rty_FOCOutputs->Vq = rtb_algDD_o2_n;
  }

  // End of Saturate: '<S1>/Saturation1'

  // Update for UnitDelay: '<S3>/Delay Input1'
  //
  //  Block description for '<S3>/Delay Input1':
  //
  //   Store in Global RAM

  control_foc_DW.DelayInput1_DSTATE = rtu_Flags->control_mode;

  // Update for DiscreteTransferFcn: '<S91>/Filter Differentiator TF'
  control_foc_DW.FilterDifferentiatorTF_states =
    control_foc_DW.FilterDifferentiatorTF_tmp;

  // Update for UnitDelay: '<S1>/Unit Delay' incorporates:
  //   Sum: '<S1>/Sum3'

  control_foc_DW.UnitDelay_DSTATE = rtb_algDD_o1_p - rtb_Add;

  // Update for DiscreteIntegrator: '<S98>/Integrator'
  control_foc_DW.Integrator_DSTATE = 1.82857148E-5F * rtb_FilterDifferentiatorTF
    + rtb_DProdOut;
  control_foc_DW.Integrator_PrevResetState = static_cast<int8_T>
    (rtb_FixPtRelationalOperator);

  // Update for DiscreteTransferFcn: '<S39>/Filter Differentiator TF'
  control_foc_DW.FilterDifferentiatorTF_states_k =
    control_foc_DW.FilterDifferentiatorTF_tmp_c;

  // Update for DiscreteIntegrator: '<S46>/Integrator'
  control_foc_DW.Integrator_DSTATE_o = 1.82857148E-5F *
    rtb_FilterDifferentiatorTF_f + rtb_Unary_Minus;
  control_foc_DW.Integrator_PrevResetState_k = static_cast<int8_T>
    (rtb_FixPtRelationalOperator);
}

//
// File trailer for generated code.
//
// [EOF]
//
