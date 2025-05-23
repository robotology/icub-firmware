//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: supervisor.cpp
//
// Code generated for Simulink model 'supervisor'.
//
// Model version                  : 3.18
// Simulink Coder version         : 24.2 (R2024b) 21-Jun-2024
// C/C++ source code generated on : Fri May 23 10:49:23 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "supervisor.h"
#include "supervisor_types.h"
#include "rtwtypes.h"
#include <cmath>
#include "rt_roundf.h"
#include "rtw_defines.h"

// Named constants for Chart: '<Root>/Supervisor'
const int32_T supervisor_CALL_EVENT = -1;
const uint8_T supervisor_IN_ButtonPressed = 1U;
const uint8_T supervisor_IN_Current = 1U;
const uint8_T supervisor_IN_HWFault = 2U;
const uint8_T supervisor_IN_Home = 1U;
const uint8_T supervisor_IN_Idle = 3U;
const uint8_T supervisor_IN_LimitNonConfigured = 1U;
const uint8_T supervisor_IN_NoFault = 2U;
const uint8_T supervisor_IN_NotConfigured = 4U;
const uint8_T supervisor_IN_OverCurrentFault = 3U;
const uint8_T supervisor_IN_Position = 5U;
const uint8_T supervisor_IN_Velocity = 6U;
const uint8_T supervisor_IN_Voltage = 7U;
const int32_T supervisor_event_ControlModeSetpointChange = 599;
const int32_T supervisor_event_DispatcherSetpointChange = 1005;
const int32_T supervisor_event_SetCtrlMode = 906;
const int32_T supervisor_event_initialControlModeTrigger = 598;

// Forward declaration for local functions
static void supervisor_ResetTargets(Targets *rty_targets);
static void supervisor_TargetsManager(Targets *rty_targets, Flags *rty_Flags,
  DW_supervisor_f_T *localDW);
static void supervisor_Idle(const EstimatedData *rtu_EstimatedData, const
  SensorsData *rtu_SensorsData, Targets *rty_targets, Flags *rty_Flags,
  DW_supervisor_f_T *localDW);
static void supervisor_Velocity(const EstimatedData *rtu_EstimatedData, const
  FOCOutputs *rtu_ControlOutputs, const SensorsData *rtu_SensorsData, Targets
  *rty_targets, Flags *rty_Flags, DW_supervisor_f_T *localDW);
static boolean_T supervisor_isConfigurationSet(DW_supervisor_f_T *localDW);
static void supervisor_ControlModeHandler(const EstimatedData *rtu_EstimatedData,
  const FOCOutputs *rtu_ControlOutputs, const SensorsData *rtu_SensorsData,
  Targets *rty_targets, Flags *rty_Flags, DW_supervisor_f_T *localDW);
static void supervisor_SetLimits(real32_T limits_overload, real32_T limits_peak,
  real32_T limits_nominal, const EstimatedData *rtu_EstimatedData, const
  FOCOutputs *rtu_ControlOutputs, const SensorsData *rtu_SensorsData, Targets
  *rty_targets, ActuatorConfiguration *rty_ConfigurationParameters, Flags
  *rty_Flags, DW_supervisor_f_T *localDW);
static void supervisor_SetPid(ControlModes pid_type, real32_T pid_P, real32_T
  pid_I, real32_T pid_D, uint8_T pid_shift_factor, ActuatorConfiguration
  *rty_ConfigurationParameters);
static void supervisor_SetTarget(real32_T tg_velocity, real32_T tg_current,
  real32_T tg_voltage, Targets *rty_targets, Flags *rty_Flags, DW_supervisor_f_T
  *localDW);
static void supervisor_hardwareConfigMotor(uint8_T b_motor_id,
  ActuatorConfiguration *rty_ConfigurationParameters);

// Function for Chart: '<Root>/Supervisor'
static void supervisor_ResetTargets(Targets *rty_targets)
{
  rty_targets->voltage = 0.0F;
  rty_targets->current = 0.0F;
  rty_targets->velocity = 0.0F;
  rty_targets->position = 0.0F;
}

// Function for Chart: '<Root>/Supervisor'
static void supervisor_TargetsManager(Targets *rty_targets, Flags *rty_Flags,
  DW_supervisor_f_T *localDW)
{
  if ((localDW->is_TargetsManager == supervisor_IN_Home) && ((localDW->sfEvent ==
        supervisor_event_ControlModeSetpointChange) || (localDW->sfEvent ==
        supervisor_event_DispatcherSetpointChange))) {
    switch (localDW->is_ControlModeHandler) {
     case supervisor_IN_Current:
      rty_targets->current = static_cast<real32_T>(localDW->newSetpoint);
      rty_Flags->enable_sending_msg_status = true;
      break;

     case supervisor_IN_Voltage:
      rty_targets->voltage = static_cast<real32_T>(localDW->newSetpoint);
      rty_Flags->enable_sending_msg_status = true;
      break;

     default:
      if ((localDW->is_ControlModeHandler == supervisor_IN_Idle) ||
          (localDW->is_ControlModeHandler == supervisor_IN_HWFault)) {
        supervisor_ResetTargets(rty_targets);
      } else {
        switch (localDW->is_ControlModeHandler) {
         case supervisor_IN_Velocity:
          rty_targets->velocity = static_cast<real32_T>(localDW->newSetpoint);
          rty_Flags->enable_sending_msg_status = true;
          break;

         case supervisor_IN_Position:
          rty_targets->position = static_cast<real32_T>(localDW->newSetpoint);
          rty_Flags->enable_sending_msg_status = true;
          break;

         default:
          // Default Transition
          break;
        }
      }
      break;
    }
  }
}

