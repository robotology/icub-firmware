//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: can_encoder_types.h
//
// Code generated for Simulink model 'can_encoder'.
//
// Model version                  : 1.322
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Mon Sep 20 12:43:44 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_can_encoder_types_h_
#define RTW_HEADER_can_encoder_types_h_
#include "rtwtypes.h"

// Model Code Variants
#ifndef DEFINED_TYPEDEF_FOR_BUS_MSG_FOC_
#define DEFINED_TYPEDEF_FOR_BUS_MSG_FOC_

// Fields of a FOC message.
struct BUS_MSG_FOC
{
  // Current feedback in mA.
  real32_T current;

  // Position feedback in deg.
  real32_T position;

  // Velocity feedback in deg/s.
  real32_T velocity;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_MESSAGES_TX_
#define DEFINED_TYPEDEF_FOR_BUS_MESSAGES_TX_

// Aggregate of all CAN transmitted messages.
struct BUS_MESSAGES_TX
{
  BUS_MSG_FOC foc;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_EVENTS_TX_
#define DEFINED_TYPEDEF_FOR_BUS_EVENTS_TX_

// Aggregate of all events specifying types of transmitted messages.
struct BUS_EVENTS_TX
{
  boolean_T foc;
};

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
  // Number of available output packets.
  uint8_T available;
  uint8_T lengths;
  BUS_CAN_PACKET packets;
};

#endif
#endif                                 // RTW_HEADER_can_encoder_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
