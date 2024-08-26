//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: AMC_BLDC.h
//
// Code generated for Simulink model 'AMC_BLDC'.
//
// Model version                  : 8.8
// Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
// C/C++ source code generated on : Mon Aug 26 15:37:13 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef AMC_BLDC_h_
#define AMC_BLDC_h_
#include "rtwtypes.h"
#include "AMC_BLDC_types.h"
#include "motion_controller_single.h"
#include "can_decoder.h"
#include "SupervisorFSM_TX.h"
#include "can_encoder.h"
#include "rtw_defines.h"
#include "zero_crossing_types.h"

// Macros for accessing real-time model data structure
#ifndef rtmCounterLimit
#define rtmCounterLimit(rtm, idx)      ((rtm)->Timing.TaskCounters.cLimit[(idx)])
#endif

#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetErrorStatusPointer
#define rtmGetErrorStatusPointer(rtm)  ((const char_T **)(&((rtm)->errorStatus)))
#endif

#ifndef rtmStepTask
#define rtmStepTask(rtm, idx)          ((rtm)->Timing.TaskCounters.TID[(idx)] == 0)
#endif

#ifndef rtmTaskCounter
#define rtmTaskCounter(rtm, idx)       ((rtm)->Timing.TaskCounters.TID[(idx)])
#endif

// Block signals (default storage)
struct B_AMC_BLDC_T {
  ActuatorConfiguration
    ZOHBlockInsertedForAdapter_InsertedFor_Adapter_at_outport_0;// '<Root>/Adapter' 
  ReceivedEvents CAN_Decoder[MAX_EVENTS_PER_TICK];// '<S2>/CAN_Decoder'
  BUS_MESSAGES_TX MessagesTx;          // '<Root>/SupervisorFSM_TX'
};

// Block states (default storage) for system '<Root>'
struct DW_AMC_BLDC_T {
  ActuatorConfiguration
    ZOHBlockInsertedForAdapter_InsertedFor_Adapter_at_outport_0_;// synthesized block 
  FOCOutputs TmpRTBAtSupervisorFSM_TXInport4_Buf0;// synthesized block
  FOCOutputs TmpRTBAtSupervisorFSM_TXInport4_Buf1;// synthesized block
  FOCOutputs TmpRTBAtSupervisorFSM_TXInport4_Buf2;// synthesized block
  void* TmpRTBAtSupervisorFSM_TXInport4_SEMAPHORE;// synthesized block
  int8_T TmpRTBAtSupervisorFSM_TXInport4_LstBufWR;// synthesized block
  int8_T TmpRTBAtSupervisorFSM_TXInport4_RDBuf;// synthesized block
  MdlrefDW_motion_controller_single_T MotionControllerSingle_InstanceData;// '<Root>/Motion Controller Single' 
  MdlrefDW_can_decoder_T CAN_Decoder_InstanceData;// '<S2>/CAN_Decoder'
  MdlrefDW_SupervisorFSM_TX_T SupervisorFSM_TX_InstanceData;// '<Root>/SupervisorFSM_TX' 
  MdlrefDW_can_encoder_T CAN_Encoder_InstanceData;// '<S2>/CAN_Encoder'
};

// External inputs (root inport signals with default storage)
struct ExtU_AMC_BLDC_T {
  SensorsData SensorsData_p;           // '<Root>/SensorsData'
  ExternalFlags ExternalFlags_p;       // '<Root>/External  Flags'
  BUS_CAN_MULTIPLE PacketsRx;          // '<Root>/PacketsRx'
};

// External outputs (root outports fed by signals with default storage)
struct ExtY_AMC_BLDC_T {
  FOCOutputs ControlOutputs;           // '<Root>/ControlOutputs'
  ActuatorConfiguration ConfigurationParameters;// '<Root>/ConfigurationParameters' 
  Flags Flags_p;                       // '<Root>/Flags'
  EstimatedData EstimatedData_p;       // '<Root>/EstimatedData'
  BUS_CAN_MULTIPLE PacketsTx;          // '<Root>/PacketsTx'
};

// Real-time Model Data Structure
struct tag_RTM_AMC_BLDC_T {
  const char_T *errorStatus;

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
};

// Block signals (default storage)
#ifdef __cplusplus

extern "C"
{

#endif

  extern struct B_AMC_BLDC_T AMC_BLDC_B;

#ifdef __cplusplus

}

#endif

// Block states (default storage)
extern struct DW_AMC_BLDC_T AMC_BLDC_DW;

#ifdef __cplusplus

extern "C"
{

#endif

  // External inputs (root inport signals with default storage)
  extern struct ExtU_AMC_BLDC_T AMC_BLDC_U;

  // External outputs (root outports fed by signals with default storage)
  extern struct ExtY_AMC_BLDC_T AMC_BLDC_Y;

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

extern ActuatorConfiguration AmcbldcInitConf;// Variable: AmcbldcInitConf
                                                //  Referenced by: '<Root>/Motion Controller Single'

extern real32_T CAN_ANGLE_DEG2ICUB;    // Variable: CAN_ANGLE_DEG2ICUB
                                          //  Referenced by: '<S2>/CAN_Encoder'
                                          //  2^16/360

extern real32_T CAN_ANGLE_ICUB2DEG;    // Variable: CAN_ANGLE_ICUB2DEG
                                          //  Referenced by: '<S2>/CAN_Decoder'
                                          //  360/2^16

extern uint8_T CAN_ID_AMC;             // Variable: CAN_ID_AMC
                                          //  Referenced by:
                                          //    '<S2>/CAN_Decoder'
                                          //    '<S2>/CAN_Encoder'
                                          //  4 bits defining the ID of the AMC_BLDC board.


#ifdef __cplusplus

extern "C"
{

#endif

  // Model entry point functions
  extern void AMC_BLDC_initialize(void);
  extern void AMC_BLDC_step0(void);
  extern void AMC_BLDC_step_FOC(void);
  extern void AMC_BLDC_step_1ms(void);
  extern void AMC_BLDC_terminate(void);

#ifdef __cplusplus

}

#endif

// Real-time Model object
#ifdef __cplusplus

extern "C"
{

#endif

  extern RT_MODEL_AMC_BLDC_T *const AMC_BLDC_M;

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
//  '<Root>' : 'AMC_BLDC'
//  '<S1>'   : 'AMC_BLDC/Adapter'
//  '<S2>'   : 'AMC_BLDC/Messaging'

#endif                                 // AMC_BLDC_h_

//
// File trailer for generated code.
//
// [EOF]
//
