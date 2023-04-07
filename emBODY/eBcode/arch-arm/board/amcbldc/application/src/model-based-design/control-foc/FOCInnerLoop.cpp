//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: FOCInnerLoop.cpp
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 5.7
// Simulink Coder version         : 9.9 (R2023a) 19-Nov-2022
// C/C++ source code generated on : Thu Apr  6 14:46:35 2023
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "control_foc_types.h"
#include "FOCInnerLoop.h"
#include "control_foc.h"

extern "C"
{

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
  // InitializeConditions for DiscreteIntegrator: '<S97>/Integrator'
  control_foc_DW.Integrator_PrevResetState = 2;

  // InitializeConditions for DiscreteIntegrator: '<S45>/Integrator'
  control_foc_DW.Integrator_PrevResetState_k = 2;
}

// Outputs for atomic system: '<Root>/FOC inner loop'
void FOCInnerLoop(const ConfigurationParameters *rtu_ConfigurationParameters,
                  const SensorsData *rtu_Sensors, const EstimatedData
                  *rtu_Estimates, const Targets *rtu_Targets, const
                  ControlOuterOutputs *rtu_OuterOutputs, ControlOutputs
                  *rty_FOCOutputs)
{
  real32_T rtb_IaIbIc0[2];
  real32_T DProdOut;
  real32_T rtb_Add;
  real32_T rtb_Diff;
  real32_T rtb_Product;
  real32_T rtb_SinCos_o1;
  real32_T rtb_SinCos_o2;
  real32_T rtb_Unary_Minus;
  real32_T rtb_algDD_o1_p;
  real32_T rtb_algDD_o2_n;
  real32_T rtb_sum_alpha;
  int8_T tmp;
  int8_T tmp_0;

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
    rtb_sum_alpha = rtu_ConfigurationParameters->motorconfig.Vcc;
  } else {
    rtb_sum_alpha = rtu_ConfigurationParameters->motorconfig.Vmax;
  }

  // Product: '<S1>/Product' incorporates:
  //   Gain: '<S1>/Gain4'
  //   MinMax: '<S1>/Min'

  rtb_Product = 0.5F * rtb_sum_alpha * control_foc_ConstB.Sum5;

  // Gain: '<S1>/Ia+Ib+Ic=0'
  rtb_algDD_o1_p = rtu_Sensors->motorsensors.Iabc[1];
  rtb_algDD_o2_n = rtu_Sensors->motorsensors.Iabc[0];
  rtb_Unary_Minus = rtu_Sensors->motorsensors.Iabc[2];
  for (int32_T i = 0; i < 2; i++) {
    rtb_IaIbIc0[i] = (rtCP_IaIbIc0_Gain[i + 2] * rtb_algDD_o1_p +
                      rtCP_IaIbIc0_Gain[i] * rtb_algDD_o2_n) +
      rtCP_IaIbIc0_Gain[i + 4] * rtb_Unary_Minus;
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
  // Switch: '<S114>/Switch' incorporates:
  //   Product: '<S7>/acos'
  //   Product: '<S7>/asin'
  //   Product: '<S7>/bcos'
  //   Product: '<S7>/bsin'
  //   Sum: '<S7>/sum_Ds'
  //   Sum: '<S7>/sum_Qs'
  //   UnaryMinus: '<S114>/Unary_Minus'

  rtb_IaIbIc0[0] = -(rtb_algDD_o2_n * rtb_SinCos_o2 - rtb_algDD_o1_p *
                     rtb_SinCos_o1);
  rtb_IaIbIc0[1] = rtb_algDD_o1_p * rtb_SinCos_o2 + rtb_algDD_o2_n *
    rtb_SinCos_o1;

  // AlgorithmDescriptorDelegate generated from: '<S7>/a16'
  rtb_algDD_o1_p = rtb_IaIbIc0[0];

  // BusCreator: '<S1>/Bus Creator1' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S7>/a16'

  rty_FOCOutputs->Iq_fbk.current = rtb_IaIbIc0[1];

  // Sum: '<S1>/Sum' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S7>/a16'

  rtb_Unary_Minus = rtu_OuterOutputs->motorcurrent.current - rtb_IaIbIc0[1];

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // Product: '<S102>/PProd Out'
  rtb_algDD_o2_n = rtb_Unary_Minus * rtu_ConfigurationParameters->CurLoopPID.P;

  // Product: '<S94>/IProd Out'
  rtb_Diff = rtb_Unary_Minus * rtu_ConfigurationParameters->CurLoopPID.I;

  // SampleTimeMath: '<S92>/Tsamp' incorporates:
  //   SampleTimeMath: '<S40>/Tsamp'
  //
  //  About '<S92>/Tsamp':
  //   y = u * K where K = ( w * Ts )
  //
  //  About '<S40>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  DProdOut = rtu_ConfigurationParameters->CurLoopPID.N * 1.82857148E-5F;

  // Math: '<S90>/Reciprocal' incorporates:
  //   Constant: '<S90>/Filter Den Constant'
  //   Math: '<S38>/Reciprocal'
  //   SampleTimeMath: '<S92>/Tsamp'
  //   Sum: '<S90>/SumDen'
  //
  //  About '<S90>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S38>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S92>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_sum_alpha = 1.0F / (DProdOut + 1.0F);

  // DiscreteTransferFcn: '<S90>/Filter Differentiator TF'
  if (rtu_OuterOutputs->pid_reset &&
      (control_foc_PrevZCX.FilterDifferentiatorTF_Reset_ZC != POS_ZCSIG)) {
    control_foc_DW.FilterDifferentiatorTF_states = 0.0F;
  }

  control_foc_PrevZCX.FilterDifferentiatorTF_Reset_ZC =
    rtu_OuterOutputs->pid_reset;

  // Product: '<S90>/Divide' incorporates:
  //   Constant: '<S90>/Filter Den Constant'
  //   Math: '<S90>/Reciprocal'
  //   Product: '<S38>/Divide'
  //   SampleTimeMath: '<S92>/Tsamp'
  //   Sum: '<S90>/SumNum'
  //
  //  About '<S90>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S92>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  DProdOut = (DProdOut - 1.0F) * rtb_sum_alpha;

  // DiscreteTransferFcn: '<S90>/Filter Differentiator TF' incorporates:
  //   Product: '<S89>/DProd Out'
  //   Product: '<S90>/Divide'

  control_foc_DW.FilterDifferentiatorTF_tmp = rtb_Unary_Minus *
    rtu_ConfigurationParameters->CurLoopPID.D - DProdOut *
    control_foc_DW.FilterDifferentiatorTF_states;

  // Product: '<S100>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S90>/Filter Differentiator TF'
  //   Math: '<S90>/Reciprocal'
  //   Product: '<S90>/DenCoefOut'
  //
  //  About '<S90>/Reciprocal':
  //   Operator: reciprocal

  rtb_Unary_Minus = (control_foc_DW.FilterDifferentiatorTF_tmp -
                     control_foc_DW.FilterDifferentiatorTF_states) *
    rtb_sum_alpha * rtu_ConfigurationParameters->CurLoopPID.N;

  // Sum: '<S109>/SumI1' incorporates:
  //   Sum: '<S107>/Sum Fdbk'
  //   Sum: '<S108>/SumI3'
  //   UnitDelay: '<S1>/Unit Delay'

  control_foc_B.SumI1 = (control_foc_DW.UnitDelay_DSTATE - ((rtb_algDD_o2_n +
    control_foc_DW.Integrator_DSTATE) + rtb_Unary_Minus)) + rtb_Diff;

  // DiscreteIntegrator: '<S97>/Integrator'
  if (rtu_OuterOutputs->pid_reset && (control_foc_DW.Integrator_PrevResetState <=
       0)) {
    control_foc_DW.Integrator_DSTATE = 0.0F;
  }

  // DiscreteIntegrator: '<S97>/Integrator'
  control_foc_B.Integrator = 1.82857148E-5F * control_foc_B.SumI1 +
    control_foc_DW.Integrator_DSTATE;

  // Switch: '<S1>/Switch1' incorporates:
  //   Gain: '<S1>/Gain6'
  //   Product: '<S1>/Divide2'
  //   Sum: '<S106>/Sum'
  //   Sum: '<S1>/Sum2'
  //   Sum: '<S1>/Sum6'

  if (rtu_OuterOutputs->cur_en) {
    rtb_algDD_o2_n = ((rtb_algDD_o2_n + control_foc_B.Integrator) +
                      rtb_Unary_Minus) + rtb_Add;
  } else {
    rtb_algDD_o2_n = rtu_Targets->motorvoltage.voltage * rtb_Product * 0.01F +
      rtu_OuterOutputs->current_limiter;
  }

  // End of Switch: '<S1>/Switch1'

  // Switch: '<S8>/Switch2' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S8>/LowerRelop1'
  //   RelationalOperator: '<S8>/UpperRelop'
  //   Switch: '<S8>/Switch'

  if (rtb_algDD_o2_n > rtb_Product) {
    rtb_algDD_o2_n = rtb_Product;
  } else if (rtb_algDD_o2_n < -rtb_Product) {
    // Switch: '<S8>/Switch' incorporates:
    //   Gain: '<S1>/Gain2'

    rtb_algDD_o2_n = -rtb_Product;
  }

  // End of Switch: '<S8>/Switch2'

  // Outputs for Atomic SubSystem: '<S1>/Park Transform'
  // Product: '<S50>/PProd Out' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S7>/a16'
  //   Gain: '<S1>/Gain'

  rtb_Unary_Minus = -rtb_IaIbIc0[0] * rtu_ConfigurationParameters->CurLoopPID.P;

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // DiscreteTransferFcn: '<S38>/Filter Differentiator TF' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S7>/a16'
  //   DiscreteTransferFcn: '<S90>/Filter Differentiator TF'
  //   Gain: '<S1>/Gain'
  //   Product: '<S37>/DProd Out'

  if (rtu_OuterOutputs->pid_reset &&
      (control_foc_PrevZCX.FilterDifferentiatorTF_Reset__o != POS_ZCSIG)) {
    control_foc_DW.FilterDifferentiatorTF_states_k = 0.0F;
  }

  control_foc_PrevZCX.FilterDifferentiatorTF_Reset__o =
    rtu_OuterOutputs->pid_reset;

  // Outputs for Atomic SubSystem: '<S1>/Park Transform'
  control_foc_DW.FilterDifferentiatorTF_tmp_c = -rtb_IaIbIc0[0] *
    rtu_ConfigurationParameters->CurLoopPID.D - DProdOut *
    control_foc_DW.FilterDifferentiatorTF_states_k;

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // Product: '<S48>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S38>/Filter Differentiator TF'
  //   Product: '<S38>/DenCoefOut'

  DProdOut = (control_foc_DW.FilterDifferentiatorTF_tmp_c -
              control_foc_DW.FilterDifferentiatorTF_states_k) * rtb_sum_alpha *
    rtu_ConfigurationParameters->CurLoopPID.N;

  // Sum: '<S56>/Sum Fdbk'
  rtb_Diff = (rtb_Unary_Minus + control_foc_DW.Integrator_DSTATE_o) + DProdOut;

  // Switch: '<S36>/Switch' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S36>/u_GTE_up'
  //   RelationalOperator: '<S36>/u_GT_lo'
  //   Switch: '<S36>/Switch1'

  if (rtb_Diff >= rtb_Product) {
    rtb_sum_alpha = rtb_Product;
  } else if (rtb_Diff > -rtb_Product) {
    // Switch: '<S36>/Switch1'
    rtb_sum_alpha = rtb_Diff;
  } else {
    rtb_sum_alpha = -rtb_Product;
  }

  // Sum: '<S36>/Diff' incorporates:
  //   Switch: '<S36>/Switch'

  rtb_Diff -= rtb_sum_alpha;

  // Outputs for Atomic SubSystem: '<S1>/Park Transform'
  // Product: '<S42>/IProd Out' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S7>/a16'
  //   Gain: '<S1>/Gain'

  rtb_sum_alpha = -rtb_IaIbIc0[0] * rtu_ConfigurationParameters->CurLoopPID.I;

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // Switch: '<S33>/Switch1' incorporates:
  //   Constant: '<S33>/Clamping_zero'
  //   Constant: '<S33>/Constant'
  //   Constant: '<S33>/Constant2'
  //   RelationalOperator: '<S33>/fix for DT propagation issue'

  if (rtb_Diff > 0.0F) {
    tmp = 1;
  } else {
    tmp = -1;
  }

  // Switch: '<S33>/Switch2' incorporates:
  //   Constant: '<S33>/Clamping_zero'
  //   Constant: '<S33>/Constant3'
  //   Constant: '<S33>/Constant4'
  //   RelationalOperator: '<S33>/fix for DT propagation issue1'

  if (rtb_sum_alpha > 0.0F) {
    tmp_0 = 1;
  } else {
    tmp_0 = -1;
  }

  // Switch: '<S33>/Switch' incorporates:
  //   Constant: '<S33>/Clamping_zero'
  //   Logic: '<S33>/AND3'
  //   RelationalOperator: '<S33>/Equal1'
  //   RelationalOperator: '<S33>/Relational Operator'
  //   Switch: '<S33>/Switch1'
  //   Switch: '<S33>/Switch2'

  if ((rtb_Diff != 0.0F) && (tmp == tmp_0)) {
    // Switch: '<S33>/Switch' incorporates:
    //   Constant: '<S33>/Constant1'

    control_foc_B.Switch = 0.0F;
  } else {
    // Switch: '<S33>/Switch'
    control_foc_B.Switch = rtb_sum_alpha;
  }

  // End of Switch: '<S33>/Switch'

  // DiscreteIntegrator: '<S45>/Integrator'
  if (rtu_OuterOutputs->pid_reset && (control_foc_DW.Integrator_PrevResetState_k
       <= 0)) {
    control_foc_DW.Integrator_DSTATE_o = 0.0F;
  }

  // DiscreteIntegrator: '<S45>/Integrator'
  control_foc_B.Integrator_j = 1.82857148E-5F * control_foc_B.Switch +
    control_foc_DW.Integrator_DSTATE_o;

  // Sum: '<S55>/Sum'
  rtb_Diff = (rtb_Unary_Minus + control_foc_B.Integrator_j) + DProdOut;

  // Switch: '<S53>/Switch2' incorporates:
  //   RelationalOperator: '<S53>/LowerRelop1'

  if (!(rtb_Diff > rtb_Product)) {
    // Switch: '<S53>/Switch' incorporates:
    //   Gain: '<S1>/Gain2'
    //   RelationalOperator: '<S53>/UpperRelop'

    if (rtb_Diff < -rtb_Product) {
      rtb_Product = -rtb_Product;
    } else {
      rtb_Product = rtb_Diff;
    }

    // End of Switch: '<S53>/Switch'
  }

  // End of Switch: '<S53>/Switch2'

  // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
  // Switch: '<S63>/Switch' incorporates:
  //   Product: '<S5>/dsin'
  //   Product: '<S5>/qcos'
  //   Sum: '<S5>/sum_beta'

  rtb_IaIbIc0[0] = rtb_algDD_o2_n * rtb_SinCos_o2 + rtb_Product * rtb_SinCos_o1;

  // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'

  // Switch: '<S1>/Switch2' incorporates:
  //   Constant: '<S1>/Constant1'

  if (rtu_OuterOutputs->out_en) {
    // Gain: '<S1>/Gain3' incorporates:
    //   Product: '<S1>/Divide1'

    rtb_Unary_Minus = rtb_algDD_o2_n /
      rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F;

    // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
    // Gain: '<S4>/sqrt3_by_two' incorporates:
    //   Product: '<S5>/dcos'
    //   Product: '<S5>/qsin'
    //   Sum: '<S5>/sum_alpha'
    //   UnaryMinus: '<S63>/Unary_Minus'

    rtb_Diff = -(rtb_Product * rtb_SinCos_o2 - rtb_algDD_o2_n * rtb_SinCos_o1) *
      0.866025388F;

    // Gain: '<S4>/one_by_two' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S5>/a16'

    rtb_SinCos_o1 = 0.5F * rtb_IaIbIc0[0];

    // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'

    // Sum: '<S4>/add_c'
    rtb_SinCos_o2 = (0.0F - rtb_SinCos_o1) - rtb_Diff;

    // Sum: '<S4>/add_b'
    rtb_SinCos_o1 = rtb_Diff - rtb_SinCos_o1;

    // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
    // MinMax: '<S1>/Min1' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S5>/a16'

    if ((rtb_IaIbIc0[0] <= rtb_SinCos_o1) || rtIsNaNF(rtb_SinCos_o1)) {
      rtb_Diff = rtb_IaIbIc0[0];
    } else {
      rtb_Diff = rtb_SinCos_o1;
    }

    // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'
    if ((!(rtb_Diff <= rtb_SinCos_o2)) && (!rtIsNaNF(rtb_SinCos_o2))) {
      rtb_Diff = rtb_SinCos_o2;
    }

    // Saturate: '<S1>/Saturation1'
    if (rtb_Unary_Minus > 100.0F) {
      rtb_Unary_Minus = 100.0F;
    } else if (rtb_Unary_Minus < -100.0F) {
      rtb_Unary_Minus = -100.0F;
    }

    // End of Saturate: '<S1>/Saturation1'

    // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
    // Sum: '<S1>/Sum1' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S5>/a16'
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_Product = (rtb_IaIbIc0[0] - rtb_Diff) /
      rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F + 5.0F;

    // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'

    // Saturate: '<S1>/Saturation'
    if (rtb_Product > 100.0F) {
      rtb_Product = 100.0F;
    } else if (rtb_Product < 0.0F) {
      rtb_Product = 0.0F;
    }

    // Sum: '<S1>/Sum1' incorporates:
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_SinCos_o1 = (rtb_SinCos_o1 - rtb_Diff) /
      rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F + 5.0F;

    // Saturate: '<S1>/Saturation'
    if (rtb_SinCos_o1 > 100.0F) {
      rtb_SinCos_o1 = 100.0F;
    } else if (rtb_SinCos_o1 < 0.0F) {
      rtb_SinCos_o1 = 0.0F;
    }

    // Sum: '<S1>/Sum1' incorporates:
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_SinCos_o2 = (rtb_SinCos_o2 - rtb_Diff) /
      rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F + 5.0F;

    // Saturate: '<S1>/Saturation'
    if (rtb_SinCos_o2 > 100.0F) {
      rtb_SinCos_o2 = 100.0F;
    } else if (rtb_SinCos_o2 < 0.0F) {
      rtb_SinCos_o2 = 0.0F;
    }
  } else {
    rtb_Unary_Minus = 0.0F;
    rtb_Product = 0.0F;
    rtb_SinCos_o1 = 0.0F;
    rtb_SinCos_o2 = 0.0F;
  }

  // End of Switch: '<S1>/Switch2'

  // BusCreator: '<S1>/Bus Creator2'
  rty_FOCOutputs->Id_fbk.current = rtb_algDD_o1_p;

  // BusCreator: '<S1>/Bus Creator'
  rty_FOCOutputs->Vq = rtb_Unary_Minus;
  rty_FOCOutputs->Vabc[0] = rtb_Product;
  rty_FOCOutputs->Vabc[1] = rtb_SinCos_o1;
  rty_FOCOutputs->Vabc[2] = rtb_SinCos_o2;

  // Sum: '<S1>/Sum3'
  control_foc_B.Sum3 = rtb_algDD_o2_n - rtb_Add;
}

