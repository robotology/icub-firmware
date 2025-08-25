//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: motion_controller.cpp
//
// Code generated for Simulink model 'motion_controller'.
//
// Model version                  : 5.44
// Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
// C/C++ source code generated on : Mon Aug 11 10:31:42 2025
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
#include "position_velocity_cascade.h"

// System initialize for referenced model: 'motion_controller'
void motion_controller_Init(Flags *rty_Flags, ActuatorConfiguration
  *rty_ActuatorsConfiguration, FOCSlowInputs *rty_FOCSlowInputs,
  B_motion_controller_c_T *localB, DW_motion_controller_f_T *localDW)
{
  // Start for Constant: '<S2>/Velocity Estimation Mode'
  localB->VelocityEstimationMode = EstimationVelocityModes_MovingAverage;

  // Start for Constant: '<S2>/Velocity estimation window'
  localB->Velocityestimationwindow = 64U;

  // SystemInitialize for BusCreator generated from: '<Root>/Bus Creator' incorporates:
  //   Constant: '<S2>/Environment Temperature'
  //   Constant: '<S2>/lambda'

  rty_FOCSlowInputs->global_configuration.estimation.environment_temperature =
    25.0F;
  rty_FOCSlowInputs->global_configuration.estimation.current_rms_lambda = 0.995F;
  rty_FOCSlowInputs->global_configuration.estimation.velocity_est_mode =
    localB->VelocityEstimationMode;
  rty_FOCSlowInputs->global_configuration.estimation.velocity_est_window =
    localB->Velocityestimationwindow;

  // SystemInitialize for ModelReference: '<S1>/Motor Velocity Estimator'
  estimation_velocity_Init(&(localDW->MotorVelocityEstimator_InstanceData.rtdw));

  // SystemInitialize for ModelReference: '<S1>/Current Filter'
  filter_current_Init(&(localDW->CurrentFilter_InstanceData.rtdw));

  // SystemInitialize for ModelReference: '<S1>/Joint Velocity Estimator'
  estimation_velocity_Init(&(localDW->JointVelocityEstimator_InstanceData.rtdw));

  // SystemInitialize for ModelReference generated from: '<Root>/Motor Supervisor' 
  supervisor_Init(&localB->targets, rty_ActuatorsConfiguration, rty_Flags,
                  &(localDW->MotorSupervisor_InstanceData.rtdw));

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
                 B_motion_controller_c_T *localB, DW_motion_controller_f_T
                 *localDW)
{
  // Constant: '<S2>/Velocity Estimation Mode'
  localB->VelocityEstimationMode = EstimationVelocityModes_MovingAverage;

  // Constant: '<S2>/Velocity estimation window'
  localB->Velocityestimationwindow = 64U;

  // BusCreator generated from: '<Root>/Bus Creator' incorporates:
  //   Constant: '<S2>/Environment Temperature'
  //   Constant: '<S2>/lambda'

  rty_FOCSlowInputs->global_configuration.estimation.environment_temperature =
    25.0F;
  rty_FOCSlowInputs->global_configuration.estimation.current_rms_lambda = 0.995F;
  rty_FOCSlowInputs->global_configuration.estimation.velocity_est_mode =
    localB->VelocityEstimationMode;
  rty_FOCSlowInputs->global_configuration.estimation.velocity_est_window =
    localB->Velocityestimationwindow;

  // RateTransition: '<Root>/Rate Transition1'
  localB->RateTransition1 = *rtu_SensorData;

  // ModelReference: '<S1>/Motor Velocity Estimator'
  estimation_velocity(&localB->VelocityEstimationMode,
                      &localB->RateTransition1.motorsensors.qencoder.rotor_angle,
                      &localB->Velocityestimationwindow, &localB->velocity,
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
                      &localB->Velocityestimationwindow, &localB->velocity_j,
                      &(localDW->JointVelocityEstimator_InstanceData.rtdw));

  // BusCreator generated from: '<S1>/Estimation_BusCreator'
  rty_EstimatedData->rotor_velocity = localB->velocity;
  rty_EstimatedData->motor_temperature = 0.0F;
  rty_EstimatedData->joint_velocity = localB->velocity_j;

  // ModelReference generated from: '<Root>/Motor Supervisor'
  supervisor(rtu_ExternalFlags, rty_EstimatedData, &localB->RateTransition,
             &localB->RateTransition1, &rtu_Events[0], rtu_InitConf,
             &localB->targets, rty_ActuatorsConfiguration, rty_Flags,
             &(localDW->MotorSupervisor_InstanceData.rtdw));

  // ModelReference generated from: '<Root>/Position velocity cascade'
  position_velocity_cascade(rty_EstimatedData, &localB->targets,
    &rtu_JointData->position, &rty_ActuatorsConfiguration->thresholds.jntVelMax,
    &rty_ActuatorsConfiguration->thresholds.motorNominalCurrents,
    &rty_ActuatorsConfiguration->thresholds.motorPeakCurrents,
    &rty_ActuatorsConfiguration->pids.currentPID.I,
    &rty_ActuatorsConfiguration->pids.velocityPID.P,
    &rty_ActuatorsConfiguration->pids.velocityPID.I,
    &rty_ActuatorsConfiguration->pids.velocityPID.D,
    &rty_ActuatorsConfiguration->pids.velocityPID.N,
    &rty_ActuatorsConfiguration->pids.positionPID.P,
    &rty_ActuatorsConfiguration->pids.positionPID.I,
    &rty_ActuatorsConfiguration->pids.positionPID.D,
    &rty_ActuatorsConfiguration->pids.positionPID.N,
    &rty_ActuatorsConfiguration->motor.reference_encoder,
    &localB->RateTransition1.motorsensors.qencoder.rotor_angle,
    &rty_Flags->enable_thermal_protection, &rty_Flags->control_mode,
    &rty_FOCSlowInputs->control_outer_outputs,
    &(localDW->Positionvelocitycascade_InstanceData.rtb),
    &(localDW->Positionvelocitycascade_InstanceData.rtdw),
    &(localDW->Positionvelocitycascade_InstanceData.rtzce));

  // BusCreator: '<Root>/Bus Creator'
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
}

//
// File trailer for generated code.
//
// [EOF]
//
