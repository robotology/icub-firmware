//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_decoder.cpp
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 6.115
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Tue Apr 16 11:28:54 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "can_decoder.h"
#include "rtwtypes.h"
#include "can_decoder_types.h"
#include <cstring>
#include <stddef.h>
#include "rtw_defines.h"
#include "can_decoder_private.h"

// Named constants for Chart: '<S2>/Decoding Logic'
const int32_T can_decoder_CALL_EVENT = -1;
const uint8_T can_decoder_IN_Event_Error = 1U;
const uint8_T can_decoder_IN_Home = 1U;
const uint8_T can_decoder_IN_Home_p = 2U;
const int32_T can_decoder_event_ev_error_mode_unrecognized = 0;
const int32_T can_decoder_event_ev_error_pck_malformed = 1;
const int32_T can_decoder_event_ev_error_pck_not4us = 2;

// Forward declaration for local functions
static int32_T can_decoder_safe_cast_to_MCStreaming(int32_T input);
static void can_decoder_ERROR_HANDLING(boolean_T rtu_pck_available,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW);
static int16_T can_decoder_merge_2bytes_signed(uint16_T bl, uint16_T bh);
static uint16_T can_decoder_merge_2bytes_unsigned(uint16_T bl, uint16_T bh);
static void can_decoder_SET_OPTIONS(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW);
static boolean_T can_decoder_is_controlmode_recognized(int32_T mode);
static int32_T can_decoder_safe_cast_to_MCControlModes(int32_T input);
static ControlModes can_decoder_convertMode(MCControlModes MCMode);

// Forward declaration for local functions
static CANClassTypes can_decoder_convert_to_enum_CANClassTypes(int32_T input);
static int32_T can_decoder_safe_cast_to_MCStreaming(int32_T input)
{
  int32_T output;

  // Initialize output value to default value for MCStreaming (Desired_Targets)
  output = 15;
  if ((input == 0) || (input == 15)) {
    // Set output value to input value if it is a member of MCStreaming
    output = input;
  }

  return output;
}

// Function for Chart: '<S2>/Decoding Logic'
static void can_decoder_ERROR_HANDLING(boolean_T rtu_pck_available,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW)
{
  boolean_T guard1;
  guard1 = false;
  switch (localDW->is_ERROR_HANDLING) {
   case can_decoder_IN_Event_Error:
    localDW->is_ERROR_HANDLING = can_decoder_IN_Home_p;
    localDW->cmd_processed = 0U;
    break;

   case can_decoder_IN_Home_p:
    if (localDW->sfEvent == can_decoder_event_ev_error_pck_not4us) {
      localB->error_type = CANErrorTypes_Packet_Not4Us;
      localDW->ev_errorEventCounter++;
      guard1 = true;
    } else if (localDW->sfEvent == can_decoder_event_ev_error_pck_malformed) {
      localB->error_type = CANErrorTypes_Packet_Malformed;
      localDW->ev_errorEventCounter++;
      guard1 = true;
    } else if (localDW->sfEvent == can_decoder_event_ev_error_mode_unrecognized)
    {
      localB->error_type = CANErrorTypes_Mode_Unrecognized;
      localDW->ev_errorEventCounter++;
      guard1 = true;
    } else {
      if (!localDW->ev_async) {
        if (rtu_pck_available && (localDW->cmd_processed == 0)) {
          localB->error_type = CANErrorTypes_Packet_Unrecognized;
          localDW->ev_errorEventCounter++;
        } else if (localDW->cmd_processed > 1) {
          localB->error_type = CANErrorTypes_Packet_MultiFunctionsDetected;
          localDW->ev_errorEventCounter++;
        } else {
          localB->error_type = CANErrorTypes_No_Error;
        }
      }

      localDW->ev_async = false;
      localDW->is_ERROR_HANDLING = can_decoder_IN_Home_p;
      localDW->cmd_processed = 0U;
    }
    break;
  }

  if (guard1) {
    localDW->is_ERROR_HANDLING = can_decoder_IN_Event_Error;
    localDW->ev_async = true;
  }
}

