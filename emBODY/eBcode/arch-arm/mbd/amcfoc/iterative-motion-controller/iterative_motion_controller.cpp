//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: iterative_motion_controller.cpp
//
// Code generated for Simulink model 'iterative_motion_controller'.
//
// Model version                  : 3.14
// Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
// C/C++ source code generated on : Fri Jun  6 14:55:37 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "iterative_motion_controller.h"
#include "rtwtypes.h"
#include "iterative_motion_controller_types.h"
#include <cstring>
#include "rtw_mutex.h"
#include "rtw_defines.h"
#include "mul_s32_sat.h"
#include "SupervisorFSM_TX.h"
#include "motion_controller.h"

// Exported block parameters
ActuatorConfiguration AmcfocInitConf[2] = { {
    {
      60000.0F,
      1.0F,
      2.0F,
      3.0F,
      32000U,
      60.0F
    },

    {
      {
        ControlModes_Current,
        100.0F,
        -100.0F,
        2.0F,
        500.0F,
        0.0F,
        0.0F,
        0.0F,
        0.0F,
        0U
      },

      {
        ControlModes_Velocity,
        3.0F,
        -3.0F,
        10.0F,
        10.0F,
        0.0F,
        0.0F,
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
        0,
        0,
        false
      },
      0.0F,
      0.0F,
      -3.0F,
      3.0F,
      24.0F,
      25.6F,
      1000.0F,
      0.0F,
      0.0F,
      0.0F
    }
  }, { {
      60000.0F,
      1.0F,
      2.0F,
      3.0F,
      32000U,
      60.0F
    },

    {
      {
        ControlModes_Current,
        100.0F,
        -100.0F,
        2.0F,
        500.0F,
        0.0F,
        0.0F,
        0.0F,
        0.0F,
        0U
      },

      {
        ControlModes_Velocity,
        3.0F,
        -3.0F,
        10.0F,
        10.0F,
        0.0F,
        0.0F,
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
        0,
        0,
        false
      },
      0.0F,
      0.0F,
      -3.0F,
      3.0F,
      24.0F,
      25.6F,
      1000.0F,
      0.0F,
      0.0F,
      0.0F
    }
  } } ;                                // Variable: AmcfocInitConf
                                          //  Referenced by: '<Root>/Constant'


// Block signals and states (default storage)
DW_iterative_motion_controller_T iterative_motion_controller_DW;

// External inputs (root inport signals with default storage)
ExtU_iterative_motion_controller_T iterative_motion_controller_U;

// External outputs (root outports fed by signals with default storage)
ExtY_iterative_motion_controller_T iterative_motion_controller_Y;

// Real-time model
RT_MODEL_iterative_motion_controller_T iterative_motion_controller_M_ =
  RT_MODEL_iterative_motion_controller_T();
RT_MODEL_iterative_motion_controller_T *const iterative_motion_controller_M =
  &iterative_motion_controller_M_;

// Model step function for TID0
void AMCFOC_step_Time_base(void)       // Sample time: [5e-06s, 0.0s]
{
  // local block i/o variables
  ActuatorConfiguration rtb_ImpSel_InsertedFor_InitConf_at_outport_0;
  SensorsData rtb_ImpSel_InsertedFor_SensorData_at_outport_0;

  // local scratch DWork variables
  int32_T ForEach_itr;
  int32_T i;

  // Outputs for Iterator SubSystem: '<Root>/Iterative Motion Controller' incorporates:
  //   ForEach: '<S1>/For Each'

  for (ForEach_itr = 0; ForEach_itr < N_MOTORS; ForEach_itr++) {
    // ModelReference: '<S1>/Motion Controller' incorporates:
    //   Inport: '<Root>/ExternalFlags'

    motion_controllerTID0();

    // ForEachSliceSelector generated from: '<S1>/SensorData' incorporates:
    //   Inport: '<Root>/SensorData'

    rtb_ImpSel_InsertedFor_SensorData_at_outport_0 =
      iterative_motion_controller_U.SensorData[ForEach_itr];
    for (i = 0; i < MAX_EVENTS_PER_TICK; i++) {
      // ForEachSliceSelector generated from: '<S1>/Events'
      iterative_motion_controller_DW.ImpSel_InsertedFor_Events_at_outport_0[i] =
        iterative_motion_controller_DW.M[N_MOTORS * i + ForEach_itr];
    }

    // ForEachSliceSelector generated from: '<S1>/InitConf' incorporates:
    //   Constant: '<Root>/Constant'

    rtb_ImpSel_InsertedFor_InitConf_at_outport_0 = AmcfocInitConf[ForEach_itr];
  }

  // End of Outputs for SubSystem: '<Root>/Iterative Motion Controller'
}

