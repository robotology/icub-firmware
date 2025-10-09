//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: motion_controller_single.cpp
//
// Code generated for Simulink model 'motion_controller_single'.
//
// Model version                  : 5.0
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Thu Oct  9 17:31:44 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "motion_controller_single.h"
#include "motion_controller_single_types.h"
#include "motion_controller.h"

const JointData motion_controller_single_rtZJointData = {
  0.0F                                 // position
};                                     // JointData ground

// System initialize for referenced model: 'motion_controller_single'
void motion_controller_single_Init(Flags *rty_Flags, ActuatorConfiguration
  *rty_ConfigurationParameters, FOCSlowInputs *rty_FOCSlowInputs, SensorsData
  *rty_SensorDataCalibration, DW_motion_controller_single_f_T *localDW)
{
  // SystemInitialize for ModelReference: '<Root>/Motion Controller'
  motion_controller_Init(rty_Flags, rty_ConfigurationParameters,
    rty_FOCSlowInputs, rty_SensorDataCalibration,
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
void mc_1ms_tick(const SensorsData *rtu_SensorData, const ExternalFlags
                 *rtu_ExternalFlags, const ReceivedEvents rtu_messages_rx[4],
                 const FOCOutputs *rtu_FOCOutputs, EstimatedData *rty_Estimates,
                 Flags *rty_Flags, ActuatorConfiguration
                 *rty_ConfigurationParameters, FOCSlowInputs *rty_FOCSlowInputs,
                 SensorsData *rty_SensorDataCalibration,
                 DW_motion_controller_single_f_T *localDW)
{
  // ModelReference: '<Root>/Motion Controller' incorporates:
  //   Constant: '<Root>/Constant'

  mc_step_1ms(rtu_SensorData, rtu_ExternalFlags, &rtu_messages_rx[0],
              &AmcbldcInitConf, &motion_controller_single_rtZJointData,
              rtu_FOCOutputs, rty_Estimates, rty_Flags,
              rty_ConfigurationParameters, rty_FOCSlowInputs,
              rty_SensorDataCalibration,
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
  motion_controller_single_M->setErrorStatusPointer(rt_errorStatus);

  // Model Initialize function for ModelReference Block: '<Root>/Motion Controller' 
  motion_controller_initialize(&(localDW->MotionController_InstanceData.rtdw));
}

const char_T* RT_MODEL_motion_controller_single_T::getErrorStatus() const
{
  return (*(errorStatus));
}

void RT_MODEL_motion_controller_single_T::setErrorStatus(const char_T* const
  aErrorStatus) const
{
  (*(errorStatus) = aErrorStatus);
}

const char_T** RT_MODEL_motion_controller_single_T::getErrorStatusPointer()
  const
{
  return errorStatus;
}

void RT_MODEL_motion_controller_single_T::setErrorStatusPointer(const char_T
  ** aErrorStatusPointer)
{
  (errorStatus = aErrorStatusPointer);
}

//
// File trailer for generated code.
//
// [EOF]
//
