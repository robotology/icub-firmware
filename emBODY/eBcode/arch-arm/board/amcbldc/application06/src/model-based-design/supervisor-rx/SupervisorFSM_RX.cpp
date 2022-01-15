//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_RX.cpp
//
// Code generated for Simulink model 'SupervisorFSM_RX'.
//
// Model version                  : 3.144
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Fri Jan 14 15:25:19 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "SupervisorFSM_RX.h"
#include "SupervisorFSM_RX_private.h"
#include "uMultiWord2Double.h"
#include "uMultiWordShl.h"

// Named constants for Chart: '<Root>/SupervisorFSM_RX'
const int32_T Supe_event_OutOfBoardFaultEvent = 2;
const int32_T Super_event_setControlModeEvent = 4;
const uint8_T Superviso_IN_LimitNonConfigured = 1U;
const int32_T Superviso_event_BoardFaultEvent = 0;
const int32_T Superviso_event_MotorFaultEvent = 1;
const int32_T SupervisorFSM_RX_CALL_EVENT = -1;
const uint8_T SupervisorFSM_RX_IN_Configured = 1U;
const uint8_T SupervisorFSM_RX_IN_Current = 1U;
const uint8_T SupervisorFSM_RX_IN_Fault = 2U;
const uint8_T SupervisorFSM_RX_IN_HWFault = 2U;
const uint8_T SupervisorFSM_RX_IN_Home = 1U;
const uint8_T SupervisorFSM_RX_IN_Idle = 3U;
const uint8_T SupervisorFSM_RX_IN_NoFault = 2U;
const uint8_T SupervisorFSM_RX_IN_Position = 5U;
const uint8_T SupervisorFSM_RX_IN_Velocity = 6U;
const uint8_T SupervisorFSM_RX_IN_Voltage = 7U;
const uint8_T SupervisorFSM__IN_ButtonPressed = 1U;
const uint8_T SupervisorFSM__IN_NotConfigured = 4U;
const uint8_T SupervisorFS_IN_NotConfigured_o = 3U;
const uint8_T SupervisorF_IN_OverCurrentFault = 3U;
const int32_T SupervisorF_event_setLimitEvent = 6;
namespace amc_bldc_codegen
{
  // Function for Chart: '<Root>/SupervisorFSM_RX'
  ControlModes SupervisorFSM_RX::SupervisorFSM_RX_convert(MCControlModes
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

  // Function for Chart: '<Root>/SupervisorFSM_RX'
  boolean_T SupervisorFSM_RX::SupervisorFSM_RX_IsNewCtrl_Idle(const
    BUS_MESSAGES_RX *arg_MessagesRx)
  {
    boolean_T out;
    out = (SupervisorFSM_RX_convert(arg_MessagesRx->control_mode.mode) ==
           ControlModes_Idle);
    return out;
  }

  // Function for Chart: '<Root>/SupervisorFSM_RX'
  boolean_T SupervisorFSM_RX::SupervisorFS_IsNewCtrl_Position(const
    BUS_MESSAGES_RX *arg_MessagesRx)
  {
    boolean_T out;
    out = (SupervisorFSM_RX_convert(arg_MessagesRx->control_mode.mode) ==
           ControlModes_Position);
    return out;
  }

  // Function for Chart: '<Root>/SupervisorFSM_RX'
  boolean_T SupervisorFSM_RX::SupervisorFSM_IsNewCtrl_Current(const
    BUS_MESSAGES_RX *arg_MessagesRx)
  {
    boolean_T out;
    out = (SupervisorFSM_RX_convert(arg_MessagesRx->control_mode.mode) ==
           ControlModes_Current);
    return out;
  }

  // Function for Chart: '<Root>/SupervisorFSM_RX'
  boolean_T SupervisorFSM_RX::SupervisorFSM_IsNewCtrl_Voltage(const
    BUS_MESSAGES_RX *arg_MessagesRx)
  {
    boolean_T out;
    out = (SupervisorFSM_RX_convert(arg_MessagesRx->control_mode.mode) ==
           ControlModes_Voltage);
    return out;
  }

  // Function for Chart: '<Root>/SupervisorFSM_RX'
  boolean_T SupervisorFSM_RX::SupervisorFS_IsNewCtrl_Velocity(const
    BUS_MESSAGES_RX *arg_MessagesRx)
  {
    boolean_T out;
    out = (SupervisorFSM_RX_convert(arg_MessagesRx->control_mode.mode) ==
           ControlModes_Velocity);
    return out;
  }

  // Function for Chart: '<Root>/SupervisorFSM_RX'
  void SupervisorFSM_RX::SupervisorFSM_RX_Current(const SensorsData
    *arg_SensorsData, const BUS_MESSAGES_RX *arg_MessagesRx, Flags *arg_Flags,
    Targets *arg_Targets)
  {
    if (SupervisorFSM_RX_DW.sfEvent == Superviso_event_MotorFaultEvent) {
      SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER = SupervisorFSM_RX_IN_HWFault;
      arg_Flags->control_mode = ControlModes_HwFaultCM;
      rtw_disableMotor();
    } else {
      boolean_T guard1 = false;
      guard1 = false;
      if ((SupervisorFSM_RX_DW.sfEvent == Super_event_setControlModeEvent) &&
          (!SupervisorFSM_RX_IsNewCtrl_Idle(arg_MessagesRx)) &&
          (!SupervisorFS_IsNewCtrl_Position(arg_MessagesRx))) {
        if (!SupervisorFSM_IsNewCtrl_Current(arg_MessagesRx)) {
          if (SupervisorFSM_IsNewCtrl_Voltage(arg_MessagesRx)) {
            arg_Targets->motorvoltage.voltage =
              arg_SensorsData->motorsensors.voltage;
            SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
              SupervisorFSM_RX_IN_Voltage;
            arg_Flags->control_mode = ControlModes_Voltage;
          } else if (SupervisorFS_IsNewCtrl_Velocity(arg_MessagesRx)) {
            SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
              SupervisorFSM_RX_IN_Velocity;
            arg_Flags->control_mode = ControlModes_Velocity;
            arg_Targets->jointvelocities.velocity = 0.0F;
          } else {
            guard1 = true;
          }
        } else {
          guard1 = true;
        }
      } else if (SupervisorFSM_RX_DW.sfEvent == Super_event_setControlModeEvent)
      {
        if (!SupervisorFSM_IsNewCtrl_Current(arg_MessagesRx)) {
          if (SupervisorFS_IsNewCtrl_Position(arg_MessagesRx)) {
            SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
              SupervisorFSM_RX_IN_Position;
            arg_Flags->control_mode = ControlModes_Position;
            arg_Targets->jointpositions = arg_SensorsData->jointpositions;
          } else if (SupervisorFSM_RX_IsNewCtrl_Idle(arg_MessagesRx)) {
            SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
              SupervisorFSM_RX_IN_Idle;
            arg_Flags->control_mode = ControlModes_Idle;
            rtw_disableMotor();
          } else {
            guard1 = true;
          }
        } else {
          guard1 = true;
        }
      }

      if (guard1) {
        SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
          SupervisorFSM_RX_IN_Current;
        arg_Flags->control_mode = ControlModes_Current;
      }
    }
  }

  // Function for Chart: '<Root>/SupervisorFSM_RX'
  boolean_T SupervisorFSM_RX::SupervisorFSM_RX_IsBoardReady(void) const
  {
    return SupervisorFSM_RX_DW.BoardSt == BoardState_Configured;
  }

  // Function for Chart: '<Root>/SupervisorFSM_RX'
  boolean_T SupervisorFSM_RX::SupervisorFS_isConfigurationSet(void) const
  {
    return SupervisorFSM_RX_DW.IsCurrentLimitSet;
  }

  // Function for Chart: '<Root>/SupervisorFSM_RX'
  boolean_T SupervisorFSM_RX::SupervisorFSM_IsInHardwareFault(void) const
  {
    return SupervisorFSM_RX_DW.isFaultButtonPressed ||
      SupervisorFSM_RX_DW.isInOverCurrent;
  }

  // Function for Chart: '<Root>/SupervisorFSM_RX'
  void SupervisorFSM_RX::Supervisor_CONTROL_MODE_HANDLER(const SensorsData
    *arg_SensorsData, const ControlOutputs *arg_ControlOutputs, const
    BUS_MESSAGES_RX *arg_MessagesRx, Flags *arg_Flags, Targets *arg_Targets)
  {
    boolean_T guard1 = false;
    boolean_T guard2 = false;
    boolean_T guard3 = false;
    boolean_T guard4 = false;
    guard1 = false;
    guard2 = false;
    guard3 = false;
    guard4 = false;
    switch (SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER) {
     case SupervisorFSM_RX_IN_Current:
      // Chart: '<Root>/SupervisorFSM_RX'
      SupervisorFSM_RX_Current(arg_SensorsData, arg_MessagesRx, arg_Flags,
        arg_Targets);
      break;

     case SupervisorFSM_RX_IN_HWFault:
      if ((SupervisorFSM_RX_DW.sfEvent == Super_event_setControlModeEvent) &&
          SupervisorFSM_RX_IsNewCtrl_Idle(arg_MessagesRx) &&
          (!SupervisorFS_isConfigurationSet()) &&
          (!SupervisorFSM_IsInHardwareFault())) {
        SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
          SupervisorFSM__IN_NotConfigured;

        // Chart: '<Root>/SupervisorFSM_RX'
        arg_Flags->control_mode = ControlModes_NotConfigured;
        rtw_disableMotor();
      } else if ((SupervisorFSM_RX_DW.sfEvent == Super_event_setControlModeEvent)
                 && SupervisorFSM_RX_IsNewCtrl_Idle(arg_MessagesRx) &&
                 (!SupervisorFSM_IsInHardwareFault())) {
        // Chart: '<Root>/SupervisorFSM_RX'
        SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER = SupervisorFSM_RX_IN_Idle;
        arg_Flags->control_mode = ControlModes_Idle;
        rtw_disableMotor();
      }
      break;

     case SupervisorFSM_RX_IN_Idle:
      if (SupervisorFSM_RX_DW.sfEvent == Superviso_event_MotorFaultEvent) {
        SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
          SupervisorFSM_RX_IN_HWFault;

        // Chart: '<Root>/SupervisorFSM_RX'
        arg_Flags->control_mode = ControlModes_HwFaultCM;
        rtw_disableMotor();
      } else if (SupervisorFSM_RX_DW.sfEvent == Super_event_setControlModeEvent)
      {
        if (!SupervisorFSM_RX_IsNewCtrl_Idle(arg_MessagesRx)) {
          rtw_enableMotor();
          if (SupervisorFS_IsNewCtrl_Position(arg_MessagesRx)) {
            SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
              SupervisorFSM_RX_IN_Position;

            // Chart: '<Root>/SupervisorFSM_RX'
            arg_Flags->control_mode = ControlModes_Position;
            arg_Targets->jointpositions = arg_SensorsData->jointpositions;
          } else if (SupervisorFSM_IsNewCtrl_Current(arg_MessagesRx)) {
            // Chart: '<Root>/SupervisorFSM_RX'
            arg_Targets->motorcurrent.current = 0.0F;
            SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
              SupervisorFSM_RX_IN_Current;

            // Chart: '<Root>/SupervisorFSM_RX'
            arg_Flags->control_mode = ControlModes_Current;
          } else if (SupervisorFSM_IsNewCtrl_Voltage(arg_MessagesRx)) {
            // Chart: '<Root>/SupervisorFSM_RX'
            arg_Targets->motorvoltage.voltage = 0.0F;
            SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
              SupervisorFSM_RX_IN_Voltage;

            // Chart: '<Root>/SupervisorFSM_RX'
            arg_Flags->control_mode = ControlModes_Voltage;
          } else if (SupervisorFS_IsNewCtrl_Velocity(arg_MessagesRx)) {
            SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
              SupervisorFSM_RX_IN_Velocity;

            // Chart: '<Root>/SupervisorFSM_RX'
            arg_Flags->control_mode = ControlModes_Velocity;
            arg_Targets->jointvelocities.velocity = 0.0F;
          } else {
            guard1 = true;
          }
        } else {
          guard1 = true;
        }
      }
      break;

     case SupervisorFSM__IN_NotConfigured:
      if (SupervisorFSM_RX_IsBoardReady() && SupervisorFS_isConfigurationSet())
      {
        SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER = SupervisorFSM_RX_IN_Idle;

        // Chart: '<Root>/SupervisorFSM_RX'
        arg_Flags->control_mode = ControlModes_Idle;
        rtw_disableMotor();
      } else if (SupervisorFSM_RX_DW.sfEvent == Superviso_event_MotorFaultEvent)
      {
        SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
          SupervisorFSM_RX_IN_HWFault;

        // Chart: '<Root>/SupervisorFSM_RX'
        arg_Flags->control_mode = ControlModes_HwFaultCM;
        rtw_disableMotor();
      }
      break;

     case SupervisorFSM_RX_IN_Position:
      if (SupervisorFSM_RX_DW.sfEvent == Superviso_event_MotorFaultEvent) {
        SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
          SupervisorFSM_RX_IN_HWFault;

        // Chart: '<Root>/SupervisorFSM_RX'
        arg_Flags->control_mode = ControlModes_HwFaultCM;
        rtw_disableMotor();
      } else if ((SupervisorFSM_RX_DW.sfEvent == Super_event_setControlModeEvent)
                 && (!SupervisorFSM_RX_IsNewCtrl_Idle(arg_MessagesRx)) &&
                 (!SupervisorFS_IsNewCtrl_Position(arg_MessagesRx))) {
        if (SupervisorFSM_IsNewCtrl_Current(arg_MessagesRx)) {
          // Chart: '<Root>/SupervisorFSM_RX'
          arg_Targets->motorcurrent = arg_ControlOutputs->Iq_fbk;
          SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
            SupervisorFSM_RX_IN_Current;

          // Chart: '<Root>/SupervisorFSM_RX'
          arg_Flags->control_mode = ControlModes_Current;
        } else if (SupervisorFSM_IsNewCtrl_Voltage(arg_MessagesRx)) {
          // Chart: '<Root>/SupervisorFSM_RX'
          arg_Targets->motorvoltage.voltage =
            arg_SensorsData->motorsensors.voltage;
          SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
            SupervisorFSM_RX_IN_Voltage;

          // Chart: '<Root>/SupervisorFSM_RX'
          arg_Flags->control_mode = ControlModes_Voltage;
        } else if (SupervisorFS_IsNewCtrl_Velocity(arg_MessagesRx)) {
          SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
            SupervisorFSM_RX_IN_Velocity;

          // Chart: '<Root>/SupervisorFSM_RX'
          arg_Flags->control_mode = ControlModes_Velocity;
          arg_Targets->jointvelocities.velocity = 0.0F;
        } else {
          guard2 = true;
        }
      } else {
        guard2 = true;
      }
      break;

     case SupervisorFSM_RX_IN_Velocity:
      if (SupervisorFSM_RX_DW.sfEvent == Superviso_event_MotorFaultEvent) {
        SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
          SupervisorFSM_RX_IN_HWFault;

        // Chart: '<Root>/SupervisorFSM_RX'
        arg_Flags->control_mode = ControlModes_HwFaultCM;
        rtw_disableMotor();
      } else if (SupervisorFSM_RX_DW.sfEvent == Super_event_setControlModeEvent)
      {
        if (!SupervisorFS_IsNewCtrl_Velocity(arg_MessagesRx)) {
          if (SupervisorFSM_IsNewCtrl_Voltage(arg_MessagesRx)) {
            // Chart: '<Root>/SupervisorFSM_RX'
            arg_Targets->motorvoltage.voltage =
              arg_SensorsData->motorsensors.voltage;
            SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
              SupervisorFSM_RX_IN_Voltage;

            // Chart: '<Root>/SupervisorFSM_RX'
            arg_Flags->control_mode = ControlModes_Voltage;
          } else if (SupervisorFSM_IsNewCtrl_Current(arg_MessagesRx)) {
            // Chart: '<Root>/SupervisorFSM_RX'
            arg_Targets->motorcurrent = arg_ControlOutputs->Iq_fbk;
            SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
              SupervisorFSM_RX_IN_Current;

            // Chart: '<Root>/SupervisorFSM_RX'
            arg_Flags->control_mode = ControlModes_Current;
          } else if (SupervisorFS_IsNewCtrl_Position(arg_MessagesRx)) {
            SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
              SupervisorFSM_RX_IN_Position;

            // Chart: '<Root>/SupervisorFSM_RX'
            arg_Flags->control_mode = ControlModes_Position;
            arg_Targets->jointpositions = arg_SensorsData->jointpositions;
          } else if (SupervisorFSM_RX_IsNewCtrl_Idle(arg_MessagesRx)) {
            SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
              SupervisorFSM_RX_IN_Idle;

            // Chart: '<Root>/SupervisorFSM_RX'
            arg_Flags->control_mode = ControlModes_Idle;
            rtw_disableMotor();
          } else {
            guard3 = true;
          }
        } else {
          guard3 = true;
        }
      }
      break;

     case SupervisorFSM_RX_IN_Voltage:
      if (SupervisorFSM_RX_DW.sfEvent == Superviso_event_MotorFaultEvent) {
        SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
          SupervisorFSM_RX_IN_HWFault;

        // Chart: '<Root>/SupervisorFSM_RX'
        arg_Flags->control_mode = ControlModes_HwFaultCM;
        rtw_disableMotor();
      } else if ((SupervisorFSM_RX_DW.sfEvent == Super_event_setControlModeEvent)
                 && SupervisorFS_IsNewCtrl_Velocity(arg_MessagesRx)) {
        if ((!SupervisorFSM_IsNewCtrl_Voltage(arg_MessagesRx)) &&
            SupervisorFS_IsNewCtrl_Velocity(arg_MessagesRx)) {
          SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
            SupervisorFSM_RX_IN_Velocity;

          // Chart: '<Root>/SupervisorFSM_RX'
          arg_Flags->control_mode = ControlModes_Velocity;
          arg_Targets->jointvelocities.velocity = 0.0F;
        } else {
          guard4 = true;
        }
      } else if (SupervisorFSM_RX_DW.sfEvent == Super_event_setControlModeEvent)
      {
        if (!SupervisorFSM_IsNewCtrl_Voltage(arg_MessagesRx)) {
          if (SupervisorFSM_IsNewCtrl_Current(arg_MessagesRx)) {
            // Chart: '<Root>/SupervisorFSM_RX'
            arg_Targets->motorcurrent = arg_ControlOutputs->Iq_fbk;
            SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
              SupervisorFSM_RX_IN_Current;

            // Chart: '<Root>/SupervisorFSM_RX'
            arg_Flags->control_mode = ControlModes_Current;
          } else if (SupervisorFS_IsNewCtrl_Position(arg_MessagesRx)) {
            SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
              SupervisorFSM_RX_IN_Position;

            // Chart: '<Root>/SupervisorFSM_RX'
            arg_Flags->control_mode = ControlModes_Position;
            arg_Targets->jointpositions = arg_SensorsData->jointpositions;
          } else if (SupervisorFSM_RX_IsNewCtrl_Idle(arg_MessagesRx)) {
            SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
              SupervisorFSM_RX_IN_Idle;

            // Chart: '<Root>/SupervisorFSM_RX'
            arg_Flags->control_mode = ControlModes_Idle;
            rtw_disableMotor();
          } else {
            guard4 = true;
          }
        } else {
          guard4 = true;
        }
      }
      break;
    }

    if (guard4) {
      SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER = SupervisorFSM_RX_IN_Voltage;

      // Chart: '<Root>/SupervisorFSM_RX'
      arg_Flags->control_mode = ControlModes_Voltage;
    }

    if (guard3) {
      SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER = SupervisorFSM_RX_IN_Velocity;

      // Chart: '<Root>/SupervisorFSM_RX'
      arg_Flags->control_mode = ControlModes_Velocity;
      arg_Targets->jointvelocities.velocity = 0.0F;
    }

    if (guard2) {
      if (SupervisorFSM_RX_DW.sfEvent == Super_event_setControlModeEvent) {
        SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER = SupervisorFSM_RX_IN_Idle;

        // Chart: '<Root>/SupervisorFSM_RX'
        arg_Flags->control_mode = ControlModes_Idle;
        rtw_disableMotor();
      }
    }

    if (guard1) {
      SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER = SupervisorFSM_RX_IN_Idle;

      // Chart: '<Root>/SupervisorFSM_RX'
      arg_Flags->control_mode = ControlModes_Idle;
      rtw_disableMotor();
    }
  }

  // Function for Chart: '<Root>/SupervisorFSM_RX'
  boolean_T SupervisorFSM_RX::SupervisorFSM_isBoardConfigured(void)
  {
    return true;
  }

  // Function for Chart: '<Root>/SupervisorFSM_RX'
  real32_T SupervisorFSM_RX::SupervisorFSM_RX_ConvertPid(real32_T in, uint8_T
    shift)
  {
    uint64m_T tmp;
    uint32_T qY;
    static const uint64m_T tmp_0 = { { 1U, 0U }// chunks
    };

    qY = 15U - shift;
    if (15U - shift > 15U) {
      qY = 0U;
    }

    uMultiWordShl(&tmp_0.chunks[0U], 2, qY, &tmp.chunks[0U], 2);
    return in * static_cast<real32_T>(uMultiWord2Double(&tmp.chunks[0U], 2, 0)) /
      32768.0F;
  }
}

namespace amc_bldc_codegen
{
  // System initialize for referenced model: 'SupervisorFSM_RX'
  void SupervisorFSM_RX::init(Flags *arg_Flags, Targets *arg_Targets,
    ConfigurationParameters *arg_Output)
  {
    // SystemInitialize for Chart: '<Root>/SupervisorFSM_RX'
    SupervisorFSM_RX_DW.sfEvent = SupervisorFSM_RX_CALL_EVENT;
    arg_Flags->control_mode = ControlModes_NotConfigured;
    arg_Targets->jointpositions.position = 0.0F;
    arg_Targets->jointvelocities.velocity = 0.0F;
    arg_Targets->motorcurrent.current = 0.0F;
    arg_Targets->motorvoltage.voltage = 0.0F;
    arg_Output->motorconfig.Imin = 0.0F;
    arg_Output->motorconfig.Imax = 0.0F;
    arg_Output->motorconfig.has_hall_sens = false;
    arg_Output->motorconfig.pole_pairs = 0U;
    arg_Output->motorconfig.reduction = 0.0F;
    arg_Output->motorconfig.Kp = 0.0F;
    arg_Output->motorconfig.Ki = 0.0F;
    arg_Output->motorconfig.Kd = 0.0F;
    arg_Output->motorconfig.Ks = 0U;
    arg_Output->motorconfig.Kbemf = 0.0F;
    arg_Output->motorconfig.Rphase = 0.0F;
    arg_Output->motorconfig.Vmax = 0.0F;
    arg_Output->motorconfig.Vcc = 0.0F;
    arg_Output->estimationconfig.velocity_mode =
      EstimationVelocityModes_Disabled;
    arg_Output->PosLoopPID.OutMax = 0.0F;
    arg_Output->PosLoopPID.OutMin = 0.0F;
    arg_Output->PosLoopPID.P = 0.0F;
    arg_Output->PosLoopPID.I = 0.0F;
    arg_Output->PosLoopPID.D = 0.0F;
    arg_Output->PosLoopPID.N = 0.0F;
    arg_Output->PosLoopPID.I0 = 0.0F;
    arg_Output->PosLoopPID.D0 = 0.0F;
    arg_Output->VelLoopPID.OutMax = 0.0F;
    arg_Output->VelLoopPID.OutMin = 0.0F;
    arg_Output->VelLoopPID.P = 0.0F;
    arg_Output->VelLoopPID.I = 0.0F;
    arg_Output->VelLoopPID.D = 0.0F;
    arg_Output->VelLoopPID.N = 0.0F;
    arg_Output->VelLoopPID.I0 = 0.0F;
    arg_Output->VelLoopPID.D0 = 0.0F;
    arg_Output->DirLoopPID.OutMax = 0.0F;
    arg_Output->DirLoopPID.OutMin = 0.0F;
    arg_Output->DirLoopPID.P = 0.0F;
    arg_Output->DirLoopPID.I = 0.0F;
    arg_Output->DirLoopPID.D = 0.0F;
    arg_Output->DirLoopPID.N = 0.0F;
    arg_Output->DirLoopPID.I0 = 0.0F;
    arg_Output->DirLoopPID.D0 = 0.0F;
    arg_Output->thresholds.jntPosMin = 0.0F;
    arg_Output->thresholds.jntPosMax = 0.0F;
    arg_Output->thresholds.hardwareJntPosMin = 0.0F;
    arg_Output->thresholds.hardwareJntPosMax = 0.0F;
    arg_Output->thresholds.rotorPosMin = 0.0F;
    arg_Output->thresholds.rotorPosMax = 0.0F;
    arg_Output->thresholds.jntVelMax = 0.0F;
    arg_Output->thresholds.velocityTimeout = 0U;
    arg_Output->thresholds.motorNominalCurrents = 0.0F;
    arg_Output->thresholds.motorPeakCurrents = 0.0F;
    arg_Output->thresholds.motorOverloadCurrents = 0.0F;
    arg_Output->thresholds.motorPwmLimit = 0U;
  }

