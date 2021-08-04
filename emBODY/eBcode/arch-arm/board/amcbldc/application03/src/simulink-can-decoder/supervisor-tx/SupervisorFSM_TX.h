//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: SupervisorFSM_TX.h
//
// Code generated for Simulink model 'SupervisorFSM_TX'.
//
// Model version                  : 2.43
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Wed Aug  4 14:13:07 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_SupervisorFSM_TX_h_
#define RTW_HEADER_SupervisorFSM_TX_h_
#include <cstring>
#include <stddef.h>
#include "rtwtypes.h"
#include "SupervisorFSM_TX_types.h"
#include <stddef.h>

// Class declaration for model SupervisorFSM_TX
class SupervisorFSM_TXModelClass {
  // public data and function members
 public:
  // Block signals for model 'SupervisorFSM_TX'
  struct B_SupervisorFSM_TX_T {
    BUS_MESSAGES_TX MessagesTx;        // '<Root>/Chart'
  };

  // Block states (default storage) for model 'SupervisorFSM_TX'
  struct DW_SupervisorFSM_TX_T {
    uint32_T ev_focEventCounter;       // '<Root>/Chart'
    uint8_T is_active_c3_SupervisorFSM_TX;// '<Root>/Chart'
  };

  // Real-time Model Data Structure
  struct RT_MODEL_SupervisorFSM_TX_T {
    const char_T **errorStatus;
  };

  // model step function
  void step(const real32_T *rtu_SensorsData_jointpositions_, const real32_T
            *rtu_SensorsData_motorsensors_om, const real32_T
            *rtu_SensorsData_motorsensors_cu, const MCControlModes
            *rtu_MessagesRx_control_mode_mod, real32_T
            *rty_MessagesTx_foc_current, real32_T *rty_MessagesTx_foc_position,
            real32_T *rty_MessagesTx_foc_velocity, boolean_T *rty_EventsTx_foc);

  // Initial conditions function
  void init(real32_T *rty_MessagesTx_foc_current, real32_T
            *rty_MessagesTx_foc_position, real32_T *rty_MessagesTx_foc_velocity);

  // Constructor
  SupervisorFSM_TXModelClass();

  // Destructor
  ~SupervisorFSM_TXModelClass();

  // Real-Time Model get method
  SupervisorFSM_TXModelClass::RT_MODEL_SupervisorFSM_TX_T * getRTM();

  //member function to setup error status pointer
  void setErrorStatusPointer(const char_T **rt_errorStatus);

  // private data and function members
 private:
  // Block signals
  B_SupervisorFSM_TX_T SupervisorFSM_TX_B;

  // Block states
  DW_SupervisorFSM_TX_T SupervisorFSM_TX_DW;

  // Real-Time Model
  RT_MODEL_SupervisorFSM_TX_T SupervisorFSM_TX_M;
};

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
//  '<Root>' : 'SupervisorFSM_TX'
//  '<S1>'   : 'SupervisorFSM_TX/Chart'

#endif                                 // RTW_HEADER_SupervisorFSM_TX_h_

//
// File trailer for generated code.
//
// [EOF]
//
