//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: supervisor.cpp
//
// Code generated for Simulink model 'supervisor'.
//
// Model version                  : 5.31
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Thu Oct 16 10:36:45 2025
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
const uint8_T supervisor_IN_Calibration = 1U;
const uint8_T supervisor_IN_Current = 2U;
const uint8_T supervisor_IN_Finish = 1U;
const uint8_T supervisor_IN_HWFault = 3U;
const uint8_T supervisor_IN_Home = 1U;
const uint8_T supervisor_IN_Idle = 4U;
const uint8_T supervisor_IN_Idx_found = 2U;
const uint8_T supervisor_IN_LimitNonConfigured = 1U;
const uint8_T supervisor_IN_NO_ACTIVE_CHILD = 0U;
const uint8_T supervisor_IN_NoFault = 2U;
const uint8_T supervisor_IN_NotConfigured = 5U;
const uint8_T supervisor_IN_OverCurrentFault = 3U;
const uint8_T supervisor_IN_Position = 6U;
const uint8_T supervisor_IN_Read_0_encoder = 3U;
const uint8_T supervisor_IN_Search_idx = 4U;
const uint8_T supervisor_IN_SetConfigParam = 2U;
const uint8_T supervisor_IN_SetExternalConfig = 3U;
const uint8_T supervisor_IN_Velocity = 7U;
const uint8_T supervisor_IN_Voltage = 8U;
const uint8_T supervisor_IN_Zero_electric = 5U;
const uint8_T supervisor_IN_velocity_check = 6U;
const int32_T supervisor_event_ControlModeSetpointChange = 599;
const int32_T supervisor_event_DispatcherSetpointChange = 1005;
const int32_T supervisor_event_EnableCalibration = 1628;
const int32_T supervisor_event_SetCtrlMode = 906;
const int32_T supervisor_event_SetMotorConfigExternal = 1891;
const int32_T supervisor_event_SetMotorParam = 1892;
const int32_T supervisor_event_initialControlModeTrigger = 598;

// Forward declaration for local functions
static void supervisor_ResetTargets(Targets *rty_targets);
static void supervisor_TargetsManager(Targets *rty_targets, DW_supervisor_f_T
  *localDW);
static real32_T supervisor_ComputeOffset(real32_T angle);
static real32_T supervisor_CheckIdx(real32_T idx, real32_T p_idx,
  DW_supervisor_f_T *localDW);
static void supervisor_Calibration(const EstimatedData *rtu_EstimatedData, const
  SensorsData *rtu_SensorsData, Targets *rty_targets, ActuatorConfiguration
  *rty_ConfigurationParameters, Flags *rty_Flags, SensorsData
  *rty_SensorsDataCalibration, DW_supervisor_f_T *localDW);
static void supervisor_Current(const EstimatedData *rtu_EstimatedData, const
  FOCOutputs *rtu_ControlOutputs, const SensorsData *rtu_SensorsData, Targets
  *rty_targets, Flags *rty_Flags, DW_supervisor_f_T *localDW);
static void supervisor_Idle(const EstimatedData *rtu_EstimatedData, const
  SensorsData *rtu_SensorsData, Targets *rty_targets, Flags *rty_Flags,
  SensorsData *rty_SensorsDataCalibration, DW_supervisor_f_T *localDW);
static void supervisor_Velocity(const EstimatedData *rtu_EstimatedData, const
  FOCOutputs *rtu_ControlOutputs, const SensorsData *rtu_SensorsData, Targets
  *rty_targets, Flags *rty_Flags, DW_supervisor_f_T *localDW);
static boolean_T supervisor_isConfigurationSet(DW_supervisor_f_T *localDW);
static void supervisor_ControlModeHandler(const EstimatedData *rtu_EstimatedData,
  const FOCOutputs *rtu_ControlOutputs, const SensorsData *rtu_SensorsData,
  Targets *rty_targets, ActuatorConfiguration *rty_ConfigurationParameters,
  Flags *rty_Flags, SensorsData *rty_SensorsDataCalibration, DW_supervisor_f_T
  *localDW);
static boolean_T supervisor_SetMotorParam(const MotorConfigurationExtSet cfg_set,
  ActuatorConfiguration *rty_ConfigurationParameters);
static void supervisor_hardwareConfigMotor(uint8_T b_motor_id,
  ActuatorConfiguration *rty_ConfigurationParameters);
static void supervisor_CheckCalibration(boolean_T
  motor_config_has_quadrature_encoder, int16_T motor_config_rotor_index_offset,
  const EstimatedData *rtu_EstimatedData, const FOCOutputs *rtu_ControlOutputs,
  const SensorsData *rtu_SensorsData, Targets *rty_targets,
  ActuatorConfiguration *rty_ConfigurationParameters, Flags *rty_Flags,
  SensorsData *rty_SensorsDataCalibration, DW_supervisor_f_T *localDW);
static void supervisor_ConfigurationManager(const EstimatedData
  *rtu_EstimatedData, const FOCOutputs *rtu_ControlOutputs, const SensorsData
  *rtu_SensorsData, Targets *rty_targets, ActuatorConfiguration
  *rty_ConfigurationParameters, Flags *rty_Flags, SensorsData
  *rty_SensorsDataCalibration, DW_supervisor_f_T *localDW);
static void supervisor_SetLimits(real32_T limits_overload, real32_T limits_peak,
  real32_T limits_nominal, const EstimatedData *rtu_EstimatedData, const
  FOCOutputs *rtu_ControlOutputs, const SensorsData *rtu_SensorsData, Targets
  *rty_targets, ActuatorConfiguration *rty_ConfigurationParameters, Flags
  *rty_Flags, SensorsData *rty_SensorsDataCalibration, DW_supervisor_f_T
  *localDW);
static void supervisor_SetPid(ControlModes pid_type, real32_T pid_P, real32_T
  pid_I, real32_T pid_D, uint8_T pid_shift_factor, ActuatorConfiguration
  *rty_ConfigurationParameters);
static void supervisor_SetTarget(real32_T tg_position, real32_T tg_velocity,
  real32_T tg_current, real32_T tg_voltage, Targets *rty_targets,
  DW_supervisor_f_T *localDW);

// Function for Chart: '<Root>/Supervisor'
static void supervisor_ResetTargets(Targets *rty_targets)
{
  rty_targets->voltage = 0.0F;
  rty_targets->current = 0.0F;
  rty_targets->velocity = 0.0F;
  rty_targets->position = 0.0F;
  rty_targets->trajectory_time = 0.0F;
}

