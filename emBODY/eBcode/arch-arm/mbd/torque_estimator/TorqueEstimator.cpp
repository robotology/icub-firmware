//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: TorqueEstimator.cpp
//
// Code generated for Simulink model 'TorqueEstimator'.
//
// Model version                  : 2.100
// Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
// C/C++ source code generated on : Tue Oct 22 09:33:53 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#include "TorqueEstimator.h"
#include "rtwtypes.h"
#include <cmath>

// Output and update for atomic system: '<Root>/TorqueEstimator'
void TorqueEstimator::TorqueEstimator_l(real_T rtu_Current, real_T rtu_Velocity,
  real_T rtu_Km, real_T rtu_Kw, real_T rtu_S0, real_T rtu_S1, real_T rtu_Vth,
  real_T rtu_Fc_pos, real_T rtu_Fc_neg, real_T rtu_Fs_pos, real_T rtu_Fs_neg,
  real_T *rty_Torque, real_T *rty_Motor_torque, real_T *rty_Friction,
  DW_TorqueEstimator *localDW)
{
  real_T Fc;
  real_T Fs;
  real_T tmp;
  real_T tmp_0;
  if (rtu_Velocity >= 0.0) {
    Fc = rtu_Fc_pos;
    Fs = rtu_Fs_pos;
  } else {
    Fc = rtu_Fc_neg;
    Fs = rtu_Fs_neg;
  }

  Fc = rtu_Velocity - std::abs(rtu_Velocity) * rtu_S0 * localDW->z / ((Fs - Fc) *
    std::exp(-std::abs(rtu_Velocity / rtu_Vth)) + Fc);
  localDW->z += 0.001 * Fc;
  tmp = rtu_Km * rtu_Current;
  *rty_Motor_torque = tmp;
  Fs = rtu_Kw * rtu_Velocity;
  tmp_0 = rtu_S0 * localDW->z;
  Fc *= rtu_S1;
  *rty_Friction = (Fs + tmp_0) + Fc;
  *rty_Torque = ((tmp - Fs) - tmp_0) - Fc;
}

// Model step function
void TorqueEstimator::step()
{
  // MATLAB Function: '<Root>/TorqueEstimator' incorporates:
  //   Inport: '<Root>/Current'
  //   Inport: '<Root>/Fc_neg'
  //   Inport: '<Root>/Fc_pos'
  //   Inport: '<Root>/Fs_neg'
  //   Inport: '<Root>/Fs_pos'
  //   Inport: '<Root>/Km'
  //   Inport: '<Root>/Kw'
  //   Inport: '<Root>/S0'
  //   Inport: '<Root>/S1'
  //   Inport: '<Root>/Velocity'
  //   Inport: '<Root>/Vth'
  //   Outport: '<Root>/Friction'
  //   Outport: '<Root>/Motor_torque'
  //   Outport: '<Root>/Torque'

  TorqueEstimator_l(rtU.Current, rtU.Velocity, rtU.Km, rtU.Kw, rtU.S0, rtU.S1,
                    rtU.Vth, rtU.Fc_pos, rtU.Fc_neg, rtU.Fs_pos, rtU.Fs_neg,
                    &rtY.Torque, &rtY.Motor_torque, &rtY.Friction,
                    &rtDW.sf_TorqueEstimator);
}

// Model initialize function
void TorqueEstimator::initialize()
{
  // (no initialization code required)
}

// Constructor
TorqueEstimator::TorqueEstimator() :
  rtU(),
  rtY(),
  rtDW(),
  rtM()
{
  // Currently there is no constructor body generated.
}

// Destructor
// Currently there is no destructor body generated.
TorqueEstimator::~TorqueEstimator() = default;

// Real-Time Model get method
TorqueEstimator::RT_MODEL * TorqueEstimator::getRTM()
{
  return (&rtM);
}

//
// File trailer for generated code.
//
// [EOF]
//
