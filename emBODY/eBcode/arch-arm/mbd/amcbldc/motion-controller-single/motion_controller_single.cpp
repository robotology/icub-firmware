//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: motion_controller_single.cpp
//
// Code generated for Simulink model 'motion_controller_single'.
//
// Model version                  : 4.16
// Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
// C/C++ source code generated on : Mon Aug 11 10:31:52 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "motion_controller_single.h"
#include "motion_controller_single_types.h"
#include "rtw_mutex.h"
#include "rtwtypes.h"
#include "motion_controller.h"

const JointData motion_controller_single_rtZJointData = {
  0.0F                                 // position
};                                     // JointData ground

// System initialize for referenced model: 'motion_controller_single'
void motion_controller_single_Init(Flags *rty_Flags, ActuatorConfiguration
  *rty_ConfigurationParameters, FOCSlowInputs *rty_FOCSlowInputs,
  DW_motion_controller_single_f_T *localDW)
{
  // Start for RateTransition generated from: '<Root>/Motion Controller'
  rtw_mutex_init();

  // SystemInitialize for ModelReference: '<Root>/Motion Controller'
  motion_controller_Init(rty_Flags, rty_ConfigurationParameters,
    rty_FOCSlowInputs, &(localDW->MotionController_InstanceData.rtb),
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
}

// Output and update for referenced model: 'motion_controller_single'
void motion_controller_singleTID1(const SensorsData *rtu_SensorData,
  DW_motion_controller_single_f_T *localDW)
{
  int8_T wrBufIdx;

  // RateTransition generated from: '<Root>/Motion Controller'
  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (localDW->TmpRTBAtMotionControllerInport1_LstBufWR + 1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx == localDW->TmpRTBAtMotionControllerInport1_RDBuf) {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  switch (wrBufIdx) {
   case 0:
    localDW->TmpRTBAtMotionControllerInport1_Buf0 = *rtu_SensorData;
    break;

   case 1:
    localDW->TmpRTBAtMotionControllerInport1_Buf1 = *rtu_SensorData;
    break;

   case 2:
    localDW->TmpRTBAtMotionControllerInport1_Buf2 = *rtu_SensorData;
    break;
  }

  localDW->TmpRTBAtMotionControllerInport1_LstBufWR = wrBufIdx;

  // End of RateTransition generated from: '<Root>/Motion Controller'
}

// Output and update for referenced model: 'motion_controller_single'
void mc_1ms_tick(const ExternalFlags *rtu_ExternalFlags, const ReceivedEvents
                 rtu_messages_rx[4], const FOCOutputs *rtu_FOCOutputs,
                 EstimatedData *rty_Estimates, Flags *rty_Flags,
                 ActuatorConfiguration *rty_ConfigurationParameters,
                 FOCSlowInputs *rty_FOCSlowInputs,
                 DW_motion_controller_single_f_T *localDW)
{
  SensorsData rtb_TmpRTBAtMotionControllerInport1;

  // RateTransition generated from: '<Root>/Motion Controller'
  rtw_mutex_lock();
  localDW->TmpRTBAtMotionControllerInport1_RDBuf =
    localDW->TmpRTBAtMotionControllerInport1_LstBufWR;
  rtw_mutex_unlock();
  switch (localDW->TmpRTBAtMotionControllerInport1_RDBuf) {
   case 0:
    rtb_TmpRTBAtMotionControllerInport1 =
      localDW->TmpRTBAtMotionControllerInport1_Buf0;
    break;

   case 1:
    rtb_TmpRTBAtMotionControllerInport1 =
      localDW->TmpRTBAtMotionControllerInport1_Buf1;
    break;

   case 2:
    rtb_TmpRTBAtMotionControllerInport1 =
      localDW->TmpRTBAtMotionControllerInport1_Buf2;
    break;
  }

  // End of RateTransition generated from: '<Root>/Motion Controller'

  // ModelReference: '<Root>/Motion Controller' incorporates:
  //   Constant: '<Root>/Constant'

  mc_step_1ms(&rtb_TmpRTBAtMotionControllerInport1, rtu_ExternalFlags,
              &rtu_messages_rx[0], &AmcbldcInitConf,
              &motion_controller_single_rtZJointData, rtu_FOCOutputs,
              rty_Estimates, rty_Flags, rty_ConfigurationParameters,
              rty_FOCSlowInputs, &(localDW->MotionController_InstanceData.rtb),
              &(localDW->MotionController_InstanceData.rtdw));
}

// Termination for referenced model: 'motion_controller_single'
void mc_terminate(DW_motion_controller_single_f_T *localDW)
{
  // Terminate for RateTransition generated from: '<Root>/Motion Controller'
  rtw_mutex_destroy();

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
