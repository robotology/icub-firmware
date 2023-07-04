//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: AMC_BLDC.cpp
//
// Code generated for Simulink model 'AMC_BLDC'.
//
// Model version                  : 6.17
// Simulink Coder version         : 9.9 (R2023a) 19-Nov-2022
// C/C++ source code generated on : Tue Jun 27 10:19:19 2023
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
#include "thermal_model.h"
#include "estimation_velocity.h"
#include "filter_current.h"
#include "control_foc.h"
#include "control_outer.h"
#define can_decoder_MDLREF_HIDE_CHILD_
#include "can_decoder.h"
#define can_encoder_MDLREF_HIDE_CHILD_
#include "can_encoder.h"
#define SupervisorFSM_RX_MDLREF_HIDE_CHILD_
#include "SupervisorFSM_RX.h"
#define SupervisorFSM_TX_MDLREF_HIDE_CHILD_
#include "SupervisorFSM_TX.h"

// Exported block parameters
ConfigurationParameters InitConfParams = {
  {
    true,
    true,
    false,
    false,
    true,
    false,
    16000,
    0,
    0U,
    7U,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    44.0F,
    24.0F,
    25.9F,
    271.0F,
    16.0F,
    797.5F
  },

  {
    EstimationVelocityModes_MovingAverage,
    0.9995F
  },

  {
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
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0U
  },

  {
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
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    0U
  },

  {
    1.0F,
    359.0F,
    0.0F,
    360.0F,
    0.0F,
    0.0F,
    40000.0F,
    10U,
    2.0F,
    5.0F,
    10.0F,
    32000U,
    70.0F
  },
  25.0F
} ;                                    // Variable: InitConfParams
                                          //  Referenced by: '<S7>/SupervisorFSM_RX'


real32_T CAN_ANGLE_DEG2ICUB = 182.044449F;// Variable: CAN_ANGLE_DEG2ICUB
                                             //  Referenced by: '<S6>/CAN_Encoder'
                                             //  2^16/360

real32_T CAN_ANGLE_ICUB2DEG = 0.00549316406F;// Variable: CAN_ANGLE_ICUB2DEG
                                                //  Referenced by: '<S6>/CAN_Decoder'
                                                //  360/2^16

uint8_T CAN_ID_AMC = 3U;               // Variable: CAN_ID_AMC
                                          //  Referenced by:
                                          //    '<S6>/CAN_Decoder'
                                          //    '<S6>/CAN_Encoder'
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
void AMC_BLDC_step0(void)        // Sample time: [1.1428571428571438E-6s, 0.0s]
{
  // (no output/update code required)
}

