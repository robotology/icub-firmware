//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: control_foc.cpp
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 1.128
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Mon Sep 20 12:43:59 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "control_foc.h"
#include "control_foc_private.h"

// Output and update for referenced model: 'control_foc'
void control_focModelClass::control_foc_ISR(const boolean_T *rtu_Flags_PID_reset,
  const real32_T *rtu_Config_motorconfig_Kp, const real32_T
  *rtu_Config_motorconfig_Ki, const real32_T *rtu_Config_motorconfig_Kbemf,
  const real32_T *rtu_Config_motorconfig_Rphase, const real32_T
  *rtu_Config_motorconfig_Vmax, const real32_T *rtu_Config_motorconfig_Vcc,
  const real32_T rtu_Sensors_motorsensors_Iabc[3], const real32_T
  *rtu_Sensors_motorsensors_angl_k, const real32_T
  *rtu_Sensors_motorsensors_omeg_k, const real32_T
  *rtu_Targets_motorcurrent_curr_k, const real32_T
  *rtu_Targets_motorvoltage_volt_e, const boolean_T *rtu_OuterOutputs_vel_en,
  const boolean_T *rtu_OuterOutputs_cur_en, const boolean_T
  *rtu_OuterOutputs_out_en, const real32_T *rtu_OuterOutputs_motorcurrent_p,
  uint16_T rty_Vabc_PWM_ticks[3], real32_T *rty_Iq_fbk_current)
{
  // RootInportFunctionCallGenerator generated from: '<Root>/ISR' incorporates:
  //   SubSystem: '<Root>/FOC inner loop'

  FOCInnerLoop(rtu_OuterOutputs_motorcurrent_p, rtu_Targets_motorcurrent_curr_k,
               rtu_Targets_motorvoltage_volt_e, rtu_Flags_PID_reset,
               rtu_OuterOutputs_cur_en, rtu_OuterOutputs_vel_en,
               rtu_OuterOutputs_out_en, rtu_Sensors_motorsensors_angl_k,
               rtu_Sensors_motorsensors_Iabc, rtu_Config_motorconfig_Rphase,
               rtu_Config_motorconfig_Kbemf, rtu_Config_motorconfig_Kp,
               rtu_Config_motorconfig_Vmax, rtu_Config_motorconfig_Ki,
               rtu_Config_motorconfig_Vcc, rtu_Sensors_motorsensors_omeg_k,
               rty_Vabc_PWM_ticks, rty_Iq_fbk_current);

  // End of Outputs for RootInportFunctionCallGenerator generated from: '<Root>/ISR' 
}

// Model initialize function
void control_focModelClass::initialize()
{
  // Registration code

  // initialize non-finites
  rt_InitInfAndNaN(sizeof(real_T));
}

// Constructor
control_focModelClass::control_focModelClass() :
  control_foc_DW()
{
  // Currently there is no constructor body generated.
}

// Destructor
control_focModelClass::~control_focModelClass()
{
  // Currently there is no destructor body generated.
}

// Real-Time Model get method
control_focModelClass::RT_MODEL_control_foc_T * control_focModelClass::getRTM()
{
  return (&control_foc_M);
}

// member function to setup error status pointer
void control_focModelClass::setErrorStatusPointer(const char_T **rt_errorStatus)
{
  rtmSetErrorStatusPointer((&control_foc_M), rt_errorStatus);
}

//
// File trailer for generated code.
//
// [EOF]
//