// Function for Chart: '<S2>/Decoding Logic'
static int16_T can_decoder_merge_2bytes_signed(uint16_T bl, uint16_T bh)
{
  int16_T sw;
  uint16_T x;
  x = static_cast<uint16_T>(static_cast<uint16_T>(bh << 8) | bl);
  std::memcpy((void *)&sw, (void *)&x, (size_t)1 * sizeof(int16_T));
  return sw;
}

// Function for Chart: '<S2>/Decoding Logic'
static uint16_T can_decoder_merge_2bytes_unsigned(uint16_T bl, uint16_T bh)
{
  return static_cast<uint16_T>(static_cast<uint16_T>(bh << 8) | bl);
}

// Function for Chart: '<S2>/Decoding Logic'
static void can_decoder_SET_OPTIONS(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW)
{
  int32_T b_previousEvent;
  real32_T c;
  boolean_T guard1;
  if ((localDW->is_SET_OPTIONS == can_decoder_IN_Home) && (rtu_pck_available &&
       (rtu_pck_input->ID.CLS == CANClassTypes_Motor_Control_Command))) {
    if (rtu_pck_input->ID.DST_TYP == rtu_CAN_ID_DST) {
      if (rtu_pck_input->PAYLOAD.LEN >= 1) {
        guard1 = false;
        if (rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
            (MCOPC_Set_Current_Limit)) {
          if (rtu_pck_input->PAYLOAD.LEN == 8) {
            //  msg_set_current_limit.motor = pck_input.PAYLOAD.CMD.M;
            localB->msg_set_current_limit.nominal = 0.001F *
              static_cast<real32_T>(can_decoder_merge_2bytes_signed(static_cast<
              uint16_T>(rtu_pck_input->PAYLOAD.ARG[2]), static_cast<uint16_T>
              (rtu_pck_input->PAYLOAD.ARG[3])));
            localB->msg_set_current_limit.peak = 0.001F * static_cast<real32_T>
              (can_decoder_merge_2bytes_unsigned(static_cast<uint16_T>
                (rtu_pck_input->PAYLOAD.ARG[4]), static_cast<uint16_T>
                (rtu_pck_input->PAYLOAD.ARG[5])));
            localB->msg_set_current_limit.overload = 0.001F *
              static_cast<real32_T>(can_decoder_merge_2bytes_unsigned(
              static_cast<uint16_T>(rtu_pck_input->PAYLOAD.ARG[6]), static_cast<
              uint16_T>(rtu_pck_input->PAYLOAD.ARG[7])));
            localDW->cmd_processed = static_cast<uint16_T>
              (localDW->cmd_processed + 1);
            localB->event_type = EventTypes_SetLimit;
          } else {
            guard1 = true;
          }
        } else if ((rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
                    (MCOPC_Set_Current_PID)) || (rtu_pck_input->PAYLOAD.CMD.OPC ==
                    static_cast<int32_T>(MCOPC_Set_Velocity_PID))) {
          if (rtu_pck_input->PAYLOAD.LEN == 8) {
            //  msg_set_pid.motor = pck_input.PAYLOAD.CMD.M;
            localB->msg_set_pid.shift_factor = rtu_pck_input->PAYLOAD.ARG[7];
            c = static_cast<real32_T>(0x01 << (15 -
              localB->msg_set_pid.shift_factor)) / 32768.0F;
            localB->msg_set_pid.P = c * static_cast<real32_T>
              (can_decoder_merge_2bytes_signed(static_cast<uint16_T>
                (rtu_pck_input->PAYLOAD.ARG[1]), static_cast<uint16_T>
                (rtu_pck_input->PAYLOAD.ARG[2])));
            localB->msg_set_pid.I = c * static_cast<real32_T>
              (can_decoder_merge_2bytes_signed(static_cast<uint16_T>
                (rtu_pck_input->PAYLOAD.ARG[3]), static_cast<uint16_T>
                (rtu_pck_input->PAYLOAD.ARG[4])));
            localB->msg_set_pid.D = c * static_cast<real32_T>
              (can_decoder_merge_2bytes_signed(static_cast<uint16_T>
                (rtu_pck_input->PAYLOAD.ARG[5]), static_cast<uint16_T>
                (rtu_pck_input->PAYLOAD.ARG[6])));
            localB->msg_set_pid.type = ControlModes_Current;
            localDW->cmd_processed = static_cast<uint16_T>
              (localDW->cmd_processed + 1);
            if (rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
                (MCOPC_Set_Current_PID)) {
              localB->event_type = EventTypes_SetPid;
            } else {
              localB->msg_set_pid.P *= 0.001F;
              localB->msg_set_pid.I *= 0.001F;
              localB->msg_set_pid.D *= 0.001F;
              localB->msg_set_pid.type = ControlModes_Velocity;
              localB->event_type = EventTypes_SetPid;
            }
          } else {
            guard1 = true;
          }
        }

        if (guard1) {
          b_previousEvent = localDW->sfEvent;
          localDW->sfEvent = can_decoder_event_ev_error_pck_malformed;
          if (localDW->is_active_ERROR_HANDLING != 0U) {
            can_decoder_ERROR_HANDLING(true, localB, localDW);
          }

          localDW->sfEvent = b_previousEvent;
          localDW->is_SET_OPTIONS = can_decoder_IN_Home;
        }
      } else {
        b_previousEvent = localDW->sfEvent;
        localDW->sfEvent = can_decoder_event_ev_error_pck_malformed;
        if (localDW->is_active_ERROR_HANDLING != 0U) {
          can_decoder_ERROR_HANDLING(true, localB, localDW);
        }

        localDW->sfEvent = b_previousEvent;
        localDW->is_SET_OPTIONS = can_decoder_IN_Home;
      }
    } else {
      b_previousEvent = localDW->sfEvent;
      localDW->sfEvent = can_decoder_event_ev_error_pck_not4us;
      if (localDW->is_active_ERROR_HANDLING != 0U) {
        can_decoder_ERROR_HANDLING(true, localB, localDW);
      }

      localDW->sfEvent = b_previousEvent;
      localDW->is_SET_OPTIONS = can_decoder_IN_Home;
    }
  }
}

