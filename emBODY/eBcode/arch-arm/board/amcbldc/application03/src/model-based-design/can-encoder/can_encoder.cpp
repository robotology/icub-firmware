//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: can_encoder.cpp
//
// Code generated for Simulink model 'can_encoder'.
//
// Model version                  : 1.327
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Tue Sep 21 16:48:15 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "can_encoder.h"
#include "can_encoder_private.h"

namespace can_messaging
{
  // Output and update for referenced model: 'can_encoder'
  void CAN_Encoder::step(const BUS_MESSAGES_TX &arg_messages_tx, const
    BUS_EVENTS_TX &arg_events_tx, BUS_CAN &arg_pck_tx)
  {
    int32_T rtb_DataTypeConversion2;
    real32_T u;
    int16_T rtb_DataTypeConversion1;
    int16_T rtb_DataTypeConversion3;

    // Outputs for Atomic SubSystem: '<Root>/CAN_Encoder'
    // DataTypeConversion: '<S1>/Data Type Conversion1'
    if (arg_messages_tx.foc.current < 0.0F) {
      u = std::ceil(arg_messages_tx.foc.current);
    } else {
      u = std::floor(arg_messages_tx.foc.current);
    }

    if (rtIsNaNF(u) || rtIsInfF(u)) {
      u = 0.0F;
    } else {
      u = std::fmod(u, 65536.0F);
    }

    rtb_DataTypeConversion1 = static_cast<int16_T>(u < 0.0F ? static_cast<
      int32_T>(static_cast<int16_T>(-static_cast<int16_T>(static_cast<uint16_T>(
      -u)))) : static_cast<int32_T>(static_cast<int16_T>(static_cast<uint16_T>(u))));

    // End of DataTypeConversion: '<S1>/Data Type Conversion1'

    // Gain: '<S1>/Gain'
    u = rtP_CAN_ANGLE_DEG2ICUB * arg_messages_tx.foc.velocity;

    // DataTypeConversion: '<S1>/Data Type Conversion3'
    if (u < 0.0F) {
      u = std::ceil(u);
    } else {
      u = std::floor(u);
    }

    if (rtIsNaNF(u) || rtIsInfF(u)) {
      u = 0.0F;
    } else {
      u = std::fmod(u, 65536.0F);
    }

    rtb_DataTypeConversion3 = static_cast<int16_T>(u < 0.0F ?
      static_cast<int32_T>(static_cast<int16_T>(-static_cast<int16_T>(
      static_cast<uint16_T>(-u)))) : static_cast<int32_T>(static_cast<int16_T>(
      static_cast<uint16_T>(u))));

    // End of DataTypeConversion: '<S1>/Data Type Conversion3'

    // Gain: '<S1>/Gain1'
    u = rtP_CAN_ANGLE_DEG2ICUB * arg_messages_tx.foc.position;

    // DataTypeConversion: '<S1>/Data Type Conversion2'
    if (u < 0.0F) {
      u = std::ceil(u);
    } else {
      u = std::floor(u);
    }

    if (rtIsNaNF(u) || rtIsInfF(u)) {
      u = 0.0F;
    } else {
      u = std::fmod(u, 4.2949673E+9F);
    }

    rtb_DataTypeConversion2 = u < 0.0F ? -static_cast<int32_T>
      (static_cast<uint32_T>(-u)) : static_cast<int32_T>(static_cast<uint32_T>(u));

    // End of DataTypeConversion: '<S1>/Data Type Conversion2'

    // MATLAB Function: '<S1>/MATLAB Function'
    arg_pck_tx.packets.PAYLOAD[0] = static_cast<uint8_T>(rtb_DataTypeConversion1
      & 255);
    arg_pck_tx.packets.PAYLOAD[1] = static_cast<uint8_T>
      ((rtb_DataTypeConversion1 & 32767) >> 8);
    arg_pck_tx.packets.PAYLOAD[2] = static_cast<uint8_T>(rtb_DataTypeConversion3
      & 255);
    arg_pck_tx.packets.PAYLOAD[3] = static_cast<uint8_T>
      ((rtb_DataTypeConversion3 & 32767) >> 8);
    arg_pck_tx.packets.PAYLOAD[4] = static_cast<uint8_T>(rtb_DataTypeConversion2
      & 255);
    arg_pck_tx.packets.PAYLOAD[5] = static_cast<uint8_T>
      ((rtb_DataTypeConversion2 & 65280) >> 8);
    arg_pck_tx.packets.PAYLOAD[6] = static_cast<uint8_T>
      ((rtb_DataTypeConversion2 & 16711680) >> 16);
    arg_pck_tx.packets.PAYLOAD[7] = static_cast<uint8_T>
      ((rtb_DataTypeConversion2 & MAX_int32_T) >> 24);

    // MATLAB Function: '<S1>/format_can_id' incorporates:
    //   Constant: '<S1>/Constant1'
    //   Constant: '<S1>/Motor Control Streaming'
    //   Constant: '<S1>/TYPE2FOC'

    arg_pck_tx.packets.ID = 256U;
    arg_pck_tx.packets.ID = static_cast<uint16_T>(rtP_CAN_ID_AMC << 4 |
      arg_pck_tx.packets.ID);
    arg_pck_tx.packets.ID = static_cast<uint16_T>(arg_pck_tx.packets.ID | 15);

    // DataTypeConversion: '<S1>/Data Type Conversion' incorporates:
    //   RelationalOperator: '<S2>/FixPt Relational Operator'
    //   UnitDelay: '<S2>/Delay Input1'
    //
    //  Block description for '<S2>/Delay Input1':
    //
    //   Store in Global RAM

    arg_pck_tx.available = (arg_events_tx.foc !=
      can_encoder_DW.DelayInput1_DSTATE);

    // BusCreator: '<S1>/Bus Creator' incorporates:
    //   Constant: '<S1>/lengths'

    arg_pck_tx.lengths = 8U;

    // Update for UnitDelay: '<S2>/Delay Input1'
    //
    //  Block description for '<S2>/Delay Input1':
    //
    //   Store in Global RAM

    can_encoder_DW.DelayInput1_DSTATE = arg_events_tx.foc;

    // End of Outputs for SubSystem: '<Root>/CAN_Encoder'
  }

  // Model initialize function
  void CAN_Encoder::initialize()
  {
    // Registration code

    // initialize non-finites
    rt_InitInfAndNaN(sizeof(real_T));
  }

  // Constructor
  CAN_Encoder::CAN_Encoder() :
    can_encoder_DW()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  CAN_Encoder::~CAN_Encoder()
  {
    // Currently there is no destructor body generated.
  }

  // Real-Time Model get method
  can_messaging::CAN_Encoder::RT_MODEL_can_encoder_T * CAN_Encoder::getRTM()
  {
    return (&can_encoder_M);
  }

  // member function to setup error status pointer
  void CAN_Encoder::setErrorStatusPointer(const char_T **rt_errorStatus)
  {
    rtmSetErrorStatusPointer((&can_encoder_M), rt_errorStatus);
  }
}

//
// File trailer for generated code.
//
// [EOF]
//
