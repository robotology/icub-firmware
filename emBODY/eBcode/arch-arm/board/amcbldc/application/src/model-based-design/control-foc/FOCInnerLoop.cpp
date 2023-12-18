//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: FOCInnerLoop.cpp
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 6.4
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Thu Dec 14 10:43:43 2023
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "FOCInnerLoop.h"
#include "rtwtypes.h"
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

  // Start for MATLABSystem: '<S1>/Moving RMS'
  flag = (obj->isInitialized == 1);
  if (flag) {
    // Start for MATLABSystem: '<S1>/Moving RMS'
    obj->TunablePropsChanged = true;
  }

  // Start for MATLABSystem: '<S1>/Moving RMS'
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
    // Start for MATLABSystem: '<S1>/Moving RMS'
    obj->_pobj0.TunablePropsChanged = true;
  }

  // Start for MATLABSystem: '<S1>/Moving RMS'
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
  // InitializeConditions for DiscreteTransferFcn: '<S95>/Filter Differentiator TF' 
  localDW->FilterDifferentiatorTF_states = 0.0F;

  // InitializeConditions for DiscreteIntegrator: '<S102>/Integrator'
  localDW->Integrator_DSTATE = 0.0F;
  localDW->Integrator_PrevResetState = 2;

  // InitializeConditions for DiscreteTransferFcn: '<S41>/Filter Differentiator TF' 
  localDW->FilterDifferentiatorTF_states_k = 0.0F;

  // InitializeConditions for DiscreteIntegrator: '<S48>/Integrator'
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
  int32_T i;
  real32_T rtb_IaIbIc0[2];
  real32_T DProdOut;
  real32_T rtb_Add;
  real32_T rtb_Cos;
  real32_T rtb_FilterDifferentiatorTF_f;
  real32_T rtb_PProdOut_k;
  real32_T rtb_Product;
  real32_T rtb_Sin;
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
  if ((rtu_ConfigurationParameters->motorconfig.Vcc <=
       rtu_ConfigurationParameters->motorconfig.Vmax) || rtIsNaNF
      (rtu_ConfigurationParameters->motorconfig.Vmax)) {
    rtb_algDD_o2_l = rtu_ConfigurationParameters->motorconfig.Vcc;
  } else {
    rtb_algDD_o2_l = rtu_ConfigurationParameters->motorconfig.Vmax;
  }

  // Product: '<S1>/Product' incorporates:
  //   Gain: '<S1>/Gain4'
  //   MinMax: '<S1>/Min'

  rtb_Product = 0.5F * rtb_algDD_o2_l * localC->Sum5;

  // Gain: '<S1>/Ia+Ib+Ic=0'
  rtb_algDD_o1_g = rtu_Sensors->motorsensors.Iabc[1];
  rtb_Sin = rtu_Sensors->motorsensors.Iabc[0];
  rtb_Cos = rtu_Sensors->motorsensors.Iabc[2];
  for (i = 0; i < 2; i++) {
    rtb_IaIbIc0[i] = (rtCP_IaIbIc0_Gain[i + 2] * rtb_algDD_o1_g +
                      rtCP_IaIbIc0_Gain[i] * rtb_Sin) + rtCP_IaIbIc0_Gain[i + 4]
      * rtb_Cos;
  }

  // End of Gain: '<S1>/Ia+Ib+Ic=0'

  // Outputs for Atomic SubSystem: '<S10>/Two phase CRL wrap'
  // AlgorithmDescriptorDelegate generated from: '<S11>/a16'
  arm_clarke_f32(rtb_IaIbIc0[0], rtb_IaIbIc0[1], &rtb_algDD_o1_g,
                 &rtb_algDD_o2_l);

  // End of Outputs for SubSystem: '<S10>/Two phase CRL wrap'

  // Gain: '<S3>/Gain1'
  rtb_PProdOut_k = 0.0174532924F * rtu_Sensors->motorsensors.angle;

  // Trigonometry: '<S1>/Sin'
  rtb_Sin = arm_sin_f32(rtb_PProdOut_k);

  // Trigonometry: '<S1>/Cos'
  rtb_Cos = arm_cos_f32(rtb_PProdOut_k);

  // Outputs for Atomic SubSystem: '<S8>/Two inputs CRL'
  // AlgorithmDescriptorDelegate generated from: '<S119>/a16'
  arm_park_f32(rtb_algDD_o1_g, rtb_algDD_o2_l, &rtb_algDD_o1, &rtb_algDD_o2,
               rtb_Sin, rtb_Cos);

  // End of Outputs for SubSystem: '<S8>/Two inputs CRL'

  // Sum: '<S1>/Sum'
  rtb_PProdOut_k = rtu_OuterOutputs->motorcurrent.current - rtb_algDD_o2;

  // Product: '<S107>/PProd Out'
  rtb_algDD_o1_g = rtb_PProdOut_k * rtu_ConfigurationParameters->CurLoopPID.P;

  // Product: '<S99>/IProd Out'
  rtb_algDD_o2_l = rtb_PProdOut_k * rtu_ConfigurationParameters->CurLoopPID.I;

  // SampleTimeMath: '<S97>/Tsamp' incorporates:
  //   SampleTimeMath: '<S43>/Tsamp'
  //
  //  About '<S97>/Tsamp':
  //   y = u * K where K = ( w * Ts )
  //
  //  About '<S43>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_FilterDifferentiatorTF_f = rtu_ConfigurationParameters->CurLoopPID.N *
    1.82857148E-5F;

  // Math: '<S95>/Reciprocal' incorporates:
  //   Constant: '<S95>/Filter Den Constant'
  //   Math: '<S41>/Reciprocal'
  //   SampleTimeMath: '<S97>/Tsamp'
  //   Sum: '<S95>/SumDen'
  //
  //  About '<S95>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S41>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S97>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  DProdOut = 1.0F / (rtb_FilterDifferentiatorTF_f + 1.0F);

  // DiscreteTransferFcn: '<S95>/Filter Differentiator TF'
  if (rtu_OuterOutputs->pid_reset && (localZCE->FilterDifferentiatorTF_Reset_ZCE
       != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_states = 0.0F;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE = rtu_OuterOutputs->pid_reset;

  // Product: '<S95>/Divide' incorporates:
  //   Constant: '<S95>/Filter Den Constant'
  //   Math: '<S95>/Reciprocal'
  //   Product: '<S41>/Divide'
  //   SampleTimeMath: '<S97>/Tsamp'
  //   Sum: '<S95>/SumNum'
  //
  //  About '<S95>/Reciprocal':
  //   Operator: reciprocal
  //
  //  About '<S97>/Tsamp':
  //   y = u * K where K = ( w * Ts )

  rtb_FilterDifferentiatorTF_f = (rtb_FilterDifferentiatorTF_f - 1.0F) *
    DProdOut;

  // DiscreteTransferFcn: '<S95>/Filter Differentiator TF' incorporates:
  //   Product: '<S94>/DProd Out'
  //   Product: '<S95>/Divide'

  localDW->FilterDifferentiatorTF_tmp = rtb_PProdOut_k *
    rtu_ConfigurationParameters->CurLoopPID.D - rtb_FilterDifferentiatorTF_f *
    localDW->FilterDifferentiatorTF_states;

  // Product: '<S105>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S95>/Filter Differentiator TF'
  //   Math: '<S95>/Reciprocal'
  //   Product: '<S95>/DenCoefOut'
  //
  //  About '<S95>/Reciprocal':
  //   Operator: reciprocal

  rtb_PProdOut_k = (localDW->FilterDifferentiatorTF_tmp -
                    localDW->FilterDifferentiatorTF_states) * DProdOut *
    rtu_ConfigurationParameters->CurLoopPID.N;

  // Sum: '<S114>/SumI1' incorporates:
  //   Sum: '<S112>/Sum Fdbk'
  //   Sum: '<S113>/SumI3'
  //   UnitDelay: '<S1>/Unit Delay'

  localB->SumI1 = (localDW->UnitDelay_DSTATE - ((rtb_algDD_o1_g +
    localDW->Integrator_DSTATE) + rtb_PProdOut_k)) + rtb_algDD_o2_l;

  // DiscreteIntegrator: '<S102>/Integrator'
  if (rtu_OuterOutputs->pid_reset && (localDW->Integrator_PrevResetState <= 0))
  {
    localDW->Integrator_DSTATE = 0.0F;
  }

  // DiscreteIntegrator: '<S102>/Integrator'
  localB->Integrator = 1.82857148E-5F * localB->SumI1 +
    localDW->Integrator_DSTATE;

  // Switch: '<S1>/Switch1' incorporates:
  //   Gain: '<S1>/Gain6'
  //   Product: '<S1>/Divide2'
  //   Sum: '<S111>/Sum'
  //   Sum: '<S1>/Sum2'
  //   Sum: '<S1>/Sum6'

  if (rtu_OuterOutputs->cur_en) {
    rtb_algDD_o1_g = ((rtb_algDD_o1_g + localB->Integrator) + rtb_PProdOut_k) +
      rtb_Add;
  } else {
    rtb_algDD_o1_g = rtu_Targets->motorvoltage.voltage * rtb_Product * 0.01F +
      rtu_OuterOutputs->current_limiter;
  }

  // End of Switch: '<S1>/Switch1'

  // Switch: '<S9>/Switch2' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S9>/LowerRelop1'
  //   RelationalOperator: '<S9>/UpperRelop'
  //   Switch: '<S9>/Switch'

  if (rtb_algDD_o1_g > rtb_Product) {
    rtb_algDD_o1_g = rtb_Product;
  } else if (rtb_algDD_o1_g < -rtb_Product) {
    // Switch: '<S9>/Switch' incorporates:
    //   Gain: '<S1>/Gain2'

    rtb_algDD_o1_g = -rtb_Product;
  }

  // End of Switch: '<S9>/Switch2'

  // Product: '<S53>/PProd Out' incorporates:
  //   Gain: '<S1>/Gain'

  rtb_PProdOut_k = -rtb_algDD_o1 * rtu_ConfigurationParameters->CurLoopPID.P;

  // DiscreteTransferFcn: '<S41>/Filter Differentiator TF' incorporates:
  //   DiscreteTransferFcn: '<S95>/Filter Differentiator TF'
  //   Gain: '<S1>/Gain'
  //   Product: '<S40>/DProd Out'

  if (rtu_OuterOutputs->pid_reset &&
      (localZCE->FilterDifferentiatorTF_Reset_ZCE_o != POS_ZCSIG)) {
    localDW->FilterDifferentiatorTF_states_k = 0.0F;
  }

  localZCE->FilterDifferentiatorTF_Reset_ZCE_o = rtu_OuterOutputs->pid_reset;
  localDW->FilterDifferentiatorTF_tmp_c = -rtb_algDD_o1 *
    rtu_ConfigurationParameters->CurLoopPID.D - rtb_FilterDifferentiatorTF_f *
    localDW->FilterDifferentiatorTF_states_k;

  // Product: '<S51>/NProd Out' incorporates:
  //   DiscreteTransferFcn: '<S41>/Filter Differentiator TF'
  //   Product: '<S41>/DenCoefOut'

  DProdOut = (localDW->FilterDifferentiatorTF_tmp_c -
              localDW->FilterDifferentiatorTF_states_k) * DProdOut *
    rtu_ConfigurationParameters->CurLoopPID.N;

  // Sum: '<S59>/Sum Fdbk'
  rtb_FilterDifferentiatorTF_f = (rtb_PProdOut_k + localDW->Integrator_DSTATE_o)
    + DProdOut;

  // Switch: '<S39>/Switch' incorporates:
  //   Gain: '<S1>/Gain2'
  //   RelationalOperator: '<S39>/u_GTE_up'
  //   RelationalOperator: '<S39>/u_GT_lo'
  //   Switch: '<S39>/Switch1'

  if (rtb_FilterDifferentiatorTF_f >= rtb_Product) {
    rtb_algDD_o2_l = rtb_Product;
  } else if (rtb_FilterDifferentiatorTF_f > -rtb_Product) {
    // Switch: '<S39>/Switch1'
    rtb_algDD_o2_l = rtb_FilterDifferentiatorTF_f;
  } else {
    rtb_algDD_o2_l = -rtb_Product;
  }

  // Sum: '<S39>/Diff' incorporates:
  //   Switch: '<S39>/Switch'

  rtb_algDD_o2_l = rtb_FilterDifferentiatorTF_f - rtb_algDD_o2_l;

  // Product: '<S45>/IProd Out' incorporates:
  //   Gain: '<S1>/Gain'

  rtb_FilterDifferentiatorTF_f = -rtb_algDD_o1 *
    rtu_ConfigurationParameters->CurLoopPID.I;

  // Switch: '<S36>/Switch1' incorporates:
  //   Constant: '<S36>/Clamping_zero'
  //   Constant: '<S36>/Constant'
  //   Constant: '<S36>/Constant2'
  //   RelationalOperator: '<S36>/fix for DT propagation issue'

  if (rtb_algDD_o2_l > 0.0F) {
    tmp = 1;
  } else {
    tmp = -1;
  }

  // Switch: '<S36>/Switch2' incorporates:
  //   Constant: '<S36>/Clamping_zero'
  //   Constant: '<S36>/Constant3'
  //   Constant: '<S36>/Constant4'
  //   RelationalOperator: '<S36>/fix for DT propagation issue1'

  if (rtb_FilterDifferentiatorTF_f > 0.0F) {
    tmp_0 = 1;
  } else {
    tmp_0 = -1;
  }

  // Switch: '<S36>/Switch' incorporates:
  //   Constant: '<S36>/Clamping_zero'
  //   Logic: '<S36>/AND3'
  //   RelationalOperator: '<S36>/Equal1'
  //   RelationalOperator: '<S36>/Relational Operator'
  //   Switch: '<S36>/Switch1'
  //   Switch: '<S36>/Switch2'

  if ((rtb_algDD_o2_l != 0.0F) && (tmp == tmp_0)) {
    // Switch: '<S36>/Switch' incorporates:
    //   Constant: '<S36>/Constant1'

    localB->Switch = 0.0F;
  } else {
    // Switch: '<S36>/Switch'
    localB->Switch = rtb_FilterDifferentiatorTF_f;
  }

  // End of Switch: '<S36>/Switch'

  // DiscreteIntegrator: '<S48>/Integrator'
  if (rtu_OuterOutputs->pid_reset && (localDW->Integrator_PrevResetState_k <= 0))
  {
    localDW->Integrator_DSTATE_o = 0.0F;
  }

  // DiscreteIntegrator: '<S48>/Integrator'
  localB->Integrator_j = 1.82857148E-5F * localB->Switch +
    localDW->Integrator_DSTATE_o;

  // Sum: '<S58>/Sum'
  rtb_FilterDifferentiatorTF_f = (rtb_PProdOut_k + localB->Integrator_j) +
    DProdOut;

  // Switch: '<S56>/Switch2' incorporates:
  //   RelationalOperator: '<S56>/LowerRelop1'

  if (!(rtb_FilterDifferentiatorTF_f > rtb_Product)) {
    // Switch: '<S56>/Switch' incorporates:
    //   Gain: '<S1>/Gain2'
    //   RelationalOperator: '<S56>/UpperRelop'

    if (rtb_FilterDifferentiatorTF_f < -rtb_Product) {
      rtb_Product = -rtb_Product;
    } else {
      rtb_Product = rtb_FilterDifferentiatorTF_f;
    }

    // End of Switch: '<S56>/Switch'
  }

  // End of Switch: '<S56>/Switch2'

  // Outputs for Atomic SubSystem: '<S6>/Two inputs CRL'
  // AlgorithmDescriptorDelegate generated from: '<S67>/a16'
  arm_inv_park_f32(rtb_Product, rtb_algDD_o1_g, &rtb_FilterDifferentiatorTF_f,
                   &rtb_PProdOut_k, rtb_Sin, rtb_Cos);

  // End of Outputs for SubSystem: '<S6>/Two inputs CRL'

  // Switch: '<S1>/Switch2' incorporates:
  //   Constant: '<S1>/Constant1'

  if (rtu_OuterOutputs->out_en) {
    // Gain: '<S66>/sqrt3_by_two'
    rtb_Cos = 0.866025388F * rtb_PProdOut_k;

    // Gain: '<S66>/one_by_two'
    rtb_Product = 0.5F * rtb_FilterDifferentiatorTF_f;

    // Sum: '<S66>/add_c'
    rtb_Sin = (0.0F - rtb_Product) - rtb_Cos;

    // Sum: '<S66>/add_b'
    rtb_Product = rtb_Cos - rtb_Product;

    // MinMax: '<S1>/Min1'
    if ((rtb_FilterDifferentiatorTF_f <= rtb_Product) || rtIsNaNF(rtb_Product))
    {
      rtb_Cos = rtb_FilterDifferentiatorTF_f;
    } else {
      rtb_Cos = rtb_Product;
    }

    if ((!(rtb_Cos <= rtb_Sin)) && (!rtIsNaNF(rtb_Sin))) {
      rtb_Cos = rtb_Sin;
    }

    // Gain: '<S1>/Gain3' incorporates:
    //   Product: '<S1>/Divide1'

    rtb_PProdOut_k = rtb_algDD_o1_g /
      rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F;

    // Saturate: '<S1>/Saturation1'
    if (rtb_PProdOut_k > 100.0F) {
      rtb_PProdOut_k = 100.0F;
    } else if (rtb_PProdOut_k < -100.0F) {
      rtb_PProdOut_k = -100.0F;
    }

    // End of Saturate: '<S1>/Saturation1'

    // Sum: '<S1>/Sum1' incorporates:
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_FilterDifferentiatorTF_f = (rtb_FilterDifferentiatorTF_f - rtb_Cos) /
      rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F + 5.0F;

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

    rtb_algDD_o2_l = (rtb_Product - rtb_Cos) /
      rtu_ConfigurationParameters->motorconfig.Vcc * 100.0F + 5.0F;

    // Saturate: '<S1>/Saturation'
    if (rtb_algDD_o2_l > 100.0F) {
      rtb_algDD_o2_l = 100.0F;
    } else if (rtb_algDD_o2_l < 0.0F) {
      rtb_algDD_o2_l = 0.0F;
    }

    // Sum: '<S1>/Sum1' incorporates:
    //   Constant: '<S1>/Constant2'
    //   Gain: '<S1>/Gain1'
    //   MinMax: '<S1>/Min1'
    //   Product: '<S1>/Divide'
    //   Sum: '<S1>/Sum4'

    rtb_Sin = (rtb_Sin - rtb_Cos) / rtu_ConfigurationParameters->motorconfig.Vcc
      * 100.0F + 5.0F;

    // Saturate: '<S1>/Saturation'
    if (rtb_Sin > 100.0F) {
      rtb_Sin = 100.0F;
    } else if (rtb_Sin < 0.0F) {
      rtb_Sin = 0.0F;
    }
  } else {
    rtb_PProdOut_k = 0.0F;
    rtb_FilterDifferentiatorTF_f = 0.0F;
    rtb_algDD_o2_l = 0.0F;
    rtb_Sin = 0.0F;
  }

  // End of Switch: '<S1>/Switch2'

  // BusCreator: '<S1>/Bus Creator1'
  rty_FOCOutputs->Iq_fbk.current = rtb_algDD_o2;

  // BusCreator: '<S1>/Bus Creator2'
  rty_FOCOutputs->Id_fbk.current = rtb_algDD_o1;

  // Saturate: '<S1>/Saturation2'
  if (rtu_ConfigurationParameters->estimationconfig.current_rms_lambda > 1.0F) {
    rtb_Product = 1.0F;
  } else if (rtu_ConfigurationParameters->estimationconfig.current_rms_lambda <
             0.0F) {
    rtb_Product = 0.0F;
  } else {
    rtb_Product =
      rtu_ConfigurationParameters->estimationconfig.current_rms_lambda;
  }

  // End of Saturate: '<S1>/Saturation2'
  control_foc_MovingRMS(rtb_algDD_o2, rtb_Product, &localB->MovingRMS,
                        &localDW->MovingRMS);

  // BusCreator: '<S1>/Bus Creator3'
  rty_FOCOutputs->Iq_rms.current = localB->MovingRMS.MovingRMS;
  control_foc_MovingRMS(rtb_algDD_o1, rtb_Product, &localB->MovingRMS1,
                        &localDW->MovingRMS1);

  // BusCreator: '<S1>/Bus Creator4'
  rty_FOCOutputs->Id_rms.current = localB->MovingRMS1.MovingRMS;

  // BusCreator: '<S1>/Bus Creator'
  rty_FOCOutputs->Vq = rtb_PProdOut_k;
  rty_FOCOutputs->Vabc[0] = rtb_FilterDifferentiatorTF_f;
  rty_FOCOutputs->Vabc[1] = rtb_algDD_o2_l;
  rty_FOCOutputs->Vabc[2] = rtb_Sin;

  // Sum: '<S1>/Sum3'
  localB->Sum3 = rtb_algDD_o1_g - rtb_Add;
}

// Update for atomic system: '<Root>/FOC inner loop'
void FOCInnerLoop_Update(const ControlOuterOutputs *rtu_OuterOutputs,
  B_FOCInnerLoop_T *localB, DW_FOCInnerLoop_T *localDW)
{
  // Update for DiscreteTransferFcn: '<S95>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_states = localDW->FilterDifferentiatorTF_tmp;

  // Update for UnitDelay: '<S1>/Unit Delay'
  localDW->UnitDelay_DSTATE = localB->Sum3;

  // Update for DiscreteIntegrator: '<S102>/Integrator'
  localDW->Integrator_DSTATE = 1.82857148E-5F * localB->SumI1 +
    localB->Integrator;
  localDW->Integrator_PrevResetState = static_cast<int8_T>
    (rtu_OuterOutputs->pid_reset);

  // Update for DiscreteTransferFcn: '<S41>/Filter Differentiator TF'
  localDW->FilterDifferentiatorTF_states_k =
    localDW->FilterDifferentiatorTF_tmp_c;

  // Update for DiscreteIntegrator: '<S48>/Integrator' incorporates:
  //   DiscreteIntegrator: '<S102>/Integrator'

  localDW->Integrator_DSTATE_o = 1.82857148E-5F * localB->Switch +
    localB->Integrator_j;
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
