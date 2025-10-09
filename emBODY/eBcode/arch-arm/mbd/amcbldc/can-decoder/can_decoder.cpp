//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_decoder.cpp
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 10.72
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Thu Oct  9 17:30:42 2025
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

// Named constants for Chart: '<S3>/Decoding Logic'
const int32_T can_decoder_CALL_EVENT = -1;
const uint8_T can_decoder_IN_Event_Error = 1U;
const uint8_T can_decoder_IN_Home = 1U;
const uint8_T can_decoder_IN_Home_e = 2U;
const int32_T can_decoder_event_ev_error_mode_unrecognized = 50;
const int32_T can_decoder_event_ev_error_pck_malformed = 99;
const int32_T can_decoder_event_ev_error_pck_not4us = 120;

// Forward declaration for local functions
static void can_decoder_ErrorHandler(boolean_T rtu_pck_available,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW);
static boolean_T can_decoder_isPayloadValid(uint8_T expected_size, boolean_T
  rtu_pck_available, const BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T
  rtu_CAN_ID_DST, B_DecodingLogic_can_decoder_T *localB,
  DW_DecodingLogic_can_decoder_T *localDW);
static boolean_T can_decoder_is_controlmode_recognized(int32_T mode);
static ControlModes can_decoder_convertMode(MCControlModes MCMode);
static void can_decoder_SetControlMode(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW);
static int16_T can_decoder_merge_2bytes_signed(uint16_T bl, uint16_T bh);
static uint16_T can_decoder_merge_2bytes_unsigned(uint16_T bl, uint16_T bh);
static void can_decoder_SetCurrentLimits(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW);
static void can_decoder_SetMotorConfig(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW);
static void can_decoder_SetCurrentPID(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW);
static void can_decoder_SetVelocityPID(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW);
static void can_decoder_merge_4bytes_single(const uint8_T d[8], real32_T sw[2]);
static void can_decoder_unpack_hall_config(const uint8_T d[8], real32_T
  hall_data[2]);
static void can_decoder_SetMotorParameter(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW);
static void can_decoder_SetCommand(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW);
static void can_decoder_SetTargets(const BUS_CAN_PACKET_RX *rtu_pck_input,
  uint8_T rtu_CAN_ID_DST, uint8_T rtu_CAN_VOLT_REF_SHIFT, real32_T
  rtu_CAN_VOLT_REF_GAIN, B_DecodingLogic_can_decoder_T *localB,
  DW_DecodingLogic_can_decoder_T *localDW);
static void can_decoder_SetStreaming(const BUS_CAN_PACKET_RX *rtu_pck_input,
  uint8_T rtu_CAN_ID_DST, uint8_T rtu_CAN_VOLT_REF_SHIFT, real32_T
  rtu_CAN_VOLT_REF_GAIN, B_DecodingLogic_can_decoder_T *localB,
  DW_DecodingLogic_can_decoder_T *localDW);

// Function for Chart: '<S3>/Decoding Logic'
static void can_decoder_ErrorHandler(boolean_T rtu_pck_available,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW)
{
  boolean_T guard1;
  guard1 = false;
  switch (localDW->is_ErrorHandler) {
   case can_decoder_IN_Event_Error:
    localDW->is_ErrorHandler = can_decoder_IN_Home_e;
    localDW->cmd_processed = 0U;
    break;

   case can_decoder_IN_Home_e:
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
      localDW->is_ErrorHandler = can_decoder_IN_Home_e;
      localDW->cmd_processed = 0U;
    }
    break;
  }

  if (guard1) {
    localDW->is_ErrorHandler = can_decoder_IN_Event_Error;
    localDW->ev_async = true;
  }
}

