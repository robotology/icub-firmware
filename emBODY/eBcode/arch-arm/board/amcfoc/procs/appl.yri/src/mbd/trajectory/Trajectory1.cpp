//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: Trajectory.cpp
//
// Code generated for Simulink model 'Trajectory'.
//
// Model version                  : 1.211
// Simulink Coder version         : 24.2 (R2024b) 21-Jun-2024
// C/C++ source code generated on : Thu Apr 24 14:56:11 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "Trajectory1.h"
#include <stdbool.h>
#include <stdint.h>
#include "Trajectory_types.h"
#include <cmath>
#include <cstring>

namespace namespace_s1
{
  //
  // Output and update for action system:
  //    '<S24>/If Action Subsystem'
  //    '<S22>/If Action Subsystem'
  //
  void Trajectory::Trajectory_IfActionSubsystem(bool rtu_bVelocityMove, float
    rtu_vTimer, float rtu_vT, float *rty_vn)
  {
    // SignalConversion generated from: '<S30>/vn' incorporates:
    //   Constant: '<S30>/Constant'

    *rty_vn = 0.0F;

    // If: '<S30>/If' incorporates:
    //   Logic: '<S30>/OR'
    //   RelationalOperator: '<S30>/Less Than'

    if (rtu_bVelocityMove || (rtu_vTimer < rtu_vT)) {
      // Outputs for IfAction SubSystem: '<S30>/If Action Subsystem' incorporates:
      //   ActionPort: '<S34>/Action Port'

      // FunctionCaller: '<S34>/Function Caller'
      Trajectory_velocity_stop();

      // End of Outputs for SubSystem: '<S30>/If Action Subsystem'
    }

    // End of If: '<S30>/If'
  }
}

namespace namespace_s1
{
  //
  // Output and update for action system:
  //    '<S24>/If Action Subsystem1'
  //    '<S22>/If Action Subsystem1'
  //
  void Trajectory::Trajectory_IfActionSubsystem1(float *rty_an)
  {
    // SignalConversion generated from: '<S31>/an' incorporates:
    //   Constant: '<S31>/Constant'

    *rty_an = 0.0F;
  }
}

namespace namespace_s1
{
  // Model step function
  bool Trajectory::Trajectory_is_done() const
  {
    // Outputs for Function Call SubSystem: '<Root>/Done'
    // SignalConversion generated from: '<S1>/Done' incorporates:
    //   Constant: '<S17>/Constant'
    //   DataStoreRead: '<S1>/Data Store Read'
    //   RelationalOperator: '<S17>/Compare'
    //   Sum: '<S1>/Add'

    return Trajectory_DW.Trajectory_h.xTimer - Trajectory_DW.Trajectory_h.xT >=
      0.0F;

    // End of Outputs for SubSystem: '<Root>/Done'
  }

  // Model step function
  void Trajectory::Trajectory_config_limits(const float rtu_pos_min, const float
    rtu_pos_max, const float rtu_vel_max, const float rtu_acc_max)
  {
    // Outputs for Function Call SubSystem: '<Root>/config_limits'
    // BusAssignment: '<S3>/Bus Assignment' incorporates:
    //   DataStoreWrite: '<S3>/Data Store Write'
    //   SignalConversion generated from: '<S3>/pos_max'
    //   SignalConversion generated from: '<S3>/pos_min'

    Trajectory_DW.Trajectory_h.pos_min = rtu_pos_min;
    Trajectory_DW.Trajectory_h.pos_max = rtu_pos_max;

    // Switch: '<S3>/Switch' incorporates:
    //   SignalConversion generated from: '<S3>/vel_max'

    if (rtu_vel_max != 0.0F) {
      // BusAssignment: '<S3>/Bus Assignment' incorporates:
      //   DataStoreWrite: '<S3>/Data Store Write'

      Trajectory_DW.Trajectory_h.vel_max = rtu_vel_max;
    }

    // End of Switch: '<S3>/Switch'

    // Switch: '<S3>/Switch1' incorporates:
    //   SignalConversion generated from: '<S3>/acc_max'

    if (rtu_acc_max != 0.0F) {
      // BusAssignment: '<S3>/Bus Assignment' incorporates:
      //   DataStoreWrite: '<S3>/Data Store Write'

      Trajectory_DW.Trajectory_h.acc_max = rtu_acc_max;
    }

    // End of Switch: '<S3>/Switch1'
    // End of Outputs for SubSystem: '<Root>/config_limits'
  }

