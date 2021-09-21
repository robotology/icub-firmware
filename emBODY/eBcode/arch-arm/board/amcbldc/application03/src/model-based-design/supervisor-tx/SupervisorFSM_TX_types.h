//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: SupervisorFSM_TX_types.h
//
// Code generated for Simulink model 'SupervisorFSM_TX'.
//
// Model version                  : 2.50
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Mon Sep 20 12:43:29 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_SupervisorFSM_TX_types_h_
#define RTW_HEADER_SupervisorFSM_TX_types_h_
#include "rtwtypes.h"

// Model Code Variants
#ifndef DEFINED_TYPEDEF_FOR_JointPositions_
#define DEFINED_TYPEDEF_FOR_JointPositions_

struct JointPositions
{
  // joint positions
  real32_T position;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_Thresholds_
#define DEFINED_TYPEDEF_FOR_Thresholds_

struct Thresholds
{
  real32_T current_low;
  real32_T current_high;
  real32_T voltage_low;
  real32_T voltage_high;
  real32_T temperature_low;
  real32_T temperature_high;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MotorSensors_
#define DEFINED_TYPEDEF_FOR_MotorSensors_

struct MotorSensors
{
  real32_T Iabc[3];
  real32_T angle;
  real32_T omega;
  real32_T temperature;
  real32_T voltage;
  Thresholds threshold;
  real32_T current;
  uint8_T hallABC;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SensorsData_
#define DEFINED_TYPEDEF_FOR_SensorsData_

struct SensorsData
{
  // position encoders
  JointPositions jointpositions;
  MotorSensors motorsensors;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MCControlModes_
#define DEFINED_TYPEDEF_FOR_MCControlModes_

typedef enum {
  MCControlModes_Idle = 0,             // Default value
  MCControlModes_OpenLoop = 80,
  MCControlModes_SpeedVoltage = 10,
  MCControlModes_Current = 6
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

  // Nominal current in mA.
  int16_T nominal;

  // Peak current in mA.
  uint16_T peak;

  // Overload current in mA.
  uint16_T overload;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_MSG_DESIRED_CURRENT_
#define DEFINED_TYPEDEF_FOR_BUS_MSG_DESIRED_CURRENT_

// Fields of a DESIRED_CURRENT message.
struct BUS_MSG_DESIRED_CURRENT
{
  // Nominal current in mA.
  int16_T current;
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
#endif                                 // RTW_HEADER_SupervisorFSM_TX_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
