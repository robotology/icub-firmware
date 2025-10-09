//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: process_sensors.h
//
// Code generated for Simulink model 'process_sensors'.
//
// Model version                  : 2.0
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Thu Oct  9 17:31:31 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef process_sensors_h_
#define process_sensors_h_
#include "rtwtypes.h"
#include "process_sensors_types.h"
#include "zero_crossing_types.h"

// Block signals for model 'process_sensors'
struct B_process_sensors_c_T {
  real32_T In;                         // '<S3>/In'
  real32_T In_b;                       // '<S2>/In'
};

// Block states (default storage) for model 'process_sensors'
struct DW_process_sensors_f_T {
  boolean_T UnitDelay1_DSTATE;         // '<Root>/Unit Delay1'
};

// Zero-crossing (trigger) state for model 'process_sensors'
struct ZCE_process_sensors_T {
  ZCSigState SampleandHold1_Trig_ZCE;  // '<Root>/Sample and Hold1'
  ZCSigState SampleandHold_Trig_ZCE;   // '<Root>/Sample and Hold'
};

struct MdlrefDW_process_sensors_T {
  B_process_sensors_c_T rtb;
  DW_process_sensors_f_T rtdw;
  ZCE_process_sensors_T rtzce;
};

// Model reference registration function
extern void process_sensors_initialize(ZCE_process_sensors_T *localZCE);
extern void process_sensors(const ActuatorConfiguration *rtu_ActuatorConfig,
  const Flags *rtu_Flags, const SensorsData *rtu_SensorData, SensorsData
  *rty_SensorDataDecoded, B_process_sensors_c_T *localB, DW_process_sensors_f_T *
  localDW, ZCE_process_sensors_T *localZCE);

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S5>/Data Type Duplicate' : Unused code path elimination


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
//  '<Root>' : 'process_sensors'
//  '<S1>'   : 'process_sensors/Mechanical to Electrical Position'
//  '<S2>'   : 'process_sensors/Sample and Hold'
//  '<S3>'   : 'process_sensors/Sample and Hold1'
//  '<S4>'   : 'process_sensors/Mechanical to Electrical Position/MechToElec'
//  '<S5>'   : 'process_sensors/Mechanical to Electrical Position/MechToElec/floating-point'
//  '<S6>'   : 'process_sensors/Mechanical to Electrical Position/MechToElec/floating-point/Compensate Offset'
//  '<S7>'   : 'process_sensors/Mechanical to Electrical Position/MechToElec/floating-point/Mech To Elec'
//  '<S8>'   : 'process_sensors/Mechanical to Electrical Position/MechToElec/floating-point/Compensate Offset/If Action Subsystem'
//  '<S9>'   : 'process_sensors/Mechanical to Electrical Position/MechToElec/floating-point/Compensate Offset/If Action Subsystem1'
//  '<S10>'  : 'process_sensors/Mechanical to Electrical Position/MechToElec/floating-point/Mech To Elec/Variant Subsystem'
//  '<S11>'  : 'process_sensors/Mechanical to Electrical Position/MechToElec/floating-point/Mech To Elec/Variant Subsystem/Input Port'

#endif                                 // process_sensors_h_

//
// File trailer for generated code.
//
// [EOF]
//
