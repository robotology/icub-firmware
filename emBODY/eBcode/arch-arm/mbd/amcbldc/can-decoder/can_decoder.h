//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_decoder.h
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 6.3
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Wed Mar  6 15:00:25 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_can_decoder_h_
#define RTW_HEADER_can_decoder_h_
#include "rtwtypes.h"
#include "can_decoder_types.h"

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

extern void can_decoder_Init(void);
extern void can_decoder(const BUS_CAN_MULTIPLE *rtu_pck_rx_raw, const
  ConfigurationParameters *rtu_ConfigurationParameters, BUS_MESSAGES_RX_MULTIPLE
  *rty_messages_rx, BUS_STATUS_RX_MULTIPLE *rty_status_rx,
  BUS_CAN_RX_ERRORS_MULTIPLE *rty_errors_rx);

// Model reference registration function
extern void can_decoder_initialize(const char_T **rt_errorStatus);

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
//  '<S6>'   : 'can_decoder/Cycling Decoder/CAN_Decoder/Detect Change1'
//  '<S7>'   : 'can_decoder/Cycling Decoder/CAN_Decoder/Detect Change2'
//  '<S8>'   : 'can_decoder/Cycling Decoder/CAN_Decoder/Detect Change3'
//  '<S9>'   : 'can_decoder/Cycling Decoder/CAN_Decoder/Detect Change4'
//  '<S10>'  : 'can_decoder/Cycling Decoder/CAN_Decoder/Detect Change5'
//  '<S11>'  : 'can_decoder/Cycling Decoder/CAN_Decoder/Detect Change6'
//  '<S12>'  : 'can_decoder/Cycling Decoder/CAN_RX_RAW2STRUCT/RAW2STRUCT Decoding Logic'

#endif                                 // RTW_HEADER_can_decoder_h_

//
// File trailer for generated code.
//
// [EOF]
//
