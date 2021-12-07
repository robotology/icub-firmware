//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: AMC_BLDC.cpp
//
// Code generated for Simulink model 'AMC_BLDC'.
//
// Model version                  : 3.93
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Tue Dec  7 09:16:47 2021
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
//#include "embot_core.h"
//#include <hal_trace.h>
//#include <stdio.h>

 
namespace amc_bldc_codegen
{
  // Model step function for TID0
  void AMC_BLDC::step_FOC()      // Sample time: [3.5714285714285717E-5s, 0.0s]
  {
    // local block i/o variables
    ControlOutputs rtb_FOC;
    ControlOuterOutputs rtb_TmpRTBAtFOCInport6;
    EstimatedData rtb_RTBInsertedForAdapter_Inser;
    SensorsData rtb_BusConversion_InsertedFor_F;
    int8_T wrBufIdx;

    // BusCreator generated from: '<S1>/FOC' incorporates:
    //   Inport: '<Root>/SensorsData_jointpositions_position'
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'
    //   Inport: '<Root>/SensorsData_motorsensors_angle'
    //   Inport: '<Root>/SensorsData_motorsensors_current'
    //   Inport: '<Root>/SensorsData_motorsensors_hallABC'
    //   Inport: '<Root>/SensorsData_motorsensors_temperature'
    //   Inport: '<Root>/SensorsData_motorsensors_voltage'

    rtb_BusConversion_InsertedFor_F.jointpositions.position =
      AMC_BLDC_U.SensorsData_jointpositions_posi;
    rtb_BusConversion_InsertedFor_F.motorsensors.Iabc[0] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[0];
    rtb_BusConversion_InsertedFor_F.motorsensors.Iabc[1] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[1];
    rtb_BusConversion_InsertedFor_F.motorsensors.Iabc[2] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[2];
    rtb_BusConversion_InsertedFor_F.motorsensors.angle =
      AMC_BLDC_U.SensorsData_motorsensors_angle;
    rtb_BusConversion_InsertedFor_F.motorsensors.temperature =
      AMC_BLDC_U.SensorsData_motorsensors_temper;
    rtb_BusConversion_InsertedFor_F.motorsensors.voltage =
      AMC_BLDC_U.SensorsData_motorsensors_voltag;
    rtb_BusConversion_InsertedFor_F.motorsensors.current =
      AMC_BLDC_U.SensorsData_motorsensors_curren;
    rtb_BusConversion_InsertedFor_F.motorsensors.hallABC =
      AMC_BLDC_U.SensorsData_motorsensors_hallAB;

    // RateTransition generated from: '<S1>/Adapter5'
    rtw_mutex_lock(AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_c);
    AMC_BLDC_DW.RTBInsertedForAdapter_Insert_de =
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_d;
    rtw_mutex_unlock(AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_c);
    rtb_RTBInsertedForAdapter_Inser =
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_j[AMC_BLDC_DW.RTBInsertedForAdapter_Insert_de];

    // RateTransition generated from: '<S1>/FOC'
    rtw_mutex_lock(AMC_BLDC_DW.TmpRTBAtFOCInport6_d0_SEMAPHORE);
    AMC_BLDC_DW.TmpRTBAtFOCInport6_RDBuf =
      AMC_BLDC_DW.TmpRTBAtFOCInport6_LstBufWR;
    rtw_mutex_unlock(AMC_BLDC_DW.TmpRTBAtFOCInport6_d0_SEMAPHORE);
    rtb_TmpRTBAtFOCInport6 =
      AMC_BLDC_DW.TmpRTBAtFOCInport6_Buf[AMC_BLDC_DW.TmpRTBAtFOCInport6_RDBuf];

    // ModelReference: '<S1>/FOC'
    AMC_BLDC_B.Flags_k.control_mode = ControlModes_Current;
    FOCMDLOBJ1.step(&AMC_BLDC_B.Flags_k, &AMC_BLDC_B.ConfigurationParameters_m,
                    &rtb_BusConversion_InsertedFor_F,
                    &rtb_RTBInsertedForAdapter_Inser, &AMC_BLDC_B.Targets_n,
                    &rtb_TmpRTBAtFOCInport6, &rtb_FOC);
                    
    // TODO: remove
//    static char msg_t1[64] = {};
//    static uint32_t counter_t1;
//    if(counter_t1 % 1000 == 0)
//    {
//        sprintf(msg_t1, "[AMC_BLDC]: %f --> %p\n", AMC_BLDC_B.ConfigurationParameters_m.motorconfig.Vcc, &AMC_BLDC_B.ConfigurationParameters_m);
//        //snprintf(msg_t1, "%f --> %p\n", sizeof(msg_t1), AMC_BLDC_B.ConfigurationParameters_m.motorconfig.Vcc, &AMC_BLDC_B.ConfigurationParameters_m);
//        embot::core::print(msg_t1);
//        //hal_trace_puts(msg_t1);
//        counter_t1 = 0;
//    }
//    counter_t1++;

    // RateTransition generated from: '<S1>/Adapter'
    rtw_mutex_lock(AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_g);
    wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_l +
      1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == AMC_BLDC_DW.RTBInsertedForAdapter_Insert_lf) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock(AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_g);
    switch (wrBufIdx) {
     case 0:
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedF = rtb_FOC;
      break;

     case 1:
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_m = rtb_FOC;
      break;

     case 2:
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_b = rtb_FOC;
      break;
    }

    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_l = wrBufIdx;

