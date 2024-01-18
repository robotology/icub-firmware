//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_outer.cpp
//
// Code generated for Simulink model 'control_outer'.
//
// Model version                  : 6.3
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Mon Jan 15 14:41:42 2024
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
#include <cstring>

// System initialize for referenced model: 'control_outer'
void control_outer_Init(DW_control_outer_f_T *localDW)
{
  // InitializeConditions for UnitDelay: '<S18>/Delay Input1'
  //
  //  Block description for '<S18>/Delay Input1':
  //
  //   Store in Global RAM

  localDW->DelayInput1_DSTATE = ControlModes_Idle;

  // InitializeConditions for DiscreteTransferFcn: '<S47>/Filter Differentiator TF' 
  localDW->FilterDifferentiatorTF_states = 0.0F;

  // InitializeConditions for DiscreteIntegrator: '<S54>/Integrator'
  localDW->Integrator_DSTATE = 0.0F;
  localDW->Integrator_PrevResetState = 2;

  // InitializeConditions for DiscreteTransferFcn: '<S97>/Filter Differentiator TF' 
  localDW->FilterDifferentiatorTF_states_i = 0.0F;

  // InitializeConditions for DiscreteIntegrator: '<S104>/Integrator'
  localDW->Integrator_DSTATE_i = 0.0F;
  localDW->Integrator_PrevResetState_n = 2;

  // InitializeConditions for DiscreteTransferFcn: '<S147>/Filter Differentiator TF' 
  localDW->FilterDifferentiatorTF_states_c = 0.0F;

  // InitializeConditions for DiscreteIntegrator: '<S154>/Integrator'
  localDW->Integrator_DSTATE_b = 0.0F;
  localDW->Integrator_PrevResetState_c = 2;

  // InitializeConditions for DiscreteIntegrator: '<S1>/Discrete-Time Integrator' 
  localDW->DiscreteTimeIntegrator_PrevResetState = 0;
}

// Enable for referenced model: 'control_outer'
void control_outer_Enable(DW_control_outer_f_T *localDW)
{
  // Enable for DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
  localDW->DiscreteTimeIntegrator_SYSTEM_ENABLE = 1U;
}

// Disable for referenced model: 'control_outer'
void control_outer_Disable(B_control_outer_c_T *localB, DW_control_outer_f_T
  *localDW)
{
  // Disable for DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
  localDW->DiscreteTimeIntegrator_DSTATE = localB->DiscreteTimeIntegrator;
}

