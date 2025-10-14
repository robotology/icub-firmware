//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: trajectory_planner.cpp
//
// Code generated for Simulink model 'trajectory_planner'.
//
// Model version                  : 2.33
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Tue Oct 14 16:17:51 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "trajectory_planner.h"
#include "trajectory_planner_types.h"
#include <cmath>
#include "rtwtypes.h"
#include "zero_crossing_types.h"

// System initialize for referenced model: 'trajectory_planner'
void trajectory_planner_Init(B_trajectory_planner_c_T *localB,
  DW_trajectory_planner_f_T *localDW)
{
  // SystemInitialize for IfAction SubSystem: '<Root>/If Action Subsystem2'
  // InitializeConditions for DiscreteFilter: '<S4>/Discrete Filter'
  localDW->DiscreteFilter_icLoad = 1U;

  // SystemInitialize for Triggered SubSystem: '<S4>/Compute Coefficients'
  // SystemInitialize for SignalConversion generated from: '<S6>/Den' incorporates:
  //   Outport: '<S6>/Den'

  localB->OutportBufferForDen[0] = 1.0;
  localB->OutportBufferForDen[1] = 0.0;
  localB->OutportBufferForDen[2] = 0.0;
  localB->OutportBufferForDen[3] = 0.0;

  // End of SystemInitialize for SubSystem: '<S4>/Compute Coefficients'
  // End of SystemInitialize for SubSystem: '<Root>/If Action Subsystem2'
}

