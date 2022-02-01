//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_decoder.cpp
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 2.87
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Mon Jan 31 18:31:50 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "can_decoder.h"
#include "can_decoder_private.h"

// Named constants for Chart: '<S2>/Decoding Logic'
const int32_T ca_event_ev_error_pck_malformed = 1;
const int32_T can_d_event_ev_error_pck_not4us = 2;
const int32_T can_decoder_CALL_EVENT = -1;
const uint8_T can_decoder_IN_Event_Error = 1U;
const uint8_T can_decoder_IN_Home = 1U;
const uint8_T can_decoder_IN_Home_a = 2U;
const int32_T event_ev_error_mode_unrecognize = 0;
namespace amc_bldc_codegen
{
  int32_T CAN_Decoder::can_de_safe_cast_to_MCStreaming(int32_T input)
  {
    int32_T y;

    // Initialize output value to default value for MCStreaming (Desired_Current) 
    y = 15;
    if ((input == 0) || (input == 15)) {
      // Set output value to input value if it is a member of MCStreaming
      y = input;
    }

    return y;
  }

  // Function for Chart: '<S2>/Decoding Logic'
  void CAN_Decoder::can_decoder_ERROR_HANDLING(boolean_T rtu_pck_available,
    CANErrorTypes *rty_error_type, DW_DecodingLogic_can_decoder_T *localDW)
  {
    boolean_T guard1 = false;
    guard1 = false;
    switch (localDW->is_ERROR_HANDLING) {
     case can_decoder_IN_Event_Error:
      localDW->is_ERROR_HANDLING = can_decoder_IN_Home_a;
      localDW->cmd_processed = 0U;
      break;

     case can_decoder_IN_Home_a:
      if (localDW->sfEvent == can_d_event_ev_error_pck_not4us) {
        *rty_error_type = CANErrorTypes_Packet_Not4Us;
        localDW->ev_errorEventCounter++;
        localDW->is_ERROR_HANDLING = can_decoder_IN_Event_Error;
        localDW->ev_async = true;
      } else if (localDW->sfEvent == ca_event_ev_error_pck_malformed) {
        *rty_error_type = CANErrorTypes_Packet_Malformed;
        localDW->ev_errorEventCounter++;
        localDW->is_ERROR_HANDLING = can_decoder_IN_Event_Error;
        localDW->ev_async = true;
      } else if (localDW->sfEvent == event_ev_error_mode_unrecognize) {
        *rty_error_type = CANErrorTypes_Mode_Unrecognized;
        localDW->ev_errorEventCounter++;
        localDW->is_ERROR_HANDLING = can_decoder_IN_Event_Error;
        localDW->ev_async = true;
      } else if (localDW->ev_async) {
        guard1 = true;
      } else if (rtu_pck_available && (localDW->cmd_processed == 0)) {
        *rty_error_type = CANErrorTypes_Packet_Unrecognized;
        localDW->ev_async = false;
        localDW->ev_errorEventCounter++;
        localDW->is_ERROR_HANDLING = can_decoder_IN_Home_a;
        localDW->cmd_processed = 0U;
      } else if (localDW->cmd_processed > 1) {
        *rty_error_type = CANErrorTypes_Packet_MultiFunctionsDetected;
        localDW->ev_async = false;
        localDW->ev_errorEventCounter++;
        localDW->is_ERROR_HANDLING = can_decoder_IN_Home_a;
        localDW->cmd_processed = 0U;
      } else {
        *rty_error_type = CANErrorTypes_No_Error;
        guard1 = true;
      }
      break;
    }

    if (guard1) {
      localDW->ev_async = false;
      localDW->is_ERROR_HANDLING = can_decoder_IN_Home_a;
      localDW->cmd_processed = 0U;
    }
  }

  // Function for Chart: '<S2>/Decoding Logic'
  boolean_T CAN_Decoder::can_d_is_controlmode_recognized(int32_T mode)
  {
    return (mode == static_cast<int32_T>(MCControlModes_Idle)) || (mode ==
      static_cast<int32_T>(MCControlModes_OpenLoop)) || (mode == static_cast<
      int32_T>(MCControlModes_SpeedVoltage)) || (mode == static_cast<int32_T>
      (MCControlModes_Current));
  }

  // Function for Chart: '<S2>/Decoding Logic'
  int16_T CAN_Decoder::can_decoder_merge_2bytes_signed(uint16_T bl, uint16_T bh)
  {
    int16_T sw;
    uint16_T x;
    x = static_cast<uint16_T>(static_cast<uint16_T>(bh << 8) | bl);
    std::memcpy((void *)&sw, (void *)&x, (uint32_T)((size_t)1 * sizeof(int16_T)));
    return sw;
  }

  int32_T CAN_Decoder::can_safe_cast_to_MCControlModes(int32_T input)
  {
    int32_T y;

    // Initialize output value to default value for MCControlModes (Idle)
    y = 0;
    if ((input == 0) || (input == 6) || ((input >= 10) && (input <= 11)) ||
        (input == 80) || (input == 160) || (input == 176)) {
      // Set output value to input value if it is a member of MCControlModes
      y = input;
    }

    return y;
  }