// Function for Chart: '<S2>/Decoding Logic'
static boolean_T can_decoder_is_controlmode_recognized(int32_T mode)
{
  return (mode == static_cast<int32_T>(MCControlModes_Idle)) || (mode ==
    static_cast<int32_T>(MCControlModes_OpenLoop)) || (mode ==
    static_cast<int32_T>(MCControlModes_SpeedVoltage)) || (mode ==
    static_cast<int32_T>(MCControlModes_Current));
}

static int32_T can_decoder_safe_cast_to_MCControlModes(int32_T input)
{
  int32_T output;

  // Initialize output value to default value for MCControlModes (Idle)
  output = 0;
  if ((input == 0) || (input == 6) || ((input >= 10) && (input <= 11)) || (input
       == 80) || (input == 160) || (input == 176)) {
    // Set output value to input value if it is a member of MCControlModes
    output = input;
  }

  return output;
}

// Function for Chart: '<S2>/Decoding Logic'
static ControlModes can_decoder_convertMode(MCControlModes MCMode)
{
  ControlModes mode;
  switch (MCMode) {
   case MCControlModes_Idle:
    mode = ControlModes_Idle;
    break;

   case MCControlModes_Current:
    mode = ControlModes_Current;
    break;

   case MCControlModes_SpeedVoltage:
    mode = ControlModes_Velocity;
    break;

   case MCControlModes_OpenLoop:
    mode = ControlModes_Voltage;
    break;

   default:
    mode = ControlModes_Idle;
    break;
  }

  return mode;
}

// System initialize for atomic system: '<S2>/Decoding Logic'
void can_decoder_DecodingLogic_Init(DW_DecodingLogic_can_decoder_T *localDW)
{
  localDW->sfEvent = can_decoder_CALL_EVENT;
}