    // End of RateTransition generated from: '<S1>/Adapter'
  }

  // Model step function for TID1
  void AMC_BLDC::step_Time()           // Sample time: [0.001s, 0.0s]
  {
    // local block i/o variables
    ControlOuterOutputs rtb_OuterControl;
    BUS_CAN rtb_BusConversion_InsertedFor_C;
    SensorsData rtb_BusConversion_InsertedFor_O;
    int8_T wrBufIdx;

    // BusCreator generated from: '<S1>/OuterControl' incorporates:
    //   Inport: '<Root>/SensorsData_jointpositions_position'
    //   Inport: '<Root>/SensorsData_motorsensors_angle'
    //   Inport: '<Root>/SensorsData_motorsensors_current'
    //   Inport: '<Root>/SensorsData_motorsensors_hallABC'
    //   Inport: '<Root>/SensorsData_motorsensors_temperature'
    //   Inport: '<Root>/SensorsData_motorsensors_voltage'

    rtb_BusConversion_InsertedFor_O.jointpositions.position =
      AMC_BLDC_U.SensorsData_jointpositions_posi;
    rtb_BusConversion_InsertedFor_O.motorsensors.angle =
      AMC_BLDC_U.SensorsData_motorsensors_angle;
    rtb_BusConversion_InsertedFor_O.motorsensors.temperature =
      AMC_BLDC_U.SensorsData_motorsensors_temper;
    rtb_BusConversion_InsertedFor_O.motorsensors.voltage =
      AMC_BLDC_U.SensorsData_motorsensors_voltag;
    rtb_BusConversion_InsertedFor_O.motorsensors.current =
      AMC_BLDC_U.SensorsData_motorsensors_curren;
    rtb_BusConversion_InsertedFor_O.motorsensors.hallABC =
      AMC_BLDC_U.SensorsData_motorsensors_hallAB;

    // BusCreator generated from: '<S2>/Estimation_Velocity' incorporates:
    //   Inport: '<Root>/SensorsData_jointpositions_position'

    AMC_BLDC_B.BusConversion_InsertedFor_Estim.jointpositions.position =
      AMC_BLDC_U.SensorsData_jointpositions_posi;

    // BusCreator generated from: '<S1>/OuterControl' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    rtb_BusConversion_InsertedFor_O.motorsensors.Iabc[0] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[0];

    // BusCreator generated from: '<S2>/Estimation_Velocity' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    AMC_BLDC_B.BusConversion_InsertedFor_Estim.motorsensors.Iabc[0] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[0];

    // BusCreator generated from: '<S1>/OuterControl' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    rtb_BusConversion_InsertedFor_O.motorsensors.Iabc[1] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[1];

    // BusCreator generated from: '<S2>/Estimation_Velocity' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    AMC_BLDC_B.BusConversion_InsertedFor_Estim.motorsensors.Iabc[1] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[1];

    // BusCreator generated from: '<S1>/OuterControl' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'

    rtb_BusConversion_InsertedFor_O.motorsensors.Iabc[2] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[2];

    // BusCreator generated from: '<S2>/Estimation_Velocity' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'
    //   Inport: '<Root>/SensorsData_motorsensors_angle'
    //   Inport: '<Root>/SensorsData_motorsensors_current'
    //   Inport: '<Root>/SensorsData_motorsensors_hallABC'
    //   Inport: '<Root>/SensorsData_motorsensors_temperature'
    //   Inport: '<Root>/SensorsData_motorsensors_voltage'

    AMC_BLDC_B.BusConversion_InsertedFor_Estim.motorsensors.Iabc[2] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[2];
    AMC_BLDC_B.BusConversion_InsertedFor_Estim.motorsensors.angle =
      AMC_BLDC_U.SensorsData_motorsensors_angle;
    AMC_BLDC_B.BusConversion_InsertedFor_Estim.motorsensors.temperature =
      AMC_BLDC_U.SensorsData_motorsensors_temper;
    AMC_BLDC_B.BusConversion_InsertedFor_Estim.motorsensors.voltage =
      AMC_BLDC_U.SensorsData_motorsensors_voltag;
    AMC_BLDC_B.BusConversion_InsertedFor_Estim.motorsensors.current =
      AMC_BLDC_U.SensorsData_motorsensors_curren;
    AMC_BLDC_B.BusConversion_InsertedFor_Estim.motorsensors.hallABC =
      AMC_BLDC_U.SensorsData_motorsensors_hallAB;

    // ModelReference: '<S2>/Estimation_Velocity' incorporates:
    //   Outport: '<Root>/EstimatedData'

    Estimation_VelocityMDLOBJ3.step(AMC_BLDC_B.BusConversion_InsertedFor_Estim,
      AMC_BLDC_Y.EstimatedData_p);

    // RateTransition generated from: '<S1>/Adapter'
    rtw_mutex_lock(AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_g);
    AMC_BLDC_DW.RTBInsertedForAdapter_Insert_lf =
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_l;
    rtw_mutex_unlock(AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_g);
    switch (AMC_BLDC_DW.RTBInsertedForAdapter_Insert_lf) {
     case 0:
      // Outport: '<Root>/ControlOutputs'
      AMC_BLDC_Y.ControlOutputs_p = AMC_BLDC_DW.RTBInsertedForAdapter_InsertedF;
      break;

     case 1:
      // Outport: '<Root>/ControlOutputs'
      AMC_BLDC_Y.ControlOutputs_p = AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_m;
      break;

     case 2:
      // Outport: '<Root>/ControlOutputs'
      AMC_BLDC_Y.ControlOutputs_p = AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_b;
      break;
    }

    // End of RateTransition generated from: '<S1>/Adapter'

    // BusCreator generated from: '<S4>/SupervisorFSM_RX' incorporates:
    //   Inport: '<Root>/SensorsData_jointpositions_position'

    AMC_BLDC_B.BusConversion_InsertedFor_Super.jointpositions.position =
      AMC_BLDC_U.SensorsData_jointpositions_posi;

    // BusCreator generated from: '<S4>/SupervisorFSM_RX' incorporates:
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'
    //   Inport: '<Root>/SensorsData_motorsensors_angle'
    //   Inport: '<Root>/SensorsData_motorsensors_current'
    //   Inport: '<Root>/SensorsData_motorsensors_hallABC'
    //   Inport: '<Root>/SensorsData_motorsensors_temperature'
    //   Inport: '<Root>/SensorsData_motorsensors_voltage'

    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.Iabc[0] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[0];
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.Iabc[1] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[1];
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.Iabc[2] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[2];
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.angle =
      AMC_BLDC_U.SensorsData_motorsensors_angle;
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.temperature =
      AMC_BLDC_U.SensorsData_motorsensors_temper;
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.voltage =
      AMC_BLDC_U.SensorsData_motorsensors_voltag;
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.current =
      AMC_BLDC_U.SensorsData_motorsensors_curren;
    AMC_BLDC_B.BusConversion_InsertedFor_Super.motorsensors.hallABC =
      AMC_BLDC_U.SensorsData_motorsensors_hallAB;

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
    CAN_RX_raw2structMDLOBJ6.step(rtb_BusConversion_InsertedFor_C,
      AMC_BLDC_B.pck_rx_struct);

    // ModelReference: '<S3>/CAN_Decoder'
    CAN_DecoderMDLOBJ4.step(AMC_BLDC_B.pck_rx_struct, AMC_BLDC_B.CAN_Decoder_o1,
      AMC_BLDC_B.CAN_Decoder_o2, AMC_BLDC_B.CAN_Decoder_o3);

    // ModelReference: '<S4>/SupervisorFSM_RX' incorporates:
    //   Outport: '<Root>/ControlOutputs'
    //   Outport: '<Root>/EstimatedData'

    SupervisorFSM_RXMDLOBJ7.step(AMC_BLDC_B.CAN_Decoder_o2,
      AMC_BLDC_B.CAN_Decoder_o1, AMC_BLDC_B.CAN_Decoder_o3,
      AMC_BLDC_Y.EstimatedData_p, AMC_BLDC_B.BusConversion_InsertedFor_Super,
      AMC_BLDC_Y.ControlOutputs_p, AMC_BLDC_B.Targets_n,
      AMC_BLDC_B.ConfigurationParameters_m, AMC_BLDC_B.Flags_k);

    // ModelReference: '<S1>/OuterControl' incorporates:
    //   Outport: '<Root>/EstimatedData'
    
    AMC_BLDC_B.Flags_k.control_mode = ControlModes_Current;

    OuterControlMDLOBJ2.step(AMC_BLDC_B.Flags_k,
      AMC_BLDC_B.ConfigurationParameters_m, AMC_BLDC_B.Targets_n,
      rtb_BusConversion_InsertedFor_O, AMC_BLDC_Y.EstimatedData_p,
      rtb_OuterControl);

    // RateTransition generated from: '<S1>/FOC'
    rtw_mutex_lock(AMC_BLDC_DW.TmpRTBAtFOCInport6_d0_SEMAPHORE);
    wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.TmpRTBAtFOCInport6_LstBufWR + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == AMC_BLDC_DW.TmpRTBAtFOCInport6_RDBuf) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock(AMC_BLDC_DW.TmpRTBAtFOCInport6_d0_SEMAPHORE);
    AMC_BLDC_DW.TmpRTBAtFOCInport6_Buf[wrBufIdx] = rtb_OuterControl;
    AMC_BLDC_DW.TmpRTBAtFOCInport6_LstBufWR = wrBufIdx;

    // End of RateTransition generated from: '<S1>/FOC'

    // BusCreator generated from: '<S4>/SupervisorFSM_TX' incorporates:
    //   Inport: '<Root>/SensorsData_jointpositions_position'
    //   Inport: '<Root>/SensorsData_motorsensors_Iabc'
    //   Inport: '<Root>/SensorsData_motorsensors_angle'
    //   Inport: '<Root>/SensorsData_motorsensors_current'
    //   Inport: '<Root>/SensorsData_motorsensors_hallABC'
    //   Inport: '<Root>/SensorsData_motorsensors_temperature'
    //   Inport: '<Root>/SensorsData_motorsensors_voltage'

    rtb_BusConversion_InsertedFor_O.jointpositions.position =
      AMC_BLDC_U.SensorsData_jointpositions_posi;
    rtb_BusConversion_InsertedFor_O.motorsensors.Iabc[0] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[0];
    rtb_BusConversion_InsertedFor_O.motorsensors.Iabc[1] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[1];
    rtb_BusConversion_InsertedFor_O.motorsensors.Iabc[2] =
      AMC_BLDC_U.SensorsData_motorsensors_Iabc[2];
    rtb_BusConversion_InsertedFor_O.motorsensors.angle =
      AMC_BLDC_U.SensorsData_motorsensors_angle;
    rtb_BusConversion_InsertedFor_O.motorsensors.temperature =
      AMC_BLDC_U.SensorsData_motorsensors_temper;
    rtb_BusConversion_InsertedFor_O.motorsensors.voltage =
      AMC_BLDC_U.SensorsData_motorsensors_voltag;
    rtb_BusConversion_InsertedFor_O.motorsensors.current =
      AMC_BLDC_U.SensorsData_motorsensors_curren;
    rtb_BusConversion_InsertedFor_O.motorsensors.hallABC =
      AMC_BLDC_U.SensorsData_motorsensors_hallAB;

    // ModelReference: '<S4>/SupervisorFSM_TX' incorporates:
    //   Outport: '<Root>/ControlOutputs'
    //   Outport: '<Root>/EstimatedData'

    SupervisorFSM_TXMDLOBJ8.step(AMC_BLDC_B.CAN_Decoder_o1,
      rtb_BusConversion_InsertedFor_O, AMC_BLDC_Y.EstimatedData_p,
      AMC_BLDC_Y.ControlOutputs_p, AMC_BLDC_B.MessagesTx,
      AMC_BLDC_B.SupervisorFSM_TX_o2);

    // ModelReference: '<S3>/CAN_Encoder' incorporates:
    //   Outport: '<Root>/PacketsTx'

    CAN_EncoderMDLOBJ5.step(AMC_BLDC_B.MessagesTx,
      AMC_BLDC_B.SupervisorFSM_TX_o2, AMC_BLDC_Y.PacketsTx);

    // RateTransition generated from: '<S1>/Adapter5' incorporates:
    //   Outport: '<Root>/EstimatedData'

    rtw_mutex_lock(AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_c);
    wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_d +
      1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == AMC_BLDC_DW.RTBInsertedForAdapter_Insert_de) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock(AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_c);
    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_j[wrBufIdx] =
      AMC_BLDC_Y.EstimatedData_p;
    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_d = wrBufIdx;

    // End of RateTransition generated from: '<S1>/Adapter5'
  }

  // Model initialize function
  void AMC_BLDC::initialize()
  {
    // Model Initialize function for ModelReference Block: '<S1>/FOC'

    // Set error status pointer for ModelReference Block: '<S1>/FOC'
    FOCMDLOBJ1.setErrorStatusPointer(rtmGetErrorStatusPointer((&AMC_BLDC_M)));
    FOCMDLOBJ1.initialize();

    // Model Initialize function for ModelReference Block: '<S1>/OuterControl'

    // Set error status pointer for ModelReference Block: '<S1>/OuterControl'
    OuterControlMDLOBJ2.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));
    OuterControlMDLOBJ2.initialize();

    // Model Initialize function for ModelReference Block: '<S2>/Estimation_Velocity' 

    // Set error status pointer for ModelReference Block: '<S2>/Estimation_Velocity' 
    Estimation_VelocityMDLOBJ3.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));
    Estimation_VelocityMDLOBJ3.initialize();

    // Model Initialize function for ModelReference Block: '<S3>/CAN_Decoder'

    // Set error status pointer for ModelReference Block: '<S3>/CAN_Decoder'
    CAN_DecoderMDLOBJ4.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));

    // Model Initialize function for ModelReference Block: '<S3>/CAN_Encoder'

    // Set error status pointer for ModelReference Block: '<S3>/CAN_Encoder'
    CAN_EncoderMDLOBJ5.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));
    CAN_EncoderMDLOBJ5.initialize();

    // Model Initialize function for ModelReference Block: '<S3>/CAN_RX_raw2struct' 

    // Set error status pointer for ModelReference Block: '<S3>/CAN_RX_raw2struct' 
    CAN_RX_raw2structMDLOBJ6.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));

    // Model Initialize function for ModelReference Block: '<S4>/SupervisorFSM_RX' 

    // Set error status pointer for ModelReference Block: '<S4>/SupervisorFSM_RX' 
    SupervisorFSM_RXMDLOBJ7.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));

    // Model Initialize function for ModelReference Block: '<S4>/SupervisorFSM_TX' 

    // Set error status pointer for ModelReference Block: '<S4>/SupervisorFSM_TX' 
    SupervisorFSM_TXMDLOBJ8.setErrorStatusPointer(rtmGetErrorStatusPointer
      ((&AMC_BLDC_M)));

    // Start for RateTransition generated from: '<S1>/Adapter5'
    rtw_mutex_init(&AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_c);

    // Start for RateTransition generated from: '<S1>/FOC'
    rtw_mutex_init(&AMC_BLDC_DW.TmpRTBAtFOCInport6_d0_SEMAPHORE);

    // Start for RateTransition generated from: '<S1>/Adapter'
    rtw_mutex_init(&AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_g);

    // SystemInitialize for ModelReference: '<S1>/FOC'
    FOCMDLOBJ1.init();

    // SystemInitialize for ModelReference: '<S1>/OuterControl'
    OuterControlMDLOBJ2.init();

    // SystemInitialize for ModelReference: '<S2>/Estimation_Velocity'
    Estimation_VelocityMDLOBJ3.init();

    // SystemInitialize for ModelReference: '<S3>/CAN_Decoder'
    CAN_DecoderMDLOBJ4.init(&AMC_BLDC_B.CAN_Decoder_o1,
      &AMC_BLDC_B.CAN_Decoder_o2, &AMC_BLDC_B.CAN_Decoder_o3);

    // SystemInitialize for ModelReference: '<S4>/SupervisorFSM_RX'
    SupervisorFSM_RXMDLOBJ7.init(&AMC_BLDC_B.Flags_k, &AMC_BLDC_B.Targets_n,
      &AMC_BLDC_B.ConfigurationParameters_m);

    // SystemInitialize for ModelReference: '<S4>/SupervisorFSM_TX'
    SupervisorFSM_TXMDLOBJ8.init(&AMC_BLDC_B.MessagesTx,
      &AMC_BLDC_B.SupervisorFSM_TX_o2);
  }

  // Model terminate function
  void AMC_BLDC::terminate()
  {
    // Terminate for RateTransition generated from: '<S1>/Adapter5'
    rtw_mutex_destroy(AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_c);

    // Terminate for RateTransition generated from: '<S1>/FOC'
    rtw_mutex_destroy(AMC_BLDC_DW.TmpRTBAtFOCInport6_d0_SEMAPHORE);

    // Terminate for RateTransition generated from: '<S1>/Adapter'
    rtw_mutex_destroy(AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_g);
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
