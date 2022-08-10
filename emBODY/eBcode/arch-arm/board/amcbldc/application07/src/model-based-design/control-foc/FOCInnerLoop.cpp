//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: FOCInnerLoop.cpp
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 3.10
// Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
// C/C++ source code generated on : Tue Aug  9 15:18:35 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "control_foc_types.h"
#include "FOCInnerLoop.h"
#include "control_foc.h"

extern "C" {

#include "rt_nonfinite.h"

}
#include "arm_math.h"
#include <cmath>
#include "control_foc_private.h"
#include "rtwtypes.h"
#include "zero_crossing_types.h"

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
  real32_T rtb_FilterDifferentiatorTF_f;
  real32_T rtb_PProdOut_k;
  real32_T rtb_Product;
  real32_T rtb_SinCos_o1;
  real32_T rtb_SinCos_o2;
  real32_T rtb_Unary_Minus;
  real32_T rtb_algDD_o1_p;
  real32_T rtb_algDD_o2_n;
  real32_T rtb_sum_beta;
  int8_T rtb_Unary_Minus_0;
  int8_T rtb_sum_alpha_0;
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
  rtb_Unary_Minus = 0.0174532924F * rtu_Sensors->motorsensors.angle;

  // Trigonometry: '<S1>/SinCos'
  rtb_SinCos_o1 = std::sin(rtb_Unary_Minus);
  rtb_SinCos_o2 = std::cos(rtb_Unary_Minus);

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

  rtb_Unary_Minus = rtu_OuterOutputs->motorcurrent.current - rtb_IaIbIc0[1];

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // Product: '<S103>/PProd Out'
  rtb_algDD_o1_p = rtb_Unary_Minus * rtu_ConfigurationParameters->CurLoopPID.P;

  // SampleTimeMath: '<S93>/Tsamp' incorporates:
  //   SampleTimeMath: '<S41>/Tsamp'
  //
  //  About '<S93>/Tsamp':
  //   y = u * K where K = ( w * Ts )
  //
  //  About '<S41>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_FilterDifferentiatorTF_f = rtu_ConfigurationParameters->CurLoopPID.N *
    1.82857148E-5F;

  // Math: '<S91>/Reciprocal' incorporates:
  //   Constant: '<S91>/Constant'
  //   Math: '<S39>/Reciprocal'
  //   SampleTimeMath: '<S93>/Tsamp'
  //   Sum: '<S91>/SumDen'
  //
  //  About '<S91>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S39>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S93>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_sum_beta = 1.0F / (rtb_FilterDifferentiatorTF_f + 1.0F);

  // RelationalOperator: '<S3>/FixPt Relational Operator' incorporates:
  //   UnitDelay: '<S3>/Delay Input1'
  //
  //  Block description for '<S3>/Delay Input1':
  //
  //   Store in Global RAM

  rtb_FixPtRelationalOperator = (rtu_Flags->control_mode !=
    control_foc_DW.DelayInput1_DSTATE);

  // DiscreteTransferFcn: '<S91>/Filter Differentiator TF' incorporates:
  //   Constant: '<S91>/Constant'
  //   Math: '<S91>/Reciprocal'
  //   Product: '<S90>/DProd Out'
  //   Product: '<S91>/Divide'
  //   SampleTimeMath: '<S93>/Tsamp'
  //   Sum: '<S91>/SumNum'
  //
  //  About '<S91>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S93>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  if (rtb_FixPtRelationalOperator &&
      (control_foc_PrevZCX.FilterDifferentiatorTF_Reset_ZC != POS_ZCSIG)) {
    control_foc_DW.FilterDifferentiatorTF_states = 0.0F;
  }

  control_foc_PrevZCX.FilterDifferentiatorTF_Reset_ZC =
    rtb_FixPtRelationalOperator;
  control_foc_DW.FilterDifferentiatorTF_tmp = rtb_Unary_Minus *
    rtu_ConfigurationParameters->CurLoopPID.D - (rtb_FilterDifferentiatorTF_f -
    1.0F) * rtb_sum_beta * control_foc_DW.FilterDifferentiatorTF_states;

  // Product: '<S101>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S91>/Filter Differentiator TF'
  //   Math: '<S91>/Reciprocal'
  //   Product: '<S91>/DenCoefOut'
  //
  //  About '<S91>/Reciprocal':
  //   Operator: reciprocal

  rtb_algDD_o2_n = (control_foc_DW.FilterDifferentiatorTF_tmp +
                    -control_foc_DW.FilterDifferentiatorTF_states) *
    rtb_sum_beta * rtu_ConfigurationParameters->CurLoopPID.N;

  // Sum: '<S110>/SumI1' incorporates:
  //   Product: '<S95>/IProd Out'
  //   Sum: '<S108>/Sum Fdbk'
  //   Sum: '<S109>/SumI3'
  //   UnitDelay: '<S1>/Unit Delay'

  rtb_FilterDifferentiatorTF = (control_foc_DW.UnitDelay_DSTATE -
    ((rtb_algDD_o1_p + control_foc_DW.Integrator_DSTATE) + rtb_algDD_o2_n)) +
    rtb_Unary_Minus * rtu_ConfigurationParameters->CurLoopPID.I;

  // DiscreteIntegrator: '<S98>/Integrator'
  if (rtb_FixPtRelationalOperator && (control_foc_DW.Integrator_PrevResetState <=
       0)) {
    control_foc_DW.Integrator_DSTATE = 0.0F;
  }

  // DiscreteIntegrator: '<S98>/Integrator'
  rtb_DProdOut = 1.82857148E-5F * rtb_FilterDifferentiatorTF +
    control_foc_DW.Integrator_DSTATE;

  // Switch: '<S1>/Switch1' incorporates:
  //   Gain: '<S1>/Gain6'
  //   Product: '<S1>/Divide2'
  //   Sum: '<S107>/Sum'
  //   Sum: '<S1>/Sum2'
  //   Sum: '<S1>/Sum6'

  if (rtu_OuterOutputs->cur_en) {
    rtb_algDD_o2_n = ((rtb_algDD_o1_p + rtb_DProdOut) + rtb_algDD_o2_n) +
      rtb_Add;
  } else {
    rtb_algDD_o2_n = rtu_Targets->motorvoltage.voltage * rtb_Product * 0.01F +
      rtu_OuterOutputs->current_limiter;
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

  rtb_algDD_o2_n = rtb_algDD_o1_p / rtb_Product * 100.0F;

  // Outputs for Atomic SubSystem: '<S1>/Park Transform'
  // Product: '<S51>/PProd Out' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S8>/a16'
  //   Gain: '<S1>/Gain'

  rtb_PProdOut_k = -rtb_IaIbIc0[0] * rtu_ConfigurationParameters->CurLoopPID.P;

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // DiscreteTransferFcn: '<S39>/Filter Differentiator TF' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S8>/a16'
  //   Constant: '<S39>/Constant'
  //   Gain: '<S1>/Gain'
  //   Product: '<S38>/DProd Out'
  //   Product: '<S39>/Divide'
  //   Sum: '<S39>/SumNum'

  if (rtb_FixPtRelationalOperator &&
      (control_foc_PrevZCX.FilterDifferentiatorTF_Reset__o != POS_ZCSIG)) {
    control_foc_DW.FilterDifferentiatorTF_states_k = 0.0F;
  }

  control_foc_PrevZCX.FilterDifferentiatorTF_Reset__o =
    rtb_FixPtRelationalOperator;

  // Outputs for Atomic SubSystem: '<S1>/Park Transform'
  control_foc_DW.FilterDifferentiatorTF_tmp_c = -rtb_IaIbIc0[0] *
    rtu_ConfigurationParameters->CurLoopPID.D - (rtb_FilterDifferentiatorTF_f -
    1.0F) * rtb_sum_beta * control_foc_DW.FilterDifferentiatorTF_states_k;

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // Product: '<S49>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S39>/Filter Differentiator TF'
  //   Product: '<S39>/DenCoefOut'

  rtb_sum_beta = (control_foc_DW.FilterDifferentiatorTF_tmp_c +
                  -control_foc_DW.FilterDifferentiatorTF_states_k) *
    rtb_sum_beta * rtu_ConfigurationParameters->CurLoopPID.N;

  // Sum: '<S57>/Sum Fdbk'
  rtb_FilterDifferentiatorTF_f = (rtb_PProdOut_k +
    control_foc_DW.Integrator_DSTATE_o) + rtb_sum_beta;

  // Switch: '<S37>/Switch' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S37>/u_GTE_up'
  //   RelationalOperator: '<S37>/u_GT_lo'
  //   Switch: '<S37>/Switch1'

  if (rtb_FilterDifferentiatorTF_f >= rtb_Product) {
    rtb_Unary_Minus = rtb_Product;
  } else if (rtb_FilterDifferentiatorTF_f > -rtb_Product) {
    // Switch: '<S37>/Switch1'
    rtb_Unary_Minus = rtb_FilterDifferentiatorTF_f;
  } else {
    rtb_Unary_Minus = -rtb_Product;
  }

  // End of Switch: '<S37>/Switch'

  // Sum: '<S37>/Diff'
  rtb_FilterDifferentiatorTF_f -= rtb_Unary_Minus;

  // Outputs for Atomic SubSystem: '<S1>/Park Transform'
  // Product: '<S43>/IProd Out' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S8>/a16'
  //   Gain: '<S1>/Gain'

  rtb_Unary_Minus = -rtb_IaIbIc0[0] * rtu_ConfigurationParameters->CurLoopPID.I;

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // Switch: '<S34>/Switch1' incorporates:
  //   Constant: '<S34>/Constant'
  //   Constant: '<S34>/Constant2'
  //   Constant: '<S34>/Constant5'
  //   RelationalOperator: '<S34>/fix for DT propagation issue'

  if (rtb_FilterDifferentiatorTF_f > 0.0F) {
    rtb_sum_alpha_0 = 1;
  } else {
    rtb_sum_alpha_0 = -1;
  }

  // End of Switch: '<S34>/Switch1'

  // Switch: '<S34>/Switch2' incorporates:
  //   Constant: '<S34>/Constant3'
  //   Constant: '<S34>/Constant4'
  //   Constant: '<S34>/Constant5'
  //   RelationalOperator: '<S34>/fix for DT propagation issue1'

  if (rtb_Unary_Minus > 0.0F) {
    rtb_Unary_Minus_0 = 1;
  } else {
    rtb_Unary_Minus_0 = -1;
  }

  // End of Switch: '<S34>/Switch2'

  // Switch: '<S34>/Switch' incorporates:
  //   Constant: '<S34>/Constant1'
  //   Constant: '<S34>/Constant5'
  //   Logic: '<S34>/AND3'
  //   RelationalOperator: '<S34>/Equal1'
  //   RelationalOperator: '<S34>/Relational Operator'

  if ((rtb_FilterDifferentiatorTF_f != 0.0F) && (rtb_sum_alpha_0 ==
       rtb_Unary_Minus_0)) {
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

    rtb_PProdOut_k = (rtb_IaIbIc0[0] - rtb_SinCos_o2) /
      rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F + 5.0F;

    // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'

    // Saturate: '<S1>/Saturation'
    if (rtb_PProdOut_k > 100.0F) {
      rty_FOCOutputs->Vabc[0] = 100.0F;
    } else if (rtb_PProdOut_k < 0.0F) {
      rty_FOCOutputs->Vabc[0] = 0.0F;
    } else {
      rty_FOCOutputs->Vabc[0] = rtb_PProdOut_k;
    }

    // Sum: '<S1>/Sum1' incorporates:
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_PProdOut_k = (rtb_Product - rtb_SinCos_o2) /
      rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F + 5.0F;

    // Saturate: '<S1>/Saturation'
    if (rtb_PProdOut_k > 100.0F) {
      rty_FOCOutputs->Vabc[1] = 100.0F;
    } else if (rtb_PProdOut_k < 0.0F) {
      rty_FOCOutputs->Vabc[1] = 0.0F;
    } else {
      rty_FOCOutputs->Vabc[1] = rtb_PProdOut_k;
    }

    // Sum: '<S1>/Sum1' incorporates:
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_PProdOut_k = (rtb_SinCos_o1 - rtb_SinCos_o2) /
      rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F + 5.0F;

    // Saturate: '<S1>/Saturation'
    if (rtb_PProdOut_k > 100.0F) {
      rty_FOCOutputs->Vabc[2] = 100.0F;
    } else if (rtb_PProdOut_k < 0.0F) {
      rty_FOCOutputs->Vabc[2] = 0.0F;
    } else {
      rty_FOCOutputs->Vabc[2] = rtb_PProdOut_k;
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
