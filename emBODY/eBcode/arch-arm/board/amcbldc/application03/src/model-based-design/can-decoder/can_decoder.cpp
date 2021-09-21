//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: can_decoder.cpp
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 1.280
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Mon Sep 20 12:43:37 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "can_decoder.h"
#include "can_decoder_private.h"
#include "Double2MultiWord.h"
#include "MultiWordIor.h"
#include "uMultiWord2Double.h"
#include "uMultiWordShl.h"

// Named constants for Chart: '<S1>/Decoding Logic'
const int32_T ca_event_ev_error_pck_malformed = 1;
const int32_T can_d_event_ev_error_pck_not4us = 2;
const int32_T can_decoder_CALL_EVENT = -1;
const uint8_T can_decoder_IN_Event_Error = 1U;
const uint8_T can_decoder_IN_Home = 1U;
const uint8_T can_decoder_IN_Home_o = 2U;
const int32_T event_ev_error_mode_unrecognize = 0;

real32_T rtP_CAN_ANGLE_DEG2ICUB = 182.044449F;
uint16_T rtP_CAN_ID_HOST = 0U; 
uint8_T rtP_CAN_ID_AMC = 1U; 

namespace can_messaging
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

  // Function for Chart: '<S1>/Decoding Logic'
  real_T CAN_Decoder::can_decoder_merge_2bytes(real_T bl, real_T bh)
  {
    uint64m_T tmp;
    uint64m_T tmp_0;
    uint64m_T tmp_1;
    uint64m_T tmp_2;
    Double2MultiWord(bh, &tmp_2.chunks[0U], 2);
    uMultiWordShl(&tmp_2.chunks[0U], 2, 8U, &tmp_1.chunks[0U], 2);
    Double2MultiWord(uMultiWord2Double(&tmp_1.chunks[0U], 2, 0), &tmp_0.chunks
                     [0U], 2);
    Double2MultiWord(bl, &tmp_2.chunks[0U], 2);
    MultiWordIor(&tmp_0.chunks[0U], &tmp_2.chunks[0U], &tmp.chunks[0U], 2);
    return uMultiWord2Double(&tmp.chunks[0U], 2, 0);
  }

  // Function for Chart: '<S1>/Decoding Logic'
  void CAN_Decoder::can_decoder_ERROR_HANDLING(const BUS_CAN_RX *arg_pck_rx)
  {
    boolean_T guard1 = false;
    guard1 = false;
    switch (can_decoder_DW.is_ERROR_HANDLING) {
     case can_decoder_IN_Event_Error:
      can_decoder_DW.is_ERROR_HANDLING = can_decoder_IN_Home_o;
      can_decoder_DW.cmd_processed = 0U;
      break;

     case can_decoder_IN_Home_o:
      if (can_decoder_DW.sfEvent == can_d_event_ev_error_pck_not4us) {
        can_decoder_B.error_type = CANErrorTypes_Packet_Not4Us;
        can_decoder_DW.ev_errorEventCounter++;
        can_decoder_DW.is_ERROR_HANDLING = can_decoder_IN_Event_Error;
        can_decoder_DW.ev_async = true;
      } else if (can_decoder_DW.sfEvent == ca_event_ev_error_pck_malformed) {
        can_decoder_B.error_type = CANErrorTypes_Packet_Malformed;
        can_decoder_DW.ev_errorEventCounter++;
        can_decoder_DW.is_ERROR_HANDLING = can_decoder_IN_Event_Error;
        can_decoder_DW.ev_async = true;
      } else if (can_decoder_DW.sfEvent == event_ev_error_mode_unrecognize) {
        can_decoder_B.error_type = CANErrorTypes_Mode_Unrecognized;
        can_decoder_DW.ev_errorEventCounter++;
        can_decoder_DW.is_ERROR_HANDLING = can_decoder_IN_Event_Error;
        can_decoder_DW.ev_async = true;
      } else if (can_decoder_DW.ev_async) {
        guard1 = true;
      } else if ((arg_pck_rx->available > 0) && (can_decoder_DW.cmd_processed ==
                  0)) {
        can_decoder_B.error_type = CANErrorTypes_Packet_Unrecognized;
        can_decoder_DW.ev_async = false;
        can_decoder_DW.ev_errorEventCounter++;
        can_decoder_DW.is_ERROR_HANDLING = can_decoder_IN_Home_o;
        can_decoder_DW.cmd_processed = 0U;
      } else if (can_decoder_DW.cmd_processed > 1) {
        can_decoder_B.error_type = CANErrorTypes_Packet_MultiFunctionsDetected;
        can_decoder_DW.ev_async = false;
        can_decoder_DW.ev_errorEventCounter++;
        can_decoder_DW.is_ERROR_HANDLING = can_decoder_IN_Home_o;
        can_decoder_DW.cmd_processed = 0U;
      } else {
        can_decoder_B.error_type = CANErrorTypes_No_Error;
        guard1 = true;
      }
      break;
    }

    if (guard1) {
      can_decoder_DW.ev_async = false;
      can_decoder_DW.is_ERROR_HANDLING = can_decoder_IN_Home_o;
      can_decoder_DW.cmd_processed = 0U;
    }
  }

  // Function for Chart: '<S1>/Decoding Logic'
  real_T CAN_Decoder::can_d_is_controlmode_recognized(real_T mode)
  {
    real_T ok;
    ok = 1.0;
    if ((mode != 0.0) && (mode != 80.0) && (mode != 10.0) && (mode != 6.0)) {
      ok = 0.0;
    }

    return ok;
  }

  int32_T CAN_Decoder::can_safe_cast_to_MCControlModes(int32_T input)
  {
    int32_T y;

    // Initialize output value to default value for MCControlModes (Idle)
    y = 0;
    if ((input == 0) || (input == 6) || (input == 10) || (input == 80)) {
      // Set output value to input value if it is a member of MCControlModes
      y = input;
    }

    return y;
  }
}

