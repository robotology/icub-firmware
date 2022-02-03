//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_encoder.cpp
//
// Code generated for Simulink model 'can_encoder'.
//
// Model version                  : 2.49
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Mon Jan 31 18:31:57 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "can_encoder.h"
#include "can_encoder_private.h"

namespace amc_bldc_codegen
{
  //
  // Output and update for atomic system:
  //    '<S5>/format_can_id'
  //    '<S8>/format_can_id'
  //
  void CAN_Encoder::can_encoder_format_can_id(uint8_T rtu_class, uint8_T
    rtu_can_id_amc, uint8_T rtu_dst_typ, uint16_T *rty_pkt_id)
  {
    *rty_pkt_id = static_cast<uint16_T>(rtu_class << 8);
    *rty_pkt_id = static_cast<uint16_T>(rtu_can_id_amc << 4 | *rty_pkt_id);
    *rty_pkt_id = static_cast<uint16_T>(rtu_dst_typ | *rty_pkt_id);
  }
}

namespace amc_bldc_codegen
{
  // Output and update for referenced model: 'can_encoder'
  void CAN_Encoder::step(const BUS_MESSAGES_TX &arg_messages_tx, const
    BUS_EVENTS_TX &arg_events_tx, BUS_CAN_MULTIPLE &arg_pck_tx)
  {
    BUS_CAN rtb_BusCreator_d;
    BUS_CAN rtb_BusCreator_j;
    int32_T rtb_DataTypeConversion2;
    real32_T u;
    int16_T rtb_DataTypeConversion3;
    int16_T rtb_DataTypeConversion3_e;
    uint16_T rtb_pkt_id;

    // Outputs for Atomic SubSystem: '<Root>/CAN_Encoder'
    // DataTypeConversion: '<S4>/Data Type Conversion3'
    if (arg_messages_tx.status.pwm_fbk < 0.0F) {
      u = std::ceil(arg_messages_tx.status.pwm_fbk);
    } else {
      u = std::floor(arg_messages_tx.status.pwm_fbk);
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

    // End of DataTypeConversion: '<S4>/Data Type Conversion3'

    // MATLAB Function: '<S4>/format_status_pck' incorporates:
    //   BusCreator: '<S4>/Bus Creator'
    //   Concatenate: '<S1>/Vector Concatenate'

    arg_pck_tx.packets[0].packet.PAYLOAD[0] = static_cast<uint8_T>
      (arg_messages_tx.status.control_mode);
    arg_pck_tx.packets[0].packet.PAYLOAD[1] = 0U;
    arg_pck_tx.packets[0].packet.PAYLOAD[2] = static_cast<uint8_T>
      (rtb_DataTypeConversion3 & 255);
    arg_pck_tx.packets[0].packet.PAYLOAD[3] = static_cast<uint8_T>
      ((rtb_DataTypeConversion3 & 32767) >> 8);
    arg_pck_tx.packets[0].packet.PAYLOAD[4] =
      arg_messages_tx.status.flags.ExternalFaultAsserted;
    arg_pck_tx.packets[0].packet.PAYLOAD[5] = 0U;
    arg_pck_tx.packets[0].packet.PAYLOAD[6] = 0U;
    arg_pck_tx.packets[0].packet.PAYLOAD[7] = 0U;

    // Gain: '<S2>/Gain2'
    u = 1000.0F * arg_messages_tx.foc.current;

    // DataTypeConversion: '<S2>/Data Type Conversion1'
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

    // End of DataTypeConversion: '<S2>/Data Type Conversion1'

    // Gain: '<S2>/Gain'
    u = CAN_ANGLE_DEG2ICUB * arg_messages_tx.foc.velocity;

    // DataTypeConversion: '<S2>/Data Type Conversion3'
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

    rtb_DataTypeConversion3_e = static_cast<int16_T>(u < 0.0F ?
      static_cast<int32_T>(static_cast<int16_T>(-static_cast<int16_T>(
      static_cast<uint16_T>(-u)))) : static_cast<int32_T>(static_cast<int16_T>(
      static_cast<uint16_T>(u))));

    // End of DataTypeConversion: '<S2>/Data Type Conversion3'

    // Gain: '<S2>/Gain1'
    u = CAN_ANGLE_DEG2ICUB * arg_messages_tx.foc.position;

    // DataTypeConversion: '<S2>/Data Type Conversion2'
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

    // End of DataTypeConversion: '<S2>/Data Type Conversion2'

    // MATLAB Function: '<S2>/format_foc_pck' incorporates:
    //   BusCreator: '<S2>/Bus Creator'

    rtb_BusCreator_d.packet.PAYLOAD[0] = static_cast<uint8_T>
      (rtb_DataTypeConversion3 & 255);
    rtb_BusCreator_d.packet.PAYLOAD[1] = static_cast<uint8_T>
      ((rtb_DataTypeConversion3 & 32767) >> 8);
    rtb_BusCreator_d.packet.PAYLOAD[2] = static_cast<uint8_T>
      (rtb_DataTypeConversion3_e & 255);
    rtb_BusCreator_d.packet.PAYLOAD[3] = static_cast<uint8_T>
      ((rtb_DataTypeConversion3_e & 32767) >> 8);
    rtb_BusCreator_d.packet.PAYLOAD[4] = static_cast<uint8_T>
      (rtb_DataTypeConversion2 & 255);
    rtb_BusCreator_d.packet.PAYLOAD[5] = static_cast<uint8_T>
      ((rtb_DataTypeConversion2 & 65280) >> 8);
    rtb_BusCreator_d.packet.PAYLOAD[6] = static_cast<uint8_T>
      ((rtb_DataTypeConversion2 & 16711680) >> 16);
    rtb_BusCreator_d.packet.PAYLOAD[7] = static_cast<uint8_T>
      ((rtb_DataTypeConversion2 & MAX_int32_T) >> 24);

    // MATLAB Function: '<S8>/format_can_id' incorporates:
    //   Constant: '<S4>/Constant'
    //   Constant: '<S4>/Motor Control Streaming'
    //   Constant: '<S4>/TYPESTATUS'

    can_encoder_format_can_id(1, CAN_ID_AMC, 3, &rtb_pkt_id);

    // BusCreator: '<S4>/Bus Creator' incorporates:
    //   BusCreator: '<S4>/Bus Creator1'
    //   Concatenate: '<S1>/Vector Concatenate'
    //   Constant: '<S4>/length'

    arg_pck_tx.packets[0].available = arg_events_tx.status;
    arg_pck_tx.packets[0].length = 8U;
    arg_pck_tx.packets[0].packet.ID = rtb_pkt_id;

    // MATLAB Function: '<S5>/format_can_id' incorporates:
    //   Constant: '<S2>/Constant'
    //   Constant: '<S2>/Motor Control Streaming'
    //   Constant: '<S2>/TYPE2FOC'

    can_encoder_format_can_id(1, CAN_ID_AMC, 0, &rtb_pkt_id);

    // BusCreator: '<S2>/Bus Creator' incorporates:
    //   BusCreator: '<S2>/Bus Creator1'
    //   Constant: '<S2>/length'

    rtb_BusCreator_d.available = arg_events_tx.foc;
    rtb_BusCreator_d.length = 8U;
    rtb_BusCreator_d.packet.ID = rtb_pkt_id;

    // BusCreator: '<S3>/Bus Creator' incorporates:
    //   BusCreator: '<S3>/Bus Creator6'
    //   Constant: '<S3>/ID_raw'
    //   Constant: '<S3>/available'
    //   Constant: '<S3>/length'

    rtb_BusCreator_j.available = false;
    rtb_BusCreator_j.length = 0U;
    rtb_BusCreator_j.packet.ID = 0U;
    for (rtb_DataTypeConversion2 = 0; rtb_DataTypeConversion2 < 8;
         rtb_DataTypeConversion2++) {
      rtb_BusCreator_j.packet.PAYLOAD[rtb_DataTypeConversion2] = 0U;
    }

    // End of BusCreator: '<S3>/Bus Creator'

    // Concatenate: '<S1>/Vector Concatenate'
    arg_pck_tx.packets[1] = rtb_BusCreator_d;
    arg_pck_tx.packets[2] = rtb_BusCreator_j;
    arg_pck_tx.packets[3] = rtb_BusCreator_j;

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
    can_encoder_M()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  CAN_Encoder::~CAN_Encoder()
  {
    // Currently there is no destructor body generated.
  }

  // Real-Time Model get method
  amc_bldc_codegen::CAN_Encoder::RT_MODEL_can_encoder_T * CAN_Encoder::getRTM()
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
