//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: thermal_model.cpp
//
// Code generated for Simulink model 'thermal_model'.
//
// Model version                  : 6.3
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Thu Dec 14 10:44:18 2023
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "thermal_model.h"
#include "thermal_model_types.h"
#include "mw_cmsis.h"
#include "thermal_model_private.h"
#include "rtwtypes.h"
#include <cstring>

// System initialize for referenced model: 'thermal_model'
void thermal_model_Init(DW_thermal_model_f_T *localDW)
{
  // InitializeConditions for DiscreteFilter: '<Root>/DigitalFilter'
  localDW->DigitalFilter_states = 0.0F;
}

// Output and update for referenced model: 'thermal_model'
void thermal_model(const ControlOutputs *rtu_ControlOutputs, const
                   ConfigurationParameters *rtu_ConfigurationParameters,
                   MotorTemperature *rty_MotorTemperature, DW_thermal_model_f_T *
                   localDW)
{
  real32_T rtb_Product1[2];
  real32_T rtb_Square[2];
  real32_T rtb_Gain2;
  real32_T rtb_Sum1;
  real32_T rtb_Sum_a;

  // SignalConversion generated from: '<Root>/Square'
  rtb_Product1[0] = rtu_ControlOutputs->Iq_rms.current;
  rtb_Product1[1] = rtu_ControlOutputs->Id_rms.current;

  // Math: '<Root>/Square'
  mw_arm_mult_f32(&rtb_Product1[0], &rtb_Product1[0], &rtb_Square[0], 2U);

  // Gain: '<S1>/Gain2'
  rtb_Gain2 = 2.0F *
    rtu_ConfigurationParameters->motorconfig.thermal_time_constant;

  // Sum: '<Root>/Sum1'
  rtb_Sum1 = rtb_Square[0] + rtb_Square[1];

  // DiscreteFilter: '<Root>/DigitalFilter' incorporates:
  //   Constant: '<Root>/Ts'
  //   Product: '<S1>/Divide'
  //   Product: '<S1>/Divide1'
  //   Product: '<S1>/Product1'
  //   Sum: '<S1>/Sum'
  //   Sum: '<S1>/Sum1'

  rtb_Sum_a = thermal_model_ConstB.Product[0] *
    rtu_ConfigurationParameters->motorconfig.resistance *
    rtu_ConfigurationParameters->motorconfig.thermal_resistance / (rtb_Gain2 +
    0.01F) * rtb_Sum1 + localDW->DigitalFilter_states;
  localDW->DigitalFilter_states = thermal_model_ConstB.Product[1] *
    rtu_ConfigurationParameters->motorconfig.resistance *
    rtu_ConfigurationParameters->motorconfig.thermal_resistance / (rtb_Gain2 +
    0.01F) * rtb_Sum1 - (0.01F - rtb_Gain2) / (rtb_Gain2 + 0.01F) * rtb_Sum_a;

  // BusCreator: '<Root>/Bus Creator1' incorporates:
  //   Sum: '<Root>/Sum'

  rty_MotorTemperature->temperature = rtb_Sum_a +
    rtu_ConfigurationParameters->environment_temperature;
}

// Model initialize function
void thermal_model_initialize(const char_T **rt_errorStatus,
  RT_MODEL_thermal_model_T *const thermal_model_M, DW_thermal_model_f_T *localDW)
{
  // Registration code

  // initialize error status
  rtmSetErrorStatusPointer(thermal_model_M, rt_errorStatus);

  // states (dwork)
  (void) std::memset(static_cast<void *>(localDW), 0,
                     sizeof(DW_thermal_model_f_T));
}

//
// File trailer for generated code.
//
// [EOF]
//
