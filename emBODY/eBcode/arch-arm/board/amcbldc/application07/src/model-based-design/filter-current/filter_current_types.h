//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: filter_current_types.h
//
// Code generated for Simulink model 'filter_current'.
//
// Model version                  : 3.7
// Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
// C/C++ source code generated on : Tue Aug  9 15:19:10 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_filter_current_types_h_
#define RTW_HEADER_filter_current_types_h_
#include "rtwtypes.h"

// Model Code Variants
#ifndef DEFINED_TYPEDEF_FOR_MotorCurrent_
#define DEFINED_TYPEDEF_FOR_MotorCurrent_

struct MotorCurrent
{
  // motor current
  real32_T current;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_ControlOutputs_
#define DEFINED_TYPEDEF_FOR_ControlOutputs_

struct ControlOutputs
{
  // control effort (quadrature)
  real32_T Vq;

  // control effort (3-phases)
  real32_T Vabc[3];

  // quadrature current
  MotorCurrent Iq_fbk;
};

#endif

#ifndef struct_c_dsp_internal_MedianFilterCG_T
#define struct_c_dsp_internal_MedianFilterCG_T

struct c_dsp_internal_MedianFilterCG_T
{
  int32_T isInitialized;
  boolean_T isSetupComplete;
  real32_T pWinLen;
  real32_T pBuf[10];
  real32_T pHeap[10];
  real32_T pMidHeap;
  real32_T pIdx;
  real32_T pPos[10];
  real32_T pMinHeapLength;
  real32_T pMaxHeapLength;
};

#endif                                // struct_c_dsp_internal_MedianFilterCG_T

#ifndef struct_cell_wrap_filter_current_T
#define struct_cell_wrap_filter_current_T

struct cell_wrap_filter_current_T
{
  uint32_T f1[8];
};

#endif                                 // struct_cell_wrap_filter_current_T

#ifndef struct_dsp_MedianFilter_filter_curre_T
#define struct_dsp_MedianFilter_filter_curre_T

struct dsp_MedianFilter_filter_curre_T
{
  boolean_T matlabCodegenIsDeleted;
  int32_T isInitialized;
  boolean_T isSetupComplete;
  cell_wrap_filter_current_T inputVarSize;
  int32_T NumChannels;
  c_dsp_internal_MedianFilterCG_T pMID;
};

#endif                                // struct_dsp_MedianFilter_filter_curre_T

// Forward declaration for rtModel
typedef struct tag_RTM_filter_current_T RT_MODEL_filter_current_T;

#endif                                 // RTW_HEADER_filter_current_types_h_

//
// File trailer for generated code.
//
// [EOF]
//
