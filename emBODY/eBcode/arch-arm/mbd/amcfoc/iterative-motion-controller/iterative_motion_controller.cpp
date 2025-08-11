//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: iterative_motion_controller.cpp
//
// Code generated for Simulink model 'iterative_motion_controller'.
//
// Model version                  : 4.14
// Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
// C/C++ source code generated on : Mon Aug 11 10:40:02 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "iterative_motion_controller.h"
#include <cstring>
#include "rtw_mutex.h"
#include "rtwtypes.h"
#include "iterative_motion_controller_types.h"
#include "iterative_motion_controller_private.h"
#include "mul_s32_sat.h"
#include "rtw_defines.h"
#include "control_foc.h"
#include "motion_controller.h"
#include "SupervisorFSM_TX.h"

const JointData iterative_motion_controller_rtZJointData = {
  0.0F                                 // position
};                                     // JointData ground

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
      0.0F,
      0.0F,
      0.0F,
      0.0F,
      0.0F,
      ReferenceEncoder_Motor
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
      0.0F,
      0.0F,
      0.0F,
      0.0F,
      0.0F,
      ReferenceEncoder_Motor
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
  // local scratch DWork variables
  int32_T ForEach_itr;
}

// Model step function for TID1
void AMCFOC_step_FOC(void)             // Sample time: [4.5e-05s, 0.0s]
{
  // local scratch DWork variables
  int32_T ForEach_itr;
  FOCOutputs rtb_ImpAsg_InsertedFor_FOCOutputs_at_inport_0[2];
  SensorsData rtb_ImpSel_InsertedFor_SensorData_at_outport_0;
  int8_T wrBufIdx;

  // Outputs for Iterator SubSystem: '<Root>/Iterative Motion Controller' incorporates:
  //   ForEach: '<S1>/For Each'

  for (ForEach_itr = 0; ForEach_itr < 2; ForEach_itr++) {
    // ForEachSliceSelector generated from: '<S1>/SensorData' incorporates:
    //   Inport: '<Root>/SensorData'

    rtb_ImpSel_InsertedFor_SensorData_at_outport_0 =
      iterative_motion_controller_U.SensorData[ForEach_itr];

    // RateTransition: '<S1>/Rate Transition1'
    rtw_mutex_lock();
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].RateTransition1_RDBuf
      = iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      RateTransition1_LstBufWR;
    rtw_mutex_unlock();

    // RateTransition: '<S1>/Rate Transition1'
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].RateTransition1 =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr]
      .RateTransition1_Buf[iterative_motion_controller_DW.CoreSubsys[ForEach_itr]
      .RateTransition1_RDBuf];

    // ModelReference: '<S1>/FOC' incorporates:
    //   Constant: '<S1>/Constant'

    control_foc(&rtb_ImpSel_InsertedFor_SensorData_at_outport_0,
                &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                RateTransition1, &rtCP_Constant_Value,
                &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].FOC,
                &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                  FOC_InstanceData.rtb),
                &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                  FOC_InstanceData.rtdw),
                &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                  FOC_InstanceData.rtzce));

    // RateTransition: '<S1>/Rate Transition2' incorporates:
    //   ModelReference: '<S1>/FOC'

    rtw_mutex_lock();
    wrBufIdx = static_cast<int8_T>
      (iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
       RateTransition2_LstBufWR + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        RateTransition2_RDBuf) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock();
    switch (wrBufIdx) {
     case 0:
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        RateTransition2_Buf0 =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].FOC;
      break;

     case 1:
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        RateTransition2_Buf1 =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].FOC;
      break;

     case 2:
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        RateTransition2_Buf2 =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].FOC;
      break;
    }

    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      RateTransition2_LstBufWR = wrBufIdx;

    // End of RateTransition: '<S1>/Rate Transition2'

    // RateTransition: '<S1>/Transition to 1 ms'
    rtw_mutex_lock();
    wrBufIdx = static_cast<int8_T>
      (iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
       Transitionto1ms_LstBufWR + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        Transitionto1ms_RDBuf) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock();
    switch (wrBufIdx) {
     case 0:
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        Transitionto1ms_Buf0 = rtb_ImpSel_InsertedFor_SensorData_at_outport_0;
      break;

     case 1:
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        Transitionto1ms_Buf1 = rtb_ImpSel_InsertedFor_SensorData_at_outport_0;
      break;

     case 2:
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        Transitionto1ms_Buf2 = rtb_ImpSel_InsertedFor_SensorData_at_outport_0;
      break;
    }

    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      Transitionto1ms_LstBufWR = wrBufIdx;

    // End of RateTransition: '<S1>/Transition to 1 ms'

    // ForEachSliceAssignment generated from: '<S1>/FOCOutputs' incorporates:
    //   ModelReference: '<S1>/FOC'

    rtb_ImpAsg_InsertedFor_FOCOutputs_at_inport_0[ForEach_itr] =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].FOC;
  }

  // End of Outputs for SubSystem: '<Root>/Iterative Motion Controller'

  // Outport: '<Root>/FOCOutputs' incorporates:
  //   ForEachSliceAssignment generated from: '<S1>/FOCOutputs'

  std::memcpy(&iterative_motion_controller_Y.FOCOutputs_h[0],
              &rtb_ImpAsg_InsertedFor_FOCOutputs_at_inport_0[0], sizeof
              (FOCOutputs) << 1U);
}

