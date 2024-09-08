//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: TorqueEstimator.h
//
// Code generated for Simulink model 'TorqueEstimator'.
//
// Model version                  : 2.98
// Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
// C/C++ source code generated on : Sun Sep  8 21:32:31 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#ifndef TorqueEstimator_h_
#define TorqueEstimator_h_
#include <cmath>
#include "rtwtypes.h"

// Macros for accessing real-time model data structure
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

// Class declaration for model TorqueEstimator
class TorqueEstimator final
{
  // public data and function members
 public:
  // Block signals and states (default storage) for system '<Root>/TorqueEstimator' 
  struct DW_TorqueEstimator {
    real_T z;                          // '<Root>/TorqueEstimator'
  };

  // Block signals and states (default storage) for system '<Root>'
  struct DW {
    DW_TorqueEstimator sf_TorqueEstimator;// '<Root>/TorqueEstimator'
  };

  // External inputs (root inport signals with default storage)
  struct ExtU {
    real_T Current;                    // '<Root>/Current'
    real_T Velocity;                   // '<Root>/Velocity'
    real_T Km;                         // '<Root>/Km'
    real_T Kw;                         // '<Root>/Kw'
    real_T S0;                         // '<Root>/S0'
    real_T S1;                         // '<Root>/S1'
    real_T Vth;                        // '<Root>/Vth'
    real_T Fc;                         // '<Root>/Fc'
    real_T Fs;                         // '<Root>/Fs'
  };

  // External outputs (root outports fed by signals with default storage)
  struct ExtY {
    real_T Torque;                     // '<Root>/Torque'
    real_T Motor;                      // '<Root>/Motor'
    real_T Friction;                   // '<Root>/Friction'
  };

  // Real-time Model Data Structure
  struct RT_MODEL {
    const char_T * volatile errorStatus;
  };

  // Copy Constructor
  TorqueEstimator(TorqueEstimator const&) = delete;

  // Assignment Operator
  TorqueEstimator& operator= (TorqueEstimator const&) & = delete;

  // Move Constructor
  TorqueEstimator(TorqueEstimator &&) = delete;

  // Move Assignment Operator
  TorqueEstimator& operator= (TorqueEstimator &&) = delete;

  // Real-Time Model get method
  TorqueEstimator::RT_MODEL * getRTM();

  // External inputs
  ExtU rtU;

  // External outputs
  ExtY rtY;

  // model initialize function
  static void initialize();

  // model step function
  void step();

  // Constructor
  TorqueEstimator();

  // Destructor
  ~TorqueEstimator();

  // private data and function members
 private:
  // Block states
  DW rtDW;

  // private member function(s) for subsystem '<Root>/TorqueEstimator'
  static void TorqueEstimator_l(real_T rtu_Current, real_T rtu_Velocity, real_T
    rtu_Km, real_T rtu_Kw, real_T rtu_S0, real_T rtu_S1, real_T rtu_Vth, real_T
    rtu_Fc, real_T rtu_Fs, real_T *rty_Torque, real_T *rty_Motor, real_T
    *rty_Friction, DW_TorqueEstimator *localDW);

  // Real-Time Model
  RT_MODEL rtM;
};

//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Note that this particular code originates from a subsystem build,
//  and has its own system numbers different from the parent model.
//  Refer to the system hierarchy for this subsystem below, and use the
//  MATLAB hilite_system command to trace the generated code back
//  to the parent model.  For example,
//
//  hilite_system('torque_est_code_source/TorqueEstimator')    - opens subsystem torque_est_code_source/TorqueEstimator
//  hilite_system('torque_est_code_source/TorqueEstimator/Kp') - opens and selects block Kp
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'torque_est_code_source'
//  '<S1>'   : 'torque_est_code_source/TorqueEstimator'

#endif                                 // TorqueEstimator_h_

//
// File trailer for generated code.
//
// [EOF]
//
