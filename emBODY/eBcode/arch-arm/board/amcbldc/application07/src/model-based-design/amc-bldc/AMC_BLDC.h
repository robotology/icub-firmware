//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: AMC_BLDC.h
//
// Code generated for Simulink model 'AMC_BLDC'.
//
// Model version                  : 3.268
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Wed Mar 16 14:14:12 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_AMC_BLDC_h_
#define RTW_HEADER_AMC_BLDC_h_
#include <stddef.h>
#include "rtwtypes.h"
#include "zero_crossing_types.h"
#include "AMC_BLDC_types.h"

// Child system includes
#include "control_foc.h"
#include "estimation_velocity.h"
#include "can_decoder.h"
#include "SupervisorFSM_RX.h"
#include "SupervisorFSM_TX.h"
#include "can_encoder.h"
#include "control_outer.h"
#include "rtw_mutex.h"

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


// Class declaration for model AMC_BLDC
namespace amc_bldc_codegen
{
  class AMC_BLDC
  {
    // public data and function members
   public:
    // Block signals (default storage)
    struct B_AMC_BLDC_T {
      BUS_MESSAGES_RX_MULTIPLE CAN_Decoder_o1;// '<S6>/CAN_Decoder'
      BUS_MESSAGES_TX MessagesTx;      // '<S7>/SupervisorFSM_TX'
      SensorsData RTBInsertedForAdapter_InsertedF;// '<Root>/Adapter3'
      Targets Targets_n;               // '<S7>/SupervisorFSM_RX'
      BUS_EVENTS_RX_MULTIPLE CAN_Decoder_o2;// '<S6>/CAN_Decoder'
      BUS_CAN_RX_ERRORS_MULTIPLE CAN_Decoder_o3;// '<S6>/CAN_Decoder'
      ControlOutputs RTBInsertedForAdapter_Inserte_a;// '<Root>/Adapter1'
      BUS_EVENTS_TX SupervisorFSM_TX_o2;// '<S7>/SupervisorFSM_TX'
    };

    // Block states (default storage) for system '<Root>'
    struct DW_AMC_BLDC_T {
      ConfigurationParameters ZOHBlockInsertedForAdapter_Inse;// synthesized block 
      ConfigurationParameters RTBInsertedForAdapter_InsertedF[3];// synthesized block 
      SensorsData RTBInsertedForAdapter_Inserte_e;// synthesized block
      SensorsData RTBInsertedForAdapter_Inserte_c;// synthesized block
      SensorsData RTBInsertedForAdapter_Inserte_j;// synthesized block
      Targets RTBInsertedForAdapter_Inserte_m[3];// synthesized block
      ControlOutputs RTBInsertedForAdapter_Inserte_d;// synthesized block
      ControlOutputs RTBInsertedForAdapter_Insert_j2;// synthesized block
      ControlOutputs RTBInsertedForAdapter_Inserte_o;// synthesized block
      ControlOuterOutputs RTBInsertedForAdapter_Inserte_i[3];// synthesized block 
      Flags RTBInsertedForAdapter_Inserte_l[3];// synthesized block
      EstimatedData RTBInsertedForAdapter_Inserte_k[3];// synthesized block
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
      SensorsData SensorsData_p;       // '<Root>/B_-1_-1'
      ExternalFlags ExternalFlags_p;   // '<Root>/B_-1_-1'
      BUS_CAN_MULTIPLE PacketsRx;      // '<Root>/B_-1_-1'
    };

    // External outputs (root outports fed by signals with default storage)
    struct ExtY_AMC_BLDC_T {
      ControlOutputs ControlOutputs_p; // '<Root>/ControlOutputs'
      ConfigurationParameters ConfigurationParameters_p;// '<Root>/ConfigurationParameters' 
      Flags Flags_p;                   // '<Root>/Flags'
      EstimatedData EstimatedData_p;   // '<Root>/EstimatedData'
      BUS_CAN_MULTIPLE PacketsTx;      // '<Root>/PacketsTx'
    };

    // Real-time Model Data Structure
    struct RT_MODEL_AMC_BLDC_T {
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

    // Real-Time Model get method
    amc_bldc_codegen::AMC_BLDC::RT_MODEL_AMC_BLDC_T * getRTM();

    // External inputs
    ExtU_AMC_BLDC_T AMC_BLDC_U;

    // External outputs
    ExtY_AMC_BLDC_T AMC_BLDC_Y;

    // Block signals
    B_AMC_BLDC_T AMC_BLDC_B;

    // Block states
    DW_AMC_BLDC_T AMC_BLDC_DW;

    // model initialize function
    void initialize();

    // model step function
    void step0();

    // model step function
    void step_FOC();

    // model step function
    void step_Time();

    // model terminate function
    void terminate();

    // Constructor
    AMC_BLDC();

    // Destructor
    ~AMC_BLDC();

    // private data and function members
   private:
    // model instance variable for '<S5>/Estimation_Velocity'
    amc_bldc_codegen::estimation_velocity Estimation_VelocityMDLOBJ1;

    // model instance variable for '<Root>/FOC'
    amc_bldc_codegen::control_foc FOCMDLOBJ2;

    // model instance variable for '<S6>/CAN_Decoder'
    amc_bldc_codegen::CAN_Decoder CAN_DecoderMDLOBJ2;

    // model instance variable for '<S6>/CAN_Encoder'
    amc_bldc_codegen::CAN_Encoder CAN_EncoderMDLOBJ4;

    // model instance variable for '<Root>/OuterControl'
    amc_bldc_codegen::control_outer OuterControlMDLOBJ5;

    // model instance variable for '<S7>/SupervisorFSM_RX'
    amc_bldc_codegen::SupervisorFSM_RX SupervisorFSM_RXMDLOBJ6;

    // model instance variable for '<S7>/SupervisorFSM_TX'
    amc_bldc_codegen::SupervisorFSM_TX SupervisorFSM_TXMDLOBJ7;

    // Real-Time Model
    RT_MODEL_AMC_BLDC_T AMC_BLDC_M;
  };
}

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
//  '<S2>'   : 'AMC_BLDC/Adapter1'
//  '<S3>'   : 'AMC_BLDC/Adapter2'
//  '<S4>'   : 'AMC_BLDC/Adapter3'
//  '<S5>'   : 'AMC_BLDC/Estimation'
//  '<S6>'   : 'AMC_BLDC/Messaging'
//  '<S7>'   : 'AMC_BLDC/Supervision'

#endif                                 // RTW_HEADER_AMC_BLDC_h_

//
// File trailer for generated code.
//
// [EOF]
//
