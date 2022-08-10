//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: AMC_BLDC.cpp
//
// Code generated for Simulink model 'AMC_BLDC'.
//
// Model version                  : 4.103
// Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
// C/C++ source code generated on : Tue Aug  9 15:19:58 2022
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
#define estimation_velocity_MDLREF_HIDE_CHILD_
#include "estimation_velocity.h"
#define filter_current_MDLREF_HIDE_CHILD_
#include "filter_current.h"
#define control_foc_MDLREF_HIDE_CHILD_
#include "control_foc.h"
#define can_decoder_MDLREF_HIDE_CHILD_
#include "can_decoder.h"
#define can_encoder_MDLREF_HIDE_CHILD_
#include "can_encoder.h"
#define control_outer_MDLREF_HIDE_CHILD_
#include "control_outer.h"
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
    1.0F,
    0.0F,
    0.0F,
    0.0F,
    0.0F,
    24.0F,
    24.0F
  },

  {
    EstimationVelocityModes_MovingAverage
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
    100.0F,
    10U,
    2.0F,
    5.0F,
    10.0F,
    32000U
  }
} ;                                    // Variable: InitConfParams
                                          //  Referenced by: '<S7>/SupervisorFSM_RX'


real32_T CAN_ANGLE_DEG2ICUB = 182.044449F;// Variable: CAN_ANGLE_DEG2ICUB
                                             //  Referenced by: '<S6>/CAN_Encoder'
                                             //  2^16/360

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
  FOCSlowInputs rtb_BusConversion_InsertedFor_F;
  int8_T wrBufIdx;

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
    AMC_BLDC_DW.RTBInsertedForAdapter_Insert_mp;
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
  //   Inport generated from: '<Root>/In Bus Element5'
  //   Outport generated from: '<Root>/Out Bus Element'

  control_foc(&AMC_BLDC_U.SensorsData_p, &rtb_BusConversion_InsertedFor_F,
              &AMC_BLDC_Y.ControlOutputs_p);

  // RateTransition generated from: '<Root>/Adapter1' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element'

  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.RTBInsertedForAdapter_Insert_js + 1);
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
    AMC_BLDC_DW.RTBInsertedForAdapter_Insert_j2 = AMC_BLDC_Y.ControlOutputs_p;
    break;

   case 2:
    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_o = AMC_BLDC_Y.ControlOutputs_p;
    break;
  }

  AMC_BLDC_DW.RTBInsertedForAdapter_Insert_js = wrBufIdx;

  // End of RateTransition generated from: '<Root>/Adapter1'

  // RateTransition generated from: '<Root>/Adapter3' incorporates:
  //   Inport generated from: '<Root>/In Bus Element5'

  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_g + 1);
  if (wrBufIdx == 3) {
    wrBufIdx = 0;
  }

  if (wrBufIdx == AMC_BLDC_DW.RTBInsertedForAdapter_Insert_pa) {
    wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }
  }

  rtw_mutex_unlock();
  switch (wrBufIdx) {
   case 0:
    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_e = AMC_BLDC_U.SensorsData_p;
    break;

   case 1:
    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_c = AMC_BLDC_U.SensorsData_p;
    break;

   case 2:
    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_j = AMC_BLDC_U.SensorsData_p;
    break;
  }

  AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_g = wrBufIdx;

  // End of RateTransition generated from: '<Root>/Adapter3'
}