// Function for Chart: '<Root>/Supervisor'
static void supervisor_Idle(const EstimatedData *rtu_EstimatedData, const
  SensorsData *rtu_SensorsData, Targets *rty_targets, Flags *rty_Flags,
  DW_supervisor_f_T *localDW)
{
  int32_T g_previousEvent;
  boolean_T guard1;
  guard1 = false;
  if ((localDW->isInFault != 0.0) || (localDW->isFaultButtonPressed != 0.0)) {
    if (localDW->isInFault != 0.0) {
      localDW->is_ControlModeHandler = supervisor_IN_HWFault;
      rty_Flags->control_mode = ControlModes_HwFaultCM;
      rtw_disableMotor();

      // this updates the targets value
      g_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
      }

      localDW->sfEvent = g_previousEvent;
    } else if (localDW->isFaultButtonPressed != 0.0) {
      localDW->is_ControlModeHandler = supervisor_IN_Idle;
      rty_Flags->control_mode = ControlModes_Idle;
      rtw_disableMotor();

      // this updates the targets value
      g_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
      }

      localDW->sfEvent = g_previousEvent;
    } else {
      guard1 = true;
    }
  } else {
    guard1 = true;
  }

  if (guard1) {
    if (static_cast<int32_T>(localDW->requestedControlMode) != 1) {
      rtw_enableMotor();
      if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
          (static_cast<int32_T>(localDW->requestedControlMode) == 2)) {
        localDW->is_ControlModeHandler = supervisor_IN_Position;
        rty_Flags->control_mode = ControlModes_Position;
        localDW->newSetpoint = rtu_SensorsData->position;
        g_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
        }

        localDW->sfEvent = g_previousEvent;
      } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) && (
                  static_cast<int32_T>(localDW->requestedControlMode) == 4)) {
        localDW->newSetpoint = 0.0;
        localDW->is_ControlModeHandler = supervisor_IN_Current;
        rty_Flags->control_mode = ControlModes_Current;
        g_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
        }

        localDW->sfEvent = g_previousEvent;
      } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) && (
                  static_cast<int32_T>(localDW->requestedControlMode) == 6)) {
        localDW->newSetpoint = 0.0;
        localDW->is_ControlModeHandler = supervisor_IN_Voltage;
        rty_Flags->control_mode = ControlModes_Voltage;
        g_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
        }

        localDW->sfEvent = g_previousEvent;
      } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) && (
                  static_cast<int32_T>(localDW->requestedControlMode) == 5)) {
        localDW->newSetpoint = rtu_EstimatedData->velocity;
        localDW->is_ControlModeHandler = supervisor_IN_Velocity;
        rty_Flags->control_mode = ControlModes_Velocity;
        g_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
        }

        localDW->sfEvent = g_previousEvent;
      }
    }
  }
}

// Function for Chart: '<Root>/Supervisor'
static void supervisor_Velocity(const EstimatedData *rtu_EstimatedData, const
  FOCOutputs *rtu_ControlOutputs, const SensorsData *rtu_SensorsData, Targets
  *rty_targets, Flags *rty_Flags, DW_supervisor_f_T *localDW)
{
  int32_T e_previousEvent;
  boolean_T guard1;
  boolean_T guard2;
  guard1 = false;
  guard2 = false;
  if ((localDW->isInFault != 0.0) || (localDW->isFaultButtonPressed != 0.0)) {
    if (localDW->isInFault != 0.0) {
      localDW->is_ControlModeHandler = supervisor_IN_HWFault;
      rty_Flags->control_mode = ControlModes_HwFaultCM;
      rtw_disableMotor();

      // this updates the targets value
      e_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
      }

      localDW->sfEvent = e_previousEvent;
    } else if (localDW->isFaultButtonPressed != 0.0) {
      guard1 = true;
    } else {
      guard2 = true;
    }
  } else {
    guard2 = true;
  }

  if (guard2) {
    if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
        (static_cast<int32_T>(localDW->requestedControlMode) != 5)) {
      if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
          (static_cast<int32_T>(localDW->requestedControlMode) == 6)) {
        localDW->newSetpoint = rtu_ControlOutputs->Vq;
        localDW->is_ControlModeHandler = supervisor_IN_Voltage;
        rty_Flags->control_mode = ControlModes_Voltage;
        e_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
        }

        localDW->sfEvent = e_previousEvent;
      } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) && (
                  static_cast<int32_T>(localDW->requestedControlMode) == 4)) {
        localDW->newSetpoint = rtu_EstimatedData->Iq_filtered;
        localDW->is_ControlModeHandler = supervisor_IN_Current;
        rty_Flags->control_mode = ControlModes_Current;
        e_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
        }

        localDW->sfEvent = e_previousEvent;
      } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) && (
                  static_cast<int32_T>(localDW->requestedControlMode) == 2)) {
        localDW->is_ControlModeHandler = supervisor_IN_Position;
        rty_Flags->control_mode = ControlModes_Position;
        localDW->newSetpoint = rtu_SensorsData->position;
        e_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
        }

        localDW->sfEvent = e_previousEvent;
      } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) && (
                  static_cast<int32_T>(localDW->requestedControlMode) == 1)) {
        guard1 = true;
      }
    }
  }

  if (guard1) {
    localDW->is_ControlModeHandler = supervisor_IN_Idle;
    rty_Flags->control_mode = ControlModes_Idle;
    rtw_disableMotor();

    // this updates the targets value
    e_previousEvent = localDW->sfEvent;
    localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
    if (localDW->is_active_TargetsManager != 0) {
      supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
    }

    localDW->sfEvent = e_previousEvent;
  }
}

