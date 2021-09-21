//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: can_rx_raw2struct_types.h
//
// Code generated for Simulink model 'can_rx_raw2struct'.
//
// Model version                  : 1.315
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Mon Sep 20 12:43:51 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_can_rx_raw2struct_types_h_
#define RTW_HEADER_can_rx_raw2struct_types_h_
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
  // Number of available output packets.
  uint8_T available;
  uint8_T lengths;
  BUS_CAN_PACKET packets;
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

  // 7 bytes for the command argument.
  uint8_T ARG[7];
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
  // Number of available input packets.
  uint8_T available;
  BUS_CAN_PACKET_RX packets;
};

#endif
#endif                                 // RTW_HEADER_can_rx_raw2struct_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
