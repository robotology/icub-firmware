//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_decoder.h
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 2.87
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Mon Jan 31 18:31:50 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_can_decoder_h_
#define RTW_HEADER_can_decoder_h_
#include <cstring>
#include <stddef.h>
#include "rtwtypes.h"
#include "can_decoder_types.h"
#include <stddef.h>

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern uint8_T CAN_ID_AMC;             // Variable: CAN_ID_AMC
                                          //  Referenced by: '<S2>/Constant'
                                          //  4 bits defining the ID of the AMC_BLDC board.


// Class declaration for model can_decoder
namespace amc_bldc_codegen
{
  class CAN_Decoder
  {
    // public data and function members
   public:
    // Block states (default storage) for system '<S2>/Decoding Logic'
    struct DW_DecodingLogic_can_decoder_T {
      int32_T sfEvent;                 // '<S2>/Decoding Logic'
      uint32_T ev_set_control_modeEventCounter;// '<S2>/Decoding Logic'
      uint32_T ev_set_current_limitEventCounte;// '<S2>/Decoding Logic'
      uint32_T ev_desired_currentEventCounter;// '<S2>/Decoding Logic'
      uint32_T ev_errorEventCounter;   // '<S2>/Decoding Logic'
      uint32_T ev_set_current_pidEventCounter;// '<S2>/Decoding Logic'
      uint16_T cmd_processed;          // '<S2>/Decoding Logic'
      uint8_T is_active_c3_can_decoder;// '<S2>/Decoding Logic'
      uint8_T is_SET_CONTROL_MODE;     // '<S2>/Decoding Logic'
      uint8_T is_active_SET_CONTROL_MODE;// '<S2>/Decoding Logic'
      uint8_T is_DESIRED_CURRENT;      // '<S2>/Decoding Logic'
      uint8_T is_active_DESIRED_CURRENT;// '<S2>/Decoding Logic'
      uint8_T is_SET_CURRENT_OPTIONS;  // '<S2>/Decoding Logic'
      uint8_T is_active_SET_CURRENT_OPTIONS;// '<S2>/Decoding Logic'
      uint8_T is_ERROR_HANDLING;       // '<S2>/Decoding Logic'
      uint8_T is_active_ERROR_HANDLING;// '<S2>/Decoding Logic'
      boolean_T ev_async;              // '<S2>/Decoding Logic'
    };

    // Block signals for system '<Root>/Cycling Decoder'
    struct B_CoreSubsys_can_decoder_T {
      BUS_CAN_RX pck_rx_struct;        // '<S3>/RAW2STRUCT Decoding Logic'
      BUS_MSG_CURRENT_PID msg_set_current_pid;// '<S2>/Decoding Logic'
      BUS_MSG_CURRENT_LIMIT msg_set_current_limit;// '<S2>/Decoding Logic'
      BUS_MSG_CONTROL_MODE msg_set_control_mode;// '<S2>/Decoding Logic'
      BUS_MSG_DESIRED_CURRENT msg_desired_current;// '<S2>/Decoding Logic'
      CANErrorTypes error_type;        // '<S2>/Decoding Logic'
      boolean_T ev_set_control_mode;   // '<S2>/Decoding Logic'
      boolean_T ev_set_current_limit;  // '<S2>/Decoding Logic'
      boolean_T ev_desired_current;    // '<S2>/Decoding Logic'
      boolean_T ev_error;              // '<S2>/Decoding Logic'
      boolean_T ev_set_current_pid;    // '<S2>/Decoding Logic'
    };

    // Real-time Model Data Structure
    struct RT_MODEL_can_decoder_T {
      const char_T **errorStatus;
    };

    // model data, for system '<S2>/Decoding Logic'
    struct self_DecodingLogic_can_decode_T {
      DW_DecodingLogic_can_decoder_T dwork;
    };

    // model data, for system '<Root>/Cycling Decoder'
    struct self_CoreSubsys_can_decoder_T {
      self_DecodingLogic_can_decode_T self_sf_DecodingLogic;
      B_CoreSubsys_can_decoder_T blockIO;
    };

