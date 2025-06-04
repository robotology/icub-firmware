//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_foc.cpp
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 8.9
// Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
// C/C++ source code generated on : Wed Jun  4 17:58:12 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "control_foc.h"
#include "control_foc_types.h"
#include "FOCInnerLoop.h"

// System initialize for referenced model: 'control_foc'
void control_foc_Init(DW_control_foc_f_T *localDW)
{
  // SystemInitialize for Atomic SubSystem: '<Root>/FOC inner loop'
  FOCInnerLoop_Init(&localDW->FOCinnerloop);

  // End of SystemInitialize for SubSystem: '<Root>/FOC inner loop'
}

// Output and update for referenced model: 'control_foc'
void control_foc(const SensorsData *rtu_Sensors, const FOCSlowInputs
                 *rtu_FocSlowInputs, FOCOutputs *rty_FOCOutputs,
                 B_control_foc_c_T *localB, DW_control_foc_f_T *localDW,
                 ZCE_control_foc_T *localZCE)
{
  // Outputs for Atomic SubSystem: '<Root>/FOC inner loop'
  FOCInnerLoop(rtu_Sensors, &rtu_FocSlowInputs->actuator_configuration,
               &rtu_FocSlowInputs->targets,
               &rtu_FocSlowInputs->control_outer_outputs, rty_FOCOutputs,
               &localB->FOCinnerloop, &localDW->FOCinnerloop,
               &localZCE->FOCinnerloop);

  // End of Outputs for SubSystem: '<Root>/FOC inner loop'

  // Update for Atomic SubSystem: '<Root>/FOC inner loop'
  FOCInnerLoop_Update(&rtu_FocSlowInputs->control_outer_outputs,
                      &localB->FOCinnerloop, &localDW->FOCinnerloop);

  // End of Update for SubSystem: '<Root>/FOC inner loop'
}

// Model initialize function
void control_foc_initialize(ZCE_control_foc_T *localZCE)
{
  localZCE->FOCinnerloop.FilterDifferentiatorTF_Reset_ZCE = POS_ZCSIG;
  localZCE->FOCinnerloop.FilterDifferentiatorTF_Reset_ZCE_o = POS_ZCSIG;
}

//
// File trailer for generated code.
//
// [EOF]
//
