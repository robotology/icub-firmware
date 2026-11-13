//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: iterative_motion_controller.h
//
// Code generated for Simulink model 'iterative_motion_controller'.
//
// Model version                  : 6.1
// Simulink Coder version         : 26.1 (R2026a) 20-Nov-2025
// C/C++ source code generated on : Tue Oct 20 12:03:16 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef ITERATIVE_MOTION_CONTROLLER_H_
#define ITERATIVE_MOTION_CONTROLLER_H_
#include "rtwtypes.h"
#include "iterative_motion_controller_types.h"
#include "SupervisorFSM_TX.h"
#include "control_foc.h"
#include "motion_controller.h"
#include "process_sensors.h"
#include "zero_crossing_types.h"

// Includes for objects with custom storage classes
#include "rtw_defines.h"

// Block signals and states (default storage) for system '<Root>/Iterative Motion Controller' 
struct DW_CoreSubsys_iterative_motion_controller_T {
  MdlrefDW_SupervisorFSM_TX_T Model_InstanceData;// '<S1>/Model'
  MdlrefDW_control_foc_T FOC_InstanceData;// '<S1>/FOC'
  MdlrefDW_motion_controller_T MotionController_InstanceData;// '<S1>/Motion Controller' 
  MdlrefDW_process_sensors_T ProcessSensors_InstanceData;// '<S1>/Process Sensors' 
  FOCSlowInputs RateTransition1_Buf[3];// '<S1>/Rate Transition1'
  ActuatorConfiguration TmpRTBAtProcessSensorsInport1_Buf[3];// synthesized block 
  FOCSlowInputs RateTransition1;       // '<S1>/Rate Transition1'
  ActuatorConfiguration ConfigurationParameters;// '<S1>/Motion Controller'
  SensorsData TmpRTBAtFOCInport4_Buf[3];// synthesized block
  BUS_MESSAGES_TX MessagesTx;          // '<S1>/Model'
  SensorsData SensorsDataCalibration;  // '<S1>/Motion Controller'
  SensorsData Transitionto1ms_Buf0;    // '<S1>/Transition to 1 ms'
  SensorsData Transitionto1ms_Buf1;    // '<S1>/Transition to 1 ms'
  SensorsData Transitionto1ms_Buf2;    // '<S1>/Transition to 1 ms'
  Flags TmpRTBAtProcessSensorsInport2_Buf[3];// synthesized block
  Flags TmpRTBAtFOCInport3_Buf[3];     // synthesized block
  FOCOutputs RateTransition2_Buf0;     // '<S1>/Rate Transition2'
  FOCOutputs RateTransition2_Buf1;     // '<S1>/Rate Transition2'
  FOCOutputs RateTransition2_Buf2;     // '<S1>/Rate Transition2'
  Flags Flags_a;                       // '<S1>/Motion Controller'
  EstimatedData MotionController_o1;   // '<S1>/Motion Controller'
  BUS_STATUS_TX Model_o2;              // '<S1>/Model'
  void* TmpRTBAtProcessSensorsInport1_SEMAPHORE;// synthesized block
  void* TmpRTBAtProcessSensorsInport2_SEMAPHORE;// synthesized block
  void* RateTransition1_SEMAPHORE;     // '<S1>/Rate Transition1'
  void* TmpRTBAtFOCInport3_SEMAPHORE;  // synthesized block
  void* TmpRTBAtFOCInport4_SEMAPHORE;  // synthesized block
  void* RateTransition2_SEMAPHORE;     // '<S1>/Rate Transition2'
  void* Transitionto1ms_SEMAPHORE;     // '<S1>/Transition to 1 ms'
  int8_T TmpRTBAtProcessSensorsInport1_LstBufWR;// synthesized block
  int8_T TmpRTBAtProcessSensorsInport1_RDBuf;// synthesized block
  int8_T TmpRTBAtProcessSensorsInport2_LstBufWR;// synthesized block
  int8_T TmpRTBAtProcessSensorsInport2_RDBuf;// synthesized block
  int8_T RateTransition1_LstBufWR;     // '<S1>/Rate Transition1'
  int8_T RateTransition1_RDBuf;        // '<S1>/Rate Transition1'
  int8_T TmpRTBAtFOCInport3_LstBufWR;  // synthesized block
  int8_T TmpRTBAtFOCInport3_RDBuf;     // synthesized block
  int8_T TmpRTBAtFOCInport4_LstBufWR;  // synthesized block
  int8_T TmpRTBAtFOCInport4_RDBuf;     // synthesized block
  int8_T RateTransition2_LstBufWR;     // '<S1>/Rate Transition2'
  int8_T RateTransition2_RDBuf;        // '<S1>/Rate Transition2'
  int8_T Transitionto1ms_LstBufWR;     // '<S1>/Transition to 1 ms'
  int8_T Transitionto1ms_RDBuf;        // '<S1>/Transition to 1 ms'
};

