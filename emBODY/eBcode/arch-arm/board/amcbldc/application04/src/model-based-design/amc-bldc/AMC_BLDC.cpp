//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: AMC_BLDC.cpp
//
// Code generated for Simulink model 'AMC_BLDC'.
//
// Model version                  : 3.76
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Thu Dec  2 09:39:37 2021
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
                                             //  Referenced by: '<S3>/CAN_Encoder'
                                             //  2^16/360

uint8_T CAN_ID_AMC = 3U;               // Variable: CAN_ID_AMC
                                          //  Referenced by:
                                          //    '<S3>/CAN_Decoder'
                                          //    '<S3>/CAN_Encoder'
                                          //  4 bits defining the ID of the AMC_BLDC board.

namespace amc_bldc_codegen
{
  // Model step function for TID0
  void AMC_BLDC::step_FOC()            // Explicit Task: FOC.ISR
  {
    real32_T rtb_velocity;

    // RateTransition generated from: '<S1>/Adapter1' incorporates:
    //   Inport: '<Root>/SensorsData_jointpositions_position'

    AMC_BLDC_B.BusConversion_InsertedFor_FOC_a.jointpositions.position =
      AMC_BLDC_U.SensorsData_jointpositions_posi;

    // RateTransition generated from: '<S1>/Adapter1' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    AMC_BLDC_B.BusConversion_InsertedFor_FOC_a.motorsensors.Iabc[0] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[0];
    AMC_BLDC_B.BusConversion_InsertedFor_FOC_a.motorsensors.Iabc[1] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[1];
    AMC_BLDC_B.BusConversion_InsertedFor_FOC_a.motorsensors.Iabc[2] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[2];

    // RateTransition generated from: '<S1>/Adapter1' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_angle'

    AMC_BLDC_B.BusConversion_InsertedFor_FOC_a.motorsensors.angle =
      AMC_BLDC_U.SensorsData_motorsensors_angle;

    // RateTransition generated from: '<S1>/Adapter1' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_temperature'

    AMC_BLDC_B.BusConversion_InsertedFor_FOC_a.motorsensors.temperature =
      AMC_BLDC_U.SensorsData_motorsensors_temper;

    // RateTransition generated from: '<S1>/Adapter1' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_voltage'

    AMC_BLDC_B.BusConversion_InsertedFor_FOC_a.motorsensors.voltage =
      AMC_BLDC_U.SensorsData_motorsensors_voltag;

    // RateTransition generated from: '<S1>/Adapter1' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_current'

    AMC_BLDC_B.BusConversion_InsertedFor_FOC_a.motorsensors.current =
      AMC_BLDC_U.SensorsData_motorsensors_curren;

    // RateTransition generated from: '<S1>/Adapter1' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_hallABC'

    AMC_BLDC_B.BusConversion_InsertedFor_FOC_a.motorsensors.hallABC =
      AMC_BLDC_U.SensorsData_motorsensors_hallAB;

    // RateTransition generated from: '<S1>/Adapter5'
    rtb_velocity = AMC_BLDC_DW.RTBInsertedForAdapter_InsertedF;

    // BusCreator generated from: '<S1>/FOC'
    AMC_BLDC_B.BusConversion_InsertedFor_FOC_i.jointvelocities.velocity =
      rtb_velocity;

    // RateTransition generated from: '<S1>/Adapter2'
    AMC_BLDC_B.RTBInsertedForAdapter_Insert_io =
      AMC_BLDC_B.RTBInsertedForAdapter_Insert_hb;

    // RateTransition generated from: '<S1>/Adapter3'
    AMC_BLDC_B.RTBInsertedForAdapter_InsertedF =
      AMC_BLDC_B.RTBInsertedForAdapter_Inserte_k;

    // RateTransition generated from: '<S1>/Adapter4'
    AMC_BLDC_B.RTBInsertedForAdapter_Inserte_b =
      AMC_BLDC_B.RTBInsertedForAdapter_Inserte_e;

    // RateTransition generated from: '<S1>/Adapter'
    AMC_BLDC_B.RTBInsertedForAdapter_Inserte_m = AMC_BLDC_B.OuterControl;

    // HiddenFcnCallGenerator: '<S1>/FOC'
    // ModelReference generated from: '<S1>/FOC'
    FOCMDLOBJ8.control_foc_ISR(&AMC_BLDC_B.RTBInsertedForAdapter_Insert_io,
      &AMC_BLDC_B.RTBInsertedForAdapter_InsertedF,
      &AMC_BLDC_B.BusConversion_InsertedFor_FOC_a,
      &AMC_BLDC_B.BusConversion_InsertedFor_FOC_i,
      &AMC_BLDC_B.RTBInsertedForAdapter_Inserte_b,
      &AMC_BLDC_B.RTBInsertedForAdapter_Inserte_m, &AMC_BLDC_B.FOC);

    // End of Outputs for HiddenFcnCallGenerator: '<S1>/FOC'

    // RateTransition generated from: '<S4>/Adapter1'
    if (AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_h == 0) {
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_p[0] = AMC_BLDC_B.FOC.Vabc[0];
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_p[1] = AMC_BLDC_B.FOC.Vabc[1];
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_p[2] = AMC_BLDC_B.FOC.Vabc[2];
    }

    // End of RateTransition generated from: '<S4>/Adapter1'

    // RateTransition generated from: '<S4>/Adapter6'
    if (AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_l == 0) {
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_m[0] = AMC_BLDC_B.FOC.Vabc[0];
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_m[1] = AMC_BLDC_B.FOC.Vabc[1];
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_m[2] = AMC_BLDC_B.FOC.Vabc[2];
    }

    // End of RateTransition generated from: '<S4>/Adapter6'

    // Outport: '<Root>/ControlOutputs_Vabc'
    AMC_BLDC_Y.ControlOutputs_Vabc[0] = AMC_BLDC_B.FOC.Vabc[0];
    AMC_BLDC_Y.ControlOutputs_Vabc[1] = AMC_BLDC_B.FOC.Vabc[1];
    AMC_BLDC_Y.ControlOutputs_Vabc[2] = AMC_BLDC_B.FOC.Vabc[2];

    // Outport: '<Root>/ControlOutputs_Iq_fbk_current'
    AMC_BLDC_Y.ControlOutputs_Iq_fbk_current = AMC_BLDC_B.FOC.Iq_fbk.current;
  }

