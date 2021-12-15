//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: AMC_BLDC.h
//
// Code generated for Simulink model 'AMC_BLDC'.
//
// Model version                  : 3.96
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Tue Dec 14 19:26:07 2021
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
#include "can_rx_raw2struct.h"
#include "can_decoder.h"
#include "SupervisorFSM_RX.h"
#include "control_outer.h"
#include "SupervisorFSM_TX.h"
#include "can_encoder.h"
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

extern real32_T CAN_ANGLE_DEG2ICUB;    // Variable: CAN_ANGLE_DEG2ICUB
                                          //  Referenced by: '<S3>/CAN_Encoder'
                                          //  2^16/360

extern uint8_T CAN_ID_AMC;             // Variable: CAN_ID_AMC
                                          //  Referenced by:
                                          //    '<S3>/CAN_Decoder'
                                          //    '<S3>/CAN_Encoder'
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
      ConfigurationParameters ConfigurationParameters_m;// '<S4>/SupervisorFSM_RX' 
      SensorsData BusConversion_InsertedFor_Estim;
      SensorsData BusConversion_InsertedFor_Super;
      BUS_CAN_RX pck_rx_struct;        // '<S3>/CAN_RX_raw2struct'
      Targets Targets_n;               // '<S4>/SupervisorFSM_RX'
      BUS_MESSAGES_RX CAN_Decoder_o1;  // '<S3>/CAN_Decoder'
      BUS_MESSAGES_TX MessagesTx;      // '<S4>/SupervisorFSM_TX'
      Flags Flags_k;                   // '<S4>/SupervisorFSM_RX'
      BUS_EVENTS_TX SupervisorFSM_TX_o2;// '<S4>/SupervisorFSM_TX'
      BUS_EVENTS_RX CAN_Decoder_o2;    // '<S3>/CAN_Decoder'
      BUS_CAN_RX_ERRORS CAN_Decoder_o3;// '<S3>/CAN_Decoder'
    };

    // Block states (default storage) for system '<Root>'
    struct DW_AMC_BLDC_T {
      ControlOutputs RTBInsertedForAdapter_InsertedF;// synthesized block
      ControlOutputs RTBInsertedForAdapter_Inserte_m;// synthesized block
      ControlOutputs RTBInsertedForAdapter_Inserte_b;// synthesized block
      ControlOuterOutputs TmpRTBAtFOCInport6_Buf[3];// synthesized block
      EstimatedData RTBInsertedForAdapter_Inserte_j[3];// synthesized block
      void* RTBInsertedForAdapter_Insert_mf;// synthesized block
      void* TmpRTBAtFOCInport6_SEMAPHORE;// synthesized block
      void* RTBInsertedForAdapter_Insert_bb;// synthesized block
      int8_T RTBInsertedForAdapter_Inserte_d;// synthesized block
      int8_T RTBInsertedForAdapter_Insert_de;// synthesized block
      int8_T TmpRTBAtFOCInport6_LstBufWR;// synthesized block
      int8_T TmpRTBAtFOCInport6_RDBuf; // synthesized block
      int8_T RTBInsertedForAdapter_Inserte_l;// synthesized block
      int8_T RTBInsertedForAdapter_Insert_lf;// synthesized block
    };

    // External inputs (root inport signals with default storage)
    struct ExtU_AMC_BLDC_T {
      real32_T SensorsData_jointpositions_posi;// '<Root>/B_-1_-1'
      real32_T SensorsData_motorsensors_Iabc[3];// '<Root>/B_-1_-1'
      real32_T SensorsData_motorsensors_angle;// '<Root>/B_-1_-1'
      real32_T SensorsData_motorsensors_temper;// '<Root>/B_-1_-1'
      real32_T SensorsData_motorsensors_voltag;// '<Root>/B_-1_-1'
      real32_T SensorsData_motorsensors_curren;// '<Root>/B_-1_-1'
      uint8_T SensorsData_motorsensors_hallAB;// '<Root>/B_-1_-1'
      uint8_T PacketsRx_available;     // '<Root>/B_-1_-1'
      uint8_T PacketsRx_lengths;       // '<Root>/B_-1_-1'
      uint16_T PacketsRx_packets_ID;   // '<Root>/B_-1_-1'
      uint8_T PacketsRx_packets_PAYLOAD[8];// '<Root>/B_-1_-1'
    };

    // External outputs (root outports fed by signals with default storage)
    struct ExtY_AMC_BLDC_T {
      ControlOutputs ControlOutputs_p; // '<Root>/ControlOutputs'
      EstimatedData EstimatedData_p;   // '<Root>/EstimatedData'
      BUS_CAN PacketsTx;               // '<Root>/PacketsTx'
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
          uint32_T TID[2];
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
    // model instance variable for '<S1>/FOC'
    amc_bldc_codegen::control_foc FOCMDLOBJ1;

    // model instance variable for '<S1>/OuterControl'
    amc_bldc_codegen::control_outer OuterControlMDLOBJ2;

    // model instance variable for '<S2>/Estimation_Velocity'
    amc_bldc_codegen::estimation_velocity Estimation_VelocityMDLOBJ3;

    // model instance variable for '<S3>/CAN_Decoder'
    amc_bldc_codegen::CAN_Decoder CAN_DecoderMDLOBJ4;

    // model instance variable for '<S3>/CAN_Encoder'
    amc_bldc_codegen::CAN_Encoder CAN_EncoderMDLOBJ5;

    // model instance variable for '<S3>/CAN_RX_raw2struct'
    amc_bldc_codegen::CAN_RX_raw2struct CAN_RX_raw2structMDLOBJ6;

    // model instance variable for '<S4>/SupervisorFSM_RX'
    amc_bldc_codegen::SupervisorFSM_RX SupervisorFSM_RXMDLOBJ7;

    // model instance variable for '<S4>/SupervisorFSM_TX'
    amc_bldc_codegen::SupervisorFSM_TX SupervisorFSM_TXMDLOBJ8;

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
//  '<S1>'   : 'AMC_BLDC/Control'
//  '<S2>'   : 'AMC_BLDC/Estimation'
//  '<S3>'   : 'AMC_BLDC/Messaging'
//  '<S4>'   : 'AMC_BLDC/Supervision'
//  '<S5>'   : 'AMC_BLDC/Control/Adapter'
//  '<S6>'   : 'AMC_BLDC/Control/Adapter1'
//  '<S7>'   : 'AMC_BLDC/Control/Adapter2'
//  '<S8>'   : 'AMC_BLDC/Control/Adapter3'
//  '<S9>'   : 'AMC_BLDC/Control/Adapter4'
//  '<S10>'  : 'AMC_BLDC/Control/Adapter5'

#endif                                 // RTW_HEADER_AMC_BLDC_h_

//
// File trailer for generated code.
//
// [EOF]
//