// Function for Chart: '<Root>/Supervisor'
static boolean_T supervisor_isConfigurationSet(DW_supervisor_f_T *localDW)
{
  return localDW->areLimitsSet;
}

// Function for Chart: '<Root>/Supervisor'
static void supervisor_ControlModeHandler(const EstimatedData *rtu_EstimatedData,
  const FOCOutputs *rtu_ControlOutputs, const SensorsData *rtu_SensorsData,
  Targets *rty_targets, Flags *rty_Flags, DW_supervisor_f_T *localDW)
{
  int32_T m_previousEvent;
  boolean_T guard1;
  boolean_T guard2;
  boolean_T guard3;
  boolean_T guard4;
  boolean_T guard5;
  boolean_T guard6;
  boolean_T guard7;
  boolean_T guard8;
  guard1 = false;
  guard2 = false;
  guard3 = false;
  guard4 = false;
  guard5 = false;
  guard6 = false;
  guard7 = false;
  guard8 = false;
  switch (localDW->is_ControlModeHandler) {
   case supervisor_IN_Current:
    if ((localDW->isInFault != 0.0) || (localDW->isFaultButtonPressed != 0.0)) {
      if (localDW->isInFault != 0.0) {
        localDW->is_ControlModeHandler = supervisor_IN_HWFault;

        // Chart: '<Root>/Supervisor'
        rty_Flags->control_mode = ControlModes_HwFaultCM;
        rtw_disableMotor();

        // this updates the targets value
        m_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          // Chart: '<Root>/Supervisor'
          supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
        }

        localDW->sfEvent = m_previousEvent;
      } else if (localDW->isFaultButtonPressed != 0.0) {
        guard1 = true;
      } else {
        guard7 = true;
      }
    } else {
      guard7 = true;
    }
    break;

   case supervisor_IN_HWFault:
    m_previousEvent = (static_cast<int32_T>(localDW->requestedControlMode) == 1);
    if ((localDW->isInFault == 0.0) && (m_previousEvent != 0) &&
        (!supervisor_isConfigurationSet(localDW))) {
      localDW->is_ControlModeHandler = supervisor_IN_NotConfigured;

      // Chart: '<Root>/Supervisor'
      rty_Flags->control_mode = ControlModes_NotConfigured;
      rtw_disableMotor();
    } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
               ((localDW->isInFault == 0.0) && (static_cast<int32_T>
                 (localDW->requestedControlMode) == 1))) {
      localDW->is_ControlModeHandler = supervisor_IN_Idle;

      // Chart: '<Root>/Supervisor'
      rty_Flags->control_mode = ControlModes_Idle;
      rtw_disableMotor();

      // this updates the targets value
      m_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        // Chart: '<Root>/Supervisor'
        supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
      }

      localDW->sfEvent = m_previousEvent;
    }
    break;

   case supervisor_IN_Idle:
    // Chart: '<Root>/Supervisor'
    supervisor_Idle(rtu_EstimatedData, rtu_SensorsData, rty_targets, rty_Flags,
                    localDW);
    break;

   case supervisor_IN_NotConfigured:
    if (localDW->sfEvent == supervisor_event_initialControlModeTrigger) {
      localDW->requestedControlMode = ControlModes_Idle;
      localDW->is_ControlModeHandler = supervisor_IN_Idle;

      // Chart: '<Root>/Supervisor'
      rty_Flags->control_mode = ControlModes_Idle;
      rtw_disableMotor();

      // this updates the targets value
      m_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        // Chart: '<Root>/Supervisor'
        supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
      }

      localDW->sfEvent = m_previousEvent;
    } else if (localDW->isInFault != 0.0) {
      localDW->is_ControlModeHandler = supervisor_IN_HWFault;

      // Chart: '<Root>/Supervisor'
      rty_Flags->control_mode = ControlModes_HwFaultCM;
      rtw_disableMotor();

      // this updates the targets value
      m_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        // Chart: '<Root>/Supervisor'
        supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
      }

      localDW->sfEvent = m_previousEvent;
    }
    break;

   case supervisor_IN_Position:
    if ((localDW->isInFault != 0.0) || (localDW->isFaultButtonPressed != 0.0)) {
      if (localDW->isInFault != 0.0) {
        localDW->is_ControlModeHandler = supervisor_IN_HWFault;

        // Chart: '<Root>/Supervisor'
        rty_Flags->control_mode = ControlModes_HwFaultCM;
        rtw_disableMotor();

        // this updates the targets value
        m_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          // Chart: '<Root>/Supervisor'
          supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
        }

        localDW->sfEvent = m_previousEvent;
      } else if (localDW->isFaultButtonPressed != 0.0) {
        guard2 = true;
      } else {
        guard8 = true;
      }
    } else {
      guard8 = true;
    }
    break;

   case supervisor_IN_Velocity:
    // Chart: '<Root>/Supervisor'
    supervisor_Velocity(rtu_EstimatedData, rtu_ControlOutputs, rtu_SensorsData,
                        rty_targets, rty_Flags, localDW);
    break;

   case supervisor_IN_Voltage:
    if ((localDW->isInFault != 0.0) || (localDW->isFaultButtonPressed != 0.0)) {
      if (localDW->isInFault != 0.0) {
        localDW->is_ControlModeHandler = supervisor_IN_HWFault;

        // Chart: '<Root>/Supervisor'
        rty_Flags->control_mode = ControlModes_HwFaultCM;
        rtw_disableMotor();

        // this updates the targets value
        m_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          // Chart: '<Root>/Supervisor'
          supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
        }

        localDW->sfEvent = m_previousEvent;
      } else if (localDW->isFaultButtonPressed != 0.0) {
        guard3 = true;
      } else {
        guard6 = true;
      }
    } else {
      guard6 = true;
    }
    break;
  }

  if (guard8) {
    if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
        (static_cast<int32_T>(localDW->requestedControlMode) != 1)) {
      if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
          (static_cast<int32_T>(localDW->requestedControlMode) == 4)) {
        // Chart: '<Root>/Supervisor'
        localDW->newSetpoint = rtu_EstimatedData->Iq_filtered;
        localDW->is_ControlModeHandler = supervisor_IN_Current;

        // Chart: '<Root>/Supervisor'
        rty_Flags->control_mode = ControlModes_Current;
        m_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          // Chart: '<Root>/Supervisor'
          supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
        }

        localDW->sfEvent = m_previousEvent;
      } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) && (
                  static_cast<int32_T>(localDW->requestedControlMode) == 6)) {
        // Chart: '<Root>/Supervisor'
        localDW->newSetpoint = rtu_ControlOutputs->Vq;
        localDW->is_ControlModeHandler = supervisor_IN_Voltage;

        // Chart: '<Root>/Supervisor'
        rty_Flags->control_mode = ControlModes_Voltage;
        m_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          // Chart: '<Root>/Supervisor'
          supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
        }

        localDW->sfEvent = m_previousEvent;
      } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) && (
                  static_cast<int32_T>(localDW->requestedControlMode) == 5)) {
        // Chart: '<Root>/Supervisor'
        localDW->newSetpoint = rtu_EstimatedData->velocity;
        localDW->is_ControlModeHandler = supervisor_IN_Velocity;

        // Chart: '<Root>/Supervisor'
        rty_Flags->control_mode = ControlModes_Velocity;
        m_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          // Chart: '<Root>/Supervisor'
          supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
        }

        localDW->sfEvent = m_previousEvent;
      } else {
        guard5 = true;
      }
    } else {
      guard5 = true;
    }
  }

  if (guard7) {
    if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
        ((static_cast<int32_T>(localDW->requestedControlMode) != 1) && (
          static_cast<int32_T>(localDW->requestedControlMode) != 2))) {
      if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
          (static_cast<int32_T>(localDW->requestedControlMode) == 6)) {
        // Chart: '<Root>/Supervisor'
        localDW->newSetpoint = rtu_ControlOutputs->Vq;
        localDW->is_ControlModeHandler = supervisor_IN_Voltage;

        // Chart: '<Root>/Supervisor'
        rty_Flags->control_mode = ControlModes_Voltage;
        m_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          // Chart: '<Root>/Supervisor'
          supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
        }

        localDW->sfEvent = m_previousEvent;
      } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) && (
                  static_cast<int32_T>(localDW->requestedControlMode) == 5)) {
        // Chart: '<Root>/Supervisor'
        localDW->newSetpoint = rtu_EstimatedData->velocity;
        localDW->is_ControlModeHandler = supervisor_IN_Velocity;

        // Chart: '<Root>/Supervisor'
        rty_Flags->control_mode = ControlModes_Velocity;
        m_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          // Chart: '<Root>/Supervisor'
          supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
        }

        localDW->sfEvent = m_previousEvent;
      } else {
        guard4 = true;
      }
    } else {
      guard4 = true;
    }
  }

  if (guard6) {
    if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
        (static_cast<int32_T>(localDW->requestedControlMode) == 4)) {
      // Chart: '<Root>/Supervisor'
      localDW->newSetpoint = rtu_EstimatedData->Iq_filtered;
      localDW->is_ControlModeHandler = supervisor_IN_Current;

      // Chart: '<Root>/Supervisor'
      rty_Flags->control_mode = ControlModes_Current;
      m_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        // Chart: '<Root>/Supervisor'
        supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
      }

      localDW->sfEvent = m_previousEvent;
    } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) && (
                static_cast<int32_T>(localDW->requestedControlMode) == 2)) {
      localDW->is_ControlModeHandler = supervisor_IN_Position;

      // Chart: '<Root>/Supervisor'
      rty_Flags->control_mode = ControlModes_Position;
      localDW->newSetpoint = rtu_SensorsData->position;
      m_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        // Chart: '<Root>/Supervisor'
        supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
      }

      localDW->sfEvent = m_previousEvent;
    } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) && (
                static_cast<int32_T>(localDW->requestedControlMode) == 1)) {
      guard3 = true;
    } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) && (
                static_cast<int32_T>(localDW->requestedControlMode) == 5)) {
      // Chart: '<Root>/Supervisor'
      localDW->newSetpoint = rtu_EstimatedData->velocity;
      localDW->is_ControlModeHandler = supervisor_IN_Velocity;

      // Chart: '<Root>/Supervisor'
      rty_Flags->control_mode = ControlModes_Velocity;
      m_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        // Chart: '<Root>/Supervisor'
        supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
      }

      localDW->sfEvent = m_previousEvent;
    }
  }

  if (guard5) {
    if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
        (static_cast<int32_T>(localDW->requestedControlMode) == 1)) {
      guard2 = true;
    }
  }

  if (guard4) {
    if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
        (static_cast<int32_T>(localDW->requestedControlMode) == 2)) {
      localDW->is_ControlModeHandler = supervisor_IN_Position;

      // Chart: '<Root>/Supervisor'
      rty_Flags->control_mode = ControlModes_Position;
      localDW->newSetpoint = rtu_SensorsData->position;
      m_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        // Chart: '<Root>/Supervisor'
        supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
      }

      localDW->sfEvent = m_previousEvent;
    } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) && (
                static_cast<int32_T>(localDW->requestedControlMode) == 1)) {
      guard1 = true;
    }
  }

  if (guard3) {
    localDW->is_ControlModeHandler = supervisor_IN_Idle;

    // Chart: '<Root>/Supervisor'
    rty_Flags->control_mode = ControlModes_Idle;
    rtw_disableMotor();

    // this updates the targets value
    m_previousEvent = localDW->sfEvent;
    localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
    if (localDW->is_active_TargetsManager != 0) {
      // Chart: '<Root>/Supervisor'
      supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
    }

    localDW->sfEvent = m_previousEvent;
  }

  if (guard2) {
    localDW->is_ControlModeHandler = supervisor_IN_Idle;

    // Chart: '<Root>/Supervisor'
    rty_Flags->control_mode = ControlModes_Idle;
    rtw_disableMotor();

    // this updates the targets value
    m_previousEvent = localDW->sfEvent;
    localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
    if (localDW->is_active_TargetsManager != 0) {
      // Chart: '<Root>/Supervisor'
      supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
    }

    localDW->sfEvent = m_previousEvent;
  }

  if (guard1) {
    localDW->is_ControlModeHandler = supervisor_IN_Idle;

    // Chart: '<Root>/Supervisor'
    rty_Flags->control_mode = ControlModes_Idle;
    rtw_disableMotor();

    // this updates the targets value
    m_previousEvent = localDW->sfEvent;
    localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
    if (localDW->is_active_TargetsManager != 0) {
      // Chart: '<Root>/Supervisor'
      supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
    }

    localDW->sfEvent = m_previousEvent;
  }
}