  // Model step function for TID1
  void AMC_BLDC::step_CAN_RX_raw2struct() // Explicit Task: CAN_RX_raw2struct.D1 
  {
    // local block i/o variables
    BUS_CAN_RX rtb_pck_rx_struct;
    BUS_CAN rtb_BusConversion_InsertedFor_C;

    // BusCreator generated from: '<S3>/CAN_RX_raw2struct' incorporates:
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

    // End of BusCreator generated from: '<S3>/CAN_RX_raw2struct'

    // ModelReference: '<S3>/CAN_RX_raw2struct'
    CAN_RX_raw2structMDLOBJ5.step(rtb_BusConversion_InsertedFor_C,
      rtb_pck_rx_struct);

    // RateTransition generated from: '<S3>/CAN_Decoder'
    AMC_BLDC_B.TmpRTBAtCAN_DecoderInport1 = rtb_pck_rx_struct;
  }

  // Model step function for TID2
  void AMC_BLDC::step_CAN_Decoder()    // Explicit Task: CAN_Decoder.D1
  {
    // ModelReference: '<S3>/CAN_Decoder'
    CAN_DecoderMDLOBJ3.step(AMC_BLDC_B.TmpRTBAtCAN_DecoderInport1,
      AMC_BLDC_B.CAN_Decoder_o1, AMC_BLDC_B.CAN_Decoder_o2,
      AMC_BLDC_B.CAN_Decoder_o3);

    // RateTransition generated from: '<S4>/Adapter2'
    AMC_BLDC_B.RTBInsertedForAdapter_Inserte_i = AMC_BLDC_B.CAN_Decoder_o1;

    // RateTransition generated from: '<S4>/Adapter3'
    AMC_BLDC_B.RTBInsertedForAdapter_Inserte_o = AMC_BLDC_B.CAN_Decoder_o3;

    // RateTransition generated from: '<S4>/Adapter5'
    AMC_BLDC_B.RTBInsertedForAdapter_Inserte_f = AMC_BLDC_B.CAN_Decoder_o1;

    // RateTransition generated from: '<S4>/Adapter'
    AMC_BLDC_B.RTBInsertedForAdapter_Inser_mdo = AMC_BLDC_B.CAN_Decoder_o2;
  }