// Function for Chart: '<S3>/Decoding Logic'
static boolean_T can_decoder_isPayloadValid(uint8_T expected_size, boolean_T
  rtu_pck_available, const BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T
  rtu_CAN_ID_DST, B_DecodingLogic_can_decoder_T *localB,
  DW_DecodingLogic_can_decoder_T *localDW)
{
  int32_T d_previousEvent;
  boolean_T r;
  r = true;
  if (rtu_pck_input->ID.DST_TYP != rtu_CAN_ID_DST) {
    d_previousEvent = localDW->sfEvent;
    localDW->sfEvent = can_decoder_event_ev_error_pck_not4us;
    if (localDW->is_active_ErrorHandler != 0) {
      can_decoder_ErrorHandler(rtu_pck_available, localB, localDW);
    }

    localDW->sfEvent = d_previousEvent;
    r = false;
  } else if (rtu_pck_input->PAYLOAD.LEN < 1) {
    d_previousEvent = localDW->sfEvent;
    localDW->sfEvent = can_decoder_event_ev_error_pck_malformed;
    if (localDW->is_active_ErrorHandler != 0) {
      can_decoder_ErrorHandler(rtu_pck_available, localB, localDW);
    }

    localDW->sfEvent = d_previousEvent;
    r = false;
  } else if (rtu_pck_input->PAYLOAD.LEN < expected_size) {
    d_previousEvent = localDW->sfEvent;
    localDW->sfEvent = can_decoder_event_ev_error_pck_malformed;
    if (localDW->is_active_ErrorHandler != 0) {
      can_decoder_ErrorHandler(rtu_pck_available, localB, localDW);
    }

    localDW->sfEvent = d_previousEvent;
    r = false;
  }

  return r;
}

// Function for Chart: '<S3>/Decoding Logic'
static boolean_T can_decoder_is_controlmode_recognized(int32_T mode)
{
  return (mode == static_cast<int32_T>(MCControlModes_Idle)) || (mode ==
    static_cast<int32_T>(MCControlModes_OpenLoop)) || (mode ==
    static_cast<int32_T>(MCControlModes_SpeedVoltage)) || (mode ==
    static_cast<int32_T>(MCControlModes_Current));
}

// Function for Chart: '<S3>/Decoding Logic'
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

// Function for Chart: '<S3>/Decoding Logic'
static void can_decoder_SetControlMode(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW)
{
  int32_T b_previousEvent;
  if (can_decoder_isPayloadValid(2, rtu_pck_available, rtu_pck_input,
       rtu_CAN_ID_DST, localB, localDW)) {
    if (!can_decoder_is_controlmode_recognized(static_cast<int32_T>
         (rtu_pck_input->PAYLOAD.ARG[1]))) {
      b_previousEvent = localDW->sfEvent;
      localDW->sfEvent = can_decoder_event_ev_error_mode_unrecognized;
      if (localDW->is_active_ErrorHandler != 0) {
        can_decoder_ErrorHandler(rtu_pck_available, localB, localDW);
      }

      localDW->sfEvent = b_previousEvent;
    } else {
      //  msg_set_control_mode.motor = pck_input.PAYLOAD.CMD.M;
      localB->msg_set_control_mode = can_decoder_convertMode
        (static_cast<MCControlModes>(rtu_pck_input->PAYLOAD.ARG[1]));
      localDW->cmd_processed = static_cast<uint16_T>(localDW->cmd_processed + 1);
      localB->event_type = EventTypes_SetControlMode;
    }
  }
}

// Function for Chart: '<S3>/Decoding Logic'
static int16_T can_decoder_merge_2bytes_signed(uint16_T bl, uint16_T bh)
{
  int16_T sw;
  uint16_T x;
  x = static_cast<uint16_T>(static_cast<uint16_T>(bh << 8) | bl);
  std::memcpy((void *)&sw, (void *)&x, (size_t)1 * sizeof(int16_T));
  return sw;
}

// Function for Chart: '<S3>/Decoding Logic'
static uint16_T can_decoder_merge_2bytes_unsigned(uint16_T bl, uint16_T bh)
{
  return static_cast<uint16_T>(static_cast<uint16_T>(bh << 8) | bl);
}