// Function for Chart: '<Root>/Supervisor'
static void supervisor_SetLimits(real32_T limits_overload, real32_T limits_peak,
  real32_T limits_nominal, const EstimatedData *rtu_EstimatedData, const
  FOCOutputs *rtu_ControlOutputs, const SensorsData *rtu_SensorsData, Targets
  *rty_targets, ActuatorConfiguration *rty_ConfigurationParameters, Flags
  *rty_Flags, DW_supervisor_f_T *localDW)
{
  int32_T b_previousEvent;

  // Chart: '<Root>/Supervisor'
  rty_ConfigurationParameters->thresholds.motorNominalCurrents = std::abs
    (limits_nominal);
  rty_ConfigurationParameters->thresholds.motorPeakCurrents = std::abs
    (limits_peak);
  rty_ConfigurationParameters->thresholds.motorOverloadCurrents = std::abs
    (limits_overload);
  if (!localDW->areLimitsSet) {
    localDW->areLimitsSet = true;
    b_previousEvent = localDW->sfEvent;
    localDW->sfEvent = supervisor_event_initialControlModeTrigger;
    if (localDW->is_active_ControlModeHandler != 0) {
      // Chart: '<Root>/Supervisor'
      supervisor_ControlModeHandler(rtu_EstimatedData, rtu_ControlOutputs,
        rtu_SensorsData, rty_targets, rty_Flags, localDW);
    }

    localDW->sfEvent = b_previousEvent;
  }
}