  // Function for Chart: '<S2>/Decoding Logic'
  uint16_T CAN_Decoder::can_decod_merge_2bytes_unsigned(uint16_T bl, uint16_T bh)
  {
    return static_cast<uint16_T>(static_cast<uint16_T>(bh << 8) | bl);
  }
}

namespace amc_bldc_codegen
{
  // System initialize for atomic system: '<S2>/Decoding Logic'
  void CAN_Decoder::can_decoder_DecodingLogic_Init
    (self_DecodingLogic_can_decode_T *can_decoder_self_arg, BUS_MSG_CONTROL_MODE
     *rty_msg_set_control_mode, BUS_MSG_CURRENT_LIMIT *rty_msg_set_current_limit,
     CANErrorTypes *rty_error_type, BUS_MSG_DESIRED_CURRENT
     *rty_msg_desired_current, BUS_MSG_CURRENT_PID *rty_msg_set_current_pid,
     boolean_T *rty_ev_set_control_mode, boolean_T *rty_ev_set_current_limit,
     boolean_T *rty_ev_desired_current, boolean_T *rty_ev_error, boolean_T
     *rty_ev_set_current_pid)
  {
    static const BUS_MSG_CONTROL_MODE tmp = { false,// motor
      MCControlModes_Idle              // mode
    };

    static const BUS_MSG_CURRENT_LIMIT tmp_0 = { false,// motor
      0.0F,                            // nominal
      0.0F,                            // peak
      0.0F                             // overload
    };

    static const BUS_MSG_DESIRED_CURRENT tmp_1 = { 0.0F// current
    };

    static const BUS_MSG_CURRENT_PID tmp_2 = { false,// motor
      0.0F,                            // Kp
      0.0F,                            // Ki
      0.0F,                            // Kd
      0U                               // Ks
    };

    *rty_msg_set_control_mode = tmp;
    *rty_msg_set_current_limit = tmp_0;
    *rty_error_type = CANErrorTypes_No_Error;
    *rty_msg_desired_current = tmp_1;
    *rty_msg_set_current_pid = tmp_2;
    *rty_ev_set_control_mode = false;
    *rty_ev_set_current_limit = false;
    *rty_ev_desired_current = false;
    *rty_ev_error = false;
    *rty_ev_set_current_pid = false;
    can_decoder_self_arg->dwork.sfEvent = can_decoder_CALL_EVENT;
    rty_msg_set_control_mode->motor = false;
    rty_msg_set_control_mode->mode = MCControlModes_Idle;
    rty_msg_set_current_limit->motor = false;
    rty_msg_set_current_limit->nominal = 0.0F;
    rty_msg_set_current_limit->peak = 0.0F;
    rty_msg_set_current_limit->overload = 0.0F;
    *rty_error_type = CANErrorTypes_No_Error;
    rty_msg_desired_current->current = 0.0F;
    rty_msg_set_current_pid->motor = false;
    rty_msg_set_current_pid->Kp = 0.0F;
    rty_msg_set_current_pid->Ki = 0.0F;
    rty_msg_set_current_pid->Kd = 0.0F;
    rty_msg_set_current_pid->Ks = 0U;
    *rty_ev_set_control_mode = false;
    *rty_ev_set_current_limit = false;
    *rty_ev_desired_current = false;
    *rty_ev_error = false;
    *rty_ev_set_current_pid = false;
  }

