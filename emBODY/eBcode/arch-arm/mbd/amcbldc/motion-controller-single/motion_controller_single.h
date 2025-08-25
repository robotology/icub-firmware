//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: motion_controller_single.h
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
#ifndef motion_controller_single_h_
#define motion_controller_single_h_
#include "rtwtypes.h"
#include "motion_controller_single_types.h"
#include "motion_controller.h"
#include "zero_crossing_types.h"

// Includes for objects with custom storage classes
#include "rtw_defines.h"

// Block states (default storage) for model 'motion_controller_single'
struct DW_motion_controller_single_f_T {
  SensorsData TmpRTBAtMotionControllerInport1_Buf0;// synthesized block
  SensorsData TmpRTBAtMotionControllerInport1_Buf1;// synthesized block
  SensorsData TmpRTBAtMotionControllerInport1_Buf2;// synthesized block
  void* TmpRTBAtMotionControllerInport1_SEMAPHORE;// synthesized block
  int8_T TmpRTBAtMotionControllerInport1_LstBufWR;// synthesized block
  int8_T TmpRTBAtMotionControllerInport1_RDBuf;// synthesized block
  MdlrefDW_motion_controller_T MotionController_InstanceData;// '<Root>/Motion Controller' 
};

// Real-time Model Data Structure
struct tag_RTM_motion_controller_single_T {
  const char_T **errorStatus;
  const char_T* getErrorStatus() const;
  void setErrorStatus(const char_T* const aErrorStatus) const;
  const char_T** getErrorStatusPointer() const;
  void setErrorStatusPointer(const char_T** aErrorStatusPointer);
};

struct MdlrefDW_motion_controller_single_T {
  DW_motion_controller_single_f_T rtdw;
  RT_MODEL_motion_controller_single_T rtm;
};

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern ActuatorConfiguration AmcbldcInitConf;// Variable: AmcbldcInitConf
                                                //  Referenced by: '<Root>/Constant'


// Model reference registration function
extern void mc_initialize(const char_T **rt_errorStatus,
  RT_MODEL_motion_controller_single_T *const motion_controller_single_M,
  DW_motion_controller_single_f_T *localDW);
extern void motion_controller_single_Init(Flags *rty_Flags,
  ActuatorConfiguration *rty_ConfigurationParameters, FOCSlowInputs
  *rty_FOCSlowInputs, DW_motion_controller_single_f_T *localDW);
extern void motion_controller_single_Enable(DW_motion_controller_single_f_T
  *localDW);
extern void motion_controller_singleTID0(void);
extern void motion_controller_singleTID1(const SensorsData *rtu_SensorData,
  DW_motion_controller_single_f_T *localDW);
extern void mc_1ms_tick(const ExternalFlags *rtu_ExternalFlags, const
  ReceivedEvents rtu_messages_rx[4], const FOCOutputs *rtu_FOCOutputs,
  EstimatedData *rty_Estimates, Flags *rty_Flags, ActuatorConfiguration
  *rty_ConfigurationParameters, FOCSlowInputs *rty_FOCSlowInputs,
  DW_motion_controller_single_f_T *localDW);
extern void mc_terminate(DW_motion_controller_single_f_T *localDW);
extern const JointData motion_controller_single_rtZJointData;// JointData ground 

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
//  '<Root>' : 'motion_controller_single'

#endif                                 // motion_controller_single_h_

//
// File trailer for generated code.
//
// [EOF]
//
