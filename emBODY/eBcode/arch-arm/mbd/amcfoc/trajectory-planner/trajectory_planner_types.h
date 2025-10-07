//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: trajectory_planner_types.h
//
// Code generated for Simulink model 'trajectory_planner'.
//
// Model version                  : 2.33
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Tue Oct  7 09:47:50 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef trajectory_planner_types_h_
#define trajectory_planner_types_h_
#include "rtwtypes.h"
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

#ifndef DEFINED_TYPEDEF_FOR_Targets_
#define DEFINED_TYPEDEF_FOR_Targets_

struct Targets
{
  // Target time for position control
  real32_T trajectory_time;
  real32_T position;
  real32_T velocity;
  real32_T current;
  real32_T voltage;
};

#endif
#endif                                 // trajectory_planner_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