// Function for Chart: '<S3>/Decoding Logic'
static void can_decoder_SetCurrentLimits(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW)
{
  if (can_decoder_isPayloadValid(8, rtu_pck_available, rtu_pck_input,
       rtu_CAN_ID_DST, localB, localDW)) {
    //  msg_set_current_limit.motor = pck_input.PAYLOAD.CMD.M;
    localB->msg_set_current_limit.nominal = 0.001F * static_cast<real32_T>
      (can_decoder_merge_2bytes_signed(static_cast<uint16_T>
        (rtu_pck_input->PAYLOAD.ARG[2]), static_cast<uint16_T>
        (rtu_pck_input->PAYLOAD.ARG[3])));
    localB->msg_set_current_limit.peak = 0.001F * static_cast<real32_T>
      (can_decoder_merge_2bytes_unsigned(static_cast<uint16_T>
        (rtu_pck_input->PAYLOAD.ARG[4]), static_cast<uint16_T>
        (rtu_pck_input->PAYLOAD.ARG[5])));
    localB->msg_set_current_limit.overload = 0.001F * static_cast<real32_T>
      (can_decoder_merge_2bytes_unsigned(static_cast<uint16_T>
        (rtu_pck_input->PAYLOAD.ARG[6]), static_cast<uint16_T>
        (rtu_pck_input->PAYLOAD.ARG[7])));
    localDW->cmd_processed = static_cast<uint16_T>(localDW->cmd_processed + 1);
    localB->event_type = EventTypes_SetLimit;
  }
}

// Function for Chart: '<S3>/Decoding Logic'
static void can_decoder_SetMotorConfig(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW)
{
  if (can_decoder_isPayloadValid(8, rtu_pck_available, rtu_pck_input,
       rtu_CAN_ID_DST, localB, localDW)) {
    localB->msg_set_motor_config.has_quadrature_encoder =
      ((rtu_pck_input->PAYLOAD.ARG[1] & static_cast<uint32_T>(0x01)) != 0U);
    localB->msg_set_motor_config.has_hall_sens = ((rtu_pck_input->PAYLOAD.ARG[1]
      & static_cast<uint32_T>(0x02)) != 0U);
    localB->msg_set_motor_config.has_temperature_sens =
      ((rtu_pck_input->PAYLOAD.ARG[1] & static_cast<uint32_T>(0x04)) != 0U);
    localB->msg_set_motor_config.use_index = ((rtu_pck_input->PAYLOAD.ARG[1] &
      static_cast<uint32_T>(0x08)) != 0U);
    localB->msg_set_motor_config.has_speed_quadrature_encoder =
      ((rtu_pck_input->PAYLOAD.ARG[1] & static_cast<uint32_T>(0x10)) != 0U);
    localB->msg_set_motor_config.enable_verbosity = ((rtu_pck_input->
      PAYLOAD.ARG[1] & static_cast<uint32_T>(0x20)) != 0U);
    localB->msg_set_motor_config.rotor_encoder_resolution =
      can_decoder_merge_2bytes_signed(static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[2]), static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[3]));
    localB->msg_set_motor_config.rotor_index_offset =
      can_decoder_merge_2bytes_signed(static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[4]), static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[5]));
    localB->msg_set_motor_config.pole_pairs = static_cast<uint8_T>
      (rtu_pck_input->PAYLOAD.ARG[6] >> 1);
    localB->msg_set_motor_config.encoder_tolerance = rtu_pck_input->PAYLOAD.ARG
      [7];
    localDW->cmd_processed = static_cast<uint16_T>(localDW->cmd_processed + 1);
    localB->event_type = EventTypes_SetMotorConfig;
  }
}

// Function for Chart: '<S3>/Decoding Logic'
static void can_decoder_SetCurrentPID(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW)
{
  real_T c;
  if (can_decoder_isPayloadValid(8, rtu_pck_available, rtu_pck_input,
       rtu_CAN_ID_DST, localB, localDW)) {
    // msg_set_pid.motor = pck_input.PAYLOAD.CMD.M;
    localB->msg_set_pid.shift_factor = rtu_pck_input->PAYLOAD.ARG[7];
    c = static_cast<real32_T>(0x01 << (15 - localB->msg_set_pid.shift_factor)) /
      32768.0F;
    localB->msg_set_pid.P = static_cast<real32_T>(c * static_cast<real_T>
      (can_decoder_merge_2bytes_signed(static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[1]), static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[2]))));
    localB->msg_set_pid.I = static_cast<real32_T>(c * static_cast<real_T>
      (can_decoder_merge_2bytes_signed(static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[3]), static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[4]))));
    localB->msg_set_pid.D = static_cast<real32_T>(c * static_cast<real_T>
      (can_decoder_merge_2bytes_signed(static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[5]), static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[6]))));
    localB->msg_set_pid.type = ControlModes_Current;
    localB->event_type = EventTypes_SetPid;
    localDW->cmd_processed = static_cast<uint16_T>(localDW->cmd_processed + 1);
  }
}

