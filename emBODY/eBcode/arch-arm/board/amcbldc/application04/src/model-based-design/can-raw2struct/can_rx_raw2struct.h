//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: can_rx_raw2struct.h
//
// Code generated for Simulink model 'can_rx_raw2struct'.
//
// Model version                  : 1.315
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Mon Sep 20 12:43:51 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_can_rx_raw2struct_h_
#define RTW_HEADER_can_rx_raw2struct_h_
#include <cstring>
#include <stddef.h>
#include "rtwtypes.h"
#include "can_rx_raw2struct_types.h"
#include <stddef.h>

// Class declaration for model can_rx_raw2struct
namespace can_messaging
{
  class CAN_RX_raw2struct {
    // public data and function members
   public:
    // Real-time Model Data Structure
    struct RT_MODEL_can_rx_raw2struct_T {
      const char_T **errorStatus;
    };

    // model step function
    void step(const BUS_CAN &arg_pck_rx_raw, BUS_CAN_RX &arg_pck_rx_struct);

    // Constructor
    CAN_RX_raw2struct();

    // Destructor
    ~CAN_RX_raw2struct();

    // Real-Time Model get method
    can_messaging::CAN_RX_raw2struct::RT_MODEL_can_rx_raw2struct_T * getRTM();

    //member function to setup error status pointer
    void setErrorStatusPointer(const char_T **rt_errorStatus);

    // private data and function members
   private:
    // Real-Time Model
    RT_MODEL_can_rx_raw2struct_T can_rx_raw2struct_M;

    // private member function(s) for subsystem '<Root>/TmpModelReferenceSubsystem'
    CANClassTypes c_convert_to_enum_CANClassTypes(int32_T input);
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
//  '<Root>' : 'can_rx_raw2struct'
//  '<S1>'   : 'can_rx_raw2struct/CAN_RX_RAW2STRUCT'
//  '<S2>'   : 'can_rx_raw2struct/CAN_RX_RAW2STRUCT/RAW2STRUCT Decoding Logic'

#endif                                 // RTW_HEADER_can_rx_raw2struct_h_

//
// File trailer for generated code.
//
// [EOF]
//
