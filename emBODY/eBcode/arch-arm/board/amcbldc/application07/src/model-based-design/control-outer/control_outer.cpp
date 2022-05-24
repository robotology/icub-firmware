//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_outer.cpp
//
// Code generated for Simulink model 'control_outer'.
//
// Model version                  : 2.43
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Tue May 24 10:01:04 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "control_outer.h"
#include "control_outer_private.h"

MdlrefDW_control_outer_T control_outer_MdlrefDW;

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

  control_outer_DW.DelayInput1_DSTATE_f = ControlModes_Idle;

  // InitializeConditions for DiscreteIntegrator: '<S53>/Integrator'
  control_outer_DW.Integrator_PrevResetState = 2;

  // InitializeConditions for DiscreteIntegrator: '<S103>/Integrator'
  control_outer_DW.Integrator_PrevResetState_n = 2;

  // InitializeConditions for DiscreteIntegrator: '<S155>/Integrator'
  control_outer_DW.Integrator_PrevResetState_c = 2;
}

// Output and update for referenced model: 'control_outer'
void control_outer(const Flags *rtu_Flags, const ConfigurationParameters
                   *rtu_ConfigurationParameters, const Targets *rtu_Targets,
                   const SensorsData *rtu_Sensors, const EstimatedData
                   *rtu_Estimates, ControlOuterOutputs *rty_OuterOutputs)
{
  real32_T rtb_Abs;
  real32_T rtb_DProdOut;
  real32_T rtb_DProdOut_f;
  real32_T rtb_FilterDifferentiatorTF;
  real32_T rtb_FilterDifferentiatorTF_n;
  real32_T rtb_PProdOut;
  real32_T rtb_PProdOut_o;
  real32_T rtb_Sum1;
  real32_T rtb_Switch2;
  real32_T rtb_Switch2_o;
  real32_T rtb_Uk1;
  real32_T rtb_UnitDelay1;
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

  // Logic: '<S3>/NOR' incorporates:
  //   Constant: '<S13>/Constant'
  //   Constant: '<S15>/Constant'
  //   Constant: '<S16>/Constant'
  //   RelationalOperator: '<S13>/Compare'
  //   RelationalOperator: '<S15>/Compare'
  //   RelationalOperator: '<S16>/Compare'

  rty_OuterOutputs->out_en = ((!(rtu_Flags->control_mode ==
    ControlModes_NotConfigured)) && (!(rtu_Flags->control_mode ==
    ControlModes_Idle)) && (!(rtu_Flags->control_mode == ControlModes_HwFaultCM)));

  // Sum: '<S4>/Sum3'
  rtb_UnitDelay1 = rtu_Targets->jointpositions.position -
    rtu_Sensors->jointpositions.position;

  // Product: '<S58>/PProd Out'
  rtb_PProdOut = rtb_UnitDelay1 * rtu_ConfigurationParameters->PosLoopPID.P;

  // SampleTimeMath: '<S48>/Tsamp'
  //
  //  About '<S48>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_Switch2_o = rtu_ConfigurationParameters->PosLoopPID.N * 0.0005F;

  // Math: '<S46>/Reciprocal' incorporates:
  //   Constant: '<S46>/Constant'
  //   Sum: '<S46>/SumDen'
  //
  //  About '<S46>/Reciprocal':
  //   Operator: reciprocal

  rtb_Switch2 = 1.0F / (rtb_Switch2_o + 1.0F);

  // RelationalOperator: '<S2>/FixPt Relational Operator' incorporates:
  //   UnitDelay: '<S2>/Delay Input1'
  //
  //  Block description for '<S2>/Delay Input1':
  //
  //   Store in Global RAM

  rtb_FixPtRelationalOperator = (rtu_Flags->control_mode !=
    control_outer_DW.DelayInput1_DSTATE_f);

  // DiscreteTransferFcn: '<S46>/Filter Differentiator TF' incorporates:
  //   Constant: '<S46>/Constant'
  //   Product: '<S45>/DProd Out'
  //   Product: '<S46>/Divide'
  //   Sum: '<S46>/SumNum'

  if (rtb_FixPtRelationalOperator &&
      (control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC != 1)) {
    control_outer_DW.FilterDifferentiatorTF_states = 0.0F;
  }

  control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC =
    rtb_FixPtRelationalOperator;
  control_outer_DW.FilterDifferentiatorTF_tmp = rtb_UnitDelay1 *
    rtu_ConfigurationParameters->PosLoopPID.D - (rtb_Switch2_o - 1.0F) *
    rtb_Switch2 * control_outer_DW.FilterDifferentiatorTF_states;

  // Product: '<S56>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S46>/Filter Differentiator TF'
  //   Product: '<S46>/DenCoefOut'

  rtb_Switch2 = (control_outer_DW.FilterDifferentiatorTF_tmp +
                 -control_outer_DW.FilterDifferentiatorTF_states) * rtb_Switch2 *
    rtu_ConfigurationParameters->PosLoopPID.N;

  // Sum: '<S65>/SumI1' incorporates:
  //   Product: '<S50>/IProd Out'
  //   Sum: '<S63>/Sum Fdbk'
  //   Sum: '<S64>/SumI3'
  //   UnitDelay: '<Root>/Unit Delay'

  rtb_DProdOut = (control_outer_DW.UnitDelay_DSTATE - ((rtb_PProdOut +
    control_outer_DW.Integrator_DSTATE) + rtb_Switch2)) + rtb_UnitDelay1 *
    rtu_ConfigurationParameters->PosLoopPID.I;

  // DiscreteIntegrator: '<S53>/Integrator'
  if (rtb_FixPtRelationalOperator && (control_outer_DW.Integrator_PrevResetState
       <= 0)) {
    control_outer_DW.Integrator_DSTATE = 0.0F;
  }

  // DiscreteIntegrator: '<S53>/Integrator'
  rtb_FilterDifferentiatorTF = 0.0005F * rtb_DProdOut +
    control_outer_DW.Integrator_DSTATE;

  // RelationalOperator: '<S11>/Compare' incorporates:
  //   Constant: '<S11>/Constant'

  rtb_Compare = (rtu_Flags->control_mode == ControlModes_Position);

  // Product: '<S108>/PProd Out'
  rtb_PProdOut_o = rtb_UnitDelay1 * rtu_ConfigurationParameters->DirLoopPID.P;

  // Product: '<S100>/IProd Out'
  rtb_Switch2_o = rtb_UnitDelay1 * rtu_ConfigurationParameters->DirLoopPID.I;

  // Product: '<S95>/DProd Out'
  rtb_DProdOut_f = rtb_UnitDelay1 * rtu_ConfigurationParameters->DirLoopPID.D;

  // SampleTimeMath: '<S98>/Tsamp'
  //
  //  About '<S98>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_Uk1 = rtu_ConfigurationParameters->DirLoopPID.N * 0.0005F;

  // Math: '<S96>/Reciprocal' incorporates:
  //   Constant: '<S96>/Constant'
  //   Sum: '<S96>/SumDen'
  //
  //  About '<S96>/Reciprocal':
  //   Operator: reciprocal

  rtb_UnitDelay1 = 1.0F / (rtb_Uk1 + 1.0F);

  // DiscreteTransferFcn: '<S96>/Filter Differentiator TF' incorporates:
  //   Constant: '<S96>/Constant'
  //   DiscreteTransferFcn: '<S46>/Filter Differentiator TF'
  //   Product: '<S96>/Divide'
  //   Sum: '<S96>/SumNum'

  if (rtb_FixPtRelationalOperator &&
      (control_outer_PrevZCX.FilterDifferentiatorTF_Reset__m != 1)) {
    control_outer_DW.FilterDifferentiatorTF_states_i = 0.0F;
  }

  control_outer_PrevZCX.FilterDifferentiatorTF_Reset__m =
    rtb_FixPtRelationalOperator;
  control_outer_DW.FilterDifferentiatorTF_tmp_m = rtb_DProdOut_f - (rtb_Uk1 -
    1.0F) * rtb_UnitDelay1 * control_outer_DW.FilterDifferentiatorTF_states_i;

  // Product: '<S106>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S96>/Filter Differentiator TF'
  //   Product: '<S96>/DenCoefOut'

  rtb_UnitDelay1 = (control_outer_DW.FilterDifferentiatorTF_tmp_m +
                    -control_outer_DW.FilterDifferentiatorTF_states_i) *
    rtb_UnitDelay1 * rtu_ConfigurationParameters->DirLoopPID.N;

  // Sum: '<S115>/SumI1' incorporates:
  //   Sum: '<S113>/Sum Fdbk'
  //   Sum: '<S114>/SumI3'
  //   UnitDelay: '<Root>/Unit Delay'

  rtb_FilterDifferentiatorTF_n = (control_outer_DW.UnitDelay_DSTATE -
    ((rtb_PProdOut_o + control_outer_DW.Integrator_DSTATE_i) + rtb_UnitDelay1))
    + rtb_Switch2_o;

  // DiscreteIntegrator: '<S103>/Integrator'
  if (rtb_FixPtRelationalOperator &&
      (control_outer_DW.Integrator_PrevResetState_n <= 0)) {
    control_outer_DW.Integrator_DSTATE_i = 0.0F;
  }

  // DiscreteIntegrator: '<S103>/Integrator'
  rtb_DProdOut_f = 0.0005F * rtb_FilterDifferentiatorTF_n +
    control_outer_DW.Integrator_DSTATE_i;

  // Switch: '<Root>/Switch3' incorporates:
  //   Constant: '<Root>/Constant1'
  //   Constant: '<S14>/Constant'
  //   Constant: '<S17>/Constant'
  //   Logic: '<S3>/OR'
  //   RelationalOperator: '<S14>/Compare'
  //   RelationalOperator: '<S17>/Compare'

  if (rtb_Compare || (rtu_Flags->control_mode == ControlModes_PositionDirect) ||
      (rtu_Flags->control_mode == ControlModes_Velocity)) {
    // Switch: '<S4>/Switch5' incorporates:
    //   Sum: '<S112>/Sum'
    //   Sum: '<S62>/Sum'

    if (rtb_Compare) {
      rtb_UnitDelay1 = (rtb_PProdOut + rtb_FilterDifferentiatorTF) + rtb_Switch2;
    } else {
      rtb_UnitDelay1 += rtb_PProdOut_o + rtb_DProdOut_f;
    }

    // End of Switch: '<S4>/Switch5'
  } else {
    rtb_UnitDelay1 = 0.0F;
  }

  // End of Switch: '<Root>/Switch3'

  // Sum: '<Root>/Sum2'
  rtb_Switch2_o = rtb_UnitDelay1 + rtu_Targets->jointvelocities.velocity;

  // Switch: '<S5>/Switch2' incorporates:
  //   RelationalOperator: '<S5>/LowerRelop1'

  if (rtb_Switch2_o > rtb_Abs) {
    rtb_Switch2 = rtb_Abs;
  } else {
    // Gain: '<Root>/Gain'
    rtb_Switch2 = -rtb_Abs;

    // Switch: '<S5>/Switch' incorporates:
    //   Gain: '<Root>/Gain'
    //   RelationalOperator: '<S5>/UpperRelop'

    if (!(rtb_Switch2_o < -rtb_Abs)) {
      rtb_Switch2 = rtb_Switch2_o;
    }

    // End of Switch: '<S5>/Switch'
  }

  // End of Switch: '<S5>/Switch2'

  // Product: '<Root>/Product2' incorporates:
  //   Sum: '<Root>/Sum1'

  rtb_Switch2_o = (rtb_Switch2 - rtu_Estimates->jointvelocities.velocity) *
    rtu_ConfigurationParameters->motorconfig.reduction;

  // Product: '<S160>/PProd Out'
  rtb_Abs = rtb_Switch2_o * rtu_ConfigurationParameters->VelLoopPID.P;

  // SampleTimeMath: '<S150>/Tsamp'
  //
  //  About '<S150>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_Uk1 = rtu_ConfigurationParameters->VelLoopPID.N * 0.0005F;

  // Math: '<S148>/Reciprocal' incorporates:
  //   Constant: '<S148>/Constant'
  //   Sum: '<S148>/SumDen'
  //
  //  About '<S148>/Reciprocal':
  //   Operator: reciprocal

  rtb_UnitDelay1 = 1.0F / (rtb_Uk1 + 1.0F);

  // DiscreteTransferFcn: '<S148>/Filter Differentiator TF' incorporates:
  //   Constant: '<S148>/Constant'
  //   DiscreteTransferFcn: '<S46>/Filter Differentiator TF'
  //   Product: '<S147>/DProd Out'
  //   Product: '<S148>/Divide'
  //   Sum: '<S148>/SumNum'

  if (rtb_FixPtRelationalOperator &&
      (control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e != 1)) {
    control_outer_DW.FilterDifferentiatorTF_states_c = 0.0F;
  }

  control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e =
    rtb_FixPtRelationalOperator;
  control_outer_DW.FilterDifferentiatorTF_tmp_p = rtb_Switch2_o *
    rtu_ConfigurationParameters->VelLoopPID.D - (rtb_Uk1 - 1.0F) *
    rtb_UnitDelay1 * control_outer_DW.FilterDifferentiatorTF_states_c;

  // Product: '<S158>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S148>/Filter Differentiator TF'
  //   Product: '<S148>/DenCoefOut'

  rtb_PProdOut = (control_outer_DW.FilterDifferentiatorTF_tmp_p +
                  -control_outer_DW.FilterDifferentiatorTF_states_c) *
    rtb_UnitDelay1 * rtu_ConfigurationParameters->VelLoopPID.N;

  // Sum: '<S165>/Sum Fdbk'
  rtb_Uk1 = (rtb_Abs + control_outer_DW.Integrator_DSTATE_b) + rtb_PProdOut;

  // DeadZone: '<S146>/DeadZone' incorporates:
  //   Saturate: '<S163>/Saturation'

  if (rtb_Uk1 > 100.0F) {
    rtb_UnitDelay1 = rtb_Uk1 - 100.0F;
    rtb_PProdOut_o = 100.0F;
  } else {
    if (rtb_Uk1 >= -100.0F) {
      rtb_UnitDelay1 = 0.0F;
    } else {
      rtb_UnitDelay1 = rtb_Uk1 - -100.0F;
    }

    if (rtb_Uk1 < -100.0F) {
      rtb_PProdOut_o = -100.0F;
    } else {
      rtb_PProdOut_o = rtb_Uk1;
    }
  }

  // End of DeadZone: '<S146>/DeadZone'

  // Sum: '<S167>/SumI1' incorporates:
  //   Product: '<S152>/IProd Out'
  //   Sum: '<S166>/SumI3'
  //   UnitDelay: '<Root>/Unit Delay1'

  rtb_Switch2_o = rtb_Switch2_o * rtu_ConfigurationParameters->VelLoopPID.I +
    (control_outer_DW.UnitDelay1_DSTATE - rtb_PProdOut_o);

  // Signum: '<S144>/SignPreSat'
  if (rtb_UnitDelay1 < 0.0F) {
    // DataTypeConversion: '<S144>/DataTypeConv1'
    rtb_PProdOut_o = -1.0F;
  } else if (rtb_UnitDelay1 > 0.0F) {
    // DataTypeConversion: '<S144>/DataTypeConv1'
    rtb_PProdOut_o = 1.0F;
  } else if (rtb_UnitDelay1 == 0.0F) {
    // DataTypeConversion: '<S144>/DataTypeConv1'
    rtb_PProdOut_o = 0.0F;
  } else {
    // DataTypeConversion: '<S144>/DataTypeConv1'
    rtb_PProdOut_o = (rtNaNF);
  }

  // End of Signum: '<S144>/SignPreSat'

  // DataTypeConversion: '<S144>/DataTypeConv1'
  if (rtIsNaNF(rtb_PProdOut_o)) {
    rtb_PProdOut_o = 0.0F;
  } else {
    rtb_PProdOut_o = std::fmod(rtb_PProdOut_o, 256.0F);
  }

  // Signum: '<S144>/SignPreIntegrator'
  if (rtb_Switch2_o < 0.0F) {
    // DataTypeConversion: '<S144>/DataTypeConv2'
    rtb_Sum1 = -1.0F;
  } else if (rtb_Switch2_o > 0.0F) {
    // DataTypeConversion: '<S144>/DataTypeConv2'
    rtb_Sum1 = 1.0F;
  } else if (rtb_Switch2_o == 0.0F) {
    // DataTypeConversion: '<S144>/DataTypeConv2'
    rtb_Sum1 = 0.0F;
  } else {
    // DataTypeConversion: '<S144>/DataTypeConv2'
    rtb_Sum1 = (rtNaNF);
  }

  // End of Signum: '<S144>/SignPreIntegrator'

  // DataTypeConversion: '<S144>/DataTypeConv2'
  if (rtIsNaNF(rtb_Sum1)) {
    rtb_Sum1 = 0.0F;
  } else {
    rtb_Sum1 = std::fmod(rtb_Sum1, 256.0F);
  }

  // Switch: '<S144>/Switch' incorporates:
  //   Constant: '<S144>/Constant1'
  //   DataTypeConversion: '<S144>/DataTypeConv1'
  //   DataTypeConversion: '<S144>/DataTypeConv2'
  //   Gain: '<S144>/ZeroGain'
  //   Logic: '<S144>/AND3'
  //   RelationalOperator: '<S144>/Equal1'
  //   RelationalOperator: '<S144>/NotEqual'

  if ((0.0F * rtb_Uk1 != rtb_UnitDelay1) && ((rtb_PProdOut_o < 0.0F ?
        static_cast<int32_T>(static_cast<int8_T>(-static_cast<int8_T>(
           static_cast<uint8_T>(-rtb_PProdOut_o)))) : static_cast<int32_T>(
         static_cast<int8_T>(static_cast<uint8_T>(rtb_PProdOut_o)))) ==
       (rtb_Sum1 < 0.0F ? static_cast<int32_T>(static_cast<int8_T>(-static_cast<
          int8_T>(static_cast<uint8_T>(-rtb_Sum1)))) : static_cast<int32_T>(
         static_cast<int8_T>(static_cast<uint8_T>(rtb_Sum1)))))) {
    rtb_PProdOut_o = 0.0F;
  } else {
    rtb_PProdOut_o = rtb_Switch2_o;
  }

  // End of Switch: '<S144>/Switch'

  // DiscreteIntegrator: '<S155>/Integrator'
  if (rtb_FixPtRelationalOperator &&
      (control_outer_DW.Integrator_PrevResetState_c <= 0)) {
    control_outer_DW.Integrator_DSTATE_b = 0.0F;
  }

  // DiscreteIntegrator: '<S155>/Integrator'
  rtb_UnitDelay1 = 0.0005F * rtb_PProdOut_o +
    control_outer_DW.Integrator_DSTATE_b;

  // Switch: '<Root>/Switch1'
  if (rtb_Compare_m) {
    // Sum: '<S164>/Sum'
    rtb_Uk1 = (rtb_Abs + rtb_UnitDelay1) + rtb_PProdOut;

    // Saturate: '<S162>/Saturation'
    if (rtb_Uk1 > 100.0F) {
      rtb_Uk1 = 100.0F;
    } else if (rtb_Uk1 < -100.0F) {
      rtb_Uk1 = -100.0F;
    }

    // End of Saturate: '<S162>/Saturation'
  } else {
    rtb_Uk1 = rtu_Targets->motorcurrent.current;
  }

  // End of Switch: '<Root>/Switch1'

  // Signum: '<S1>/Sign'
  if (rtb_Uk1 < 0.0F) {
    rtb_Abs = -1.0F;
  } else if (rtb_Uk1 > 0.0F) {
    rtb_Abs = 1.0F;
  } else if (rtb_Uk1 == 0.0F) {
    rtb_Abs = 0.0F;
  } else {
    rtb_Abs = (rtNaNF);
  }

  // End of Signum: '<S1>/Sign'

  // Sum: '<S1>/Sum' incorporates:
  //   Abs: '<S1>/Abs'

  rtb_Switch2_o = std::abs(rtb_Uk1) -
    rtu_ConfigurationParameters->thresholds.motorPeakCurrents;

  // Saturate: '<S1>/Saturation'
  if (rtb_Switch2_o <= 0.0F) {
    // Saturate: '<S1>/Saturation'
    rtb_PProdOut = 0.0F;
  } else {
    // Saturate: '<S1>/Saturation'
    rtb_PProdOut = rtb_Switch2_o;
  }

  // End of Saturate: '<S1>/Saturation'

  // Logic: '<S1>/Logical Operator' incorporates:
  //   Constant: '<S7>/Constant'
  //   RelationalOperator: '<S7>/Compare'
  //   RelationalOperator: '<S8>/FixPt Relational Operator'
  //   UnitDelay: '<S8>/Delay Input1'
  //
  //  Block description for '<S8>/Delay Input1':
  //
  //   Store in Global RAM

  rtb_Compare = ((rtb_Abs != control_outer_DW.DelayInput1_DSTATE) ||
                 (rtb_Switch2_o <= 0.0F));

  // DiscreteFilter: '<S1>/Discrete Filter'
  if ((((control_outer_PrevZCX.DiscreteFilter_Reset_ZCE == 1) != rtb_Compare) &&
       (control_outer_PrevZCX.DiscreteFilter_Reset_ZCE != 3)) || rtb_Compare) {
    control_outer_DW.DiscreteFilter_states = 0.0F;
    control_outer_DW.DiscreteFilter_denStates = 0.0F;
  }

  control_outer_PrevZCX.DiscreteFilter_Reset_ZCE = rtb_Compare;
  rtb_Sum1 = (0.997506261F * rtb_PProdOut + -0.997506261F *
              control_outer_DW.DiscreteFilter_states) - -0.995012462F *
    control_outer_DW.DiscreteFilter_denStates;
  control_outer_DW.DiscreteFilter_tmp = rtb_Sum1;

  // End of DiscreteFilter: '<S1>/Discrete Filter'

  // Switch: '<S9>/Switch2' incorporates:
  //   RelationalOperator: '<S9>/LowerRelop1'

  if (rtb_Uk1 > rtu_ConfigurationParameters->thresholds.motorPeakCurrents) {
    rtb_Switch2_o = rtu_ConfigurationParameters->thresholds.motorPeakCurrents;
  } else {
    // Gain: '<S1>/Gain'
    rtb_Switch2_o = -rtu_ConfigurationParameters->thresholds.motorPeakCurrents;

    // Switch: '<S9>/Switch' incorporates:
    //   Gain: '<S1>/Gain'
    //   RelationalOperator: '<S9>/UpperRelop'

    if (!(rtb_Uk1 < -rtu_ConfigurationParameters->thresholds.motorPeakCurrents))
    {
      rtb_Switch2_o = rtb_Uk1;
    }

    // End of Switch: '<S9>/Switch'
  }

  // End of Switch: '<S9>/Switch2'

  // Saturate: '<S1>/Saturation1'
  if (rtb_Sum1 <= 0.0F) {
    rtb_Sum1 = 0.0F;
  }

  // End of Saturate: '<S1>/Saturation1'

  // Sum: '<S1>/Sum1' incorporates:
  //   Product: '<S1>/Product'

  rtb_Sum1 = rtb_Abs * rtb_Sum1 + rtb_Switch2_o;

  // BusCreator: '<Root>/Bus Creator2'
  rty_OuterOutputs->motorcurrent.current = rtb_Sum1;

  // BusCreator: '<Root>/Bus Creator1'
  rty_OuterOutputs->vel_en = rtb_Compare_m;

  // Update for UnitDelay: '<S2>/Delay Input1'
  //
  //  Block description for '<S2>/Delay Input1':
  //
  //   Store in Global RAM

  control_outer_DW.DelayInput1_DSTATE_f = rtu_Flags->control_mode;

  // Update for DiscreteTransferFcn: '<S46>/Filter Differentiator TF'
  control_outer_DW.FilterDifferentiatorTF_states =
    control_outer_DW.FilterDifferentiatorTF_tmp;

  // Update for UnitDelay: '<Root>/Unit Delay' incorporates:
  //   Sum: '<Root>/Sum3'

  control_outer_DW.UnitDelay_DSTATE = rtb_Switch2 -
    rtu_Targets->jointvelocities.velocity;

  // Update for DiscreteIntegrator: '<S53>/Integrator'
  control_outer_DW.Integrator_DSTATE = 0.0005F * rtb_DProdOut +
    rtb_FilterDifferentiatorTF;
  control_outer_DW.Integrator_PrevResetState = static_cast<int8_T>
    (rtb_FixPtRelationalOperator);

  // Update for DiscreteTransferFcn: '<S96>/Filter Differentiator TF'
  control_outer_DW.FilterDifferentiatorTF_states_i =
    control_outer_DW.FilterDifferentiatorTF_tmp_m;

  // Update for DiscreteIntegrator: '<S103>/Integrator' incorporates:
  //   DiscreteIntegrator: '<S53>/Integrator'

  control_outer_DW.Integrator_DSTATE_i = 0.0005F * rtb_FilterDifferentiatorTF_n
    + rtb_DProdOut_f;
  control_outer_DW.Integrator_PrevResetState_n = static_cast<int8_T>
    (rtb_FixPtRelationalOperator);

  // Update for DiscreteTransferFcn: '<S148>/Filter Differentiator TF'
  control_outer_DW.FilterDifferentiatorTF_states_c =
    control_outer_DW.FilterDifferentiatorTF_tmp_p;

  // Update for UnitDelay: '<Root>/Unit Delay1'
  control_outer_DW.UnitDelay1_DSTATE = rtb_Sum1;

  // Update for DiscreteIntegrator: '<S155>/Integrator' incorporates:
  //   DiscreteIntegrator: '<S53>/Integrator'

  control_outer_DW.Integrator_DSTATE_b = 0.0005F * rtb_PProdOut_o +
    rtb_UnitDelay1;
  control_outer_DW.Integrator_PrevResetState_c = static_cast<int8_T>
    (rtb_FixPtRelationalOperator);

  // Update for UnitDelay: '<S8>/Delay Input1'
  //
  //  Block description for '<S8>/Delay Input1':
  //
  //   Store in Global RAM

  control_outer_DW.DelayInput1_DSTATE = rtb_Abs;

  // Update for DiscreteFilter: '<S1>/Discrete Filter'
  control_outer_DW.DiscreteFilter_states = rtb_PProdOut;
  control_outer_DW.DiscreteFilter_denStates =
    control_outer_DW.DiscreteFilter_tmp;
}

// Model initialize function
void control_outer_initialize(const char_T **rt_errorStatus)
{
  RT_MODEL_control_outer_T *const control_outer_M = &(control_outer_MdlrefDW.rtm);

  // Registration code

  // initialize non-finites
  rt_InitInfAndNaN(sizeof(real_T));

  // initialize error status
  rtmSetErrorStatusPointer(control_outer_M, rt_errorStatus);
  control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC = POS_ZCSIG;
  control_outer_PrevZCX.FilterDifferentiatorTF_Reset__m = POS_ZCSIG;
  control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e = POS_ZCSIG;
  control_outer_PrevZCX.DiscreteFilter_Reset_ZCE = UNINITIALIZED_ZCSIG;
}

//
// File trailer for generated code.
//
// [EOF]
//
