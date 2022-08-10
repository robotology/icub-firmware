//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: estimation_velocity.h
//
// Code generated for Simulink model 'estimation_velocity'.
//
// Model version                  : 3.3
// Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
// C/C++ source code generated on : Tue Aug  9 15:19:01 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_estimation_velocity_h_
#define RTW_HEADER_estimation_velocity_h_
#include "rtwtypes.h"
#include "estimation_velocity_types.h"
#include "rtGetInf.h"

extern "C" {

#include "rt_nonfinite.h"

}
// Block states (default storage) for model 'estimation_velocity'
#ifndef estimation_velocity_MDLREF_HIDE_CHILD_

struct DW_estimation_velocity_f_T {
  real32_T DelayLine_Buff[15];         // '<Root>/Delay Line'
  real32_T Delay_DSTATE[16];           // '<Root>/Delay'
  int32_T DelayLine_BUFF_OFFSET;       // '<Root>/Delay Line'
  uint32_T CircBufIdx;                 // '<Root>/Delay'
  dsp_simulink_QRSolver_estimat_T obj; // '<S1>/QR Solver'
  boolean_T objisempty;                // '<S1>/QR Solver'
};

#endif                                 //estimation_velocity_MDLREF_HIDE_CHILD_

#ifndef estimation_velocity_MDLREF_HIDE_CHILD_

// Real-time Model Data Structure
struct tag_RTM_estimation_velocity_T {
  const char_T **errorStatus;
};

#endif                                 //estimation_velocity_MDLREF_HIDE_CHILD_

#ifndef estimation_velocity_MDLREF_HIDE_CHILD_

struct MdlrefDW_estimation_velocity_T {
  RT_MODEL_estimation_velocity_T rtm;
};

#endif                                 //estimation_velocity_MDLREF_HIDE_CHILD_

extern void estimation_velocity_Init(void);
extern void estimation_velocity(const SensorsData *rtu_SensorsData, const
  ConfigurationParameters *rtu_ConfigurationParameters, JointVelocities
  *rty_EstimatedVelocity);

// Model reference registration function
extern void estimation_velocity_initialize(const char_T **rt_errorStatus);

#ifndef estimation_velocity_MDLREF_HIDE_CHILD_

extern MdlrefDW_estimation_velocity_T estimation_velocity_MdlrefDW;

#endif                                 //estimation_velocity_MDLREF_HIDE_CHILD_

#ifndef estimation_velocity_MDLREF_HIDE_CHILD_

// Block states (default storage)
extern DW_estimation_velocity_f_T estimation_velocity_DW;

#endif                                 //estimation_velocity_MDLREF_HIDE_CHILD_

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S2>/Check Signal Attributes' : Unused code path elimination
//  Block '<S3>/Check Signal Attributes' : Unused code path elimination
//  Block '<S4>/Check Signal Attributes' : Unused code path elimination


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
//  '<Root>' : 'estimation_velocity'
//  '<S1>'   : 'estimation_velocity/Least Squares Polynomial Fit'
//  '<S2>'   : 'estimation_velocity/Least Squares Polynomial Fit/Check Signal Attributes'
//  '<S3>'   : 'estimation_velocity/Least Squares Polynomial Fit/Check Signal Attributes1'
//  '<S4>'   : 'estimation_velocity/Least Squares Polynomial Fit/Check Signal Attributes2'

#endif                                 // RTW_HEADER_estimation_velocity_h_

//
// File trailer for generated code.
//
// [EOF]
//