// Function for Chart: '<S3>/Decoding Logic'
static void can_decoder_SetVelocityPID(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW)
{
  real_T c;
  if (can_decoder_isPayloadValid(8, rtu_pck_available, rtu_pck_input,
       rtu_CAN_ID_DST, localB, localDW)) {
    localB->msg_set_pid.shift_factor = rtu_pck_input->PAYLOAD.ARG[7];
    c = static_cast<real32_T>(0x01 << (15 - localB->msg_set_pid.shift_factor)) /
      32768.0F;
    localB->msg_set_pid.P = static_cast<real32_T>(c * static_cast<real_T>
      (can_decoder_merge_2bytes_signed(static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[1]), static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[2]))));
    localB->msg_set_pid.I = static_cast<real32_T>(c * static_cast<real_T>
      (can_decoder_merge_2bytes_signed(static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[3]), static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[4]))));
    localB->msg_set_pid.D = static_cast<real32_T>(c * static_cast<real_T>
      (can_decoder_merge_2bytes_signed(static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[5]), static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[6]))));
    localB->msg_set_pid.P *= 0.001F;
    localB->msg_set_pid.I *= 0.001F;
    localB->msg_set_pid.D *= 0.001F;
    localB->msg_set_pid.type = ControlModes_Velocity;
    localB->event_type = EventTypes_SetPid;
    localDW->cmd_processed = static_cast<uint16_T>(localDW->cmd_processed + 1);
  }
}

// Function for Chart: '<S3>/Decoding Logic'
static void can_decoder_merge_4bytes_single(const uint8_T d[8], real32_T sw[2])
{
  uint8_T x[4];
  sw[0] = 0.0F;
  sw[1] = 0.0F;
  x[0] = d[4];
  x[1] = d[3];
  x[2] = d[2];
  x[3] = d[1];
  std::memcpy((void *)&sw[0], (void *)&x[0], (size_t)1 * sizeof(real32_T));
}

// Function for Chart: '<S3>/Decoding Logic'
static void can_decoder_unpack_hall_config(const uint8_T d[8], real32_T
  hall_data[2])
{
  int16_T y;
  uint8_T x[2];
  x[0] = d[2];
  x[1] = d[1];
  std::memcpy((void *)&y, (void *)&x[0], (size_t)1 * sizeof(int16_T));
  hall_data[0] = y;
  hall_data[1] = static_cast<real32_T>(d[3] >> 4);
}

// Function for Chart: '<S3>/Decoding Logic'
static void can_decoder_SetMotorParameter(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW)
{
  if (can_decoder_isPayloadValid(8, rtu_pck_available, rtu_pck_input,
       rtu_CAN_ID_DST, localB, localDW)) {
    if (rtu_pck_input->PAYLOAD.ARG[1] == MCMotorParamsSet_Kbemf) {
      can_decoder_merge_4bytes_single(rtu_pck_input->PAYLOAD.ARG,
        localB->msg_set_motor_config_extra.value);
      localB->msg_set_motor_config_extra.key = rtu_pck_input->PAYLOAD.ARG[1];
    } else if (rtu_pck_input->PAYLOAD.ARG[1] == MCMotorParamsSet_hall) {
      can_decoder_unpack_hall_config(rtu_pck_input->PAYLOAD.ARG,
        localB->msg_set_motor_config_extra.value);
      localB->msg_set_motor_config_extra.key = rtu_pck_input->PAYLOAD.ARG[1];
    } else if (rtu_pck_input->PAYLOAD.ARG[1] == MCMotorParamsSet_elect_vmax) {
      can_decoder_merge_4bytes_single(rtu_pck_input->PAYLOAD.ARG,
        localB->msg_set_motor_config_extra.value);
      localB->msg_set_motor_config_extra.key = rtu_pck_input->PAYLOAD.ARG[1];
    }
  }
}

