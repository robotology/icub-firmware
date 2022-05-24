//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_RX.cpp
//
// Code generated for Simulink model 'SupervisorFSM_RX'.
//
// Model version                  : 3.196
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Tue May 24 10:00:34 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "SupervisorFSM_RX.h"
#include "SupervisorFSM_RX_private.h"
#include "uMultiWord2Double.h"
#include "uMultiWordShl.h"

// Named constants for Chart: '<S2>/ControlMode_SM_motor0'
const uint8_T SupervisorFSM_RX_IN_Current = 1U;
const uint8_T SupervisorFSM_RX_IN_HWFault = 2U;
const uint8_T SupervisorFSM_RX_IN_Idle = 3U;
const uint8_T SupervisorFSM_RX_IN_Position = 5U;
const uint8_T SupervisorFSM_RX_IN_Velocity = 6U;
const uint8_T SupervisorFSM_RX_IN_Voltage = 7U;
const uint8_T SupervisorFSM__IN_NotConfigured = 4U;

// Named constants for Chart: '<Root>/SupervisorFSM_RX'
const uint8_T Superviso_IN_LimitNonConfigured = 1U;
const uint8_T SupervisorFSM_RX_IN_Configured = 1U;
const uint8_T SupervisorFSM_RX_IN_Fault = 2U;
const uint8_T SupervisorFSM_RX_IN_Init = 1U;
const uint8_T SupervisorFSM_RX_IN_NoFault = 2U;
const uint8_T SupervisorFSM_RX_IN_state1 = 1U;
const uint8_T SupervisorFSM__IN_ButtonPressed = 1U;
const uint8_T SupervisorFS_IN_NotConfigured_o = 3U;
const uint8_T SupervisorF_IN_OverCurrentFault = 3U;
MdlrefDW_SupervisorFSM_RX_T SupervisorFSM_RX_MdlrefDW;

// Block signals (default storage)
B_SupervisorFSM_RX_c_T SupervisorFSM_RX_B;

// Block states (default storage)
DW_SupervisorFSM_RX_f_T SupervisorFSM_RX_DW;

// Forward declaration for local functions
static boolean_T Supervisor_CheckSetPointCurrent(void);
static boolean_T Supervisor_CheckSetPointVoltage(void);
static boolean_T Superviso_CheckSetPointVelocity(void);
static boolean_T Superviso_CheckSetPointPosition(void);

// Forward declaration for local functions
static boolean_T SupervisorFSM_RX_IsNewCtrl_Idle(void);
static boolean_T SupervisorFS_IsNewCtrl_Position(void);
static boolean_T SupervisorFSM_IsNewCtrl_Current(void);
static boolean_T SupervisorFSM_IsNewCtrl_Voltage(void);
static boolean_T SupervisorFS_IsNewCtrl_Velocity(void);
static void SupervisorFSM_RX_Idle(void);
static boolean_T SupervisorFSM_RX_IsBoardReady(void);
static boolean_T SupervisorFS_isConfigurationSet(void);
static void SupervisorFSM_RX_Voltage(void);

// Forward declaration for local functions
static ControlModes SupervisorFSM_RX_convert(MCControlModes mccontrolmode);
static real32_T SupervisorFSM_RX_ConvertPid(real32_T in, uint8_T shift);

// Forward declaration for local functions
static boolean_T SupervisorFSM_isBoardConfigured(void);

// Declare global variables for system: model 'SupervisorFSM_RX'
const SensorsData *SupervisorFSM_R_rtu_SensorsData;// '<Root>/SensorsData'
const ControlOutputs *SupervisorFS_rtu_ControlOutputs;// '<Root>/ControlOutputs' 
const BUS_MESSAGES_RX_MULTIPLE *SupervisorFSM_RX_rtu_MessagesRx;// '<Root>/MessagesRx' 
const BUS_EVENTS_RX_MULTIPLE *SupervisorFSM_RX_rtu_EventsRx;// '<Root>/EventsRx' 
const BUS_CAN_RX_ERRORS_MULTIPLE *SupervisorFSM_RX_rtu_ErrorsRx;// '<Root>/ErrorsRx' 

// Function for Chart: '<S4>/Chart1'
static boolean_T Supervisor_CheckSetPointCurrent(void)
{
  return (SupervisorFSM_RX_DW.newSetpoint.type == ControlModes_Current) &&
    (SupervisorFSM_RX_B.controlModeDefined == ControlModes_Current);
}

// Function for Chart: '<S4>/Chart1'
static boolean_T Supervisor_CheckSetPointVoltage(void)
{
  return (SupervisorFSM_RX_DW.newSetpoint.type == ControlModes_Voltage) &&
    (SupervisorFSM_RX_B.controlModeDefined == ControlModes_Voltage);
}

// Function for Chart: '<S4>/Chart1'
static boolean_T Superviso_CheckSetPointVelocity(void)
{
  return (SupervisorFSM_RX_DW.newSetpoint.type == ControlModes_Velocity) &&
    (SupervisorFSM_RX_B.controlModeDefined == ControlModes_Velocity);
}

// Function for Chart: '<S4>/Chart1'
static boolean_T Superviso_CheckSetPointPosition(void)
{
  return (SupervisorFSM_RX_DW.newSetpoint.type == ControlModes_Position) &&
    (SupervisorFSM_RX_B.controlModeDefined == ControlModes_Position);
}

// System initialize for function-call system: '<Root>/SetpointHandler'
void Supervisor_SetpointHandler_Init(void)
{
  // SystemInitialize for Chart: '<S4>/Chart1'
  SupervisorFSM_RX_DW.newSetpoint.type = ControlModes_NotConfigured;
  SupervisorFSM_RX_DW.newSetpoint.value = 0.0F;
  SupervisorFSM_RX_B.targets.jointpositions.position = 0.0F;
  SupervisorFSM_RX_B.targets.jointvelocities.velocity = 0.0F;
  SupervisorFSM_RX_B.targets.motorcurrent.current = 0.0F;
  SupervisorFSM_RX_B.targets.motorvoltage.voltage = 0.0F;
}

// Output and update for function-call system: '<Root>/SetpointHandler'
void SupervisorFSM_R_SetpointHandler(void)
{
  // Chart: '<S4>/Chart1'
  if (SupervisorFSM_RX_DW.is_active_c4_SupervisorFSM_RX == 0U) {
    SupervisorFSM_RX_DW.is_active_c4_SupervisorFSM_RX = 1U;
    if (SupervisorFSM_RX_B.receivedNewSetpoint) {
      SupervisorFSM_RX_DW.newSetpoint = SupervisorFSM_RX_B.newSetpoint;
    } else {
      SupervisorFSM_RX_DW.newSetpoint = SupervisorFSM_RX_B.newSetpoint_p;
    }

    SupervisorFSM_RX_B.targets.motorvoltage.voltage = 0.0F;
    SupervisorFSM_RX_B.targets.motorcurrent.current = 0.0F;
    SupervisorFSM_RX_B.targets.jointvelocities.velocity = 0.0F;
    SupervisorFSM_RX_B.targets.jointpositions.position = 0.0F;
    if (Supervisor_CheckSetPointCurrent()) {
      SupervisorFSM_RX_B.targets.motorcurrent.current =
        SupervisorFSM_RX_DW.newSetpoint.value;
      SupervisorFSM_RX_B.hasSetpointChanged = true;
    } else if (Supervisor_CheckSetPointVoltage()) {
      SupervisorFSM_RX_B.targets.motorvoltage.voltage =
        SupervisorFSM_RX_DW.newSetpoint.value;
      SupervisorFSM_RX_B.hasSetpointChanged = true;
    } else if (Superviso_CheckSetPointVelocity()) {
      SupervisorFSM_RX_B.targets.jointvelocities.velocity =
        SupervisorFSM_RX_DW.newSetpoint.value;
      SupervisorFSM_RX_B.hasSetpointChanged = true;
    } else if (Superviso_CheckSetPointPosition()) {
      SupervisorFSM_RX_B.targets.jointpositions.position =
        SupervisorFSM_RX_DW.newSetpoint.value;
      SupervisorFSM_RX_B.hasSetpointChanged = true;
    }
  } else {
    if (SupervisorFSM_RX_B.receivedNewSetpoint) {
      SupervisorFSM_RX_DW.newSetpoint = SupervisorFSM_RX_B.newSetpoint;
    } else {
      SupervisorFSM_RX_DW.newSetpoint = SupervisorFSM_RX_B.newSetpoint_p;
    }

    SupervisorFSM_RX_B.targets.motorvoltage.voltage = 0.0F;
    SupervisorFSM_RX_B.targets.motorcurrent.current = 0.0F;
    SupervisorFSM_RX_B.targets.jointvelocities.velocity = 0.0F;
    SupervisorFSM_RX_B.targets.jointpositions.position = 0.0F;
    if (Supervisor_CheckSetPointCurrent()) {
      SupervisorFSM_RX_B.targets.motorcurrent.current =
        SupervisorFSM_RX_DW.newSetpoint.value;
      SupervisorFSM_RX_B.hasSetpointChanged = true;
    } else if (Supervisor_CheckSetPointVoltage()) {
      SupervisorFSM_RX_B.targets.motorvoltage.voltage =
        SupervisorFSM_RX_DW.newSetpoint.value;
      SupervisorFSM_RX_B.hasSetpointChanged = true;
    } else if (Superviso_CheckSetPointVelocity()) {
      SupervisorFSM_RX_B.targets.jointvelocities.velocity =
        SupervisorFSM_RX_DW.newSetpoint.value;
      SupervisorFSM_RX_B.hasSetpointChanged = true;
    } else if (Superviso_CheckSetPointPosition()) {
      SupervisorFSM_RX_B.targets.jointpositions.position =
        SupervisorFSM_RX_DW.newSetpoint.value;
      SupervisorFSM_RX_B.hasSetpointChanged = true;
    }
  }

  // End of Chart: '<S4>/Chart1'
}

