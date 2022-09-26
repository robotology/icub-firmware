//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_encoder.cpp
//
// Code generated for Simulink model 'can_encoder'.
//
// Model version                  : 4.0
// Simulink Coder version         : 9.8 (R2022b) 13-May-2022
// C/C++ source code generated on : Mon Sep 26 16:37:43 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "can_encoder.h"
#include "rtwtypes.h"
#include "can_encoder_types.h"
#include <cstring>
#include <stddef.h>
#include "can_encoder_private.h"
#include "rtw_defines.h"

MdlrefDW_can_encoder_T can_encoder_MdlrefDW;

//
// Output and update for atomic system:
//    '<S5>/format_can_id'
//    '<S8>/format_can_id'
//
void can_encoder_format_can_id(uint8_T rtu_class, uint8_T rtu_can_id_amc,
  uint8_T rtu_dst_typ, uint16_T *rty_pkt_id)
{
  *rty_pkt_id = static_cast<uint16_T>(rtu_class << 8);
  *rty_pkt_id = static_cast<uint16_T>(rtu_can_id_amc << 4 | *rty_pkt_id);
  *rty_pkt_id = static_cast<uint16_T>(rtu_dst_typ | *rty_pkt_id);
}

// Output and update for referenced model: 'can_encoder'
void can_encoder(const BUS_MESSAGES_TX *rtu_messages_tx, const BUS_STATUS_TX
                 *rtu_status_tx, const ConfigurationParameters
                 *rtu_ConfigurationParameters, BUS_CAN_MULTIPLE *rty_pck_tx)
{
  BUS_CAN rtb_BusCreator_n_0[4];
  BUS_CAN rtb_BusCreator_hs;
  BUS_CAN rtb_BusCreator_m;
  BUS_CAN rtb_BusCreator_n;
  int32_T i;
  real32_T tmp;
  uint32_T qY;
  int16_T rtb_DataTypeConversion1_0;
  int16_T rtb_DataTypeConversion_k;
  uint8_T b_tmp[4];
  uint8_T tmp2[2];

  // Outputs for Atomic SubSystem: '<Root>/CAN_Encoder'
  // MATLAB Function: '<S4>/format_status_pck' incorporates:
  //   BusCreator: '<S4>/Bus Creator'

  rtb_BusCreator_m.packet.PAYLOAD[0] = static_cast<uint8_T>
    (rtu_messages_tx->status.control_mode);
  rtb_BusCreator_m.packet.PAYLOAD[1] = 0U;
  qY = 5U - rtu_ConfigurationParameters->CurLoopPID.shift_factor;
  if (5U - rtu_ConfigurationParameters->CurLoopPID.shift_factor > 5U) {
    qY = 0U;
  }

  // DataTypeConversion: '<S4>/Data Type Conversion' incorporates:
  //   Constant: '<S4>/length1'
  //   Product: '<S4>/Product'

  tmp = rtu_messages_tx->status.pwm_fbk * 10.0F;
  if (tmp < 32768.0F) {
    if (tmp >= -32768.0F) {
      rtb_DataTypeConversion_k = static_cast<int16_T>(tmp);
    } else {
      rtb_DataTypeConversion_k = MIN_int16_T;
    }
  } else {
    rtb_DataTypeConversion_k = MAX_int16_T;
  }

  // MATLAB Function: '<S4>/format_status_pck' incorporates:
  //   BusCreator: '<S4>/Bus Creator'
  //   DataTypeConversion: '<S4>/Data Type Conversion'

  rtb_DataTypeConversion_k = static_cast<int16_T>(rtb_DataTypeConversion_k <<
    static_cast<uint8_T>(qY));
  std::memcpy((void *)&tmp2[0], (void *)&rtb_DataTypeConversion_k, (size_t)2 *
              sizeof(uint8_T));
  rtb_BusCreator_m.packet.PAYLOAD[2] = tmp2[0];
  rtb_BusCreator_m.packet.PAYLOAD[3] = tmp2[1];
  rtb_BusCreator_m.packet.PAYLOAD[4] =
    rtu_messages_tx->status.flags.ExternalFaultAsserted;
  rtb_BusCreator_m.packet.PAYLOAD[5] = 0U;
  rtb_BusCreator_m.packet.PAYLOAD[6] = 0U;
  rtb_BusCreator_m.packet.PAYLOAD[7] = 0U;

  // DataTypeConversion: '<S2>/Data Type Conversion' incorporates:
  //   Gain: '<S2>/Gain2'

  tmp = 1000.0F * rtu_messages_tx->foc.current;
  if (tmp < 32768.0F) {
    if (tmp >= -32768.0F) {
      rtb_DataTypeConversion_k = static_cast<int16_T>(tmp);
    } else {
      rtb_DataTypeConversion_k = MIN_int16_T;
    }
  } else {
    rtb_DataTypeConversion_k = MAX_int16_T;
  }

  // DataTypeConversion: '<S2>/Data Type Conversion1' incorporates:
  //   Gain: '<S2>/Gain1'

  tmp = CAN_ANGLE_DEG2ICUB / 1000.0F * rtu_messages_tx->foc.velocity;
  if (tmp < 32768.0F) {
    if (tmp >= -32768.0F) {
      rtb_DataTypeConversion1_0 = static_cast<int16_T>(tmp);
    } else {
      rtb_DataTypeConversion1_0 = MIN_int16_T;
    }
  } else {
    rtb_DataTypeConversion1_0 = MAX_int16_T;
  }

  // DataTypeConversion: '<S2>/Data Type Conversion2' incorporates:
  //   Gain: '<S2>/Gain3'

  tmp = CAN_ANGLE_DEG2ICUB * rtu_messages_tx->foc.position;
  if (tmp < 2.14748365E+9F) {
    if (tmp >= -2.14748365E+9F) {
      i = static_cast<int32_T>(tmp);
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

  std::memcpy((void *)&tmp2[0], (void *)&rtb_DataTypeConversion_k, (size_t)2 *
              sizeof(uint8_T));
  rtb_BusCreator_n.packet.PAYLOAD[0] = tmp2[0];
  rtb_BusCreator_n.packet.PAYLOAD[1] = tmp2[1];
  std::memcpy((void *)&tmp2[0], (void *)&rtb_DataTypeConversion1_0, (size_t)2 *
              sizeof(uint8_T));
  rtb_BusCreator_n.packet.PAYLOAD[2] = tmp2[0];
  rtb_BusCreator_n.packet.PAYLOAD[3] = tmp2[1];
  std::memcpy((void *)&b_tmp[0], (void *)&i, (size_t)4 * sizeof(uint8_T));
  rtb_BusCreator_n.packet.PAYLOAD[4] = b_tmp[0];
  rtb_BusCreator_n.packet.PAYLOAD[5] = b_tmp[1];
  rtb_BusCreator_n.packet.PAYLOAD[6] = b_tmp[2];
  rtb_BusCreator_n.packet.PAYLOAD[7] = b_tmp[3];

  // BusCreator: '<S4>/Bus Creator' incorporates:
  //   Constant: '<S4>/Constant'
  //   Constant: '<S4>/Motor Control Streaming'
  //   Constant: '<S4>/TYPESTATUS'
  //   Constant: '<S4>/length'
  //   MATLAB Function: '<S8>/format_can_id'

  can_encoder_format_can_id(1, CAN_ID_AMC, 3, &rtb_BusCreator_m.packet.ID);
  rtb_BusCreator_m.available = rtu_status_tx->status;
  rtb_BusCreator_m.length = 8U;

  // BusCreator: '<S2>/Bus Creator' incorporates:
  //   Constant: '<S2>/Constant'
  //   Constant: '<S2>/Motor Control Streaming'
  //   Constant: '<S2>/TYPE2FOC'
  //   Constant: '<S2>/length'
  //   MATLAB Function: '<S5>/format_can_id'

  can_encoder_format_can_id(1, CAN_ID_AMC, 0, &rtb_BusCreator_n.packet.ID);
  rtb_BusCreator_n.available = rtu_status_tx->foc;
  rtb_BusCreator_n.length = 8U;

  // BusCreator: '<S3>/Bus Creator' incorporates:
  //   BusCreator: '<S3>/Bus Creator6'
  //   Constant: '<S3>/ID_raw'
  //   Constant: '<S3>/available'
  //   Constant: '<S3>/length'

  rtb_BusCreator_hs.available = false;
  rtb_BusCreator_hs.length = 0U;
  rtb_BusCreator_hs.packet.ID = 0U;
  for (i = 0; i < 8; i++) {
    rtb_BusCreator_hs.packet.PAYLOAD[i] = 0U;
  }

  // End of BusCreator: '<S3>/Bus Creator'

  // Concatenate: '<S1>/Vector Concatenate'
  rtb_BusCreator_n_0[0] = rtb_BusCreator_m;
  rtb_BusCreator_n_0[1] = rtb_BusCreator_n;
  rtb_BusCreator_n_0[2] = rtb_BusCreator_hs;
  rtb_BusCreator_n_0[3] = rtb_BusCreator_hs;
  for (i = 0; i < CAN_MAX_NUM_PACKETS; i++) {
    rty_pck_tx->packets[i] = rtb_BusCreator_n_0[i];
  }

  // End of Concatenate: '<S1>/Vector Concatenate'
  // End of Outputs for SubSystem: '<Root>/CAN_Encoder'
}

// Model initialize function
void can_encoder_initialize(const char_T **rt_errorStatus)
{
  RT_MODEL_can_encoder_T *const can_encoder_M = &(can_encoder_MdlrefDW.rtm);

  // Registration code

  // initialize error status
  rtmSetErrorStatusPointer(can_encoder_M, rt_errorStatus);
}

//
// File trailer for generated code.
//
// [EOF]
//
