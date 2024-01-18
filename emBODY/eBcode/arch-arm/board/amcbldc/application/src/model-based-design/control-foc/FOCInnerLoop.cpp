//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: FOCInnerLoop.cpp
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 6.16
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Mon Jan 15 14:41:30 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "FOCInnerLoop.h"
#include "control_foc_types.h"

extern "C"
{

#include "rt_nonfinite.h"

}

#include "arm_math.h"
#include <cmath>
#include "mw_cmsis.h"
#include "rtwtypes.h"
#include "control_foc_private.h"
#include "zero_crossing_types.h"

// System initialize for atomic system: '<Root>/FOC inner loop'
void FOCInnerLoop_Init(DW_FOCInnerLoop_T *localDW)
{
  // InitializeConditions for DiscreteTransferFcn: '<S100>/Filter Differentiator TF' 
  localDW->FilterDifferentiatorTF_states = 0.0F;

  // InitializeConditions for DiscreteIntegrator: '<S107>/Integrator'
  localDW->Integrator_DSTATE = 0.0F;
  localDW->Integrator_PrevResetState = 2;

  // InitializeConditions for DiscreteTransferFcn: '<S46>/Filter Differentiator TF' 
  localDW->FilterDifferentiatorTF_states_k = 0.0F;

  // InitializeConditions for DiscreteIntegrator: '<S53>/Integrator'
  localDW->Integrator_DSTATE_o = 0.0F;
  localDW->Integrator_PrevResetState_k = 2;
}