  // Output and update for referenced model: 'SupervisorFSM_RX'
  void SupervisorFSM_RX::step(const BUS_EVENTS_RX &arg_EventsRx, const
    BUS_MESSAGES_RX &arg_MessagesRx, const BUS_CAN_RX_ERRORS &arg_ErrorsRx,
    const EstimatedData &arg_EstimatedData, const SensorsData &arg_SensorsData,
    const ControlOutputs &arg_ControlOutputs, Targets &arg_Targets,
    ConfigurationParameters &arg_Output, Flags &arg_Flags, ExternalFlags
    *arg_SensorsData1)
  {
    // Chart: '<Root>/SupervisorFSM_RX'
    SupervisorFSM_RX_DW.sfEvent = SupervisorFSM_RX_CALL_EVENT;
    SupervisorFSM_RX_DW.EventsRx_control_mode_prev =
      SupervisorFSM_RX_DW.EventsRx_control_mode_start;
    SupervisorFSM_RX_DW.EventsRx_control_mode_start = arg_EventsRx.control_mode;
    SupervisorFSM_RX_DW.EventsRx_current_pid_prev =
      SupervisorFSM_RX_DW.EventsRx_current_pid_start;
    SupervisorFSM_RX_DW.EventsRx_current_pid_start = arg_EventsRx.current_pid;
    SupervisorFSM_RX_DW.ErrorsRx_event_prev =
      SupervisorFSM_RX_DW.ErrorsRx_event_start;
    SupervisorFSM_RX_DW.ErrorsRx_event_start = arg_ErrorsRx.event;
    SupervisorFSM_RX_DW.EventsRx_desired_current_prev =
      SupervisorFSM_RX_DW.EventsRx_desired_current_start;
    SupervisorFSM_RX_DW.EventsRx_desired_current_start =
      arg_EventsRx.desired_current;
    SupervisorFSM_RX_DW.EventsRx_current_limit_prev =
      SupervisorFSM_RX_DW.EventsRx_current_limit_start;
    SupervisorFSM_RX_DW.EventsRx_current_limit_start =
      arg_EventsRx.current_limit;
    SupervisorFSM_RX_DW.ExternalFlags_fault_button_prev =
      SupervisorFSM_RX_DW.ExternalFlags_fault_button_star;
    SupervisorFSM_RX_DW.ExternalFlags_fault_button_star =
      arg_SensorsData1->fault_button;
    if (SupervisorFSM_RX_DW.is_active_c2_SupervisorFSM_RX == 0U) {
      SupervisorFSM_RX_DW.EventsRx_control_mode_prev = arg_EventsRx.control_mode;
      SupervisorFSM_RX_DW.EventsRx_control_mode_start =
        arg_EventsRx.control_mode;
      SupervisorFSM_RX_DW.EventsRx_current_pid_prev = arg_EventsRx.current_pid;
      SupervisorFSM_RX_DW.EventsRx_current_pid_start = arg_EventsRx.current_pid;
      SupervisorFSM_RX_DW.ErrorsRx_event_prev = arg_ErrorsRx.event;
      SupervisorFSM_RX_DW.ErrorsRx_event_start = arg_ErrorsRx.event;
      SupervisorFSM_RX_DW.EventsRx_desired_current_prev =
        arg_EventsRx.desired_current;
      SupervisorFSM_RX_DW.EventsRx_desired_current_start =
        arg_EventsRx.desired_current;
      SupervisorFSM_RX_DW.EventsRx_current_limit_prev =
        arg_EventsRx.current_limit;
      SupervisorFSM_RX_DW.EventsRx_current_limit_start =
        arg_EventsRx.current_limit;
      SupervisorFSM_RX_DW.ExternalFlags_fault_button_prev =
        arg_SensorsData1->fault_button;
      SupervisorFSM_RX_DW.ExternalFlags_fault_button_star =
        arg_SensorsData1->fault_button;
      SupervisorFSM_RX_DW.is_active_c2_SupervisorFSM_RX = 1U;
      arg_Output.motorconfig.pole_pairs = 7U;
      arg_Output.motorconfig.Kp = 2.0F;
      arg_Output.motorconfig.Ki = 500.0F;
      arg_Output.motorconfig.Kd = 0.0F;
      arg_Output.motorconfig.Ks = 0U;
      arg_Output.motorconfig.Vmax = 24.0F;
      arg_Output.motorconfig.Vcc = 24.0F;
      arg_Output.motorconfig.reduction = 1.0F;
      arg_Output.estimationconfig.velocity_mode =
        EstimationVelocityModes_MovingAverage;
      arg_Output.thresholds.jntPosMin = 1.0F;
      arg_Output.thresholds.jntPosMax = 359.0F;
      arg_Output.thresholds.hardwareJntPosMin = 0.0F;
      arg_Output.thresholds.hardwareJntPosMax = 360.0F;
      arg_Output.thresholds.rotorPosMin = 0.0F;
      arg_Output.thresholds.rotorPosMax = 0.0F;
      arg_Output.thresholds.jntVelMax = 100.0F;
      arg_Output.thresholds.velocityTimeout = 10U;
      arg_Output.thresholds.motorNominalCurrents = 100.0F;
      arg_Output.thresholds.motorPeakCurrents = 100.0F;
      arg_Output.thresholds.motorOverloadCurrents = 100.0F;
      arg_Output.thresholds.motorPwmLimit = 32000U;
      SupervisorFSM_RX_DW.is_active_CAN_RX_HANDLER = 1U;
      SupervisorFSM_RX_DW.is_active_EVENT_DISPATCHER = 1U;
      SupervisorFSM_RX_DW.is_EVENT_DISPATCHER = SupervisorFSM_RX_IN_Home;
      SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER = 1U;
      SupervisorFSM_RX_DW.is_CONTROL_MODE_HANDLER =
        SupervisorFSM__IN_NotConfigured;
      arg_Flags.control_mode = ControlModes_NotConfigured;
      rtw_disableMotor();
      SupervisorFSM_RX_DW.is_active_LIMITS_HANDLER = 1U;
      SupervisorFSM_RX_DW.is_LIMITS_HANDLER = SupervisorFSM_RX_IN_Home;
      SupervisorFSM_RX_DW.is_active_STATE_HANDLER = 1U;
      SupervisorFSM_RX_DW.is_STATE_HANDLER = SupervisorFS_IN_NotConfigured_o;
      SupervisorFSM_RX_DW.BoardSt = BoardState_NotConfigured;
      SupervisorFSM_RX_DW.is_active_FAULT_HANDLER = 1U;
      SupervisorFSM_RX_DW.is_active_FaultButtonPressed = 1U;
      SupervisorFSM_RX_DW.is_FaultButtonPressed = SupervisorFSM_RX_IN_NoFault;
      SupervisorFSM_RX_DW.isFaultButtonPressed = false;

      // MotorFaultFlags.FautltButtonPressed=0;
      SupervisorFSM_RX_DW.is_active_OverCurrent = 1U;
      SupervisorFSM_RX_DW.is_OverCurrent = Superviso_IN_LimitNonConfigured;
    } else {
      int32_T c_previousEvent;
      if (SupervisorFSM_RX_DW.is_active_CAN_RX_HANDLER != 0U) {
        if ((SupervisorFSM_RX_DW.is_active_EVENT_DISPATCHER != 0U) &&
            (SupervisorFSM_RX_DW.is_EVENT_DISPATCHER == 1) &&
            (SupervisorFSM_RX_DW.ErrorsRx_event_prev ==
             SupervisorFSM_RX_DW.ErrorsRx_event_start)) {
          if (SupervisorFSM_RX_DW.EventsRx_desired_current_prev !=
              SupervisorFSM_RX_DW.EventsRx_desired_current_start) {
            arg_Targets.motorcurrent.current =
              arg_MessagesRx.desired_current.current;
            SupervisorFSM_RX_DW.is_EVENT_DISPATCHER = SupervisorFSM_RX_IN_Home;
          } else if (SupervisorFSM_RX_DW.EventsRx_control_mode_prev !=
                     SupervisorFSM_RX_DW.EventsRx_control_mode_start) {
            c_previousEvent = SupervisorFSM_RX_DW.sfEvent;
            SupervisorFSM_RX_DW.sfEvent = Super_event_setControlModeEvent;
            if (SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER != 0U) {
              Supervisor_CONTROL_MODE_HANDLER(&arg_SensorsData,
                &arg_ControlOutputs, &arg_MessagesRx, &arg_Flags, &arg_Targets);
            }

            SupervisorFSM_RX_DW.sfEvent = c_previousEvent;
            SupervisorFSM_RX_DW.is_EVENT_DISPATCHER = SupervisorFSM_RX_IN_Home;
          } else if (SupervisorFSM_RX_DW.EventsRx_current_limit_prev !=
                     SupervisorFSM_RX_DW.EventsRx_current_limit_start) {
            c_previousEvent = SupervisorFSM_RX_DW.sfEvent;
            SupervisorFSM_RX_DW.sfEvent = SupervisorF_event_setLimitEvent;
            if ((SupervisorFSM_RX_DW.is_active_LIMITS_HANDLER != 0U) &&
                (SupervisorFSM_RX_DW.is_LIMITS_HANDLER == 1) &&
                (SupervisorFSM_RX_DW.sfEvent == SupervisorF_event_setLimitEvent)
                && (SupervisorFSM_RX_DW.EventsRx_current_limit_prev !=
                    SupervisorFSM_RX_DW.EventsRx_current_limit_start)) {
              arg_Output.thresholds.motorOverloadCurrents = std::abs
                (arg_MessagesRx.current_limit.overload);
              SupervisorFSM_RX_DW.IsCurrentLimitSet = true;
              SupervisorFSM_RX_DW.is_LIMITS_HANDLER = SupervisorFSM_RX_IN_Home;
            }

            SupervisorFSM_RX_DW.sfEvent = c_previousEvent;
            SupervisorFSM_RX_DW.is_EVENT_DISPATCHER = SupervisorFSM_RX_IN_Home;
          } else if (SupervisorFSM_RX_DW.EventsRx_current_pid_prev !=
                     SupervisorFSM_RX_DW.EventsRx_current_pid_start) {
            arg_Output.motorconfig.Kp = SupervisorFSM_RX_ConvertPid
              (arg_MessagesRx.current_pid.Kp, arg_MessagesRx.current_pid.Ks);
            arg_Output.motorconfig.Ki = SupervisorFSM_RX_ConvertPid
              (arg_MessagesRx.current_pid.Ki, arg_MessagesRx.current_pid.Ks);
            arg_Output.motorconfig.Kd = SupervisorFSM_RX_ConvertPid
              (arg_MessagesRx.current_pid.Kd, arg_MessagesRx.current_pid.Ks);
            arg_Output.motorconfig.Ks = arg_MessagesRx.current_pid.Ks;
            SupervisorFSM_RX_DW.is_EVENT_DISPATCHER = SupervisorFSM_RX_IN_Home;
          }
        }

        if (SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER != 0U) {
          Supervisor_CONTROL_MODE_HANDLER(&arg_SensorsData, &arg_ControlOutputs,
            &arg_MessagesRx, &arg_Flags, &arg_Targets);
        }

        if ((SupervisorFSM_RX_DW.is_active_LIMITS_HANDLER != 0U) &&
            (SupervisorFSM_RX_DW.is_LIMITS_HANDLER == 1) &&
            (SupervisorFSM_RX_DW.sfEvent == SupervisorF_event_setLimitEvent) &&
            (SupervisorFSM_RX_DW.EventsRx_current_limit_prev !=
             SupervisorFSM_RX_DW.EventsRx_current_limit_start)) {
          arg_Output.thresholds.motorOverloadCurrents = std::abs
            (arg_MessagesRx.current_limit.overload);
          SupervisorFSM_RX_DW.IsCurrentLimitSet = true;
          SupervisorFSM_RX_DW.is_LIMITS_HANDLER = SupervisorFSM_RX_IN_Home;
        }
      }

      if (SupervisorFSM_RX_DW.is_active_STATE_HANDLER != 0U) {
        switch (SupervisorFSM_RX_DW.is_STATE_HANDLER) {
         case SupervisorFSM_RX_IN_Configured:
          if (SupervisorFSM_RX_DW.sfEvent == Superviso_event_BoardFaultEvent) {
            SupervisorFSM_RX_DW.is_STATE_HANDLER = SupervisorFSM_RX_IN_Fault;
            SupervisorFSM_RX_DW.BoardSt = BoardState_Fault;
          }
          break;

         case SupervisorFSM_RX_IN_Fault:
          if (SupervisorFSM_RX_DW.sfEvent == Supe_event_OutOfBoardFaultEvent) {
            if (!SupervisorFSM_isBoardConfigured()) {
              SupervisorFSM_RX_DW.is_STATE_HANDLER =
                SupervisorFS_IN_NotConfigured_o;
              SupervisorFSM_RX_DW.BoardSt = BoardState_NotConfigured;
            } else {
              SupervisorFSM_RX_DW.is_STATE_HANDLER =
                SupervisorFSM_RX_IN_Configured;
              SupervisorFSM_RX_DW.BoardSt = BoardState_Configured;
            }
          }
          break;

         case SupervisorFS_IN_NotConfigured_o:
          if (SupervisorFSM_RX_DW.sfEvent == Superviso_event_BoardFaultEvent) {
            SupervisorFSM_RX_DW.is_STATE_HANDLER = SupervisorFSM_RX_IN_Fault;
            SupervisorFSM_RX_DW.BoardSt = BoardState_Fault;
          } else if (SupervisorFSM_isBoardConfigured()) {
            SupervisorFSM_RX_DW.is_STATE_HANDLER =
              SupervisorFSM_RX_IN_Configured;
            SupervisorFSM_RX_DW.BoardSt = BoardState_Configured;
          }
          break;
        }
      }

      if (SupervisorFSM_RX_DW.is_active_FAULT_HANDLER != 0U) {
        if (SupervisorFSM_RX_DW.is_active_FaultButtonPressed != 0U) {
          switch (SupervisorFSM_RX_DW.is_FaultButtonPressed) {
           case SupervisorFSM__IN_ButtonPressed:
            if ((SupervisorFSM_RX_DW.ExternalFlags_fault_button_prev !=
                 SupervisorFSM_RX_DW.ExternalFlags_fault_button_star) &&
                (!SupervisorFSM_RX_DW.ExternalFlags_fault_button_star)) {
              SupervisorFSM_RX_DW.is_FaultButtonPressed =
                SupervisorFSM_RX_IN_NoFault;
              SupervisorFSM_RX_DW.isFaultButtonPressed = false;

              // MotorFaultFlags.FautltButtonPressed=0;
            }
            break;

           case SupervisorFSM_RX_IN_NoFault:
            if ((SupervisorFSM_RX_DW.ExternalFlags_fault_button_prev !=
                 SupervisorFSM_RX_DW.ExternalFlags_fault_button_star) &&
                SupervisorFSM_RX_DW.ExternalFlags_fault_button_star) {
              SupervisorFSM_RX_DW.is_FaultButtonPressed =
                SupervisorFSM__IN_ButtonPressed;

              // MotorFaultFlags.FautltButtonPressed=1;
              SupervisorFSM_RX_DW.isFaultButtonPressed = true;
              c_previousEvent = SupervisorFSM_RX_DW.sfEvent;
              SupervisorFSM_RX_DW.sfEvent = Superviso_event_MotorFaultEvent;
              if (SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER != 0U) {
                Supervisor_CONTROL_MODE_HANDLER(&arg_SensorsData,
                  &arg_ControlOutputs, &arg_MessagesRx, &arg_Flags, &arg_Targets);
              }

              SupervisorFSM_RX_DW.sfEvent = c_previousEvent;
            }
            break;
          }
        }

        if (SupervisorFSM_RX_DW.is_active_OverCurrent != 0U) {
          switch (SupervisorFSM_RX_DW.is_OverCurrent) {
           case Superviso_IN_LimitNonConfigured:
            if (SupervisorFS_isConfigurationSet()) {
              SupervisorFSM_RX_DW.is_OverCurrent = SupervisorFSM_RX_IN_NoFault;
              SupervisorFSM_RX_DW.isInOverCurrent = false;

              // MotorFaultFlags.overCurrent=0;
            }
            break;

           case SupervisorFSM_RX_IN_NoFault:
            if (std::abs(arg_ControlOutputs.Iq_fbk.current) >=
                arg_Output.thresholds.motorOverloadCurrents) {
              SupervisorFSM_RX_DW.is_OverCurrent =
                SupervisorF_IN_OverCurrentFault;

              // MotorFaultFlags.overCurrent=1;
              SupervisorFSM_RX_DW.isInOverCurrent = true;
              c_previousEvent = SupervisorFSM_RX_DW.sfEvent;
              SupervisorFSM_RX_DW.sfEvent = Superviso_event_MotorFaultEvent;
              if (SupervisorFSM_RX_DW.is_active_CONTROL_MODE_HANDLER != 0U) {
                Supervisor_CONTROL_MODE_HANDLER(&arg_SensorsData,
                  &arg_ControlOutputs, &arg_MessagesRx, &arg_Flags, &arg_Targets);
              }

              SupervisorFSM_RX_DW.sfEvent = c_previousEvent;
            }
            break;

           case SupervisorF_IN_OverCurrentFault:
            if (std::abs(arg_ControlOutputs.Iq_fbk.current) <
                arg_Output.thresholds.motorOverloadCurrents) {
              SupervisorFSM_RX_DW.is_OverCurrent = SupervisorFSM_RX_IN_NoFault;
              SupervisorFSM_RX_DW.isInOverCurrent = false;

              // MotorFaultFlags.overCurrent=0;
            }
            break;
          }
        }
      }
    }

    // End of Chart: '<Root>/SupervisorFSM_RX'
  }

  // Constructor
  SupervisorFSM_RX::SupervisorFSM_RX() :
    SupervisorFSM_RX_DW(),
    SupervisorFSM_RX_M()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  SupervisorFSM_RX::~SupervisorFSM_RX()
  {
    // Currently there is no destructor body generated.
  }

  // Real-Time Model get method
  amc_bldc_codegen::SupervisorFSM_RX::RT_MODEL_SupervisorFSM_RX_T
    * SupervisorFSM_RX::getRTM()
  {
    return (&SupervisorFSM_RX_M);
  }

  // member function to setup error status pointer
  void SupervisorFSM_RX::setErrorStatusPointer(const char_T **rt_errorStatus)
  {
    rtmSetErrorStatusPointer((&SupervisorFSM_RX_M), rt_errorStatus);
  }
}

//
// File trailer for generated code.
//
// [EOF]
//
