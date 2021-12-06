//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: AMC_BLDC.h
//
// Code generated for Simulink model 'AMC_BLDC'.
//
// Model version                  : 3.76
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Thu Dec  2 09:39:37 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_AMC_BLDC_h_
#define RTW_HEADER_AMC_BLDC_h_
#include "rtwtypes.h"
#include "zero_crossing_types.h"
#include "AMC_BLDC_types.h"

// Child system includes
#include "control_foc.h"
#include "can_rx_raw2struct.h"
#include "can_decoder.h"
#include "estimation_velocity.h"
#include "SupervisorFSM_RX.h"
#include "control_outer.h"
#include "SupervisorFSM_TX.h"
#include "can_encoder.h"

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
      ConfigurationParameters RTBInsertedForAdapter_InsertedF;// '<S1>/Adapter3' 
      ConfigurationParameters ConfigurationParameters_m;// '<S4>/SupervisorFSM_RX' 
      ConfigurationParameters RTBInsertedForAdapter_Inserte_k;// '<S4>/Adapter12' 
      SensorsData BusConversion_InsertedFor_FOC_a;
      SensorsData TmpRTBAtEstimation_VelocityInpo;
      SensorsData BusConversion_InsertedFor_Super;
      SensorsData TmpRTBAtOuterControlInport4;
      BUS_CAN_RX TmpRTBAtCAN_DecoderInport1;// '<S3>/CAN_RX_raw2struct'
      Targets RTBInsertedForAdapter_Inserte_b;// '<S1>/Adapter4'
      Targets Targets_n;               // '<S4>/SupervisorFSM_RX'
      Targets RTBInsertedForAdapter_Inserte_e;// '<S4>/Adapter9'
      ControlOutputs FOC;              // '<S1>/FOC'
      ControlOutputs BusConversion_InsertedFor_Sup_j;
      BUS_MESSAGES_RX CAN_Decoder_o1;  // '<S3>/CAN_Decoder'
      BUS_MESSAGES_RX RTBInsertedForAdapter_Inserte_i;// '<S4>/Adapter2'
      BUS_MESSAGES_RX RTBInsertedForAdapter_Inserte_f;// '<S4>/Adapter5'
      ControlOuterOutputs RTBInsertedForAdapter_Inserte_m;// '<S1>/Adapter'
      ControlOuterOutputs OuterControl;// '<S1>/OuterControl'
      BUS_MESSAGES_TX MessagesTx;      // '<S4>/SupervisorFSM_TX'
      BUS_MESSAGES_TX RTBInsertedForAdapter_Inserte_h;// '<S4>/Adapter10'
      Flags RTBInsertedForAdapter_Insert_io;// '<S1>/Adapter2'
      Flags Flags_k;                   // '<S4>/SupervisorFSM_RX'
      Flags RTBInsertedForAdapter_Insert_hb;// '<S4>/Adapter8'
      EstimatedData BusConversion_InsertedFor_FOC_i;
      EstimatedData RTBInsertedForAdapter_Insert_il;// '<S4>/Component1'
      EstimatedData sensorsdata;       // '<S4>/Adapter11'
      BUS_EVENTS_TX SupervisorFSM_TX_o2;// '<S4>/SupervisorFSM_TX'
      BUS_EVENTS_TX RTBInsertedForAdapter_Insert_it;// '<S4>/Adapter7'
      BUS_EVENTS_RX CAN_Decoder_o2;    // '<S3>/CAN_Decoder'
      BUS_EVENTS_RX RTBInsertedForAdapter_Inser_mdo;// '<S4>/Adapter'
      BUS_CAN_RX_ERRORS CAN_Decoder_o3;// '<S3>/CAN_Decoder'
      BUS_CAN_RX_ERRORS RTBInsertedForAdapter_Inserte_o;// '<S4>/Adapter3'
      real32_T position;
      real32_T Iabc[3];
      real32_T angle;
      real32_T temperature;
      real32_T voltage;
      real32_T current;
      real32_T position_o;
      real32_T Iabc_p[3];
      real32_T angle_o;
      real32_T temperature_i;
      real32_T voltage_g;
      real32_T current_l;
      uint8_T hallABC;
      uint8_T hallABC_c;
    };

    // Block states (default storage) for system '<Root>'
    struct DW_AMC_BLDC_T {
      volatile real32_T RTBInsertedForAdapter_InsertedF;// synthesized block
      volatile real32_T RTBInsertedForAdapter_Inserte_p[3];// synthesized block
      volatile real32_T RTBInsertedForAdapter_Inserte_m[3];// synthesized block
      volatile int8_T RTBInsertedForAdapter_Inserte_h;// synthesized block
      volatile int8_T RTBInsertedForAdapter_Inserte_l;// synthesized block
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
      real32_T ControlOutputs_Vabc[3]; // '<Root>/ControlOutputs_Vabc'
      real32_T ControlOutputs_Iq_fbk_current;
                                      // '<Root>/ControlOutputs_Iq_fbk_current'
      EstimatedData EstimatedData_p;   // '<Root>/EstimatedData'
      BUS_CAN PacketsTx;               // '<Root>/PacketsTx'
    };

    // Real-time Model Data Structure
    struct RT_MODEL_AMC_BLDC_T {
      const char_T *errorStatus;
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
    void step_CAN_RX_raw2struct();

    // model step function
    void step_CAN_Decoder();

    // model step function
    void step_FOC_transitions();

    // model step function
    void step_Estimation_Velocity();

    // model step function
    void step_SupervisorFSM_RX();

    // model step function
    void step_OuterControl();

    // model step function
    void step_SupervisorFSM_TX();

    // model step function
    void step_CAN_Encoder();

    // model terminate function
    static void terminate();

    // Constructor
    AMC_BLDC();

    // Destructor
    ~AMC_BLDC();

    // private data and function members
   private:
    // model instance variable for '<S1>/OuterControl'
    amc_bldc_codegen::control_outer OuterControlMDLOBJ1;

    // model instance variable for '<S2>/Estimation_Velocity'
    amc_bldc_codegen::estimation_velocity Estimation_VelocityMDLOBJ2;

    // model instance variable for '<S3>/CAN_Decoder'
    amc_bldc_codegen::CAN_Decoder CAN_DecoderMDLOBJ3;

    // model instance variable for '<S3>/CAN_Encoder'
    amc_bldc_codegen::CAN_Encoder CAN_EncoderMDLOBJ4;

    // model instance variable for '<S3>/CAN_RX_raw2struct'
    amc_bldc_codegen::CAN_RX_raw2struct CAN_RX_raw2structMDLOBJ5;

    // model instance variable for '<S4>/SupervisorFSM_RX'
    amc_bldc_codegen::SupervisorFSM_RX SupervisorFSM_RXMDLOBJ6;

    // model instance variable for '<S4>/SupervisorFSM_TX'
    amc_bldc_codegen::SupervisorFSM_TX SupervisorFSM_TXMDLOBJ7;

    // model instance variable for '<S1>/FOC'
    amc_bldc_codegen::control_foc FOCMDLOBJ8;

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
//  '<S11>'  : 'AMC_BLDC/Control/EstimatedData2VirtualBus'
//  '<S12>'  : 'AMC_BLDC/Control/FOCOutputs2VirtualBus'
//  '<S13>'  : 'AMC_BLDC/Estimation/Adapter'
//  '<S14>'  : 'AMC_BLDC/Supervision/Adapter'
//  '<S15>'  : 'AMC_BLDC/Supervision/Adapter1'
//  '<S16>'  : 'AMC_BLDC/Supervision/Adapter10'
//  '<S17>'  : 'AMC_BLDC/Supervision/Adapter11'
//  '<S18>'  : 'AMC_BLDC/Supervision/Adapter12'
//  '<S19>'  : 'AMC_BLDC/Supervision/Adapter2'
//  '<S20>'  : 'AMC_BLDC/Supervision/Adapter3'
//  '<S21>'  : 'AMC_BLDC/Supervision/Adapter4'
//  '<S22>'  : 'AMC_BLDC/Supervision/Adapter5'
//  '<S23>'  : 'AMC_BLDC/Supervision/Adapter6'
//  '<S24>'  : 'AMC_BLDC/Supervision/Adapter7'
//  '<S25>'  : 'AMC_BLDC/Supervision/Adapter8'
//  '<S26>'  : 'AMC_BLDC/Supervision/Adapter9'
//  '<S27>'  : 'AMC_BLDC/Supervision/Component'
//  '<S28>'  : 'AMC_BLDC/Supervision/Component1'

#endif                                 // RTW_HEADER_AMC_BLDC_h_

//
// File trailer for generated code.
//
// [EOF]
//