// Model step function for TID2
void AMC_BLDC_step_Time(void)          // Sample time: [0.001s, 0.0s]
{
  // local block i/o variables
  Targets rtb_SupervisorFSM_RX_o2;
  ControlOuterOutputs rtb_OuterControl;
  BUS_STATUS_TX rtb_SupervisorFSM_TX_o2;
  ControlOutputs rtb_BusConversion_InsertedFor_F;
  int8_T wrBufIdx;

  // RateTransition generated from: '<Root>/Adapter1'
  rtw_mutex_lock();
  AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_a =
    AMC_BLDC_DW.RTBInsertedForAdapter_Insert_js;
  rtw_mutex_unlock();
  switch (AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_a) {
   case 0:
    // RateTransition generated from: '<Root>/Adapter1'
    AMC_BLDC_B.RTBInsertedForAdapter_Inserte_a =
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_d;
    break;

   case 1:
    // RateTransition generated from: '<Root>/Adapter1'
    AMC_BLDC_B.RTBInsertedForAdapter_Inserte_a =
      AMC_BLDC_DW.RTBInsertedForAdapter_Insert_j2;
    break;

   case 2:
    // RateTransition generated from: '<Root>/Adapter1'
    AMC_BLDC_B.RTBInsertedForAdapter_Inserte_a =
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_o;
    break;
  }

  // End of RateTransition generated from: '<Root>/Adapter1'

  // BusCreator generated from: '<S5>/Filter_Current'
  rtb_BusConversion_InsertedFor_F.Vq = 0.0F;
  rtb_BusConversion_InsertedFor_F.Vabc[0] = 0.0F;
  rtb_BusConversion_InsertedFor_F.Vabc[1] = 0.0F;
  rtb_BusConversion_InsertedFor_F.Vabc[2] = 0.0F;
  rtb_BusConversion_InsertedFor_F.Iq_fbk =
    AMC_BLDC_B.RTBInsertedForAdapter_Inserte_a.Iq_fbk;

  // RateTransition generated from: '<Root>/Adapter3'
  rtw_mutex_lock();
  AMC_BLDC_DW.RTBInsertedForAdapter_Insert_pa =
    AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_g;
  rtw_mutex_unlock();
  switch (AMC_BLDC_DW.RTBInsertedForAdapter_Insert_pa) {
   case 0:
    // RateTransition generated from: '<Root>/Adapter3'
    AMC_BLDC_B.RTBInsertedForAdapter_InsertedF =
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_e;
    break;

   case 1:
    // RateTransition generated from: '<Root>/Adapter3'
    AMC_BLDC_B.RTBInsertedForAdapter_InsertedF =
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_c;
    break;

   case 2:
    // RateTransition generated from: '<Root>/Adapter3'
    AMC_BLDC_B.RTBInsertedForAdapter_InsertedF =
      AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_j;
    break;
  }

  // End of RateTransition generated from: '<Root>/Adapter3'

  // UnitDelay generated from: '<Root>/Adapter4'
  AMC_BLDC_B.ZOHBlockInsertedForAdapter_Inse =
    AMC_BLDC_DW.ZOHBlockInsertedForAdapter_Inse;

  // ModelReference: '<S5>/Estimation_Velocity'
  estimation_velocity(&AMC_BLDC_B.RTBInsertedForAdapter_InsertedF,
                      &AMC_BLDC_B.ZOHBlockInsertedForAdapter_Inse,
                      &AMC_BLDC_Y.EstimatedData_p.jointvelocities);

  // ModelReference: '<S5>/Filter_Current'
  filter_current(&rtb_BusConversion_InsertedFor_F,
                 &AMC_BLDC_Y.EstimatedData_p.Iq_filtered);

  // RateTransition generated from: '<Root>/Adapter2' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element2'

  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.RTBInsertedForAdapter_Insert_b2 + 1);
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

  // ModelReference: '<S6>/CAN_Decoder' incorporates:
  //   Inport generated from: '<Root>/In Bus Element2'

  can_decoder(&AMC_BLDC_U.PacketsRx, &AMC_BLDC_B.ZOHBlockInsertedForAdapter_Inse,
              &AMC_BLDC_B.CAN_Decoder_o1, &AMC_BLDC_B.CAN_Decoder_o2,
              &AMC_BLDC_B.CAN_Decoder_o3);

  // ModelReference: '<S7>/SupervisorFSM_RX' incorporates:
  //   Inport generated from: '<Root>/In Bus Element'
  //   Outport generated from: '<Root>/Out Bus Element3'
  //   Outport generated from: '<Root>/Out Bus Element2'
  //   Outport generated from: '<Root>/Out Bus Element4'

  SupervisorFSM_RX(&AMC_BLDC_B.RTBInsertedForAdapter_InsertedF,
                   &AMC_BLDC_U.ExternalFlags_p,
                   &AMC_BLDC_B.RTBInsertedForAdapter_Inserte_a,
                   &AMC_BLDC_B.CAN_Decoder_o1, &AMC_BLDC_B.CAN_Decoder_o2,
                   &AMC_BLDC_B.CAN_Decoder_o3, &AMC_BLDC_Y.Flags_p,
                   &rtb_SupervisorFSM_RX_o2,
                   &AMC_BLDC_Y.ConfigurationParameters_p);

  // ModelReference: '<S7>/SupervisorFSM_TX' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element3'
  //   Outport generated from: '<Root>/Out Bus Element2'
  //   Outport generated from: '<Root>/Out Bus Element4'

  SupervisorFSM_TX(&AMC_BLDC_B.RTBInsertedForAdapter_InsertedF,
                   &AMC_BLDC_Y.EstimatedData_p, &AMC_BLDC_Y.Flags_p,
                   &AMC_BLDC_B.RTBInsertedForAdapter_Inserte_a,
                   &AMC_BLDC_B.MessagesTx, &rtb_SupervisorFSM_TX_o2);

  // ModelReference: '<S6>/CAN_Encoder' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element1'

  can_encoder(&AMC_BLDC_B.MessagesTx, &rtb_SupervisorFSM_TX_o2,
              &AMC_BLDC_B.ZOHBlockInsertedForAdapter_Inse, &AMC_BLDC_Y.PacketsTx);

  // ModelReference: '<Root>/OuterControl' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element3'
  //   Outport generated from: '<Root>/Out Bus Element2'
  //   Outport generated from: '<Root>/Out Bus Element4'

  control_outer(&AMC_BLDC_Y.Flags_p, &AMC_BLDC_Y.ConfigurationParameters_p,
                &rtb_SupervisorFSM_RX_o2,
                &AMC_BLDC_B.RTBInsertedForAdapter_InsertedF,
                &AMC_BLDC_Y.EstimatedData_p, &rtb_OuterControl);

  // RateTransition generated from: '<Root>/Adapter2'
  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.RTBInsertedForAdapter_Insert_p5 + 1);
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

  // RateTransition generated from: '<Root>/Adapter2' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element4'

  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.RTBInsertedForAdapter_Insert_hj + 1);
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
  AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_l[wrBufIdx] = AMC_BLDC_Y.Flags_p;
  AMC_BLDC_DW.RTBInsertedForAdapter_Insert_hj = wrBufIdx;

  // RateTransition generated from: '<Root>/Adapter2' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element3'

  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.RTBInsertedForAdapter_Insert_mp + 1);
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
  AMC_BLDC_DW.RTBInsertedForAdapter_Insert_mp = wrBufIdx;

  // RateTransition generated from: '<Root>/Adapter2'
  rtw_mutex_lock();
  wrBufIdx = static_cast<int8_T>(AMC_BLDC_DW.RTBInsertedForAdapter_Insert_jj + 1);
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
  AMC_BLDC_DW.RTBInsertedForAdapter_Inserte_m[wrBufIdx] =
    rtb_SupervisorFSM_RX_o2;
  AMC_BLDC_DW.RTBInsertedForAdapter_Insert_jj = wrBufIdx;

  // Update for UnitDelay generated from: '<Root>/Adapter4' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element3'

  AMC_BLDC_DW.ZOHBlockInsertedForAdapter_Inse =
    AMC_BLDC_Y.ConfigurationParameters_p;
}