  // Model step function
  void Trajectory::Trajectory_do_step(float *rty_p, float *rty_v, float *rty_a,
    int32_t *rty_value)
  {
    trajectoryhidParameters rtb_DataStoreRead_p;
    float rtb_a;
    float rtb_p;
    float rtb_v;
    float rtb_vA_k;
    float rtb_vV_d;
    float rtb_vX_p;
    float rtb_xA;
    float rtb_xV;
    float rtb_xX;
    int32_t tmp;

    // Outputs for Function Call SubSystem: '<Root>/do_step'
    // DataStoreRead: '<S4>/Data Store Read'
    rtb_DataStoreRead_p = Trajectory_DW.Trajectory_h;

    // Product: '<S4>/Divide' incorporates:
    //   DataStoreRead: '<S4>/Data Store Read'

    rtb_vV_d = Trajectory_DW.Trajectory_h.xTimer *
      Trajectory_DW.Trajectory_h.xInvT;

    // Sum: '<S4>/Add3' incorporates:
    //   DataStoreRead: '<S4>/Data Store Read'
    //   Product: '<S4>/Divide1'
    //   Product: '<S4>/Divide2'
    //   Product: '<S4>/Divide3'
    //   Sum: '<S4>/Add'
    //   Sum: '<S4>/Add1'

    rtb_xA = ((Trajectory_DW.Trajectory_h.xK3 * rtb_vV_d +
               Trajectory_DW.Trajectory_h.xK2) * rtb_vV_d +
              Trajectory_DW.Trajectory_h.xK1) * rtb_vV_d +
      Trajectory_DW.Trajectory_h.xK0;

    // Switch: '<S4>/Switch' incorporates:
    //   DataStoreRead: '<S4>/Data Store Read'
    //   Gain: '<S4>/Gain'
    //   Sum: '<S4>/Add4'
    //   Sum: '<S4>/Add6'
    //   Switch: '<S4>/Switch1'

    if (Trajectory_DW.Trajectory_h.xTimer - Trajectory_DW.Trajectory_h.xT > 0.0F)
    {
      rtb_xA = 0.0F;
      rtb_xV = 0.0F;
    } else {
      rtb_xV = 0.001F * rtb_xA + Trajectory_DW.Trajectory_h.xV;
    }

    // End of Switch: '<S4>/Switch'

    // Sum: '<S4>/Add2' incorporates:
    //   DataStoreRead: '<S4>/Data Store Read'
    //   Gain: '<S4>/Gain2'

    rtb_xX = 0.001F * rtb_xV + Trajectory_DW.Trajectory_h.xX;

    // Switch: '<S4>/Switch4' incorporates:
    //   Constant: '<S4>/Constant3'
    //   DataStoreRead: '<S4>/Data Store Read'
    //   Sum: '<S4>/Add12'

    if (Trajectory_DW.Trajectory_h.vTimer - Trajectory_DW.Trajectory_h.vT > 0.0F)
    {
      // Switch: '<S4>/Switch2' incorporates:
      //   Constant: '<S4>/Constant2'

      if (Trajectory_DW.Trajectory_h.bVelocityMove) {
        tmp = 2;
      } else {
        tmp = 0;
      }

      // End of Switch: '<S4>/Switch2'
    } else {
      tmp = 1;
    }

    // SwitchCase: '<S4>/Switch Case' incorporates:
    //   Switch: '<S4>/Switch4'

    switch (tmp) {
     case 1:
      // Outputs for IfAction SubSystem: '<S4>/vTimer-vT' incorporates:
      //   ActionPort: '<S21>/Action Port'

      // Product: '<S21>/Divide4' incorporates:
      //   DataStoreRead: '<S4>/Data Store Read'

      rtb_vV_d = Trajectory_DW.Trajectory_h.vTimer *
        Trajectory_DW.Trajectory_h.vInvT;

      // Sum: '<S21>/Add8' incorporates:
      //   DataStoreRead: '<S4>/Data Store Read'
      //   Product: '<S21>/Divide5'
      //   Product: '<S21>/Divide6'
      //   Sum: '<S21>/Add7'

      rtb_vA_k = (Trajectory_DW.Trajectory_h.vK2 * rtb_vV_d +
                  Trajectory_DW.Trajectory_h.vK1) * rtb_vV_d +
        Trajectory_DW.Trajectory_h.vK0;

      // Sum: '<S21>/Add9' incorporates:
      //   DataStoreRead: '<S4>/Data Store Read'
      //   Gain: '<S21>/Gain1'
      //   SignalConversion generated from: '<S4>/Bus Selector'

      rtb_vV_d = 0.001F * rtb_vA_k + Trajectory_DW.Trajectory_h.vV;

      // Sum: '<S21>/Add10' incorporates:
      //   DataStoreRead: '<S4>/Data Store Read'
      //   Gain: '<S21>/Gain3'
      //   SignalConversion generated from: '<S4>/Bus Selector'

      rtb_vX_p = 0.001F * rtb_vV_d + Trajectory_DW.Trajectory_h.vX;

      // Sum: '<S21>/Add'
      rtb_p = rtb_vX_p + rtb_xX;

      // Sum: '<S21>/Add1'
      rtb_v = rtb_vV_d + rtb_xV;

      // Sum: '<S21>/Add2'
      rtb_a = rtb_vA_k + rtb_xA;

      // End of Outputs for SubSystem: '<S4>/vTimer-vT'
      break;

     case 2:
      // Outputs for IfAction SubSystem: '<S4>/bVelocityMove' incorporates:
      //   ActionPort: '<S20>/Action Port'

      // SignalConversion generated from: '<S20>/vAn' incorporates:
      //   Constant: '<S20>/Constant'

      rtb_vA_k = 0.0F;

      // Sum: '<S20>/Add12' incorporates:
      //   DataStoreRead: '<S4>/Data Store Read'
      //   Gain: '<S20>/Gain4'
      //   SignalConversion generated from: '<S4>/Bus Selector'
      //
      rtb_vX_p = 0.001F * Trajectory_DW.Trajectory_h.vV +
        Trajectory_DW.Trajectory_h.vX;

      // Sum: '<S20>/Add'
      rtb_p = rtb_vX_p + rtb_xX;

      // Sum: '<S20>/Add1' incorporates:
      //   DataStoreRead: '<S4>/Data Store Read'
      //   SignalConversion generated from: '<S4>/Bus Selector'

      rtb_v = Trajectory_DW.Trajectory_h.vV + rtb_xV;

      // SignalConversion generated from: '<S20>/vVn' incorporates:
      //   DataStoreRead: '<S4>/Data Store Read'
      //   SignalConversion generated from: '<S4>/Bus Selector'

      rtb_vV_d = Trajectory_DW.Trajectory_h.vV;

      // SignalConversion generated from: '<S20>/xA'
      rtb_a = rtb_xA;

      // End of Outputs for SubSystem: '<S4>/bVelocityMove'
      break;

     default:
      // Outputs for IfAction SubSystem: '<S4>/Switch Case Action Subsystem2' incorporates:
      //   ActionPort: '<S19>/Action Port'

      // SignalConversion generated from: '<S19>/xX'
      rtb_p = rtb_xX;

      // SignalConversion generated from: '<S19>/xV'
      rtb_v = rtb_xV;

      // SignalConversion generated from: '<S19>/xA'
      rtb_a = rtb_xA;

      // SignalConversion generated from: '<S19>/vA' incorporates:
      //   DataStoreRead: '<S4>/Data Store Read'
      //   SignalConversion generated from: '<S4>/Bus Selector'

      rtb_vA_k = Trajectory_DW.Trajectory_h.vA;

      // SignalConversion generated from: '<S19>/vV' incorporates:
      //   DataStoreRead: '<S4>/Data Store Read'
      //   SignalConversion generated from: '<S4>/Bus Selector'

      rtb_vV_d = Trajectory_DW.Trajectory_h.vV;

      // SignalConversion generated from: '<S19>/vX' incorporates:
      //   DataStoreRead: '<S4>/Data Store Read'
      //   SignalConversion generated from: '<S4>/Bus Selector'

      rtb_vX_p = Trajectory_DW.Trajectory_h.vX;

      // End of Outputs for SubSystem: '<S4>/Switch Case Action Subsystem2'
      break;
    }

    // End of SwitchCase: '<S4>/Switch Case'

    // BusAssignment: '<S4>/Bus Assignment' incorporates:
    //   Constant: '<S4>/Constant'
    //   Constant: '<S4>/Constant1'
    //   DataStoreRead: '<S4>/Data Store Read'
    //   DataStoreWrite: '<S4>/Data Store Write'
    //   Sum: '<S4>/Add11'
    //   Sum: '<S4>/Add5'

    Trajectory_DW.Trajectory_h.xTimer += 0.001F;
    Trajectory_DW.Trajectory_h.vTimer += 0.001F;
    Trajectory_DW.Trajectory_h.xA = rtb_xA;
    Trajectory_DW.Trajectory_h.xV = rtb_xV;
    Trajectory_DW.Trajectory_h.xX = rtb_xX;
    Trajectory_DW.Trajectory_h.vX = rtb_vX_p;
    Trajectory_DW.Trajectory_h.vV = rtb_vV_d;
    Trajectory_DW.Trajectory_h.vA = rtb_vA_k;

    // Outputs for Enabled SubSystem: '<S4>/Enabled Subsystem' incorporates:
    //   EnablePort: '<S18>/Enable'

    // RelationalOperator: '<S4>/Equal' incorporates:
    //   SignalConversion generated from: '<S4>/Bus Selector'
    //
    if (rtb_DataStoreRead_p.pos_min != rtb_DataStoreRead_p.pos_max) {
      // If: '<S18>/If'
      if (rtb_p <= rtb_DataStoreRead_p.pos_min) {
        // Outputs for IfAction SubSystem: '<S18>/If Action Subsystem2' incorporates:
        //   ActionPort: '<S24>/Action Port'

        // Merge: '<S18>/Merge' incorporates:
        //   SignalConversion generated from: '<S24>/pos_min'

        Trajectory_B.p = rtb_DataStoreRead_p.pos_min;

        // If: '<S24>/If'
        if (rtb_v < 0.0F) {
          // Outputs for IfAction SubSystem: '<S24>/If Action Subsystem' incorporates:
          //   ActionPort: '<S30>/Action Port'

          Trajectory_IfActionSubsystem(rtb_DataStoreRead_p.bVelocityMove,
            rtb_DataStoreRead_p.vTimer, rtb_DataStoreRead_p.vT, &Trajectory_B.v);

          // End of Outputs for SubSystem: '<S24>/If Action Subsystem'
        } else {
          // Outputs for IfAction SubSystem: '<S24>/If Action Subsystem2' incorporates:
          //   ActionPort: '<S32>/Action Port'

          // Merge: '<S18>/Merge1' incorporates:
          //   SignalConversion generated from: '<S32>/v'

          Trajectory_B.v = rtb_v;

          // End of Outputs for SubSystem: '<S24>/If Action Subsystem2'
        }

        // End of If: '<S24>/If'

        // If: '<S24>/If1'
        if (rtb_a < 0.0F) {
          // Outputs for IfAction SubSystem: '<S24>/If Action Subsystem1' incorporates:
          //   ActionPort: '<S31>/Action Port'

          Trajectory_IfActionSubsystem1(&Trajectory_B.a);

          // End of Outputs for SubSystem: '<S24>/If Action Subsystem1'
        } else {
          // Outputs for IfAction SubSystem: '<S24>/If Action Subsystem3' incorporates:
          //   ActionPort: '<S33>/Action Port'

          // Merge: '<S18>/Merge2' incorporates:
          //   SignalConversion generated from: '<S33>/a'

          Trajectory_B.a = rtb_a;

          // End of Outputs for SubSystem: '<S24>/If Action Subsystem3'
        }

        // End of If: '<S24>/If1'

        // Merge: '<S18>/Merge3' incorporates:
        //   Constant: '<S24>/Zero'
        //   SignalConversion generated from: '<S24>/value'

        Trajectory_B.return_value = -1;

        // End of Outputs for SubSystem: '<S18>/If Action Subsystem2'
      } else if (rtb_p >= rtb_DataStoreRead_p.pos_max) {
        // Outputs for IfAction SubSystem: '<S18>/If Action Subsystem' incorporates:
        //   ActionPort: '<S22>/Action Port'

        // Merge: '<S18>/Merge' incorporates:
        //   SignalConversion generated from: '<S22>/pos_max'

        Trajectory_B.p = rtb_DataStoreRead_p.pos_max;

        // If: '<S22>/If'
        if (rtb_v > 0.0F) {
          // Outputs for IfAction SubSystem: '<S22>/If Action Subsystem' incorporates:
          //   ActionPort: '<S25>/Action Port'

          Trajectory_IfActionSubsystem(rtb_DataStoreRead_p.bVelocityMove,
            rtb_DataStoreRead_p.vTimer, rtb_DataStoreRead_p.vT, &Trajectory_B.v);

          // End of Outputs for SubSystem: '<S22>/If Action Subsystem'
        } else {
          // Outputs for IfAction SubSystem: '<S22>/If Action Subsystem2' incorporates:
          //   ActionPort: '<S27>/Action Port'

          // Merge: '<S18>/Merge1' incorporates:
          //   SignalConversion generated from: '<S27>/v'

          Trajectory_B.v = rtb_v;

          // End of Outputs for SubSystem: '<S22>/If Action Subsystem2'
        }

        // End of If: '<S22>/If'

        // If: '<S22>/If1'
        if (rtb_a > 0.0F) {
          // Outputs for IfAction SubSystem: '<S22>/If Action Subsystem1' incorporates:
          //   ActionPort: '<S26>/Action Port'

          Trajectory_IfActionSubsystem1(&Trajectory_B.a);

          // End of Outputs for SubSystem: '<S22>/If Action Subsystem1'
        } else {
          // Outputs for IfAction SubSystem: '<S22>/If Action Subsystem3' incorporates:
          //   ActionPort: '<S28>/Action Port'

          // Merge: '<S18>/Merge2' incorporates:
          //   SignalConversion generated from: '<S28>/a'

          Trajectory_B.a = rtb_a;

          // End of Outputs for SubSystem: '<S22>/If Action Subsystem3'
        }

        // End of If: '<S22>/If1'

        // Merge: '<S18>/Merge3' incorporates:
        //   Constant: '<S22>/Zero'
        //   SignalConversion generated from: '<S22>/value'

        Trajectory_B.return_value = 1;

        // End of Outputs for SubSystem: '<S18>/If Action Subsystem'
      } else {
        // Outputs for IfAction SubSystem: '<S18>/If Action Subsystem1' incorporates:
        //   ActionPort: '<S23>/Action Port'

        // Merge: '<S18>/Merge' incorporates:
        //   SignalConversion generated from: '<S23>/p'

        Trajectory_B.p = rtb_p;

        // Merge: '<S18>/Merge1' incorporates:
        //   SignalConversion generated from: '<S23>/v'

        Trajectory_B.v = rtb_v;

        // Merge: '<S18>/Merge2' incorporates:
        //   SignalConversion generated from: '<S23>/a'

        Trajectory_B.a = rtb_a;

        // Merge: '<S18>/Merge3' incorporates:
        //   Constant: '<S23>/Zero'
        //   SignalConversion generated from: '<S23>/value'

        Trajectory_B.return_value = 0;

        // End of Outputs for SubSystem: '<S18>/If Action Subsystem1'
      }

      // End of If: '<S18>/If'
    }

    // End of RelationalOperator: '<S4>/Equal'
    // End of Outputs for SubSystem: '<S4>/Enabled Subsystem'

    // SignalConversion generated from: '<S4>/a'
    *rty_a = Trajectory_B.a;

    // SignalConversion generated from: '<S4>/p'
    *rty_p = Trajectory_B.p;

    // SignalConversion generated from: '<S4>/v'
    *rty_v = Trajectory_B.v;

    // SignalConversion generated from: '<S4>/value'
    *rty_value = Trajectory_B.return_value;

    // End of Outputs for SubSystem: '<Root>/do_step'
  }