// Function for Chart: '<S2>/ControlMode_SM_motor0'
static boolean_T SupervisorFSM_RX_IsNewCtrl_Idle(void)
{
  return SupervisorFSM_RX_B.requiredControlMode == ControlModes_Idle;
}

// Function for Chart: '<S2>/ControlMode_SM_motor0'
static boolean_T SupervisorFS_IsNewCtrl_Position(void)
{
  return SupervisorFSM_RX_B.requiredControlMode == ControlModes_Position;
}

// Function for Chart: '<S2>/ControlMode_SM_motor0'
static boolean_T SupervisorFSM_IsNewCtrl_Current(void)
{
  return SupervisorFSM_RX_B.requiredControlMode == ControlModes_Current;
}

// Function for Chart: '<S2>/ControlMode_SM_motor0'
static boolean_T SupervisorFSM_IsNewCtrl_Voltage(void)
{
  return SupervisorFSM_RX_B.requiredControlMode == ControlModes_Voltage;
}

// Function for Chart: '<S2>/ControlMode_SM_motor0'
static boolean_T SupervisorFS_IsNewCtrl_Velocity(void)
{
  return SupervisorFSM_RX_B.requiredControlMode == ControlModes_Velocity;
}

// Function for Chart: '<S2>/ControlMode_SM_motor0'
static void SupervisorFSM_RX_Idle(void)
{
  boolean_T guard1 = false;
  boolean_T guard2 = false;
  SupervisorFSM_RX_B.controlModeDefined = ControlModes_Idle;
  guard1 = false;
  guard2 = false;
  if (SupervisorFSM_RX_B.isInOverCurrent ||
      SupervisorFSM_RX_B.isFaultButtonPressed) {
    if (SupervisorFSM_RX_B.isInOverCurrent) {
      SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX = SupervisorFSM_RX_IN_HWFault;
      SupervisorFSM_RX_B.controlModeDefined = ControlModes_HwFaultCM;
      rtw_disableMotor();

      // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
      // this updates the targets value
      SupervisorFSM_R_SetpointHandler();

      // End of Outputs for SubSystem: '<Root>/SetpointHandler'
    } else if (SupervisorFSM_RX_B.isFaultButtonPressed) {
      guard1 = true;
    } else {
      guard2 = true;
    }
  } else {
    guard2 = true;
  }

  if (guard2) {
    if (!SupervisorFSM_RX_IsNewCtrl_Idle()) {
      rtw_enableMotor();
      if (SupervisorFS_IsNewCtrl_Position()) {
        SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
          SupervisorFSM_RX_IN_Position;
        SupervisorFSM_RX_B.controlModeDefined = ControlModes_Position;
        SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Position;

        // Chart: '<S2>/ControlMode_SM_motor0'
        SupervisorFSM_RX_B.newSetpoint_p.value =
          SupervisorFSM_R_rtu_SensorsData->jointpositions.position;

        // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
        SupervisorFSM_R_SetpointHandler();

        // End of Outputs for SubSystem: '<Root>/SetpointHandler'
      } else if (SupervisorFSM_IsNewCtrl_Current()) {
        SupervisorFSM_RX_B.newSetpoint_p.value = 0.0F;
        SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
          SupervisorFSM_RX_IN_Current;
        SupervisorFSM_RX_B.controlModeDefined = ControlModes_Current;
        SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Current;

        // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
        SupervisorFSM_R_SetpointHandler();

        // End of Outputs for SubSystem: '<Root>/SetpointHandler'
      } else if (SupervisorFSM_IsNewCtrl_Voltage()) {
        SupervisorFSM_RX_B.newSetpoint_p.value = 0.0F;
        SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
          SupervisorFSM_RX_IN_Voltage;
        SupervisorFSM_RX_B.controlModeDefined = ControlModes_Voltage;
        SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Voltage;

        // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
        SupervisorFSM_R_SetpointHandler();

        // End of Outputs for SubSystem: '<Root>/SetpointHandler'
      } else if (SupervisorFS_IsNewCtrl_Velocity()) {
        SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
          SupervisorFSM_RX_IN_Velocity;
        SupervisorFSM_RX_B.controlModeDefined = ControlModes_Velocity;
        SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Velocity;
        SupervisorFSM_RX_B.newSetpoint_p.value = 0.0F;

        // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
        SupervisorFSM_R_SetpointHandler();

        // End of Outputs for SubSystem: '<Root>/SetpointHandler'
      } else {
        guard1 = true;
      }
    } else {
      guard1 = true;
    }
  }

  if (guard1) {
    SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX = SupervisorFSM_RX_IN_Idle;
    SupervisorFSM_RX_B.controlModeDefined = ControlModes_Idle;
    rtw_disableMotor();

    // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
    // this updates the targets value
    SupervisorFSM_R_SetpointHandler();

    // End of Outputs for SubSystem: '<Root>/SetpointHandler'
  }
}

// Function for Chart: '<S2>/ControlMode_SM_motor0'
static boolean_T SupervisorFSM_RX_IsBoardReady(void)
{
  return SupervisorFSM_RX_B.BoardSt == BoardState_Configured;
}

// Function for Chart: '<S2>/ControlMode_SM_motor0'
static boolean_T SupervisorFS_isConfigurationSet(void)
{
  return SupervisorFSM_RX_B.areLimitsSet;
}

