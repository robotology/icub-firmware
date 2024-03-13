//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: filter_current.cpp
//
// Code generated for Simulink model 'filter_current'.
//
// Model version                  : 6.3
// Simulink Coder version         : 23.2 (R2023b) 01-Aug-2023
// C/C++ source code generated on : Wed Mar 13 10:36:38 2024
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
#include "filter_current_private.h"

// Forward declaration for local functions
static void filter_current_MedianFilterCG_resetImpl
  (c_dsp_internal_MedianFilterCG_filter_current_T *obj);
static void filter_current_MedianFilterCG_trickleDownMax
  (c_dsp_internal_MedianFilterCG_filter_current_T *obj, real32_T i);
static void filter_current_MedianFilterCG_trickleDownMin
  (c_dsp_internal_MedianFilterCG_filter_current_T *obj, real32_T i);
static void filter_current_MedianFilterCG_resetImpl
  (c_dsp_internal_MedianFilterCG_filter_current_T *obj)
{
  real32_T cnt1;
  real32_T cnt2;

  // Start for MATLABSystem: '<Root>/Median Filter'
  std::memset(&obj->pBuf[0], 0, sizeof(real32_T) << 5U);
  std::memset(&obj->pPos[0], 0, sizeof(real32_T) << 5U);
  std::memset(&obj->pHeap[0], 0, sizeof(real32_T) << 5U);
  obj->pWinLen = 32.0F;
  obj->pIdx = obj->pWinLen;

  // Start for MATLABSystem: '<Root>/Median Filter'
  obj->pMidHeap = std::ceil((obj->pWinLen + 1.0F) / 2.0F);
  cnt1 = (obj->pWinLen - 1.0F) / 2.0F;
  if (cnt1 < 0.0F) {
    obj->pMinHeapLength = std::ceil(cnt1);
  } else {
    obj->pMinHeapLength = std::floor(cnt1);
  }

  cnt1 = obj->pWinLen / 2.0F;
  if (cnt1 < 0.0F) {
    obj->pMaxHeapLength = std::ceil(cnt1);
  } else {
    obj->pMaxHeapLength = std::floor(cnt1);
  }

  cnt1 = 1.0F;
  cnt2 = obj->pWinLen;
  for (int32_T i = 0; i < 32; i++) {
    // Start for MATLABSystem: '<Root>/Median Filter'
    if (std::fmod(32.0F - static_cast<real32_T>(i), 2.0F) == 0.0F) {
      obj->pPos[31 - i] = cnt1;
      cnt1++;
    } else {
      obj->pPos[31 - i] = cnt2;
      cnt2--;
    }

    obj->pHeap[static_cast<int32_T>(obj->pPos[31 - i]) - 1] = 32.0F -
      static_cast<real32_T>(i);
  }
}

static void filter_current_MedianFilterCG_trickleDownMax
  (c_dsp_internal_MedianFilterCG_filter_current_T *obj, real32_T i)
{
  boolean_T exitg1;
  exitg1 = false;
  while ((!exitg1) && (i >= -obj->pMaxHeapLength)) {
    real32_T ind2;
    real32_T temp;
    real32_T tmp;
    real32_T u_tmp;
    if ((i < -1.0F) && (i > -obj->pMaxHeapLength) && (obj->pBuf[static_cast<
         int32_T>(obj->pHeap[static_cast<int32_T>(i + obj->pMidHeap) - 1]) - 1] <
         obj->pBuf[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>((i -
            1.0F) + obj->pMidHeap) - 1]) - 1])) {
      i--;
    }

    u_tmp = i / 2.0F;
    if (u_tmp < 0.0F) {
      temp = std::ceil(u_tmp);
    } else {
      temp = std::floor(u_tmp);
    }

    ind2 = i + obj->pMidHeap;
    tmp = obj->pHeap[static_cast<int32_T>(ind2) - 1];
    if (!(obj->pBuf[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(temp +
           obj->pMidHeap) - 1]) - 1] < obj->pBuf[static_cast<int32_T>(tmp) - 1]))
    {
      exitg1 = true;
    } else {
      if (u_tmp < 0.0F) {
        temp = std::ceil(u_tmp);
      } else {
        temp = std::floor(u_tmp);
      }

      u_tmp = temp + obj->pMidHeap;
      temp = obj->pHeap[static_cast<int32_T>(u_tmp) - 1];
      obj->pHeap[static_cast<int32_T>(u_tmp) - 1] = tmp;
      obj->pHeap[static_cast<int32_T>(ind2) - 1] = temp;
      obj->pPos[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(u_tmp) - 1])
        - 1] = u_tmp;
      obj->pPos[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(ind2) - 1])
        - 1] = ind2;
      i *= 2.0F;
    }
  }
}