  // Model step function
  int32_t Trajectory::Trajectory_get_acc_ref() const
  {
    // Outputs for Function Call SubSystem: '<Root>/get_acc_ref'
    // SignalConversion generated from: '<S5>/value' incorporates:
    //   DataStoreRead: '<S5>/Data Store Read'
    //   DataTypeConversion: '<S5>/Data Type Conversion'
    //   Sum: '<S5>/Add'

    return static_cast<int32_t>(std::floor(Trajectory_DW.Trajectory_h.xA +
      Trajectory_DW.Trajectory_h.vA));

    // End of Outputs for SubSystem: '<Root>/get_acc_ref'
  }

  // Model step function
  int32_t Trajectory::Trajectory_get_pos_ref() const
  {
    // Outputs for Function Call SubSystem: '<Root>/get_pos_ref'
    // SignalConversion generated from: '<S6>/value' incorporates:
    //   DataStoreRead: '<S6>/Data Store Read'
    //   DataTypeConversion: '<S6>/Data Type Conversion'
    //   Sum: '<S6>/Add'

    return static_cast<int32_t>(std::floor(Trajectory_DW.Trajectory_h.xX +
      Trajectory_DW.Trajectory_h.vX));

    // End of Outputs for SubSystem: '<Root>/get_pos_ref'
  }

