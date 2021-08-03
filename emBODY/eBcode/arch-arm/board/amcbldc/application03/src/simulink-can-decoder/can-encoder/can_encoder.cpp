//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: can_encoder.cpp
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
#include "can_encoder.h"
#include "can_encoder_private.h"

// Model block global parameters (default storage)
real32_T rtP_CAN_ANGLE_DEG2ICUB = 182.044449F;

uint16_T rtP_CAN_ID_HOST = 0U;

namespace can_messaging
{
  // Output and update for referenced model: 'can_encoder'
  void CAN_Encoder::step(const real32_T *rtu_messages_tx_foc_current, const
    real32_T *rtu_messages_tx_foc_position, const real32_T
    *rtu_messages_tx_foc_velocity, const boolean_T *rtu_events_tx_foc, uint8_T
    *rty_pck_tx_available, uint16_T *rty_pck_tx_packets_ID, uint8_T
    rty_pck_tx_packets_PAYLOAD[8])
  {
    int32_T rtb_DataTypeConversion2;
    real32_T rtb_Gain;
    int16_T rtb_DataTypeConversion1;
    int16_T rtb_DataTypeConversion3;

    // Outputs for Atomic SubSystem: '<Root>/CAN_Encoder'
    // DataTypeConversion: '<S1>/Data Type Conversion1'
    rtb_Gain = *rtu_messages_tx_foc_current;
    if (rtb_Gain < 0.0F) {
      rtb_Gain = std::ceil(rtb_Gain);
    } else {
      rtb_Gain = std::floor(rtb_Gain);
    }

    if (rtIsNaNF(rtb_Gain) || rtIsInfF(rtb_Gain)) {
      rtb_Gain = 0.0F;
    } else {
      rtb_Gain = std::fmod(rtb_Gain, 65536.0F);
    }

    rtb_DataTypeConversion1 = static_cast<int16_T>(rtb_Gain < 0.0F ?
      static_cast<int32_T>(static_cast<int16_T>(-static_cast<int16_T>(
      static_cast<uint16_T>(-rtb_Gain)))) : static_cast<int32_T>
      (static_cast<int16_T>(static_cast<uint16_T>(rtb_Gain))));

    // End of DataTypeConversion: '<S1>/Data Type Conversion1'

    // Gain: '<S1>/Gain1'
    rtb_Gain = rtP_CAN_ANGLE_DEG2ICUB * *rtu_messages_tx_foc_position;

    // DataTypeConversion: '<S1>/Data Type Conversion2'
    if (rtb_Gain < 0.0F) {
      rtb_Gain = std::ceil(rtb_Gain);
    } else {
      rtb_Gain = std::floor(rtb_Gain);
    }

    if (rtIsNaNF(rtb_Gain) || rtIsInfF(rtb_Gain)) {
      rtb_Gain = 0.0F;
    } else {
      rtb_Gain = std::fmod(rtb_Gain, 4.2949673E+9F);
    }

    rtb_DataTypeConversion2 = rtb_Gain < 0.0F ? -static_cast<int32_T>(
      static_cast<uint32_T>(-rtb_Gain)) : static_cast<int32_T>
      (static_cast<uint32_T>(rtb_Gain));

    // End of DataTypeConversion: '<S1>/Data Type Conversion2'

    // Gain: '<S1>/Gain'
    rtb_Gain = rtP_CAN_ANGLE_DEG2ICUB * *rtu_messages_tx_foc_velocity;

    // DataTypeConversion: '<S1>/Data Type Conversion3'
    if (rtb_Gain < 0.0F) {
      rtb_Gain = std::ceil(rtb_Gain);
    } else {
      rtb_Gain = std::floor(rtb_Gain);
    }

    if (rtIsNaNF(rtb_Gain) || rtIsInfF(rtb_Gain)) {
      rtb_Gain = 0.0F;
    } else {
      rtb_Gain = std::fmod(rtb_Gain, 65536.0F);
    }

    rtb_DataTypeConversion3 = static_cast<int16_T>(rtb_Gain < 0.0F ?
      static_cast<int32_T>(static_cast<int16_T>(-static_cast<int16_T>(
      static_cast<uint16_T>(-rtb_Gain)))) : static_cast<int32_T>
      (static_cast<int16_T>(static_cast<uint16_T>(rtb_Gain))));

    // End of DataTypeConversion: '<S1>/Data Type Conversion3'

    // MATLAB Function: '<S1>/MATLAB Function'
    rty_pck_tx_packets_PAYLOAD[0] = static_cast<uint8_T>(rtb_DataTypeConversion1
      & 255);
    rty_pck_tx_packets_PAYLOAD[1] = static_cast<uint8_T>
      ((rtb_DataTypeConversion1 & 32767) >> 8);
    rty_pck_tx_packets_PAYLOAD[2] = static_cast<uint8_T>(rtb_DataTypeConversion3
      & 255);
    rty_pck_tx_packets_PAYLOAD[3] = static_cast<uint8_T>
      ((rtb_DataTypeConversion3 & 32767) >> 8);
    rty_pck_tx_packets_PAYLOAD[4] = static_cast<uint8_T>(rtb_DataTypeConversion2
      & 255);
    rty_pck_tx_packets_PAYLOAD[5] = static_cast<uint8_T>
      ((rtb_DataTypeConversion2 & 65280) >> 8);
    rty_pck_tx_packets_PAYLOAD[6] = static_cast<uint8_T>
      ((rtb_DataTypeConversion2 & 16711680) >> 16);
    rty_pck_tx_packets_PAYLOAD[7] = static_cast<uint8_T>
      ((rtb_DataTypeConversion2 & MAX_int32_T) >> 24);

    // Constant: '<S1>/Constant'
    *rty_pck_tx_packets_ID = rtP_CAN_ID_HOST;

    // DataTypeConversion: '<S1>/Data Type Conversion'
    *rty_pck_tx_available = *rtu_events_tx_foc;

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
  CAN_Encoder::CAN_Encoder()
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
