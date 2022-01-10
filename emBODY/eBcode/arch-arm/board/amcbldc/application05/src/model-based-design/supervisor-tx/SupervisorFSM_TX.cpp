//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_TX.cpp
//
// Code generated for Simulink model 'SupervisorFSM_TX'.
//
// Model version                  : 3.15
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Mon Jan 10 17:04:36 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "SupervisorFSM_TX.h"
#include "SupervisorFSM_TX_private.h"

namespace amc_bldc_codegen
{
  // System initialize for referenced model: 'SupervisorFSM_TX'
  void SupervisorFSM_TX::init(BUS_MESSAGES_TX *arg_MessagesTx, BUS_EVENTS_TX
    *arg_EventsTx) const
  {
    // SystemInitialize for Chart: '<Root>/SupervisorFSM_TX'
    arg_MessagesTx->foc.current = 0.0F;
    arg_MessagesTx->foc.position = 0.0F;
    arg_MessagesTx->foc.velocity = 0.0F;

    // SystemInitialize for BusCreator: '<Root>/BUS_EVENTS_TX'
    arg_EventsTx->foc = SupervisorFSM_TX_B.ev_foc;
  }

  // Output and update for referenced model: 'SupervisorFSM_TX'
  void SupervisorFSM_TX::step(const BUS_MESSAGES_RX &arg_MessagesRx, const
    SensorsData &arg_SensorsData, const EstimatedData &arg_EstimatedData, const
    ControlOutputs &arg_Input, BUS_MESSAGES_TX &arg_MessagesTx, BUS_EVENTS_TX &
    arg_EventsTx, ConfigurationParameters *arg_ControlOutputs1)
  {
    // Chart: '<Root>/SupervisorFSM_TX'
    if (SupervisorFSM_TX_DW.is_active_c3_SupervisorFSM_TX == 0U) {
      SupervisorFSM_TX_DW.is_active_c3_SupervisorFSM_TX = 1U;
    } else if (arg_MessagesRx.control_mode.mode != MCControlModes_Idle) {
      arg_MessagesTx.foc.current = arg_Input.Iq_fbk.current;
      arg_MessagesTx.foc.velocity = arg_EstimatedData.jointvelocities.velocity;
      arg_MessagesTx.foc.position = arg_SensorsData.jointpositions.position;
      SupervisorFSM_TX_DW.ev_focEventCounter++;
    }

    if (SupervisorFSM_TX_DW.ev_focEventCounter > 0U) {
      SupervisorFSM_TX_B.ev_foc = !SupervisorFSM_TX_B.ev_foc;
      SupervisorFSM_TX_DW.ev_focEventCounter--;
    }

    // End of Chart: '<Root>/SupervisorFSM_TX'

    // BusCreator: '<Root>/BUS_EVENTS_TX'
    arg_EventsTx.foc = SupervisorFSM_TX_B.ev_foc;
  }

  // Constructor
  SupervisorFSM_TX::SupervisorFSM_TX() :
    SupervisorFSM_TX_B(),
    SupervisorFSM_TX_DW(),
    SupervisorFSM_TX_M()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  SupervisorFSM_TX::~SupervisorFSM_TX()
  {
    // Currently there is no destructor body generated.
  }

  // Real-Time Model get method
  amc_bldc_codegen::SupervisorFSM_TX::RT_MODEL_SupervisorFSM_TX_T
    * SupervisorFSM_TX::getRTM()
  {
    return (&SupervisorFSM_TX_M);
  }

  // member function to setup error status pointer
  void SupervisorFSM_TX::setErrorStatusPointer(const char_T **rt_errorStatus)
  {
    rtmSetErrorStatusPointer((&SupervisorFSM_TX_M), rt_errorStatus);
  }
}

//
// File trailer for generated code.
//
// [EOF]
//
