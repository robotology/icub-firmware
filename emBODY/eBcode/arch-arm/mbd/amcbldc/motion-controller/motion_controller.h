//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: motion_controller.h
//
// Code generated for Simulink model 'motion_controller'.
//
// Model version                  : 5.2
// Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
// C/C++ source code generated on : Wed Jun  4 17:59:01 2025
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
#include "control_foc.h"
#include "estimation_velocity.h"
#include "filter_current.h"
#include "supervisor.h"
#include "control_outer.h"
#include "rtw_defines.h"
#include "zero_crossing_types.h"

// Block signals for model 'motion_controller'
struct B_motion_controller_c_T {
  FOCSlowInputs RateTransition;        // '<Root>/Rate Transition'
  SensorsData Transitionto1ms;         // '<Root>/Transition to 1ms'
  FOCOutputs RateTransition2;          // '<Root>/Rate Transition2'
  Targets targets;                     // '<Root>/Motor Supervisor'
  real32_T velocity;                   // '<S1>/Velocity Estimator'
};

// Block states (default storage) for model 'motion_controller'
struct DW_motion_controller_f_T {
  FOCSlowInputs RateTransition_Buf[3]; // '<Root>/Rate Transition'
  SensorsData Transitionto1ms_Buf0;    // '<Root>/Transition to 1ms'
  SensorsData Transitionto1ms_Buf1;    // '<Root>/Transition to 1ms'
  SensorsData Transitionto1ms_Buf2;    // '<Root>/Transition to 1ms'
  FOCOutputs RateTransition2_Buf0;     // '<Root>/Rate Transition2'
  FOCOutputs RateTransition2_Buf1;     // '<Root>/Rate Transition2'
  FOCOutputs RateTransition2_Buf2;     // '<Root>/Rate Transition2'
  void* RateTransition_SEMAPHORE;      // '<Root>/Rate Transition'
  void* RateTransition2_SEMAPHORE;     // '<Root>/Rate Transition2'
  void* Transitionto1ms_SEMAPHORE;     // '<Root>/Transition to 1ms'
  int8_T RateTransition_LstBufWR;      // '<Root>/Rate Transition'
  int8_T RateTransition_RDBuf;         // '<Root>/Rate Transition'
  int8_T RateTransition2_LstBufWR;     // '<Root>/Rate Transition2'
  int8_T RateTransition2_RDBuf;        // '<Root>/Rate Transition2'
  int8_T Transitionto1ms_LstBufWR;     // '<Root>/Transition to 1ms'
  int8_T Transitionto1ms_RDBuf;        // '<Root>/Transition to 1ms'
  MdlrefDW_control_foc_T FOC_InstanceData;// '<Root>/FOC'
  MdlrefDW_estimation_velocity_T VelocityEstimator_InstanceData;// '<S1>/Velocity Estimator' 
  MdlrefDW_filter_current_T CurrentFilter_InstanceData;// '<S1>/Current Filter'
  MdlrefDW_supervisor_T MotorSupervisor_InstanceData;// '<Root>/Motor Supervisor' 
  MdlrefDW_control_outer_T Controlouter_InstanceData;// '<Root>/Control outer'
};

struct MdlrefDW_motion_controller_T {
  B_motion_controller_c_T rtb;
  DW_motion_controller_f_T rtdw;
};

// Model reference registration function
extern void motion_controller_initialize(DW_motion_controller_f_T *localDW);
extern void motion_controller_Init(Flags *rty_Flags, ActuatorConfiguration
  *rty_ActuatorsConfiguration, B_motion_controller_c_T *localB,
  DW_motion_controller_f_T *localDW);
extern void motion_controller_Enable(DW_motion_controller_f_T *localDW);
extern void motion_controller_Disable(DW_motion_controller_f_T *localDW);
extern void motion_controllerTID0(void);
extern void motion_controllerTID1(const SensorsData *rtu_SensorData, FOCOutputs *
  rty_FOCOutputs, B_motion_controller_c_T *localB, DW_motion_controller_f_T
  *localDW);
extern void mc_step_1ms(const ExternalFlags *rtu_ExternalFlags, const
  ReceivedEvents rtu_Events[MAX_EVENTS_PER_TICK], const ActuatorConfiguration
  *rtu_InitConf, EstimatedData *rty_EstimatedData, Flags *rty_Flags,
  ActuatorConfiguration *rty_ActuatorsConfiguration, B_motion_controller_c_T
  *localB, DW_motion_controller_f_T *localDW);
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