// Model step function for TID2
void AMCFOC_step_Time_1ms(void)        // Sample time: [0.001s, 0.0s]
{
  // local scratch DWork variables
  int32_T ForEach_itr;
  ActuatorConfiguration
    rtb_ImpAsg_InsertedFor_ConfigurationParameters_at_inport_0[2];
  ActuatorConfiguration rtb_ImpSel_InsertedFor_InitConf_at_outport_0;
  BUS_MESSAGES_TX rtb_ImpAsg_InsertedFor_Messages_at_inport_0[2];
  BUS_STATUS_TX rtb_ImpAsg_InsertedFor_MessagesFlags_at_inport_0[2];
  EstimatedData rtb_ImpAsg_InsertedFor_Estimates_at_inport_0[2];
  FOCOutputs rtb_RateTransition2;
  Flags rtb_ImpAsg_InsertedFor_Flags_at_inport_0[2];
  ReceivedEvents M[8];
  ReceivedEvents rtb_ImpSel_InsertedFor_Events_at_outport_0[4];
  ReceivedEvents struct_temp;
  SensorsData rtb_Transitionto1ms;
  int32_T i;
  int8_T wrBufIdx;
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
  for (i = 0; i < 8; i++) {
    M[i] = struct_temp;
  }

  //  Bin Events
  idx = 0U;
  counter_motor_1 = 0U;
  counter_motor_2 = 0U;
  while (idx < mul_s32_sat(N_MOTORS, MAX_EVENTS_PER_TICK)) {
    if ((iterative_motion_controller_U.EventsList[idx].motor_id == 0) &&
        (counter_motor_1 < MAX_EVENTS_PER_TICK)) {
      M[iterative_motion_controller_U.EventsList[idx].motor_id +
        (counter_motor_1 << 1)] = iterative_motion_controller_U.EventsList[idx];
      i = counter_motor_1 + 1;
      if (counter_motor_1 + 1 > 255) {
        i = 255;
      }

      counter_motor_1 = static_cast<uint8_T>(i);
    } else if ((iterative_motion_controller_U.EventsList[idx].motor_id == 1) &&
               (counter_motor_2 < MAX_EVENTS_PER_TICK)) {
      M[iterative_motion_controller_U.EventsList[idx].motor_id +
        (counter_motor_2 << 1)] = iterative_motion_controller_U.EventsList[idx];
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

  for (ForEach_itr = 0; ForEach_itr < 2; ForEach_itr++) {
    // ForEachSliceSelector generated from: '<S1>/Events'
    rtb_ImpSel_InsertedFor_Events_at_outport_0[0] = M[ForEach_itr];
    rtb_ImpSel_InsertedFor_Events_at_outport_0[1] = M[ForEach_itr + 2];
    rtb_ImpSel_InsertedFor_Events_at_outport_0[2] = M[ForEach_itr + 4];
    rtb_ImpSel_InsertedFor_Events_at_outport_0[3] = M[ForEach_itr + 6];

    // ForEachSliceSelector generated from: '<S1>/InitConf' incorporates:
    //   Constant: '<Root>/Constant'

    rtb_ImpSel_InsertedFor_InitConf_at_outport_0 = AmcfocInitConf[ForEach_itr];

    // RateTransition: '<S1>/Transition to 1 ms'
    rtw_mutex_lock();
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Transitionto1ms_RDBuf
      = iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      Transitionto1ms_LstBufWR;
    rtw_mutex_unlock();
    switch (iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
            Transitionto1ms_RDBuf) {
     case 0:
      rtb_Transitionto1ms =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        Transitionto1ms_Buf0;
      break;

     case 1:
      rtb_Transitionto1ms =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        Transitionto1ms_Buf1;
      break;

     case 2:
      rtb_Transitionto1ms =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        Transitionto1ms_Buf2;
      break;
    }

    // End of RateTransition: '<S1>/Transition to 1 ms'

    // RateTransition: '<S1>/Rate Transition2'
    rtw_mutex_lock();
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].RateTransition2_RDBuf
      = iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      RateTransition2_LstBufWR;
    rtw_mutex_unlock();
    switch (iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
            RateTransition2_RDBuf) {
     case 0:
      rtb_RateTransition2 =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        RateTransition2_Buf0;
      break;

     case 1:
      rtb_RateTransition2 =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        RateTransition2_Buf1;
      break;

     case 2:
      rtb_RateTransition2 =
        iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        RateTransition2_Buf2;
      break;
    }

    // End of RateTransition: '<S1>/Rate Transition2'

    // ModelReference: '<S1>/Motion Controller' incorporates:
    //   Inport: '<Root>/ExternalFlags'

    mc_step_1ms(&rtb_Transitionto1ms,
                &iterative_motion_controller_U.ExternalFlags_j,
                &rtb_ImpSel_InsertedFor_Events_at_outport_0[0],
                &rtb_ImpSel_InsertedFor_InitConf_at_outport_0,
                &iterative_motion_controller_rtZJointData, &rtb_RateTransition2,
                &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                MotionController_o1,
                &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Flags_p,
                &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                ConfigurationParameters,
                &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                MotionController_o4,
                &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                  MotionController_InstanceData.rtb),
                &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                  MotionController_InstanceData.rtdw));

    // ModelReference generated from: '<S1>/Model' incorporates:
    //   Inport: '<Root>/ExternalFlags'

    SupervisorFSM_TX(&rtb_Transitionto1ms,
                     &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                     MotionController_o1,
                     &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                     Flags_p, &rtb_RateTransition2,
                     &iterative_motion_controller_U.ExternalFlags_j.fault_button,
                     &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                     MessagesTx,
                     &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                     Model_o2,
                     &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                       Model_InstanceData.rtdw));

    // RateTransition: '<S1>/Rate Transition1' incorporates:
    //   ModelReference: '<S1>/Motion Controller'

    rtw_mutex_lock();
    wrBufIdx = static_cast<int8_T>
      (iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
       RateTransition1_LstBufWR + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        RateTransition1_RDBuf) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock();
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr]
      .RateTransition1_Buf[wrBufIdx] =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].MotionController_o4;
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      RateTransition1_LstBufWR = wrBufIdx;

    // End of RateTransition: '<S1>/Rate Transition1'

    // ForEachSliceAssignment generated from: '<S1>/Messages' incorporates:
    //   ModelReference generated from: '<S1>/Model'

    rtb_ImpAsg_InsertedFor_Messages_at_inport_0[ForEach_itr] =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].MessagesTx;

    // ForEachSliceAssignment generated from: '<S1>/MessagesFlags'
    rtb_ImpAsg_InsertedFor_MessagesFlags_at_inport_0[ForEach_itr] =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Model_o2;

    // ForEachSliceAssignment generated from: '<S1>/Flags'
    rtb_ImpAsg_InsertedFor_Flags_at_inport_0[ForEach_itr] =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Flags_p;

    // ForEachSliceAssignment generated from: '<S1>/Estimates'
    rtb_ImpAsg_InsertedFor_Estimates_at_inport_0[ForEach_itr] =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].MotionController_o1;

    // ForEachSliceAssignment generated from: '<S1>/ConfigurationParameters' incorporates:
    //   ModelReference: '<S1>/Motion Controller'

    rtb_ImpAsg_InsertedFor_ConfigurationParameters_at_inport_0[ForEach_itr] =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      ConfigurationParameters;
  }

  // End of Outputs for SubSystem: '<Root>/Iterative Motion Controller'

  // Outport: '<Root>/Estimates'
  iterative_motion_controller_Y.Estimates[0] =
    rtb_ImpAsg_InsertedFor_Estimates_at_inport_0[0];

  // Outport: '<Root>/Flags'
  iterative_motion_controller_Y.Flags_d[0] =
    rtb_ImpAsg_InsertedFor_Flags_at_inport_0[0];

  // Outport: '<Root>/ConfigurationParameters' incorporates:
  //   ForEachSliceAssignment generated from: '<S1>/ConfigurationParameters'

  std::memcpy(&iterative_motion_controller_Y.ConfigurationParameters[0],
              &rtb_ImpAsg_InsertedFor_ConfigurationParameters_at_inport_0[0],
              sizeof(ActuatorConfiguration) << 1U);

  // Outport: '<Root>/Messages' incorporates:
  //   ForEachSliceAssignment generated from: '<S1>/Messages'

  std::memcpy(&iterative_motion_controller_Y.Messages[0],
              &rtb_ImpAsg_InsertedFor_Messages_at_inport_0[0], sizeof
              (BUS_MESSAGES_TX) << 1U);

  // Outport: '<Root>/MessagesFlags'
  iterative_motion_controller_Y.MessagesFlags[0] =
    rtb_ImpAsg_InsertedFor_MessagesFlags_at_inport_0[0];

  // Outport: '<Root>/Estimates'
  iterative_motion_controller_Y.Estimates[1] =
    rtb_ImpAsg_InsertedFor_Estimates_at_inport_0[1];

  // Outport: '<Root>/Flags'
  iterative_motion_controller_Y.Flags_d[1] =
    rtb_ImpAsg_InsertedFor_Flags_at_inport_0[1];

  // Outport: '<Root>/MessagesFlags'
  iterative_motion_controller_Y.MessagesFlags[1] =
    rtb_ImpAsg_InsertedFor_MessagesFlags_at_inport_0[1];
}

