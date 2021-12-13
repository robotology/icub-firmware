//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_RX_types.h
//
// Code generated for Simulink model 'SupervisorFSM_RX'.
//
// Model version                  : 3.105
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Mon Dec 13 08:27:35 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_SupervisorFSM_RX_types_h_
#define RTW_HEADER_SupervisorFSM_RX_types_h_
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

#ifndef DEFINED_TYPEDEF_FOR_MotorSensors_
#define DEFINED_TYPEDEF_FOR_MotorSensors_

// electrical angle = angle * num_poles
struct MotorSensors
{
  real32_T Iabc[3];
  real32_T angle;
  real32_T temperature;
  real32_T voltage;
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

#ifndef DEFINED_TYPEDEF_FOR_MotorCurrent_
#define DEFINED_TYPEDEF_FOR_MotorCurrent_

struct MotorCurrent
{
  // motor current
  real32_T current;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_ControlOutputs_
#define DEFINED_TYPEDEF_FOR_ControlOutputs_

struct ControlOutputs
{
  // control effort
  real32_T Vabc[3];

  // quadrature current
  MotorCurrent Iq_fbk;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_JointVelocities_
#define DEFINED_TYPEDEF_FOR_JointVelocities_

struct JointVelocities
{
  // joint velocities
  real32_T velocity;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_EstimatedData_
#define DEFINED_TYPEDEF_FOR_EstimatedData_

struct EstimatedData
{
  // velocities
  JointVelocities jointvelocities;
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

#ifndef DEFINED_TYPEDEF_FOR_BoardState_
#define DEFINED_TYPEDEF_FOR_BoardState_

typedef enum {
  BoardState_NotConfigured = 0,        // Default value
  BoardState_Configured,
  BoardState_Fault
} BoardState;

#endif

#ifndef DEFINED_TYPEDEF_FOR_ControlModes_
#define DEFINED_TYPEDEF_FOR_ControlModes_

typedef enum {
  ControlModes_NotConfigured = 0,      // Default value
  ControlModes_Idle,
  ControlModes_Position,
  ControlModes_PositionDirect,
  ControlModes_Current,
  ControlModes_Velocity,
  ControlModes_Voltage,
  ControlModes_Torque,
  ControlModes_HwFaultCM
} ControlModes;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Flags_
#define DEFINED_TYPEDEF_FOR_Flags_

struct Flags
{
  // control mode
  ControlModes control_mode;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MotorVoltage_
#define DEFINED_TYPEDEF_FOR_MotorVoltage_

struct MotorVoltage
{
  // motor voltage
  real32_T voltage;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_Targets_
#define DEFINED_TYPEDEF_FOR_Targets_

struct Targets
{
  JointPositions jointpositions;
  JointVelocities jointvelocities;
  MotorCurrent motorcurrent;
  MotorVoltage motorvoltage;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_JointLimits_
#define DEFINED_TYPEDEF_FOR_JointLimits_

struct JointLimits
{
  // joint limits
  real32_T limits[2];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_VelocityLimits_
#define DEFINED_TYPEDEF_FOR_VelocityLimits_

struct VelocityLimits
{
  // velocity limits
  real32_T limits[2];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MotorConfig_
#define DEFINED_TYPEDEF_FOR_MotorConfig_

struct MotorConfig
{
  real32_T Imin;
  real32_T Imax;
  boolean_T has_hall_sens;
  uint8_T pole_pairs;
  real32_T reduction;
  real32_T Kp;
  real32_T Ki;
  boolean_T has_speed_sens;
  real32_T Kbemf;
  real32_T Rphase;
  real32_T Vmax;
  real32_T Vcc;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_PIDConfig_
#define DEFINED_TYPEDEF_FOR_PIDConfig_

struct PIDConfig
{
  real32_T OutMax;
  real32_T OutMin;
  real32_T P;
  real32_T I;
  real32_T D;
  real32_T N;
  real32_T I0;
  real32_T D0;
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

#ifndef DEFINED_TYPEDEF_FOR_ConfigurationParameters_
#define DEFINED_TYPEDEF_FOR_ConfigurationParameters_

struct ConfigurationParameters
{
  JointLimits jointlimits;
  VelocityLimits velocitylimits;
  MotorConfig motorconfig;
  PIDConfig PosLoopPID;
  PIDConfig VelLoopPID;
  PIDConfig DirLoopPID;
  Thresholds thresholds;
};

#endif
#endif                                 // RTW_HEADER_SupervisorFSM_RX_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
