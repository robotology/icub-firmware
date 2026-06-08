//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: Trajectory.h
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
#ifndef Trajectory_h_
#define Trajectory_h_
#include <stdbool.h>
#include <stdint.h>
#include "Trajectory_types.h"

// Class declaration for model Trajectory
namespace namespace_s1
{
  class Trajectory
  {
    // public data and function members
   public:
    // Block signals (default storage)
    struct B_Trajectory_T {
      float p;                         // '<S18>/Merge'
      float v;                         // '<S18>/Merge1'
      float a;                         // '<S18>/Merge2'
      int32_t return_value;            // '<S18>/Merge3'
    };

    // Block states (default storage) for system '<Root>'
    struct DW_Trajectory_T {
      trajectoryhidParameters Trajectory_h;// '<Root>/Data Store Memory'
    };

    // model step function
    void Trajectory_config_limits(const float rtu_pos_min, const float
      rtu_pos_max, const float rtu_vel_max, const float rtu_acc_max);

    // model step function
    void Trajectory_do_step(float *rty_p, float *rty_v, float *rty_a, int32_t
      *rty_value);

    // model step function
    void Trajectory_init(const float rtu_p0, const float rtu_v0, const float
                         rtu_a0);

    // model step function
    void Trajectory_set_pos_end(const float rtu_xStar, const float rtu_velAvg);

    // model step function
    void Trajectory_set_pos_raw(const float rtu_p0);

    // model step function
    void Trajectory_set_vel_end(const float rtu_vStar, const float rtu_accAvg);

    // model step function
    void Trajectory_start2end(const float rtu_start, const float rtu_end, const
      float rtu_velAvg);

    // model step function
    void Trajectory_stop(const float rtu_pos);

    // model initialize function
    void initialize();

    // model step function
    bool Trajectory_is_done() const;

    // model step function
    int32_t Trajectory_get_acc_ref() const;

    // model step function
    int32_t Trajectory_get_pos_ref() const;

    // model step function
    float Trajectory_get_target_position() const;

    // model step function
    float Trajectory_get_target_velocity() const;

    // model step function
    int32_t Trajectory_get_vel_ref() const;

    // model step function
    void Trajectory_velocity_stop();

    // Constructor
    Trajectory();

    // Destructor
    ~Trajectory();

    // private data and function members
   private:
    // Block signals
    B_Trajectory_T Trajectory_B;

    // Block states
    DW_Trajectory_T Trajectory_DW;

    // private member function(s) for subsystem '<S24>/If Action Subsystem'
    void Trajectory_IfActionSubsystem(bool rtu_bVelocityMove, float rtu_vTimer,
      float rtu_vT, float *rty_vn);

    // private member function(s) for subsystem '<S24>/If Action Subsystem1'
    static void Trajectory_IfActionSubsystem1(float *rty_an);
  };
}

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S39>/Data Type Duplicate' : Unused code path elimination
//  Block '<S39>/Data Type Propagation' : Unused code path elimination
//  Block '<S40>/Data Type Duplicate' : Unused code path elimination
//  Block '<S40>/Data Type Propagation' : Unused code path elimination
//  Block '<S41>/Data Type Duplicate' : Unused code path elimination
//  Block '<S41>/Data Type Propagation' : Unused code path elimination


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
//  '<Root>' : 'Trajectory'
//  '<S1>'   : 'Trajectory/Done'
//  '<S2>'   : 'Trajectory/Initialize Function'
//  '<S3>'   : 'Trajectory/config_limits'
//  '<S4>'   : 'Trajectory/do_step'
//  '<S5>'   : 'Trajectory/get_acc_ref'
//  '<S6>'   : 'Trajectory/get_pos_ref'
//  '<S7>'   : 'Trajectory/get_target_position'
//  '<S8>'   : 'Trajectory/get_target_velocity'
//  '<S9>'   : 'Trajectory/get_vel_ref'
//  '<S10>'  : 'Trajectory/init'
//  '<S11>'  : 'Trajectory/set_pos_end'
//  '<S12>'  : 'Trajectory/set_pos_raw'
//  '<S13>'  : 'Trajectory/set_vel_end'
//  '<S14>'  : 'Trajectory/start2end'
//  '<S15>'  : 'Trajectory/stop'
//  '<S16>'  : 'Trajectory/velocity_stop'
//  '<S17>'  : 'Trajectory/Done/Compare To Constant'
//  '<S18>'  : 'Trajectory/do_step/Enabled Subsystem'
//  '<S19>'  : 'Trajectory/do_step/Switch Case Action Subsystem2'
//  '<S20>'  : 'Trajectory/do_step/bVelocityMove'
//  '<S21>'  : 'Trajectory/do_step/vTimer-vT'
//  '<S22>'  : 'Trajectory/do_step/Enabled Subsystem/If Action Subsystem'
//  '<S23>'  : 'Trajectory/do_step/Enabled Subsystem/If Action Subsystem1'
//  '<S24>'  : 'Trajectory/do_step/Enabled Subsystem/If Action Subsystem2'
//  '<S25>'  : 'Trajectory/do_step/Enabled Subsystem/If Action Subsystem/If Action Subsystem'
//  '<S26>'  : 'Trajectory/do_step/Enabled Subsystem/If Action Subsystem/If Action Subsystem1'
//  '<S27>'  : 'Trajectory/do_step/Enabled Subsystem/If Action Subsystem/If Action Subsystem2'
//  '<S28>'  : 'Trajectory/do_step/Enabled Subsystem/If Action Subsystem/If Action Subsystem3'
//  '<S29>'  : 'Trajectory/do_step/Enabled Subsystem/If Action Subsystem/If Action Subsystem/If Action Subsystem'
//  '<S30>'  : 'Trajectory/do_step/Enabled Subsystem/If Action Subsystem2/If Action Subsystem'
//  '<S31>'  : 'Trajectory/do_step/Enabled Subsystem/If Action Subsystem2/If Action Subsystem1'
//  '<S32>'  : 'Trajectory/do_step/Enabled Subsystem/If Action Subsystem2/If Action Subsystem2'
//  '<S33>'  : 'Trajectory/do_step/Enabled Subsystem/If Action Subsystem2/If Action Subsystem3'
//  '<S34>'  : 'Trajectory/do_step/Enabled Subsystem/If Action Subsystem2/If Action Subsystem/If Action Subsystem'
//  '<S35>'  : 'Trajectory/set_pos_end/Compare To Zero'
//  '<S36>'  : 'Trajectory/set_pos_end/Compare To Zero1'
//  '<S37>'  : 'Trajectory/set_pos_end/Subsystem'
//  '<S38>'  : 'Trajectory/set_pos_end/Subsystem2'
//  '<S39>'  : 'Trajectory/set_pos_end/posSaturation'
//  '<S40>'  : 'Trajectory/set_pos_end/velSaturation'
//  '<S41>'  : 'Trajectory/set_pos_raw/Saturation Dynamic'

#endif                                 // Trajectory_h_

//
// File trailer for generated code.
//
// [EOF]
//