// Model step function for TID1
void AMCFOC_step_FOC(void)             // Sample time: [4.5e-05s, 0.0s]
{
  // local block i/o variables
  ActuatorConfiguration rtb_ImpSel_InsertedFor_InitConf_at_outport_0;
  SensorsData rtb_ImpSel_InsertedFor_SensorData_at_outport_0;

  // local scratch DWork variables
  int32_T ForEach_itr;
  FOCOutputs rtb_ImpAsg_InsertedFor_FOCOutputs_at_inport_0[N_MOTORS];
  int32_T i;
  int8_T wrBufIdx;

  // Outputs for Iterator SubSystem: '<Root>/Iterative Motion Controller' incorporates:
  //   ForEach: '<S1>/For Each'

  for (ForEach_itr = 0; ForEach_itr < N_MOTORS; ForEach_itr++) {
    // ForEachSliceSelector generated from: '<S1>/SensorData' incorporates:
    //   Inport: '<Root>/SensorData'

    rtb_ImpSel_InsertedFor_SensorData_at_outport_0 =
      iterative_motion_controller_U.SensorData[ForEach_itr];

    // ModelReference: '<S1>/Motion Controller'
    motion_controllerTID1(&rtb_ImpSel_InsertedFor_SensorData_at_outport_0,
                          &iterative_motion_controller_DW.CoreSubsys[ForEach_itr]
                          .MotionController_o1,
                          &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr]
      .MotionController_InstanceData.rtb),
                          &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr]
      .MotionController_InstanceData.rtdw));

    // RateTransition generated from: '<S1>/Model'
    rtw_mutex_lock();
    wrBufIdx = static_cast<int8_T>
      (iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
       TmpRTBAtModelInport1_LstBufWR + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtModelInport1_RDBuf) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock();
    switch (wrBufIdx) {
     case 0:
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtModelInport1_Buf0 =
        rtb_ImpSel_InsertedFor_SensorData_at_outport_0;
      break;

     case 1:
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtModelInport1_Buf1 =
        rtb_ImpSel_InsertedFor_SensorData_at_outport_0;
      break;

     case 2:
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtModelInport1_Buf2 =
        rtb_ImpSel_InsertedFor_SensorData_at_outport_0;
      break;
    }

    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtModelInport1_LstBufWR = wrBufIdx;

    // RateTransition generated from: '<S1>/Model' incorporates:
    //   ModelReference: '<S1>/Motion Controller'

    rtw_mutex_lock();
    wrBufIdx = static_cast<int8_T>
      (iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
       TmpRTBAtModelInport4_LstBufWR + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtModelInport4_RDBuf) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock();
    switch (wrBufIdx) {
     case 0:
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtModelInport4_Buf0 =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        MotionController_o1;
      break;

     case 1:
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtModelInport4_Buf1 =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        MotionController_o1;
      break;

     case 2:
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtModelInport4_Buf2 =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        MotionController_o1;
      break;
    }

    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtModelInport4_LstBufWR = wrBufIdx;

    // ForEachSliceAssignment generated from: '<S1>/FOCOutputs' incorporates:
    //   ModelReference: '<S1>/Motion Controller'

    rtb_ImpAsg_InsertedFor_FOCOutputs_at_inport_0[ForEach_itr] =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].MotionController_o1;
    for (i = 0; i < MAX_EVENTS_PER_TICK; i++) {
      // ForEachSliceSelector generated from: '<S1>/Events'
      iterative_motion_controller_DW.ImpSel_InsertedFor_Events_at_outport_0[i] =
        iterative_motion_controller_DW.M[N_MOTORS * i + ForEach_itr];
    }

    // ForEachSliceSelector generated from: '<S1>/InitConf' incorporates:
    //   Constant: '<Root>/Constant'

    rtb_ImpSel_InsertedFor_InitConf_at_outport_0 = AmcfocInitConf[ForEach_itr];
  }

  // End of Outputs for SubSystem: '<Root>/Iterative Motion Controller'

  // Outport: '<Root>/FOCOutputs' incorporates:
  //   ForEachSliceAssignment generated from: '<S1>/FOCOutputs'

  std::memcpy(&iterative_motion_controller_Y.FOCOutputs_h[0],
              &rtb_ImpAsg_InsertedFor_FOCOutputs_at_inport_0[0],
              static_cast<uint32_T>(N_MOTORS) * sizeof(FOCOutputs));
}