  // Output and update for atomic system: '<S2>/Decoding Logic'
  void CAN_Decoder::can_decoder_DecodingLogic(self_DecodingLogic_can_decode_T
    *can_decoder_self_arg, boolean_T rtu_pck_available, const BUS_CAN_PACKET_RX *
    rtu_pck_input, uint8_T rtu_CAN_ID_DST, BUS_MSG_CONTROL_MODE
    *rty_msg_set_control_mode, BUS_MSG_CURRENT_LIMIT *rty_msg_set_current_limit,
    CANErrorTypes *rty_error_type, BUS_MSG_DESIRED_CURRENT
    *rty_msg_desired_current, BUS_MSG_CURRENT_PID *rty_msg_set_current_pid,
    boolean_T *rty_ev_set_control_mode, boolean_T *rty_ev_set_current_limit,
    boolean_T *rty_ev_desired_current, boolean_T *rty_ev_error, boolean_T
    *rty_ev_set_current_pid)
  {
    can_decoder_self_arg->dwork.sfEvent = can_decoder_CALL_EVENT;

    // Chart: '<S2>/Decoding Logic'
    // This state chart is responsible for decoding incoming CAN packets.
    if (can_decoder_self_arg->dwork.is_active_c3_can_decoder == 0U) {
      can_decoder_self_arg->dwork.is_active_c3_can_decoder = 1U;
      can_decoder_self_arg->dwork.is_active_SET_CONTROL_MODE = 1U;
      can_decoder_self_arg->dwork.is_SET_CONTROL_MODE = can_decoder_IN_Home;
      can_decoder_self_arg->dwork.is_active_DESIRED_CURRENT = 1U;
      can_decoder_self_arg->dwork.is_DESIRED_CURRENT = can_decoder_IN_Home;
      can_decoder_self_arg->dwork.is_active_SET_CURRENT_OPTIONS = 1U;
      can_decoder_self_arg->dwork.is_SET_CURRENT_OPTIONS = can_decoder_IN_Home;
      can_decoder_self_arg->dwork.is_active_ERROR_HANDLING = 1U;
      can_decoder_self_arg->dwork.ev_async = false;
      can_decoder_self_arg->dwork.is_ERROR_HANDLING = can_decoder_IN_Home_a;
      can_decoder_self_arg->dwork.cmd_processed = 0U;
    } else {
      int32_T b_previousEvent;
      boolean_T tmp;
      tmp = !rtu_pck_available;
      tmp = !tmp;
      if ((can_decoder_self_arg->dwork.is_active_SET_CONTROL_MODE != 0U) &&
          (can_decoder_self_arg->dwork.is_SET_CONTROL_MODE == 1) && (tmp &&
           (rtu_pck_input->ID.CLS == CANClassTypes_Motor_Control_Command))) {
        if (rtu_pck_input->ID.DST_TYP == rtu_CAN_ID_DST) {
          if (rtu_pck_input->PAYLOAD.LEN >= 1) {
            if (rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
                (MCOPC_Set_Control_Mode)) {
              if (rtu_pck_input->PAYLOAD.LEN >= 2) {
                if (can_d_is_controlmode_recognized(static_cast<int32_T>
                     (rtu_pck_input->PAYLOAD.ARG[1]))) {
                  rty_msg_set_control_mode->motor = rtu_pck_input->PAYLOAD.CMD.M;
                  rty_msg_set_control_mode->mode = static_cast<MCControlModes>
                    (can_safe_cast_to_MCControlModes(rtu_pck_input->PAYLOAD.ARG
                      [1]));
                  b_previousEvent = can_decoder_self_arg->dwork.cmd_processed +
                    1;
                  if (can_decoder_self_arg->dwork.cmd_processed + 1 > 65535) {
                    b_previousEvent = 65535;
                  }

                  can_decoder_self_arg->dwork.cmd_processed =
                    static_cast<uint16_T>(b_previousEvent);
                  can_decoder_self_arg->dwork.ev_set_control_modeEventCounter++;
                  can_decoder_self_arg->dwork.is_SET_CONTROL_MODE =
                    can_decoder_IN_Home;
                } else {
                  b_previousEvent = can_decoder_self_arg->dwork.sfEvent;
                  can_decoder_self_arg->dwork.sfEvent =
                    event_ev_error_mode_unrecognize;
                  if (can_decoder_self_arg->dwork.is_active_ERROR_HANDLING != 0U)
                  {
                    can_decoder_ERROR_HANDLING(true, rty_error_type,
                      &can_decoder_self_arg->dwork);
                  }

                  can_decoder_self_arg->dwork.sfEvent = b_previousEvent;
                  can_decoder_self_arg->dwork.is_SET_CONTROL_MODE =
                    can_decoder_IN_Home;
                }
              } else {
                b_previousEvent = can_decoder_self_arg->dwork.sfEvent;
                can_decoder_self_arg->dwork.sfEvent =
                  ca_event_ev_error_pck_malformed;
                if (can_decoder_self_arg->dwork.is_active_ERROR_HANDLING != 0U)
                {
                  can_decoder_ERROR_HANDLING(true, rty_error_type,
                    &can_decoder_self_arg->dwork);
                }

                can_decoder_self_arg->dwork.sfEvent = b_previousEvent;
                can_decoder_self_arg->dwork.is_SET_CONTROL_MODE =
                  can_decoder_IN_Home;
              }
            } else {
              can_decoder_self_arg->dwork.is_SET_CONTROL_MODE =
                can_decoder_IN_Home;
            }
          } else {
            b_previousEvent = can_decoder_self_arg->dwork.sfEvent;
            can_decoder_self_arg->dwork.sfEvent =
              ca_event_ev_error_pck_malformed;
            if (can_decoder_self_arg->dwork.is_active_ERROR_HANDLING != 0U) {
              can_decoder_ERROR_HANDLING(true, rty_error_type,
                &can_decoder_self_arg->dwork);
            }

            can_decoder_self_arg->dwork.sfEvent = b_previousEvent;
            can_decoder_self_arg->dwork.is_SET_CONTROL_MODE =
              can_decoder_IN_Home;
          }
        } else {
          b_previousEvent = can_decoder_self_arg->dwork.sfEvent;
          can_decoder_self_arg->dwork.sfEvent = can_d_event_ev_error_pck_not4us;
          if (can_decoder_self_arg->dwork.is_active_ERROR_HANDLING != 0U) {
            can_decoder_ERROR_HANDLING(true, rty_error_type,
              &can_decoder_self_arg->dwork);
          }

          can_decoder_self_arg->dwork.sfEvent = b_previousEvent;
          can_decoder_self_arg->dwork.is_SET_CONTROL_MODE = can_decoder_IN_Home;
        }
      }

      if ((can_decoder_self_arg->dwork.is_active_DESIRED_CURRENT != 0U) &&
          (can_decoder_self_arg->dwork.is_DESIRED_CURRENT == 1) && (tmp &&
           (rtu_pck_input->ID.CLS == CANClassTypes_Motor_Control_Streaming) &&
           (can_de_safe_cast_to_MCStreaming(rtu_pck_input->ID.DST_TYP) ==
            static_cast<int32_T>(MCStreaming_Desired_Current)))) {
        if ((rtu_pck_input->PAYLOAD.LEN == 8) && (rtu_CAN_ID_DST <= 4)) {
          b_previousEvent = (rtu_CAN_ID_DST - 1) << 1;
          if (b_previousEvent < 0) {
            b_previousEvent = 0;
          }

          rty_msg_desired_current->current = static_cast<real32_T>
            (can_decoder_merge_2bytes_signed(static_cast<uint16_T>
              (rtu_pck_input->PAYLOAD.ARG[static_cast<uint8_T>(b_previousEvent)]),
              static_cast<uint16_T>(rtu_pck_input->PAYLOAD.ARG
               [static_cast<uint8_T>(b_previousEvent) + 1]))) * 0.001F;
          b_previousEvent = can_decoder_self_arg->dwork.cmd_processed + 1;
          if (can_decoder_self_arg->dwork.cmd_processed + 1 > 65535) {
            b_previousEvent = 65535;
          }

          can_decoder_self_arg->dwork.cmd_processed = static_cast<uint16_T>
            (b_previousEvent);
          can_decoder_self_arg->dwork.ev_desired_currentEventCounter++;
          can_decoder_self_arg->dwork.is_DESIRED_CURRENT = can_decoder_IN_Home;
        } else {
          b_previousEvent = can_decoder_self_arg->dwork.sfEvent;
          can_decoder_self_arg->dwork.sfEvent = ca_event_ev_error_pck_malformed;
          if (can_decoder_self_arg->dwork.is_active_ERROR_HANDLING != 0U) {
            can_decoder_ERROR_HANDLING(true, rty_error_type,
              &can_decoder_self_arg->dwork);
          }

          can_decoder_self_arg->dwork.sfEvent = b_previousEvent;
          can_decoder_self_arg->dwork.is_DESIRED_CURRENT = can_decoder_IN_Home;
        }
      }

      if ((can_decoder_self_arg->dwork.is_active_SET_CURRENT_OPTIONS != 0U) &&
          (can_decoder_self_arg->dwork.is_SET_CURRENT_OPTIONS == 1) && (tmp &&
           (rtu_pck_input->ID.CLS == CANClassTypes_Motor_Control_Command))) {
        if (rtu_pck_input->ID.DST_TYP == rtu_CAN_ID_DST) {
          if (rtu_pck_input->PAYLOAD.LEN >= 1) {
            boolean_T guard1 = false;
            guard1 = false;
            if (rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
                (MCOPC_Set_Current_Limit)) {
              if (rtu_pck_input->PAYLOAD.LEN == 8) {
                rty_msg_set_current_limit->motor = rtu_pck_input->PAYLOAD.CMD.M;
                rty_msg_set_current_limit->nominal = 0.001F *
                  static_cast<real32_T>(can_decoder_merge_2bytes_signed(
                  static_cast<uint16_T>(rtu_pck_input->PAYLOAD.ARG[2]),
                  static_cast<uint16_T>(rtu_pck_input->PAYLOAD.ARG[3])));
                rty_msg_set_current_limit->peak = 0.001F * static_cast<real32_T>
                  (can_decod_merge_2bytes_unsigned(static_cast<uint16_T>
                    (rtu_pck_input->PAYLOAD.ARG[4]), static_cast<uint16_T>
                    (rtu_pck_input->PAYLOAD.ARG[5])));
                rty_msg_set_current_limit->overload = 0.001F *
                  static_cast<real32_T>(can_decod_merge_2bytes_unsigned(
                  static_cast<uint16_T>(rtu_pck_input->PAYLOAD.ARG[6]),
                  static_cast<uint16_T>(rtu_pck_input->PAYLOAD.ARG[7])));
                b_previousEvent = can_decoder_self_arg->dwork.cmd_processed + 1;
                if (can_decoder_self_arg->dwork.cmd_processed + 1 > 65535) {
                  b_previousEvent = 65535;
                }

                can_decoder_self_arg->dwork.cmd_processed = static_cast<uint16_T>
                  (b_previousEvent);
                can_decoder_self_arg->dwork.ev_set_current_limitEventCounte++;
                can_decoder_self_arg->dwork.is_SET_CURRENT_OPTIONS =
                  can_decoder_IN_Home;
              } else {
                guard1 = true;
              }
            } else if (rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
                       (MCOPC_Set_Current_PID)) {
              if (rtu_pck_input->PAYLOAD.LEN == 8) {
                rty_msg_set_current_pid->motor = rtu_pck_input->PAYLOAD.CMD.M;
                rty_msg_set_current_pid->Kp = can_decoder_merge_2bytes_signed(
                  static_cast<uint16_T>(rtu_pck_input->PAYLOAD.ARG[1]),
                  static_cast<uint16_T>(rtu_pck_input->PAYLOAD.ARG[2]));
                rty_msg_set_current_pid->Ki = can_decoder_merge_2bytes_signed(
                  static_cast<uint16_T>(rtu_pck_input->PAYLOAD.ARG[3]),
                  static_cast<uint16_T>(rtu_pck_input->PAYLOAD.ARG[4]));
                rty_msg_set_current_pid->Kd = can_decoder_merge_2bytes_signed(
                  static_cast<uint16_T>(rtu_pck_input->PAYLOAD.ARG[5]),
                  static_cast<uint16_T>(rtu_pck_input->PAYLOAD.ARG[6]));
                rty_msg_set_current_pid->Ks = rtu_pck_input->PAYLOAD.ARG[7];
                b_previousEvent = can_decoder_self_arg->dwork.cmd_processed + 1;
                if (can_decoder_self_arg->dwork.cmd_processed + 1 > 65535) {
                  b_previousEvent = 65535;
                }

                can_decoder_self_arg->dwork.cmd_processed = static_cast<uint16_T>
                  (b_previousEvent);
                can_decoder_self_arg->dwork.ev_set_current_pidEventCounter++;
                can_decoder_self_arg->dwork.is_SET_CURRENT_OPTIONS =
                  can_decoder_IN_Home;
              } else {
                guard1 = true;
              }
            } else {
              can_decoder_self_arg->dwork.is_SET_CURRENT_OPTIONS =
                can_decoder_IN_Home;
            }

            if (guard1) {
              b_previousEvent = can_decoder_self_arg->dwork.sfEvent;
              can_decoder_self_arg->dwork.sfEvent =
                ca_event_ev_error_pck_malformed;
              if (can_decoder_self_arg->dwork.is_active_ERROR_HANDLING != 0U) {
                can_decoder_ERROR_HANDLING(true, rty_error_type,
                  &can_decoder_self_arg->dwork);
              }

              can_decoder_self_arg->dwork.sfEvent = b_previousEvent;
              can_decoder_self_arg->dwork.is_SET_CURRENT_OPTIONS =
                can_decoder_IN_Home;
            }
          } else {
            b_previousEvent = can_decoder_self_arg->dwork.sfEvent;
            can_decoder_self_arg->dwork.sfEvent =
              ca_event_ev_error_pck_malformed;
            if (can_decoder_self_arg->dwork.is_active_ERROR_HANDLING != 0U) {
              can_decoder_ERROR_HANDLING(true, rty_error_type,
                &can_decoder_self_arg->dwork);
            }

            can_decoder_self_arg->dwork.sfEvent = b_previousEvent;
            can_decoder_self_arg->dwork.is_SET_CURRENT_OPTIONS =
              can_decoder_IN_Home;
          }
        } else {
          b_previousEvent = can_decoder_self_arg->dwork.sfEvent;
          can_decoder_self_arg->dwork.sfEvent = can_d_event_ev_error_pck_not4us;
          if (can_decoder_self_arg->dwork.is_active_ERROR_HANDLING != 0U) {
            can_decoder_ERROR_HANDLING(true, rty_error_type,
              &can_decoder_self_arg->dwork);
          }

          can_decoder_self_arg->dwork.sfEvent = b_previousEvent;
          can_decoder_self_arg->dwork.is_SET_CURRENT_OPTIONS =
            can_decoder_IN_Home;
        }
      }

      if (can_decoder_self_arg->dwork.is_active_ERROR_HANDLING != 0U) {
        can_decoder_ERROR_HANDLING(rtu_pck_available, rty_error_type,
          &can_decoder_self_arg->dwork);
      }
    }

    // End of Chart: '<S2>/Decoding Logic'
    if (can_decoder_self_arg->dwork.ev_set_control_modeEventCounter > 0U) {
      *rty_ev_set_control_mode = !*rty_ev_set_control_mode;
      can_decoder_self_arg->dwork.ev_set_control_modeEventCounter--;
    }

    if (can_decoder_self_arg->dwork.ev_set_current_limitEventCounte > 0U) {
      *rty_ev_set_current_limit = !*rty_ev_set_current_limit;
      can_decoder_self_arg->dwork.ev_set_current_limitEventCounte--;
    }

    if (can_decoder_self_arg->dwork.ev_desired_currentEventCounter > 0U) {
      *rty_ev_desired_current = !*rty_ev_desired_current;
      can_decoder_self_arg->dwork.ev_desired_currentEventCounter--;
    }

    if (can_decoder_self_arg->dwork.ev_errorEventCounter > 0U) {
      *rty_ev_error = !*rty_ev_error;
      can_decoder_self_arg->dwork.ev_errorEventCounter--;
    }

    if (can_decoder_self_arg->dwork.ev_set_current_pidEventCounter > 0U) {
      *rty_ev_set_current_pid = !*rty_ev_set_current_pid;
      can_decoder_self_arg->dwork.ev_set_current_pidEventCounter--;
    }
  }
}

