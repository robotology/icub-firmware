//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: iterative_motion_controller.cpp
//
// Code generated for Simulink model 'iterative_motion_controller'.
//
// Model version                  : 6.1
// Simulink Coder version         : 26.1 (R2026a) 20-Nov-2025
// C/C++ source code generated on : Tue Oct 20 12:03:16 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "iterative_motion_controller.h"
#include "rtw_mutex.h"
#include "rtwtypes.h"
#include "iterative_motion_controller_types.h"
#include "rtw_defines.h"
#include "process_sensors.h"
#include "control_foc.h"
#include "motion_controller.h"
#include "SupervisorFSM_TX.h"

const JointData iterative_motion_controller_rtZJointData = {
  0.0F                                 // position
};                                     // JointData ground

// Exported block parameters
ActuatorConfiguration Amcmj1InitConf = {
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
    30.0F,
    true,
    ReferenceEncoder_Motor
  }
} ;                                    // Variable: Amcmj1InitConf
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
  // local block i/o variables
  SensorsData rtb_ProcessSensors;
  FOCOutputs rtb_FOC;

  // local scratch DWork variables
  int32_T ForEach_itr;
  ActuatorConfiguration rtb_TmpRTBAtProcessSensorsInport1;
  FOCOutputs rtb_ImpAsg_InsertedFor_FOCOutputs_at_inport_0;
  Flags rtb_TmpRTBAtProcessSensorsInport2;
  SensorsData rtb_TmpRTBAtFOCInport4;
  int8_T wrBufIdx;

  // Outputs for Iterator SubSystem: '<Root>/Iterative Motion Controller' incorporates:
  //   ForEach: '<S1>/For Each'

  for (ForEach_itr = 0; ForEach_itr < 1; ForEach_itr++) {
    // RateTransition generated from: '<S1>/Process Sensors'
    rtw_mutex_lock();
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtProcessSensorsInport1_RDBuf =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtProcessSensorsInport1_LstBufWR;
    rtw_mutex_unlock();
    rtb_TmpRTBAtProcessSensorsInport1 =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtProcessSensorsInport1_Buf[iterative_motion_controller_DW.CoreSubsys
      [ForEach_itr].TmpRTBAtProcessSensorsInport1_RDBuf];

    // RateTransition generated from: '<S1>/Process Sensors'
    rtw_mutex_lock();
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtProcessSensorsInport2_RDBuf =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtProcessSensorsInport2_LstBufWR;
    rtw_mutex_unlock();
    rtb_TmpRTBAtProcessSensorsInport2 =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtProcessSensorsInport2_Buf[iterative_motion_controller_DW.CoreSubsys
      [ForEach_itr].TmpRTBAtProcessSensorsInport2_RDBuf];

    // ModelReference generated from: '<S1>/Process Sensors' incorporates:
    //   Inport: '<Root>/SensorData'

    process_sensors(&rtb_TmpRTBAtProcessSensorsInport1,
                    &rtb_TmpRTBAtProcessSensorsInport2,
                    &iterative_motion_controller_U.SensorData,
                    &rtb_ProcessSensors,
                    &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                      ProcessSensors_InstanceData.rtb),
                    &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                      ProcessSensors_InstanceData.rtdw),
                    &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                      ProcessSensors_InstanceData.rtzce));

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

    // RateTransition generated from: '<S1>/FOC'
    rtw_mutex_lock();
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtFOCInport3_RDBuf =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtFOCInport3_LstBufWR;
    rtw_mutex_unlock();
    rtb_TmpRTBAtProcessSensorsInport2 =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtFOCInport3_Buf[iterative_motion_controller_DW.CoreSubsys[ForEach_itr]
      .TmpRTBAtFOCInport3_RDBuf];

    // RateTransition generated from: '<S1>/FOC'
    rtw_mutex_lock();
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtFOCInport4_RDBuf =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtFOCInport4_LstBufWR;
    rtw_mutex_unlock();
    rtb_TmpRTBAtFOCInport4 =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtFOCInport4_Buf[iterative_motion_controller_DW.CoreSubsys[ForEach_itr]
      .TmpRTBAtFOCInport4_RDBuf];

    // ModelReference generated from: '<S1>/FOC'
    control_foc(&rtb_ProcessSensors,
                &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                RateTransition1, &rtb_TmpRTBAtProcessSensorsInport2,
                &rtb_TmpRTBAtFOCInport4, &rtb_FOC,
                &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                  FOC_InstanceData.rtb),
                &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                  FOC_InstanceData.rtdw),
                &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                  FOC_InstanceData.rtzce));

    // RateTransition: '<S1>/Rate Transition2' incorporates:
    //   ModelReference generated from: '<S1>/FOC'

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
        RateTransition2_Buf0 = rtb_FOC;
      break;

     case 1:
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        RateTransition2_Buf1 = rtb_FOC;
      break;

     case 2:
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        RateTransition2_Buf2 = rtb_FOC;
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
        Transitionto1ms_Buf0 = rtb_ProcessSensors;
      break;

     case 1:
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        Transitionto1ms_Buf1 = rtb_ProcessSensors;
      break;

     case 2:
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        Transitionto1ms_Buf2 = rtb_ProcessSensors;
      break;
    }

    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      Transitionto1ms_LstBufWR = wrBufIdx;

    // End of RateTransition: '<S1>/Transition to 1 ms'

    // ForEachSliceAssignment generated from: '<S1>/FOCOutputs' incorporates:
    //   ModelReference generated from: '<S1>/FOC'

    rtb_ImpAsg_InsertedFor_FOCOutputs_at_inport_0 = rtb_FOC;
  }

  // End of Outputs for SubSystem: '<Root>/Iterative Motion Controller'

  // Outport: '<Root>/FOCOutputs' incorporates:
  //   ForEachSliceAssignment generated from: '<S1>/FOCOutputs'

  iterative_motion_controller_Y.FOCOutputs_h =
    rtb_ImpAsg_InsertedFor_FOCOutputs_at_inport_0;
}

