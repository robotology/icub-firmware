//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: SupervisorFSM_TX.cpp
//
// Code generated for Simulink model 'SupervisorFSM_TX'.
//
// Model version                  : 3.27
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Mon Jan 31 18:31:42 2022
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
  // Function for Chart: '<Root>/SupervisorFSM_TX'
  MCControlModes SupervisorFSM_TX::SupervisorFSM_TX_convert(ControlModes
    controlmode)
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
}

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
    arg_MessagesTx->status.control_mode = MCControlModes_Idle;
    arg_MessagesTx->status.pwm_fbk = 0.0F;
    arg_MessagesTx->status.flags.dirty = false;
    arg_MessagesTx->status.flags.stuck = false;
    arg_MessagesTx->status.flags.index_broken = false;
    arg_MessagesTx->status.flags.phase_broken = false;
    arg_MessagesTx->status.flags.not_calibrated = 0.0F;
    arg_MessagesTx->status.flags.ExternalFaultAsserted = false;
    arg_MessagesTx->status.flags.UnderVoltageFailure = false;
    arg_MessagesTx->status.flags.OverVoltageFailure = false;
    arg_MessagesTx->status.flags.OverCurrentFailure = false;
    arg_MessagesTx->status.flags.DHESInvalidValue = false;
    arg_MessagesTx->status.flags.AS5045CSumError = false;
    arg_MessagesTx->status.flags.DHESInvalidSequence = false;
    arg_MessagesTx->status.flags.CANInvalidProtocol = false;
    arg_MessagesTx->status.flags.CAN_BufferOverRun = false;
    arg_MessagesTx->status.flags.SetpointExpired = false;
    arg_MessagesTx->status.flags.CAN_TXIsPasv = false;
    arg_MessagesTx->status.flags.CAN_RXIsPasv = false;
    arg_MessagesTx->status.flags.CAN_IsWarnTX = false;
    arg_MessagesTx->status.flags.CAN_IsWarnRX = false;
    arg_MessagesTx->status.flags.OverHeating = false;
    arg_MessagesTx->status.flags.ADCCalFailure = false;
    arg_MessagesTx->status.flags.I2TFailure = false;
    arg_MessagesTx->status.flags.EMUROMFault = false;
    arg_MessagesTx->status.flags.EMUROMCRCFault = false;
    arg_MessagesTx->status.flags.EncoderFault = false;
    arg_MessagesTx->status.flags.FirmwareSPITimingError = false;
    arg_MessagesTx->status.flags.AS5045CalcError = false;
    arg_MessagesTx->status.flags.FirmwarePWMFatalError = false;
    arg_MessagesTx->status.flags.CAN_TXWasPasv = false;
    arg_MessagesTx->status.flags.CAN_RXWasPasv = false;
    arg_MessagesTx->status.flags.CAN_RTRFlagActive = false;
    arg_MessagesTx->status.flags.CAN_WasWarn = false;
    arg_MessagesTx->status.flags.CAN_DLCError = false;
    arg_MessagesTx->status.flags.SiliconRevisionFault = false;
    arg_MessagesTx->status.flags.PositionLimitUpper = false;
    arg_MessagesTx->status.flags.PositionLimitLower = false;

    // SystemInitialize for BusCreator: '<Root>/BUS_EVENTS_TX'
    arg_EventsTx->foc = SupervisorFSM_TX_B.ev_foc;
    arg_EventsTx->status = SupervisorFSM_TX_B.ev_status;
  }

  // Output and update for referenced model: 'SupervisorFSM_TX'
  void SupervisorFSM_TX::step(const Flags &arg_Flags, const SensorsData &
    arg_SensorsData, const EstimatedData &arg_EstimatedData, const
    ControlOutputs &arg_ControlOutputs, const ConfigurationParameters &
    arg_ConfigurationParameters, BUS_MESSAGES_TX &arg_MessagesTx, BUS_EVENTS_TX
    &arg_EventsTx)
  {
    // Chart: '<Root>/SupervisorFSM_TX'
    if (SupervisorFSM_TX_DW.is_active_c3_SupervisorFSM_TX == 0U) {
      SupervisorFSM_TX_DW.is_active_c3_SupervisorFSM_TX = 1U;
    } else {
      if (arg_Flags.control_mode != ControlModes_Idle) {
        arg_MessagesTx.foc.current = arg_ControlOutputs.Iq_fbk.current;
        arg_MessagesTx.foc.velocity = arg_EstimatedData.jointvelocities.velocity;
        arg_MessagesTx.foc.position = arg_SensorsData.jointpositions.position;
        SupervisorFSM_TX_DW.ev_focEventCounter++;
      }

      if (arg_Flags.enable_sending_msg_status) {
        arg_MessagesTx.status.control_mode = SupervisorFSM_TX_convert
          (arg_Flags.control_mode);
        arg_MessagesTx.status.pwm_fbk = arg_ControlOutputs.Vq;
        arg_MessagesTx.status.flags.ExternalFaultAsserted =
          arg_Flags.fault_button;
        SupervisorFSM_TX_DW.ev_statusEventCounter++;
      }
    }

    if (SupervisorFSM_TX_DW.ev_focEventCounter > 0U) {
      SupervisorFSM_TX_B.ev_foc = !SupervisorFSM_TX_B.ev_foc;
      SupervisorFSM_TX_DW.ev_focEventCounter--;
    }

    if (SupervisorFSM_TX_DW.ev_statusEventCounter > 0U) {
      SupervisorFSM_TX_B.ev_status = !SupervisorFSM_TX_B.ev_status;
      SupervisorFSM_TX_DW.ev_statusEventCounter--;
    }

    // End of Chart: '<Root>/SupervisorFSM_TX'

    // BusCreator: '<Root>/BUS_EVENTS_TX'
    arg_EventsTx.foc = SupervisorFSM_TX_B.ev_foc;
    arg_EventsTx.status = SupervisorFSM_TX_B.ev_status;
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
