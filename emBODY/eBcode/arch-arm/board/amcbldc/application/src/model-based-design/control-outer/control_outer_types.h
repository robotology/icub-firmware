//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_outer_types.h
//
// Code generated for Simulink model 'control_outer'.
//
// Model version                  : 6.3
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Mon Jan 15 14:41:42 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_control_outer_types_h_
#define RTW_HEADER_control_outer_types_h_
#include "rtwtypes.h"
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
  boolean_T enable_sending_msg_status;
  boolean_T fault_button;
  boolean_T enable_thermal_protection;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MotorConfig_
#define DEFINED_TYPEDEF_FOR_MotorConfig_

struct MotorConfig
{
  // Angular offset in degrees between the stator windings and the hall sensors. 
  real32_T hall_sens_offset;
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
  real32_T Kbemf;
  real32_T Rphase;
  real32_T Imin;
  real32_T Imax;
  real32_T Vmax;
  real32_T resistance;
  real32_T inductance;
  real32_T thermal_resistance;
  real32_T thermal_time_constant;
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

  // Forgetting factor in [0, 1] for exponential weighting-based estimation of RMS current value 
  real32_T current_rms_lambda;
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

  // The critical temperature of the motor that triggers i2t current protection. 
  real32_T motorCriticalTemperature;
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
  real32_T environment_temperature;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_JointPositions_
#define DEFINED_TYPEDEF_FOR_JointPositions_

struct JointPositions
{
  // joint positions
  real32_T position;
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

#ifndef DEFINED_TYPEDEF_FOR_MotorCurrent_
#define DEFINED_TYPEDEF_FOR_MotorCurrent_

struct MotorCurrent
{
  // motor current
  real32_T current;
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

#ifndef DEFINED_TYPEDEF_FOR_MotorSensors_
#define DEFINED_TYPEDEF_FOR_MotorSensors_

struct MotorSensors
{
  real32_T Iabc[3];

  // electrical angle = angle * pole_pairs
  real32_T angle;
  real32_T temperature;
  real32_T voltage;
  real32_T current;
  uint8_T hallABC;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SupplyVoltage_
#define DEFINED_TYPEDEF_FOR_SupplyVoltage_

struct SupplyVoltage
{
  real32_T voltage;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_SensorsData_
#define DEFINED_TYPEDEF_FOR_SensorsData_

struct SensorsData
{
  // position encoders
  JointPositions jointpositions;

  // motor probes
  MotorSensors motorsensors;

  // supply probes
  SupplyVoltage supplyvoltagesensors;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_MotorTemperature_
#define DEFINED_TYPEDEF_FOR_MotorTemperature_

struct MotorTemperature
{
  // motor temperature
  real32_T temperature;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_EstimatedData_
#define DEFINED_TYPEDEF_FOR_EstimatedData_

struct EstimatedData
{
  // velocity
  JointVelocities jointvelocities;

  // filtered motor current
  MotorCurrent Iq_filtered;

  // motor temperature
  MotorTemperature motor_temperature;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_ControlOuterOutputs_
#define DEFINED_TYPEDEF_FOR_ControlOuterOutputs_

struct ControlOuterOutputs
{
  boolean_T vel_en;
  boolean_T cur_en;
  boolean_T out_en;
  boolean_T pid_reset;
  MotorCurrent motorcurrent;
  real32_T current_limiter;
};

#endif

// Forward declaration for rtModel
typedef struct tag_RTM_control_outer_T RT_MODEL_control_outer_T;

#endif                                 // RTW_HEADER_control_outer_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
