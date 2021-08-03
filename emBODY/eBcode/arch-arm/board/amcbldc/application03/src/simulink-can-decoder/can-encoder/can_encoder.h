//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: can_encoder.h
//
// Code generated for Simulink model 'can_encoder'.
//
// Model version                  : 1.314
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Tue Jul 20 13:35:44 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_can_encoder_h_
#define RTW_HEADER_can_encoder_h_
#include <cmath>
#include <cstring>
#include <stddef.h>
#include "rtwtypes.h"
#include "can_encoder_types.h"
#include <stddef.h>
#include "rt_nonfinite.h"
#include "rtGetInf.h"

// Model block global parameters (default storage)
extern real32_T rtP_CAN_ANGLE_DEG2ICUB;// Variable: CAN_ANGLE_DEG2ICUB
                                          //  Referenced by:
                                          //    '<S1>/Gain'
                                          //    '<S1>/Gain1'

extern uint16_T rtP_CAN_ID_HOST;       // Variable: CAN_ID_HOST
                                          //  Referenced by: '<S1>/Constant'


// Class declaration for model can_encoder
namespace can_messaging
{
  class CAN_Encoder {
    // public data and function members
   public:
    // Invariant block signals for model 'can_encoder'
    struct ConstB_can_encoder_h_T {
      uint8_T lengths;                 // '<S1>/lengths'
    };

    // Real-time Model Data Structure
    struct RT_MODEL_can_encoder_T {
      const char_T **errorStatus;
    };

    // model initialize function
    void initialize();

    // model step function
    void step(const real32_T *rtu_messages_tx_foc_current, const real32_T
              *rtu_messages_tx_foc_position, const real32_T
              *rtu_messages_tx_foc_velocity, const boolean_T *rtu_events_tx_foc,
              uint8_T *rty_pck_tx_available, uint16_T *rty_pck_tx_packets_ID,
              uint8_T rty_pck_tx_packets_PAYLOAD[8]);

    // Constructor
    CAN_Encoder();

    // Destructor
    ~CAN_Encoder();

    // Real-Time Model get method
    can_messaging::CAN_Encoder::RT_MODEL_can_encoder_T * getRTM();

    //member function to setup error status pointer
    void setErrorStatusPointer(const char_T **rt_errorStatus);

    // private data and function members
   private:
    // Real-Time Model
    RT_MODEL_can_encoder_T can_encoder_M;
  };
}

// Invariant block signals (default storage)
extern const can_messaging::CAN_Encoder::ConstB_can_encoder_h_T
  can_encoder_ConstB;

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
//  '<S2>'   : 'can_encoder/CAN_Encoder/MATLAB Function'

#endif                                 // RTW_HEADER_can_encoder_h_

//
// File trailer for generated code.
//
// [EOF]
//
