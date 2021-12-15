//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: estimation_velocity.h
//
// Code generated for Simulink model 'estimation_velocity'.
//
// Model version                  : 2.29
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Tue Dec 14 19:25:56 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_estimation_velocity_h_
#define RTW_HEADER_estimation_velocity_h_
#include <cmath>
#include <cstring>
#include <stddef.h>
#include "rtwtypes.h"
#include "estimation_velocity_types.h"
#include <stddef.h>
#include "mw_cmsis.h"
#include "rtGetInf.h"
#include "rt_nonfinite.h"

// Class declaration for model estimation_velocity
namespace amc_bldc_codegen
{
  class estimation_velocity
  {
    // public data and function members
   public:
    // Block states (default storage) for model 'estimation_velocity'
    struct DW_estimation_velocity_T {
      real32_T DelayLine_Buff[16];     // '<Root>/Delay Line'
      real32_T Delay_DSTATE[16];       // '<Root>/Delay'
      int32_T DelayLine_BUFF_OFFSET;   // '<Root>/Delay Line'
      uint32_T CircBufIdx;             // '<Root>/Delay'
      dsp_simulink_QRSolver_estimat_T obj;// '<S1>/QR Solver'
      boolean_T objisempty;            // '<S1>/QR Solver'
    };

    // Real-time Model Data Structure
    struct RT_MODEL_estimation_velocity_T {
      const char_T **errorStatus;
    };

    // model initialize function
    void initialize();

    // Initial conditions function
    void init();

    // model step function
    void step(const SensorsData &arg_SensorsData, EstimatedData &
              arg_EstimatedData);

    // Real-Time Model get method
    amc_bldc_codegen::estimation_velocity::RT_MODEL_estimation_velocity_T
      * getRTM();

    //member function to setup error status pointer
    void setErrorStatusPointer(const char_T **rt_errorStatus);

    // Block states
    DW_estimation_velocity_T estimation_velocity_DW;

    // Constructor
    estimation_velocity();

    // Destructor
    ~estimation_velocity();

    // private data and function members
   private:
    // private member function(s) for subsystem '<Root>/TmpModelReferenceSubsystem'
    real32_T estimation_velocity_xnrm2(int32_T n, const real32_T x[32], int32_T
      ix0);
    void estimation_velocity_xgeqp3(const real32_T A[32], real32_T b_A[32],
      real32_T tau[2], int32_T jpvt[2]);

    // Real-Time Model
    RT_MODEL_estimation_velocity_T estimation_velocity_M;
  };
}

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S2>/Check Signal Attributes' : Unused code path elimination
//  Block '<S3>/Check Signal Attributes' : Unused code path elimination
//  Block '<S4>/Check Signal Attributes' : Unused code path elimination
//  Block '<Root>/Manual Switch' : Eliminated due to constant selection input


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
