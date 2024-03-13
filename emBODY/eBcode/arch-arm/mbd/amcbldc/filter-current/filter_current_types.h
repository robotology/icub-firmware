//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: filter_current_types.h
//
// Code generated for Simulink model 'filter_current'.
//
// Model version                  : 6.14
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Mon Mar  4 13:07:41 2024
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_filter_current_types_h_
#define RTW_HEADER_filter_current_types_h_
#include "rtwtypes.h"
#ifndef DEFINED_TYPEDEF_FOR_FOCOutputs_
#define DEFINED_TYPEDEF_FOR_FOCOutputs_

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

#ifndef struct_c_dsp_internal_MedianFilterCG_filter_current_T
#define struct_c_dsp_internal_MedianFilterCG_filter_current_T

struct c_dsp_internal_MedianFilterCG_filter_current_T
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

#endif                 // struct_c_dsp_internal_MedianFilterCG_filter_current_T

#ifndef struct_cell_wrap_filter_current_T
#define struct_cell_wrap_filter_current_T

struct cell_wrap_filter_current_T
{
  uint32_T f1[8];
};

#endif                                 // struct_cell_wrap_filter_current_T

#ifndef struct_dsp_simulink_MedianFilter_filter_current_T
#define struct_dsp_simulink_MedianFilter_filter_current_T

struct dsp_simulink_MedianFilter_filter_current_T
{
  boolean_T matlabCodegenIsDeleted;
  int32_T isInitialized;
  boolean_T isSetupComplete;
  cell_wrap_filter_current_T inputVarSize;
  int32_T NumChannels;
  c_dsp_internal_MedianFilterCG_filter_current_T pMID;
};

#endif                     // struct_dsp_simulink_MedianFilter_filter_current_T
#endif                                 // RTW_HEADER_filter_current_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
