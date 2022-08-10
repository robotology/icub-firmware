//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_outer.cpp
//
// Code generated for Simulink model 'control_outer'.
//
// Model version                  : 3.31
// Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
// C/C++ source code generated on : Tue Aug  9 15:18:47 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "control_outer.h"
#include "control_outer_types.h"
#include <cmath>
#include "rtwtypes.h"
#include "control_outer_private.h"
#include "zero_crossing_types.h"

MdlrefDW_control_outer_T control_outer_MdlrefDW;

// Block signals (default storage)
B_control_outer_c_T control_outer_B;

// Block states (default storage)
DW_control_outer_f_T control_outer_DW;

// Previous zero-crossings (trigger) states
ZCE_control_outer_T control_outer_PrevZCX;

// System initialize for referenced model: 'control_outer'
void control_outer_Init(void)
{
  // InitializeConditions for UnitDelay: '<S2>/Delay Input1'
  //
  //  Block description for '<S2>/Delay Input1':
  //
  //   Store in Global RAM

  control_outer_DW.DelayInput1_DSTATE = ControlModes_Idle;

  // InitializeConditions for DiscreteIntegrator: '<S54>/Integrator'
  control_outer_DW.Integrator_PrevResetState = 2;

  // InitializeConditions for DiscreteIntegrator: '<S104>/Integrator'
  control_outer_DW.Integrator_PrevResetState_n = 2;

  // InitializeConditions for DiscreteIntegrator: '<S156>/Integrator'
  control_outer_DW.Integrator_PrevResetState_c = 2;
}

// Enable for referenced model: 'control_outer'
void control_outer_Enable(void)
{
  // Enable for DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
  control_outer_DW.DiscreteTimeIntegrator_SYSTEM_E = 1U;
}

// Disable for referenced model: 'control_outer'
void control_outer_Disable(void)
{
  // Disable for DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
  control_outer_DW.DiscreteTimeIntegrator_DSTATE =
    control_outer_B.DiscreteTimeIntegrator;
}