// Function for Chart: '<Root>/Supervisor'
static void supervisor_TargetsManager(Targets *rty_targets, DW_supervisor_f_T
  *localDW)
{
  if ((localDW->is_TargetsManager == supervisor_IN_Home) && ((localDW->sfEvent ==
        supervisor_event_ControlModeSetpointChange) || (localDW->sfEvent ==
        supervisor_event_DispatcherSetpointChange))) {
    if ((static_cast<uint32_T>(localDW->is_ControlModeHandler ==
          supervisor_IN_Current) | static_cast<uint32_T>
         (localDW->is_ControlModeHandler == supervisor_IN_Calibration)) != 0U) {
      rty_targets->current = static_cast<real32_T>(localDW->newSetpoint);
    } else if (localDW->is_ControlModeHandler == supervisor_IN_Voltage) {
      rty_targets->voltage = static_cast<real32_T>(localDW->newSetpoint);
    } else if ((localDW->is_ControlModeHandler == supervisor_IN_Idle) ||
               (localDW->is_ControlModeHandler == supervisor_IN_HWFault)) {
      supervisor_ResetTargets(rty_targets);
    } else {
      switch (localDW->is_ControlModeHandler) {
       case supervisor_IN_Velocity:
        rty_targets->velocity = static_cast<real32_T>(localDW->newSetpoint);
        break;

       case supervisor_IN_Position:
        rty_targets->position = static_cast<real32_T>(localDW->newSetpoint);
        rty_targets->velocity = static_cast<real32_T>
          (localDW->trajectoryVelocity);
        break;

       default:
        // Default Transition
        break;
      }
    }
  }
}

// Function for Chart: '<Root>/Supervisor'
static real32_T supervisor_ComputeOffset(real32_T angle)
{
  real32_T computed_offset;
  computed_offset = 45.0F - angle;
  if (45.0F - angle > 360.0F) {
    computed_offset = (45.0F - angle) - 360.0F;
  }

  return computed_offset;
}

// Function for Chart: '<Root>/Supervisor'
static real32_T supervisor_CheckIdx(real32_T idx, real32_T p_idx,
  DW_supervisor_f_T *localDW)
{
  real32_T p_idx_out;
  p_idx_out = 0.0F;
  localDW->diff_index = idx - p_idx;
  if (localDW->diff_index != 0.0F) {
    p_idx_out = idx;
  }

  return p_idx_out;
}

// Function for Chart: '<Root>/Supervisor'
static void supervisor_Calibration(const EstimatedData *rtu_EstimatedData, const
  SensorsData *rtu_SensorsData, Targets *rty_targets, ActuatorConfiguration
  *rty_ConfigurationParameters, Flags *rty_Flags, SensorsData
  *rty_SensorsDataCalibration, DW_supervisor_f_T *localDW)
{
  int32_T b_previousEvent;
  boolean_T guard1;
  boolean_T guard2;
  guard1 = false;
  guard2 = false;
  if ((localDW->finish != 0.0) && (rty_Flags->calibration_type ==
       CalibrationTypes_Full_Calibration)) {
    rty_Flags->calibration_type = CalibrationTypes_None;
    rty_Flags->calibration_done = true;
    if (localDW->offset < 32768.0) {
      if (localDW->offset >= -32768.0) {
        rty_ConfigurationParameters->motor.externals.rotor_index_offset =
          static_cast<int16_T>(localDW->offset);
      } else {
        rty_ConfigurationParameters->motor.externals.rotor_index_offset =
          MIN_int16_T;
      }
    } else {
      rty_ConfigurationParameters->motor.externals.rotor_index_offset =
        MAX_int16_T;
    }

    guard2 = true;
  } else if (localDW->finish != 0.0) {
    rty_Flags->calibration_type = CalibrationTypes_None;
    rty_Flags->calibration_done = true;
    guard2 = true;
  } else {
    rty_SensorsDataCalibration->motorsensors.Iabc[0] =
      rtu_SensorsData->motorsensors.Iabc[0];
    rty_SensorsDataCalibration->motorsensors.Iabc[1] =
      rtu_SensorsData->motorsensors.Iabc[1];
    rty_SensorsDataCalibration->motorsensors.Iabc[2] =
      rtu_SensorsData->motorsensors.Iabc[2];
    switch (localDW->is_Calibration) {
     case supervisor_IN_Finish:
      if (localDW->temporalCounter_i1 >= 100U) {
        localDW->finish = 1.0;
      }

      localDW->newSetpoint = 0.0;
      b_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        supervisor_TargetsManager(rty_targets, localDW);
      }

      localDW->sfEvent = b_previousEvent;
      break;

     case supervisor_IN_Idx_found:
      localDW->condition = 4.0;
      localDW->temporalCounter_i1 = 0U;
      localDW->is_Calibration = supervisor_IN_velocity_check;
      localDW->newSetpoint =
        rty_ConfigurationParameters->thresholds.motorNominalCurrents;
      b_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        supervisor_TargetsManager(rty_targets, localDW);
      }

      localDW->sfEvent = b_previousEvent;
      break;

     case supervisor_IN_Read_0_encoder:
      localDW->temporalCounter_i1 = 0U;
      localDW->is_Calibration = supervisor_IN_Finish;
      localDW->newSetpoint = 0.0;
      b_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        supervisor_TargetsManager(rty_targets, localDW);
      }

      localDW->sfEvent = b_previousEvent;
      break;

     case supervisor_IN_Search_idx:
      if (localDW->count == 360.0) {
        localDW->is_Calibration = supervisor_IN_Zero_electric;
        localDW->finish = 0.0;
        localDW->p_index = rtu_SensorsData->motorsensors.qencoder.Idx_counter;
        localDW->diff_index = 0.0F;
        localDW->tzeroalign = 0.7;
        localDW->talign = 0.01;
        localDW->v_threshold = 1.0;
        localDW->count = 0.0;
        rty_SensorsDataCalibration->motorsensors.electrical_angle =
          static_cast<real32_T>(localDW->count);
      } else if ((localDW->diff_index != 0.0F) && (rty_Flags->calibration_type ==
                  CalibrationTypes_Search_Index)) {
        localDW->temporalCounter_i1 = 0U;
        localDW->is_Calibration = supervisor_IN_Finish;
        localDW->newSetpoint = 0.0;
        b_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          supervisor_TargetsManager(rty_targets, localDW);
        }

        localDW->sfEvent = b_previousEvent;
      } else {
        localDW->condition = 2.0;
        localDW->temporalCounter_i1 = 0U;
        localDW->is_Calibration = supervisor_IN_velocity_check;
        localDW->newSetpoint =
          rty_ConfigurationParameters->thresholds.motorNominalCurrents;
        b_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          supervisor_TargetsManager(rty_targets, localDW);
        }

        localDW->sfEvent = b_previousEvent;
      }
      break;

     case supervisor_IN_Zero_electric:
      if (rty_Flags->calibration_type != CalibrationTypes_None) {
        localDW->condition = 1.0;
        localDW->temporalCounter_i1 = 0U;
        localDW->is_Calibration = supervisor_IN_velocity_check;
        localDW->newSetpoint =
          rty_ConfigurationParameters->thresholds.motorNominalCurrents;
        b_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          supervisor_TargetsManager(rty_targets, localDW);
        }

        localDW->sfEvent = b_previousEvent;
      }
      break;

     case supervisor_IN_velocity_check:
      if ((localDW->diff_index != 0.0F) && (rty_Flags->calibration_type ==
           CalibrationTypes_Full_Calibration) && (localDW->condition == 2.0)) {
        localDW->condition = 3.0;
        guard1 = true;
      } else if (std::abs(rtu_EstimatedData->rotor_velocity) >
                 localDW->v_threshold) {
        guard1 = true;
      } else if (((localDW->temporalCounter_i1 >= static_cast<uint32_T>(std::
                    ceil(localDW->tzeroalign * 1000.0))) && (localDW->condition ==
        1.0)) || ((localDW->temporalCounter_i1 >= static_cast<uint32_T>(std::
                    ceil(localDW->talign * 1000.0))) && (localDW->condition ==
                   2.0))) {
        localDW->is_Calibration = supervisor_IN_Search_idx;
        localDW->count++;
        rty_SensorsDataCalibration->motorsensors.electrical_angle = static_cast<
          real32_T>(localDW->count);
        localDW->p_index = supervisor_CheckIdx
          (rtu_SensorsData->motorsensors.qencoder.Idx_counter,
           static_cast<real32_T>(localDW->p_index), localDW);
      } else if ((localDW->temporalCounter_i1 >= static_cast<uint32_T>(std::ceil
                   (localDW->talign * 1000.0))) && (localDW->condition == 3.0))
      {
        localDW->is_Calibration = supervisor_IN_Idx_found;
        rty_SensorsDataCalibration->motorsensors.electrical_angle = 0.0F;
      } else if ((localDW->temporalCounter_i1 >= static_cast<uint32_T>(std::ceil
                   (localDW->tzeroalign * 1000.0))) && (localDW->condition ==
                  4.0)) {
        localDW->is_Calibration = supervisor_IN_Read_0_encoder;
        localDW->offset = supervisor_ComputeOffset
          (rtu_SensorsData->motorsensors.qencoder.rotor_angle);
      } else {
        localDW->newSetpoint =
          rty_ConfigurationParameters->thresholds.motorNominalCurrents;
        b_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          supervisor_TargetsManager(rty_targets, localDW);
        }

        localDW->sfEvent = b_previousEvent;
      }
      break;
    }
  }

  if (guard2) {
    localDW->is_Calibration = supervisor_IN_NO_ACTIVE_CHILD;
    localDW->is_ControlModeHandler = supervisor_IN_Idle;
    rty_Flags->control_mode = ControlModes_Idle;
    rtw_disableMotor();

    // this updates the targets value
    b_previousEvent = localDW->sfEvent;
    localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
    if (localDW->is_active_TargetsManager != 0) {
      supervisor_TargetsManager(rty_targets, localDW);
    }

    localDW->sfEvent = b_previousEvent;
  }

  if (guard1) {
    localDW->temporalCounter_i1 = 0U;
    localDW->is_Calibration = supervisor_IN_velocity_check;
    localDW->newSetpoint =
      rty_ConfigurationParameters->thresholds.motorNominalCurrents;
    b_previousEvent = localDW->sfEvent;
    localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
    if (localDW->is_active_TargetsManager != 0) {
      supervisor_TargetsManager(rty_targets, localDW);
    }

    localDW->sfEvent = b_previousEvent;
  }
}

