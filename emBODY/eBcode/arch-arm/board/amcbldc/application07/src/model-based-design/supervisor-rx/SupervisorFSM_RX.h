//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_RX.h
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
#ifndef RTW_HEADER_SupervisorFSM_RX_h_
#define RTW_HEADER_SupervisorFSM_RX_h_
#include "rtwtypes.h"
#include "SupervisorFSM_RX_types.h"

// user code (top of header file)
#include "rtw_enable_disable_motors.h"

// Block signals for model 'SupervisorFSM_RX'
#ifndef SupervisorFSM_RX_MDLREF_HIDE_CHILD_

struct B_SupervisorFSM_RX_c_T {
  Thresholds thresholds;               // '<S3>/Chart'
  PIDConfig CurrentPID;                // '<S4>/Chart'
  PIDConfig VelocityPID;               // '<S4>/Chart'
  PIDConfig PositionPID;               // '<S4>/Chart'
  PIDConfig OpenLoopPID;               // '<S4>/Chart'
  MotorConfig motorConfig;             // '<S1>/CAN Event Dispatcher'
  Targets targets;                     // '<S5>/Chart1'
  BUS_MSG_PID newPID;                  // '<S1>/CAN Event Dispatcher'
  SV_Limits newLimit;                  // '<S1>/CAN Event Dispatcher'
  BUS_MSG_DESIRED_TARGETS newSetpoint; // '<S1>/CAN Event Dispatcher'
  EstimationConfig estimationConfig;   // '<S1>/CAN Event Dispatcher'
  real32_T newSetpoint_i;              // '<S2>/ControlMode_SM_motor0'
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

#endif                                 //SupervisorFSM_RX_MDLREF_HIDE_CHILD_

// Block states (default storage) for model 'SupervisorFSM_RX'
#ifndef SupervisorFSM_RX_MDLREF_HIDE_CHILD_

struct DW_SupervisorFSM_RX_f_T {
  ConfigurationParameters UnitDelay_DSTATE;// '<Root>/Unit Delay'
  uint8_T is_active_c2_SupervisorFSM_RX;// '<Root>/SupervisorRX State Handler'
  uint8_T is_STATE_HANDLER;            // '<Root>/SupervisorRX State Handler'
  uint8_T is_active_STATE_HANDLER;     // '<Root>/SupervisorRX State Handler'
  uint8_T is_active_FAULT_HANDLER;     // '<Root>/SupervisorRX State Handler'
  uint8_T is_OverCurrent;              // '<Root>/SupervisorRX State Handler'
  uint8_T is_active_OverCurrent;       // '<Root>/SupervisorRX State Handler'
  uint8_T is_FaultButtonPressed;       // '<Root>/SupervisorRX State Handler'
  uint8_T is_active_FaultButtonPressed;// '<Root>/SupervisorRX State Handler'
  uint8_T is_CAN_MESSAGES_FOR_LOOP;    // '<Root>/SupervisorRX State Handler'
  uint8_T is_active_CAN_MESSAGES_FOR_LOOP;// '<Root>/SupervisorRX State Handler' 
  uint8_T is_active_c11_SupervisorFSM_RX;// '<S1>/CAN Event Dispatcher'
  uint8_T is_active_c3_SupervisorFSM_RX;// '<S4>/Chart'
  uint8_T is_active_c14_SupervisorFSM_RX;// '<S3>/Chart'
  uint8_T is_active_c12_SupervisorFSM_RX;// '<S2>/ControlMode_SM_motor0'
  uint8_T is_c12_SupervisorFSM_RX;     // '<S2>/ControlMode_SM_motor0'
  uint8_T is_active_c4_SupervisorFSM_RX;// '<S5>/Chart1'
  boolean_T ExternalFlags_fault_button_prev;// '<Root>/SupervisorRX State Handler' 
  boolean_T ExternalFlags_fault_button_star;// '<Root>/SupervisorRX State Handler' 
};

#endif                                 //SupervisorFSM_RX_MDLREF_HIDE_CHILD_

#ifndef SupervisorFSM_RX_MDLREF_HIDE_CHILD_

// Real-time Model Data Structure
struct tag_RTM_SupervisorFSM_RX_T {
  const char_T **errorStatus;
};

#endif                                 //SupervisorFSM_RX_MDLREF_HIDE_CHILD_

#ifndef SupervisorFSM_RX_MDLREF_HIDE_CHILD_

struct MdlrefDW_SupervisorFSM_RX_T {
  RT_MODEL_SupervisorFSM_RX_T rtm;
};

#endif                                 //SupervisorFSM_RX_MDLREF_HIDE_CHILD_

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern ConfigurationParameters InitConfParams;// Variable: InitConfParams
                                                 //  Referenced by:
                                                 //    '<S1>/CAN Event Dispatcher'
                                                 //    '<S3>/Chart'
                                                 //    '<S4>/Chart'

extern void SupervisorFSM_RX_Init(Flags *rty_Flags, ConfigurationParameters
  *rty_ConfigurationParameters);
extern void SupervisorFSM_RX(const SensorsData *rtu_SensorsData, const
  ExternalFlags *rtu_ExternalFlags, const ControlOutputs *rtu_ControlOutputs,
  const BUS_MESSAGES_RX_MULTIPLE *rtu_MessagesRx, const BUS_STATUS_RX_MULTIPLE
  *rtu_StatusRx, const BUS_CAN_RX_ERRORS_MULTIPLE *rtu_ErrorsRx, Flags
  *rty_Flags, Targets *rty_Targets, ConfigurationParameters
  *rty_ConfigurationParameters);

// Model reference registration function
extern void SupervisorFSM_RX_initialize(const char_T **rt_errorStatus);

#ifndef SupervisorFSM_RX_MDLREF_HIDE_CHILD_

extern void Supervisor_SetpointHandler_Init(void);
extern void SupervisorFSM_R_SetpointHandler(void);
extern void Superv_ControlModeHandlerMotor0(void);
extern void SupervisorFS_LimitsHandler_Init(void);
extern void SupervisorFSM_RX_LimitsHandler(void);
extern void SupervisorFSM_R_PIDHandler_Init(void);
extern void SupervisorFSM_RX_PIDHandler(void);
extern void Supervis_CANMessageHandler_Init(void);
extern void SupervisorFSM_CANMessageHandler(void);

#endif                                 //SupervisorFSM_RX_MDLREF_HIDE_CHILD_

#ifndef SupervisorFSM_RX_MDLREF_HIDE_CHILD_

extern MdlrefDW_SupervisorFSM_RX_T SupervisorFSM_RX_MdlrefDW;

#endif                                 //SupervisorFSM_RX_MDLREF_HIDE_CHILD_

#ifndef SupervisorFSM_RX_MDLREF_HIDE_CHILD_

// Block signals (default storage)
extern B_SupervisorFSM_RX_c_T SupervisorFSM_RX_B;

// Block states (default storage)
extern DW_SupervisorFSM_RX_f_T SupervisorFSM_RX_DW;

#endif                                 //SupervisorFSM_RX_MDLREF_HIDE_CHILD_

//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Use the MATLAB hilite_system command to trace the generated code back
//  to the model.  For example,
//
//  hilite_system('<S3>')    - opens system 3
//  hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'SupervisorFSM_RX'
//  '<S1>'   : 'SupervisorFSM_RX/CAN Message Handler'
//  '<S2>'   : 'SupervisorFSM_RX/Control Mode Handler Motor 0'
//  '<S3>'   : 'SupervisorFSM_RX/Limits Handler'
//  '<S4>'   : 'SupervisorFSM_RX/PID Handler'
//  '<S5>'   : 'SupervisorFSM_RX/SetpointHandler'
//  '<S6>'   : 'SupervisorFSM_RX/SupervisorRX State Handler'
//  '<S7>'   : 'SupervisorFSM_RX/CAN Message Handler/CAN Event Dispatcher'
//  '<S8>'   : 'SupervisorFSM_RX/Control Mode Handler Motor 0/ControlMode_SM_motor0'
//  '<S9>'   : 'SupervisorFSM_RX/Limits Handler/Chart'
//  '<S10>'  : 'SupervisorFSM_RX/PID Handler/Chart'
//  '<S11>'  : 'SupervisorFSM_RX/SetpointHandler/Chart1'

#endif                                 // RTW_HEADER_SupervisorFSM_RX_h_

//
// File trailer for generated code.
//
// [EOF]
//
