//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_RX.h
//
// Code generated for Simulink model 'SupervisorFSM_RX'.
//
// Model version                  : 7.7
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Wed Mar 13 10:35:00 2024
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

// Includes for objects with custom storage classes
#include "rtw_defines.h"

// user code (top of header file)
#include "rtw_enable_disable_motors.h"
#include "rtw_motor_config.h"

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern ConfigurationParameters InitConfParams;// Variable: InitConfParams
                                                 //  Referenced by:
                                                 //    '<Root>/Constant4'
                                                 //    '<S1>/CAN Event Dispatcher'
                                                 //    '<S3>/Chart'
                                                 //    '<S4>/Chart'

extern void SupervisorFSM_RX_Init(Flags *rty_Flags, ConfigurationParameters
  *rty_ConfigurationParameters);
extern void SupervisorFSM_RX(const SensorsData *rtu_SensorsData, const
  ExternalFlags *rtu_ExternalFlags, const ControlOutputs *rtu_ControlOutputs,
  const BUS_MESSAGES_RX_MULTIPLE *rtu_MessagesRx, const EstimatedData
  *rtu_EstimatedData, const BUS_STATUS_RX_MULTIPLE *rtu_StatusRx, const
  BUS_CAN_RX_ERRORS_MULTIPLE *rtu_ErrorsRx, Flags *rty_Flags, Targets
  *rty_Targets, ConfigurationParameters *rty_ConfigurationParameters);

// Model reference registration function
extern void SupervisorFSM_RX_initialize(const char_T **rt_errorStatus);

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
