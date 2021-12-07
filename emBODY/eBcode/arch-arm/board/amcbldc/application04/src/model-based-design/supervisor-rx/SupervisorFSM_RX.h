//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_RX.h
//
// Code generated for Simulink model 'SupervisorFSM_RX'.
//
// Model version                  : 3.103
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Tue Dec  7 09:15:45 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_SupervisorFSM_RX_h_
#define RTW_HEADER_SupervisorFSM_RX_h_
#include <cstring>
#include <stddef.h>
#include "rtwtypes.h"
#include "SupervisorFSM_RX_types.h"
#include <stddef.h>

// Class declaration for model SupervisorFSM_RX
namespace amc_bldc_codegen
{
  class SupervisorFSM_RX
  {
    // public data and function members
   public:
    // Block states (default storage) for model 'SupervisorFSM_RX'
    struct DW_SupervisorFSM_RX_T {
      int32_T sfEvent;                 // '<Root>/SupervisorFSM_RX'
      BoardState BoardSt;              // '<Root>/SupervisorFSM_RX'
      uint16_T CurrentLimit;           // '<Root>/SupervisorFSM_RX'
      uint8_T is_active_c2_SupervisorFSM_RX;// '<Root>/SupervisorFSM_RX'
      uint8_T is_active_CAN_RX_HANDLER;// '<Root>/SupervisorFSM_RX'
      uint8_T is_EVENT_DISPATCHER;     // '<Root>/SupervisorFSM_RX'
      uint8_T is_active_EVENT_DISPATCHER;// '<Root>/SupervisorFSM_RX'
      uint8_T is_CONTROL_MODE_HANDLER; // '<Root>/SupervisorFSM_RX'
      uint8_T is_active_CONTROL_MODE_HANDLER;// '<Root>/SupervisorFSM_RX'
      uint8_T is_LIMITS_HANDLER;       // '<Root>/SupervisorFSM_RX'
      uint8_T is_active_LIMITS_HANDLER;// '<Root>/SupervisorFSM_RX'
      uint8_T is_STATE_HANDLER;        // '<Root>/SupervisorFSM_RX'
      uint8_T is_active_STATE_HANDLER; // '<Root>/SupervisorFSM_RX'
      uint8_T is_active_FAULT_HANDLER; // '<Root>/SupervisorFSM_RX'
      uint8_T is_OverCurrent;          // '<Root>/SupervisorFSM_RX'
      uint8_T is_active_OverCurrent;   // '<Root>/SupervisorFSM_RX'
      uint8_T is_FaultBottomPressed;   // '<Root>/SupervisorFSM_RX'
      uint8_T is_active_FaultBottomPressed;// '<Root>/SupervisorFSM_RX'
      boolean_T IsCurrentLimitSet;     // '<Root>/SupervisorFSM_RX'
      boolean_T EventsRx_control_mode_prev;// '<Root>/SupervisorFSM_RX'
      boolean_T EventsRx_control_mode_start;// '<Root>/SupervisorFSM_RX'
      boolean_T ErrorsRx_event_prev;   // '<Root>/SupervisorFSM_RX'
      boolean_T ErrorsRx_event_start;  // '<Root>/SupervisorFSM_RX'
      boolean_T EventsRx_desired_current_prev;// '<Root>/SupervisorFSM_RX'
      boolean_T EventsRx_desired_current_start;// '<Root>/SupervisorFSM_RX'
      boolean_T EventsRx_current_limit_prev;// '<Root>/SupervisorFSM_RX'
      boolean_T EventsRx_current_limit_start;// '<Root>/SupervisorFSM_RX'
    };

    // Real-time Model Data Structure
    struct RT_MODEL_SupervisorFSM_RX_T {
      const char_T **errorStatus;
    };

    // Initial conditions function
    void init(Flags *arg_Flags, Targets *arg_Targets, ConfigurationParameters
              *arg_Output);

    // model step function
    void step(const BUS_EVENTS_RX &arg_EventsRx, const BUS_MESSAGES_RX &
              arg_MessagesRx, const BUS_CAN_RX_ERRORS &arg_ErrorsRx, const
              EstimatedData &arg_EstimatedData, const SensorsData &
              arg_SensorsData, const ControlOutputs &arg_ControlOutputs, Targets
              &arg_Targets, ConfigurationParameters &arg_Output, Flags &
              arg_Flags);

    // Real-Time Model get method
    amc_bldc_codegen::SupervisorFSM_RX::RT_MODEL_SupervisorFSM_RX_T * getRTM();

    //member function to setup error status pointer
    void setErrorStatusPointer(const char_T **rt_errorStatus);

    // Block states
    DW_SupervisorFSM_RX_T SupervisorFSM_RX_DW;

    // Constructor
    SupervisorFSM_RX();

    // Destructor
    ~SupervisorFSM_RX();

    // private data and function members
   private:
    // private member function(s) for subsystem '<Root>/TmpModelReferenceSubsystem'
    boolean_T SupervisorFSM_RX_IsBoardReady(void) const;
    boolean_T SupervisorFS_isConfigurationSet(void) const;
    ControlModes SupervisorFSM_RX_convert(MCControlModes mccontrolmode);
    boolean_T SupervisorFSM_RX_IsNewCtrl_Idle(const BUS_MESSAGES_RX
      *arg_MessagesRx);
    boolean_T SupervisorFS_IsNewCtrl_Velocity(const BUS_MESSAGES_RX
      *arg_MessagesRx);
    boolean_T SupervisorFS_IsNewCtrl_Position(const BUS_MESSAGES_RX
      *arg_MessagesRx);
    boolean_T SupervisorFSM_IsNewCtrl_Voltage(const BUS_MESSAGES_RX
      *arg_MessagesRx);
    boolean_T SupervisorFSM_IsNewCtrl_Current(const BUS_MESSAGES_RX
      *arg_MessagesRx);
    void Supervisor_CONTROL_MODE_HANDLER(const SensorsData *arg_SensorsData,
      const ControlOutputs *arg_ControlOutputs, const BUS_MESSAGES_RX
      *arg_MessagesRx, Flags *arg_Flags, Targets *arg_Targets);
    boolean_T SupervisorFSM_isBoardConfigured(void);

    // Real-Time Model
    RT_MODEL_SupervisorFSM_RX_T SupervisorFSM_RX_M;
  };
}

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
//  '<S1>'   : 'SupervisorFSM_RX/SupervisorFSM_RX'

#endif                                 // RTW_HEADER_SupervisorFSM_RX_h_

//
// File trailer for generated code.
//
// [EOF]
//