// Block signals and states (default storage) for system '<Root>'
struct DW_iterative_motion_controller_T {
  DW_CoreSubsys_iterative_motion_controller_T CoreSubsys[1];// '<Root>/Iterative Motion Controller' 
};

// External inputs (root inport signals with default storage)
struct ExtU_iterative_motion_controller_T {
  SensorsData SensorData;              // '<Root>/SensorData'
  ExternalFlags ExternalFlags_j;       // '<Root>/ExternalFlags'
  ReceivedEvents EventsList[4];        // '<Root>/EventsList'
};

// External outputs (root outports fed by signals with default storage)
struct ExtY_iterative_motion_controller_T {
  FOCOutputs FOCOutputs_h;             // '<Root>/FOCOutputs'
  EstimatedData Estimates;             // '<Root>/Estimates'
  Flags Flags_d;                       // '<Root>/Flags'
  ActuatorConfiguration ConfigurationParameters;// '<Root>/ConfigurationParameters' 
  BUS_MESSAGES_TX Messages;            // '<Root>/Messages'
  BUS_STATUS_TX MessagesFlags;         // '<Root>/MessagesFlags'
};

// Real-time Model Data Structure
struct tag_RTM_iterative_motion_controller_T {
  //
  //  Timing:
  //  The following substructure contains information regarding
  //  the timing information for the model.

  struct {
    struct {
      uint32_T TID[3];
      uint32_T cLimit[3];
    } TaskCounters;
  } Timing;

  boolean_T StepTask(int32_T idx) const;
  uint32_T &CounterLimit(int32_T idx);
  uint32_T &TaskCounter(int32_T idx);
};

// Block signals and states (default storage)
#ifdef __cplusplus

extern "C"
{

#endif

  extern struct DW_iterative_motion_controller_T iterative_motion_controller_DW;

#ifdef __cplusplus

}

#endif

#ifdef __cplusplus

extern "C"
{

#endif

  // External inputs (root inport signals with default storage)
  extern struct ExtU_iterative_motion_controller_T iterative_motion_controller_U;

  // External outputs (root outports fed by signals with default storage)
  extern struct ExtY_iterative_motion_controller_T iterative_motion_controller_Y;

#ifdef __cplusplus

}

#endif

// External data declarations for dependent source files
extern const JointData iterative_motion_controller_rtZJointData;// JointData ground 

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern ActuatorConfiguration Amcmj1InitConf;// Variable: Amcmj1InitConf
                                               //  Referenced by: '<Root>/Constant'


#ifdef __cplusplus

extern "C"
{

#endif

  // Model entry point functions
  extern void AMCFOC_initialize(void);
  extern void AMCFOC_step_Time_base(void);// Sample time: [5e-06s, 0.0s]
  extern void AMCFOC_step_FOC(void);   // Sample time: [4.5e-05s, 0.0s]
  extern void AMCFOC_step_Time_1ms(void);// Sample time: [0.001s, 0.0s]
  extern void AMCFOC_terminate(void);

#ifdef __cplusplus

}

#endif

// Real-time Model object
#ifdef __cplusplus

extern "C"
{

#endif

  extern RT_MODEL_iterative_motion_controller_T *const
    iterative_motion_controller_M;

#ifdef __cplusplus

}

#endif

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
//  '<Root>' : 'iterative_motion_controller'
//  '<S1>'   : 'iterative_motion_controller/Iterative Motion Controller'
//  '<S2>'   : 'iterative_motion_controller/Sort Events'

#endif                                 // ITERATIVE_MOTION_CONTROLLER_H_

//
// File trailer for generated code.
//
// [EOF]
//