// Model initialize function
void AMC_BLDC_initialize(void)
{
  // Model Initialize function for ModelReference Block: '<S5>/Estimation_Velocity' 
  estimation_velocity_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M));

  // Model Initialize function for ModelReference Block: '<S5>/Filter_Current'
  filter_current_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M));

  // Model Initialize function for ModelReference Block: '<Root>/FOC'
  control_foc_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M));

  // Model Initialize function for ModelReference Block: '<S6>/CAN_Decoder'
  can_decoder_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M));

  // Model Initialize function for ModelReference Block: '<S6>/CAN_Encoder'
  can_encoder_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M));

  // Model Initialize function for ModelReference Block: '<Root>/OuterControl'
  control_outer_initialize(rtmGetErrorStatusPointer(AMC_BLDC_M));

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

  // SystemInitialize for ModelReference: '<S5>/Estimation_Velocity'
  estimation_velocity_Init();

  // SystemInitialize for ModelReference: '<S5>/Filter_Current'
  filter_current_Init();

  // SystemInitialize for ModelReference: '<Root>/FOC' incorporates:
  //   Inport generated from: '<Root>/In Bus Element5'
  //   Outport generated from: '<Root>/Out Bus Element'

  control_foc_Init();

  // SystemInitialize for ModelReference: '<S6>/CAN_Decoder' incorporates:
  //   Inport generated from: '<Root>/In Bus Element2'

  can_decoder_Init();

  // SystemInitialize for ModelReference: '<Root>/OuterControl' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element3'
  //   Outport generated from: '<Root>/Out Bus Element4'

  control_outer_Init();

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

  // Enable for ModelReference: '<Root>/OuterControl' incorporates:
  //   Outport generated from: '<Root>/Out Bus Element3'
  //   Outport generated from: '<Root>/Out Bus Element2'
  //   Outport generated from: '<Root>/Out Bus Element4'

  control_outer_Enable();
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

  // Terminate for RateTransition generated from: '<Root>/Adapter1'
  rtw_mutex_destroy();

  // Terminate for RateTransition generated from: '<Root>/Adapter3'
  rtw_mutex_destroy();

  // Terminate for ModelReference: '<S5>/Filter_Current'
  filter_current_Term();
}

//
// File trailer for generated code.
//
// [EOF]
//