// Output and update for atomic system: '<S2>/Decoding Logic'
void can_decoder_DecodingLogic(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, const ActuatorConfiguration
  *rtu_ConfigurationParameters, uint8_T rtu_CAN_ID_DST, uint8_T
  rtu_CAN_VOLT_REF_SHIFT, real32_T rtu_CAN_VOLT_REF_GAIN,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW)
{
  int32_T b_previousEvent;
  int16_T tmp_merged;
  uint8_T idx;
  boolean_T tmp;
  boolean_T tmp_0;
  localDW->sfEvent = can_decoder_CALL_EVENT;
  localB->msg_set_control_mode = ControlModes_NotConfigured;
  localB->msg_set_current_limit.overload = 0.0F;
  localB->msg_set_current_limit.peak = 0.0F;
  localB->msg_set_current_limit.nominal = 0.0F;
  localB->msg_set_current_limit.type = ControlModes_NotConfigured;
  localB->msg_desired_targets.position = 0.0F;
  localB->msg_desired_targets.velocity = 0.0F;
  localB->msg_desired_targets.current = 0.0F;
  localB->msg_desired_targets.voltage = 0.0F;
  localB->msg_set_pid.type = ControlModes_NotConfigured;
  localB->msg_set_pid.OutMax = 0.0F;
  localB->msg_set_pid.OutMin = 0.0F;
  localB->msg_set_pid.P = 0.0F;
  localB->msg_set_pid.I = 0.0F;
  localB->msg_set_pid.D = 0.0F;
  localB->msg_set_pid.N = 0.0F;
  localB->msg_set_pid.I0 = 0.0F;
  localB->msg_set_pid.D0 = 0.0F;
  localB->msg_set_pid.shift_factor = 0U;
  localB->msg_set_motor_config.enable_verbosity = false;
  localB->msg_set_motor_config.has_hall_sens = false;
  localB->msg_set_motor_config.has_quadrature_encoder = false;
  localB->msg_set_motor_config.has_speed_quadrature_encoder = false;
  localB->msg_set_motor_config.has_torque_sens = false;
  localB->msg_set_motor_config.encoder_tolerance = 0U;
  localB->msg_set_motor_config.pole_pairs = 0U;
  localB->msg_set_motor_config.rotor_encoder_resolution = 0;
  localB->msg_set_motor_config.rotor_index_offset = 0;
  localB->msg_set_motor_config.use_index = false;
  localB->error_type = CANErrorTypes_No_Error;
  localB->event_type = EventTypes_None;

  // Chart: '<S2>/Decoding Logic'
  // This state chart is responsible for decoding incoming CAN packets.
  if (localDW->is_active_c3_can_decoder == 0U) {
    localDW->is_active_c3_can_decoder = 1U;
    localDW->is_active_SET_CONTROL_MODE = 1U;
    localDW->is_SET_CONTROL_MODE = can_decoder_IN_Home;
    localDW->is_active_DESIRED_TARGETS = 1U;
    localDW->is_DESIRED_TARGETS = can_decoder_IN_Home;
    localDW->is_active_SET_OPTIONS = 1U;
    localDW->is_SET_OPTIONS = can_decoder_IN_Home;
    localDW->is_active_SET_MOTOR_CONFIG = 1U;
    localDW->is_SET_MOTOR_CONFIG = can_decoder_IN_Home;
    localDW->is_active_ERROR_HANDLING = 1U;
    localDW->ev_async = false;
    localDW->is_ERROR_HANDLING = can_decoder_IN_Home_p;
    localDW->cmd_processed = 0U;
  } else {
    tmp = !rtu_pck_available;
    tmp_0 = (tmp || (rtu_pck_input->ID.CLS !=
                     CANClassTypes_Motor_Control_Command));
    if ((localDW->is_active_SET_CONTROL_MODE != 0U) &&
        (localDW->is_SET_CONTROL_MODE == can_decoder_IN_Home) && (!tmp_0)) {
      if (rtu_pck_input->ID.DST_TYP == rtu_CAN_ID_DST) {
        if (rtu_pck_input->PAYLOAD.LEN >= 1) {
          if (rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
              (MCOPC_Set_Control_Mode)) {
            if (rtu_pck_input->PAYLOAD.LEN >= 2) {
              if (can_decoder_is_controlmode_recognized(static_cast<int32_T>
                   (rtu_pck_input->PAYLOAD.ARG[1]))) {
                //  msg_set_control_mode.motor = pck_input.PAYLOAD.CMD.M;
                localB->msg_set_control_mode = can_decoder_convertMode(
                  static_cast<MCControlModes>
                  (can_decoder_safe_cast_to_MCControlModes
                   (rtu_pck_input->PAYLOAD.ARG[1])));
                localDW->cmd_processed = static_cast<uint16_T>
                  (localDW->cmd_processed + 1);
                localB->event_type = EventTypes_SetControlMode;
              } else {
                localDW->sfEvent = can_decoder_event_ev_error_mode_unrecognized;
                if (localDW->is_active_ERROR_HANDLING != 0U) {
                  can_decoder_ERROR_HANDLING(true, localB, localDW);
                }

                localDW->sfEvent = -1;
                localDW->is_SET_CONTROL_MODE = can_decoder_IN_Home;
              }
            } else {
              localDW->sfEvent = can_decoder_event_ev_error_pck_malformed;
              if (localDW->is_active_ERROR_HANDLING != 0U) {
                can_decoder_ERROR_HANDLING(true, localB, localDW);
              }

              localDW->sfEvent = -1;
              localDW->is_SET_CONTROL_MODE = can_decoder_IN_Home;
            }
          }
        } else {
          localDW->sfEvent = can_decoder_event_ev_error_pck_malformed;
          if (localDW->is_active_ERROR_HANDLING != 0U) {
            can_decoder_ERROR_HANDLING(true, localB, localDW);
          }

          localDW->sfEvent = -1;
          localDW->is_SET_CONTROL_MODE = can_decoder_IN_Home;
        }
      } else {
        localDW->sfEvent = can_decoder_event_ev_error_pck_not4us;
        if (localDW->is_active_ERROR_HANDLING != 0U) {
          can_decoder_ERROR_HANDLING(true, localB, localDW);
        }

        localDW->sfEvent = -1;
        localDW->is_SET_CONTROL_MODE = can_decoder_IN_Home;
      }
    }

    if ((localDW->is_active_DESIRED_TARGETS != 0U) &&
        (localDW->is_DESIRED_TARGETS == can_decoder_IN_Home) && ((!tmp) &&
         (rtu_pck_input->ID.CLS == CANClassTypes_Motor_Control_Streaming) &&
         (can_decoder_safe_cast_to_MCStreaming(rtu_pck_input->ID.DST_TYP) ==
          static_cast<int32_T>(MCStreaming_Desired_Targets)))) {
      if ((rtu_pck_input->PAYLOAD.LEN == 8) && (rtu_CAN_ID_DST <= 4)) {
        idx = static_cast<uint8_T>((rtu_CAN_ID_DST - 1) << 1);
        tmp_merged = can_decoder_merge_2bytes_signed(static_cast<uint16_T>
          (rtu_pck_input->PAYLOAD.ARG[idx]), static_cast<uint16_T>
          (rtu_pck_input->PAYLOAD.ARG[idx + 1]));
        localB->msg_desired_targets.current = 0.001F * static_cast<real32_T>
          (tmp_merged);
        localB->msg_desired_targets.voltage = static_cast<real32_T>
          (static_cast<int16_T>(tmp_merged >> (rtu_CAN_VOLT_REF_SHIFT -
             rtu_ConfigurationParameters->pids.currentPID.shift_factor))) /
          rtu_CAN_VOLT_REF_GAIN;
        localB->msg_desired_targets.velocity = 1000.0F * static_cast<real32_T>
          (tmp_merged) * CAN_ANGLE_ICUB2DEG;
        localDW->cmd_processed = static_cast<uint16_T>(localDW->cmd_processed +
          1);
        localB->event_type = EventTypes_SetTarget;
      } else {
        localDW->sfEvent = can_decoder_event_ev_error_pck_malformed;
        if (localDW->is_active_ERROR_HANDLING != 0U) {
          can_decoder_ERROR_HANDLING(true, localB, localDW);
        }

        localDW->sfEvent = -1;
        localDW->is_DESIRED_TARGETS = can_decoder_IN_Home;
      }
    }

    if (localDW->is_active_SET_OPTIONS != 0U) {
      can_decoder_SET_OPTIONS(rtu_pck_available, rtu_pck_input, rtu_CAN_ID_DST,
        localB, localDW);
    }

    if ((localDW->is_active_SET_MOTOR_CONFIG != 0U) &&
        (localDW->is_SET_MOTOR_CONFIG == can_decoder_IN_Home) && (!tmp_0)) {
      if (rtu_pck_input->ID.DST_TYP == rtu_CAN_ID_DST) {
        if (rtu_pck_input->PAYLOAD.LEN >= 1) {
          if (rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
              (MCOPC_Set_Motor_Config)) {
            if (rtu_pck_input->PAYLOAD.LEN == 8) {
              localB->msg_set_motor_config.has_quadrature_encoder =
                ((rtu_pck_input->PAYLOAD.ARG[1] & 1U) != 0U);
              localB->msg_set_motor_config.has_hall_sens =
                ((rtu_pck_input->PAYLOAD.ARG[1] & 2U) != 0U);
              localB->msg_set_motor_config.has_torque_sens =
                ((rtu_pck_input->PAYLOAD.ARG[1] & 4U) != 0U);
              localB->msg_set_motor_config.use_index =
                ((rtu_pck_input->PAYLOAD.ARG[1] & 8U) != 0U);
              localB->msg_set_motor_config.has_speed_quadrature_encoder =
                ((rtu_pck_input->PAYLOAD.ARG[1] & 16U) != 0U);
              localB->msg_set_motor_config.enable_verbosity =
                ((rtu_pck_input->PAYLOAD.ARG[1] & 32U) != 0U);
              localB->msg_set_motor_config.rotor_encoder_resolution =
                can_decoder_merge_2bytes_signed(static_cast<uint16_T>
                (rtu_pck_input->PAYLOAD.ARG[2]), static_cast<uint16_T>
                (rtu_pck_input->PAYLOAD.ARG[3]));
              localB->msg_set_motor_config.rotor_index_offset =
                can_decoder_merge_2bytes_signed(static_cast<uint16_T>
                (rtu_pck_input->PAYLOAD.ARG[4]), static_cast<uint16_T>
                (rtu_pck_input->PAYLOAD.ARG[5]));
              localB->msg_set_motor_config.pole_pairs = static_cast<uint8_T>(
                static_cast<uint32_T>(rtu_pck_input->PAYLOAD.ARG[6]) >> 1);
              localB->msg_set_motor_config.encoder_tolerance =
                rtu_pck_input->PAYLOAD.ARG[7];
              localDW->cmd_processed = static_cast<uint16_T>
                (localDW->cmd_processed + 1);
              localB->event_type = EventTypes_SetMotorConfig;
            } else {
              b_previousEvent = localDW->sfEvent;
              localDW->sfEvent = can_decoder_event_ev_error_pck_malformed;
              if (localDW->is_active_ERROR_HANDLING != 0U) {
                can_decoder_ERROR_HANDLING(true, localB, localDW);
              }

              localDW->sfEvent = b_previousEvent;
              localDW->is_SET_MOTOR_CONFIG = can_decoder_IN_Home;
            }
          }
        } else {
          b_previousEvent = localDW->sfEvent;
          localDW->sfEvent = can_decoder_event_ev_error_pck_malformed;
          if (localDW->is_active_ERROR_HANDLING != 0U) {
            can_decoder_ERROR_HANDLING(true, localB, localDW);
          }

          localDW->sfEvent = b_previousEvent;
          localDW->is_SET_MOTOR_CONFIG = can_decoder_IN_Home;
        }
      } else {
        b_previousEvent = localDW->sfEvent;
        localDW->sfEvent = can_decoder_event_ev_error_pck_not4us;
        if (localDW->is_active_ERROR_HANDLING != 0U) {
          can_decoder_ERROR_HANDLING(true, localB, localDW);
        }

        localDW->sfEvent = b_previousEvent;
        localDW->is_SET_MOTOR_CONFIG = can_decoder_IN_Home;
      }
    }

    if (localDW->is_active_ERROR_HANDLING != 0U) {
      can_decoder_ERROR_HANDLING(rtu_pck_available, localB, localDW);
    }
  }

  // End of Chart: '<S2>/Decoding Logic'
  if (localDW->ev_errorEventCounter > 0U) {
    localB->ev_error = !localB->ev_error;
    localDW->ev_errorEventCounter--;
  }
}