// Function for Chart: '<S3>/Decoding Logic'
static void can_decoder_SetCommand(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW)
{
  if (rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
      (MCOPC_Set_Control_Mode)) {
    can_decoder_SetControlMode(rtu_pck_available, rtu_pck_input, rtu_CAN_ID_DST,
      localB, localDW);
  } else if (rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
             (MCOPC_Set_Current_Limit)) {
    can_decoder_SetCurrentLimits(rtu_pck_available, rtu_pck_input,
      rtu_CAN_ID_DST, localB, localDW);
  } else if (rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
             (MCOPC_Set_Motor_Config)) {
    can_decoder_SetMotorConfig(rtu_pck_available, rtu_pck_input, rtu_CAN_ID_DST,
      localB, localDW);
  } else if (rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
             (MCOPC_Set_Current_PID)) {
    can_decoder_SetCurrentPID(rtu_pck_available, rtu_pck_input, rtu_CAN_ID_DST,
      localB, localDW);
  } else if (rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
             (MCOPC_Set_Velocity_PID)) {
    can_decoder_SetVelocityPID(rtu_pck_available, rtu_pck_input, rtu_CAN_ID_DST,
      localB, localDW);
  } else if (rtu_pck_input->PAYLOAD.CMD.OPC == static_cast<int32_T>
             (MCOPC_Set_Parameter)) {
    can_decoder_SetMotorParameter(rtu_pck_available, rtu_pck_input,
      rtu_CAN_ID_DST, localB, localDW);
  }
}

// Function for Chart: '<S3>/Decoding Logic'
static void can_decoder_SetTargets(const BUS_CAN_PACKET_RX *rtu_pck_input,
  uint8_T rtu_CAN_ID_DST, uint8_T rtu_CAN_VOLT_REF_SHIFT, real32_T
  rtu_CAN_VOLT_REF_GAIN, B_DecodingLogic_can_decoder_T *localB,
  DW_DecodingLogic_can_decoder_T *localDW)
{
  int32_T idx;
  if ((rtu_pck_input->PAYLOAD.LEN == 8) && (rtu_CAN_ID_DST <= 4)) {
    idx = (rtu_CAN_ID_DST - 1) << 1;
    idx = can_decoder_merge_2bytes_signed(static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[idx]), static_cast<uint16_T>
      (rtu_pck_input->PAYLOAD.ARG[idx + 1]));
    localB->msg_desired_targets.current = 0.001F * static_cast<real32_T>(idx);
    localB->msg_desired_targets.voltage = static_cast<real32_T>(idx >>
      rtu_CAN_VOLT_REF_SHIFT) / rtu_CAN_VOLT_REF_GAIN;
    localB->msg_desired_targets.velocity = 1000.0F * static_cast<real32_T>(idx) *
      CAN_ANGLE_ICUB2DEG;
    localDW->cmd_processed = static_cast<uint16_T>(localDW->cmd_processed + 1);
    localB->event_type = EventTypes_SetTarget;
  }
}

// Function for Chart: '<S3>/Decoding Logic'
static void can_decoder_SetStreaming(const BUS_CAN_PACKET_RX *rtu_pck_input,
  uint8_T rtu_CAN_ID_DST, uint8_T rtu_CAN_VOLT_REF_SHIFT, real32_T
  rtu_CAN_VOLT_REF_GAIN, B_DecodingLogic_can_decoder_T *localB,
  DW_DecodingLogic_can_decoder_T *localDW)
{
  if (rtu_pck_input->ID.DST_TYP == static_cast<int32_T>
      (MCStreaming_Desired_Targets)) {
    can_decoder_SetTargets(rtu_pck_input, rtu_CAN_ID_DST, rtu_CAN_VOLT_REF_SHIFT,
      rtu_CAN_VOLT_REF_GAIN, localB, localDW);
  }
}

// System initialize for atomic system: '<S3>/Decoding Logic'
void can_decoder_DecodingLogic_Init(DW_DecodingLogic_can_decoder_T *localDW)
{
  localDW->sfEvent = can_decoder_CALL_EVENT;
}

