//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_RX_private.h
//
// Code generated for Simulink model 'SupervisorFSM_RX'.
//
// Model version                  : 7.5
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Tue Feb 13 11:54:08 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_SupervisorFSM_RX_private_h_
#define RTW_HEADER_SupervisorFSM_RX_private_h_
#include "rtwtypes.h"
#include "SupervisorFSM_RX_types.h"

// Block signals for model 'SupervisorFSM_RX'
struct B_SupervisorFSM_RX_c_T {
  Thresholds thresholds;               // '<S3>/Chart'
  MotorConfig motorConfig;             // '<S1>/CAN Event Dispatcher'
  PIDConfig CurrentPID;                // '<S4>/Chart'
  PIDConfig VelocityPID;               // '<S4>/Chart'
  PIDConfig PositionPID;               // '<S4>/Chart'
  PIDConfig OpenLoopPID;               // '<S4>/Chart'
  Targets targets;                     // '<S5>/Chart1'
  BUS_MSG_PID newPID;                  // '<S1>/CAN Event Dispatcher'
  SV_Limits newLimit;                  // '<S1>/CAN Event Dispatcher'
  BUS_MSG_DESIRED_TARGETS newSetpoint; // '<S1>/CAN Event Dispatcher'
  EstimationConfig estimationConfig;   // '<S1>/CAN Event Dispatcher'
  real32_T newSetpoint_g;              // '<S2>/ControlMode_SM_motor0'
  int32_T messageIndex;                // '<Root>/SupervisorRX State Handler'
  ControlModes requiredControlMode;    // '<S1>/CAN Event Dispatcher'
  ControlModes newPIDType;             // '<S1>/CAN Event Dispatcher'
  ControlModes controlModeDefined;     // '<S2>/ControlMode_SM_motor0'
  BoardState BoardSt;                  // '<Root>/SupervisorRX State Handler'
  boolean_T isInOverCurrent;           // '<Root>/SupervisorRX State Handler'
  boolean_T isFaultButtonPressed;      // '<Root>/SupervisorRX State Handler'
  boolean_T enableSendingMsgStatus;    // '<S1>/CAN Event Dispatcher'
  boolean_T receivedNewSetpoint;       // '<S1>/CAN Event Dispatcher'
  boolean_T areLimitsSet;              // '<S3>/Chart'
  boolean_T hasSetpointChanged;        // '<S5>/Chart1'
};

// Block states (default storage) for model 'SupervisorFSM_RX'
struct DW_SupervisorFSM_RX_f_T {
  ConfigurationParameters UnitDelay_DSTATE;// '<Root>/Unit Delay'
  uint8_T is_active_c2_SupervisorFSM_RX;// '<Root>/SupervisorRX State Handler'
  uint8_T is_active_STATE_HANDLER;     // '<Root>/SupervisorRX State Handler'
  uint8_T is_STATE_HANDLER;            // '<Root>/SupervisorRX State Handler'
  uint8_T is_active_FAULT_HANDLER;     // '<Root>/SupervisorRX State Handler'
  uint8_T is_active_FaultButtonPressed;// '<Root>/SupervisorRX State Handler'
  uint8_T is_FaultButtonPressed;       // '<Root>/SupervisorRX State Handler'
  uint8_T is_active_OverCurrent;       // '<Root>/SupervisorRX State Handler'
  uint8_T is_OverCurrent;              // '<Root>/SupervisorRX State Handler'
  uint8_T is_active_CAN_MESSAGES_FOR_LOOP;// '<Root>/SupervisorRX State Handler' 
  uint8_T is_CAN_MESSAGES_FOR_LOOP;    // '<Root>/SupervisorRX State Handler'
  uint8_T is_active_c11_SupervisorFSM_RX;// '<S1>/CAN Event Dispatcher'
  uint8_T is_active_c3_SupervisorFSM_RX;// '<S4>/Chart'
  uint8_T is_active_c14_SupervisorFSM_RX;// '<S3>/Chart'
  uint8_T is_active_c12_SupervisorFSM_RX;// '<S2>/ControlMode_SM_motor0'
  uint8_T is_c12_SupervisorFSM_RX;     // '<S2>/ControlMode_SM_motor0'
  uint8_T is_active_c4_SupervisorFSM_RX;// '<S5>/Chart1'
  boolean_T ExternalFlags_fault_button_prev;// '<Root>/SupervisorRX State Handler' 
  boolean_T ExternalFlags_fault_button_start;// '<Root>/SupervisorRX State Handler' 
};

// Invariant block signals for model 'SupervisorFSM_RX'
struct ConstB_SupervisorFSM_RX_h_T {
  boolean_T Constant5;                 // '<Root>/Constant5'
};

// Real-time Model Data Structure
struct tag_RTM_SupervisorFSM_RX_T {
  const char_T **errorStatus;
};

// Macros for accessing real-time model data structure
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         (*((rtm)->errorStatus))
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    (*((rtm)->errorStatus) = (val))
#endif

#ifndef rtmGetErrorStatusPointer
#define rtmGetErrorStatusPointer(rtm)  (rtm)->errorStatus
#endif

#ifndef rtmSetErrorStatusPointer
#define rtmSetErrorStatusPointer(rtm, val) ((rtm)->errorStatus = (val))
#endif

struct MdlrefDW_SupervisorFSM_RX_T {
  RT_MODEL_SupervisorFSM_RX_T rtm;
};

extern void SupervisorFSM_RX_SetpointHandler(void);
extern void SupervisorFSM_RX_ControlModeHandlerMotor0(void);
extern void SupervisorFSM_RX_LimitsHandler(void);
extern void SupervisorFSM_RX_PIDHandler(void);
extern void SupervisorFSM_RX_CANMessageHandler(void);

// Invariant block signals (default storage)
extern const ConstB_SupervisorFSM_RX_h_T SupervisorFSM_RX_ConstB;
extern MdlrefDW_SupervisorFSM_RX_T SupervisorFSM_RX_MdlrefDW;

// Block signals (default storage)
extern B_SupervisorFSM_RX_c_T SupervisorFSM_RX_B;

// Block states (default storage)
extern DW_SupervisorFSM_RX_f_T SupervisorFSM_RX_DW;

// Declare global variables for system: model 'SupervisorFSM_RX'
extern const SensorsData *SupervisorFSM_RX_rtu_SensorsData;// '<Root>/SensorsData' 
extern const ControlOutputs *SupervisorFSM_RX_rtu_ControlOutputs;// '<Root>/ControlOutputs' 
extern const BUS_MESSAGES_RX_MULTIPLE *SupervisorFSM_RX_rtu_MessagesRx;// '<Root>/MessagesRx' 
extern const BUS_STATUS_RX_MULTIPLE *SupervisorFSM_RX_rtu_StatusRx;// '<Root>/StatusRx' 
extern const BUS_CAN_RX_ERRORS_MULTIPLE *SupervisorFSM_RX_rtu_ErrorsRx;// '<Root>/ErrorsRx' 

#endif                                // RTW_HEADER_SupervisorFSM_RX_private_h_

//
// File trailer for generated code.
//
// [EOF]
//
