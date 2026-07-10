//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: process_sensors_types.h
//
// Code generated for Simulink model 'process_sensors'.
//
// Model version                  : 2.1
// Simulink Coder version         : 26.1 (R2026a) 20-Nov-2025
// C/C++ source code generated on : Thu Jul  9 10:01:18 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef PROCESS_SENSORS_TYPES_H_
#define PROCESS_SENSORS_TYPES_H_
#include "rtwtypes.h"
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

#ifndef DEFINED_TYPEDEF_FOR_REFERENCEENCODER_
#define DEFINED_TYPEDEF_FOR_REFERENCEENCODER_

typedef enum {
  ReferenceEncoder_Motor = 0,          // Default value
  ReferenceEncoder_Joint
} ReferenceEncoder;

#endif

#ifndef DEFINED_TYPEDEF_FOR_MOTORCONFIGURATION_
#define DEFINED_TYPEDEF_FOR_MOTORCONFIGURATION_

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
  boolean_T hall_sensors_swapBC;
  ReferenceEncoder reference_encoder;
};

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

#ifndef DEFINED_TYPEDEF_FOR_PIDSCONFIGURATION_
#define DEFINED_TYPEDEF_FOR_PIDSCONFIGURATION_

struct PIDsConfiguration
{
  PID currentPID;
  PID velocityPID;
  PID positionPID;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_THRESHOLDS_
#define DEFINED_TYPEDEF_FOR_THRESHOLDS_

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

#ifndef DEFINED_TYPEDEF_FOR_ACTUATORCONFIGURATION_
#define DEFINED_TYPEDEF_FOR_ACTUATORCONFIGURATION_

struct ActuatorConfiguration
{
  Thresholds thresholds;
  PIDsConfiguration pids;
  MotorConfiguration motor;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_CALIBRATIONTYPES_
#define DEFINED_TYPEDEF_FOR_CALIBRATIONTYPES_

typedef enum {
  CalibrationTypes_None = 0,           // Default value
  CalibrationTypes_Search_Index,
  CalibrationTypes_Full_Calibration
} CalibrationTypes;

#endif

#ifndef DEFINED_TYPEDEF_FOR_DRIVERSENSORS_
#define DEFINED_TYPEDEF_FOR_DRIVERSENSORS_

struct DriverSensors
{
  // power supply voltage
  real32_T Vcc;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_HARDWAREFAULTS_
#define DEFINED_TYPEDEF_FOR_HARDWAREFAULTS_

struct HardwareFaults
{
  boolean_T overcurrent;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_FLAGS_
#define DEFINED_TYPEDEF_FOR_FLAGS_

struct Flags
{
  // Flag thath enables offset calibration in case of Full Calibration required
  boolean_T emit_offset_calibration;

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

#ifndef DEFINED_TYPEDEF_FOR_QUADRATUREENCODER_
#define DEFINED_TYPEDEF_FOR_QUADRATUREENCODER_

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

#ifndef DEFINED_TYPEDEF_FOR_MOTORSENSORS_
#define DEFINED_TYPEDEF_FOR_MOTORSENSORS_

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

#ifndef DEFINED_TYPEDEF_FOR_SENSORSDATA_
#define DEFINED_TYPEDEF_FOR_SENSORSDATA_

struct SensorsData
{
  DriverSensors driversensors;
  MotorSensors motorsensors;
};

#endif
#endif                                 // PROCESS_SENSORS_TYPES_H_

//
// File trailer for generated code.
//
// [EOF]
//
