//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: AMC_BLDC.cpp
//
// Code generated for Simulink model 'AMC_BLDC'.
//
// Model version                  : 11.0
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Thu Oct  9 17:31:53 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "AMC_BLDC.h"
#include "rtw_mutex.h"
#include "AMC_BLDC_types.h"
#include "rtwtypes.h"
#include "process_sensors.h"
#include "control_foc.h"
#include "can_decoder.h"
#include "motion_controller_single.h"
#include "SupervisorFSM_TX.h"
#include "can_encoder.h"

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
      -20.0F,
      -20.0F,
      0.0F,
      100.0F,
      0.0F,
      0.0F,
      10U
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
      0,
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
    30.0F,
    false,
    ReferenceEncoder_Motor
  }
} ;                                    // Variable: AmcbldcInitConf
                                          //  Referenced by: '<Root>/Motion Controller Single'


real32_T CAN_ANGLE_DEG2ICUB = 182.044449F;// Variable: CAN_ANGLE_DEG2ICUB
                                             //  Referenced by: '<S4>/CAN_Encoder'
                                             //  2^16/360

real32_T CAN_ANGLE_ICUB2DEG = 0.00549316406F;// Variable: CAN_ANGLE_ICUB2DEG
                                                //  Referenced by: '<S4>/CAN_Decoder'
                                                //  360/2^16

uint8_T CAN_ID_AMC = 3U;               // Variable: CAN_ID_AMC
                                          //  Referenced by:
                                          //    '<S4>/CAN_Decoder'
                                          //    '<S4>/CAN_Encoder'
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
void AMC_BLDC_step0(void)              // Sample time: [5e-06s, 0.0s]
{
  // (no output/update code required)
}

