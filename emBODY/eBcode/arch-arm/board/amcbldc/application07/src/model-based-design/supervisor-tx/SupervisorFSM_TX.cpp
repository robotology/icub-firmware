//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_TX.cpp
//
// Code generated for Simulink model 'SupervisorFSM_TX'.
//
// Model version                  : 3.31
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Tue May 24 10:00:43 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "SupervisorFSM_TX.h"
#include "SupervisorFSM_TX_private.h"

// Forward declaration for local functions
static MCControlModes SupervisorFSM_TX_convert(ControlModes controlmode);

// Function for Chart: '<Root>/SupervisorFSM_TX'
static MCControlModes SupervisorFSM_TX_convert(ControlModes controlmode)
{
  MCControlModes mccontrolmode;
  switch (controlmode) {
   case ControlModes_Idle:
    mccontrolmode = MCControlModes_Idle;
    break;

   case ControlModes_Current:
    mccontrolmode = MCControlModes_Current;
    break;

   case ControlModes_Velocity:
    mccontrolmode = MCControlModes_SpeedVoltage;
    break;

   case ControlModes_Voltage:
    mccontrolmode = MCControlModes_OpenLoop;
    break;

   case ControlModes_HwFaultCM:
    mccontrolmode = MCControlModes_HWFault;
    break;

   default:
    mccontrolmode = MCControlModes_Idle;
    break;
  }

  return mccontrolmode;
}

// System initialize for referenced model: 'SupervisorFSM_TX'
void SupervisorFSM_TX_Init(BUS_MESSAGES_TX *rty_MessagesTx, BUS_EVENTS_TX
  *rty_EventsTx, B_SupervisorFSM_TX_c_T *localB)
{
  // SystemInitialize for Chart: '<Root>/SupervisorFSM_TX'
  rty_MessagesTx->foc.current = 0.0F;
  rty_MessagesTx->foc.position = 0.0F;
  rty_MessagesTx->foc.velocity = 0.0F;
  rty_MessagesTx->status.control_mode = MCControlModes_Idle;
  rty_MessagesTx->status.pwm_fbk = 0.0F;
  rty_MessagesTx->status.flags.dirty = false;
  rty_MessagesTx->status.flags.stuck = false;
  rty_MessagesTx->status.flags.index_broken = false;
  rty_MessagesTx->status.flags.phase_broken = false;
  rty_MessagesTx->status.flags.not_calibrated = 0.0F;
  rty_MessagesTx->status.flags.ExternalFaultAsserted = false;
  rty_MessagesTx->status.flags.UnderVoltageFailure = false;
  rty_MessagesTx->status.flags.OverVoltageFailure = false;
  rty_MessagesTx->status.flags.OverCurrentFailure = false;
  rty_MessagesTx->status.flags.DHESInvalidValue = false;
  rty_MessagesTx->status.flags.AS5045CSumError = false;
  rty_MessagesTx->status.flags.DHESInvalidSequence = false;
  rty_MessagesTx->status.flags.CANInvalidProtocol = false;
  rty_MessagesTx->status.flags.CAN_BufferOverRun = false;
  rty_MessagesTx->status.flags.SetpointExpired = false;
  rty_MessagesTx->status.flags.CAN_TXIsPasv = false;
  rty_MessagesTx->status.flags.CAN_RXIsPasv = false;
  rty_MessagesTx->status.flags.CAN_IsWarnTX = false;
  rty_MessagesTx->status.flags.CAN_IsWarnRX = false;
  rty_MessagesTx->status.flags.OverHeating = false;
  rty_MessagesTx->status.flags.ADCCalFailure = false;
  rty_MessagesTx->status.flags.I2TFailure = false;
  rty_MessagesTx->status.flags.EMUROMFault = false;
  rty_MessagesTx->status.flags.EMUROMCRCFault = false;
  rty_MessagesTx->status.flags.EncoderFault = false;
  rty_MessagesTx->status.flags.FirmwareSPITimingError = false;
  rty_MessagesTx->status.flags.AS5045CalcError = false;
  rty_MessagesTx->status.flags.FirmwarePWMFatalError = false;
  rty_MessagesTx->status.flags.CAN_TXWasPasv = false;
  rty_MessagesTx->status.flags.CAN_RXWasPasv = false;
  rty_MessagesTx->status.flags.CAN_RTRFlagActive = false;
  rty_MessagesTx->status.flags.CAN_WasWarn = false;
  rty_MessagesTx->status.flags.CAN_DLCError = false;
  rty_MessagesTx->status.flags.SiliconRevisionFault = false;
  rty_MessagesTx->status.flags.PositionLimitUpper = false;
  rty_MessagesTx->status.flags.PositionLimitLower = false;

  // SystemInitialize for BusCreator: '<Root>/BUS_EVENTS_TX'
  rty_EventsTx->foc = localB->ev_foc;
  rty_EventsTx->status = localB->ev_status;
}

// Output and update for referenced model: 'SupervisorFSM_TX'
void SupervisorFSM_TX(const SensorsData *rtu_SensorsData, const EstimatedData
                      *rtu_EstimatedData, const Flags *rtu_Flags, const
                      ControlOutputs *rtu_ControlOutputs, BUS_MESSAGES_TX
                      *rty_MessagesTx, BUS_EVENTS_TX *rty_EventsTx,
                      B_SupervisorFSM_TX_c_T *localB, DW_SupervisorFSM_TX_f_T
                      *localDW)
{
  // Chart: '<Root>/SupervisorFSM_TX'
  if (localDW->is_active_c3_SupervisorFSM_TX == 0U) {
    localDW->is_active_c3_SupervisorFSM_TX = 1U;
  } else if (rtu_Flags->enable_sending_msg_status) {
    rty_MessagesTx->foc.current = rtu_ControlOutputs->Iq_fbk.current;
    rty_MessagesTx->foc.velocity = rtu_EstimatedData->jointvelocities.velocity;
    rty_MessagesTx->foc.position = rtu_SensorsData->jointpositions.position;
    localDW->ev_focEventCounter++;
    rty_MessagesTx->status.control_mode = SupervisorFSM_TX_convert
      (rtu_Flags->control_mode);
    rty_MessagesTx->status.pwm_fbk = rtu_ControlOutputs->Vq;
    rty_MessagesTx->status.flags.ExternalFaultAsserted = rtu_Flags->fault_button;
    localDW->ev_statusEventCounter++;
  }

  if (localDW->ev_focEventCounter > 0U) {
    localB->ev_foc = !localB->ev_foc;
    localDW->ev_focEventCounter--;
  }

  if (localDW->ev_statusEventCounter > 0U) {
    localB->ev_status = !localB->ev_status;
    localDW->ev_statusEventCounter--;
  }

  // End of Chart: '<Root>/SupervisorFSM_TX'

  // BusCreator: '<Root>/BUS_EVENTS_TX'
  rty_EventsTx->foc = localB->ev_foc;
  rty_EventsTx->status = localB->ev_status;
}

// Model initialize function
void SupervisorFSM_TX_initialize(const char_T **rt_errorStatus,
  RT_MODEL_SupervisorFSM_TX_T *const SupervisorFSM_TX_M)
{
  // Registration code

  // initialize error status
  rtmSetErrorStatusPointer(SupervisorFSM_TX_M, rt_errorStatus);
}

//
// File trailer for generated code.
//
// [EOF]
//