// Function for Chart: '<S2>/ControlMode_SM_motor0'
static void SupervisorFSM_RX_Voltage(void)
{
  boolean_T guard1 = false;
  boolean_T guard2 = false;
  boolean_T guard3 = false;
  SupervisorFSM_RX_B.controlModeDefined = ControlModes_Voltage;
  guard1 = false;
  guard2 = false;
  guard3 = false;
  if (SupervisorFSM_RX_B.isInOverCurrent ||
      SupervisorFSM_RX_B.isFaultButtonPressed) {
    if (SupervisorFSM_RX_B.isInOverCurrent) {
      SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX = SupervisorFSM_RX_IN_HWFault;
      SupervisorFSM_RX_B.controlModeDefined = ControlModes_HwFaultCM;
      rtw_disableMotor();

      // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
      // this updates the targets value
      SupervisorFSM_R_SetpointHandler();

      // End of Outputs for SubSystem: '<Root>/SetpointHandler'
    } else if (SupervisorFSM_RX_B.isFaultButtonPressed) {
      guard1 = true;
    } else {
      guard3 = true;
    }
  } else {
    guard3 = true;
  }

  if (guard3) {
    if (SupervisorFS_IsNewCtrl_Velocity()) {
      if ((!SupervisorFSM_IsNewCtrl_Voltage()) &&
          SupervisorFS_IsNewCtrl_Velocity()) {
        SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
          SupervisorFSM_RX_IN_Velocity;
        SupervisorFSM_RX_B.controlModeDefined = ControlModes_Velocity;
        SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Velocity;
        SupervisorFSM_RX_B.newSetpoint_p.value = 0.0F;

        // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
        SupervisorFSM_R_SetpointHandler();

        // End of Outputs for SubSystem: '<Root>/SetpointHandler'
      } else {
        guard2 = true;
      }
    } else if (!SupervisorFSM_IsNewCtrl_Voltage()) {
      if (SupervisorFSM_IsNewCtrl_Current()) {
        // Chart: '<S2>/ControlMode_SM_motor0'
        SupervisorFSM_RX_B.newSetpoint_p.value =
          SupervisorFS_rtu_ControlOutputs->Iq_fbk.current;
        SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
          SupervisorFSM_RX_IN_Current;
        SupervisorFSM_RX_B.controlModeDefined = ControlModes_Current;
        SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Current;

        // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
        SupervisorFSM_R_SetpointHandler();

        // End of Outputs for SubSystem: '<Root>/SetpointHandler'
      } else if (SupervisorFS_IsNewCtrl_Position()) {
        SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
          SupervisorFSM_RX_IN_Position;
        SupervisorFSM_RX_B.controlModeDefined = ControlModes_Position;
        SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Position;

        // Chart: '<S2>/ControlMode_SM_motor0'
        SupervisorFSM_RX_B.newSetpoint_p.value =
          SupervisorFSM_R_rtu_SensorsData->jointpositions.position;

        // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
        SupervisorFSM_R_SetpointHandler();

        // End of Outputs for SubSystem: '<Root>/SetpointHandler'
      } else if (SupervisorFSM_RX_IsNewCtrl_Idle()) {
        guard1 = true;
      } else {
        guard2 = true;
      }
    } else {
      guard2 = true;
    }
  }

  if (guard2) {
    SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX = SupervisorFSM_RX_IN_Voltage;
    SupervisorFSM_RX_B.controlModeDefined = ControlModes_Voltage;
    SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Voltage;

    // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
    SupervisorFSM_R_SetpointHandler();

    // End of Outputs for SubSystem: '<Root>/SetpointHandler'
  }

  if (guard1) {
    SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX = SupervisorFSM_RX_IN_Idle;
    SupervisorFSM_RX_B.controlModeDefined = ControlModes_Idle;
    rtw_disableMotor();

    // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
    // this updates the targets value
    SupervisorFSM_R_SetpointHandler();

    // End of Outputs for SubSystem: '<Root>/SetpointHandler'
  }
}

// System initialize for function-call system: '<Root>/Control Mode Handler Motor 0'
void S_ControlModeHandlerMotor0_Init(void)
{
  // SystemInitialize for Chart: '<S2>/ControlMode_SM_motor0'
  SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_NotConfigured;
  SupervisorFSM_RX_B.newSetpoint_p.value = 0.0F;
}

