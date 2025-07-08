//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: FOCInnerLoop.cpp
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 9.11
// Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
// C/C++ source code generated on : Tue Jul  8 13:04:41 2025
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
  // InitializeConditions for DiscreteTransferFcn: '<S117>/Filter Differentiator TF' 
  localDW->FilterDifferentiatorTF_icLoad = 1U;

  // InitializeConditions for DiscreteIntegrator: '<S124>/Integrator'
  localDW->Integrator_PrevResetState = 2;
  localDW->Integrator_IC_LOADING = 1U;

  // InitializeConditions for DiscreteTransferFcn: '<S53>/Filter Differentiator TF' 
  localDW->FilterDifferentiatorTF_icLoad_c = 1U;

  // InitializeConditions for DiscreteIntegrator: '<S60>/Integrator'
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
  real32_T DProdOut;
  real32_T rtb_Diff;
  real32_T rtb_IaIbIc0_idx_0;
  real32_T rtb_IaIbIc0_idx_1;
  real32_T rtb_PProdOut_k;
  real32_T rtb_Product;
  real32_T rtb_SinCos_o1;
  real32_T rtb_SinCos_o2;
  real32_T rtb_Switch_c_idx_0;
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
  rtb_IaIbIc0_idx_0 = 0.0F;
  rtb_IaIbIc0_idx_1 = 0.0F;
  for (int32_T i = 0; i < 3; i++) {
    int32_T rtb_IaIbIc0_idx_0_tmp;
    rtb_Diff = rtu_Sensors->motorsensors.Iabc[i];
    rtb_IaIbIc0_idx_0_tmp = i << 1;
    rtb_IaIbIc0_idx_0 += rtCP_IaIbIc0_Gain[rtb_IaIbIc0_idx_0_tmp] * rtb_Diff;
    rtb_IaIbIc0_idx_1 += rtCP_IaIbIc0_Gain[rtb_IaIbIc0_idx_0_tmp + 1] * rtb_Diff;
  }

  // End of Gain: '<S1>/Ia+Ib+Ic=0'

  // Outputs for Atomic SubSystem: '<S16>/Two phase CRL wrap'
  // Gain: '<S17>/one_by_sqrt3' incorporates:
  //   Sum: '<S17>/a_plus_2b'

  rtb_sum_beta = ((rtb_IaIbIc0_idx_0 + rtb_IaIbIc0_idx_1) + rtb_IaIbIc0_idx_1) *
    0.577350259F;

  // End of Outputs for SubSystem: '<S16>/Two phase CRL wrap'

  // Gain: '<S5>/Gain1'
  rtb_Diff = 0.0174532924F * rtu_Sensors->motorsensors.electrical_angle;

  // Trigonometry: '<S19>/SinCos'
  rtb_SinCos_o1 = std::sin(rtb_Diff);
  rtb_SinCos_o2 = std::cos(rtb_Diff);

  // Outputs for Atomic SubSystem: '<S144>/Two inputs CRL'
  // Outputs for Atomic SubSystem: '<S16>/Two phase CRL wrap'
  // Switch: '<S146>/Switch' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S17>/a16'
  //   Product: '<S145>/acos'
  //   Product: '<S145>/asin'
  //   Product: '<S145>/bcos'
  //   Product: '<S145>/bsin'
  //   Sum: '<S145>/sum_Ds'
  //   Sum: '<S145>/sum_Qs'

  rtb_Switch_c_idx_0 = rtb_IaIbIc0_idx_0 * rtb_SinCos_o2 + rtb_sum_beta *
    rtb_SinCos_o1;
  rtb_IaIbIc0_idx_0 = rtb_sum_beta * rtb_SinCos_o2 - rtb_IaIbIc0_idx_0 *
    rtb_SinCos_o1;

  // End of Outputs for SubSystem: '<S16>/Two phase CRL wrap'

  // AlgorithmDescriptorDelegate generated from: '<S145>/a16'
  rtb_IaIbIc0_idx_1 = rtb_Switch_c_idx_0;

  // Sum: '<S1>/Sum' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S145>/a16'

  rtb_Diff = rtu_OuterOutputs->motorcurrent - rtb_IaIbIc0_idx_0;

  // End of Outputs for SubSystem: '<S144>/Two inputs CRL'

  // Product: '<S129>/PProd Out'
  rtb_sum_beta = rtb_Diff * rtu_ConfigurationParameters->pids.currentPID.P;

  // Product: '<S121>/IProd Out'
  rtb_PProdOut_k = rtb_Diff * rtu_ConfigurationParameters->pids.currentPID.I;

  // Product: '<S115>/DProd Out'
  DProdOut = rtb_Diff * rtu_ConfigurationParameters->pids.currentPID.D;

  // SampleTimeMath: '<S119>/Tsamp'
  //
  //  About '<S119>/Tsamp':
  //   y = u * K where K = ( w * Ts )
  //
  rtb_Diff = rtu_ConfigurationParameters->pids.currentPID.N * 2.25E-5F;

  // Sum: '<S86>/sum_alpha' incorporates:
  //   Constant: '<S117>/Filter Den Constant'
  //   Math: '<S117>/Reciprocal'
  //   Sum: '<S117>/SumDen'
  //
  //  About '<S117>/Reciprocal':
  //   Operator: reciprocal

  localB->Switch = 1.0F / (rtb_Diff + 1.0F);

  // DiscreteTransferFcn: '<S117>/Filter Differentiator TF'
  if (rtu_OuterOutputs->pid_reset && (localZCE->FilterDifferentiatorTF_Reset_ZCE
       != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_icLoad = 1U;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE = rtu_OuterOutputs->pid_reset;
  if (localDW->FilterDifferentiatorTF_icLoad != 0) {
    localDW->FilterDifferentiatorTF_states = 0.0F;
    localDW->FilterDifferentiatorTF_icLoad = 0U;
  }

  // Product: '<S117>/Divide' incorporates:
  //   Constant: '<S117>/Filter Den Constant'
  //   Product: '<S53>/Divide'
  //   Sum: '<S117>/SumNum'

  rtb_Diff = (rtb_Diff - 1.0F) * localB->Switch;

  // DiscreteTransferFcn: '<S117>/Filter Differentiator TF' incorporates:
  //   Product: '<S117>/Divide'

  localDW->FilterDifferentiatorTF_tmp = DProdOut - rtb_Diff *
    localDW->FilterDifferentiatorTF_states;

  // Product: '<S127>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S117>/Filter Differentiator TF'
  //   Product: '<S117>/DenCoefOut'

  DProdOut = (localDW->FilterDifferentiatorTF_tmp -
              localDW->FilterDifferentiatorTF_states) * localB->Switch *
    rtu_ConfigurationParameters->pids.currentPID.N;

  // Sum: '<S136>/SumI1' incorporates:
  //   Sum: '<S134>/Sum Fdbk'
  //   Sum: '<S135>/SumI3'
  //   UnitDelay: '<S1>/Unit Delay'

  localB->SumI1 = (localDW->UnitDelay_DSTATE - ((rtb_sum_beta +
    localDW->Integrator_DSTATE) + DProdOut)) + rtb_PProdOut_k;

  // DiscreteIntegrator: '<S124>/Integrator' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S145>/a16'

  if (localDW->Integrator_IC_LOADING != 0) {
    // Outputs for Atomic SubSystem: '<S144>/Two inputs CRL'
    localDW->Integrator_DSTATE = rtb_IaIbIc0_idx_0;

    // End of Outputs for SubSystem: '<S144>/Two inputs CRL'
  }

  if (rtu_OuterOutputs->pid_reset && (localDW->Integrator_PrevResetState <= 0))
  {
    // Outputs for Atomic SubSystem: '<S144>/Two inputs CRL'
    localDW->Integrator_DSTATE = rtb_IaIbIc0_idx_0;

    // End of Outputs for SubSystem: '<S144>/Two inputs CRL'
  }

  // DiscreteIntegrator: '<S124>/Integrator'
  localB->Integrator = 2.25E-5F * localB->SumI1 + localDW->Integrator_DSTATE;

  // Switch: '<S1>/Switch1'
  if (rtu_OuterOutputs->cur_en) {
    // Switch: '<S1>/Switch1' incorporates:
    //   Sum: '<S133>/Sum'

    localDW->UnitDelay_DSTATE = (rtb_sum_beta + localB->Integrator) + DProdOut;
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

  // Outputs for Atomic SubSystem: '<S144>/Two inputs CRL'
  // Product: '<S65>/PProd Out' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S145>/a16'
  //   Gain: '<S1>/Gain'

  rtb_PProdOut_k = -rtb_Switch_c_idx_0 *
    rtu_ConfigurationParameters->pids.currentPID.P;

  // End of Outputs for SubSystem: '<S144>/Two inputs CRL'

  // DiscreteTransferFcn: '<S53>/Filter Differentiator TF' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S145>/a16'
  //   DiscreteTransferFcn: '<S117>/Filter Differentiator TF'
  //   Gain: '<S1>/Gain'
  //   Product: '<S51>/DProd Out'

  if (rtu_OuterOutputs->pid_reset &&
      (localZCE->FilterDifferentiatorTF_Reset_ZCE_o != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_icLoad_c = 1U;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE_o = rtu_OuterOutputs->pid_reset;
  if (localDW->FilterDifferentiatorTF_icLoad_c != 0) {
    localDW->FilterDifferentiatorTF_states_k = 0.0F;
    localDW->FilterDifferentiatorTF_icLoad_c = 0U;
  }

  // Outputs for Atomic SubSystem: '<S144>/Two inputs CRL'
  localDW->FilterDifferentiatorTF_tmp_c = -rtb_Switch_c_idx_0 *
    rtu_ConfigurationParameters->pids.currentPID.D - rtb_Diff *
    localDW->FilterDifferentiatorTF_states_k;

  // End of Outputs for SubSystem: '<S144>/Two inputs CRL'

  // Product: '<S63>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S53>/Filter Differentiator TF'
  //   Math: '<S53>/Reciprocal'
  //   Product: '<S53>/DenCoefOut'
  //
  //  About '<S53>/Reciprocal':
  //   Operator: reciprocal

  rtb_sum_beta = (localDW->FilterDifferentiatorTF_tmp_c -
                  localDW->FilterDifferentiatorTF_states_k) * localB->Switch *
    rtu_ConfigurationParameters->pids.currentPID.N;

  // Sum: '<S71>/Sum Fdbk'
  DProdOut = (rtb_PProdOut_k + localDW->Integrator_DSTATE_o) + rtb_sum_beta;

  // Switch: '<S50>/Switch' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S50>/u_GTE_up'
  //   RelationalOperator: '<S50>/u_GT_lo'
  //   Switch: '<S50>/Switch1'

  if (DProdOut >= rtb_Product) {
    rtb_Diff = rtb_Product;
  } else if (DProdOut > -rtb_Product) {
    // Switch: '<S50>/Switch1'
    rtb_Diff = DProdOut;
  } else {
    rtb_Diff = -rtb_Product;
  }

  // Sum: '<S50>/Diff' incorporates:
  //   Switch: '<S50>/Switch'

  rtb_Diff = DProdOut - rtb_Diff;

  // Outputs for Atomic SubSystem: '<S144>/Two inputs CRL'
  // Sum: '<S86>/sum_alpha' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S145>/a16'
  //   Gain: '<S1>/Gain'
  //   Product: '<S57>/IProd Out'

  localB->Switch = -rtb_Switch_c_idx_0 *
    rtu_ConfigurationParameters->pids.currentPID.I;

  // End of Outputs for SubSystem: '<S144>/Two inputs CRL'

  // Switch: '<S47>/Switch1' incorporates:
  //   Constant: '<S47>/Clamping_zero'
  //   Constant: '<S47>/Constant'
  //   Constant: '<S47>/Constant2'
  //   RelationalOperator: '<S47>/fix for DT propagation issue'

  if (rtb_Diff > 0.0F) {
    tmp = 1;
  } else {
    tmp = -1;
  }

  // Switch: '<S47>/Switch2' incorporates:
  //   Constant: '<S47>/Clamping_zero'
  //   Constant: '<S47>/Constant3'
  //   Constant: '<S47>/Constant4'
  //   RelationalOperator: '<S47>/fix for DT propagation issue1'

  if (localB->Switch > 0.0F) {
    tmp_0 = 1;
  } else {
    tmp_0 = -1;
  }

  // Switch: '<S47>/Switch' incorporates:
  //   Constant: '<S47>/Clamping_zero'
  //   Logic: '<S47>/AND3'
  //   RelationalOperator: '<S47>/Equal1'
  //   RelationalOperator: '<S47>/Relational Operator'
  //   Switch: '<S47>/Switch1'
  //   Switch: '<S47>/Switch2'

  if ((rtb_Diff != 0.0F) && (tmp == tmp_0)) {
    // Sum: '<S86>/sum_alpha' incorporates:
    //   Constant: '<S47>/Constant1'
    //   Switch: '<S47>/Switch'

    localB->Switch = 0.0F;
  }

  // End of Switch: '<S47>/Switch'

  // DiscreteIntegrator: '<S60>/Integrator' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S145>/a16'

  if (localDW->Integrator_IC_LOADING_b != 0) {
    // Outputs for Atomic SubSystem: '<S144>/Two inputs CRL'
    localDW->Integrator_DSTATE_o = rtb_Switch_c_idx_0;

    // End of Outputs for SubSystem: '<S144>/Two inputs CRL'
  }

  if (rtu_OuterOutputs->pid_reset && (localDW->Integrator_PrevResetState_k <= 0))
  {
    // Outputs for Atomic SubSystem: '<S144>/Two inputs CRL'
    localDW->Integrator_DSTATE_o = rtb_Switch_c_idx_0;

    // End of Outputs for SubSystem: '<S144>/Two inputs CRL'
  }

  // DiscreteIntegrator: '<S60>/Integrator'
  localB->Integrator_j = 2.25E-5F * localB->Switch +
    localDW->Integrator_DSTATE_o;

  // Sum: '<S70>/Sum'
  rtb_PProdOut_k = (rtb_PProdOut_k + localB->Integrator_j) + rtb_sum_beta;

  // Switch: '<S68>/Switch2' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S68>/LowerRelop1'
  //   RelationalOperator: '<S68>/UpperRelop'
  //   Switch: '<S68>/Switch'

  if (rtb_PProdOut_k > rtb_Product) {
    rtb_PProdOut_k = rtb_Product;
  } else if (rtb_PProdOut_k < -rtb_Product) {
    // Switch: '<S68>/Switch' incorporates:
    //   Gain: '<S1>/Gain2'

    rtb_PProdOut_k = -rtb_Product;
  }

  // End of Switch: '<S68>/Switch2'

  // Outputs for Atomic SubSystem: '<S85>/Two inputs CRL'
  // Switch: '<S87>/Switch' incorporates:
  //   Product: '<S86>/dcos'
  //   Product: '<S86>/qsin'
  //   Sum: '<S86>/sum_alpha'

  rtb_Switch_c_idx_0 = rtb_PProdOut_k * rtb_SinCos_o2 -
    localDW->UnitDelay_DSTATE * rtb_SinCos_o1;

  // End of Outputs for SubSystem: '<S85>/Two inputs CRL'

  // Switch: '<S1>/Switch2' incorporates:
  //   Constant: '<S1>/Constant1'

  if (rtu_OuterOutputs->out_en) {
    // Outputs for Atomic SubSystem: '<S85>/Two inputs CRL'
    // Gain: '<S82>/sqrt3_by_two' incorporates:
    //   Product: '<S86>/dsin'
    //   Product: '<S86>/qcos'
    //   Sum: '<S86>/sum_beta'

    rtb_SinCos_o2 = (localDW->UnitDelay_DSTATE * rtb_SinCos_o2 + rtb_PProdOut_k *
                     rtb_SinCos_o1) * 0.866025388F;

    // Gain: '<S82>/one_by_two' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S86>/a16'

    rtb_SinCos_o1 = 0.5F * rtb_Switch_c_idx_0;

    // End of Outputs for SubSystem: '<S85>/Two inputs CRL'

    // Sum: '<S82>/add_c'
    rtb_Product = (0.0F - rtb_SinCos_o1) - rtb_SinCos_o2;

    // Sum: '<S82>/add_b'
    rtb_SinCos_o1 = rtb_SinCos_o2 - rtb_SinCos_o1;

    // Outputs for Atomic SubSystem: '<S85>/Two inputs CRL'
    // MinMax: '<S1>/Min1' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S86>/a16'

    if (rtb_Switch_c_idx_0 <= rtb_SinCos_o1) {
      rtb_SinCos_o2 = rtb_Switch_c_idx_0;
    } else {
      rtb_SinCos_o2 = rtb_SinCos_o1;
    }

    // End of Outputs for SubSystem: '<S85>/Two inputs CRL'
    if (rtb_SinCos_o2 > rtb_Product) {
      rtb_SinCos_o2 = rtb_Product;
    }

    // Gain: '<S1>/Gain3' incorporates:
    //   Product: '<S1>/Divide1'

    rtb_PProdOut_k = localDW->UnitDelay_DSTATE / rtu_Sensors->driversensors.Vcc *
      100.0F;

    // Saturate: '<S1>/Saturation1'
    if (rtb_PProdOut_k > 100.0F) {
      // BusCreator: '<S1>/Bus Creator'
      rty_FOCOutputs->Vq = 100.0F;
    } else if (rtb_PProdOut_k < -100.0F) {
      // BusCreator: '<S1>/Bus Creator'
      rty_FOCOutputs->Vq = -100.0F;
    } else {
      // BusCreator: '<S1>/Bus Creator'
      rty_FOCOutputs->Vq = rtb_PProdOut_k;
    }

    // End of Saturate: '<S1>/Saturation1'

    // Outputs for Atomic SubSystem: '<S85>/Two inputs CRL'
    // Sum: '<S1>/Sum1' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S86>/a16'
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_Switch_c_idx_0 = (rtb_Switch_c_idx_0 - rtb_SinCos_o2) /
      rtu_Sensors->driversensors.Vcc * 100.0F + 5.0F;

    // End of Outputs for SubSystem: '<S85>/Two inputs CRL'

    // Saturate: '<S1>/Saturation'
    if (rtb_Switch_c_idx_0 > 100.0F) {
      rtb_Switch_c_idx_0 = 100.0F;
    } else if (rtb_Switch_c_idx_0 < 0.0F) {
      rtb_Switch_c_idx_0 = 0.0F;
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
    rtb_Switch_c_idx_0 = 0.0F;
    rtb_SinCos_o1 = 0.0F;
    rtb_Product = 0.0F;
  }

  // End of Switch: '<S1>/Switch2'

  // BusCreator: '<S1>/Bus Creator' incorporates:
  //   AlgorithmDescriptorDelegate generated from: '<S145>/a16'
  //   Constant: '<S18>/Constant'
  //   Constant: '<S18>/Constant1'

  rty_FOCOutputs->Vabc[0] = rtb_Switch_c_idx_0;
  rty_FOCOutputs->Vabc[1] = rtb_SinCos_o1;
  rty_FOCOutputs->Vabc[2] = rtb_Product;

  // Outputs for Atomic SubSystem: '<S144>/Two inputs CRL'
  rty_FOCOutputs->Iq_fbk = rtb_IaIbIc0_idx_0;

  // End of Outputs for SubSystem: '<S144>/Two inputs CRL'
  rty_FOCOutputs->Id_fbk = rtb_IaIbIc0_idx_1;
  rty_FOCOutputs->Iq_rms = 0.0F;
  rty_FOCOutputs->Id_rms = 0.0F;
}

// Update for atomic system: '<Root>/FOC inner loop'
void FOCInnerLoop_Update(const ControlOuterOutputs *rtu_OuterOutputs,
  B_FOCInnerLoop_T *localB, DW_FOCInnerLoop_T *localDW)
{
  // Update for DiscreteTransferFcn: '<S117>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_icLoad = 0U;
  localDW->FilterDifferentiatorTF_states = localDW->FilterDifferentiatorTF_tmp;

  // Update for DiscreteIntegrator: '<S124>/Integrator'
  localDW->Integrator_IC_LOADING = 0U;
  localDW->Integrator_DSTATE = 2.25E-5F * localB->SumI1 + localB->Integrator;
  localDW->Integrator_PrevResetState = static_cast<int8_T>
    (rtu_OuterOutputs->pid_reset);

  // Update for DiscreteTransferFcn: '<S53>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_icLoad_c = 0U;
  localDW->FilterDifferentiatorTF_states_k =
    localDW->FilterDifferentiatorTF_tmp_c;

  // Update for DiscreteIntegrator: '<S60>/Integrator' incorporates:
  //   DiscreteIntegrator: '<S124>/Integrator'

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