namespace amc_bldc_codegen
{
  // Function for MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic'
  CANClassTypes CAN_Decoder::c_convert_to_enum_CANClassTypes(int32_T input)
  {
    int32_T output;

    // Initialize output value to default value for CANClassTypes (Motor_Control_Command) 
    output = 0;
    if (((input >= 0) && (input <= 2)) || ((input >= 4) && (input <= 7))) {
      // Set output value to input value if it is a member of CANClassTypes
      output = input;
    }

    return static_cast<CANClassTypes>(output);
  }
}

namespace amc_bldc_codegen
{
  // System initialize for referenced model: 'can_decoder'
  void CAN_Decoder::init(void)
  {
    static const BUS_CAN_RX tmp = { false,// available
      { { CANClassTypes_Motor_Control_Command,// CLS
          0U,                          // SRC
          0U                           // DST_TYP
        },                             // ID

        { 0U,                          // LEN
          { false,                     // M
            0U                         // OPC
          },                           // CMD

          { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U }// ARG
        }                              // PAYLOAD
      }                                // packet
    };

    // SystemInitialize for Iterator SubSystem: '<Root>/Cycling Decoder'
    // SystemInitialize for Atomic SubSystem: '<S1>/CAN_RX_RAW2STRUCT'
    // SystemInitialize for MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic'
    self_CoreSubsys[0].blockIO.pck_rx_struct = tmp;

    // End of SystemInitialize for SubSystem: '<S1>/CAN_RX_RAW2STRUCT'

    // SystemInitialize for Atomic SubSystem: '<S1>/CAN_Decoder'
    // SystemInitialize for Chart: '<S2>/Decoding Logic'
    can_decoder_DecodingLogic_Init(&self_CoreSubsys[0].self_sf_DecodingLogic,
      &self_CoreSubsys[0].blockIO.msg_set_control_mode, &self_CoreSubsys[0].
      blockIO.msg_set_current_limit, &self_CoreSubsys[0].blockIO.error_type,
      &self_CoreSubsys[0].blockIO.msg_desired_current, &self_CoreSubsys[0].
      blockIO.msg_set_current_pid, &self_CoreSubsys[0].
      blockIO.ev_set_control_mode, &self_CoreSubsys[0].
      blockIO.ev_set_current_limit, &self_CoreSubsys[0].
      blockIO.ev_desired_current, &self_CoreSubsys[0].blockIO.ev_error,
      &self_CoreSubsys[0].blockIO.ev_set_current_pid);

    // End of SystemInitialize for SubSystem: '<S1>/CAN_Decoder'

    // SystemInitialize for Atomic SubSystem: '<S1>/CAN_RX_RAW2STRUCT'
    // SystemInitialize for MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic'
    self_CoreSubsys[1].blockIO.pck_rx_struct = tmp;

    // End of SystemInitialize for SubSystem: '<S1>/CAN_RX_RAW2STRUCT'

    // SystemInitialize for Atomic SubSystem: '<S1>/CAN_Decoder'
    // SystemInitialize for Chart: '<S2>/Decoding Logic'
    can_decoder_DecodingLogic_Init(&self_CoreSubsys[1].self_sf_DecodingLogic,
      &self_CoreSubsys[1].blockIO.msg_set_control_mode, &self_CoreSubsys[1].
      blockIO.msg_set_current_limit, &self_CoreSubsys[1].blockIO.error_type,
      &self_CoreSubsys[1].blockIO.msg_desired_current, &self_CoreSubsys[1].
      blockIO.msg_set_current_pid, &self_CoreSubsys[1].
      blockIO.ev_set_control_mode, &self_CoreSubsys[1].
      blockIO.ev_set_current_limit, &self_CoreSubsys[1].
      blockIO.ev_desired_current, &self_CoreSubsys[1].blockIO.ev_error,
      &self_CoreSubsys[1].blockIO.ev_set_current_pid);

    // End of SystemInitialize for SubSystem: '<S1>/CAN_Decoder'

    // SystemInitialize for Atomic SubSystem: '<S1>/CAN_RX_RAW2STRUCT'
    // SystemInitialize for MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic'
    self_CoreSubsys[2].blockIO.pck_rx_struct = tmp;

    // End of SystemInitialize for SubSystem: '<S1>/CAN_RX_RAW2STRUCT'

    // SystemInitialize for Atomic SubSystem: '<S1>/CAN_Decoder'
    // SystemInitialize for Chart: '<S2>/Decoding Logic'
    can_decoder_DecodingLogic_Init(&self_CoreSubsys[2].self_sf_DecodingLogic,
      &self_CoreSubsys[2].blockIO.msg_set_control_mode, &self_CoreSubsys[2].
      blockIO.msg_set_current_limit, &self_CoreSubsys[2].blockIO.error_type,
      &self_CoreSubsys[2].blockIO.msg_desired_current, &self_CoreSubsys[2].
      blockIO.msg_set_current_pid, &self_CoreSubsys[2].
      blockIO.ev_set_control_mode, &self_CoreSubsys[2].
      blockIO.ev_set_current_limit, &self_CoreSubsys[2].
      blockIO.ev_desired_current, &self_CoreSubsys[2].blockIO.ev_error,
      &self_CoreSubsys[2].blockIO.ev_set_current_pid);

    // End of SystemInitialize for SubSystem: '<S1>/CAN_Decoder'

    // SystemInitialize for Atomic SubSystem: '<S1>/CAN_RX_RAW2STRUCT'
    // SystemInitialize for MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic'
    self_CoreSubsys[3].blockIO.pck_rx_struct = tmp;

    // End of SystemInitialize for SubSystem: '<S1>/CAN_RX_RAW2STRUCT'

    // SystemInitialize for Atomic SubSystem: '<S1>/CAN_Decoder'
    // SystemInitialize for Chart: '<S2>/Decoding Logic'
    can_decoder_DecodingLogic_Init(&self_CoreSubsys[3].self_sf_DecodingLogic,
      &self_CoreSubsys[3].blockIO.msg_set_control_mode, &self_CoreSubsys[3].
      blockIO.msg_set_current_limit, &self_CoreSubsys[3].blockIO.error_type,
      &self_CoreSubsys[3].blockIO.msg_desired_current, &self_CoreSubsys[3].
      blockIO.msg_set_current_pid, &self_CoreSubsys[3].
      blockIO.ev_set_control_mode, &self_CoreSubsys[3].
      blockIO.ev_set_current_limit, &self_CoreSubsys[3].
      blockIO.ev_desired_current, &self_CoreSubsys[3].blockIO.ev_error,
      &self_CoreSubsys[3].blockIO.ev_set_current_pid);

    // End of SystemInitialize for SubSystem: '<S1>/CAN_Decoder'
    // End of SystemInitialize for SubSystem: '<Root>/Cycling Decoder'
  }

