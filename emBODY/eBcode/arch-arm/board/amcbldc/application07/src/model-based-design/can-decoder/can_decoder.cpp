//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_decoder.cpp
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 3.49
// Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
// C/C++ source code generated on : Tue Aug  9 15:18:14 2022
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
#include "can_decoder_private.h"

// Named constants for Chart: '<S2>/Decoding Logic'
const int32_T ca_event_ev_error_pck_malformed = 1;
const int32_T can_d_event_ev_error_pck_not4us = 2;
const int32_T can_decoder_CALL_EVENT = -1;
const uint8_T can_decoder_IN_Event_Error = 1U;
const uint8_T can_decoder_IN_Home = 1U;
const uint8_T can_decoder_IN_Home_b = 2U;
const int32_T event_ev_error_mode_unrecognize = 0;
MdlrefDW_can_decoder_T can_decoder_MdlrefDW;

// Block signals (default storage)
B_can_decoder_c_T can_decoder_B;

// Block states (default storage)
DW_can_decoder_f_T can_decoder_DW;

// Forward declaration for local functions
static int32_T can_de_safe_cast_to_MCStreaming(int32_T input);
static void can_decoder_ERROR_HANDLING(boolean_T rtu_pck_available,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW);
static int16_T can_decoder_merge_2bytes_signed(uint16_T bl, uint16_T bh);
static boolean_T can_d_is_controlmode_recognized(int32_T mode);
static int32_T can_safe_cast_to_MCControlModes(int32_T input);
static uint16_T can_decod_merge_2bytes_unsigned(uint16_T bl, uint16_T bh);

