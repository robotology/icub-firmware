//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_RX.cpp
//
// Code generated for Simulink model 'SupervisorFSM_RX'.
//
// Model version                  : 4.67
// Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
// C/C++ source code generated on : Tue Aug  9 15:17:46 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "SupervisorFSM_RX.h"
#include "rtwtypes.h"
#include "SupervisorFSM_RX_types.h"
#include <cmath>
#include "rt_roundd_snf.h"
#include "SupervisorFSM_RX_private.h"

// Named constants for Chart: '<S2>/ControlMode_SM_motor0'
const uint8_T SupervisorFSM_RX_IN_Current = 1U;
const uint8_T SupervisorFSM_RX_IN_HWFault = 2U;
const uint8_T SupervisorFSM_RX_IN_Idle = 3U;
const uint8_T SupervisorFSM_RX_IN_Position = 5U;
const uint8_T SupervisorFSM_RX_IN_Velocity = 6U;
const uint8_T SupervisorFSM_RX_IN_Voltage = 7U;
const uint8_T SupervisorFSM__IN_NotConfigured = 4U;

// Named constants for Chart: '<Root>/SupervisorRX State Handler'
const uint8_T Superviso_IN_LimitNonConfigured = 1U;
const uint8_T SupervisorFSM_RX_IN_Configured = 1U;
const uint8_T SupervisorFSM_RX_IN_Fault = 2U;
const uint8_T SupervisorFSM_RX_IN_NoFault = 2U;
const uint8_T SupervisorFSM_RX_IN_state1 = 1U;
const uint8_T SupervisorFSM__IN_ButtonPressed = 1U;
const uint8_T SupervisorFS_IN_NotConfigured_m = 3U;
const uint8_T SupervisorF_IN_OverCurrentFault = 3U;
MdlrefDW_SupervisorFSM_RX_T SupervisorFSM_RX_MdlrefDW;

// Block signals (default storage)
B_SupervisorFSM_RX_c_T SupervisorFSM_RX_B;

// Block states (default storage)
DW_SupervisorFSM_RX_f_T SupervisorFSM_RX_DW;

// Forward declaration for local functions
static boolean_T Supervisor_CheckSetPointVoltage(void);
static boolean_T Supervisor_CheckSetPointCurrent(void);
static boolean_T Superviso_CheckSetPointVelocity(void);
static boolean_T Superviso_CheckSetPointPosition(void);

// Forward declaration for local functions
static boolean_T SupervisorFSM_RX_IsBoardReady(void);
static boolean_T SupervisorFS_isConfigurationSet(void);
static boolean_T SupervisorFS_IsNewCtrl_Velocity(void);
static boolean_T SupervisorFSM_IsNewCtrl_Voltage(void);
static boolean_T SupervisorFSM_IsNewCtrl_Current(void);
static boolean_T SupervisorFS_IsNewCtrl_Position(void);
static boolean_T SupervisorFSM_RX_IsNewCtrl_Idle(void);
static void SupervisorFSM_RX_Voltage(void);

// Forward declaration for local functions
static ControlModes SupervisorFSM_RX_convert(MCControlModes mccontrolmode);
static void SupervisorFSM_formatMotorConfig(const BUS_MESSAGES_RX *Selector2);

// Forward declaration for local functions
static boolean_T SupervisorFSM_isBoardConfigured(void);

// Declare global variables for system: model 'SupervisorFSM_RX'
const SensorsData *SupervisorFSM_R_rtu_SensorsData;// '<Root>/SensorsData'
const ControlOutputs *SupervisorFS_rtu_ControlOutputs;// '<Root>/ControlOutputs' 
const BUS_MESSAGES_RX_MULTIPLE *SupervisorFSM_RX_rtu_MessagesRx;// '<Root>/MessagesRx' 
const BUS_STATUS_RX_MULTIPLE *SupervisorFSM_RX_rtu_StatusRx;// '<Root>/StatusRx' 
const BUS_CAN_RX_ERRORS_MULTIPLE *SupervisorFSM_RX_rtu_ErrorsRx;// '<Root>/ErrorsRx' 

// Function for Chart: '<S5>/Chart1'
static boolean_T Supervisor_CheckSetPointVoltage(void)
{
  return SupervisorFSM_RX_B.controlModeDefined == ControlModes_Voltage;
}

// Function for Chart: '<S5>/Chart1'
static boolean_T Supervisor_CheckSetPointCurrent(void)
{
  return SupervisorFSM_RX_B.controlModeDefined == ControlModes_Current;
}

// Function for Chart: '<S5>/Chart1'
static boolean_T Superviso_CheckSetPointVelocity(void)
{
  return SupervisorFSM_RX_B.controlModeDefined == ControlModes_Velocity;
}

// Function for Chart: '<S5>/Chart1'
static boolean_T Superviso_CheckSetPointPosition(void)
{
  return SupervisorFSM_RX_B.controlModeDefined == ControlModes_Position;
}

// System initialize for function-call system: '<Root>/SetpointHandler'
void Supervisor_SetpointHandler_Init(void)
{
  // SystemInitialize for Chart: '<S5>/Chart1'
  SupervisorFSM_RX_B.targets.jointpositions.position = 0.0F;
  SupervisorFSM_RX_B.targets.jointvelocities.velocity = 0.0F;
  SupervisorFSM_RX_B.targets.motorcurrent.current = 0.0F;
  SupervisorFSM_RX_B.targets.motorvoltage.voltage = 0.0F;
}