  // Model step function
  float Trajectory::Trajectory_get_target_position() const
  {
    // Outputs for Function Call SubSystem: '<Root>/get_target_position'
    // SignalConversion generated from: '<S7>/value' incorporates:
    //   DataStoreRead: '<S7>/Data Store Read'

    return Trajectory_DW.Trajectory_h.target_pos;

    // End of Outputs for SubSystem: '<Root>/get_target_position'
  }

  // Model step function
  float Trajectory::Trajectory_get_target_velocity() const
  {
    // Outputs for Function Call SubSystem: '<Root>/get_target_velocity'
    // SignalConversion generated from: '<S8>/value' incorporates:
    //   DataStoreRead: '<S8>/Data Store Read'

    return Trajectory_DW.Trajectory_h.target_vel;

    // End of Outputs for SubSystem: '<Root>/get_target_velocity'
  }

  // Model step function
  int32_t Trajectory::Trajectory_get_vel_ref() const
  {
    // Outputs for Function Call SubSystem: '<Root>/get_vel_ref'
    // SignalConversion generated from: '<S9>/value' incorporates:
    //   DataStoreRead: '<S9>/Data Store Read'
    //   DataTypeConversion: '<S9>/Data Type Conversion'
    //   Sum: '<S9>/Add'

    return static_cast<int32_t>(std::floor(Trajectory_DW.Trajectory_h.xV +
      Trajectory_DW.Trajectory_h.vV));

    // End of Outputs for SubSystem: '<Root>/get_vel_ref'
  }