// Output and update for function-call system: '<Root>/Control Mode Handler Motor 0'
void Superv_ControlModeHandlerMotor0(void)
{
  // Chart: '<S2>/ControlMode_SM_motor0'
  if (SupervisorFSM_RX_DW.is_active_c12_SupervisorFSM_RX == 0U) {
    SupervisorFSM_RX_DW.is_active_c12_SupervisorFSM_RX = 1U;
    if (SupervisorFSM_RX_IsBoardReady() && SupervisorFS_isConfigurationSet()) {
      SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX = SupervisorFSM_RX_IN_Idle;
      SupervisorFSM_RX_B.controlModeDefined = ControlModes_Idle;
      rtw_disableMotor();

      // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
      // this updates the targets value
      SupervisorFSM_R_SetpointHandler();

      // End of Outputs for SubSystem: '<Root>/SetpointHandler'
    } else {
      SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
        SupervisorFSM__IN_NotConfigured;
      SupervisorFSM_RX_B.controlModeDefined = ControlModes_NotConfigured;
      rtw_disableMotor();
    }
  } else {
    boolean_T guard1 = false;
    boolean_T guard2 = false;
    boolean_T guard3 = false;
    boolean_T guard4 = false;
    boolean_T guard5 = false;
    boolean_T guard6 = false;
    boolean_T guard7 = false;
    boolean_T guard8 = false;
    boolean_T guard9 = false;
    guard1 = false;
    guard2 = false;
    guard3 = false;
    guard4 = false;
    guard5 = false;
    guard6 = false;
    guard7 = false;
    guard8 = false;
    guard9 = false;
    switch (SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX) {
     case SupervisorFSM_RX_IN_Current:
      SupervisorFSM_RX_B.controlModeDefined = ControlModes_Current;
      if (SupervisorFSM_RX_B.isInOverCurrent ||
          SupervisorFSM_RX_B.isFaultButtonPressed) {
        if (SupervisorFSM_RX_B.isInOverCurrent) {
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_HWFault;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_HwFaultCM;
          rtw_disableMotor();

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          // this updates the targets value
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else if (SupervisorFSM_RX_B.isFaultButtonPressed) {
          guard1 = true;
        } else {
          guard6 = true;
        }
      } else {
        guard6 = true;
      }
      break;

     case SupervisorFSM_RX_IN_HWFault:
      SupervisorFSM_RX_B.controlModeDefined = ControlModes_HwFaultCM;
      if (SupervisorFSM_RX_IsNewCtrl_Idle() && (!SupervisorFS_isConfigurationSet
           ())) {
        SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
          SupervisorFSM__IN_NotConfigured;
        SupervisorFSM_RX_B.controlModeDefined = ControlModes_NotConfigured;
        rtw_disableMotor();
      } else if (SupervisorFSM_RX_IsNewCtrl_Idle()) {
        SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX = SupervisorFSM_RX_IN_Idle;
        SupervisorFSM_RX_B.controlModeDefined = ControlModes_Idle;
        rtw_disableMotor();

        // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
        // this updates the targets value
        SupervisorFSM_R_SetpointHandler();

        // End of Outputs for SubSystem: '<Root>/SetpointHandler'
      }
      break;

     case SupervisorFSM_RX_IN_Idle:
      SupervisorFSM_RX_Idle();
      break;

     case SupervisorFSM__IN_NotConfigured:
      SupervisorFSM_RX_B.controlModeDefined = ControlModes_NotConfigured;
      if (SupervisorFSM_RX_IsBoardReady() && SupervisorFS_isConfigurationSet())
      {
        SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX = SupervisorFSM_RX_IN_Idle;
        SupervisorFSM_RX_B.controlModeDefined = ControlModes_Idle;
        rtw_disableMotor();

        // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
        // this updates the targets value
        SupervisorFSM_R_SetpointHandler();

        // End of Outputs for SubSystem: '<Root>/SetpointHandler'
      } else if (SupervisorFSM_RX_B.isInOverCurrent) {
        SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
          SupervisorFSM_RX_IN_HWFault;
        SupervisorFSM_RX_B.controlModeDefined = ControlModes_HwFaultCM;
        rtw_disableMotor();

        // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
        // this updates the targets value
        SupervisorFSM_R_SetpointHandler();

        // End of Outputs for SubSystem: '<Root>/SetpointHandler'
      }
      break;

     case SupervisorFSM_RX_IN_Position:
      SupervisorFSM_RX_B.controlModeDefined = ControlModes_Position;
      if (SupervisorFSM_RX_B.isInOverCurrent ||
          SupervisorFSM_RX_B.isFaultButtonPressed) {
        if (SupervisorFSM_RX_B.isInOverCurrent) {
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_HWFault;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_HwFaultCM;
          rtw_disableMotor();

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          // this updates the targets value
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else if (SupervisorFSM_RX_B.isFaultButtonPressed) {
          guard3 = true;
        } else {
          guard9 = true;
        }
      } else {
        guard9 = true;
      }
      break;

     case SupervisorFSM_RX_IN_Velocity:
      SupervisorFSM_RX_B.controlModeDefined = ControlModes_Velocity;
      if (SupervisorFSM_RX_B.isInOverCurrent ||
          SupervisorFSM_RX_B.isFaultButtonPressed) {
        if (SupervisorFSM_RX_B.isInOverCurrent) {
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_HWFault;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_HwFaultCM;
          rtw_disableMotor();

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          // this updates the targets value
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else if (SupervisorFSM_RX_B.isFaultButtonPressed) {
          guard4 = true;
        } else {
          guard8 = true;
        }
      } else {
        guard8 = true;
      }
      break;

     default:
      // case IN_Voltage:
      SupervisorFSM_RX_Voltage();
      break;
    }

    if (guard9) {
      if ((!SupervisorFSM_RX_IsNewCtrl_Idle()) &&
          (!SupervisorFS_IsNewCtrl_Position())) {
        if (SupervisorFSM_IsNewCtrl_Current()) {
          SupervisorFSM_RX_B.newSetpoint_p.value =
            SupervisorFS_rtu_ControlOutputs->Iq_fbk.current;
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_Current;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Current;
          SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Current;

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else if (SupervisorFSM_IsNewCtrl_Voltage()) {
          SupervisorFSM_RX_B.newSetpoint_p.value =
            SupervisorFSM_R_rtu_SensorsData->motorsensors.voltage;
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_Voltage;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Voltage;
          SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Voltage;

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else if (SupervisorFS_IsNewCtrl_Velocity()) {
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_Velocity;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Velocity;
          SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Velocity;
          SupervisorFSM_RX_B.newSetpoint_p.value = 0.0F;

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else {
          guard7 = true;
        }
      } else {
        guard7 = true;
      }
    }

    if (guard8) {
      if (!SupervisorFS_IsNewCtrl_Velocity()) {
        if (SupervisorFSM_IsNewCtrl_Voltage()) {
          SupervisorFSM_RX_B.newSetpoint_p.value =
            SupervisorFSM_R_rtu_SensorsData->motorsensors.voltage;
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_Voltage;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Voltage;
          SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Voltage;

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else if (SupervisorFSM_IsNewCtrl_Current()) {
          SupervisorFSM_RX_B.newSetpoint_p.value =
            SupervisorFS_rtu_ControlOutputs->Iq_fbk.current;
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_Current;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Current;
          SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Current;

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else if (SupervisorFS_IsNewCtrl_Position()) {
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_Position;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Position;
          SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Position;
          SupervisorFSM_RX_B.newSetpoint_p.value =
            SupervisorFSM_R_rtu_SensorsData->jointpositions.position;

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else if (SupervisorFSM_RX_IsNewCtrl_Idle()) {
          guard4 = true;
        } else {
          guard5 = true;
        }
      } else {
        guard5 = true;
      }
    }

    if (guard7) {
      if (SupervisorFSM_RX_IsNewCtrl_Idle()) {
        guard3 = true;
      }
    }

    if (guard6) {
      if ((!SupervisorFSM_RX_IsNewCtrl_Idle()) &&
          (!SupervisorFS_IsNewCtrl_Position())) {
        if (!SupervisorFSM_IsNewCtrl_Current()) {
          if (SupervisorFSM_IsNewCtrl_Voltage()) {
            SupervisorFSM_RX_B.newSetpoint_p.value =
              SupervisorFSM_R_rtu_SensorsData->motorsensors.voltage;
            SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
              SupervisorFSM_RX_IN_Voltage;
            SupervisorFSM_RX_B.controlModeDefined = ControlModes_Voltage;
            SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Voltage;

            // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
            SupervisorFSM_R_SetpointHandler();

            // End of Outputs for SubSystem: '<Root>/SetpointHandler'
          } else if (SupervisorFS_IsNewCtrl_Velocity()) {
            SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
              SupervisorFSM_RX_IN_Velocity;
            SupervisorFSM_RX_B.controlModeDefined = ControlModes_Velocity;
            SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Velocity;
            SupervisorFSM_RX_B.newSetpoint_p.value = 0.0F;

            // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
            SupervisorFSM_R_SetpointHandler();

            // End of Outputs for SubSystem: '<Root>/SetpointHandler'
          } else {
            guard2 = true;
          }
        } else {
          guard2 = true;
        }
      } else if (!SupervisorFSM_IsNewCtrl_Current()) {
        if (SupervisorFS_IsNewCtrl_Position()) {
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_Position;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Position;
          SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Position;
          SupervisorFSM_RX_B.newSetpoint_p.value =
            SupervisorFSM_R_rtu_SensorsData->jointpositions.position;

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else if (SupervisorFSM_RX_IsNewCtrl_Idle()) {
          guard1 = true;
        } else {
          guard2 = true;
        }
      } else {
        guard2 = true;
      }
    }

    if (guard5) {
      SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX = SupervisorFSM_RX_IN_Velocity;
      SupervisorFSM_RX_B.controlModeDefined = ControlModes_Velocity;
      SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Velocity;
      SupervisorFSM_RX_B.newSetpoint_p.value = 0.0F;

      // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
      SupervisorFSM_R_SetpointHandler();

      // End of Outputs for SubSystem: '<Root>/SetpointHandler'
    }

    if (guard4) {
      SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX = SupervisorFSM_RX_IN_Idle;
      SupervisorFSM_RX_B.controlModeDefined = ControlModes_Idle;
      rtw_disableMotor();

      // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
      // this updates the targets value
      SupervisorFSM_R_SetpointHandler();

      // End of Outputs for SubSystem: '<Root>/SetpointHandler'
    }

    if (guard3) {
      SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX = SupervisorFSM_RX_IN_Idle;
      SupervisorFSM_RX_B.controlModeDefined = ControlModes_Idle;
      rtw_disableMotor();

      // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
      // this updates the targets value
      SupervisorFSM_R_SetpointHandler();

      // End of Outputs for SubSystem: '<Root>/SetpointHandler'
    }

    if (guard2) {
      SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX = SupervisorFSM_RX_IN_Current;
      SupervisorFSM_RX_B.controlModeDefined = ControlModes_Current;
      SupervisorFSM_RX_B.newSetpoint_p.type = ControlModes_Current;

      // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
      SupervisorFSM_R_SetpointHandler();

      // End of Outputs for SubSystem: '<Root>/SetpointHandler'
    }

    if (guard1) {
      SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX = SupervisorFSM_RX_IN_Idle;
      SupervisorFSM_RX_B.controlModeDefined = ControlModes_Idle;
      rtw_disableMotor();

      // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
      // this updates the targets value
      SupervisorFSM_R_SetpointHandler();

      // End of Outputs for SubSystem: '<Root>/SetpointHandler'
    }
  }

  // End of Chart: '<S2>/ControlMode_SM_motor0'
}

// System initialize for function-call system: '<Root>/Limits Handler'
void SupervisorFS_LimitsHandler_Init(void)
{
  // SystemInitialize for Chart: '<S3>/Chart'
  SupervisorFSM_RX_B.thresholds.jntPosMin = 0.0F;
  SupervisorFSM_RX_B.thresholds.jntPosMax = 0.0F;
  SupervisorFSM_RX_B.thresholds.hardwareJntPosMin = 0.0F;
  SupervisorFSM_RX_B.thresholds.hardwareJntPosMax = 0.0F;
  SupervisorFSM_RX_B.thresholds.rotorPosMin = 0.0F;
  SupervisorFSM_RX_B.thresholds.rotorPosMax = 0.0F;
  SupervisorFSM_RX_B.thresholds.jntVelMax = 0.0F;
  SupervisorFSM_RX_B.thresholds.velocityTimeout = 0U;
  SupervisorFSM_RX_B.thresholds.motorNominalCurrents = 0.0F;
  SupervisorFSM_RX_B.thresholds.motorPeakCurrents = 0.0F;
  SupervisorFSM_RX_B.thresholds.motorOverloadCurrents = 0.0F;
  SupervisorFSM_RX_B.thresholds.motorPwmLimit = 0U;

  // SystemInitialize for Function Call SubSystem: '<Root>/Control Mode Handler Motor 0' 
  S_ControlModeHandlerMotor0_Init();

  // End of SystemInitialize for SubSystem: '<Root>/Control Mode Handler Motor 0' 
}

// Output and update for function-call system: '<Root>/Limits Handler'
void SupervisorFSM_RX_LimitsHandler(void)
{
  // Chart: '<S3>/Chart'
  if (SupervisorFSM_RX_DW.is_active_c14_SupervisorFSM_RX == 0U) {
    SupervisorFSM_RX_DW.is_active_c14_SupervisorFSM_RX = 1U;
    SupervisorFSM_RX_B.thresholds = InitConfParams.thresholds;
    if (SupervisorFSM_RX_B.newLimit.type == ControlModes_Current) {
      SupervisorFSM_RX_B.thresholds.motorNominalCurrents = std::abs
        (SupervisorFSM_RX_B.newLimit.nominal);
      SupervisorFSM_RX_B.thresholds.motorPeakCurrents = std::abs
        (SupervisorFSM_RX_B.newLimit.peak);
      SupervisorFSM_RX_B.thresholds.motorOverloadCurrents = std::abs
        (SupervisorFSM_RX_B.newLimit.overload);
      SupervisorFSM_RX_B.hasThresholdsChanged = true;
      if (!SupervisorFSM_RX_B.areLimitsSet) {
        SupervisorFSM_RX_B.areLimitsSet = true;

        // Outputs for Function Call SubSystem: '<Root>/Control Mode Handler Motor 0' 
        Superv_ControlModeHandlerMotor0();

        // End of Outputs for SubSystem: '<Root>/Control Mode Handler Motor 0'
      }
    }
  } else if (SupervisorFSM_RX_B.newLimit.type == ControlModes_Current) {
    SupervisorFSM_RX_B.thresholds.motorNominalCurrents = std::abs
      (SupervisorFSM_RX_B.newLimit.nominal);
    SupervisorFSM_RX_B.thresholds.motorPeakCurrents = std::abs
      (SupervisorFSM_RX_B.newLimit.peak);
    SupervisorFSM_RX_B.thresholds.motorOverloadCurrents = std::abs
      (SupervisorFSM_RX_B.newLimit.overload);
    SupervisorFSM_RX_B.hasThresholdsChanged = true;
    if (!SupervisorFSM_RX_B.areLimitsSet) {
      SupervisorFSM_RX_B.areLimitsSet = true;

      // Outputs for Function Call SubSystem: '<Root>/Control Mode Handler Motor 0' 
      Superv_ControlModeHandlerMotor0();

      // End of Outputs for SubSystem: '<Root>/Control Mode Handler Motor 0'
    }
  }

  // End of Chart: '<S3>/Chart'
}

// Function for Chart: '<S1>/CAN Event Dispatcher'
static ControlModes SupervisorFSM_RX_convert(MCControlModes mccontrolmode)
{
  ControlModes controlmode;
  switch (mccontrolmode) {
   case MCControlModes_Idle:
    controlmode = ControlModes_Idle;
    break;

   case MCControlModes_Current:
    controlmode = ControlModes_Current;
    break;

   case MCControlModes_SpeedVoltage:
    controlmode = ControlModes_Velocity;
    break;

   case MCControlModes_OpenLoop:
    controlmode = ControlModes_Voltage;
    break;

   default:
    controlmode = ControlModes_Idle;
    break;
  }

  return controlmode;
}

// Function for Chart: '<S1>/CAN Event Dispatcher'
static real32_T SupervisorFSM_RX_ConvertPid(real32_T in, uint8_T shift)
{
  uint64m_T tmp;
  uint32_T qY;
  static const uint64m_T tmp_0 = { { 1U, 0U }// chunks
  };

  qY = 15U - shift;
  if (15U - shift > 15U) {
    qY = 0U;
  }

  uMultiWordShl(&tmp_0.chunks[0U], 2, qY, &tmp.chunks[0U], 2);
  return in * static_cast<real32_T>(uMultiWord2Double(&tmp.chunks[0U], 2, 0)) /
    32768.0F;
}

// System initialize for function-call system: '<Root>/CAN Message Handler'
void Supervis_CANMessageHandler_Init(void)
{
  SupervisorFSM_RX_B.newSetpoint.type = ControlModes_NotConfigured;
  SupervisorFSM_RX_B.newSetpoint.value = 0.0F;
  SupervisorFSM_RX_B.newLimit.overload = 0.0F;
  SupervisorFSM_RX_B.newLimit.peak = 0.0F;
  SupervisorFSM_RX_B.newLimit.nominal = 0.0F;
  SupervisorFSM_RX_B.newLimit.type = ControlModes_NotConfigured;
  SupervisorFSM_RX_B.Motorconfig.Imin = 0.0F;
  SupervisorFSM_RX_B.Motorconfig.Imax = 0.0F;
  SupervisorFSM_RX_B.Motorconfig.has_hall_sens = false;
  SupervisorFSM_RX_B.Motorconfig.pole_pairs = 0U;
  SupervisorFSM_RX_B.Motorconfig.reduction = 0.0F;
  SupervisorFSM_RX_B.Motorconfig.Kp = 0.0F;
  SupervisorFSM_RX_B.Motorconfig.Ki = 0.0F;
  SupervisorFSM_RX_B.Motorconfig.Kd = 0.0F;
  SupervisorFSM_RX_B.Motorconfig.Ks = 0U;
  SupervisorFSM_RX_B.Motorconfig.Kbemf = 0.0F;
  SupervisorFSM_RX_B.Motorconfig.Rphase = 0.0F;
  SupervisorFSM_RX_B.Motorconfig.Vmax = 0.0F;
  SupervisorFSM_RX_B.Motorconfig.Vcc = 0.0F;

  // SystemInitialize for Function Call SubSystem: '<Root>/SetpointHandler'
  Supervisor_SetpointHandler_Init();

  // End of SystemInitialize for SubSystem: '<Root>/SetpointHandler'

  // SystemInitialize for Chart: '<S1>/CAN Event Dispatcher' incorporates:
  //   SubSystem: '<Root>/Limits Handler'

  SupervisorFS_LimitsHandler_Init();
}

// Output and update for function-call system: '<Root>/CAN Message Handler'
void SupervisorFSM_CANMessageHandler(void)
{
  // Chart: '<S1>/CAN Event Dispatcher' incorporates:
  //   Selector: '<S1>/Selector'
  //   Selector: '<S1>/Selector1'
  //   Selector: '<S1>/Selector2'

  if (SupervisorFSM_RX_DW.is_active_c11_SupervisorFSM_RX == 0U) {
    SupervisorFSM_RX_DW.is_active_c11_SupervisorFSM_RX = 1U;
    SupervisorFSM_RX_B.Motorconfig = InitConfParams.motorconfig;
    if (!SupervisorFSM_RX_rtu_ErrorsRx->errors[SupervisorFSM_RX_B.messageIndex -
        1].event) {
      if (SupervisorFSM_RX_rtu_EventsRx->events[SupervisorFSM_RX_B.messageIndex
          - 1].desired_current) {
        SupervisorFSM_RX_B.newSetpoint.type = ControlModes_Current;
        SupervisorFSM_RX_B.newSetpoint.value =
          SupervisorFSM_RX_rtu_MessagesRx->
          messages[SupervisorFSM_RX_B.messageIndex - 1].desired_current.current;
        SupervisorFSM_RX_B.enableSendingMsgStatus = true;
        SupervisorFSM_RX_B.receivedNewSetpoint = true;

        // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
        SupervisorFSM_R_SetpointHandler();

        // End of Outputs for SubSystem: '<Root>/SetpointHandler'
      } else if (SupervisorFSM_RX_rtu_EventsRx->
                 events[SupervisorFSM_RX_B.messageIndex - 1].control_mode) {
        SupervisorFSM_RX_B.requiredControlMode = SupervisorFSM_RX_convert
          (SupervisorFSM_RX_rtu_MessagesRx->
           messages[SupervisorFSM_RX_B.messageIndex - 1].control_mode.mode);
        SupervisorFSM_RX_B.receivedNewSetpoint = false;

        // Outputs for Function Call SubSystem: '<Root>/Control Mode Handler Motor 0' 
        Superv_ControlModeHandlerMotor0();

        // End of Outputs for SubSystem: '<Root>/Control Mode Handler Motor 0'
      } else if (SupervisorFSM_RX_rtu_EventsRx->
                 events[SupervisorFSM_RX_B.messageIndex - 1].current_limit) {
        SupervisorFSM_RX_B.newLimit.type = ControlModes_Current;
        SupervisorFSM_RX_B.newLimit.peak =
          SupervisorFSM_RX_rtu_MessagesRx->
          messages[SupervisorFSM_RX_B.messageIndex - 1].current_limit.peak;
        SupervisorFSM_RX_B.newLimit.nominal =
          SupervisorFSM_RX_rtu_MessagesRx->
          messages[SupervisorFSM_RX_B.messageIndex - 1].current_limit.nominal;
        SupervisorFSM_RX_B.newLimit.overload =
          SupervisorFSM_RX_rtu_MessagesRx->
          messages[SupervisorFSM_RX_B.messageIndex - 1].current_limit.overload;

        // Outputs for Function Call SubSystem: '<Root>/Limits Handler'
        SupervisorFSM_RX_LimitsHandler();

        // End of Outputs for SubSystem: '<Root>/Limits Handler'
      } else if (SupervisorFSM_RX_rtu_EventsRx->
                 events[SupervisorFSM_RX_B.messageIndex - 1].current_pid) {
        SupervisorFSM_RX_B.Motorconfig.Kp = SupervisorFSM_RX_ConvertPid
          (SupervisorFSM_RX_rtu_MessagesRx->
           messages[SupervisorFSM_RX_B.messageIndex - 1].current_pid.Kp,
           SupervisorFSM_RX_rtu_MessagesRx->
           messages[SupervisorFSM_RX_B.messageIndex - 1].current_pid.Ks);
        SupervisorFSM_RX_B.Motorconfig.Ki = SupervisorFSM_RX_ConvertPid
          (SupervisorFSM_RX_rtu_MessagesRx->
           messages[SupervisorFSM_RX_B.messageIndex - 1].current_pid.Ki,
           SupervisorFSM_RX_rtu_MessagesRx->
           messages[SupervisorFSM_RX_B.messageIndex - 1].current_pid.Ks);
        SupervisorFSM_RX_B.Motorconfig.Kd = SupervisorFSM_RX_ConvertPid
          (SupervisorFSM_RX_rtu_MessagesRx->
           messages[SupervisorFSM_RX_B.messageIndex - 1].current_pid.Kd,
           SupervisorFSM_RX_rtu_MessagesRx->
           messages[SupervisorFSM_RX_B.messageIndex - 1].current_pid.Ks);
        SupervisorFSM_RX_B.Motorconfig.Ks =
          SupervisorFSM_RX_rtu_MessagesRx->
          messages[SupervisorFSM_RX_B.messageIndex - 1].current_pid.Ks;
        SupervisorFSM_RX_B.hasMotorConfigChanged = true;
      }
    }
  } else if (!SupervisorFSM_RX_rtu_ErrorsRx->
             errors[SupervisorFSM_RX_B.messageIndex - 1].event) {
    if (SupervisorFSM_RX_rtu_EventsRx->events[SupervisorFSM_RX_B.messageIndex -
        1].desired_current) {
      SupervisorFSM_RX_B.newSetpoint.type = ControlModes_Current;
      SupervisorFSM_RX_B.newSetpoint.value =
        SupervisorFSM_RX_rtu_MessagesRx->
        messages[SupervisorFSM_RX_B.messageIndex - 1].desired_current.current;
      SupervisorFSM_RX_B.enableSendingMsgStatus = true;
      SupervisorFSM_RX_B.receivedNewSetpoint = true;

      // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
      SupervisorFSM_R_SetpointHandler();

      // End of Outputs for SubSystem: '<Root>/SetpointHandler'
    } else if (SupervisorFSM_RX_rtu_EventsRx->
               events[SupervisorFSM_RX_B.messageIndex - 1].control_mode) {
      SupervisorFSM_RX_B.requiredControlMode = SupervisorFSM_RX_convert
        (SupervisorFSM_RX_rtu_MessagesRx->
         messages[SupervisorFSM_RX_B.messageIndex - 1].control_mode.mode);
      SupervisorFSM_RX_B.receivedNewSetpoint = false;

      // Outputs for Function Call SubSystem: '<Root>/Control Mode Handler Motor 0' 
      Superv_ControlModeHandlerMotor0();

      // End of Outputs for SubSystem: '<Root>/Control Mode Handler Motor 0'
    } else if (SupervisorFSM_RX_rtu_EventsRx->
               events[SupervisorFSM_RX_B.messageIndex - 1].current_limit) {
      SupervisorFSM_RX_B.newLimit.type = ControlModes_Current;
      SupervisorFSM_RX_B.newLimit.peak =
        SupervisorFSM_RX_rtu_MessagesRx->
        messages[SupervisorFSM_RX_B.messageIndex - 1].current_limit.peak;
      SupervisorFSM_RX_B.newLimit.nominal =
        SupervisorFSM_RX_rtu_MessagesRx->
        messages[SupervisorFSM_RX_B.messageIndex - 1].current_limit.nominal;
      SupervisorFSM_RX_B.newLimit.overload =
        SupervisorFSM_RX_rtu_MessagesRx->
        messages[SupervisorFSM_RX_B.messageIndex - 1].current_limit.overload;

      // Outputs for Function Call SubSystem: '<Root>/Limits Handler'
      SupervisorFSM_RX_LimitsHandler();

      // End of Outputs for SubSystem: '<Root>/Limits Handler'
    } else if (SupervisorFSM_RX_rtu_EventsRx->
               events[SupervisorFSM_RX_B.messageIndex - 1].current_pid) {
      uint8_T Motorconfig_tmp;

      // Selector: '<S1>/Selector2'
      Motorconfig_tmp = SupervisorFSM_RX_rtu_MessagesRx->
        messages[SupervisorFSM_RX_B.messageIndex - 1].current_pid.Ks;
      SupervisorFSM_RX_B.Motorconfig.Kp = SupervisorFSM_RX_ConvertPid
        (SupervisorFSM_RX_rtu_MessagesRx->
         messages[SupervisorFSM_RX_B.messageIndex - 1].current_pid.Kp,
         Motorconfig_tmp);
      SupervisorFSM_RX_B.Motorconfig.Ki = SupervisorFSM_RX_ConvertPid
        (SupervisorFSM_RX_rtu_MessagesRx->
         messages[SupervisorFSM_RX_B.messageIndex - 1].current_pid.Ki,
         Motorconfig_tmp);
      SupervisorFSM_RX_B.Motorconfig.Kd = SupervisorFSM_RX_ConvertPid
        (SupervisorFSM_RX_rtu_MessagesRx->
         messages[SupervisorFSM_RX_B.messageIndex - 1].current_pid.Kd,
         Motorconfig_tmp);
      SupervisorFSM_RX_B.Motorconfig.Ks = Motorconfig_tmp;
      SupervisorFSM_RX_B.hasMotorConfigChanged = true;
    }
  }

  // End of Chart: '<S1>/CAN Event Dispatcher'
}

// Function for Chart: '<Root>/SupervisorFSM_RX'
static boolean_T SupervisorFSM_isBoardConfigured(void)
{
  return true;
}

// System initialize for referenced model: 'SupervisorFSM_RX'
void SupervisorFSM_RX_Init(Flags *rty_Flags)
{
  SupervisorFSM_RX_B.ConfigurationParameters_o.motorconfig.Imin = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.motorconfig.Imax = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.motorconfig.has_hall_sens = false;
  SupervisorFSM_RX_B.ConfigurationParameters_o.motorconfig.pole_pairs = 0U;
  SupervisorFSM_RX_B.ConfigurationParameters_o.motorconfig.reduction = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.motorconfig.Kp = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.motorconfig.Ki = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.motorconfig.Kd = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.motorconfig.Ks = 0U;
  SupervisorFSM_RX_B.ConfigurationParameters_o.motorconfig.Kbemf = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.motorconfig.Rphase = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.motorconfig.Vmax = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.motorconfig.Vcc = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.estimationconfig.velocity_mode =
    EstimationVelocityModes_Disabled;
  SupervisorFSM_RX_B.ConfigurationParameters_o.PosLoopPID.OutMax = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.PosLoopPID.OutMin = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.PosLoopPID.P = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.PosLoopPID.I = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.PosLoopPID.D = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.PosLoopPID.N = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.PosLoopPID.I0 = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.PosLoopPID.D0 = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.VelLoopPID.OutMax = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.VelLoopPID.OutMin = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.VelLoopPID.P = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.VelLoopPID.I = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.VelLoopPID.D = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.VelLoopPID.N = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.VelLoopPID.I0 = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.VelLoopPID.D0 = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.DirLoopPID.OutMax = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.DirLoopPID.OutMin = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.DirLoopPID.P = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.DirLoopPID.I = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.DirLoopPID.D = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.DirLoopPID.N = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.DirLoopPID.I0 = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.DirLoopPID.D0 = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.thresholds.jntPosMin = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.thresholds.jntPosMax = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.thresholds.hardwareJntPosMin =
    0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.thresholds.hardwareJntPosMax =
    0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.thresholds.rotorPosMin = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.thresholds.rotorPosMax = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.thresholds.jntVelMax = 0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.thresholds.velocityTimeout = 0U;
  SupervisorFSM_RX_B.ConfigurationParameters_o.thresholds.motorNominalCurrents =
    0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.thresholds.motorPeakCurrents =
    0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.thresholds.motorOverloadCurrents =
    0.0F;
  SupervisorFSM_RX_B.ConfigurationParameters_o.thresholds.motorPwmLimit = 0U;

  // SystemInitialize for Chart: '<Root>/SupervisorFSM_RX' incorporates:
  //   SubSystem: '<Root>/CAN Message Handler'

  Supervis_CANMessageHandler_Init();

  // SystemInitialize for BusCreator: '<Root>/Bus Creator'
  rty_Flags->control_mode = SupervisorFSM_RX_B.controlModeDefined;
  rty_Flags->enable_sending_msg_status =
    SupervisorFSM_RX_B.enableSendingMsgStatus;
  rty_Flags->fault_button = SupervisorFSM_RX_B.isFaultButtonPressed;
}

// Output and update for referenced model: 'SupervisorFSM_RX'
void SupervisorFSM_RX(const SensorsData *rtu_SensorsData, const ExternalFlags
                      *rtu_ExternalFlags, const ControlOutputs
                      *rtu_ControlOutputs, const BUS_MESSAGES_RX_MULTIPLE
                      *rtu_MessagesRx, const BUS_EVENTS_RX_MULTIPLE
                      *rtu_EventsRx, const BUS_CAN_RX_ERRORS_MULTIPLE
                      *rtu_ErrorsRx, Flags *rty_Flags, Targets *rty_Targets,
                      ConfigurationParameters *rty_ConfigurationParameters)
{
  SupervisorFSM_R_rtu_SensorsData = rtu_SensorsData;
  SupervisorFS_rtu_ControlOutputs = rtu_ControlOutputs;
  SupervisorFSM_RX_rtu_MessagesRx = rtu_MessagesRx;
  SupervisorFSM_RX_rtu_EventsRx = rtu_EventsRx;
  SupervisorFSM_RX_rtu_ErrorsRx = rtu_ErrorsRx;

  // Chart: '<Root>/SupervisorFSM_RX'
  SupervisorFSM_RX_DW.ExternalFlags_fault_button_prev =
    SupervisorFSM_RX_DW.ExternalFlags_fault_button_star;
  SupervisorFSM_RX_DW.ExternalFlags_fault_button_star =
    rtu_ExternalFlags->fault_button;
  if (SupervisorFSM_RX_DW.is_active_c2_SupervisorFSM_RX == 0U) {
    SupervisorFSM_RX_DW.ExternalFlags_fault_button_prev =
      rtu_ExternalFlags->fault_button;
    SupervisorFSM_RX_DW.ExternalFlags_fault_button_star =
      rtu_ExternalFlags->fault_button;
    SupervisorFSM_RX_DW.is_active_c2_SupervisorFSM_RX = 1U;
    SupervisorFSM_RX_DW.is_active_INIT_CONFIGURATION_PA = 1U;
    SupervisorFSM_RX_DW.is_INIT_CONFIGURATION_PARAMETER =
      SupervisorFSM_RX_IN_Init;
    SupervisorFSM_RX_B.ConfigurationParameters_o = InitConfParams;
    SupervisorFSM_RX_DW.is_active_STATE_HANDLER = 1U;
    SupervisorFSM_RX_DW.is_STATE_HANDLER = SupervisorFS_IN_NotConfigured_o;
    SupervisorFSM_RX_B.BoardSt = BoardState_NotConfigured;
    SupervisorFSM_RX_DW.is_active_FAULT_HANDLER = 1U;
    SupervisorFSM_RX_DW.is_active_FaultButtonPressed = 1U;
    SupervisorFSM_RX_DW.is_FaultButtonPressed = SupervisorFSM_RX_IN_NoFault;
    SupervisorFSM_RX_B.isFaultButtonPressed = false;
    SupervisorFSM_RX_DW.is_active_OverCurrent = 1U;
    SupervisorFSM_RX_DW.is_OverCurrent = Superviso_IN_LimitNonConfigured;
    SupervisorFSM_RX_DW.is_active_CAN_MESSAGES_FOR_LOOP = 1U;
    SupervisorFSM_RX_B.messageIndex = 1;
    while (SupervisorFSM_RX_B.messageIndex < 5) {
      // Outputs for Function Call SubSystem: '<Root>/CAN Message Handler'
      SupervisorFSM_CANMessageHandler();

      // End of Outputs for SubSystem: '<Root>/CAN Message Handler'
      if (SupervisorFSM_RX_B.messageIndex > 2147483646) {
        SupervisorFSM_RX_B.messageIndex = MAX_int32_T;
      } else {
        SupervisorFSM_RX_B.messageIndex++;
      }
    }

    SupervisorFSM_RX_DW.is_CAN_MESSAGES_FOR_LOOP = SupervisorFSM_RX_IN_state1;
  } else {
    if ((SupervisorFSM_RX_DW.is_active_INIT_CONFIGURATION_PA != 0U) &&
        (SupervisorFSM_RX_DW.is_INIT_CONFIGURATION_PARAMETER == 1)) {
      SupervisorFSM_RX_B.ConfigurationParameters_o = InitConfParams;
    }

    if (SupervisorFSM_RX_DW.is_active_STATE_HANDLER != 0U) {
      switch (SupervisorFSM_RX_DW.is_STATE_HANDLER) {
       case SupervisorFSM_RX_IN_Configured:
        SupervisorFSM_RX_B.BoardSt = BoardState_Configured;
        break;

       case SupervisorFSM_RX_IN_Fault:
        SupervisorFSM_RX_B.BoardSt = BoardState_Fault;
        break;

       case SupervisorFS_IN_NotConfigured_o:
        SupervisorFSM_RX_B.BoardSt = BoardState_NotConfigured;
        if (SupervisorFSM_isBoardConfigured()) {
          SupervisorFSM_RX_DW.is_STATE_HANDLER = SupervisorFSM_RX_IN_Configured;
          SupervisorFSM_RX_B.BoardSt = BoardState_Configured;
        }
        break;
      }
    }

    if (SupervisorFSM_RX_DW.is_active_FAULT_HANDLER != 0U) {
      if (SupervisorFSM_RX_DW.is_active_FaultButtonPressed != 0U) {
        switch (SupervisorFSM_RX_DW.is_FaultButtonPressed) {
         case SupervisorFSM__IN_ButtonPressed:
          SupervisorFSM_RX_B.isFaultButtonPressed = true;
          if ((SupervisorFSM_RX_DW.ExternalFlags_fault_button_prev !=
               SupervisorFSM_RX_DW.ExternalFlags_fault_button_star) &&
              (!SupervisorFSM_RX_DW.ExternalFlags_fault_button_star)) {
            SupervisorFSM_RX_DW.is_FaultButtonPressed =
              SupervisorFSM_RX_IN_NoFault;
            SupervisorFSM_RX_B.isFaultButtonPressed = false;
          }
          break;

         case SupervisorFSM_RX_IN_NoFault:
          SupervisorFSM_RX_B.isFaultButtonPressed = false;
          if ((SupervisorFSM_RX_DW.ExternalFlags_fault_button_prev !=
               SupervisorFSM_RX_DW.ExternalFlags_fault_button_star) &&
              SupervisorFSM_RX_DW.ExternalFlags_fault_button_star) {
            SupervisorFSM_RX_DW.is_FaultButtonPressed =
              SupervisorFSM__IN_ButtonPressed;
            SupervisorFSM_RX_B.isFaultButtonPressed = true;

            // Outputs for Function Call SubSystem: '<Root>/Control Mode Handler Motor 0' 
            Superv_ControlModeHandlerMotor0();

            // End of Outputs for SubSystem: '<Root>/Control Mode Handler Motor 0' 
          }
          break;
        }
      }

      if (SupervisorFSM_RX_DW.is_active_OverCurrent != 0U) {
        switch (SupervisorFSM_RX_DW.is_OverCurrent) {
         case Superviso_IN_LimitNonConfigured:
          if (SupervisorFSM_RX_B.areLimitsSet) {
            SupervisorFSM_RX_DW.is_OverCurrent = SupervisorFSM_RX_IN_NoFault;
            SupervisorFSM_RX_B.isInOverCurrent = false;

            // MotorFaultFlags.overCurrent=0;
          }
          break;

         case SupervisorFSM_RX_IN_NoFault:
          SupervisorFSM_RX_B.isInOverCurrent = false;
          if (std::abs(rtu_ControlOutputs->Iq_fbk.current) >=
              SupervisorFSM_RX_B.ConfigurationParameters_o.thresholds.motorOverloadCurrents)
          {
            SupervisorFSM_RX_DW.is_OverCurrent = SupervisorF_IN_OverCurrentFault;

            // MotorFaultFlags.overCurrent=1;
            SupervisorFSM_RX_B.isInOverCurrent = true;

            // Outputs for Function Call SubSystem: '<Root>/Control Mode Handler Motor 0' 
            Superv_ControlModeHandlerMotor0();

            // End of Outputs for SubSystem: '<Root>/Control Mode Handler Motor 0' 
          }
          break;

         case SupervisorF_IN_OverCurrentFault:
          SupervisorFSM_RX_B.isInOverCurrent = true;
          if (std::abs(rtu_ControlOutputs->Iq_fbk.current) <
              SupervisorFSM_RX_B.ConfigurationParameters_o.thresholds.motorOverloadCurrents)
          {
            SupervisorFSM_RX_DW.is_OverCurrent = SupervisorFSM_RX_IN_NoFault;
            SupervisorFSM_RX_B.isInOverCurrent = false;

            // MotorFaultFlags.overCurrent=0;
          }
          break;
        }
      }
    }

    if ((SupervisorFSM_RX_DW.is_active_CAN_MESSAGES_FOR_LOOP != 0U) &&
        (SupervisorFSM_RX_DW.is_CAN_MESSAGES_FOR_LOOP == 1)) {
      SupervisorFSM_RX_B.messageIndex = 1;
      while (SupervisorFSM_RX_B.messageIndex < 5) {
        // Outputs for Function Call SubSystem: '<Root>/CAN Message Handler'
        SupervisorFSM_CANMessageHandler();

        // End of Outputs for SubSystem: '<Root>/CAN Message Handler'
        if (SupervisorFSM_RX_B.messageIndex > 2147483646) {
          SupervisorFSM_RX_B.messageIndex = MAX_int32_T;
        } else {
          SupervisorFSM_RX_B.messageIndex++;
        }
      }

      SupervisorFSM_RX_DW.is_CAN_MESSAGES_FOR_LOOP = SupervisorFSM_RX_IN_state1;
    }
  }

  // End of Chart: '<Root>/SupervisorFSM_RX'

  // BusCreator: '<Root>/Bus Creator'
  rty_Flags->control_mode = SupervisorFSM_RX_B.controlModeDefined;
  rty_Flags->enable_sending_msg_status =
    SupervisorFSM_RX_B.enableSendingMsgStatus;
  rty_Flags->fault_button = SupervisorFSM_RX_B.isFaultButtonPressed;

  // Switch: '<Root>/Switch'
  if (SupervisorFSM_RX_B.hasMotorConfigChanged) {
    rty_ConfigurationParameters->motorconfig = SupervisorFSM_RX_B.Motorconfig;
  } else {
    rty_ConfigurationParameters->motorconfig =
      SupervisorFSM_RX_B.ConfigurationParameters_o.motorconfig;
  }

  // End of Switch: '<Root>/Switch'

  // Switch: '<Root>/Switch1'
  if (SupervisorFSM_RX_B.hasThresholdsChanged) {
    rty_ConfigurationParameters->thresholds = SupervisorFSM_RX_B.thresholds;
  } else {
    rty_ConfigurationParameters->thresholds =
      SupervisorFSM_RX_B.ConfigurationParameters_o.thresholds;
  }

  // End of Switch: '<Root>/Switch1'

  // BusCreator: '<Root>/Bus Creator1'
  rty_ConfigurationParameters->estimationconfig =
    SupervisorFSM_RX_B.ConfigurationParameters_o.estimationconfig;
  rty_ConfigurationParameters->PosLoopPID =
    SupervisorFSM_RX_B.ConfigurationParameters_o.PosLoopPID;
  rty_ConfigurationParameters->VelLoopPID =
    SupervisorFSM_RX_B.ConfigurationParameters_o.VelLoopPID;
  rty_ConfigurationParameters->DirLoopPID =
    SupervisorFSM_RX_B.ConfigurationParameters_o.DirLoopPID;

  // Switch: '<Root>/Switch2' incorporates:
  //   BusCreator: '<Root>/Bus Creator2'
  //   BusCreator: '<Root>/Bus Creator3'
  //   BusCreator: '<Root>/Bus Creator4'
  //   BusCreator: '<Root>/Bus Creator5'
  //   Constant: '<Root>/Constant'
  //   Constant: '<Root>/Constant1'
  //   Constant: '<Root>/Constant2'
  //   Constant: '<Root>/Constant3'

  if (SupervisorFSM_RX_B.hasSetpointChanged) {
    *rty_Targets = SupervisorFSM_RX_B.targets;
  } else {
    rty_Targets->jointpositions.position = 0.0F;
    rty_Targets->jointvelocities.velocity = 0.0F;
    rty_Targets->motorcurrent.current = 0.0F;
    rty_Targets->motorvoltage.voltage = 0.0F;
  }

  // End of Switch: '<Root>/Switch2'
}

// Model initialize function
void SupervisorFSM_RX_initialize(const char_T **rt_errorStatus)
{
  RT_MODEL_SupervisorFSM_RX_T *const SupervisorFSM_RX_M =
    &(SupervisorFSM_RX_MdlrefDW.rtm);

  // Registration code

  // initialize error status
  rtmSetErrorStatusPointer(SupervisorFSM_RX_M, rt_errorStatus);
}

//
// File trailer for generated code.
//
// [EOF]
//