// Model step function for TID1
void AMC_BLDC_step_FOC(void)       // Sample time: [3.65714285714286E-5s, 0.0s]
{
  int8_T wrBufIdx;

  // RateTransition generated from: '<Root>/Adapter2'
  rtw_mutex_lock();
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_1_RD =
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_1_Ls;
  rtw_mutex_unlock();
  AMC_BLDC_B.BusConversion_InsertedFor_FOC_at_inport_1_BusCreator1.flags =
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_1_Bu[AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_1_RD];

  // RateTransition generated from: '<Root>/Adapter2'
  rtw_mutex_lock();
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_2_RD =
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_2_Ls;
  rtw_mutex_unlock();
  AMC_BLDC_B.BusConversion_InsertedFor_FOC_at_inport_1_BusCreator1.configurationparameters
    =
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_2_Bu[AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_2_RD];

  // RateTransition generated from: '<Root>/Adapter2'
  rtw_mutex_lock();
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_3_RD =
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_3_Ls;
  rtw_mutex_unlock();
  AMC_BLDC_B.BusConversion_InsertedFor_FOC_at_inport_1_BusCreator1.estimateddata
    =
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_3_Bu[AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_3_RD];

  // RateTransition generated from: '<Root>/Adapter2'
  rtw_mutex_lock();
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_4_RD =
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_4_Ls;
  rtw_mutex_unlock();
  AMC_BLDC_B.BusConversion_InsertedFor_FOC_at_inport_1_BusCreator1.targets =
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_4_Bu[AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_4_RD];

  // RateTransition generated from: '<Root>/Adapter2'
  rtw_mutex_lock();
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_5_RD =
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_5_Ls;
  rtw_mutex_unlock();
  AMC_BLDC_B.BusConversion_InsertedFor_FOC_at_inport_1_BusCreator1.controlouteroutputs
    =
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_5_Bu[AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_5_RD];

  // ModelReference: '<Root>/FOC' incorporates:
  //   Inport generated from: '<Root>/In Bus Element5'
  //   Outport generated from: '<Root>/Out Bus Element'

  control_foc(&AMC_BLDC_U.SensorsData_p,
              &AMC_BLDC_B.BusConversion_InsertedFor_FOC_at_inport_1_BusCreator1,
              &AMC_BLDC_Y.ControlOutputs_p, &(AMC_BLDC_DW.FOC_InstanceData.rtb),
              &(AMC_BLDC_DW.FOC_InstanceData.rtdw),
              &(AMC_BLDC_DW.FOC_InstanceData.rtzce));

  // RateTransition generated from: '<Root>/Adapter1' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element'

  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_LstB +
     1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx ==
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_RDBu)
  {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  switch (wrBufIdx) {
   case 0:
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Buf0 =
      AMC_BLDC_Y.ControlOutputs_p;
    break;

   case 1:
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Buf1 =
      AMC_BLDC_Y.ControlOutputs_p;
    break;

   case 2:
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Buf2 =
      AMC_BLDC_Y.ControlOutputs_p;
    break;
  }

  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_LstB =
    wrBufIdx;

  // End of RateTransition generated from: '<Root>/Adapter1'

  // RateTransition generated from: '<Root>/Adapter3' incorporates:
  //   Inport generated from: '<Root>/In Bus Element5'

  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_LstB +
     1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx ==
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_RDBu)
  {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  switch (wrBufIdx) {
   case 0:
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Bu_e =
      AMC_BLDC_U.SensorsData_p;
    break;

   case 1:
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Bu_c =
      AMC_BLDC_U.SensorsData_p;
    break;

   case 2:
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Bu_j =
      AMC_BLDC_U.SensorsData_p;
    break;
  }

  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_LstB =
    wrBufIdx;

  // End of RateTransition generated from: '<Root>/Adapter3'
}