  // Model step function
  void Trajectory::Trajectory_init(const float rtu_p0, const float rtu_v0, const
    float rtu_a0)
  {
    // Outputs for Function Call SubSystem: '<Root>/init'
    // SignalConversion generated from: '<S10>/p0'
    Trajectory_DW.Trajectory_h.xX = rtu_p0;

    // SignalConversion generated from: '<S10>/v0'
    Trajectory_DW.Trajectory_h.xV = rtu_v0;

    // SignalConversion generated from: '<S10>/a0'
    Trajectory_DW.Trajectory_h.xA = rtu_a0;

    // BusAssignment: '<S10>/Bus Assignment' incorporates:
    //   Constant: '<S10>/Constant'
    //   DataStoreWrite: '<S10>/Data Store Write'

    Trajectory_DW.Trajectory_h.xTimer = 0.0F;
    Trajectory_DW.Trajectory_h.xT = 0.0F;
    Trajectory_DW.Trajectory_h.vX = 0.0F;
    Trajectory_DW.Trajectory_h.vV = 0.0F;
    Trajectory_DW.Trajectory_h.vA = 0.0F;
    Trajectory_DW.Trajectory_h.vTimer = 0.0F;
    Trajectory_DW.Trajectory_h.vT = 0.0F;

    // End of Outputs for SubSystem: '<Root>/init'
  }

