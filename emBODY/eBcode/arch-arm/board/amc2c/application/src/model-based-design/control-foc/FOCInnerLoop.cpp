//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: FOCInnerLoop.cpp
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 5.13
// Simulink Coder version         : 9.9 (R2023a) 19-Nov-2022
// C/C++ source code generated on : Mon Oct 16 10:08:10 2023
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "rtwtypes.h"
#include "FOCInnerLoop.h"
#include "control_foc_types.h"

extern "C"
{

#include "rt_nonfinite.h"

}

#include <cmath>
#include "mw_cmsis.h"
#include "arm_math.h"
#include "control_foc_private.h"
#include "zero_crossing_types.h"

// Forward declaration for local functions
static void control_foc_SystemCore_setup(dsp_simulink_MovingRMS_control_foc_T
  *obj);
static void control_foc_SystemCore_setup(dsp_simulink_MovingRMS_control_foc_T
  *obj)
{
  real32_T val;
  boolean_T flag;
  obj->isSetupComplete = false;
  obj->isInitialized = 1;
  flag = (obj->isInitialized == 1);
  if (flag) {
    obj->TunablePropsChanged = true;
  }

  obj->ForgettingFactor = 0.0F;
  obj->TunablePropsChanged = false;
  obj->NumChannels = 1;
  obj->FrameLength = 1;
  if (obj->ForgettingFactor != 0.0F) {
    val = obj->ForgettingFactor;
  } else {
    val = 2.22044605E-16F;
  }

  obj->_pobj0.isInitialized = 0;
  obj->_pobj0.isInitialized = 0;
  flag = (obj->_pobj0.isInitialized == 1);
  if (flag) {
    obj->_pobj0.TunablePropsChanged = true;
  }

  obj->_pobj0.ForgettingFactor = val;
  obj->pStatistic = &obj->_pobj0;
  obj->isSetupComplete = true;
  obj->TunablePropsChanged = false;
}

// System initialize for atomic system:
void control_foc_MovingRMS_Init(DW_MovingRMS_control_foc_T *localDW)
{
  c_dsp_internal_ExponentialMovingAverage_control_foc_T *obj;

  // Start for MATLABSystem: '<S1>/Moving RMS'
  localDW->obj.isInitialized = 0;
  localDW->obj.NumChannels = -1;
  localDW->obj.FrameLength = -1;
  localDW->obj.matlabCodegenIsDeleted = false;
  localDW->objisempty = true;
  control_foc_SystemCore_setup(&localDW->obj);

  // InitializeConditions for MATLABSystem: '<S1>/Moving RMS'
  obj = localDW->obj.pStatistic;
  if (obj->isInitialized == 1) {
    obj->pwN = 1.0F;
    obj->pmN = 0.0F;
  }

  // End of InitializeConditions for MATLABSystem: '<S1>/Moving RMS'
}

