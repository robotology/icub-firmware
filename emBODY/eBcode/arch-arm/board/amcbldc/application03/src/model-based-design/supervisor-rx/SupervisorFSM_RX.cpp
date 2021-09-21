//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: SupervisorFSM_RX.cpp
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
#include "SupervisorFSM_RX.h"
#include "SupervisorFSM_RX_private.h"

// Named constants for Chart: '<Root>/SupervisorFSM_RX'
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

// Function for Chart: '<Root>/SupervisorFSM_RX'
void SupervisorFSM_RXModelClass::Supervisor_CONTROL_MODE_HANDLER(const
  InternalMessages *arg_Internal_Messages, const MotorSensors *arg_MotorSensors,
  const BUS_MESSAGES_RX *arg_MessagesRx, Flags *arg_Flags)
{
  int32_T b_previousEvent;
  switch (SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER) {
   case SupervisorFSM_RX_IN_Motor_OFF:
    if (arg_Internal_Messages->Command == BoardCommand_Reset) {
      SupervisorFSM_RX_DW.is_Motor_OFF = SupervisorFS_IN_NO_ACTIVE_CHILD;
      SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
        SupervisorFSM_IN_Not_Configured;
      SupervisorFSM_RX_DW.is_Not_Configured = Supe_IN_Hardware_Not_Configured;
      arg_Flags->control_mode = ControlModes_NotConfigured;
    } else {
      switch (SupervisorFSM_RX_DW.is_Motor_OFF) {
       case SupervisorFSM_IN_Hardware_Fault:
        if (arg_Internal_Messages->Command == BoardCommand_ForceIdle) {
          b_previousEvent = SupervisorFSM_RX_DW.sfEvent;
          SupervisorFSM_RX_DW.sfEvent = SupervisorFS_event_ForceIdleEvt;
          if (SupervisorFSM_RX_DW.is_active_FAULT_HANDLER != 0U) {
            SupervisorFSM_RX_FAULT_HANDLER(arg_Internal_Messages,
              arg_MotorSensors, arg_MessagesRx, arg_Flags);
          }

          SupervisorFSM_RX_DW.sfEvent = b_previousEvent;
          if (SupervisorFSM_RX_DW.is_Motor_OFF == 1) {
            SupervisorFSM_RX_DW.is_Motor_OFF = SupervisorFSM_RX_IN_Idle;
            arg_Flags->control_mode = ControlModes_Idle;
          }
        }
        break;

       case SupervisorFSM_RX_IN_Idle:
        if (SupervisorFSM_RX_DW.sfEvent == SupervisorFSM__event_HwFaultEvt) {
          SupervisorFSM_RX_DW.is_Motor_OFF = SupervisorFSM_IN_Hardware_Fault;
          arg_Flags->control_mode = ControlModes_HwFaultCM;
        } else if ((arg_Internal_Messages->Command == BoardCommand_SetPosition) ||
                   (arg_Internal_Messages->Command == BoardCommand_SetCurrent) ||
                   (arg_Internal_Messages->Command == BoardCommand_SetVelocity) ||
                   (arg_Internal_Messages->Command == BoardCommand_SetVoltage) ||
                   (arg_Internal_Messages->Command == BoardCommand_SetTorque)) {
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
    if (arg_Internal_Messages->Command == BoardCommand_Reset) {
      SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFS_IN_NO_ACTIVE_CHILD;
      SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
        SupervisorFSM_IN_Not_Configured;
      SupervisorFSM_RX_DW.is_Not_Configured = Supe_IN_Hardware_Not_Configured;
      arg_Flags->control_mode = ControlModes_NotConfigured;
    } else if (SupervisorFSM_RX_DW.sfEvent == SupervisorFSM__event_HwFaultEvt) {
      SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFS_IN_NO_ACTIVE_CHILD;
      SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
        SupervisorFSM_RX_IN_Motor_OFF;
      SupervisorFSM_RX_DW.is_Motor_OFF = SupervisorFSM_IN_Hardware_Fault;
      arg_Flags->control_mode = ControlModes_HwFaultCM;
    } else if ((arg_Internal_Messages->State == BoardState_FaultPressed) ||
               (arg_Internal_Messages->Command == BoardCommand_SetIdle) ||
               (arg_Internal_Messages->Command == BoardCommand_ForceIdle)) {
      SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFS_IN_NO_ACTIVE_CHILD;
      SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
        SupervisorFSM_RX_IN_Motor_OFF;
      SupervisorFSM_RX_DW.is_Motor_OFF = SupervisorFSM_RX_IN_Idle;
      arg_Flags->control_mode = ControlModes_Idle;
    } else if (SupervisorFSM_RX_DW.sfEvent == Superviso_event_FaultPressedEvt) {
      SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFS_IN_NO_ACTIVE_CHILD;
      SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
        SupervisorFSM_RX_IN_Motor_OFF;
      SupervisorFSM_RX_DW.is_Motor_OFF = SupervisorFSM_RX_IN_Idle;
      arg_Flags->control_mode = ControlModes_Idle;
    } else {
      switch (SupervisorFSM_RX_DW.is_Motor_ON) {
       case SupervisorFSM_RX_IN_Command:
        switch (arg_Internal_Messages->Command) {
         case BoardCommand_SetPosition:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Position;
          arg_Flags->control_mode = ControlModes_Position;
          break;

         case BoardCommand_SetVelocity:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Current;
          arg_Flags->control_mode = ControlModes_Current;
          break;

         case BoardCommand_SetCurrent:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Velocity;
          arg_Flags->control_mode = ControlModes_Velocity;
          break;

         case BoardCommand_SetVoltage:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Voltage;
          arg_Flags->control_mode = ControlModes_Voltage;
          break;

         default:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Torque;
          arg_Flags->control_mode = ControlModes_Torque;
          break;
        }
        break;

       case SupervisorFSM_RX_IN_Current:
        switch (arg_Internal_Messages->Command) {
         case BoardCommand_SetVelocity:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Velocity;
          arg_Flags->control_mode = ControlModes_Velocity;
          break;

         case BoardCommand_SetVoltage:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Voltage;
          arg_Flags->control_mode = ControlModes_Voltage;
          break;

         case BoardCommand_SetTorque:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Torque;
          arg_Flags->control_mode = ControlModes_Torque;
          break;

         case BoardCommand_SetPosition:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Position;
          arg_Flags->control_mode = ControlModes_Position;
          break;
        }
        break;

       case SupervisorFSM_RX_IN_Position:
        switch (arg_Internal_Messages->Command) {
         case BoardCommand_SetCurrent:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Current;
          arg_Flags->control_mode = ControlModes_Current;
          break;

         case BoardCommand_SetVelocity:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Velocity;
          arg_Flags->control_mode = ControlModes_Velocity;
          break;

         case BoardCommand_SetVoltage:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Voltage;
          arg_Flags->control_mode = ControlModes_Voltage;
          break;

         case BoardCommand_SetTorque:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Torque;
          arg_Flags->control_mode = ControlModes_Torque;
          break;
        }
        break;

       case SupervisorFSM_RX_IN_Torque:
        switch (arg_Internal_Messages->Command) {
         case BoardCommand_SetVoltage:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Voltage;
          arg_Flags->control_mode = ControlModes_Voltage;
          break;

         case BoardCommand_SetVelocity:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Velocity;
          arg_Flags->control_mode = ControlModes_Velocity;
          break;

         case BoardCommand_SetCurrent:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Current;
          arg_Flags->control_mode = ControlModes_Current;
          break;

         case BoardCommand_SetPosition:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Position;
          arg_Flags->control_mode = ControlModes_Position;
          break;
        }
        break;

       case SupervisorFSM_RX_IN_Velocity:
        switch (arg_Internal_Messages->Command) {
         case BoardCommand_SetVoltage:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Voltage;
          arg_Flags->control_mode = ControlModes_Voltage;
          break;

         case BoardCommand_SetTorque:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Torque;
          arg_Flags->control_mode = ControlModes_Torque;
          break;

         case BoardCommand_SetCurrent:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Current;
          arg_Flags->control_mode = ControlModes_Current;
          break;

         case BoardCommand_SetPosition:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Position;
          arg_Flags->control_mode = ControlModes_Position;
          break;
        }
        break;

       case SupervisorFSM_RX_IN_Voltage:
        switch (arg_Internal_Messages->Command) {
         case BoardCommand_SetTorque:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Torque;
          arg_Flags->control_mode = ControlModes_Torque;
          break;

         case BoardCommand_SetVelocity:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Velocity;
          arg_Flags->control_mode = ControlModes_Velocity;
          break;

         case BoardCommand_SetCurrent:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Current;
          arg_Flags->control_mode = ControlModes_Current;
          break;

         case BoardCommand_SetPosition:
          SupervisorFSM_RX_DW.is_Motor_ON = SupervisorFSM_RX_IN_Position;
          arg_Flags->control_mode = ControlModes_Position;
          break;
        }
        break;
      }
    }
    break;

   case SupervisorFSM_IN_Not_Configured:
    switch (SupervisorFSM_RX_DW.is_Not_Configured) {
     case Su_IN_Controller_Not_Configured:
      if (arg_Internal_Messages->State == BoardState_ControllerConfigured) {
        SupervisorFSM_RX_DW.is_Not_Configured = SupervisorFS_IN_NO_ACTIVE_CHILD;
        SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
          SupervisorFSM_RX_IN_Motor_OFF;
        SupervisorFSM_RX_DW.is_Motor_OFF = SupervisorFSM_RX_IN_Idle;
        arg_Flags->control_mode = ControlModes_Idle;
      }
      break;

     case Supe_IN_Hardware_Not_Configured:
      if (arg_Internal_Messages->State == BoardState_HardwareConfigured) {
        SupervisorFSM_RX_DW.is_Not_Configured = Su_IN_Controller_Not_Configured;
        arg_Flags->control_mode = ControlModes_NotConfigured;
      }
      break;
    }
    break;
  }
}

// Function for Chart: '<Root>/SupervisorFSM_RX'
void SupervisorFSM_RXModelClass::SupervisorFSM_RX_FAULT_HANDLER(const
  InternalMessages *arg_Internal_Messages, const MotorSensors *arg_MotorSensors,
  const BUS_MESSAGES_RX *arg_MessagesRx, Flags *arg_Flags)
{
  int32_T f_previousEvent;
  switch (SupervisorFSM_RX_DW.is_FAULT_HANDLER) {
   case SupervisorFSM_RX_IN_FAULT:
    if (SupervisorFSM_RX_DW.sfEvent == SupervisorFS_event_ForceIdleEvt) {
      SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_NO_FAULT;
    }
    break;

   case SupervisorFSM_RX_IN_NO_FAULT:
    // Chart: '<Root>/SupervisorFSM_RX'
    if (arg_Internal_Messages->Command == BoardCommand_ForceIdle) {
      SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_NO_FAULT;
    } else if (arg_MotorSensors->voltage <
               arg_MotorSensors->threshold.voltage_low) {
      SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_FAULT;
      f_previousEvent = SupervisorFSM_RX_DW.sfEvent;
      SupervisorFSM_RX_DW.sfEvent = SupervisorFSM__event_HwFaultEvt;
      if (SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER != 0U) {
        Supervisor_CONTROL_MODE_HANDLER(arg_Internal_Messages, arg_MotorSensors,
          arg_MessagesRx, arg_Flags);
      }

      SupervisorFSM_RX_DW.sfEvent = f_previousEvent;
    } else if (arg_MotorSensors->voltage >
               arg_MotorSensors->threshold.voltage_high) {
      SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_FAULT;
      f_previousEvent = SupervisorFSM_RX_DW.sfEvent;
      SupervisorFSM_RX_DW.sfEvent = SupervisorFSM__event_HwFaultEvt;
      if (SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER != 0U) {
        Supervisor_CONTROL_MODE_HANDLER(arg_Internal_Messages, arg_MotorSensors,
          arg_MessagesRx, arg_Flags);
      }

      SupervisorFSM_RX_DW.sfEvent = f_previousEvent;
    } else if ((arg_MotorSensors->current >
                arg_MotorSensors->threshold.current_high) ||
               (arg_MotorSensors->current > SupervisorFSM_RX_DW.CurrLimit)) {
      SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_FAULT;
      f_previousEvent = SupervisorFSM_RX_DW.sfEvent;
      SupervisorFSM_RX_DW.sfEvent = SupervisorFSM__event_HwFaultEvt;
      if (SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER != 0U) {
        Supervisor_CONTROL_MODE_HANDLER(arg_Internal_Messages, arg_MotorSensors,
          arg_MessagesRx, arg_Flags);
      }

      SupervisorFSM_RX_DW.sfEvent = f_previousEvent;
    } else if (arg_MotorSensors->temperature >
               arg_MotorSensors->threshold.temperature_high) {
      SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_FAULT;
      f_previousEvent = SupervisorFSM_RX_DW.sfEvent;
      SupervisorFSM_RX_DW.sfEvent = SupervisorFSM__event_HwFaultEvt;
      if (SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER != 0U) {
        Supervisor_CONTROL_MODE_HANDLER(arg_Internal_Messages, arg_MotorSensors,
          arg_MessagesRx, arg_Flags);
      }

      SupervisorFSM_RX_DW.sfEvent = f_previousEvent;
    } else if (arg_Internal_Messages->State == BoardState_FaultPressed) {
      SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_FAULT;
      f_previousEvent = SupervisorFSM_RX_DW.sfEvent;
      SupervisorFSM_RX_DW.sfEvent = Superviso_event_FaultPressedEvt;
      if (SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER != 0U) {
        Supervisor_CONTROL_MODE_HANDLER(arg_Internal_Messages, arg_MotorSensors,
          arg_MessagesRx, arg_Flags);
      }

      SupervisorFSM_RX_DW.sfEvent = f_previousEvent;
    } else if (SupervisorFSM_RX_DW.sfEvent == Superv_event_SetCurrentLimitEvt) {
      SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_IN_SET_CURR_LIMIT;
      SupervisorFSM_RX_DW.CurrLimit = arg_MessagesRx->current_limit.overload;
    }

    // End of Chart: '<Root>/SupervisorFSM_RX'
    break;

   case SupervisorFSM_IN_SET_CURR_LIMIT:
    SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_NO_FAULT;
    break;
  }
}

// Function for Chart: '<Root>/SupervisorFSM_RX'
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
void SupervisorFSM_RXModelClass::init(Flags *arg_Flags, Targets *arg_Targets)
{
  // SystemInitialize for Chart: '<Root>/SupervisorFSM_RX'
  SupervisorFSM_RX_DW.sfEvent = SupervisorFSM_RX_CALL_EVENT;
  SupervisorFSM_RX_DW.CurrLimit = MAX_uint16_T;
  arg_Flags->control_mode = ControlModes_NotConfigured;
  arg_Flags->PID_reset = false;
  arg_Targets->jointpositions.position = 0.0F;
  arg_Targets->jointvelocities.velocity = 0.0F;
  arg_Targets->motorcurrent.current = 0.0F;
  arg_Targets->motorvoltage.voltage = 0.0F;
}

// Output and update for referenced model: 'SupervisorFSM_RX'
void SupervisorFSM_RXModelClass::step(const InternalMessages &
  arg_Internal_Messages, const MotorSensors &arg_MotorSensors, const
  BUS_EVENTS_RX &arg_EventsRx, const BUS_MESSAGES_RX &arg_MessagesRx, const
  BUS_CAN_RX_ERRORS &arg_ErrorsRx, Flags &arg_Flags, Targets &arg_Targets)
{
  int32_T b_previousEvent;

  // Chart: '<Root>/SupervisorFSM_RX'
  SupervisorFSM_RX_DW.sfEvent = SupervisorFSM_RX_CALL_EVENT;
  SupervisorFSM_RX_DW.ErrorsRx_event_prev =
    SupervisorFSM_RX_DW.ErrorsRx_event_start;
  SupervisorFSM_RX_DW.ErrorsRx_event_start = arg_ErrorsRx.event;
  SupervisorFSM_RX_DW.EventsRx_control_mode_prev =
    SupervisorFSM_RX_DW.EventsRx_control_mode_start;
  SupervisorFSM_RX_DW.EventsRx_control_mode_start = arg_EventsRx.control_mode;
  SupervisorFSM_RX_DW.EventsRx_current_limit_prev =
    SupervisorFSM_RX_DW.EventsRx_current_limit_start;
  SupervisorFSM_RX_DW.EventsRx_current_limit_start = arg_EventsRx.current_limit;
  SupervisorFSM_RX_DW.EventsRx_desired_current_prev =
    SupervisorFSM_RX_DW.EventsRx_desired_current_start;
  SupervisorFSM_RX_DW.EventsRx_desired_current_start =
    arg_EventsRx.desired_current;
  if (SupervisorFSM_RX_DW.is_active_c3_SupervisorFSM_RX == 0U) {
    SupervisorFSM_RX_DW.ErrorsRx_event_prev = arg_ErrorsRx.event;
    SupervisorFSM_RX_DW.ErrorsRx_event_start = arg_ErrorsRx.event;
    SupervisorFSM_RX_DW.EventsRx_control_mode_prev = arg_EventsRx.control_mode;
    SupervisorFSM_RX_DW.EventsRx_control_mode_start = arg_EventsRx.control_mode;
    SupervisorFSM_RX_DW.EventsRx_current_limit_prev = arg_EventsRx.current_limit;
    SupervisorFSM_RX_DW.EventsRx_current_limit_start =
      arg_EventsRx.current_limit;
    SupervisorFSM_RX_DW.EventsRx_desired_current_prev =
      arg_EventsRx.desired_current;
    SupervisorFSM_RX_DW.EventsRx_desired_current_start =
      arg_EventsRx.desired_current;
    SupervisorFSM_RX_DW.is_active_c3_SupervisorFSM_RX = 1U;
    SupervisorFSM_RX_DW.is_active_FAULT_HANDLER = 1U;
    SupervisorFSM_RX_DW.is_FAULT_HANDLER = SupervisorFSM_RX_IN_NO_FAULT;
    SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER = 1U;
    SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
      SupervisorFSM_IN_Not_Configured;
    SupervisorFSM_RX_DW.is_Not_Configured = Supe_IN_Hardware_Not_Configured;
    arg_Flags.control_mode = ControlModes_NotConfigured;
    SupervisorFSM_RX_DW.is_active_CAN_RX_HANDLER = 1U;
    SupervisorFSM_RX_DW.is_CAN_RX_HANDLER = SupervisorFSM_RX_IN_Home;
  } else {
    if (SupervisorFSM_RX_DW.is_active_FAULT_HANDLER != 0U) {
      SupervisorFSM_RX_FAULT_HANDLER(&arg_Internal_Messages, &arg_MotorSensors,
        &arg_MessagesRx, &arg_Flags);
    }

    if (SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER != 0U) {
      Supervisor_CONTROL_MODE_HANDLER(&arg_Internal_Messages, &arg_MotorSensors,
        &arg_MessagesRx, &arg_Flags);
    }

    if ((SupervisorFSM_RX_DW.is_active_CAN_RX_HANDLER != 0U) &&
        (SupervisorFSM_RX_DW.is_CAN_RX_HANDLER == 1) &&
        (SupervisorFSM_RX_DW.ErrorsRx_event_prev ==
         SupervisorFSM_RX_DW.ErrorsRx_event_start)) {
      if ((SupervisorFSM_RX_DW.EventsRx_control_mode_prev !=
           SupervisorFSM_RX_DW.EventsRx_control_mode_start) ||
          (SupervisorFSM_RX_DW.EventsRx_current_limit_prev !=
           SupervisorFSM_RX_DW.EventsRx_current_limit_start) ||
          (SupervisorFSM_RX_DW.EventsRx_desired_current_prev !=
           SupervisorFSM_RX_DW.EventsRx_desired_current_start)) {
        if (SupervisorFSM_RX_DW.EventsRx_control_mode_prev !=
            SupervisorFSM_RX_DW.EventsRx_control_mode_start) {
          arg_Flags.control_mode = SupervisorFSM_RX_convert
            (arg_MessagesRx.control_mode.mode);
        } else if (SupervisorFSM_RX_DW.EventsRx_desired_current_prev !=
                   SupervisorFSM_RX_DW.EventsRx_desired_current_start) {
          b_previousEvent = SupervisorFSM_RX_DW.sfEvent;
          SupervisorFSM_RX_DW.sfEvent = Superv_event_SetCurrentLimitEvt;
          if (SupervisorFSM_RX_DW.is_active_FAULT_HANDLER != 0U) {
            SupervisorFSM_RX_FAULT_HANDLER(&arg_Internal_Messages,
              &arg_MotorSensors, &arg_MessagesRx, &arg_Flags);
          }

          SupervisorFSM_RX_DW.sfEvent = b_previousEvent;
        } else if (SupervisorFSM_RX_DW.EventsRx_current_limit_prev !=
                   SupervisorFSM_RX_DW.EventsRx_current_limit_start) {
          arg_Targets.motorcurrent.current =
            arg_MessagesRx.desired_current.current;
        }
      }

      SupervisorFSM_RX_DW.is_CAN_RX_HANDLER = SupervisorFSM_RX_IN_Home;
    }
  }

  // End of Chart: '<Root>/SupervisorFSM_RX'
}

// Constructor
SupervisorFSM_RXModelClass::SupervisorFSM_RXModelClass() :
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
