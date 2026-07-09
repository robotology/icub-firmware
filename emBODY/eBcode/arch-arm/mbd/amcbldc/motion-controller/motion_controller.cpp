//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: motion_controller.cpp
//
// Code generated for Simulink model 'motion_controller'.
//
// Model version                  : 6.15
// Simulink Coder version         : 26.1 (R2026a) 20-Nov-2025
// C/C++ source code generated on : Thu Jul  9 10:01:24 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "motion_controller.h"
#include "motion_controller_types.h"
#include "motion_controller_private.h"
#include "estimation_velocity.h"
#include "filter_current.h"
#include "supervisor.h"
#include "trajectory_planner.h"
#include "position_velocity_cascade.h"

// System initialize for referenced model: 'motion_controller'
void motion_controller_Init(Flags *rty_Flags, ActuatorConfiguration
  *rty_ActuatorsConfiguration, SensorsData *rty_SensorDataCalibration,
  B_motion_controller_c_T *localB, DW_motion_controller_f_T *localDW)
{
  // Start for Constant: '<S2>/Velocity Estimation Mode'
  localB->VelocityEstimationMode = EstimationVelocityModes_MovingAverage;

  // Start for Constant: '<S2>/Velocity estimation window'
  localB->Velocityestimationwindow = 64U;

  // SystemInitialize for BusCreator generated from: '<Root>/Bus Creator' incorporates:
  //   Constant: '<S2>/Environment Temperature'
  //   Constant: '<S2>/lambda'

  localB->estimation.environment_temperature = 25.0F;
  localB->estimation.current_rms_lambda = 0.995F;
  localB->estimation.velocity_est_mode = localB->VelocityEstimationMode;
  localB->estimation.velocity_est_window = localB->Velocityestimationwindow;

  // SystemInitialize for BusCreator generated from: '<Root>/Bus Creator'
  localB->BusConversion_InsertedFor_BusCreator_at_inport_0_BusCreator1.estimation
    = localB->estimation;

  // SystemInitialize for ModelReference: '<S1>/Motor Velocity Estimator'
  estimation_velocity_Init(&(localDW->MotorVelocityEstimator_InstanceData.rtdw));

  // SystemInitialize for ModelReference: '<S1>/Current Filter'
  filter_current_Init(&(localDW->CurrentFilter_InstanceData.rtdw));

  // SystemInitialize for ModelReference: '<S1>/Joint Velocity Estimator'
  estimation_velocity_Init(&(localDW->JointVelocityEstimator_InstanceData.rtdw));

  // SystemInitialize for ModelReference generated from: '<Root>/Motor Supervisor' 
  supervisor_Init(&localB->targets, rty_ActuatorsConfiguration, rty_Flags,
                  rty_SensorDataCalibration,
                  &(localDW->MotorSupervisor_InstanceData.rtdw));

  // SystemInitialize for ModelReference generated from: '<Root>/Trajectory Planner' 
  trajectory_planner_Init(&(localDW->TrajectoryPlanner_InstanceData.rtb),
    &(localDW->TrajectoryPlanner_InstanceData.rtdw));

  // SystemInitialize for ModelReference generated from: '<Root>/Position velocity cascade' 
  position_velocity_cascade_Init
    (&(localDW->Positionvelocitycascade_InstanceData.rtdw));
}

// Enable for referenced model: 'motion_controller'
void motion_controller_Enable(DW_motion_controller_f_T *localDW)
{
  // Enable for ModelReference generated from: '<Root>/Position velocity cascade' 
  position_velocity_cascade_Enable
    (&(localDW->Positionvelocitycascade_InstanceData.rtdw));
}

// Disable for referenced model: 'motion_controller'
void motion_controller_Disable(DW_motion_controller_f_T *localDW)
{
  // Disable for ModelReference generated from: '<Root>/Position velocity cascade' 
  position_velocity_cascade_Disable
    (&(localDW->Positionvelocitycascade_InstanceData.rtb),
     &(localDW->Positionvelocitycascade_InstanceData.rtdw));
}

