//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: filter_current.h
//
// Code generated for Simulink model 'filter_current'.
//
// Model version                  : 3.7
// Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
// C/C++ source code generated on : Tue Aug  9 15:19:10 2022
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

// Block states (default storage) for model 'filter_current'
#ifndef filter_current_MDLREF_HIDE_CHILD_

struct DW_filter_current_f_T {
  dsp_MedianFilter_filter_curre_T obj; // '<Root>/Median Filter'
  boolean_T objisempty;                // '<Root>/Median Filter'
};

#endif                                 //filter_current_MDLREF_HIDE_CHILD_

#ifndef filter_current_MDLREF_HIDE_CHILD_

// Real-time Model Data Structure
struct tag_RTM_filter_current_T {
  const char_T **errorStatus;
};

#endif                                 //filter_current_MDLREF_HIDE_CHILD_

#ifndef filter_current_MDLREF_HIDE_CHILD_

struct MdlrefDW_filter_current_T {
  RT_MODEL_filter_current_T rtm;
};

#endif                                 //filter_current_MDLREF_HIDE_CHILD_

extern void filter_current_Init(void);
extern void filter_current(const ControlOutputs *rtu_ControlOutputs,
  MotorCurrent *rty_FilteredCurrent);
extern void filter_current_Term(void);

// Model reference registration function
extern void filter_current_initialize(const char_T **rt_errorStatus);

#ifndef filter_current_MDLREF_HIDE_CHILD_

extern MdlrefDW_filter_current_T filter_current_MdlrefDW;

#endif                                 //filter_current_MDLREF_HIDE_CHILD_

#ifndef filter_current_MDLREF_HIDE_CHILD_

// Block states (default storage)
extern DW_filter_current_f_T filter_current_DW;

#endif                                 //filter_current_MDLREF_HIDE_CHILD_

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