// Model step function for TID1
void AMC_BLDC_step_FOC(void)           // Sample time: [4.5e-05s, 0.0s]
{
  // local block i/o variables
  SensorsData rtb_ProcessSensors;
  ActuatorConfiguration rtb_TmpRTBAtProcessSensorsInport1;
  Flags rtb_TmpRTBAtProcessSensorsInport2;
  SensorsData rtb_TmpRTBAtFOCInport4;
  int8_T wrBufIdx;

  // RateTransition generated from: '<Root>/Process Sensors'
  rtw_mutex_lock();
  AMC_BLDC_DW.TmpRTBAtProcessSensorsInport1_RDBuf =
    AMC_BLDC_DW.TmpRTBAtProcessSensorsInport1_LstBufWR;
  rtw_mutex_unlock();
  rtb_TmpRTBAtProcessSensorsInport1 =
    AMC_BLDC_DW.TmpRTBAtProcessSensorsInport1_Buf[AMC_BLDC_DW.TmpRTBAtProcessSensorsInport1_RDBuf];

  // RateTransition generated from: '<Root>/Process Sensors'
  rtw_mutex_lock();
  AMC_BLDC_DW.TmpRTBAtProcessSensorsInport2_RDBuf =
    AMC_BLDC_DW.TmpRTBAtProcessSensorsInport2_LstBufWR;
  rtw_mutex_unlock();
  rtb_TmpRTBAtProcessSensorsInport2 =
    AMC_BLDC_DW.TmpRTBAtProcessSensorsInport2_Buf[AMC_BLDC_DW.TmpRTBAtProcessSensorsInport2_RDBuf];

  // ModelReference generated from: '<Root>/Process Sensors' incorporates:
  //   Inport generated from: '<Root>/In Bus Element7'

  process_sensors(&rtb_TmpRTBAtProcessSensorsInport1,
                  &rtb_TmpRTBAtProcessSensorsInport2, &AMC_BLDC_U.SensorsData_p,
                  &rtb_ProcessSensors,
                  &(AMC_BLDC_DW.ProcessSensors_InstanceData.rtb),
                  &(AMC_BLDC_DW.ProcessSensors_InstanceData.rtdw),
                  &(AMC_BLDC_DW.ProcessSensors_InstanceData.rtzce));

  // RateTransition generated from: '<Root>/FOC'
  rtw_mutex_lock();
  AMC_BLDC_DW.TmpRTBAtFOCInport2_RDBuf = AMC_BLDC_DW.TmpRTBAtFOCInport2_LstBufWR;
  rtw_mutex_unlock();

  // RateTransition generated from: '<Root>/FOC'
  AMC_BLDC_B.TmpRTBAtFOCInport2 =
    AMC_BLDC_DW.TmpRTBAtFOCInport2_Buf[AMC_BLDC_DW.TmpRTBAtFOCInport2_RDBuf];

  // RateTransition generated from: '<Root>/FOC'
  rtw_mutex_lock();
  AMC_BLDC_DW.TmpRTBAtFOCInport3_RDBuf = AMC_BLDC_DW.TmpRTBAtFOCInport3_LstBufWR;
  rtw_mutex_unlock();
  rtb_TmpRTBAtProcessSensorsInport2 =
    AMC_BLDC_DW.TmpRTBAtFOCInport3_Buf[AMC_BLDC_DW.TmpRTBAtFOCInport3_RDBuf];

  // RateTransition generated from: '<Root>/FOC'
  rtw_mutex_lock();
  AMC_BLDC_DW.TmpRTBAtFOCInport4_RDBuf = AMC_BLDC_DW.TmpRTBAtFOCInport4_LstBufWR;
  rtw_mutex_unlock();
  rtb_TmpRTBAtFOCInport4 =
    AMC_BLDC_DW.TmpRTBAtFOCInport4_Buf[AMC_BLDC_DW.TmpRTBAtFOCInport4_RDBuf];

  // ModelReference generated from: '<Root>/FOC' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element'

  control_foc(&rtb_ProcessSensors, &AMC_BLDC_B.TmpRTBAtFOCInport2,
              &rtb_TmpRTBAtProcessSensorsInport2, &rtb_TmpRTBAtFOCInport4,
              &AMC_BLDC_Y.ControlOutputs, &(AMC_BLDC_DW.FOC_InstanceData.rtb),
              &(AMC_BLDC_DW.FOC_InstanceData.rtdw),
              &(AMC_BLDC_DW.FOC_InstanceData.rtzce));

  // RateTransition generated from: '<Root>/Component2' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element'

  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0_Ls +
     1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx ==
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0_RD)
  {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  switch (wrBufIdx) {
   case 0:
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0_Bu =
      AMC_BLDC_Y.ControlOutputs;
    break;

   case 1:
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0__h =
      AMC_BLDC_Y.ControlOutputs;
    break;

   case 2:
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0__a =
      AMC_BLDC_Y.ControlOutputs;
    break;
  }

  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0_Ls =
    wrBufIdx;

  // End of RateTransition generated from: '<Root>/Component2'

  // RateTransition generated from: '<Root>/Motion Controller Single' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element'

  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport4_LstBufWR + 1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx == AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport4_RDBuf) {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  switch (wrBufIdx) {
   case 0:
    AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport4_Buf0 =
      AMC_BLDC_Y.ControlOutputs;
    break;

   case 1:
    AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport4_Buf1 =
      AMC_BLDC_Y.ControlOutputs;
    break;

   case 2:
    AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport4_Buf2 =
      AMC_BLDC_Y.ControlOutputs;
    break;
  }

  AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport4_LstBufWR = wrBufIdx;

  // End of RateTransition generated from: '<Root>/Motion Controller Single'

  // RateTransition generated from: '<Root>/SupervisorFSM_TX'
  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport1_LstBufWR + 1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx == AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport1_RDBuf) {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  switch (wrBufIdx) {
   case 0:
    AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport1_Buf0 = rtb_ProcessSensors;
    break;

   case 1:
    AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport1_Buf1 = rtb_ProcessSensors;
    break;

   case 2:
    AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport1_Buf2 = rtb_ProcessSensors;
    break;
  }

  AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport1_LstBufWR = wrBufIdx;

  // End of RateTransition generated from: '<Root>/SupervisorFSM_TX'

  // RateTransition generated from: '<Root>/Motion Controller Single'
  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport1_LstBufWR + 1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx == AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport1_RDBuf) {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  switch (wrBufIdx) {
   case 0:
    AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport1_Buf0 = rtb_ProcessSensors;
    break;

   case 1:
    AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport1_Buf1 = rtb_ProcessSensors;
    break;

   case 2:
    AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport1_Buf2 = rtb_ProcessSensors;
    break;
  }

  AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport1_LstBufWR = wrBufIdx;

  // End of RateTransition generated from: '<Root>/Motion Controller Single'
}

