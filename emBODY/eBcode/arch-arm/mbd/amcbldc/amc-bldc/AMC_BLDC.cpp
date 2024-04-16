//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: AMC_BLDC.cpp
//
// Code generated for Simulink model 'AMC_BLDC'.
//
// Model version                  : 7.83
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Tue Apr 16 11:32:02 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "AMC_BLDC.h"
#include "rtw_mutex.h"
#include "rtwtypes.h"
#include "AMC_BLDC_types.h"
#include "can_decoder.h"
#include "SupervisorFSM_TX.h"
#include "can_encoder.h"
#include "motion_controller_single.h"
#include "rtw_defines.h"

// Exported block parameters
ActuatorConfiguration AmcbldcInitConf = {
  {
    40000.0F,
    0.35F,
    0.7F,
    1.0F,
    32000U,
    0.0F
  },

  {
    {
      ControlModes_Current,
      0.0F,
      0.0F,
      2.0F,
      500.0F,
      0.0F,
      10.0F,
      0.0F,
      0.0F,
      0U
    },

    {
      ControlModes_Velocity,
      0.0F,
      0.0F,
      -3.0e-5F,
      -3.0e-5F,
      0.0F,
      10.0F,
      0.0F,
      0.0F,
      0U
    },

    {
      ControlModes_Position,
      0.0F,
      0.0F,
      0.0F,
      0.0F,
      0.0F,
      0.0F,
      0.0F,
      0.0F,
      0U
    }
  },

  {
    {
      false,
      true,
      false,
      false,
      false,
      0U,
      7U,
      16000,
      0,
      false
    },
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    24.0F,
    25.9F,
    271.0F,
    16.0F,
    797.5F,
    30.0F
  }
} ;                                    // Variable: AmcbldcInitConf
                                          //  Referenced by: '<Root>/Motion Controller Single'


real32_T CAN_ANGLE_DEG2ICUB = 182.044449F;// Variable: CAN_ANGLE_DEG2ICUB
                                             //  Referenced by: '<S2>/CAN_Encoder'
                                             //  2^16/360

real32_T CAN_ANGLE_ICUB2DEG = 0.00549316406F;// Variable: CAN_ANGLE_ICUB2DEG
                                                //  Referenced by: '<S2>/CAN_Decoder'
                                                //  360/2^16

uint8_T CAN_ID_AMC = 3U;               // Variable: CAN_ID_AMC
                                          //  Referenced by:
                                          //    '<S2>/CAN_Decoder'
                                          //    '<S2>/CAN_Encoder'
                                          //  4 bits defining the ID of the AMC_BLDC board.


// Block signals (default storage)
B_AMC_BLDC_T AMC_BLDC_B;

// Block states (default storage)
DW_AMC_BLDC_T AMC_BLDC_DW;

// External inputs (root inport signals with default storage)
ExtU_AMC_BLDC_T AMC_BLDC_U;

// External outputs (root outports fed by signals with default storage)
ExtY_AMC_BLDC_T AMC_BLDC_Y;

// Real-time model
RT_MODEL_AMC_BLDC_T AMC_BLDC_M_ = RT_MODEL_AMC_BLDC_T();
RT_MODEL_AMC_BLDC_T *const AMC_BLDC_M = &AMC_BLDC_M_;

// Model step function for TID0
void AMC_BLDC_step0(void)              // Sample time: [5.0E-6s, 0.0s]
{
  // ModelReference: '<Root>/Motion Controller Single'
  motion_controller_singleTID0();
}