  // Model step function for TID3
  void AMC_BLDC::step_FOC_transitions() // Sample time: [0.001s, 0.0s]
  {
    // RateTransition generated from: '<S1>/OuterControl' incorporates:
    //   Inport: '<Root>/SensorsData_jointpositions_position'
    //   Inport: '<Root>/SensorsData_motorsensors_angle'
    //   Inport: '<Root>/SensorsData_motorsensors_current'
    //   Inport: '<Root>/SensorsData_motorsensors_hallABC'
    //   Inport: '<Root>/SensorsData_motorsensors_temperature'
    //   Inport: '<Root>/SensorsData_motorsensors_voltage'

    AMC_BLDC_B.TmpRTBAtOuterControlInport4.jointpositions.position =
      AMC_BLDC_U.SensorsData_jointpositions_posi;
    AMC_BLDC_B.TmpRTBAtOuterControlInport4.motorsensors.angle =
      AMC_BLDC_U.SensorsData_motorsensors_angle;
    AMC_BLDC_B.TmpRTBAtOuterControlInport4.motorsensors.temperature =
      AMC_BLDC_U.SensorsData_motorsensors_temper;
    AMC_BLDC_B.TmpRTBAtOuterControlInport4.motorsensors.voltage =
      AMC_BLDC_U.SensorsData_motorsensors_voltag;
    AMC_BLDC_B.TmpRTBAtOuterControlInport4.motorsensors.current =
      AMC_BLDC_U.SensorsData_motorsensors_curren;
    AMC_BLDC_B.TmpRTBAtOuterControlInport4.motorsensors.hallABC =
      AMC_BLDC_U.SensorsData_motorsensors_hallAB;

    // RateTransition generated from: '<S2>/Estimation_Velocity' incorporates:
    //   Inport: '<Root>/SensorsData_jointpositions_position'
    //   Inport: '<Root>/SensorsData_motorsensors_angle'
    //   Inport: '<Root>/SensorsData_motorsensors_current'
    //   Inport: '<Root>/SensorsData_motorsensors_hallABC'
    //   Inport: '<Root>/SensorsData_motorsensors_temperature'
    //   Inport: '<Root>/SensorsData_motorsensors_voltage'

    AMC_BLDC_B.TmpRTBAtEstimation_VelocityInpo.jointpositions.position =
      AMC_BLDC_U.SensorsData_jointpositions_posi;
    AMC_BLDC_B.TmpRTBAtEstimation_VelocityInpo.motorsensors.angle =
      AMC_BLDC_U.SensorsData_motorsensors_angle;
    AMC_BLDC_B.TmpRTBAtEstimation_VelocityInpo.motorsensors.temperature =
      AMC_BLDC_U.SensorsData_motorsensors_temper;
    AMC_BLDC_B.TmpRTBAtEstimation_VelocityInpo.motorsensors.voltage =
      AMC_BLDC_U.SensorsData_motorsensors_voltag;
    AMC_BLDC_B.TmpRTBAtEstimation_VelocityInpo.motorsensors.current =
      AMC_BLDC_U.SensorsData_motorsensors_curren;
    AMC_BLDC_B.TmpRTBAtEstimation_VelocityInpo.motorsensors.hallABC =
      AMC_BLDC_U.SensorsData_motorsensors_hallAB;

    // RateTransition generated from: '<S4>/Adapter4' incorporates:
    //   Inport: '<Root>/SensorsData_jointpositions_position'

    AMC_BLDC_B.position_o = AMC_BLDC_U.SensorsData_jointpositions_posi;

    // RateTransition generated from: '<S4>/Adapter4' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_angle'

    AMC_BLDC_B.angle_o = AMC_BLDC_U.SensorsData_motorsensors_angle;

    // RateTransition generated from: '<S4>/Adapter4' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_temperature'

    AMC_BLDC_B.temperature_i = AMC_BLDC_U.SensorsData_motorsensors_temper;

    // RateTransition generated from: '<S4>/Adapter4' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_voltage'

    AMC_BLDC_B.voltage_g = AMC_BLDC_U.SensorsData_motorsensors_voltag;

    // RateTransition generated from: '<S4>/Adapter4' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_current'

    AMC_BLDC_B.current_l = AMC_BLDC_U.SensorsData_motorsensors_curren;

    // RateTransition generated from: '<S4>/Adapter4' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_hallABC'

    AMC_BLDC_B.hallABC_c = AMC_BLDC_U.SensorsData_motorsensors_hallAB;

    // RateTransition generated from: '<S4>/Component' incorporates:
    //   Inport: '<Root>/SensorsData_jointpositions_position'

    AMC_BLDC_B.position = AMC_BLDC_U.SensorsData_jointpositions_posi;

    // RateTransition generated from: '<S1>/OuterControl' incorporates:
    //   BusCreator generated from: '<S1>/OuterControl'
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    AMC_BLDC_B.TmpRTBAtOuterControlInport4.motorsensors.Iabc[0] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[0];

    // RateTransition generated from: '<S2>/Estimation_Velocity' incorporates:
    //   BusCreator generated from: '<S2>/Estimation_Velocity'
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    AMC_BLDC_B.TmpRTBAtEstimation_VelocityInpo.motorsensors.Iabc[0] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[0];

    // RateTransition generated from: '<S4>/Adapter4' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    AMC_BLDC_B.Iabc_p[0] = AMC_BLDC_U.SensorsData_motorsensors_Iabc[0];

    // RateTransition generated from: '<S4>/Component' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    AMC_BLDC_B.Iabc[0] = AMC_BLDC_U.SensorsData_motorsensors_Iabc[0];

    // RateTransition generated from: '<S1>/OuterControl' incorporates:
    //   BusCreator generated from: '<S1>/OuterControl'
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    AMC_BLDC_B.TmpRTBAtOuterControlInport4.motorsensors.Iabc[1] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[1];

    // RateTransition generated from: '<S2>/Estimation_Velocity' incorporates:
    //   BusCreator generated from: '<S2>/Estimation_Velocity'
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    AMC_BLDC_B.TmpRTBAtEstimation_VelocityInpo.motorsensors.Iabc[1] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[1];

    // RateTransition generated from: '<S4>/Adapter4' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    AMC_BLDC_B.Iabc_p[1] = AMC_BLDC_U.SensorsData_motorsensors_Iabc[1];

    // RateTransition generated from: '<S4>/Component' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    AMC_BLDC_B.Iabc[1] = AMC_BLDC_U.SensorsData_motorsensors_Iabc[1];

    // RateTransition generated from: '<S1>/OuterControl' incorporates:
    //   BusCreator generated from: '<S1>/OuterControl'
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    AMC_BLDC_B.TmpRTBAtOuterControlInport4.motorsensors.Iabc[2] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[2];

    // RateTransition generated from: '<S2>/Estimation_Velocity' incorporates:
    //   BusCreator generated from: '<S2>/Estimation_Velocity'
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    AMC_BLDC_B.TmpRTBAtEstimation_VelocityInpo.motorsensors.Iabc[2] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[2];

    // RateTransition generated from: '<S4>/Adapter4' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    AMC_BLDC_B.Iabc_p[2] = AMC_BLDC_U.SensorsData_motorsensors_Iabc[2];

    // RateTransition generated from: '<S4>/Component' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    AMC_BLDC_B.Iabc[2] = AMC_BLDC_U.SensorsData_motorsensors_Iabc[2];

    // RateTransition generated from: '<S4>/Component' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_angle'

    AMC_BLDC_B.angle = AMC_BLDC_U.SensorsData_motorsensors_angle;

    // RateTransition generated from: '<S4>/Component' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_temperature'

    AMC_BLDC_B.temperature = AMC_BLDC_U.SensorsData_motorsensors_temper;

    // RateTransition generated from: '<S4>/Component' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_voltage'

    AMC_BLDC_B.voltage = AMC_BLDC_U.SensorsData_motorsensors_voltag;

    // RateTransition generated from: '<S4>/Component' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_current'

    AMC_BLDC_B.current = AMC_BLDC_U.SensorsData_motorsensors_curren;

    // RateTransition generated from: '<S4>/Component' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_hallABC'

    AMC_BLDC_B.hallABC = AMC_BLDC_U.SensorsData_motorsensors_hallAB;
  }

