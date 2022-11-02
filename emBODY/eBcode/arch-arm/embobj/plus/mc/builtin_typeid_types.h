//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: builtin_typeid_types.h
//
// Code generated for Simulink model 'TorqueModel'.
//
// Model version                  : 3.9
// Simulink Coder version         : 9.8 (R2022b) 13-May-2022
// C/C++ source code generated on : Thu Oct 27 10:27:11 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//

#ifndef BUILTIN_TYPEID_TYPES_H
#define BUILTIN_TYPEID_TYPES_H
#ifndef BUILTIN_TYPEID_TYPES
#define BUILTIN_TYPEID_TYPES

// Enumeration of built-in data types
typedef enum {
  SS_DOUBLE = 0,
  SS_SINGLE = 1,
  SS_INT8 = 2,
  SS_UINT8 = 3,
  SS_INT16 = 4,
  SS_UINT16 = 5,
  SS_INT32 = 6,
  SS_UINT32 = 7,
  SS_BOOLEAN = 8
} BuiltInDTypeId;

#define SS_NUM_BUILT_IN_DTYPE          ((int)SS_BOOLEAN+1)

// Enumeration for MAT-file logging code
typedef int DTypeId;

// Enumeration of pre-defined data types
typedef enum {
  SS_FCN_CALL = 9,
  SS_INTEGER = 10,
  SS_POINTER = 11,
  SS_INTERNAL_DTYPE2 = 12,
  SS_TIMER_UINT32_PAIR = 13,
  SS_CONNECTION_TYPE = 14
} PreDefinedDTypeId;

#endif                                 // BUILTIN_TYPEID_TYPES
#endif                                 // BUILTIN_TYPEID_TYPES_H

//
// File trailer for generated code.
//
// [EOF]
//
