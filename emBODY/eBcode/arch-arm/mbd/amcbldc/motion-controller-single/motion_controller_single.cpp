//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: motion_controller_single.cpp
//
// Code generated for Simulink model 'motion_controller_single'.
//
// Model version                  : 2.0
// Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
// C/C++ source code generated on : Wed Aug 28 12:32:35 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "motion_controller_single.h"
#include "motion_controller_single_types.h"
#include "rtw_defines.h"
#include "motion_controller_single_private.h"
#include "motion_controller.h"

// System initialize for referenced model: 'motion_controller_single'
void motion_controller_single_Init(Flags *rty_Flags, ActuatorConfiguration
  *rty_ConfigurationParameters, DW_motion_controller_single_f_T *localDW)
{
  // SystemInitialize for ModelReference: '<Root>/Motion Controller'
  motion_controller_Init(rty_Flags, rty_ConfigurationParameters,
    &(localDW->MotionController_InstanceData.rtb),
    &(localDW->MotionController_InstanceData.rtdw));
}

// Enable for referenced model: 'motion_controller_single'
void motion_controller_single_Enable(DW_motion_controller_single_f_T *localDW)
{
  // Enable for ModelReference: '<Root>/Motion Controller'
  motion_controller_Enable(&(localDW->MotionController_InstanceData.rtdw));
}

// Output and update for referenced model: 'motion_controller_single'
void motion_controller_singleTID0(void)
{
  // ModelReference: '<Root>/Motion Controller'
  motion_controllerTID0();
}

// Output and update for referenced model: 'motion_controller_single'
void motion_controller_singleTID1(const SensorsData *rtu_SensorData, FOCOutputs *
  rty_FOCOutputs, DW_motion_controller_single_f_T *localDW)
{
  // ModelReference: '<Root>/Motion Controller'
  motion_controllerTID1(rtu_SensorData, rty_FOCOutputs,
                        &(localDW->MotionController_InstanceData.rtb),
                        &(localDW->MotionController_InstanceData.rtdw));
}

// Output and update for referenced model: 'motion_controller_single'
void mc_1ms_tick(const ExternalFlags *rtu_ExternalFlags, const ReceivedEvents
                 rtu_messages_rx[MAX_EVENTS_PER_TICK], EstimatedData
                 *rty_Estimates, Flags *rty_Flags, ActuatorConfiguration
                 *rty_ConfigurationParameters, B_motion_controller_single_c_T
                 *localB, DW_motion_controller_single_f_T *localDW)
{
  // BusCreator: '<Root>/Bus Creator1' incorporates:
  //   Constant: '<Root>/current_rms_lambda'
  //   Constant: '<Root>/environment_temperature'
  //   Constant: '<Root>/velocity_est_mode'

  localB->BusCreator.estimation.environment_temperature = 25.0F;
  localB->BusCreator.estimation.current_rms_lambda = 0.99995F;
  localB->BusCreator.estimation.velocity_est_mode =
    EstimationVelocityModes_MovingAverage;

  // Constant: '<Root>/Constant'
  localB->Constant = AmcbldcInitConf;

  // ModelReference: '<Root>/Motion Controller'
  mc_step_1ms(rtu_ExternalFlags, &localB->BusCreator, &rtu_messages_rx[0],
              &localB->Constant, rty_Estimates, rty_Flags,
              rty_ConfigurationParameters,
              &(localDW->MotionController_InstanceData.rtb),
              &(localDW->MotionController_InstanceData.rtdw));
}

// Termination for referenced model: 'motion_controller_single'
void mc_terminate(DW_motion_controller_single_f_T *localDW)
{
  // Terminate for ModelReference: '<Root>/Motion Controller'
  motion_controller_Term(&(localDW->MotionController_InstanceData.rtdw));
}

// Model initialize function
void mc_initialize(const char_T **rt_errorStatus,
                   RT_MODEL_motion_controller_single_T *const
                   motion_controller_single_M, DW_motion_controller_single_f_T
                   *localDW)
{
  // Registration code

  // initialize error status
  rtmSetErrorStatusPointer(motion_controller_single_M, rt_errorStatus);

  // Model Initialize function for ModelReference Block: '<Root>/Motion Controller' 
  motion_controller_initialize(&(localDW->MotionController_InstanceData.rtdw));
}

//
// File trailer for generated code.
//
// [EOF]
//