namespace can_messaging
{
  // System initialize for referenced model: 'can_decoder'
  void CAN_Decoder::init(BUS_MESSAGES_RX *arg_messages_rx, BUS_EVENTS_RX
    *arg_events_rx, BUS_CAN_RX_ERRORS *arg_errors_rx)
  {
    // SystemInitialize for Atomic SubSystem: '<Root>/CAN_Decoder'
    // SystemInitialize for Chart: '<S1>/Decoding Logic'
    can_decoder_DW.sfEvent = can_decoder_CALL_EVENT;
    can_decoder_B.msg_set_control_mode.motor = false;
    can_decoder_B.msg_set_control_mode.mode = MCControlModes_Idle;
    can_decoder_B.msg_set_current_limit.motor = false;
    can_decoder_B.msg_set_current_limit.nominal = 0;
    can_decoder_B.msg_set_current_limit.peak = 0U;
    can_decoder_B.msg_set_current_limit.overload = 0U;
    can_decoder_B.msg_desired_current.current = 0;

    // SystemInitialize for BusCreator: '<S1>/Bus Creator1'
    arg_events_rx->control_mode = can_decoder_B.ev_set_control_mode;
    arg_events_rx->current_limit = can_decoder_B.ev_set_current_limit;
    arg_events_rx->desired_current = can_decoder_B.ev_desired_current;

    // SystemInitialize for BusCreator: '<S1>/Bus Creator2'
    arg_messages_rx->control_mode = can_decoder_B.msg_set_control_mode;
    arg_messages_rx->current_limit = can_decoder_B.msg_set_current_limit;
    arg_messages_rx->desired_current = can_decoder_B.msg_desired_current;

    // SystemInitialize for BusCreator: '<S1>/Bus Creator3'
    arg_errors_rx->event = can_decoder_B.ev_error;
    arg_errors_rx->type = can_decoder_B.error_type;

    // End of SystemInitialize for SubSystem: '<Root>/CAN_Decoder'
  }

