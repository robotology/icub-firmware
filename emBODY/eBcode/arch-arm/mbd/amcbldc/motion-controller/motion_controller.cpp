//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: motion_controller.cpp
//
// Code generated for Simulink model 'motion_controller'.
//
// Model version                  : 3.9
// Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
// C/C++ source code generated on : Mon Aug 26 10:36:44 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "motion_controller.h"
#include "motion_controller_types.h"
#include "rtw_mutex.h"
#include "rtwtypes.h"
#include "rtw_defines.h"
#include "control_foc.h"
#include "estimation_velocity.h"
#include "filter_current.h"
#include "supervisor.h"
#include "control_outer.h"

// System initialize for referenced model: 'motion_controller'
void motion_controller_Init(Flags *rty_Flags, ActuatorConfiguration
  *rty_ActuatorsConfiguration, B_motion_controller_c_T *localB,
  DW_motion_controller_f_T *localDW)
{
  // Start for RateTransition: '<Root>/Rate Transition'
  rtw_mutex_init();

  // Start for RateTransition: '<Root>/Rate Transition2'
  rtw_mutex_init();

  // Start for RateTransition: '<Root>/Transition to 1ms'
  rtw_mutex_init();

  // SystemInitialize for ModelReference generated from: '<Root>/Control outer'
  control_outer_Init(&(localDW->Controlouter_InstanceData.rtdw));

  // SystemInitialize for ModelReference: '<S1>/Current Filter'
  filter_current_Init(&(localDW->CurrentFilter_InstanceData.rtdw));

  // SystemInitialize for ModelReference: '<S1>/Velocity Estimator'
  estimation_velocity_Init(&(localDW->VelocityEstimator_InstanceData.rtdw));

  // SystemInitialize for ModelReference: '<Root>/FOC'
  control_foc_Init(&(localDW->FOC_InstanceData.rtdw));

  // SystemInitialize for ModelReference: '<Root>/Motor Supervisor'
  supervisor_Init(&localB->targets, rty_ActuatorsConfiguration, rty_Flags,
                  &(localDW->MotorSupervisor_InstanceData.rtdw));
}

// Enable for referenced model: 'motion_controller'
void motion_controller_Enable(DW_motion_controller_f_T *localDW)
{
  // Enable for ModelReference generated from: '<Root>/Control outer'
  control_outer_Enable(&(localDW->Controlouter_InstanceData.rtdw));
}

// Disable for referenced model: 'motion_controller'
void motion_controller_Disable(DW_motion_controller_f_T *localDW)
{
  // Disable for ModelReference generated from: '<Root>/Control outer'
  control_outer_Disable(&(localDW->Controlouter_InstanceData.rtb),
                        &(localDW->Controlouter_InstanceData.rtdw));
}

// Output and update for referenced model: 'motion_controller'
void motion_controllerTID0(void)
{
}

