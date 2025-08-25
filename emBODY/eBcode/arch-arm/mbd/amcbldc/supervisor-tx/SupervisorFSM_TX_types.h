//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_TX_types.h
//
// Code generated for Simulink model 'SupervisorFSM_TX'.
//
// Model version                  : 10.22
// Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
// C/C++ source code generated on : Mon Aug 11 10:29:49 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef SupervisorFSM_TX_types_h_
#define SupervisorFSM_TX_types_h_
#include "rtwtypes.h"
#ifndef DEFINED_TYPEDEF_FOR_DriverSensors_
#define DEFINED_TYPEDEF_FOR_DriverSensors_

struct DriverSensors
{
  // power supply voltage
  real32_T Vcc;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_QuadratureEncoder_
#define DEFINED_TYPEDEF_FOR_QuadratureEncoder_

struct QuadratureEncoder
{
  // Offset of the rotor-stator calibration, difference angle between the 0 of the sensors(index) and the electrical zero 
  real32_T offset;

  // Mechanical Angle before gearbox
  real32_T rotor_angle;

  // Counter of the QENC
  real32_T counter;

  // Last QENC count where the index has been detected
  real32_T Idx_counter;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MotorSensors_
#define DEFINED_TYPEDEF_FOR_MotorSensors_

struct MotorSensors
{
  QuadratureEncoder qencoder;
  real32_T Iabc[3];

  // electrical angle = angle * pole_pairs
  real32_T electrical_angle;
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
  DriverSensors driversensors;
  MotorSensors motorsensors;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_EstimatedData_
#define DEFINED_TYPEDEF_FOR_EstimatedData_

struct EstimatedData
{
  // velocity
  real32_T rotor_velocity;

  // filtered motor current
  real32_T Iq_filtered;

  // motor temperature
  real32_T motor_temperature;

  // velocity
  real32_T joint_velocity;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_CalibrationTypes_
#define DEFINED_TYPEDEF_FOR_CalibrationTypes_

typedef enum {
  CalibrationTypes_None = 0,           // Default value
  CalibrationTypes_Search_Index,
  CalibrationTypes_Full_Calibration
} CalibrationTypes;

#endif

#ifndef DEFINED_TYPEDEF_FOR_HardwareFaults_
#define DEFINED_TYPEDEF_FOR_HardwareFaults_

struct HardwareFaults
{
  boolean_T overcurrent;
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

#ifndef DEFINED_TYPEDEF_FOR_Flags_
#define DEFINED_TYPEDEF_FOR_Flags_

struct Flags
{
  // Flag that shows if:
  // 0. None calibration
  // 1. Search Index must be done
  // 2. Full calibration must be done
  CalibrationTypes calibration_type;
  boolean_T calibration_done;
  boolean_T enable_sending_msg_status;
  HardwareFaults hw_faults;
  boolean_T enable_thermal_protection;

  // control mode
  ControlModes control_mode;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_FOCOutputs_
#define DEFINED_TYPEDEF_FOR_FOCOutputs_

struct FOCOutputs
{
  boolean_T calibrationdone;

  // control effort (quadrature)
  real32_T Vq;

  // control effort (3-phases)
  real32_T Vabc[3];

  // quadrature current
  real32_T Iq_fbk;

  // direct current
  real32_T Id_fbk;

  // RMS of Iq
  real32_T Iq_rms;

  // RMS of Id
  real32_T Id_rms;
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

  // control effort (quadrature)
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
#endif                                 // SupervisorFSM_TX_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
