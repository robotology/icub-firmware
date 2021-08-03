//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: SupervisorFSM_RX.cpp
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
#include "SupervisorFSM_RX.h"
#include "SupervisorFSM_RX_private.h"

// Named constants for Chart: '<Root>/Chart'
const uint8_T Su_IN_Controller_Not_Configured = 1U;
const uint8_T Supe_IN_Hardware_Not_Configured = 2U;
const int32_T Superv_event_SetCurrentLimitEvt = 3;
const int32_T Superviso_event_FaultPressedEvt = 0;
const uint8_T SupervisorFSM_IN_Hardware_Fault = 1U;
const uint8_T SupervisorFSM_IN_Not_Configured = 3U;
const uint8_T SupervisorFSM_IN_SET_CURR_LIMIT = 3U;
const int32_T SupervisorFSM_RX_CALL_EVENT = -1;
const uint8_T SupervisorFSM_RX_IN_Command = 1U;
const uint8_T SupervisorFSM_RX_IN_Current = 2U;
const uint8_T SupervisorFSM_RX_IN_FAULT = 1U;
const uint8_T SupervisorFSM_RX_IN_Home = 1U;
const uint8_T SupervisorFSM_RX_IN_Idle = 2U;
const uint8_T SupervisorFSM_RX_IN_Motor_OFF = 1U;
const uint8_T SupervisorFSM_RX_IN_Motor_ON = 2U;
const uint8_T SupervisorFSM_RX_IN_NO_FAULT = 2U;
const uint8_T SupervisorFSM_RX_IN_Position = 3U;
const uint8_T SupervisorFSM_RX_IN_Torque = 4U;
const uint8_T SupervisorFSM_RX_IN_Velocity = 5U;
const uint8_T SupervisorFSM_RX_IN_Voltage = 6U;
const int32_T SupervisorFSM__event_HwFaultEvt = 2;
const uint8_T SupervisorFS_IN_NO_ACTIVE_CHILD = 0U;
const int32_T SupervisorFS_event_ForceIdleEvt = 1;