// Output and update for function-call system: '<Root>/SetpointHandler'
void SupervisorFSM_R_SetpointHandler(void)
{
  // Chart: '<S5>/Chart1'
  if (SupervisorFSM_RX_DW.is_active_c4_SupervisorFSM_RX == 0U) {
    real32_T newSetpoint;
    SupervisorFSM_RX_DW.is_active_c4_SupervisorFSM_RX = 1U;
    if (SupervisorFSM_RX_B.receivedNewSetpoint) {
      if (Supervisor_CheckSetPointVoltage()) {
        newSetpoint = SupervisorFSM_RX_B.newSetpoint.voltage;
      } else if (Supervisor_CheckSetPointCurrent()) {
        newSetpoint = SupervisorFSM_RX_B.newSetpoint.current;
      } else if (Superviso_CheckSetPointVelocity()) {
        newSetpoint = SupervisorFSM_RX_B.newSetpoint.velocity;
      } else {
        newSetpoint = 0.0F;
      }
    } else {
      newSetpoint = SupervisorFSM_RX_B.newSetpoint_i;
    }

    SupervisorFSM_RX_B.targets.motorvoltage.voltage = 0.0F;
    SupervisorFSM_RX_B.targets.motorcurrent.current = 0.0F;
    SupervisorFSM_RX_B.targets.jointvelocities.velocity = 0.0F;
    SupervisorFSM_RX_B.targets.jointpositions.position = 0.0F;
    if (Supervisor_CheckSetPointCurrent()) {
      SupervisorFSM_RX_B.targets.motorcurrent.current = newSetpoint;
      SupervisorFSM_RX_B.hasSetpointChanged = true;
    } else if (Supervisor_CheckSetPointVoltage()) {
      SupervisorFSM_RX_B.targets.motorvoltage.voltage = newSetpoint;
      SupervisorFSM_RX_B.hasSetpointChanged = true;
    } else if (Superviso_CheckSetPointVelocity()) {
      SupervisorFSM_RX_B.targets.jointvelocities.velocity = newSetpoint;
      SupervisorFSM_RX_B.hasSetpointChanged = true;
    } else if (Superviso_CheckSetPointPosition()) {
      SupervisorFSM_RX_B.targets.jointpositions.position = newSetpoint;
      SupervisorFSM_RX_B.hasSetpointChanged = true;
    }
  } else {
    real32_T newSetpoint;
    if (SupervisorFSM_RX_B.receivedNewSetpoint) {
      if (Supervisor_CheckSetPointVoltage()) {
        newSetpoint = SupervisorFSM_RX_B.newSetpoint.voltage;
      } else if (Supervisor_CheckSetPointCurrent()) {
        newSetpoint = SupervisorFSM_RX_B.newSetpoint.current;
      } else if (Superviso_CheckSetPointVelocity()) {
        newSetpoint = SupervisorFSM_RX_B.newSetpoint.velocity;
      } else {
        newSetpoint = 0.0F;
      }
    } else {
      newSetpoint = SupervisorFSM_RX_B.newSetpoint_i;
    }

    SupervisorFSM_RX_B.targets.motorvoltage.voltage = 0.0F;
    SupervisorFSM_RX_B.targets.motorcurrent.current = 0.0F;
    SupervisorFSM_RX_B.targets.jointvelocities.velocity = 0.0F;
    SupervisorFSM_RX_B.targets.jointpositions.position = 0.0F;
    if (Supervisor_CheckSetPointCurrent()) {
      SupervisorFSM_RX_B.targets.motorcurrent.current = newSetpoint;
      SupervisorFSM_RX_B.hasSetpointChanged = true;
    } else if (Supervisor_CheckSetPointVoltage()) {
      SupervisorFSM_RX_B.targets.motorvoltage.voltage = newSetpoint;
      SupervisorFSM_RX_B.hasSetpointChanged = true;
    } else if (Superviso_CheckSetPointVelocity()) {
      SupervisorFSM_RX_B.targets.jointvelocities.velocity = newSetpoint;
      SupervisorFSM_RX_B.hasSetpointChanged = true;
    } else if (Superviso_CheckSetPointPosition()) {
      SupervisorFSM_RX_B.targets.jointpositions.position = newSetpoint;
      SupervisorFSM_RX_B.hasSetpointChanged = true;
    }
  }

  // End of Chart: '<S5>/Chart1'
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
static boolean_T SupervisorFS_IsNewCtrl_Velocity(void)
{
  return SupervisorFSM_RX_B.requiredControlMode == ControlModes_Velocity;
}

// Function for Chart: '<S2>/ControlMode_SM_motor0'
static boolean_T SupervisorFSM_IsNewCtrl_Voltage(void)
{
  return SupervisorFSM_RX_B.requiredControlMode == ControlModes_Voltage;
}

// Function for Chart: '<S2>/ControlMode_SM_motor0'
static boolean_T SupervisorFSM_IsNewCtrl_Current(void)
{
  return SupervisorFSM_RX_B.requiredControlMode == ControlModes_Current;
}

// Function for Chart: '<S2>/ControlMode_SM_motor0'
static boolean_T SupervisorFS_IsNewCtrl_Position(void)
{
  return SupervisorFSM_RX_B.requiredControlMode == ControlModes_Position;
}

// Function for Chart: '<S2>/ControlMode_SM_motor0'
static boolean_T SupervisorFSM_RX_IsNewCtrl_Idle(void)
{
  return SupervisorFSM_RX_B.requiredControlMode == ControlModes_Idle;
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
        SupervisorFSM_RX_B.newSetpoint_i = 0.0F;

        // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
        SupervisorFSM_R_SetpointHandler();

        // End of Outputs for SubSystem: '<Root>/SetpointHandler'
      } else {
        guard2 = true;
      }
    } else if (!SupervisorFSM_IsNewCtrl_Voltage()) {
      if (SupervisorFSM_IsNewCtrl_Current()) {
        // Chart: '<S2>/ControlMode_SM_motor0'
        SupervisorFSM_RX_B.newSetpoint_i =
          SupervisorFS_rtu_ControlOutputs->Iq_fbk.current;
        SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
          SupervisorFSM_RX_IN_Current;
        SupervisorFSM_RX_B.controlModeDefined = ControlModes_Current;

        // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
        SupervisorFSM_R_SetpointHandler();

        // End of Outputs for SubSystem: '<Root>/SetpointHandler'
      } else if (SupervisorFS_IsNewCtrl_Position()) {
        SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
          SupervisorFSM_RX_IN_Position;
        SupervisorFSM_RX_B.controlModeDefined = ControlModes_Position;

        // Chart: '<S2>/ControlMode_SM_motor0'
        SupervisorFSM_RX_B.newSetpoint_i =
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
    boolean_T guard10 = false;
    boolean_T guard11 = false;
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
    guard10 = false;
    guard11 = false;
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
          guard7 = true;
        }
      } else {
        guard7 = true;
      }
      break;

     case SupervisorFSM_RX_IN_HWFault:
      {
        boolean_T tmp;
        SupervisorFSM_RX_B.controlModeDefined = ControlModes_HwFaultCM;
        tmp = !SupervisorFSM_RX_B.isInOverCurrent;
        if (tmp && SupervisorFSM_RX_IsNewCtrl_Idle() &&
            (!SupervisorFS_isConfigurationSet())) {
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM__IN_NotConfigured;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_NotConfigured;
          rtw_disableMotor();
        } else if (tmp && SupervisorFSM_RX_IsNewCtrl_Idle()) {
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX = SupervisorFSM_RX_IN_Idle;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Idle;
          rtw_disableMotor();

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          // this updates the targets value
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        }
      }
      break;

     case SupervisorFSM_RX_IN_Idle:
      SupervisorFSM_RX_B.controlModeDefined = ControlModes_Idle;
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
          guard8 = true;
        }
      } else {
        guard8 = true;
      }
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
          guard4 = true;
        } else {
          guard11 = true;
        }
      } else {
        guard11 = true;
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
          guard5 = true;
        } else {
          guard10 = true;
        }
      } else {
        guard10 = true;
      }
      break;

     default:
      // case IN_Voltage:
      SupervisorFSM_RX_Voltage();
      break;
    }

    if (guard11) {
      if ((!SupervisorFSM_RX_IsNewCtrl_Idle()) &&
          (!SupervisorFS_IsNewCtrl_Position())) {
        if (SupervisorFSM_IsNewCtrl_Current()) {
          SupervisorFSM_RX_B.newSetpoint_i =
            SupervisorFS_rtu_ControlOutputs->Iq_fbk.current;
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_Current;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Current;

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else if (SupervisorFSM_IsNewCtrl_Voltage()) {
          SupervisorFSM_RX_B.newSetpoint_i = SupervisorFS_rtu_ControlOutputs->Vq;
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_Voltage;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Voltage;

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else if (SupervisorFS_IsNewCtrl_Velocity()) {
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_Velocity;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Velocity;
          SupervisorFSM_RX_B.newSetpoint_i = 0.0F;

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else {
          guard9 = true;
        }
      } else {
        guard9 = true;
      }
    }

    if (guard10) {
      if (!SupervisorFS_IsNewCtrl_Velocity()) {
        if (SupervisorFSM_IsNewCtrl_Voltage()) {
          SupervisorFSM_RX_B.newSetpoint_i = SupervisorFS_rtu_ControlOutputs->Vq;
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_Voltage;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Voltage;

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else if (SupervisorFSM_IsNewCtrl_Current()) {
          SupervisorFSM_RX_B.newSetpoint_i =
            SupervisorFS_rtu_ControlOutputs->Iq_fbk.current;
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_Current;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Current;

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else if (SupervisorFS_IsNewCtrl_Position()) {
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_Position;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Position;
          SupervisorFSM_RX_B.newSetpoint_i =
            SupervisorFSM_R_rtu_SensorsData->jointpositions.position;

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else if (SupervisorFSM_RX_IsNewCtrl_Idle()) {
          guard5 = true;
        } else {
          guard6 = true;
        }
      } else {
        guard6 = true;
      }
    }

    if (guard9) {
      if (SupervisorFSM_RX_IsNewCtrl_Idle()) {
        guard4 = true;
      }
    }

    if (guard8) {
      if (!SupervisorFSM_RX_IsNewCtrl_Idle()) {
        rtw_enableMotor();
        if (SupervisorFS_IsNewCtrl_Position()) {
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_Position;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Position;
          SupervisorFSM_RX_B.newSetpoint_i =
            SupervisorFSM_R_rtu_SensorsData->jointpositions.position;

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else if (SupervisorFSM_IsNewCtrl_Current()) {
          SupervisorFSM_RX_B.newSetpoint_i = 0.0F;
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_Current;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Current;

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else if (SupervisorFSM_IsNewCtrl_Voltage()) {
          SupervisorFSM_RX_B.newSetpoint_i = 0.0F;
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_Voltage;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Voltage;

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else if (SupervisorFS_IsNewCtrl_Velocity()) {
          SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
            SupervisorFSM_RX_IN_Velocity;
          SupervisorFSM_RX_B.controlModeDefined = ControlModes_Velocity;
          SupervisorFSM_RX_B.newSetpoint_i = 0.0F;

          // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
          SupervisorFSM_R_SetpointHandler();

          // End of Outputs for SubSystem: '<Root>/SetpointHandler'
        } else {
          guard3 = true;
        }
      } else {
        guard3 = true;
      }
    }

    if (guard7) {
      if ((!SupervisorFSM_RX_IsNewCtrl_Idle()) &&
          (!SupervisorFS_IsNewCtrl_Position())) {
        if (!SupervisorFSM_IsNewCtrl_Current()) {
          if (SupervisorFSM_IsNewCtrl_Voltage()) {
            SupervisorFSM_RX_B.newSetpoint_i =
              SupervisorFS_rtu_ControlOutputs->Vq;
            SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
              SupervisorFSM_RX_IN_Voltage;
            SupervisorFSM_RX_B.controlModeDefined = ControlModes_Voltage;

            // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
            SupervisorFSM_R_SetpointHandler();

            // End of Outputs for SubSystem: '<Root>/SetpointHandler'
          } else if (SupervisorFS_IsNewCtrl_Velocity()) {
            SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX =
              SupervisorFSM_RX_IN_Velocity;
            SupervisorFSM_RX_B.controlModeDefined = ControlModes_Velocity;
            SupervisorFSM_RX_B.newSetpoint_i = 0.0F;

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
          SupervisorFSM_RX_B.newSetpoint_i =
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

    if (guard6) {
      SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX = SupervisorFSM_RX_IN_Velocity;
      SupervisorFSM_RX_B.controlModeDefined = ControlModes_Velocity;
      SupervisorFSM_RX_B.newSetpoint_i = 0.0F;

      // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
      SupervisorFSM_R_SetpointHandler();

      // End of Outputs for SubSystem: '<Root>/SetpointHandler'
    }

    if (guard5) {
      SupervisorFSM_RX_DW.is_c12_SupervisorFSM_RX = SupervisorFSM_RX_IN_Idle;
      SupervisorFSM_RX_B.controlModeDefined = ControlModes_Idle;
      rtw_disableMotor();

      // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
      // this updates the targets value
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
    if (!SupervisorFSM_RX_B.areLimitsSet) {
      SupervisorFSM_RX_B.areLimitsSet = true;

      // Outputs for Function Call SubSystem: '<Root>/Control Mode Handler Motor 0' 
      Superv_ControlModeHandlerMotor0();

      // End of Outputs for SubSystem: '<Root>/Control Mode Handler Motor 0'
    }
  }

  // End of Chart: '<S3>/Chart'
}

// System initialize for function-call system: '<Root>/PID Handler'
void SupervisorFSM_R_PIDHandler_Init(void)
{
  // SystemInitialize for Chart: '<S4>/Chart'
  SupervisorFSM_RX_B.CurrentPID.OutMax = 0.0F;
  SupervisorFSM_RX_B.CurrentPID.OutMin = 0.0F;
  SupervisorFSM_RX_B.CurrentPID.P = 0.0F;
  SupervisorFSM_RX_B.CurrentPID.I = 0.0F;
  SupervisorFSM_RX_B.CurrentPID.D = 0.0F;
  SupervisorFSM_RX_B.CurrentPID.N = 0.0F;
  SupervisorFSM_RX_B.CurrentPID.I0 = 0.0F;
  SupervisorFSM_RX_B.CurrentPID.D0 = 0.0F;
  SupervisorFSM_RX_B.CurrentPID.shift_factor = 0U;
  SupervisorFSM_RX_B.VelocityPID.OutMax = 0.0F;
  SupervisorFSM_RX_B.VelocityPID.OutMin = 0.0F;
  SupervisorFSM_RX_B.VelocityPID.P = 0.0F;
  SupervisorFSM_RX_B.VelocityPID.I = 0.0F;
  SupervisorFSM_RX_B.VelocityPID.D = 0.0F;
  SupervisorFSM_RX_B.VelocityPID.N = 0.0F;
  SupervisorFSM_RX_B.VelocityPID.I0 = 0.0F;
  SupervisorFSM_RX_B.VelocityPID.D0 = 0.0F;
  SupervisorFSM_RX_B.VelocityPID.shift_factor = 0U;
  SupervisorFSM_RX_B.PositionPID.OutMax = 0.0F;
  SupervisorFSM_RX_B.PositionPID.OutMin = 0.0F;
  SupervisorFSM_RX_B.PositionPID.P = 0.0F;
  SupervisorFSM_RX_B.PositionPID.I = 0.0F;
  SupervisorFSM_RX_B.PositionPID.D = 0.0F;
  SupervisorFSM_RX_B.PositionPID.N = 0.0F;
  SupervisorFSM_RX_B.PositionPID.I0 = 0.0F;
  SupervisorFSM_RX_B.PositionPID.D0 = 0.0F;
  SupervisorFSM_RX_B.PositionPID.shift_factor = 0U;
  SupervisorFSM_RX_B.OpenLoopPID.OutMax = 0.0F;
  SupervisorFSM_RX_B.OpenLoopPID.OutMin = 0.0F;
  SupervisorFSM_RX_B.OpenLoopPID.P = 0.0F;
  SupervisorFSM_RX_B.OpenLoopPID.I = 0.0F;
  SupervisorFSM_RX_B.OpenLoopPID.D = 0.0F;
  SupervisorFSM_RX_B.OpenLoopPID.N = 0.0F;
  SupervisorFSM_RX_B.OpenLoopPID.I0 = 0.0F;
  SupervisorFSM_RX_B.OpenLoopPID.D0 = 0.0F;
  SupervisorFSM_RX_B.OpenLoopPID.shift_factor = 0U;
}

// Output and update for function-call system: '<Root>/PID Handler'
void SupervisorFSM_RX_PIDHandler(void)
{
  // Chart: '<S4>/Chart'
  if (SupervisorFSM_RX_DW.is_active_c3_SupervisorFSM_RX == 0U) {
    SupervisorFSM_RX_DW.is_active_c3_SupervisorFSM_RX = 1U;
    SupervisorFSM_RX_B.PositionPID = InitConfParams.PosLoopPID;
    SupervisorFSM_RX_B.CurrentPID = InitConfParams.CurLoopPID;
    SupervisorFSM_RX_B.VelocityPID = InitConfParams.VelLoopPID;
    switch (SupervisorFSM_RX_B.newPIDType) {
     case ControlModes_Current:
      SupervisorFSM_RX_B.CurrentPID.P = SupervisorFSM_RX_B.newPID.Kp;
      SupervisorFSM_RX_B.CurrentPID.I = SupervisorFSM_RX_B.newPID.Ki;
      SupervisorFSM_RX_B.CurrentPID.D = SupervisorFSM_RX_B.newPID.Kd;
      SupervisorFSM_RX_B.CurrentPID.shift_factor = SupervisorFSM_RX_B.newPID.Ks;
      break;

     case ControlModes_Position:
      SupervisorFSM_RX_B.PositionPID.P = SupervisorFSM_RX_B.newPID.Kp;
      SupervisorFSM_RX_B.PositionPID.I = SupervisorFSM_RX_B.newPID.Ki;
      SupervisorFSM_RX_B.PositionPID.D = SupervisorFSM_RX_B.newPID.Kd;
      SupervisorFSM_RX_B.PositionPID.shift_factor = SupervisorFSM_RX_B.newPID.Ks;
      break;

     case ControlModes_Voltage:
      SupervisorFSM_RX_B.OpenLoopPID.P = SupervisorFSM_RX_B.newPID.Kp;
      SupervisorFSM_RX_B.OpenLoopPID.I = SupervisorFSM_RX_B.newPID.Ki;
      SupervisorFSM_RX_B.OpenLoopPID.D = SupervisorFSM_RX_B.newPID.Kd;
      SupervisorFSM_RX_B.OpenLoopPID.shift_factor = SupervisorFSM_RX_B.newPID.Ks;
      break;

     case ControlModes_Velocity:
      SupervisorFSM_RX_B.VelocityPID.P = SupervisorFSM_RX_B.newPID.Kp;
      SupervisorFSM_RX_B.VelocityPID.I = SupervisorFSM_RX_B.newPID.Ki;
      SupervisorFSM_RX_B.VelocityPID.D = SupervisorFSM_RX_B.newPID.Kd;
      SupervisorFSM_RX_B.VelocityPID.shift_factor = SupervisorFSM_RX_B.newPID.Ks;
      break;
    }
  } else {
    switch (SupervisorFSM_RX_B.newPIDType) {
     case ControlModes_Current:
      SupervisorFSM_RX_B.CurrentPID.P = SupervisorFSM_RX_B.newPID.Kp;
      SupervisorFSM_RX_B.CurrentPID.I = SupervisorFSM_RX_B.newPID.Ki;
      SupervisorFSM_RX_B.CurrentPID.D = SupervisorFSM_RX_B.newPID.Kd;
      SupervisorFSM_RX_B.CurrentPID.shift_factor = SupervisorFSM_RX_B.newPID.Ks;
      break;

     case ControlModes_Position:
      SupervisorFSM_RX_B.PositionPID.P = SupervisorFSM_RX_B.newPID.Kp;
      SupervisorFSM_RX_B.PositionPID.I = SupervisorFSM_RX_B.newPID.Ki;
      SupervisorFSM_RX_B.PositionPID.D = SupervisorFSM_RX_B.newPID.Kd;
      SupervisorFSM_RX_B.PositionPID.shift_factor = SupervisorFSM_RX_B.newPID.Ks;
      break;

     case ControlModes_Voltage:
      SupervisorFSM_RX_B.OpenLoopPID.P = SupervisorFSM_RX_B.newPID.Kp;
      SupervisorFSM_RX_B.OpenLoopPID.I = SupervisorFSM_RX_B.newPID.Ki;
      SupervisorFSM_RX_B.OpenLoopPID.D = SupervisorFSM_RX_B.newPID.Kd;
      SupervisorFSM_RX_B.OpenLoopPID.shift_factor = SupervisorFSM_RX_B.newPID.Ks;
      break;

     case ControlModes_Velocity:
      SupervisorFSM_RX_B.VelocityPID.P = SupervisorFSM_RX_B.newPID.Kp;
      SupervisorFSM_RX_B.VelocityPID.I = SupervisorFSM_RX_B.newPID.Ki;
      SupervisorFSM_RX_B.VelocityPID.D = SupervisorFSM_RX_B.newPID.Kd;
      SupervisorFSM_RX_B.VelocityPID.shift_factor = SupervisorFSM_RX_B.newPID.Ks;
      break;
    }
  }

  // End of Chart: '<S4>/Chart'
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
static void SupervisorFSM_formatMotorConfig(const BUS_MESSAGES_RX *Selector2)
{
  SupervisorFSM_RX_B.motorConfig.use_index = Selector2->motor_config.use_index;
  SupervisorFSM_RX_B.motorConfig.pole_pairs = static_cast<uint8_T>(rt_roundd_snf
    (static_cast<real_T>(Selector2->motor_config.number_poles) / 2.0));
  SupervisorFSM_RX_B.motorConfig.encoder_tolerance =
    Selector2->motor_config.encoder_tolerance;
  SupervisorFSM_RX_B.motorConfig.rotor_encoder_resolution =
    Selector2->motor_config.rotor_encoder_resolution;
  SupervisorFSM_RX_B.motorConfig.rotor_index_offset =
    Selector2->motor_config.rotor_index_offset;
  SupervisorFSM_RX_B.motorConfig.has_quadrature_encoder =
    Selector2->motor_config.has_quadrature_encoder;
  SupervisorFSM_RX_B.motorConfig.has_hall_sens =
    Selector2->motor_config.has_hall_sens;
  SupervisorFSM_RX_B.motorConfig.has_speed_quadrature_encoder =
    Selector2->motor_config.has_speed_quadrature_encoder;
  SupervisorFSM_RX_B.motorConfig.has_torque_sens =
    Selector2->motor_config.has_torque_sens;
  SupervisorFSM_RX_B.motorConfig.enable_verbosity =
    Selector2->motor_config.enable_verbosity;
}

// System initialize for function-call system: '<Root>/CAN Message Handler'
void Supervis_CANMessageHandler_Init(void)
{
  // SystemInitialize for Chart: '<S1>/CAN Event Dispatcher'
  SupervisorFSM_RX_B.newSetpoint.current = 0.0F;
  SupervisorFSM_RX_B.newSetpoint.voltage = 0.0F;
  SupervisorFSM_RX_B.newSetpoint.velocity = 0.0F;
  SupervisorFSM_RX_B.newLimit.overload = 0.0F;
  SupervisorFSM_RX_B.newLimit.peak = 0.0F;
  SupervisorFSM_RX_B.newLimit.nominal = 0.0F;
  SupervisorFSM_RX_B.newLimit.type = ControlModes_NotConfigured;
  SupervisorFSM_RX_B.newPID.motor = false;
  SupervisorFSM_RX_B.newPID.Kp = 0.0F;
  SupervisorFSM_RX_B.newPID.Ki = 0.0F;
  SupervisorFSM_RX_B.newPID.Kd = 0.0F;
  SupervisorFSM_RX_B.newPID.Ks = 0U;
  SupervisorFSM_RX_B.motorConfig.has_hall_sens = false;
  SupervisorFSM_RX_B.motorConfig.has_quadrature_encoder = false;
  SupervisorFSM_RX_B.motorConfig.has_speed_quadrature_encoder = false;
  SupervisorFSM_RX_B.motorConfig.has_torque_sens = false;
  SupervisorFSM_RX_B.motorConfig.use_index = false;
  SupervisorFSM_RX_B.motorConfig.enable_verbosity = false;
  SupervisorFSM_RX_B.motorConfig.rotor_encoder_resolution = 0;
  SupervisorFSM_RX_B.motorConfig.rotor_index_offset = 0;
  SupervisorFSM_RX_B.motorConfig.encoder_tolerance = 0U;
  SupervisorFSM_RX_B.motorConfig.pole_pairs = 0U;
  SupervisorFSM_RX_B.motorConfig.reduction = 0.0F;
  SupervisorFSM_RX_B.motorConfig.Kbemf = 0.0F;
  SupervisorFSM_RX_B.motorConfig.Rphase = 0.0F;
  SupervisorFSM_RX_B.motorConfig.Imin = 0.0F;
  SupervisorFSM_RX_B.motorConfig.Imax = 0.0F;
  SupervisorFSM_RX_B.motorConfig.Vcc = 0.0F;
  SupervisorFSM_RX_B.motorConfig.Vmax = 0.0F;
  SupervisorFSM_RX_B.estimationConfig.velocity_mode =
    EstimationVelocityModes_Disabled;

  // SystemInitialize for Function Call SubSystem: '<Root>/SetpointHandler'
  Supervisor_SetpointHandler_Init();

  // End of SystemInitialize for SubSystem: '<Root>/SetpointHandler'

  // SystemInitialize for Function Call SubSystem: '<Root>/Limits Handler'
  SupervisorFS_LimitsHandler_Init();

  // End of SystemInitialize for SubSystem: '<Root>/Limits Handler'

  // SystemInitialize for Function Call SubSystem: '<Root>/PID Handler'
  SupervisorFSM_R_PIDHandler_Init();

  // End of SystemInitialize for SubSystem: '<Root>/PID Handler'
}

// Output and update for function-call system: '<Root>/CAN Message Handler'
void SupervisorFSM_CANMessageHandler(void)
{
  BUS_MESSAGES_RX Selector2;

  // Selector: '<S1>/Selector2'
  Selector2 = SupervisorFSM_RX_rtu_MessagesRx->
    messages[SupervisorFSM_RX_B.messageIndex - 1];

  // Chart: '<S1>/CAN Event Dispatcher' incorporates:
  //   Selector: '<S1>/Selector'
  //   Selector: '<S1>/Selector1'
  //   Selector: '<S1>/Selector2'

  if (SupervisorFSM_RX_DW.is_active_c11_SupervisorFSM_RX == 0U) {
    SupervisorFSM_RX_DW.is_active_c11_SupervisorFSM_RX = 1U;
    SupervisorFSM_RX_B.motorConfig = InitConfParams.motorconfig;
    SupervisorFSM_RX_B.estimationConfig = InitConfParams.estimationconfig;
    if (!SupervisorFSM_RX_rtu_ErrorsRx->errors[SupervisorFSM_RX_B.messageIndex -
        1].event) {
      if (SupervisorFSM_RX_rtu_StatusRx->status[SupervisorFSM_RX_B.messageIndex
          - 1].control_mode) {
        SupervisorFSM_RX_B.requiredControlMode = SupervisorFSM_RX_convert
          (SupervisorFSM_RX_rtu_MessagesRx->
           messages[SupervisorFSM_RX_B.messageIndex - 1].control_mode.mode);
        SupervisorFSM_RX_B.receivedNewSetpoint = false;

        // Outputs for Function Call SubSystem: '<Root>/Control Mode Handler Motor 0' 
        Superv_ControlModeHandlerMotor0();

        // End of Outputs for SubSystem: '<Root>/Control Mode Handler Motor 0'
      } else if (SupervisorFSM_RX_rtu_StatusRx->
                 status[SupervisorFSM_RX_B.messageIndex - 1].current_limit) {
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
      } else if (SupervisorFSM_RX_rtu_StatusRx->
                 status[SupervisorFSM_RX_B.messageIndex - 1].current_pid) {
        SupervisorFSM_RX_B.newPIDType = ControlModes_Current;
        SupervisorFSM_RX_B.newPID = SupervisorFSM_RX_rtu_MessagesRx->
          messages[SupervisorFSM_RX_B.messageIndex - 1].pid;

        // Outputs for Function Call SubSystem: '<Root>/PID Handler'
        SupervisorFSM_RX_PIDHandler();

        // End of Outputs for SubSystem: '<Root>/PID Handler'
      } else if (SupervisorFSM_RX_rtu_StatusRx->
                 status[SupervisorFSM_RX_B.messageIndex - 1].velocity_pid) {
        SupervisorFSM_RX_B.newPIDType = ControlModes_Velocity;
        SupervisorFSM_RX_B.newPID = SupervisorFSM_RX_rtu_MessagesRx->
          messages[SupervisorFSM_RX_B.messageIndex - 1].pid;

        // Outputs for Function Call SubSystem: '<Root>/PID Handler'
        SupervisorFSM_RX_PIDHandler();

        // End of Outputs for SubSystem: '<Root>/PID Handler'
      } else if (SupervisorFSM_RX_rtu_StatusRx->
                 status[SupervisorFSM_RX_B.messageIndex - 1].motor_config) {
        SupervisorFSM_formatMotorConfig(&Selector2);
      } else if (SupervisorFSM_RX_rtu_StatusRx->
                 status[SupervisorFSM_RX_B.messageIndex - 1].desired_targets) {
        SupervisorFSM_RX_B.newSetpoint =
          SupervisorFSM_RX_rtu_MessagesRx->
          messages[SupervisorFSM_RX_B.messageIndex - 1].desired_targets;
        SupervisorFSM_RX_B.enableSendingMsgStatus = true;
        SupervisorFSM_RX_B.receivedNewSetpoint = true;

        // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
        SupervisorFSM_R_SetpointHandler();

        // End of Outputs for SubSystem: '<Root>/SetpointHandler'
      }
    }
  } else {
    SupervisorFSM_RX_B.enableSendingMsgStatus =
      ((SupervisorFSM_RX_B.messageIndex != 1) &&
       SupervisorFSM_RX_B.enableSendingMsgStatus);
    if (!SupervisorFSM_RX_rtu_ErrorsRx->errors[SupervisorFSM_RX_B.messageIndex -
        1].event) {
      if (SupervisorFSM_RX_rtu_StatusRx->status[SupervisorFSM_RX_B.messageIndex
          - 1].control_mode) {
        SupervisorFSM_RX_B.requiredControlMode = SupervisorFSM_RX_convert
          (SupervisorFSM_RX_rtu_MessagesRx->
           messages[SupervisorFSM_RX_B.messageIndex - 1].control_mode.mode);
        SupervisorFSM_RX_B.receivedNewSetpoint = false;

        // Outputs for Function Call SubSystem: '<Root>/Control Mode Handler Motor 0' 
        Superv_ControlModeHandlerMotor0();

        // End of Outputs for SubSystem: '<Root>/Control Mode Handler Motor 0'
      } else if (SupervisorFSM_RX_rtu_StatusRx->
                 status[SupervisorFSM_RX_B.messageIndex - 1].current_limit) {
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
      } else if (SupervisorFSM_RX_rtu_StatusRx->
                 status[SupervisorFSM_RX_B.messageIndex - 1].current_pid) {
        SupervisorFSM_RX_B.newPIDType = ControlModes_Current;
        SupervisorFSM_RX_B.newPID = SupervisorFSM_RX_rtu_MessagesRx->
          messages[SupervisorFSM_RX_B.messageIndex - 1].pid;

        // Outputs for Function Call SubSystem: '<Root>/PID Handler'
        SupervisorFSM_RX_PIDHandler();

        // End of Outputs for SubSystem: '<Root>/PID Handler'
      } else if (SupervisorFSM_RX_rtu_StatusRx->
                 status[SupervisorFSM_RX_B.messageIndex - 1].velocity_pid) {
        SupervisorFSM_RX_B.newPIDType = ControlModes_Velocity;
        SupervisorFSM_RX_B.newPID = SupervisorFSM_RX_rtu_MessagesRx->
          messages[SupervisorFSM_RX_B.messageIndex - 1].pid;

        // Outputs for Function Call SubSystem: '<Root>/PID Handler'
        SupervisorFSM_RX_PIDHandler();

        // End of Outputs for SubSystem: '<Root>/PID Handler'
      } else if (SupervisorFSM_RX_rtu_StatusRx->
                 status[SupervisorFSM_RX_B.messageIndex - 1].motor_config) {
        SupervisorFSM_formatMotorConfig(&Selector2);
      } else if (SupervisorFSM_RX_rtu_StatusRx->
                 status[SupervisorFSM_RX_B.messageIndex - 1].desired_targets) {
        SupervisorFSM_RX_B.newSetpoint =
          SupervisorFSM_RX_rtu_MessagesRx->
          messages[SupervisorFSM_RX_B.messageIndex - 1].desired_targets;
        SupervisorFSM_RX_B.enableSendingMsgStatus = true;
        SupervisorFSM_RX_B.receivedNewSetpoint = true;

        // Outputs for Function Call SubSystem: '<Root>/SetpointHandler'
        SupervisorFSM_R_SetpointHandler();

        // End of Outputs for SubSystem: '<Root>/SetpointHandler'
      }
    }
  }

  // End of Chart: '<S1>/CAN Event Dispatcher'
}

// Function for Chart: '<Root>/SupervisorRX State Handler'
static boolean_T SupervisorFSM_isBoardConfigured(void)
{
  return true;
}

// System initialize for referenced model: 'SupervisorFSM_RX'
void SupervisorFSM_RX_Init(Flags *rty_Flags, ConfigurationParameters
  *rty_ConfigurationParameters)
{
  // SystemInitialize for Chart: '<Root>/SupervisorRX State Handler' incorporates:
  //   SubSystem: '<Root>/CAN Message Handler'

  Supervis_CANMessageHandler_Init();

  // SystemInitialize for BusCreator: '<Root>/Bus Creator'
  rty_Flags->control_mode = SupervisorFSM_RX_B.controlModeDefined;
  rty_Flags->enable_sending_msg_status =
    SupervisorFSM_RX_B.enableSendingMsgStatus;
  rty_Flags->fault_button = false;

  // SystemInitialize for BusCreator: '<Root>/Bus Creator1'
  rty_ConfigurationParameters->motorconfig = SupervisorFSM_RX_B.motorConfig;
  rty_ConfigurationParameters->estimationconfig =
    SupervisorFSM_RX_B.estimationConfig;
  rty_ConfigurationParameters->CurLoopPID = SupervisorFSM_RX_B.CurrentPID;
  rty_ConfigurationParameters->PosLoopPID = SupervisorFSM_RX_B.PositionPID;
  rty_ConfigurationParameters->VelLoopPID = SupervisorFSM_RX_B.VelocityPID;
  rty_ConfigurationParameters->DirLoopPID = SupervisorFSM_RX_B.OpenLoopPID;
  rty_ConfigurationParameters->thresholds = SupervisorFSM_RX_B.thresholds;
}

// Output and update for referenced model: 'SupervisorFSM_RX'
void SupervisorFSM_RX(const SensorsData *rtu_SensorsData, const ExternalFlags
                      *rtu_ExternalFlags, const ControlOutputs
                      *rtu_ControlOutputs, const BUS_MESSAGES_RX_MULTIPLE
                      *rtu_MessagesRx, const BUS_STATUS_RX_MULTIPLE
                      *rtu_StatusRx, const BUS_CAN_RX_ERRORS_MULTIPLE
                      *rtu_ErrorsRx, Flags *rty_Flags, Targets *rty_Targets,
                      ConfigurationParameters *rty_ConfigurationParameters)
{
  real32_T rtb_UnitDelay_thresholds_motorO;
  SupervisorFSM_R_rtu_SensorsData = rtu_SensorsData;
  SupervisorFS_rtu_ControlOutputs = rtu_ControlOutputs;
  SupervisorFSM_RX_rtu_MessagesRx = rtu_MessagesRx;
  SupervisorFSM_RX_rtu_StatusRx = rtu_StatusRx;
  SupervisorFSM_RX_rtu_ErrorsRx = rtu_ErrorsRx;

  // UnitDelay: '<Root>/Unit Delay'
  rtb_UnitDelay_thresholds_motorO =
    SupervisorFSM_RX_DW.UnitDelay_DSTATE.thresholds.motorOverloadCurrents;

  // Chart: '<Root>/SupervisorRX State Handler' incorporates:
  //   UnitDelay: '<Root>/Unit Delay'

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
    SupervisorFSM_RX_DW.is_active_STATE_HANDLER = 1U;
    SupervisorFSM_RX_DW.is_STATE_HANDLER = SupervisorFS_IN_NotConfigured_m;
    SupervisorFSM_RX_B.BoardSt = BoardState_NotConfigured;
    SupervisorFSM_RX_DW.is_active_FAULT_HANDLER = 1U;
    SupervisorFSM_RX_DW.is_active_FaultButtonPressed = 1U;
    SupervisorFSM_RX_DW.is_FaultButtonPressed = SupervisorFSM_RX_IN_NoFault;
    SupervisorFSM_RX_B.isFaultButtonPressed = false;
    SupervisorFSM_RX_DW.is_active_OverCurrent = 1U;
    SupervisorFSM_RX_DW.is_OverCurrent = Superviso_IN_LimitNonConfigured;
    SupervisorFSM_RX_DW.is_active_CAN_MESSAGES_FOR_LOOP = 1U;

    // Outputs for Function Call SubSystem: '<Root>/PID Handler'
    SupervisorFSM_RX_PIDHandler();

    // End of Outputs for SubSystem: '<Root>/PID Handler'

    // Outputs for Function Call SubSystem: '<Root>/Limits Handler'
    SupervisorFSM_RX_LimitsHandler();

    // End of Outputs for SubSystem: '<Root>/Limits Handler'
    SupervisorFSM_RX_B.messageIndex = 1;
    while (SupervisorFSM_RX_B.messageIndex < 5) {
      // Outputs for Function Call SubSystem: '<Root>/CAN Message Handler'
      SupervisorFSM_CANMessageHandler();

      // End of Outputs for SubSystem: '<Root>/CAN Message Handler'
      SupervisorFSM_RX_B.messageIndex++;
    }

    SupervisorFSM_RX_DW.is_CAN_MESSAGES_FOR_LOOP = SupervisorFSM_RX_IN_state1;
  } else {
    if (SupervisorFSM_RX_DW.is_active_STATE_HANDLER != 0U) {
      switch (SupervisorFSM_RX_DW.is_STATE_HANDLER) {
       case SupervisorFSM_RX_IN_Configured:
        SupervisorFSM_RX_B.BoardSt = BoardState_Configured;
        break;

       case SupervisorFSM_RX_IN_Fault:
        SupervisorFSM_RX_B.BoardSt = BoardState_Fault;
        break;

       case SupervisorFS_IN_NotConfigured_m:
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
              rtb_UnitDelay_thresholds_motorO) {
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
              rtb_UnitDelay_thresholds_motorO) {
            SupervisorFSM_RX_DW.is_OverCurrent = SupervisorFSM_RX_IN_NoFault;
            SupervisorFSM_RX_B.isInOverCurrent = false;

            // MotorFaultFlags.overCurrent=0;
          }
          break;
        }
      }
    }

    if ((SupervisorFSM_RX_DW.is_active_CAN_MESSAGES_FOR_LOOP != 0U) &&
        (SupervisorFSM_RX_DW.is_CAN_MESSAGES_FOR_LOOP ==
         SupervisorFSM_RX_IN_state1)) {
      SupervisorFSM_RX_B.messageIndex = 1;
      while (SupervisorFSM_RX_B.messageIndex < 5) {
        // Outputs for Function Call SubSystem: '<Root>/CAN Message Handler'
        SupervisorFSM_CANMessageHandler();

        // End of Outputs for SubSystem: '<Root>/CAN Message Handler'
        SupervisorFSM_RX_B.messageIndex++;
      }

      SupervisorFSM_RX_DW.is_CAN_MESSAGES_FOR_LOOP = SupervisorFSM_RX_IN_state1;
    }
  }

  // End of Chart: '<Root>/SupervisorRX State Handler'

  // BusCreator: '<Root>/Bus Creator'
  rty_Flags->control_mode = SupervisorFSM_RX_B.controlModeDefined;
  rty_Flags->enable_sending_msg_status =
    SupervisorFSM_RX_B.enableSendingMsgStatus;
  rty_Flags->fault_button = SupervisorFSM_RX_B.isFaultButtonPressed;

  // BusCreator: '<Root>/Bus Creator1'
  rty_ConfigurationParameters->motorconfig = SupervisorFSM_RX_B.motorConfig;
  rty_ConfigurationParameters->estimationconfig =
    SupervisorFSM_RX_B.estimationConfig;
  rty_ConfigurationParameters->CurLoopPID = SupervisorFSM_RX_B.CurrentPID;
  rty_ConfigurationParameters->PosLoopPID = SupervisorFSM_RX_B.PositionPID;
  rty_ConfigurationParameters->VelLoopPID = SupervisorFSM_RX_B.VelocityPID;
  rty_ConfigurationParameters->DirLoopPID = SupervisorFSM_RX_B.OpenLoopPID;
  rty_ConfigurationParameters->thresholds = SupervisorFSM_RX_B.thresholds;

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

  // Update for UnitDelay: '<Root>/Unit Delay'
  SupervisorFSM_RX_DW.UnitDelay_DSTATE = *rty_ConfigurationParameters;
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