// Forward declaration for local functions
static CANClassTypes c_convert_to_enum_CANClassTypes(int32_T input);
static int32_T can_de_safe_cast_to_MCStreaming(int32_T input)
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
  boolean_T guard1 = false;
  guard1 = false;
  switch (localDW->is_ERROR_HANDLING) {
   case can_decoder_IN_Event_Error:
    localDW->is_ERROR_HANDLING = can_decoder_IN_Home_b;
    localDW->cmd_processed = 0U;
    break;

   case can_decoder_IN_Home_b:
    if (localDW->sfEvent == can_d_event_ev_error_pck_not4us) {
      localB->error_type = CANErrorTypes_Packet_Not4Us;
      localDW->ev_errorEventCounter++;
      guard1 = true;
    } else if (localDW->sfEvent == ca_event_ev_error_pck_malformed) {
      localB->error_type = CANErrorTypes_Packet_Malformed;
      localDW->ev_errorEventCounter++;
      guard1 = true;
    } else if (localDW->sfEvent == event_ev_error_mode_unrecognize) {
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
      localDW->is_ERROR_HANDLING = can_decoder_IN_Home_b;
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
  std::memcpy((void *)&sw, (void *)&x, (uint32_T)((size_t)1 * sizeof(int16_T)));
  return sw;
}

// Function for Chart: '<S2>/Decoding Logic'
static boolean_T can_d_is_controlmode_recognized(int32_T mode)
{
  return (mode == static_cast<int32_T>(MCControlModes_Idle)) || (mode ==
    static_cast<int32_T>(MCControlModes_OpenLoop)) || (mode ==
    static_cast<int32_T>(MCControlModes_SpeedVoltage)) || (mode ==
    static_cast<int32_T>(MCControlModes_Current));
}

static int32_T can_safe_cast_to_MCControlModes(int32_T input)
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
static uint16_T can_decod_merge_2bytes_unsigned(uint16_T bl, uint16_T bh)
{
  return static_cast<uint16_T>(static_cast<uint16_T>(bh << 8) | bl);
}

// System initialize for atomic system: '<S2>/Decoding Logic'
void can_decoder_DecodingLogic_Init(B_DecodingLogic_can_decoder_T *localB,
  DW_DecodingLogic_can_decoder_T *localDW)
{
  localDW->sfEvent = can_decoder_CALL_EVENT;
  localB->msg_set_control_mode.motor = false;
  localB->msg_set_control_mode.mode = MCControlModes_Idle;
  localB->msg_set_current_limit.motor = false;
  localB->msg_set_current_limit.nominal = 0.0F;
  localB->msg_set_current_limit.peak = 0.0F;
  localB->msg_set_current_limit.overload = 0.0F;
  localB->msg_desired_targets.current = 0.0F;
  localB->msg_desired_targets.voltage = 0.0F;
  localB->msg_desired_targets.velocity = 0.0F;
  localB->msg_set_pid.motor = false;
  localB->msg_set_pid.Kp = 0.0F;
  localB->msg_set_pid.Ki = 0.0F;
  localB->msg_set_pid.Kd = 0.0F;
  localB->msg_set_pid.Ks = 0U;
  localB->msg_set_motor_config.has_hall_sens = false;
  localB->msg_set_motor_config.has_quadrature_encoder = false;
  localB->msg_set_motor_config.has_speed_quadrature_encoder = false;
  localB->msg_set_motor_config.has_torque_sens = false;
  localB->msg_set_motor_config.use_index = false;
  localB->msg_set_motor_config.enable_verbosity = false;
  localB->msg_set_motor_config.number_poles = 0U;
  localB->msg_set_motor_config.encoder_tolerance = 0U;
  localB->msg_set_motor_config.rotor_encoder_resolution = 0;
  localB->msg_set_motor_config.rotor_index_offset = 0;
}

// Output and update for atomic system: '<S2>/Decoding Logic'
void can_decoder_DecodingLogic(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, const ConfigurationParameters
  *rtu_ConfigurationParameters, uint8_T rtu_CAN_ID_DST, uint8_T
  rtu_CAN_VOLT_REF_SHIFT, real32_T rtu_CAN_VOLT_REF_GAIN,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW)
{
  localDW->sfEvent = can_decoder_CALL_EVENT;

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
    localDW->is_ERROR_HANDLING = can_decoder_IN_Home_b;
    localDW->cmd_processed = 0U;
  } else {
    if ((localDW->is_active_SET_CONTROL_MODE != 0U) &&
        (localDW->is_SET_CONTROL_MODE == can_decoder_IN_Home) &&
        (rtu_pck_available && (rtu_pck_input->ID.CLS ==
          CANClassTypes_Motor_Control_Command))) {
      if (rtu_pck_input->ID.DST_TYP == rtu_CAN_ID_DST) {
        if (rtu_pck_input->PAYLOAD.LEN >= 1) {
          if (rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
              (MCOPC_Set_Control_Mode)) {
            if (rtu_pck_input->PAYLOAD.LEN >= 2) {
              if (can_d_is_controlmode_recognized(static_cast<int32_T>
                   (rtu_pck_input->PAYLOAD.ARG[1]))) {
                localB->msg_set_control_mode.motor =
                  rtu_pck_input->PAYLOAD.CMD.M;
                localB->msg_set_control_mode.mode = static_cast<MCControlModes>
                  (can_safe_cast_to_MCControlModes(rtu_pck_input->PAYLOAD.ARG[1]));
                localDW->cmd_processed = static_cast<uint16_T>
                  (localDW->cmd_processed + 1);
                localDW->ev_set_control_modeEventCounter++;
              } else {
                localDW->sfEvent = event_ev_error_mode_unrecognize;
                if (localDW->is_active_ERROR_HANDLING != 0U) {
                  can_decoder_ERROR_HANDLING(true, localB, localDW);
                }

                localDW->sfEvent = -1;
                localDW->is_SET_CONTROL_MODE = can_decoder_IN_Home;
              }
            } else {
              localDW->sfEvent = ca_event_ev_error_pck_malformed;
              if (localDW->is_active_ERROR_HANDLING != 0U) {
                can_decoder_ERROR_HANDLING(true, localB, localDW);
              }

              localDW->sfEvent = -1;
              localDW->is_SET_CONTROL_MODE = can_decoder_IN_Home;
            }
          }
        } else {
          localDW->sfEvent = ca_event_ev_error_pck_malformed;
          if (localDW->is_active_ERROR_HANDLING != 0U) {
            can_decoder_ERROR_HANDLING(true, localB, localDW);
          }

          localDW->sfEvent = -1;
          localDW->is_SET_CONTROL_MODE = can_decoder_IN_Home;
        }
      } else {
        localDW->sfEvent = can_d_event_ev_error_pck_not4us;
        if (localDW->is_active_ERROR_HANDLING != 0U) {
          can_decoder_ERROR_HANDLING(true, localB, localDW);
        }

        localDW->sfEvent = -1;
        localDW->is_SET_CONTROL_MODE = can_decoder_IN_Home;
      }
    }

    if ((localDW->is_active_DESIRED_TARGETS != 0U) &&
        (localDW->is_DESIRED_TARGETS == can_decoder_IN_Home) &&
        (rtu_pck_available && (rtu_pck_input->ID.CLS ==
          CANClassTypes_Motor_Control_Streaming) &&
         (can_de_safe_cast_to_MCStreaming(rtu_pck_input->ID.DST_TYP) ==
          static_cast<int32_T>(MCStreaming_Desired_Targets)))) {
      if ((rtu_pck_input->PAYLOAD.LEN == 8) && (rtu_CAN_ID_DST <= 4)) {
        int16_T tmp_merged;
        uint8_T idx;
        idx = static_cast<uint8_T>((rtu_CAN_ID_DST - 1) << 1);
        tmp_merged = can_decoder_merge_2bytes_signed(static_cast<uint16_T>
          (rtu_pck_input->PAYLOAD.ARG[idx]), static_cast<uint16_T>
          (rtu_pck_input->PAYLOAD.ARG[idx + 1]));
        localB->msg_desired_targets.current = 0.001F * static_cast<real32_T>
          (tmp_merged);
        localB->msg_desired_targets.voltage = static_cast<real32_T>(static_cast<
          int16_T>(tmp_merged >> (rtu_CAN_VOLT_REF_SHIFT -
          rtu_ConfigurationParameters->CurLoopPID.shift_factor))) /
          rtu_CAN_VOLT_REF_GAIN;
        localB->msg_desired_targets.velocity = tmp_merged;
        localDW->cmd_processed = static_cast<uint16_T>(localDW->cmd_processed +
          1);
        localDW->ev_desired_targetsEventCounter++;
      } else {
        localDW->sfEvent = ca_event_ev_error_pck_malformed;
        if (localDW->is_active_ERROR_HANDLING != 0U) {
          can_decoder_ERROR_HANDLING(true, localB, localDW);
        }

        localDW->sfEvent = -1;
        localDW->is_DESIRED_TARGETS = can_decoder_IN_Home;
      }
    }

    if ((localDW->is_active_SET_OPTIONS != 0U) && (localDW->is_SET_OPTIONS ==
         can_decoder_IN_Home) && (rtu_pck_available && (rtu_pck_input->ID.CLS ==
          CANClassTypes_Motor_Control_Command))) {
      if (rtu_pck_input->ID.DST_TYP == rtu_CAN_ID_DST) {
        if (rtu_pck_input->PAYLOAD.LEN >= 1) {
          boolean_T guard1 = false;
          guard1 = false;
          if (rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
              (MCOPC_Set_Current_Limit)) {
            if (rtu_pck_input->PAYLOAD.LEN == 8) {
              localB->msg_set_current_limit.motor = rtu_pck_input->PAYLOAD.CMD.M;
              localB->msg_set_current_limit.nominal = 0.001F *
                static_cast<real32_T>(can_decoder_merge_2bytes_signed(
                static_cast<uint16_T>(rtu_pck_input->PAYLOAD.ARG[2]),
                static_cast<uint16_T>(rtu_pck_input->PAYLOAD.ARG[3])));
              localB->msg_set_current_limit.peak = 0.001F * static_cast<real32_T>
                (can_decod_merge_2bytes_unsigned(static_cast<uint16_T>
                  (rtu_pck_input->PAYLOAD.ARG[4]), static_cast<uint16_T>
                  (rtu_pck_input->PAYLOAD.ARG[5])));
              localB->msg_set_current_limit.overload = 0.001F *
                static_cast<real32_T>(can_decod_merge_2bytes_unsigned(
                static_cast<uint16_T>(rtu_pck_input->PAYLOAD.ARG[6]),
                static_cast<uint16_T>(rtu_pck_input->PAYLOAD.ARG[7])));
              localDW->cmd_processed = static_cast<uint16_T>
                (localDW->cmd_processed + 1);
              localDW->ev_set_current_limitEventCounte++;
            } else {
              guard1 = true;
            }
          } else if ((rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
                      (MCOPC_Set_Current_PID)) ||
                     (rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
                      (MCOPC_Set_Velocity_PID))) {
            if (rtu_pck_input->PAYLOAD.LEN == 8) {
              real32_T c;
              localB->msg_set_pid.motor = rtu_pck_input->PAYLOAD.CMD.M;
              localB->msg_set_pid.Ks = rtu_pck_input->PAYLOAD.ARG[7];
              c = static_cast<real32_T>(0x01 << (15 - localB->msg_set_pid.Ks)) /
                32768.0F;
              localB->msg_set_pid.Kp = c * static_cast<real32_T>
                (can_decoder_merge_2bytes_signed(static_cast<uint16_T>
                  (rtu_pck_input->PAYLOAD.ARG[1]), static_cast<uint16_T>
                  (rtu_pck_input->PAYLOAD.ARG[2])));
              localB->msg_set_pid.Ki = c * static_cast<real32_T>
                (can_decoder_merge_2bytes_signed(static_cast<uint16_T>
                  (rtu_pck_input->PAYLOAD.ARG[3]), static_cast<uint16_T>
                  (rtu_pck_input->PAYLOAD.ARG[4])));
              localB->msg_set_pid.Kd = c * static_cast<real32_T>
                (can_decoder_merge_2bytes_signed(static_cast<uint16_T>
                  (rtu_pck_input->PAYLOAD.ARG[5]), static_cast<uint16_T>
                  (rtu_pck_input->PAYLOAD.ARG[6])));
              localDW->cmd_processed = static_cast<uint16_T>
                (localDW->cmd_processed + 1);
              if (rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
                  (MCOPC_Set_Current_PID)) {
                localDW->ev_set_current_pidEventCounter++;
              } else {
                localDW->ev_set_velocity_pidEventCounter++;
              }
            } else {
              guard1 = true;
            }
          }

          if (guard1) {
            localDW->sfEvent = ca_event_ev_error_pck_malformed;
            if (localDW->is_active_ERROR_HANDLING != 0U) {
              can_decoder_ERROR_HANDLING(true, localB, localDW);
            }

            localDW->sfEvent = -1;
            localDW->is_SET_OPTIONS = can_decoder_IN_Home;
          }
        } else {
          localDW->sfEvent = ca_event_ev_error_pck_malformed;
          if (localDW->is_active_ERROR_HANDLING != 0U) {
            can_decoder_ERROR_HANDLING(true, localB, localDW);
          }

          localDW->sfEvent = -1;
          localDW->is_SET_OPTIONS = can_decoder_IN_Home;
        }
      } else {
        localDW->sfEvent = can_d_event_ev_error_pck_not4us;
        if (localDW->is_active_ERROR_HANDLING != 0U) {
          can_decoder_ERROR_HANDLING(true, localB, localDW);
        }

        localDW->sfEvent = -1;
        localDW->is_SET_OPTIONS = can_decoder_IN_Home;
      }
    }

    if ((localDW->is_active_SET_MOTOR_CONFIG != 0U) &&
        (localDW->is_SET_MOTOR_CONFIG == can_decoder_IN_Home) &&
        (rtu_pck_available && (rtu_pck_input->ID.CLS ==
          CANClassTypes_Motor_Control_Command))) {
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
              localB->msg_set_motor_config.number_poles =
                rtu_pck_input->PAYLOAD.ARG[6];
              localB->msg_set_motor_config.encoder_tolerance =
                rtu_pck_input->PAYLOAD.ARG[7];
              localDW->cmd_processed = static_cast<uint16_T>
                (localDW->cmd_processed + 1);
              localDW->ev_set_motor_configEventCounter++;
            } else {
              localDW->sfEvent = ca_event_ev_error_pck_malformed;
              if (localDW->is_active_ERROR_HANDLING != 0U) {
                can_decoder_ERROR_HANDLING(true, localB, localDW);
              }

              localDW->sfEvent = -1;
              localDW->is_SET_MOTOR_CONFIG = can_decoder_IN_Home;
            }
          }
        } else {
          localDW->sfEvent = ca_event_ev_error_pck_malformed;
          if (localDW->is_active_ERROR_HANDLING != 0U) {
            can_decoder_ERROR_HANDLING(true, localB, localDW);
          }

          localDW->sfEvent = -1;
          localDW->is_SET_MOTOR_CONFIG = can_decoder_IN_Home;
        }
      } else {
        localDW->sfEvent = can_d_event_ev_error_pck_not4us;
        if (localDW->is_active_ERROR_HANDLING != 0U) {
          can_decoder_ERROR_HANDLING(true, localB, localDW);
        }

        localDW->sfEvent = -1;
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

  if (localDW->ev_set_control_modeEventCounter > 0U) {
    localB->ev_set_control_mode = !localB->ev_set_control_mode;
    localDW->ev_set_control_modeEventCounter--;
  }

  if (localDW->ev_set_current_limitEventCounte > 0U) {
    localB->ev_set_current_limit = !localB->ev_set_current_limit;
    localDW->ev_set_current_limitEventCounte--;
  }

  if (localDW->ev_desired_targetsEventCounter > 0U) {
    localB->ev_desired_targets = !localB->ev_desired_targets;
    localDW->ev_desired_targetsEventCounter--;
  }

  if (localDW->ev_set_current_pidEventCounter > 0U) {
    localB->ev_set_current_pid = !localB->ev_set_current_pid;
    localDW->ev_set_current_pidEventCounter--;
  }

  if (localDW->ev_set_velocity_pidEventCounter > 0U) {
    localB->ev_set_velocity_pid = !localB->ev_set_velocity_pid;
    localDW->ev_set_velocity_pidEventCounter--;
  }

  if (localDW->ev_set_motor_configEventCounter > 0U) {
    localB->ev_set_motor_config = !localB->ev_set_motor_config;
    localDW->ev_set_motor_configEventCounter--;
  }
}

