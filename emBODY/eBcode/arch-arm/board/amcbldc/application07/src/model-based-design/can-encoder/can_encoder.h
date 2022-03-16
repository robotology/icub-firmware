//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_encoder.h
//
// Code generated for Simulink model 'can_encoder'.
//
// Model version                  : 2.55
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Wed Mar  9 12:12:40 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_can_encoder_h_
#define RTW_HEADER_can_encoder_h_
#include <cstring>
#include <stddef.h>
#include "rtwtypes.h"
#include "can_encoder_types.h"
#include <stddef.h>

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern real32_T CAN_ANGLE_DEG2ICUB;    // Variable: CAN_ANGLE_DEG2ICUB
                                          //  Referenced by:
                                          //    '<S2>/Gain1'
                                          //    '<S2>/Gain3'
                                          //  2^16/360

extern uint8_T CAN_ID_AMC;             // Variable: CAN_ID_AMC
                                          //  Referenced by:
                                          //    '<S2>/Constant'
                                          //    '<S4>/Constant'
                                          //  4 bits defining the ID of the AMC_BLDC board.


// Class declaration for model can_encoder
namespace amc_bldc_codegen
{
  class CAN_Encoder
  {
    // public data and function members
   public:
    // Real-time Model Data Structure
    struct RT_MODEL_can_encoder_T {
      const char_T **errorStatus;
    };

    // model step function
    void step(const BUS_MESSAGES_TX &arg_messages_tx, const BUS_EVENTS_TX &
              arg_events_tx, BUS_CAN_MULTIPLE &arg_pck_tx);

    // Real-Time Model get method
    amc_bldc_codegen::CAN_Encoder::RT_MODEL_can_encoder_T * getRTM();

    //member function to setup error status pointer
    void setErrorStatusPointer(const char_T **rt_errorStatus);

    // Constructor
    CAN_Encoder();

    // Destructor
    ~CAN_Encoder();

    // private data and function members
   private:
    // private member function(s) for subsystem '<S5>/format_can_id'
    static void can_encoder_format_can_id(uint8_T rtu_class, uint8_T
      rtu_can_id_amc, uint8_T rtu_dst_typ, uint16_T *rty_pkt_id);

    // Real-Time Model
    RT_MODEL_can_encoder_T can_encoder_M;
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
//  '<Root>' : 'can_encoder'
//  '<S1>'   : 'can_encoder/CAN_Encoder'
//  '<S2>'   : 'can_encoder/CAN_Encoder/FOC'
//  '<S3>'   : 'can_encoder/CAN_Encoder/NotUsed'
//  '<S4>'   : 'can_encoder/CAN_Encoder/STATUS'
//  '<S5>'   : 'can_encoder/CAN_Encoder/FOC/format_can_id'
//  '<S6>'   : 'can_encoder/CAN_Encoder/FOC/format_foc_pck'
//  '<S7>'   : 'can_encoder/CAN_Encoder/FOC/format_can_id/format_can_id'
//  '<S8>'   : 'can_encoder/CAN_Encoder/STATUS/format_can_id'
//  '<S9>'   : 'can_encoder/CAN_Encoder/STATUS/format_status_pck'
//  '<S10>'  : 'can_encoder/CAN_Encoder/STATUS/format_can_id/format_can_id'

#endif                                 // RTW_HEADER_can_encoder_h_

//
// File trailer for generated code.
//
// [EOF]
//