// Function for Chart: '<Root>/Supervisor'
static void supervisor_Current(const EstimatedData *rtu_EstimatedData, const
  FOCOutputs *rtu_ControlOutputs, const SensorsData *rtu_SensorsData, Targets
  *rty_targets, Flags *rty_Flags, DW_supervisor_f_T *localDW)
{
  int32_T d_previousEvent;
  boolean_T guard1;
  boolean_T guard2;
  boolean_T guard3;
  guard1 = false;
  guard2 = false;
  guard3 = false;
  if ((localDW->isInFault != 0.0) || (localDW->isFaultButtonPressed != 0.0)) {
    if (localDW->isInFault != 0.0) {
      localDW->is_ControlModeHandler = supervisor_IN_HWFault;
      rty_Flags->control_mode = ControlModes_HwFaultCM;
      rtw_disableMotor();

      // this updates the targets value
      d_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        supervisor_TargetsManager(rty_targets, localDW);
      }

      localDW->sfEvent = d_previousEvent;
    } else if (localDW->isFaultButtonPressed != 0.0) {
      guard1 = true;
    } else {
      guard3 = true;
    }
  } else {
    guard3 = true;
  }

  if (guard3) {
    if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
        ((localDW->requestedControlMode != ControlModes_Idle) &&
         (localDW->requestedControlMode != ControlModes_Position))) {
      if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
          (localDW->requestedControlMode == ControlModes_Voltage)) {
        localDW->newSetpoint = rtu_ControlOutputs->Vq;
        localDW->is_ControlModeHandler = supervisor_IN_Voltage;
        rty_Flags->control_mode = ControlModes_Voltage;
        d_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          supervisor_TargetsManager(rty_targets, localDW);
        }

        localDW->sfEvent = d_previousEvent;
      } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
                 (localDW->requestedControlMode == ControlModes_Velocity)) {
        localDW->newSetpoint = rtu_EstimatedData->rotor_velocity;
        localDW->is_ControlModeHandler = supervisor_IN_Velocity;
        rty_Flags->control_mode = ControlModes_Velocity;
        d_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          supervisor_TargetsManager(rty_targets, localDW);
        }

        localDW->sfEvent = d_previousEvent;
      } else {
        guard2 = true;
      }
    } else {
      guard2 = true;
    }
  }

  if (guard2) {
    if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
        (localDW->requestedControlMode == ControlModes_Position)) {
      localDW->is_ControlModeHandler = supervisor_IN_Position;
      rty_Flags->control_mode = ControlModes_Position;
      localDW->newSetpoint = rtu_SensorsData->motorsensors.qencoder.rotor_angle;
      localDW->trajectoryVelocity = 1.0;
      d_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        supervisor_TargetsManager(rty_targets, localDW);
      }

      localDW->sfEvent = d_previousEvent;
    } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
               (localDW->requestedControlMode == ControlModes_Idle)) {
      guard1 = true;
    }
  }

  if (guard1) {
    localDW->is_ControlModeHandler = supervisor_IN_Idle;
    rty_Flags->control_mode = ControlModes_Idle;
    rtw_disableMotor();

    // this updates the targets value
    d_previousEvent = localDW->sfEvent;
    localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
    if (localDW->is_active_TargetsManager != 0) {
      supervisor_TargetsManager(rty_targets, localDW);
    }

    localDW->sfEvent = d_previousEvent;
  }
}

