//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: FOCInnerLoop.cpp
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 2.85
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Mon Jan 10 17:04:57 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "FOCInnerLoop.h"

// Include model header file for global data
#include "control_foc.h"
#include "control_foc_private.h"

namespace amc_bldc_codegen
{
  // System initialize for atomic system: '<Root>/FOC inner loop'
  void control_foc::FOCInnerLoop_Init()
  {
    // InitializeConditions for UnitDelay: '<S3>/Delay Input1'
    //
    //  Block description for '<S3>/Delay Input1':
    //
    //   Store in Global RAM

    control_foc_DW.DelayInput1_DSTATE = ControlModes_Idle;

    // InitializeConditions for DiscreteIntegrator: '<S46>/Integrator'
    control_foc_DW.Integrator_PrevResetState = 2;

    // InitializeConditions for DiscreteIntegrator: '<S98>/Integrator'
    control_foc_DW.Integrator_PrevResetState_k = 2;
  }

  // Output and update for atomic system: '<Root>/FOC inner loop'
  void control_foc::FOCInnerLoop(const Flags *rtu_Flags, const
    ConfigurationParameters *rtu_ConfigurationParameters, const SensorsData
    *rtu_Sensors, const EstimatedData *rtu_Estimates, const Targets *rtu_Targets,
    const ControlOuterOutputs *rtu_OuterOutputs, ControlOutputs *rty_FOCOutputs)
  {
    real32_T rtb_IaIbIc0[2];
    real32_T rtb_Add;
    real32_T rtb_FilterDifferentiatorTF_o;
    real32_T rtb_SinCos_o1;
    real32_T rtb_SinCos_o2;
    real32_T rtb_Sum_g;
    real32_T rtb_Switch2;
    real32_T rtb_Unary_Minus;
    real32_T rtb_algDD_o1_p;
    real32_T rtb_algDD_o2_n;
    real32_T rtb_sum_alpha;
    real32_T rtb_sum_beta;
    boolean_T rtb_FixPtRelationalOperator;

    // Switch: '<S1>/Switch3'
    if (rtu_OuterOutputs->vel_en) {
      rtb_Sum_g = rtu_OuterOutputs->motorcurrent.current;
    } else {
      rtb_Sum_g = rtu_Targets->motorcurrent.current;
    }

    // End of Switch: '<S1>/Switch3'

    // Sum: '<S1>/Add' incorporates:
    //   Product: '<S1>/Product1'
    //   Product: '<S1>/Product2'

    rtb_Add = rtb_Sum_g * rtu_ConfigurationParameters->motorconfig.Rphase +
      rtu_Estimates->jointvelocities.velocity *
      rtu_ConfigurationParameters->motorconfig.Kbemf;

    // Gain: '<S1>/Ia+Ib+Ic=0'
    for (int32_T i = 0; i < 2; i++) {
      rtb_IaIbIc0[i] = 0.0F;
      rtb_IaIbIc0[i] += rtCP_IaIbIc0_Gain[i] * rtu_Sensors->motorsensors.Iabc[0];
      rtb_IaIbIc0[i] += rtCP_IaIbIc0_Gain[i + 2] *
        rtu_Sensors->motorsensors.Iabc[1];
      rtb_IaIbIc0[i] += rtCP_IaIbIc0_Gain[i + 4] *
        rtu_Sensors->motorsensors.Iabc[2];
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

    // Product: '<S51>/PProd Out' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S8>/a16'
    //   Gain: '<S1>/Gain'

    rtb_sum_alpha = -rtb_IaIbIc0[0] *
      rtu_ConfigurationParameters->motorconfig.Kp;

    // End of Outputs for SubSystem: '<S1>/Park Transform'

    // RelationalOperator: '<S3>/FixPt Relational Operator' incorporates:
    //   UnitDelay: '<S3>/Delay Input1'
    //
    //  Block description for '<S3>/Delay Input1':
    //
    //   Store in Global RAM

    rtb_FixPtRelationalOperator = (rtu_Flags->control_mode !=
      control_foc_DW.DelayInput1_DSTATE);

    // DiscreteTransferFcn: '<S39>/Filter Differentiator TF' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S8>/a16'
    //   Gain: '<S1>/Gain'
    //   Product: '<S38>/DProd Out'

    if (rtb_FixPtRelationalOperator &&
        (control_foc_PrevZCX.FilterDifferentiatorTF_Reset_ZC != 1)) {
      control_foc_DW.FilterDifferentiatorTF_states = 0.0F;
    }

    control_foc_PrevZCX.FilterDifferentiatorTF_Reset_ZC =
      rtb_FixPtRelationalOperator;

    // Outputs for Atomic SubSystem: '<S1>/Park Transform'
    control_foc_DW.FilterDifferentiatorTF_tmp = -rtb_IaIbIc0[0] *
      rtu_ConfigurationParameters->motorconfig.Kd - -0.999642968F *
      control_foc_DW.FilterDifferentiatorTF_states;

    // End of Outputs for SubSystem: '<S1>/Park Transform'

    // Product: '<S49>/NProd Out' incorporates:
    //   Constant: '<S1>/Constant'
    //   DiscreteTransferFcn: '<S39>/Filter Differentiator TF'
    //   Product: '<S39>/DenCoefOut'

    rtb_algDD_o2_n = (control_foc_DW.FilterDifferentiatorTF_tmp +
                      -control_foc_DW.FilterDifferentiatorTF_states) *
      0.999821484F * 10.0F;

    // Sum: '<S57>/Sum Fdbk'
    rtb_algDD_o1_p = (rtb_sum_alpha + control_foc_DW.Integrator_DSTATE) +
      rtb_algDD_o2_n;

    // Switch: '<S37>/Switch' incorporates:
    //   Gain: '<S1>/Gain2'
    //   RelationalOperator: '<S37>/u_GTE_up'
    //   RelationalOperator: '<S37>/u_GT_lo'
    //   Switch: '<S37>/Switch1'

    if (rtb_algDD_o1_p >= rtu_ConfigurationParameters->motorconfig.Vmax) {
      rtb_Unary_Minus = rtu_ConfigurationParameters->motorconfig.Vmax;
    } else if (rtb_algDD_o1_p > -rtu_ConfigurationParameters->motorconfig.Vmax)
    {
      // Switch: '<S37>/Switch1'
      rtb_Unary_Minus = rtb_algDD_o1_p;
    } else {
      rtb_Unary_Minus = -rtu_ConfigurationParameters->motorconfig.Vmax;
    }

    // End of Switch: '<S37>/Switch'

    // Sum: '<S37>/Diff'
    rtb_sum_beta = rtb_algDD_o1_p - rtb_Unary_Minus;

    // Outputs for Atomic SubSystem: '<S1>/Park Transform'
    // Product: '<S43>/IProd Out' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S8>/a16'
    //   Gain: '<S1>/Gain'

    rtb_Unary_Minus = -rtb_IaIbIc0[0] *
      rtu_ConfigurationParameters->motorconfig.Ki;

    // End of Outputs for SubSystem: '<S1>/Park Transform'

    // Signum: '<S34>/SignPreSat'
    if (rtb_sum_beta < 0.0F) {
      // DataTypeConversion: '<S34>/DataTypeConv1'
      rtb_FilterDifferentiatorTF_o = -1.0F;
    } else if (rtb_sum_beta > 0.0F) {
      // DataTypeConversion: '<S34>/DataTypeConv1'
      rtb_FilterDifferentiatorTF_o = 1.0F;
    } else if (rtb_sum_beta == 0.0F) {
      // DataTypeConversion: '<S34>/DataTypeConv1'
      rtb_FilterDifferentiatorTF_o = 0.0F;
    } else {
      // DataTypeConversion: '<S34>/DataTypeConv1'
      rtb_FilterDifferentiatorTF_o = (rtNaNF);
    }

    // End of Signum: '<S34>/SignPreSat'

    // DataTypeConversion: '<S34>/DataTypeConv1'
    if (rtIsNaNF(rtb_FilterDifferentiatorTF_o)) {
      rtb_FilterDifferentiatorTF_o = 0.0F;
    } else {
      rtb_FilterDifferentiatorTF_o = std::fmod(rtb_FilterDifferentiatorTF_o,
        256.0F);
    }

    // Signum: '<S34>/SignPreIntegrator'
    if (rtb_Unary_Minus < 0.0F) {
      // DataTypeConversion: '<S34>/DataTypeConv2'
      rtb_Switch2 = -1.0F;
    } else if (rtb_Unary_Minus > 0.0F) {
      // DataTypeConversion: '<S34>/DataTypeConv2'
      rtb_Switch2 = 1.0F;
    } else if (rtb_Unary_Minus == 0.0F) {
      // DataTypeConversion: '<S34>/DataTypeConv2'
      rtb_Switch2 = 0.0F;
    } else {
      // DataTypeConversion: '<S34>/DataTypeConv2'
      rtb_Switch2 = (rtNaNF);
    }

    // End of Signum: '<S34>/SignPreIntegrator'

    // DataTypeConversion: '<S34>/DataTypeConv2'
    if (rtIsNaNF(rtb_Switch2)) {
      rtb_Switch2 = 0.0F;
    } else {
      rtb_Switch2 = std::fmod(rtb_Switch2, 256.0F);
    }

    // Switch: '<S34>/Switch' incorporates:
    //   Constant: '<S34>/Constant1'
    //   DataTypeConversion: '<S34>/DataTypeConv1'
    //   DataTypeConversion: '<S34>/DataTypeConv2'
    //   Gain: '<S34>/ZeroGain'
    //   Logic: '<S34>/AND3'
    //   RelationalOperator: '<S34>/Equal1'
    //   RelationalOperator: '<S34>/NotEqual'

    if ((0.0F * rtb_algDD_o1_p != rtb_sum_beta) &&
        ((rtb_FilterDifferentiatorTF_o < 0.0F ? static_cast<int32_T>(
           static_cast<int8_T>(-static_cast<int8_T>(static_cast<uint8_T>
             (-rtb_FilterDifferentiatorTF_o)))) : static_cast<int32_T>(
           static_cast<int8_T>(static_cast<uint8_T>(rtb_FilterDifferentiatorTF_o))))
         == (rtb_Switch2 < 0.0F ? static_cast<int32_T>(static_cast<int8_T>(-
            static_cast<int8_T>(static_cast<uint8_T>(-rtb_Switch2)))) :
             static_cast<int32_T>(static_cast<int8_T>(static_cast<uint8_T>
            (rtb_Switch2)))))) {
      rtb_sum_beta = 0.0F;
    } else {
      rtb_sum_beta = rtb_Unary_Minus;
    }

    // End of Switch: '<S34>/Switch'

    // DiscreteIntegrator: '<S46>/Integrator'
    if (rtb_FixPtRelationalOperator && (control_foc_DW.Integrator_PrevResetState
         <= 0)) {
      control_foc_DW.Integrator_DSTATE = 0.0F;
    }

    // DiscreteIntegrator: '<S46>/Integrator'
    rtb_algDD_o1_p = 1.78571427E-5F * rtb_sum_beta +
      control_foc_DW.Integrator_DSTATE;

    // Sum: '<S56>/Sum'
    rtb_algDD_o2_n += rtb_sum_alpha + rtb_algDD_o1_p;

    // Switch: '<S54>/Switch2' incorporates:
    //   Gain: '<S1>/Gain2'
    //   RelationalOperator: '<S54>/LowerRelop1'
    //   RelationalOperator: '<S54>/UpperRelop'
    //   Switch: '<S54>/Switch'

    if (rtb_algDD_o2_n > rtu_ConfigurationParameters->motorconfig.Vmax) {
      rtb_Switch2 = rtu_ConfigurationParameters->motorconfig.Vmax;
    } else if (rtb_algDD_o2_n < -rtu_ConfigurationParameters->motorconfig.Vmax)
    {
      // Switch: '<S54>/Switch' incorporates:
      //   Gain: '<S1>/Gain2'

      rtb_Switch2 = -rtu_ConfigurationParameters->motorconfig.Vmax;
    } else {
      rtb_Switch2 = rtb_algDD_o2_n;
    }

    // End of Switch: '<S54>/Switch2'

    // Outputs for Atomic SubSystem: '<S1>/Park Transform'
    // Sum: '<S1>/Sum' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S8>/a16'

    rtb_Sum_g -= rtb_IaIbIc0[1];

    // End of Outputs for SubSystem: '<S1>/Park Transform'

    // Product: '<S103>/PProd Out'
    rtb_algDD_o2_n = rtb_Sum_g * rtu_ConfigurationParameters->motorconfig.Kp;

    // DiscreteTransferFcn: '<S91>/Filter Differentiator TF' incorporates:
    //   Product: '<S90>/DProd Out'

    if (rtb_FixPtRelationalOperator &&
        (control_foc_PrevZCX.FilterDifferentiatorTF_Reset__m != 1)) {
      control_foc_DW.FilterDifferentiatorTF_states_l = 0.0F;
    }

    control_foc_PrevZCX.FilterDifferentiatorTF_Reset__m =
      rtb_FixPtRelationalOperator;
    control_foc_DW.FilterDifferentiatorTF_tmp_m = rtb_Sum_g *
      rtu_ConfigurationParameters->motorconfig.Kd - -0.999642968F *
      control_foc_DW.FilterDifferentiatorTF_states_l;

    // Product: '<S101>/NProd Out' incorporates:
    //   Constant: '<S1>/Constant'
    //   DiscreteTransferFcn: '<S91>/Filter Differentiator TF'
    //   Product: '<S91>/DenCoefOut'

    rtb_sum_alpha = (control_foc_DW.FilterDifferentiatorTF_tmp_m +
                     -control_foc_DW.FilterDifferentiatorTF_states_l) *
      0.999821484F * 10.0F;

    // Sum: '<S110>/SumI1' incorporates:
    //   Product: '<S95>/IProd Out'
    //   Sum: '<S108>/Sum Fdbk'
    //   Sum: '<S109>/SumI3'
    //   UnitDelay: '<S1>/Unit Delay'

    rtb_Sum_g = (control_foc_DW.UnitDelay_DSTATE - ((rtb_algDD_o2_n +
      control_foc_DW.Integrator_DSTATE_m) + rtb_sum_alpha)) + rtb_Sum_g *
      rtu_ConfigurationParameters->motorconfig.Ki;

    // DiscreteIntegrator: '<S98>/Integrator'
    if (rtb_FixPtRelationalOperator &&
        (control_foc_DW.Integrator_PrevResetState_k <= 0)) {
      control_foc_DW.Integrator_DSTATE_m = 0.0F;
    }

    // DiscreteIntegrator: '<S98>/Integrator'
    rtb_FilterDifferentiatorTF_o = 1.78571427E-5F * rtb_Sum_g +
      control_foc_DW.Integrator_DSTATE_m;

    // Sum: '<S1>/Sum2' incorporates:
    //   Sum: '<S107>/Sum'

    rtb_algDD_o2_n = ((rtb_algDD_o2_n + rtb_FilterDifferentiatorTF_o) +
                      rtb_sum_alpha) + rtb_Add;

    // Switch: '<S9>/Switch2' incorporates:
    //   Gain: '<S1>/Gain2'
    //   RelationalOperator: '<S9>/LowerRelop1'
    //   RelationalOperator: '<S9>/UpperRelop'
    //   Switch: '<S9>/Switch'

    if (rtb_algDD_o2_n > rtu_ConfigurationParameters->motorconfig.Vmax) {
      rtb_algDD_o2_n = rtu_ConfigurationParameters->motorconfig.Vmax;
    } else if (rtb_algDD_o2_n < -rtu_ConfigurationParameters->motorconfig.Vmax)
    {
      // Switch: '<S9>/Switch' incorporates:
      //   Gain: '<S1>/Gain2'

      rtb_algDD_o2_n = -rtu_ConfigurationParameters->motorconfig.Vmax;
    }

    // End of Switch: '<S9>/Switch2'

    // Switch: '<S1>/Switch1'
    if (rtu_OuterOutputs->cur_en) {
      rtb_Unary_Minus = rtb_algDD_o2_n;
    } else {
      rtb_Unary_Minus = rtu_Targets->motorvoltage.voltage;
    }

    // End of Switch: '<S1>/Switch1'

    // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
    // Switch: '<S64>/Switch' incorporates:
    //   Product: '<S6>/dsin'
    //   Product: '<S6>/qcos'
    //   Sum: '<S6>/sum_beta'

    rtb_IaIbIc0[0] = rtb_Unary_Minus * rtb_SinCos_o2 + rtb_Switch2 *
      rtb_SinCos_o1;

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

      rtb_SinCos_o1 = -(rtb_Switch2 * rtb_SinCos_o2 - rtb_Unary_Minus *
                        rtb_SinCos_o1) * 0.866025388F;

      // Gain: '<S5>/one_by_two' incorporates:
      //   AlgorithmDescriptorDelegate generated from: '<S6>/a16'

      rtb_SinCos_o2 = 0.5F * rtb_IaIbIc0[0];

      // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'

      // Sum: '<S5>/add_c'
      rtb_Unary_Minus = (0.0F - rtb_SinCos_o2) - rtb_SinCos_o1;

      // Sum: '<S5>/add_b'
      rtb_SinCos_o2 = rtb_SinCos_o1 - rtb_SinCos_o2;

      // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
      // MinMax: '<S1>/Min' incorporates:
      //   AlgorithmDescriptorDelegate generated from: '<S6>/a16'

      if ((rtb_IaIbIc0[0] <= rtb_SinCos_o2) || rtIsNaNF(rtb_SinCos_o2)) {
        rtb_SinCos_o1 = rtb_IaIbIc0[0];
      } else {
        rtb_SinCos_o1 = rtb_SinCos_o2;
      }

      // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'
      if ((!(rtb_SinCos_o1 <= rtb_Unary_Minus)) && (!rtIsNaNF(rtb_Unary_Minus)))
      {
        rtb_SinCos_o1 = rtb_Unary_Minus;
      }

      // End of MinMax: '<S1>/Min'

      // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
      // Gain: '<S1>/Gain1' incorporates:
      //   AlgorithmDescriptorDelegate generated from: '<S6>/a16'
      //   Product: '<S1>/Divide'
      //   Sum: '<S1>/Sum1'

      rtb_sum_alpha = (rtb_IaIbIc0[0] - rtb_SinCos_o1) /
        rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F;

      // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'

      // Saturate: '<S1>/Saturation'
      if (rtb_sum_alpha > 100.0F) {
        rty_FOCOutputs->Vabc[0] = 100.0F;
      } else if (rtb_sum_alpha < 0.0F) {
        rty_FOCOutputs->Vabc[0] = 0.0F;
      } else {
        rty_FOCOutputs->Vabc[0] = rtb_sum_alpha;
      }

      // Gain: '<S1>/Gain1' incorporates:
      //   Product: '<S1>/Divide'
      //   Sum: '<S1>/Sum1'

      rtb_sum_alpha = (rtb_SinCos_o2 - rtb_SinCos_o1) /
        rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F;

      // Saturate: '<S1>/Saturation'
      if (rtb_sum_alpha > 100.0F) {
        rty_FOCOutputs->Vabc[1] = 100.0F;
      } else if (rtb_sum_alpha < 0.0F) {
        rty_FOCOutputs->Vabc[1] = 0.0F;
      } else {
        rty_FOCOutputs->Vabc[1] = rtb_sum_alpha;
      }

      // Gain: '<S1>/Gain1' incorporates:
      //   Product: '<S1>/Divide'
      //   Sum: '<S1>/Sum1'

      rtb_sum_alpha = (rtb_Unary_Minus - rtb_SinCos_o1) /
        rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F;

      // Saturate: '<S1>/Saturation'
      if (rtb_sum_alpha > 100.0F) {
        rty_FOCOutputs->Vabc[2] = 100.0F;
      } else if (rtb_sum_alpha < 0.0F) {
        rty_FOCOutputs->Vabc[2] = 0.0F;
      } else {
        rty_FOCOutputs->Vabc[2] = rtb_sum_alpha;
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

    // Update for UnitDelay: '<S3>/Delay Input1'
    //
    //  Block description for '<S3>/Delay Input1':
    //
    //   Store in Global RAM

    control_foc_DW.DelayInput1_DSTATE = rtu_Flags->control_mode;

    // Update for DiscreteTransferFcn: '<S39>/Filter Differentiator TF'
    control_foc_DW.FilterDifferentiatorTF_states =
      control_foc_DW.FilterDifferentiatorTF_tmp;

    // Update for DiscreteIntegrator: '<S46>/Integrator'
    control_foc_DW.Integrator_DSTATE = 1.78571427E-5F * rtb_sum_beta +
      rtb_algDD_o1_p;
    control_foc_DW.Integrator_PrevResetState = static_cast<int8_T>
      (rtb_FixPtRelationalOperator);

    // Update for DiscreteTransferFcn: '<S91>/Filter Differentiator TF'
    control_foc_DW.FilterDifferentiatorTF_states_l =
      control_foc_DW.FilterDifferentiatorTF_tmp_m;

    // Update for UnitDelay: '<S1>/Unit Delay' incorporates:
    //   Sum: '<S1>/Sum3'

    control_foc_DW.UnitDelay_DSTATE = rtb_algDD_o2_n - rtb_Add;

    // Update for DiscreteIntegrator: '<S98>/Integrator'
    control_foc_DW.Integrator_DSTATE_m = 1.78571427E-5F * rtb_Sum_g +
      rtb_FilterDifferentiatorTF_o;
    control_foc_DW.Integrator_PrevResetState_k = static_cast<int8_T>
      (rtb_FixPtRelationalOperator);
  }
}

//
// File trailer for generated code.
//
// [EOF]
//