// Function for Chart: '<Root>/Supervisor'
static void supervisor_SetPid(ControlModes pid_type, real32_T pid_P, real32_T
  pid_I, real32_T pid_D, uint8_T pid_shift_factor, ActuatorConfiguration
  *rty_ConfigurationParameters)
{
  switch (pid_type) {
   case ControlModes_Current:
    // Chart: '<Root>/Supervisor'
    rty_ConfigurationParameters->pids.currentPID.P = pid_P;
    rty_ConfigurationParameters->pids.currentPID.I = pid_I;
    rty_ConfigurationParameters->pids.currentPID.D = pid_D;
    rty_ConfigurationParameters->pids.currentPID.shift_factor = pid_shift_factor;
    rty_ConfigurationParameters->pids.currentPID.type = ControlModes_Current;
    break;

   case ControlModes_Position:
    // Chart: '<Root>/Supervisor'
    rty_ConfigurationParameters->pids.positionPID.P = pid_P;
    rty_ConfigurationParameters->pids.positionPID.I = pid_I;
    rty_ConfigurationParameters->pids.positionPID.D = pid_D;
    rty_ConfigurationParameters->pids.positionPID.shift_factor =
      pid_shift_factor;
    rty_ConfigurationParameters->pids.positionPID.type = ControlModes_Position;
    break;

   default:
    if ((pid_type != ControlModes_Voltage) && (pid_type == ControlModes_Velocity))
    {
      // Chart: '<Root>/Supervisor'
      rty_ConfigurationParameters->pids.velocityPID.P = pid_P;
      rty_ConfigurationParameters->pids.velocityPID.I = pid_I;
      rty_ConfigurationParameters->pids.velocityPID.D = pid_D;
      rty_ConfigurationParameters->pids.velocityPID.shift_factor =
        pid_shift_factor;
    }
    break;
  }
}

