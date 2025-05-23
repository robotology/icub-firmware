//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_outer.cpp
//
// Code generated for Simulink model 'control_outer'.
//
// Model version                  : 8.0
// Simulink Coder version         : 24.2 (R2024b) 21-Jun-2024
// C/C++ source code generated on : Fri May 23 10:48:30 2025
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

// System initialize for referenced model: 'control_outer'
void control_outer_Init(DW_control_outer_f_T *localDW)
{
  // InitializeConditions for UnitDelay: '<S18>/Delay Input1'
  //
  //  Block description for '<S18>/Delay Input1':
  //
  //   Store in Global RAM

  localDW->DelayInput1_DSTATE = ControlModes_Idle;

  // InitializeConditions for DiscreteIntegrator: '<S57>/Integrator'
  localDW->Integrator_PrevResetState = 2;

  // InitializeConditions for DiscreteIntegrator: '<S111>/Integrator'
  localDW->Integrator_PrevResetState_n = 2;

  // InitializeConditions for DiscreteIntegrator: '<S165>/Integrator'
  localDW->Integrator_PrevResetState_c = 2;
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
void control_outer(const Flags *rtu_Flags, const ActuatorConfiguration
                   *rtu_ConfigurationParameters, const EstimatedData
                   *rtu_Estimates, const SensorsData *rtu_Sensors, const Targets
                   *rtu_Targets, ControlOuterOutputs *rty_OuterOutputs,
                   B_control_outer_c_T *localB, DW_control_outer_f_T *localDW,
                   ZCE_control_outer_T *localZCE)
{
  int32_T rowIdx;
  int32_T tmp;
  real32_T Integrator;
  real32_T Integrator_p;
  real32_T rtb_Abs;
  real32_T rtb_DProdOut_b;
  real32_T rtb_DenCoefOut;
  real32_T rtb_NProdOut;
  real32_T rtb_NProdOut_f;
  real32_T rtb_Sum2;
  real32_T rtb_SumI1;
  real32_T rtb_SumI1_e;
  real32_T rtb_Switch2;
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
  rtb_DenCoefOut = rtu_Targets->position - rtu_Sensors->position;

  // Product: '<S62>/PProd Out' incorporates:
  //   Product: '<S116>/PProd Out'

  rtb_Sum2 = rtb_DenCoefOut * rtu_ConfigurationParameters->pids.positionPID.P;

  // SampleTimeMath: '<S52>/Tsamp' incorporates:
  //   SampleTimeMath: '<S106>/Tsamp'
  //
  //  About '<S52>/Tsamp':
  //   y = u * K where K = ( w * Ts )
  //    *
  //  About '<S106>/Tsamp':
  //   y = u * K where K = ( w * Ts )
  //
  rtb_SumI1 = rtu_ConfigurationParameters->pids.positionPID.N * 0.0005F;

  // Math: '<S50>/Reciprocal' incorporates:
  //   Constant: '<S50>/Filter Den Constant'
  //   Math: '<S104>/Reciprocal'
  //   SampleTimeMath: '<S52>/Tsamp'
  //   Sum: '<S50>/SumDen'
  //
  //  About '<S50>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S104>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S52>/Tsamp':
  //   y = u * K where K = ( w * Ts )
  //
  rtb_SumI1_e = 1.0F / (rtb_SumI1 + 1.0F);

  // DiscreteTransferFcn: '<S50>/Filter Differentiator TF'
  if (rtb_FixPtRelationalOperator && (localZCE->FilterDifferentiatorTF_Reset_ZCE
       != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_states = 0.0F;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE = rtb_FixPtRelationalOperator;

  // Product: '<S48>/DProd Out' incorporates:
  //   Product: '<S102>/DProd Out'

  Integrator_p = rtb_DenCoefOut *
    rtu_ConfigurationParameters->pids.positionPID.D;

  // Product: '<S50>/Divide' incorporates:
  //   Constant: '<S50>/Filter Den Constant'
  //   Math: '<S50>/Reciprocal'
  //   Product: '<S104>/Divide'
  //   SampleTimeMath: '<S52>/Tsamp'
  //   Sum: '<S50>/SumNum'
  //
  //  About '<S50>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S52>/Tsamp':
  //   y = u * K where K = ( w * Ts )
  //
  rtb_NProdOut_f = (rtb_SumI1 - 1.0F) * rtb_SumI1_e;

  // DiscreteTransferFcn: '<S50>/Filter Differentiator TF' incorporates:
  //   Product: '<S48>/DProd Out'
  //   Product: '<S50>/Divide'

  localDW->FilterDifferentiatorTF_tmp = Integrator_p - rtb_NProdOut_f *
    localDW->FilterDifferentiatorTF_states;

  // Product: '<S60>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S50>/Filter Differentiator TF'
  //   Math: '<S50>/Reciprocal'
  //   Product: '<S50>/DenCoefOut'
  //
  //  About '<S50>/Reciprocal':
  //   Operator: reciprocal

  rtb_NProdOut = (localDW->FilterDifferentiatorTF_tmp -
                  localDW->FilterDifferentiatorTF_states) * rtb_SumI1_e *
    rtu_ConfigurationParameters->pids.positionPID.N;

  // Product: '<S54>/IProd Out' incorporates:
  //   Product: '<S108>/IProd Out'

  rtb_DenCoefOut *= rtu_ConfigurationParameters->pids.positionPID.I;

  // Sum: '<S69>/SumI1' incorporates:
  //   Product: '<S54>/IProd Out'
  //   Product: '<S62>/PProd Out'
  //   Sum: '<S67>/Sum Fdbk'
  //   Sum: '<S68>/SumI3'
  //   UnitDelay: '<Root>/Unit Delay'

  rtb_SumI1 = (localDW->UnitDelay_DSTATE - ((rtb_Sum2 +
    localDW->Integrator_DSTATE) + rtb_NProdOut)) + rtb_DenCoefOut;

  // DiscreteIntegrator: '<S57>/Integrator'
  if (rtb_FixPtRelationalOperator && (localDW->Integrator_PrevResetState <= 0))
  {
    localDW->Integrator_DSTATE = 0.0F;
  }

  // DiscreteIntegrator: '<S57>/Integrator'
  Integrator = 0.0005F * rtb_SumI1 + localDW->Integrator_DSTATE;

  // RelationalOperator: '<S11>/Compare' incorporates:
  //   Constant: '<S11>/Constant'

  rtb_Compare = (rtu_Flags->control_mode == ControlModes_Position);

  // DiscreteTransferFcn: '<S104>/Filter Differentiator TF' incorporates:
  //   DiscreteTransferFcn: '<S50>/Filter Differentiator TF'

  if (rtb_FixPtRelationalOperator &&
      (localZCE->FilterDifferentiatorTF_Reset_ZCE_m != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_states_i = 0.0F;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE_m = rtb_FixPtRelationalOperator;
  localDW->FilterDifferentiatorTF_tmp_m = Integrator_p - rtb_NProdOut_f *
    localDW->FilterDifferentiatorTF_states_i;

  // Product: '<S114>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S104>/Filter Differentiator TF'
  //   Product: '<S104>/DenCoefOut'

  rtb_NProdOut_f = (localDW->FilterDifferentiatorTF_tmp_m -
                    localDW->FilterDifferentiatorTF_states_i) * rtb_SumI1_e *
    rtu_ConfigurationParameters->pids.positionPID.N;

  // Sum: '<S123>/SumI1' incorporates:
  //   Sum: '<S121>/Sum Fdbk'
  //   Sum: '<S122>/SumI3'
  //   UnitDelay: '<Root>/Unit Delay'

  rtb_SumI1_e = (localDW->UnitDelay_DSTATE - ((rtb_Sum2 +
    localDW->Integrator_DSTATE_i) + rtb_NProdOut_f)) + rtb_DenCoefOut;

  // DiscreteIntegrator: '<S111>/Integrator'
  if (rtb_FixPtRelationalOperator && (localDW->Integrator_PrevResetState_n <= 0))
  {
    localDW->Integrator_DSTATE_i = 0.0F;
  }

  // DiscreteIntegrator: '<S111>/Integrator'
  Integrator_p = 0.0005F * rtb_SumI1_e + localDW->Integrator_DSTATE_i;

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
    //   Product: '<S62>/PProd Out'
    //   Sum: '<S120>/Sum'
    //   Sum: '<S66>/Sum'

    if (rtb_Compare) {
      rtb_DenCoefOut = (rtb_Sum2 + Integrator) + rtb_NProdOut;
    } else {
      rtb_DenCoefOut = (rtb_Sum2 + Integrator_p) + rtb_NProdOut_f;
    }

    // End of Switch: '<S3>/Switch5'
  } else {
    rtb_DenCoefOut = 0.0F;
  }

  // Sum: '<Root>/Sum2' incorporates:
  //   Switch: '<Root>/Switch3'

  rtb_Sum2 = rtb_DenCoefOut + rtu_Targets->velocity;

  // Switch: '<S4>/Switch2' incorporates:
  //   Gain: '<Root>/Gain'
  //   RelationalOperator: '<S4>/LowerRelop1'
  //   RelationalOperator: '<S4>/UpperRelop'
  //   Switch: '<S4>/Switch'

  if (rtb_Sum2 > rtb_Abs) {
    rtb_Sum2 = rtb_Abs;
  } else if (rtb_Sum2 < -rtb_Abs) {
    // Switch: '<S4>/Switch' incorporates:
    //   Gain: '<Root>/Gain'

    rtb_Sum2 = -rtb_Abs;
  }

  // End of Switch: '<S4>/Switch2'

  // Sum: '<Root>/Sum1'
  rtb_NProdOut_f = rtb_Sum2 - rtu_Estimates->velocity;

  // Product: '<S170>/PProd Out'
  rtb_NProdOut = rtb_NProdOut_f *
    rtu_ConfigurationParameters->pids.velocityPID.P;

  // Product: '<S162>/IProd Out'
  rtb_Abs = rtb_NProdOut_f * rtu_ConfigurationParameters->pids.velocityPID.I;

  // Product: '<S156>/DProd Out'
  rtb_DProdOut_b = rtb_NProdOut_f *
    rtu_ConfigurationParameters->pids.velocityPID.D;

  // SampleTimeMath: '<S160>/Tsamp'
  //
  //  About '<S160>/Tsamp':
  //   y = u * K where K = ( w * Ts )
  //
  rtb_NProdOut_f = rtu_ConfigurationParameters->pids.velocityPID.N * 0.0005F;

  // Math: '<S158>/Reciprocal' incorporates:
  //   Constant: '<S158>/Filter Den Constant'
  //   Sum: '<S158>/SumDen'
  //
  //  About '<S158>/Reciprocal':
  //   Operator: reciprocal

  rtb_DenCoefOut = 1.0F / (rtb_NProdOut_f + 1.0F);

  // DiscreteTransferFcn: '<S158>/Filter Differentiator TF' incorporates:
  //   Constant: '<S158>/Filter Den Constant'
  //   DiscreteTransferFcn: '<S50>/Filter Differentiator TF'
  //   Product: '<S158>/Divide'
  //   Sum: '<S158>/SumNum'

  if (rtb_FixPtRelationalOperator &&
      (localZCE->FilterDifferentiatorTF_Reset_ZCE_e != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_states_c = 0.0F;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE_e = rtb_FixPtRelationalOperator;
  localDW->FilterDifferentiatorTF_tmp_p = rtb_DProdOut_b - (rtb_NProdOut_f -
    1.0F) * rtb_DenCoefOut * localDW->FilterDifferentiatorTF_states_c;

  // Product: '<S168>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S158>/Filter Differentiator TF'
  //   Product: '<S158>/DenCoefOut'

  rtb_NProdOut_f = (localDW->FilterDifferentiatorTF_tmp_p -
                    localDW->FilterDifferentiatorTF_states_c) * rtb_DenCoefOut *
    rtu_ConfigurationParameters->pids.velocityPID.N;

  // Sum: '<S177>/SumI1' incorporates:
  //   Sum: '<S175>/Sum Fdbk'
  //   Sum: '<S176>/SumI3'
  //   UnitDelay: '<Root>/Unit Delay1'

  rtb_DenCoefOut = (localDW->UnitDelay1_DSTATE - ((rtb_NProdOut +
    localDW->Integrator_DSTATE_b) + rtb_NProdOut_f)) + rtb_Abs;

  // DiscreteIntegrator: '<S165>/Integrator'
  if (rtb_FixPtRelationalOperator && (localDW->Integrator_PrevResetState_c <= 0))
  {
    localDW->Integrator_DSTATE_b = 0.0F;
  }

  // DiscreteIntegrator: '<S165>/Integrator'
  rtb_Abs = 0.0005F * rtb_DenCoefOut + localDW->Integrator_DSTATE_b;

  // Switch: '<Root>/Switch1'
  if (rtb_Compare_m) {
    // Switch: '<Root>/Switch1' incorporates:
    //   Sum: '<S174>/Sum'

    localDW->UnitDelay1_DSTATE = (rtb_NProdOut + rtb_Abs) + rtb_NProdOut_f;
  } else {
    // Switch: '<Root>/Switch1'
    localDW->UnitDelay1_DSTATE = rtu_Targets->current;
  }

  // End of Switch: '<Root>/Switch1'

  // Switch: '<S5>/Switch2' incorporates:
  //   Gain: '<Root>/Gain1'
  //   RelationalOperator: '<S5>/LowerRelop1'
  //   RelationalOperator: '<S5>/UpperRelop'
  //   Switch: '<S5>/Switch'

  if (localDW->UnitDelay1_DSTATE > rtb_Switch2) {
    // Switch: '<Root>/Switch1'
    localDW->UnitDelay1_DSTATE = rtb_Switch2;
  } else if (localDW->UnitDelay1_DSTATE < -rtb_Switch2) {
    // Switch: '<Root>/Switch1' incorporates:
    //   Gain: '<Root>/Gain1'
    //   Switch: '<S5>/Switch'

    localDW->UnitDelay1_DSTATE = -rtb_Switch2;
  }

  // End of Switch: '<S5>/Switch2'

  // Sum: '<S1>/Sum' incorporates:
  //   Abs: '<S1>/Abs1'

  rtb_Switch2 = rtu_ConfigurationParameters->thresholds.motorPeakCurrents - std::
    abs(rtu_Estimates->Iq_filtered);

  // CombinatorialLogic: '<S9>/Logic' incorporates:
  //   Constant: '<S7>/Constant'
  //   Gain: '<S1>/Gain1'
  //   Logic: '<S1>/Logical Operator'
  //   Memory: '<S9>/Memory'
  //   RelationalOperator: '<S1>/Relational Operator'
  //   RelationalOperator: '<S7>/Compare'

  rowIdx = static_cast<int32_T>(((((rtb_Switch2 > 0.1F *
    rtu_ConfigurationParameters->thresholds.motorPeakCurrents) ||
    rtb_FixPtRelationalOperator) + (static_cast<uint32_T>(rtb_Switch2 <= 0.0F) <<
    1)) << 1) + localDW->Memory_PreviousInput);
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
    localB->DiscreteTimeIntegrator = 0.0005F * rtb_Switch2 +
      localDW->DiscreteTimeIntegrator_DSTATE;
  }

  // End of DiscreteIntegrator: '<S1>/Discrete-Time Integrator'

  // BusCreator: '<Root>/Bus Creator1' incorporates:
  //   UnitDelay: '<Root>/Unit Delay1'

  rty_OuterOutputs->vel_en = rtb_Compare_m;
  rty_OuterOutputs->pid_reset = rtb_FixPtRelationalOperator;
  rty_OuterOutputs->motorcurrent = localDW->UnitDelay1_DSTATE;

  // Switch: '<S1>/Switch1' incorporates:
  //   Constant: '<S1>/Constant1'
  //   Constant: '<S1>/Constant2'
  //   Constant: '<S8>/Constant'
  //   RelationalOperator: '<S8>/Compare'

  if (rtu_Estimates->Iq_filtered < 0.0F) {
    tmp = -1;
  } else {
    tmp = 1;
  }

  // BusCreator: '<Root>/Bus Creator1' incorporates:
  //   Product: '<S1>/Product'
  //   Switch: '<S1>/Switch1'

  rty_OuterOutputs->current_limiter = static_cast<real32_T>(tmp) *
    localB->DiscreteTimeIntegrator *
    rtu_ConfigurationParameters->pids.currentPID.I;

  // Sum: '<Root>/Sum3' incorporates:
  //   UnitDelay: '<Root>/Unit Delay'

  localDW->UnitDelay_DSTATE = rtb_Sum2 - rtu_Targets->velocity;

  // Update for UnitDelay: '<S18>/Delay Input1'
  //
  //  Block description for '<S18>/Delay Input1':
  //
  //   Store in Global RAM

  localDW->DelayInput1_DSTATE = rtu_Flags->control_mode;

  // Update for DiscreteTransferFcn: '<S50>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_states = localDW->FilterDifferentiatorTF_tmp;

  // Update for DiscreteIntegrator: '<S57>/Integrator'
  localDW->Integrator_DSTATE = 0.0005F * rtb_SumI1 + Integrator;
  localDW->Integrator_PrevResetState = static_cast<int8_T>
    (rtb_FixPtRelationalOperator);

  // Update for DiscreteTransferFcn: '<S104>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_states_i =
    localDW->FilterDifferentiatorTF_tmp_m;

  // Update for DiscreteIntegrator: '<S111>/Integrator' incorporates:
  //   DiscreteIntegrator: '<S57>/Integrator'

  localDW->Integrator_DSTATE_i = 0.0005F * rtb_SumI1_e + Integrator_p;
  localDW->Integrator_PrevResetState_n = static_cast<int8_T>
    (rtb_FixPtRelationalOperator);

  // Update for DiscreteTransferFcn: '<S158>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_states_c =
    localDW->FilterDifferentiatorTF_tmp_p;

  // Update for DiscreteIntegrator: '<S165>/Integrator' incorporates:
  //   DiscreteIntegrator: '<S57>/Integrator'

  localDW->Integrator_DSTATE_b = 0.0005F * rtb_DenCoefOut + rtb_Abs;
  localDW->Integrator_PrevResetState_c = static_cast<int8_T>
    (rtb_FixPtRelationalOperator);

  // Update for Memory: '<S9>/Memory' incorporates:
  //   CombinatorialLogic: '<S9>/Logic'

  localDW->Memory_PreviousInput = rtCP_Logic_table[static_cast<uint32_T>(rowIdx)];

  // Update for DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
  localDW->DiscreteTimeIntegrator_SYSTEM_ENABLE = 0U;
  localDW->DiscreteTimeIntegrator_DSTATE = 0.0005F * rtb_Switch2 +
    localB->DiscreteTimeIntegrator;
  localDW->DiscreteTimeIntegrator_PrevResetState = static_cast<int8_T>
    (rtb_Compare);
}

// Model initialize function
void control_outer_initialize(ZCE_control_outer_T *localZCE)
{
  localZCE->FilterDifferentiatorTF_Reset_ZCE = POS_ZCSIG;
  localZCE->FilterDifferentiatorTF_Reset_ZCE_m = POS_ZCSIG;
  localZCE->FilterDifferentiatorTF_Reset_ZCE_e = POS_ZCSIG;
}

//
// File trailer for generated code.
//
// [EOF]
//
