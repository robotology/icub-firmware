//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_foc.cpp
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 3.10
// Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
// C/C++ source code generated on : Tue Aug  9 15:18:35 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "control_foc.h"
#include "control_foc_types.h"
#include "FOCInnerLoop.h"
#include "control_foc_private.h"

extern "C" {

#include "rt_nonfinite.h"

}
  MdlrefDW_control_foc_T control_foc_MdlrefDW;

// Block states (default storage)
DW_control_foc_f_T control_foc_DW;

// Previous zero-crossings (trigger) states
ZCE_control_foc_T control_foc_PrevZCX;

// System initialize for referenced model: 'control_foc'
void control_foc_Init(void)
{
  // SystemInitialize for Atomic SubSystem: '<Root>/FOC inner loop'
  FOCInnerLoop_Init();

  // End of SystemInitialize for SubSystem: '<Root>/FOC inner loop'
}

// Output and update for referenced model: 'control_foc'
void control_foc(const SensorsData *rtu_Sensors, const FOCSlowInputs
                 *rtu_FOCSlowInputs, ControlOutputs *rty_FOCOutputs)
{
  // Outputs for Atomic SubSystem: '<Root>/FOC inner loop'
  FOCInnerLoop(&rtu_FOCSlowInputs->flags,
               &rtu_FOCSlowInputs->configurationparameters, rtu_Sensors,
               &rtu_FOCSlowInputs->estimateddata, &rtu_FOCSlowInputs->targets,
               &rtu_FOCSlowInputs->controlouteroutputs, rty_FOCOutputs);

  // End of Outputs for SubSystem: '<Root>/FOC inner loop'
}

// Model initialize function
void control_foc_initialize(const char_T **rt_errorStatus)
{
  RT_MODEL_control_foc_T *const control_foc_M = &(control_foc_MdlrefDW.rtm);

  // Registration code

  // initialize non-finites
  rt_InitInfAndNaN(sizeof(real_T));

  // initialize error status
  rtmSetErrorStatusPointer(control_foc_M, rt_errorStatus);
  control_foc_PrevZCX.FilterDifferentiatorTF_Reset_ZC = POS_ZCSIG;
  control_foc_PrevZCX.FilterDifferentiatorTF_Reset__o = POS_ZCSIG;
}

//
// File trailer for generated code.
//
// [EOF]
//