  // Model step function
  void Trajectory::Trajectory_set_pos_end(const float rtu_xStar, const float
    rtu_velAvg)
  {
    float rtb_Divide_f;
    float rtb_Gain_k;
    float rtb_Reciprocal;
    float rtb_Switch2_n;
    float rtb_Switch_l;

    // Outputs for Function Call SubSystem: '<Root>/set_pos_end'
    // Switch: '<S39>/Switch2' incorporates:
    //   DataStoreRead: '<S11>/Data Store Read'
    //   RelationalOperator: '<S39>/LowerRelop1'
    //   RelationalOperator: '<S39>/UpperRelop'
    //   SignalConversion generated from: '<S11>/xStar'
    //   Switch: '<S39>/Switch'

    if (rtu_xStar > Trajectory_DW.Trajectory_h.pos_max) {
      rtb_Switch2_n = Trajectory_DW.Trajectory_h.pos_max;
    } else if (rtu_xStar < Trajectory_DW.Trajectory_h.pos_min) {
      // Switch: '<S39>/Switch'
      rtb_Switch2_n = Trajectory_DW.Trajectory_h.pos_min;
    } else {
      rtb_Switch2_n = rtu_xStar;
    }

    // End of Switch: '<S39>/Switch2'

    // Sum: '<S11>/Add' incorporates:
    //   DataStoreRead: '<S11>/Data Store Read'

    rtb_Divide_f = rtb_Switch2_n - Trajectory_DW.Trajectory_h.xX;

    // Switch: '<S11>/Switch' incorporates:
    //   Constant: '<S35>/Constant'
    //   Constant: '<S36>/Constant'
    //   Gain: '<S11>/Gain5'
    //   Logic: '<S11>/XOR'
    //   RelationalOperator: '<S35>/Compare'
    //   RelationalOperator: '<S36>/Compare'
    //   SignalConversion generated from: '<S11>/velAvg'

    if (static_cast<bool>((rtb_Divide_f > 0.0F) ^ (rtu_velAvg > 0.0F))) {
      rtb_Switch_l = -rtu_velAvg;
    } else {
      rtb_Switch_l = rtu_velAvg;
    }

    // End of Switch: '<S11>/Switch'

    // Switch: '<S40>/Switch2' incorporates:
    //   DataStoreRead: '<S11>/Data Store Read'
    //   Gain: '<S11>/Gain3'
    //   RelationalOperator: '<S40>/LowerRelop1'
    //   RelationalOperator: '<S40>/UpperRelop'
    //   Switch: '<S40>/Switch'

    if (rtb_Switch_l > Trajectory_DW.Trajectory_h.vel_max) {
      rtb_Switch_l = Trajectory_DW.Trajectory_h.vel_max;
    } else if (rtb_Switch_l < -Trajectory_DW.Trajectory_h.vel_max) {
      // Switch: '<S40>/Switch' incorporates:
      //   Gain: '<S11>/Gain3'

      rtb_Switch_l = -Trajectory_DW.Trajectory_h.vel_max;
    }

    // End of Switch: '<S40>/Switch2'

    // Gain: '<S11>/Gain1'
    rtb_Switch_l *= 2.0F;

    // Product: '<S11>/Divide'
    rtb_Divide_f /= rtb_Switch_l;

    // Product: '<S11>/Reciprocal'
    rtb_Reciprocal = 1.0F / rtb_Divide_f;

    // Gain: '<S37>/Gain' incorporates:
    //   DataStoreRead: '<S11>/Data Store Read'
    //   Product: '<S37>/Divide'
    //   Sum: '<S37>/Add'

    rtb_Gain_k = (Trajectory_DW.Trajectory_h.xV * rtb_Reciprocal +
                  Trajectory_DW.Trajectory_h.xA) * 0.75F;

    // Sum: '<S38>/Add1' incorporates:
    //   DataStoreRead: '<S11>/Data Store Read'
    //   Gain: '<S38>/Gain'
    //   Gain: '<S38>/Gain1'
    //   Product: '<S38>/Divide'
    //   Sum: '<S38>/Add'

    rtb_Switch_l = (0.0F - (Trajectory_DW.Trajectory_h.xV - rtb_Switch_l) *
                    rtb_Reciprocal * 3.75F) - 1.25F *
      Trajectory_DW.Trajectory_h.xA;

    // BusAssignment: '<S11>/Bus Assignment' incorporates:
    //   DataStoreRead: '<S11>/Data Store Read'
    //   DataStoreWrite: '<S11>/Data Store Write'
    //   Gain: '<S11>/Gain'
    //   Gain: '<S11>/Gain2'
    //   Gain: '<S11>/Gain4'
    //   Sum: '<S11>/Add1'
    //   Sum: '<S11>/Add2'

    Trajectory_DW.Trajectory_h.xK0 = 0.5F * Trajectory_DW.Trajectory_h.xA -
      rtb_Gain_k;
    Trajectory_DW.Trajectory_h.xK1 = (0.0F - 0.5F *
      Trajectory_DW.Trajectory_h.xA) - rtb_Switch_l;
    Trajectory_DW.Trajectory_h.xK2 = rtb_Gain_k;
    Trajectory_DW.Trajectory_h.xK3 = rtb_Switch_l;
    Trajectory_DW.Trajectory_h.xT = rtb_Divide_f;
    Trajectory_DW.Trajectory_h.target_pos = rtb_Switch2_n;
    Trajectory_DW.Trajectory_h.xInvT = rtb_Reciprocal;
    Trajectory_DW.Trajectory_h.xTimer = -rtb_Divide_f;

    // End of Outputs for SubSystem: '<Root>/set_pos_end'
  }

