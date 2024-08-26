//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_decoder_types.h
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 6.115
// Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
// C/C++ source code generated on : Mon Aug 26 12:00:41 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef can_decoder_types_h_
#define can_decoder_types_h_
#include "rtwtypes.h"
#include "can_decoder_types.h"

// Includes for objects with custom storage classes
#include "rtw_defines.h"

//
//  Constraints for division operations in dimension variants

#if (1 == 0) || ((CAN_MAX_NUM_PACKETS % 1) != 0)
# error "The preprocessor definition '1' must not be equal to zero and     the division of 'CAN_MAX_NUM_PACKETS' by '1' must not have a remainder."
#endif

//
//  Registered constraints for dimension variants

#if MAX_EVENTS_PER_TICK <= 0
# error "The preprocessor definition 'MAX_EVENTS_PER_TICK' must be greater than '0'"
#endif

#if CAN_MAX_NUM_PACKETS <= 0
# error "The preprocessor definition 'CAN_MAX_NUM_PACKETS' must be greater than '0'"
#endif

#if (CAN_MAX_NUM_PACKETS+1) <= MAX_EVENTS_PER_TICK
# error "The preprocessor definition '(CAN_MAX_NUM_PACKETS+1)' must be greater than 'MAX_EVENTS_PER_TICK'"
#endif

#if MAX_EVENTS_PER_TICK >= 16
# error "The preprocessor definition 'MAX_EVENTS_PER_TICK' must be less than '16'"
#endif

#if CAN_MAX_NUM_PACKETS >= 16
# error "The preprocessor definition 'CAN_MAX_NUM_PACKETS' must be less than '16'"
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

#ifndef DEFINED_TYPEDEF_FOR_BUS_CAN_MULTIPLE_
#define DEFINED_TYPEDEF_FOR_BUS_CAN_MULTIPLE_

struct BUS_CAN_MULTIPLE
{
  BUS_CAN packets[CAN_MAX_NUM_PACKETS];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_Thresholds_
#define DEFINED_TYPEDEF_FOR_Thresholds_

struct Thresholds
{
  // Can be only non-negative
  real32_T jntVelMax;

  // Current that can be kept for an indefinite period of time w/o damaging the motor
  // Expressed in [A] as all the internal computations are done this way
  // Can be only non-negative
  real32_T motorNominalCurrents;

  // Current that can be applied for a short period of time
  // Expressed in [A] as all the internal computations are done this way
  // Can be only non-negative
  real32_T motorPeakCurrents;

  // Currents over this threshold can instantaneously damages the motor
  // Expressed in [A] as all the internal computations are done this way
  // Can be only non-negative
  real32_T motorOverloadCurrents;

  // Expressed in ticks
  // Max value is 32000
  // Can be only non-negative
  uint32_T motorPwmLimit;

  // The critical temperature of the motor that triggers i2t current protection. 
  real32_T motorCriticalTemperature;
};

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
  ControlModes_HwFaultCM
} ControlModes;

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

#ifndef DEFINED_TYPEDEF_FOR_PIDsConfiguration_
#define DEFINED_TYPEDEF_FOR_PIDsConfiguration_

struct PIDsConfiguration
{
  PID currentPID;
  PID velocityPID;
  PID positionPID;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MotorConfigurationExternal_
#define DEFINED_TYPEDEF_FOR_MotorConfigurationExternal_

struct MotorConfigurationExternal
{
  boolean_T enable_verbosity;
  boolean_T has_hall_sens;
  boolean_T has_quadrature_encoder;
  boolean_T has_speed_quadrature_encoder;
  boolean_T has_torque_sens;
  uint8_T encoder_tolerance;
  uint8_T pole_pairs;
  int16_T rotor_encoder_resolution;
  int16_T rotor_index_offset;
  boolean_T use_index;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MotorConfiguration_
#define DEFINED_TYPEDEF_FOR_MotorConfiguration_

struct MotorConfiguration
{
  MotorConfigurationExternal externals;
  real32_T Kbemf;
  real32_T Rphase;
  real32_T Imin;
  real32_T Imax;
  real32_T Vmax;
  real32_T resistance;
  real32_T inductance;
  real32_T thermal_resistance;
  real32_T thermal_time_constant;
  real32_T hall_sensors_offset;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_ActuatorConfiguration_
#define DEFINED_TYPEDEF_FOR_ActuatorConfiguration_

struct ActuatorConfiguration
{
  Thresholds thresholds;
  PIDsConfiguration pids;
  MotorConfiguration motor;
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

#ifndef DEFINED_TYPEDEF_FOR_EventTypes_
#define DEFINED_TYPEDEF_FOR_EventTypes_

typedef enum {
  EventTypes_None = 0,                 // Default value
  EventTypes_SetLimit,
  EventTypes_SetControlMode,
  EventTypes_SetMotorConfig,
  EventTypes_SetPid,
  EventTypes_SetTarget
} EventTypes;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Targets_
#define DEFINED_TYPEDEF_FOR_Targets_

struct Targets
{
  real32_T position;
  real32_T velocity;
  real32_T current;
  real32_T voltage;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SupervisorInputLimits_
#define DEFINED_TYPEDEF_FOR_SupervisorInputLimits_

struct SupervisorInputLimits
{
  real32_T overload;
  real32_T peak;
  real32_T nominal;
  ControlModes type;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_ReceivedEvents_
#define DEFINED_TYPEDEF_FOR_ReceivedEvents_

struct ReceivedEvents
{
  EventTypes event_type;
  Targets targets_content;
  PID pid_content;
  ControlModes control_mode_content;
  SupervisorInputLimits limits_content;
  MotorConfigurationExternal motor_config_content;
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

#ifndef DEFINED_TYPEDEF_FOR_MCOPC_
#define DEFINED_TYPEDEF_FOR_MCOPC_

typedef enum {
  MCOPC_Set_Control_Mode = 9,          // Default value
  MCOPC_Set_Current_Limit = 72,
  MCOPC_Set_Current_PID = 101,
  MCOPC_Set_Velocity_PID = 105,
  MCOPC_Set_Motor_Config = 119
} MCOPC;

#endif

#ifndef DEFINED_TYPEDEF_FOR_MCStreaming_
#define DEFINED_TYPEDEF_FOR_MCStreaming_

typedef enum {
  MCStreaming_Desired_Targets = 15,    // Default value
  MCStreaming_FOC = 0
} MCStreaming;

#endif

// Forward declaration for rtModel
typedef struct tag_RTM_can_decoder_T RT_MODEL_can_decoder_T;

#endif                                 // can_decoder_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