// Output and update for atomic system:
void control_foc_MovingRMS(real32_T rtu_0, real32_T rtu_1,
  B_MovingRMS_control_foc_T *localB, DW_MovingRMS_control_foc_T *localDW)
{
  c_dsp_internal_ExponentialMovingAverage_control_foc_T *obj;
  real32_T a;
  real32_T lambda;
  real32_T pmLocal;
  real32_T varargin_1;
  boolean_T p;

  // MATLABSystem: '<S1>/Moving RMS'
  varargin_1 = localDW->obj.ForgettingFactor;
  if ((varargin_1 == rtu_1) || (rtIsNaNF(varargin_1) && rtIsNaNF(rtu_1))) {
  } else {
    p = (localDW->obj.isInitialized == 1);
    if (p) {
      localDW->obj.TunablePropsChanged = true;
    }

    localDW->obj.ForgettingFactor = rtu_1;
  }

  if (localDW->obj.TunablePropsChanged) {
    localDW->obj.TunablePropsChanged = false;
    obj = localDW->obj.pStatistic;
    p = (obj->isInitialized == 1);
    if (p) {
      obj->TunablePropsChanged = true;
    }

    localDW->obj.pStatistic->ForgettingFactor = localDW->obj.ForgettingFactor;
  }

  a = std::abs(rtu_0);
  obj = localDW->obj.pStatistic;
  if (obj->isInitialized != 1) {
    obj->isSetupComplete = false;
    obj->isInitialized = 1;
    obj->pwN = 1.0F;
    obj->pmN = 0.0F;
    obj->plambda = obj->ForgettingFactor;
    obj->isSetupComplete = true;
    obj->TunablePropsChanged = false;
    obj->pwN = 1.0F;
    obj->pmN = 0.0F;
  }

  if (obj->TunablePropsChanged) {
    obj->TunablePropsChanged = false;
    obj->plambda = obj->ForgettingFactor;
  }

  varargin_1 = obj->pwN;
  pmLocal = obj->pmN;
  lambda = obj->plambda;
  a = (1.0F - 1.0F / varargin_1) * pmLocal + 1.0F / varargin_1 * (a * a);
  obj->pwN = lambda * varargin_1 + 1.0F;
  obj->pmN = a;

  // MATLABSystem: '<S1>/Moving RMS'
  mw_arm_sqrt_f32(a, &localB->MovingRMS);
}

// Termination for atomic system:
void control_foc_MovingRMS_Term(DW_MovingRMS_control_foc_T *localDW)
{
  c_dsp_internal_ExponentialMovingAverage_control_foc_T *obj;

  // Terminate for MATLABSystem: '<S1>/Moving RMS'
  if (!localDW->obj.matlabCodegenIsDeleted) {
    localDW->obj.matlabCodegenIsDeleted = true;
    if ((localDW->obj.isInitialized == 1) && localDW->obj.isSetupComplete) {
      obj = localDW->obj.pStatistic;
      if (obj->isInitialized == 1) {
        obj->isInitialized = 2;
      }

      localDW->obj.NumChannels = -1;
      localDW->obj.FrameLength = -1;
    }
  }

  // End of Terminate for MATLABSystem: '<S1>/Moving RMS'
}

