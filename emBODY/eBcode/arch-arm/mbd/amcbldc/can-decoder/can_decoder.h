//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_decoder.h
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 7.4
// Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
// C/C++ source code generated on : Mon Oct  7 15:55:50 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef can_decoder_h_
#define can_decoder_h_
#include "rtwtypes.h"
#include "can_decoder_types.h"
#include "rtw_defines.h"

// Block signals for system '<S2>/Decoding Logic'
struct B_DecodingLogic_can_decoder_T {
  PID msg_set_pid;                     // '<S2>/Decoding Logic'
  Targets msg_desired_targets;         // '<S2>/Decoding Logic'
  SupervisorInputLimits msg_set_current_limit;// '<S2>/Decoding Logic'
  MotorConfigurationExternal msg_set_motor_config;// '<S2>/Decoding Logic'
  EventTypes event_type;               // '<S2>/Decoding Logic'
  ControlModes msg_set_control_mode;   // '<S2>/Decoding Logic'
  CANErrorTypes error_type;            // '<S2>/Decoding Logic'
  boolean_T ev_error;                  // '<S2>/Decoding Logic'
};

// Block states (default storage) for system '<S2>/Decoding Logic'
struct DW_DecodingLogic_can_decoder_T {
  int32_T sfEvent;                     // '<S2>/Decoding Logic'
  uint32_T ev_errorEventCounter;       // '<S2>/Decoding Logic'
  uint16_T cmd_processed;              // '<S2>/Decoding Logic'
  uint8_T is_active_c3_can_decoder;    // '<S2>/Decoding Logic'
  uint8_T is_active_SET_CONTROL_MODE;  // '<S2>/Decoding Logic'
  uint8_T is_SET_CONTROL_MODE;         // '<S2>/Decoding Logic'
  uint8_T is_active_DESIRED_TARGETS;   // '<S2>/Decoding Logic'
  uint8_T is_DESIRED_TARGETS;          // '<S2>/Decoding Logic'
  uint8_T is_active_SET_OPTIONS;       // '<S2>/Decoding Logic'
  uint8_T is_SET_OPTIONS;              // '<S2>/Decoding Logic'
  uint8_T is_active_SET_MOTOR_CONFIG;  // '<S2>/Decoding Logic'
  uint8_T is_SET_MOTOR_CONFIG;         // '<S2>/Decoding Logic'
  uint8_T is_active_ERROR_HANDLING;    // '<S2>/Decoding Logic'
  uint8_T is_ERROR_HANDLING;           // '<S2>/Decoding Logic'
  boolean_T ev_async;                  // '<S2>/Decoding Logic'
};

// Block signals for system '<Root>/Cycling Decoder'
struct B_CoreSubsys_can_decoder_T {
  BUS_CAN_RX pck_rx_struct;            // '<S3>/RAW2STRUCT Decoding Logic'
  B_DecodingLogic_can_decoder_T sf_DecodingLogic;// '<S2>/Decoding Logic'
};

// Block states (default storage) for system '<Root>/Cycling Decoder'
struct DW_CoreSubsys_can_decoder_T {
  DW_DecodingLogic_can_decoder_T sf_DecodingLogic;// '<S2>/Decoding Logic'
};

// Block signals for model 'can_decoder'
struct B_can_decoder_c_T {
  B_CoreSubsys_can_decoder_T CoreSubsys[MAX_EVENTS_PER_TICK];// '<Root>/Cycling Decoder' 
};

// Block states (default storage) for model 'can_decoder'
struct DW_can_decoder_f_T {
  DW_CoreSubsys_can_decoder_T CoreSubsys[MAX_EVENTS_PER_TICK];// '<Root>/Cycling Decoder' 
};

// Real-time Model Data Structure
struct tag_RTM_can_decoder_T {
  const char_T **errorStatus;
};

struct MdlrefDW_can_decoder_T {
  B_can_decoder_c_T rtb;
  DW_can_decoder_f_T rtdw;
  RT_MODEL_can_decoder_T rtm;
};

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern real32_T CAN_ANGLE_ICUB2DEG;    // Variable: CAN_ANGLE_ICUB2DEG
                                          //  Referenced by: '<S2>/Decoding Logic'
                                          //  360/2^16

extern uint8_T CAN_ID_AMC;             // Variable: CAN_ID_AMC
                                          //  Referenced by: '<S2>/Constant'
                                          //  4 bits defining the ID of the AMC_BLDC board.


// Model reference registration function
extern void can_decoder_initialize(const char_T **rt_errorStatus,
  RT_MODEL_can_decoder_T *const can_decoder_M);
extern void can_decoder_DecodingLogic_Init(DW_DecodingLogic_can_decoder_T
  *localDW);
extern void can_decoder_DecodingLogic(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, uint8_T rtu_CAN_ID_DST, uint8_T
  rtu_CAN_VOLT_REF_SHIFT, real32_T rtu_CAN_VOLT_REF_GAIN,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW);
extern void can_decoder_Init(B_can_decoder_c_T *localB, DW_can_decoder_f_T
  *localDW);
extern void can_decoder(const BUS_CAN_MULTIPLE *rtu_pck_rx_raw, ReceivedEvents
  rty_messages_rx[MAX_EVENTS_PER_TICK], B_can_decoder_c_T *localB,
  DW_can_decoder_f_T *localDW);

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

#endif                                 // can_decoder_h_

//
// File trailer for generated code.
//
// [EOF]
//