// Model step function for TID1
void AMC_BLDC_step_FOC(void)           // Sample time: [4.5E-5s, 0.0s]
{
  int8_T wrBufIdx;

  // ModelReference: '<Root>/Motion Controller Single' incorporates:
  //   Inport generated from: '<Root>/In Bus Element6'
  //   Outport generated from: '<Root>/Out Bus Element'

  motion_controller_singleTID1(&AMC_BLDC_U.SensorsData_p,
    &AMC_BLDC_Y.ControlOutputs,
    &(AMC_BLDC_DW.MotionControllerSingle_InstanceData.rtdw));

  // RateTransition generated from: '<Root>/SupervisorFSM_TX' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element'

  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport4_LstBufWR + 1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx == AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport4_RDBuf) {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  switch (wrBufIdx) {
   case 0:
    AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport4_Buf0 = AMC_BLDC_Y.ControlOutputs;
    break;

   case 1:
    AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport4_Buf1 = AMC_BLDC_Y.ControlOutputs;
    break;

   case 2:
    AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport4_Buf2 = AMC_BLDC_Y.ControlOutputs;
    break;
  }

  AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport4_LstBufWR = wrBufIdx;

  // End of RateTransition generated from: '<Root>/SupervisorFSM_TX'
}

// Model step function for TID2
void AMC_BLDC_step_1ms(void)           // Sample time: [0.001s, 0.0s]
{
  // local block i/o variables
  BUS_STATUS_TX rtb_SupervisorFSM_TX_o2;
  FOCOutputs rtb_TmpRTBAtSupervisorFSM_TXInport4;

  // UnitDelay generated from: '<Root>/Adapter'
  AMC_BLDC_B.ZOHBlockInsertedForAdapter_InsertedFor_Adapter_at_outport_0 =
    AMC_BLDC_DW.ZOHBlockInsertedForAdapter_InsertedFor_Adapter_at_outport_0_;

  // ModelReference: '<S2>/CAN_Decoder' incorporates:
  //   Inport generated from: '<Root>/In Bus Element2'

  can_decoder(&AMC_BLDC_U.PacketsRx,
              &AMC_BLDC_B.ZOHBlockInsertedForAdapter_InsertedFor_Adapter_at_outport_0,
              &AMC_BLDC_B.CAN_Decoder[0],
              &(AMC_BLDC_DW.CAN_Decoder_InstanceData.rtb),
              &(AMC_BLDC_DW.CAN_Decoder_InstanceData.rtdw));

  // ModelReference: '<Root>/Motion Controller Single' incorporates:
  //   Inport generated from: '<Root>/In Bus Element4'
  //   Outport generated from: '<Root>/Out Bus Element3'
  //   Outport generated from: '<Root>/Out Bus Element2'
  //   Outport generated from: '<Root>/Out Bus Element4'

  mc_1ms_tick(&AMC_BLDC_U.ExternalFlags_p, &AMC_BLDC_B.CAN_Decoder[0],
              &AMC_BLDC_Y.EstimatedData_p, &AMC_BLDC_Y.Flags_p,
              &AMC_BLDC_Y.ConfigurationParameters,
              &(AMC_BLDC_DW.MotionControllerSingle_InstanceData.rtb),
              &(AMC_BLDC_DW.MotionControllerSingle_InstanceData.rtdw));

  // RateTransition generated from: '<Root>/SupervisorFSM_TX'
  rtw_mutex_lock();
  AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport4_RDBuf =
    AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport4_LstBufWR;
  rtw_mutex_unlock();
  switch (AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport4_RDBuf) {
   case 0:
    rtb_TmpRTBAtSupervisorFSM_TXInport4 =
      AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport4_Buf0;
    break;

   case 1:
    rtb_TmpRTBAtSupervisorFSM_TXInport4 =
      AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport4_Buf1;
    break;

   case 2:
    rtb_TmpRTBAtSupervisorFSM_TXInport4 =
      AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport4_Buf2;
    break;
  }

  // End of RateTransition generated from: '<Root>/SupervisorFSM_TX'

  // ModelReference generated from: '<Root>/SupervisorFSM_TX' incorporates:
  //   Inport generated from: '<Root>/In Bus Element4'
  //   Inport generated from: '<Root>/In Bus Element6'
  //   Outport generated from: '<Root>/Out Bus Element2'
  //   Outport generated from: '<Root>/Out Bus Element4'

  SupervisorFSM_TX(&AMC_BLDC_U.SensorsData_p, &AMC_BLDC_Y.EstimatedData_p,
                   &AMC_BLDC_Y.Flags_p, &rtb_TmpRTBAtSupervisorFSM_TXInport4,
                   &AMC_BLDC_U.ExternalFlags_p.fault_button,
                   &AMC_BLDC_B.MessagesTx, &rtb_SupervisorFSM_TX_o2,
                   &(AMC_BLDC_DW.SupervisorFSM_TX_InstanceData.rtb),
                   &(AMC_BLDC_DW.SupervisorFSM_TX_InstanceData.rtdw));

  // ModelReference: '<S2>/CAN_Encoder' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element1'

  can_encoder(&AMC_BLDC_B.MessagesTx, &rtb_SupervisorFSM_TX_o2,
              &AMC_BLDC_B.ZOHBlockInsertedForAdapter_InsertedFor_Adapter_at_outport_0,
              &AMC_BLDC_Y.PacketsTx, &(AMC_BLDC_DW.CAN_Encoder_InstanceData.rtb));

  // Update for UnitDelay generated from: '<Root>/Adapter' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element3'

  AMC_BLDC_DW.ZOHBlockInsertedForAdapter_InsertedFor_Adapter_at_outport_0_ =
    AMC_BLDC_Y.ConfigurationParameters;
}

