//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: motion_controller_single.h
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
#ifndef motion_controller_single_h_
#define motion_controller_single_h_
#include "rtwtypes.h"
#include "motion_controller_single_types.h"
#include "motion_controller.h"
#include "rtw_defines.h"
#include "zero_crossing_types.h"

// Block signals for model 'motion_controller_single'
struct B_motion_controller_single_c_T {
  ActuatorConfiguration Constant;      // '<Root>/Constant'
  GlobalConfiguration BusCreator;      // '<Root>/Bus Creator'
};

// Block states (default storage) for model 'motion_controller_single'
struct DW_motion_controller_single_f_T {
  MdlrefDW_motion_controller_T MotionController_InstanceData;// '<Root>/Motion Controller' 
};

// Real-time Model Data Structure
struct tag_RTM_motion_controller_single_T {
  const char_T **errorStatus;
};

struct MdlrefDW_motion_controller_single_T {
  B_motion_controller_single_c_T rtb;
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
  ActuatorConfiguration *rty_ConfigurationParameters,
  DW_motion_controller_single_f_T *localDW);
extern void motion_controller_single_Enable(DW_motion_controller_single_f_T
  *localDW);
extern void motion_controller_singleTID0(void);
extern void motion_controller_singleTID1(const SensorsData *rtu_SensorData,
  FOCOutputs *rty_FOCOutputs, DW_motion_controller_single_f_T *localDW);
extern void mc_1ms_tick(const ExternalFlags *rtu_ExternalFlags, const
  ReceivedEvents rtu_messages_rx[MAX_EVENTS_PER_TICK], EstimatedData
  *rty_Estimates, Flags *rty_Flags, ActuatorConfiguration
  *rty_ConfigurationParameters, B_motion_controller_single_c_T *localB,
  DW_motion_controller_single_f_T *localDW);
extern void mc_terminate(DW_motion_controller_single_f_T *localDW);

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
