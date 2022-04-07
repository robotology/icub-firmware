//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_decoder.h
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 2.101
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Tue May 17 09:46:12 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_can_decoder_h_
#define RTW_HEADER_can_decoder_h_
#include <cstring>
#include "rtwtypes.h"
#include "can_decoder_types.h"

// Block states (default storage) for system '<S2>/Decoding Logic'
#ifndef can_decoder_MDLREF_HIDE_CHILD_

struct DW_DecodingLogic_can_decoder_T {
  int32_T sfEvent;                     // '<S2>/Decoding Logic'
  uint32_T ev_errorEventCounter;       // '<S2>/Decoding Logic'
  uint16_T cmd_processed;              // '<S2>/Decoding Logic'
  uint8_T is_active_c3_can_decoder;    // '<S2>/Decoding Logic'
  uint8_T is_SET_CONTROL_MODE;         // '<S2>/Decoding Logic'
  uint8_T is_active_SET_CONTROL_MODE;  // '<S2>/Decoding Logic'
  uint8_T is_DESIRED_CURRENT;          // '<S2>/Decoding Logic'
  uint8_T is_active_DESIRED_CURRENT;   // '<S2>/Decoding Logic'
  uint8_T is_SET_CURRENT_OPTIONS;      // '<S2>/Decoding Logic'
  uint8_T is_active_SET_CURRENT_OPTIONS;// '<S2>/Decoding Logic'
  uint8_T is_ERROR_HANDLING;           // '<S2>/Decoding Logic'
  uint8_T is_active_ERROR_HANDLING;    // '<S2>/Decoding Logic'
  boolean_T ev_async;                  // '<S2>/Decoding Logic'
};

#endif                                 //can_decoder_MDLREF_HIDE_CHILD_

// Block signals for system '<Root>/Cycling Decoder'
#ifndef can_decoder_MDLREF_HIDE_CHILD_

struct B_CoreSubsys_can_decoder_T {
  BUS_CAN_RX pck_rx_struct;            // '<S3>/RAW2STRUCT Decoding Logic'
  BUS_MSG_CURRENT_PID msg_set_current_pid;// '<S2>/Decoding Logic'
  BUS_MSG_CURRENT_LIMIT msg_set_current_limit;// '<S2>/Decoding Logic'
  BUS_MSG_DESIRED_CURRENT msg_desired_current;// '<S2>/Decoding Logic'
  BUS_MSG_CONTROL_MODE msg_set_control_mode;// '<S2>/Decoding Logic'
  CANErrorTypes error_type;            // '<S2>/Decoding Logic'
  boolean_T ev_set_control_mode;       // '<S2>/Decoding Logic'
  boolean_T ev_set_current_limit;      // '<S2>/Decoding Logic'
  boolean_T ev_desired_current;        // '<S2>/Decoding Logic'
  boolean_T ev_set_current_pid;        // '<S2>/Decoding Logic'
  boolean_T ev_error;                  // '<S2>/Decoding Logic'
};

#endif                                 //can_decoder_MDLREF_HIDE_CHILD_

// Block states (default storage) for system '<Root>/Cycling Decoder'
#ifndef can_decoder_MDLREF_HIDE_CHILD_

struct DW_CoreSubsys_can_decoder_T {
  boolean_T DelayInput1_DSTATE;        // '<S5>/Delay Input1'
  DW_DecodingLogic_can_decoder_T sf_DecodingLogic;// '<S2>/Decoding Logic'
};

#endif                                 //can_decoder_MDLREF_HIDE_CHILD_

// Block signals for model 'can_decoder'
#ifndef can_decoder_MDLREF_HIDE_CHILD_

struct B_can_decoder_c_T {
  B_CoreSubsys_can_decoder_T CoreSubsys[4];// '<Root>/Cycling Decoder'
};

#endif                                 //can_decoder_MDLREF_HIDE_CHILD_

