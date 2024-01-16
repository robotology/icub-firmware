//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_foc.cpp
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
#include "control_foc.h"
#include "control_foc_types.h"
#include "FOCInnerLoop.h"
#include "control_foc_private.h"
#include <cstring>

extern "C"
{

#include "rt_nonfinite.h"

}

// System initialize for referenced model: 'control_foc'
void control_foc_Init(DW_control_foc_f_T *localDW)
{
  // SystemInitialize for Atomic SubSystem: '<Root>/FOC inner loop'
  FOCInnerLoop_Init(&localDW->FOCinnerloop);

  // End of SystemInitialize for SubSystem: '<Root>/FOC inner loop'
}

// Output and update for referenced model: 'control_foc'
void control_foc(const SensorsData *rtu_Sensors, const FOCSlowInputs
                 *rtu_FOCSlowInputs, ControlOutputs *rty_FOCOutputs,
                 B_control_foc_c_T *localB, DW_control_foc_f_T *localDW,
                 ZCE_control_foc_T *localZCE)
{
  // Outputs for Atomic SubSystem: '<Root>/FOC inner loop'
  FOCInnerLoop(&rtu_FOCSlowInputs->configurationparameters, rtu_Sensors,
               &rtu_FOCSlowInputs->estimateddata, &rtu_FOCSlowInputs->targets,
               &rtu_FOCSlowInputs->controlouteroutputs, rty_FOCOutputs,
               &localB->FOCinnerloop, &control_foc_ConstB.FOCinnerloop,
               &localDW->FOCinnerloop, &localZCE->FOCinnerloop);

  // End of Outputs for SubSystem: '<Root>/FOC inner loop'

  // Update for Atomic SubSystem: '<Root>/FOC inner loop'
  FOCInnerLoop_Update(&rtu_FOCSlowInputs->controlouteroutputs,
                      &localB->FOCinnerloop, &localDW->FOCinnerloop);

  // End of Update for SubSystem: '<Root>/FOC inner loop'
}

// Model initialize function
void control_foc_initialize(const char_T **rt_errorStatus,
  RT_MODEL_control_foc_T *const control_foc_M, B_control_foc_c_T *localB,
  DW_control_foc_f_T *localDW, ZCE_control_foc_T *localZCE)
{
  // Registration code

  // initialize non-finites
  rt_InitInfAndNaN(sizeof(real_T));

  // initialize error status
  rtmSetErrorStatusPointer(control_foc_M, rt_errorStatus);

  // block I/O
  (void) std::memset((static_cast<void *>(localB)), 0,
                     sizeof(B_control_foc_c_T));

  // states (dwork)
  (void) std::memset(static_cast<void *>(localDW), 0,
                     sizeof(DW_control_foc_f_T));
  localZCE->FOCinnerloop.FilterDifferentiatorTF_Reset_ZCE = POS_ZCSIG;
  localZCE->FOCinnerloop.FilterDifferentiatorTF_Reset_ZCE_o = POS_ZCSIG;
}

//
// File trailer for generated code.
//
// [EOF]
//
