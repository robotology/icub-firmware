//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: motion_controller.h
//
// Code generated for Simulink model 'motion_controller'.
//
// Model version                  : 6.15
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Thu Oct 16 10:37:03 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef motion_controller_h_
#define motion_controller_h_
#include "rtwtypes.h"
#include "motion_controller_types.h"
#include "estimation_velocity.h"
#include "filter_current.h"
#include "supervisor.h"
#include "trajectory_planner.h"
#include "position_velocity_cascade.h"
#include "zero_crossing_types.h"

// Includes for objects with custom storage classes
#include "rtw_defines.h"

// Block signals for model 'motion_controller'
struct B_motion_controller_c_T {
  SensorsData RateTransition1;         // '<Root>/Rate Transition1'
  FOCOutputs RateTransition;           // '<Root>/Rate Transition'
  Targets targets;                     // '<Root>/Motor Supervisor'
  uint32_T Velocityestimationwindow;   // '<S2>/Velocity estimation window'
  real32_T velocity;                   // '<S1>/Motor Velocity Estimator'
  real32_T velocity_j;                 // '<S1>/Joint Velocity Estimator'
  EstimationVelocityModes VelocityEstimationMode;// '<S2>/Velocity Estimation Mode' 
};

// Block states (default storage) for model 'motion_controller'
struct DW_motion_controller_f_T {
  MdlrefDW_estimation_velocity_T MotorVelocityEstimator_InstanceData;// '<S1>/Motor Velocity Estimator' 
  MdlrefDW_filter_current_T CurrentFilter_InstanceData;// '<S1>/Current Filter'
  MdlrefDW_estimation_velocity_T JointVelocityEstimator_InstanceData;// '<S1>/Joint Velocity Estimator' 
  MdlrefDW_supervisor_T MotorSupervisor_InstanceData;// '<Root>/Motor Supervisor' 
  MdlrefDW_trajectory_planner_T TrajectoryPlanner_InstanceData;// '<Root>/Trajectory Planner' 
  MdlrefDW_position_velocity_cascade_T Positionvelocitycascade_InstanceData;// '<Root>/Position velocity cascade' 
};

struct MdlrefDW_motion_controller_T {
  B_motion_controller_c_T rtb;
  DW_motion_controller_f_T rtdw;
};

// Model reference registration function
extern void motion_controller_initialize(DW_motion_controller_f_T *localDW);
extern void motion_controller_Init(Flags *rty_Flags, ActuatorConfiguration
  *rty_ActuatorsConfiguration, FOCSlowInputs *rty_FOCSlowInputs, SensorsData
  *rty_SensorDataCalibration, B_motion_controller_c_T *localB,
  DW_motion_controller_f_T *localDW);
extern void motion_controller_Enable(DW_motion_controller_f_T *localDW);
extern void motion_controller_Disable(DW_motion_controller_f_T *localDW);
extern void mc_step_1ms(const SensorsData *rtu_SensorData, const ExternalFlags
  *rtu_ExternalFlags, const ReceivedEvents rtu_Events[4], const
  ActuatorConfiguration *rtu_InitConf, const JointData *rtu_JointData, const
  FOCOutputs *rtu_FOCOutputs, EstimatedData *rty_EstimatedData, Flags *rty_Flags,
  ActuatorConfiguration *rty_ActuatorsConfiguration, FOCSlowInputs
  *rty_FOCSlowInputs, SensorsData *rty_SensorDataCalibration,
  B_motion_controller_c_T *localB, DW_motion_controller_f_T *localDW);
extern void motion_controller_Term(DW_motion_controller_f_T *localDW);

//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Use the MATLAB hilite_system command to trace the generated code back
//  to the model.  For example,
//
//  hilite_system('<S3>')    - opens system 3
//  hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'motion_controller'
//  '<S1>'   : 'motion_controller/Estimation'
//  '<S2>'   : 'motion_controller/Global configuration'
//  '<S3>'   : 'motion_controller/Estimation/Thermal model'
//  '<S4>'   : 'motion_controller/Estimation/Thermal model/Thermal model OFF'

#endif                                 // motion_controller_h_

//
// File trailer for generated code.
//
// [EOF]
//
