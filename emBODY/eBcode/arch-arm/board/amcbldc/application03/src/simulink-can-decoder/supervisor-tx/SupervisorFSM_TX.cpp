//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: SupervisorFSM_TX.cpp
//
// Code generated for Simulink model 'SupervisorFSM_TX'.
//
// Model version                  : 2.41
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Tue Aug  3 16:24:53 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "SupervisorFSM_TX.h"
#include "SupervisorFSM_TX_private.h"

// System initialize for referenced model: 'SupervisorFSM_TX'
void SupervisorFSM_TXModelClass::init(real32_T *rty_MessagesTx_foc_current,
  real32_T *rty_MessagesTx_foc_position, real32_T *rty_MessagesTx_foc_velocity,
  boolean_T *rty_EventsTx_foc)
{
  // SystemInitialize for Chart: '<Root>/Chart'
  SupervisorFSM_TX_B.MessagesTx.foc.current = 0.0F;
  SupervisorFSM_TX_B.MessagesTx.foc.position = 0.0F;
  SupervisorFSM_TX_B.MessagesTx.foc.velocity = 0.0F;
  SupervisorFSM_TX_B.EventsTx.foc = false;

  // SystemInitialize for SignalConversion generated from: '<Root>/EventsTx'
  *rty_EventsTx_foc = SupervisorFSM_TX_B.EventsTx.foc;

  // SystemInitialize for SignalConversion generated from: '<Root>/MessagesTx'
  *rty_MessagesTx_foc_current = SupervisorFSM_TX_B.MessagesTx.foc.current;

  // SystemInitialize for SignalConversion generated from: '<Root>/MessagesTx'
  *rty_MessagesTx_foc_position = SupervisorFSM_TX_B.MessagesTx.foc.position;

  // SystemInitialize for SignalConversion generated from: '<Root>/MessagesTx'
  *rty_MessagesTx_foc_velocity = SupervisorFSM_TX_B.MessagesTx.foc.velocity;
}

// Output and update for referenced model: 'SupervisorFSM_TX'
void SupervisorFSM_TXModelClass::step(const real32_T
  *rtu_SensorsData_jointpositions_, const real32_T
  *rtu_SensorsData_motorsensors_om, const real32_T
  *rtu_SensorsData_motorsensors_cu, const MCControlModes
  *rtu_MessagesRx_control_mode_mod, real32_T *rty_MessagesTx_foc_current,
  real32_T *rty_MessagesTx_foc_position, real32_T *rty_MessagesTx_foc_velocity,
  boolean_T *rty_EventsTx_foc)
{
  real32_T rtb_jointpositions_position;
  real32_T rtb_motorsensors_current;
  real32_T rtb_motorsensors_omega;

  // BusCreator generated from: '<Root>/Chart'
  rtb_jointpositions_position = *rtu_SensorsData_jointpositions_;

  // BusCreator generated from: '<Root>/Chart'
  rtb_motorsensors_omega = *rtu_SensorsData_motorsensors_om;
  rtb_motorsensors_current = *rtu_SensorsData_motorsensors_cu;

  // Chart: '<Root>/Chart' incorporates:
  //   BusCreator generated from: '<Root>/Chart'

  if (SupervisorFSM_TX_DW.is_active_c3_SupervisorFSM_TX == 0U) {
    SupervisorFSM_TX_DW.is_active_c3_SupervisorFSM_TX = 1U;
  } else if (*rtu_MessagesRx_control_mode_mod != MCControlModes_Idle) {
    SupervisorFSM_TX_B.MessagesTx.foc.current = rtb_motorsensors_current;
    SupervisorFSM_TX_B.MessagesTx.foc.velocity = rtb_motorsensors_omega;
    SupervisorFSM_TX_B.MessagesTx.foc.position = rtb_jointpositions_position;
    SupervisorFSM_TX_B.EventsTx.foc = true;
  } else {
    SupervisorFSM_TX_B.EventsTx.foc = false;
  }

  // End of Chart: '<Root>/Chart'

  // SignalConversion generated from: '<Root>/EventsTx'
  *rty_EventsTx_foc = SupervisorFSM_TX_B.EventsTx.foc;

  // SignalConversion generated from: '<Root>/MessagesTx'
  *rty_MessagesTx_foc_current = SupervisorFSM_TX_B.MessagesTx.foc.current;

  // SignalConversion generated from: '<Root>/MessagesTx'
  *rty_MessagesTx_foc_position = SupervisorFSM_TX_B.MessagesTx.foc.position;

  // SignalConversion generated from: '<Root>/MessagesTx'
  *rty_MessagesTx_foc_velocity = SupervisorFSM_TX_B.MessagesTx.foc.velocity;
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