// Output and update for referenced model: 'motion_controller'
void motion_controllerTID1(const SensorsData *rtu_SensorData, FOCOutputs
  *rty_FOCOutputs, B_motion_controller_c_T *localB, DW_motion_controller_f_T
  *localDW)
{
  int8_T wrBufIdx;

  // RateTransition: '<Root>/Rate Transition'
  rtw_mutex_lock();
  localDW->RateTransition_RDBuf = localDW->RateTransition_LstBufWR;
  rtw_mutex_unlock();

  // RateTransition: '<Root>/Rate Transition'
  localB->RateTransition = localDW->RateTransition_Buf
    [localDW->RateTransition_RDBuf];

  // ModelReference: '<Root>/FOC'
  control_foc(rtu_SensorData, &localB->RateTransition, rty_FOCOutputs,
              &(localDW->FOC_InstanceData.rtb), &(localDW->FOC_InstanceData.rtdw),
              &(localDW->FOC_InstanceData.rtzce));

  // RateTransition: '<Root>/Rate Transition2'
  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>(localDW->RateTransition2_LstBufWR + 1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx == localDW->RateTransition2_RDBuf) {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  switch (wrBufIdx) {
   case 0:
    localDW->RateTransition2_Buf0 = *rty_FOCOutputs;
    break;

   case 1:
    localDW->RateTransition2_Buf1 = *rty_FOCOutputs;
    break;

   case 2:
    localDW->RateTransition2_Buf2 = *rty_FOCOutputs;
    break;
  }

  localDW->RateTransition2_LstBufWR = wrBufIdx;

  // End of RateTransition: '<Root>/Rate Transition2'

  // RateTransition: '<Root>/Transition to 1ms'
  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>(localDW->Transitionto1ms_LstBufWR + 1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx == localDW->Transitionto1ms_RDBuf) {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  switch (wrBufIdx) {
   case 0:
    localDW->Transitionto1ms_Buf0 = *rtu_SensorData;
    break;

   case 1:
    localDW->Transitionto1ms_Buf1 = *rtu_SensorData;
    break;

   case 2:
    localDW->Transitionto1ms_Buf2 = *rtu_SensorData;
    break;
  }

  localDW->Transitionto1ms_LstBufWR = wrBufIdx;

  // End of RateTransition: '<Root>/Transition to 1ms'
}

// Output and update for referenced model: 'motion_controller'
void mc_step_1ms(const ExternalFlags *rtu_ExternalFlags, const
                 GlobalConfiguration *rtu_globalConfig, const ReceivedEvents
                 rtu_Events[MAX_EVENTS_PER_TICK], const ActuatorConfiguration
                 *rtu_InitConf, EstimatedData *rty_EstimatedData, Flags
                 *rty_Flags, ActuatorConfiguration *rty_ActuatorsConfiguration,
                 B_motion_controller_c_T *localB, DW_motion_controller_f_T
                 *localDW)
{
  ActuatorConfiguration rtb_BusCreator_actuator_configuration;
  ControlOuterOutputs rtb_Controlouter;
  int8_T wrBufIdx;

  // RateTransition: '<Root>/Transition to 1ms'
  rtw_mutex_lock();
  localDW->Transitionto1ms_RDBuf = localDW->Transitionto1ms_LstBufWR;
  rtw_mutex_unlock();
  switch (localDW->Transitionto1ms_RDBuf) {
   case 0:
    // RateTransition: '<Root>/Transition to 1ms'
    localB->Transitionto1ms = localDW->Transitionto1ms_Buf0;
    break;

   case 1:
    // RateTransition: '<Root>/Transition to 1ms'
    localB->Transitionto1ms = localDW->Transitionto1ms_Buf1;
    break;

   case 2:
    // RateTransition: '<Root>/Transition to 1ms'
    localB->Transitionto1ms = localDW->Transitionto1ms_Buf2;
    break;
  }

  // End of RateTransition: '<Root>/Transition to 1ms'

  // ModelReference: '<S1>/Velocity Estimator'
  estimation_velocity(&rtu_globalConfig->estimation.velocity_est_mode,
                      &localB->Transitionto1ms.position,
                      &rty_EstimatedData->velocity,
                      &(localDW->VelocityEstimator_InstanceData.rtdw));

  // RateTransition: '<Root>/Rate Transition2'
  rtw_mutex_lock();
  localDW->RateTransition2_RDBuf = localDW->RateTransition2_LstBufWR;
  rtw_mutex_unlock();
  switch (localDW->RateTransition2_RDBuf) {
   case 0:
    // RateTransition: '<Root>/Rate Transition2'
    localB->RateTransition2 = localDW->RateTransition2_Buf0;
    break;

   case 1:
    // RateTransition: '<Root>/Rate Transition2'
    localB->RateTransition2 = localDW->RateTransition2_Buf1;
    break;

   case 2:
    // RateTransition: '<Root>/Rate Transition2'
    localB->RateTransition2 = localDW->RateTransition2_Buf2;
    break;
  }

  // End of RateTransition: '<Root>/Rate Transition2'

  // ModelReference: '<S1>/Current Filter'
  filter_current(&localB->RateTransition2, &rty_EstimatedData->Iq_filtered,
                 &(localDW->CurrentFilter_InstanceData.rtdw));

  // BusCreator generated from: '<S1>/Estimation_BusCreator'
  rty_EstimatedData->motor_temperature = 0.0F;

  // ModelReference: '<Root>/Motor Supervisor'
  supervisor(rtu_ExternalFlags, rty_EstimatedData, &localB->RateTransition2,
             &localB->Transitionto1ms, &rtu_Events[0], rtu_InitConf,
             &localB->targets, rty_ActuatorsConfiguration, rty_Flags,
             &(localDW->MotorSupervisor_InstanceData.rtdw));

  // ModelReference generated from: '<Root>/Control outer'
  control_outer(rty_Flags, rty_ActuatorsConfiguration, rty_EstimatedData,
                &localB->Transitionto1ms, &localB->targets, &rtb_Controlouter,
                &(localDW->Controlouter_InstanceData.rtb),
                &(localDW->Controlouter_InstanceData.rtdw),
                &(localDW->Controlouter_InstanceData.rtzce));

  // BusCreator: '<Root>/Bus Creator'
  rtb_BusCreator_actuator_configuration = *rty_ActuatorsConfiguration;

  // RateTransition: '<Root>/Rate Transition' incorporates:
  //   BusCreator: '<Root>/Bus Creator'

  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>(localDW->RateTransition_LstBufWR + 1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx == localDW->RateTransition_RDBuf) {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  localDW->RateTransition_Buf[wrBufIdx].global_configuration = *rtu_globalConfig;
  localDW->RateTransition_Buf[wrBufIdx].actuator_configuration =
    rtb_BusCreator_actuator_configuration;
  localDW->RateTransition_Buf[wrBufIdx].estimated_data = *rty_EstimatedData;
  localDW->RateTransition_Buf[wrBufIdx].targets = localB->targets;
  localDW->RateTransition_Buf[wrBufIdx].control_outer_outputs = rtb_Controlouter;
  localDW->RateTransition_LstBufWR = wrBufIdx;

  // End of RateTransition: '<Root>/Rate Transition'
}

// Termination for referenced model: 'motion_controller'
void motion_controller_Term(DW_motion_controller_f_T *localDW)
{
  // Terminate for RateTransition: '<Root>/Rate Transition'
  rtw_mutex_destroy();

  // Terminate for RateTransition: '<Root>/Rate Transition2'
  rtw_mutex_destroy();

  // Terminate for RateTransition: '<Root>/Transition to 1ms'
  rtw_mutex_destroy();

  // Terminate for ModelReference: '<S1>/Current Filter'
  filter_current_Term(&(localDW->CurrentFilter_InstanceData.rtdw));
}

// Model initialize function
void motion_controller_initialize(DW_motion_controller_f_T *localDW)
{
  // Model Initialize function for ModelReference Block: '<Root>/FOC'
  control_foc_initialize(&(localDW->FOC_InstanceData.rtzce));

  // Model Initialize function for ModelReference Block: '<Root>/Control outer'
  control_outer_initialize(&(localDW->Controlouter_InstanceData.rtzce));
}

//
// File trailer for generated code.
//
// [EOF]
//
