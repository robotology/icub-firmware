//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_encoder_types.h
//
// Code generated for Simulink model 'can_encoder'.
//
// Model version                  : 3.26
// Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
// C/C++ source code generated on : Tue Aug  9 15:18:24 2022
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
  // Current feedback in A.
  real32_T current;

  // Position feedback in deg.
  real32_T position;

  // Velocity feedback in deg/s.
  real32_T velocity;
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

#ifndef DEFINED_TYPEDEF_FOR_BUS_FLAGS_TX_
#define DEFINED_TYPEDEF_FOR_BUS_FLAGS_TX_

struct BUS_FLAGS_TX
{
  boolean_T dirty;
  boolean_T stuck;
  boolean_T index_broken;
  boolean_T phase_broken;
  real32_T not_calibrated;
  boolean_T ExternalFaultAsserted;
  boolean_T UnderVoltageFailure;
  boolean_T OverVoltageFailure;
  boolean_T OverCurrentFailure;
  boolean_T DHESInvalidValue;
  boolean_T AS5045CSumError;
  boolean_T DHESInvalidSequence;
  boolean_T CANInvalidProtocol;
  boolean_T CAN_BufferOverRun;
  boolean_T SetpointExpired;
  boolean_T CAN_TXIsPasv;
  boolean_T CAN_RXIsPasv;
  boolean_T CAN_IsWarnTX;
  boolean_T CAN_IsWarnRX;
  boolean_T OverHeating;
  boolean_T ADCCalFailure;
  boolean_T I2TFailure;
  boolean_T EMUROMFault;
  boolean_T EMUROMCRCFault;
  boolean_T EncoderFault;
  boolean_T FirmwareSPITimingError;
  boolean_T AS5045CalcError;
  boolean_T FirmwarePWMFatalError;
  boolean_T CAN_TXWasPasv;
  boolean_T CAN_RXWasPasv;
  boolean_T CAN_RTRFlagActive;
  boolean_T CAN_WasWarn;
  boolean_T CAN_DLCError;
  boolean_T SiliconRevisionFault;
  boolean_T PositionLimitUpper;
  boolean_T PositionLimitLower;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_MSG_STATUS_
#define DEFINED_TYPEDEF_FOR_BUS_MSG_STATUS_

struct BUS_MSG_STATUS
{
  MCControlModes control_mode;
  real32_T pwm_fbk;
  BUS_FLAGS_TX flags;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_MESSAGES_TX_
#define DEFINED_TYPEDEF_FOR_BUS_MESSAGES_TX_

// Aggregate of all CAN transmitted messages.
struct BUS_MESSAGES_TX
{
  BUS_MSG_FOC foc;
  BUS_MSG_STATUS status;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_BUS_STATUS_TX_
#define DEFINED_TYPEDEF_FOR_BUS_STATUS_TX_

// Aggregate of all events specifying types of transmitted messages.
struct BUS_STATUS_TX
{
  boolean_T foc;
  boolean_T status;
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

// Forward declaration for rtModel
typedef struct tag_RTM_can_encoder_T RT_MODEL_can_encoder_T;

#endif                                 // RTW_HEADER_can_encoder_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
