//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: filter_current.cpp
//
// Code generated for Simulink model 'filter_current'.
//
// Model version                  : 8.0
// Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
// C/C++ source code generated on : Mon Aug 11 10:31:07 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "filter_current.h"
#include "filter_current_types.h"
#include "rtwtypes.h"
#include <cmath>
#include <cstring>

// Forward declaration for local functions
static void filter_current_MedianFilter_resetImpl
  (c_dsp_internal_codegen_MedianFilter_filter_current_T *obj);
static void filter_current_MedianFilter_trickleDownMin
  (c_dsp_internal_codegen_MedianFilter_filter_current_T *obj, real32_T i);
static void filter_current_MedianFilter_trickleDownMax
  (c_dsp_internal_codegen_MedianFilter_filter_current_T *obj, real32_T i);
static void filter_current_MedianFilter_resetImpl
  (c_dsp_internal_codegen_MedianFilter_filter_current_T *obj)
{
  int32_T cnt1;
  real32_T cnt2;

  // Start for MATLABSystem: '<Root>/Median Filter'
  std::memset(&obj->pBuf[0], 0, sizeof(real32_T) << 5U);
  std::memset(&obj->pPos[0], 0, sizeof(real32_T) << 5U);
  std::memset(&obj->pHeap[0], 0, sizeof(real32_T) << 5U);
  obj->pWinLen = 32.0F;
  obj->pIdx = obj->pWinLen;

  // Start for MATLABSystem: '<Root>/Median Filter'
  obj->pMidHeap = std::ceil((obj->pWinLen + 1.0F) / 2.0F);
  cnt2 = (obj->pWinLen - 1.0F) / 2.0F;
  if (cnt2 < 0.0F) {
    obj->pMinHeapLength = std::ceil(cnt2);
  } else {
    obj->pMinHeapLength = std::floor(cnt2);
  }

  cnt2 = obj->pWinLen / 2.0F;
  if (cnt2 < 0.0F) {
    obj->pMaxHeapLength = std::ceil(cnt2);
  } else {
    obj->pMaxHeapLength = std::floor(cnt2);
  }

  cnt1 = 1;
  cnt2 = obj->pWinLen;
  for (int32_T i = 0; i < 32; i++) {
    // Start for MATLABSystem: '<Root>/Median Filter'
    if (std::fmod(32.0F - static_cast<real32_T>(i), 2.0F) == 0.0F) {
      obj->pPos[31 - i] = static_cast<real32_T>(cnt1);
      cnt1++;
    } else {
      obj->pPos[31 - i] = cnt2;
      cnt2--;
    }

    obj->pHeap[static_cast<int32_T>(obj->pPos[31 - i]) - 1] = 32.0F -
      static_cast<real32_T>(i);
  }
}

static void filter_current_MedianFilter_trickleDownMin
  (c_dsp_internal_codegen_MedianFilter_filter_current_T *obj, real32_T i)
{
  boolean_T exitg1;
  exitg1 = false;
  while ((!exitg1) && (i <= obj->pMinHeapLength)) {
    real32_T tmp;
    real32_T tmp_0;
    real32_T u;
    real32_T y;
    if ((i > 1.0F) && (i < obj->pMinHeapLength) && (obj->pBuf[static_cast<
         int32_T>(obj->pHeap[static_cast<int32_T>((i + 1.0F) + obj->pMidHeap) -
                  1]) - 1] < obj->pBuf[static_cast<int32_T>(obj->pHeap[
          static_cast<int32_T>(i + obj->pMidHeap) - 1]) - 1])) {
      i++;
    }

    u = i / 2.0F;
    if (u < 0.0F) {
      y = std::ceil(u);
    } else {
      y = std::floor(u);
    }

    u = i + obj->pMidHeap;
    y += obj->pMidHeap;
    tmp = obj->pHeap[static_cast<int32_T>(u) - 1];
    tmp_0 = obj->pHeap[static_cast<int32_T>(y) - 1];
    if (obj->pBuf[static_cast<int32_T>(tmp) - 1] >= obj->pBuf
        [static_cast<int32_T>(tmp_0) - 1]) {
      exitg1 = true;
    } else {
      obj->pHeap[static_cast<int32_T>(u) - 1] = tmp_0;
      obj->pHeap[static_cast<int32_T>(y) - 1] = tmp;
      obj->pPos[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(u) - 1]) -
        1] = u;
      obj->pPos[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(y) - 1]) -
        1] = y;
      i *= 2.0F;
    }
  }
}