// Function for Chart: '<Root>/Supervisor'
static void supervisor_Idle(const EstimatedData *rtu_EstimatedData, const
  SensorsData *rtu_SensorsData, Targets *rty_targets, Flags *rty_Flags,
  SensorsData *rty_SensorsDataCalibration, DW_supervisor_f_T *localDW)
{
  int32_T g_previousEvent;
  boolean_T guard1;
  if (localDW->sfEvent == supervisor_event_EnableCalibration) {
    localDW->is_ControlModeHandler = supervisor_IN_Calibration;
    rty_Flags->control_mode = ControlModes_Current;
    *rty_SensorsDataCalibration = *rtu_SensorsData;
    localDW->is_Calibration = supervisor_IN_Zero_electric;
    localDW->finish = 0.0;
    localDW->p_index = rtu_SensorsData->motorsensors.qencoder.Idx_counter;
    localDW->diff_index = 0.0F;
    localDW->tzeroalign = 0.7;
    localDW->talign = 0.01;
    localDW->v_threshold = 1.0;
    localDW->count = 0.0;
    rty_SensorsDataCalibration->motorsensors.electrical_angle =
      static_cast<real32_T>(localDW->count);
  } else {
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
          supervisor_TargetsManager(rty_targets, localDW);
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
          supervisor_TargetsManager(rty_targets, localDW);
        }

        localDW->sfEvent = g_previousEvent;
      } else {
        guard1 = true;
      }
    } else {
      guard1 = true;
    }

    if (guard1) {
      if (localDW->requestedControlMode != ControlModes_Idle) {
        rtw_enableMotor();
        if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
            (localDW->requestedControlMode == ControlModes_Position)) {
          localDW->is_ControlModeHandler = supervisor_IN_Position;
          rty_Flags->control_mode = ControlModes_Position;
          localDW->newSetpoint =
            rtu_SensorsData->motorsensors.qencoder.rotor_angle;
          localDW->trajectoryVelocity = 1.0;
          g_previousEvent = localDW->sfEvent;
          localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
          if (localDW->is_active_TargetsManager != 0) {
            supervisor_TargetsManager(rty_targets, localDW);
          }

          localDW->sfEvent = g_previousEvent;
        } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
                   (localDW->requestedControlMode == ControlModes_Current)) {
          localDW->newSetpoint = 0.0;
          localDW->is_ControlModeHandler = supervisor_IN_Current;
          rty_Flags->control_mode = ControlModes_Current;
          g_previousEvent = localDW->sfEvent;
          localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
          if (localDW->is_active_TargetsManager != 0) {
            supervisor_TargetsManager(rty_targets, localDW);
          }

          localDW->sfEvent = g_previousEvent;
        } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
                   (localDW->requestedControlMode == ControlModes_Voltage)) {
          localDW->newSetpoint = 0.0;
          localDW->is_ControlModeHandler = supervisor_IN_Voltage;
          rty_Flags->control_mode = ControlModes_Voltage;
          g_previousEvent = localDW->sfEvent;
          localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
          if (localDW->is_active_TargetsManager != 0) {
            supervisor_TargetsManager(rty_targets, localDW);
          }

          localDW->sfEvent = g_previousEvent;
        } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
                   (localDW->requestedControlMode == ControlModes_Velocity)) {
          localDW->newSetpoint = rtu_EstimatedData->rotor_velocity;
          localDW->is_ControlModeHandler = supervisor_IN_Velocity;
          rty_Flags->control_mode = ControlModes_Velocity;
          g_previousEvent = localDW->sfEvent;
          localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
          if (localDW->is_active_TargetsManager != 0) {
            supervisor_TargetsManager(rty_targets, localDW);
          }

          localDW->sfEvent = g_previousEvent;
        }
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
        supervisor_TargetsManager(rty_targets, localDW);
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
        (localDW->requestedControlMode != ControlModes_Velocity)) {
      if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
          (localDW->requestedControlMode == ControlModes_Voltage)) {
        localDW->newSetpoint = rtu_ControlOutputs->Vq;
        localDW->is_ControlModeHandler = supervisor_IN_Voltage;
        rty_Flags->control_mode = ControlModes_Voltage;
        e_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          supervisor_TargetsManager(rty_targets, localDW);
        }

        localDW->sfEvent = e_previousEvent;
      } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
                 (localDW->requestedControlMode == ControlModes_Current)) {
        localDW->newSetpoint = rtu_EstimatedData->Iq_filtered;
        localDW->is_ControlModeHandler = supervisor_IN_Current;
        rty_Flags->control_mode = ControlModes_Current;
        e_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          supervisor_TargetsManager(rty_targets, localDW);
        }

        localDW->sfEvent = e_previousEvent;
      } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
                 (localDW->requestedControlMode == ControlModes_Position)) {
        localDW->is_ControlModeHandler = supervisor_IN_Position;
        rty_Flags->control_mode = ControlModes_Position;
        localDW->newSetpoint =
          rtu_SensorsData->motorsensors.qencoder.rotor_angle;
        localDW->trajectoryVelocity = 1.0;
        e_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          supervisor_TargetsManager(rty_targets, localDW);
        }

        localDW->sfEvent = e_previousEvent;
      } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
                 (localDW->requestedControlMode == ControlModes_Idle)) {
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
      supervisor_TargetsManager(rty_targets, localDW);
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
  Targets *rty_targets, ActuatorConfiguration *rty_ConfigurationParameters,
  Flags *rty_Flags, SensorsData *rty_SensorsDataCalibration, DW_supervisor_f_T
  *localDW)
{
  int32_T b_previousEvent;
  boolean_T guard1;
  boolean_T guard2;
  boolean_T guard3;
  boolean_T guard4;
  boolean_T guard5;
  boolean_T out;
  guard1 = false;
  guard2 = false;
  guard3 = false;
  guard4 = false;
  guard5 = false;
  switch (localDW->is_ControlModeHandler) {
   case supervisor_IN_Calibration:
    // Chart: '<Root>/Supervisor'
    supervisor_Calibration(rtu_EstimatedData, rtu_SensorsData, rty_targets,
      rty_ConfigurationParameters, rty_Flags, rty_SensorsDataCalibration,
      localDW);
    break;

   case supervisor_IN_Current:
    // Chart: '<Root>/Supervisor'
    supervisor_Current(rtu_EstimatedData, rtu_ControlOutputs, rtu_SensorsData,
                       rty_targets, rty_Flags, localDW);
    break;

   case supervisor_IN_HWFault:
    out = (localDW->requestedControlMode == ControlModes_Idle);
    if ((localDW->isInFault == 0.0) && out && (!supervisor_isConfigurationSet
         (localDW))) {
      localDW->is_ControlModeHandler = supervisor_IN_NotConfigured;

      // Chart: '<Root>/Supervisor'
      rty_Flags->control_mode = ControlModes_NotConfigured;
      rty_Flags->calibration_done = true;
      rtw_disableMotor();
    } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
               ((localDW->isInFault == 0.0) && (localDW->requestedControlMode ==
      ControlModes_Idle))) {
      localDW->is_ControlModeHandler = supervisor_IN_Idle;

      // Chart: '<Root>/Supervisor'
      rty_Flags->control_mode = ControlModes_Idle;
      rtw_disableMotor();

      // this updates the targets value
      b_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        // Chart: '<Root>/Supervisor'
        supervisor_TargetsManager(rty_targets, localDW);
      }

      localDW->sfEvent = b_previousEvent;
    }
    break;

   case supervisor_IN_Idle:
    // Chart: '<Root>/Supervisor'
    supervisor_Idle(rtu_EstimatedData, rtu_SensorsData, rty_targets, rty_Flags,
                    rty_SensorsDataCalibration, localDW);
    break;

   case supervisor_IN_NotConfigured:
    if (localDW->sfEvent == supervisor_event_initialControlModeTrigger) {
      localDW->requestedControlMode = ControlModes_Idle;
      localDW->is_ControlModeHandler = supervisor_IN_Idle;

      // Chart: '<Root>/Supervisor'
      rty_Flags->control_mode = ControlModes_Idle;
      rtw_disableMotor();

      // this updates the targets value
      b_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        // Chart: '<Root>/Supervisor'
        supervisor_TargetsManager(rty_targets, localDW);
      }

      localDW->sfEvent = b_previousEvent;
    } else if (localDW->isInFault != 0.0) {
      localDW->is_ControlModeHandler = supervisor_IN_HWFault;

      // Chart: '<Root>/Supervisor'
      rty_Flags->control_mode = ControlModes_HwFaultCM;
      rtw_disableMotor();

      // this updates the targets value
      b_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        // Chart: '<Root>/Supervisor'
        supervisor_TargetsManager(rty_targets, localDW);
      }

      localDW->sfEvent = b_previousEvent;
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
        b_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          // Chart: '<Root>/Supervisor'
          supervisor_TargetsManager(rty_targets, localDW);
        }

        localDW->sfEvent = b_previousEvent;
      } else if (localDW->isFaultButtonPressed != 0.0) {
        guard1 = true;
      } else {
        guard5 = true;
      }
    } else {
      guard5 = true;
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
        b_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          // Chart: '<Root>/Supervisor'
          supervisor_TargetsManager(rty_targets, localDW);
        }

        localDW->sfEvent = b_previousEvent;
      } else if (localDW->isFaultButtonPressed != 0.0) {
        guard2 = true;
      } else {
        guard4 = true;
      }
    } else {
      guard4 = true;
    }
    break;
  }

  if (guard5) {
    if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
        (localDW->requestedControlMode != ControlModes_Idle)) {
      if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
          (localDW->requestedControlMode == ControlModes_Current)) {
        // Chart: '<Root>/Supervisor'
        localDW->newSetpoint = rtu_EstimatedData->Iq_filtered;
        localDW->is_ControlModeHandler = supervisor_IN_Current;

        // Chart: '<Root>/Supervisor'
        rty_Flags->control_mode = ControlModes_Current;
        b_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          // Chart: '<Root>/Supervisor'
          supervisor_TargetsManager(rty_targets, localDW);
        }

        localDW->sfEvent = b_previousEvent;
      } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
                 (localDW->requestedControlMode == ControlModes_Voltage)) {
        // Chart: '<Root>/Supervisor'
        localDW->newSetpoint = rtu_ControlOutputs->Vq;
        localDW->is_ControlModeHandler = supervisor_IN_Voltage;

        // Chart: '<Root>/Supervisor'
        rty_Flags->control_mode = ControlModes_Voltage;
        b_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          // Chart: '<Root>/Supervisor'
          supervisor_TargetsManager(rty_targets, localDW);
        }

        localDW->sfEvent = b_previousEvent;
      } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
                 (localDW->requestedControlMode == ControlModes_Velocity)) {
        // Chart: '<Root>/Supervisor'
        localDW->newSetpoint = rtu_EstimatedData->rotor_velocity;
        localDW->is_ControlModeHandler = supervisor_IN_Velocity;

        // Chart: '<Root>/Supervisor'
        rty_Flags->control_mode = ControlModes_Velocity;
        b_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
        if (localDW->is_active_TargetsManager != 0) {
          // Chart: '<Root>/Supervisor'
          supervisor_TargetsManager(rty_targets, localDW);
        }

        localDW->sfEvent = b_previousEvent;
      } else {
        guard3 = true;
      }
    } else {
      guard3 = true;
    }
  }

  if (guard4) {
    if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
        (localDW->requestedControlMode == ControlModes_Current)) {
      // Chart: '<Root>/Supervisor'
      localDW->newSetpoint = rtu_EstimatedData->Iq_filtered;
      localDW->is_ControlModeHandler = supervisor_IN_Current;

      // Chart: '<Root>/Supervisor'
      rty_Flags->control_mode = ControlModes_Current;
      b_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        // Chart: '<Root>/Supervisor'
        supervisor_TargetsManager(rty_targets, localDW);
      }

      localDW->sfEvent = b_previousEvent;
    } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
               (localDW->requestedControlMode == ControlModes_Position)) {
      localDW->is_ControlModeHandler = supervisor_IN_Position;

      // Chart: '<Root>/Supervisor'
      rty_Flags->control_mode = ControlModes_Position;
      localDW->newSetpoint = rtu_SensorsData->motorsensors.qencoder.rotor_angle;
      localDW->trajectoryVelocity = 1.0;
      b_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        // Chart: '<Root>/Supervisor'
        supervisor_TargetsManager(rty_targets, localDW);
      }

      localDW->sfEvent = b_previousEvent;
    } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
               (localDW->requestedControlMode == ControlModes_Idle)) {
      guard2 = true;
    } else if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
               (localDW->requestedControlMode == ControlModes_Velocity)) {
      // Chart: '<Root>/Supervisor'
      localDW->newSetpoint = rtu_EstimatedData->rotor_velocity;
      localDW->is_ControlModeHandler = supervisor_IN_Velocity;

      // Chart: '<Root>/Supervisor'
      rty_Flags->control_mode = ControlModes_Velocity;
      b_previousEvent = localDW->sfEvent;
      localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
      if (localDW->is_active_TargetsManager != 0) {
        // Chart: '<Root>/Supervisor'
        supervisor_TargetsManager(rty_targets, localDW);
      }

      localDW->sfEvent = b_previousEvent;
    }
  }

  if (guard3) {
    if ((localDW->sfEvent == supervisor_event_SetCtrlMode) &&
        (localDW->requestedControlMode == ControlModes_Idle)) {
      guard1 = true;
    }
  }

  if (guard2) {
    localDW->is_ControlModeHandler = supervisor_IN_Idle;

    // Chart: '<Root>/Supervisor'
    rty_Flags->control_mode = ControlModes_Idle;
    rtw_disableMotor();

    // this updates the targets value
    b_previousEvent = localDW->sfEvent;
    localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
    if (localDW->is_active_TargetsManager != 0) {
      // Chart: '<Root>/Supervisor'
      supervisor_TargetsManager(rty_targets, localDW);
    }

    localDW->sfEvent = b_previousEvent;
  }

  if (guard1) {
    localDW->is_ControlModeHandler = supervisor_IN_Idle;

    // Chart: '<Root>/Supervisor'
    rty_Flags->control_mode = ControlModes_Idle;
    rtw_disableMotor();

    // this updates the targets value
    b_previousEvent = localDW->sfEvent;
    localDW->sfEvent = supervisor_event_ControlModeSetpointChange;
    if (localDW->is_active_TargetsManager != 0) {
      // Chart: '<Root>/Supervisor'
      supervisor_TargetsManager(rty_targets, localDW);
    }

    localDW->sfEvent = b_previousEvent;
  }
}