// Model step function for TID2
void AMCFOC_step_Time_1ms(void)        // Sample time: [0.001s, 0.0s]
{
  // local block i/o variables
  FOCSlowInputs rtb_MotionController_o4;

  // local scratch DWork variables
  int32_T ForEach_itr;
  ActuatorConfiguration
    rtb_ImpAsg_InsertedFor_ConfigurationParameters_at_inport_0;
  BUS_MESSAGES_TX rtb_ImpAsg_InsertedFor_Messages_at_inport_0;
  BUS_STATUS_TX rtb_ImpAsg_InsertedFor_MessagesFlags_at_inport_0;
  EstimatedData rtb_ImpAsg_InsertedFor_Estimates_at_inport_0;
  FOCOutputs rtb_RateTransition2;
  Flags rtb_ImpAsg_InsertedFor_Flags_at_inport_0;
  ReceivedEvents M[4];
  ReceivedEvents struct_temp;
  SensorsData rtb_Transitionto1ms;
  int32_T tmp;
  int8_T wrBufIdx;
  uint8_T counter_motor_1;
  uint8_T counter_motor_2;
  uint8_T idx;

  // Chart: '<Root>/Sort Events' incorporates:
  //   Inport: '<Root>/EventsList'

  struct_temp.motor_id = 0U;
  struct_temp.event_type = EventTypes_None;
  struct_temp.targets_content.trajectory_time = 0.0F;
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
  struct_temp.motor_config_set.key = MCMotorParamsSet_None;
  struct_temp.motor_config_set.value[0] = 0.0F;
  struct_temp.motor_config_set.value[1] = 0.0F;
  M[0] = struct_temp;
  M[1] = struct_temp;
  M[2] = struct_temp;
  M[3] = struct_temp;

  //  Bin Events
  idx = 0U;
  counter_motor_1 = 0U;
  counter_motor_2 = 0U;
  while (idx < N_MOTORS * static_cast<real_T>(MAX_EVENTS_PER_TICK)) {
    if ((iterative_motion_controller_U.EventsList[idx].motor_id == 0) &&
        (counter_motor_1 < MAX_EVENTS_PER_TICK)) {
      M[counter_motor_1] = iterative_motion_controller_U.EventsList[idx];
      tmp = counter_motor_1 + 1;
      if (counter_motor_1 + 1 > 255) {
        tmp = 255;
      }

      counter_motor_1 = static_cast<uint8_T>(tmp);
    } else if ((iterative_motion_controller_U.EventsList[idx].motor_id == 1) &&
               (counter_motor_2 < MAX_EVENTS_PER_TICK)) {
      M[counter_motor_2] = iterative_motion_controller_U.EventsList[idx];
      tmp = counter_motor_2 + 1;
      if (counter_motor_2 + 1 > 255) {
        tmp = 255;
      }

      counter_motor_2 = static_cast<uint8_T>(tmp);
    }

    tmp = idx + 1;
    if (idx + 1 > 255) {
      tmp = 255;
    }

    idx = static_cast<uint8_T>(tmp);
  }

  // End of Chart: '<Root>/Sort Events'

  // Outputs for Iterator SubSystem: '<Root>/Iterative Motion Controller' incorporates:
  //   ForEach: '<S1>/For Each'

  for (ForEach_itr = 0; ForEach_itr < 1; ForEach_itr++) {
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
    //   Constant: '<Root>/Constant'
    //   ForEachSliceSelector generated from: '<S1>/Events'
    //   ForEachSliceSelector generated from: '<S1>/InitConf'
    //   Inport: '<Root>/ExternalFlags'

    mc_step_1ms(&rtb_Transitionto1ms,
                &iterative_motion_controller_U.ExternalFlags_j, &M[0],
                &Amcmj1InitConf, &iterative_motion_controller_rtZJointData,
                &rtb_RateTransition2,
                &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                MotionController_o1,
                &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Flags_a,
                &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                ConfigurationParameters, &rtb_MotionController_o4,
                &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                SensorsDataCalibration,
                &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                  MotionController_InstanceData.rtb),
                &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                  MotionController_InstanceData.rtdw));

    // RateTransition generated from: '<S1>/FOC'
    rtw_mutex_lock();
    wrBufIdx = static_cast<int8_T>
      (iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
       TmpRTBAtFOCInport3_LstBufWR + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtFOCInport3_RDBuf) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock();
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtFOCInport3_Buf[wrBufIdx] =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Flags_a;
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtFOCInport3_LstBufWR = wrBufIdx;

    // RateTransition generated from: '<S1>/FOC'
    rtw_mutex_lock();
    wrBufIdx = static_cast<int8_T>
      (iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
       TmpRTBAtFOCInport4_LstBufWR + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtFOCInport4_RDBuf) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock();
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtFOCInport4_Buf[wrBufIdx] =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      SensorsDataCalibration;
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtFOCInport4_LstBufWR = wrBufIdx;

    // ModelReference generated from: '<S1>/Model' incorporates:
    //   Inport: '<Root>/ExternalFlags'

    SupervisorFSM_TX(&rtb_Transitionto1ms,
                     &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                     MotionController_o1,
                     &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                     Flags_a, &rtb_RateTransition2,
                     &iterative_motion_controller_U.ExternalFlags_j.fault_button,
                     &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                     MessagesTx,
                     &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                     Model_o2,
                     &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                       Model_InstanceData.rtdw));

    // RateTransition generated from: '<S1>/Process Sensors' incorporates:
    //   ModelReference: '<S1>/Motion Controller'

    rtw_mutex_lock();
    wrBufIdx = static_cast<int8_T>
      (iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
       TmpRTBAtProcessSensorsInport1_LstBufWR + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtProcessSensorsInport1_RDBuf) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock();
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtProcessSensorsInport1_Buf[wrBufIdx] =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      ConfigurationParameters;
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtProcessSensorsInport1_LstBufWR = wrBufIdx;

    // RateTransition generated from: '<S1>/Process Sensors'
    rtw_mutex_lock();
    wrBufIdx = static_cast<int8_T>
      (iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
       TmpRTBAtProcessSensorsInport2_LstBufWR + 1);
    if (wrBufIdx == 3) {
      wrBufIdx = 0;
    }

    if (wrBufIdx == iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
        TmpRTBAtProcessSensorsInport2_RDBuf) {
      wrBufIdx = static_cast<int8_T>(wrBufIdx + 1);
      if (wrBufIdx == 3) {
        wrBufIdx = 0;
      }
    }

    rtw_mutex_unlock();
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtProcessSensorsInport2_Buf[wrBufIdx] =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Flags_a;
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      TmpRTBAtProcessSensorsInport2_LstBufWR = wrBufIdx;

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
      .RateTransition1_Buf[wrBufIdx] = rtb_MotionController_o4;
    iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      RateTransition1_LstBufWR = wrBufIdx;

    // End of RateTransition: '<S1>/Rate Transition1'

    // ForEachSliceAssignment generated from: '<S1>/Messages' incorporates:
    //   ModelReference generated from: '<S1>/Model'

    rtb_ImpAsg_InsertedFor_Messages_at_inport_0 =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].MessagesTx;

    // ForEachSliceAssignment generated from: '<S1>/MessagesFlags'
    rtb_ImpAsg_InsertedFor_MessagesFlags_at_inport_0 =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Model_o2;

    // ForEachSliceAssignment generated from: '<S1>/Flags'
    rtb_ImpAsg_InsertedFor_Flags_at_inport_0 =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Flags_a;

    // ForEachSliceAssignment generated from: '<S1>/Estimates'
    rtb_ImpAsg_InsertedFor_Estimates_at_inport_0 =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].MotionController_o1;

    // ForEachSliceAssignment generated from: '<S1>/ConfigurationParameters' incorporates:
    //   ModelReference: '<S1>/Motion Controller'

    rtb_ImpAsg_InsertedFor_ConfigurationParameters_at_inport_0 =
      iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
      ConfigurationParameters;
  }

  // End of Outputs for SubSystem: '<Root>/Iterative Motion Controller'

  // Outport: '<Root>/Flags'
  iterative_motion_controller_Y.Flags_d =
    rtb_ImpAsg_InsertedFor_Flags_at_inport_0;

  // Outport: '<Root>/ConfigurationParameters' incorporates:
  //   ForEachSliceAssignment generated from: '<S1>/ConfigurationParameters'

  iterative_motion_controller_Y.ConfigurationParameters =
    rtb_ImpAsg_InsertedFor_ConfigurationParameters_at_inport_0;

  // Outport: '<Root>/Messages' incorporates:
  //   ForEachSliceAssignment generated from: '<S1>/Messages'

  iterative_motion_controller_Y.Messages =
    rtb_ImpAsg_InsertedFor_Messages_at_inport_0;

  // Outport: '<Root>/MessagesFlags'
  iterative_motion_controller_Y.MessagesFlags =
    rtb_ImpAsg_InsertedFor_MessagesFlags_at_inport_0;

  // Outport: '<Root>/Estimates'
  iterative_motion_controller_Y.Estimates =
    rtb_ImpAsg_InsertedFor_Estimates_at_inport_0;
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
    for (i_1 = 0; i_1 < 1; i_1++) {
      motion_controller_initialize
        (&(iterative_motion_controller_DW.CoreSubsys[i_1].
           MotionController_InstanceData.rtdw));
    }
  }

  // Model Initialize function for ModelReference Block: '<S1>/FOC'
  {
    int32_T i_1;
    for (i_1 = 0; i_1 < 1; i_1++) {
      control_foc_initialize(&(iterative_motion_controller_DW.CoreSubsys[i_1].
        FOC_InstanceData.rtzce));
    }
  }

  // Model Initialize function for ModelReference Block: '<S1>/Process Sensors'
  {
    int32_T i_1;
    for (i_1 = 0; i_1 < 1; i_1++) {
      process_sensors_initialize(&(iterative_motion_controller_DW.CoreSubsys[i_1]
        .ProcessSensors_InstanceData.rtzce));
    }
  }

  {
    // local scratch DWork variables
    int32_T ForEach_itr;

    // SystemInitialize for Iterator SubSystem: '<Root>/Iterative Motion Controller' 
    for (ForEach_itr = 0; ForEach_itr < 1; ForEach_itr++) {
      // Start for RateTransition generated from: '<S1>/Process Sensors'
      rtw_mutex_init();

      // Start for RateTransition generated from: '<S1>/Process Sensors'
      rtw_mutex_init();

      // Start for RateTransition: '<S1>/Rate Transition1'
      rtw_mutex_init();

      // Start for RateTransition generated from: '<S1>/FOC'
      rtw_mutex_init();

      // Start for RateTransition generated from: '<S1>/FOC'
      rtw_mutex_init();

      // Start for RateTransition: '<S1>/Rate Transition2'
      rtw_mutex_init();

      // Start for RateTransition: '<S1>/Transition to 1 ms'
      rtw_mutex_init();

      // SystemInitialize for ModelReference generated from: '<S1>/FOC'
      control_foc_Init(&(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
                         FOC_InstanceData.rtdw));

      // SystemInitialize for ModelReference generated from: '<S1>/Model'
      SupervisorFSM_TX_Init
        (&iterative_motion_controller_DW.CoreSubsys[ForEach_itr].MessagesTx,
         &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Model_o2);

      // SystemInitialize for ModelReference: '<S1>/Motion Controller'
      motion_controller_Init
        (&iterative_motion_controller_DW.CoreSubsys[ForEach_itr].Flags_a,
         &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
         ConfigurationParameters,
         &iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
         SensorsDataCalibration,
         &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
           MotionController_InstanceData.rtb),
         &(iterative_motion_controller_DW.CoreSubsys[ForEach_itr].
           MotionController_InstanceData.rtdw));
    }

    // End of SystemInitialize for SubSystem: '<Root>/Iterative Motion Controller' 
    // Enable for Iterator SubSystem: '<Root>/Iterative Motion Controller'
    for (ForEach_itr = 0; ForEach_itr < 1; ForEach_itr++) {
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
  for (ForEach_itr = 0; ForEach_itr < 1; ForEach_itr++) {
    // Terminate for RateTransition generated from: '<S1>/Process Sensors'
    rtw_mutex_destroy();

    // Terminate for RateTransition generated from: '<S1>/Process Sensors'
    rtw_mutex_destroy();

    // Terminate for RateTransition: '<S1>/Rate Transition1'
    rtw_mutex_destroy();

    // Terminate for RateTransition generated from: '<S1>/FOC'
    rtw_mutex_destroy();

    // Terminate for RateTransition generated from: '<S1>/FOC'
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
