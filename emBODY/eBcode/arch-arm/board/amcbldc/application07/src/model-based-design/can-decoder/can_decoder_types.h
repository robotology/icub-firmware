//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_decoder_types.h
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 3.49
// Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
// C/C++ source code generated on : Tue Aug  9 15:18:14 2022
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

#ifndef DEFINED_TYPEDEF_FOR_MotorConfig_
#define DEFINED_TYPEDEF_FOR_MotorConfig_

struct MotorConfig
{
  boolean_T has_hall_sens;
  boolean_T has_quadrature_encoder;
  boolean_T has_speed_quadrature_encoder;
  boolean_T has_torque_sens;
  boolean_T use_index;
  boolean_T enable_verbosity;
  int16_T rotor_encoder_resolution;
  int16_T rotor_index_offset;
  uint8_T encoder_tolerance;
  uint8_T pole_pairs;
  real32_T reduction;
  real32_T Kbemf;
  real32_T Rphase;
  real32_T Imin;
  real32_T Imax;
  real32_T Vcc;
  real32_T Vmax;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_EstimationVelocityModes_
#define DEFINED_TYPEDEF_FOR_EstimationVelocityModes_

typedef enum {
  EstimationVelocityModes_Disabled = 0,// Default value
  EstimationVelocityModes_MovingAverage,
  EstimationVelocityModes_LeastSquares
} EstimationVelocityModes;

#endif

#ifndef DEFINED_TYPEDEF_FOR_EstimationConfig_
#define DEFINED_TYPEDEF_FOR_EstimationConfig_

struct EstimationConfig
{
  EstimationVelocityModes velocity_mode;
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
  uint8_T shift_factor;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_Thresholds_
#define DEFINED_TYPEDEF_FOR_Thresholds_

struct Thresholds
{
  // It shall be greater than hardwareJntPosMin
  real32_T jntPosMin;

  // It shall be smaller than hardwareJntPosMax
  real32_T jntPosMax;

  // Imposed by hardware constraint
  real32_T hardwareJntPosMin;

  // Imposed by hardware constraint
  real32_T hardwareJntPosMax;

  // If robotMin == rotorMax == 0, there's no check
  real32_T rotorPosMin;

  // If robotMin == rotorMax == 0, there's no check
  real32_T rotorPosMax;

  // Can be only non-negative
  real32_T jntVelMax;

  // Timeout on reception of velocity setpoint
  // Can be only non-negative
  uint32_T velocityTimeout;

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
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_ConfigurationParameters_
#define DEFINED_TYPEDEF_FOR_ConfigurationParameters_

struct ConfigurationParameters
{
  MotorConfig motorconfig;
  EstimationConfig estimationconfig;
  PIDConfig CurLoopPID;
  PIDConfig PosLoopPID;
  PIDConfig VelLoopPID;
  PIDConfig DirLoopPID;
  Thresholds thresholds;
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

#ifndef DEFINED_TYPEDEF_FOR_BUS_MSG_DESIRED_TARGETS_
#define DEFINED_TYPEDEF_FOR_BUS_MSG_DESIRED_TARGETS_

// Fields of a DESIRED_TARGETS message.
struct BUS_MSG_DESIRED_TARGETS
{
  // Target current in A.
  real32_T current;

  // Target voltage in %.
  real32_T voltage;

  // Target veocity in deg/s.
  real32_T velocity;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_MSG_PID_
#define DEFINED_TYPEDEF_FOR_BUS_MSG_PID_

// Fields of a CURRENT_PID message.
struct BUS_MSG_PID
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

#ifndef DEFINED_TYPEDEF_FOR_BUS_MSG_MOTOR_CONFIG_
#define DEFINED_TYPEDEF_FOR_BUS_MSG_MOTOR_CONFIG_

struct BUS_MSG_MOTOR_CONFIG
{
  boolean_T has_hall_sens;
  boolean_T has_quadrature_encoder;
  boolean_T has_speed_quadrature_encoder;
  boolean_T has_torque_sens;
  boolean_T use_index;
  boolean_T enable_verbosity;

  // Number of polese of the motor.
  uint8_T number_poles;

  // Encoder tolerance.
  uint8_T encoder_tolerance;

  // Resolution of rotor encoder.
  int16_T rotor_encoder_resolution;

  // Offset of the rotor encoder.
  int16_T rotor_index_offset;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_MESSAGES_RX_
#define DEFINED_TYPEDEF_FOR_BUS_MESSAGES_RX_

// Aggregate of all CAN received messages.
struct BUS_MESSAGES_RX
{
  BUS_MSG_CONTROL_MODE control_mode;
  BUS_MSG_CURRENT_LIMIT current_limit;
  BUS_MSG_DESIRED_TARGETS desired_targets;
  BUS_MSG_PID pid;
  BUS_MSG_MOTOR_CONFIG motor_config;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_MESSAGES_RX_MULTIPLE_
#define DEFINED_TYPEDEF_FOR_BUS_MESSAGES_RX_MULTIPLE_

struct BUS_MESSAGES_RX_MULTIPLE
{
  BUS_MESSAGES_RX messages[4];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_STATUS_RX_
#define DEFINED_TYPEDEF_FOR_BUS_STATUS_RX_

// Aggregate of all events specifying types of received messages.
struct BUS_STATUS_RX
{
  boolean_T control_mode;
  boolean_T current_limit;
  boolean_T desired_targets;
  boolean_T current_pid;
  boolean_T velocity_pid;
  boolean_T motor_config;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_STATUS_RX_MULTIPLE_
#define DEFINED_TYPEDEF_FOR_BUS_STATUS_RX_MULTIPLE_

struct BUS_STATUS_RX_MULTIPLE
{
  BUS_STATUS_RX status[4];
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

#endif                                 // RTW_HEADER_can_decoder_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