    // Initial conditions function
    void init();

    // model step function
    BUS_MESSAGES_RX_MULTIPLE step(const BUS_CAN_MULTIPLE &arg_pck_rx_raw,
      BUS_EVENTS_RX_MULTIPLE &arg_events_rx, BUS_CAN_RX_ERRORS_MULTIPLE &
      arg_errors_rx);

    // Real-Time Model get method
    amc_bldc_codegen::CAN_Decoder::RT_MODEL_can_decoder_T * getRTM();

    //member function to setup error status pointer
    void setErrorStatusPointer(const char_T **rt_errorStatus);

    // model data
    self_CoreSubsys_can_decoder_T self_CoreSubsys[4];

    // Constructor
    CAN_Decoder();

    // Destructor
    ~CAN_Decoder();

    // private data and function members
   private:
    // private member function(s) for subsystem '<S2>/Decoding Logic'
    static void can_decoder_DecodingLogic_Init(self_DecodingLogic_can_decode_T
      *can_decoder_self_arg, BUS_MSG_CONTROL_MODE *rty_msg_set_control_mode,
      BUS_MSG_CURRENT_LIMIT *rty_msg_set_current_limit, CANErrorTypes
      *rty_error_type, BUS_MSG_DESIRED_CURRENT *rty_msg_desired_current,
      BUS_MSG_CURRENT_PID *rty_msg_set_current_pid, boolean_T
      *rty_ev_set_control_mode, boolean_T *rty_ev_set_current_limit, boolean_T
      *rty_ev_desired_current, boolean_T *rty_ev_error, boolean_T
      *rty_ev_set_current_pid);
    void can_decoder_DecodingLogic(self_DecodingLogic_can_decode_T
      *can_decoder_self_arg, boolean_T rtu_pck_available, const
      BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST,
      BUS_MSG_CONTROL_MODE *rty_msg_set_control_mode, BUS_MSG_CURRENT_LIMIT
      *rty_msg_set_current_limit, CANErrorTypes *rty_error_type,
      BUS_MSG_DESIRED_CURRENT *rty_msg_desired_current, BUS_MSG_CURRENT_PID
      *rty_msg_set_current_pid, boolean_T *rty_ev_set_control_mode, boolean_T
      *rty_ev_set_current_limit, boolean_T *rty_ev_desired_current, boolean_T
      *rty_ev_error, boolean_T *rty_ev_set_current_pid);
    int32_T can_de_safe_cast_to_MCStreaming(int32_T input);
    void can_decoder_ERROR_HANDLING(boolean_T rtu_pck_available, CANErrorTypes
      *rty_error_type, DW_DecodingLogic_can_decoder_T *localDW);
    boolean_T can_d_is_controlmode_recognized(int32_T mode);
    int16_T can_decoder_merge_2bytes_signed(uint16_T bl, uint16_T bh);
    int32_T can_safe_cast_to_MCControlModes(int32_T input);
    uint16_T can_decod_merge_2bytes_unsigned(uint16_T bl, uint16_T bh);

    // private member function(s) for subsystem '<Root>/TmpModelReferenceSubsystem'
    CANClassTypes c_convert_to_enum_CANClassTypes(int32_T input);

    // Real-Time Model
    RT_MODEL_can_decoder_T can_decoder_M;
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
//  '<Root>' : 'can_decoder'
//  '<S1>'   : 'can_decoder/Cycling Decoder'
//  '<S2>'   : 'can_decoder/Cycling Decoder/CAN_Decoder'
//  '<S3>'   : 'can_decoder/Cycling Decoder/CAN_RX_RAW2STRUCT'
//  '<S4>'   : 'can_decoder/Cycling Decoder/CAN_Decoder/Decoding Logic'
//  '<S5>'   : 'can_decoder/Cycling Decoder/CAN_RX_RAW2STRUCT/RAW2STRUCT Decoding Logic'

#endif                                 // RTW_HEADER_can_decoder_h_

//
// File trailer for generated code.
//
// [EOF]
//