// Function for Chart: '<Root>/Chart'
void SupervisorFSM_RXModelClass::Supervisor_CONTROL_MODE_HANDLER(void)
{
  int32_T b_previousEvent;
  switch (SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER) {
   case SupervisorFSM_RX_IN_Motor_OFF:
    if (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command ==
        BoardCommand_Reset) {
      SupervisorFSM_RX_DW.is_Motor_OFF = SupervisorFS_IN_NO_ACTIVE_CHILD;
      SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
        SupervisorFSM_IN_Not_Configured;
      SupervisorFSM_RX_DW.is_Not_Configured = Supe_IN_Hardware_Not_Configured;
      SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_NotConfigured;
    } else {
      switch (SupervisorFSM_RX_DW.is_Motor_OFF) {
       case SupervisorFSM_IN_Hardware_Fault:
        if (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command ==
            BoardCommand_ForceIdle) {
          b_previousEvent = SupervisorFSM_RX_DW.sfEvent;
          SupervisorFSM_RX_DW.sfEvent = SupervisorFS_event_ForceIdleEvt;
          if (SupervisorFSM_RX_DW.is_active_FAULT_HANDLER != 0U) {
            SupervisorFSM_RX_FAULT_HANDLER();
          }

          SupervisorFSM_RX_DW.sfEvent = b_previousEvent;
          if (SupervisorFSM_RX_DW.is_Motor_OFF == 1) {
            SupervisorFSM_RX_DW.is_Motor_OFF = SupervisorFSM_RX_IN_Idle;
            SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Idle;
          }
        }
        break;

       case SupervisorFSM_RX_IN_Idle:
        if (SupervisorFSM_RX_DW.sfEvent == SupervisorFSM__event_HwFaultEvt) {
          SupervisorFSM_RX_DW.is_Motor_OFF = SupervisorFSM_IN_Hardware_Fault;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_HwFaultCM;
        } else if ((SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command ==
                    BoardCommand_SetPosition) ||
                   (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command ==
                    BoardCommand_SetCurrent) ||
                   (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command ==
                    BoardCommand_SetVelocity) ||
                   (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command ==
                    BoardCommand_SetVoltage) ||
                   (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command ==
                    BoardCommand_SetTorque)) {
          SupervisorFSM_RX_DW.is_Motor_OFF = SupervisorFS_IN_NO_ACTIVE_CHILD;
          SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
            SupervisorFSM_RX_IN_Motor_ON;
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Command;
        }
        break;
      }
    }
    break;

   case SupervisorFSM_RX_IN_Motor_ON:
    if (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command ==
        BoardCommand_Reset) {
      SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFS_IN_NO_ACTIVE_CHILD;
      SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
        SupervisorFSM_IN_Not_Configured;
      SupervisorFSM_RX_DW.is_Not_Configured = Supe_IN_Hardware_Not_Configured;
      SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_NotConfigured;
    } else if (SupervisorFSM_RX_DW.sfEvent == SupervisorFSM__event_HwFaultEvt) {
      SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFS_IN_NO_ACTIVE_CHILD;
      SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
        SupervisorFSM_RX_IN_Motor_OFF;
      SupervisorFSM_RX_DW.is_Motor_OFF = SupervisorFSM_IN_Hardware_Fault;
      SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_HwFaultCM;
    } else if ((SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.State ==
                BoardState_FaultPressed) ||
               (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command ==
                BoardCommand_SetIdle) ||
               (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command ==
                BoardCommand_ForceIdle)) {
      SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFS_IN_NO_ACTIVE_CHILD;
      SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
        SupervisorFSM_RX_IN_Motor_OFF;
      SupervisorFSM_RX_DW.is_Motor_OFF = SupervisorFSM_RX_IN_Idle;
      SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Idle;
    } else if (SupervisorFSM_RX_DW.sfEvent == Superviso_event_FaultPressedEvt) {
      SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFS_IN_NO_ACTIVE_CHILD;
      SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
        SupervisorFSM_RX_IN_Motor_OFF;
      SupervisorFSM_RX_DW.is_Motor_OFF = SupervisorFSM_RX_IN_Idle;
      SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Idle;
    } else {
      switch (SupervisorFSM_RX_DW.is_Motor_ON) {
       case SupervisorFSM_RX_IN_Command:
        switch (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command) {
         case BoardCommand_SetPosition:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Position;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Position;
          break;

         case BoardCommand_SetVelocity:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Current;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Current;
          break;

         case BoardCommand_SetCurrent:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Velocity;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Velocity;
          break;

         case BoardCommand_SetVoltage:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Voltage;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Voltage;
          break;

         case BoardCommand_SetTorque:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Torque;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Torque;
          break;
        }
        break;

       case SupervisorFSM_RX_IN_Current:
        switch (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command) {
         case BoardCommand_SetVelocity:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Velocity;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Velocity;
          break;

         case BoardCommand_SetVoltage:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Voltage;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Voltage;
          break;

         case BoardCommand_SetTorque:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Torque;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Torque;
          break;

         case BoardCommand_SetPosition:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Position;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Position;
          break;
        }
        break;

       case SupervisorFSM_RX_IN_Position:
        switch (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command) {
         case BoardCommand_SetCurrent:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Current;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Current;
          break;

         case BoardCommand_SetVelocity:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Velocity;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Velocity;
          break;

         case BoardCommand_SetVoltage:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Voltage;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Voltage;
          break;

         case BoardCommand_SetTorque:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Torque;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Torque;
          break;
        }
        break;

       case SupervisorFSM_RX_IN_Torque:
        switch (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command) {
         case BoardCommand_SetVoltage:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Voltage;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Voltage;
          break;

         case BoardCommand_SetVelocity:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Velocity;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Velocity;
          break;

         case BoardCommand_SetCurrent:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Current;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Current;
          break;

         case BoardCommand_SetPosition:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Position;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Position;
          break;
        }
        break;

       case SupervisorFSM_RX_IN_Velocity:
        switch (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command) {
         case BoardCommand_SetVoltage:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Voltage;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Voltage;
          break;

         case BoardCommand_SetTorque:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Torque;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Torque;
          break;

         case BoardCommand_SetCurrent:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Current;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Current;
          break;

         case BoardCommand_SetPosition:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Position;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Position;
          break;
        }
        break;

       case SupervisorFSM_RX_IN_Voltage:
        switch (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command) {
         case BoardCommand_SetTorque:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Torque;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Torque;
          break;

         case BoardCommand_SetVelocity:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Velocity;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Velocity;
          break;

         case BoardCommand_SetCurrent:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Current;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Current;
          break;

         case BoardCommand_SetPosition:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Position;
          SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Position;
          break;
        }
        break;
      }
    }
    break;

   case SupervisorFSM_IN_Not_Configured:
    switch (SupervisorFSM_RX_DW.is_Not_Configured) {
     case Su_IN_Controller_Not_Configured:
      if (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.State ==
          BoardState_ControllerConfigured) {
        SupervisorFSM_RX_DW.is_Not_Configured = SupervisorFS_IN_NO_ACTIVE_CHILD;
        SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
          SupervisorFSM_RX_IN_Motor_OFF;
        SupervisorFSM_RX_DW.is_Motor_OFF = SupervisorFSM_RX_IN_Idle;
        SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_Idle;
      }
      break;

     case Supe_IN_Hardware_Not_Configured:
      if (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.State ==
          BoardState_HardwareConfigured) {
        SupervisorFSM_RX_DW.is_Not_Configured = Su_IN_Controller_Not_Configured;
        SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_NotConfigured;
      }
      break;
    }
    break;
  }
}