  // Model step function
  void Trajectory::Trajectory_set_pos_raw(const float rtu_p0)
  {
    float rtb_Switch_d;

    // Outputs for Function Call SubSystem: '<Root>/set_pos_raw'
    // Switch: '<S12>/Switch' incorporates:
    //   DataStoreRead: '<S12>/Data Store Read'
    //   SignalConversion generated from: '<S12>/p0'
    //   Sum: '<S12>/Add'

    if (Trajectory_DW.Trajectory_h.pos_max - Trajectory_DW.Trajectory_h.pos_min
        != 0.0F) {
      // Switch: '<S41>/Switch2' incorporates:
      //   RelationalOperator: '<S41>/LowerRelop1'
      //   RelationalOperator: '<S41>/UpperRelop'
      //   SignalConversion generated from: '<S12>/p0'
      //   Switch: '<S41>/Switch'

      if (rtu_p0 > Trajectory_DW.Trajectory_h.pos_max) {
        rtb_Switch_d = Trajectory_DW.Trajectory_h.pos_max;
      } else if (rtu_p0 < Trajectory_DW.Trajectory_h.pos_min) {
        // Switch: '<S41>/Switch'
        rtb_Switch_d = Trajectory_DW.Trajectory_h.pos_min;
      } else {
        rtb_Switch_d = rtu_p0;
      }

      // End of Switch: '<S41>/Switch2'
    } else {
      rtb_Switch_d = rtu_p0;
    }

    // End of Switch: '<S12>/Switch'

    // BusAssignment: '<S12>/Bus Assignment' incorporates:
    //   Constant: '<S12>/Constant'
    //   Constant: '<S12>/Constant1'
    //   DataStoreWrite: '<S12>/Data Store Write'

    Trajectory_DW.Trajectory_h.target_pos = rtb_Switch_d;
    Trajectory_DW.Trajectory_h.xX = rtb_Switch_d;
    Trajectory_DW.Trajectory_h.xV = 0.0F;
    Trajectory_DW.Trajectory_h.xA = 0.0F;
    Trajectory_DW.Trajectory_h.xTimer = 0.0F;
    Trajectory_DW.Trajectory_h.xT = 0.0F;
    Trajectory_DW.Trajectory_h.vX = 0.0F;
    Trajectory_DW.Trajectory_h.vV = 0.0F;
    Trajectory_DW.Trajectory_h.vA = 0.0F;
    Trajectory_DW.Trajectory_h.vTimer = 0.0F;
    Trajectory_DW.Trajectory_h.vT = 0.0F;
    Trajectory_DW.Trajectory_h.bVelocityMove = false;

    // End of Outputs for SubSystem: '<Root>/set_pos_raw'
  }