// Output and update for atomic system: '<S3>/Decoding Logic'
void can_decoder_DecodingLogic(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST, uint8_T
  rtu_CAN_VOLT_REF_SHIFT, real32_T rtu_CAN_VOLT_REF_GAIN,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW)
{
  localDW->sfEvent = can_decoder_CALL_EVENT;
  localB->msg_set_control_mode = ControlModes_NotConfigured;
  localB->msg_set_current_limit.overload = 0.0F;
  localB->msg_set_current_limit.peak = 0.0F;
  localB->msg_set_current_limit.nominal = 0.0F;
  localB->msg_set_current_limit.type = ControlModes_NotConfigured;
  localB->msg_desired_targets.trajectory_time = 0.0F;
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
  localB->msg_set_motor_config.has_temperature_sens = false;
  localB->msg_set_motor_config.encoder_tolerance = 0U;
  localB->msg_set_motor_config.pole_pairs = 0U;
  localB->msg_set_motor_config.rotor_encoder_resolution = 0;
  localB->msg_set_motor_config.rotor_index_offset = 0;
  localB->msg_set_motor_config.use_index = false;
  localB->error_type = CANErrorTypes_No_Error;
  localB->event_type = EventTypes_None;
  localB->msg_set_motor_config_extra.key = MCMotorParamsSet_None;
  localB->msg_set_motor_config_extra.value[0] = 0.0F;
  localB->msg_set_motor_config_extra.value[1] = 0.0F;

  // Chart: '<S3>/Decoding Logic'
  // This state chart is responsible for decoding incoming CAN packets.
  if (localDW->is_active_c1_can_decoder == 0) {
    localDW->is_active_c1_can_decoder = 1U;
    localDW->is_active_CANParser = 1U;
    localDW->is_CANParser = can_decoder_IN_Home;
    localDW->is_active_ErrorHandler = 1U;
    localDW->ev_async = false;
    localDW->is_ErrorHandler = can_decoder_IN_Home_e;
    localDW->cmd_processed = 0U;
  } else {
    if ((localDW->is_active_CANParser != 0) && (localDW->is_CANParser ==
         can_decoder_IN_Home) && rtu_pck_available) {
      switch (rtu_pck_input->ID.CLS) {
       case CANClassTypes_Motor_Control_Command:
        can_decoder_SetCommand(true, rtu_pck_input, rtu_CAN_ID_DST, localB,
          localDW);
        break;

       case CANClassTypes_Motor_Control_Streaming:
        can_decoder_SetStreaming(rtu_pck_input, rtu_CAN_ID_DST,
          rtu_CAN_VOLT_REF_SHIFT, rtu_CAN_VOLT_REF_GAIN, localB, localDW);
        break;
      }
    }

    if (localDW->is_active_ErrorHandler != 0) {
      can_decoder_ErrorHandler(rtu_pck_available, localB, localDW);
    }
  }

  // End of Chart: '<S3>/Decoding Logic'
  if (localDW->ev_errorEventCounter > 0U) {
    localB->ev_error = !localB->ev_error;
    localDW->ev_errorEventCounter--;
  }
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

  // SystemInitialize for Iterator SubSystem: '<S1>/For Each Subsystem'
  for (ForEach_itr = 0; ForEach_itr < CAN_MAX_NUM_PACKETS; ForEach_itr++) {
    // SystemInitialize for MATLAB Function: '<S2>/RAW2STRUCT Decoding Logic'
    localB->CoreSubsys[ForEach_itr].pck_rx_struct = tmp;

    // SystemInitialize for Atomic SubSystem: '<S2>/CAN_Decoder'
    // SystemInitialize for Chart: '<S3>/Decoding Logic'
    can_decoder_DecodingLogic_Init(&localDW->CoreSubsys[ForEach_itr].
      sf_DecodingLogic);

    // End of SystemInitialize for SubSystem: '<S2>/CAN_Decoder'
  }

  // End of SystemInitialize for SubSystem: '<S1>/For Each Subsystem'
}