  // Model step function for TID4
  void AMC_BLDC::step_Estimation_Velocity() // Explicit Task: Estimation_Velocity.D1 
  {
    // local block i/o variables
    EstimatedData rtb_Estimation_Velocity;

    // ModelReference: '<S2>/Estimation_Velocity'
    Estimation_VelocityMDLOBJ2.step(AMC_BLDC_B.TmpRTBAtEstimation_VelocityInpo,
      rtb_Estimation_Velocity);

    // Outport: '<Root>/EstimatedData' incorporates:
    //   RateTransition generated from: '<S2>/Adapter'

    AMC_BLDC_Y.EstimatedData_p = rtb_Estimation_Velocity;
  }

  // Model step function for TID5
  void AMC_BLDC::step_SupervisorFSM_RX() // Explicit Task: SupervisorFSM_RX.D1
  {
    real32_T rtb_Vabc[3];
    real32_T rtb_current_f;

    // BusCreator generated from: '<S4>/SupervisorFSM_RX'
    AMC_BLDC_B.BusConversion_InsertedFor_Super.jointpositions.position =
      AMC_BLDC_B.position;

    // BusCreator generated from: '<S4>/SupervisorFSM_RX'
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.Iabc[0] =
      AMC_BLDC_B.Iabc[0];
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.Iabc[1] =
      AMC_BLDC_B.Iabc[1];
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.Iabc[2] =
      AMC_BLDC_B.Iabc[2];
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.angle =
      AMC_BLDC_B.angle;
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.temperature =
      AMC_BLDC_B.temperature;
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.voltage =
      AMC_BLDC_B.voltage;
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.current =
      AMC_BLDC_B.current;
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.hallABC =
      AMC_BLDC_B.hallABC;

    // RateTransition generated from: '<S4>/Adapter1'
    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_h = 1;
    rtb_Vabc[0] = AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_p[0];
    rtb_Vabc[1] = AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_p[1];
    rtb_Vabc[2] = AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_p[2];
    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_h = 0;

    // RateTransition generated from: '<S4>/Adapter1'
    rtb_current_f = AMC_BLDC_B.FOC.Iq_fbk.current;

    // BusCreator generated from: '<S4>/SupervisorFSM_RX'
    AMC_BLDC_B.BusConversion_InsertedFor_Sup_j.Iq_fbk.current = rtb_current_f;

    // BusCreator generated from: '<S4>/SupervisorFSM_RX'
    AMC_BLDC_B.BusConversion_InsertedFor_Sup_j.Vabc[0] = rtb_Vabc[0];
    AMC_BLDC_B.BusConversion_InsertedFor_Sup_j.Vabc[1] = rtb_Vabc[1];
    AMC_BLDC_B.BusConversion_InsertedFor_Sup_j.Vabc[2] = rtb_Vabc[2];

    // ModelReference: '<S4>/SupervisorFSM_RX'
    SupervisorFSM_RXMDLOBJ6.step(AMC_BLDC_B.RTBInsertedForAdapter_Inser_mdo,
      AMC_BLDC_B.RTBInsertedForAdapter_Inserte_i,
      AMC_BLDC_B.RTBInsertedForAdapter_Inserte_o,
      AMC_BLDC_B.RTBInsertedForAdapter_Insert_il,
      AMC_BLDC_B.BusConversion_InsertedFor_Super,
      AMC_BLDC_B.BusConversion_InsertedFor_Sup_j, AMC_BLDC_B.Targets_n,
      AMC_BLDC_B.ConfigurationParameters_m, AMC_BLDC_B.Flags_k);

    // RateTransition generated from: '<S4>/Adapter12'
    AMC_BLDC_B.RTBInsertedForAdapter_Inserte_k =
      AMC_BLDC_B.ConfigurationParameters_m;

    // RateTransition generated from: '<S4>/Adapter8'
    AMC_BLDC_B.RTBInsertedForAdapter_Insert_hb = AMC_BLDC_B.Flags_k;

    // RateTransition generated from: '<S4>/Adapter9'
    AMC_BLDC_B.RTBInsertedForAdapter_Inserte_e = AMC_BLDC_B.Targets_n;
  }

