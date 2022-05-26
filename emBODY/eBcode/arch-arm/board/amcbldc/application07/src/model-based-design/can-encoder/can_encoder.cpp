//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: can_encoder.cpp
//
// Code generated for Simulink model 'can_encoder'.
//
// Model version                  : 2.57
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Thu May 26 11:44:36 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "can_encoder.h"
#include "can_encoder_private.h"

MdlrefDW_can_encoder_T can_encoder_MdlrefDW;

// Block states (default storage)
DW_can_encoder_f_T can_encoder_DW;

//
// Output and update for atomic system:
//    '<S6>/format_can_id'
//    '<S10>/format_can_id'
//
void can_encoder_format_can_id(uint8_T rtu_class, uint8_T rtu_can_id_amc,
  uint8_T rtu_dst_typ, uint16_T *rty_pkt_id)
{
  *rty_pkt_id = static_cast<uint16_T>(rtu_class << 8);
  *rty_pkt_id = static_cast<uint16_T>(rtu_can_id_amc << 4 | *rty_pkt_id);
  *rty_pkt_id = static_cast<uint16_T>(rtu_dst_typ | *rty_pkt_id);
}

// Output and update for referenced model: 'can_encoder'
void can_encoder(const BUS_MESSAGES_TX *rtu_messages_tx, const BUS_EVENTS_TX
                 *rtu_events_tx, BUS_CAN_MULTIPLE *rty_pck_tx)
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
  if (rtu_messages_tx->status.pwm_fbk < 32768.0F) {
    if (rtu_messages_tx->status.pwm_fbk >= -32768.0F) {
      rtb_DataTypeConversion_0 = static_cast<int16_T>
        (rtu_messages_tx->status.pwm_fbk);
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

  rty_pck_tx->packets[0].packet.PAYLOAD[0] = static_cast<uint8_T>
    (rtu_messages_tx->status.control_mode);
  rty_pck_tx->packets[0].packet.PAYLOAD[1] = 0U;
  std::memcpy((void *)&tmp[0], (void *)&rtb_DataTypeConversion_0, (uint32_T)
              ((size_t)2 * sizeof(uint8_T)));
  rty_pck_tx->packets[0].packet.PAYLOAD[2] = tmp[0];
  rty_pck_tx->packets[0].packet.PAYLOAD[3] = tmp[1];
  rty_pck_tx->packets[0].packet.PAYLOAD[4] =
    rtu_messages_tx->status.flags.ExternalFaultAsserted;
  rty_pck_tx->packets[0].packet.PAYLOAD[5] = 0U;
  rty_pck_tx->packets[0].packet.PAYLOAD[6] = 0U;
  rty_pck_tx->packets[0].packet.PAYLOAD[7] = 0U;

  // DataTypeConversion: '<S2>/Data Type Conversion' incorporates:
  //   Gain: '<S2>/Gain2'

  tmp_0 = 1000.0F * rtu_messages_tx->foc.current;
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
  //   Gain: '<S2>/Gain1'

  tmp_0 = CAN_ANGLE_DEG2ICUB / 1000.0F * rtu_messages_tx->foc.velocity;
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
  //   Gain: '<S2>/Gain3'

  tmp_0 = CAN_ANGLE_DEG2ICUB * rtu_messages_tx->foc.position;
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

  // MATLAB Function: '<S10>/format_can_id' incorporates:
  //   Constant: '<S4>/Constant'
  //   Constant: '<S4>/Motor Control Streaming'
  //   Constant: '<S4>/TYPESTATUS'

  can_encoder_format_can_id(1, CAN_ID_AMC, 3, &rtb_pkt_id);

  // BusCreator: '<S4>/Bus Creator' incorporates:
  //   BusCreator: '<S4>/Bus Creator1'
  //   Concatenate: '<S1>/Vector Concatenate'
  //   Constant: '<S4>/length'
  //   RelationalOperator: '<S9>/FixPt Relational Operator'
  //   UnitDelay: '<S9>/Delay Input1'
  //
  //  Block description for '<S9>/Delay Input1':
  //
  //   Store in Global RAM

  rty_pck_tx->packets[0].available = (rtu_events_tx->status !=
    can_encoder_DW.DelayInput1_DSTATE);
  rty_pck_tx->packets[0].length = 8U;
  rty_pck_tx->packets[0].packet.ID = rtb_pkt_id;

  // MATLAB Function: '<S6>/format_can_id' incorporates:
  //   Constant: '<S2>/Constant'
  //   Constant: '<S2>/Motor Control Streaming'
  //   Constant: '<S2>/TYPE2FOC'

  can_encoder_format_can_id(1, CAN_ID_AMC, 0, &rtb_pkt_id);

  // BusCreator: '<S2>/Bus Creator' incorporates:
  //   BusCreator: '<S2>/Bus Creator1'
  //   Constant: '<S2>/length'
  //   RelationalOperator: '<S5>/FixPt Relational Operator'
  //   UnitDelay: '<S5>/Delay Input1'
  //
  //  Block description for '<S5>/Delay Input1':
  //
  //   Store in Global RAM

  rtb_BusCreator_d.available = (rtu_events_tx->foc !=
    can_encoder_DW.DelayInput1_DSTATE_c);
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
  rty_pck_tx->packets[1] = rtb_BusCreator_d;
  rty_pck_tx->packets[2] = rtb_BusCreator_j;
  rty_pck_tx->packets[3] = rtb_BusCreator_j;

  // Update for UnitDelay: '<S9>/Delay Input1'
  //
  //  Block description for '<S9>/Delay Input1':
  //
  //   Store in Global RAM

  can_encoder_DW.DelayInput1_DSTATE = rtu_events_tx->status;

  // Update for UnitDelay: '<S5>/Delay Input1'
  //
  //  Block description for '<S5>/Delay Input1':
  //
  //   Store in Global RAM

  can_encoder_DW.DelayInput1_DSTATE_c = rtu_events_tx->foc;

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
