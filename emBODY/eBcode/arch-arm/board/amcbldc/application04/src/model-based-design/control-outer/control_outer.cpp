//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_outer.cpp
//
// Code generated for Simulink model 'control_outer'.
//
// Model version                  : 2.24
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Mon Dec 13 08:28:18 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "control_outer.h"
#include "control_outer_private.h"

namespace amc_bldc_codegen
{
  // System initialize for referenced model: 'control_outer'
  void control_outer::init(void)
  {
    // InitializeConditions for UnitDelay: '<S1>/Delay Input1'
    //
    //  Block description for '<S1>/Delay Input1':
    //
    //   Store in Global RAM

    control_outer_DW.DelayInput1_DSTATE = ControlModes_Idle;
  }

  // Output and update for referenced model: 'control_outer'
  void control_outer::step(const Flags &arg_Flags, const ConfigurationParameters
    &arg_ConfigurationParameters, const Targets &arg_Targets, const SensorsData
    &arg_Sensors, const EstimatedData &arg_Estimates, ControlOuterOutputs &
    arg_OuterOutputs)
  {
    real32_T rtb_DProdOut;
    real32_T rtb_DProdOut_f;
    real32_T rtb_FilterDifferentiatorTF;
    real32_T rtb_FilterDifferentiatorTF_n;
    real32_T rtb_NProdOut_j;
    real32_T rtb_PProdOut;
    real32_T rtb_PProdOut_o;
    real32_T rtb_Saturation;
    real32_T rtb_SignPreIntegrator;
    real32_T rtb_Switch2;
    real32_T rtb_TSamp;
    real32_T tmp;
    real32_T tmp_0;
    boolean_T rtb_Compare;
    boolean_T rtb_FixPtRelationalOperator;

    // RelationalOperator: '<S9>/Compare' incorporates:
    //   Constant: '<S9>/Constant'

    arg_OuterOutputs.vel_en = (arg_Flags.control_mode != ControlModes_Current);

    // RelationalOperator: '<S7>/Compare' incorporates:
    //   Constant: '<S7>/Constant'

    arg_OuterOutputs.cur_en = (arg_Flags.control_mode != ControlModes_Voltage);

    // Logic: '<S3>/NOR' incorporates:
    //   Constant: '<S10>/Constant'
    //   Constant: '<S12>/Constant'
    //   Constant: '<S13>/Constant'
    //   RelationalOperator: '<S10>/Compare'
    //   RelationalOperator: '<S12>/Compare'
    //   RelationalOperator: '<S13>/Compare'

    arg_OuterOutputs.out_en = ((!(arg_Flags.control_mode ==
      ControlModes_NotConfigured)) && (!(arg_Flags.control_mode ==
      ControlModes_Idle)) && (!(arg_Flags.control_mode == ControlModes_HwFaultCM)));

    // SampleTimeMath: '<S2>/TSamp'
    //
    //  About '<S2>/TSamp':
    //   y = u * K where K = 1 / ( w * Ts )

    rtb_TSamp = arg_Sensors.jointpositions.position * 1000.0F;

    // Sum: '<S4>/Sum3'
    rtb_NProdOut_j = arg_Targets.jointpositions.position -
      arg_Sensors.jointpositions.position;

    // Product: '<S55>/PProd Out'
    rtb_PProdOut = rtb_NProdOut_j * arg_ConfigurationParameters.PosLoopPID.P;

    // SampleTimeMath: '<S45>/Tsamp'
    //
    //  About '<S45>/Tsamp':
    //   y = u * K where K = ( w * Ts )

    rtb_Saturation = arg_ConfigurationParameters.PosLoopPID.N * 0.0005F;

    // Math: '<S43>/Reciprocal' incorporates:
    //   Constant: '<S43>/Constant'
    //   Sum: '<S43>/SumDen'
    //
    //  About '<S43>/Reciprocal':
    //   Operator: reciprocal

    rtb_Switch2 = 1.0F / (rtb_Saturation + 1.0F);

    // RelationalOperator: '<S1>/FixPt Relational Operator' incorporates:
    //   UnitDelay: '<S1>/Delay Input1'
    //
    //  Block description for '<S1>/Delay Input1':
    //
    //   Store in Global RAM

    rtb_FixPtRelationalOperator = (arg_Flags.control_mode !=
      control_outer_DW.DelayInput1_DSTATE);

    // DiscreteTransferFcn: '<S43>/Filter Differentiator TF' incorporates:
    //   Constant: '<S43>/Constant'
    //   Product: '<S42>/DProd Out'
    //   Product: '<S43>/Divide'
    //   Sum: '<S43>/SumNum'

    if ((((control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC == 1) !=
          rtb_FixPtRelationalOperator) &&
         (control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC != 3)) ||
        rtb_FixPtRelationalOperator) {
      control_outer_DW.FilterDifferentiatorTF_states = 0.0F;
    }

    control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC =
      rtb_FixPtRelationalOperator;
    control_outer_DW.FilterDifferentiatorTF_tmp = rtb_NProdOut_j *
      arg_ConfigurationParameters.PosLoopPID.D - (rtb_Saturation - 1.0F) *
      rtb_Switch2 * control_outer_DW.FilterDifferentiatorTF_states;

    // Product: '<S53>/NProd Out' incorporates:
    //   DiscreteTransferFcn: '<S43>/Filter Differentiator TF'
    //   Product: '<S43>/DenCoefOut'

    rtb_Switch2 = (control_outer_DW.FilterDifferentiatorTF_tmp +
                   -control_outer_DW.FilterDifferentiatorTF_states) *
      rtb_Switch2 * arg_ConfigurationParameters.PosLoopPID.N;

    // Sum: '<S62>/SumI1' incorporates:
    //   Product: '<S47>/IProd Out'
    //   Sum: '<S60>/Sum Fdbk'
    //   Sum: '<S61>/SumI3'
    //   UnitDelay: '<Root>/Unit Delay'

    rtb_DProdOut = (control_outer_DW.UnitDelay_DSTATE - ((rtb_PProdOut +
      control_outer_DW.Integrator_DSTATE) + rtb_Switch2)) + rtb_NProdOut_j *
      arg_ConfigurationParameters.PosLoopPID.I;

    // DiscreteIntegrator: '<S50>/Integrator'
    if (rtb_FixPtRelationalOperator ||
        (control_outer_DW.Integrator_PrevResetState != 0)) {
      control_outer_DW.Integrator_DSTATE = 0.0F;
    }

    // DiscreteIntegrator: '<S50>/Integrator'
    rtb_FilterDifferentiatorTF = 0.0005F * rtb_DProdOut +
      control_outer_DW.Integrator_DSTATE;

    // RelationalOperator: '<S8>/Compare' incorporates:
    //   Constant: '<S8>/Constant'

    rtb_Compare = (arg_Flags.control_mode == ControlModes_Position);

    // Product: '<S105>/PProd Out'
    rtb_PProdOut_o = rtb_NProdOut_j * arg_ConfigurationParameters.DirLoopPID.P;

    // Product: '<S97>/IProd Out'
    rtb_Saturation = rtb_NProdOut_j * arg_ConfigurationParameters.DirLoopPID.I;

    // Product: '<S92>/DProd Out'
    rtb_DProdOut_f = rtb_NProdOut_j * arg_ConfigurationParameters.DirLoopPID.D;

    // SampleTimeMath: '<S95>/Tsamp'
    //
    //  About '<S95>/Tsamp':
    //   y = u * K where K = ( w * Ts )

    rtb_SignPreIntegrator = arg_ConfigurationParameters.DirLoopPID.N * 0.0005F;

    // Math: '<S93>/Reciprocal' incorporates:
    //   Constant: '<S93>/Constant'
    //   Sum: '<S93>/SumDen'
    //
    //  About '<S93>/Reciprocal':
    //   Operator: reciprocal

    rtb_NProdOut_j = 1.0F / (rtb_SignPreIntegrator + 1.0F);

    // DiscreteTransferFcn: '<S93>/Filter Differentiator TF' incorporates:
    //   Constant: '<S93>/Constant'
    //   Product: '<S93>/Divide'
    //   Sum: '<S93>/SumNum'

    if ((((control_outer_PrevZCX.FilterDifferentiatorTF_Reset__m == 1) !=
          rtb_FixPtRelationalOperator) &&
         (control_outer_PrevZCX.FilterDifferentiatorTF_Reset__m != 3)) ||
        rtb_FixPtRelationalOperator) {
      control_outer_DW.FilterDifferentiatorTF_states_i = 0.0F;
    }

    control_outer_PrevZCX.FilterDifferentiatorTF_Reset__m =
      rtb_FixPtRelationalOperator;
    control_outer_DW.FilterDifferentiatorTF_tmp_m = rtb_DProdOut_f -
      (rtb_SignPreIntegrator - 1.0F) * rtb_NProdOut_j *
      control_outer_DW.FilterDifferentiatorTF_states_i;

    // Product: '<S103>/NProd Out' incorporates:
    //   DiscreteTransferFcn: '<S93>/Filter Differentiator TF'
    //   Product: '<S93>/DenCoefOut'

    rtb_NProdOut_j = (control_outer_DW.FilterDifferentiatorTF_tmp_m +
                      -control_outer_DW.FilterDifferentiatorTF_states_i) *
      rtb_NProdOut_j * arg_ConfigurationParameters.DirLoopPID.N;

    // Sum: '<S112>/SumI1' incorporates:
    //   Sum: '<S110>/Sum Fdbk'
    //   Sum: '<S111>/SumI3'
    //   UnitDelay: '<Root>/Unit Delay'

    rtb_FilterDifferentiatorTF_n = (control_outer_DW.UnitDelay_DSTATE -
      ((rtb_PProdOut_o + control_outer_DW.Integrator_DSTATE_i) + rtb_NProdOut_j))
      + rtb_Saturation;

    // DiscreteIntegrator: '<S100>/Integrator'
    if (rtb_FixPtRelationalOperator ||
        (control_outer_DW.Integrator_PrevResetState_n != 0)) {
      control_outer_DW.Integrator_DSTATE_i = 0.0F;
    }

    // DiscreteIntegrator: '<S100>/Integrator'
    rtb_DProdOut_f = 0.0005F * rtb_FilterDifferentiatorTF_n +
      control_outer_DW.Integrator_DSTATE_i;

    // Switch: '<Root>/Switch3' incorporates:
    //   Constant: '<Root>/Constant1'
    //   Constant: '<S11>/Constant'
    //   Constant: '<S14>/Constant'
    //   Logic: '<S3>/OR'
    //   RelationalOperator: '<S11>/Compare'
    //   RelationalOperator: '<S14>/Compare'

    if (rtb_Compare || (arg_Flags.control_mode == ControlModes_PositionDirect) ||
        (arg_Flags.control_mode == ControlModes_Velocity)) {
      // Switch: '<S4>/Switch5' incorporates:
      //   Sum: '<S109>/Sum'
      //   Sum: '<S59>/Sum'

      if (rtb_Compare) {
        rtb_Switch2 += rtb_PProdOut + rtb_FilterDifferentiatorTF;
      } else {
        rtb_Switch2 = (rtb_PProdOut_o + rtb_DProdOut_f) + rtb_NProdOut_j;
      }

      // End of Switch: '<S4>/Switch5'
    } else {
      rtb_Switch2 = 0.0F;
    }

    // End of Switch: '<Root>/Switch3'

    // Sum: '<Root>/Sum2'
    rtb_Saturation = rtb_Switch2 + arg_Targets.jointvelocities.velocity;

    // Switch: '<S5>/Switch2' incorporates:
    //   RelationalOperator: '<S5>/LowerRelop1'
    //   RelationalOperator: '<S5>/UpperRelop'
    //   Switch: '<S5>/Switch'

    if (rtb_Saturation > arg_ConfigurationParameters.velocitylimits.limits[1]) {
      rtb_Switch2 = arg_ConfigurationParameters.velocitylimits.limits[1];
    } else if (rtb_Saturation <
               arg_ConfigurationParameters.velocitylimits.limits[0]) {
      // Switch: '<S5>/Switch'
      rtb_Switch2 = arg_ConfigurationParameters.velocitylimits.limits[0];
    } else {
      rtb_Switch2 = rtb_Saturation;
    }

    // End of Switch: '<S5>/Switch2'

    // Switch: '<Root>/Switch' incorporates:
    //   Product: '<Root>/Product1'
    //   Sum: '<S2>/Diff'
    //   UnitDelay: '<S2>/UD'
    //
    //  Block description for '<S2>/Diff':
    //
    //   Add in CPU
    //
    //  Block description for '<S2>/UD':
    //
    //   Store in Global RAM

    if (arg_ConfigurationParameters.motorconfig.has_speed_sens) {
      rtb_NProdOut_j = arg_Estimates.jointvelocities.velocity;
    } else {
      rtb_NProdOut_j = (rtb_TSamp - control_outer_DW.UD_DSTATE) *
        arg_ConfigurationParameters.motorconfig.reduction;
    }

    // End of Switch: '<Root>/Switch'

    // Sum: '<Root>/Sum1' incorporates:
    //   Product: '<Root>/Product2'

    rtb_PProdOut = arg_ConfigurationParameters.motorconfig.reduction *
      rtb_Switch2 - rtb_NProdOut_j;

    // Product: '<S157>/PProd Out'
    rtb_Saturation = rtb_PProdOut * arg_ConfigurationParameters.VelLoopPID.P;

    // SampleTimeMath: '<S147>/Tsamp'
    //
    //  About '<S147>/Tsamp':
    //   y = u * K where K = ( w * Ts )

    rtb_SignPreIntegrator = arg_ConfigurationParameters.VelLoopPID.N * 0.0005F;

    // Math: '<S145>/Reciprocal' incorporates:
    //   Constant: '<S145>/Constant'
    //   Sum: '<S145>/SumDen'
    //
    //  About '<S145>/Reciprocal':
    //   Operator: reciprocal

    rtb_NProdOut_j = 1.0F / (rtb_SignPreIntegrator + 1.0F);

    // DiscreteTransferFcn: '<S145>/Filter Differentiator TF' incorporates:
    //   Constant: '<S145>/Constant'
    //   Product: '<S144>/DProd Out'
    //   Product: '<S145>/Divide'
    //   Sum: '<S145>/SumNum'

    if ((((control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e == 1) !=
          rtb_FixPtRelationalOperator) &&
         (control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e != 3)) ||
        rtb_FixPtRelationalOperator) {
      control_outer_DW.FilterDifferentiatorTF_states_c = 0.0F;
    }

    control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e =
      rtb_FixPtRelationalOperator;
    control_outer_DW.FilterDifferentiatorTF_tmp_p = rtb_PProdOut *
      arg_ConfigurationParameters.VelLoopPID.D - (rtb_SignPreIntegrator - 1.0F) *
      rtb_NProdOut_j * control_outer_DW.FilterDifferentiatorTF_states_c;

    // Product: '<S155>/NProd Out' incorporates:
    //   DiscreteTransferFcn: '<S145>/Filter Differentiator TF'
    //   Product: '<S145>/DenCoefOut'

    rtb_NProdOut_j = (control_outer_DW.FilterDifferentiatorTF_tmp_p +
                      -control_outer_DW.FilterDifferentiatorTF_states_c) *
      rtb_NProdOut_j * arg_ConfigurationParameters.VelLoopPID.N;

    // Sum: '<S162>/Sum Fdbk'
    rtb_SignPreIntegrator = (rtb_Saturation +
      control_outer_DW.Integrator_DSTATE_b) + rtb_NProdOut_j;

    // Gain: '<S141>/ZeroGain'
    rtb_PProdOut_o = 0.0F * rtb_SignPreIntegrator;

    // DeadZone: '<S143>/DeadZone'
    if (rtb_SignPreIntegrator > 100.0F) {
      rtb_SignPreIntegrator -= 100.0F;
    } else if (rtb_SignPreIntegrator >= -100.0F) {
      rtb_SignPreIntegrator = 0.0F;
    } else {
      rtb_SignPreIntegrator -= -100.0F;
    }

    // End of DeadZone: '<S143>/DeadZone'

    // Product: '<S149>/IProd Out'
    rtb_PProdOut *= arg_ConfigurationParameters.VelLoopPID.I;

    // Signum: '<S141>/SignPreSat'
    if (rtb_SignPreIntegrator < 0.0F) {
      // DataTypeConversion: '<S141>/DataTypeConv1'
      tmp = -1.0F;
    } else if (rtb_SignPreIntegrator > 0.0F) {
      // DataTypeConversion: '<S141>/DataTypeConv1'
      tmp = 1.0F;
    } else if (rtb_SignPreIntegrator == 0.0F) {
      // DataTypeConversion: '<S141>/DataTypeConv1'
      tmp = 0.0F;
    } else {
      // DataTypeConversion: '<S141>/DataTypeConv1'
      tmp = (rtNaNF);
    }

    // End of Signum: '<S141>/SignPreSat'

    // DataTypeConversion: '<S141>/DataTypeConv1'
    if (rtIsNaNF(tmp)) {
      tmp = 0.0F;
    } else {
      tmp = std::fmod(tmp, 256.0F);
    }

    // Signum: '<S141>/SignPreIntegrator'
    if (rtb_PProdOut < 0.0F) {
      // DataTypeConversion: '<S141>/DataTypeConv2'
      tmp_0 = -1.0F;
    } else if (rtb_PProdOut > 0.0F) {
      // DataTypeConversion: '<S141>/DataTypeConv2'
      tmp_0 = 1.0F;
    } else if (rtb_PProdOut == 0.0F) {
      // DataTypeConversion: '<S141>/DataTypeConv2'
      tmp_0 = 0.0F;
    } else {
      // DataTypeConversion: '<S141>/DataTypeConv2'
      tmp_0 = (rtNaNF);
    }

    // End of Signum: '<S141>/SignPreIntegrator'

    // DataTypeConversion: '<S141>/DataTypeConv2'
    if (rtIsNaNF(tmp_0)) {
      tmp_0 = 0.0F;
    } else {
      tmp_0 = std::fmod(tmp_0, 256.0F);
    }

    // Switch: '<S141>/Switch' incorporates:
    //   Constant: '<S141>/Constant1'
    //   DataTypeConversion: '<S141>/DataTypeConv1'
    //   DataTypeConversion: '<S141>/DataTypeConv2'
    //   Logic: '<S141>/AND3'
    //   RelationalOperator: '<S141>/Equal1'
    //   RelationalOperator: '<S141>/NotEqual'

    if ((rtb_PProdOut_o != rtb_SignPreIntegrator) && ((tmp < 0.0F ? static_cast<
          int32_T>(static_cast<int8_T>(-static_cast<int8_T>(static_cast<uint8_T>
             (-tmp)))) : static_cast<int32_T>(static_cast<int8_T>
           (static_cast<uint8_T>(tmp)))) == (tmp_0 < 0.0F ? static_cast<int32_T>
          (static_cast<int8_T>(-static_cast<int8_T>(static_cast<uint8_T>(-tmp_0))))
          : static_cast<int32_T>(static_cast<int8_T>(static_cast<uint8_T>(tmp_0))))))
    {
      rtb_PProdOut = 0.0F;
    }

    // End of Switch: '<S141>/Switch'

    // DiscreteIntegrator: '<S152>/Integrator'
    if (rtb_FixPtRelationalOperator ||
        (control_outer_DW.Integrator_PrevResetState_c != 0)) {
      control_outer_DW.Integrator_DSTATE_b = 0.0F;
    }

    // DiscreteIntegrator: '<S152>/Integrator'
    rtb_PProdOut_o = 0.0005F * rtb_PProdOut +
      control_outer_DW.Integrator_DSTATE_b;

    // Sum: '<S161>/Sum'
    rtb_NProdOut_j += rtb_Saturation + rtb_PProdOut_o;

    // Saturate: '<S159>/Saturation'
    if (rtb_NProdOut_j > 100.0F) {
      // BusCreator: '<Root>/Bus Creator2'
      arg_OuterOutputs.motorcurrent.current = 100.0F;
    } else if (rtb_NProdOut_j < -100.0F) {
      // BusCreator: '<Root>/Bus Creator2'
      arg_OuterOutputs.motorcurrent.current = -100.0F;
    } else {
      // BusCreator: '<Root>/Bus Creator2'
      arg_OuterOutputs.motorcurrent.current = rtb_NProdOut_j;
    }

    // End of Saturate: '<S159>/Saturation'

    // Update for UnitDelay: '<S2>/UD'
    //
    //  Block description for '<S2>/UD':
    //
    //   Store in Global RAM

    control_outer_DW.UD_DSTATE = rtb_TSamp;

    // Update for UnitDelay: '<S1>/Delay Input1'
    //
    //  Block description for '<S1>/Delay Input1':
    //
    //   Store in Global RAM

    control_outer_DW.DelayInput1_DSTATE = arg_Flags.control_mode;

    // Update for DiscreteTransferFcn: '<S43>/Filter Differentiator TF'
    control_outer_DW.FilterDifferentiatorTF_states =
      control_outer_DW.FilterDifferentiatorTF_tmp;

    // Update for UnitDelay: '<Root>/Unit Delay' incorporates:
    //   Sum: '<Root>/Sum3'

    control_outer_DW.UnitDelay_DSTATE = rtb_Switch2 -
      arg_Targets.jointvelocities.velocity;

    // Update for DiscreteIntegrator: '<S50>/Integrator'
    control_outer_DW.Integrator_DSTATE = 0.0005F * rtb_DProdOut +
      rtb_FilterDifferentiatorTF;
    control_outer_DW.Integrator_PrevResetState = static_cast<int8_T>
      (rtb_FixPtRelationalOperator);

    // Update for DiscreteTransferFcn: '<S93>/Filter Differentiator TF'
    control_outer_DW.FilterDifferentiatorTF_states_i =
      control_outer_DW.FilterDifferentiatorTF_tmp_m;

    // Update for DiscreteIntegrator: '<S100>/Integrator'
    control_outer_DW.Integrator_DSTATE_i = 0.0005F *
      rtb_FilterDifferentiatorTF_n + rtb_DProdOut_f;
    control_outer_DW.Integrator_PrevResetState_n = static_cast<int8_T>
      (rtb_FixPtRelationalOperator);

    // Update for DiscreteTransferFcn: '<S145>/Filter Differentiator TF'
    control_outer_DW.FilterDifferentiatorTF_states_c =
      control_outer_DW.FilterDifferentiatorTF_tmp_p;

    // Update for DiscreteIntegrator: '<S152>/Integrator'
    control_outer_DW.Integrator_DSTATE_b = 0.0005F * rtb_PProdOut +
      rtb_PProdOut_o;
    control_outer_DW.Integrator_PrevResetState_c = static_cast<int8_T>
      (rtb_FixPtRelationalOperator);
  }

  // Model initialize function
  void control_outer::initialize()
  {
    // Registration code

    // initialize non-finites
    rt_InitInfAndNaN(sizeof(real_T));
    control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC = UNINITIALIZED_ZCSIG;
    control_outer_PrevZCX.FilterDifferentiatorTF_Reset__m = UNINITIALIZED_ZCSIG;
    control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e = UNINITIALIZED_ZCSIG;
  }

  // Constructor
  control_outer::control_outer() :
    control_outer_DW(),
    control_outer_PrevZCX(),
    control_outer_M()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  control_outer::~control_outer()
  {
    // Currently there is no destructor body generated.
  }

  // Real-Time Model get method
  amc_bldc_codegen::control_outer::RT_MODEL_control_outer_T * control_outer::
    getRTM()
  {
    return (&control_outer_M);
  }

  // member function to setup error status pointer
  void control_outer::setErrorStatusPointer(const char_T **rt_errorStatus)
  {
    rtmSetErrorStatusPointer((&control_outer_M), rt_errorStatus);
  }
}

//
// File trailer for generated code.
//
// [EOF]
//
