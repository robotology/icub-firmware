//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: SupervisorFSM_TX.cpp
//
// Code generated for Simulink model 'SupervisorFSM_TX'.
//
// Model version                  : 2.50
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Mon Sep 20 12:43:29 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "SupervisorFSM_TX.h"
#include "SupervisorFSM_TX_private.h"

// System initialize for referenced model: 'SupervisorFSM_TX'
void SupervisorFSM_TXModelClass::init(BUS_MESSAGES_TX *arg_MessagesTx,
  BUS_EVENTS_TX *arg_EventsTx)
{
  // SystemInitialize for Chart: '<Root>/SupervisorFSM_TX'
  arg_MessagesTx->foc.current = 0.0F;
  arg_MessagesTx->foc.position = 0.0F;
  arg_MessagesTx->foc.velocity = 0.0F;

  // SystemInitialize for BusCreator: '<Root>/BUS_EVENTS_TX'
  arg_EventsTx->foc = SupervisorFSM_TX_B.ev_foc;
}

// Output and update for referenced model: 'SupervisorFSM_TX'
void SupervisorFSM_TXModelClass::step(const SensorsData &arg_SensorsData, const
  BUS_MESSAGES_RX &arg_MessagesRx, BUS_MESSAGES_TX &arg_MessagesTx,
  BUS_EVENTS_TX &arg_EventsTx)
{
  // Chart: '<Root>/SupervisorFSM_TX'
  if (SupervisorFSM_TX_DW.is_active_c3_SupervisorFSM_TX == 0U) {
    SupervisorFSM_TX_DW.is_active_c3_SupervisorFSM_TX = 1U;
  } else if (arg_MessagesRx.control_mode.mode != MCControlModes_Idle) {
    arg_MessagesTx.foc.current = arg_SensorsData.motorsensors.current;
    arg_MessagesTx.foc.velocity = arg_SensorsData.motorsensors.omega;
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
SupervisorFSM_TXModelClass::SupervisorFSM_TXModelClass() :
  SupervisorFSM_TX_B(),
  SupervisorFSM_TX_DW()
{
  // Currently there is no constructor body generated.
}

// Destructor
SupervisorFSM_TXModelClass::~SupervisorFSM_TXModelClass()
{
  // Currently there is no destructor body generated.
}

// Real-Time Model get method
SupervisorFSM_TXModelClass::RT_MODEL_SupervisorFSM_TX_T
  * SupervisorFSM_TXModelClass::getRTM()
{
  return (&SupervisorFSM_TX_M);
}

// member function to setup error status pointer
void SupervisorFSM_TXModelClass::setErrorStatusPointer(const char_T
  **rt_errorStatus)
{
  rtmSetErrorStatusPointer((&SupervisorFSM_TX_M), rt_errorStatus);
}

//
// File trailer for generated code.
//
// [EOF]
//