// Output and update for referenced model: 'trajectory_planner'
void trajectory_planner(const Flags *rtu_Flags, const Targets *rtu_Targets,
  const SensorsData *rtu_SensorData, Targets *rty_Targets_out,
  B_trajectory_planner_c_T *localB, DW_trajectory_planner_f_T *localDW,
  ZCE_trajectory_planner_T *localZCE)
{
  // SwitchCase: '<Root>/Switch Case'
  if (rtu_Flags->control_mode == ControlModes_Position) {
    real_T rtb_UnitDelay;
    real32_T u0;
    uint8_T rtb_FixPtRelationalOperator;
    boolean_T rtb_OR;

    // Outputs for IfAction SubSystem: '<Root>/If Action Subsystem2' incorporates:
    //   ActionPort: '<S1>/Action Port'

    // BusAssignment: '<S1>/Bus Assignment'
    *rty_Targets_out = *rtu_Targets;

    // UnitDelay: '<S4>/Unit Delay'
    rtb_UnitDelay = localDW->UnitDelay_DSTATE;

    // RelationalOperator: '<S8>/FixPt Relational Operator' incorporates:
    //   UnitDelay: '<S4>/Unit Delay'
    //   UnitDelay: '<S8>/Delay Input1'
    //
    //  Block description for '<S8>/Delay Input1':
    //
    //   Store in Global RAM

    rtb_FixPtRelationalOperator = (localDW->UnitDelay_DSTATE !=
      localDW->DelayInput1_DSTATE);

    // Outputs for Triggered SubSystem: '<S4>/Compute Coefficients' incorporates:
    //   TriggerPort: '<S6>/Trigger'

    if ((rtb_FixPtRelationalOperator > 0) &&
        (localZCE->ComputeCoefficients_Trig_ZCE != POS_ZCSIG)) {
      real_T Divide9_tmp;
      real_T Divide9_tmp_0;
      real_T rtb_Divide5;
      real_T rtb_Divide6;
      real_T rtb_Divide8;
      real_T rtb_Fcn4;

      // Math: '<S6>/Square' incorporates:
      //   UnitDelay: '<S4>/Unit Delay'

      rtb_Divide5 = localDW->UnitDelay_DSTATE * localDW->UnitDelay_DSTATE;

      // Product: '<S6>/Divide6' incorporates:
      //   Constant: '<S6>/Constant1'
      //   Constant: '<S6>/Constant7'
      //   Product: '<S6>/Divide'
      //   UnitDelay: '<S4>/Unit Delay'

      rtb_Divide6 = -150.765868956161 / localDW->UnitDelay_DSTATE / rtb_Divide5 *
        1.0E-9;

      // Product: '<S6>/Divide4' incorporates:
      //   Constant: '<S6>/Constant'
      //   Constant: '<S6>/Constant3'
      //   Product: '<S6>/Divide2'
      //   UnitDelay: '<S4>/Unit Delay'

      rtb_Divide8 = -15.9669610709384 / localDW->UnitDelay_DSTATE * 0.004;

      // Product: '<S6>/Divide5' incorporates:
      //   Constant: '<S6>/Constant2'
      //   Constant: '<S6>/Constant6'
      //   Product: '<S6>/Divide1'

      rtb_Divide5 = -84.9812819469538 / rtb_Divide5 * 2.0E-6;

      // Fcn: '<S6>/Fcn4'
      rtb_Fcn4 = ((rtb_Divide8 + rtb_Divide5) + rtb_Divide6) - 8.0;

      // Product: '<S6>/Divide9' incorporates:
      //   Gain: '<S6>/Gain1'

      Divide9_tmp_0 = rtb_Divide6 / rtb_Fcn4;

      // Product: '<S6>/Divide9'
      localB->Divide9[0] = Divide9_tmp_0;

      // Product: '<S6>/Divide9' incorporates:
      //   Gain: '<S6>/Gain1'

      Divide9_tmp = 3.0 * rtb_Divide6 / rtb_Fcn4;

      // Product: '<S6>/Divide9'
      localB->Divide9[1] = Divide9_tmp;
      localB->Divide9[2] = Divide9_tmp;
      localB->Divide9[3] = Divide9_tmp_0;

      // SignalConversion generated from: '<S6>/Den' incorporates:
      //   Constant: '<S6>/Constant4'

      localB->OutportBufferForDen[0] = 1.0;

      // Fcn: '<S6>/Fcn1' incorporates:
      //   Fcn: '<S6>/Fcn2'

      Divide9_tmp_0 = 3.0 * rtb_Divide6;

      // SignalConversion generated from: '<S6>/Den' incorporates:
      //   Fcn: '<S6>/Fcn1'
      //   Fcn: '<S6>/Fcn2'
      //   Fcn: '<S6>/Fcn3'
      //   Product: '<S6>/Divide3'
      //   Product: '<S6>/Divide7'
      //   Product: '<S6>/Divide8'

      localB->OutportBufferForDen[1] = (((-rtb_Divide8 + rtb_Divide5) +
        Divide9_tmp_0) + 24.0) / rtb_Fcn4;
      localB->OutportBufferForDen[2] = (((-rtb_Divide8 - rtb_Divide5) +
        Divide9_tmp_0) - 24.0) / rtb_Fcn4;
      localB->OutportBufferForDen[3] = (((rtb_Divide8 - rtb_Divide5) +
        rtb_Divide6) + 8.0) / rtb_Fcn4;
    }

    localZCE->ComputeCoefficients_Trig_ZCE = (rtb_FixPtRelationalOperator > 0);

    // End of Outputs for SubSystem: '<S4>/Compute Coefficients'

    // Logic: '<S4>/OR' incorporates:
    //   UnitDelay: '<S4>/Unit Delay1'

    rtb_OR = ((rtb_FixPtRelationalOperator != 0) || localDW->UnitDelay1_DSTATE);

    // Outputs for Triggered SubSystem: '<S4>/Compute Init State' incorporates:
    //   TriggerPort: '<S7>/Trigger'

    if (rtb_OR && (localZCE->ComputeInitState_Trig_ZCE != POS_ZCSIG)) {
      // Product: '<S7>/Product2' incorporates:
      //   Constant: '<S7>/Constant'
      //   DataTypeConversion: '<S1>/Cast To Double2'
      //   Product: '<S7>/Product1'
      //   Sum: '<S7>/Sum'
      //   Sum: '<S7>/Sum of Elements'
      //   Sum: '<S7>/Sum of Elements1'
      //   Sum: '<S7>/Sum1'

      localB->Product2 = 1.0 / (((localB->Divide9[1] + localB->Divide9[2]) +
        localB->Divide9[3]) - ((localB->OutportBufferForDen[1] +
        localB->OutportBufferForDen[2]) + localB->OutportBufferForDen[3]) *
        localB->Divide9[0]) * (1.0 - localB->Divide9[0]) *
        rtu_SensorData->motorsensors.qencoder.rotor_angle;
    }

    localZCE->ComputeInitState_Trig_ZCE = rtb_OR;

    // End of Outputs for SubSystem: '<S4>/Compute Init State'

    // DiscreteFilter: '<S4>/Discrete Filter' incorporates:
    //   DataTypeConversion: '<S1>/Cast To Double'

    if (rtb_OR && (localZCE->DiscreteFilter_Reset_ZCE != POS_ZCSIG)) {
      localDW->DiscreteFilter_icLoad = 1U;
    }

    // Outputs for Triggered SubSystem: '<S4>/Compute Init State' incorporates:
    //   TriggerPort: '<S7>/Trigger'

    localZCE->DiscreteFilter_Reset_ZCE = rtb_OR;

    // End of Outputs for SubSystem: '<S4>/Compute Init State'
    if (localDW->DiscreteFilter_icLoad != 0) {
      localDW->DiscreteFilter_states[0] = localB->Product2;
      localDW->DiscreteFilter_states[1] = localB->Product2;
      localDW->DiscreteFilter_states[2] = localB->Product2;
    }

    localDW->DiscreteFilter_tmp = ((rtu_Targets->position -
      localDW->DiscreteFilter_states[0] * localB->OutportBufferForDen[1]) -
      localDW->DiscreteFilter_states[1] * localB->OutportBufferForDen[2]) -
      localDW->DiscreteFilter_states[2] * localB->OutportBufferForDen[3];

    // BusAssignment: '<S1>/Bus Assignment' incorporates:
    //   DataTypeConversion: '<S1>/Cast To Single'
    //   DiscreteFilter: '<S4>/Discrete Filter'

    rty_Targets_out->position = static_cast<real32_T>(((localB->Divide9[0] *
      localDW->DiscreteFilter_tmp + localDW->DiscreteFilter_states[0] *
      localB->Divide9[1]) + localDW->DiscreteFilter_states[1] * localB->Divide9
      [2]) + localDW->DiscreteFilter_states[2] * localB->Divide9[3]);

    // RelationalOperator: '<S3>/FixPt Relational Operator' incorporates:
    //   UnitDelay: '<S3>/Delay Input1'
    //
    //  Block description for '<S3>/Delay Input1':
    //
    //   Store in Global RAM

    localDW->UnitDelay1_DSTATE = (rtu_Targets->position !=
      localDW->DelayInput1_DSTATE_j);

    // Outputs for Triggered SubSystem: '<S5>/Sample and Hold' incorporates:
    //   TriggerPort: '<S9>/Trigger'

    if (localDW->UnitDelay1_DSTATE && (localZCE->SampleandHold_Trig_ZCE !=
         POS_ZCSIG)) {
      // SignalConversion generated from: '<S9>/In'
      localB->In = rtu_SensorData->motorsensors.qencoder.rotor_angle;
    }

    localZCE->SampleandHold_Trig_ZCE = localDW->UnitDelay1_DSTATE;

    // End of Outputs for SubSystem: '<S5>/Sample and Hold'

    // Abs: '<S5>/Abs' incorporates:
    //   Product: '<S5>/Divide'
    //   Sum: '<S5>/Subtract'

    u0 = std::abs((rtu_Targets->position - localB->In) / rtu_Targets->velocity);

    // MinMax: '<S5>/Max'
    if (u0 >= 0.1F) {
      // Update for UnitDelay: '<S4>/Unit Delay'
      localDW->UnitDelay_DSTATE = u0;
    } else {
      // Update for UnitDelay: '<S4>/Unit Delay' incorporates:
      //   DataTypeConversion: '<S1>/Cast To Double1'

      localDW->UnitDelay_DSTATE = 0.10000000149011612;
    }

    // End of MinMax: '<S5>/Max'

    // Update for UnitDelay: '<S8>/Delay Input1'
    //
    //  Block description for '<S8>/Delay Input1':
    //
    //   Store in Global RAM

    localDW->DelayInput1_DSTATE = rtb_UnitDelay;

    // Update for DiscreteFilter: '<S4>/Discrete Filter'
    localDW->DiscreteFilter_icLoad = 0U;
    localDW->DiscreteFilter_states[2] = localDW->DiscreteFilter_states[1];
    localDW->DiscreteFilter_states[1] = localDW->DiscreteFilter_states[0];
    localDW->DiscreteFilter_states[0] = localDW->DiscreteFilter_tmp;

    // Update for UnitDelay: '<S3>/Delay Input1'
    //
    //  Block description for '<S3>/Delay Input1':
    //
    //   Store in Global RAM

    localDW->DelayInput1_DSTATE_j = rtu_Targets->position;

    // End of Outputs for SubSystem: '<Root>/If Action Subsystem2'
  } else {
    // Outputs for IfAction SubSystem: '<Root>/If Action Subsystem3' incorporates:
    //   ActionPort: '<S2>/Action Port'

    // SignalConversion generated from: '<S2>/Targets'
    *rty_Targets_out = *rtu_Targets;

    // End of Outputs for SubSystem: '<Root>/If Action Subsystem3'
  }

  // End of SwitchCase: '<Root>/Switch Case'
}

// Model initialize function
void trajectory_planner_initialize(ZCE_trajectory_planner_T *localZCE)
{
  localZCE->ComputeCoefficients_Trig_ZCE = POS_ZCSIG;
  localZCE->ComputeInitState_Trig_ZCE = POS_ZCSIG;
  localZCE->SampleandHold_Trig_ZCE = POS_ZCSIG;
  localZCE->DiscreteFilter_Reset_ZCE = POS_ZCSIG;
}

//
// File trailer for generated code.
//
// [EOF]
//
