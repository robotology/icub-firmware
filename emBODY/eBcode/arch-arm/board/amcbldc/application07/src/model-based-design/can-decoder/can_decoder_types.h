//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_decoder_types.h
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 2.101
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Tue May 24 10:00:48 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_can_decoder_types_h_
#define RTW_HEADER_can_decoder_types_h_
#include "rtwtypes.h"

// Model Code Variants
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

#ifndef DEFINED_TYPEDEF_FOR_BUS_CAN_MULTIPLE_
#define DEFINED_TYPEDEF_FOR_BUS_CAN_MULTIPLE_

struct BUS_CAN_MULTIPLE
{
  BUS_CAN packets[4];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MCControlModes_
#define DEFINED_TYPEDEF_FOR_MCControlModes_

typedef enum {
  MCControlModes_Idle = 0,             // Default value
  MCControlModes_OpenLoop = 80,
  MCControlModes_SpeedVoltage = 10,
  MCControlModes_SpeedCurrent = 11,
  MCControlModes_Current = 6,
  MCControlModes_NotConfigured = 176,
  MCControlModes_HWFault = 160
} MCControlModes;

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_MSG_CONTROL_MODE_
#define DEFINED_TYPEDEF_FOR_BUS_MSG_CONTROL_MODE_

// Fields of a CONTROL_MODE message.
struct BUS_MSG_CONTROL_MODE
{
  // Motor selector.
  boolean_T motor;

  // Control mode.
  MCControlModes mode;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_MSG_CURRENT_LIMIT_
#define DEFINED_TYPEDEF_FOR_BUS_MSG_CURRENT_LIMIT_

// Fields of a CURRENT_LIMIT message.
struct BUS_MSG_CURRENT_LIMIT
{
  // Motor selector.
  boolean_T motor;

  // Nominal current in A.
  real32_T nominal;

  // Peak current in A.
  real32_T peak;

  // Overload current in A.
  real32_T overload;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_MSG_DESIRED_CURRENT_
#define DEFINED_TYPEDEF_FOR_BUS_MSG_DESIRED_CURRENT_

// Fields of a DESIRED_CURRENT message.
struct BUS_MSG_DESIRED_CURRENT
{
  // Nominal current in A.
  real32_T current;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_MSG_CURRENT_PID_
#define DEFINED_TYPEDEF_FOR_BUS_MSG_CURRENT_PID_

// Fields of a CURRENT_PID message.
struct BUS_MSG_CURRENT_PID
{
  // Motor selector.
  boolean_T motor;

  // Proportional gain.
  real32_T Kp;

  // Integral gain.
  real32_T Ki;

  // Derivative gain.
  real32_T Kd;

  // Shift factor.
  uint8_T Ks;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_MESSAGES_RX_
#define DEFINED_TYPEDEF_FOR_BUS_MESSAGES_RX_

// Aggregate of all CAN received messages.
struct BUS_MESSAGES_RX
{
  BUS_MSG_CONTROL_MODE control_mode;
  BUS_MSG_CURRENT_LIMIT current_limit;
  BUS_MSG_DESIRED_CURRENT desired_current;
  BUS_MSG_CURRENT_PID current_pid;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_MESSAGES_RX_MULTIPLE_
#define DEFINED_TYPEDEF_FOR_BUS_MESSAGES_RX_MULTIPLE_

struct BUS_MESSAGES_RX_MULTIPLE
{
  BUS_MESSAGES_RX messages[4];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_EVENTS_RX_
#define DEFINED_TYPEDEF_FOR_BUS_EVENTS_RX_

// Aggregate of all events specifying types of received messages.
struct BUS_EVENTS_RX
{
  boolean_T control_mode;
  boolean_T current_limit;
  boolean_T desired_current;
  boolean_T current_pid;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_EVENTS_RX_MULTIPLE_
#define DEFINED_TYPEDEF_FOR_BUS_EVENTS_RX_MULTIPLE_

struct BUS_EVENTS_RX_MULTIPLE
{
  BUS_EVENTS_RX events[4];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_CANErrorTypes_
#define DEFINED_TYPEDEF_FOR_CANErrorTypes_

typedef enum {
  CANErrorTypes_No_Error = 0,          // Default value
  CANErrorTypes_Packet_Not4Us,
  CANErrorTypes_Packet_Unrecognized,
  CANErrorTypes_Packet_Malformed,
  CANErrorTypes_Packet_MultiFunctionsDetected,
  CANErrorTypes_Mode_Unrecognized
} CANErrorTypes;

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_CAN_RX_ERRORS_
#define DEFINED_TYPEDEF_FOR_BUS_CAN_RX_ERRORS_

// Specifies the CAN error types.
struct BUS_CAN_RX_ERRORS
{
  // True if an error has been detected.
  boolean_T event;
  CANErrorTypes type;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_CAN_RX_ERRORS_MULTIPLE_
#define DEFINED_TYPEDEF_FOR_BUS_CAN_RX_ERRORS_MULTIPLE_

struct BUS_CAN_RX_ERRORS_MULTIPLE
{
  BUS_CAN_RX_ERRORS errors[4];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_CANClassTypes_
#define DEFINED_TYPEDEF_FOR_CANClassTypes_

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

#ifndef DEFINED_TYPEDEF_FOR_MCOPC_
#define DEFINED_TYPEDEF_FOR_MCOPC_

typedef enum {
  MCOPC_Set_Control_Mode = 9,          // Default value
  MCOPC_Set_Current_Limit = 72,
  MCOPC_Set_Current_PID = 101
} MCOPC;

#endif

#ifndef DEFINED_TYPEDEF_FOR_MCStreaming_
#define DEFINED_TYPEDEF_FOR_MCStreaming_

typedef enum {
  MCStreaming_Desired_Current = 15,    // Default value
  MCStreaming_FOC = 0
} MCStreaming;

#endif

// Forward declaration for rtModel
typedef struct tag_RTM_can_decoder_T RT_MODEL_can_decoder_T;

#endif                                 // RTW_HEADER_can_decoder_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
