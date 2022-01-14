//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_rx_raw2struct.cpp
//
// Code generated for Simulink model 'can_rx_raw2struct'.
//
// Model version                  : 2.7
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Fri Jan 14 15:25:44 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "can_rx_raw2struct.h"
#include "can_rx_raw2struct_private.h"

namespace amc_bldc_codegen
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

namespace amc_bldc_codegen
{
  // Output and update for referenced model: 'can_rx_raw2struct'
  void CAN_RX_raw2struct::step(const BUS_CAN &arg_pck_rx_raw, BUS_CAN_RX &
    arg_pck_rx_struct)
  {
    uint8_T minval;

    // Outputs for Atomic SubSystem: '<Root>/CAN_RX_RAW2STRUCT'
    // MATLAB Function: '<S1>/RAW2STRUCT Decoding Logic'
    arg_pck_rx_struct.available = arg_pck_rx_raw.available;
    arg_pck_rx_struct.packets.ID.CLS = c_convert_to_enum_CANClassTypes(
      static_cast<int32_T>(static_cast<uint16_T>(static_cast<uint32_T>
      (arg_pck_rx_raw.packets.ID & 1792) >> 8)));
    arg_pck_rx_struct.packets.ID.SRC = static_cast<uint8_T>(static_cast<uint32_T>
      (arg_pck_rx_raw.packets.ID & 240) >> 4);
    arg_pck_rx_struct.packets.ID.DST_TYP = static_cast<uint8_T>
      (arg_pck_rx_raw.packets.ID & 15);
    minval = 8U;
    if (8 > arg_pck_rx_raw.lengths) {
      minval = arg_pck_rx_raw.lengths;
    }

    arg_pck_rx_struct.packets.PAYLOAD.LEN = 0U;
    if (0 < minval) {
      arg_pck_rx_struct.packets.PAYLOAD.LEN = minval;
    }

    arg_pck_rx_struct.packets.PAYLOAD.CMD.M = ((arg_pck_rx_raw.packets.PAYLOAD[0]
      & 128U) != 0U);
    arg_pck_rx_struct.packets.PAYLOAD.CMD.OPC = static_cast<uint8_T>
      (arg_pck_rx_raw.packets.PAYLOAD[0] & 127);
    for (int32_T i = 0; i < 8; i++) {
      arg_pck_rx_struct.packets.PAYLOAD.ARG[i] =
        arg_pck_rx_raw.packets.PAYLOAD[i];
    }

    // End of MATLAB Function: '<S1>/RAW2STRUCT Decoding Logic'
    // End of Outputs for SubSystem: '<Root>/CAN_RX_RAW2STRUCT'
  }

  // Constructor
  CAN_RX_raw2struct::CAN_RX_raw2struct() :
    can_rx_raw2struct_M()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  CAN_RX_raw2struct::~CAN_RX_raw2struct()
  {
    // Currently there is no destructor body generated.
  }

  // Real-Time Model get method
  amc_bldc_codegen::CAN_RX_raw2struct::RT_MODEL_can_rx_raw2struct_T
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
