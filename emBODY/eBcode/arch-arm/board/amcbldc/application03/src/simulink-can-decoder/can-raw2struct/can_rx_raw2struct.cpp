//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: can_rx_raw2struct.cpp
//
// Code generated for Simulink model 'can_rx_raw2struct'.
//
// Model version                  : 1.310
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Tue Jul 20 13:35:51 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "can_rx_raw2struct.h"
#include "can_rx_raw2struct_private.h"

namespace can_messaging
{
  // Function for MATLAB Function: '<S1>/RAW2STRUCT Decoding Logic'
  CANClassTypes CAN_RX_raw2struct::c_convert_to_enum_CANClassTypes(int32_T input)
  {
    int32_T output;

    // Initialize output value to default value for CANClassTypes (Motor_Control_Command) 
    output = 0;
    if (((input >= 0) && (input <= 2)) || ((input >= 4) && (input <= 7))) {
      // Set output value to input value if it is a member of CANClassTypes
      output = input;
    }

    return static_cast<CANClassTypes>(output);
  }
}

namespace can_messaging
{
  // Output and update for referenced model: 'can_rx_raw2struct'
  void CAN_RX_raw2struct::step(const uint8_T *rtu_pck_rx_raw_available, const
    uint8_T *rtu_pck_rx_raw_lengths, const uint16_T *rtu_pck_rx_raw_packets_ID,
    const uint8_T rtu_pck_rx_raw_packets_PAYLOAD[8], uint8_T
    *rty_pck_rx_struct_available, CANClassTypes *rty_pck_rx_struct_packets_ID_CL,
    uint8_T *rty_pck_rx_struct_packets_ID_SR, uint8_T
    *rty_pck_rx_struct_packets_ID_DS, uint8_T *rty_pck_rx_struct_packets_PAYLO,
    boolean_T *rty_pck_rx_struct_packets_PAY_f, uint8_T
    *rty_pck_rx_struct_packets_PAY_k, uint8_T rty_pck_rx_struct_packets_PAY_h[7])
  {
    int32_T i;
    uint32_T qY;
    uint8_T rtb_BusConversion_InsertedFor_0;
    uint8_T varargin_1_idx_1;

    // Outputs for Atomic SubSystem: '<Root>/CAN_RX_RAW2STRUCT'
    // SignalConversion generated from: '<Root>/pck_rx_struct' incorporates:
    //   BusCreator generated from: '<S1>/RAW2STRUCT Decoding Logic'

    *rty_pck_rx_struct_available = *rtu_pck_rx_raw_available;

    // MATLAB Function: '<S1>/RAW2STRUCT Decoding Logic' incorporates:
    //   BusCreator generated from: '<S1>/RAW2STRUCT Decoding Logic'

    varargin_1_idx_1 = *rtu_pck_rx_raw_lengths;
    rtb_BusConversion_InsertedFor_0 = 8U;
    if (8 > varargin_1_idx_1) {
      rtb_BusConversion_InsertedFor_0 = varargin_1_idx_1;
    }

    varargin_1_idx_1 = rtb_BusConversion_InsertedFor_0;
    rtb_BusConversion_InsertedFor_0 = 0U;
    if (0 < varargin_1_idx_1) {
      rtb_BusConversion_InsertedFor_0 = varargin_1_idx_1;
    }

    for (i = 0; i < 7; i++) {
      // SignalConversion generated from: '<Root>/pck_rx_struct' incorporates:
      //   MATLAB Function: '<S1>/RAW2STRUCT Decoding Logic'

      rty_pck_rx_struct_packets_PAY_h[i] = 0U;
    }

    // MATLAB Function: '<S1>/RAW2STRUCT Decoding Logic' incorporates:
    //   BusCreator generated from: '<S1>/RAW2STRUCT Decoding Logic'
    //   SignalConversion generated from: '<Root>/pck_rx_struct'

    qY = rtb_BusConversion_InsertedFor_0 - 1U;
    if (rtb_BusConversion_InsertedFor_0 - 1U > rtb_BusConversion_InsertedFor_0)
    {
      qY = 0U;
    }

    for (i = 1; i - 1 < static_cast<uint8_T>(qY); i++) {
      rty_pck_rx_struct_packets_PAY_h[static_cast<uint8_T>(i) - 1] =
        rtu_pck_rx_raw_packets_PAYLOAD[static_cast<uint8_T>(i)];
    }

    // SignalConversion generated from: '<Root>/pck_rx_struct' incorporates:
    //   MATLAB Function: '<S1>/RAW2STRUCT Decoding Logic'

    *rty_pck_rx_struct_packets_ID_CL = c_convert_to_enum_CANClassTypes(
      static_cast<int32_T>(static_cast<uint16_T>(static_cast<uint32_T>
      (*rtu_pck_rx_raw_packets_ID & 1792) >> 8)));

    // SignalConversion generated from: '<Root>/pck_rx_struct' incorporates:
    //   MATLAB Function: '<S1>/RAW2STRUCT Decoding Logic'

    *rty_pck_rx_struct_packets_ID_SR = static_cast<uint8_T>(static_cast<uint32_T>
      (*rtu_pck_rx_raw_packets_ID & 240) >> 4);

    // SignalConversion generated from: '<Root>/pck_rx_struct' incorporates:
    //   MATLAB Function: '<S1>/RAW2STRUCT Decoding Logic'

    *rty_pck_rx_struct_packets_ID_DS = static_cast<uint8_T>
      (*rtu_pck_rx_raw_packets_ID & 15);

    // SignalConversion generated from: '<Root>/pck_rx_struct' incorporates:
    //   MATLAB Function: '<S1>/RAW2STRUCT Decoding Logic'

    *rty_pck_rx_struct_packets_PAYLO = rtb_BusConversion_InsertedFor_0;

    // SignalConversion generated from: '<Root>/pck_rx_struct' incorporates:
    //   BusCreator generated from: '<S1>/RAW2STRUCT Decoding Logic'
    //   MATLAB Function: '<S1>/RAW2STRUCT Decoding Logic'

    *rty_pck_rx_struct_packets_PAY_f = ((rtu_pck_rx_raw_packets_PAYLOAD[0] &
      128U) != 0U);

    // SignalConversion generated from: '<Root>/pck_rx_struct' incorporates:
    //   BusCreator generated from: '<S1>/RAW2STRUCT Decoding Logic'
    //   MATLAB Function: '<S1>/RAW2STRUCT Decoding Logic'

    *rty_pck_rx_struct_packets_PAY_k = static_cast<uint8_T>
      (rtu_pck_rx_raw_packets_PAYLOAD[0] & 127);

    // End of Outputs for SubSystem: '<Root>/CAN_RX_RAW2STRUCT'
  }

  // Constructor
  CAN_RX_raw2struct::CAN_RX_raw2struct()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  CAN_RX_raw2struct::~CAN_RX_raw2struct()
  {
    // Currently there is no destructor body generated.
  }

  // Real-Time Model get method
  can_messaging::CAN_RX_raw2struct::RT_MODEL_can_rx_raw2struct_T
    * CAN_RX_raw2struct::getRTM()
  {
    return (&can_rx_raw2struct_M);
  }

  // member function to setup error status pointer
  void CAN_RX_raw2struct::setErrorStatusPointer(const char_T **rt_errorStatus)
  {
    rtmSetErrorStatusPointer((&can_rx_raw2struct_M), rt_errorStatus);
  }
}

//
// File trailer for generated code.
//
// [EOF]
//
