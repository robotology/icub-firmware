//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: can_decoder.h
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 1.280
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Mon Sep 20 12:43:37 2021
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

// Shared type includes
#include "multiword_types.h"
#include <stddef.h>

// Model block global parameters (default storage)
extern uint8_T rtP_CAN_ID_AMC;         // Variable: CAN_ID_AMC
                                          //  Referenced by: '<S1>/Constant'


// Class declaration for model can_decoder
namespace can_messaging
{
  class CAN_Decoder {
    // public data and function members
   public:
    // Block signals for model 'can_decoder'
    struct B_can_decoder_T {
      BUS_MSG_DESIRED_CURRENT msg_desired_current;// '<S1>/Decoding Logic'
      BUS_MSG_CURRENT_LIMIT msg_set_current_limit;// '<S1>/Decoding Logic'
      BUS_MSG_CONTROL_MODE msg_set_control_mode;// '<S1>/Decoding Logic'
      CANErrorTypes error_type;        // '<S1>/Decoding Logic'
      boolean_T ev_set_control_mode;   // '<S1>/Decoding Logic'
      boolean_T ev_set_current_limit;  // '<S1>/Decoding Logic'
      boolean_T ev_desired_current;    // '<S1>/Decoding Logic'
      boolean_T ev_error;              // '<S1>/Decoding Logic'
    };

    // Block states (default storage) for model 'can_decoder'
    struct DW_can_decoder_T {
      int32_T sfEvent;                 // '<S1>/Decoding Logic'
      uint32_T ev_set_control_modeEventCounter;// '<S1>/Decoding Logic'
      uint32_T ev_set_current_limitEventCounte;// '<S1>/Decoding Logic'
      uint32_T ev_desired_currentEventCounter;// '<S1>/Decoding Logic'
      uint32_T ev_errorEventCounter;   // '<S1>/Decoding Logic'
      uint16_T cmd_processed;          // '<S1>/Decoding Logic'
      uint8_T is_active_c3_can_decoder;// '<S1>/Decoding Logic'
      uint8_T is_SET_CONTROL_MODE;     // '<S1>/Decoding Logic'
      uint8_T is_active_SET_CONTROL_MODE;// '<S1>/Decoding Logic'
      uint8_T is_DESIRED_CURRENT;      // '<S1>/Decoding Logic'
      uint8_T is_active_DESIRED_CURRENT;// '<S1>/Decoding Logic'
      uint8_T is_SET_CURRENT_LIMIT;    // '<S1>/Decoding Logic'
      uint8_T is_active_SET_CURRENT_LIMIT;// '<S1>/Decoding Logic'
      uint8_T is_ERROR_HANDLING;       // '<S1>/Decoding Logic'
      uint8_T is_active_ERROR_HANDLING;// '<S1>/Decoding Logic'
      boolean_T ev_async;              // '<S1>/Decoding Logic'
    };

    // Real-time Model Data Structure
    struct RT_MODEL_can_decoder_T {
      const char_T **errorStatus;
    };

    // Block signals
    B_can_decoder_T can_decoder_B;

    // Block states
    DW_can_decoder_T can_decoder_DW;

    // model step function
    void step(const BUS_CAN_RX &arg_pck_rx, BUS_MESSAGES_RX &arg_messages_rx,
              BUS_EVENTS_RX &arg_events_rx, BUS_CAN_RX_ERRORS &arg_errors_rx);

    // Initial conditions function
    void init(BUS_MESSAGES_RX *arg_messages_rx, BUS_EVENTS_RX *arg_events_rx,
              BUS_CAN_RX_ERRORS *arg_errors_rx);

    // Constructor
    CAN_Decoder();

    // Destructor
    ~CAN_Decoder();

    // Real-Time Model get method
    can_messaging::CAN_Decoder::RT_MODEL_can_decoder_T * getRTM();

    //member function to setup error status pointer
    void setErrorStatusPointer(const char_T **rt_errorStatus);

    // private data and function members
   private:
    // Real-Time Model
    RT_MODEL_can_decoder_T can_decoder_M;

    // private member function(s) for subsystem '<Root>/TmpModelReferenceSubsystem'
    int32_T can_de_safe_cast_to_MCStreaming(int32_T input);
    real_T can_decoder_merge_2bytes(real_T bl, real_T bh);
    void can_decoder_ERROR_HANDLING(const BUS_CAN_RX *arg_pck_rx);
    real_T can_d_is_controlmode_recognized(real_T mode);
    int32_T can_safe_cast_to_MCControlModes(int32_T input);
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
//  '<S1>'   : 'can_decoder/CAN_Decoder'
//  '<S2>'   : 'can_decoder/CAN_Decoder/Decoding Logic'

#endif                                 // RTW_HEADER_can_decoder_h_

//
// File trailer for generated code.
//
// [EOF]
//