static void filter_current_MedianFilterCG_trickleDownMin
  (c_dsp_internal_MedianFilterCG_filter_current_T *obj, real32_T i)
{
  boolean_T exitg1;
  exitg1 = false;
  while ((!exitg1) && (i <= obj->pMinHeapLength)) {
    real32_T ind1;
    real32_T tmp;
    real32_T tmp_0;
    real32_T u_tmp;
    if ((i > 1.0F) && (i < obj->pMinHeapLength) && (obj->pBuf
         [static_cast<int32_T>(obj->pHeap[static_cast<int32_T>((i + 1.0F) +
           obj->pMidHeap) - 1]) - 1] < obj->pBuf[static_cast<int32_T>(obj->
          pHeap[static_cast<int32_T>(i + obj->pMidHeap) - 1]) - 1])) {
      i++;
    }

    u_tmp = i / 2.0F;
    if (u_tmp < 0.0F) {
      tmp = std::ceil(u_tmp);
    } else {
      tmp = std::floor(u_tmp);
    }

    ind1 = i + obj->pMidHeap;
    tmp_0 = obj->pHeap[static_cast<int32_T>(ind1) - 1];
    if (!(obj->pBuf[static_cast<int32_T>(tmp_0) - 1] < obj->pBuf
          [static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(tmp +
           obj->pMidHeap) - 1]) - 1])) {
      exitg1 = true;
    } else {
      if (u_tmp < 0.0F) {
        tmp = std::ceil(u_tmp);
      } else {
        tmp = std::floor(u_tmp);
      }

      u_tmp = tmp + obj->pMidHeap;
      obj->pHeap[static_cast<int32_T>(ind1) - 1] = obj->pHeap
        [static_cast<int32_T>(u_tmp) - 1];
      obj->pHeap[static_cast<int32_T>(u_tmp) - 1] = tmp_0;
      obj->pPos[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(ind1) - 1])
        - 1] = ind1;
      obj->pPos[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(u_tmp) - 1])
        - 1] = u_tmp;
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
void filter_current(const ControlOutputs *rtu_ControlOutputs, MotorCurrent
                    *rty_FilteredCurrent, DW_filter_current_f_T *localDW)
{
  c_dsp_internal_MedianFilterCG_filter_current_T *obj;
  int32_T vprev_tmp;
  real32_T ind2;
  real32_T p;
  real32_T temp;
  real32_T u_tmp;
  real32_T vprev;
  boolean_T exitg1;
  boolean_T flag;

  // MATLABSystem: '<Root>/Median Filter'
  obj = &localDW->obj.pMID;
  if (localDW->obj.pMID.isInitialized != 1) {
    localDW->obj.pMID.isInitialized = 1;
    localDW->obj.pMID.isSetupComplete = true;
    filter_current_MedianFilterCG_resetImpl(&localDW->obj.pMID);
  }

  vprev_tmp = static_cast<int32_T>(localDW->obj.pMID.pIdx) - 1;
  vprev = localDW->obj.pMID.pBuf[vprev_tmp];
  localDW->obj.pMID.pBuf[vprev_tmp] = rtu_ControlOutputs->Iq_fbk.current;
  p = localDW->obj.pMID.pPos[static_cast<int32_T>(localDW->obj.pMID.pIdx) - 1];
  localDW->obj.pMID.pIdx++;
  if (localDW->obj.pMID.pWinLen + 1.0F == localDW->obj.pMID.pIdx) {
    localDW->obj.pMID.pIdx = 1.0F;
  }

  if (p > localDW->obj.pMID.pMidHeap) {
    if (vprev < rtu_ControlOutputs->Iq_fbk.current) {
      vprev = p - localDW->obj.pMID.pMidHeap;
      filter_current_MedianFilterCG_trickleDownMin(&localDW->obj.pMID, vprev *
        2.0F);
    } else {
      vprev = p - localDW->obj.pMID.pMidHeap;
      exitg1 = false;
      while ((!exitg1) && (vprev > 0.0F)) {
        u_tmp = std::floor(vprev / 2.0F);
        flag = (obj->pBuf[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>
                 (vprev + obj->pMidHeap) - 1]) - 1] < obj->pBuf
                [static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(u_tmp +
                  obj->pMidHeap) - 1]) - 1]);
        if (!flag) {
          exitg1 = true;
        } else {
          p = vprev + obj->pMidHeap;
          ind2 = u_tmp + obj->pMidHeap;
          temp = obj->pHeap[static_cast<int32_T>(p) - 1];
          obj->pHeap[static_cast<int32_T>(p) - 1] = obj->pHeap
            [static_cast<int32_T>(ind2) - 1];
          obj->pHeap[static_cast<int32_T>(ind2) - 1] = temp;
          obj->pPos[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(p) - 1])
            - 1] = p;
          obj->pPos[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(ind2) -
            1]) - 1] = ind2;
          vprev = std::floor(vprev / 2.0F);
        }
      }

      if (vprev == 0.0F) {
        filter_current_MedianFilterCG_trickleDownMax(&localDW->obj.pMID, -1.0F);
      }
    }
  } else if (p < localDW->obj.pMID.pMidHeap) {
    if (rtu_ControlOutputs->Iq_fbk.current < vprev) {
      vprev = p - localDW->obj.pMID.pMidHeap;
      filter_current_MedianFilterCG_trickleDownMax(&localDW->obj.pMID, vprev *
        2.0F);
    } else {
      vprev = p - localDW->obj.pMID.pMidHeap;
      exitg1 = false;
      while ((!exitg1) && (vprev < 0.0F)) {
        u_tmp = vprev / 2.0F;
        if (u_tmp < 0.0F) {
          p = std::ceil(u_tmp);
        } else {
          p = -0.0F;
        }

        flag = (obj->pBuf[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(p
                  + obj->pMidHeap) - 1]) - 1] < obj->pBuf[static_cast<int32_T>
                (obj->pHeap[static_cast<int32_T>(vprev + obj->pMidHeap) - 1]) -
                1]);
        if (!flag) {
          exitg1 = true;
        } else {
          if (u_tmp < 0.0F) {
            p = std::ceil(u_tmp);
          } else {
            p = -0.0F;
          }

          p += obj->pMidHeap;
          ind2 = vprev + obj->pMidHeap;
          temp = obj->pHeap[static_cast<int32_T>(p) - 1];
          obj->pHeap[static_cast<int32_T>(p) - 1] = obj->pHeap
            [static_cast<int32_T>(ind2) - 1];
          obj->pHeap[static_cast<int32_T>(ind2) - 1] = temp;
          obj->pPos[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(p) - 1])
            - 1] = p;
          obj->pPos[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(ind2) -
            1]) - 1] = ind2;
          if (u_tmp < 0.0F) {
            vprev = std::ceil(u_tmp);
          } else {
            vprev = -0.0F;
          }
        }
      }

      if (vprev == 0.0F) {
        filter_current_MedianFilterCG_trickleDownMin(&localDW->obj.pMID, 1.0F);
      }
    }
  } else {
    if (localDW->obj.pMID.pMaxHeapLength != 0.0F) {
      filter_current_MedianFilterCG_trickleDownMax(&localDW->obj.pMID, -1.0F);
    }

    if (localDW->obj.pMID.pMinHeapLength > 0.0F) {
      filter_current_MedianFilterCG_trickleDownMin(&localDW->obj.pMID, 1.0F);
    }
  }

  vprev = localDW->obj.pMID.pBuf[static_cast<int32_T>(localDW->obj.pMID.pHeap[
    static_cast<int32_T>(localDW->obj.pMID.pMidHeap) - 1]) - 1];
  vprev += localDW->obj.pMID.pBuf[static_cast<int32_T>(localDW->obj.pMID.pHeap[
    static_cast<int32_T>(localDW->obj.pMID.pMidHeap - 1.0F) - 1]) - 1];
  rty_FilteredCurrent->current = vprev / 2.0F;

  // End of MATLABSystem: '<Root>/Median Filter'
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

// Model initialize function
void filter_current_initialize(const char_T **rt_errorStatus,
  RT_MODEL_filter_current_T *const filter_current_M, DW_filter_current_f_T
  *localDW)
{
  // Registration code

  // initialize error status
  rtmSetErrorStatusPointer(filter_current_M, rt_errorStatus);

  // states (dwork)
  (void) std::memset(static_cast<void *>(localDW), 0,
                     sizeof(DW_filter_current_f_T));
}

//
// File trailer for generated code.
//
// [EOF]
//