// Block states (default storage) for model 'can_decoder'
#ifndef can_decoder_MDLREF_HIDE_CHILD_

struct DW_can_decoder_f_T {
  DW_CoreSubsys_can_decoder_T CoreSubsys[4];// '<Root>/Cycling Decoder'
};

#endif                                 //can_decoder_MDLREF_HIDE_CHILD_

#ifndef can_decoder_MDLREF_HIDE_CHILD_

// Real-time Model Data Structure
struct tag_RTM_can_decoder_T {
  const char_T **errorStatus;
};

#endif                                 //can_decoder_MDLREF_HIDE_CHILD_

#ifndef can_decoder_MDLREF_HIDE_CHILD_

struct MdlrefDW_can_decoder_T {
  RT_MODEL_can_decoder_T rtm;
};

#endif                                 //can_decoder_MDLREF_HIDE_CHILD_

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

extern void can_decoder_Init(void);
extern void can_decoder(const BUS_CAN_MULTIPLE *rtu_pck_rx_raw,
  BUS_MESSAGES_RX_MULTIPLE *rty_messages_rx, BUS_EVENTS_RX_MULTIPLE
  *rty_events_rx, BUS_CAN_RX_ERRORS_MULTIPLE *rty_errors_rx);

// Model reference registration function
extern void can_decoder_initialize(const char_T **rt_errorStatus);

#ifndef can_decoder_MDLREF_HIDE_CHILD_

extern void can_decoder_DecodingLogic_Init(BUS_MSG_CONTROL_MODE
  *rty_msg_set_control_mode, BUS_MSG_CURRENT_LIMIT *rty_msg_set_current_limit,
  CANErrorTypes *rty_error_type, BUS_MSG_DESIRED_CURRENT
  *rty_msg_desired_current, BUS_MSG_CURRENT_PID *rty_msg_set_current_pid,
  boolean_T *rty_ev_set_control_mode, boolean_T *rty_ev_set_current_limit,
  boolean_T *rty_ev_desired_current, boolean_T *rty_ev_set_current_pid,
  boolean_T *rty_ev_error, DW_DecodingLogic_can_decoder_T *localDW);
extern void can_decoder_DecodingLogic(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST, BUS_MSG_CONTROL_MODE
  *rty_msg_set_control_mode, BUS_MSG_CURRENT_LIMIT *rty_msg_set_current_limit,
  CANErrorTypes *rty_error_type, BUS_MSG_DESIRED_CURRENT
  *rty_msg_desired_current, BUS_MSG_CURRENT_PID *rty_msg_set_current_pid,
  boolean_T *rty_ev_set_control_mode, boolean_T *rty_ev_set_current_limit,
  boolean_T *rty_ev_desired_current, boolean_T *rty_ev_set_current_pid,
  boolean_T *rty_ev_error, DW_DecodingLogic_can_decoder_T *localDW);

#endif                                 //can_decoder_MDLREF_HIDE_CHILD_

#ifndef can_decoder_MDLREF_HIDE_CHILD_

extern MdlrefDW_can_decoder_T can_decoder_MdlrefDW;

#endif                                 //can_decoder_MDLREF_HIDE_CHILD_

#ifndef can_decoder_MDLREF_HIDE_CHILD_

// Block signals (default storage)
extern B_can_decoder_c_T can_decoder_B;

// Block states (default storage)
extern DW_can_decoder_f_T can_decoder_DW;

#endif                                 //can_decoder_MDLREF_HIDE_CHILD_

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
//  '<S5>'   : 'can_decoder/Cycling Decoder/CAN_Decoder/Detect Change'
//  '<S6>'   : 'can_decoder/Cycling Decoder/CAN_RX_RAW2STRUCT/RAW2STRUCT Decoding Logic'

#endif                                 // RTW_HEADER_can_decoder_h_

//
// File trailer for generated code.
//
// [EOF]
//