// Function for Chart: '<Root>/Chart'
void SupervisorFSM_RXModelClass::SupervisorFSM_RX_FAULT_HANDLER(void)
{
  int32_T f_previousEvent;
  switch (SupervisorFSM_RX_DW.is_FAULT_HANDLER) {
   case SupervisorFSM_RX_IN_FAULT:
    if (SupervisorFSM_RX_DW.sfEvent == SupervisorFS_event_ForceIdleEvt) {
      SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_NO_FAULT;
    }
    break;

   case SupervisorFSM_RX_IN_NO_FAULT:
    if (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command ==
        BoardCommand_ForceIdle) {
      SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_NO_FAULT;
    } else if (SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.voltage <
               SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.threshold.voltage_low)
    {
      SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_FAULT;
      f_previousEvent = SupervisorFSM_RX_DW.sfEvent;
      SupervisorFSM_RX_DW.sfEvent = SupervisorFSM__event_HwFaultEvt;
      if (SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER != 0U) {
        Supervisor_CONTROL_MODE_HANDLER();
      }

      SupervisorFSM_RX_DW.sfEvent = f_previousEvent;
    } else if (SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.voltage >
               SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.threshold.voltage_high)
    {
      SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_FAULT;
      f_previousEvent = SupervisorFSM_RX_DW.sfEvent;
      SupervisorFSM_RX_DW.sfEvent = SupervisorFSM__event_HwFaultEvt;
      if (SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER != 0U) {
        Supervisor_CONTROL_MODE_HANDLER();
      }

      SupervisorFSM_RX_DW.sfEvent = f_previousEvent;
    } else if ((SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.current >
                SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.threshold.current_high)
               || (SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.current >
                   SupervisorFSM_RX_DW.CurrLimit)) {
      SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_FAULT;
      f_previousEvent = SupervisorFSM_RX_DW.sfEvent;
      SupervisorFSM_RX_DW.sfEvent = SupervisorFSM__event_HwFaultEvt;
      if (SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER != 0U) {
        Supervisor_CONTROL_MODE_HANDLER();
      }

      SupervisorFSM_RX_DW.sfEvent = f_previousEvent;
    } else if (SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.temperature >
               SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.threshold.temperature_high)
    {
      SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_FAULT;
      f_previousEvent = SupervisorFSM_RX_DW.sfEvent;
      SupervisorFSM_RX_DW.sfEvent = SupervisorFSM__event_HwFaultEvt;
      if (SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER != 0U) {
        Supervisor_CONTROL_MODE_HANDLER();
      }

      SupervisorFSM_RX_DW.sfEvent = f_previousEvent;
    } else if (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.State ==
               BoardState_FaultPressed) {
      SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_FAULT;
      f_previousEvent = SupervisorFSM_RX_DW.sfEvent;
      SupervisorFSM_RX_DW.sfEvent = Superviso_event_FaultPressedEvt;
      if (SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER != 0U) {
        Supervisor_CONTROL_MODE_HANDLER();
      }

      SupervisorFSM_RX_DW.sfEvent = f_previousEvent;
    } else if (SupervisorFSM_RX_DW.sfEvent == Superv_event_SetCurrentLimitEvt) {
      SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_IN_SET_CURR_LIMIT;
      SupervisorFSM_RX_DW.CurrLimit =
        SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_h.current_limit.overload;
    }
    break;

   case SupervisorFSM_IN_SET_CURR_LIMIT:
    SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_NO_FAULT;
    break;
  }
}