// Model step function for TID2
void AMCFOC_step_Time_1ms(void)        // Sample time: [0.001s, 0.0s]
{
  // local block i/o variables
  ActuatorConfiguration rtb_ImpSel_InsertedFor_InitConf_at_outport_0;
  SensorsData rtb_ImpSel_InsertedFor_SensorData_at_outport_0;

  // local scratch DWork variables
  int32_T ForEach_itr;
  ActuatorConfiguration
    rtb_ImpAsg_InsertedFor_ConfigurationParameters_at_inport_0[N_MOTORS];
  BUS_MESSAGES_TX rtb_ImpAsg_InsertedFor_Messages_at_inport_0[N_MOTORS];
  BUS_STATUS_TX rtb_ImpAsg_InsertedFor_MessagesFlags_at_inport_0[N_MOTORS];
  EstimatedData rtb_ImpAsg_InsertedFor_Estimates_at_inport_0[N_MOTORS];
  FOCOutputs rtb_TmpRTBAtModelInport4;
  Flags rtb_ImpAsg_InsertedFor_Flags_at_inport_0[N_MOTORS];
  ReceivedEvents struct_temp;
  SensorsData rtb_TmpRTBAtModelInport1;
  int32_T i;
  uint8_T counter_motor_1;
  uint8_T counter_motor_2;
  uint8_T idx;

  // Chart: '<Root>/Sort Events' incorporates:
  //   Inport: '<Root>/EventsList'

  struct_temp.motor_id = 0U;
  struct_temp.event_type = EventTypes_None;
  struct_temp.targets_content.position = 0.0F;
  struct_temp.targets_content.velocity = 0.0F;
  struct_temp.targets_content.current = 0.0F;
  struct_temp.targets_content.voltage = 0.0F;
  struct_temp.pid_content.type = ControlModes_NotConfigured;
  struct_temp.pid_content.OutMax = 0.0F;
  struct_temp.pid_content.OutMin = 0.0F;
  struct_temp.pid_content.P = 0.0F;
  struct_temp.pid_content.I = 0.0F;
  struct_temp.pid_content.D = 0.0F;
  struct_temp.pid_content.N = 0.0F;
  struct_temp.pid_content.I0 = 0.0F;
  struct_temp.pid_content.D0 = 0.0F;
  struct_temp.pid_content.shift_factor = 0U;
  struct_temp.control_mode_content = ControlModes_NotConfigured;
  struct_temp.limits_content.overload = 0.0F;
  struct_temp.limits_content.peak = 0.0F;
  struct_temp.limits_content.nominal = 0.0F;
  struct_temp.limits_content.type = ControlModes_NotConfigured;
  struct_temp.motor_config_content.enable_verbosity = false;
  struct_temp.motor_config_content.has_hall_sens = false;
  struct_temp.motor_config_content.has_quadrature_encoder = false;
  struct_temp.motor_config_content.has_speed_quadrature_encoder = false;
  struct_temp.motor_config_content.has_temperature_sens = false;
  struct_temp.motor_config_content.encoder_tolerance = 0U;
  struct_temp.motor_config_content.pole_pairs = 0U;
  struct_temp.motor_config_content.rotor_encoder_resolution = 0;
  struct_temp.motor_config_content.rotor_index_offset = 0;
  struct_temp.motor_config_content.use_index = false;
  for (i = 0; i < MAX_EVENTS_PER_TICK * N_MOTORS; i++) {
    iterative_motion_controller_DW.M[i] = struct_temp;
  }

  //  Bin Events
  idx = 0U;
  counter_motor_1 = 0U;
  counter_motor_2 = 0U;
  while (idx < mul_s32_sat(N_MOTORS, MAX_EVENTS_PER_TICK)) {
    if ((iterative_motion_controller_U.EventsList[idx].motor_id == 0) &&
        (counter_motor_1 < MAX_EVENTS_PER_TICK)) {
      iterative_motion_controller_DW.M[iterative_motion_controller_U.EventsList[idx]
        .motor_id + N_MOTORS * counter_motor_1] =
        iterative_motion_controller_U.EventsList[idx];
      i = counter_motor_1 + 1;
      if (counter_motor_1 + 1 > 255) {
        i = 255;
      }

      counter_motor_1 = static_cast<uint8_T>(i);
    } else if ((iterative_motion_controller_U.EventsList[idx].motor_id == 1) &&
               (counter_motor_2 < MAX_EVENTS_PER_TICK)) {
      iterative_motion_controller_DW.M[iterative_motion_controller_U.EventsList[idx]
        .motor_id + N_MOTORS * counter_motor_2] =
        iterative_motion_controller_U.EventsList[idx];
      i = counter_motor_2 + 1;
      if (counter_motor_2 + 1 > 255) {
        i = 255;
      }

      counter_motor_2 = static_cast<uint8_T>(i);
    }

    i = idx + 1;
    if (idx + 1 > 255) {
      i = 255;
    }

    idx = static_cast<uint8_T>(i);
  }

  // End of Chart: '<Root>/Sort Events'

  // Outputs for Iterator SubSystem: '<Root>/Iterative Motion Controller' incorporates:
  //   ForEach: '<S1>/For Each'

  for (ForEach_itr = 0; ForEach_itr < N_MOTORS; ForEach_itr++) {
    // ForEachSliceSelector generated from: '<S1>/SensorData' incorporates:
    //   Inport: '<Root>/SensorData'

    rtb_ImpSel_InsertedFor_SensorData_at_outport_0 =
      iterative_motion_controller_U.SensorData[ForEach_itr];
    for (i = 0; i < MAX_EVENTS_PER_TICK; i++) {
      // ForEachSliceSelector generated from: '<S1>/Events'
      iterative_motion_controller_DW.ImpSel_InsertedFor_Events_at_outport_0[i] =
        iterative_motion_controller_DW.M[N_MOTORS * i + ForEach_itr];
    }

    // ForEachSliceSelector generated from: '<S1>/InitConf' incorporates:
    //   Constant: '<Root>/Constant'

    rtb_ImpSel_InsertedFor_InitConf_at_outport_0 = AmcfocInitConf[ForEach_itr];

    // ModelReference: '<S1>/Motion Controller' incorporates:
    //   Inport: '<Root>/ExternalFlags'

    mc_step_1ms(&iterative_motion_controller_U.ExternalFlags_j,
                &iterative_motion_controller_DW.ImpSel_InsertedFor_Events_at_outport_0
                [0], &rtb_ImpSel_InsertedFor_InitConf_at_outport_0,
                &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                MotionController_o2,
                &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Flags_l,
                &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                ConfigurationParameters,
                &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                  MotionController_InstanceData.rtb),
                &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                  MotionController_InstanceData.rtdw));

    // RateTransition generated from: '<S1>/Model'
    rtw_mutex_lock();
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtModelInport1_RDBuf =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtModelInport1_LstBufWR;
    rtw_mutex_unlock();
    switch (iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
            TmpRTBAtModelInport1_RDBuf) {
     case 0:
      rtb_TmpRTBAtModelInport1 =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtModelInport1_Buf0;
      break;

     case 1:
      rtb_TmpRTBAtModelInport1 =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtModelInport1_Buf1;
      break;

     case 2:
      rtb_TmpRTBAtModelInport1 =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtModelInport1_Buf2;
      break;
    }

    // RateTransition generated from: '<S1>/Model'
    rtw_mutex_lock();
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtModelInport4_RDBuf =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtModelInport4_LstBufWR;
    rtw_mutex_unlock();
    switch (iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
            TmpRTBAtModelInport4_RDBuf) {
     case 0:
      rtb_TmpRTBAtModelInport4 =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtModelInport4_Buf0;
      break;

     case 1:
      rtb_TmpRTBAtModelInport4 =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtModelInport4_Buf1;
      break;

     case 2:
      rtb_TmpRTBAtModelInport4 =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtModelInport4_Buf2;
      break;
    }

    // ModelReference generated from: '<S1>/Model' incorporates:
    //   Inport: '<Root>/ExternalFlags'

    SupervisorFSM_TX(&rtb_TmpRTBAtModelInport1,
                     &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                     MotionController_o2,
                     &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                     Flags_l, &rtb_TmpRTBAtModelInport4,
                     &iterative_motion_controller_U.ExternalFlags_j.fault_button,
                     &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                     MessagesTx,
                     &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                     Model_o2,
                     &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                       Model_InstanceData.rtdw));

    // ForEachSliceAssignment generated from: '<S1>/Messages' incorporates:
    //   ModelReference generated from: '<S1>/Model'

    rtb_ImpAsg_InsertedFor_Messages_at_inport_0[ForEach_itr] =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].MessagesTx;

    // ForEachSliceAssignment generated from: '<S1>/MessagesFlags'
    rtb_ImpAsg_InsertedFor_MessagesFlags_at_inport_0[ForEach_itr] =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Model_o2;

    // ForEachSliceAssignment generated from: '<S1>/Flags'
    rtb_ImpAsg_InsertedFor_Flags_at_inport_0[ForEach_itr] =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Flags_l;

    // ForEachSliceAssignment generated from: '<S1>/Estimates'
    rtb_ImpAsg_InsertedFor_Estimates_at_inport_0[ForEach_itr] =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].MotionController_o2;

    // ForEachSliceAssignment generated from: '<S1>/ConfigurationParameters' incorporates:
    //   ModelReference: '<S1>/Motion Controller'

    rtb_ImpAsg_InsertedFor_ConfigurationParameters_at_inport_0[ForEach_itr] =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      ConfigurationParameters;
  }

  // End of Outputs for SubSystem: '<Root>/Iterative Motion Controller'

  // Outport: '<Root>/ConfigurationParameters' incorporates:
  //   ForEachSliceAssignment generated from: '<S1>/ConfigurationParameters'

  std::memcpy(&iterative_motion_controller_Y.ConfigurationParameters[0],
              &rtb_ImpAsg_InsertedFor_ConfigurationParameters_at_inport_0[0],
              static_cast<uint32_T>(N_MOTORS) * sizeof(ActuatorConfiguration));

  // Outport: '<Root>/Messages' incorporates:
  //   ForEachSliceAssignment generated from: '<S1>/Messages'

  std::memcpy(&iterative_motion_controller_Y.Messages[0],
              &rtb_ImpAsg_InsertedFor_Messages_at_inport_0[0],
              static_cast<uint32_T>(N_MOTORS) * sizeof(BUS_MESSAGES_TX));
  for (i = 0; i < N_MOTORS; i++) {
    // Outport: '<Root>/Estimates'
    iterative_motion_controller_Y.Estimates[i] =
      rtb_ImpAsg_InsertedFor_Estimates_at_inport_0[i];

    // Outport: '<Root>/Flags'
    iterative_motion_controller_Y.Flags_d[i] =
      rtb_ImpAsg_InsertedFor_Flags_at_inport_0[i];

    // Outport: '<Root>/MessagesFlags'
    iterative_motion_controller_Y.MessagesFlags[i] =
      rtb_ImpAsg_InsertedFor_MessagesFlags_at_inport_0[i];
  }
}

