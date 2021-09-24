//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: SupervisorFSM_RX.h
//
// Code generated for Simulink model 'SupervisorFSM_RX'.
//
// Model version                  : 2.44
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Mon Sep 20 12:43:16 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_SupervisorFSM_RX_h_
#define RTW_HEADER_SupervisorFSM_RX_h_
#include <cstring>
#include <stddef.h>
#include "rtwtypes.h"
#include "SupervisorFSM_RX_types.h"
#include <stddef.h>

// Class declaration for model SupervisorFSM_RX
class SupervisorFSM_RXModelClass {
  // public data and function members
 public:
  // Block states (default storage) for model 'SupervisorFSM_RX'
  struct DW_SupervisorFSM_RX_T {
    int32_T sfEvent;                   // '<Root>/SupervisorFSM_RX'
    uint16_T CurrLimit;                // '<Root>/SupervisorFSM_RX'
    uint8_T is_active_c3_SupervisorFSM_RX;// '<Root>/SupervisorFSM_RX'
    uint8_T is_CONTROL_MODE_HANDLER;   // '<Root>/SupervisorFSM_RX'
    uint8_T is_active_CONTROL_MODE_HANDLER;// '<Root>/SupervisorFSM_RX'
    uint8_T is_Not_Configured;         // '<Root>/SupervisorFSM_RX'
    uint8_T is_Motor_ON;               // '<Root>/SupervisorFSM_RX'
    uint8_T is_Motor_OFF;              // '<Root>/SupervisorFSM_RX'
    uint8_T is_FAULT_HANDLER;          // '<Root>/SupervisorFSM_RX'
    uint8_T is_active_FAULT_HANDLER;   // '<Root>/SupervisorFSM_RX'
    uint8_T is_CAN_RX_HANDLER;         // '<Root>/SupervisorFSM_RX'
    uint8_T is_active_CAN_RX_HANDLER;  // '<Root>/SupervisorFSM_RX'
    boolean_T ErrorsRx_event_prev;     // '<Root>/SupervisorFSM_RX'
    boolean_T ErrorsRx_event_start;    // '<Root>/SupervisorFSM_RX'
    boolean_T EventsRx_control_mode_prev;// '<Root>/SupervisorFSM_RX'
    boolean_T EventsRx_control_mode_start;// '<Root>/SupervisorFSM_RX'
    boolean_T EventsRx_current_limit_prev;// '<Root>/SupervisorFSM_RX'
    boolean_T EventsRx_current_limit_start;// '<Root>/SupervisorFSM_RX'
    boolean_T EventsRx_desired_current_prev;// '<Root>/SupervisorFSM_RX'
    boolean_T EventsRx_desired_current_start;// '<Root>/SupervisorFSM_RX'
  };

  // Real-time Model Data Structure
  struct RT_MODEL_SupervisorFSM_RX_T {
    const char_T **errorStatus;
  };

  // model step function
  void step(const InternalMessages &arg_Internal_Messages, const MotorSensors &
            arg_MotorSensors, const BUS_EVENTS_RX &arg_EventsRx, const
            BUS_MESSAGES_RX &arg_MessagesRx, const BUS_CAN_RX_ERRORS &
            arg_ErrorsRx, Flags &arg_Flags, Targets &arg_Targets);

  // Initial conditions function
  void init(Flags *arg_Flags, Targets *arg_Targets);

  // Constructor
  SupervisorFSM_RXModelClass();

  // Destructor
  ~SupervisorFSM_RXModelClass();

  // Real-Time Model get method
  SupervisorFSM_RXModelClass::RT_MODEL_SupervisorFSM_RX_T * getRTM();

  //member function to setup error status pointer
  void setErrorStatusPointer(const char_T **rt_errorStatus);

  // private data and function members
 private:
  // Block states
  DW_SupervisorFSM_RX_T SupervisorFSM_RX_DW;

  // Real-Time Model
  RT_MODEL_SupervisorFSM_RX_T SupervisorFSM_RX_M;

  // private member function(s) for subsystem '<Root>/TmpModelReferenceSubsystem'
  void Supervisor_CONTROL_MODE_HANDLER(const InternalMessages
    *arg_Internal_Messages, const MotorSensors *arg_MotorSensors, const
    BUS_MESSAGES_RX *arg_MessagesRx, Flags *arg_Flags);
  void SupervisorFSM_RX_FAULT_HANDLER(const InternalMessages
    *arg_Internal_Messages, const MotorSensors *arg_MotorSensors, const
    BUS_MESSAGES_RX *arg_MessagesRx, Flags *arg_Flags);
  ControlModes SupervisorFSM_RX_convert(MCControlModes mccontrolmode);
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
//  '<Root>' : 'SupervisorFSM_RX'
//  '<S1>'   : 'SupervisorFSM_RX/SupervisorFSM_RX'

#endif                                 // RTW_HEADER_SupervisorFSM_RX_h_

//
// File trailer for generated code.
//
// [EOF]
//