// Function for MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic'
static CANClassTypes c_convert_to_enum_CANClassTypes(int32_T input)
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
void can_decoder_Init(void)
{
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
  // SystemInitialize for Atomic SubSystem: '<S1>/CAN_RX_RAW2STRUCT'
  // SystemInitialize for MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic'
  can_decoder_B.CoreSubsys[0].pck_rx_struct = tmp;

  // End of SystemInitialize for SubSystem: '<S1>/CAN_RX_RAW2STRUCT'

  // SystemInitialize for Atomic SubSystem: '<S1>/CAN_Decoder'
  // SystemInitialize for Chart: '<S2>/Decoding Logic'
  can_decoder_DecodingLogic_Init(&can_decoder_B.CoreSubsys[0].sf_DecodingLogic,
    &can_decoder_DW.CoreSubsys[0].sf_DecodingLogic);

  // End of SystemInitialize for SubSystem: '<S1>/CAN_Decoder'

  // SystemInitialize for Atomic SubSystem: '<S1>/CAN_RX_RAW2STRUCT'
  // SystemInitialize for MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic'
  can_decoder_B.CoreSubsys[1].pck_rx_struct = tmp;

  // End of SystemInitialize for SubSystem: '<S1>/CAN_RX_RAW2STRUCT'

  // SystemInitialize for Atomic SubSystem: '<S1>/CAN_Decoder'
  // SystemInitialize for Chart: '<S2>/Decoding Logic'
  can_decoder_DecodingLogic_Init(&can_decoder_B.CoreSubsys[1].sf_DecodingLogic,
    &can_decoder_DW.CoreSubsys[1].sf_DecodingLogic);

  // End of SystemInitialize for SubSystem: '<S1>/CAN_Decoder'

  // SystemInitialize for Atomic SubSystem: '<S1>/CAN_RX_RAW2STRUCT'
  // SystemInitialize for MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic'
  can_decoder_B.CoreSubsys[2].pck_rx_struct = tmp;

  // End of SystemInitialize for SubSystem: '<S1>/CAN_RX_RAW2STRUCT'

  // SystemInitialize for Atomic SubSystem: '<S1>/CAN_Decoder'
  // SystemInitialize for Chart: '<S2>/Decoding Logic'
  can_decoder_DecodingLogic_Init(&can_decoder_B.CoreSubsys[2].sf_DecodingLogic,
    &can_decoder_DW.CoreSubsys[2].sf_DecodingLogic);

  // End of SystemInitialize for SubSystem: '<S1>/CAN_Decoder'

  // SystemInitialize for Atomic SubSystem: '<S1>/CAN_RX_RAW2STRUCT'
  // SystemInitialize for MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic'
  can_decoder_B.CoreSubsys[3].pck_rx_struct = tmp;

  // End of SystemInitialize for SubSystem: '<S1>/CAN_RX_RAW2STRUCT'

  // SystemInitialize for Atomic SubSystem: '<S1>/CAN_Decoder'
  // SystemInitialize for Chart: '<S2>/Decoding Logic'
  can_decoder_DecodingLogic_Init(&can_decoder_B.CoreSubsys[3].sf_DecodingLogic,
    &can_decoder_DW.CoreSubsys[3].sf_DecodingLogic);

  // End of SystemInitialize for SubSystem: '<S1>/CAN_Decoder'
  // End of SystemInitialize for SubSystem: '<Root>/Cycling Decoder'
}

