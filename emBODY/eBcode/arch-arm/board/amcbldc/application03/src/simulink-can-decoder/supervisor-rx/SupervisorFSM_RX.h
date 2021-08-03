//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: SupervisorFSM_RX.h
//
// Code generated for Simulink model 'SupervisorFSM_RX'.
//
// Model version                  : 2.33
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Tue Jul 20 13:35:15 2021
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
  // Block signals for model 'SupervisorFSM_RX'
  struct B_SupervisorFSM_RX_T {
    MotorSensors BusConversion_InsertedFor_Chart;
    Targets Targets_n;                 // '<Root>/Chart'
    BUS_MESSAGES_RX BusConversion_InsertedFor_Cha_h;
    InternalMessages BusConversion_InsertedFor_Cha_i;
    Flags Flags_o;                     // '<Root>/Chart'
  };

  // Block states (default storage) for model 'SupervisorFSM_RX'
  struct DW_SupervisorFSM_RX_T {
    int32_T sfEvent;                   // '<Root>/Chart'
    uint16_T CurrLimit;                // '<Root>/Chart'
    uint8_T is_active_c3_SupervisorFSM_RX;// '<Root>/Chart'
    uint8_T is_CONTROL_MODE_HANDLER;   // '<Root>/Chart'
    uint8_T is_active_CONTROL_MODE_HANDLER;// '<Root>/Chart'
    uint8_T is_Not_Configured;         // '<Root>/Chart'
    uint8_T is_Motor_ON;               // '<Root>/Chart'
    uint8_T is_Motor_OFF;              // '<Root>/Chart'
    uint8_T is_FAULT_HANDLER;          // '<Root>/Chart'
    uint8_T is_active_FAULT_HANDLER;   // '<Root>/Chart'
    uint8_T is_CAN_RX_HANDLER;         // '<Root>/Chart'
    uint8_T is_active_CAN_RX_HANDLER;  // '<Root>/Chart'
  };

  // Real-time Model Data Structure
  struct RT_MODEL_SupervisorFSM_RX_T {
    const char_T **errorStatus;
  };

  // model step function
  void step(const BoardState *rtu_InternalMessages_State, const BoardCommand
            *rtu_InternalMessages_Command, const real32_T rtu_MotorSensors_Iabc
            [3], const real32_T *rtu_MotorSensors_angle, const real32_T
            *rtu_MotorSensors_omega, const real32_T
            *rtu_MotorSensors_temperature, const real32_T
            *rtu_MotorSensors_voltage, const real32_T
            *rtu_MotorSensors_threshold_curr, const real32_T
            *rtu_MotorSensors_threshold_cu_k, const real32_T
            *rtu_MotorSensors_threshold_volt, const real32_T
            *rtu_MotorSensors_threshold_vo_k, const real32_T
            *rtu_MotorSensors_threshold_temp, const real32_T
            *rtu_MotorSensors_threshold_te_c, const real32_T
            *rtu_MotorSensors_current, const boolean_T *rtu_ErrorsRx_event,
            const boolean_T *rtu_EventsRx_control_mode, const boolean_T
            *rtu_EventsRx_current_limit, const boolean_T
            *rtu_EventsRx_desired_current, const boolean_T
            *rtu_MessagesRx_control_mode_mot, const MCControlModes
            *rtu_MessagesRx_control_mode_mod, const boolean_T
            *rtu_MessagesRx_current_limit_mo, const int16_T
            *rtu_MessagesRx_current_limit_no, const uint16_T
            *rtu_MessagesRx_current_limit_pe, const uint16_T
            *rtu_MessagesRx_current_limit_ov, const int16_T
            *rtu_MessagesRx_desired_current_, ControlModes
            *rty_Flags_control_mode, boolean_T *rty_Flags_PID_reset, real32_T
            *rty_Targets_jointpositions_posi, real32_T
            *rty_Targets_jointvelocities_vel, real32_T
            *rty_Targets_motorcurrent_curren, real32_T
            *rty_Targets_motorvoltage_voltag);

  // Initial conditions function
  void init(ControlModes *rty_Flags_control_mode, boolean_T *rty_Flags_PID_reset,
            real32_T *rty_Targets_jointpositions_posi, real32_T
            *rty_Targets_jointvelocities_vel, real32_T
            *rty_Targets_motorcurrent_curren, real32_T
            *rty_Targets_motorvoltage_voltag);

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
  // Block signals
  B_SupervisorFSM_RX_T SupervisorFSM_RX_B;

  // Block states
  DW_SupervisorFSM_RX_T SupervisorFSM_RX_DW;

  // Real-Time Model
  RT_MODEL_SupervisorFSM_RX_T SupervisorFSM_RX_M;

  // private member function(s) for subsystem '<Root>/TmpModelReferenceSubsystem'
  void Supervisor_CONTROL_MODE_HANDLER(void);
  void SupervisorFSM_RX_FAULT_HANDLER(void);
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
//  '<S1>'   : 'SupervisorFSM_RX/Chart'

#endif                                 // RTW_HEADER_SupervisorFSM_RX_h_

//
// File trailer for generated code.
//
// [EOF]
//
