//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: iterative_motion_controller.h
//
// Code generated for Simulink model 'iterative_motion_controller'.
//
// Model version                  : 2.54
// Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
// C/C++ source code generated on : Thu Oct 10 13:02:35 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef iterative_motion_controller_h_
#define iterative_motion_controller_h_
#include "rtwtypes.h"
#include "iterative_motion_controller_types.h"
#include "SupervisorFSM_TX.h"
#include "motion_controller.h"
#include "rtw_defines.h"
#include "zero_crossing_types.h"

// Macros for accessing real-time model data structure
#ifndef rtmStepTask
#define rtmStepTask(rtm, idx)          ((rtm)->Timing.TaskCounters.TID[(idx)] == 0)
#endif

#ifndef rtmTaskCounter
#define rtmTaskCounter(rtm, idx)       ((rtm)->Timing.TaskCounters.TID[(idx)])
#endif

// Block signals and states (default storage) for system '<Root>/Iterative Motion Controller' 
struct DW_CoreSubsys_iterative_motion_controller_T {
  MdlrefDW_SupervisorFSM_TX_T Model_InstanceData;// '<S1>/Model'
  MdlrefDW_motion_controller_T MotionController_InstanceData;// '<S1>/Motion Controller' 
  ActuatorConfiguration ConfigurationParameters;// '<S1>/Motion Controller'
  BUS_MESSAGES_TX MessagesTx;          // '<S1>/Model'
  SensorsData TmpRTBAtModelInport1_Buf0;// synthesized block
  SensorsData TmpRTBAtModelInport1_Buf1;// synthesized block
  SensorsData TmpRTBAtModelInport1_Buf2;// synthesized block
  FOCOutputs MotionController_o1;      // '<S1>/Motion Controller'
  FOCOutputs TmpRTBAtModelInport4_Buf0;// synthesized block
  FOCOutputs TmpRTBAtModelInport4_Buf1;// synthesized block
  FOCOutputs TmpRTBAtModelInport4_Buf2;// synthesized block
  Flags Flags_l;                       // '<S1>/Motion Controller'
  EstimatedData MotionController_o2;   // '<S1>/Motion Controller'
  void* TmpRTBAtModelInport1_SEMAPHORE;// synthesized block
  void* TmpRTBAtModelInport4_SEMAPHORE;// synthesized block
  int8_T TmpRTBAtModelInport1_LstBufWR;// synthesized block
  int8_T TmpRTBAtModelInport1_RDBuf;   // synthesized block
  int8_T TmpRTBAtModelInport4_LstBufWR;// synthesized block
  int8_T TmpRTBAtModelInport4_RDBuf;   // synthesized block
};

// Block signals and states (default storage) for system '<Root>'
struct DW_iterative_motion_controller_T {
  DW_CoreSubsys_iterative_motion_controller_T CoreSubsys[N_MOTORS];// '<Root>/Iterative Motion Controller' 
  ReceivedEvents M[(MAX_EVENTS_PER_TICK * N_MOTORS)];
  ReceivedEvents
    rtb_ImpSel_InsertedFor_Events_at_outport_0_m[MAX_EVENTS_PER_TICK];
};

// External inputs (root inport signals with default storage)
struct ExtU_iterative_motion_controller_T {
  SensorsData SensorData[N_MOTORS];    // '<Root>/SensorData'
  ExternalFlags ExternalFlags_j;       // '<Root>/ExternalFlags'
  ReceivedEvents EventsList[(MAX_EVENTS_PER_TICK * N_MOTORS)];// '<Root>/EventsList' 
};

// External outputs (root outports fed by signals with default storage)
struct ExtY_iterative_motion_controller_T {
  FOCOutputs FOCOutputs_h[N_MOTORS];   // '<Root>/FOCOutputs'
  EstimatedData Estimates[N_MOTORS];   // '<Root>/Estimates'
  Flags Flags_d[N_MOTORS];             // '<Root>/Flags'
  ActuatorConfiguration ConfigurationParameters[N_MOTORS];// '<Root>/ConfigurationParameters' 
  BUS_MESSAGES_TX Messages[N_MOTORS];  // '<Root>/Messages'
  BUS_STATUS_TX MessagesFlags[N_MOTORS];// '<Root>/MessagesFlags'
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
    } TaskCounters;
  } Timing;
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

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern ActuatorConfiguration AmcfocInitConf[2];// Variable: AmcfocInitConf
                                                  //  Referenced by: '<Root>/Constant'


#ifdef __cplusplus

extern "C"
{

#endif

  // Model entry point functions
  extern void AMCFOC_initialize(void);
  extern void AMCFOC_step_Time_0(void);
  extern void AMCFOC_step_FOC(void);
  extern void AMCFOC_step_Time_1ms(void);
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

#endif                                 // iterative_motion_controller_h_

//
// File trailer for generated code.
//
// [EOF]
//