  // Model step function for TID6
  void AMC_BLDC::step_OuterControl()   // Explicit Task: OuterControl.D1
  {
    // ModelReference: '<S1>/OuterControl' incorporates:
    //   Outport: '<Root>/EstimatedData'

    OuterControlMDLOBJ1.step(AMC_BLDC_B.RTBInsertedForAdapter_Insert_hb,
      AMC_BLDC_B.RTBInsertedForAdapter_Inserte_k,
      AMC_BLDC_B.RTBInsertedForAdapter_Inserte_e,
      AMC_BLDC_B.TmpRTBAtOuterControlInport4, AMC_BLDC_Y.EstimatedData_p,
      AMC_BLDC_B.OuterControl);

    // RateTransition generated from: '<S1>/Adapter5' incorporates:
    //   Outport: '<Root>/EstimatedData'

    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedF =
      AMC_BLDC_Y.EstimatedData_p.jointvelocities.velocity;

    // RateTransition generated from: '<S4>/Adapter11' incorporates:
    //   Outport: '<Root>/EstimatedData'

    AMC_BLDC_B.sensorsdata = AMC_BLDC_Y.EstimatedData_p;

    // RateTransition generated from: '<S4>/Component1' incorporates:
    //   Outport: '<Root>/EstimatedData'

    AMC_BLDC_B.RTBInsertedForAdapter_Insert_il = AMC_BLDC_Y.EstimatedData_p;
  }