// Model initialize function
void AMC_BLDC_initialize(void)
{
  // Model Initialize function for ModelReference Block: '<S2>/CAN_Decoder'
  can_decoder_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M),
    &(AMC_BLDC_DW.CAN_Decoder_InstanceData.rtm));

  // Model Initialize function for ModelReference Block: '<S2>/CAN_Encoder'
  can_encoder_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M),
    &(AMC_BLDC_DW.CAN_Encoder_InstanceData.rtm));

  // Model Initialize function for ModelReference Block: '<Root>/Motion Controller Single' 
  mc_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M),
                &(AMC_BLDC_DW.MotionControllerSingle_InstanceData.rtm),
                &(AMC_BLDC_DW.MotionControllerSingle_InstanceData.rtdw));

  // Model Initialize function for ModelReference Block: '<Root>/SupervisorFSM_TX' 
  SupervisorFSM_TX_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M),
    &(AMC_BLDC_DW.SupervisorFSM_TX_InstanceData.rtm));

  // Start for RateTransition generated from: '<Root>/SupervisorFSM_TX'
  rtw_mutex_init();

  // SystemInitialize for ModelReference: '<S2>/CAN_Decoder'
  can_decoder_Init(&(AMC_BLDC_DW.CAN_Decoder_InstanceData.rtb),
                   &(AMC_BLDC_DW.CAN_Decoder_InstanceData.rtdw));

  // SystemInitialize for ModelReference: '<Root>/Motion Controller Single' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element3'
  //   Outport generated from: '<Root>/Out Bus Element4'

  motion_controller_single_Init(&AMC_BLDC_Y.Flags_p,
    &AMC_BLDC_Y.ConfigurationParameters,
    &(AMC_BLDC_DW.MotionControllerSingle_InstanceData.rtdw));

  // SystemInitialize for ModelReference generated from: '<Root>/SupervisorFSM_TX' 
  SupervisorFSM_TX_Init(&AMC_BLDC_B.MessagesTx);

  // Enable for ModelReference: '<Root>/Motion Controller Single'
  motion_controller_single_Enable
    (&(AMC_BLDC_DW.MotionControllerSingle_InstanceData.rtdw));
}

// Model terminate function
void AMC_BLDC_terminate(void)
{
  // Terminate for ModelReference: '<Root>/Motion Controller Single'
  mc_terminate(&(AMC_BLDC_DW.MotionControllerSingle_InstanceData.rtdw));

  // Terminate for RateTransition generated from: '<Root>/SupervisorFSM_TX'
  rtw_mutex_destroy();
}

//
// File trailer for generated code.
//
// [EOF]
//
