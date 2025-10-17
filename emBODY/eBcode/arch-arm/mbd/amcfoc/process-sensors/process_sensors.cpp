//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: process_sensors.cpp
//
// Code generated for Simulink model 'process_sensors'.
//
// Model version                  : 2.0
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Thu Oct 16 10:36:34 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "process_sensors.h"
#include "process_sensors_types.h"
#include <cmath>
#include "rt_remf.h"
#include "rtwtypes.h"
#include "zero_crossing_types.h"

// Output and update for referenced model: 'process_sensors'
void process_sensors(const ActuatorConfiguration *rtu_ActuatorConfig, const
                     Flags *rtu_Flags, const SensorsData *rtu_SensorData,
                     SensorsData *rty_SensorDataDecoded, B_process_sensors_c_T
                     *localB, DW_process_sensors_f_T *localDW,
                     ZCE_process_sensors_T *localZCE)
{
  real32_T rtb_Rem;

  // SignalConversion generated from: '<Root>/Bus Assignment'
  *rty_SensorDataDecoded = *rtu_SensorData;

  // Outputs for Triggered SubSystem: '<Root>/Sample and Hold' incorporates:
  //   TriggerPort: '<S2>/Trigger'

  // UnitDelay: '<Root>/Unit Delay1'
  if (localDW->UnitDelay1_DSTATE && (localZCE->SampleandHold_Trig_ZCE !=
       POS_ZCSIG)) {
    // SignalConversion generated from: '<S2>/In'
    localB->In_b = rtu_SensorData->motorsensors.qencoder.Idx_counter;
  }

  localZCE->SampleandHold_Trig_ZCE = localDW->UnitDelay1_DSTATE;

  // End of Outputs for SubSystem: '<Root>/Sample and Hold'

  // Outputs for Triggered SubSystem: '<Root>/Sample and Hold1' incorporates:
  //   TriggerPort: '<S3>/Trigger'

  if (localDW->UnitDelay1_DSTATE && (localZCE->SampleandHold1_Trig_ZCE !=
       POS_ZCSIG)) {
    // SignalConversion generated from: '<S3>/In' incorporates:
    //   Constant: '<Root>/One'

    localB->In = 1.0F;
  }

  // Outputs for Triggered SubSystem: '<Root>/Sample and Hold' incorporates:
  //   TriggerPort: '<S2>/Trigger'

  localZCE->SampleandHold1_Trig_ZCE = localDW->UnitDelay1_DSTATE;

  // End of UnitDelay: '<Root>/Unit Delay1'
  // End of Outputs for SubSystem: '<Root>/Sample and Hold'
  // End of Outputs for SubSystem: '<Root>/Sample and Hold1'

  // Switch: '<Root>/Switch1' incorporates:
  //   Sum: '<Root>/Add'
  //   Sum: '<Root>/Add2'

  if (localB->In > 0.0F) {
    rty_SensorDataDecoded->motorsensors.qencoder.rotor_angle =
      rtu_SensorData->motorsensors.qencoder.counter - localB->In_b;
  } else {
    rty_SensorDataDecoded->motorsensors.qencoder.rotor_angle =
      rtu_SensorData->motorsensors.qencoder.counter -
      rtu_SensorData->motorsensors.qencoder.Idx_counter;
  }

  // If: '<S6>/If' incorporates:
  //   Constant: '<S8>/Constant'
  //   Product: '<S11>/Product'
  //   Sum: '<S8>/Add'
  //   Sum: '<S9>/Add'
  //   Switch: '<Root>/Switch1'
  //   Switch: '<S5>/Switch'

  if (rty_SensorDataDecoded->motorsensors.qencoder.rotor_angle <=
      rtu_ActuatorConfig->motor.externals.rotor_index_offset) {
    // Outputs for IfAction SubSystem: '<S6>/If Action Subsystem' incorporates:
    //   ActionPort: '<S8>/Action Port'

    rtb_Rem = (rty_SensorDataDecoded->motorsensors.qencoder.rotor_angle + 360.0F)
      - static_cast<real32_T>
      (rtu_ActuatorConfig->motor.externals.rotor_index_offset);

    // End of Outputs for SubSystem: '<S6>/If Action Subsystem'
  } else {
    // Outputs for IfAction SubSystem: '<S6>/If Action Subsystem1' incorporates:
    //   ActionPort: '<S9>/Action Port'

    rtb_Rem = rty_SensorDataDecoded->motorsensors.qencoder.rotor_angle -
      static_cast<real32_T>
      (rtu_ActuatorConfig->motor.externals.rotor_index_offset);

    // End of Outputs for SubSystem: '<S6>/If Action Subsystem1'
  }

  rtb_Rem *= static_cast<real32_T>
    (rtu_ActuatorConfig->motor.externals.pole_pairs);

  // End of If: '<S6>/If'

  // BusAssignment: '<Root>/Bus Assignment' incorporates:
  //   Constant: '<Root>/Constant'
  //   Gain: '<S7>/Multiply'
  //   Gain: '<S7>/Multiply1'
  //   Math: '<Root>/Rem'
  //   Rounding: '<S7>/Floor'
  //   Sum: '<S7>/Add'

  rty_SensorDataDecoded->motorsensors.electrical_angle = rt_remf(rtb_Rem - std::
    floor(0.00277777785F * rtb_Rem) * 360.0F, 360.0F);

  // Switch: '<Root>/Switch'
  if (!rtu_ActuatorConfig->motor.externals.has_quadrature_encoder) {
    *rty_SensorDataDecoded = *rtu_SensorData;
  }

  // End of Switch: '<Root>/Switch'

  // Update for UnitDelay: '<Root>/Unit Delay1'
  localDW->UnitDelay1_DSTATE = rtu_Flags->calibration_done;
}

// Model initialize function
void process_sensors_initialize(ZCE_process_sensors_T *localZCE)
{
  localZCE->SampleandHold_Trig_ZCE = POS_ZCSIG;
  localZCE->SampleandHold1_Trig_ZCE = POS_ZCSIG;
}

//
// File trailer for generated code.
//
// [EOF]
//