// Model step function for TID2
void AMC_BLDC_step_Time_1ms(void)      // Sample time: [0.001s, 0.0s]
{
  // local block i/o variables
  Targets rtb_SupervisorFSM_RX_o2;
  ControlOuterOutputs rtb_OuterControl;
  BUS_STATUS_TX rtb_SupervisorFSM_TX_o2;
  int8_T wrBufIdx;

  // UnitDelay generated from: '<Root>/Adapter4'
  AMC_BLDC_B.ZOHBlockInsertedForAdapter_InsertedFor_Adapter4_at_outport_0 =
    AMC_BLDC_DW.ZOHBlockInsertedForAdapter_InsertedFor_Adapter4_at_outport_0;

  // RateTransition generated from: '<Root>/Adapter3'
  rtw_mutex_lock();
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_RDBu =
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_LstB;
  rtw_mutex_unlock();
  switch
    (AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_RDBu) {
   case 0:
    // RateTransition generated from: '<Root>/Adapter3'
    AMC_BLDC_B.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0 =
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Bu_e;
    break;

   case 1:
    // RateTransition generated from: '<Root>/Adapter3'
    AMC_BLDC_B.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0 =
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Bu_c;
    break;

   case 2:
    // RateTransition generated from: '<Root>/Adapter3'
    AMC_BLDC_B.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0 =
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Bu_j;
    break;
  }

  // End of RateTransition generated from: '<Root>/Adapter3'

  // ModelReference: '<S5>/Estimation_Velocity'
  estimation_velocity
    (&AMC_BLDC_B.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0,
     &AMC_BLDC_B.ZOHBlockInsertedForAdapter_InsertedFor_Adapter4_at_outport_0,
     &AMC_BLDC_Y.EstimatedData_p.jointvelocities,
     &(AMC_BLDC_DW.Estimation_Velocity_InstanceData.rtdw));

  // RateTransition generated from: '<Root>/Adapter1'
  rtw_mutex_lock();
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_RDBu =
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_LstB;
  rtw_mutex_unlock();
  switch
    (AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_RDBu) {
   case 0:
    // RateTransition generated from: '<Root>/Adapter1'
    AMC_BLDC_B.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0 =
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Buf0;
    break;

   case 1:
    // RateTransition generated from: '<Root>/Adapter1'
    AMC_BLDC_B.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0 =
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Buf1;
    break;

   case 2:
    // RateTransition generated from: '<Root>/Adapter1'
    AMC_BLDC_B.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0 =
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Buf2;
    break;
  }

  // End of RateTransition generated from: '<Root>/Adapter1'

  // ModelReference: '<S5>/Filter_Current'
  filter_current
    (&AMC_BLDC_B.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0,
     &AMC_BLDC_Y.EstimatedData_p.Iq_filtered,
     &(AMC_BLDC_DW.Filter_Current_InstanceData.rtdw));

  // RateTransition generated from: '<S5>/Adapter1'
  rtw_mutex_lock();
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_RD_a =
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Ls_j;
  rtw_mutex_unlock();
  AMC_BLDC_Y.EstimatedData_p.motor_temperature =
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Buf[AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_RD_a];

  // ModelReference: '<S6>/CAN_Decoder' incorporates:
  //   Inport generated from: '<Root>/In Bus Element2'

  can_decoder(&AMC_BLDC_U.PacketsRx,
              &AMC_BLDC_B.ZOHBlockInsertedForAdapter_InsertedFor_Adapter4_at_outport_0,
              &AMC_BLDC_B.CAN_Decoder_o1, &AMC_BLDC_B.CAN_Decoder_o2,
              &AMC_BLDC_B.CAN_Decoder_o3);

  // ModelReference: '<S7>/SupervisorFSM_RX' incorporates:
  //   Inport generated from: '<Root>/In Bus Element'
  //   Outport generated from: '<Root>/Out Bus Element3'
  //   Outport generated from: '<Root>/Out Bus Element2'
  //   Outport generated from: '<Root>/Out Bus Element4'

  SupervisorFSM_RX
    (&AMC_BLDC_B.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0,
     &AMC_BLDC_U.ExternalFlags_p,
     &AMC_BLDC_B.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0,
     &AMC_BLDC_B.CAN_Decoder_o1, &AMC_BLDC_Y.EstimatedData_p,
     &AMC_BLDC_B.CAN_Decoder_o2, &AMC_BLDC_B.CAN_Decoder_o3, &AMC_BLDC_Y.Flags_p,
     &rtb_SupervisorFSM_RX_o2, &AMC_BLDC_Y.ConfigurationParameters_p);

  // ModelReference: '<S7>/SupervisorFSM_TX' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element3'
  //   Outport generated from: '<Root>/Out Bus Element2'
  //   Outport generated from: '<Root>/Out Bus Element4'

  SupervisorFSM_TX
    (&AMC_BLDC_B.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0,
     &AMC_BLDC_Y.EstimatedData_p, &AMC_BLDC_Y.Flags_p,
     &AMC_BLDC_B.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0,
     &AMC_BLDC_B.MessagesTx, &rtb_SupervisorFSM_TX_o2);

  // ModelReference: '<S6>/CAN_Encoder' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element1'

  can_encoder(&AMC_BLDC_B.MessagesTx, &rtb_SupervisorFSM_TX_o2,
              &AMC_BLDC_B.ZOHBlockInsertedForAdapter_InsertedFor_Adapter4_at_outport_0,
              &AMC_BLDC_Y.PacketsTx);

  // ModelReference: '<Root>/OuterControl' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element3'
  //   Outport generated from: '<Root>/Out Bus Element2'
  //   Outport generated from: '<Root>/Out Bus Element4'

  control_outer(&AMC_BLDC_Y.Flags_p, &AMC_BLDC_Y.ConfigurationParameters_p,
                &rtb_SupervisorFSM_RX_o2,
                &AMC_BLDC_B.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0,
                &AMC_BLDC_Y.EstimatedData_p, &rtb_OuterControl,
                &(AMC_BLDC_DW.OuterControl_InstanceData.rtb),
                &(AMC_BLDC_DW.OuterControl_InstanceData.rtdw),
                &(AMC_BLDC_DW.OuterControl_InstanceData.rtzce));

  // RateTransition generated from: '<Root>/Adapter2'
  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_5_Ls +
     1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx ==
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_5_RD)
  {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_5_Bu[wrBufIdx]
    = rtb_OuterControl;
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_5_Ls =
    wrBufIdx;

  // RateTransition generated from: '<Root>/Adapter2' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element4'

  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_1_Ls +
     1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx ==
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_1_RD)
  {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_1_Bu[wrBufIdx]
    = AMC_BLDC_Y.Flags_p;
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_1_Ls =
    wrBufIdx;

  // RateTransition generated from: '<Root>/Adapter2' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element3'

  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_2_Ls +
     1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx ==
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_2_RD)
  {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_2_Bu[wrBufIdx]
    = AMC_BLDC_Y.ConfigurationParameters_p;
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_2_Ls =
    wrBufIdx;

  // RateTransition generated from: '<Root>/Adapter2'
  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_4_Ls +
     1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx ==
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_4_RD)
  {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_4_Bu[wrBufIdx]
    = rtb_SupervisorFSM_RX_o2;
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_4_Ls =
    wrBufIdx;

  // RateTransition generated from: '<Root>/Adapter2' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element2'

  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_3_Ls +
     1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx ==
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_3_RD)
  {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_3_Bu[wrBufIdx]
    = AMC_BLDC_Y.EstimatedData_p;
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter2_at_outport_0_3_Ls =
    wrBufIdx;

  // RateTransition generated from: '<S5>/Adapter3'
  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Ls_g +
     1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx ==
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_RD_p)
  {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  switch (wrBufIdx) {
   case 0:
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Buf0 =
      AMC_BLDC_B.ZOHBlockInsertedForAdapter_InsertedFor_Adapter4_at_outport_0;
    break;

   case 1:
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Buf1 =
      AMC_BLDC_B.ZOHBlockInsertedForAdapter_InsertedFor_Adapter4_at_outport_0;
    break;

   case 2:
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Buf2 =
      AMC_BLDC_B.ZOHBlockInsertedForAdapter_InsertedFor_Adapter4_at_outport_0;
    break;
  }

  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Ls_g =
    wrBufIdx;

  // End of RateTransition generated from: '<S5>/Adapter3'

  // RateTransition generated from: '<S5>/Adapter'
  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter_at_outport_0_LstBu +
     1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx ==
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter_at_outport_0_RDBuf)
  {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  switch (wrBufIdx) {
   case 0:
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter_at_outport_0_Buf0 =
      AMC_BLDC_B.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0;
    break;

   case 1:
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter_at_outport_0_Buf1 =
      AMC_BLDC_B.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0;
    break;

   case 2:
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter_at_outport_0_Buf2 =
      AMC_BLDC_B.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0;
    break;
  }

  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter_at_outport_0_LstBu =
    wrBufIdx;

  // End of RateTransition generated from: '<S5>/Adapter'

  // Update for UnitDelay generated from: '<Root>/Adapter4' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element3'

  AMC_BLDC_DW.ZOHBlockInsertedForAdapter_InsertedFor_Adapter4_at_outport_0 =
    AMC_BLDC_Y.ConfigurationParameters_p;
}