  // Model step function for TID7
  void AMC_BLDC::step_SupervisorFSM_TX() // Explicit Task: SupervisorFSM_TX.D1
  {
    ControlOutputs rtb_BusConversion_InsertedFor_l;
    SensorsData rtb_BusConversion_InsertedFor_S;
    real32_T rtb_current_m;

    // BusCreator generated from: '<S4>/SupervisorFSM_TX'
    rtb_BusConversion_InsertedFor_S.jointpositions.position =
      AMC_BLDC_B.position_o;
    rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[0] = AMC_BLDC_B.Iabc_p[0];
    rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[1] = AMC_BLDC_B.Iabc_p[1];
    rtb_BusConversion_InsertedFor_S.motorsensors.Iabc[2] = AMC_BLDC_B.Iabc_p[2];
    rtb_BusConversion_InsertedFor_S.motorsensors.angle = AMC_BLDC_B.angle_o;
    rtb_BusConversion_InsertedFor_S.motorsensors.temperature =
      AMC_BLDC_B.temperature_i;
    rtb_BusConversion_InsertedFor_S.motorsensors.voltage = AMC_BLDC_B.voltage_g;
    rtb_BusConversion_InsertedFor_S.motorsensors.current = AMC_BLDC_B.current_l;
    rtb_BusConversion_InsertedFor_S.motorsensors.hallABC = AMC_BLDC_B.hallABC_c;

    // RateTransition generated from: '<S4>/Adapter6'
    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_l = 1;
    rtb_BusConversion_InsertedFor_l.Vabc[0] =
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_m[0];
    rtb_BusConversion_InsertedFor_l.Vabc[1] =
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_m[1];
    rtb_BusConversion_InsertedFor_l.Vabc[2] =
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_m[2];
    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_l = 0;

    // RateTransition generated from: '<S4>/Adapter6'
    rtb_current_m = AMC_BLDC_B.FOC.Iq_fbk.current;

    // BusCreator generated from: '<S4>/SupervisorFSM_TX'
    rtb_BusConversion_InsertedFor_l.Iq_fbk.current = rtb_current_m;

    // ModelReference: '<S4>/SupervisorFSM_TX'
    SupervisorFSM_TXMDLOBJ7.step(AMC_BLDC_B.RTBInsertedForAdapter_Inserte_f,
      rtb_BusConversion_InsertedFor_S, AMC_BLDC_B.sensorsdata,
      rtb_BusConversion_InsertedFor_l, AMC_BLDC_B.MessagesTx,
      AMC_BLDC_B.SupervisorFSM_TX_o2);

    // RateTransition generated from: '<S4>/Adapter10'
    AMC_BLDC_B.RTBInsertedForAdapter_Inserte_h = AMC_BLDC_B.MessagesTx;

    // RateTransition generated from: '<S4>/Adapter7'
    AMC_BLDC_B.RTBInsertedForAdapter_Insert_it = AMC_BLDC_B.SupervisorFSM_TX_o2;
  }

