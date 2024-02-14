//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: filter_current.h
//
// Code generated for Simulink model 'filter_current'.
//
// Model version                  : 6.3
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Tue Feb 13 11:55:05 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_filter_current_h_
#define RTW_HEADER_filter_current_h_
#include "rtwtypes.h"
#include "filter_current_types.h"
#include <cstring>

// Block states (default storage) for model 'filter_current'
struct DW_filter_current_f_T {
  dsp_simulink_MedianFilter_filter_current_T obj;// '<Root>/Median Filter'
  boolean_T objisempty;                // '<Root>/Median Filter'
};

// Real-time Model Data Structure
struct tag_RTM_filter_current_T {
  const char_T **errorStatus;
};

struct MdlrefDW_filter_current_T {
  DW_filter_current_f_T rtdw;
  RT_MODEL_filter_current_T rtm;
};

// Model reference registration function
extern void filter_current_initialize(const char_T **rt_errorStatus,
  RT_MODEL_filter_current_T *const filter_current_M, DW_filter_current_f_T
  *localDW);
extern void filter_current_Init(DW_filter_current_f_T *localDW);
extern void filter_current(const ControlOutputs *rtu_ControlOutputs,
  MotorCurrent *rty_FilteredCurrent, DW_filter_current_f_T *localDW);
extern void filter_current_Term(DW_filter_current_f_T *localDW);

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
//  '<Root>' : 'filter_current'

#endif                                 // RTW_HEADER_filter_current_h_

//
// File trailer for generated code.
//
// [EOF]
//
