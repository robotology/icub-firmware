//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_decoder_private.h
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 6.3
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Tue Feb 13 11:54:24 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_can_decoder_private_h_
#define RTW_HEADER_can_decoder_private_h_
#include "rtwtypes.h"
#include "can_decoder_types.h"
#include "rtw_defines.h"

// Block signals for system '<S2>/Decoding Logic'
struct B_DecodingLogic_can_decoder_T {
  BUS_MSG_PID msg_set_pid;             // '<S2>/Decoding Logic'
  BUS_MSG_MOTOR_CONFIG msg_set_motor_config;// '<S2>/Decoding Logic'
  BUS_MSG_DESIRED_TARGETS msg_desired_targets;// '<S2>/Decoding Logic'
  BUS_MSG_CURRENT_LIMIT msg_set_current_limit;// '<S2>/Decoding Logic'
  BUS_MSG_CONTROL_MODE msg_set_control_mode;// '<S2>/Decoding Logic'
  CANErrorTypes error_type;            // '<S2>/Decoding Logic'
  boolean_T ev_error;                  // '<S2>/Decoding Logic'
  boolean_T ev_set_control_mode;       // '<S2>/Decoding Logic'
  boolean_T ev_set_current_limit;      // '<S2>/Decoding Logic'
  boolean_T ev_desired_targets;        // '<S2>/Decoding Logic'
  boolean_T ev_set_current_pid;        // '<S2>/Decoding Logic'
  boolean_T ev_set_velocity_pid;       // '<S2>/Decoding Logic'
  boolean_T ev_set_motor_config;       // '<S2>/Decoding Logic'
};

// Block states (default storage) for system '<S2>/Decoding Logic'
struct DW_DecodingLogic_can_decoder_T {
  int32_T sfEvent;                     // '<S2>/Decoding Logic'
  uint32_T ev_errorEventCounter;       // '<S2>/Decoding Logic'
  uint32_T ev_set_control_modeEventCounter;// '<S2>/Decoding Logic'
  uint32_T ev_set_current_limitEventCounter;// '<S2>/Decoding Logic'
  uint32_T ev_desired_targetsEventCounter;// '<S2>/Decoding Logic'
  uint32_T ev_set_current_pidEventCounter;// '<S2>/Decoding Logic'
  uint32_T ev_set_velocity_pidEventCounter;// '<S2>/Decoding Logic'
  uint32_T ev_set_motor_configEventCounter;// '<S2>/Decoding Logic'
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
  boolean_T DelayInput1_DSTATE;        // '<S6>/Delay Input1'
  boolean_T DelayInput1_DSTATE_n;      // '<S7>/Delay Input1'
  boolean_T DelayInput1_DSTATE_nk;     // '<S8>/Delay Input1'
  boolean_T DelayInput1_DSTATE_h;      // '<S9>/Delay Input1'
  boolean_T DelayInput1_DSTATE_l;      // '<S10>/Delay Input1'
  boolean_T DelayInput1_DSTATE_d;      // '<S11>/Delay Input1'
  boolean_T DelayInput1_DSTATE_j;      // '<S5>/Delay Input1'
  DW_DecodingLogic_can_decoder_T sf_DecodingLogic;// '<S2>/Decoding Logic'
};

// Block signals for model 'can_decoder'
struct B_can_decoder_c_T {
  B_CoreSubsys_can_decoder_T CoreSubsys[CAN_MAX_NUM_PACKETS];// '<Root>/Cycling Decoder' 
};

// Block states (default storage) for model 'can_decoder'
struct DW_can_decoder_f_T {
  DW_CoreSubsys_can_decoder_T CoreSubsys[CAN_MAX_NUM_PACKETS];// '<Root>/Cycling Decoder' 
};

// Real-time Model Data Structure
struct tag_RTM_can_decoder_T {
  const char_T **errorStatus;
};

// Macros for accessing real-time model data structure
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         (*((rtm)->errorStatus))
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    (*((rtm)->errorStatus) = (val))
#endif

#ifndef rtmGetErrorStatusPointer
#define rtmGetErrorStatusPointer(rtm)  (rtm)->errorStatus
#endif

#ifndef rtmSetErrorStatusPointer
#define rtmSetErrorStatusPointer(rtm, val) ((rtm)->errorStatus = (val))
#endif

struct MdlrefDW_can_decoder_T {
  RT_MODEL_can_decoder_T rtm;
};

extern void can_decoder_DecodingLogic_Init(DW_DecodingLogic_can_decoder_T
  *localDW);
extern void can_decoder_DecodingLogic(boolean_T rtu_pck_available, const
  BUS_CAN_PACKET_RX *rtu_pck_input, const ConfigurationParameters
  *rtu_ConfigurationParameters, uint8_T rtu_CAN_ID_DST, uint8_T
  rtu_CAN_VOLT_REF_SHIFT, real32_T rtu_CAN_VOLT_REF_GAIN,
  B_DecodingLogic_can_decoder_T *localB, DW_DecodingLogic_can_decoder_T *localDW);
extern MdlrefDW_can_decoder_T can_decoder_MdlrefDW;

// Block signals (default storage)
extern B_can_decoder_c_T can_decoder_B;

// Block states (default storage)
extern DW_can_decoder_f_T can_decoder_DW;

#endif                                 // RTW_HEADER_can_decoder_private_h_

//
// File trailer for generated code.
//
// [EOF]
//
