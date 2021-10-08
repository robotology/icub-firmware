//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: multiword_types.h
//
// Code generated for Simulink model 'can_decoder'.
//
// Model version                  : 1.280
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Mon Sep 20 12:43:37 2021
//
#ifndef MULTIWORD_TYPES_H
#define MULTIWORD_TYPES_H
#include "rtwtypes.h"

//
//  MultiWord supporting definitions

typedef long int long_T;

//
//  MultiWord types

typedef struct {
  uint32_T chunks[2];
} int64m_T;

typedef struct {
  int64m_T re;
  int64m_T im;
} cint64m_T;

typedef struct {
  uint32_T chunks[2];
} uint64m_T;

typedef struct {
  uint64m_T re;
  uint64m_T im;
} cuint64m_T;

#endif                                 // MULTIWORD_TYPES_H

//
// File trailer for generated code.
//
// [EOF]
//