// Model step function for TID3
void AMC_BLDC_step_Time_10ms(void)     // Sample time: [0.01s, 0.0s]
{
  // local block i/o variables
  MotorTemperature rtb_Estimation_Temperature;
  ConfigurationParameters
    rtb_RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0;
  ControlOutputs rtb_RTBInsertedForAdapter_InsertedFor_Adapter_at_outport_0;
  int8_T wrBufIdx;

  // RateTransition generated from: '<S5>/Adapter'
  rtw_mutex_lock();
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter_at_outport_0_RDBuf =
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter_at_outport_0_LstBu;
  rtw_mutex_unlock();
  switch
    (AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter_at_outport_0_RDBuf) {
   case 0:
    rtb_RTBInsertedForAdapter_InsertedFor_Adapter_at_outport_0 =
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter_at_outport_0_Buf0;
    break;

   case 1:
    rtb_RTBInsertedForAdapter_InsertedFor_Adapter_at_outport_0 =
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter_at_outport_0_Buf1;
    break;

   case 2:
    rtb_RTBInsertedForAdapter_InsertedFor_Adapter_at_outport_0 =
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter_at_outport_0_Buf2;
    break;
  }

  // End of RateTransition generated from: '<S5>/Adapter'

  // RateTransition generated from: '<S5>/Adapter3'
  rtw_mutex_lock();
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_RD_p =
    AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Ls_g;
  rtw_mutex_unlock();
  switch
    (AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_RD_p) {
   case 0:
    rtb_RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0 =
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Buf0;
    break;

   case 1:
    rtb_RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0 =
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Buf1;
    break;

   case 2:
    rtb_RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0 =
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0_Buf2;
    break;
  }

  // End of RateTransition generated from: '<S5>/Adapter3'

  // ModelReference: '<S5>/Estimation_Temperature'
  thermal_model(&rtb_RTBInsertedForAdapter_InsertedFor_Adapter_at_outport_0,
                &rtb_RTBInsertedForAdapter_InsertedFor_Adapter3_at_outport_0,
                &rtb_Estimation_Temperature,
                &(AMC_BLDC_DW.Estimation_Temperature_InstanceData.rtdw));

  // RateTransition generated from: '<S5>/Adapter1'
  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>
    (AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Ls_j +
     1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx ==
      AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_RD_a)
  {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Buf[wrBufIdx]
    = rtb_Estimation_Temperature;
  AMC_BLDC_DW.RTBInsertedForAdapter_InsertedFor_Adapter1_at_outport_0_Ls_j =
    wrBufIdx;

  // End of RateTransition generated from: '<S5>/Adapter1'
}

// Model initialize function
void AMC_BLDC_initialize(void)
{
  // Model Initialize function for ModelReference Block: '<S5>/Estimation_Temperature' 
  thermal_model_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M),
    &(AMC_BLDC_DW.Estimation_Temperature_InstanceData.rtm),
    &(AMC_BLDC_DW.Estimation_Temperature_InstanceData.rtdw));

  // Model Initialize function for ModelReference Block: '<S5>/Estimation_Velocity' 
  estimation_velocity_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M),
    &(AMC_BLDC_DW.Estimation_Velocity_InstanceData.rtm),
    &(AMC_BLDC_DW.Estimation_Velocity_InstanceData.rtdw));

  // Model Initialize function for ModelReference Block: '<S5>/Filter_Current'
  filter_current_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M),
    &(AMC_BLDC_DW.Filter_Current_InstanceData.rtm),
    &(AMC_BLDC_DW.Filter_Current_InstanceData.rtdw));

  // Model Initialize function for ModelReference Block: '<Root>/FOC'
  control_foc_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M),
    &(AMC_BLDC_DW.FOC_InstanceData.rtm), &(AMC_BLDC_DW.FOC_InstanceData.rtb),
    &(AMC_BLDC_DW.FOC_InstanceData.rtdw), &(AMC_BLDC_DW.FOC_InstanceData.rtzce));

  // Model Initialize function for ModelReference Block: '<S6>/CAN_Decoder'
  can_decoder_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M));

  // Model Initialize function for ModelReference Block: '<S6>/CAN_Encoder'
  can_encoder_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M));

  // Model Initialize function for ModelReference Block: '<Root>/OuterControl'
  control_outer_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M),
    &(AMC_BLDC_DW.OuterControl_InstanceData.rtm),
    &(AMC_BLDC_DW.OuterControl_InstanceData.rtb),
    &(AMC_BLDC_DW.OuterControl_InstanceData.rtdw),
    &(AMC_BLDC_DW.OuterControl_InstanceData.rtzce));

  // Model Initialize function for ModelReference Block: '<S7>/SupervisorFSM_RX' 
  SupervisorFSM_RX_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M));

  // Model Initialize function for ModelReference Block: '<S7>/SupervisorFSM_TX' 
  SupervisorFSM_TX_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M));

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

  // Start for RateTransition generated from: '<S5>/Adapter1'
  rtw_mutex_init();

  // Start for RateTransition generated from: '<S5>/Adapter3'
  rtw_mutex_init();

  // Start for RateTransition generated from: '<S5>/Adapter'
  rtw_mutex_init();

  // SystemInitialize for ModelReference: '<S5>/Estimation_Temperature'
  thermal_model_Init(&(AMC_BLDC_DW.Estimation_Temperature_InstanceData.rtdw));

  // SystemInitialize for ModelReference: '<S5>/Estimation_Velocity'
  estimation_velocity_Init(&(AMC_BLDC_DW.Estimation_Velocity_InstanceData.rtdw));

  // SystemInitialize for ModelReference: '<S5>/Filter_Current'
  filter_current_Init(&(AMC_BLDC_DW.Filter_Current_InstanceData.rtdw));

  // SystemInitialize for ModelReference: '<Root>/FOC'
  control_foc_Init(&(AMC_BLDC_DW.FOC_InstanceData.rtdw));

  // SystemInitialize for ModelReference: '<S6>/CAN_Decoder' incorporates:
  //   Inport generated from: '<Root>/In Bus Element2'

  can_decoder_Init();

  // SystemInitialize for ModelReference: '<Root>/OuterControl'
  control_outer_Init(&(AMC_BLDC_DW.OuterControl_InstanceData.rtdw));

  // SystemInitialize for ModelReference: '<S7>/SupervisorFSM_RX' incorporates:
  //   Inport generated from: '<Root>/In Bus Element'
  //   Outport generated from: '<Root>/Out Bus Element3'
  //   Outport generated from: '<Root>/Out Bus Element4'

  SupervisorFSM_RX_Init(&AMC_BLDC_Y.Flags_p,
                        &AMC_BLDC_Y.ConfigurationParameters_p);

  // SystemInitialize for ModelReference: '<S7>/SupervisorFSM_TX' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element3'
  //   Outport generated from: '<Root>/Out Bus Element4'

  SupervisorFSM_TX_Init(&AMC_BLDC_B.MessagesTx);

  // Enable for ModelReference: '<Root>/OuterControl'
  control_outer_Enable(&(AMC_BLDC_DW.OuterControl_InstanceData.rtdw));
}

// Model terminate function
void AMC_BLDC_terminate(void)
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

  // Terminate for ModelReference: '<Root>/FOC'
  control_foc_Term(&(AMC_BLDC_DW.FOC_InstanceData.rtdw));

  // Terminate for RateTransition generated from: '<Root>/Adapter1'
  rtw_mutex_destroy();

  // Terminate for RateTransition generated from: '<Root>/Adapter3'
  rtw_mutex_destroy();

  // Terminate for ModelReference: '<S5>/Filter_Current'
  filter_current_Term(&(AMC_BLDC_DW.Filter_Current_InstanceData.rtdw));

  // Terminate for RateTransition generated from: '<S5>/Adapter1'
  rtw_mutex_destroy();

  // Terminate for RateTransition generated from: '<S5>/Adapter3'
  rtw_mutex_destroy();

  // Terminate for RateTransition generated from: '<S5>/Adapter'
  rtw_mutex_destroy();
}

//
// File trailer for generated code.
//
// [EOF]
//
