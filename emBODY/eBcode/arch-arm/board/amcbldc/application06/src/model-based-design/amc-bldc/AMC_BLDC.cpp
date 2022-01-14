//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: AMC_BLDC.cpp
//
// Code generated for Simulink model 'AMC_BLDC'.
//
// Model version                  : 3.203
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Fri Jan 14 15:26:10 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "AMC_BLDC.h"
#include "AMC_BLDC_private.h"

// Exported block parameters
real32_T CAN_ANGLE_DEG2ICUB = 182.044449F;// Variable: CAN_ANGLE_DEG2ICUB
                                             //  Referenced by: '<S6>/CAN_Encoder'
                                             //  2^16/360

uint8_T CAN_ID_AMC = 3U;               // Variable: CAN_ID_AMC
                                          //  Referenced by:
                                          //    '<S6>/CAN_Decoder'
                                          //    '<S6>/CAN_Encoder'
                                          //  4 bits defining the ID of the AMC_BLDC board.

namespace amc_bldc_codegen
{
  // Model step function for TID0
  void AMC_BLDC::step_FOC()      // Sample time: [3.5714285714285717E-5s, 0.0s]
  {
    FOCSlowInputs rtb_BusConversion_InsertedFor_F;
    SensorsData rtb_BusConversion_InsertedFor_k;
    int8_T wrBufIdx;

    // BusCreator generated from: '<Root>/FOC' incorporates:
    //   Inport: '<Root>/SensorsData_jointpositions_position'
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'
    //   Inport: '<Root>/SensorsData_motorsensors_angle'
    //   Inport: '<Root>/SensorsData_motorsensors_current'
    //   Inport: '<Root>/SensorsData_motorsensors_hallABC'
    //   Inport: '<Root>/SensorsData_motorsensors_temperature'
    //   Inport: '<Root>/SensorsData_motorsensors_voltage'

    rtb_BusConversion_InsertedFor_k.jointpositions.position =
      AMC_BLDC_U.SensorsData_jointpositions_posi;
    rtb_BusConversion_InsertedFor_k.motorsensors.Iabc[0] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[0];
    rtb_BusConversion_InsertedFor_k.motorsensors.Iabc[1] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[1];
    rtb_BusConversion_InsertedFor_k.motorsensors.Iabc[2] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[2];
    rtb_BusConversion_InsertedFor_k.motorsensors.angle =
      AMC_BLDC_U.SensorsData_motorsensors_angle;
    rtb_BusConversion_InsertedFor_k.motorsensors.temperature =
      AMC_BLDC_U.SensorsData_motorsensors_temper;
    rtb_BusConversion_InsertedFor_k.motorsensors.voltage =
      AMC_BLDC_U.SensorsData_motorsensors_voltag;
    rtb_BusConversion_InsertedFor_k.motorsensors.current =
      AMC_BLDC_U.SensorsData_motorsensors_curren;
    rtb_BusConversion_InsertedFor_k.motorsensors.hallABC =
      AMC_BLDC_U.SensorsData_motorsensors_hallAB;

    // RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_lock();
    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_p =
      AMC_BLDC_DW.RTBInsertedForAdapter_Insert_hj;
    rtw_mutex_unlock();

    // BusCreator generated from: '<Root>/FOC' incorporates:
    //   RateTransition generated from: '<Root>/Adapter2'

    rtb_BusConversion_InsertedFor_F.flags =
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_l[AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_p];

    // RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_lock();
    AMC_BLDC_DW.RTBInsertedForAdapter_Insert_m3 =
      AMC_BLDC_DW.RTBInsertedForAdapter_Inser_mpz;
    rtw_mutex_unlock();

    // BusCreator generated from: '<Root>/FOC' incorporates:
    //   RateTransition generated from: '<Root>/Adapter2'

    rtb_BusConversion_InsertedFor_F.configurationparameters =
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedF[AMC_BLDC_DW.RTBInsertedForAdapter_Insert_m3];

    // RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_lock();
    AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ko =
      AMC_BLDC_DW.RTBInsertedForAdapter_Insert_b2;
    rtw_mutex_unlock();

    // BusCreator generated from: '<Root>/FOC' incorporates:
    //   RateTransition generated from: '<Root>/Adapter2'

    rtb_BusConversion_InsertedFor_F.estimateddata =
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_k[AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ko];

    // RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_lock();
    AMC_BLDC_DW.RTBInsertedForAdapter_Insert_mb =
      AMC_BLDC_DW.RTBInsertedForAdapter_Insert_jj;
    rtw_mutex_unlock();

    // BusCreator generated from: '<Root>/FOC' incorporates:
    //   RateTransition generated from: '<Root>/Adapter2'

    rtb_BusConversion_InsertedFor_F.targets =
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_m[AMC_BLDC_DW.RTBInsertedForAdapter_Insert_mb];

    // RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_lock();
    AMC_BLDC_DW.RTBInsertedForAdapter_Insert_bw =
      AMC_BLDC_DW.RTBInsertedForAdapter_Insert_p5;
    rtw_mutex_unlock();

    // BusCreator generated from: '<Root>/FOC' incorporates:
    //   RateTransition generated from: '<Root>/Adapter2'

    rtb_BusConversion_InsertedFor_F.controlouteroutputs =
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_i[AMC_BLDC_DW.RTBInsertedForAdapter_Insert_bw];

    // ModelReference: '<Root>/FOC' incorporates:
    //   Outport: '<Root>/ControlOutputs'

    FOCMDLOBJ2.step(&rtb_BusConversion_InsertedFor_k,
                    &rtb_BusConversion_InsertedFor_F,
                    &AMC_BLDC_Y.ControlOutputs_p);

    // RateTransition generated from: '<Root>/Adapter1' incorporates:
    //   Outport: '<Root>/ControlOutputs'

    rtw_mutex_lock();
    wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.RTBInsertedForAdapter_Insert_js +
      1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_a) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock();
    switch (wrBufIdx) {
     case 0:
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_d = AMC_BLDC_Y.ControlOutputs_p;
      break;

     case 1:
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_j = AMC_BLDC_Y.ControlOutputs_p;
      break;

     case 2:
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_o = AMC_BLDC_Y.ControlOutputs_p;
      break;
    }