// Function for Chart: '<Root>/Supervisor'
static boolean_T supervisor_SetMotorParam(const MotorConfigurationExtSet cfg_set,
  ActuatorConfiguration *rty_ConfigurationParameters)
{
  boolean_T wait_required;
  wait_required = false;
  switch (cfg_set.key) {
   case MCMotorParamsSet_Kbemf:
    rty_ConfigurationParameters->motor.Kbemf = cfg_set.value[0];
    break;

   case MCMotorParamsSet_elect_vmax:
    rty_ConfigurationParameters->motor.Vmax = cfg_set.value[0];
    break;

   case MCMotorParamsSet_hall:
    rty_ConfigurationParameters->motor.hall_sensors_offset = cfg_set.value[0];
    rty_ConfigurationParameters->motor.hall_sensors_swapBC = (cfg_set.value[1]
      != 0.0F);
    wait_required = true;
    break;
  }

  return wait_required;
}

// Function for Chart: '<Root>/Supervisor'
static void supervisor_hardwareConfigMotor(uint8_T b_motor_id,
  ActuatorConfiguration *rty_ConfigurationParameters)
{
  real32_T tmp;
  uint16_T tmp_0;
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

  rtw_configMotor(b_motor_id, static_cast<uint8_T>
                  (rty_ConfigurationParameters->motor.externals.has_quadrature_encoder),
                  rty_ConfigurationParameters->motor.externals.rotor_encoder_resolution,
                  rty_ConfigurationParameters->motor.externals.pole_pairs,
                  static_cast<uint8_T>
                  (rty_ConfigurationParameters->motor.externals.has_hall_sens),
                  static_cast<uint8_T>
                  (rty_ConfigurationParameters->motor.hall_sensors_swapBC),
                  tmp_0);
}