// Output and update for referenced model: 'motion_controller'
void mc_step_1ms(const SensorsData *rtu_SensorData, const ExternalFlags
                 *rtu_ExternalFlags, const ReceivedEvents rtu_Events[4], const
                 ActuatorConfiguration *rtu_InitConf, const JointData
                 *rtu_JointData, const FOCOutputs *rtu_FOCOutputs, EstimatedData
                 *rty_EstimatedData, Flags *rty_Flags, ActuatorConfiguration
                 *rty_ActuatorsConfiguration, FOCSlowInputs *rty_FOCSlowInputs,
                 SensorsData *rty_SensorDataCalibration, B_motion_controller_c_T
                 *localB, DW_motion_controller_f_T *localDW)
{
  // local block i/o variables
  Targets rtb_TrajectoryPlanner;

  // Constant: '<S2>/Velocity Estimation Mode'
  localB->VelocityEstimationMode = EstimationVelocityModes_MovingAverage;

  // Constant: '<S2>/Velocity estimation window'
  localB->Velocityestimationwindow = 64U;

  // BusCreator generated from: '<Root>/Bus Creator' incorporates:
  //   Constant: '<S2>/Environment Temperature'
  //   Constant: '<S2>/lambda'

  localB->estimation.environment_temperature = 25.0F;
  localB->estimation.current_rms_lambda = 0.995F;
  localB->estimation.velocity_est_mode = localB->VelocityEstimationMode;
  localB->estimation.velocity_est_window = localB->Velocityestimationwindow;

  // BusCreator generated from: '<Root>/Bus Creator'
  localB->BusConversion_InsertedFor_BusCreator_at_inport_0_BusCreator1.estimation
    = localB->estimation;

  // RateTransition: '<Root>/Rate Transition1'
  localB->RateTransition1 = *rtu_SensorData;

  // ModelReference: '<S1>/Motor Velocity Estimator'
  estimation_velocity(&localB->VelocityEstimationMode,
                      &localB->RateTransition1.motorsensors.qencoder.rotor_angle,
                      &localB->Velocityestimationwindow,
                      &rty_EstimatedData->rotor_velocity,
                      &(localDW->MotorVelocityEstimator_InstanceData.rtdw));

  // RateTransition: '<Root>/Rate Transition'
  localB->RateTransition = *rtu_FOCOutputs;

  // ModelReference: '<S1>/Current Filter'
  filter_current(&localB->RateTransition, &rty_EstimatedData->Iq_filtered,
                 &(localDW->CurrentFilter_InstanceData.rtdw));

  // ModelReference: '<S1>/Joint Velocity Estimator' incorporates:
  //   Constant: '<S1>/Constant'

  estimation_velocity(&motion_controller_ConstP.Constant_Value,
                      &rtu_JointData->position,
                      &localB->Velocityestimationwindow,
                      &rty_EstimatedData->joint_velocity,
                      &(localDW->JointVelocityEstimator_InstanceData.rtdw));

  // BusCreator generated from: '<S1>/Estimation_BusCreator'
  rty_EstimatedData->motor_temperature = 0.0F;

  // ModelReference generated from: '<Root>/Motor Supervisor'
  supervisor(rtu_ExternalFlags, rty_EstimatedData, &localB->RateTransition,
             &localB->RateTransition1, &rtu_Events[0], rtu_InitConf,
             &localB->targets, rty_ActuatorsConfiguration, rty_Flags,
             rty_SensorDataCalibration,
             &(localDW->MotorSupervisor_InstanceData.rtdw));

  // ModelReference generated from: '<Root>/Trajectory Planner'
  trajectory_planner(rty_Flags, &localB->targets, &localB->RateTransition1,
                     &rtb_TrajectoryPlanner,
                     &(localDW->TrajectoryPlanner_InstanceData.rtb),
                     &(localDW->TrajectoryPlanner_InstanceData.rtdw),
                     &(localDW->TrajectoryPlanner_InstanceData.rtzce));

  // ModelReference generated from: '<Root>/Position velocity cascade'
  position_velocity_cascade(rty_EstimatedData, &rtb_TrajectoryPlanner,
    &rtu_JointData->position, rty_ActuatorsConfiguration,
    &localB->RateTransition1.motorsensors.qencoder.rotor_angle,
    &rty_Flags->enable_thermal_protection, &rty_Flags->control_mode,
    &rty_FOCSlowInputs->control_outer_outputs,
    &(localDW->Positionvelocitycascade_InstanceData.rtb),
    &(localDW->Positionvelocitycascade_InstanceData.rtdw),
    &(localDW->Positionvelocitycascade_InstanceData.rtzce));

  // BusCreator: '<Root>/Bus Creator'
  rty_FOCSlowInputs->global_configuration =
    localB->BusConversion_InsertedFor_BusCreator_at_inport_0_BusCreator1;
  rty_FOCSlowInputs->actuator_configuration = *rty_ActuatorsConfiguration;
  rty_FOCSlowInputs->estimated_data = *rty_EstimatedData;
  rty_FOCSlowInputs->targets = localB->targets;
}

// Termination for referenced model: 'motion_controller'
void motion_controller_Term(DW_motion_controller_f_T *localDW)
{
  // Terminate for ModelReference: '<S1>/Current Filter'
  filter_current_Term(&(localDW->CurrentFilter_InstanceData.rtdw));
}

// Model initialize function
void motion_controller_initialize(DW_motion_controller_f_T *localDW)
{
  // Model Initialize function for ModelReference Block: '<Root>/Position velocity cascade' 
  position_velocity_cascade_initialize
    (&(localDW->Positionvelocitycascade_InstanceData.rtzce));

  // Model Initialize function for ModelReference Block: '<Root>/Trajectory Planner' 
  trajectory_planner_initialize(&(localDW->TrajectoryPlanner_InstanceData.rtzce));
}

//
// File trailer for generated code.
//
// [EOF]
//