    AMC_BLDC_DW.RTBInsertedForAdapter_Insert_js = wrBufIdx;

    // End of RateTransition generated from: '<Root>/Adapter1'

    // RateTransition generated from: '<Root>/Adapter3' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    rtw_mutex_lock();
    wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.RTBInsertedForAdapter_Insert_hm +
      1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ol) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock();
    switch (wrBufIdx) {
     case 0:
      AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ce[0] =
        AMC_BLDC_U.SensorsData_motorsensors_Iabc[0];
      AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ce[1] =
        AMC_BLDC_U.SensorsData_motorsensors_Iabc[1];
      AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ce[2] =
        AMC_BLDC_U.SensorsData_motorsensors_Iabc[2];
      break;

     case 1:
      AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ki[0] =
        AMC_BLDC_U.SensorsData_motorsensors_Iabc[0];
      AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ki[1] =
        AMC_BLDC_U.SensorsData_motorsensors_Iabc[1];
      AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ki[2] =
        AMC_BLDC_U.SensorsData_motorsensors_Iabc[2];
      break;

     case 2:
      AMC_BLDC_DW.RTBInsertedForAdapter_Insert_o4[0] =
        AMC_BLDC_U.SensorsData_motorsensors_Iabc[0];
      AMC_BLDC_DW.RTBInsertedForAdapter_Insert_o4[1] =
        AMC_BLDC_U.SensorsData_motorsensors_Iabc[1];
      AMC_BLDC_DW.RTBInsertedForAdapter_Insert_o4[2] =
        AMC_BLDC_U.SensorsData_motorsensors_Iabc[2];
      break;
    }

    AMC_BLDC_DW.RTBInsertedForAdapter_Insert_hm = wrBufIdx;

    // End of RateTransition generated from: '<Root>/Adapter3'
  }

  // Model step function for TID1
  void AMC_BLDC::step_Time()           // Sample time: [0.001s, 0.0s]
  {
    // local block i/o variables
    ControlOuterOutputs rtb_OuterControl;
    BUS_CAN rtb_BusConversion_InsertedFor_C;
    ConfigurationParameters *rtb_ZOHBlockInsertedForAdapte_0;
    SensorsData rtb_BusConversion_InsertedFor_O;
    SensorsData rtb_BusConversion_InsertedFor_S;
    real32_T rtb_angle;
    real32_T rtb_current;
    real32_T rtb_position;
    real32_T rtb_temperature;
    real32_T rtb_voltage;
    int8_T wrBufIdx;
    uint8_T rtb_hallABC;

    // UnitDelay generated from: '<Root>/Adapter'
    rtb_ZOHBlockInsertedForAdapte_0 =
      &AMC_BLDC_DW.ZOHBlockInsertedForAdapter_Inse;

    // RateTransition generated from: '<Root>/Adapter3' incorporates:
    //   Inport: '<Root>/SensorsData_jointpositions_position'

    rtb_position = AMC_BLDC_U.SensorsData_jointpositions_posi;

    // BusCreator generated from: '<S5>/Estimation_Velocity'
    AMC_BLDC_B.BusConversion_InsertedFor_Estim.jointpositions.position =
      rtb_position;

    // RateTransition generated from: '<Root>/Adapter3'
    rtw_mutex_lock();
    AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ol =
      AMC_BLDC_DW.RTBInsertedForAdapter_Insert_hm;
    rtw_mutex_unlock();
    switch (AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ol) {
     case 0:
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[0] =
        AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ce[0];
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[1] =
        AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ce[1];
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[2] =
        AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ce[2];
      break;

     case 1:
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[0] =
        AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ki[0];
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[1] =
        AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ki[1];
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[2] =
        AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ki[2];
      break;

     case 2:
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[0] =
        AMC_BLDC_DW.RTBInsertedForAdapter_Insert_o4[0];
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[1] =
        AMC_BLDC_DW.RTBInsertedForAdapter_Insert_o4[1];
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[2] =
        AMC_BLDC_DW.RTBInsertedForAdapter_Insert_o4[2];
      break;
    }

    // RateTransition generated from: '<Root>/Adapter3' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_angle'

    rtb_angle = AMC_BLDC_U.SensorsData_motorsensors_angle;

    // RateTransition generated from: '<Root>/Adapter3' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_temperature'

    rtb_temperature = AMC_BLDC_U.SensorsData_motorsensors_temper;

    // RateTransition generated from: '<Root>/Adapter3' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_voltage'

    rtb_voltage = AMC_BLDC_U.SensorsData_motorsensors_voltag;

    // RateTransition generated from: '<Root>/Adapter3' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_current'

    rtb_current = AMC_BLDC_U.SensorsData_motorsensors_curren;

    // RateTransition generated from: '<Root>/Adapter3' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_hallABC'

    rtb_hallABC = AMC_BLDC_U.SensorsData_motorsensors_hallAB;

    // BusCreator generated from: '<S5>/Estimation_Velocity'
    AMC_BLDC_B.BusConversion_InsertedFor_Estim.motorsensors.Iabc[0] =
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[0];
    AMC_BLDC_B.BusConversion_InsertedFor_Estim.motorsensors.Iabc[1] =
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[1];
    AMC_BLDC_B.BusConversion_InsertedFor_Estim.motorsensors.Iabc[2] =
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[2];
    AMC_BLDC_B.BusConversion_InsertedFor_Estim.motorsensors.angle = rtb_angle;
    AMC_BLDC_B.BusConversion_InsertedFor_Estim.motorsensors.temperature =
      rtb_temperature;
    AMC_BLDC_B.BusConversion_InsertedFor_Estim.motorsensors.voltage =
      rtb_voltage;
    AMC_BLDC_B.BusConversion_InsertedFor_Estim.motorsensors.current =
      rtb_current;
    AMC_BLDC_B.BusConversion_InsertedFor_Estim.motorsensors.hallABC =
      rtb_hallABC;

    // ModelReference: '<S5>/Estimation_Velocity' incorporates:
    //   Outport: '<Root>/EstimatedData'

    Estimation_VelocityMDLOBJ1.step(AMC_BLDC_B.BusConversion_InsertedFor_Estim,
      AMC_BLDC_Y.EstimatedData_p, &rtb_ZOHBlockInsertedForAdapte_0[0]);

    // BusCreator generated from: '<S6>/CAN_RX_raw2struct' incorporates:
    //   Inport: '<Root>/PacketsRx_available'
    //   Inport: '<Root>/PacketsRx_lengths'
    //   Inport: '<Root>/PacketsRx_packets_ID'
    //   Inport: '<Root>/PacketsRx_packets_PAYLOAD'

    rtb_BusConversion_InsertedFor_C.available = AMC_BLDC_U.PacketsRx_available;
    rtb_BusConversion_InsertedFor_C.lengths = AMC_BLDC_U.PacketsRx_lengths;
    rtb_BusConversion_InsertedFor_C.packets.ID = AMC_BLDC_U.PacketsRx_packets_ID;
    for (int32_T i = 0; i < 8; i++) {
      rtb_BusConversion_InsertedFor_C.packets.PAYLOAD[i] =
        AMC_BLDC_U.PacketsRx_packets_PAYLOAD[i];
    }

    // End of BusCreator generated from: '<S6>/CAN_RX_raw2struct'

    // ModelReference: '<S6>/CAN_RX_raw2struct'
    CAN_RX_raw2structMDLOBJ5.step(rtb_BusConversion_InsertedFor_C,
      AMC_BLDC_B.pck_rx_struct);

    // ModelReference: '<S6>/CAN_Decoder'
    CAN_DecoderMDLOBJ3.step(AMC_BLDC_B.pck_rx_struct, AMC_BLDC_B.CAN_Decoder_o1,
      AMC_BLDC_B.CAN_Decoder_o2, AMC_BLDC_B.CAN_Decoder_o3);

    // RateTransition generated from: '<Root>/Adapter1'
    rtw_mutex_lock();
    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_a =
      AMC_BLDC_DW.RTBInsertedForAdapter_Insert_js;
    rtw_mutex_unlock();
    switch (AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_a) {
     case 0:
      // RateTransition generated from: '<Root>/Adapter1'
      AMC_BLDC_B.RTBInsertedForAdapter_InsertedF =
        AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_d;
      break;

     case 1:
      // RateTransition generated from: '<Root>/Adapter1'
      AMC_BLDC_B.RTBInsertedForAdapter_InsertedF =
        AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_j;
      break;

     case 2:
      // RateTransition generated from: '<Root>/Adapter1'
      AMC_BLDC_B.RTBInsertedForAdapter_InsertedF =
        AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_o;
      break;
    }

    // End of RateTransition generated from: '<Root>/Adapter1'

    // BusCreator generated from: '<S7>/SupervisorFSM_RX'
    AMC_BLDC_B.BusConversion_InsertedFor_Super.jointpositions.position =
      rtb_position;

    // BusCreator generated from: '<S7>/SupervisorFSM_RX'
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.Iabc[0] =
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[0];
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.Iabc[1] =
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[1];
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.Iabc[2] =
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[2];
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.angle = rtb_angle;
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.temperature =
      rtb_temperature;
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.voltage =
      rtb_voltage;
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.current =
      rtb_current;
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.hallABC =
      rtb_hallABC;

    // BusCreator generated from: '<S7>/SupervisorFSM_RX' incorporates:
    //   Inport: '<Root>/ExternalFlags_fault_button'

    AMC_BLDC_B.BusConversion_InsertedFor_Sup_j.fault_button =
      AMC_BLDC_U.ExternalFlags_fault_button;

    // ModelReference: '<S7>/SupervisorFSM_RX' incorporates:
    //   Outport: '<Root>/ConfigurationParameters'
    //   Outport: '<Root>/EstimatedData'

    SupervisorFSM_RXMDLOBJ7.step(AMC_BLDC_B.CAN_Decoder_o2,
      AMC_BLDC_B.CAN_Decoder_o1, AMC_BLDC_B.CAN_Decoder_o3,
      AMC_BLDC_Y.EstimatedData_p, AMC_BLDC_B.BusConversion_InsertedFor_Super,
      AMC_BLDC_B.RTBInsertedForAdapter_InsertedF, AMC_BLDC_B.Targets_n,
      AMC_BLDC_Y.ConfigurationParameters_p, AMC_BLDC_B.Flags_k,
      &AMC_BLDC_B.BusConversion_InsertedFor_Sup_j);

    // BusCreator generated from: '<S7>/SupervisorFSM_TX'
    rtb_BusConversion_InsertedFor_S.jointpositions.position = rtb_position;
    rtb_BusConversion_InsertedFor_S.motorsensors.angle = rtb_angle;
    rtb_BusConversion_InsertedFor_S.motorsensors.temperature = rtb_temperature;
    rtb_BusConversion_InsertedFor_S.motorsensors.voltage = rtb_voltage;
    rtb_BusConversion_InsertedFor_S.motorsensors.current = rtb_current;
    rtb_BusConversion_InsertedFor_S.motorsensors.hallABC = rtb_hallABC;

    // ModelReference: '<S7>/SupervisorFSM_TX' incorporates:
    //   Outport: '<Root>/ConfigurationParameters'
    //   Outport: '<Root>/EstimatedData'

    SupervisorFSM_TXMDLOBJ8.step(AMC_BLDC_B.CAN_Decoder_o1,
      rtb_BusConversion_InsertedFor_S, AMC_BLDC_Y.EstimatedData_p,
      AMC_BLDC_B.RTBInsertedForAdapter_InsertedF, AMC_BLDC_B.MessagesTx,
      AMC_BLDC_B.SupervisorFSM_TX_o2, &AMC_BLDC_Y.ConfigurationParameters_p);

    // ModelReference: '<S6>/CAN_Encoder' incorporates:
    //   Outport: '<Root>/PacketsTx'

    CAN_EncoderMDLOBJ4.step(AMC_BLDC_B.MessagesTx,
      AMC_BLDC_B.SupervisorFSM_TX_o2, AMC_BLDC_Y.PacketsTx);

    // BusCreator generated from: '<Root>/OuterControl'
    rtb_BusConversion_InsertedFor_O.jointpositions.position = rtb_position;
    rtb_BusConversion_InsertedFor_O.motorsensors.Iabc[0] =
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[0];
    rtb_BusConversion_InsertedFor_O.motorsensors.Iabc[1] =
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[1];
    rtb_BusConversion_InsertedFor_O.motorsensors.Iabc[2] =
      rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[2];
    rtb_BusConversion_InsertedFor_O.motorsensors.angle = rtb_angle;
    rtb_BusConversion_InsertedFor_O.motorsensors.temperature = rtb_temperature;
    rtb_BusConversion_InsertedFor_O.motorsensors.voltage = rtb_voltage;
    rtb_BusConversion_InsertedFor_O.motorsensors.current = rtb_current;
    rtb_BusConversion_InsertedFor_O.motorsensors.hallABC = rtb_hallABC;

    // ModelReference: '<Root>/OuterControl' incorporates:
    //   Outport: '<Root>/ConfigurationParameters'
    //   Outport: '<Root>/EstimatedData'

    OuterControlMDLOBJ6.step(AMC_BLDC_B.Flags_k,
      AMC_BLDC_Y.ConfigurationParameters_p, AMC_BLDC_B.Targets_n,
      rtb_BusConversion_InsertedFor_O, AMC_BLDC_Y.EstimatedData_p,
      rtb_OuterControl);

    // RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_lock();
    wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.RTBInsertedForAdapter_Insert_p5 +
      1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == AMC_BLDC_DW.RTBInsertedForAdapter_Insert_bw) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock();
    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_i[wrBufIdx] = rtb_OuterControl;
    AMC_BLDC_DW.RTBInsertedForAdapter_Insert_p5 = wrBufIdx;

    // RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_lock();
    wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.RTBInsertedForAdapter_Insert_hj +
      1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_p) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock();
    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_l[wrBufIdx] = AMC_BLDC_B.Flags_k;
    AMC_BLDC_DW.RTBInsertedForAdapter_Insert_hj = wrBufIdx;

    // RateTransition generated from: '<Root>/Adapter2' incorporates:
    //   Outport: '<Root>/ConfigurationParameters'

    rtw_mutex_lock();
    wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.RTBInsertedForAdapter_Inser_mpz +
      1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == AMC_BLDC_DW.RTBInsertedForAdapter_Insert_m3) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock();
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedF[wrBufIdx] =
      AMC_BLDC_Y.ConfigurationParameters_p;
    AMC_BLDC_DW.RTBInsertedForAdapter_Inser_mpz = wrBufIdx;

    // RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_lock();
    wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.RTBInsertedForAdapter_Insert_jj +
      1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == AMC_BLDC_DW.RTBInsertedForAdapter_Insert_mb) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock();
    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_m[wrBufIdx] = AMC_BLDC_B.Targets_n;
    AMC_BLDC_DW.RTBInsertedForAdapter_Insert_jj = wrBufIdx;

    // RateTransition generated from: '<Root>/Adapter2' incorporates:
    //   Outport: '<Root>/EstimatedData'

    rtw_mutex_lock();
    wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.RTBInsertedForAdapter_Insert_b2 +
      1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == AMC_BLDC_DW.RTBInsertedForAdapter_Insert_ko) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock();
    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_k[wrBufIdx] =
      AMC_BLDC_Y.EstimatedData_p;
    AMC_BLDC_DW.RTBInsertedForAdapter_Insert_b2 = wrBufIdx;

    // Update for UnitDelay generated from: '<Root>/Adapter' incorporates:
    //   Outport: '<Root>/ConfigurationParameters'

    AMC_BLDC_DW.ZOHBlockInsertedForAdapter_Inse =
      AMC_BLDC_Y.ConfigurationParameters_p;
  }

  // Model initialize function
  void AMC_BLDC::initialize()
  {
    // Model Initialize function for ModelReference Block: '<S5>/Estimation_Velocity' 

    // Set error status pointer for ModelReference Block: '<S5>/Estimation_Velocity' 
    Estimation_VelocityMDLOBJ1.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));
    Estimation_VelocityMDLOBJ1.initialize();

    // Model Initialize function for ModelReference Block: '<Root>/FOC'

    // Set error status pointer for ModelReference Block: '<Root>/FOC'
    FOCMDLOBJ2.setErrorStatusPointer(rtmGetErrorStatusPointer((&AMC_BLDC_M)));
    FOCMDLOBJ2.initialize();

    // Model Initialize function for ModelReference Block: '<S6>/CAN_Decoder'

    // Set error status pointer for ModelReference Block: '<S6>/CAN_Decoder'
    CAN_DecoderMDLOBJ3.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));

    // Model Initialize function for ModelReference Block: '<S6>/CAN_Encoder'

    // Set error status pointer for ModelReference Block: '<S6>/CAN_Encoder'
    CAN_EncoderMDLOBJ4.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));
    CAN_EncoderMDLOBJ4.initialize();

    // Model Initialize function for ModelReference Block: '<S6>/CAN_RX_raw2struct' 

    // Set error status pointer for ModelReference Block: '<S6>/CAN_RX_raw2struct' 
    CAN_RX_raw2structMDLOBJ5.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));

    // Model Initialize function for ModelReference Block: '<Root>/OuterControl' 

    // Set error status pointer for ModelReference Block: '<Root>/OuterControl'
    OuterControlMDLOBJ6.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));
    OuterControlMDLOBJ6.initialize();

    // Model Initialize function for ModelReference Block: '<S7>/SupervisorFSM_RX' 

    // Set error status pointer for ModelReference Block: '<S7>/SupervisorFSM_RX' 
    SupervisorFSM_RXMDLOBJ7.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));

    // Model Initialize function for ModelReference Block: '<S7>/SupervisorFSM_TX' 

    // Set error status pointer for ModelReference Block: '<S7>/SupervisorFSM_TX' 
    SupervisorFSM_TXMDLOBJ8.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));

    // Start for RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_init();

    // Start for RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_init();

    // Start for RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_init();

    // Start for RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_init();

    // Start for RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_init();

    // Start for RateTransition generated from: '<Root>/Adapter1'
    rtw_mutex_init();

    // Start for RateTransition generated from: '<Root>/Adapter3'
    rtw_mutex_init();

    // SystemInitialize for ModelReference: '<S5>/Estimation_Velocity'
    Estimation_VelocityMDLOBJ1.init();

    // SystemInitialize for ModelReference: '<Root>/FOC'
    FOCMDLOBJ2.init();

    // SystemInitialize for ModelReference: '<S6>/CAN_Decoder'
    CAN_DecoderMDLOBJ3.init(&AMC_BLDC_B.CAN_Decoder_o1,
      &AMC_BLDC_B.CAN_Decoder_o2, &AMC_BLDC_B.CAN_Decoder_o3);

    // SystemInitialize for ModelReference: '<Root>/OuterControl'
    OuterControlMDLOBJ6.init();

    // SystemInitialize for ModelReference: '<S7>/SupervisorFSM_RX' incorporates:
    //   Outport: '<Root>/ConfigurationParameters'

    SupervisorFSM_RXMDLOBJ7.init(&AMC_BLDC_B.Flags_k, &AMC_BLDC_B.Targets_n,
      &AMC_BLDC_Y.ConfigurationParameters_p);

    // SystemInitialize for ModelReference: '<S7>/SupervisorFSM_TX'
    SupervisorFSM_TXMDLOBJ8.init(&AMC_BLDC_B.MessagesTx,
      &AMC_BLDC_B.SupervisorFSM_TX_o2);
  }

  // Model terminate function
  void AMC_BLDC::terminate()
  {
    // Terminate for RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_destroy();

    // Terminate for RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_destroy();

    // Terminate for RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_destroy();

    // Terminate for RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_destroy();

    // Terminate for RateTransition generated from: '<Root>/Adapter2'
    rtw_mutex_destroy();

    // Terminate for RateTransition generated from: '<Root>/Adapter1'
    rtw_mutex_destroy();

    // Terminate for RateTransition generated from: '<Root>/Adapter3'
    rtw_mutex_destroy();
  }

  // Constructor
  AMC_BLDC::AMC_BLDC() :
    AMC_BLDC_U(),
    AMC_BLDC_Y(),
    AMC_BLDC_B(),
    AMC_BLDC_DW(),
    AMC_BLDC_M()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  AMC_BLDC::~AMC_BLDC()
  {
    // Currently there is no destructor body generated.
  }

  // Real-Time Model get method
  amc_bldc_codegen::AMC_BLDC::RT_MODEL_AMC_BLDC_T * AMC_BLDC::getRTM()
  {
    return (&AMC_BLDC_M);
  }
}

//
// File trailer for generated code.
//
// [EOF]
//