// Function for Chart: '<Root>/Supervisor'
static void supervisor_CheckCalibration(boolean_T
  motor_config_has_quadrature_encoder, int16_T motor_config_rotor_index_offset,
  const EstimatedData *rtu_EstimatedData, const FOCOutputs *rtu_ControlOutputs,
  const SensorsData *rtu_SensorsData, Targets *rty_targets,
  ActuatorConfiguration *rty_ConfigurationParameters, Flags *rty_Flags,
  SensorsData *rty_SensorsDataCalibration, DW_supervisor_f_T *localDW)
{
  int32_T b_previousEvent;
  rty_Flags->calibration_done = true;
  localDW->offset = motor_config_rotor_index_offset;
  if (motor_config_has_quadrature_encoder) {
    rty_Flags->calibration_done = false;
    if (localDW->offset == -1.0) {
      rty_Flags->calibration_type = CalibrationTypes_Full_Calibration;
    } else {
      rty_Flags->calibration_type = CalibrationTypes_Search_Index;
    }

    b_previousEvent = localDW->sfEvent;
    localDW->sfEvent = supervisor_event_EnableCalibration;
    if (localDW->is_active_ControlModeHandler != 0) {
      supervisor_ControlModeHandler(rtu_EstimatedData, rtu_ControlOutputs,
        rtu_SensorsData, rty_targets, rty_ConfigurationParameters, rty_Flags,
        rty_SensorsDataCalibration, localDW);
    }

    localDW->sfEvent = b_previousEvent;
  }
}

// Function for Chart: '<Root>/Supervisor'
static void supervisor_ConfigurationManager(const EstimatedData
  *rtu_EstimatedData, const FOCOutputs *rtu_ControlOutputs, const SensorsData
  *rtu_SensorsData, Targets *rty_targets, ActuatorConfiguration
  *rty_ConfigurationParameters, Flags *rty_Flags, SensorsData
  *rty_SensorsDataCalibration, DW_supervisor_f_T *localDW)
{
  boolean_T guard1;
  boolean_T guard2;
  boolean_T guard3;
  guard1 = false;
  guard2 = false;
  guard3 = false;
  switch (localDW->is_ConfigurationManager) {
   case supervisor_IN_Home:
    if (localDW->sfEvent == supervisor_event_SetMotorConfigExternal) {
      // Chart: '<Root>/Supervisor'
      rty_ConfigurationParameters->motor.externals = localDW->temp_motor_config;
      localDW->is_ConfigurationManager = supervisor_IN_SetExternalConfig;
      localDW->cfg_is_set = true;
    } else if ((localDW->sfEvent == supervisor_event_SetMotorParam) &&
               supervisor_SetMotorParam(localDW->temp_config_set,
                rty_ConfigurationParameters)) {
      localDW->is_ConfigurationManager = supervisor_IN_SetConfigParam;
      localDW->param_is_set = true;
    } else if ((localDW->sfEvent == supervisor_event_SetMotorParam) &&
               (!supervisor_SetMotorParam(localDW->temp_config_set,
                 rty_ConfigurationParameters))) {
      // Chart: '<Root>/Supervisor'
      localDW->is_ConfigurationManager = supervisor_IN_Home;
      localDW->param_is_set = false;
      localDW->cfg_is_set = false;
    }
    break;

   case supervisor_IN_SetConfigParam:
    if (localDW->motor_id_to_configure_prev !=
        localDW->motor_id_to_configure_start) {
      guard1 = true;
    } else if (localDW->sfEvent == supervisor_event_SetMotorConfigExternal) {
      // Chart: '<Root>/Supervisor'
      rty_ConfigurationParameters->motor.externals = localDW->temp_motor_config;
      localDW->is_ConfigurationManager = supervisor_IN_SetExternalConfig;
      localDW->cfg_is_set = true;
    } else if (localDW->param_is_set && localDW->cfg_is_set) {
      // Chart: '<Root>/Supervisor'
      supervisor_hardwareConfigMotor(localDW->motor_id_to_configure,
        rty_ConfigurationParameters);
      supervisor_CheckCalibration
        (localDW->temp_motor_config.has_quadrature_encoder,
         localDW->temp_motor_config.rotor_index_offset, rtu_EstimatedData,
         rtu_ControlOutputs, rtu_SensorsData, rty_targets,
         rty_ConfigurationParameters, rty_Flags, rty_SensorsDataCalibration,
         localDW);
      guard1 = true;
    } else if (localDW->sfEvent == supervisor_event_SetMotorParam) {
      // Chart: '<Root>/Supervisor'
      supervisor_SetMotorParam(localDW->temp_config_set,
        rty_ConfigurationParameters);
      localDW->is_ConfigurationManager = supervisor_IN_SetConfigParam;
      localDW->param_is_set = true;
    }
    break;

   case supervisor_IN_SetExternalConfig:
    if (localDW->motor_id_to_configure_prev !=
        localDW->motor_id_to_configure_start) {
      guard2 = true;
    } else if ((localDW->sfEvent == supervisor_event_SetMotorParam) &&
               supervisor_SetMotorParam(localDW->temp_config_set,
                rty_ConfigurationParameters)) {
      localDW->is_ConfigurationManager = supervisor_IN_SetConfigParam;
      localDW->param_is_set = true;
    } else if (localDW->sfEvent == supervisor_event_SetMotorConfigExternal) {
      // Chart: '<Root>/Supervisor'
      rty_ConfigurationParameters->motor.externals = localDW->temp_motor_config;
      guard3 = true;
    } else if ((localDW->sfEvent == supervisor_event_SetMotorParam) &&
               (!supervisor_SetMotorParam(localDW->temp_config_set,
                 rty_ConfigurationParameters))) {
      guard3 = true;
    } else if (localDW->param_is_set && localDW->cfg_is_set) {
      // Chart: '<Root>/Supervisor'
      supervisor_hardwareConfigMotor(localDW->motor_id_to_configure,
        rty_ConfigurationParameters);
      supervisor_CheckCalibration
        (localDW->temp_motor_config.has_quadrature_encoder,
         localDW->temp_motor_config.rotor_index_offset, rtu_EstimatedData,
         rtu_ControlOutputs, rtu_SensorsData, rty_targets,
         rty_ConfigurationParameters, rty_Flags, rty_SensorsDataCalibration,
         localDW);
      guard2 = true;
    }
    break;
  }

  if (guard3) {
    localDW->is_ConfigurationManager = supervisor_IN_SetExternalConfig;
    localDW->cfg_is_set = true;
  }

  if (guard2) {
    localDW->is_ConfigurationManager = supervisor_IN_Home;
    localDW->param_is_set = false;
    localDW->cfg_is_set = false;
  }

  if (guard1) {
    localDW->is_ConfigurationManager = supervisor_IN_Home;
    localDW->param_is_set = false;
    localDW->cfg_is_set = false;
  }
}

