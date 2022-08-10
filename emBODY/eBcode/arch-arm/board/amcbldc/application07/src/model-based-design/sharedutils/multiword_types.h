//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: multiword_types.h
//
// Code generated for Simulink model 'SupervisorFSM_RX'.
//
// Model version                  : 4.6
// Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
// C/C++ source code generated on : Tue Jun  7 15:41:38 2022
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