// Model step function for TID2
void AMC_BLDC_step_1ms(void)           // Sample time: [0.001s, 0.0s]
{
  FOCOutputs rtb_RTBInsertedForAdapter_InsertedFor_Component2_at_outport_;
  FOCOutputs rtb_TmpRTBAtMotionControllerSingleInport4;
  SensorsData rtb_TmpRTBAtMotionControllerSingleInport1;
  SensorsData rtb_TmpRTBAtSupervisorFSM_TXInport1;
  int8_T wrBufIdx;

  // ModelReference: '<S4>/CAN_Decoder' incorporates:
  //   Inport generated from: '<Root>/In Bus Element2'

  can_decoder(&AMC_BLDC_U.PacketsRx, &AMC_BLDC_B.CAN_Decoder[0],
              &(AMC_BLDC_DW.CAN_Decoder_InstanceData.rtb),
              &(AMC_BLDC_DW.CAN_Decoder_InstanceData.rtdw));

  // RateTransition generated from: '<Root>/Motion Controller Single'
  rtw_mutex_lock();
  AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport1_RDBuf =
    AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport1_LstBufWR;
  rtw_mutex_unlock();
  switch (AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport1_RDBuf) {
   case 0:
    rtb_TmpRTBAtMotionControllerSingleInport1 =
      AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport1_Buf0;
    break;

   case 1:
    rtb_TmpRTBAtMotionControllerSingleInport1 =
      AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport1_Buf1;
    break;

   case 2:
    rtb_TmpRTBAtMotionControllerSingleInport1 =
      AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport1_Buf2;
    break;
  }

  // End of RateTransition generated from: '<Root>/Motion Controller Single'

  // RateTransition generated from: '<Root>/Motion Controller Single'
  rtw_mutex_lock();
  AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport4_RDBuf =
    AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport4_LstBufWR;
  rtw_mutex_unlock();
  switch (AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport4_RDBuf) {
   case 0:
    rtb_TmpRTBAtMotionControllerSingleInport4 =
      AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport4_Buf0;
    break;

   case 1:
    rtb_TmpRTBAtMotionControllerSingleInport4 =
      AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport4_Buf1;
    break;

   case 2:
    rtb_TmpRTBAtMotionControllerSingleInport4 =
      AMC_BLDC_DW.TmpRTBAtMotionControllerSingleInport4_Buf2;
    break;
  }

  // End of RateTransition generated from: '<Root>/Motion Controller Single'

  // ModelReference: '<Root>/Motion Controller Single' incorporates:
  //   Inport generated from: '<Root>/In Bus Element1'
  //   Outport generated from: '<Root>/Out Bus Element3'
  //   Outport generated from: '<Root>/Out Bus Element2'
  //   Outport generated from: '<Root>/Out Bus Element4'

  mc_1ms_tick(&rtb_TmpRTBAtMotionControllerSingleInport1,
              &AMC_BLDC_U.ExternalFlags_p, &AMC_BLDC_B.CAN_Decoder[0],
              &rtb_TmpRTBAtMotionControllerSingleInport4,
              &AMC_BLDC_Y.EstimatedData_p, &AMC_BLDC_Y.Flags_p,
              &AMC_BLDC_Y.ConfigurationParameters,
              &AMC_BLDC_B.MotionControllerSingle_o4,
              &AMC_BLDC_B.SensorsDataCalibration,
              &(AMC_BLDC_DW.MotionControllerSingle_InstanceData.rtdw));

  // RateTransition generated from: '<Root>/FOC'
  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.TmpRTBAtFOCInport2_LstBufWR + 1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx == AMC_BLDC_DW.TmpRTBAtFOCInport2_RDBuf) {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  AMC_BLDC_DW.TmpRTBAtFOCInport2_Buf[wrBufIdx] =
    AMC_BLDC_B.MotionControllerSingle_o4;
  AMC_BLDC_DW.TmpRTBAtFOCInport2_LstBufWR = wrBufIdx;

  // RateTransition generated from: '<Root>/FOC' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element4'

  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.TmpRTBAtFOCInport3_LstBufWR + 1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx == AMC_BLDC_DW.TmpRTBAtFOCInport3_RDBuf) {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  AMC_BLDC_DW.TmpRTBAtFOCInport3_Buf[wrBufIdx] = AMC_BLDC_Y.Flags_p;
  AMC_BLDC_DW.TmpRTBAtFOCInport3_LstBufWR = wrBufIdx;

  // RateTransition generated from: '<Root>/FOC'
  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.TmpRTBAtFOCInport4_LstBufWR + 1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx == AMC_BLDC_DW.TmpRTBAtFOCInport4_RDBuf) {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  AMC_BLDC_DW.TmpRTBAtFOCInport4_Buf[wrBufIdx] =
    AMC_BLDC_B.SensorsDataCalibration;
  AMC_BLDC_DW.TmpRTBAtFOCInport4_LstBufWR = wrBufIdx;

  // RateTransition generated from: '<Root>/SupervisorFSM_TX'
  rtw_mutex_lock();
  AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport1_RDBuf =
    AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport1_LstBufWR;
  rtw_mutex_unlock();
  switch (AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport1_RDBuf) {
   case 0:
    rtb_TmpRTBAtSupervisorFSM_TXInport1 =
      AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport1_Buf0;
    break;

   case 1:
    rtb_TmpRTBAtSupervisorFSM_TXInport1 =
      AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport1_Buf1;
    break;

   case 2:
    rtb_TmpRTBAtSupervisorFSM_TXInport1 =
      AMC_BLDC_DW.TmpRTBAtSupervisorFSM_TXInport1_Buf2;
    break;
  }

  // End of RateTransition generated from: '<Root>/SupervisorFSM_TX'

  // RateTransition generated from: '<Root>/Component2'
  rtw_mutex_lock();
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0_RD =
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0_Ls;
  rtw_mutex_unlock();
  switch
    (AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0_RD) {
   case 0:
    rtb_RTBInsertedForAdapter_InsertedFor_Component2_at_outport_ =
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0_Bu;
    break;

   case 1:
    rtb_RTBInsertedForAdapter_InsertedFor_Component2_at_outport_ =
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0__h;
    break;

   case 2:
    rtb_RTBInsertedForAdapter_InsertedFor_Component2_at_outport_ =
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0__a;
    break;
  }

  // End of RateTransition generated from: '<Root>/Component2'

  // ModelReference generated from: '<Root>/SupervisorFSM_TX' incorporates:
  //   Inport generated from: '<Root>/In Bus Element1'
  //   Outport generated from: '<Root>/Out Bus Element2'
  //   Outport generated from: '<Root>/Out Bus Element4'

  SupervisorFSM_TX(&rtb_TmpRTBAtSupervisorFSM_TXInport1,
                   &AMC_BLDC_Y.EstimatedData_p, &AMC_BLDC_Y.Flags_p,
                   &rtb_RTBInsertedForAdapter_InsertedFor_Component2_at_outport_,
                   &AMC_BLDC_U.ExternalFlags_p.fault_button,
                   &AMC_BLDC_B.MessagesTx, &AMC_BLDC_B.SupervisorFSM_TX_o2,
                   &(AMC_BLDC_DW.SupervisorFSM_TX_InstanceData.rtdw));

  // ModelReference: '<S4>/CAN_Encoder' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element1'

  can_encoder(&AMC_BLDC_B.MessagesTx, &AMC_BLDC_B.SupervisorFSM_TX_o2,
              &AMC_BLDC_Y.PacketsTx);

  // RateTransition generated from: '<Root>/Process Sensors' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element3'

  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (AMC_BLDC_DW.TmpRTBAtProcessSensorsInport1_LstBufWR + 1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx == AMC_BLDC_DW.TmpRTBAtProcessSensorsInport1_RDBuf) {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  AMC_BLDC_DW.TmpRTBAtProcessSensorsInport1_Buf[wrBufIdx] =
    AMC_BLDC_Y.ConfigurationParameters;
  AMC_BLDC_DW.TmpRTBAtProcessSensorsInport1_LstBufWR = wrBufIdx;

  // RateTransition generated from: '<Root>/Process Sensors' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element4'

  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (AMC_BLDC_DW.TmpRTBAtProcessSensorsInport2_LstBufWR + 1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx == AMC_BLDC_DW.TmpRTBAtProcessSensorsInport2_RDBuf) {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  AMC_BLDC_DW.TmpRTBAtProcessSensorsInport2_Buf[wrBufIdx] = AMC_BLDC_Y.Flags_p;
  AMC_BLDC_DW.TmpRTBAtProcessSensorsInport2_LstBufWR = wrBufIdx;
}

// Model initialize function
void AMC_BLDC_initialize(void)
{
  // Registration code

  // Set task counter limit used by the static main program
  (AMC_BLDC_M)->Timing.TaskCounters.cLimit[0] = 1;
  (AMC_BLDC_M)->Timing.TaskCounters.cLimit[1] = 9;
  (AMC_BLDC_M)->Timing.TaskCounters.cLimit[2] = 200;

  // Model Initialize function for ModelReference Block: '<S4>/CAN_Decoder'
  can_decoder_initialize(AMC_BLDC_M->getErrorStatusPointer(),
    &(AMC_BLDC_DW.CAN_Decoder_InstanceData.rtm));

  // Model Initialize function for ModelReference Block: '<S4>/CAN_Encoder'
  can_encoder_initialize(AMC_BLDC_M->getErrorStatusPointer(),
    &(AMC_BLDC_DW.CAN_Encoder_InstanceData.rtm));

  // Model Initialize function for ModelReference Block: '<Root>/Motion Controller Single' 
  mc_initialize(AMC_BLDC_M->getErrorStatusPointer(),
                &(AMC_BLDC_DW.MotionControllerSingle_InstanceData.rtm),
                &(AMC_BLDC_DW.MotionControllerSingle_InstanceData.rtdw));

  // Model Initialize function for ModelReference Block: '<Root>/FOC'
  control_foc_initialize(&(AMC_BLDC_DW.FOC_InstanceData.rtzce));

  // Model Initialize function for ModelReference Block: '<Root>/Process Sensors' 
  process_sensors_initialize(&(AMC_BLDC_DW.ProcessSensors_InstanceData.rtzce));

  // Start for RateTransition generated from: '<Root>/Process Sensors'
  rtw_mutex_init();

  // Start for RateTransition generated from: '<Root>/Process Sensors'
  rtw_mutex_init();

  // Start for RateTransition generated from: '<Root>/FOC'
  rtw_mutex_init();

  // Start for RateTransition generated from: '<Root>/FOC'
  rtw_mutex_init();

  // Start for RateTransition generated from: '<Root>/FOC'
  rtw_mutex_init();

  // Start for RateTransition generated from: '<Root>/Component2'
  rtw_mutex_init();

  // Start for RateTransition generated from: '<Root>/Motion Controller Single'
  rtw_mutex_init();

  // Start for RateTransition generated from: '<Root>/SupervisorFSM_TX'
  rtw_mutex_init();

  // Start for RateTransition generated from: '<Root>/Motion Controller Single'
  rtw_mutex_init();

  // SystemInitialize for ModelReference generated from: '<Root>/FOC'
  control_foc_Init(&(AMC_BLDC_DW.FOC_InstanceData.rtdw));

  // SystemInitialize for ModelReference: '<S4>/CAN_Decoder'
  can_decoder_Init(&(AMC_BLDC_DW.CAN_Decoder_InstanceData.rtb),
                   &(AMC_BLDC_DW.CAN_Decoder_InstanceData.rtdw));

  // SystemInitialize for ModelReference: '<Root>/Motion Controller Single' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element3'
  //   Outport generated from: '<Root>/Out Bus Element4'

  motion_controller_single_Init(&AMC_BLDC_Y.Flags_p,
    &AMC_BLDC_Y.ConfigurationParameters, &AMC_BLDC_B.MotionControllerSingle_o4,
    &AMC_BLDC_B.SensorsDataCalibration,
    &(AMC_BLDC_DW.MotionControllerSingle_InstanceData.rtdw));

  // SystemInitialize for ModelReference generated from: '<Root>/SupervisorFSM_TX' 
  SupervisorFSM_TX_Init(&AMC_BLDC_B.MessagesTx, &AMC_BLDC_B.SupervisorFSM_TX_o2);

  // Enable for ModelReference: '<Root>/Motion Controller Single'
  motion_controller_single_Enable
    (&(AMC_BLDC_DW.MotionControllerSingle_InstanceData.rtdw));
}

// Model terminate function
void AMC_BLDC_terminate(void)
{
  // Terminate for RateTransition generated from: '<Root>/Process Sensors'
  rtw_mutex_destroy();

  // Terminate for RateTransition generated from: '<Root>/Process Sensors'
  rtw_mutex_destroy();

  // Terminate for RateTransition generated from: '<Root>/FOC'
  rtw_mutex_destroy();

  // Terminate for RateTransition generated from: '<Root>/FOC'
  rtw_mutex_destroy();

  // Terminate for RateTransition generated from: '<Root>/FOC'
  rtw_mutex_destroy();

  // Terminate for RateTransition generated from: '<Root>/Component2'
  rtw_mutex_destroy();

  // Terminate for RateTransition generated from: '<Root>/Motion Controller Single' 
  rtw_mutex_destroy();

  // Terminate for RateTransition generated from: '<Root>/SupervisorFSM_TX'
  rtw_mutex_destroy();

  // Terminate for RateTransition generated from: '<Root>/Motion Controller Single' 
  rtw_mutex_destroy();

  // Terminate for ModelReference: '<Root>/Motion Controller Single'
  mc_terminate(&(AMC_BLDC_DW.MotionControllerSingle_InstanceData.rtdw));
}

boolean_T RT_MODEL_AMC_BLDC_T::StepTask(int32_T idx) const
{
  return (Timing.TaskCounters.TID[(idx)] == 0);
}

uint32_T &RT_MODEL_AMC_BLDC_T::CounterLimit(int32_T idx)
{
  return (Timing.TaskCounters.cLimit[(idx)]);
}

const char_T* RT_MODEL_AMC_BLDC_T::getErrorStatus() const
{
  return (errorStatus);
}

void RT_MODEL_AMC_BLDC_T::setErrorStatus(const char_T* const aErrorStatus)
{
  (errorStatus = aErrorStatus);
}

uint32_T &RT_MODEL_AMC_BLDC_T::TaskCounter(int32_T idx)
{
  return (Timing.TaskCounters.TID[(idx)]);
}

const char_T** RT_MODEL_AMC_BLDC_T::getErrorStatusPointer()
{
  return &errorStatus;
}

//
// File trailer for generated code.
//
// [EOF]
//