// Function for Chart: '<Root>/Supervisor'
static void supervisor_SetTarget(real32_T tg_velocity, real32_T tg_current,
  real32_T tg_voltage, Targets *rty_targets, Flags *rty_Flags, DW_supervisor_f_T
  *localDW)
{
  int32_T b_previousEvent;
  switch (localDW->is_ControlModeHandler) {
   case supervisor_IN_Voltage:
    localDW->newSetpoint = tg_voltage;
    break;

   case supervisor_IN_Current:
    localDW->newSetpoint = tg_current;
    break;

   case supervisor_IN_Velocity:
    localDW->newSetpoint = tg_velocity;
    break;

   default:
    localDW->newSetpoint = 0.0;
    break;
  }

  b_previousEvent = localDW->sfEvent;
  localDW->sfEvent = supervisor_event_DispatcherSetpointChange;
  if (localDW->is_active_TargetsManager != 0) {
    // Chart: '<Root>/Supervisor'
    supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
  }

  localDW->sfEvent = b_previousEvent;
}

// Function for Chart: '<Root>/Supervisor'
static void supervisor_hardwareConfigMotor(uint8_T b_motor_id,
  ActuatorConfiguration *rty_ConfigurationParameters)
{
  real32_T tmp;
  uint16_T tmp_0;

  // Chart: '<Root>/Supervisor'
  tmp = rt_roundf(rty_ConfigurationParameters->motor.hall_sensors_offset *
                  65536.0F / 360.0F);
  if (tmp < 65536.0F) {
    if (tmp >= 0.0F) {
      tmp_0 = static_cast<uint16_T>(tmp);
    } else {
      tmp_0 = 0U;
    }
  } else {
    tmp_0 = MAX_uint16_T;
  }

  // Chart: '<Root>/Supervisor'
  rtw_configMotor(b_motor_id, static_cast<uint8_T>
                  (rty_ConfigurationParameters->motor.externals.has_quadrature_encoder),
                  rty_ConfigurationParameters->motor.externals.rotor_encoder_resolution,
                  rty_ConfigurationParameters->motor.externals.pole_pairs,
                  static_cast<uint8_T>
                  (rty_ConfigurationParameters->motor.externals.has_hall_sens),
                  1U, tmp_0);
}

// System initialize for referenced model: 'supervisor'
void supervisor_Init(Targets *rty_targets, ActuatorConfiguration
                     *rty_ConfigurationParameters, Flags *rty_Flags,
                     DW_supervisor_f_T *localDW)
{
  // SystemInitialize for Chart: '<Root>/Supervisor'
  localDW->sfEvent = supervisor_CALL_EVENT;
  rty_targets->position = 0.0F;
  rty_targets->velocity = 0.0F;
  rty_targets->current = 0.0F;
  rty_targets->voltage = 0.0F;
  rty_Flags->enable_sending_msg_status = false;
  rty_Flags->hw_faults.overcurrent = false;
  rty_Flags->enable_thermal_protection = false;
  rty_Flags->control_mode = ControlModes_NotConfigured;
  rty_ConfigurationParameters->thresholds.jntVelMax = 0.0F;
  rty_ConfigurationParameters->thresholds.motorNominalCurrents = 0.0F;
  rty_ConfigurationParameters->thresholds.motorPeakCurrents = 0.0F;
  rty_ConfigurationParameters->thresholds.motorOverloadCurrents = 0.0F;
  rty_ConfigurationParameters->thresholds.motorPwmLimit = 0U;
  rty_ConfigurationParameters->thresholds.motorCriticalTemperature = 0.0F;
  rty_ConfigurationParameters->pids.currentPID.type = ControlModes_NotConfigured;
  rty_ConfigurationParameters->pids.currentPID.OutMax = 0.0F;
  rty_ConfigurationParameters->pids.currentPID.OutMin = 0.0F;
  rty_ConfigurationParameters->pids.currentPID.P = 0.0F;
  rty_ConfigurationParameters->pids.currentPID.I = 0.0F;
  rty_ConfigurationParameters->pids.currentPID.D = 0.0F;
  rty_ConfigurationParameters->pids.currentPID.N = 0.0F;
  rty_ConfigurationParameters->pids.currentPID.I0 = 0.0F;
  rty_ConfigurationParameters->pids.currentPID.D0 = 0.0F;
  rty_ConfigurationParameters->pids.currentPID.shift_factor = 0U;
  rty_ConfigurationParameters->pids.velocityPID.type =
    ControlModes_NotConfigured;
  rty_ConfigurationParameters->pids.velocityPID.OutMax = 0.0F;
  rty_ConfigurationParameters->pids.velocityPID.OutMin = 0.0F;
  rty_ConfigurationParameters->pids.velocityPID.P = 0.0F;
  rty_ConfigurationParameters->pids.velocityPID.I = 0.0F;
  rty_ConfigurationParameters->pids.velocityPID.D = 0.0F;
  rty_ConfigurationParameters->pids.velocityPID.N = 0.0F;
  rty_ConfigurationParameters->pids.velocityPID.I0 = 0.0F;
  rty_ConfigurationParameters->pids.velocityPID.D0 = 0.0F;
  rty_ConfigurationParameters->pids.velocityPID.shift_factor = 0U;
  rty_ConfigurationParameters->pids.positionPID.type =
    ControlModes_NotConfigured;
  rty_ConfigurationParameters->pids.positionPID.OutMax = 0.0F;
  rty_ConfigurationParameters->pids.positionPID.OutMin = 0.0F;
  rty_ConfigurationParameters->pids.positionPID.P = 0.0F;
  rty_ConfigurationParameters->pids.positionPID.I = 0.0F;
  rty_ConfigurationParameters->pids.positionPID.D = 0.0F;
  rty_ConfigurationParameters->pids.positionPID.N = 0.0F;
  rty_ConfigurationParameters->pids.positionPID.I0 = 0.0F;
  rty_ConfigurationParameters->pids.positionPID.D0 = 0.0F;
  rty_ConfigurationParameters->pids.positionPID.shift_factor = 0U;
  rty_ConfigurationParameters->motor.externals.enable_verbosity = false;
  rty_ConfigurationParameters->motor.externals.has_hall_sens = false;
  rty_ConfigurationParameters->motor.externals.has_quadrature_encoder = false;
  rty_ConfigurationParameters->motor.externals.has_speed_quadrature_encoder =
    false;
  rty_ConfigurationParameters->motor.externals.has_temperature_sens = false;
  rty_ConfigurationParameters->motor.externals.encoder_tolerance = 0U;
  rty_ConfigurationParameters->motor.externals.pole_pairs = 0U;
  rty_ConfigurationParameters->motor.externals.rotor_encoder_resolution = 0;
  rty_ConfigurationParameters->motor.externals.rotor_index_offset = 0;
  rty_ConfigurationParameters->motor.externals.use_index = false;
  rty_ConfigurationParameters->motor.Kbemf = 0.0F;
  rty_ConfigurationParameters->motor.Rphase = 0.0F;
  rty_ConfigurationParameters->motor.Imin = 0.0F;
  rty_ConfigurationParameters->motor.Imax = 0.0F;
  rty_ConfigurationParameters->motor.Vmax = 0.0F;
  rty_ConfigurationParameters->motor.resistance = 0.0F;
  rty_ConfigurationParameters->motor.inductance = 0.0F;
  rty_ConfigurationParameters->motor.thermal_resistance = 0.0F;
  rty_ConfigurationParameters->motor.thermal_time_constant = 0.0F;
  rty_ConfigurationParameters->motor.hall_sensors_offset = 0.0F;
}