// Function for Chart: '<Root>/Chart'
ControlModes SupervisorFSM_RXModelClass::SupervisorFSM_RX_convert(MCControlModes
  mccontrolmode)
{
  ControlModes controlmode;
  switch (mccontrolmode) {
   case MCControlModes_Idle:
    controlmode = ControlModes_Idle;
    break;

   case MCControlModes_Current:
    controlmode = ControlModes_Current;
    break;

   case MCControlModes_SpeedVoltage:
    controlmode = ControlModes_Velocity;
    break;

   case MCControlModes_OpenLoop:
    controlmode = ControlModes_Voltage;
    break;

   default:
    controlmode = ControlModes_Idle;
    break;
  }

  return controlmode;
}

// System initialize for referenced model: 'SupervisorFSM_RX'
void SupervisorFSM_RXModelClass::init(ControlModes *rty_Flags_control_mode,
  boolean_T *rty_Flags_PID_reset, real32_T *rty_Targets_jointpositions_posi,
  real32_T *rty_Targets_jointvelocities_vel, real32_T
  *rty_Targets_motorcurrent_curren, real32_T *rty_Targets_motorvoltage_voltag)
{
  // SystemInitialize for Chart: '<Root>/Chart'
  SupervisorFSM_RX_DW.sfEvent = SupervisorFSM_RX_CALL_EVENT;
  SupervisorFSM_RX_DW.CurrLimit = MAX_uint16_T;
  SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_NotConfigured;
  SupervisorFSM_RX_B.Flags_o.PID_reset = false;
  SupervisorFSM_RX_B.Targets_n.jointpositions.position = 0.0F;
  SupervisorFSM_RX_B.Targets_n.jointvelocities.velocity = 0.0F;
  SupervisorFSM_RX_B.Targets_n.motorcurrent.current = 0.0F;
  SupervisorFSM_RX_B.Targets_n.motorvoltage.voltage = 0.0F;

  // SystemInitialize for SignalConversion generated from: '<Root>/Flags'
  *rty_Flags_control_mode = SupervisorFSM_RX_B.Flags_o.control_mode;

  // SystemInitialize for SignalConversion generated from: '<Root>/Flags'
  *rty_Flags_PID_reset = SupervisorFSM_RX_B.Flags_o.PID_reset;

  // SystemInitialize for SignalConversion generated from: '<Root>/Targets'
  *rty_Targets_jointpositions_posi =
    SupervisorFSM_RX_B.Targets_n.jointpositions.position;

  // SystemInitialize for SignalConversion generated from: '<Root>/Targets'
  *rty_Targets_jointvelocities_vel =
    SupervisorFSM_RX_B.Targets_n.jointvelocities.velocity;

  // SystemInitialize for SignalConversion generated from: '<Root>/Targets'
  *rty_Targets_motorcurrent_curren =
    SupervisorFSM_RX_B.Targets_n.motorcurrent.current;

  // SystemInitialize for SignalConversion generated from: '<Root>/Targets'
  *rty_Targets_motorvoltage_voltag =
    SupervisorFSM_RX_B.Targets_n.motorvoltage.voltage;
}