// Outputs for atomic system: '<Root>/FOC inner loop'
void FOCInnerLoop(const ConfigurationParameters *rtu_ConfigurationParameters,
                  const SensorsData *rtu_Sensors, const EstimatedData
                  *rtu_Estimates, const Targets *rtu_Targets, const
                  ControlOuterOutputs *rtu_OuterOutputs, ControlOutputs
                  *rty_FOCOutputs, B_FOCInnerLoop_T *localB, const
                  ConstB_FOCInnerLoop_T *localC, DW_FOCInnerLoop_T *localDW,
                  ZCE_FOCInnerLoop_T *localZCE)
{
  real32_T rtb_IaIbIc0[2];
  real32_T DProdOut;
  real32_T q;
  real32_T rtb_Add;
  real32_T rtb_Cos;
  real32_T rtb_FilterDifferentiatorTF_f;
  real32_T rtb_Gain1;
  real32_T rtb_NProdOut;
  real32_T rtb_Product;
  real32_T rtb_algDD_o1_g;
  real32_T rtb_algDD_o2_l;
  real32_T rtb_y;
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
  if ((rtu_ConfigurationParameters->motorconfig.Vmax <=
       rtu_Sensors->supplyvoltagesensors.voltage) || rtIsNaNF
      (rtu_Sensors->supplyvoltagesensors.voltage)) {
    rtb_algDD_o1_g = rtu_ConfigurationParameters->motorconfig.Vmax;
  } else {
    rtb_algDD_o1_g = rtu_Sensors->supplyvoltagesensors.voltage;
  }

  // Product: '<S1>/Product' incorporates:
  //   Gain: '<S1>/Gain4'
  //   MinMax: '<S1>/Min'

  rtb_Product = 0.5F * rtb_algDD_o1_g * localC->Sum5;

  // Gain: '<S1>/Ia+Ib+Ic=0'
  q = rtu_Sensors->motorsensors.Iabc[1];
  rtb_Cos = rtu_Sensors->motorsensors.Iabc[0];
  rtb_algDD_o2_l = rtu_Sensors->motorsensors.Iabc[2];
  for (int32_T i = 0; i < 2; i++) {
    rtb_IaIbIc0[i] = (rtCP_IaIbIc0_Gain[i + 2] * q + rtCP_IaIbIc0_Gain[i] *
                      rtb_Cos) + rtCP_IaIbIc0_Gain[i + 4] * rtb_algDD_o2_l;
  }

  // End of Gain: '<S1>/Ia+Ib+Ic=0'

  // Outputs for Atomic SubSystem: '<S12>/Two phase CRL wrap'
  // AlgorithmDescriptorDelegate generated from: '<S13>/a16'
  arm_clarke_f32(rtb_IaIbIc0[0], rtb_IaIbIc0[1], &rtb_algDD_o1_g,
                 &rtb_algDD_o2_l);

  // End of Outputs for SubSystem: '<S12>/Two phase CRL wrap'

  // Gain: '<S5>/Gain1'
  rtb_Gain1 = 0.0174532924F * rtu_Sensors->motorsensors.angle;

  // MATLAB Function: '<S15>/Wrap Angle'
  if (rtIsNaNF(rtb_Gain1)) {
    rtb_y = (rtNaNF);
  } else if (rtIsInfF(rtb_Gain1)) {
    rtb_y = (rtNaNF);
  } else if (rtb_Gain1 == 0.0F) {
    rtb_y = 0.0F;
  } else {
    boolean_T rEQ0;
    rtb_y = std::fmod(rtb_Gain1, 6.28318548F);
    rEQ0 = (rtb_y == 0.0F);
    if (!rEQ0) {
      q = std::abs(rtb_Gain1 / 6.28318548F);
      rEQ0 = !(std::abs(q - std::floor(q + 0.5F)) > 1.1920929E-7F * q);
    }

    if (rEQ0) {
      rtb_y = 0.0F;
    } else if (rtb_Gain1 < 0.0F) {
      rtb_y += 6.28318548F;
    }
  }

  if ((rtb_y == 0.0F) && (rtb_Gain1 > 0.0F)) {
    rtb_y = 6.28318548F;
  }

  // Trigonometry: '<S15>/Sin' incorporates:
  //   MATLAB Function: '<S15>/Wrap Angle'

  q = arm_sin_f32(rtb_y);

  // Trigonometry: '<S15>/Cos' incorporates:
  //   MATLAB Function: '<S15>/Wrap Angle'

  rtb_Cos = arm_cos_f32(rtb_y);

  // Outputs for Atomic SubSystem: '<S10>/Two inputs CRL'
  // AlgorithmDescriptorDelegate generated from: '<S124>/a16'
  arm_park_f32(rtb_algDD_o1_g, rtb_algDD_o2_l, &rtb_Gain1, &rtb_y, q, rtb_Cos);

  // End of Outputs for SubSystem: '<S10>/Two inputs CRL'

  // Sum: '<S1>/Sum'
  rtb_algDD_o2_l = rtu_OuterOutputs->motorcurrent.current - rtb_y;

  // Product: '<S112>/PProd Out'
  rtb_algDD_o1_g = rtb_algDD_o2_l * rtu_ConfigurationParameters->CurLoopPID.P;

  // SampleTimeMath: '<S102>/Tsamp' incorporates:
  //   SampleTimeMath: '<S48>/Tsamp'
  //
  //  About '<S102>/Tsamp':
  //   y = u * K where K = ( w * Ts )
  //
  //  About '<S48>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_NProdOut = rtu_ConfigurationParameters->CurLoopPID.N * 1.82857148E-5F;

  // Math: '<S100>/Reciprocal' incorporates:
  //   Constant: '<S100>/Filter Den Constant'
  //   Math: '<S46>/Reciprocal'
  //   SampleTimeMath: '<S102>/Tsamp'
  //   Sum: '<S100>/SumDen'
  //
  //  About '<S100>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S46>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S102>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  DProdOut = 1.0F / (rtb_NProdOut + 1.0F);

  // DiscreteTransferFcn: '<S100>/Filter Differentiator TF'
  if (rtu_OuterOutputs->pid_reset && (localZCE->FilterDifferentiatorTF_Reset_ZCE
       != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_states = 0.0F;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE = rtu_OuterOutputs->pid_reset;

  // Product: '<S100>/Divide' incorporates:
  //   Constant: '<S100>/Filter Den Constant'
  //   Math: '<S100>/Reciprocal'
  //   Product: '<S46>/Divide'
  //   SampleTimeMath: '<S102>/Tsamp'
  //   Sum: '<S100>/SumNum'
  //
  //  About '<S100>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S102>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_FilterDifferentiatorTF_f = (rtb_NProdOut - 1.0F) * DProdOut;

  // DiscreteTransferFcn: '<S100>/Filter Differentiator TF' incorporates:
  //   Product: '<S100>/Divide'
  //   Product: '<S99>/DProd Out'

  localDW->FilterDifferentiatorTF_tmp = rtb_algDD_o2_l *
    rtu_ConfigurationParameters->CurLoopPID.D - rtb_FilterDifferentiatorTF_f *
    localDW->FilterDifferentiatorTF_states;

  // Product: '<S110>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S100>/Filter Differentiator TF'
  //   Math: '<S100>/Reciprocal'
  //   Product: '<S100>/DenCoefOut'
  //
  //  About '<S100>/Reciprocal':
  //   Operator: reciprocal

  rtb_NProdOut = (localDW->FilterDifferentiatorTF_tmp -
                  localDW->FilterDifferentiatorTF_states) * DProdOut *
    rtu_ConfigurationParameters->CurLoopPID.N;

  // Sum: '<S119>/SumI1' incorporates:
  //   Product: '<S104>/IProd Out'
  //   Sum: '<S117>/Sum Fdbk'
  //   Sum: '<S118>/SumI3'
  //   UnitDelay: '<S1>/Unit Delay'

  localB->SumI1 = (localDW->UnitDelay_DSTATE - ((rtb_algDD_o1_g +
    localDW->Integrator_DSTATE) + rtb_NProdOut)) + rtb_algDD_o2_l *
    rtu_ConfigurationParameters->CurLoopPID.I;

  // DiscreteIntegrator: '<S107>/Integrator'
  if (rtu_OuterOutputs->pid_reset && (localDW->Integrator_PrevResetState <= 0))
  {
    localDW->Integrator_DSTATE = 0.0F;
  }

  // DiscreteIntegrator: '<S107>/Integrator'
  localB->Integrator = 1.82857148E-5F * localB->SumI1 +
    localDW->Integrator_DSTATE;

  // Switch: '<S1>/Switch1' incorporates:
  //   Gain: '<S1>/Gain6'
  //   Product: '<S1>/Divide2'
  //   Sum: '<S116>/Sum'
  //   Sum: '<S1>/Sum2'
  //   Sum: '<S1>/Sum6'

  if (rtu_OuterOutputs->cur_en) {
    rtb_NProdOut = ((rtb_algDD_o1_g + localB->Integrator) + rtb_NProdOut) +
      rtb_Add;
  } else {
    rtb_NProdOut = rtu_Targets->motorvoltage.voltage * rtb_Product * 0.01F +
      rtu_OuterOutputs->current_limiter;
  }

  // End of Switch: '<S1>/Switch1'

  // Switch: '<S11>/Switch2' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S11>/LowerRelop1'
  //   RelationalOperator: '<S11>/UpperRelop'
  //   Switch: '<S11>/Switch'

  if (rtb_NProdOut > rtb_Product) {
    rtb_NProdOut = rtb_Product;
  } else if (rtb_NProdOut < -rtb_Product) {
    // Switch: '<S11>/Switch' incorporates:
    //   Gain: '<S1>/Gain2'

    rtb_NProdOut = -rtb_Product;
  }

  // End of Switch: '<S11>/Switch2'

  // Product: '<S58>/PProd Out' incorporates:
  //   Gain: '<S1>/Gain'

  rtb_algDD_o2_l = -rtb_Gain1 * rtu_ConfigurationParameters->CurLoopPID.P;

  // DiscreteTransferFcn: '<S46>/Filter Differentiator TF' incorporates:
  //   DiscreteTransferFcn: '<S100>/Filter Differentiator TF'
  //   Gain: '<S1>/Gain'
  //   Product: '<S45>/DProd Out'

  if (rtu_OuterOutputs->pid_reset &&
      (localZCE->FilterDifferentiatorTF_Reset_ZCE_o != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_states_k = 0.0F;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE_o = rtu_OuterOutputs->pid_reset;
  localDW->FilterDifferentiatorTF_tmp_c = -rtb_Gain1 *
    rtu_ConfigurationParameters->CurLoopPID.D - rtb_FilterDifferentiatorTF_f *
    localDW->FilterDifferentiatorTF_states_k;

  // Product: '<S56>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S46>/Filter Differentiator TF'
  //   Product: '<S46>/DenCoefOut'

  DProdOut = (localDW->FilterDifferentiatorTF_tmp_c -
              localDW->FilterDifferentiatorTF_states_k) * DProdOut *
    rtu_ConfigurationParameters->CurLoopPID.N;

  // Sum: '<S64>/Sum Fdbk'
  rtb_FilterDifferentiatorTF_f = (rtb_algDD_o2_l + localDW->Integrator_DSTATE_o)
    + DProdOut;

  // Switch: '<S44>/Switch' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S44>/u_GTE_up'
  //   RelationalOperator: '<S44>/u_GT_lo'
  //   Switch: '<S44>/Switch1'

  if (rtb_FilterDifferentiatorTF_f >= rtb_Product) {
    rtb_algDD_o1_g = rtb_Product;
  } else if (rtb_FilterDifferentiatorTF_f > -rtb_Product) {
    // Switch: '<S44>/Switch1'
    rtb_algDD_o1_g = rtb_FilterDifferentiatorTF_f;
  } else {
    rtb_algDD_o1_g = -rtb_Product;
  }

  // Sum: '<S44>/Diff' incorporates:
  //   Switch: '<S44>/Switch'

  rtb_algDD_o1_g = rtb_FilterDifferentiatorTF_f - rtb_algDD_o1_g;

  // Product: '<S50>/IProd Out' incorporates:
  //   Gain: '<S1>/Gain'

  rtb_FilterDifferentiatorTF_f = -rtb_Gain1 *
    rtu_ConfigurationParameters->CurLoopPID.I;

  // Switch: '<S41>/Switch1' incorporates:
  //   Constant: '<S41>/Clamping_zero'
  //   Constant: '<S41>/Constant'
  //   Constant: '<S41>/Constant2'
  //   RelationalOperator: '<S41>/fix for DT propagation issue'

  if (rtb_algDD_o1_g > 0.0F) {
    tmp = 1;
  } else {
    tmp = -1;
  }

  // Switch: '<S41>/Switch2' incorporates:
  //   Constant: '<S41>/Clamping_zero'
  //   Constant: '<S41>/Constant3'
  //   Constant: '<S41>/Constant4'
  //   RelationalOperator: '<S41>/fix for DT propagation issue1'

  if (rtb_FilterDifferentiatorTF_f > 0.0F) {
    tmp_0 = 1;
  } else {
    tmp_0 = -1;
  }

  // Switch: '<S41>/Switch' incorporates:
  //   Constant: '<S41>/Clamping_zero'
  //   Logic: '<S41>/AND3'
  //   RelationalOperator: '<S41>/Equal1'
  //   RelationalOperator: '<S41>/Relational Operator'
  //   Switch: '<S41>/Switch1'
  //   Switch: '<S41>/Switch2'

  if ((rtb_algDD_o1_g != 0.0F) && (tmp == tmp_0)) {
    // Switch: '<S41>/Switch' incorporates:
    //   Constant: '<S41>/Constant1'

    localB->Switch = 0.0F;
  } else {
    // Switch: '<S41>/Switch'
    localB->Switch = rtb_FilterDifferentiatorTF_f;
  }

  // End of Switch: '<S41>/Switch'

  // DiscreteIntegrator: '<S53>/Integrator'
  if (rtu_OuterOutputs->pid_reset && (localDW->Integrator_PrevResetState_k <= 0))
  {
    localDW->Integrator_DSTATE_o = 0.0F;
  }

  // DiscreteIntegrator: '<S53>/Integrator'
  localB->Integrator_j = 1.82857148E-5F * localB->Switch +
    localDW->Integrator_DSTATE_o;

  // Sum: '<S63>/Sum'
  rtb_FilterDifferentiatorTF_f = (rtb_algDD_o2_l + localB->Integrator_j) +
    DProdOut;

  // Switch: '<S61>/Switch2' incorporates:
  //   RelationalOperator: '<S61>/LowerRelop1'

  if (!(rtb_FilterDifferentiatorTF_f > rtb_Product)) {
    // Switch: '<S61>/Switch' incorporates:
    //   Gain: '<S1>/Gain2'
    //   RelationalOperator: '<S61>/UpperRelop'

    if (rtb_FilterDifferentiatorTF_f < -rtb_Product) {
      rtb_Product = -rtb_Product;
    } else {
      rtb_Product = rtb_FilterDifferentiatorTF_f;
    }

    // End of Switch: '<S61>/Switch'
  }

  // End of Switch: '<S61>/Switch2'

  // Outputs for Atomic SubSystem: '<S8>/Two inputs CRL'
  // AlgorithmDescriptorDelegate generated from: '<S72>/a16'
  arm_inv_park_f32(rtb_Product, rtb_NProdOut, &rtb_FilterDifferentiatorTF_f,
                   &rtb_algDD_o1_g, q, rtb_Cos);

  // End of Outputs for SubSystem: '<S8>/Two inputs CRL'

  // Switch: '<S1>/Switch2' incorporates:
  //   Constant: '<S1>/Constant1'

  if (rtu_OuterOutputs->out_en) {
    // Gain: '<S71>/sqrt3_by_two'
    rtb_Cos = 0.866025388F * rtb_algDD_o1_g;

    // Gain: '<S71>/one_by_two'
    rtb_Product = 0.5F * rtb_FilterDifferentiatorTF_f;

    // Sum: '<S71>/add_c'
    q = (0.0F - rtb_Product) - rtb_Cos;

    // Sum: '<S71>/add_b'
    rtb_Product = rtb_Cos - rtb_Product;

    // MinMax: '<S1>/Min1'
    if ((rtb_FilterDifferentiatorTF_f <= rtb_Product) || rtIsNaNF(rtb_Product))
    {
      rtb_Cos = rtb_FilterDifferentiatorTF_f;
    } else {
      rtb_Cos = rtb_Product;
    }

    if ((!(rtb_Cos <= q)) && (!rtIsNaNF(q))) {
      rtb_Cos = q;
    }

    // Gain: '<S1>/Gain3' incorporates:
    //   Product: '<S1>/Divide1'

    rtb_algDD_o1_g = rtb_NProdOut / rtu_Sensors->supplyvoltagesensors.voltage *
      100.0F;

    // Saturate: '<S1>/Saturation1'
    if (rtb_algDD_o1_g > 100.0F) {
      rtb_algDD_o1_g = 100.0F;
    } else if (rtb_algDD_o1_g < -100.0F) {
      rtb_algDD_o1_g = -100.0F;
    }

    // End of Saturate: '<S1>/Saturation1'

    // Sum: '<S1>/Sum1' incorporates:
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_FilterDifferentiatorTF_f = (rtb_FilterDifferentiatorTF_f - rtb_Cos) /
      rtu_Sensors->supplyvoltagesensors.voltage * 100.0F + 5.0F;

    // Saturate: '<S1>/Saturation'
    if (rtb_FilterDifferentiatorTF_f > 100.0F) {
      rtb_FilterDifferentiatorTF_f = 100.0F;
    } else if (rtb_FilterDifferentiatorTF_f < 0.0F) {
      rtb_FilterDifferentiatorTF_f = 0.0F;
    }

    // Sum: '<S1>/Sum1' incorporates:
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_Product = (rtb_Product - rtb_Cos) /
      rtu_Sensors->supplyvoltagesensors.voltage * 100.0F + 5.0F;

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

    q = (q - rtb_Cos) / rtu_Sensors->supplyvoltagesensors.voltage * 100.0F +
      5.0F;

    // Saturate: '<S1>/Saturation'
    if (q > 100.0F) {
      q = 100.0F;
    } else if (q < 0.0F) {
      q = 0.0F;
    }
  } else {
    rtb_algDD_o1_g = 0.0F;
    rtb_FilterDifferentiatorTF_f = 0.0F;
    rtb_Product = 0.0F;
    q = 0.0F;
  }

  // End of Switch: '<S1>/Switch2'

  // BusCreator: '<S1>/Bus Creator1'
  rty_FOCOutputs->Iq_fbk.current = rtb_y;

  // BusCreator: '<S1>/Bus Creator2'
  rty_FOCOutputs->Id_fbk.current = rtb_Gain1;

  // BusCreator: '<S1>/Bus Creator3' incorporates:
  //   Constant: '<S14>/Constant'

  rty_FOCOutputs->Iq_rms.current = 0.0F;

  // BusCreator: '<S1>/Bus Creator4' incorporates:
  //   Constant: '<S14>/Constant1'

  rty_FOCOutputs->Id_rms.current = 0.0F;

  // BusCreator: '<S1>/Bus Creator'
  rty_FOCOutputs->Vq = rtb_algDD_o1_g;
  rty_FOCOutputs->Vabc[0] = rtb_FilterDifferentiatorTF_f;
  rty_FOCOutputs->Vabc[1] = rtb_Product;
  rty_FOCOutputs->Vabc[2] = q;

  // Sum: '<S1>/Sum3'
  localB->Sum3 = rtb_NProdOut - rtb_Add;
}

// Update for atomic system: '<Root>/FOC inner loop'
void FOCInnerLoop_Update(const ControlOuterOutputs *rtu_OuterOutputs,
  B_FOCInnerLoop_T *localB, DW_FOCInnerLoop_T *localDW)
{
  // Update for DiscreteTransferFcn: '<S100>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_states = localDW->FilterDifferentiatorTF_tmp;

  // Update for UnitDelay: '<S1>/Unit Delay'
  localDW->UnitDelay_DSTATE = localB->Sum3;

  // Update for DiscreteIntegrator: '<S107>/Integrator'
  localDW->Integrator_DSTATE = 1.82857148E-5F * localB->SumI1 +
    localB->Integrator;
  localDW->Integrator_PrevResetState = static_cast<int8_T>
    (rtu_OuterOutputs->pid_reset);

  // Update for DiscreteTransferFcn: '<S46>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_states_k =
    localDW->FilterDifferentiatorTF_tmp_c;

  // Update for DiscreteIntegrator: '<S53>/Integrator' incorporates:
  //   DiscreteIntegrator: '<S107>/Integrator'

  localDW->Integrator_DSTATE_o = 1.82857148E-5F * localB->Switch +
    localB->Integrator_j;
  localDW->Integrator_PrevResetState_k = static_cast<int8_T>
    (rtu_OuterOutputs->pid_reset);
}

//
// File trailer for generated code.
//
// [EOF]
//
