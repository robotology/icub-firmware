//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: thermal_model.cpp
//
// Code generated for Simulink model 'thermal_model'.
//
// Model version                  : 5.21
// Simulink Coder version         : 9.9 (R2023a) 19-Nov-2022
// C/C++ source code generated on : Tue Jun 27 10:19:07 2023
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "thermal_model.h"
#include "thermal_model_types.h"
#include "mw_cmsis.h"
#include "rtwtypes.h"
#include "thermal_model_private.h"
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
  real32_T rtb_Sum_a;
  real32_T rtb_Sum_p_tmp;

  // SignalConversion generated from: '<Root>/Square'
  rtb_Product1[0] = rtu_ControlOutputs->Iq_rms.current;
  rtb_Product1[1] = rtu_ControlOutputs->Id_rms.current;

  // Math: '<Root>/Square'
  mw_arm_mult_f32(&rtb_Product1[0], &rtb_Product1[0], &rtb_Square[0], 2U);

  // Gain: '<S1>/Gain2'
  rtb_Gain2 = 2.0F *
    rtu_ConfigurationParameters->motorconfig.thermal_time_constant;

  // DiscreteFilter: '<Root>/DigitalFilter' incorporates:
  //   Constant: '<Root>/Ts'
  //   Constant: '<S1>/Constant2'
  //   Product: '<S1>/Divide'
  //   Product: '<S1>/Divide1'
  //   Product: '<S1>/Product'
  //   Product: '<S1>/Product1'
  //   Sum: '<Root>/Sum1'
  //   Sum: '<S1>/Sum'
  //   Sum: '<S1>/Sum1'

  rtb_Sum_p_tmp = 0.01F * rtu_ConfigurationParameters->motorconfig.resistance *
    rtu_ConfigurationParameters->motorconfig.thermal_resistance / (rtb_Gain2 +
    0.01F) * (rtb_Square[0] + rtb_Square[1]);
  rtb_Sum_a = rtb_Sum_p_tmp + localDW->DigitalFilter_states;
  localDW->DigitalFilter_states = rtb_Sum_p_tmp - (0.01F - rtb_Gain2) /
    (rtb_Gain2 + 0.01F) * rtb_Sum_a;

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
