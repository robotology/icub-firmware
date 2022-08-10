//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: AMC_BLDC.h
//
// Code generated for Simulink model 'AMC_BLDC'.
//
// Model version                  : 4.103
// Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
// C/C++ source code generated on : Tue Aug  9 15:19:58 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_AMC_BLDC_h_
#define RTW_HEADER_AMC_BLDC_h_
#include "rtwtypes.h"
#include "AMC_BLDC_types.h"
#include <stddef.h>
#include "zero_crossing_types.h"

// Macros for accessing real-time model data structure
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
  BUS_MESSAGES_RX_MULTIPLE CAN_Decoder_o1;// '<S6>/CAN_Decoder'
  ConfigurationParameters ZOHBlockInsertedForAdapter_Inse;// '<Root>/Adapter4'
  BUS_MESSAGES_TX MessagesTx;          // '<S7>/SupervisorFSM_TX'
  SensorsData RTBInsertedForAdapter_InsertedF;// '<Root>/Adapter3'
  BUS_STATUS_RX_MULTIPLE CAN_Decoder_o2;// '<S6>/CAN_Decoder'
  BUS_CAN_RX_ERRORS_MULTIPLE CAN_Decoder_o3;// '<S6>/CAN_Decoder'
  ControlOutputs RTBInsertedForAdapter_Inserte_a;// '<Root>/Adapter1'
};

// Block states (default storage) for system '<Root>'
struct DW_AMC_BLDC_T {
  ConfigurationParameters ZOHBlockInsertedForAdapter_Inse;// synthesized block
  ConfigurationParameters RTBInsertedForAdapter_InsertedF[3];// synthesized block 
  SensorsData RTBInsertedForAdapter_Inserte_e;// synthesized block
  SensorsData RTBInsertedForAdapter_Inserte_c;// synthesized block
  SensorsData RTBInsertedForAdapter_Inserte_j;// synthesized block
  Targets RTBInsertedForAdapter_Inserte_m[3];// synthesized block
  ControlOuterOutputs RTBInsertedForAdapter_Inserte_i[3];// synthesized block
  ControlOutputs RTBInsertedForAdapter_Inserte_d;// synthesized block
  ControlOutputs RTBInsertedForAdapter_Insert_j2;// synthesized block
  ControlOutputs RTBInsertedForAdapter_Inserte_o;// synthesized block
  EstimatedData RTBInsertedForAdapter_Inserte_k[3];// synthesized block
  Flags RTBInsertedForAdapter_Inserte_l[3];// synthesized block
  void* RTBInsertedForAdapter_Insert_mf;// synthesized block
  void* RTBInsertedForAdapter_Inserte_b;// synthesized block
  void* RTBInsertedForAdapter_Inserte_f;// synthesized block
  void* RTBInsertedForAdapter_Insert_ey;// synthesized block
  void* RTBInsertedForAdapter_Insert_ci;// synthesized block
  void* RTBInsertedForAdapter_Inserte_h;// synthesized block
  void* RTBInsertedForAdapter_Insert_bz;// synthesized block
  int8_T RTBInsertedForAdapter_Insert_hj;// synthesized block
  int8_T RTBInsertedForAdapter_Inserte_p;// synthesized block
  int8_T RTBInsertedForAdapter_Insert_mp;// synthesized block
  int8_T RTBInsertedForAdapter_Insert_m3;// synthesized block
  int8_T RTBInsertedForAdapter_Insert_b2;// synthesized block
  int8_T RTBInsertedForAdapter_Insert_ko;// synthesized block
  int8_T RTBInsertedForAdapter_Insert_jj;// synthesized block
  int8_T RTBInsertedForAdapter_Insert_mb;// synthesized block
  int8_T RTBInsertedForAdapter_Insert_p5;// synthesized block
  int8_T RTBInsertedForAdapter_Insert_bw;// synthesized block
  int8_T RTBInsertedForAdapter_Insert_js;// synthesized block
  int8_T RTBInsertedForAdapter_Inserte_a;// synthesized block
  int8_T RTBInsertedForAdapter_Inserte_g;// synthesized block
  int8_T RTBInsertedForAdapter_Insert_pa;// synthesized block
};

// External inputs (root inport signals with default storage)
struct ExtU_AMC_BLDC_T {
  SensorsData SensorsData_p;           // '<Root>/SensorsData'
  ExternalFlags ExternalFlags_p;       // '<Root>/ExternalFlags'
  BUS_CAN_MULTIPLE PacketsRx;          // '<Root>/PacketsRx'
};

// External outputs (root outports fed by signals with default storage)
struct ExtY_AMC_BLDC_T {
  ControlOutputs ControlOutputs_p;     // '<Root>/ControlOutputs'
  ConfigurationParameters ConfigurationParameters_p;// '<Root>/ConfigurationParameters' 
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
    } TaskCounters;
  } Timing;
};

// Block signals (default storage)
#ifdef __cplusplus

extern "C" {

#endif

  extern struct B_AMC_BLDC_T AMC_BLDC_B;

#ifdef __cplusplus

}
#endif

// Block states (default storage)
extern struct DW_AMC_BLDC_T AMC_BLDC_DW;

#ifdef __cplusplus

extern "C" {

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

extern ConfigurationParameters InitConfParams;// Variable: InitConfParams
                                                 //  Referenced by: '<S7>/SupervisorFSM_RX'

extern real32_T CAN_ANGLE_DEG2ICUB;    // Variable: CAN_ANGLE_DEG2ICUB
                                          //  Referenced by: '<S6>/CAN_Encoder'
                                          //  2^16/360

extern uint8_T CAN_ID_AMC;             // Variable: CAN_ID_AMC
                                          //  Referenced by:
                                          //    '<S6>/CAN_Decoder'
                                          //    '<S6>/CAN_Encoder'
                                          //  4 bits defining the ID of the AMC_BLDC board.


#ifdef __cplusplus

extern "C" {

#endif

  // Model entry point functions
  extern void AMC_BLDC_initialize(void);
  extern void AMC_BLDC_step0(void);
  extern void AMC_BLDC_step_FOC(void);
  extern void AMC_BLDC_step_Time(void);
  extern void AMC_BLDC_terminate(void);

#ifdef __cplusplus

}
#endif

// Real-time Model object
#ifdef __cplusplus

extern "C" {

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
//  '<S1>'   : 'AMC_BLDC/Adapter1'
//  '<S2>'   : 'AMC_BLDC/Adapter2'
//  '<S3>'   : 'AMC_BLDC/Adapter3'
//  '<S4>'   : 'AMC_BLDC/Adapter4'
//  '<S5>'   : 'AMC_BLDC/Estimation'
//  '<S6>'   : 'AMC_BLDC/Messaging'
//  '<S7>'   : 'AMC_BLDC/Supervision'
//  '<S8>'   : 'AMC_BLDC/Estimation/Adapter'
//  '<S9>'   : 'AMC_BLDC/Estimation/Mux'

#endif                                 // RTW_HEADER_AMC_BLDC_h_

//
// File trailer for generated code.
//
// [EOF]
//