// Model initialize function
void AMCFOC_initialize(void)
{
  // Registration code

  // Set task counter limit used by the static main program
  (iterative_motion_controller_M)->Timing.TaskCounters.cLimit[0] = 1;
  (iterative_motion_controller_M)->Timing.TaskCounters.cLimit[1] = 9;
  (iterative_motion_controller_M)->Timing.TaskCounters.cLimit[2] = 200;

  // Model Initialize function for ModelReference Block: '<S1>/Motion Controller' 
  {
    int32_T i_1;
    for (i_1 = 0; i_1 < 2; i_1++) {
      motion_controller_initialize
        (&(iterative_motion_controller_DW.CoreSubsys[i_1].
           MotionController_InstanceData.rtdw));
    }
  }

  {
    // local scratch DWork variables
    int32_T ForEach_itr;
    ReceivedEvents struct_temp;
    int32_T i;

    // SystemInitialize for Iterator SubSystem: '<Root>/Iterative Motion Controller' 
    for (ForEach_itr = 0; ForEach_itr < N_MOTORS; ForEach_itr++) {
      // Start for RateTransition generated from: '<S1>/Model'
      rtw_mutex_init();

      // Start for RateTransition generated from: '<S1>/Model'
      rtw_mutex_init();

      // SystemInitialize for ModelReference generated from: '<S1>/Model'
      SupervisorFSM_TX_Init
        (&iterative_motion_controller_DW.CoreSubsys[ForEach_itr].MessagesTx,
         &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Model_o2);

      // SystemInitialize for ModelReference: '<S1>/Motion Controller'
      motion_controller_Init
        (&iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Flags_l,
         &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
         ConfigurationParameters,
         &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
           MotionController_InstanceData.rtb),
         &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
           MotionController_InstanceData.rtdw));
    }

    // End of SystemInitialize for SubSystem: '<Root>/Iterative Motion Controller' 

    // SystemInitialize for Chart: '<Root>/Sort Events'
    struct_temp.motor_id = 0U;
    struct_temp.event_type = EventTypes_None;
    struct_temp.targets_content.position = 0.0F;
    struct_temp.targets_content.velocity = 0.0F;
    struct_temp.targets_content.current = 0.0F;
    struct_temp.targets_content.voltage = 0.0F;
    struct_temp.pid_content.type = ControlModes_NotConfigured;
    struct_temp.pid_content.OutMax = 0.0F;
    struct_temp.pid_content.OutMin = 0.0F;
    struct_temp.pid_content.P = 0.0F;
    struct_temp.pid_content.I = 0.0F;
    struct_temp.pid_content.D = 0.0F;
    struct_temp.pid_content.N = 0.0F;
    struct_temp.pid_content.I0 = 0.0F;
    struct_temp.pid_content.D0 = 0.0F;
    struct_temp.pid_content.shift_factor = 0U;
    struct_temp.control_mode_content = ControlModes_NotConfigured;
    struct_temp.limits_content.overload = 0.0F;
    struct_temp.limits_content.peak = 0.0F;
    struct_temp.limits_content.nominal = 0.0F;
    struct_temp.limits_content.type = ControlModes_NotConfigured;
    struct_temp.motor_config_content.enable_verbosity = false;
    struct_temp.motor_config_content.has_hall_sens = false;
    struct_temp.motor_config_content.has_quadrature_encoder = false;
    struct_temp.motor_config_content.has_speed_quadrature_encoder = false;
    struct_temp.motor_config_content.has_temperature_sens = false;
    struct_temp.motor_config_content.encoder_tolerance = 0U;
    struct_temp.motor_config_content.pole_pairs = 0U;
    struct_temp.motor_config_content.rotor_encoder_resolution = 0;
    struct_temp.motor_config_content.rotor_index_offset = 0;
    struct_temp.motor_config_content.use_index = false;
    for (i = 0; i < MAX_EVENTS_PER_TICK * N_MOTORS; i++) {
      iterative_motion_controller_DW.M[i] = struct_temp;
    }

    // End of SystemInitialize for Chart: '<Root>/Sort Events'
    // Enable for Iterator SubSystem: '<Root>/Iterative Motion Controller'
    for (ForEach_itr = 0; ForEach_itr < N_MOTORS; ForEach_itr++) {
      // Enable for Iterator SubSystem: '<Root>/Iterative Motion Controller'
      // Enable for ModelReference: '<S1>/Motion Controller'
      motion_controller_Enable
        (&(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
           MotionController_InstanceData.rtdw));

      // End of Enable for SubSystem: '<Root>/Iterative Motion Controller'
    }

    // End of Enable for SubSystem: '<Root>/Iterative Motion Controller'
  }
}