// Function for Chart: '<Root>/Supervisor'
static void supervisor_SetLimits(real32_T limits_overload, real32_T limits_peak,
  real32_T limits_nominal, const EstimatedData *rtu_EstimatedData, const
  FOCOutputs *rtu_ControlOutputs, const SensorsData *rtu_SensorsData, Targets
  *rty_targets, ActuatorConfiguration *rty_ConfigurationParameters, Flags
  *rty_Flags, SensorsData *rty_SensorsDataCalibration, DW_supervisor_f_T
  *localDW)
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
        rtu_SensorsData, rty_targets, rty_ConfigurationParameters, rty_Flags,
        rty_SensorsDataCalibration, localDW);
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
static void supervisor_SetTarget(real32_T tg_position, real32_T tg_velocity,
  real32_T tg_current, real32_T tg_voltage, Targets *rty_targets,
  DW_supervisor_f_T *localDW)
{
  int32_T b_previousEvent;
  boolean_T guard1;
  guard1 = false;
  if (localDW->is_ControlModeHandler == supervisor_IN_Voltage) {
    localDW->newSetpoint = tg_voltage;
    guard1 = true;
  } else if (localDW->is_ControlModeHandler == supervisor_IN_Current) {
    localDW->newSetpoint = tg_current;
    guard1 = true;
  } else if (localDW->is_ControlModeHandler == supervisor_IN_Velocity) {
    localDW->newSetpoint = tg_velocity;
    guard1 = true;
  } else if (localDW->is_ControlModeHandler == supervisor_IN_Position) {
    localDW->newSetpoint = tg_position;
    localDW->trajectoryVelocity = tg_velocity;
    guard1 = true;
  } else if (localDW->is_ControlModeHandler != supervisor_IN_Calibration) {
    localDW->newSetpoint = 0.0;
    guard1 = true;
  }

  if (guard1) {
    b_previousEvent = localDW->sfEvent;
    localDW->sfEvent = supervisor_event_DispatcherSetpointChange;
    if (localDW->is_active_TargetsManager != 0) {
      // Chart: '<Root>/Supervisor'
      supervisor_TargetsManager(rty_targets, localDW);
    }

    localDW->sfEvent = b_previousEvent;
  }
}

// System initialize for referenced model: 'supervisor'
void supervisor_Init(Targets *rty_targets, ActuatorConfiguration
                     *rty_ConfigurationParameters, Flags *rty_Flags, SensorsData
                     *rty_SensorsDataCalibration, DW_supervisor_f_T *localDW)
{
  // SystemInitialize for Chart: '<Root>/Supervisor'
  localDW->sfEvent = supervisor_CALL_EVENT;
  rty_targets->trajectory_time = 0.0F;
  rty_targets->position = 0.0F;
  rty_targets->velocity = 0.0F;
  rty_targets->current = 0.0F;
  rty_targets->voltage = 0.0F;
  rty_Flags->calibration_type = CalibrationTypes_None;
  rty_Flags->calibration_done = false;
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
  rty_ConfigurationParameters->motor.hall_sensors_swapBC = false;
  rty_ConfigurationParameters->motor.reference_encoder = ReferenceEncoder_Motor;
  rty_SensorsDataCalibration->driversensors.Vcc = 0.0F;
  rty_SensorsDataCalibration->motorsensors.qencoder.offset = 0.0F;
  rty_SensorsDataCalibration->motorsensors.qencoder.rotor_angle = 0.0F;
  rty_SensorsDataCalibration->motorsensors.qencoder.counter = 0.0F;
  rty_SensorsDataCalibration->motorsensors.qencoder.Idx_counter = 0.0F;
  rty_SensorsDataCalibration->motorsensors.Iabc[0] = 0.0F;
  rty_SensorsDataCalibration->motorsensors.Iabc[1] = 0.0F;
  rty_SensorsDataCalibration->motorsensors.Iabc[2] = 0.0F;
  rty_SensorsDataCalibration->motorsensors.electrical_angle = 0.0F;
  rty_SensorsDataCalibration->motorsensors.temperature = 0.0F;
  rty_SensorsDataCalibration->motorsensors.voltage = 0.0F;
  rty_SensorsDataCalibration->motorsensors.current = 0.0F;
  rty_SensorsDataCalibration->motorsensors.hallABC = 0U;
}