// System initialize for atomic system: '<Root>/FOC inner loop'
void FOCInnerLoop_Init(DW_FOCInnerLoop_T *localDW)
{
  // InitializeConditions for DiscreteTransferFcn: '<S90>/Filter Differentiator TF' 
  localDW->FilterDifferentiatorTF_states = 0.0F;

  // InitializeConditions for DiscreteIntegrator: '<S97>/Integrator'
  localDW->Integrator_DSTATE = 0.0F;
  localDW->Integrator_PrevResetState = 2;

  // InitializeConditions for DiscreteTransferFcn: '<S38>/Filter Differentiator TF' 
  localDW->FilterDifferentiatorTF_states_k = 0.0F;

  // InitializeConditions for DiscreteIntegrator: '<S45>/Integrator'
  localDW->Integrator_DSTATE_o = 0.0F;
  localDW->Integrator_PrevResetState_k = 2;
  control_foc_MovingRMS_Init(&localDW->MovingRMS);
  control_foc_MovingRMS_Init(&localDW->MovingRMS1);
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
  // local block i/o variables
  real32_T rtb_Saturation2;
  real32_T rtb_algDD_o1;
  real32_T rtb_algDD_o2;
  int32_T i;
  real32_T rtb_IaIbIc0[2];
  real32_T DProdOut;
  real32_T rtb_Add;
  real32_T rtb_Product;
  real32_T rtb_SinCos_o1;
  real32_T rtb_SinCos_o2;
  real32_T rtb_Unary_Minus;
  real32_T rtb_algDD_o1_p;
  real32_T rtb_algDD_o2_n;
  real32_T rtb_sum_alpha;
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
  if ((rtu_ConfigurationParameters->motorconfig.Vcc <=
       rtu_ConfigurationParameters->motorconfig.Vmax) || rtIsNaNF
      (rtu_ConfigurationParameters->motorconfig.Vmax)) {
    rtb_sum_alpha = rtu_ConfigurationParameters->motorconfig.Vcc;
  } else {
    rtb_sum_alpha = rtu_ConfigurationParameters->motorconfig.Vmax;
  }

  // Product: '<S1>/Product' incorporates:
  //   Gain: '<S1>/Gain4'
  //   MinMax: '<S1>/Min'

  rtb_Product = 0.5F * rtb_sum_alpha * localC->Sum5;

  // Gain: '<S1>/Ia+Ib+Ic=0'
  rtb_algDD_o1_p = rtu_Sensors->motorsensors.Iabc[1];
  rtb_algDD_o2_n = rtu_Sensors->motorsensors.Iabc[0];
  rtb_Unary_Minus = rtu_Sensors->motorsensors.Iabc[2];
  for (i = 0; i < 2; i++) {
    rtb_IaIbIc0[i] = (rtCP_IaIbIc0_Gain[i + 2] * rtb_algDD_o1_p +
                      rtCP_IaIbIc0_Gain[i] * rtb_algDD_o2_n) +
      rtCP_IaIbIc0_Gain[i + 4] * rtb_Unary_Minus;
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
  // AlgorithmDescriptorDelegate generated from: '<S7>/a16' incorporates:
  //   Product: '<S7>/asin'
  //   Product: '<S7>/bcos'
  //   Sum: '<S7>/sum_Qs'
  //   UnaryMinus: '<S114>/Unary_Minus'

  rtb_algDD_o1 = -(rtb_algDD_o2_n * rtb_SinCos_o2 - rtb_algDD_o1_p *
                   rtb_SinCos_o1);

  // AlgorithmDescriptorDelegate generated from: '<S7>/a16' incorporates:
  //   Product: '<S7>/acos'
  //   Product: '<S7>/bsin'
  //   Sum: '<S7>/sum_Ds'

  rtb_algDD_o2 = rtb_algDD_o1_p * rtb_SinCos_o2 + rtb_algDD_o2_n * rtb_SinCos_o1;

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // Sum: '<S1>/Sum'
  rtb_Unary_Minus = rtu_OuterOutputs->motorcurrent.current - rtb_algDD_o2;

  // Product: '<S102>/PProd Out'
  rtb_algDD_o1_p = rtb_Unary_Minus * rtu_ConfigurationParameters->CurLoopPID.P;

  // Product: '<S94>/IProd Out'
  rtb_algDD_o2_n = rtb_Unary_Minus * rtu_ConfigurationParameters->CurLoopPID.I;

  // SampleTimeMath: '<S92>/Tsamp' incorporates:
  //   SampleTimeMath: '<S40>/Tsamp'
  //
  //  About '<S92>/Tsamp':
  //   y = u * K where K = ( w * Ts )
  //
  //  About '<S40>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  DProdOut = rtu_ConfigurationParameters->CurLoopPID.N * 3.0E-5F;

  // Math: '<S90>/Reciprocal' incorporates:
  //   Constant: '<S90>/Filter Den Constant'
  //   Math: '<S38>/Reciprocal'
  //   SampleTimeMath: '<S92>/Tsamp'
  //   Sum: '<S90>/SumDen'
  //
  //  About '<S90>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S38>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S92>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_sum_alpha = 1.0F / (DProdOut + 1.0F);

  // DiscreteTransferFcn: '<S90>/Filter Differentiator TF'
  if (rtu_OuterOutputs->pid_reset && (localZCE->FilterDifferentiatorTF_Reset_ZCE
       != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_states = 0.0F;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE = rtu_OuterOutputs->pid_reset;

  // Product: '<S90>/Divide' incorporates:
  //   Constant: '<S90>/Filter Den Constant'
  //   Math: '<S90>/Reciprocal'
  //   Product: '<S38>/Divide'
  //   SampleTimeMath: '<S92>/Tsamp'
  //   Sum: '<S90>/SumNum'
  //
  //  About '<S90>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S92>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  DProdOut = (DProdOut - 1.0F) * rtb_sum_alpha;

  // DiscreteTransferFcn: '<S90>/Filter Differentiator TF' incorporates:
  //   Product: '<S89>/DProd Out'
  //   Product: '<S90>/Divide'

  localDW->FilterDifferentiatorTF_tmp = rtb_Unary_Minus *
    rtu_ConfigurationParameters->CurLoopPID.D - DProdOut *
    localDW->FilterDifferentiatorTF_states;

  // Product: '<S100>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S90>/Filter Differentiator TF'
  //   Math: '<S90>/Reciprocal'
  //   Product: '<S90>/DenCoefOut'
  //
  //  About '<S90>/Reciprocal':
  //   Operator: reciprocal

  rtb_Unary_Minus = (localDW->FilterDifferentiatorTF_tmp -
                     localDW->FilterDifferentiatorTF_states) * rtb_sum_alpha *
    rtu_ConfigurationParameters->CurLoopPID.N;

  // Sum: '<S109>/SumI1' incorporates:
  //   Sum: '<S107>/Sum Fdbk'
  //   Sum: '<S108>/SumI3'
  //   UnitDelay: '<S1>/Unit Delay'

  localB->SumI1 = (localDW->UnitDelay_DSTATE - ((rtb_algDD_o1_p +
    localDW->Integrator_DSTATE) + rtb_Unary_Minus)) + rtb_algDD_o2_n;

  // DiscreteIntegrator: '<S97>/Integrator'
  if (rtu_OuterOutputs->pid_reset && (localDW->Integrator_PrevResetState <= 0))
  {
    localDW->Integrator_DSTATE = 0.0F;
  }

  // DiscreteIntegrator: '<S97>/Integrator'
  localB->Integrator = 3.0E-5F * localB->SumI1 + localDW->Integrator_DSTATE;

  // Switch: '<S1>/Switch1' incorporates:
  //   Gain: '<S1>/Gain6'
  //   Product: '<S1>/Divide2'
  //   Sum: '<S106>/Sum'
  //   Sum: '<S1>/Sum2'
  //   Sum: '<S1>/Sum6'

  if (rtu_OuterOutputs->cur_en) {
    rtb_algDD_o1_p = ((rtb_algDD_o1_p + localB->Integrator) + rtb_Unary_Minus) +
      rtb_Add;
  } else {
    rtb_algDD_o1_p = rtu_Targets->motorvoltage.voltage * rtb_Product * 0.01F +
      rtu_OuterOutputs->current_limiter;
  }

  // End of Switch: '<S1>/Switch1'

  // Switch: '<S8>/Switch2' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S8>/LowerRelop1'
  //   RelationalOperator: '<S8>/UpperRelop'
  //   Switch: '<S8>/Switch'

  if (rtb_algDD_o1_p > rtb_Product) {
    rtb_algDD_o1_p = rtb_Product;
  } else if (rtb_algDD_o1_p < -rtb_Product) {
    // Switch: '<S8>/Switch' incorporates:
    //   Gain: '<S1>/Gain2'

    rtb_algDD_o1_p = -rtb_Product;
  }

  // End of Switch: '<S8>/Switch2'

  // Product: '<S50>/PProd Out' incorporates:
  //   Gain: '<S1>/Gain'

  rtb_Unary_Minus = -rtb_algDD_o1 * rtu_ConfigurationParameters->CurLoopPID.P;

  // DiscreteTransferFcn: '<S38>/Filter Differentiator TF' incorporates:
  //   DiscreteTransferFcn: '<S90>/Filter Differentiator TF'
  //   Gain: '<S1>/Gain'
  //   Product: '<S37>/DProd Out'

  if (rtu_OuterOutputs->pid_reset &&
      (localZCE->FilterDifferentiatorTF_Reset_ZCE_o != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_states_k = 0.0F;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE_o = rtu_OuterOutputs->pid_reset;
  localDW->FilterDifferentiatorTF_tmp_c = -rtb_algDD_o1 *
    rtu_ConfigurationParameters->CurLoopPID.D - DProdOut *
    localDW->FilterDifferentiatorTF_states_k;

  // Product: '<S48>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S38>/Filter Differentiator TF'
  //   Product: '<S38>/DenCoefOut'

  DProdOut = (localDW->FilterDifferentiatorTF_tmp_c -
              localDW->FilterDifferentiatorTF_states_k) * rtb_sum_alpha *
    rtu_ConfigurationParameters->CurLoopPID.N;

  // Sum: '<S56>/Sum Fdbk'
  rtb_algDD_o2_n = (rtb_Unary_Minus + localDW->Integrator_DSTATE_o) + DProdOut;

  // Switch: '<S36>/Switch' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S36>/u_GTE_up'
  //   RelationalOperator: '<S36>/u_GT_lo'
  //   Switch: '<S36>/Switch1'

  if (rtb_algDD_o2_n >= rtb_Product) {
    rtb_sum_alpha = rtb_Product;
  } else if (rtb_algDD_o2_n > -rtb_Product) {
    // Switch: '<S36>/Switch1'
    rtb_sum_alpha = rtb_algDD_o2_n;
  } else {
    rtb_sum_alpha = -rtb_Product;
  }

  // Sum: '<S36>/Diff' incorporates:
  //   Switch: '<S36>/Switch'

  rtb_algDD_o2_n -= rtb_sum_alpha;

  // Product: '<S42>/IProd Out' incorporates:
  //   Gain: '<S1>/Gain'

  rtb_sum_alpha = -rtb_algDD_o1 * rtu_ConfigurationParameters->CurLoopPID.I;

  // Switch: '<S33>/Switch1' incorporates:
  //   Constant: '<S33>/Clamping_zero'
  //   Constant: '<S33>/Constant'
  //   Constant: '<S33>/Constant2'
  //   RelationalOperator: '<S33>/fix for DT propagation issue'

  if (rtb_algDD_o2_n > 0.0F) {
    tmp = 1;
  } else {
    tmp = -1;
  }

  // Switch: '<S33>/Switch2' incorporates:
  //   Constant: '<S33>/Clamping_zero'
  //   Constant: '<S33>/Constant3'
  //   Constant: '<S33>/Constant4'
  //   RelationalOperator: '<S33>/fix for DT propagation issue1'

  if (rtb_sum_alpha > 0.0F) {
    tmp_0 = 1;
  } else {
    tmp_0 = -1;
  }

  // Switch: '<S33>/Switch' incorporates:
  //   Constant: '<S33>/Clamping_zero'
  //   Logic: '<S33>/AND3'
  //   RelationalOperator: '<S33>/Equal1'
  //   RelationalOperator: '<S33>/Relational Operator'
  //   Switch: '<S33>/Switch1'
  //   Switch: '<S33>/Switch2'

  if ((rtb_algDD_o2_n != 0.0F) && (tmp == tmp_0)) {
    // Switch: '<S33>/Switch' incorporates:
    //   Constant: '<S33>/Constant1'

    localB->Switch = 0.0F;
  } else {
    // Switch: '<S33>/Switch'
    localB->Switch = rtb_sum_alpha;
  }

  // End of Switch: '<S33>/Switch'

  // DiscreteIntegrator: '<S45>/Integrator'
  if (rtu_OuterOutputs->pid_reset && (localDW->Integrator_PrevResetState_k <= 0))
  {
    localDW->Integrator_DSTATE_o = 0.0F;
  }

  // DiscreteIntegrator: '<S45>/Integrator'
  localB->Integrator_j = 3.0E-5F * localB->Switch + localDW->Integrator_DSTATE_o;

  // Sum: '<S55>/Sum'
  rtb_algDD_o2_n = (rtb_Unary_Minus + localB->Integrator_j) + DProdOut;

  // Switch: '<S53>/Switch2' incorporates:
  //   RelationalOperator: '<S53>/LowerRelop1'

  if (!(rtb_algDD_o2_n > rtb_Product)) {
    // Switch: '<S53>/Switch' incorporates:
    //   Gain: '<S1>/Gain2'
    //   RelationalOperator: '<S53>/UpperRelop'

    if (rtb_algDD_o2_n < -rtb_Product) {
      rtb_Product = -rtb_Product;
    } else {
      rtb_Product = rtb_algDD_o2_n;
    }

    // End of Switch: '<S53>/Switch'
  }

  // End of Switch: '<S53>/Switch2'

  // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
  // Switch: '<S63>/Switch' incorporates:
  //   Product: '<S5>/dsin'
  //   Product: '<S5>/qcos'
  //   Sum: '<S5>/sum_beta'

  rtb_IaIbIc0[0] = rtb_algDD_o1_p * rtb_SinCos_o2 + rtb_Product * rtb_SinCos_o1;

  // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'

  // Switch: '<S1>/Switch2' incorporates:
  //   Constant: '<S1>/Constant1'

  if (rtu_OuterOutputs->out_en) {
    // Gain: '<S1>/Gain3' incorporates:
    //   Product: '<S1>/Divide1'

    rtb_Unary_Minus = rtb_algDD_o1_p /
      rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F;

    // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
    // Gain: '<S4>/sqrt3_by_two' incorporates:
    //   Product: '<S5>/dcos'
    //   Product: '<S5>/qsin'
    //   Sum: '<S5>/sum_alpha'
    //   UnaryMinus: '<S63>/Unary_Minus'

    rtb_algDD_o2_n = -(rtb_Product * rtb_SinCos_o2 - rtb_algDD_o1_p *
                       rtb_SinCos_o1) * 0.866025388F;

    // Gain: '<S4>/one_by_two' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S5>/a16'

    rtb_SinCos_o1 = 0.5F * rtb_IaIbIc0[0];

    // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'

    // Sum: '<S4>/add_c'
    rtb_SinCos_o2 = (0.0F - rtb_SinCos_o1) - rtb_algDD_o2_n;

    // Sum: '<S4>/add_b'
    rtb_SinCos_o1 = rtb_algDD_o2_n - rtb_SinCos_o1;

    // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
    // MinMax: '<S1>/Min1' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S5>/a16'

    if ((rtb_IaIbIc0[0] <= rtb_SinCos_o1) || rtIsNaNF(rtb_SinCos_o1)) {
      rtb_algDD_o2_n = rtb_IaIbIc0[0];
    } else {
      rtb_algDD_o2_n = rtb_SinCos_o1;
    }

    // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'
    if ((!(rtb_algDD_o2_n <= rtb_SinCos_o2)) && (!rtIsNaNF(rtb_SinCos_o2))) {
      rtb_algDD_o2_n = rtb_SinCos_o2;
    }

    // Saturate: '<S1>/Saturation1'
    if (rtb_Unary_Minus > 100.0F) {
      rtb_Unary_Minus = 100.0F;
    } else if (rtb_Unary_Minus < -100.0F) {
      rtb_Unary_Minus = -100.0F;
    }

    // End of Saturate: '<S1>/Saturation1'

    // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
    // Sum: '<S1>/Sum1' incorporates:
    //   AlgorithmDescriptorDelegate generated from: '<S5>/a16'
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_Product = (rtb_IaIbIc0[0] - rtb_algDD_o2_n) /
      rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F + 5.0F;

    // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'

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

    rtb_SinCos_o1 = (rtb_SinCos_o1 - rtb_algDD_o2_n) /
      rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F + 5.0F;

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

    rtb_SinCos_o2 = (rtb_SinCos_o2 - rtb_algDD_o2_n) /
      rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F + 5.0F;

    // Saturate: '<S1>/Saturation'
    if (rtb_SinCos_o2 > 100.0F) {
      rtb_SinCos_o2 = 100.0F;
    } else if (rtb_SinCos_o2 < 0.0F) {
      rtb_SinCos_o2 = 0.0F;
    }
  } else {
    rtb_Unary_Minus = 0.0F;
    rtb_Product = 0.0F;
    rtb_SinCos_o1 = 0.0F;
    rtb_SinCos_o2 = 0.0F;
  }

  // End of Switch: '<S1>/Switch2'

  // BusCreator: '<S1>/Bus Creator1'
  rty_FOCOutputs->Iq_fbk.current = rtb_algDD_o2;

  // BusCreator: '<S1>/Bus Creator2'
  rty_FOCOutputs->Id_fbk.current = rtb_algDD_o1;

  // Saturate: '<S1>/Saturation2'
  if (rtu_ConfigurationParameters->estimationconfig.current_rms_lambda > 1.0F) {
    // Saturate: '<S1>/Saturation2'
    rtb_Saturation2 = 1.0F;
  } else if (rtu_ConfigurationParameters->estimationconfig.current_rms_lambda <
             0.0F) {
    // Saturate: '<S1>/Saturation2'
    rtb_Saturation2 = 0.0F;
  } else {
    // Saturate: '<S1>/Saturation2'
    rtb_Saturation2 =
      rtu_ConfigurationParameters->estimationconfig.current_rms_lambda;
  }

  // End of Saturate: '<S1>/Saturation2'
  control_foc_MovingRMS(rtb_algDD_o2, rtb_Saturation2, &localB->MovingRMS,
                        &localDW->MovingRMS);

  // BusCreator: '<S1>/Bus Creator3'
  rty_FOCOutputs->Iq_rms.current = localB->MovingRMS.MovingRMS;
  control_foc_MovingRMS(rtb_algDD_o1, rtb_Saturation2, &localB->MovingRMS1,
                        &localDW->MovingRMS1);

  // BusCreator: '<S1>/Bus Creator4'
  rty_FOCOutputs->Id_rms.current = localB->MovingRMS1.MovingRMS;

  // BusCreator: '<S1>/Bus Creator'
  rty_FOCOutputs->Vq = rtb_Unary_Minus;
  rty_FOCOutputs->Vabc[0] = rtb_Product;
  rty_FOCOutputs->Vabc[1] = rtb_SinCos_o1;
  rty_FOCOutputs->Vabc[2] = rtb_SinCos_o2;

  // Sum: '<S1>/Sum3'
  localB->Sum3 = rtb_algDD_o1_p - rtb_Add;
}

// Update for atomic system: '<Root>/FOC inner loop'
void FOCInnerLoop_Update(const ControlOuterOutputs *rtu_OuterOutputs,
  B_FOCInnerLoop_T *localB, DW_FOCInnerLoop_T *localDW)
{
  // Update for DiscreteTransferFcn: '<S90>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_states = localDW->FilterDifferentiatorTF_tmp;

  // Update for UnitDelay: '<S1>/Unit Delay'
  localDW->UnitDelay_DSTATE = localB->Sum3;

  // Update for DiscreteIntegrator: '<S97>/Integrator'
  localDW->Integrator_DSTATE = 3.0E-5F * localB->SumI1 + localB->Integrator;
  localDW->Integrator_PrevResetState = static_cast<int8_T>
    (rtu_OuterOutputs->pid_reset);

  // Update for DiscreteTransferFcn: '<S38>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_states_k =
    localDW->FilterDifferentiatorTF_tmp_c;

  // Update for DiscreteIntegrator: '<S45>/Integrator' incorporates:
  //   DiscreteIntegrator: '<S97>/Integrator'

  localDW->Integrator_DSTATE_o = 3.0E-5F * localB->Switch + localB->Integrator_j;
  localDW->Integrator_PrevResetState_k = static_cast<int8_T>
    (rtu_OuterOutputs->pid_reset);
}

// Termination for atomic system: '<Root>/FOC inner loop'
void FOCInnerLoop_Term(DW_FOCInnerLoop_T *localDW)
{
  control_foc_MovingRMS_Term(&localDW->MovingRMS);
  control_foc_MovingRMS_Term(&localDW->MovingRMS1);
}

//
// File trailer for generated code.
//
// [EOF]
//
