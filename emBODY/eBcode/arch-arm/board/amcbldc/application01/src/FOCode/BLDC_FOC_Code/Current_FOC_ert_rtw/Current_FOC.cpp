//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: Current_FOC.cpp
//
// Code generated for Simulink model 'Current_FOC'.
//
// Model version                  : 2.15
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Sun May 16 18:18:18 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "Current_FOC.h"
#include "Current_FOC_private.h"

// Model step function
void Current_FOCModelClass::step()
{
  real32_T rtb_Cos;
  real32_T rtb_Gain1;
  real32_T rtb_Sin;
  real32_T rtb_algDD_o1;
  real32_T rtb_algDD_o2;
  real32_T rtb_algDD_o2_o;
  real32_T u0;
  real32_T u0_0;

  // Outputs for Atomic SubSystem: '<Root>/Current_FOC'
  // Product: '<S1>/Product' incorporates:
  //   Constant: '<S1>/Constant'
  //   Inport: '<Root>/Angle_fbk'

  rtb_Gain1 = Current_FOC_U.Angle_fbk * 7.0F;

  // Trigonometry: '<S1>/Cos'
  rtb_Cos = arm_cos_f32(rtb_Gain1);

  // Trigonometry: '<S1>/Sin'
  rtb_Sin = arm_sin_f32(rtb_Gain1);

  // Outputs for Atomic SubSystem: '<S1>/Clarke Transform'
  // AlgorithmDescriptorDelegate generated from: '<S2>/a16' incorporates:
  //   Inport: '<Root>/Iabc_fbk'

  arm_clarke_f32(Current_FOC_U.Iabc_fbk[0], Current_FOC_U.Iabc_fbk[1],
                 &rtb_Gain1, &rtb_algDD_o2_o);

  // End of Outputs for SubSystem: '<S1>/Clarke Transform'

  // Outputs for Atomic SubSystem: '<S1>/Park Transform'
  // AlgorithmDescriptorDelegate generated from: '<S7>/a16'
  arm_park_f32(rtb_Gain1, rtb_algDD_o2_o, &rtb_algDD_o1, &rtb_algDD_o2, rtb_Sin,
               rtb_Cos);

  // End of Outputs for SubSystem: '<S1>/Park Transform'

  // Gain: '<S1>/Gain1'
  rtb_Gain1 = -rtb_algDD_o1;

  // Sum: '<S1>/Subtract' incorporates:
  //   Inport: '<Root>/I_ref'

  rtb_algDD_o2 = Current_FOC_U.I_ref - rtb_algDD_o2;

  // Sum: '<S48>/Sum' incorporates:
  //   Constant: '<S1>/P'
  //   DiscreteIntegrator: '<S39>/Integrator'
  //   Gain: '<S1>/Gain1'
  //   Product: '<S44>/PProd Out'

  u0 = -rtb_algDD_o1 * 20.0F + Current_FOC_DW.Integrator_DSTATE;

  // Sum: '<S97>/Sum' incorporates:
  //   Constant: '<S1>/P'
  //   DiscreteIntegrator: '<S88>/Integrator'
  //   Product: '<S93>/PProd Out'

  u0_0 = rtb_algDD_o2 * 20.0F + Current_FOC_DW.Integrator_DSTATE_o;

  // Saturate: '<S95>/Saturation'
  if (u0_0 > 12.0F) {
    u0_0 = 12.0F;
  } else if (u0_0 < -12.0F) {
    u0_0 = -12.0F;
  }

  // End of Saturate: '<S95>/Saturation'

  // Sum: '<S1>/Sum' incorporates:
  //   Gain: '<S1>/Gain2'
  //   Gain: '<S1>/Gain3'
  //   Inport: '<Root>/I_ref'
  //   Inport: '<Root>/Speed_fbk'
  //   Sum: '<S1>/Add'

  u0_0 += 12.95F * Current_FOC_U.I_ref + 0.0022466667F * Current_FOC_U.Speed_fbk;

  // Saturate: '<S46>/Saturation'
  if (u0 > 12.0F) {
    u0 = 12.0F;
  } else if (u0 < -12.0F) {
    u0 = -12.0F;
  }

  // End of Saturate: '<S46>/Saturation'

  // Saturate: '<S1>/Saturation'
  if (u0_0 > 12.0F) {
    u0_0 = 12.0F;
  } else if (u0_0 < -12.0F) {
    u0_0 = -12.0F;
  }

  // End of Saturate: '<S1>/Saturation'

  // Outputs for Atomic SubSystem: '<S1>/Inverse Park Transform'
  // AlgorithmDescriptorDelegate generated from: '<S5>/a16'
  arm_inv_park_f32(u0, u0_0, &rtb_algDD_o2_o, &rtb_algDD_o1, rtb_Sin, rtb_Cos);

  // End of Outputs for SubSystem: '<S1>/Inverse Park Transform'

  // Gain: '<S4>/one_by_two'
  rtb_Cos = 0.5F * rtb_algDD_o2_o;

  // Gain: '<S4>/sqrt3_by_two'
  rtb_Sin = 0.866025388F * rtb_algDD_o1;

  // Update for DiscreteIntegrator: '<S39>/Integrator' incorporates:
  //   Constant: '<S1>/I'
  //   Product: '<S36>/IProd Out'

  Current_FOC_DW.Integrator_DSTATE += rtb_Gain1 * 10000.0F * 3.75E-5F;

  // Update for DiscreteIntegrator: '<S88>/Integrator' incorporates:
  //   Constant: '<S1>/I'
  //   Product: '<S85>/IProd Out'

  Current_FOC_DW.Integrator_DSTATE_o += rtb_algDD_o2 * 10000.0F * 3.75E-5F;

  // Outport: '<Root>/PWMabc_out' incorporates:
  //   Gain: '<S1>/Gain5'
  //   Sum: '<S4>/add_b'
  //   Sum: '<S4>/add_c'

  Current_FOC_Y.PWMabc_out[0] = 0.0416666679F * rtb_algDD_o2_o;
  Current_FOC_Y.PWMabc_out[1] = (rtb_Sin - rtb_Cos) * 0.0416666679F;
  Current_FOC_Y.PWMabc_out[2] = ((0.0F - rtb_Cos) - rtb_Sin) * 0.0416666679F;

  // End of Outputs for SubSystem: '<Root>/Current_FOC'
}

// Model initialize function
void Current_FOCModelClass::initialize()
{
  // (no initialization code required)
}

// Model terminate function
void Current_FOCModelClass::terminate()
{
  // (no terminate code required)
}

// Constructor
Current_FOCModelClass::Current_FOCModelClass() :
  Current_FOC_DW(),
  Current_FOC_U(),
  Current_FOC_Y(),
  Current_FOC_M()
{
  // Currently there is no constructor body generated.
}

// Destructor
Current_FOCModelClass::~Current_FOCModelClass()
{
  // Currently there is no destructor body generated.
}

// Real-Time Model get method
Current_FOCModelClass::RT_MODEL_Current_FOC_T * Current_FOCModelClass::getRTM()
{
  return (&Current_FOC_M);
}

//
// File trailer for generated code.
//
// [EOF]
//
