//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_encoder.cpp
//
// Code generated for Simulink model 'can_encoder'.
//
// Model version                  : 2.53
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Thu Feb 17 15:05:37 2022
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
    int32_T i;
    real32_T tmp_0;
    int16_T rtb_DataTypeConversion1_0;
    int16_T rtb_DataTypeConversion_0;
    uint16_T rtb_pkt_id;
    uint8_T b_tmp[4];
    uint8_T tmp[2];

    // Outputs for Atomic SubSystem: '<Root>/CAN_Encoder'
    // DataTypeConversion: '<S4>/Data Type Conversion'
    if (arg_messages_tx.status.pwm_fbk < 32768.0F) {
      if (arg_messages_tx.status.pwm_fbk >= -32768.0F) {
        rtb_DataTypeConversion_0 = static_cast<int16_T>
          (arg_messages_tx.status.pwm_fbk);
      } else {
        rtb_DataTypeConversion_0 = MIN_int16_T;
      }
    } else {
      rtb_DataTypeConversion_0 = MAX_int16_T;
    }

    // MATLAB Function: '<S4>/format_status_pck' incorporates:
    //   BusCreator: '<S4>/Bus Creator'
    //   Concatenate: '<S1>/Vector Concatenate'
    //   DataTypeConversion: '<S4>/Data Type Conversion'

    arg_pck_tx.packets[0].packet.PAYLOAD[0] = static_cast<uint8_T>
      (arg_messages_tx.status.control_mode);
    arg_pck_tx.packets[0].packet.PAYLOAD[1] = 0U;
    std::memcpy((void *)&tmp[0], (void *)&rtb_DataTypeConversion_0, (uint32_T)
                ((size_t)2 * sizeof(uint8_T)));
    arg_pck_tx.packets[0].packet.PAYLOAD[2] = tmp[0];
    arg_pck_tx.packets[0].packet.PAYLOAD[3] = tmp[1];
    arg_pck_tx.packets[0].packet.PAYLOAD[4] =
      arg_messages_tx.status.flags.ExternalFaultAsserted;
    arg_pck_tx.packets[0].packet.PAYLOAD[5] = 0U;
    arg_pck_tx.packets[0].packet.PAYLOAD[6] = 0U;
    arg_pck_tx.packets[0].packet.PAYLOAD[7] = 0U;

    // DataTypeConversion: '<S2>/Data Type Conversion' incorporates:
    //   Gain: '<S2>/Gain2'

    tmp_0 = 1000.0F * arg_messages_tx.foc.current;
    if (tmp_0 < 32768.0F) {
      if (tmp_0 >= -32768.0F) {
        rtb_DataTypeConversion_0 = static_cast<int16_T>(tmp_0);
      } else {
        rtb_DataTypeConversion_0 = MIN_int16_T;
      }
    } else {
      rtb_DataTypeConversion_0 = MAX_int16_T;
    }

    // DataTypeConversion: '<S2>/Data Type Conversion1' incorporates:
    //   Gain: '<S2>/Gain'

    tmp_0 = CAN_ANGLE_DEG2ICUB * arg_messages_tx.foc.velocity;
    if (tmp_0 < 32768.0F) {
      if (tmp_0 >= -32768.0F) {
        rtb_DataTypeConversion1_0 = static_cast<int16_T>(tmp_0);
      } else {
        rtb_DataTypeConversion1_0 = MIN_int16_T;
      }
    } else {
      rtb_DataTypeConversion1_0 = MAX_int16_T;
    }

    // DataTypeConversion: '<S2>/Data Type Conversion2' incorporates:
    //   Gain: '<S2>/Gain1'

    tmp_0 = CAN_ANGLE_DEG2ICUB * arg_messages_tx.foc.position;
    if (tmp_0 < 2.14748365E+9F) {
      if (tmp_0 >= -2.14748365E+9F) {
        i = static_cast<int32_T>(tmp_0);
      } else {
        i = MIN_int32_T;
      }
    } else {
      i = MAX_int32_T;
    }

    // MATLAB Function: '<S2>/format_foc_pck' incorporates:
    //   BusCreator: '<S2>/Bus Creator'
    //   DataTypeConversion: '<S2>/Data Type Conversion'
    //   DataTypeConversion: '<S2>/Data Type Conversion1'
    //   DataTypeConversion: '<S2>/Data Type Conversion2'

    std::memcpy((void *)&tmp[0], (void *)&rtb_DataTypeConversion_0, (uint32_T)
                ((size_t)2 * sizeof(uint8_T)));
    rtb_BusCreator_d.packet.PAYLOAD[0] = tmp[0];
    rtb_BusCreator_d.packet.PAYLOAD[1] = tmp[1];
    std::memcpy((void *)&tmp[0], (void *)&rtb_DataTypeConversion1_0, (uint32_T)
                ((size_t)2 * sizeof(uint8_T)));
    rtb_BusCreator_d.packet.PAYLOAD[2] = tmp[0];
    rtb_BusCreator_d.packet.PAYLOAD[3] = tmp[1];
    std::memcpy((void *)&b_tmp[0], (void *)&i, (uint32_T)((size_t)4 * sizeof
      (uint8_T)));
    rtb_BusCreator_d.packet.PAYLOAD[4] = b_tmp[0];
    rtb_BusCreator_d.packet.PAYLOAD[5] = b_tmp[1];
    rtb_BusCreator_d.packet.PAYLOAD[6] = b_tmp[2];
    rtb_BusCreator_d.packet.PAYLOAD[7] = b_tmp[3];

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
    for (i = 0; i < 8; i++) {
      rtb_BusCreator_j.packet.PAYLOAD[i] = 0U;
    }

    // End of BusCreator: '<S3>/Bus Creator'

    // Concatenate: '<S1>/Vector Concatenate'
    arg_pck_tx.packets[1] = rtb_BusCreator_d;
    arg_pck_tx.packets[2] = rtb_BusCreator_j;
    arg_pck_tx.packets[3] = rtb_BusCreator_j;

    // End of Outputs for SubSystem: '<Root>/CAN_Encoder'
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
