//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: FOCInnerLoop.cpp
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 8.9
// Simulink Coder version         : 24.2 (R2024b) 21-Jun-2024
// C/C++ source code generated on : Fri May 23 10:48:16 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "FOCInnerLoop.h"
#include "control_foc_types.h"
#include <cmath>
#include "rtwtypes.h"
#include "control_foc_private.h"
#include "zero_crossing_types.h"

// System initialize for atomic system: '<Root>/FOC inner loop'
void FOCInnerLoop_Init(DW_FOCInnerLoop_T *localDW)
{
  // InitializeConditions for DiscreteTransferFcn: '<S108>/Filter Differentiator TF' 
  localDW->FilterDifferentiatorTF_icLoad = 1U;

  // InitializeConditions for DiscreteIntegrator: '<S115>/Integrator'
  localDW->Integrator_PrevResetState = 2;
  localDW->Integrator_IC_LOADING = 1U;

  // InitializeConditions for DiscreteTransferFcn: '<S50>/Filter Differentiator TF' 
  localDW->FilterDifferentiatorTF_icLoad_c = 1U;

  // InitializeConditions for DiscreteIntegrator: '<S57>/Integrator'
  localDW->Integrator_PrevResetState_k = 2;
  localDW->Integrator_IC_LOADING_b = 1U;
}

