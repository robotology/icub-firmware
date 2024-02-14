//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: AMC_BLDC.h
//
// Code generated for Simulink model 'AMC_BLDC'.
//
// Model version                  : 7.13
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Tue Feb 13 11:55:25 2024
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
#include "control_foc.h"
#include "estimation_velocity.h"
#include "filter_current.h"
#include "control_outer.h"
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
  FOCSlowInputs BusConversion_InsertedFor_FOC_at_inport_1_BusCreator1;
  BUS_MESSAGES_RX_MULTIPLE CAN_Decoder_o1;// '<S6>/CAN_Decoder'
  ConfigurationParameters
    ZOHBlockInsertedForAdapter_InsertedFor_Adapter4_at_outport_0;// '<Root>/Adapter4' 
  BUS_MESSAGES_TX MessagesTx;          // '<S7>/SupervisorFSM_TX'
  SensorsData RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0;// '<Root>/Adapter3' 
  ControlOutputs RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0;// '<Root>/Adapter1' 
  BUS_STATUS_RX_MULTIPLE CAN_Decoder_o2;// '<S6>/CAN_Decoder'
  BUS_CAN_RX_ERRORS_MULTIPLE CAN_Decoder_o3;// '<S6>/CAN_Decoder'
};

// Block states (default storage) for system '<Root>'
struct DW_AMC_BLDC_T {
  ConfigurationParameters
    ZOHBlockInsertedForAdapter_InsertedFor_Adapter4_at_outport_0;// synthesized block 
  ConfigurationParameters
    RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_2_Bu[3];// synthesized block 
  SensorsData RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Buf0;// synthesized block 
  SensorsData RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Buf1;// synthesized block 
  SensorsData RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Buf2;// synthesized block 
  ControlOutputs RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Buf0;// synthesized block 
  ControlOutputs RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Buf1;// synthesized block 
  ControlOutputs RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Buf2;// synthesized block 
  Targets RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_4_Bu[3];// synthesized block 
  EstimatedData RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_3_Bu[3];// synthesized block 
  ControlOuterOutputs
    RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_5_Bu[3];// synthesized block 
  Flags RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_1_Bu[3];// synthesized block 
  MotorTemperature RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Bu_d;// synthesized block 
  MotorTemperature RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Bu_j;// synthesized block 
  MotorTemperature RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Bu_o;// synthesized block 
  void* RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_SEMA;// synthesized block 
  void* RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_1_SE;// synthesized block 
  void* RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_2_SE;// synthesized block 
  void* RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_3_SE;// synthesized block 
  void* RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_4_SE;// synthesized block 
  void* RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_5_SE;// synthesized block 
  void* RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_SE_h;// synthesized block 
  void* RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_SEMA;// synthesized block 
  int8_T RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_LstB;// synthesized block 
  int8_T RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_RDBu;// synthesized block 
  int8_T RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_1_Ls;// synthesized block 
  int8_T RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_1_RD;// synthesized block 
  int8_T RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_2_Ls;// synthesized block 
  int8_T RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_2_RD;// synthesized block 
  int8_T RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_3_Ls;// synthesized block 
  int8_T RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_3_RD;// synthesized block 
  int8_T RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_4_Ls;// synthesized block 
  int8_T RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_4_RD;// synthesized block 
  int8_T RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_5_Ls;// synthesized block 
  int8_T RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_5_RD;// synthesized block 
  int8_T RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Ls_j;// synthesized block 
  int8_T RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_RD_a;// synthesized block 
  int8_T RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_LstB;// synthesized block 
  int8_T RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_RDBu;// synthesized block 
  MdlrefDW_control_foc_T FOC_InstanceData;// '<Root>/FOC'
  MdlrefDW_estimation_velocity_T Estimation_Velocity_InstanceData;// '<S5>/Estimation_Velocity' 
  MdlrefDW_filter_current_T Filter_Current_InstanceData;// '<S5>/Filter_Current' 
  MdlrefDW_control_outer_T OuterControl_InstanceData;// '<Root>/OuterControl'
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

extern ConfigurationParameters InitConfParams;// Variable: InitConfParams
                                                 //  Referenced by: '<S7>/SupervisorFSM_RX'

extern real32_T CAN_ANGLE_DEG2ICUB;    // Variable: CAN_ANGLE_DEG2ICUB
                                          //  Referenced by: '<S6>/CAN_Encoder'
                                          //  2^16/360

extern real32_T CAN_ANGLE_ICUB2DEG;    // Variable: CAN_ANGLE_ICUB2DEG
                                          //  Referenced by: '<S6>/CAN_Decoder'
                                          //  360/2^16

extern uint8_T CAN_ID_AMC;             // Variable: CAN_ID_AMC
                                          //  Referenced by:
                                          //    '<S6>/CAN_Decoder'
                                          //    '<S6>/CAN_Encoder'
                                          //  4 bits defining the ID of the AMC_BLDC board.


#ifdef __cplusplus

extern "C"
{

#endif

  // Model entry point functions
  extern void AMC_BLDC_initialize(void);
  extern void AMC_BLDC_step0(void);
  extern void AMC_BLDC_step_FOC(void);
  extern void AMC_BLDC_step_Time_1ms(void);
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
//  '<S1>'   : 'AMC_BLDC/Adapter1'
//  '<S2>'   : 'AMC_BLDC/Adapter2'
//  '<S3>'   : 'AMC_BLDC/Adapter3'
//  '<S4>'   : 'AMC_BLDC/Adapter4'
//  '<S5>'   : 'AMC_BLDC/Estimation'
//  '<S6>'   : 'AMC_BLDC/Messaging'
//  '<S7>'   : 'AMC_BLDC/Supervision'
//  '<S8>'   : 'AMC_BLDC/Estimation/Adapter'
//  '<S9>'   : 'AMC_BLDC/Estimation/Adapter1'
//  '<S10>'  : 'AMC_BLDC/Estimation/Adapter3'
//  '<S11>'  : 'AMC_BLDC/Estimation/Estimation_Temperature'
//  '<S12>'  : 'AMC_BLDC/Estimation/Mux'
//  '<S13>'  : 'AMC_BLDC/Estimation/Estimation_Temperature/Disabled'

#endif                                 // RTW_HEADER_AMC_BLDC_h_

//
// File trailer for generated code.
//
// [EOF]
//