// Function for MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic'
static CANClassTypes can_decoder_convert_to_enum_CANClassTypes(int32_T input)
{
  CANClassTypes output;

  // Initialize output value to default value for CANClassTypes (Motor_Control_Command) 
  output = CANClassTypes_Motor_Control_Command;
  if (((input >= 0) && (input <= 2)) || ((input >= 4) && (input <= 7))) {
    // Set output value to input value if it is a member of CANClassTypes
    output = static_cast<CANClassTypes>(input);
  }

  return output;
}

// System initialize for referenced model: 'can_decoder'
void can_decoder_Init(B_can_decoder_c_T *localB, DW_can_decoder_f_T *localDW)
{
  int32_T ForEach_itr;
  static const BUS_CAN_RX tmp = { false,// available
    { { CANClassTypes_Motor_Control_Command,// CLS
        0U,                            // SRC
        0U                             // DST_TYP
      },                               // ID

      { 0U,                            // LEN
        { false,                       // M
          0U                           // OPC
        },                             // CMD

        { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U }// ARG
      }                                // PAYLOAD
    }                                  // packet
  };

  // SystemInitialize for Iterator SubSystem: '<Root>/Cycling Decoder'
  for (ForEach_itr = 0; ForEach_itr < MAX_EVENTS_PER_TICK; ForEach_itr++) {
    // SystemInitialize for Atomic SubSystem: '<S1>/CAN_RX_RAW2STRUCT'
    // SystemInitialize for MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic'
    localB->CoreSubsys[ForEach_itr].pck_rx_struct = tmp;

    // End of SystemInitialize for SubSystem: '<S1>/CAN_RX_RAW2STRUCT'

    // SystemInitialize for Atomic SubSystem: '<S1>/CAN_Decoder'
    // SystemInitialize for Chart: '<S2>/Decoding Logic'
    can_decoder_DecodingLogic_Init(&localDW->CoreSubsys[ForEach_itr].
      sf_DecodingLogic);

    // End of SystemInitialize for SubSystem: '<S1>/CAN_Decoder'
  }

  // End of SystemInitialize for SubSystem: '<Root>/Cycling Decoder'
}