// Outputs for atomic system: '<Root>/FOC inner loop'
void FOCInnerLoop(const SensorsData *rtu_Sensors, const ActuatorConfiguration
                  *rtu_ConfigurationParameters, const Targets
                  *rtu_Targets_Inport_4, const ControlOuterOutputs
                  *rtu_OuterOutputs, FOCOutputs *rty_FOCOutputs,
                  B_FOCInnerLoop_T *localB, DW_FOCInnerLoop_T *localDW,
                  ZCE_FOCInnerLoop_T *localZCE)
{
  real32_T rtb_IaIbIc0[2];
  real32_T algDD_o1;
  real32_T rtb_Diff;
  real32_T rtb_NProdOut;
  real32_T rtb_PProdOut;
  real32_T rtb_Product;
  real32_T rtb_SinCos_o1;
  real32_T rtb_SinCos_o2;
  real32_T rtb_SumFdbk_m;
  real32_T rtb_Switch_ez_idx_0;
  real32_T rtb_Switch_ez_idx_1;
  real32_T rtb_sum_beta;
  int8_T tmp;
  int8_T tmp_0;

  // MinMax: '<S1>/Min'
  if (rtu_Sensors->driversensors.Vcc <= rtu_ConfigurationParameters->motor.Vmax)
  {
    rtb_Diff = rtu_Sensors->driversensors.Vcc;
  } else {
    rtb_Diff = rtu_ConfigurationParameters->motor.Vmax;
  }

  // Product: '<S1>/Product' incorporates:
  //   Constant: '<S1>/Constant2'
  //   Constant: '<S1>/Constant3'
  //   Gain: '<S1>/Gain4'
  //   Gain: '<S1>/Gain5'
  //   MinMax: '<S1>/Min'
  //   Sum: '<S1>/Sum5'

  rtb_Product = 0.5F * rtb_Diff * 0.975F;

  // Gain: '<S1>/Ia+Ib+Ic=0'
  rtb_Switch_ez_idx_1 = rtu_Sensors->motorsensors.Iabc[1];
  algDD_o1 = rtu_Sensors->motorsensors.Iabc[0];
  rtb_Switch_ez_idx_0 = rtu_Sensors->motorsensors.Iabc[2];
  for (int32_T i = 0; i < 2; i++) {
    rtb_IaIbIc0[i] = (rtCP_IaIbIc0_Gain[i + 2] * rtb_Switch_ez_idx_1 +
                      rtCP_IaIbIc0_Gain[i] * algDD_o1) + rtCP_IaIbIc0_Gain[i + 4]
      * rtb_Switch_ez_idx_0;
  }

  // End of Gain: '<S1>/Ia+Ib+Ic=0'

  // Outputs for Atomic SubSystem: '<S13>/Two phase CRL wrap'
  // Gain: '<S14>/one_by_sqrt3' incorporates:
  //   Sum: '<S14>/a_plus_2b'

  rtb_sum_beta = ((rtb_IaIbIc0[0] + rtb_IaIbIc0[1]) + rtb_IaIbIc0[1]) *
    0.577350259F;

  // End of Outputs for SubSystem: '<S13>/Two phase CRL wrap'

  // Gain: '<S5>/Gain1'
  rtb_Diff = 0.0174532924F * rtu_Sensors->motorsensors.angle;

  // Trigonometry: '<S16>/SinCos'
  rtb_SinCos_o1 = std::sin(rtb_Diff);
  rtb_SinCos_o2 = std::cos(rtb_Diff);

  // Outputs for Atomic SubSystem: '<S11>/Two inputs CRL'
  // Outputs for Atomic SubSystem: '<S13>/Two phase CRL wrap'
  // Switch: '<S134>/Switch' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S14>/a16'
  //   Product: '<S133>/acos'
  //   Product: '<S133>/asin'
  //   Product: '<S133>/bcos'
  //   Product: '<S133>/bsin'
  //   Sum: '<S133>/sum_Ds'
  //   Sum: '<S133>/sum_Qs'

  rtb_Switch_ez_idx_0 = rtb_IaIbIc0[0] * rtb_SinCos_o2 + rtb_sum_beta *
    rtb_SinCos_o1;
  rtb_Switch_ez_idx_1 = rtb_sum_beta * rtb_SinCos_o2 - rtb_IaIbIc0[0] *
    rtb_SinCos_o1;

  // End of Outputs for SubSystem: '<S13>/Two phase CRL wrap'

  // AlgorithmDescriptorDelegate generated from: '<S133>/a16'
  algDD_o1 = rtb_Switch_ez_idx_0;

  // Sum: '<S1>/Sum' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S133>/a16'

  rtb_Diff = rtu_OuterOutputs->motorcurrent - rtb_Switch_ez_idx_1;

  // End of Outputs for SubSystem: '<S11>/Two inputs CRL'

  // Product: '<S120>/PProd Out'
  rtb_PProdOut = rtb_Diff * rtu_ConfigurationParameters->pids.currentPID.P;

  // SampleTimeMath: '<S110>/Tsamp' incorporates:
  //   SampleTimeMath: '<S52>/Tsamp'
  //
  //  About '<S110>/Tsamp':
  //   y = u * K where K = ( w * Ts )
  //    *
  //  About '<S52>/Tsamp':
  //   y = u * K where K = ( w * Ts )
  //
  rtb_SumFdbk_m = rtu_ConfigurationParameters->pids.currentPID.N * 2.25E-5F;

  // Math: '<S108>/Reciprocal' incorporates:
  //   Constant: '<S108>/Filter Den Constant'
  //   Math: '<S50>/Reciprocal'
  //   SampleTimeMath: '<S110>/Tsamp'
  //   Sum: '<S108>/SumDen'
  //
  //  About '<S108>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S50>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S110>/Tsamp':
  //   y = u * K where K = ( w * Ts )
  //
  rtb_sum_beta = 1.0F / (rtb_SumFdbk_m + 1.0F);

  // DiscreteTransferFcn: '<S108>/Filter Differentiator TF'
  if (rtu_OuterOutputs->pid_reset && (localZCE->FilterDifferentiatorTF_Reset_ZCE
       != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_icLoad = 1U;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE = rtu_OuterOutputs->pid_reset;
  if (localDW->FilterDifferentiatorTF_icLoad != 0) {
    localDW->FilterDifferentiatorTF_states = 0.0F;
    localDW->FilterDifferentiatorTF_icLoad = 0U;
  }

  // Product: '<S108>/Divide' incorporates:
  //   Constant: '<S108>/Filter Den Constant'
  //   Math: '<S108>/Reciprocal'
  //   Product: '<S50>/Divide'
  //   SampleTimeMath: '<S110>/Tsamp'
  //   Sum: '<S108>/SumNum'
  //
  //  About '<S108>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S110>/Tsamp':
  //   y = u * K where K = ( w * Ts )
  //
  rtb_SumFdbk_m = (rtb_SumFdbk_m - 1.0F) * rtb_sum_beta;

  // DiscreteTransferFcn: '<S108>/Filter Differentiator TF' incorporates:
  //   Product: '<S106>/DProd Out'
  //   Product: '<S108>/Divide'

  localDW->FilterDifferentiatorTF_tmp = rtb_Diff *
    rtu_ConfigurationParameters->pids.currentPID.D - rtb_SumFdbk_m *
    localDW->FilterDifferentiatorTF_states;

  // Product: '<S118>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S108>/Filter Differentiator TF'
  //   Math: '<S108>/Reciprocal'
  //   Product: '<S108>/DenCoefOut'
  //
  //  About '<S108>/Reciprocal':
  //   Operator: reciprocal

  rtb_NProdOut = (localDW->FilterDifferentiatorTF_tmp -
                  localDW->FilterDifferentiatorTF_states) * rtb_sum_beta *
    rtu_ConfigurationParameters->pids.currentPID.N;

  // Sum: '<S127>/SumI1' incorporates:
  //   Product: '<S112>/IProd Out'
  //   Sum: '<S125>/Sum Fdbk'
  //   Sum: '<S126>/SumI3'
  //   UnitDelay: '<S1>/Unit Delay'

  localB->SumI1 = (localDW->UnitDelay_DSTATE - ((rtb_PProdOut +
    localDW->Integrator_DSTATE) + rtb_NProdOut)) + rtb_Diff *
    rtu_ConfigurationParameters->pids.currentPID.I;

  // DiscreteIntegrator: '<S115>/Integrator' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S133>/a16'

  if (localDW->Integrator_IC_LOADING != 0) {
    // Outputs for Atomic SubSystem: '<S11>/Two inputs CRL'
    localDW->Integrator_DSTATE = rtb_Switch_ez_idx_1;

    // End of Outputs for SubSystem: '<S11>/Two inputs CRL'
  }

  if (rtu_OuterOutputs->pid_reset && (localDW->Integrator_PrevResetState <= 0))
  {
    // Outputs for Atomic SubSystem: '<S11>/Two inputs CRL'
    localDW->Integrator_DSTATE = rtb_Switch_ez_idx_1;

    // End of Outputs for SubSystem: '<S11>/Two inputs CRL'
  }

  // DiscreteIntegrator: '<S115>/Integrator'
  localB->Integrator = 2.25E-5F * localB->SumI1 + localDW->Integrator_DSTATE;

  // Switch: '<S1>/Switch1'
  if (rtu_OuterOutputs->cur_en) {
    // Switch: '<S1>/Switch1' incorporates:
    //   Sum: '<S124>/Sum'

    localDW->UnitDelay_DSTATE = (rtb_PProdOut + localB->Integrator) +
      rtb_NProdOut;
  } else {
    // Switch: '<S1>/Switch1' incorporates:
    //   Gain: '<S1>/Gain6'
    //   Product: '<S1>/Divide2'
    //   Sum: '<S1>/Sum6'

    localDW->UnitDelay_DSTATE = rtu_Targets_Inport_4->voltage *
      rtu_Sensors->driversensors.Vcc * 0.01F + rtu_OuterOutputs->current_limiter;
  }

  // End of Switch: '<S1>/Switch1'

  // Switch: '<S12>/Switch2' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S12>/LowerRelop1'
  //   RelationalOperator: '<S12>/UpperRelop'
  //   Switch: '<S12>/Switch'

  if (localDW->UnitDelay_DSTATE > rtb_Product) {
    // Switch: '<S1>/Switch1'
    localDW->UnitDelay_DSTATE = rtb_Product;
  } else if (localDW->UnitDelay_DSTATE < -rtb_Product) {
    // Switch: '<S1>/Switch1' incorporates:
    //   Gain: '<S1>/Gain2'
    //   Switch: '<S12>/Switch'

    localDW->UnitDelay_DSTATE = -rtb_Product;
  }

  // End of Switch: '<S12>/Switch2'

  // Outputs for Atomic SubSystem: '<S11>/Two inputs CRL'
  // Product: '<S62>/PProd Out' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S133>/a16'
  //   Gain: '<S1>/Gain'

  rtb_PProdOut = -rtb_Switch_ez_idx_0 *
    rtu_ConfigurationParameters->pids.currentPID.P;

  // End of Outputs for SubSystem: '<S11>/Two inputs CRL'

  // DiscreteTransferFcn: '<S50>/Filter Differentiator TF' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S133>/a16'
  //   DiscreteTransferFcn: '<S108>/Filter Differentiator TF'
  //   Gain: '<S1>/Gain'
  //   Product: '<S48>/DProd Out'

  if (rtu_OuterOutputs->pid_reset &&
      (localZCE->FilterDifferentiatorTF_Reset_ZCE_o != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_icLoad_c = 1U;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE_o = rtu_OuterOutputs->pid_reset;
  if (localDW->FilterDifferentiatorTF_icLoad_c != 0) {
    localDW->FilterDifferentiatorTF_states_k = 0.0F;
    localDW->FilterDifferentiatorTF_icLoad_c = 0U;
  }

  // Outputs for Atomic SubSystem: '<S11>/Two inputs CRL'
  localDW->FilterDifferentiatorTF_tmp_c = -rtb_Switch_ez_idx_0 *
    rtu_ConfigurationParameters->pids.currentPID.D - rtb_SumFdbk_m *
    localDW->FilterDifferentiatorTF_states_k;

  // End of Outputs for SubSystem: '<S11>/Two inputs CRL'

  // Product: '<S60>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S50>/Filter Differentiator TF'
  //   Product: '<S50>/DenCoefOut'

  rtb_sum_beta = (localDW->FilterDifferentiatorTF_tmp_c -
                  localDW->FilterDifferentiatorTF_states_k) * rtb_sum_beta *
    rtu_ConfigurationParameters->pids.currentPID.N;

  // Sum: '<S68>/Sum Fdbk'
  rtb_SumFdbk_m = (rtb_PProdOut + localDW->Integrator_DSTATE_o) + rtb_sum_beta;

  // Switch: '<S47>/Switch' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S47>/u_GTE_up'
  //   RelationalOperator: '<S47>/u_GT_lo'
  //   Switch: '<S47>/Switch1'

  if (rtb_SumFdbk_m >= rtb_Product) {
    rtb_Diff = rtb_Product;
  } else if (rtb_SumFdbk_m > -rtb_Product) {
    // Switch: '<S47>/Switch1'
    rtb_Diff = rtb_SumFdbk_m;
  } else {
    rtb_Diff = -rtb_Product;
  }

  // Sum: '<S47>/Diff' incorporates:
  //   Switch: '<S47>/Switch'

  rtb_Diff = rtb_SumFdbk_m - rtb_Diff;

  // Outputs for Atomic SubSystem: '<S11>/Two inputs CRL'
  // Sum: '<S77>/sum_alpha' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S133>/a16'
  //   Gain: '<S1>/Gain'
  //   Product: '<S54>/IProd Out'

  localB->Switch = -rtb_Switch_ez_idx_0 *
    rtu_ConfigurationParameters->pids.currentPID.I;

  // End of Outputs for SubSystem: '<S11>/Two inputs CRL'

  // Switch: '<S44>/Switch1' incorporates:
  //   Constant: '<S44>/Clamping_zero'
  //   Constant: '<S44>/Constant'
  //   Constant: '<S44>/Constant2'
  //   RelationalOperator: '<S44>/fix for DT propagation issue'

  if (rtb_Diff > 0.0F) {
    tmp = 1;
  } else {
    tmp = -1;
  }

  // Switch: '<S44>/Switch2' incorporates:
  //   Constant: '<S44>/Clamping_zero'
  //   Constant: '<S44>/Constant3'
  //   Constant: '<S44>/Constant4'
  //   RelationalOperator: '<S44>/fix for DT propagation issue1'

  if (localB->Switch > 0.0F) {
    tmp_0 = 1;
  } else {
    tmp_0 = -1;
  }

  // Switch: '<S44>/Switch' incorporates:
  //   Constant: '<S44>/Clamping_zero'
  //   Logic: '<S44>/AND3'
  //   RelationalOperator: '<S44>/Equal1'
  //   RelationalOperator: '<S44>/Relational Operator'
  //   Switch: '<S44>/Switch1'
  //   Switch: '<S44>/Switch2'

  if ((rtb_Diff != 0.0F) && (tmp == tmp_0)) {
    // Sum: '<S77>/sum_alpha' incorporates:
    //   Constant: '<S44>/Constant1'
    //   Switch: '<S44>/Switch'

    localB->Switch = 0.0F;
  }

  // End of Switch: '<S44>/Switch'

  // DiscreteIntegrator: '<S57>/Integrator' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S133>/a16'

  if (localDW->Integrator_IC_LOADING_b != 0) {
    // Outputs for Atomic SubSystem: '<S11>/Two inputs CRL'
    localDW->Integrator_DSTATE_o = rtb_Switch_ez_idx_0;

    // End of Outputs for SubSystem: '<S11>/Two inputs CRL'
  }

  if (rtu_OuterOutputs->pid_reset && (localDW->Integrator_PrevResetState_k <= 0))
  {
    // Outputs for Atomic SubSystem: '<S11>/Two inputs CRL'
    localDW->Integrator_DSTATE_o = rtb_Switch_ez_idx_0;

    // End of Outputs for SubSystem: '<S11>/Two inputs CRL'
  }

  // DiscreteIntegrator: '<S57>/Integrator'
  localB->Integrator_j = 2.25E-5F * localB->Switch +
    localDW->Integrator_DSTATE_o;

  // Sum: '<S67>/Sum'
  rtb_Diff = (rtb_PProdOut + localB->Integrator_j) + rtb_sum_beta;

  // Switch: '<S65>/Switch2' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S65>/LowerRelop1'
  //   RelationalOperator: '<S65>/UpperRelop'
  //   Switch: '<S65>/Switch'

  if (rtb_Diff > rtb_Product) {
    rtb_Diff = rtb_Product;
  } else if (rtb_Diff < -rtb_Product) {
    // Switch: '<S65>/Switch' incorporates:
    //   Gain: '<S1>/Gain2'

    rtb_Diff = -rtb_Product;
  }

  // End of Switch: '<S65>/Switch2'

  // Outputs for Atomic SubSystem: '<S9>/Two inputs CRL'
  // Switch: '<S78>/Switch' incorporates:
  //   Product: '<S77>/dcos'
  //   Product: '<S77>/qsin'
  //   Sum: '<S77>/sum_alpha'

  rtb_Switch_ez_idx_0 = rtb_Diff * rtb_SinCos_o2 - localDW->UnitDelay_DSTATE *
    rtb_SinCos_o1;

  // End of Outputs for SubSystem: '<S9>/Two inputs CRL'

  // Switch: '<S1>/Switch2' incorporates:
  //   Constant: '<S1>/Constant1'

  if (rtu_OuterOutputs->out_en) {
    // Outputs for Atomic SubSystem: '<S9>/Two inputs CRL'
    // Gain: '<S76>/sqrt3_by_two' incorporates:
    //   Product: '<S77>/dsin'
    //   Product: '<S77>/qcos'
    //   Sum: '<S77>/sum_beta'

    rtb_SinCos_o2 = (localDW->UnitDelay_DSTATE * rtb_SinCos_o2 + rtb_Diff *
                     rtb_SinCos_o1) * 0.866025388F;

    // Gain: '<S76>/one_by_two' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S77>/a16'

    rtb_SinCos_o1 = 0.5F * rtb_Switch_ez_idx_0;

    // End of Outputs for SubSystem: '<S9>/Two inputs CRL'

    // Sum: '<S76>/add_c'
    rtb_Product = (0.0F - rtb_SinCos_o1) - rtb_SinCos_o2;

    // Sum: '<S76>/add_b'
    rtb_SinCos_o1 = rtb_SinCos_o2 - rtb_SinCos_o1;

    // Outputs for Atomic SubSystem: '<S9>/Two inputs CRL'
    // MinMax: '<S1>/Min1' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S77>/a16'

    if (rtb_Switch_ez_idx_0 <= rtb_SinCos_o1) {
      rtb_SinCos_o2 = rtb_Switch_ez_idx_0;
    } else {
      rtb_SinCos_o2 = rtb_SinCos_o1;
    }

    // End of Outputs for SubSystem: '<S9>/Two inputs CRL'
    if (rtb_SinCos_o2 > rtb_Product) {
      rtb_SinCos_o2 = rtb_Product;
    }

    // Gain: '<S1>/Gain3' incorporates:
    //   Product: '<S1>/Divide1'

    rtb_Diff = localDW->UnitDelay_DSTATE / rtu_Sensors->driversensors.Vcc *
      100.0F;

    // Saturate: '<S1>/Saturation1'
    if (rtb_Diff > 100.0F) {
      // BusCreator: '<S1>/Bus Creator'
      rty_FOCOutputs->Vq = 100.0F;
    } else if (rtb_Diff < -100.0F) {
      // BusCreator: '<S1>/Bus Creator'
      rty_FOCOutputs->Vq = -100.0F;
    } else {
      // BusCreator: '<S1>/Bus Creator'
      rty_FOCOutputs->Vq = rtb_Diff;
    }

    // End of Saturate: '<S1>/Saturation1'

    // Outputs for Atomic SubSystem: '<S9>/Two inputs CRL'
    // Sum: '<S1>/Sum1' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S77>/a16'
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_Switch_ez_idx_0 = (rtb_Switch_ez_idx_0 - rtb_SinCos_o2) /
      rtu_Sensors->driversensors.Vcc * 100.0F + 5.0F;

    // End of Outputs for SubSystem: '<S9>/Two inputs CRL'

    // Saturate: '<S1>/Saturation'
    if (rtb_Switch_ez_idx_0 > 100.0F) {
      rtb_Switch_ez_idx_0 = 100.0F;
    } else if (rtb_Switch_ez_idx_0 < 0.0F) {
      rtb_Switch_ez_idx_0 = 0.0F;
    }

    // Sum: '<S1>/Sum1' incorporates:
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_SinCos_o1 = (rtb_SinCos_o1 - rtb_SinCos_o2) /
      rtu_Sensors->driversensors.Vcc * 100.0F + 5.0F;

    // Saturate: '<S1>/Saturation'
    if (rtb_SinCos_o1 > 100.0F) {
      rtb_SinCos_o1 = 100.0F;
    } else if (rtb_SinCos_o1 < 0.0F) {
      rtb_SinCos_o1 = 0.0F;
    }

    // Sum: '<S1>/Sum1' incorporates:
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_Product = (rtb_Product - rtb_SinCos_o2) / rtu_Sensors->driversensors.Vcc
      * 100.0F + 5.0F;

    // Saturate: '<S1>/Saturation'
    if (rtb_Product > 100.0F) {
      rtb_Product = 100.0F;
    } else if (rtb_Product < 0.0F) {
      rtb_Product = 0.0F;
    }
  } else {
    // BusCreator: '<S1>/Bus Creator' incorporates:
    //   Constant: '<S1>/Constant1'

    rty_FOCOutputs->Vq = 0.0F;
    rtb_Switch_ez_idx_0 = 0.0F;
    rtb_SinCos_o1 = 0.0F;
    rtb_Product = 0.0F;
  }

  // End of Switch: '<S1>/Switch2'

  // BusCreator: '<S1>/Bus Creator' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S133>/a16'
  //   Constant: '<S15>/Constant'
  //   Constant: '<S15>/Constant1'

  rty_FOCOutputs->Vabc[0] = rtb_Switch_ez_idx_0;
  rty_FOCOutputs->Vabc[1] = rtb_SinCos_o1;
  rty_FOCOutputs->Vabc[2] = rtb_Product;

  // Outputs for Atomic SubSystem: '<S11>/Two inputs CRL'
  rty_FOCOutputs->Iq_fbk = rtb_Switch_ez_idx_1;

  // End of Outputs for SubSystem: '<S11>/Two inputs CRL'
  rty_FOCOutputs->Id_fbk = algDD_o1;
  rty_FOCOutputs->Iq_rms = 0.0F;
  rty_FOCOutputs->Id_rms = 0.0F;
}

// Update for atomic system: '<Root>/FOC inner loop'
void FOCInnerLoop_Update(const ControlOuterOutputs *rtu_OuterOutputs,
  B_FOCInnerLoop_T *localB, DW_FOCInnerLoop_T *localDW)
{
  // Update for DiscreteTransferFcn: '<S108>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_icLoad = 0U;
  localDW->FilterDifferentiatorTF_states = localDW->FilterDifferentiatorTF_tmp;

  // Update for DiscreteIntegrator: '<S115>/Integrator'
  localDW->Integrator_IC_LOADING = 0U;
  localDW->Integrator_DSTATE = 2.25E-5F * localB->SumI1 + localB->Integrator;
  localDW->Integrator_PrevResetState = static_cast<int8_T>
    (rtu_OuterOutputs->pid_reset);

  // Update for DiscreteTransferFcn: '<S50>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_icLoad_c = 0U;
  localDW->FilterDifferentiatorTF_states_k =
    localDW->FilterDifferentiatorTF_tmp_c;

  // Update for DiscreteIntegrator: '<S57>/Integrator' incorporates:
  //   DiscreteIntegrator: '<S115>/Integrator'

  localDW->Integrator_IC_LOADING_b = 0U;
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
