//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: supervisor.h
//
// Code generated for Simulink model 'supervisor'.
//
// Model version                  : 3.18
// Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
// C/C++ source code generated on : Fri May 30 15:27:33 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef supervisor_h_
#define supervisor_h_
#include "rtwtypes.h"
#include "supervisor_types.h"
#include "rtw_defines.h"

// user code (top of header file)
#include "rtw_enable_disable_motors.h"
#include "rtw_motor_config.h"

// Block states (default storage) for model 'supervisor'
struct DW_supervisor_f_T {
  real_T isFaultButtonPressed;         // '<Root>/Supervisor'
  real_T isInFault;                    // '<Root>/Supervisor'
  real_T newSetpoint;                  // '<Root>/Supervisor'
  int32_T sfEvent;                     // '<Root>/Supervisor'
  ControlModes requestedControlMode;   // '<Root>/Supervisor'
  uint8_T is_active_c3_supervisor;     // '<Root>/Supervisor'
  uint8_T is_active_FaultsManager;     // '<Root>/Supervisor'
  uint8_T is_active_HWFaults;          // '<Root>/Supervisor'
  uint8_T is_HWFaults;                 // '<Root>/Supervisor'
  uint8_T is_active_FaultButton;       // '<Root>/Supervisor'
  uint8_T is_FaultButton;              // '<Root>/Supervisor'
  uint8_T is_active_ControlModeHandler;// '<Root>/Supervisor'
  uint8_T is_ControlModeHandler;       // '<Root>/Supervisor'
  uint8_T is_active_InputsDispatcher;  // '<Root>/Supervisor'
  uint8_T is_InputsDispatcher;         // '<Root>/Supervisor'
  uint8_T is_active_TargetsManager;    // '<Root>/Supervisor'
  uint8_T is_TargetsManager;           // '<Root>/Supervisor'
  boolean_T areLimitsSet;              // '<Root>/Supervisor'
  boolean_T ExternalFlags_fault_button_prev;// '<Root>/Supervisor'
  boolean_T ExternalFlags_fault_button_start;// '<Root>/Supervisor'
};

struct MdlrefDW_supervisor_T {
  DW_supervisor_f_T rtdw;
};

extern void supervisor_Init(Targets *rty_targets, ActuatorConfiguration
  *rty_ConfigurationParameters, Flags *rty_Flags, DW_supervisor_f_T *localDW);
extern void supervisor(const ExternalFlags *rtu_ExternalFlags, const
  EstimatedData *rtu_EstimatedData, const FOCOutputs *rtu_ControlOutputs, const
  SensorsData *rtu_SensorsData, const ReceivedEvents
  rtu_ReceivedEvents[MAX_EVENTS_PER_TICK], const ActuatorConfiguration
  *rtu_InitConf, Targets *rty_targets, ActuatorConfiguration
  *rty_ConfigurationParameters, Flags *rty_Flags, DW_supervisor_f_T *localDW);

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
//  '<Root>' : 'supervisor'
//  '<S1>'   : 'supervisor/Supervisor'

#endif                                 // supervisor_h_

//
// File trailer for generated code.
//
// [EOF]
//
