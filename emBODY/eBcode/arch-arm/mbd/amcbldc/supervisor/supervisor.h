//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: supervisor.h
//
// Code generated for Simulink model 'supervisor'.
//
// Model version                  : 5.29
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Thu Oct  9 17:31:23 2025
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

// Includes for objects with custom storage classes
#include "rtw_defines.h"

// user code (top of header file)
#include "rtw_enable_disable_motors.h"
#include "rtw_motor_config.h"

// Block states (default storage) for model 'supervisor'
struct DW_supervisor_f_T {
  MotorConfigurationExternal temp_motor_config;// '<Root>/Supervisor'
  MotorConfigurationExtSet temp_config_set;// '<Root>/Supervisor'
  real_T isFaultButtonPressed;         // '<Root>/Supervisor'
  real_T isInFault;                    // '<Root>/Supervisor'
  real_T newSetpoint;                  // '<Root>/Supervisor'
  real_T offset;                       // '<Root>/Supervisor'
  real_T condition;                    // '<Root>/Supervisor'
  real_T count;                        // '<Root>/Supervisor'
  real_T finish;                       // '<Root>/Supervisor'
  real_T p_index;                      // '<Root>/Supervisor'
  real_T talign;                       // '<Root>/Supervisor'
  real_T tzeroalign;                   // '<Root>/Supervisor'
  real_T v_threshold;                  // '<Root>/Supervisor'
  real_T trajectoryVelocity;           // '<Root>/Supervisor'
  real32_T diff_index;                 // '<Root>/Supervisor'
  int32_T sfEvent;                     // '<Root>/Supervisor'
  uint32_T temporalCounter_i1;         // '<Root>/Supervisor'
  ControlModes requestedControlMode;   // '<Root>/Supervisor'
  uint8_T motor_id_to_configure;       // '<Root>/Supervisor'
  uint8_T is_active_c2_supervisor;     // '<Root>/Supervisor'
  uint8_T is_active_FaultsManager;     // '<Root>/Supervisor'
  uint8_T is_active_HWFaults;          // '<Root>/Supervisor'
  uint8_T is_HWFaults;                 // '<Root>/Supervisor'
  uint8_T is_active_FaultButton;       // '<Root>/Supervisor'
  uint8_T is_FaultButton;              // '<Root>/Supervisor'
  uint8_T is_active_ControlModeHandler;// '<Root>/Supervisor'
  uint8_T is_ControlModeHandler;       // '<Root>/Supervisor'
  uint8_T is_Calibration;              // '<Root>/Supervisor'
  uint8_T is_active_InputsDispatcher;  // '<Root>/Supervisor'
  uint8_T is_InputsDispatcher;         // '<Root>/Supervisor'
  uint8_T is_active_TargetsManager;    // '<Root>/Supervisor'
  uint8_T is_TargetsManager;           // '<Root>/Supervisor'
  uint8_T is_active_ConfigurationManager;// '<Root>/Supervisor'
  uint8_T is_ConfigurationManager;     // '<Root>/Supervisor'
  uint8_T motor_id_to_configure_prev;  // '<Root>/Supervisor'
  uint8_T motor_id_to_configure_start; // '<Root>/Supervisor'
  boolean_T areLimitsSet;              // '<Root>/Supervisor'
  boolean_T param_is_set;              // '<Root>/Supervisor'
  boolean_T cfg_is_set;                // '<Root>/Supervisor'
  boolean_T ExternalFlags_fault_button_prev;// '<Root>/Supervisor'
  boolean_T ExternalFlags_fault_button_start;// '<Root>/Supervisor'
};

struct MdlrefDW_supervisor_T {
  DW_supervisor_f_T rtdw;
};

extern void supervisor_Init(Targets *rty_targets, ActuatorConfiguration
  *rty_ConfigurationParameters, Flags *rty_Flags, SensorsData
  *rty_SensorsDataCalibration, DW_supervisor_f_T *localDW);
extern void supervisor(const ExternalFlags *rtu_ExternalFlags, const
  EstimatedData *rtu_EstimatedData, const FOCOutputs *rtu_ControlOutputs, const
  SensorsData *rtu_SensorsData, const ReceivedEvents rtu_ReceivedEvents[4],
  const ActuatorConfiguration *rtu_InitConf, Targets *rty_targets,
  ActuatorConfiguration *rty_ConfigurationParameters, Flags *rty_Flags,
  SensorsData *rty_SensorsDataCalibration, DW_supervisor_f_T *localDW);

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
