//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_decoder_types.h
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 11.0
// Simulink Coder version         : 26.1 (R2026a) 20-Nov-2025
// C/C++ source code generated on : Thu Jul  9 10:00:28 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef CAN_DECODER_TYPES_H_
#define CAN_DECODER_TYPES_H_
#include "rtwtypes.h"

// Includes for objects with custom storage classes
#include "rtw_defines.h"

//
//  Constraints for division operations in dimension variants

#if ((MAX_EVENTS_PER_TICK % 1) != 0)
# error "The preprocessor definition '1' must not be equal to zero and     the division of 'MAX_EVENTS_PER_TICK' by '1' must not have a remainder."
#endif

//
//  Registered constraints for dimension variants

// Constraint 'MAX_EVENTS_PER_TICK == 4' registered by:
//  '<S2>/message_rx'

#if MAX_EVENTS_PER_TICK != 4
# error "The preprocessor definition 'MAX_EVENTS_PER_TICK' must be equal to '4'"
#endif

#if (MAX_EVENTS_PER_TICK+1) <= MAX_EVENTS_PER_TICK
# error "The preprocessor definition '(MAX_EVENTS_PER_TICK+1)' must be greater than 'MAX_EVENTS_PER_TICK'"
#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_CAN_PACKET_
#define DEFINED_TYPEDEF_FOR_BUS_CAN_PACKET_

// Fields of a transmitted CAN packet.
struct BUS_CAN_PACKET
{
  // ID of the CAN packet.
  uint16_T ID;

  // PAYLOAD of the CAN packet.
  uint8_T PAYLOAD[8];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_CAN_
#define DEFINED_TYPEDEF_FOR_BUS_CAN_

struct BUS_CAN
{
  // If true, the packet is available to be processed.
  boolean_T available;
  uint8_T length;
  BUS_CAN_PACKET packet;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_CAN_CMD_
#define DEFINED_TYPEDEF_FOR_BUS_CAN_CMD_

struct BUS_CAN_CMD
{
  // 1 bits for motor selector.
  boolean_T M;

  // 7 bits defining the operational code of the command.
  uint8_T OPC;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_CANCLASSTYPES_
#define DEFINED_TYPEDEF_FOR_CANCLASSTYPES_

typedef enum {
  CANClassTypes_Motor_Control_Command = 0,// Default value
  CANClassTypes_Motor_Control_Streaming = 1,
  CANClassTypes_Analog_Sensors_Command = 2,
  CANClassTypes_Skin_Sensor_Streaming = 4,
  CANClassTypes_Inertial_Sensor_Streaming = 5,
  CANClassTypes_Future_Use = 6,
  CANClassTypes_Management_Bootloader = 7
} CANClassTypes;

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_CAN_ID_RX_
#define DEFINED_TYPEDEF_FOR_BUS_CAN_ID_RX_

struct BUS_CAN_ID_RX
{
  // 3 bits defining the message class type.
  CANClassTypes CLS;

  // 4 bits defining the source ID.
  uint8_T SRC;

  // 4 bits definint the destination ID or the message sub-type.
  uint8_T DST_TYP;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_CAN_MULTIPLE_
#define DEFINED_TYPEDEF_FOR_BUS_CAN_MULTIPLE_

struct BUS_CAN_MULTIPLE
{
  BUS_CAN packets[MAX_EVENTS_PER_TICK];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_CAN_PAYLOAD_RX_
#define DEFINED_TYPEDEF_FOR_BUS_CAN_PAYLOAD_RX_

struct BUS_CAN_PAYLOAD_RX
{
  // Actual length of the total PAYLOAD field.
  uint8_T LEN;
  BUS_CAN_CMD CMD;

  // 8 bytes for the command argument in order to account also message of type streaming. 
  uint8_T ARG[8];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_CAN_PACKET_RX_
#define DEFINED_TYPEDEF_FOR_BUS_CAN_PACKET_RX_

// Fields of a received CAN packet.
struct BUS_CAN_PACKET_RX
{
  // ID of the CAN packet.
  BUS_CAN_ID_RX ID;

