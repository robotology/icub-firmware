//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: FOCInnerLoop.cpp
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 6.19
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Wed Mar 13 10:35:55 2024
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
#include "rtwtypes.h"
#include "control_foc_private.h"
#include "zero_crossing_types.h"

// System initialize for atomic system: '<Root>/FOC inner loop'
void FOCInnerLoop_Init(DW_FOCInnerLoop_T *localDW)
{
  // InitializeConditions for DiscreteTransferFcn: '<S99>/Filter Differentiator TF' 
  localDW->FilterDifferentiatorTF_states = 0.0F;

  // InitializeConditions for DiscreteIntegrator: '<S106>/Integrator'
  localDW->Integrator_DSTATE = 0.0F;
  localDW->Integrator_PrevResetState = 2;

  // InitializeConditions for DiscreteTransferFcn: '<S45>/Filter Differentiator TF' 
  localDW->FilterDifferentiatorTF_states_k = 0.0F;

  // InitializeConditions for DiscreteIntegrator: '<S52>/Integrator'
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
  real32_T rtb_Add;
  real32_T rtb_Diff;
  real32_T rtb_FilterDifferentiatorTF_f;
  real32_T rtb_Product;
  real32_T rtb_SinCos_o1;
  real32_T rtb_SinCos_o2;
  real32_T rtb_algDD_o1;
  real32_T rtb_algDD_o1_g;
  real32_T rtb_algDD_o2;
  real32_T rtb_algDD_o2_l;
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
    rtb_Diff = rtu_ConfigurationParameters->motorconfig.Vmax;
  } else {
    rtb_Diff = rtu_Sensors->supplyvoltagesensors.voltage;
  }

  // Product: '<S1>/Product' incorporates:
  //   Gain: '<S1>/Gain4'
  //   MinMax: '<S1>/Min'

  rtb_Product = 0.5F * rtb_Diff * localC->Sum5;

  // Gain: '<S1>/Ia+Ib+Ic=0'
  rtb_algDD_o1_g = rtu_Sensors->motorsensors.Iabc[1];
  rtb_SinCos_o1 = rtu_Sensors->motorsensors.Iabc[0];
  rtb_SinCos_o2 = rtu_Sensors->motorsensors.Iabc[2];
  for (int32_T i = 0; i < 2; i++) {
    rtb_IaIbIc0[i] = (rtCP_IaIbIc0_Gain[i + 2] * rtb_algDD_o1_g +
                      rtCP_IaIbIc0_Gain[i] * rtb_SinCos_o1) +
      rtCP_IaIbIc0_Gain[i + 4] * rtb_SinCos_o2;
  }

  // End of Gain: '<S1>/Ia+Ib+Ic=0'

  // Outputs for Atomic SubSystem: '<S12>/Two phase CRL wrap'
  // AlgorithmDescriptorDelegate generated from: '<S13>/a16'
  arm_clarke_f32(rtb_IaIbIc0[0], rtb_IaIbIc0[1], &rtb_algDD_o1_g,
                 &rtb_algDD_o2_l);

  // End of Outputs for SubSystem: '<S12>/Two phase CRL wrap'

  // Gain: '<S5>/Gain1'
  rtb_Diff = 0.0174532924F * rtu_Sensors->motorsensors.angle;

  // Trigonometry: '<S15>/SinCos'
  rtb_SinCos_o1 = std::sin(rtb_Diff);
  rtb_SinCos_o2 = std::cos(rtb_Diff);

  // Outputs for Atomic SubSystem: '<S10>/Two inputs CRL'
  // AlgorithmDescriptorDelegate generated from: '<S123>/a16'
  arm_park_f32(rtb_algDD_o1_g, rtb_algDD_o2_l, &rtb_algDD_o1, &rtb_algDD_o2,
               rtb_SinCos_o1, rtb_SinCos_o2);

  // End of Outputs for SubSystem: '<S10>/Two inputs CRL'

  // Sum: '<S1>/Sum'
  rtb_Diff = rtu_OuterOutputs->motorcurrent.current - rtb_algDD_o2;

  // Product: '<S111>/PProd Out'
  rtb_algDD_o1_g = rtb_Diff * rtu_ConfigurationParameters->CurLoopPID.P;

  // Product: '<S103>/IProd Out'
  rtb_algDD_o2_l = rtb_Diff * rtu_ConfigurationParameters->CurLoopPID.I;

  // SampleTimeMath: '<S101>/Tsamp' incorporates:
  //   SampleTimeMath: '<S47>/Tsamp'
  //
  //  About '<S101>/Tsamp':
  //   y = u * K where K = ( w * Ts )
  //
  //  About '<S47>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_FilterDifferentiatorTF_f = rtu_ConfigurationParameters->CurLoopPID.N *
    2.25E-5F;

  // Math: '<S99>/Reciprocal' incorporates:
  //   Constant: '<S99>/Filter Den Constant'
  //   Math: '<S45>/Reciprocal'
  //   SampleTimeMath: '<S101>/Tsamp'
  //   Sum: '<S99>/SumDen'
  //
  //  About '<S99>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S45>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S101>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  DProdOut = 1.0F / (rtb_FilterDifferentiatorTF_f + 1.0F);

  // DiscreteTransferFcn: '<S99>/Filter Differentiator TF'
  if (rtu_OuterOutputs->pid_reset && (localZCE->FilterDifferentiatorTF_Reset_ZCE
       != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_states = 0.0F;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE = rtu_OuterOutputs->pid_reset;

  // Product: '<S99>/Divide' incorporates:
  //   Constant: '<S99>/Filter Den Constant'
  //   Math: '<S99>/Reciprocal'
  //   Product: '<S45>/Divide'
  //   SampleTimeMath: '<S101>/Tsamp'
  //   Sum: '<S99>/SumNum'
  //
  //  About '<S99>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S101>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_FilterDifferentiatorTF_f = (rtb_FilterDifferentiatorTF_f - 1.0F) *
    DProdOut;

  // DiscreteTransferFcn: '<S99>/Filter Differentiator TF' incorporates:
  //   Product: '<S98>/DProd Out'
  //   Product: '<S99>/Divide'

  localDW->FilterDifferentiatorTF_tmp = rtb_Diff *
    rtu_ConfigurationParameters->CurLoopPID.D - rtb_FilterDifferentiatorTF_f *
    localDW->FilterDifferentiatorTF_states;

  // Product: '<S109>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S99>/Filter Differentiator TF'
  //   Math: '<S99>/Reciprocal'
  //   Product: '<S99>/DenCoefOut'
  //
  //  About '<S99>/Reciprocal':
  //   Operator: reciprocal

  rtb_Diff = (localDW->FilterDifferentiatorTF_tmp -
              localDW->FilterDifferentiatorTF_states) * DProdOut *
    rtu_ConfigurationParameters->CurLoopPID.N;

  // Sum: '<S118>/SumI1' incorporates:
  //   Sum: '<S116>/Sum Fdbk'
  //   Sum: '<S117>/SumI3'
  //   UnitDelay: '<S1>/Unit Delay'

  localB->SumI1 = (localDW->UnitDelay_DSTATE - ((rtb_algDD_o1_g +
    localDW->Integrator_DSTATE) + rtb_Diff)) + rtb_algDD_o2_l;

  // DiscreteIntegrator: '<S106>/Integrator'
  if (rtu_OuterOutputs->pid_reset && (localDW->Integrator_PrevResetState <= 0))
  {
    localDW->Integrator_DSTATE = 0.0F;
  }

  // DiscreteIntegrator: '<S106>/Integrator'
  localB->Integrator = 2.25E-5F * localB->SumI1 + localDW->Integrator_DSTATE;

  // Switch: '<S1>/Switch1' incorporates:
  //   Gain: '<S1>/Gain6'
  //   Product: '<S1>/Divide2'
  //   Sum: '<S115>/Sum'
  //   Sum: '<S1>/Sum2'
  //   Sum: '<S1>/Sum6'

  if (rtu_OuterOutputs->cur_en) {
    rtb_algDD_o1_g = ((rtb_algDD_o1_g + localB->Integrator) + rtb_Diff) +
      rtb_Add;
  } else {
    rtb_algDD_o1_g = rtu_Targets->motorvoltage.voltage *
      rtu_Sensors->supplyvoltagesensors.voltage * 0.01F +
      rtu_OuterOutputs->current_limiter;
  }

  // End of Switch: '<S1>/Switch1'

  // Switch: '<S11>/Switch2' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S11>/LowerRelop1'
  //   RelationalOperator: '<S11>/UpperRelop'
  //   Switch: '<S11>/Switch'

  if (rtb_algDD_o1_g > rtb_Product) {
    rtb_algDD_o1_g = rtb_Product;
  } else if (rtb_algDD_o1_g < -rtb_Product) {
    // Switch: '<S11>/Switch' incorporates:
    //   Gain: '<S1>/Gain2'

    rtb_algDD_o1_g = -rtb_Product;
  }

  // End of Switch: '<S11>/Switch2'

  // Product: '<S57>/PProd Out' incorporates:
  //   Gain: '<S1>/Gain'

  rtb_algDD_o2_l = -rtb_algDD_o1 * rtu_ConfigurationParameters->CurLoopPID.P;

  // DiscreteTransferFcn: '<S45>/Filter Differentiator TF' incorporates:
  //   DiscreteTransferFcn: '<S99>/Filter Differentiator TF'
  //   Gain: '<S1>/Gain'
  //   Product: '<S44>/DProd Out'

  if (rtu_OuterOutputs->pid_reset &&
      (localZCE->FilterDifferentiatorTF_Reset_ZCE_o != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_states_k = 0.0F;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE_o = rtu_OuterOutputs->pid_reset;
  localDW->FilterDifferentiatorTF_tmp_c = -rtb_algDD_o1 *
    rtu_ConfigurationParameters->CurLoopPID.D - rtb_FilterDifferentiatorTF_f *
    localDW->FilterDifferentiatorTF_states_k;

  // Product: '<S55>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S45>/Filter Differentiator TF'
  //   Product: '<S45>/DenCoefOut'

  DProdOut = (localDW->FilterDifferentiatorTF_tmp_c -
              localDW->FilterDifferentiatorTF_states_k) * DProdOut *
    rtu_ConfigurationParameters->CurLoopPID.N;

  // Sum: '<S63>/Sum Fdbk'
  rtb_FilterDifferentiatorTF_f = (rtb_algDD_o2_l + localDW->Integrator_DSTATE_o)
    + DProdOut;

  // Switch: '<S43>/Switch' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S43>/u_GTE_up'
  //   RelationalOperator: '<S43>/u_GT_lo'
  //   Switch: '<S43>/Switch1'

  if (rtb_FilterDifferentiatorTF_f >= rtb_Product) {
    rtb_Diff = rtb_Product;
  } else if (rtb_FilterDifferentiatorTF_f > -rtb_Product) {
    // Switch: '<S43>/Switch1'
    rtb_Diff = rtb_FilterDifferentiatorTF_f;
  } else {
    rtb_Diff = -rtb_Product;
  }

  // Sum: '<S43>/Diff' incorporates:
  //   Switch: '<S43>/Switch'

  rtb_Diff = rtb_FilterDifferentiatorTF_f - rtb_Diff;

  // Product: '<S49>/IProd Out' incorporates:
  //   Gain: '<S1>/Gain'

  rtb_FilterDifferentiatorTF_f = -rtb_algDD_o1 *
    rtu_ConfigurationParameters->CurLoopPID.I;

  // Switch: '<S40>/Switch1' incorporates:
  //   Constant: '<S40>/Clamping_zero'
  //   Constant: '<S40>/Constant'
  //   Constant: '<S40>/Constant2'
  //   RelationalOperator: '<S40>/fix for DT propagation issue'

  if (rtb_Diff > 0.0F) {
    tmp = 1;
  } else {
    tmp = -1;
  }

  // Switch: '<S40>/Switch2' incorporates:
  //   Constant: '<S40>/Clamping_zero'
  //   Constant: '<S40>/Constant3'
  //   Constant: '<S40>/Constant4'
  //   RelationalOperator: '<S40>/fix for DT propagation issue1'

  if (rtb_FilterDifferentiatorTF_f > 0.0F) {
    tmp_0 = 1;
  } else {
    tmp_0 = -1;
  }

  // Switch: '<S40>/Switch' incorporates:
  //   Constant: '<S40>/Clamping_zero'
  //   Logic: '<S40>/AND3'
  //   RelationalOperator: '<S40>/Equal1'
  //   RelationalOperator: '<S40>/Relational Operator'
  //   Switch: '<S40>/Switch1'
  //   Switch: '<S40>/Switch2'

  if ((rtb_Diff != 0.0F) && (tmp == tmp_0)) {
    // Switch: '<S40>/Switch' incorporates:
    //   Constant: '<S40>/Constant1'

    localB->Switch = 0.0F;
  } else {
    // Switch: '<S40>/Switch'
    localB->Switch = rtb_FilterDifferentiatorTF_f;
  }

  // End of Switch: '<S40>/Switch'

  // DiscreteIntegrator: '<S52>/Integrator'
  if (rtu_OuterOutputs->pid_reset && (localDW->Integrator_PrevResetState_k <= 0))
  {
    localDW->Integrator_DSTATE_o = 0.0F;
  }

  // DiscreteIntegrator: '<S52>/Integrator'
  localB->Integrator_j = 2.25E-5F * localB->Switch +
    localDW->Integrator_DSTATE_o;

  // Sum: '<S62>/Sum'
  rtb_FilterDifferentiatorTF_f = (rtb_algDD_o2_l + localB->Integrator_j) +
    DProdOut;

  // Switch: '<S60>/Switch2' incorporates:
  //   RelationalOperator: '<S60>/LowerRelop1'

  if (!(rtb_FilterDifferentiatorTF_f > rtb_Product)) {
    // Switch: '<S60>/Switch' incorporates:
    //   Gain: '<S1>/Gain2'
    //   RelationalOperator: '<S60>/UpperRelop'

    if (rtb_FilterDifferentiatorTF_f < -rtb_Product) {
      rtb_Product = -rtb_Product;
    } else {
      rtb_Product = rtb_FilterDifferentiatorTF_f;
    }

    // End of Switch: '<S60>/Switch'
  }

  // End of Switch: '<S60>/Switch2'

  // Outputs for Atomic SubSystem: '<S8>/Two inputs CRL'
  // AlgorithmDescriptorDelegate generated from: '<S71>/a16'
  arm_inv_park_f32(rtb_Product, rtb_algDD_o1_g, &rtb_FilterDifferentiatorTF_f,
                   &rtb_Diff, rtb_SinCos_o1, rtb_SinCos_o2);

  // End of Outputs for SubSystem: '<S8>/Two inputs CRL'

  // Switch: '<S1>/Switch2' incorporates:
  //   Constant: '<S1>/Constant1'

  if (rtu_OuterOutputs->out_en) {
    // Gain: '<S70>/sqrt3_by_two'
    rtb_SinCos_o2 = 0.866025388F * rtb_Diff;

    // Gain: '<S70>/one_by_two'
    rtb_Product = 0.5F * rtb_FilterDifferentiatorTF_f;

    // Sum: '<S70>/add_c'
    rtb_SinCos_o1 = (0.0F - rtb_Product) - rtb_SinCos_o2;

    // Sum: '<S70>/add_b'
    rtb_Product = rtb_SinCos_o2 - rtb_Product;

    // MinMax: '<S1>/Min1'
    if ((rtb_FilterDifferentiatorTF_f <= rtb_Product) || rtIsNaNF(rtb_Product))
    {
      rtb_SinCos_o2 = rtb_FilterDifferentiatorTF_f;
    } else {
      rtb_SinCos_o2 = rtb_Product;
    }

    if ((!(rtb_SinCos_o2 <= rtb_SinCos_o1)) && (!rtIsNaNF(rtb_SinCos_o1))) {
      rtb_SinCos_o2 = rtb_SinCos_o1;
    }

    // Gain: '<S1>/Gain3' incorporates:
    //   Product: '<S1>/Divide1'

    rtb_Diff = rtb_algDD_o1_g / rtu_Sensors->supplyvoltagesensors.voltage *
      100.0F;

    // Saturate: '<S1>/Saturation1'
    if (rtb_Diff > 100.0F) {
      rtb_Diff = 100.0F;
    } else if (rtb_Diff < -100.0F) {
      rtb_Diff = -100.0F;
    }

    // End of Saturate: '<S1>/Saturation1'

    // Sum: '<S1>/Sum1' incorporates:
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_FilterDifferentiatorTF_f = (rtb_FilterDifferentiatorTF_f - rtb_SinCos_o2)
      / rtu_Sensors->supplyvoltagesensors.voltage * 100.0F + 5.0F;

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

    rtb_Product = (rtb_Product - rtb_SinCos_o2) /
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

    rtb_SinCos_o1 = (rtb_SinCos_o1 - rtb_SinCos_o2) /
      rtu_Sensors->supplyvoltagesensors.voltage * 100.0F + 5.0F;

    // Saturate: '<S1>/Saturation'
    if (rtb_SinCos_o1 > 100.0F) {
      rtb_SinCos_o1 = 100.0F;
    } else if (rtb_SinCos_o1 < 0.0F) {
      rtb_SinCos_o1 = 0.0F;
    }
  } else {
    rtb_Diff = 0.0F;
    rtb_FilterDifferentiatorTF_f = 0.0F;
    rtb_Product = 0.0F;
    rtb_SinCos_o1 = 0.0F;
  }

  // End of Switch: '<S1>/Switch2'

  // BusCreator: '<S1>/Bus Creator1'
  rty_FOCOutputs->Iq_fbk.current = rtb_algDD_o2;

  // BusCreator: '<S1>/Bus Creator2'
  rty_FOCOutputs->Id_fbk.current = rtb_algDD_o1;

  // BusCreator: '<S1>/Bus Creator3' incorporates:
  //   Constant: '<S14>/Constant'

  rty_FOCOutputs->Iq_rms.current = 0.0F;

  // BusCreator: '<S1>/Bus Creator4' incorporates:
  //   Constant: '<S14>/Constant1'

  rty_FOCOutputs->Id_rms.current = 0.0F;

  // BusCreator: '<S1>/Bus Creator'
  rty_FOCOutputs->Vq = rtb_Diff;
  rty_FOCOutputs->Vabc[0] = rtb_FilterDifferentiatorTF_f;
  rty_FOCOutputs->Vabc[1] = rtb_Product;
  rty_FOCOutputs->Vabc[2] = rtb_SinCos_o1;

  // Sum: '<S1>/Sum3'
  localB->Sum3 = rtb_algDD_o1_g - rtb_Add;
}

// Update for atomic system: '<Root>/FOC inner loop'
void FOCInnerLoop_Update(const ControlOuterOutputs *rtu_OuterOutputs,
  B_FOCInnerLoop_T *localB, DW_FOCInnerLoop_T *localDW)
{
  // Update for DiscreteTransferFcn: '<S99>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_states = localDW->FilterDifferentiatorTF_tmp;

  // Update for UnitDelay: '<S1>/Unit Delay'
  localDW->UnitDelay_DSTATE = localB->Sum3;

  // Update for DiscreteIntegrator: '<S106>/Integrator'
  localDW->Integrator_DSTATE = 2.25E-5F * localB->SumI1 + localB->Integrator;
  localDW->Integrator_PrevResetState = static_cast<int8_T>
    (rtu_OuterOutputs->pid_reset);

  // Update for DiscreteTransferFcn: '<S45>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_states_k =
    localDW->FilterDifferentiatorTF_tmp_c;

  // Update for DiscreteIntegrator: '<S52>/Integrator' incorporates:
  //   DiscreteIntegrator: '<S106>/Integrator'

  localDW->Integrator_DSTATE_o = 2.25E-5F * localB->Switch +
    localB->Integrator_j;
  localDW->Integrator_PrevResetState_k = static_cast<int8_T>
    (rtu_OuterOutputs->pid_reset);
}

//
// File trailer for generated code.
//
// [EOF]
//