// Output and update for referenced model: 'can_decoder'
void can_decoder(const BUS_CAN_MULTIPLE *rtu_pck_rx_raw, const
                 ConfigurationParameters *rtu_ConfigurationParameters,
                 BUS_MESSAGES_RX_MULTIPLE *rty_messages_rx,
                 BUS_STATUS_RX_MULTIPLE *rty_status_rx,
                 BUS_CAN_RX_ERRORS_MULTIPLE *rty_errors_rx)
{
  // Outputs for Iterator SubSystem: '<Root>/Cycling Decoder' incorporates:
  //   ForEach: '<S1>/For Each'

  for (int32_T ForEach_itr = 0; ForEach_itr < 4; ForEach_itr++) {
    uint8_T minval;
    uint8_T x_idx_1;

    // Outputs for Atomic SubSystem: '<S1>/CAN_RX_RAW2STRUCT'
    // MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic' incorporates:
    //   ForEachSliceSelector generated from: '<S1>/pck_rx_raw'

    can_decoder_B.CoreSubsys[ForEach_itr].pck_rx_struct.available =
      rtu_pck_rx_raw->packets[ForEach_itr].available;
    can_decoder_B.CoreSubsys[ForEach_itr].pck_rx_struct.packet.ID.CLS =
      c_convert_to_enum_CANClassTypes(static_cast<int32_T>(static_cast<uint16_T>
      (static_cast<uint32_T>(rtu_pck_rx_raw->packets[ForEach_itr].packet.ID &
      1792) >> 8)));
    can_decoder_B.CoreSubsys[ForEach_itr].pck_rx_struct.packet.ID.SRC =
      static_cast<uint8_T>(static_cast<uint32_T>(rtu_pck_rx_raw->
      packets[ForEach_itr].packet.ID & 240) >> 4);
    can_decoder_B.CoreSubsys[ForEach_itr].pck_rx_struct.packet.ID.DST_TYP =
      static_cast<uint8_T>(rtu_pck_rx_raw->packets[ForEach_itr].packet.ID & 15);
    x_idx_1 = rtu_pck_rx_raw->packets[ForEach_itr].length;
    minval = 8U;
    if (x_idx_1 < 8) {
      minval = x_idx_1;
    }

    can_decoder_B.CoreSubsys[ForEach_itr].pck_rx_struct.packet.PAYLOAD.LEN = 0U;
    if (minval > 0) {
      can_decoder_B.CoreSubsys[ForEach_itr].pck_rx_struct.packet.PAYLOAD.LEN =
        minval;
    }

    can_decoder_B.CoreSubsys[ForEach_itr].pck_rx_struct.packet.PAYLOAD.CMD.M =
      ((rtu_pck_rx_raw->packets[ForEach_itr].packet.PAYLOAD[0] & 128U) != 0U);
    can_decoder_B.CoreSubsys[ForEach_itr].pck_rx_struct.packet.PAYLOAD.CMD.OPC =
      static_cast<uint8_T>(rtu_pck_rx_raw->packets[ForEach_itr].packet.PAYLOAD[0]
      & 127);
    for (int32_T i = 0; i < 8; i++) {
      can_decoder_B.CoreSubsys[ForEach_itr].pck_rx_struct.packet.PAYLOAD.ARG[i] =
        rtu_pck_rx_raw->packets[ForEach_itr].packet.PAYLOAD[i];
    }

    boolean_T rtb_FixPtRelationalOperator;
    boolean_T rtb_FixPtRelationalOperator_a;
    boolean_T rtb_FixPtRelationalOperator_d;
    boolean_T rtb_FixPtRelationalOperator_e;
    boolean_T rtb_FixPtRelationalOperator_i;
    boolean_T rtb_FixPtRelationalOperator_m;
    boolean_T rtb_FixPtRelationalOperator_p;

    // End of MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic'
    // End of Outputs for SubSystem: '<S1>/CAN_RX_RAW2STRUCT'

    // Outputs for Atomic SubSystem: '<S1>/CAN_Decoder'
    // Chart: '<S2>/Decoding Logic' incorporates:
    //   Constant: '<S2>/Constant'
    //   Constant: '<S2>/Constant1'
    //   Constant: '<S2>/Constant2'

    can_decoder_DecodingLogic(can_decoder_B.CoreSubsys[ForEach_itr].
      pck_rx_struct.available, &can_decoder_B.CoreSubsys[ForEach_itr].
      pck_rx_struct.packet, rtu_ConfigurationParameters, CAN_ID_AMC, 5, 10.0F,
      &can_decoder_B.CoreSubsys[ForEach_itr].sf_DecodingLogic,
      &can_decoder_DW.CoreSubsys[ForEach_itr].sf_DecodingLogic);

    // RelationalOperator: '<S6>/FixPt Relational Operator' incorporates:
    //   UnitDelay: '<S6>/Delay Input1'
    //
    //  Block description for '<S6>/Delay Input1':
    //
    //   Store in Global RAM

    rtb_FixPtRelationalOperator = (can_decoder_B.CoreSubsys[ForEach_itr].
      sf_DecodingLogic.ev_set_control_mode !=
      can_decoder_DW.CoreSubsys[ForEach_itr].DelayInput1_DSTATE);

    // RelationalOperator: '<S7>/FixPt Relational Operator' incorporates:
    //   UnitDelay: '<S7>/Delay Input1'
    //
    //  Block description for '<S7>/Delay Input1':
    //
    //   Store in Global RAM

    rtb_FixPtRelationalOperator_d = (can_decoder_B.CoreSubsys[ForEach_itr].
      sf_DecodingLogic.ev_set_current_limit !=
      can_decoder_DW.CoreSubsys[ForEach_itr].DelayInput1_DSTATE_n);

    // RelationalOperator: '<S8>/FixPt Relational Operator' incorporates:
    //   UnitDelay: '<S8>/Delay Input1'
    //
    //  Block description for '<S8>/Delay Input1':
    //
    //   Store in Global RAM

    rtb_FixPtRelationalOperator_a = (can_decoder_B.CoreSubsys[ForEach_itr].
      sf_DecodingLogic.ev_desired_targets !=
      can_decoder_DW.CoreSubsys[ForEach_itr].DelayInput1_DSTATE_nk);

    // RelationalOperator: '<S9>/FixPt Relational Operator' incorporates:
    //   UnitDelay: '<S9>/Delay Input1'
    //
    //  Block description for '<S9>/Delay Input1':
    //
    //   Store in Global RAM

    rtb_FixPtRelationalOperator_p = (can_decoder_B.CoreSubsys[ForEach_itr].
      sf_DecodingLogic.ev_set_current_pid !=
      can_decoder_DW.CoreSubsys[ForEach_itr].DelayInput1_DSTATE_h);

    // RelationalOperator: '<S10>/FixPt Relational Operator' incorporates:
    //   UnitDelay: '<S10>/Delay Input1'
    //
    //  Block description for '<S10>/Delay Input1':
    //
    //   Store in Global RAM

    rtb_FixPtRelationalOperator_m = (can_decoder_B.CoreSubsys[ForEach_itr].
      sf_DecodingLogic.ev_set_velocity_pid !=
      can_decoder_DW.CoreSubsys[ForEach_itr].DelayInput1_DSTATE_l);

    // RelationalOperator: '<S11>/FixPt Relational Operator' incorporates:
    //   UnitDelay: '<S11>/Delay Input1'
    //
    //  Block description for '<S11>/Delay Input1':
    //
    //   Store in Global RAM

    rtb_FixPtRelationalOperator_e = (can_decoder_B.CoreSubsys[ForEach_itr].
      sf_DecodingLogic.ev_set_motor_config !=
      can_decoder_DW.CoreSubsys[ForEach_itr].DelayInput1_DSTATE_d);

    // RelationalOperator: '<S5>/FixPt Relational Operator' incorporates:
    //   UnitDelay: '<S5>/Delay Input1'
    //
    //  Block description for '<S5>/Delay Input1':
    //
    //   Store in Global RAM

    rtb_FixPtRelationalOperator_i = (can_decoder_B.CoreSubsys[ForEach_itr].
      sf_DecodingLogic.ev_error != can_decoder_DW.CoreSubsys[ForEach_itr].
      DelayInput1_DSTATE_j);

    // Update for UnitDelay: '<S6>/Delay Input1'
    //
    //  Block description for '<S6>/Delay Input1':
    //
    //   Store in Global RAM

    can_decoder_DW.CoreSubsys[ForEach_itr].DelayInput1_DSTATE =
      can_decoder_B.CoreSubsys[ForEach_itr].sf_DecodingLogic.ev_set_control_mode;

    // Update for UnitDelay: '<S7>/Delay Input1'
    //
    //  Block description for '<S7>/Delay Input1':
    //
    //   Store in Global RAM

    can_decoder_DW.CoreSubsys[ForEach_itr].DelayInput1_DSTATE_n =
      can_decoder_B.CoreSubsys[ForEach_itr].
      sf_DecodingLogic.ev_set_current_limit;

    // Update for UnitDelay: '<S8>/Delay Input1'
    //
    //  Block description for '<S8>/Delay Input1':
    //
    //   Store in Global RAM

    can_decoder_DW.CoreSubsys[ForEach_itr].DelayInput1_DSTATE_nk =
      can_decoder_B.CoreSubsys[ForEach_itr].sf_DecodingLogic.ev_desired_targets;

    // Update for UnitDelay: '<S9>/Delay Input1'
    //
    //  Block description for '<S9>/Delay Input1':
    //
    //   Store in Global RAM

    can_decoder_DW.CoreSubsys[ForEach_itr].DelayInput1_DSTATE_h =
      can_decoder_B.CoreSubsys[ForEach_itr].sf_DecodingLogic.ev_set_current_pid;

    // Update for UnitDelay: '<S10>/Delay Input1'
    //
    //  Block description for '<S10>/Delay Input1':
    //
    //   Store in Global RAM

    can_decoder_DW.CoreSubsys[ForEach_itr].DelayInput1_DSTATE_l =
      can_decoder_B.CoreSubsys[ForEach_itr].sf_DecodingLogic.ev_set_velocity_pid;

    // Update for UnitDelay: '<S11>/Delay Input1'
    //
    //  Block description for '<S11>/Delay Input1':
    //
    //   Store in Global RAM

    can_decoder_DW.CoreSubsys[ForEach_itr].DelayInput1_DSTATE_d =
      can_decoder_B.CoreSubsys[ForEach_itr].sf_DecodingLogic.ev_set_motor_config;

    // Update for UnitDelay: '<S5>/Delay Input1'
    //
    //  Block description for '<S5>/Delay Input1':
    //
    //   Store in Global RAM

    can_decoder_DW.CoreSubsys[ForEach_itr].DelayInput1_DSTATE_j =
      can_decoder_B.CoreSubsys[ForEach_itr].sf_DecodingLogic.ev_error;

    // ForEachSliceAssignment generated from: '<S1>/messages_rx' incorporates:
    //   BusCreator: '<S2>/Bus Creator2'

    rty_messages_rx->messages[ForEach_itr].control_mode =
      can_decoder_B.CoreSubsys[ForEach_itr].
      sf_DecodingLogic.msg_set_control_mode;
    rty_messages_rx->messages[ForEach_itr].current_limit =
      can_decoder_B.CoreSubsys[ForEach_itr].
      sf_DecodingLogic.msg_set_current_limit;
    rty_messages_rx->messages[ForEach_itr].desired_targets =
      can_decoder_B.CoreSubsys[ForEach_itr].sf_DecodingLogic.msg_desired_targets;
    rty_messages_rx->messages[ForEach_itr].pid =
      can_decoder_B.CoreSubsys[ForEach_itr].sf_DecodingLogic.msg_set_pid;
    rty_messages_rx->messages[ForEach_itr].motor_config =
      can_decoder_B.CoreSubsys[ForEach_itr].
      sf_DecodingLogic.msg_set_motor_config;

    // ForEachSliceAssignment generated from: '<S1>/events_rx' incorporates:
    //   BusCreator: '<S2>/Bus Creator1'

    rty_status_rx->status[ForEach_itr].control_mode =
      rtb_FixPtRelationalOperator;
    rty_status_rx->status[ForEach_itr].current_limit =
      rtb_FixPtRelationalOperator_d;
    rty_status_rx->status[ForEach_itr].desired_targets =
      rtb_FixPtRelationalOperator_a;
    rty_status_rx->status[ForEach_itr].current_pid =
      rtb_FixPtRelationalOperator_p;
    rty_status_rx->status[ForEach_itr].velocity_pid =
      rtb_FixPtRelationalOperator_m;
    rty_status_rx->status[ForEach_itr].motor_config =
      rtb_FixPtRelationalOperator_e;

    // ForEachSliceAssignment generated from: '<S1>/errors_rx' incorporates:
    //   BusCreator: '<S2>/Bus Creator3'

    rty_errors_rx->errors[ForEach_itr].event = rtb_FixPtRelationalOperator_i;
    rty_errors_rx->errors[ForEach_itr].type =
      can_decoder_B.CoreSubsys[ForEach_itr].sf_DecodingLogic.error_type;

    // End of Outputs for SubSystem: '<S1>/CAN_Decoder'
  }

  // End of Outputs for SubSystem: '<Root>/Cycling Decoder'
}

// Model initialize function
void can_decoder_initialize(const char_T **rt_errorStatus)
{
  RT_MODEL_can_decoder_T *const can_decoder_M = &(can_decoder_MdlrefDW.rtm);

  // Registration code

  // initialize error status
  rtmSetErrorStatusPointer(can_decoder_M, rt_errorStatus);
}

//
// File trailer for generated code.
//
// [EOF]
//
