//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: Trajectory_types.h
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
#ifndef Trajectory_types_h_
#define Trajectory_types_h_
#include <stdbool.h>
#ifndef DEFINED_TYPEDEF_FOR_trajectoryhidParameters_
#define DEFINED_TYPEDEF_FOR_trajectoryhidParameters_

struct trajectoryhidParameters
{
  float pos_min;
  float pos_max;
  float vel_max;
  float acc_max;
  float target_pos;
  float xX;
  float xV;
  float xA;
  float xT;
  float xTimer;
  bool bVelocityMove;
  float target_vel;
  float vX;
  float vV;
  float vA;
  float vT;
  float vTimer;
  float xInvT;
  float vInvT;
  float xK0;
  float xK1;
  float xK2;
  float xK3;
  float vK0;
  float vK1;
  float vK2;
};

#endif
#endif                                 // Trajectory_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