  // Output and update for referenced model: 'can_decoder'
  BUS_MESSAGES_RX_MULTIPLE CAN_Decoder::step(const BUS_CAN_MULTIPLE &
    arg_pck_rx_raw, BUS_EVENTS_RX_MULTIPLE &arg_events_rx,
    BUS_CAN_RX_ERRORS_MULTIPLE &arg_errors_rx)
  {
    // specified return value
    BUS_MESSAGES_RX_MULTIPLE arg_messages_rx;

    // Outputs for Iterator SubSystem: '<Root>/Cycling Decoder' incorporates:
    //   ForEach: '<S1>/For Each'

    for (int32_T ForEach_itr = 0; ForEach_itr < 4; ForEach_itr++) {
      uint8_T minval;
      uint8_T x_idx_1;

      // Outputs for Atomic SubSystem: '<S1>/CAN_RX_RAW2STRUCT'
      // MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic' incorporates:
      //   ForEachSliceSelector generated from: '<S1>/pck_rx_raw'

      self_CoreSubsys[ForEach_itr].blockIO.pck_rx_struct.available =
        arg_pck_rx_raw.packets[ForEach_itr].available;
      self_CoreSubsys[ForEach_itr].blockIO.pck_rx_struct.packet.ID.CLS =
        c_convert_to_enum_CANClassTypes(static_cast<int32_T>(static_cast<
        uint16_T>(static_cast<uint32_T>(arg_pck_rx_raw.packets[ForEach_itr].
        packet.ID & 1792) >> 8)));
      self_CoreSubsys[ForEach_itr].blockIO.pck_rx_struct.packet.ID.SRC =
        static_cast<uint8_T>(static_cast<uint32_T>
        (arg_pck_rx_raw.packets[ForEach_itr].packet.ID & 240) >> 4);
      self_CoreSubsys[ForEach_itr].blockIO.pck_rx_struct.packet.ID.DST_TYP =
        static_cast<uint8_T>(arg_pck_rx_raw.packets[ForEach_itr].packet.ID & 15);
      x_idx_1 = arg_pck_rx_raw.packets[ForEach_itr].length;
      minval = 8U;
      if (8 > x_idx_1) {
        minval = x_idx_1;
      }

      self_CoreSubsys[ForEach_itr].blockIO.pck_rx_struct.packet.PAYLOAD.LEN = 0U;
      if (0 < minval) {
        self_CoreSubsys[ForEach_itr].blockIO.pck_rx_struct.packet.PAYLOAD.LEN =
          minval;
      }

      self_CoreSubsys[ForEach_itr].blockIO.pck_rx_struct.packet.PAYLOAD.CMD.M =
        ((arg_pck_rx_raw.packets[ForEach_itr].packet.PAYLOAD[0] & 128U) != 0U);
      self_CoreSubsys[ForEach_itr].blockIO.pck_rx_struct.packet.PAYLOAD.CMD.OPC =
        static_cast<uint8_T>(arg_pck_rx_raw.packets[ForEach_itr].packet.PAYLOAD
        [0] & 127);
      for (int32_T i = 0; i < 8; i++) {
        self_CoreSubsys[ForEach_itr].blockIO.pck_rx_struct.packet.PAYLOAD.ARG[i]
          = arg_pck_rx_raw.packets[ForEach_itr].packet.PAYLOAD[i];
      }

      // End of MATLAB Function: '<S3>/RAW2STRUCT Decoding Logic'
      // End of Outputs for SubSystem: '<S1>/CAN_RX_RAW2STRUCT'

      // Outputs for Atomic SubSystem: '<S1>/CAN_Decoder'
      // Chart: '<S2>/Decoding Logic' incorporates:
      //   Constant: '<S2>/Constant'

      can_decoder_DecodingLogic(&self_CoreSubsys[ForEach_itr].
        self_sf_DecodingLogic, self_CoreSubsys[ForEach_itr].
        blockIO.pck_rx_struct.available, &self_CoreSubsys[ForEach_itr].
        blockIO.pck_rx_struct.packet, CAN_ID_AMC, &self_CoreSubsys[ForEach_itr].
        blockIO.msg_set_control_mode, &self_CoreSubsys[ForEach_itr].
        blockIO.msg_set_current_limit, &self_CoreSubsys[ForEach_itr].
        blockIO.error_type, &self_CoreSubsys[ForEach_itr].
        blockIO.msg_desired_current, &self_CoreSubsys[ForEach_itr].
        blockIO.msg_set_current_pid, &self_CoreSubsys[ForEach_itr].
        blockIO.ev_set_control_mode, &self_CoreSubsys[ForEach_itr].
        blockIO.ev_set_current_limit, &self_CoreSubsys[ForEach_itr].
        blockIO.ev_desired_current, &self_CoreSubsys[ForEach_itr].
        blockIO.ev_error, &self_CoreSubsys[ForEach_itr].
        blockIO.ev_set_current_pid);

      // ForEachSliceAssignment generated from: '<S1>/messages_rx' incorporates:
      //   BusCreator: '<S2>/Bus Creator2'

      arg_messages_rx.messages[ForEach_itr].control_mode =
        self_CoreSubsys[ForEach_itr].blockIO.msg_set_control_mode;
      arg_messages_rx.messages[ForEach_itr].current_limit =
        self_CoreSubsys[ForEach_itr].blockIO.msg_set_current_limit;
      arg_messages_rx.messages[ForEach_itr].desired_current =
        self_CoreSubsys[ForEach_itr].blockIO.msg_desired_current;
      arg_messages_rx.messages[ForEach_itr].current_pid =
        self_CoreSubsys[ForEach_itr].blockIO.msg_set_current_pid;

      // ForEachSliceAssignment generated from: '<S1>/events_rx' incorporates:
      //   BusCreator: '<S2>/Bus Creator1'

      arg_events_rx.events[ForEach_itr].control_mode =
        self_CoreSubsys[ForEach_itr].blockIO.ev_set_control_mode;
      arg_events_rx.events[ForEach_itr].current_limit =
        self_CoreSubsys[ForEach_itr].blockIO.ev_set_current_limit;
      arg_events_rx.events[ForEach_itr].desired_current =
        self_CoreSubsys[ForEach_itr].blockIO.ev_desired_current;
      arg_events_rx.events[ForEach_itr].current_pid =
        self_CoreSubsys[ForEach_itr].blockIO.ev_set_current_pid;

      // ForEachSliceAssignment generated from: '<S1>/errors_rx' incorporates:
      //   BusCreator: '<S2>/Bus Creator3'

      arg_errors_rx.errors[ForEach_itr].event = self_CoreSubsys[ForEach_itr].
        blockIO.ev_error;
      arg_errors_rx.errors[ForEach_itr].type = self_CoreSubsys[ForEach_itr].
        blockIO.error_type;

      // End of Outputs for SubSystem: '<S1>/CAN_Decoder'
    }

    // End of Outputs for SubSystem: '<Root>/Cycling Decoder'
    return arg_messages_rx;
  }

  // Constructor
  CAN_Decoder::CAN_Decoder() :
    self_CoreSubsys(),
    can_decoder_M()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  CAN_Decoder::~CAN_Decoder()
  {
    // Currently there is no destructor body generated.
  }

  // Real-Time Model get method
  amc_bldc_codegen::CAN_Decoder::RT_MODEL_can_decoder_T * CAN_Decoder::getRTM()
  {
    return (&can_decoder_M);
  }

  // member function to setup error status pointer
  void CAN_Decoder::setErrorStatusPointer(const char_T **rt_errorStatus)
  {
    rtmSetErrorStatusPointer((&can_decoder_M), rt_errorStatus);
  }
}

//
// File trailer for generated code.
//
// [EOF]
//
