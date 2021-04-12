//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: adc2ft.h
//
// Code generated for Simulink model 'adc2ft'.
//
// Model version                  : 2.36
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Mon Apr 12 16:07:41 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#ifndef RTW_HEADER_adc2ft_h_
#define RTW_HEADER_adc2ft_h_
#include <cmath>
#include "rtwtypes.h"
//#include "rtw_continuous.h"
//#include "rtw_solver.h"

// Model Code Variants

// Macros for accessing real-time model data structure
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

// Class declaration for model adc2ft
namespace adc2ft_ns
{
  class adc2ft_class {
    // public data and function members
   public:
    // External inputs (root inport signals with default storage)
    struct ExtU {
      uint16_T adc[6];                 // '<Root>/adc'
    };

    // External outputs (root outports fed by signals with default storage)
    struct ExtY {
      int16_T ft_q15[6];               // '<Root>/ft_q15'
    };

    // Parameters (default storage)
    struct P {
      int16_T calibration_matrix[36];  // Variable: A
                                          //  Referenced by: '<Root>/Constant2'

      int16_T calibration_offsets[6];  // Variable: offsets
                                          //  Referenced by: '<Root>/Constant1'

    };

    // Real-time Model Data Structure
    struct RT_MODEL {
      const char_T * volatile errorStatus;
    };

    // Tunable parameters
    static P rtP;

    // External inputs
    ExtU rtU;

    // External outputs
    ExtY rtY;

    // model initialize function
    void initialize();

    // model step function
    void step();

    // Constructor
    adc2ft_class();

    // Destructor
    ~adc2ft_class();

    // Real-Time Model get method
    adc2ft_ns::adc2ft_class::RT_MODEL * getRTM();

    // private data and function members
   private:
    // Real-Time Model
    RT_MODEL rtM;
  };
}

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
//  '<Root>' : 'adc2ft'

#endif                                 // RTW_HEADER_adc2ft_h_

//
// File trailer for generated code.
//
// [EOF]
//