// Update for atomic system: '<Root>/FOC inner loop'
void FOCInnerLoop_Update(const ControlOuterOutputs *rtu_OuterOutputs)
{
  // Update for DiscreteTransferFcn: '<S90>/Filter Differentiator TF'
  control_foc_DW.FilterDifferentiatorTF_states =
    control_foc_DW.FilterDifferentiatorTF_tmp;

  // Update for UnitDelay: '<S1>/Unit Delay'
  control_foc_DW.UnitDelay_DSTATE = control_foc_B.Sum3;

  // Update for DiscreteIntegrator: '<S97>/Integrator'
  control_foc_DW.Integrator_DSTATE = 1.82857148E-5F * control_foc_B.SumI1 +
    control_foc_B.Integrator;
  control_foc_DW.Integrator_PrevResetState = static_cast<int8_T>
    (rtu_OuterOutputs->pid_reset);

  // Update for DiscreteTransferFcn: '<S38>/Filter Differentiator TF'
  control_foc_DW.FilterDifferentiatorTF_states_k =
    control_foc_DW.FilterDifferentiatorTF_tmp_c;

  // Update for DiscreteIntegrator: '<S45>/Integrator' incorporates:
  //   DiscreteIntegrator: '<S97>/Integrator'

  control_foc_DW.Integrator_DSTATE_o = 1.82857148E-5F * control_foc_B.Switch +
    control_foc_B.Integrator_j;
  control_foc_DW.Integrator_PrevResetState_k = static_cast<int8_T>
    (rtu_OuterOutputs->pid_reset);
}

//
// File trailer for generated code.
//
// [EOF]
//
