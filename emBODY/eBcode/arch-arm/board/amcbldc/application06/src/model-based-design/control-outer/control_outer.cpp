//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_outer.cpp
//
// Code generated for Simulink model 'control_outer'.
//
// Model version                  : 2.35
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Mon Jan 31 18:32:12 2022
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

    // InitializeConditions for DiscreteIntegrator: '<S49>/Integrator'
    control_outer_DW.Integrator_PrevResetState = 2;

    // InitializeConditions for DiscreteIntegrator: '<S99>/Integrator'
    control_outer_DW.Integrator_PrevResetState_n = 2;

    // InitializeConditions for DiscreteIntegrator: '<S151>/Integrator'
    control_outer_DW.Integrator_PrevResetState_c = 2;
  }

  // Output and update for referenced model: 'control_outer'
  void control_outer::step(const Flags &arg_Flags, const ConfigurationParameters
    &arg_ConfigurationParameters, const Targets &arg_Targets, const SensorsData
    &arg_Sensors, const EstimatedData &arg_Estimates, ControlOuterOutputs &
    arg_OuterOutputs)
  {
    real32_T rtb_Abs;
    real32_T rtb_DProdOut;
    real32_T rtb_DProdOut_f;
    real32_T rtb_FilterDifferentiatorTF;
    real32_T rtb_FilterDifferentiatorTF_n;
    real32_T rtb_IProdOut;
    real32_T rtb_PProdOut;
    real32_T rtb_PProdOut_o;
    real32_T rtb_Saturation;
    real32_T rtb_SignPreIntegrator;
    real32_T rtb_Switch2;
    real32_T tmp;
    boolean_T rtb_Compare;
    boolean_T rtb_FixPtRelationalOperator;

    // Abs: '<Root>/Abs'
    rtb_Abs = std::abs(arg_ConfigurationParameters.thresholds.jntVelMax);

    // RelationalOperator: '<S8>/Compare' incorporates:
    //   Constant: '<S8>/Constant'

    arg_OuterOutputs.vel_en = (arg_Flags.control_mode != ControlModes_Current);

    // RelationalOperator: '<S6>/Compare' incorporates:
    //   Constant: '<S6>/Constant'

    arg_OuterOutputs.cur_en = (arg_Flags.control_mode != ControlModes_Voltage);

    // Logic: '<S2>/NOR' incorporates:
    //   Constant: '<S11>/Constant'
    //   Constant: '<S12>/Constant'
    //   Constant: '<S9>/Constant'
    //   RelationalOperator: '<S11>/Compare'
    //   RelationalOperator: '<S12>/Compare'
    //   RelationalOperator: '<S9>/Compare'

    arg_OuterOutputs.out_en = ((!(arg_Flags.control_mode ==
      ControlModes_NotConfigured)) && (!(arg_Flags.control_mode ==
      ControlModes_Idle)) && (!(arg_Flags.control_mode == ControlModes_HwFaultCM)));

    // Sum: '<S3>/Sum3'
    rtb_SignPreIntegrator = arg_Targets.jointpositions.position -
      arg_Sensors.jointpositions.position;

    // Product: '<S54>/PProd Out'
    rtb_PProdOut = rtb_SignPreIntegrator *
      arg_ConfigurationParameters.PosLoopPID.P;

    // SampleTimeMath: '<S44>/Tsamp'
    //
    //  About '<S44>/Tsamp':
    //   y = u * K where K = ( w * Ts )

    rtb_IProdOut = arg_ConfigurationParameters.PosLoopPID.N * 0.0005F;

    // Math: '<S42>/Reciprocal' incorporates:
    //   Constant: '<S42>/Constant'
    //   Sum: '<S42>/SumDen'
    //
    //  About '<S42>/Reciprocal':
    //   Operator: reciprocal

    rtb_Switch2 = 1.0F / (rtb_IProdOut + 1.0F);

    // RelationalOperator: '<S1>/FixPt Relational Operator' incorporates:
    //   UnitDelay: '<S1>/Delay Input1'
    //
    //  Block description for '<S1>/Delay Input1':
    //
    //   Store in Global RAM

    rtb_FixPtRelationalOperator = (arg_Flags.control_mode !=
      control_outer_DW.DelayInput1_DSTATE);

    // DiscreteTransferFcn: '<S42>/Filter Differentiator TF' incorporates:
    //   Constant: '<S42>/Constant'
    //   Product: '<S41>/DProd Out'
    //   Product: '<S42>/Divide'
    //   Sum: '<S42>/SumNum'

    if (rtb_FixPtRelationalOperator &&
        (control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC != 1)) {
      control_outer_DW.FilterDifferentiatorTF_states = 0.0F;
    }

    control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC =
      rtb_FixPtRelationalOperator;
    control_outer_DW.FilterDifferentiatorTF_tmp = rtb_SignPreIntegrator *
      arg_ConfigurationParameters.PosLoopPID.D - (rtb_IProdOut - 1.0F) *
      rtb_Switch2 * control_outer_DW.FilterDifferentiatorTF_states;

    // Product: '<S52>/NProd Out' incorporates:
    //   DiscreteTransferFcn: '<S42>/Filter Differentiator TF'
    //   Product: '<S42>/DenCoefOut'

    rtb_Switch2 = (control_outer_DW.FilterDifferentiatorTF_tmp +
                   -control_outer_DW.FilterDifferentiatorTF_states) *
      rtb_Switch2 * arg_ConfigurationParameters.PosLoopPID.N;

    // Sum: '<S61>/SumI1' incorporates:
    //   Product: '<S46>/IProd Out'
    //   Sum: '<S59>/Sum Fdbk'
    //   Sum: '<S60>/SumI3'
    //   UnitDelay: '<Root>/Unit Delay'

    rtb_DProdOut = (control_outer_DW.UnitDelay_DSTATE - ((rtb_PProdOut +
      control_outer_DW.Integrator_DSTATE) + rtb_Switch2)) +
      rtb_SignPreIntegrator * arg_ConfigurationParameters.PosLoopPID.I;

    // DiscreteIntegrator: '<S49>/Integrator'
    if (rtb_FixPtRelationalOperator &&
        (control_outer_DW.Integrator_PrevResetState <= 0)) {
      control_outer_DW.Integrator_DSTATE = 0.0F;
    }

    // DiscreteIntegrator: '<S49>/Integrator'
    rtb_FilterDifferentiatorTF = 0.0005F * rtb_DProdOut +
      control_outer_DW.Integrator_DSTATE;

    // RelationalOperator: '<S7>/Compare' incorporates:
    //   Constant: '<S7>/Constant'

    rtb_Compare = (arg_Flags.control_mode == ControlModes_Position);

    // Product: '<S104>/PProd Out'
    rtb_PProdOut_o = rtb_SignPreIntegrator *
      arg_ConfigurationParameters.DirLoopPID.P;

    // Product: '<S96>/IProd Out'
    rtb_IProdOut = rtb_SignPreIntegrator *
      arg_ConfigurationParameters.DirLoopPID.I;

    // Product: '<S91>/DProd Out'
    rtb_DProdOut_f = rtb_SignPreIntegrator *
      arg_ConfigurationParameters.DirLoopPID.D;

    // SampleTimeMath: '<S94>/Tsamp'
    //
    //  About '<S94>/Tsamp':
    //   y = u * K where K = ( w * Ts )

    rtb_Saturation = arg_ConfigurationParameters.DirLoopPID.N * 0.0005F;

    // Math: '<S92>/Reciprocal' incorporates:
    //   Constant: '<S92>/Constant'
    //   Sum: '<S92>/SumDen'
    //
    //  About '<S92>/Reciprocal':
    //   Operator: reciprocal

    rtb_SignPreIntegrator = 1.0F / (rtb_Saturation + 1.0F);

    // DiscreteTransferFcn: '<S92>/Filter Differentiator TF' incorporates:
    //   Constant: '<S92>/Constant'
    //   Product: '<S92>/Divide'
    //   Sum: '<S92>/SumNum'

    if (rtb_FixPtRelationalOperator &&
        (control_outer_PrevZCX.FilterDifferentiatorTF_Reset__m != 1)) {
      control_outer_DW.FilterDifferentiatorTF_states_i = 0.0F;
    }

    control_outer_PrevZCX.FilterDifferentiatorTF_Reset__m =
      rtb_FixPtRelationalOperator;
    control_outer_DW.FilterDifferentiatorTF_tmp_m = rtb_DProdOut_f -
      (rtb_Saturation - 1.0F) * rtb_SignPreIntegrator *
      control_outer_DW.FilterDifferentiatorTF_states_i;

    // Product: '<S102>/NProd Out' incorporates:
    //   DiscreteTransferFcn: '<S92>/Filter Differentiator TF'
    //   Product: '<S92>/DenCoefOut'

    rtb_SignPreIntegrator = (control_outer_DW.FilterDifferentiatorTF_tmp_m +
      -control_outer_DW.FilterDifferentiatorTF_states_i) * rtb_SignPreIntegrator
      * arg_ConfigurationParameters.DirLoopPID.N;

    // Sum: '<S111>/SumI1' incorporates:
    //   Sum: '<S109>/Sum Fdbk'
    //   Sum: '<S110>/SumI3'
    //   UnitDelay: '<Root>/Unit Delay'

    rtb_FilterDifferentiatorTF_n = (control_outer_DW.UnitDelay_DSTATE -
      ((rtb_PProdOut_o + control_outer_DW.Integrator_DSTATE_i) +
       rtb_SignPreIntegrator)) + rtb_IProdOut;

    // DiscreteIntegrator: '<S99>/Integrator'
    if (rtb_FixPtRelationalOperator &&
        (control_outer_DW.Integrator_PrevResetState_n <= 0)) {
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

    if (rtb_Compare || (arg_Flags.control_mode == ControlModes_PositionDirect) ||
        (arg_Flags.control_mode == ControlModes_Velocity)) {
      // Switch: '<S3>/Switch5' incorporates:
      //   Sum: '<S108>/Sum'
      //   Sum: '<S58>/Sum'

      if (rtb_Compare) {
        rtb_IProdOut = (rtb_PProdOut + rtb_FilterDifferentiatorTF) + rtb_Switch2;
      } else {
        rtb_IProdOut = (rtb_PProdOut_o + rtb_DProdOut_f) + rtb_SignPreIntegrator;
      }

      // End of Switch: '<S3>/Switch5'
    } else {
      rtb_IProdOut = 0.0F;
    }

    // End of Switch: '<Root>/Switch3'

    // Sum: '<Root>/Sum2'
    rtb_IProdOut += arg_Targets.jointvelocities.velocity;

    // Switch: '<S4>/Switch2' incorporates:
    //   Gain: '<Root>/Gain'
    //   RelationalOperator: '<S4>/LowerRelop1'
    //   RelationalOperator: '<S4>/UpperRelop'
    //   Switch: '<S4>/Switch'

    if (rtb_IProdOut > rtb_Abs) {
      rtb_Switch2 = rtb_Abs;
    } else if (rtb_IProdOut < -rtb_Abs) {
      // Switch: '<S4>/Switch' incorporates:
      //   Gain: '<Root>/Gain'

      rtb_Switch2 = -rtb_Abs;
    } else {
      rtb_Switch2 = rtb_IProdOut;
    }

    // End of Switch: '<S4>/Switch2'

    // Product: '<Root>/Product2' incorporates:
    //   Sum: '<Root>/Sum1'

    rtb_IProdOut = (rtb_Switch2 - arg_Estimates.jointvelocities.velocity) *
      arg_ConfigurationParameters.motorconfig.reduction;

    // Product: '<S156>/PProd Out'
    rtb_Saturation = rtb_IProdOut * arg_ConfigurationParameters.VelLoopPID.P;

    // SampleTimeMath: '<S146>/Tsamp'
    //
    //  About '<S146>/Tsamp':
    //   y = u * K where K = ( w * Ts )

    rtb_SignPreIntegrator = arg_ConfigurationParameters.VelLoopPID.N * 0.0005F;

    // Math: '<S144>/Reciprocal' incorporates:
    //   Constant: '<S144>/Constant'
    //   Sum: '<S144>/SumDen'
    //
    //  About '<S144>/Reciprocal':
    //   Operator: reciprocal

    rtb_Abs = 1.0F / (rtb_SignPreIntegrator + 1.0F);

    // DiscreteTransferFcn: '<S144>/Filter Differentiator TF' incorporates:
    //   Constant: '<S144>/Constant'
    //   Product: '<S143>/DProd Out'
    //   Product: '<S144>/Divide'
    //   Sum: '<S144>/SumNum'

    if (rtb_FixPtRelationalOperator &&
        (control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e != 1)) {
      control_outer_DW.FilterDifferentiatorTF_states_c = 0.0F;
    }

    control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e =
      rtb_FixPtRelationalOperator;
    control_outer_DW.FilterDifferentiatorTF_tmp_p = rtb_IProdOut *
      arg_ConfigurationParameters.VelLoopPID.D - (rtb_SignPreIntegrator - 1.0F) *
      rtb_Abs * control_outer_DW.FilterDifferentiatorTF_states_c;

    // Product: '<S154>/NProd Out' incorporates:
    //   DiscreteTransferFcn: '<S144>/Filter Differentiator TF'
    //   Product: '<S144>/DenCoefOut'

    rtb_Abs = (control_outer_DW.FilterDifferentiatorTF_tmp_p +
               -control_outer_DW.FilterDifferentiatorTF_states_c) * rtb_Abs *
      arg_ConfigurationParameters.VelLoopPID.N;

    // Sum: '<S161>/Sum Fdbk'
    rtb_SignPreIntegrator = (rtb_Saturation +
      control_outer_DW.Integrator_DSTATE_b) + rtb_Abs;

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
    rtb_IProdOut *= arg_ConfigurationParameters.VelLoopPID.I;

    // Signum: '<S140>/SignPreSat'
    if (rtb_SignPreIntegrator < 0.0F) {
      // DataTypeConversion: '<S140>/DataTypeConv1'
      rtb_PProdOut_o = -1.0F;
    } else if (rtb_SignPreIntegrator > 0.0F) {
      // DataTypeConversion: '<S140>/DataTypeConv1'
      rtb_PProdOut_o = 1.0F;
    } else if (rtb_SignPreIntegrator == 0.0F) {
      // DataTypeConversion: '<S140>/DataTypeConv1'
      rtb_PProdOut_o = 0.0F;
    } else {
      // DataTypeConversion: '<S140>/DataTypeConv1'
      rtb_PProdOut_o = (rtNaNF);
    }

    // End of Signum: '<S140>/SignPreSat'

    // DataTypeConversion: '<S140>/DataTypeConv1'
    if (rtIsNaNF(rtb_PProdOut_o)) {
      rtb_PProdOut_o = 0.0F;
    } else {
      rtb_PProdOut_o = std::fmod(rtb_PProdOut_o, 256.0F);
    }

    // Signum: '<S140>/SignPreIntegrator'
    if (rtb_IProdOut < 0.0F) {
      // DataTypeConversion: '<S140>/DataTypeConv2'
      tmp = -1.0F;
    } else if (rtb_IProdOut > 0.0F) {
      // DataTypeConversion: '<S140>/DataTypeConv2'
      tmp = 1.0F;
    } else if (rtb_IProdOut == 0.0F) {
      // DataTypeConversion: '<S140>/DataTypeConv2'
      tmp = 0.0F;
    } else {
      // DataTypeConversion: '<S140>/DataTypeConv2'
      tmp = (rtNaNF);
    }

    // End of Signum: '<S140>/SignPreIntegrator'

    // DataTypeConversion: '<S140>/DataTypeConv2'
    if (rtIsNaNF(tmp)) {
      tmp = 0.0F;
    } else {
      tmp = std::fmod(tmp, 256.0F);
    }

    // Switch: '<S140>/Switch' incorporates:
    //   Constant: '<S140>/Constant1'
    //   DataTypeConversion: '<S140>/DataTypeConv1'
    //   DataTypeConversion: '<S140>/DataTypeConv2'
    //   Logic: '<S140>/AND3'
    //   RelationalOperator: '<S140>/Equal1'
    //   RelationalOperator: '<S140>/NotEqual'

    if ((rtb_PProdOut != rtb_SignPreIntegrator) && ((rtb_PProdOut_o < 0.0F ?
          static_cast<int32_T>(static_cast<int8_T>(-static_cast<int8_T>(
             static_cast<uint8_T>(-rtb_PProdOut_o)))) : static_cast<int32_T>(
           static_cast<int8_T>(static_cast<uint8_T>(rtb_PProdOut_o)))) == (tmp <
          0.0F ? static_cast<int32_T>(static_cast<int8_T>(-static_cast<int8_T>(
             static_cast<uint8_T>(-tmp)))) : static_cast<int32_T>
          (static_cast<int8_T>(static_cast<uint8_T>(tmp)))))) {
      rtb_PProdOut = 0.0F;
    } else {
      rtb_PProdOut = rtb_IProdOut;
    }

    // End of Switch: '<S140>/Switch'

    // DiscreteIntegrator: '<S151>/Integrator'
    if (rtb_FixPtRelationalOperator &&
        (control_outer_DW.Integrator_PrevResetState_c <= 0)) {
      control_outer_DW.Integrator_DSTATE_b = 0.0F;
    }

    // DiscreteIntegrator: '<S151>/Integrator'
    rtb_IProdOut = 0.0005F * rtb_PProdOut + control_outer_DW.Integrator_DSTATE_b;

    // Sum: '<S160>/Sum'
    rtb_Abs += rtb_Saturation + rtb_IProdOut;

    // Saturate: '<S158>/Saturation'
    if (rtb_Abs > 100.0F) {
      // BusCreator: '<Root>/Bus Creator2'
      arg_OuterOutputs.motorcurrent.current = 100.0F;
    } else if (rtb_Abs < -100.0F) {
      // BusCreator: '<Root>/Bus Creator2'
      arg_OuterOutputs.motorcurrent.current = -100.0F;
    } else {
      // BusCreator: '<Root>/Bus Creator2'
      arg_OuterOutputs.motorcurrent.current = rtb_Abs;
    }

    // End of Saturate: '<S158>/Saturation'

    // Update for UnitDelay: '<S1>/Delay Input1'
    //
    //  Block description for '<S1>/Delay Input1':
    //
    //   Store in Global RAM

    control_outer_DW.DelayInput1_DSTATE = arg_Flags.control_mode;

    // Update for DiscreteTransferFcn: '<S42>/Filter Differentiator TF'
    control_outer_DW.FilterDifferentiatorTF_states =
      control_outer_DW.FilterDifferentiatorTF_tmp;

    // Update for UnitDelay: '<Root>/Unit Delay' incorporates:
    //   Sum: '<Root>/Sum3'

    control_outer_DW.UnitDelay_DSTATE = rtb_Switch2 -
      arg_Targets.jointvelocities.velocity;

    // Update for DiscreteIntegrator: '<S49>/Integrator'
    control_outer_DW.Integrator_DSTATE = 0.0005F * rtb_DProdOut +
      rtb_FilterDifferentiatorTF;
    control_outer_DW.Integrator_PrevResetState = static_cast<int8_T>
      (rtb_FixPtRelationalOperator);

    // Update for DiscreteTransferFcn: '<S92>/Filter Differentiator TF'
    control_outer_DW.FilterDifferentiatorTF_states_i =
      control_outer_DW.FilterDifferentiatorTF_tmp_m;

    // Update for DiscreteIntegrator: '<S99>/Integrator'
    control_outer_DW.Integrator_DSTATE_i = 0.0005F *
      rtb_FilterDifferentiatorTF_n + rtb_DProdOut_f;
    control_outer_DW.Integrator_PrevResetState_n = static_cast<int8_T>
      (rtb_FixPtRelationalOperator);

    // Update for DiscreteTransferFcn: '<S144>/Filter Differentiator TF'
    control_outer_DW.FilterDifferentiatorTF_states_c =
      control_outer_DW.FilterDifferentiatorTF_tmp_p;

    // Update for DiscreteIntegrator: '<S151>/Integrator'
    control_outer_DW.Integrator_DSTATE_b = 0.0005F * rtb_PProdOut + rtb_IProdOut;
    control_outer_DW.Integrator_PrevResetState_c = static_cast<int8_T>
      (rtb_FixPtRelationalOperator);
  }

  // Model initialize function
  void control_outer::initialize()
  {
    // Registration code

    // initialize non-finites
    rt_InitInfAndNaN(sizeof(real_T));
    control_outer_PrevZCX.FilterDifferentiatorTF_Reset_ZC = POS_ZCSIG;
    control_outer_PrevZCX.FilterDifferentiatorTF_Reset__m = POS_ZCSIG;
    control_outer_PrevZCX.FilterDifferentiatorTF_Reset__e = POS_ZCSIG;
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
