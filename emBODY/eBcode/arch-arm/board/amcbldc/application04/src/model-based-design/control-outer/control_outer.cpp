//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: control_outer.cpp
//
// Code generated for Simulink model 'control_outer'.
//
// Model version                  : 1.100
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Mon Sep 20 12:44:08 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "control_outer.h"
#include "control_outer_private.h"

// Output and update for referenced model: 'control_outer'
void control_outerModelClass::step(const ControlModes *rtu_Flags_control_mode,
  const boolean_T *rtu_Flags_PID_reset, const real32_T
  rtu_Config_velocitylimits_limit[2], const real32_T
  *rtu_Config_motorconfig_reductio, const boolean_T
  *rtu_Config_motorconfig_has_spee, const real32_T *rtu_Config_PosLoopPID_P,
  const real32_T *rtu_Config_PosLoopPID_I, const real32_T
  *rtu_Config_PosLoopPID_D, const real32_T *rtu_Config_PosLoopPID_N, const
  real32_T *rtu_Config_VelLoopPID_P, const real32_T *rtu_Config_VelLoopPID_I,
  const real32_T *rtu_Config_VelLoopPID_D, const real32_T
  *rtu_Config_VelLoopPID_N, const real32_T *rtu_Config_DirLoopPID_P, const
  real32_T *rtu_Config_DirLoopPID_I, const real32_T *rtu_Config_DirLoopPID_D,
  const real32_T *rtu_Config_DirLoopPID_N, const real32_T
  *rtu_Sensors_jointpositions_posi, const real32_T
  *rtu_Sensors_motorsensors_omega, const real32_T
  *rtu_Targets_jointpositions_posi, const real32_T
  *rtu_Targets_jointvelocities_vel, boolean_T *rty_OuterOutputs_vel_en,
  boolean_T *rty_OuterOutputs_cur_en, boolean_T *rty_OuterOutputs_out_en,
  real32_T *rty_OuterOutputs_motorcurrent_c)
{
  real32_T rtb_DProdOut;
  real32_T rtb_DProdOut_f;
  real32_T rtb_FilterDifferentiatorTF;
  real32_T rtb_FilterDifferentiatorTF_n;
  real32_T rtb_IProdOut_c;
  real32_T rtb_NProdOut_j;
  real32_T rtb_PProdOut;
  real32_T rtb_SignPreIntegrator;
  real32_T rtb_Sum_a;
  real32_T rtb_TSamp;
  real32_T tmp;
  real32_T tmp_0;
  ZCSigState FilterDifferentiatorTF_Reset_ZC;
  int8_T Integrator_PrevResetState_tmp;
  boolean_T rtb_Compare_d;
  boolean_T zcEvent;

  // SampleTimeMath: '<S1>/TSamp'
  //
  //  About '<S1>/TSamp':
  //   y = u * K where K = 1 / ( w * Ts )

  rtb_TSamp = *rtu_Sensors_jointpositions_posi * 1000.0F;

  // RelationalOperator: '<S6>/Compare' incorporates:
  //   Constant: '<S6>/Constant'

  *rty_OuterOutputs_cur_en = (*rtu_Flags_control_mode != ControlModes_Voltage);

  // RelationalOperator: '<S7>/Compare' incorporates:
  //   Constant: '<S7>/Constant'

  rtb_Compare_d = (*rtu_Flags_control_mode == ControlModes_Position);

  // RelationalOperator: '<S8>/Compare' incorporates:
  //   Constant: '<S8>/Constant'

  *rty_OuterOutputs_vel_en = (*rtu_Flags_control_mode != ControlModes_Current);

  // Logic: '<S2>/NOR' incorporates:
  //   Constant: '<S11>/Constant'
  //   Constant: '<S12>/Constant'
  //   Constant: '<S9>/Constant'
  //   RelationalOperator: '<S11>/Compare'
  //   RelationalOperator: '<S12>/Compare'
  //   RelationalOperator: '<S9>/Compare'

  *rty_OuterOutputs_out_en = ((!(*rtu_Flags_control_mode ==
    ControlModes_NotConfigured)) && (!(*rtu_Flags_control_mode ==
    ControlModes_Idle)) && (!(*rtu_Flags_control_mode == ControlModes_HwFaultCM)));

  // Sum: '<S3>/Sum3'
  rtb_SignPreIntegrator = *rtu_Targets_jointpositions_posi -
    *rtu_Sensors_jointpositions_posi;

  // Product: '<S41>/DProd Out'
  rtb_DProdOut = rtb_SignPreIntegrator * *rtu_Config_PosLoopPID_D;

  // SampleTimeMath: '<S44>/Tsamp'
  //
  //  About '<S44>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_NProdOut_j = *rtu_Config_PosLoopPID_N * 0.0005F;

  // Math: '<S42>/Reciprocal' incorporates:
  //   Constant: '<S42>/Constant'
  //   Sum: '<S42>/SumDen'
  //
  //  About '<S42>/Reciprocal':
  //   Operator: reciprocal

  rtb_IProdOut_c = 1.0F / (rtb_NProdOut_j + 1.0F);

  // DiscreteTransferFcn: '<S42>/Filter Differentiator TF'
  zcEvent = (((control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC == 1) !=
              *rtu_Flags_PID_reset) &&
             (control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC != 3));
  if (zcEvent || (*rtu_Flags_PID_reset)) {
    control_outer_DW.FilterDifferentiatorTF_states = 0.0F;
  }

  FilterDifferentiatorTF_Reset_ZC = *rtu_Flags_PID_reset;

  // DiscreteTransferFcn: '<S42>/Filter Differentiator TF' incorporates:
  //   Constant: '<S42>/Constant'
  //   Product: '<S42>/Divide'
  //   Sum: '<S42>/SumNum'

  control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC =
    FilterDifferentiatorTF_Reset_ZC;
  control_outer_DW.FilterDifferentiatorTF_tmp = rtb_DProdOut - (rtb_NProdOut_j -
    1.0F) * rtb_IProdOut_c * control_outer_DW.FilterDifferentiatorTF_states;

  // Product: '<S46>/IProd Out'
  rtb_DProdOut = rtb_SignPreIntegrator * *rtu_Config_PosLoopPID_I;

  // Product: '<S54>/PProd Out'
  rtb_PProdOut = rtb_SignPreIntegrator * *rtu_Config_PosLoopPID_P;

  // Product: '<S52>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S42>/Filter Differentiator TF'
  //   Product: '<S42>/DenCoefOut'

  rtb_IProdOut_c = (control_outer_DW.FilterDifferentiatorTF_tmp +
                    -control_outer_DW.FilterDifferentiatorTF_states) *
    rtb_IProdOut_c * *rtu_Config_PosLoopPID_N;

  // Sum: '<S61>/SumI1' incorporates:
  //   Sum: '<S59>/Sum Fdbk'
  //   Sum: '<S60>/SumI3'
  //   UnitDelay: '<Root>/Unit Delay'

  rtb_FilterDifferentiatorTF = (control_outer_DW.UnitDelay_DSTATE -
    ((rtb_PProdOut + control_outer_DW.Integrator_DSTATE) + rtb_IProdOut_c)) +
    rtb_DProdOut;

  // DiscreteIntegrator: '<S49>/Integrator'
  if ((*rtu_Flags_PID_reset) || (control_outer_DW.Integrator_PrevResetState != 0))
  {
    control_outer_DW.Integrator_DSTATE = 0.0F;
  }

  // DiscreteIntegrator: '<S49>/Integrator'
  rtb_DProdOut = 0.0005F * rtb_FilterDifferentiatorTF +
    control_outer_DW.Integrator_DSTATE;

  // Product: '<S91>/DProd Out'
  rtb_DProdOut_f = rtb_SignPreIntegrator * *rtu_Config_DirLoopPID_D;

  // SampleTimeMath: '<S94>/Tsamp'
  //
  //  About '<S94>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_NProdOut_j = *rtu_Config_DirLoopPID_N * 0.0005F;

  // Math: '<S92>/Reciprocal' incorporates:
  //   Constant: '<S92>/Constant'
  //   Sum: '<S92>/SumDen'
  //
  //  About '<S92>/Reciprocal':
  //   Operator: reciprocal

  rtb_Sum_a = 1.0F / (rtb_NProdOut_j + 1.0F);

  // DiscreteTransferFcn: '<S92>/Filter Differentiator TF' incorporates:
  //   Constant: '<S92>/Constant'
  //   Product: '<S92>/Divide'
  //   Sum: '<S92>/SumNum'

  zcEvent = (((control_outer_PrevZCX.FilterDifferentiatorTF_Reset__m == 1) !=
              *rtu_Flags_PID_reset) &&
             (control_outer_PrevZCX.FilterDifferentiatorTF_Reset__m != 3));
  if (zcEvent || (*rtu_Flags_PID_reset)) {
    control_outer_DW.FilterDifferentiatorTF_states_i = 0.0F;
  }

  control_outer_PrevZCX.FilterDifferentiatorTF_Reset__m =
    FilterDifferentiatorTF_Reset_ZC;
  control_outer_DW.FilterDifferentiatorTF_tmp_m = rtb_DProdOut_f -
    (rtb_NProdOut_j - 1.0F) * rtb_Sum_a *
    control_outer_DW.FilterDifferentiatorTF_states_i;

  // Product: '<S96>/IProd Out'
  rtb_DProdOut_f = rtb_SignPreIntegrator * *rtu_Config_DirLoopPID_I;

  // Product: '<S104>/PProd Out'
  rtb_SignPreIntegrator *= *rtu_Config_DirLoopPID_P;

  // Product: '<S102>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S92>/Filter Differentiator TF'
  //   Product: '<S92>/DenCoefOut'

  rtb_Sum_a = (control_outer_DW.FilterDifferentiatorTF_tmp_m +
               -control_outer_DW.FilterDifferentiatorTF_states_i) * rtb_Sum_a * *
    rtu_Config_DirLoopPID_N;

  // Sum: '<S111>/SumI1' incorporates:
  //   Sum: '<S109>/Sum Fdbk'
  //   Sum: '<S110>/SumI3'
  //   UnitDelay: '<Root>/Unit Delay'

  rtb_FilterDifferentiatorTF_n = (control_outer_DW.UnitDelay_DSTATE -
    ((rtb_SignPreIntegrator + control_outer_DW.Integrator_DSTATE_i) + rtb_Sum_a))
    + rtb_DProdOut_f;

  // DiscreteIntegrator: '<S99>/Integrator'
  if ((*rtu_Flags_PID_reset) || (control_outer_DW.Integrator_PrevResetState_n !=
       0)) {
    control_outer_DW.Integrator_DSTATE_i = 0.0F;
  }

  // DiscreteIntegrator: '<S99>/Integrator'
  rtb_DProdOut_f = 0.0005F * rtb_FilterDifferentiatorTF_n +
    control_outer_DW.Integrator_DSTATE_i;

  // Switch: '<Root>/Switch3' incorporates:
  //   Constant: '<Root>/Constant1'
  //   Constant: '<S10>/Constant'
  //   Constant: '<S13>/Constant'
  //   Logic: '<S2>/OR'
  //   RelationalOperator: '<S10>/Compare'
  //   RelationalOperator: '<S13>/Compare'

  if (rtb_Compare_d || (*rtu_Flags_control_mode == ControlModes_PositionDirect) ||
      (*rtu_Flags_control_mode == ControlModes_Velocity)) {
    // Switch: '<S3>/Switch5' incorporates:
    //   Sum: '<S108>/Sum'
    //   Sum: '<S58>/Sum'

    if (rtb_Compare_d) {
      rtb_SignPreIntegrator = (rtb_PProdOut + rtb_DProdOut) + rtb_IProdOut_c;
    } else {
      rtb_SignPreIntegrator = (rtb_SignPreIntegrator + rtb_DProdOut_f) +
        rtb_Sum_a;
    }

    // End of Switch: '<S3>/Switch5'
  } else {
    rtb_SignPreIntegrator = 0.0F;
  }

  // End of Switch: '<Root>/Switch3'

  // Sum: '<Root>/Sum2'
  rtb_Sum_a = rtb_SignPreIntegrator + *rtu_Targets_jointvelocities_vel;

  // Switch: '<S4>/Switch2' incorporates:
  //   RelationalOperator: '<S4>/LowerRelop1'
  //   RelationalOperator: '<S4>/UpperRelop'
  //   Switch: '<S4>/Switch'

  if (rtb_Sum_a > rtu_Config_velocitylimits_limit[1]) {
    rtb_Sum_a = rtu_Config_velocitylimits_limit[1];
  } else if (rtb_Sum_a < rtu_Config_velocitylimits_limit[0]) {
    // Switch: '<S4>/Switch'
    rtb_Sum_a = rtu_Config_velocitylimits_limit[0];
  }

  // End of Switch: '<S4>/Switch2'

  // Product: '<Root>/Product2'
  rtb_SignPreIntegrator = *rtu_Config_motorconfig_reductio * rtb_Sum_a;

  // Switch: '<Root>/Switch'
  if (*rtu_Config_motorconfig_has_spee) {
    rtb_IProdOut_c = *rtu_Sensors_motorsensors_omega;
  } else {
    // Product: '<Root>/Product1' incorporates:
    //   Sum: '<S1>/Diff'
    //   UnitDelay: '<S1>/UD'
    //
    //  Block description for '<S1>/Diff':
    //
    //   Add in CPU
    //
    //  Block description for '<S1>/UD':
    //
    //   Store in Global RAM

    rtb_IProdOut_c = (rtb_TSamp - control_outer_DW.UD_DSTATE) *
      *rtu_Config_motorconfig_reductio;
  }

  // End of Switch: '<Root>/Switch'

  // Sum: '<Root>/Sum1'
  rtb_IProdOut_c = rtb_SignPreIntegrator - rtb_IProdOut_c;

  // Update for UnitDelay: '<Root>/Unit Delay' incorporates:
  //   Sum: '<Root>/Sum3'

  control_outer_DW.UnitDelay_DSTATE = rtb_Sum_a -
    *rtu_Targets_jointvelocities_vel;

  // Product: '<S156>/PProd Out'
  rtb_Sum_a = rtb_IProdOut_c * *rtu_Config_VelLoopPID_P;

  // Product: '<S143>/DProd Out'
  rtb_PProdOut = rtb_IProdOut_c * *rtu_Config_VelLoopPID_D;

  // SampleTimeMath: '<S146>/Tsamp'
  //
  //  About '<S146>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_NProdOut_j = *rtu_Config_VelLoopPID_N * 0.0005F;

  // Math: '<S144>/Reciprocal' incorporates:
  //   Constant: '<S144>/Constant'
  //   Sum: '<S144>/SumDen'
  //
  //  About '<S144>/Reciprocal':
  //   Operator: reciprocal

  rtb_SignPreIntegrator = 1.0F / (rtb_NProdOut_j + 1.0F);

  // DiscreteTransferFcn: '<S144>/Filter Differentiator TF' incorporates:
  //   Constant: '<S144>/Constant'
  //   Product: '<S144>/Divide'
  //   Sum: '<S144>/SumNum'

  zcEvent = (((control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e == 1) !=
              *rtu_Flags_PID_reset) &&
             (control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e != 3));
  if (zcEvent || (*rtu_Flags_PID_reset)) {
    control_outer_DW.FilterDifferentiatorTF_states_c = 0.0F;
  }

  control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e =
    FilterDifferentiatorTF_Reset_ZC;
  control_outer_DW.FilterDifferentiatorTF_tmp_p = rtb_PProdOut - (rtb_NProdOut_j
    - 1.0F) * rtb_SignPreIntegrator *
    control_outer_DW.FilterDifferentiatorTF_states_c;

  // Product: '<S154>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S144>/Filter Differentiator TF'
  //   Product: '<S144>/DenCoefOut'

  rtb_NProdOut_j = (control_outer_DW.FilterDifferentiatorTF_tmp_p +
                    -control_outer_DW.FilterDifferentiatorTF_states_c) *
    rtb_SignPreIntegrator * *rtu_Config_VelLoopPID_N;

  // Sum: '<S161>/Sum Fdbk'
  rtb_SignPreIntegrator = (rtb_Sum_a + control_outer_DW.Integrator_DSTATE_b) +
    rtb_NProdOut_j;

  // Gain: '<S140>/ZeroGain'
  rtb_PProdOut = 0.0F * rtb_SignPreIntegrator;

  // DeadZone: '<S142>/DeadZone'
  if (rtb_SignPreIntegrator > 100.0F) {
    rtb_SignPreIntegrator -= 100.0F;
  } else if (rtb_SignPreIntegrator >= -100.0F) {
    rtb_SignPreIntegrator = 0.0F;
  } else {
    rtb_SignPreIntegrator -= -100.0F;
  }

  // End of DeadZone: '<S142>/DeadZone'

  // Product: '<S148>/IProd Out'
  rtb_IProdOut_c *= *rtu_Config_VelLoopPID_I;

  // Signum: '<S140>/SignPreSat'
  if (rtb_SignPreIntegrator < 0.0F) {
    // DataTypeConversion: '<S140>/DataTypeConv1'
    tmp = -1.0F;
  } else if (rtb_SignPreIntegrator > 0.0F) {
    // DataTypeConversion: '<S140>/DataTypeConv1'
    tmp = 1.0F;
  } else if (rtb_SignPreIntegrator == 0.0F) {
    // DataTypeConversion: '<S140>/DataTypeConv1'
    tmp = 0.0F;
  } else {
    // DataTypeConversion: '<S140>/DataTypeConv1'
    tmp = (rtNaNF);
  }

  // End of Signum: '<S140>/SignPreSat'

  // DataTypeConversion: '<S140>/DataTypeConv1'
  if (rtIsNaNF(tmp)) {
    tmp = 0.0F;
  } else {
    tmp = std::fmod(tmp, 256.0F);
  }

  // Signum: '<S140>/SignPreIntegrator'
  if (rtb_IProdOut_c < 0.0F) {
    // DataTypeConversion: '<S140>/DataTypeConv2'
    tmp_0 = -1.0F;
  } else if (rtb_IProdOut_c > 0.0F) {
    // DataTypeConversion: '<S140>/DataTypeConv2'
    tmp_0 = 1.0F;
  } else if (rtb_IProdOut_c == 0.0F) {
    // DataTypeConversion: '<S140>/DataTypeConv2'
    tmp_0 = 0.0F;
  } else {
    // DataTypeConversion: '<S140>/DataTypeConv2'
    tmp_0 = (rtNaNF);
  }

  // End of Signum: '<S140>/SignPreIntegrator'

  // DataTypeConversion: '<S140>/DataTypeConv2'
  if (rtIsNaNF(tmp_0)) {
    tmp_0 = 0.0F;
  } else {
    tmp_0 = std::fmod(tmp_0, 256.0F);
  }

  // Switch: '<S140>/Switch' incorporates:
  //   Constant: '<S140>/Constant1'
  //   DataTypeConversion: '<S140>/DataTypeConv1'
  //   DataTypeConversion: '<S140>/DataTypeConv2'
  //   Logic: '<S140>/AND3'
  //   RelationalOperator: '<S140>/Equal1'
  //   RelationalOperator: '<S140>/NotEqual'

  if ((rtb_PProdOut != rtb_SignPreIntegrator) && ((tmp < 0.0F ?
        static_cast<int32_T>(static_cast<int8_T>(-static_cast<int8_T>(
           static_cast<uint8_T>(-tmp)))) : static_cast<int32_T>
        (static_cast<int8_T>(static_cast<uint8_T>(tmp)))) == (tmp_0 < 0.0F ?
        static_cast<int32_T>(static_cast<int8_T>(-static_cast<int8_T>(
           static_cast<uint8_T>(-tmp_0)))) : static_cast<int32_T>
        (static_cast<int8_T>(static_cast<uint8_T>(tmp_0)))))) {
    rtb_PProdOut = 0.0F;
  } else {
    rtb_PProdOut = rtb_IProdOut_c;
  }

  // End of Switch: '<S140>/Switch'

  // DiscreteIntegrator: '<S151>/Integrator'
  if ((*rtu_Flags_PID_reset) || (control_outer_DW.Integrator_PrevResetState_c !=
       0)) {
    control_outer_DW.Integrator_DSTATE_b = 0.0F;
  }

  // DiscreteIntegrator: '<S151>/Integrator'
  rtb_SignPreIntegrator = 0.0005F * rtb_PProdOut +
    control_outer_DW.Integrator_DSTATE_b;

  // Sum: '<S160>/Sum'
  rtb_NProdOut_j += rtb_Sum_a + rtb_SignPreIntegrator;

  // Saturate: '<S158>/Saturation'
  if (rtb_NProdOut_j > 100.0F) {
    *rty_OuterOutputs_motorcurrent_c = 100.0F;
  } else if (rtb_NProdOut_j < -100.0F) {
    *rty_OuterOutputs_motorcurrent_c = -100.0F;
  } else {
    *rty_OuterOutputs_motorcurrent_c = rtb_NProdOut_j;
  }

  // End of Saturate: '<S158>/Saturation'

  // Update for UnitDelay: '<S1>/UD'
  //
  //  Block description for '<S1>/UD':
  //
  //   Store in Global RAM

  control_outer_DW.UD_DSTATE = rtb_TSamp;

  // Update for DiscreteTransferFcn: '<S42>/Filter Differentiator TF'
  control_outer_DW.FilterDifferentiatorTF_states =
    control_outer_DW.FilterDifferentiatorTF_tmp;

  // Update for DiscreteIntegrator: '<S49>/Integrator'
  control_outer_DW.Integrator_DSTATE = 0.0005F * rtb_FilterDifferentiatorTF +
    rtb_DProdOut;
  Integrator_PrevResetState_tmp = static_cast<int8_T>(*rtu_Flags_PID_reset);

  // Update for DiscreteIntegrator: '<S49>/Integrator'
  control_outer_DW.Integrator_PrevResetState = Integrator_PrevResetState_tmp;

  // Update for DiscreteTransferFcn: '<S92>/Filter Differentiator TF'
  control_outer_DW.FilterDifferentiatorTF_states_i =
    control_outer_DW.FilterDifferentiatorTF_tmp_m;

  // Update for DiscreteIntegrator: '<S99>/Integrator'
  control_outer_DW.Integrator_DSTATE_i = 0.0005F * rtb_FilterDifferentiatorTF_n
    + rtb_DProdOut_f;
  control_outer_DW.Integrator_PrevResetState_n = Integrator_PrevResetState_tmp;

  // Update for DiscreteTransferFcn: '<S144>/Filter Differentiator TF'
  control_outer_DW.FilterDifferentiatorTF_states_c =
    control_outer_DW.FilterDifferentiatorTF_tmp_p;

  // Update for DiscreteIntegrator: '<S151>/Integrator'
  control_outer_DW.Integrator_DSTATE_b = 0.0005F * rtb_PProdOut +
    rtb_SignPreIntegrator;
  control_outer_DW.Integrator_PrevResetState_c = Integrator_PrevResetState_tmp;
}

// Model initialize function
void control_outerModelClass::initialize()
{
  // Registration code

  // initialize non-finites
  rt_InitInfAndNaN(sizeof(real_T));
  control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC = UNINITIALIZED_ZCSIG;
  control_outer_PrevZCX.FilterDifferentiatorTF_Reset__m = UNINITIALIZED_ZCSIG;
  control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e = UNINITIALIZED_ZCSIG;
}

// Constructor
control_outerModelClass::control_outerModelClass() :
  control_outer_DW(),
  control_outer_PrevZCX()
{
  // Currently there is no constructor body generated.
}

// Destructor
control_outerModelClass::~control_outerModelClass()
{
  // Currently there is no destructor body generated.
}

// Real-Time Model get method
control_outerModelClass::RT_MODEL_control_outer_T * control_outerModelClass::
  getRTM()
{
  return (&control_outer_M);
}

// member function to setup error status pointer
void control_outerModelClass::setErrorStatusPointer(const char_T
  **rt_errorStatus)
{
  rtmSetErrorStatusPointer((&control_outer_M), rt_errorStatus);
}

//
// File trailer for generated code.
//
// [EOF]
//