// Output and update for referenced model: 'control_outer'
void control_outer(const Flags *rtu_Flags, const ConfigurationParameters
                   *rtu_ConfigurationParameters, const Targets *rtu_Targets,
                   const SensorsData *rtu_Sensors, const EstimatedData
                   *rtu_Estimates, ControlOuterOutputs *rty_OuterOutputs,
                   B_control_outer_c_T *localB, DW_control_outer_f_T *localDW,
                   ZCE_control_outer_T *localZCE)
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
  real32_T rtb_Switch2;
  real32_T rtb_Switch2_f;
  boolean_T rtb_Compare;
  boolean_T rtb_Compare_m;
  boolean_T rtb_FixPtRelationalOperator;

  // Abs: '<Root>/Abs'
  rtb_Abs = std::abs(rtu_ConfigurationParameters->thresholds.jntVelMax);

  // RelationalOperator: '<S12>/Compare' incorporates:
  //   Constant: '<S12>/Constant'

  rtb_Compare_m = (rtu_Flags->control_mode != ControlModes_Current);

  // RelationalOperator: '<S10>/Compare' incorporates:
  //   Constant: '<S10>/Constant'

  rty_OuterOutputs->cur_en = (rtu_Flags->control_mode != ControlModes_Voltage);

  // Logic: '<S2>/NOR' incorporates:
  //   Constant: '<S13>/Constant'
  //   Constant: '<S15>/Constant'
  //   Constant: '<S16>/Constant'
  //   RelationalOperator: '<S13>/Compare'
  //   RelationalOperator: '<S15>/Compare'
  //   RelationalOperator: '<S16>/Compare'

  rty_OuterOutputs->out_en = ((rtu_Flags->control_mode !=
    ControlModes_NotConfigured) && (rtu_Flags->control_mode != ControlModes_Idle)
    && (rtu_Flags->control_mode != ControlModes_HwFaultCM));

  // RelationalOperator: '<S18>/FixPt Relational Operator' incorporates:
  //   UnitDelay: '<S18>/Delay Input1'
  //
  //  Block description for '<S18>/Delay Input1':
  //
  //   Store in Global RAM

  rtb_FixPtRelationalOperator = (rtu_Flags->control_mode !=
    localDW->DelayInput1_DSTATE);

  // Switch: '<Root>/Switch2'
  if (rtu_Flags->enable_thermal_protection) {
    rtb_Switch2 = rtu_ConfigurationParameters->thresholds.motorNominalCurrents;
  } else {
    rtb_Switch2 = rtu_ConfigurationParameters->thresholds.motorPeakCurrents;
  }

  // End of Switch: '<Root>/Switch2'

  // Sum: '<S3>/Sum3'
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

  // DiscreteTransferFcn: '<S47>/Filter Differentiator TF' incorporates:
  //   Constant: '<S47>/Filter Den Constant'
  //   Product: '<S46>/DProd Out'
  //   Product: '<S47>/Divide'
  //   Sum: '<S47>/SumNum'

  if (rtb_FixPtRelationalOperator && (localZCE->FilterDifferentiatorTF_Reset_ZCE
       != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_states = 0.0F;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE = rtb_FixPtRelationalOperator;
  localDW->FilterDifferentiatorTF_tmp = rtb_DenCoefOut *
    rtu_ConfigurationParameters->PosLoopPID.D - (rtb_Product - 1.0F) *
    rtb_Switch2_f * localDW->FilterDifferentiatorTF_states;

  // Product: '<S57>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S47>/Filter Differentiator TF'
  //   Product: '<S47>/DenCoefOut'

  rtb_Switch2_f = (localDW->FilterDifferentiatorTF_tmp -
                   localDW->FilterDifferentiatorTF_states) * rtb_Switch2_f *
    rtu_ConfigurationParameters->PosLoopPID.N;

  // Sum: '<S66>/SumI1' incorporates:
  //   Product: '<S51>/IProd Out'
  //   Sum: '<S64>/Sum Fdbk'
  //   Sum: '<S65>/SumI3'
  //   UnitDelay: '<Root>/Unit Delay'

  rtb_DProdOut = (localDW->UnitDelay_DSTATE - ((rtb_PProdOut +
    localDW->Integrator_DSTATE) + rtb_Switch2_f)) + rtb_DenCoefOut *
    rtu_ConfigurationParameters->PosLoopPID.I;

  // DiscreteIntegrator: '<S54>/Integrator'
  if (rtb_FixPtRelationalOperator && (localDW->Integrator_PrevResetState <= 0))
  {
    localDW->Integrator_DSTATE = 0.0F;
  }

  // DiscreteIntegrator: '<S54>/Integrator'
  rtb_FilterDifferentiatorTF = 0.0005F * rtb_DProdOut +
    localDW->Integrator_DSTATE;

  // RelationalOperator: '<S11>/Compare' incorporates:
  //   Constant: '<S11>/Constant'

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
      (localZCE->FilterDifferentiatorTF_Reset_ZCE_m != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_states_i = 0.0F;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE_m = rtb_FixPtRelationalOperator;
  localDW->FilterDifferentiatorTF_tmp_m = rtb_DProdOut_f - (rtb_Gain1_h - 1.0F) *
    rtb_DenCoefOut * localDW->FilterDifferentiatorTF_states_i;

  // Product: '<S107>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S97>/Filter Differentiator TF'
  //   Product: '<S97>/DenCoefOut'

  rtb_DenCoefOut = (localDW->FilterDifferentiatorTF_tmp_m -
                    localDW->FilterDifferentiatorTF_states_i) * rtb_DenCoefOut *
    rtu_ConfigurationParameters->DirLoopPID.N;

  // Sum: '<S116>/SumI1' incorporates:
  //   Sum: '<S114>/Sum Fdbk'
  //   Sum: '<S115>/SumI3'
  //   UnitDelay: '<Root>/Unit Delay'

  rtb_FilterDifferentiatorTF_n = (localDW->UnitDelay_DSTATE - ((rtb_PProdOut_o +
    localDW->Integrator_DSTATE_i) + rtb_DenCoefOut)) + rtb_Product;

  // DiscreteIntegrator: '<S104>/Integrator'
  if (rtb_FixPtRelationalOperator && (localDW->Integrator_PrevResetState_n <= 0))
  {
    localDW->Integrator_DSTATE_i = 0.0F;
  }

  // DiscreteIntegrator: '<S104>/Integrator'
  rtb_DProdOut_f = 0.0005F * rtb_FilterDifferentiatorTF_n +
    localDW->Integrator_DSTATE_i;

  // Switch: '<Root>/Switch3' incorporates:
  //   Constant: '<Root>/Constant1'
  //   Constant: '<S14>/Constant'
  //   Constant: '<S17>/Constant'
  //   Logic: '<S2>/OR'
  //   RelationalOperator: '<S14>/Compare'
  //   RelationalOperator: '<S17>/Compare'

  if (rtb_Compare || (rtu_Flags->control_mode == ControlModes_PositionDirect) ||
      (rtu_Flags->control_mode == ControlModes_Velocity)) {
    // Switch: '<S3>/Switch5' incorporates:
    //   Sum: '<S113>/Sum'
    //   Sum: '<S63>/Sum'

    if (rtb_Compare) {
      rtb_DenCoefOut = (rtb_PProdOut + rtb_FilterDifferentiatorTF) +
        rtb_Switch2_f;
    } else {
      rtb_DenCoefOut += rtb_PProdOut_o + rtb_DProdOut_f;
    }

    // End of Switch: '<S3>/Switch5'
  } else {
    rtb_DenCoefOut = 0.0F;
  }

  // Sum: '<Root>/Sum2' incorporates:
  //   Switch: '<Root>/Switch3'

  rtb_Switch2_f = rtb_DenCoefOut + rtu_Targets->jointvelocities.velocity;

  // Switch: '<S4>/Switch2' incorporates:
  //   Gain: '<Root>/Gain'
  //   RelationalOperator: '<S4>/LowerRelop1'
  //   RelationalOperator: '<S4>/UpperRelop'
  //   Switch: '<S4>/Switch'

  if (rtb_Switch2_f > rtb_Abs) {
    rtb_Switch2_f = rtb_Abs;
  } else if (rtb_Switch2_f < -rtb_Abs) {
    // Switch: '<S4>/Switch' incorporates:
    //   Gain: '<Root>/Gain'

    rtb_Switch2_f = -rtb_Abs;
  }

  // End of Switch: '<S4>/Switch2'

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
      (localZCE->FilterDifferentiatorTF_Reset_ZCE_e != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_states_c = 0.0F;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE_e = rtb_FixPtRelationalOperator;
  localDW->FilterDifferentiatorTF_tmp_p = rtb_PProdOut - (rtb_Product - 1.0F) *
    rtb_DenCoefOut * localDW->FilterDifferentiatorTF_states_c;

  // Product: '<S157>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S147>/Filter Differentiator TF'
  //   Product: '<S147>/DenCoefOut'

  rtb_DenCoefOut = (localDW->FilterDifferentiatorTF_tmp_p -
                    localDW->FilterDifferentiatorTF_states_c) * rtb_DenCoefOut *
    rtu_ConfigurationParameters->VelLoopPID.N;

  // Sum: '<S166>/SumI1' incorporates:
  //   Sum: '<S164>/Sum Fdbk'
  //   Sum: '<S165>/SumI3'
  //   UnitDelay: '<Root>/Unit Delay1'

  rtb_PProdOut = (localDW->UnitDelay1_DSTATE - ((rtb_Abs +
    localDW->Integrator_DSTATE_b) + rtb_DenCoefOut)) + rtb_Gain1_h;

  // DiscreteIntegrator: '<S154>/Integrator'
  if (rtb_FixPtRelationalOperator && (localDW->Integrator_PrevResetState_c <= 0))
  {
    localDW->Integrator_DSTATE_b = 0.0F;
  }

  // DiscreteIntegrator: '<S154>/Integrator'
  rtb_Product = 0.0005F * rtb_PProdOut + localDW->Integrator_DSTATE_b;

  // Switch: '<Root>/Switch1' incorporates:
  //   Sum: '<S163>/Sum'

  if (rtb_Compare_m) {
    rtb_Abs = (rtb_Abs + rtb_Product) + rtb_DenCoefOut;
  } else {
    rtb_Abs = rtu_Targets->motorcurrent.current;
  }

  // End of Switch: '<Root>/Switch1'

  // Switch: '<S5>/Switch2' incorporates:
  //   RelationalOperator: '<S5>/LowerRelop1'

  if (!(rtb_Abs > rtb_Switch2)) {
    // Gain: '<Root>/Gain1'
    rtb_Switch2 = -rtb_Switch2;

    // Switch: '<S5>/Switch' incorporates:
    //   RelationalOperator: '<S5>/UpperRelop'

    if (!(rtb_Abs < rtb_Switch2)) {
      rtb_Switch2 = rtb_Abs;
    }

    // End of Switch: '<S5>/Switch'
  }

  // End of Switch: '<S5>/Switch2'

  // BusCreator: '<Root>/Bus Creator2'
  rty_OuterOutputs->motorcurrent.current = rtb_Switch2;

  // Sum: '<S1>/Sum' incorporates:
  //   Abs: '<S1>/Abs1'

  rtb_Abs = rtu_ConfigurationParameters->thresholds.motorPeakCurrents - std::abs
    (rtu_Estimates->Iq_filtered.current);

  // CombinatorialLogic: '<S9>/Logic' incorporates:
  //   Constant: '<S7>/Constant'
  //   Gain: '<S1>/Gain1'
  //   Logic: '<S1>/Logical Operator'
  //   Memory: '<S9>/Memory'
  //   RelationalOperator: '<S1>/Relational Operator'
  //   RelationalOperator: '<S7>/Compare'

  rowIdx = static_cast<int32_T>(((((rtb_Abs > 0.1F *
    rtu_ConfigurationParameters->thresholds.motorPeakCurrents) ||
    rtb_FixPtRelationalOperator) + (static_cast<uint32_T>(rtb_Abs <= 0.0F) << 1))
    << 1) + localDW->Memory_PreviousInput);
  rtb_Compare = rtCP_Logic_table[static_cast<uint32_T>(rowIdx) + 8U];

  // DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
  if (localDW->DiscreteTimeIntegrator_SYSTEM_ENABLE != 0) {
    // DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
    localB->DiscreteTimeIntegrator = localDW->DiscreteTimeIntegrator_DSTATE;
  } else if (rtb_Compare || (localDW->DiscreteTimeIntegrator_PrevResetState != 0))
  {
    // DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
    localB->DiscreteTimeIntegrator = 0.0F;
  } else {
    // DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
    localB->DiscreteTimeIntegrator = 0.0005F * rtb_Abs +
      localDW->DiscreteTimeIntegrator_DSTATE;
  }

  // End of DiscreteIntegrator: '<S1>/Discrete-Time Integrator'

  // BusCreator: '<Root>/Bus Creator1'
  rty_OuterOutputs->vel_en = rtb_Compare_m;
  rty_OuterOutputs->pid_reset = rtb_FixPtRelationalOperator;

  // Switch: '<S1>/Switch1' incorporates:
  //   Constant: '<S1>/Constant1'
  //   Constant: '<S1>/Constant2'
  //   Constant: '<S8>/Constant'
  //   RelationalOperator: '<S8>/Compare'

  if (rtu_Estimates->Iq_filtered.current < 0.0F) {
    tmp = -1;
  } else {
    tmp = 1;
  }

  // BusCreator: '<Root>/Bus Creator1' incorporates:
  //   Product: '<S1>/Product'
  //   Switch: '<S1>/Switch1'

  rty_OuterOutputs->current_limiter = static_cast<real32_T>(tmp) *
    localB->DiscreteTimeIntegrator * rtu_ConfigurationParameters->CurLoopPID.I;

  // Update for UnitDelay: '<S18>/Delay Input1'
  //
  //  Block description for '<S18>/Delay Input1':
  //
  //   Store in Global RAM

  localDW->DelayInput1_DSTATE = rtu_Flags->control_mode;

  // Update for DiscreteTransferFcn: '<S47>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_states = localDW->FilterDifferentiatorTF_tmp;

  // Update for UnitDelay: '<Root>/Unit Delay' incorporates:
  //   Sum: '<Root>/Sum3'

  localDW->UnitDelay_DSTATE = rtb_Switch2_f -
    rtu_Targets->jointvelocities.velocity;

  // Update for DiscreteIntegrator: '<S54>/Integrator'
  localDW->Integrator_DSTATE = 0.0005F * rtb_DProdOut +
    rtb_FilterDifferentiatorTF;
  localDW->Integrator_PrevResetState = static_cast<int8_T>
    (rtb_FixPtRelationalOperator);

  // Update for DiscreteTransferFcn: '<S97>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_states_i =
    localDW->FilterDifferentiatorTF_tmp_m;

  // Update for DiscreteIntegrator: '<S104>/Integrator' incorporates:
  //   DiscreteIntegrator: '<S54>/Integrator'

  localDW->Integrator_DSTATE_i = 0.0005F * rtb_FilterDifferentiatorTF_n +
    rtb_DProdOut_f;
  localDW->Integrator_PrevResetState_n = static_cast<int8_T>
    (rtb_FixPtRelationalOperator);

  // Update for DiscreteTransferFcn: '<S147>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_states_c =
    localDW->FilterDifferentiatorTF_tmp_p;

  // Update for UnitDelay: '<Root>/Unit Delay1'
  localDW->UnitDelay1_DSTATE = rtb_Switch2;

  // Update for DiscreteIntegrator: '<S154>/Integrator' incorporates:
  //   DiscreteIntegrator: '<S54>/Integrator'

  localDW->Integrator_DSTATE_b = 0.0005F * rtb_PProdOut + rtb_Product;
  localDW->Integrator_PrevResetState_c = static_cast<int8_T>
    (rtb_FixPtRelationalOperator);

  // Update for Memory: '<S9>/Memory' incorporates:
  //   CombinatorialLogic: '<S9>/Logic'

  localDW->Memory_PreviousInput = rtCP_Logic_table[static_cast<uint32_T>(rowIdx)];

  // Update for DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
  localDW->DiscreteTimeIntegrator_SYSTEM_ENABLE = 0U;
  localDW->DiscreteTimeIntegrator_DSTATE = 0.0005F * rtb_Abs +
    localB->DiscreteTimeIntegrator;
  localDW->DiscreteTimeIntegrator_PrevResetState = static_cast<int8_T>
    (rtb_Compare);
}

// Model initialize function
void control_outer_initialize(const char_T **rt_errorStatus,
  RT_MODEL_control_outer_T *const control_outer_M, B_control_outer_c_T *localB,
  DW_control_outer_f_T *localDW, ZCE_control_outer_T *localZCE)
{
  // Registration code

  // initialize error status
  rtmSetErrorStatusPointer(control_outer_M, rt_errorStatus);

  // block I/O
  (void) std::memset((static_cast<void *>(localB)), 0,
                     sizeof(B_control_outer_c_T));

  // states (dwork)
  (void) std::memset(static_cast<void *>(localDW), 0,
                     sizeof(DW_control_outer_f_T));
  localZCE->FilterDifferentiatorTF_Reset_ZCE = POS_ZCSIG;
  localZCE->FilterDifferentiatorTF_Reset_ZCE_m = POS_ZCSIG;
  localZCE->FilterDifferentiatorTF_Reset_ZCE_e = POS_ZCSIG;
}

//
// File trailer for generated code.
//
// [EOF]
//