// Output and update for referenced model: 'can_decoder'
void can_decoder(const BUS_CAN_MULTIPLE *rtu_pck_rx_raw, ReceivedEvents
                 rty_messages_rx[4], B_can_decoder_c_T *localB,
                 DW_can_decoder_f_T *localDW)
{
  int32_T ForEach_itr;
  int32_T input;
  uint16_T rtu_pck_rx_raw_packets;
  uint8_T minval;
  uint8_T x_idx_1;

  // Outputs for Iterator SubSystem: '<S1>/For Each Subsystem' incorporates:
  //   ForEach: '<S2>/For Each'

  for (ForEach_itr = 0; ForEach_itr < CAN_MAX_NUM_PACKETS; ForEach_itr++) {
    // MATLAB Function: '<S2>/RAW2STRUCT Decoding Logic' incorporates:
    //   ForEachSliceSelector generated from: '<S2>/pck_rx_raw'

    localB->CoreSubsys[ForEach_itr].pck_rx_struct.available =
      rtu_pck_rx_raw->packets[ForEach_itr].available;
    rtu_pck_rx_raw_packets = rtu_pck_rx_raw->packets[ForEach_itr].packet.ID;
    input = static_cast<int32_T>(rtu_pck_rx_raw_packets & 1792U) >> 8;

    // Initialize output value to default value for CANClassTypes (Motor_Control_Command) 
    localB->CoreSubsys[ForEach_itr].pck_rx_struct.packet.ID.CLS =
      CANClassTypes_Motor_Control_Command;
    if ((input <= 2) || (input >= 4)) {
      // Set output value to input value if it is a member of CANClassTypes
      localB->CoreSubsys[ForEach_itr].pck_rx_struct.packet.ID.CLS = static_cast<
        CANClassTypes>(input);
    }

    localB->CoreSubsys[ForEach_itr].pck_rx_struct.packet.ID.SRC =
      static_cast<uint8_T>(static_cast<int32_T>(rtu_pck_rx_raw_packets & 240U) >>
      4);
    localB->CoreSubsys[ForEach_itr].pck_rx_struct.packet.ID.DST_TYP =
      static_cast<uint8_T>(rtu_pck_rx_raw_packets & 15U);
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
    for (input = 0; input < 8; input++) {
      localB->CoreSubsys[ForEach_itr].pck_rx_struct.packet.PAYLOAD.ARG[input] =
        rtu_pck_rx_raw->packets[ForEach_itr].packet.PAYLOAD[input];
    }

    // End of MATLAB Function: '<S2>/RAW2STRUCT Decoding Logic'

    // Outputs for Atomic SubSystem: '<S2>/CAN_Decoder'
    // Chart: '<S3>/Decoding Logic' incorporates:
    //   Constant: '<S3>/Constant'
    //   Constant: '<S3>/Constant1'
    //   Constant: '<S3>/Constant2'

    can_decoder_DecodingLogic(localB->CoreSubsys[ForEach_itr].
      pck_rx_struct.available, &localB->CoreSubsys[ForEach_itr].
      pck_rx_struct.packet, CAN_ID_AMC, 5, 10.0F, &localB->
      CoreSubsys[ForEach_itr].sf_DecodingLogic, &localDW->CoreSubsys[ForEach_itr]
      .sf_DecodingLogic);

    // ForEachSliceAssignment generated from: '<S2>/message_rx' incorporates:
    //   BusCreator: '<S3>/Bus Creator'
    //   Constant: '<S3>/motor_id'

    rty_messages_rx[ForEach_itr].motor_id = 0U;
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
    rty_messages_rx[ForEach_itr].motor_config_set = localB->
      CoreSubsys[ForEach_itr].sf_DecodingLogic.msg_set_motor_config_extra;

    // End of Outputs for SubSystem: '<S2>/CAN_Decoder'
  }

  // End of Outputs for SubSystem: '<S1>/For Each Subsystem'
}

// Model initialize function
void can_decoder_initialize(const char_T **rt_errorStatus,
  RT_MODEL_can_decoder_T *const can_decoder_M)
{
  // Registration code

  // initialize error status
  can_decoder_M->setErrorStatusPointer(rt_errorStatus);
}

const char_T* RT_MODEL_can_decoder_T::getErrorStatus() const
{
  return (*(errorStatus));
}

void RT_MODEL_can_decoder_T::setErrorStatus(const char_T* const aErrorStatus)
  const
{
  (*(errorStatus) = aErrorStatus);
}

const char_T** RT_MODEL_can_decoder_T::getErrorStatusPointer() const
{
  return errorStatus;
}

void RT_MODEL_can_decoder_T::setErrorStatusPointer(const char_T
  ** aErrorStatusPointer)
{
  (errorStatus = aErrorStatusPointer);
}

//
// File trailer for generated code.
//
// [EOF]
//
