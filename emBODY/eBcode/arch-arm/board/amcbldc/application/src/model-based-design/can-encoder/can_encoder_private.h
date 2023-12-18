//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_encoder_private.h
//
// Code generated for Simulink model 'can_encoder'.
//
// Model version                  : 6.2
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Thu Dec 14 10:43:28 2023
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_can_encoder_private_h_
#define RTW_HEADER_can_encoder_private_h_
#include "rtwtypes.h"
#include "can_encoder_types.h"

// Real-time Model Data Structure
struct tag_RTM_can_encoder_T {
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

struct MdlrefDW_can_encoder_T {
  RT_MODEL_can_encoder_T rtm;
};

extern void can_encoder_format_can_id(uint8_T rtu_class, uint8_T rtu_can_id_amc,
  uint8_T rtu_dst_typ, uint16_T *rty_pkt_id);
extern MdlrefDW_can_encoder_T can_encoder_MdlrefDW;

#endif                                 // RTW_HEADER_can_encoder_private_h_

//
// File trailer for generated code.
//
// [EOF]
//