static void filter_current_MedianFilter_trickleDownMax
  (c_dsp_internal_codegen_MedianFilter_filter_current_T *obj, real32_T i)
{
  boolean_T exitg1;
  exitg1 = false;
  while ((!exitg1) && (i >= -obj->pMaxHeapLength)) {
    real32_T ind2;
    real32_T tmp;
    real32_T tmp_0;
    real32_T u;
    if ((i < -1.0F) && (i > -obj->pMaxHeapLength) && (obj->pBuf
         [static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(i + obj->pMidHeap)
          - 1]) - 1] < obj->pBuf[static_cast<int32_T>(obj->pHeap
          [static_cast<int32_T>((i - 1.0F) + obj->pMidHeap) - 1]) - 1])) {
      i--;
    }

    u = i / 2.0F;
    if (u < 0.0F) {
      u = std::ceil(u);
    } else {
      u = std::floor(u);
    }

    u += obj->pMidHeap;
    ind2 = i + obj->pMidHeap;
    tmp = obj->pHeap[static_cast<int32_T>(u) - 1];
    tmp_0 = obj->pHeap[static_cast<int32_T>(ind2) - 1];
    if (obj->pBuf[static_cast<int32_T>(tmp) - 1] >= obj->pBuf
        [static_cast<int32_T>(tmp_0) - 1]) {
      exitg1 = true;
    } else {
      obj->pHeap[static_cast<int32_T>(u) - 1] = tmp_0;
      obj->pHeap[static_cast<int32_T>(ind2) - 1] = tmp;
      obj->pPos[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(u) - 1]) -
        1] = u;
      obj->pPos[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(ind2) - 1])
        - 1] = ind2;
      i *= 2.0F;
    }
  }
}

// System initialize for referenced model: 'filter_current'
void filter_current_Init(DW_filter_current_f_T *localDW)
{
  // Start for MATLABSystem: '<Root>/Median Filter'
  localDW->obj.matlabCodegenIsDeleted = false;
  localDW->objisempty = true;
  localDW->obj.isInitialized = 1;
  localDW->obj.NumChannels = 1;
  localDW->obj.pMID.isInitialized = 0;
  localDW->obj.isSetupComplete = true;
}