// Model initialize function
void AMCFOC_initialize(void)
{
  // Registration code

  // Set task counter limit used by the static main program
  (iterative_motion_controller_M)->Timing.TaskCounters.cLimit[0] = 1;
  (iterative_motion_controller_M)->Timing.TaskCounters.cLimit[1] = 9;
  (iterative_motion_controller_M)->Timing.TaskCounters.cLimit[2] = 200;

  // Model Initialize function for ModelReference Block: '<S1>/FOC'
  {
    int32_T i_1;
    for (i_1 = 0; i_1 < 2; i_1++) {
      control_foc_initialize(&(iterative_motion_controller_DW.CoreSubsys[i_1].
        FOC_InstanceData.rtzce));
    }
  }

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
    static const FOCSlowInputs tmp = { { { 0.0F,// environment_temperature
          0.0F,                        // current_rms_lambda
          EstimationVelocityModes_Disabled,// velocity_est_mode
          0U                           // velocity_est_window
        }                              // estimation
      },                               // global_configuration

      { { 0.0F,                        // jntVelMax
          0.0F,                        // motorNominalCurrents
          0.0F,                        // motorPeakCurrents
          0.0F,                        // motorOverloadCurrents
          0U,                          // motorPwmLimit
          0.0F                         // motorCriticalTemperature
        },                             // thresholds

        { { ControlModes_NotConfigured,// type
            0.0F,                      // OutMax
            0.0F,                      // OutMin
            0.0F,                      // P
            0.0F,                      // I
            0.0F,                      // D
            0.0F,                      // N
            0.0F,                      // I0
            0.0F,                      // D0
            0U                         // shift_factor
          },                           // currentPID

          { ControlModes_NotConfigured,// type
            0.0F,                      // OutMax
            0.0F,                      // OutMin
            0.0F,                      // P
            0.0F,                      // I
            0.0F,                      // D
            0.0F,                      // N
            0.0F,                      // I0
            0.0F,                      // D0
            0U                         // shift_factor
          },                           // velocityPID

          { ControlModes_NotConfigured,// type
            0.0F,                      // OutMax
            0.0F,                      // OutMin
            0.0F,                      // P
            0.0F,                      // I
            0.0F,                      // D
            0.0F,                      // N
            0.0F,                      // I0
            0.0F,                      // D0
            0U                         // shift_factor
          }                            // positionPID
        },                             // pids

        { { false,                     // enable_verbosity
            false,                     // has_hall_sens
            false,                     // has_quadrature_encoder
            false,                     // has_speed_quadrature_encoder
            false,                     // has_temperature_sens
            0U,                        // encoder_tolerance
            0U,                        // pole_pairs
            0,                         // rotor_encoder_resolution
            0,                         // rotor_index_offset
            false                      // use_index
          },                           // externals
          0.0F,                        // Kbemf
          0.0F,                        // Rphase
          0.0F,                        // Imin
          0.0F,                        // Imax
          0.0F,                        // Vmax
          0.0F,                        // resistance
          0.0F,                        // inductance
          0.0F,                        // thermal_resistance
          0.0F,                        // thermal_time_constant
          0.0F,                        // hall_sensors_offset
          ReferenceEncoder_Motor       // reference_encoder
        }                              // motor
      },                               // actuator_configuration

      { 0.0F,                          // rotor_velocity
        0.0F,                          // Iq_filtered
        0.0F,                          // motor_temperature
        0.0F                           // joint_velocity
      },                               // estimated_data

      { 0.0F,                          // position
        0.0F,                          // velocity
        0.0F,                          // current
        0.0F                           // voltage
      },                               // targets

      { false,                         // vel_en
        false,                         // cur_en
        false,                         // out_en
        false,                         // pid_reset
        0.0F,                          // motorcurrent
        0.0F                           // current_limiter
      }                                // control_outer_outputs
    };

    // SystemInitialize for Iterator SubSystem: '<Root>/Iterative Motion Controller' 
    for (ForEach_itr = 0; ForEach_itr < 2; ForEach_itr++) {
      // SystemInitialize for RateTransition: '<S1>/Rate Transition1'
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].RateTransition1 =
        tmp;

      // Start for RateTransition: '<S1>/Rate Transition1'
      rtw_mutex_init();

      // Start for RateTransition: '<S1>/Rate Transition2'
      rtw_mutex_init();

      // Start for RateTransition: '<S1>/Transition to 1 ms'
      rtw_mutex_init();

      // SystemInitialize for ModelReference: '<S1>/FOC'
      control_foc_Init(&(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                         FOC_InstanceData.rtdw));

      // SystemInitialize for ModelReference generated from: '<S1>/Model'
      SupervisorFSM_TX_Init
        (&iterative_motion_controller_DW.CoreSubsys[ForEach_itr].MessagesTx,
         &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Model_o2);

      // SystemInitialize for ModelReference: '<S1>/Motion Controller'
      motion_controller_Init
        (&iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Flags_p,
         &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
         ConfigurationParameters,
         &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
         MotionController_o4,
         &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
           MotionController_InstanceData.rtb),
         &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
           MotionController_InstanceData.rtdw));
    }

    // End of SystemInitialize for SubSystem: '<Root>/Iterative Motion Controller' 
    // Enable for Iterator SubSystem: '<Root>/Iterative Motion Controller'
    for (ForEach_itr = 0; ForEach_itr < 2; ForEach_itr++) {
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
  for (ForEach_itr = 0; ForEach_itr < 2; ForEach_itr++) {
    // Terminate for RateTransition: '<S1>/Rate Transition1'
    rtw_mutex_destroy();

    // Terminate for RateTransition: '<S1>/Rate Transition2'
    rtw_mutex_destroy();

    // Terminate for RateTransition: '<S1>/Transition to 1 ms'
    rtw_mutex_destroy();

    // Terminate for ModelReference: '<S1>/Motion Controller'
    motion_controller_Term
      (&(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
         MotionController_InstanceData.rtdw));
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
