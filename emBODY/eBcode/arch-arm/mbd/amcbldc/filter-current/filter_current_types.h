//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: filter_current_types.h
//
// Code generated for Simulink model 'filter_current'.
//
// Model version                  : 8.0
// Simulink Coder version         : 26.1 (R2026a) 20-Nov-2025
// C/C++ source code generated on : Thu Jul  9 10:00:51 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef FILTER_CURRENT_TYPES_H_
#define FILTER_CURRENT_TYPES_H_
#include "rtwtypes.h"
#ifndef DEFINED_TYPEDEF_FOR_FOCOUTPUTS_
#define DEFINED_TYPEDEF_FOR_FOCOUTPUTS_

struct FOCOutputs
{
  // control effort (quadrature)
  real32_T Vq;

  // control effort (3-phases)
  real32_T Vabc[3];

  // quadrature current
  real32_T Iq_fbk;

  // direct current
  real32_T Id_fbk;

  // RMS of Iq
  real32_T Iq_rms;

  // RMS of Id
  real32_T Id_rms;
};

#endif

#ifndef STRUCT_C_DSP_INTERNAL_CODEGEN_MEDIANFILTER_FILTER_CURRENT_T
#define STRUCT_C_DSP_INTERNAL_CODEGEN_MEDIANFILTER_FILTER_CURRENT_T

struct c_dsp_internal_codegen_MedianFilter_filter_current_T
{
  int32_T isInitialized;
  boolean_T isSetupComplete;
  real32_T pWinLen;
  real32_T pBuf[32];
  real32_T pHeap[32];
  real32_T pMidHeap;
  real32_T pIdx;
  real32_T pPos[32];
  real32_T pMinHeapLength;
  real32_T pMaxHeapLength;
};

#endif           // STRUCT_C_DSP_INTERNAL_CODEGEN_MEDIANFILTER_FILTER_CURRENT_T

#ifndef STRUCT_CELL_WRAP_FILTER_CURRENT_T
#define STRUCT_CELL_WRAP_FILTER_CURRENT_T

struct cell_wrap_filter_current_T
{
  uint32_T f1[8];
};

#endif                                 // STRUCT_CELL_WRAP_FILTER_CURRENT_T

#ifndef STRUCT_DSP_SIMULINK_MEDIANFILTER_FILTER_CURRENT_T
#define STRUCT_DSP_SIMULINK_MEDIANFILTER_FILTER_CURRENT_T

struct dsp_simulink_MedianFilter_filter_current_T
{
  boolean_T matlabCodegenIsDeleted;
  int32_T isInitialized;
  boolean_T isSetupComplete;
  cell_wrap_filter_current_T inputVarSize;
  int32_T NumChannels;
  c_dsp_internal_codegen_MedianFilter_filter_current_T pMID;
};

#endif                     // STRUCT_DSP_SIMULINK_MEDIANFILTER_FILTER_CURRENT_T
#endif                                 // FILTER_CURRENT_TYPES_H_

//
// File trailer for generated code.
//
// [EOF]
//