// Output and update for referenced model: 'supervisor'
void supervisor(const ExternalFlags *rtu_ExternalFlags, const EstimatedData
                *rtu_EstimatedData, const FOCOutputs *rtu_ControlOutputs, const
                SensorsData *rtu_SensorsData, const ReceivedEvents
                rtu_ReceivedEvents[4], const ActuatorConfiguration *rtu_InitConf,
                Targets *rty_targets, ActuatorConfiguration
                *rty_ConfigurationParameters, Flags *rty_Flags, SensorsData
                *rty_SensorsDataCalibration, DW_supervisor_f_T *localDW)
{
  int32_T b_previousEvent;
  int32_T ei;

  // Chart: '<Root>/Supervisor'
  if (localDW->temporalCounter_i1 < MAX_uint32_T) {
    localDW->temporalCounter_i1++;
  }

  localDW->sfEvent = supervisor_CALL_EVENT;
  localDW->ExternalFlags_fault_button_prev =
    localDW->ExternalFlags_fault_button_start;
  localDW->ExternalFlags_fault_button_start = rtu_ExternalFlags->fault_button;
  localDW->motor_id_to_configure_prev = localDW->motor_id_to_configure_start;
  localDW->motor_id_to_configure_start = localDW->motor_id_to_configure;
  if (localDW->is_active_c2_supervisor == 0) {
    *rty_ConfigurationParameters = *rtu_InitConf;
    localDW->ExternalFlags_fault_button_prev = rtu_ExternalFlags->fault_button;
    localDW->ExternalFlags_fault_button_start = rtu_ExternalFlags->fault_button;
    localDW->is_active_c2_supervisor = 1U;
    localDW->is_active_FaultsManager = 1U;
    localDW->is_active_HWFaults = 1U;
    localDW->is_HWFaults = supervisor_IN_LimitNonConfigured;
    localDW->is_active_FaultButton = 1U;
    localDW->is_FaultButton = supervisor_IN_NoFault;
    localDW->isFaultButtonPressed = 0.0;
    localDW->is_active_ControlModeHandler = 1U;
    localDW->is_ControlModeHandler = supervisor_IN_NotConfigured;
    rty_Flags->control_mode = ControlModes_NotConfigured;
    rty_Flags->calibration_done = true;
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
                             rty_ConfigurationParameters, rty_Flags,
                             rty_SensorsDataCalibration, localDW);
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
        localDW->temp_motor_config = rtu_ReceivedEvents[ei].motor_config_content;
        localDW->motor_id_to_configure = rtu_ReceivedEvents[ei].motor_id;
        b_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_SetMotorConfigExternal;
        if (localDW->is_active_ConfigurationManager != 0) {
          supervisor_ConfigurationManager(rtu_EstimatedData, rtu_ControlOutputs,
            rtu_SensorsData, rty_targets, rty_ConfigurationParameters, rty_Flags,
            rty_SensorsDataCalibration, localDW);
        }

        localDW->sfEvent = b_previousEvent;
        break;

       case EventTypes_SetTarget:
        rty_Flags->enable_sending_msg_status = true;
        supervisor_SetTarget(rtu_ReceivedEvents[ei].targets_content.position,
                             rtu_ReceivedEvents[ei].targets_content.velocity,
                             rtu_ReceivedEvents[ei].targets_content.current,
                             rtu_ReceivedEvents[ei].targets_content.voltage,
                             rty_targets, localDW);
        break;

       case EventTypes_SetControlMode:
        localDW->requestedControlMode = rtu_ReceivedEvents[ei].
          control_mode_content;
        b_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_SetCtrlMode;
        if (localDW->is_active_ControlModeHandler != 0) {
          supervisor_ControlModeHandler(rtu_EstimatedData, rtu_ControlOutputs,
            rtu_SensorsData, rty_targets, rty_ConfigurationParameters, rty_Flags,
            rty_SensorsDataCalibration, localDW);
        }

        localDW->sfEvent = b_previousEvent;
        break;

       case EventTypes_SetMotorParam:
        localDW->temp_config_set = rtu_ReceivedEvents[ei].motor_config_set;
        localDW->motor_id_to_configure = rtu_ReceivedEvents[ei].motor_id;
        b_previousEvent = localDW->sfEvent;
        localDW->sfEvent = supervisor_event_SetMotorParam;
        if (localDW->is_active_ConfigurationManager != 0) {
          supervisor_ConfigurationManager(rtu_EstimatedData, rtu_ControlOutputs,
            rtu_SensorsData, rty_targets, rty_ConfigurationParameters, rty_Flags,
            rty_SensorsDataCalibration, localDW);
        }

        localDW->sfEvent = b_previousEvent;
        break;
      }
    }

    localDW->is_InputsDispatcher = supervisor_IN_Home;
    localDW->is_active_TargetsManager = 1U;
    supervisor_ResetTargets(rty_targets);
    localDW->is_TargetsManager = supervisor_IN_Home;
    localDW->is_active_ConfigurationManager = 1U;
    localDW->is_ConfigurationManager = supervisor_IN_Home;
    localDW->param_is_set = false;
    localDW->cfg_is_set = false;
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
        rtu_SensorsData, rty_targets, rty_ConfigurationParameters, rty_Flags,
        rty_SensorsDataCalibration, localDW);
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
                               rty_ConfigurationParameters, rty_Flags,
                               rty_SensorsDataCalibration, localDW);
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
          localDW->temp_motor_config = rtu_ReceivedEvents[ei].
            motor_config_content;
          localDW->motor_id_to_configure = rtu_ReceivedEvents[ei].motor_id;
          b_previousEvent = localDW->sfEvent;
          localDW->sfEvent = supervisor_event_SetMotorConfigExternal;
          if (localDW->is_active_ConfigurationManager != 0) {
            supervisor_ConfigurationManager(rtu_EstimatedData,
              rtu_ControlOutputs, rtu_SensorsData, rty_targets,
              rty_ConfigurationParameters, rty_Flags, rty_SensorsDataCalibration,
              localDW);
          }

          localDW->sfEvent = b_previousEvent;
          break;

         case EventTypes_SetTarget:
          rty_Flags->enable_sending_msg_status = true;
          supervisor_SetTarget(rtu_ReceivedEvents[ei].targets_content.position,
                               rtu_ReceivedEvents[ei].targets_content.velocity,
                               rtu_ReceivedEvents[ei].targets_content.current,
                               rtu_ReceivedEvents[ei].targets_content.voltage,
                               rty_targets, localDW);
          break;

         case EventTypes_SetControlMode:
          localDW->requestedControlMode = rtu_ReceivedEvents[ei].
            control_mode_content;
          b_previousEvent = localDW->sfEvent;
          localDW->sfEvent = supervisor_event_SetCtrlMode;
          if (localDW->is_active_ControlModeHandler != 0) {
            supervisor_ControlModeHandler(rtu_EstimatedData, rtu_ControlOutputs,
              rtu_SensorsData, rty_targets, rty_ConfigurationParameters,
              rty_Flags, rty_SensorsDataCalibration, localDW);
          }

          localDW->sfEvent = b_previousEvent;
          break;

         case EventTypes_SetMotorParam:
          localDW->temp_config_set = rtu_ReceivedEvents[ei].motor_config_set;
          localDW->motor_id_to_configure = rtu_ReceivedEvents[ei].motor_id;
          b_previousEvent = localDW->sfEvent;
          localDW->sfEvent = supervisor_event_SetMotorParam;
          if (localDW->is_active_ConfigurationManager != 0) {
            supervisor_ConfigurationManager(rtu_EstimatedData,
              rtu_ControlOutputs, rtu_SensorsData, rty_targets,
              rty_ConfigurationParameters, rty_Flags, rty_SensorsDataCalibration,
              localDW);
          }

          localDW->sfEvent = b_previousEvent;
          break;
        }
      }

      localDW->is_InputsDispatcher = supervisor_IN_Home;
    }

    if (localDW->is_active_TargetsManager != 0) {
      supervisor_TargetsManager(rty_targets, localDW);
    }

    if (localDW->is_active_ConfigurationManager != 0) {
      supervisor_ConfigurationManager(rtu_EstimatedData, rtu_ControlOutputs,
        rtu_SensorsData, rty_targets, rty_ConfigurationParameters, rty_Flags,
        rty_SensorsDataCalibration, localDW);
    }
  }

  // End of Chart: '<Root>/Supervisor'
}

//
// File trailer for generated code.
//
// [EOF]
//