// Output and update for referenced model: 'SupervisorFSM_RX'
void SupervisorFSM_RXModelClass::step(const BoardState
  *rtu_InternalMessages_State, const BoardCommand *rtu_InternalMessages_Command,
  const real32_T rtu_MotorSensors_Iabc[3], const real32_T
  *rtu_MotorSensors_angle, const real32_T *rtu_MotorSensors_omega, const
  real32_T *rtu_MotorSensors_temperature, const real32_T
  *rtu_MotorSensors_voltage, const real32_T *rtu_MotorSensors_threshold_curr,
  const real32_T *rtu_MotorSensors_threshold_cu_k, const real32_T
  *rtu_MotorSensors_threshold_volt, const real32_T
  *rtu_MotorSensors_threshold_vo_k, const real32_T
  *rtu_MotorSensors_threshold_temp, const real32_T
  *rtu_MotorSensors_threshold_te_c, const real32_T *rtu_MotorSensors_current,
  const boolean_T *rtu_ErrorsRx_event, const boolean_T
  *rtu_EventsRx_control_mode, const boolean_T *rtu_EventsRx_current_limit, const
  boolean_T *rtu_EventsRx_desired_current, const boolean_T
  *rtu_MessagesRx_control_mode_mot, const MCControlModes
  *rtu_MessagesRx_control_mode_mod, const boolean_T
  *rtu_MessagesRx_current_limit_mo, const int16_T
  *rtu_MessagesRx_current_limit_no, const uint16_T
  *rtu_MessagesRx_current_limit_pe, const uint16_T
  *rtu_MessagesRx_current_limit_ov, const int16_T
  *rtu_MessagesRx_desired_current_, ControlModes *rty_Flags_control_mode,
  boolean_T *rty_Flags_PID_reset, real32_T *rty_Targets_jointpositions_posi,
  real32_T *rty_Targets_jointvelocities_vel, real32_T
  *rty_Targets_motorcurrent_curren, real32_T *rty_Targets_motorvoltage_voltag)
{
  int32_T b_previousEvent;
  boolean_T BusConversion_InsertedFor_Cha_0;
  boolean_T BusConversion_InsertedFor_Cha_1;
  boolean_T BusConversion_InsertedFor_Cha_g;
  boolean_T BusConversion_InsertedFor_Cha_p;

  // BusCreator generated from: '<Root>/Chart'
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.State =
    *rtu_InternalMessages_State;
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_i.Command =
    *rtu_InternalMessages_Command;

  // BusCreator generated from: '<Root>/Chart'
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.threshold.current_low =
    *rtu_MotorSensors_threshold_curr;
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.threshold.current_high =
    *rtu_MotorSensors_threshold_cu_k;
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.threshold.voltage_low =
    *rtu_MotorSensors_threshold_volt;
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.threshold.voltage_high =
    *rtu_MotorSensors_threshold_vo_k;
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.threshold.temperature_low =
    *rtu_MotorSensors_threshold_temp;
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.threshold.temperature_high =
    *rtu_MotorSensors_threshold_te_c;
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.Iabc[0] =
    rtu_MotorSensors_Iabc[0];
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.Iabc[1] =
    rtu_MotorSensors_Iabc[1];
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.Iabc[2] =
    rtu_MotorSensors_Iabc[2];
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.angle =
    *rtu_MotorSensors_angle;
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.omega =
    *rtu_MotorSensors_omega;
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.temperature =
    *rtu_MotorSensors_temperature;
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.voltage =
    *rtu_MotorSensors_voltage;
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Chart.current =
    *rtu_MotorSensors_current;

  // BusCreator generated from: '<Root>/Chart'
  BusConversion_InsertedFor_Cha_g = *rtu_ErrorsRx_event;

  // BusCreator generated from: '<Root>/Chart'
  BusConversion_InsertedFor_Cha_p = *rtu_EventsRx_control_mode;
  BusConversion_InsertedFor_Cha_0 = *rtu_EventsRx_current_limit;
  BusConversion_InsertedFor_Cha_1 = *rtu_EventsRx_desired_current;

  // BusCreator generated from: '<Root>/Chart'
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_h.control_mode.motor =
    *rtu_MessagesRx_control_mode_mot;
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_h.current_limit.motor =
    *rtu_MessagesRx_current_limit_mo;
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_h.current_limit.nominal =
    *rtu_MessagesRx_current_limit_no;
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_h.current_limit.peak =
    *rtu_MessagesRx_current_limit_pe;
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_h.current_limit.overload =
    *rtu_MessagesRx_current_limit_ov;
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_h.desired_current.current =
    *rtu_MessagesRx_desired_current_;
  SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_h.control_mode.mode =
    *rtu_MessagesRx_control_mode_mod;

  // Chart: '<Root>/Chart'
  SupervisorFSM_RX_DW.sfEvent = SupervisorFSM_RX_CALL_EVENT;
  if (SupervisorFSM_RX_DW.is_active_c3_SupervisorFSM_RX == 0U) {
    SupervisorFSM_RX_DW.is_active_c3_SupervisorFSM_RX = 1U;
    SupervisorFSM_RX_DW.is_active_FAULT_HANDLER = 1U;
    SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_NO_FAULT;
    SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER = 1U;
    SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
      SupervisorFSM_IN_Not_Configured;
    SupervisorFSM_RX_DW.is_Not_Configured = Supe_IN_Hardware_Not_Configured;
    SupervisorFSM_RX_B.Flags_o.control_mode = ControlModes_NotConfigured;
    SupervisorFSM_RX_DW.is_active_CAN_RX_HANDLER = 1U;
    SupervisorFSM_RX_DW.is_CAN_RX_HANDLER = SupervisorFSM_RX_IN_Home;
  } else {
    if (SupervisorFSM_RX_DW.is_active_FAULT_HANDLER != 0U) {
      SupervisorFSM_RX_FAULT_HANDLER();
    }

    if (SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER != 0U) {
      Supervisor_CONTROL_MODE_HANDLER();
    }

    if ((SupervisorFSM_RX_DW.is_active_CAN_RX_HANDLER != 0U) &&
        (SupervisorFSM_RX_DW.is_CAN_RX_HANDLER == 1) &&
        (!BusConversion_InsertedFor_Cha_g)) {
      if (BusConversion_InsertedFor_Cha_p || BusConversion_InsertedFor_Cha_0 ||
          BusConversion_InsertedFor_Cha_1) {
        if (BusConversion_InsertedFor_Cha_p) {
          SupervisorFSM_RX_B.Flags_o.control_mode = SupervisorFSM_RX_convert
            (SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_h.control_mode.mode);
        } else if (BusConversion_InsertedFor_Cha_0) {
          b_previousEvent = SupervisorFSM_RX_DW.sfEvent;
          SupervisorFSM_RX_DW.sfEvent = Superv_event_SetCurrentLimitEvt;
          if (SupervisorFSM_RX_DW.is_active_FAULT_HANDLER != 0U) {
            SupervisorFSM_RX_FAULT_HANDLER();
          }

          SupervisorFSM_RX_DW.sfEvent = b_previousEvent;
        } else if (BusConversion_InsertedFor_Cha_1) {
          SupervisorFSM_RX_B.Targets_n.motorcurrent.current =
            SupervisorFSM_RX_B.BusConversion_InsertedFor_Cha_h.desired_current.current;
        }
      }

      SupervisorFSM_RX_DW.is_CAN_RX_HANDLER = SupervisorFSM_RX_IN_Home;
    }
  }

  // End of Chart: '<Root>/Chart'

  // SignalConversion generated from: '<Root>/Flags'
  *rty_Flags_control_mode = SupervisorFSM_RX_B.Flags_o.control_mode;

  // SignalConversion generated from: '<Root>/Flags'
  *rty_Flags_PID_reset = SupervisorFSM_RX_B.Flags_o.PID_reset;

  // SignalConversion generated from: '<Root>/Targets'
  *rty_Targets_jointpositions_posi =
    SupervisorFSM_RX_B.Targets_n.jointpositions.position;

  // SignalConversion generated from: '<Root>/Targets'
  *rty_Targets_jointvelocities_vel =
    SupervisorFSM_RX_B.Targets_n.jointvelocities.velocity;

  // SignalConversion generated from: '<Root>/Targets'
  *rty_Targets_motorcurrent_curren =
    SupervisorFSM_RX_B.Targets_n.motorcurrent.current;

  // SignalConversion generated from: '<Root>/Targets'
  *rty_Targets_motorvoltage_voltag =
    SupervisorFSM_RX_B.Targets_n.motorvoltage.voltage;
}

// Constructor
SupervisorFSM_RXModelClass::SupervisorFSM_RXModelClass() :
  SupervisorFSM_RX_B(),
  SupervisorFSM_RX_DW()
{
  // Currently there is no constructor body generated.
}

// Destructor
SupervisorFSM_RXModelClass::~SupervisorFSM_RXModelClass()
{
  // Currently there is no destructor body generated.
}

// Real-Time Model get method
SupervisorFSM_RXModelClass::RT_MODEL_SupervisorFSM_RX_T
  * SupervisorFSM_RXModelClass::getRTM()
{
  return (&SupervisorFSM_RX_M);
}

// member function to setup error status pointer
void SupervisorFSM_RXModelClass::setErrorStatusPointer(const char_T
  **rt_errorStatus)
{
  rtmSetErrorStatusPointer((&SupervisorFSM_RX_M), rt_errorStatus);
}

//
// File trailer for generated code.
//
// [EOF]
//