// Output and update for referenced model: 'filter_current'
void filter_current(const FOCOutputs *rtu_ControlOutputs, real32_T
                    *rty_FilteredCurrent, DW_filter_current_f_T *localDW)
{
  real32_T ind2;
  real32_T p;
  real32_T tmp;
  real32_T tmp_0;
  real32_T vprev;
  real32_T y;
  boolean_T exitg1;

  // MATLABSystem: '<Root>/Median Filter'
  if (localDW->obj.pMID.isInitialized != 1) {
    localDW->obj.pMID.isInitialized = 1;
    localDW->obj.pMID.isSetupComplete = true;
    filter_current_MedianFilter_resetImpl(&localDW->obj.pMID);
  }

  vprev = localDW->obj.pMID.pBuf[static_cast<int32_T>(localDW->obj.pMID.pIdx) -
    1];
  localDW->obj.pMID.pBuf[static_cast<int32_T>(localDW->obj.pMID.pIdx) - 1] =
    rtu_ControlOutputs->Iq_fbk;
  p = localDW->obj.pMID.pPos[static_cast<int32_T>(localDW->obj.pMID.pIdx) - 1];
  localDW->obj.pMID.pIdx++;
  if (localDW->obj.pMID.pWinLen + 1.0F == localDW->obj.pMID.pIdx) {
    localDW->obj.pMID.pIdx = 1.0F;
  }

  if (p > localDW->obj.pMID.pMidHeap) {
    if (vprev < rtu_ControlOutputs->Iq_fbk) {
      filter_current_MedianFilter_trickleDownMin(&localDW->obj.pMID, (p -
        localDW->obj.pMID.pMidHeap) * 2.0F);
    } else {
      vprev = p - localDW->obj.pMID.pMidHeap;
      exitg1 = false;
      while ((!exitg1) && (vprev > 0.0F)) {
        p = vprev / 2.0F;
        y = vprev + localDW->obj.pMID.pMidHeap;
        ind2 = std::floor(p) + localDW->obj.pMID.pMidHeap;
        tmp = localDW->obj.pMID.pHeap[static_cast<int32_T>(y) - 1];
        tmp_0 = localDW->obj.pMID.pHeap[static_cast<int32_T>(ind2) - 1];
        if (localDW->obj.pMID.pBuf[static_cast<int32_T>(tmp) - 1] >=
            localDW->obj.pMID.pBuf[static_cast<int32_T>(tmp_0) - 1]) {
          exitg1 = true;
        } else {
          localDW->obj.pMID.pHeap[static_cast<int32_T>(y) - 1] = tmp_0;
          localDW->obj.pMID.pHeap[static_cast<int32_T>(ind2) - 1] = tmp;
          localDW->obj.pMID.pPos[static_cast<int32_T>(localDW->obj.pMID.pHeap[
            static_cast<int32_T>(y) - 1]) - 1] = y;
          localDW->obj.pMID.pPos[static_cast<int32_T>(localDW->obj.pMID.pHeap[
            static_cast<int32_T>(ind2) - 1]) - 1] = ind2;
          vprev = std::floor(p);
        }
      }

      if (vprev == 0.0F) {
        filter_current_MedianFilter_trickleDownMax(&localDW->obj.pMID, -1.0F);
      }
    }
  } else if (p < localDW->obj.pMID.pMidHeap) {
    if (rtu_ControlOutputs->Iq_fbk < vprev) {
      filter_current_MedianFilter_trickleDownMax(&localDW->obj.pMID, (p -
        localDW->obj.pMID.pMidHeap) * 2.0F);
    } else {
      vprev = p - localDW->obj.pMID.pMidHeap;
      exitg1 = false;
      while ((!exitg1) && (vprev < 0.0F)) {
        p = vprev / 2.0F;
        if (p < 0.0F) {
          y = std::ceil(p);
        } else {
          y = -0.0F;
        }

        y += localDW->obj.pMID.pMidHeap;
        ind2 = vprev + localDW->obj.pMID.pMidHeap;
        tmp = localDW->obj.pMID.pHeap[static_cast<int32_T>(y) - 1];
        tmp_0 = localDW->obj.pMID.pHeap[static_cast<int32_T>(ind2) - 1];
        if (localDW->obj.pMID.pBuf[static_cast<int32_T>(tmp) - 1] >=
            localDW->obj.pMID.pBuf[static_cast<int32_T>(tmp_0) - 1]) {
          exitg1 = true;
        } else {
          localDW->obj.pMID.pHeap[static_cast<int32_T>(y) - 1] = tmp_0;
          localDW->obj.pMID.pHeap[static_cast<int32_T>(ind2) - 1] = tmp;
          localDW->obj.pMID.pPos[static_cast<int32_T>(localDW->obj.pMID.pHeap[
            static_cast<int32_T>(y) - 1]) - 1] = y;
          localDW->obj.pMID.pPos[static_cast<int32_T>(localDW->obj.pMID.pHeap[
            static_cast<int32_T>(ind2) - 1]) - 1] = ind2;
          if (p < 0.0F) {
            vprev = std::ceil(p);
          } else {
            vprev = -0.0F;
          }
        }
      }

      if (vprev == 0.0F) {
        filter_current_MedianFilter_trickleDownMin(&localDW->obj.pMID, 1.0F);
      }
    }
  } else {
    if (localDW->obj.pMID.pMaxHeapLength != 0.0F) {
      filter_current_MedianFilter_trickleDownMax(&localDW->obj.pMID, -1.0F);
    }

    if (localDW->obj.pMID.pMinHeapLength > 0.0F) {
      filter_current_MedianFilter_trickleDownMin(&localDW->obj.pMID, 1.0F);
    }
  }

  // SignalConversion generated from: '<Root>/Median Filter' incorporates:
  //   MATLABSystem: '<Root>/Median Filter'
  //
  *rty_FilteredCurrent = (localDW->obj.pMID.pBuf[static_cast<int32_T>
    (localDW->obj.pMID.pHeap[static_cast<int32_T>(localDW->obj.pMID.pMidHeap -
    1.0F) - 1]) - 1] + localDW->obj.pMID.pBuf[static_cast<int32_T>
    (localDW->obj.pMID.pHeap[static_cast<int32_T>(localDW->obj.pMID.pMidHeap) -
     1]) - 1]) / 2.0F;
}

// Termination for referenced model: 'filter_current'
void filter_current_Term(DW_filter_current_f_T *localDW)
{
  // Terminate for MATLABSystem: '<Root>/Median Filter'
  if (!localDW->obj.matlabCodegenIsDeleted) {
    localDW->obj.matlabCodegenIsDeleted = true;
    if ((localDW->obj.isInitialized == 1) && localDW->obj.isSetupComplete) {
      localDW->obj.NumChannels = -1;
      if (localDW->obj.pMID.isInitialized == 1) {
        localDW->obj.pMID.isInitialized = 2;
      }
    }
  }

  // End of Terminate for MATLABSystem: '<Root>/Median Filter'
}

//
// File trailer for generated code.
//
// [EOF]
//