  // Model step function for TID8
  void AMC_BLDC::step_CAN_Encoder()    // Explicit Task: CAN_Encoder.D1
  {
    // ModelReference: '<S3>/CAN_Encoder' incorporates:
    //   Outport: '<Root>/PacketsTx'

    CAN_EncoderMDLOBJ4.step(AMC_BLDC_B.RTBInsertedForAdapter_Inserte_h,
      AMC_BLDC_B.RTBInsertedForAdapter_Insert_it, AMC_BLDC_Y.PacketsTx);
  }

  // Model initialize function
  void AMC_BLDC::initialize()
  {
    // Model Initialize function for ModelReference Block: '<S1>/OuterControl'

    // Set error status pointer for ModelReference Block: '<S1>/OuterControl'
    OuterControlMDLOBJ1.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));
    OuterControlMDLOBJ1.initialize();

    // Model Initialize function for ModelReference Block: '<S2>/Estimation_Velocity' 

    // Set error status pointer for ModelReference Block: '<S2>/Estimation_Velocity' 
    Estimation_VelocityMDLOBJ2.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));
    Estimation_VelocityMDLOBJ2.initialize();

    // Model Initialize function for ModelReference Block: '<S3>/CAN_Decoder'

    // Set error status pointer for ModelReference Block: '<S3>/CAN_Decoder'
    CAN_DecoderMDLOBJ3.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));

    // Model Initialize function for ModelReference Block: '<S3>/CAN_Encoder'

    // Set error status pointer for ModelReference Block: '<S3>/CAN_Encoder'
    CAN_EncoderMDLOBJ4.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));
    CAN_EncoderMDLOBJ4.initialize();

    // Model Initialize function for ModelReference Block: '<S3>/CAN_RX_raw2struct' 

    // Set error status pointer for ModelReference Block: '<S3>/CAN_RX_raw2struct' 
    CAN_RX_raw2structMDLOBJ5.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));

    // Model Initialize function for ModelReference Block: '<S4>/SupervisorFSM_RX' 

    // Set error status pointer for ModelReference Block: '<S4>/SupervisorFSM_RX' 
    SupervisorFSM_RXMDLOBJ6.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));

    // Model Initialize function for ModelReference Block: '<S4>/SupervisorFSM_TX' 

    // Set error status pointer for ModelReference Block: '<S4>/SupervisorFSM_TX' 
    SupervisorFSM_TXMDLOBJ7.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));

    // Model Initialize function for ModelReference Block: '<S1>/FOC'

    // Set error status pointer for ModelReference Block: '<S1>/FOC'
    FOCMDLOBJ8.setErrorStatusPointer(rtmGetErrorStatusPointer((&AMC_BLDC_M)));
    FOCMDLOBJ8.initialize();

    // SystemInitialize for ModelReference generated from: '<S1>/FOC'
    FOCMDLOBJ8.init();

    // SystemInitialize for ModelReference: '<S1>/OuterControl'
    OuterControlMDLOBJ1.init();

    // SystemInitialize for ModelReference: '<S2>/Estimation_Velocity'
    Estimation_VelocityMDLOBJ2.init();

    // SystemInitialize for ModelReference: '<S3>/CAN_Decoder'
    CAN_DecoderMDLOBJ3.init(&AMC_BLDC_B.CAN_Decoder_o1,
      &AMC_BLDC_B.CAN_Decoder_o2, &AMC_BLDC_B.CAN_Decoder_o3);

    // SystemInitialize for ModelReference: '<S4>/SupervisorFSM_RX'
    SupervisorFSM_RXMDLOBJ6.init(&AMC_BLDC_B.Flags_k, &AMC_BLDC_B.Targets_n,
      &AMC_BLDC_B.ConfigurationParameters_m);

    // SystemInitialize for ModelReference: '<S4>/SupervisorFSM_TX'
    SupervisorFSM_TXMDLOBJ7.init(&AMC_BLDC_B.MessagesTx,
      &AMC_BLDC_B.SupervisorFSM_TX_o2);
  }

  // Model terminate function
  void AMC_BLDC::terminate()
  {
    // (no terminate code required)
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