// Output and update for referenced model: 'control_outer'
void control_outer(const Flags *rtu_Flags, const ConfigurationParameters
                   *rtu_ConfigurationParameters, const Targets *rtu_Targets,
                   const SensorsData *rtu_Sensors, const EstimatedData
                   *rtu_Estimates, ControlOuterOutputs *rty_OuterOutputs)
{
  int32_T rowIdx;
  int32_T rtu_Estimates_0;
  real32_T rtb_Abs;
  real32_T rtb_DProdOut;
  real32_T rtb_DProdOut_f;
  real32_T rtb_FilterDifferentiatorTF;
  real32_T rtb_FilterDifferentiatorTF_n;
  real32_T rtb_Gain1_h;
  real32_T rtb_PProdOut;
  real32_T rtb_PProdOut_o;
  real32_T rtb_Product;
  real32_T rtb_Switch2_f;
  real32_T rtb_UnitDelay1;
  int8_T rtb_Product_0;
  int8_T rtb_UnitDelay1_0;
  boolean_T rtb_Compare;
  boolean_T rtb_Compare_m;
  boolean_T rtb_FixPtRelationalOperator;

  // Abs: '<Root>/Abs'
  rtb_Abs = std::abs(rtu_ConfigurationParameters->thresholds.jntVelMax);

  // RelationalOperator: '<S13>/Compare' incorporates:
  //   Constant: '<S13>/Constant'

  rtb_Compare_m = (rtu_Flags->control_mode != ControlModes_Current);

  // RelationalOperator: '<S11>/Compare' incorporates:
  //   Constant: '<S11>/Constant'

  rty_OuterOutputs->cur_en = (rtu_Flags->control_mode != ControlModes_Voltage);

  // Logic: '<S3>/NOR' incorporates:
  //   Constant: '<S14>/Constant'
  //   Constant: '<S16>/Constant'
  //   Constant: '<S17>/Constant'
  //   RelationalOperator: '<S14>/Compare'
  //   RelationalOperator: '<S16>/Compare'
  //   RelationalOperator: '<S17>/Compare'

  rty_OuterOutputs->out_en = ((rtu_Flags->control_mode !=
    ControlModes_NotConfigured) && (rtu_Flags->control_mode != ControlModes_Idle)
    && (rtu_Flags->control_mode != ControlModes_HwFaultCM));

  // Sum: '<S4>/Sum3'
  rtb_UnitDelay1 = rtu_Targets->jointpositions.position -
    rtu_Sensors->jointpositions.position;

  // Product: '<S59>/PProd Out'
  rtb_PProdOut = rtb_UnitDelay1 * rtu_ConfigurationParameters->PosLoopPID.P;

  // SampleTimeMath: '<S49>/Tsamp'
  //
  //  About '<S49>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_Product = rtu_ConfigurationParameters->PosLoopPID.N * 0.0005F;

  // Math: '<S47>/Reciprocal' incorporates:
  //   Constant: '<S47>/Constant'
  //   Sum: '<S47>/SumDen'
  //
  //  About '<S47>/Reciprocal':
  //   Operator: reciprocal

  rtb_Switch2_f = 1.0F / (rtb_Product + 1.0F);

  // RelationalOperator: '<S2>/FixPt Relational Operator' incorporates:
  //   UnitDelay: '<S2>/Delay Input1'
  //
  //  Block description for '<S2>/Delay Input1':
  //
  //   Store in Global RAM

  rtb_FixPtRelationalOperator = (rtu_Flags->control_mode !=
    control_outer_DW.DelayInput1_DSTATE);

  // DiscreteTransferFcn: '<S47>/Filter Differentiator TF' incorporates:
  //   Constant: '<S47>/Constant'
  //   Product: '<S46>/DProd Out'
  //   Product: '<S47>/Divide'
  //   Sum: '<S47>/SumNum'

  if (rtb_FixPtRelationalOperator &&
      (control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC != POS_ZCSIG)) {
    control_outer_DW.FilterDifferentiatorTF_states = 0.0F;
  }

  control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC =
    rtb_FixPtRelationalOperator;
  control_outer_DW.FilterDifferentiatorTF_tmp = rtb_UnitDelay1 *
    rtu_ConfigurationParameters->PosLoopPID.D - (rtb_Product - 1.0F) *
    rtb_Switch2_f * control_outer_DW.FilterDifferentiatorTF_states;

  // Product: '<S57>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S47>/Filter Differentiator TF'
  //   Product: '<S47>/DenCoefOut'

  rtb_Switch2_f = (control_outer_DW.FilterDifferentiatorTF_tmp +
                   -control_outer_DW.FilterDifferentiatorTF_states) *
    rtb_Switch2_f * rtu_ConfigurationParameters->PosLoopPID.N;

  // Sum: '<S66>/SumI1' incorporates:
  //   Product: '<S51>/IProd Out'
  //   Sum: '<S64>/Sum Fdbk'
  //   Sum: '<S65>/SumI3'
  //   UnitDelay: '<Root>/Unit Delay'

  rtb_DProdOut = (control_outer_DW.UnitDelay_DSTATE - ((rtb_PProdOut +
    control_outer_DW.Integrator_DSTATE) + rtb_Switch2_f)) + rtb_UnitDelay1 *
    rtu_ConfigurationParameters->PosLoopPID.I;

  // DiscreteIntegrator: '<S54>/Integrator'
  if (rtb_FixPtRelationalOperator && (control_outer_DW.Integrator_PrevResetState
       <= 0)) {
    control_outer_DW.Integrator_DSTATE = 0.0F;
  }

  // DiscreteIntegrator: '<S54>/Integrator'
  rtb_FilterDifferentiatorTF = 0.0005F * rtb_DProdOut +
    control_outer_DW.Integrator_DSTATE;

  // RelationalOperator: '<S12>/Compare' incorporates:
  //   Constant: '<S12>/Constant'

  rtb_Compare = (rtu_Flags->control_mode == ControlModes_Position);

  // Product: '<S109>/PProd Out'
  rtb_PProdOut_o = rtb_UnitDelay1 * rtu_ConfigurationParameters->DirLoopPID.P;

  // Product: '<S101>/IProd Out'
  rtb_Product = rtb_UnitDelay1 * rtu_ConfigurationParameters->DirLoopPID.I;

  // Product: '<S96>/DProd Out'
  rtb_DProdOut_f = rtb_UnitDelay1 * rtu_ConfigurationParameters->DirLoopPID.D;

  // SampleTimeMath: '<S99>/Tsamp'
  //
  //  About '<S99>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_Gain1_h = rtu_ConfigurationParameters->DirLoopPID.N * 0.0005F;

  // Math: '<S97>/Reciprocal' incorporates:
  //   Constant: '<S97>/Constant'
  //   Sum: '<S97>/SumDen'
  //
  //  About '<S97>/Reciprocal':
  //   Operator: reciprocal

  rtb_UnitDelay1 = 1.0F / (rtb_Gain1_h + 1.0F);

  // DiscreteTransferFcn: '<S97>/Filter Differentiator TF' incorporates:
  //   Constant: '<S97>/Constant'
  //   DiscreteTransferFcn: '<S47>/Filter Differentiator TF'
  //   Product: '<S97>/Divide'
  //   Sum: '<S97>/SumNum'

  if (rtb_FixPtRelationalOperator &&
      (control_outer_PrevZCX.FilterDifferentiatorTF_Reset__m != POS_ZCSIG)) {
    control_outer_DW.FilterDifferentiatorTF_states_i = 0.0F;
  }

  control_outer_PrevZCX.FilterDifferentiatorTF_Reset__m =
    rtb_FixPtRelationalOperator;
  control_outer_DW.FilterDifferentiatorTF_tmp_m = rtb_DProdOut_f - (rtb_Gain1_h
    - 1.0F) * rtb_UnitDelay1 * control_outer_DW.FilterDifferentiatorTF_states_i;

  // Product: '<S107>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S97>/Filter Differentiator TF'
  //   Product: '<S97>/DenCoefOut'

  rtb_UnitDelay1 = (control_outer_DW.FilterDifferentiatorTF_tmp_m +
                    -control_outer_DW.FilterDifferentiatorTF_states_i) *
    rtb_UnitDelay1 * rtu_ConfigurationParameters->DirLoopPID.N;

  // Sum: '<S116>/SumI1' incorporates:
  //   Sum: '<S114>/Sum Fdbk'
  //   Sum: '<S115>/SumI3'
  //   UnitDelay: '<Root>/Unit Delay'

  rtb_FilterDifferentiatorTF_n = (control_outer_DW.UnitDelay_DSTATE -
    ((rtb_PProdOut_o + control_outer_DW.Integrator_DSTATE_i) + rtb_UnitDelay1))
    + rtb_Product;

  // DiscreteIntegrator: '<S104>/Integrator'
  if (rtb_FixPtRelationalOperator &&
      (control_outer_DW.Integrator_PrevResetState_n <= 0)) {
    control_outer_DW.Integrator_DSTATE_i = 0.0F;
  }

  // DiscreteIntegrator: '<S104>/Integrator'
  rtb_DProdOut_f = 0.0005F * rtb_FilterDifferentiatorTF_n +
    control_outer_DW.Integrator_DSTATE_i;

  // Switch: '<Root>/Switch3' incorporates:
  //   Constant: '<Root>/Constant1'
  //   Constant: '<S15>/Constant'
  //   Constant: '<S18>/Constant'
  //   Logic: '<S3>/OR'
  //   RelationalOperator: '<S15>/Compare'
  //   RelationalOperator: '<S18>/Compare'

  if (rtb_Compare || (rtu_Flags->control_mode == ControlModes_PositionDirect) ||
      (rtu_Flags->control_mode == ControlModes_Velocity)) {
    // Switch: '<S4>/Switch5' incorporates:
    //   Sum: '<S113>/Sum'
    //   Sum: '<S63>/Sum'

    if (rtb_Compare) {
      rtb_UnitDelay1 = (rtb_PProdOut + rtb_FilterDifferentiatorTF) +
        rtb_Switch2_f;
    } else {
      rtb_UnitDelay1 += rtb_PProdOut_o + rtb_DProdOut_f;
    }

    // End of Switch: '<S4>/Switch5'
  } else {
    rtb_UnitDelay1 = 0.0F;
  }

  // End of Switch: '<Root>/Switch3'

  // Sum: '<Root>/Sum2'
  rtb_Switch2_f = rtb_UnitDelay1 + rtu_Targets->jointvelocities.velocity;

  // Switch: '<S5>/Switch2' incorporates:
  //   Gain: '<Root>/Gain'
  //   RelationalOperator: '<S5>/LowerRelop1'
  //   RelationalOperator: '<S5>/UpperRelop'
  //   Switch: '<S5>/Switch'

  if (rtb_Switch2_f > rtb_Abs) {
    rtb_Switch2_f = rtb_Abs;
  } else if (rtb_Switch2_f < -rtb_Abs) {
    // Switch: '<S5>/Switch' incorporates:
    //   Gain: '<Root>/Gain'

    rtb_Switch2_f = -rtb_Abs;
  }

  // End of Switch: '<S5>/Switch2'

  // Product: '<Root>/Product2' incorporates:
  //   Sum: '<Root>/Sum1'

  rtb_Product = (rtb_Switch2_f - rtu_Estimates->jointvelocities.velocity) *
    rtu_ConfigurationParameters->motorconfig.reduction;

  // Product: '<S161>/PProd Out'
  rtb_Abs = rtb_Product * rtu_ConfigurationParameters->VelLoopPID.P;

  // SampleTimeMath: '<S151>/Tsamp'
  //
  //  About '<S151>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_Gain1_h = rtu_ConfigurationParameters->VelLoopPID.N * 0.0005F;

  // Math: '<S149>/Reciprocal' incorporates:
  //   Constant: '<S149>/Constant'
  //   Sum: '<S149>/SumDen'
  //
  //  About '<S149>/Reciprocal':
  //   Operator: reciprocal

  rtb_UnitDelay1 = 1.0F / (rtb_Gain1_h + 1.0F);

  // DiscreteTransferFcn: '<S149>/Filter Differentiator TF' incorporates:
  //   Constant: '<S149>/Constant'
  //   DiscreteTransferFcn: '<S47>/Filter Differentiator TF'
  //   Product: '<S148>/DProd Out'
  //   Product: '<S149>/Divide'
  //   Sum: '<S149>/SumNum'

  if (rtb_FixPtRelationalOperator &&
      (control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e != POS_ZCSIG)) {
    control_outer_DW.FilterDifferentiatorTF_states_c = 0.0F;
  }

  control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e =
    rtb_FixPtRelationalOperator;
  control_outer_DW.FilterDifferentiatorTF_tmp_p = rtb_Product *
    rtu_ConfigurationParameters->VelLoopPID.D - (rtb_Gain1_h - 1.0F) *
    rtb_UnitDelay1 * control_outer_DW.FilterDifferentiatorTF_states_c;

  // Product: '<S159>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S149>/Filter Differentiator TF'
  //   Product: '<S149>/DenCoefOut'

  rtb_PProdOut = (control_outer_DW.FilterDifferentiatorTF_tmp_p +
                  -control_outer_DW.FilterDifferentiatorTF_states_c) *
    rtb_UnitDelay1 * rtu_ConfigurationParameters->VelLoopPID.N;

  // Sum: '<S166>/Sum Fdbk'
  rtb_Gain1_h = (rtb_Abs + control_outer_DW.Integrator_DSTATE_b) + rtb_PProdOut;

  // DeadZone: '<S147>/DeadZone' incorporates:
  //   Saturate: '<S164>/Saturation'

  if (rtb_Gain1_h > 100.0F) {
    rtb_UnitDelay1 = rtb_Gain1_h - 100.0F;
    rtb_Gain1_h = 100.0F;
  } else {
    if (rtb_Gain1_h >= -100.0F) {
      rtb_UnitDelay1 = 0.0F;
    } else {
      rtb_UnitDelay1 = rtb_Gain1_h - -100.0F;
    }

    if (rtb_Gain1_h < -100.0F) {
      rtb_Gain1_h = -100.0F;
    }
  }

  // End of DeadZone: '<S147>/DeadZone'

  // Sum: '<S168>/SumI1' incorporates:
  //   Product: '<S153>/IProd Out'
  //   Sum: '<S167>/SumI3'
  //   UnitDelay: '<Root>/Unit Delay1'

  rtb_Product = rtb_Product * rtu_ConfigurationParameters->VelLoopPID.I +
    (control_outer_DW.UnitDelay1_DSTATE - rtb_Gain1_h);

  // Switch: '<S145>/Switch1' incorporates:
  //   Constant: '<S145>/Constant'
  //   Constant: '<S145>/Constant2'
  //   Constant: '<S145>/Constant5'
  //   RelationalOperator: '<S145>/fix for DT propagation issue'

  if (rtb_UnitDelay1 > 0.0F) {
    rtb_UnitDelay1_0 = 1;
  } else {
    rtb_UnitDelay1_0 = -1;
  }

  // End of Switch: '<S145>/Switch1'

  // Switch: '<S145>/Switch2' incorporates:
  //   Constant: '<S145>/Constant3'
  //   Constant: '<S145>/Constant4'
  //   Constant: '<S145>/Constant5'
  //   RelationalOperator: '<S145>/fix for DT propagation issue1'

  if (rtb_Product > 0.0F) {
    rtb_Product_0 = 1;
  } else {
    rtb_Product_0 = -1;
  }

  // End of Switch: '<S145>/Switch2'

  // Switch: '<S145>/Switch' incorporates:
  //   Constant: '<S145>/Constant1'
  //   Constant: '<S145>/Constant5'
  //   Logic: '<S145>/AND3'
  //   RelationalOperator: '<S145>/Equal1'
  //   RelationalOperator: '<S145>/Relational Operator'

  if ((rtb_UnitDelay1 != 0.0F) && (rtb_UnitDelay1_0 == rtb_Product_0)) {
    rtb_PProdOut_o = 0.0F;
  } else {
    rtb_PProdOut_o = rtb_Product;
  }

  // End of Switch: '<S145>/Switch'

  // DiscreteIntegrator: '<S156>/Integrator'
  if (rtb_FixPtRelationalOperator &&
      (control_outer_DW.Integrator_PrevResetState_c <= 0)) {
    control_outer_DW.Integrator_DSTATE_b = 0.0F;
  }

  // DiscreteIntegrator: '<S156>/Integrator'
  rtb_Product = 0.0005F * rtb_PProdOut_o + control_outer_DW.Integrator_DSTATE_b;

  // Switch: '<Root>/Switch1'
  if (rtb_Compare_m) {
    // Sum: '<S165>/Sum'
    rtb_UnitDelay1 = (rtb_Abs + rtb_Product) + rtb_PProdOut;

    // Saturate: '<S163>/Saturation'
    if (rtb_UnitDelay1 > 100.0F) {
      rtb_UnitDelay1 = 100.0F;
    } else if (rtb_UnitDelay1 < -100.0F) {
      rtb_UnitDelay1 = -100.0F;
    }

    // End of Saturate: '<S163>/Saturation'
  } else {
    rtb_UnitDelay1 = rtu_Targets->motorcurrent.current;
  }

  // End of Switch: '<Root>/Switch1'

  // Switch: '<S6>/Switch2' incorporates:
  //   Gain: '<Root>/Gain1'
  //   RelationalOperator: '<S6>/LowerRelop1'
  //   RelationalOperator: '<S6>/UpperRelop'
  //   Switch: '<S6>/Switch'

  if (rtb_UnitDelay1 > rtu_ConfigurationParameters->thresholds.motorPeakCurrents)
  {
    rtb_UnitDelay1 = rtu_ConfigurationParameters->thresholds.motorPeakCurrents;
  } else if (rtb_UnitDelay1 <
             -rtu_ConfigurationParameters->thresholds.motorPeakCurrents) {
    // Switch: '<S6>/Switch' incorporates:
    //   Gain: '<Root>/Gain1'

    rtb_UnitDelay1 = -rtu_ConfigurationParameters->thresholds.motorPeakCurrents;
  }

  // End of Switch: '<S6>/Switch2'

  // BusCreator: '<Root>/Bus Creator2'
  rty_OuterOutputs->motorcurrent.current = rtb_UnitDelay1;

  // Sum: '<S1>/Sum' incorporates:
  //   Abs: '<S1>/Abs1'

  rtb_Abs = rtu_ConfigurationParameters->thresholds.motorPeakCurrents - std::abs
    (rtu_Estimates->Iq_filtered.current);

  // CombinatorialLogic: '<S10>/Logic' incorporates:
  //   Constant: '<S8>/Constant'
  //   Gain: '<S1>/Gain1'
  //   Logic: '<S1>/Logical Operator'
  //   Memory: '<S10>/Memory'
  //   RelationalOperator: '<S1>/Relational Operator'
  //   RelationalOperator: '<S8>/Compare'

  rowIdx = static_cast<int32_T>(((((rtb_Abs > 0.05F *
    rtu_ConfigurationParameters->thresholds.motorPeakCurrents) ||
    rtb_FixPtRelationalOperator) + (static_cast<uint32_T>(rtb_Abs <= 0.0F) << 1))
    << 1) + control_outer_DW.Memory_PreviousInput);
  rtb_Compare = rtCP_Logic_table[rowIdx + 8U];

  // DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
  if (control_outer_DW.DiscreteTimeIntegrator_SYSTEM_E != 0) {
    // DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
    control_outer_B.DiscreteTimeIntegrator =
      control_outer_DW.DiscreteTimeIntegrator_DSTATE;
  } else if (rtb_Compare || (control_outer_DW.DiscreteTimeIntegrator_PrevRese !=
              0)) {
    // DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
    control_outer_B.DiscreteTimeIntegrator = 0.0F;
  } else {
    // DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
    control_outer_B.DiscreteTimeIntegrator = 0.0005F * rtb_Abs +
      control_outer_DW.DiscreteTimeIntegrator_DSTATE;
  }

  // End of DiscreteIntegrator: '<S1>/Discrete-Time Integrator'

  // BusCreator: '<Root>/Bus Creator1'
  rty_OuterOutputs->vel_en = rtb_Compare_m;

  // Switch: '<S1>/Switch1' incorporates:
  //   Constant: '<S1>/Constant1'
  //   Constant: '<S1>/Constant2'
  //   Constant: '<S9>/Constant'
  //   RelationalOperator: '<S9>/Compare'

  if (rtu_Estimates->Iq_filtered.current < 0.0F) {
    rtu_Estimates_0 = -1;
  } else {
    rtu_Estimates_0 = 1;
  }

  // End of Switch: '<S1>/Switch1'

  // BusCreator: '<Root>/Bus Creator1' incorporates:
  //   Product: '<S1>/Product'

  rty_OuterOutputs->current_limiter = static_cast<real32_T>(rtu_Estimates_0) *
    control_outer_B.DiscreteTimeIntegrator *
    rtu_ConfigurationParameters->CurLoopPID.I;

  // Update for UnitDelay: '<S2>/Delay Input1'
  //
  //  Block description for '<S2>/Delay Input1':
  //
  //   Store in Global RAM

  control_outer_DW.DelayInput1_DSTATE = rtu_Flags->control_mode;

  // Update for DiscreteTransferFcn: '<S47>/Filter Differentiator TF'
  control_outer_DW.FilterDifferentiatorTF_states =
    control_outer_DW.FilterDifferentiatorTF_tmp;

  // Update for UnitDelay: '<Root>/Unit Delay' incorporates:
  //   Sum: '<Root>/Sum3'

  control_outer_DW.UnitDelay_DSTATE = rtb_Switch2_f -
    rtu_Targets->jointvelocities.velocity;

  // Update for DiscreteIntegrator: '<S54>/Integrator'
  control_outer_DW.Integrator_DSTATE = 0.0005F * rtb_DProdOut +
    rtb_FilterDifferentiatorTF;
  control_outer_DW.Integrator_PrevResetState = static_cast<int8_T>
    (rtb_FixPtRelationalOperator);

  // Update for DiscreteTransferFcn: '<S97>/Filter Differentiator TF'
  control_outer_DW.FilterDifferentiatorTF_states_i =
    control_outer_DW.FilterDifferentiatorTF_tmp_m;

  // Update for DiscreteIntegrator: '<S104>/Integrator' incorporates:
  //   DiscreteIntegrator: '<S54>/Integrator'

  control_outer_DW.Integrator_DSTATE_i = 0.0005F * rtb_FilterDifferentiatorTF_n
    + rtb_DProdOut_f;
  control_outer_DW.Integrator_PrevResetState_n = static_cast<int8_T>
    (rtb_FixPtRelationalOperator);

  // Update for DiscreteTransferFcn: '<S149>/Filter Differentiator TF'
  control_outer_DW.FilterDifferentiatorTF_states_c =
    control_outer_DW.FilterDifferentiatorTF_tmp_p;

  // Update for UnitDelay: '<Root>/Unit Delay1'
  control_outer_DW.UnitDelay1_DSTATE = rtb_UnitDelay1;

  // Update for DiscreteIntegrator: '<S156>/Integrator' incorporates:
  //   DiscreteIntegrator: '<S54>/Integrator'

  control_outer_DW.Integrator_DSTATE_b = 0.0005F * rtb_PProdOut_o + rtb_Product;
  control_outer_DW.Integrator_PrevResetState_c = static_cast<int8_T>
    (rtb_FixPtRelationalOperator);

  // Update for Memory: '<S10>/Memory' incorporates:
  //   CombinatorialLogic: '<S10>/Logic'

  control_outer_DW.Memory_PreviousInput = rtCP_Logic_table[static_cast<uint32_T>
    (rowIdx)];

  // Update for DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
  control_outer_DW.DiscreteTimeIntegrator_SYSTEM_E = 0U;
  control_outer_DW.DiscreteTimeIntegrator_DSTATE = 0.0005F * rtb_Abs +
    control_outer_B.DiscreteTimeIntegrator;
  control_outer_DW.DiscreteTimeIntegrator_PrevRese = static_cast<int8_T>
    (rtb_Compare);
}

// Model initialize function
void control_outer_initialize(const char_T **rt_errorStatus)
{
  RT_MODEL_control_outer_T *const control_outer_M = &(control_outer_MdlrefDW.rtm);

  // Registration code

  // initialize error status
  rtmSetErrorStatusPointer(control_outer_M, rt_errorStatus);
  control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC = POS_ZCSIG;
  control_outer_PrevZCX.FilterDifferentiatorTF_Reset__m = POS_ZCSIG;
  control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e = POS_ZCSIG;
}

//
// File trailer for generated code.
//
// [EOF]
//
