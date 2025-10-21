//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_TX.cpp
//
// Code generated for Simulink model 'SupervisorFSM_TX'.
//
// Model version                  : 11.21
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Tue Oct 21 09:20:16 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "SupervisorFSM_TX.h"
#include "SupervisorFSM_TX_types.h"
#include "rtwtypes.h"

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

   case ControlModes_Position:
    mccontrolmode = MCControlModes_Position;
    break;

   default:
    mccontrolmode = MCControlModes_Idle;
    break;
  }

  return mccontrolmode;
}

// System initialize for referenced model: 'SupervisorFSM_TX'
void SupervisorFSM_TX_Init(BUS_MESSAGES_TX *rty_MessagesTx, BUS_STATUS_TX
  *rty_StatusTx)
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

  // SystemInitialize for BusCreator: '<Root>/BUS_STATUS_TX' incorporates:
  //   Chart: '<Root>/SupervisorFSM_TX'

  rty_StatusTx->foc = false;
  rty_StatusTx->status = false;
}

// Output and update for referenced model: 'SupervisorFSM_TX'
void SupervisorFSM_TX(const SensorsData *rtu_SensorsData, const EstimatedData
                      *rtu_Estimates, const Flags *rtu_Flags, const FOCOutputs
                      *rtu_FOCOutputs, const boolean_T
                      *rtu_ExternalFlags_fault_button, BUS_MESSAGES_TX
                      *rty_MessagesTx, BUS_STATUS_TX *rty_StatusTx,
                      DW_SupervisorFSM_TX_f_T *localDW)
{
  boolean_T ev_foc;

  // Chart: '<Root>/SupervisorFSM_TX'
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

  // BusCreator: '<Root>/BUS_STATUS_TX' incorporates:
  //   Chart: '<Root>/SupervisorFSM_TX'

  rty_StatusTx->foc = false;

  // Chart: '<Root>/SupervisorFSM_TX'
  ev_foc = false;
  if (localDW->is_active_c3_SupervisorFSM_TX == 0) {
    localDW->is_active_c3_SupervisorFSM_TX = 1U;
  } else if (rtu_Flags->enable_sending_msg_status) {
    rty_MessagesTx->foc.current = rtu_Estimates->Iq_filtered;
    rty_MessagesTx->foc.velocity = rtu_Estimates->rotor_velocity;
    rty_MessagesTx->foc.position =
      rtu_SensorsData->motorsensors.qencoder.rotor_angle;
    ev_foc = true;
    rty_MessagesTx->status.control_mode = SupervisorFSM_TX_convert
      (rtu_Flags->control_mode);
    rty_MessagesTx->status.pwm_fbk = rtu_FOCOutputs->Vq;
    rty_MessagesTx->status.flags.ExternalFaultAsserted =
      *rtu_ExternalFlags_fault_button;
    rty_MessagesTx->status.flags.OverCurrentFailure =
      rtu_Flags->hw_faults.overcurrent;

    // BusCreator: '<Root>/BUS_STATUS_TX'
    rty_StatusTx->foc = true;
  }

  // BusCreator: '<Root>/BUS_STATUS_TX'
  rty_StatusTx->status = ev_foc;
}

//
// File trailer for generated code.
//
// [EOF]
//