// Model terminate function
void AMCFOC_terminate(void)
{
  // local scratch DWork variables
  int32_T ForEach_itr;

  // Terminate for Iterator SubSystem: '<Root>/Iterative Motion Controller'
  for (ForEach_itr = 0; ForEach_itr < N_MOTORS; ForEach_itr++) {
    // Terminate for ModelReference: '<S1>/Motion Controller'
    motion_controller_Term
      (&(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
         MotionController_InstanceData.rtdw));

    // Terminate for RateTransition generated from: '<S1>/Model'
    rtw_mutex_destroy();

    // Terminate for RateTransition generated from: '<S1>/Model'
    rtw_mutex_destroy();
  }

  // End of Terminate for SubSystem: '<Root>/Iterative Motion Controller'
}

boolean_T RT_MODEL_iterative_motion_controller_T::StepTask(int32_T idx) const
{
  return (Timing.TaskCounters.TID[(idx)] == 0);
}

uint32_T &RT_MODEL_iterative_motion_controller_T::CounterLimit(int32_T idx)
{
  return (Timing.TaskCounters.cLimit[(idx)]);
}

uint32_T &RT_MODEL_iterative_motion_controller_T::TaskCounter(int32_T idx)
{
  return (Timing.TaskCounters.TID[(idx)]);
}

//
// File trailer for generated code.
//
// [EOF]
//
