//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_outer.cpp
//
// Code generated for Simulink model 'control_outer'.
//
// Model version                  : 4.0
// Simulink Coder version         : 9.8 (R2022b) 13-May-2022
// C/C++ source code generated on : Mon Mar 13 14:26:26 2023
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

  // InitializeConditions for DiscreteIntegrator: '<S154>/Integrator'
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
  int32_T tmp;
  real32_T rtb_Abs;
  real32_T rtb_DProdOut;
  real32_T rtb_DProdOut_f;
  real32_T rtb_DenCoefOut;
  real32_T rtb_FilterDifferentiatorTF;
  real32_T rtb_FilterDifferentiatorTF_n;
  real32_T rtb_Gain1_h;
  real32_T rtb_PProdOut;
  real32_T rtb_PProdOut_o;
  real32_T rtb_Product;
  real32_T rtb_Switch2_f;
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
  rtb_DenCoefOut = rtu_Targets->jointpositions.position -
    rtu_Sensors->jointpositions.position;

  // Product: '<S59>/PProd Out'
  rtb_PProdOut = rtb_DenCoefOut * rtu_ConfigurationParameters->PosLoopPID.P;

  // SampleTimeMath: '<S49>/Tsamp'
  //
  //  About '<S49>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_Product = rtu_ConfigurationParameters->PosLoopPID.N * 0.0005F;

  // Math: '<S47>/Reciprocal' incorporates:
  //   Constant: '<S47>/Filter Den Constant'
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
  //   Constant: '<S47>/Filter Den Constant'
  //   Product: '<S46>/DProd Out'
  //   Product: '<S47>/Divide'
  //   Sum: '<S47>/SumNum'

  if (rtb_FixPtRelationalOperator &&
      (control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC != POS_ZCSIG)) {
    control_outer_DW.FilterDifferentiatorTF_states = 0.0F;
  }

  control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC =
    rtb_FixPtRelationalOperator;
  control_outer_DW.FilterDifferentiatorTF_tmp = rtb_DenCoefOut *
    rtu_ConfigurationParameters->PosLoopPID.D - (rtb_Product - 1.0F) *
    rtb_Switch2_f * control_outer_DW.FilterDifferentiatorTF_states;

  // Product: '<S57>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S47>/Filter Differentiator TF'
  //   Product: '<S47>/DenCoefOut'

  rtb_Switch2_f = (control_outer_DW.FilterDifferentiatorTF_tmp -
                   control_outer_DW.FilterDifferentiatorTF_states) *
    rtb_Switch2_f * rtu_ConfigurationParameters->PosLoopPID.N;

  // Sum: '<S66>/SumI1' incorporates:
  //   Product: '<S51>/IProd Out'
  //   Sum: '<S64>/Sum Fdbk'
  //   Sum: '<S65>/SumI3'
  //   UnitDelay: '<Root>/Unit Delay'

  rtb_DProdOut = (control_outer_DW.UnitDelay_DSTATE - ((rtb_PProdOut +
    control_outer_DW.Integrator_DSTATE) + rtb_Switch2_f)) + rtb_DenCoefOut *
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
  rtb_PProdOut_o = rtb_DenCoefOut * rtu_ConfigurationParameters->DirLoopPID.P;

  // Product: '<S101>/IProd Out'
  rtb_Product = rtb_DenCoefOut * rtu_ConfigurationParameters->DirLoopPID.I;

  // Product: '<S96>/DProd Out'
  rtb_DProdOut_f = rtb_DenCoefOut * rtu_ConfigurationParameters->DirLoopPID.D;

  // SampleTimeMath: '<S99>/Tsamp'
  //
  //  About '<S99>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_Gain1_h = rtu_ConfigurationParameters->DirLoopPID.N * 0.0005F;

  // Math: '<S97>/Reciprocal' incorporates:
  //   Constant: '<S97>/Filter Den Constant'
  //   Sum: '<S97>/SumDen'
  //
  //  About '<S97>/Reciprocal':
  //   Operator: reciprocal

  rtb_DenCoefOut = 1.0F / (rtb_Gain1_h + 1.0F);

  // DiscreteTransferFcn: '<S97>/Filter Differentiator TF' incorporates:
  //   Constant: '<S97>/Filter Den Constant'
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
    - 1.0F) * rtb_DenCoefOut * control_outer_DW.FilterDifferentiatorTF_states_i;

  // Product: '<S107>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S97>/Filter Differentiator TF'
  //   Product: '<S97>/DenCoefOut'

  rtb_DenCoefOut = (control_outer_DW.FilterDifferentiatorTF_tmp_m -
                    control_outer_DW.FilterDifferentiatorTF_states_i) *
    rtb_DenCoefOut * rtu_ConfigurationParameters->DirLoopPID.N;

  // Sum: '<S116>/SumI1' incorporates:
  //   Sum: '<S114>/Sum Fdbk'
  //   Sum: '<S115>/SumI3'
  //   UnitDelay: '<Root>/Unit Delay'

  rtb_FilterDifferentiatorTF_n = (control_outer_DW.UnitDelay_DSTATE -
    ((rtb_PProdOut_o + control_outer_DW.Integrator_DSTATE_i) + rtb_DenCoefOut))
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
      rtb_DenCoefOut = (rtb_PProdOut + rtb_FilterDifferentiatorTF) +
        rtb_Switch2_f;
    } else {
      rtb_DenCoefOut += rtb_PProdOut_o + rtb_DProdOut_f;
    }

    // End of Switch: '<S4>/Switch5'
  } else {
    rtb_DenCoefOut = 0.0F;
  }

  // Sum: '<Root>/Sum2' incorporates:
  //   Switch: '<Root>/Switch3'

  rtb_Switch2_f = rtb_DenCoefOut + rtu_Targets->jointvelocities.velocity;

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

  // Sum: '<Root>/Sum1'
  rtb_Product = rtb_Switch2_f - rtu_Estimates->jointvelocities.velocity;

  // Product: '<S159>/PProd Out'
  rtb_Abs = rtb_Product * rtu_ConfigurationParameters->VelLoopPID.P;

  // Product: '<S151>/IProd Out'
  rtb_Gain1_h = rtb_Product * rtu_ConfigurationParameters->VelLoopPID.I;

  // Product: '<S146>/DProd Out'
  rtb_PProdOut = rtb_Product * rtu_ConfigurationParameters->VelLoopPID.D;

  // SampleTimeMath: '<S149>/Tsamp'
  //
  //  About '<S149>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_Product = rtu_ConfigurationParameters->VelLoopPID.N * 0.0005F;

  // Math: '<S147>/Reciprocal' incorporates:
  //   Constant: '<S147>/Filter Den Constant'
  //   Sum: '<S147>/SumDen'
  //
  //  About '<S147>/Reciprocal':
  //   Operator: reciprocal

  rtb_DenCoefOut = 1.0F / (rtb_Product + 1.0F);

  // DiscreteTransferFcn: '<S147>/Filter Differentiator TF' incorporates:
  //   Constant: '<S147>/Filter Den Constant'
  //   DiscreteTransferFcn: '<S47>/Filter Differentiator TF'
  //   Product: '<S147>/Divide'
  //   Sum: '<S147>/SumNum'

  if (rtb_FixPtRelationalOperator &&
      (control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e != POS_ZCSIG)) {
    control_outer_DW.FilterDifferentiatorTF_states_c = 0.0F;
  }

  control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e =
    rtb_FixPtRelationalOperator;
  control_outer_DW.FilterDifferentiatorTF_tmp_p = rtb_PProdOut - (rtb_Product -
    1.0F) * rtb_DenCoefOut * control_outer_DW.FilterDifferentiatorTF_states_c;

  // Product: '<S157>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S147>/Filter Differentiator TF'
  //   Product: '<S147>/DenCoefOut'

  rtb_DenCoefOut = (control_outer_DW.FilterDifferentiatorTF_tmp_p -
                    control_outer_DW.FilterDifferentiatorTF_states_c) *
    rtb_DenCoefOut * rtu_ConfigurationParameters->VelLoopPID.N;

  // Sum: '<S166>/SumI1' incorporates:
  //   Sum: '<S164>/Sum Fdbk'
  //   Sum: '<S165>/SumI3'
  //   UnitDelay: '<Root>/Unit Delay1'

  rtb_PProdOut = (control_outer_DW.UnitDelay1_DSTATE - ((rtb_Abs +
    control_outer_DW.Integrator_DSTATE_b) + rtb_DenCoefOut)) + rtb_Gain1_h;

  // DiscreteIntegrator: '<S154>/Integrator'
  if (rtb_FixPtRelationalOperator &&
      (control_outer_DW.Integrator_PrevResetState_c <= 0)) {
    control_outer_DW.Integrator_DSTATE_b = 0.0F;
  }

  // DiscreteIntegrator: '<S154>/Integrator'
  rtb_Product = 0.0005F * rtb_PProdOut + control_outer_DW.Integrator_DSTATE_b;

  // Switch: '<Root>/Switch1' incorporates:
  //   Sum: '<S163>/Sum'

  if (rtb_Compare_m) {
    rtb_Abs = (rtb_Abs + rtb_Product) + rtb_DenCoefOut;
  } else {
    rtb_Abs = rtu_Targets->motorcurrent.current;
  }

  // End of Switch: '<Root>/Switch1'

  // Switch: '<S6>/Switch2' incorporates:
  //   Gain: '<Root>/Gain1'
  //   RelationalOperator: '<S6>/LowerRelop1'
  //   RelationalOperator: '<S6>/UpperRelop'
  //   Switch: '<S6>/Switch'

  if (rtb_Abs > rtu_ConfigurationParameters->thresholds.motorPeakCurrents) {
    rtb_Abs = rtu_ConfigurationParameters->thresholds.motorPeakCurrents;
  } else if (rtb_Abs <
             -rtu_ConfigurationParameters->thresholds.motorPeakCurrents) {
    // Switch: '<S6>/Switch' incorporates:
    //   Gain: '<Root>/Gain1'

    rtb_Abs = -rtu_ConfigurationParameters->thresholds.motorPeakCurrents;
  }

  // End of Switch: '<S6>/Switch2'

  // BusCreator: '<Root>/Bus Creator2'
  rty_OuterOutputs->motorcurrent.current = rtb_Abs;

  // Sum: '<S1>/Sum' incorporates:
  //   Abs: '<S1>/Abs1'

  rtb_DenCoefOut = rtu_ConfigurationParameters->thresholds.motorPeakCurrents -
    std::abs(rtu_Estimates->Iq_filtered.current);

  // CombinatorialLogic: '<S10>/Logic' incorporates:
  //   Constant: '<S8>/Constant'
  //   Gain: '<S1>/Gain1'
  //   Logic: '<S1>/Logical Operator'
  //   Memory: '<S10>/Memory'
  //   RelationalOperator: '<S1>/Relational Operator'
  //   RelationalOperator: '<S8>/Compare'

  rowIdx = static_cast<int32_T>(((((rtb_DenCoefOut > 0.1F *
    rtu_ConfigurationParameters->thresholds.motorPeakCurrents) ||
    rtb_FixPtRelationalOperator) + (static_cast<uint32_T>(rtb_DenCoefOut <= 0.0F)
    << 1)) << 1) + control_outer_DW.Memory_PreviousInput);
  rtb_Compare = rtCP_Logic_table[static_cast<uint32_T>(rowIdx) + 8U];

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
    control_outer_B.DiscreteTimeIntegrator = 0.0005F * rtb_DenCoefOut +
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
    tmp = -1;
  } else {
    tmp = 1;
  }

  // BusCreator: '<Root>/Bus Creator1' incorporates:
  //   Product: '<S1>/Product'
  //   Switch: '<S1>/Switch1'

  rty_OuterOutputs->current_limiter = static_cast<real32_T>(tmp) *
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

  // Update for DiscreteTransferFcn: '<S147>/Filter Differentiator TF'
  control_outer_DW.FilterDifferentiatorTF_states_c =
    control_outer_DW.FilterDifferentiatorTF_tmp_p;

  // Update for UnitDelay: '<Root>/Unit Delay1'
  control_outer_DW.UnitDelay1_DSTATE = rtb_Abs;

  // Update for DiscreteIntegrator: '<S154>/Integrator' incorporates:
  //   DiscreteIntegrator: '<S54>/Integrator'

  control_outer_DW.Integrator_DSTATE_b = 0.0005F * rtb_PProdOut + rtb_Product;
  control_outer_DW.Integrator_PrevResetState_c = static_cast<int8_T>
    (rtb_FixPtRelationalOperator);

  // Update for Memory: '<S10>/Memory' incorporates:
  //   CombinatorialLogic: '<S10>/Logic'

  control_outer_DW.Memory_PreviousInput = rtCP_Logic_table[static_cast<uint32_T>
    (rowIdx)];

  // Update for DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
  control_outer_DW.DiscreteTimeIntegrator_SYSTEM_E = 0U;
  control_outer_DW.DiscreteTimeIntegrator_DSTATE = 0.0005F * rtb_DenCoefOut +
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
