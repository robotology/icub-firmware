//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_RX.h
//
// Code generated for Simulink model 'SupervisorFSM_RX'.
//
// Model version                  : 3.158
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Fri Apr  1 11:35:50 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_SupervisorFSM_RX_h_
#define RTW_HEADER_SupervisorFSM_RX_h_
#include <cmath>
#include "rtwtypes.h"
#include "SupervisorFSM_RX_types.h"

// Shared type includes
#include "multiword_types.h"

// user code (top of header file)
#include "rtw_enable_disable_motors.h"

// Block states (default storage) for model 'SupervisorFSM_RX'
#ifndef SupervisorFSM_RX_MDLREF_HIDE_CHILD_

struct DW_SupervisorFSM_RX_f_T {
  int32_T sfEvent;                     // '<Root>/SupervisorFSM_RX'
  BoardState BoardSt;                  // '<Root>/SupervisorFSM_RX'
  uint8_T is_active_c2_SupervisorFSM_RX;// '<Root>/SupervisorFSM_RX'
  uint8_T is_active_CAN_RX_HANDLER;    // '<Root>/SupervisorFSM_RX'
  uint8_T is_EVENT_DISPATCHER;         // '<Root>/SupervisorFSM_RX'
  uint8_T is_active_EVENT_DISPATCHER;  // '<Root>/SupervisorFSM_RX'
  uint8_T is_CONTROL_MODE_HANDLER;     // '<Root>/SupervisorFSM_RX'
  uint8_T is_active_CONTROL_MODE_HANDLER;// '<Root>/SupervisorFSM_RX'
  uint8_T is_LIMITS_HANDLER;           // '<Root>/SupervisorFSM_RX'
  uint8_T is_active_LIMITS_HANDLER;    // '<Root>/SupervisorFSM_RX'
  uint8_T is_STATE_HANDLER;            // '<Root>/SupervisorFSM_RX'
  uint8_T is_active_STATE_HANDLER;     // '<Root>/SupervisorFSM_RX'
  uint8_T is_active_FAULT_HANDLER;     // '<Root>/SupervisorFSM_RX'
  uint8_T is_OverCurrent;              // '<Root>/SupervisorFSM_RX'
  uint8_T is_active_OverCurrent;       // '<Root>/SupervisorFSM_RX'
  uint8_T is_FaultButtonPressed;       // '<Root>/SupervisorFSM_RX'
  uint8_T is_active_FaultButtonPressed;// '<Root>/SupervisorFSM_RX'
  boolean_T IsCurrentLimitSet;         // '<Root>/SupervisorFSM_RX'
  boolean_T isFaultButtonPressed;      // '<Root>/SupervisorFSM_RX'
  boolean_T isInOverCurrent;           // '<Root>/SupervisorFSM_RX'
  boolean_T EventsRx_control_mode_prev;// '<Root>/SupervisorFSM_RX'
  boolean_T EventsRx_control_mode_start;// '<Root>/SupervisorFSM_RX'
  boolean_T EventsRx_current_pid_prev; // '<Root>/SupervisorFSM_RX'
  boolean_T EventsRx_current_pid_start;// '<Root>/SupervisorFSM_RX'
  boolean_T ErrorsRx_event_prev;       // '<Root>/SupervisorFSM_RX'
  boolean_T ErrorsRx_event_start;      // '<Root>/SupervisorFSM_RX'
  boolean_T EventsRx_desired_current_prev;// '<Root>/SupervisorFSM_RX'
  boolean_T EventsRx_desired_current_start;// '<Root>/SupervisorFSM_RX'
  boolean_T EventsRx_current_limit_prev;// '<Root>/SupervisorFSM_RX'
  boolean_T EventsRx_current_limit_start;// '<Root>/SupervisorFSM_RX'
  boolean_T ExternalFlags_fault_button_prev;// '<Root>/SupervisorFSM_RX'
  boolean_T ExternalFlags_fault_button_star;// '<Root>/SupervisorFSM_RX'
};

#endif                                 //SupervisorFSM_RX_MDLREF_HIDE_CHILD_

#ifndef SupervisorFSM_RX_MDLREF_HIDE_CHILD_

// Real-time Model Data Structure
struct tag_RTM_SupervisorFSM_RX_T {
  const char_T **errorStatus;
};

#endif                                 //SupervisorFSM_RX_MDLREF_HIDE_CHILD_

#ifndef SupervisorFSM_RX_MDLREF_HIDE_CHILD_

struct MdlrefDW_SupervisorFSM_RX_T {
  RT_MODEL_SupervisorFSM_RX_T rtm;
};

#endif                                 //SupervisorFSM_RX_MDLREF_HIDE_CHILD_

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern ConfigurationParameters InitConfParams;// Variable: InitConfParams
                                                 //  Referenced by: '<Root>/SupervisorFSM_RX'

extern void SupervisorFSM_RX_Init(Flags *rty_Flags, Targets *rty_Targets,
  ConfigurationParameters *rty_ConfigurationParameters);
extern void SupervisorFSM_RX(const SensorsData *rtu_SensorsData, const
  ExternalFlags *rtu_ExternalFlags, const ControlOutputs *rtu_ControlOutputs,
  const BUS_MESSAGES_RX_MULTIPLE *rtu_MessagesRx, const BUS_EVENTS_RX_MULTIPLE
  *rtu_EventsRx, const BUS_CAN_RX_ERRORS_MULTIPLE *rtu_ErrorsRx, Flags
  *rty_Flags, Targets *rty_Targets, ConfigurationParameters
  *rty_ConfigurationParameters);

// Model reference registration function
extern void SupervisorFSM_RX_initialize(const char_T **rt_errorStatus);

#ifndef SupervisorFSM_RX_MDLREF_HIDE_CHILD_

extern MdlrefDW_SupervisorFSM_RX_T SupervisorFSM_RX_MdlrefDW;

#endif                                 //SupervisorFSM_RX_MDLREF_HIDE_CHILD_

#ifndef SupervisorFSM_RX_MDLREF_HIDE_CHILD_

// Block states (default storage)
extern DW_SupervisorFSM_RX_f_T SupervisorFSM_RX_DW;

#endif                                 //SupervisorFSM_RX_MDLREF_HIDE_CHILD_

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