// Output and update for referenced model: 'supervisor'
void supervisor(const ExternalFlags *rtu_ExternalFlags, const EstimatedData
                *rtu_EstimatedData, const FOCOutputs *rtu_ControlOutputs, const
                SensorsData *rtu_SensorsData, const ReceivedEvents
                rtu_ReceivedEvents[MAX_EVENTS_PER_TICK], const
                ActuatorConfiguration *rtu_InitConf, Targets *rty_targets,
                ActuatorConfiguration *rty_ConfigurationParameters, Flags
                *rty_Flags, DW_supervisor_f_T *localDW)
{
  int32_T b_previousEvent;
  int32_T ei;

  // Chart: '<Root>/Supervisor'
  localDW->sfEvent = supervisor_CALL_EVENT;
  localDW->ExternalFlags_fault_button_prev =
    localDW->ExternalFlags_fault_button_start;
  localDW->ExternalFlags_fault_button_start = rtu_ExternalFlags->fault_button;
  if (localDW->is_active_c3_supervisor == 0) {
    *rty_ConfigurationParameters = *rtu_InitConf;
    localDW->ExternalFlags_fault_button_prev = rtu_ExternalFlags->fault_button;
    localDW->ExternalFlags_fault_button_start = rtu_ExternalFlags->fault_button;
    localDW->is_active_c3_supervisor = 1U;
    localDW->is_active_FaultsManager = 1U;
    localDW->is_active_HWFaults = 1U;
    localDW->is_HWFaults = supervisor_IN_LimitNonConfigured;
    localDW->is_active_FaultButton = 1U;
    localDW->is_FaultButton = supervisor_IN_NoFault;
    localDW->isFaultButtonPressed = 0.0;
    localDW->is_active_ControlModeHandler = 1U;
    localDW->is_ControlModeHandler = supervisor_IN_NotConfigured;
    rty_Flags->control_mode = ControlModes_NotConfigured;
    rtw_disableMotor();
    localDW->is_active_InputsDispatcher = 1U;

    //  Consume N events per tick
    for (ei = 0; ei < MAX_EVENTS_PER_TICK; ei++) {
      switch (rtu_ReceivedEvents[ei].event_type) {
       case EventTypes_SetLimit:
        supervisor_SetLimits(rtu_ReceivedEvents[ei].limits_content.overload,
                             rtu_ReceivedEvents[ei].limits_content.peak,
                             rtu_ReceivedEvents[ei].limits_content.nominal,
                             rtu_EstimatedData, rtu_ControlOutputs,
                             rtu_SensorsData, rty_targets,
                             rty_ConfigurationParameters, rty_Flags, localDW);
        break;

       case EventTypes_SetPid:
        supervisor_SetPid(rtu_ReceivedEvents[ei].pid_content.type,
                          rtu_ReceivedEvents[ei].pid_content.P,
                          rtu_ReceivedEvents[ei].pid_content.I,
                          rtu_ReceivedEvents[ei].pid_content.D,
                          rtu_ReceivedEvents[ei].pid_content.shift_factor,
                          rty_ConfigurationParameters);
        break;

       case EventTypes_SetMotorConfig:
        rty_ConfigurationParameters->motor.externals = rtu_ReceivedEvents[ei].
          motor_config_content;
        supervisor_hardwareConfigMotor(rtu_ReceivedEvents[ei].motor_id,
          rty_ConfigurationParameters);
        break;

       case EventTypes_SetTarget:
        supervisor_SetTarget(rtu_ReceivedEvents[ei].targets_content.velocity,
                             rtu_ReceivedEvents[ei].targets_content.current,
                             rtu_ReceivedEvents[ei].targets_content.voltage,
                             rty_targets, rty_Flags, localDW);
        break;

       case EventTypes_SetControlMode:
        localDW->requestedControlMode = rtu_ReceivedEvents[ei].
          control_mode_content;
        b_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_SetCtrlMode;
        if (localDW->is_active_ControlModeHandler != 0) {
          supervisor_ControlModeHandler(rtu_EstimatedData, rtu_ControlOutputs,
            rtu_SensorsData, rty_targets, rty_Flags, localDW);
        }

        localDW->sfEvent = b_previousEvent;
        break;
      }
    }

    localDW->is_InputsDispatcher = supervisor_IN_Home;
    localDW->is_active_TargetsManager = 1U;
    supervisor_ResetTargets(rty_targets);
    localDW->is_TargetsManager = supervisor_IN_Home;
  } else {
    if (localDW->is_active_FaultsManager != 0) {
      if (localDW->is_active_HWFaults != 0) {
        switch (localDW->is_HWFaults) {
         case supervisor_IN_LimitNonConfigured:
          if (localDW->areLimitsSet) {
            localDW->is_HWFaults = supervisor_IN_NoFault;
            localDW->isInFault = 0.0;
            rty_Flags->hw_faults.overcurrent = false;
          }
          break;

         case supervisor_IN_NoFault:
          if (std::abs(rtu_EstimatedData->Iq_filtered) >=
              rty_ConfigurationParameters->thresholds.motorOverloadCurrents) {
            localDW->is_HWFaults = supervisor_IN_OverCurrentFault;
            rty_Flags->hw_faults.overcurrent = true;
            localDW->isInFault = 1.0;
          }
          break;

         case supervisor_IN_OverCurrentFault:
          if (std::abs(rtu_EstimatedData->Iq_filtered) <
              rty_ConfigurationParameters->thresholds.motorOverloadCurrents) {
            localDW->is_HWFaults = supervisor_IN_NoFault;
            localDW->isInFault = 0.0;
            rty_Flags->hw_faults.overcurrent = false;
          }
          break;
        }
      }

      if (localDW->is_active_FaultButton != 0) {
        switch (localDW->is_FaultButton) {
         case supervisor_IN_ButtonPressed:
          if ((localDW->ExternalFlags_fault_button_prev !=
               localDW->ExternalFlags_fault_button_start) &&
              (!localDW->ExternalFlags_fault_button_start)) {
            localDW->is_FaultButton = supervisor_IN_NoFault;
            localDW->isFaultButtonPressed = 0.0;
          }
          break;

         case supervisor_IN_NoFault:
          if ((localDW->ExternalFlags_fault_button_prev !=
               localDW->ExternalFlags_fault_button_start) &&
              localDW->ExternalFlags_fault_button_start) {
            localDW->is_FaultButton = supervisor_IN_ButtonPressed;
            localDW->isFaultButtonPressed = 1.0;
          }
          break;
        }
      }
    }

    if (localDW->is_active_ControlModeHandler != 0) {
      supervisor_ControlModeHandler(rtu_EstimatedData, rtu_ControlOutputs,
        rtu_SensorsData, rty_targets, rty_Flags, localDW);
    }

    if ((localDW->is_active_InputsDispatcher != 0) &&
        (localDW->is_InputsDispatcher == supervisor_IN_Home)) {
      rty_Flags->enable_sending_msg_status = false;
      for (ei = 0; ei < MAX_EVENTS_PER_TICK; ei++) {
        switch (rtu_ReceivedEvents[ei].event_type) {
         case EventTypes_SetLimit:
          supervisor_SetLimits(rtu_ReceivedEvents[ei].limits_content.overload,
                               rtu_ReceivedEvents[ei].limits_content.peak,
                               rtu_ReceivedEvents[ei].limits_content.nominal,
                               rtu_EstimatedData, rtu_ControlOutputs,
                               rtu_SensorsData, rty_targets,
                               rty_ConfigurationParameters, rty_Flags, localDW);
          break;

         case EventTypes_SetPid:
          supervisor_SetPid(rtu_ReceivedEvents[ei].pid_content.type,
                            rtu_ReceivedEvents[ei].pid_content.P,
                            rtu_ReceivedEvents[ei].pid_content.I,
                            rtu_ReceivedEvents[ei].pid_content.D,
                            rtu_ReceivedEvents[ei].pid_content.shift_factor,
                            rty_ConfigurationParameters);
          break;

         case EventTypes_SetMotorConfig:
          rty_ConfigurationParameters->motor.externals = rtu_ReceivedEvents[ei].
            motor_config_content;
          supervisor_hardwareConfigMotor(rtu_ReceivedEvents[ei].motor_id,
            rty_ConfigurationParameters);
          break;

         case EventTypes_SetTarget:
          supervisor_SetTarget(rtu_ReceivedEvents[ei].targets_content.velocity,
                               rtu_ReceivedEvents[ei].targets_content.current,
                               rtu_ReceivedEvents[ei].targets_content.voltage,
                               rty_targets, rty_Flags, localDW);
          break;

         case EventTypes_SetControlMode:
          localDW->requestedControlMode = rtu_ReceivedEvents[ei].
            control_mode_content;
          b_previousEvent = localDW->sfEvent;
          localDW->sfEvent = supervisor_event_SetCtrlMode;
          if (localDW->is_active_ControlModeHandler != 0) {
            supervisor_ControlModeHandler(rtu_EstimatedData, rtu_ControlOutputs,
              rtu_SensorsData, rty_targets, rty_Flags, localDW);
          }

          localDW->sfEvent = b_previousEvent;
          break;
        }
      }

      localDW->is_InputsDispatcher = supervisor_IN_Home;
    }

    if (localDW->is_active_TargetsManager != 0) {
      supervisor_TargetsManager(rty_targets, rty_Flags, localDW);
    }
  }

  // End of Chart: '<Root>/Supervisor'
}

//
// File trailer for generated code.
//
// [EOF]
//