  // Output and update for referenced model: 'can_decoder'
  void CAN_Decoder::step(const BUS_CAN_RX &arg_pck_rx, BUS_MESSAGES_RX &
    arg_messages_rx, BUS_EVENTS_RX &arg_events_rx, BUS_CAN_RX_ERRORS &
    arg_errors_rx)
  {
    real_T tmp;
    int32_T b_previousEvent;

    // Outputs for Atomic SubSystem: '<Root>/CAN_Decoder'
    // Chart: '<S1>/Decoding Logic' incorporates:
    //   Constant: '<S1>/Constant'

    can_decoder_DW.sfEvent = can_decoder_CALL_EVENT;

    // This state chart is responsible for decoding incoming CAN packets.
    if (can_decoder_DW.is_active_c3_can_decoder == 0U) {
      can_decoder_DW.is_active_c3_can_decoder = 1U;
      can_decoder_DW.is_active_SET_CONTROL_MODE = 1U;
      can_decoder_DW.is_SET_CONTROL_MODE = can_decoder_IN_Home;
      can_decoder_DW.is_active_DESIRED_CURRENT = 1U;
      can_decoder_DW.is_DESIRED_CURRENT = can_decoder_IN_Home;
      can_decoder_DW.is_active_SET_CURRENT_LIMIT = 1U;
      can_decoder_DW.is_SET_CURRENT_LIMIT = can_decoder_IN_Home;
      can_decoder_DW.is_active_ERROR_HANDLING = 1U;
      can_decoder_DW.ev_async = false;
      can_decoder_DW.is_ERROR_HANDLING = can_decoder_IN_Home_o;
      can_decoder_DW.cmd_processed = 0U;
    } else {
      if ((can_decoder_DW.is_active_SET_CONTROL_MODE != 0U) &&
          (can_decoder_DW.is_SET_CONTROL_MODE == 1) && ((arg_pck_rx.available >
            0) && (arg_pck_rx.packets.ID.CLS ==
                   CANClassTypes_Motor_Control_Command))) {
        if (arg_pck_rx.packets.ID.DST_TYP == rtP_CAN_ID_AMC) {
          if (arg_pck_rx.packets.PAYLOAD.LEN >= 1) {
            if (arg_pck_rx.packets.PAYLOAD.CMD.OPC == static_cast<int32_T>
                (MCOPC_Set_Control_Mode)) {
              if (arg_pck_rx.packets.PAYLOAD.LEN >= 2) {
                if (can_d_is_controlmode_recognized(static_cast<real_T>
                     (arg_pck_rx.packets.PAYLOAD.ARG[0])) != 0.0) {
                  can_decoder_B.msg_set_control_mode.motor =
                    arg_pck_rx.packets.PAYLOAD.CMD.M;
                  can_decoder_B.msg_set_control_mode.mode =
                    static_cast<MCControlModes>(can_safe_cast_to_MCControlModes
                    (arg_pck_rx.packets.PAYLOAD.ARG[0]));
                  b_previousEvent = can_decoder_DW.cmd_processed + 1;
                  if (can_decoder_DW.cmd_processed + 1 > 65535) {
                    b_previousEvent = 65535;
                  }

                  can_decoder_DW.cmd_processed = static_cast<uint16_T>
                    (b_previousEvent);
                  can_decoder_DW.ev_set_control_modeEventCounter++;
                  can_decoder_DW.is_SET_CONTROL_MODE = can_decoder_IN_Home;
                } else {
                  b_previousEvent = can_decoder_DW.sfEvent;
                  can_decoder_DW.sfEvent = event_ev_error_mode_unrecognize;
                  if (can_decoder_DW.is_active_ERROR_HANDLING != 0U) {
                    can_decoder_ERROR_HANDLING(&arg_pck_rx);
                  }

                  can_decoder_DW.sfEvent = b_previousEvent;
                  can_decoder_DW.is_SET_CONTROL_MODE = can_decoder_IN_Home;
                }
              } else {
                b_previousEvent = can_decoder_DW.sfEvent;
                can_decoder_DW.sfEvent = ca_event_ev_error_pck_malformed;
                if (can_decoder_DW.is_active_ERROR_HANDLING != 0U) {
                  can_decoder_ERROR_HANDLING(&arg_pck_rx);
                }

                can_decoder_DW.sfEvent = b_previousEvent;
                can_decoder_DW.is_SET_CONTROL_MODE = can_decoder_IN_Home;
              }
            } else {
              can_decoder_DW.is_SET_CONTROL_MODE = can_decoder_IN_Home;
            }
          } else {
            b_previousEvent = can_decoder_DW.sfEvent;
            can_decoder_DW.sfEvent = ca_event_ev_error_pck_malformed;
            if (can_decoder_DW.is_active_ERROR_HANDLING != 0U) {
              can_decoder_ERROR_HANDLING(&arg_pck_rx);
            }

            can_decoder_DW.sfEvent = b_previousEvent;
            can_decoder_DW.is_SET_CONTROL_MODE = can_decoder_IN_Home;
          }
        } else {
          b_previousEvent = can_decoder_DW.sfEvent;
          can_decoder_DW.sfEvent = can_d_event_ev_error_pck_not4us;
          if (can_decoder_DW.is_active_ERROR_HANDLING != 0U) {
            can_decoder_ERROR_HANDLING(&arg_pck_rx);
          }

          can_decoder_DW.sfEvent = b_previousEvent;
          can_decoder_DW.is_SET_CONTROL_MODE = can_decoder_IN_Home;
        }
      }

      if ((can_decoder_DW.is_active_DESIRED_CURRENT != 0U) &&
          (can_decoder_DW.is_DESIRED_CURRENT == 1) && ((arg_pck_rx.available > 0)
           && (arg_pck_rx.packets.ID.CLS ==
               CANClassTypes_Motor_Control_Streaming) &&
           (can_de_safe_cast_to_MCStreaming(arg_pck_rx.packets.ID.DST_TYP) ==
            static_cast<int32_T>(MCStreaming_Desired_Current)))) {
        if (arg_pck_rx.packets.PAYLOAD.LEN == 8) {
          tmp = can_decoder_merge_2bytes(static_cast<real_T>
            (arg_pck_rx.packets.PAYLOAD.ARG[5]), static_cast<real_T>
            (arg_pck_rx.packets.PAYLOAD.ARG[6]));
          if (tmp < 32768.0) {
            if (tmp >= -32768.0) {
              can_decoder_B.msg_desired_current.current = static_cast<int16_T>
                (tmp);
            } else {
              can_decoder_B.msg_desired_current.current = MIN_int16_T;
            }
          } else {
            can_decoder_B.msg_desired_current.current = MAX_int16_T;
          }

          b_previousEvent = can_decoder_DW.cmd_processed + 1;
          if (can_decoder_DW.cmd_processed + 1 > 65535) {
            b_previousEvent = 65535;
          }

          can_decoder_DW.cmd_processed = static_cast<uint16_T>(b_previousEvent);
          can_decoder_DW.ev_desired_currentEventCounter++;
          can_decoder_DW.is_DESIRED_CURRENT = can_decoder_IN_Home;
        } else {
          b_previousEvent = can_decoder_DW.sfEvent;
          can_decoder_DW.sfEvent = ca_event_ev_error_pck_malformed;
          if (can_decoder_DW.is_active_ERROR_HANDLING != 0U) {
            can_decoder_ERROR_HANDLING(&arg_pck_rx);
          }

          can_decoder_DW.sfEvent = b_previousEvent;
          can_decoder_DW.is_DESIRED_CURRENT = can_decoder_IN_Home;
        }
      }

      if ((can_decoder_DW.is_active_SET_CURRENT_LIMIT != 0U) &&
          (can_decoder_DW.is_SET_CURRENT_LIMIT == 1) && ((arg_pck_rx.available >
            0) && (arg_pck_rx.packets.ID.CLS ==
                   CANClassTypes_Motor_Control_Command))) {
        if (arg_pck_rx.packets.ID.DST_TYP == rtP_CAN_ID_AMC) {
          if (arg_pck_rx.packets.PAYLOAD.LEN >= 1) {
            if (arg_pck_rx.packets.PAYLOAD.CMD.OPC == static_cast<int32_T>
                (MCOPC_Set_Current_Limit)) {
              if (arg_pck_rx.packets.PAYLOAD.LEN == 8) {
                can_decoder_B.msg_set_current_limit.motor =
                  arg_pck_rx.packets.PAYLOAD.CMD.M;
                tmp = can_decoder_merge_2bytes(static_cast<real_T>
                  (arg_pck_rx.packets.PAYLOAD.ARG[1]), static_cast<real_T>
                  (arg_pck_rx.packets.PAYLOAD.ARG[2]));
                if (tmp < 32768.0) {
                  if (tmp >= -32768.0) {
                    can_decoder_B.msg_set_current_limit.nominal =
                      static_cast<int16_T>(tmp);
                  } else {
                    can_decoder_B.msg_set_current_limit.nominal = MIN_int16_T;
                  }
                } else {
                  can_decoder_B.msg_set_current_limit.nominal = MAX_int16_T;
                }

                tmp = can_decoder_merge_2bytes(static_cast<real_T>
                  (arg_pck_rx.packets.PAYLOAD.ARG[3]), static_cast<real_T>
                  (arg_pck_rx.packets.PAYLOAD.ARG[4]));
                if (tmp < 65536.0) {
                  if (tmp >= 0.0) {
                    can_decoder_B.msg_set_current_limit.peak =
                      static_cast<uint16_T>(tmp);
                  } else {
                    can_decoder_B.msg_set_current_limit.peak = 0U;
                  }
                } else {
                  can_decoder_B.msg_set_current_limit.peak = MAX_uint16_T;
                }

                tmp = can_decoder_merge_2bytes(static_cast<real_T>
                  (arg_pck_rx.packets.PAYLOAD.ARG[5]), static_cast<real_T>
                  (arg_pck_rx.packets.PAYLOAD.ARG[6]));
                if (tmp < 65536.0) {
                  if (tmp >= 0.0) {
                    can_decoder_B.msg_set_current_limit.overload = static_cast<
                      uint16_T>(tmp);
                  } else {
                    can_decoder_B.msg_set_current_limit.overload = 0U;
                  }
                } else {
                  can_decoder_B.msg_set_current_limit.overload = MAX_uint16_T;
                }

                b_previousEvent = can_decoder_DW.cmd_processed + 1;
                if (can_decoder_DW.cmd_processed + 1 > 65535) {
                  b_previousEvent = 65535;
                }

                can_decoder_DW.cmd_processed = static_cast<uint16_T>
                  (b_previousEvent);
                can_decoder_DW.ev_set_current_limitEventCounte++;
                can_decoder_DW.is_SET_CURRENT_LIMIT = can_decoder_IN_Home;
              } else {
                b_previousEvent = can_decoder_DW.sfEvent;
                can_decoder_DW.sfEvent = ca_event_ev_error_pck_malformed;
                if (can_decoder_DW.is_active_ERROR_HANDLING != 0U) {
                  can_decoder_ERROR_HANDLING(&arg_pck_rx);
                }

                can_decoder_DW.sfEvent = b_previousEvent;
                can_decoder_DW.is_SET_CURRENT_LIMIT = can_decoder_IN_Home;
              }
            } else {
              can_decoder_DW.is_SET_CURRENT_LIMIT = can_decoder_IN_Home;
            }
          } else {
            b_previousEvent = can_decoder_DW.sfEvent;
            can_decoder_DW.sfEvent = ca_event_ev_error_pck_malformed;
            if (can_decoder_DW.is_active_ERROR_HANDLING != 0U) {
              can_decoder_ERROR_HANDLING(&arg_pck_rx);
            }

            can_decoder_DW.sfEvent = b_previousEvent;
            can_decoder_DW.is_SET_CURRENT_LIMIT = can_decoder_IN_Home;
          }
        } else {
          b_previousEvent = can_decoder_DW.sfEvent;
          can_decoder_DW.sfEvent = can_d_event_ev_error_pck_not4us;
          if (can_decoder_DW.is_active_ERROR_HANDLING != 0U) {
            can_decoder_ERROR_HANDLING(&arg_pck_rx);
          }

          can_decoder_DW.sfEvent = b_previousEvent;
          can_decoder_DW.is_SET_CURRENT_LIMIT = can_decoder_IN_Home;
        }
      }

      if (can_decoder_DW.is_active_ERROR_HANDLING != 0U) {
        can_decoder_ERROR_HANDLING(&arg_pck_rx);
      }
    }

    if (can_decoder_DW.ev_set_control_modeEventCounter > 0U) {
      can_decoder_B.ev_set_control_mode = !can_decoder_B.ev_set_control_mode;
      can_decoder_DW.ev_set_control_modeEventCounter--;
    }

    if (can_decoder_DW.ev_set_current_limitEventCounte > 0U) {
      can_decoder_B.ev_set_current_limit = !can_decoder_B.ev_set_current_limit;
      can_decoder_DW.ev_set_current_limitEventCounte--;
    }

    if (can_decoder_DW.ev_desired_currentEventCounter > 0U) {
      can_decoder_B.ev_desired_current = !can_decoder_B.ev_desired_current;
      can_decoder_DW.ev_desired_currentEventCounter--;
    }

    if (can_decoder_DW.ev_errorEventCounter > 0U) {
      can_decoder_B.ev_error = !can_decoder_B.ev_error;
      can_decoder_DW.ev_errorEventCounter--;
    }

    // End of Chart: '<S1>/Decoding Logic'

    // BusCreator: '<S1>/Bus Creator1'
    arg_events_rx.control_mode = can_decoder_B.ev_set_control_mode;
    arg_events_rx.current_limit = can_decoder_B.ev_set_current_limit;
    arg_events_rx.desired_current = can_decoder_B.ev_desired_current;

    // BusCreator: '<S1>/Bus Creator2'
    arg_messages_rx.control_mode = can_decoder_B.msg_set_control_mode;
    arg_messages_rx.current_limit = can_decoder_B.msg_set_current_limit;
    arg_messages_rx.desired_current = can_decoder_B.msg_desired_current;

    // BusCreator: '<S1>/Bus Creator3'
    arg_errors_rx.event = can_decoder_B.ev_error;
    arg_errors_rx.type = can_decoder_B.error_type;

    // End of Outputs for SubSystem: '<Root>/CAN_Decoder'
  }

  // Constructor
  CAN_Decoder::CAN_Decoder() :
    can_decoder_B(),
    can_decoder_DW()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  CAN_Decoder::~CAN_Decoder()
  {
    // Currently there is no destructor body generated.
  }

  // Real-Time Model get method
  can_messaging::CAN_Decoder::RT_MODEL_can_decoder_T * CAN_Decoder::getRTM()
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