  // PAYLOAD of the CAN packet.
  BUS_CAN_PAYLOAD_RX PAYLOAD;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_CAN_RX_
#define DEFINED_TYPEDEF_FOR_BUS_CAN_RX_

// Specifies the CAN input.
struct BUS_CAN_RX
{
  // If true, the packet is available to be processed.
  boolean_T available;
  BUS_CAN_PACKET_RX packet;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_CANERRORTYPES_
#define DEFINED_TYPEDEF_FOR_CANERRORTYPES_

typedef enum {
  CANErrorTypes_No_Error = 0,          // Default value
  CANErrorTypes_Packet_Not4Us,
  CANErrorTypes_Packet_Unrecognized,
  CANErrorTypes_Packet_Malformed,
  CANErrorTypes_Packet_MultiFunctionsDetected,
  CANErrorTypes_Mode_Unrecognized
} CANErrorTypes;

#endif

#ifndef DEFINED_TYPEDEF_FOR_CONTROLMODES_
#define DEFINED_TYPEDEF_FOR_CONTROLMODES_

typedef enum {
  ControlModes_NotConfigured = 0,      // Default value
  ControlModes_Idle,
  ControlModes_Position,
  ControlModes_PositionDirect,
  ControlModes_Current,
  ControlModes_Velocity,
  ControlModes_Voltage,
  ControlModes_HwFaultCM
} ControlModes;

#endif

#ifndef DEFINED_TYPEDEF_FOR_EVENTTYPES_
#define DEFINED_TYPEDEF_FOR_EVENTTYPES_

typedef enum {
  EventTypes_None = 0,                 // Default value
  EventTypes_SetLimit,
  EventTypes_SetControlMode,
  EventTypes_SetMotorConfig,
  EventTypes_SetPid,
  EventTypes_SetTarget,
  EventTypes_SetMotorParam
} EventTypes;

#endif

#ifndef DEFINED_TYPEDEF_FOR_MCCONTROLMODES_
#define DEFINED_TYPEDEF_FOR_MCCONTROLMODES_

typedef enum {
  MCControlModes_Idle = 0,             // Default value
  MCControlModes_Position = 1,
  MCControlModes_OpenLoop = 80,
  MCControlModes_SpeedVoltage = 10,
  MCControlModes_SpeedCurrent = 11,
  MCControlModes_Current = 6,
  MCControlModes_NotConfigured = 176,
  MCControlModes_HWFault = 160
} MCControlModes;

#endif

#ifndef DEFINED_TYPEDEF_FOR_MCMOTORPARAMSSET_
#define DEFINED_TYPEDEF_FOR_MCMOTORPARAMSSET_

typedef uint8_T MCMotorParamsSet;

// enum MCMotorParamsSet
const MCMotorParamsSet MCMotorParamsSet_None = 0U;// Default value
const MCMotorParamsSet MCMotorParamsSet_Kbemf = 1U;
const MCMotorParamsSet MCMotorParamsSet_hall = 2U;
const MCMotorParamsSet MCMotorParamsSet_elect_vmax = 3U;

#endif

#ifndef DEFINED_TYPEDEF_FOR_MCOPC_
#define DEFINED_TYPEDEF_FOR_MCOPC_

typedef enum {
  MCOPC_Set_Parameter = 1,             // Default value
  MCOPC_Set = 5,
  MCOPC_Set_Control_Mode = 9,
  MCOPC_Set_Current_Limit = 72,
  MCOPC_Set_Current_PID = 101,
  MCOPC_Set_Velocity_PID = 105,
  MCOPC_Set_Position_PID = 82,
  MCOPC_Set_Motor_Config = 119
} MCOPC;

#endif

#ifndef DEFINED_TYPEDEF_FOR_MCSTREAMING_
#define DEFINED_TYPEDEF_FOR_MCSTREAMING_

typedef enum {
  MCStreaming_Desired_Targets = 15,    // Default value
  MCStreaming_FOC = 0
} MCStreaming;

#endif

#ifndef DEFINED_TYPEDEF_FOR_MOTORCONFIGURATIONEXTSET_
#define DEFINED_TYPEDEF_FOR_MOTORCONFIGURATIONEXTSET_

struct MotorConfigurationExtSet
{
  MCMotorParamsSet key;
  real32_T value[2];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MOTORCONFIGURATIONEXTERNAL_
#define DEFINED_TYPEDEF_FOR_MOTORCONFIGURATIONEXTERNAL_

struct MotorConfigurationExternal
{
  boolean_T enable_verbosity;
  boolean_T has_hall_sens;
  boolean_T has_quadrature_encoder;
  boolean_T has_speed_quadrature_encoder;
  boolean_T has_temperature_sens;
  uint8_T encoder_tolerance;
  uint8_T pole_pairs;
  int16_T rotor_encoder_resolution;
  int16_T rotor_index_offset;
  boolean_T use_index;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_PID_
#define DEFINED_TYPEDEF_FOR_PID_

struct PID
{
  ControlModes type;
  real32_T OutMax;
  real32_T OutMin;
  real32_T P;
  real32_T I;
  real32_T D;
  real32_T N;
  real32_T I0;
  real32_T D0;
  uint8_T shift_factor;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SUPERVISORINPUTLIMITS_
#define DEFINED_TYPEDEF_FOR_SUPERVISORINPUTLIMITS_

struct SupervisorInputLimits
{
  real32_T overload;
  real32_T peak;
  real32_T nominal;
  ControlModes type;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_TARGETS_
#define DEFINED_TYPEDEF_FOR_TARGETS_

struct Targets
{
  // Target time for position control
  real32_T trajectory_time;
  real32_T position;
  real32_T velocity;
  real32_T current;
  real32_T voltage;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_RECEIVEDEVENTS_
#define DEFINED_TYPEDEF_FOR_RECEIVEDEVENTS_

struct ReceivedEvents
{
  uint8_T motor_id;
  EventTypes event_type;
  Targets targets_content;
  PID pid_content;
  ControlModes control_mode_content;
  SupervisorInputLimits limits_content;
  MotorConfigurationExternal motor_config_content;
  MotorConfigurationExtSet motor_config_set;
};

#endif

// Forward declaration for rtModel
typedef struct tag_RTM_can_decoder_T RT_MODEL_can_decoder_T;

#endif                                 // CAN_DECODER_TYPES_H_

//
// File trailer for generated code.
//
// [EOF]
//
