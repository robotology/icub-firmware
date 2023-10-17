//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: thermal_model.h
//
// Code generated for Simulink model 'thermal_model'.
//
// Model version                  : 5.21
// Simulink Coder version         : 9.9 (R2023a) 19-Nov-2022
// C/C++ source code generated on : Mon Oct 16 10:08:48 2023
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_thermal_model_h_
#define RTW_HEADER_thermal_model_h_
#include "rtwtypes.h"
#include "thermal_model_types.h"
#include <cstring>

// Block states (default storage) for model 'thermal_model'
struct DW_thermal_model_f_T {
  real32_T DigitalFilter_states;       // '<Root>/DigitalFilter'
};

// Real-time Model Data Structure
struct tag_RTM_thermal_model_T {
  const char_T **errorStatus;
};

struct MdlrefDW_thermal_model_T {
  DW_thermal_model_f_T rtdw;
  RT_MODEL_thermal_model_T rtm;
};

// Model reference registration function
extern void thermal_model_initialize(const char_T **rt_errorStatus,
  RT_MODEL_thermal_model_T *const thermal_model_M, DW_thermal_model_f_T *localDW);
extern void thermal_model_Init(DW_thermal_model_f_T *localDW);
extern void thermal_model(const ControlOutputs *rtu_ControlOutputs, const
  ConfigurationParameters *rtu_ConfigurationParameters, MotorTemperature
  *rty_MotorTemperature, DW_thermal_model_f_T *localDW);

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S1>/Cast To Single1' : Eliminate redundant data type conversion
//  Block '<S1>/Cast To Single2' : Eliminate redundant data type conversion


//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Use the MATLAB hilite_system command to trace the generated code back
//  to the model.  For example,
//
//  hilite_system('<S3>')    - opens system 3
//  hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'thermal_model'
//  '<S1>'   : 'thermal_model/Compute coefficients'

#endif                                 // RTW_HEADER_thermal_model_h_

//
// File trailer for generated code.
//
// [EOF]
//