// Output and update for referenced model: 'can_decoder'
void can_decoder(const BUS_CAN_MULTIPLE *rtu_pck_rx_raw, const
                 ActuatorConfiguration *rtu_ConfigurationParameters,
                 ReceivedEvents rty_messages_rx[MAX_EVENTS_PER_TICK],
                 B_can_decoder_c_T *localB, DW_can_decoder_f_T *localDW)
{
  int32_T ForEach_itr;
  int32_T i;
  uint16_T rtu_pck_rx_raw_0;
  uint8_T minval;
  uint8_T x_idx_1;

  // Outputs for Iterator SubSystem: '<Root>/Cycling Decoder' incorporates:
  //   ForEach: '<S1>/For Each'

  for (ForEach_itr = 0; ForEach_itr < MAX_EVENTS_PER_TICK; ForEach_itr++) {
    // Outputs for Atomic SubSystem: '<S1>/CAN_RX_RAW2STRUCT'
    // MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic' incorporates:
    //   ForEachSliceSelector generated from: '<S1>/pck_rx_raw'

    localB->CoreSubsys[ForEach_itr].pck_rx_struct.available =
      rtu_pck_rx_raw->packets[ForEach_itr].available;
    rtu_pck_rx_raw_0 = rtu_pck_rx_raw->packets[ForEach_itr].packet.ID;
    localB->CoreSubsys[ForEach_itr].pck_rx_struct.packet.ID.CLS =
      can_decoder_convert_to_enum_CANClassTypes(static_cast<int32_T>(
      static_cast<uint32_T>(rtu_pck_rx_raw_0 & 1792) >> 8));
    localB->CoreSubsys[ForEach_itr].pck_rx_struct.packet.ID.SRC = static_cast<
      uint8_T>(static_cast<uint32_T>(rtu_pck_rx_raw_0 & 240) >> 4);
    localB->CoreSubsys[ForEach_itr].pck_rx_struct.packet.ID.DST_TYP =
      static_cast<uint8_T>(rtu_pck_rx_raw_0 & 15);
    x_idx_1 = rtu_pck_rx_raw->packets[ForEach_itr].length;
    minval = 8U;
    if (x_idx_1 < 8) {
      minval = x_idx_1;
    }

    localB->CoreSubsys[ForEach_itr].pck_rx_struct.packet.PAYLOAD.LEN = 0U;
    if (minval > 0) {
      localB->CoreSubsys[ForEach_itr].pck_rx_struct.packet.PAYLOAD.LEN = minval;
    }

    x_idx_1 = rtu_pck_rx_raw->packets[ForEach_itr].packet.PAYLOAD[0];
    localB->CoreSubsys[ForEach_itr].pck_rx_struct.packet.PAYLOAD.CMD.M =
      ((x_idx_1 & 128U) != 0U);
    localB->CoreSubsys[ForEach_itr].pck_rx_struct.packet.PAYLOAD.CMD.OPC =
      static_cast<uint8_T>(x_idx_1 & 127);
    for (i = 0; i < 8; i++) {
      localB->CoreSubsys[ForEach_itr].pck_rx_struct.packet.PAYLOAD.ARG[i] =
        rtu_pck_rx_raw->packets[ForEach_itr].packet.PAYLOAD[i];
    }

    // End of MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic'
    // End of Outputs for SubSystem: '<S1>/CAN_RX_RAW2STRUCT'

    // Outputs for Atomic SubSystem: '<S1>/CAN_Decoder'
    // Chart: '<S2>/Decoding Logic' incorporates:
    //   Constant: '<S2>/Constant'
    //   Constant: '<S2>/Constant1'
    //   Constant: '<S2>/Constant2'

    can_decoder_DecodingLogic(localB->CoreSubsys[ForEach_itr].
      pck_rx_struct.available, &localB->CoreSubsys[ForEach_itr].
      pck_rx_struct.packet, rtu_ConfigurationParameters, CAN_ID_AMC, 5, 10.0F,
      &localB->CoreSubsys[ForEach_itr].sf_DecodingLogic, &localDW->
      CoreSubsys[ForEach_itr].sf_DecodingLogic);

    // ForEachSliceAssignment generated from: '<S1>/message_rx' incorporates:
    //   BusCreator: '<S2>/Bus Creator'

    rty_messages_rx[ForEach_itr].event_type = localB->CoreSubsys[ForEach_itr].
      sf_DecodingLogic.event_type;
    rty_messages_rx[ForEach_itr].targets_content = localB->
      CoreSubsys[ForEach_itr].sf_DecodingLogic.msg_desired_targets;
    rty_messages_rx[ForEach_itr].pid_content = localB->CoreSubsys[ForEach_itr].
      sf_DecodingLogic.msg_set_pid;
    rty_messages_rx[ForEach_itr].control_mode_content = localB->
      CoreSubsys[ForEach_itr].sf_DecodingLogic.msg_set_control_mode;
    rty_messages_rx[ForEach_itr].limits_content = localB->CoreSubsys[ForEach_itr]
      .sf_DecodingLogic.msg_set_current_limit;
    rty_messages_rx[ForEach_itr].motor_config_content = localB->
      CoreSubsys[ForEach_itr].sf_DecodingLogic.msg_set_motor_config;

    // End of Outputs for SubSystem: '<S1>/CAN_Decoder'
  }

  // End of Outputs for SubSystem: '<Root>/Cycling Decoder'
}

// Model initialize function
void can_decoder_initialize(const char_T **rt_errorStatus,
  RT_MODEL_can_decoder_T *const can_decoder_M)
{
  // Registration code

  // initialize error status
  rtmSetErrorStatusPointer(can_decoder_M, rt_errorStatus);
}

//
// File trailer for generated code.
//
// [EOF]
//