  // Model step function
  void Trajectory::Trajectory_set_vel_end(const float rtu_vStar, const float
    rtu_accAvg)
  {
    float rtb_vK0;
    float rtb_vT;

    // Outputs for Function Call SubSystem: '<Root>/set_vel_end'
    // Gain: '<S13>/Gain' incorporates:
    //   SignalConversion generated from: '<S13>/accAvg'

    rtb_vK0 = 2.0F * rtu_accAvg;

    // Product: '<S13>/Divide' incorporates:
    //   DataStoreRead: '<S13>/Data Store Read'
    //   SignalConversion generated from: '<S13>/vStar'
    //   Sum: '<S13>/Add'

    rtb_vT = 1.0F / rtb_vK0 * (rtu_vStar - Trajectory_DW.Trajectory_h.vV);

    // Gain: '<S13>/Gain2' incorporates:
    //   DataStoreRead: '<S13>/Data Store Read'

    Trajectory_DW.Trajectory_h.vK1 = -0.5F * Trajectory_DW.Trajectory_h.vA;

    // Sum: '<S13>/Add1' incorporates:
    //   Gain: '<S13>/Gain2'
    //   Gain: '<S13>/Gain3'
    //   Gain: '<S13>/Gain4'

    rtb_vK0 = 0.5F * Trajectory_DW.Trajectory_h.vK1 + 0.75F * rtb_vK0;

    // Sum: '<S13>/Add2' incorporates:
    //   Gain: '<S13>/Gain2'

    Trajectory_DW.Trajectory_h.vK2 = (0.0F - rtb_vK0) -
      Trajectory_DW.Trajectory_h.vK1;

    // BusAssignment: '<S13>/Bus Assignment' incorporates:
    //   Constant: '<S13>/Constant'
    //   DataStoreWrite: '<S13>/Data Store Write'
    //   Gain: '<S13>/Gain1'
    //   Product: '<S13>/Reciprocal'
    //   SignalConversion generated from: '<S13>/vStar'

    Trajectory_DW.Trajectory_h.bVelocityMove = true;
    Trajectory_DW.Trajectory_h.target_vel = rtu_vStar;
    Trajectory_DW.Trajectory_h.vT = rtb_vT;
    Trajectory_DW.Trajectory_h.vTimer = -rtb_vT;
    Trajectory_DW.Trajectory_h.vInvT = 1.0F / rtb_vT;
    Trajectory_DW.Trajectory_h.vK0 = rtb_vK0;

    // End of Outputs for SubSystem: '<Root>/set_vel_end'
  }

  // Model step function
  void Trajectory::Trajectory_start2end(const float rtu_start, const float
    rtu_end, const float rtu_velAvg)
  {
    // Outputs for Function Call SubSystem: '<Root>/start2end'
    // FunctionCaller: '<S14>/Function Caller' incorporates:
    //   SignalConversion generated from: '<S14>/start'

    Trajectory_set_pos_raw(rtu_start);

    // FunctionCaller: '<S14>/Function Caller2' incorporates:
    //   SignalConversion generated from: '<S14>/end'
    //   SignalConversion generated from: '<S14>/velAvg'

    Trajectory_set_pos_end(rtu_end, rtu_velAvg);

    // End of Outputs for SubSystem: '<Root>/start2end'
  }

  // Model step function
  void Trajectory::Trajectory_stop(const float rtu_pos)
  {
    // Outputs for Function Call SubSystem: '<Root>/stop'
    // FunctionCaller: '<S15>/Function Caller4' incorporates:
    //   SignalConversion generated from: '<S15>/pos'

    Trajectory_set_pos_raw(rtu_pos);

    // End of Outputs for SubSystem: '<Root>/stop'
  }

  // Model step function
  void Trajectory::Trajectory_velocity_stop()
  {
    // Outputs for Function Call SubSystem: '<Root>/velocity_stop'
    // Sum: '<S16>/Add' incorporates:
    //   DataStoreRead: '<S16>/Data Store Read'

    Trajectory_DW.Trajectory_h.xX += Trajectory_DW.Trajectory_h.vX;

    // BusAssignment: '<S16>/Bus Assignment' incorporates:
    //   Constant: '<S16>/Zero'
    //   Constant: '<S16>/Zero1'
    //   DataStoreWrite: '<S16>/Data Store Write'

    Trajectory_DW.Trajectory_h.bVelocityMove = false;
    Trajectory_DW.Trajectory_h.vTimer = 0.0F;
    Trajectory_DW.Trajectory_h.vT = 0.0F;
    Trajectory_DW.Trajectory_h.vX = 0.0F;
    Trajectory_DW.Trajectory_h.vV = 0.0F;
    Trajectory_DW.Trajectory_h.vA = 0.0F;
    Trajectory_DW.Trajectory_h.target_vel = 0.0F;

    // End of Outputs for SubSystem: '<Root>/velocity_stop'
  }

  // Model initialize function
  void Trajectory::initialize()
  {
    // Outputs for Atomic SubSystem: '<Root>/Initialize Function'
    // DataStoreWrite: '<S2>/Data Store Write'
    std::memset(&Trajectory_DW.Trajectory_h, 0, sizeof(trajectoryhidParameters));

    // End of Outputs for SubSystem: '<Root>/Initialize Function'
  }

  // Constructor
  Trajectory::Trajectory():
    Trajectory_B(),
    Trajectory_DW()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  Trajectory::~Trajectory()
  {
    // Currently there is no destructor body generated.
  }
}

//
// File trailer for generated code.
//
// [EOF]
//
