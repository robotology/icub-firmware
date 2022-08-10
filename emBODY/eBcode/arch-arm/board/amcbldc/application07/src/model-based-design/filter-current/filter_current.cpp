//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: filter_current.cpp
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
#include "filter_current.h"
#include "filter_current_types.h"
#include "rtwtypes.h"
#include <cmath>
#include "filter_current_private.h"

MdlrefDW_filter_current_T filter_current_MdlrefDW;

// Block states (default storage)
DW_filter_current_f_T filter_current_DW;

// Forward declaration for local functions
static void filter_MedianFilterCG_resetImpl(c_dsp_internal_MedianFilterCG_T *obj);
static void f_MedianFilterCG_trickleDownMax(c_dsp_internal_MedianFilterCG_T *obj,
  real32_T i);
static void f_MedianFilterCG_trickleDownMin(c_dsp_internal_MedianFilterCG_T *obj,
  real32_T i);
static void filter_MedianFilterCG_resetImpl(c_dsp_internal_MedianFilterCG_T *obj)
{
  real32_T cnt1;
  real32_T cnt2;
  for (int32_T i = 0; i < 10; i++) {
    obj->pBuf[i] = 0.0F;
    obj->pPos[i] = 0.0F;
    obj->pHeap[i] = 0.0F;
  }

  obj->pWinLen = 10.0F;
  obj->pIdx = obj->pWinLen;
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
  for (int32_T i = 0; i < 10; i++) {
    if (static_cast<int32_T>(std::fmod(-static_cast<real32_T>(i) + 10.0F, 2.0F))
        == 0) {
      obj->pPos[9 - i] = cnt1;
      cnt1++;
    } else {
      obj->pPos[9 - i] = cnt2;
      cnt2--;
    }

    obj->pHeap[static_cast<int32_T>(obj->pPos[9 - i]) - 1] =
      -static_cast<real32_T>(i) + 10.0F;
  }
}

static void f_MedianFilterCG_trickleDownMax(c_dsp_internal_MedianFilterCG_T *obj,
  real32_T i)
{
  boolean_T exitg1;
  exitg1 = false;
  while ((!exitg1) && (i >= -obj->pMaxHeapLength)) {
    real32_T ind2;
    real32_T tmp;
    real32_T u;
    real32_T u_tmp;
    if ((i < -1.0F) && (i > -obj->pMaxHeapLength) && (obj->pBuf
         [static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(i + obj->pMidHeap)
          - 1]) - 1] < obj->pBuf[static_cast<int32_T>(obj->pHeap
          [static_cast<int32_T>((i - 1.0F) + obj->pMidHeap) - 1]) - 1])) {
      i--;
    }

    u_tmp = i / 2.0F;
    if (u_tmp < 0.0F) {
      u = std::ceil(u_tmp);
    } else {
      u = std::floor(u_tmp);
    }

    ind2 = i + obj->pMidHeap;
    tmp = obj->pHeap[static_cast<int32_T>(ind2) - 1];
    if (!(obj->pBuf[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(u +
           obj->pMidHeap) - 1]) - 1] < obj->pBuf[static_cast<int32_T>(tmp) - 1]))
    {
      exitg1 = true;
    } else {
      if (u_tmp < 0.0F) {
        u = std::ceil(u_tmp);
      } else {
        u = std::floor(u_tmp);
      }

      u_tmp = u + obj->pMidHeap;
      u = obj->pHeap[static_cast<int32_T>(u_tmp) - 1];
      obj->pHeap[static_cast<int32_T>(u_tmp) - 1] = tmp;
      obj->pHeap[static_cast<int32_T>(ind2) - 1] = u;
      obj->pPos[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(u_tmp) - 1])
        - 1] = u_tmp;
      obj->pPos[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(ind2) - 1])
        - 1] = ind2;
      i *= 2.0F;
    }
  }
}

static void f_MedianFilterCG_trickleDownMin(c_dsp_internal_MedianFilterCG_T *obj,
  real32_T i)
{
  boolean_T exitg1;
  exitg1 = false;
  while ((!exitg1) && (i <= obj->pMinHeapLength)) {
    real32_T ind1;
    real32_T temp;
    real32_T tmp;
    real32_T u_tmp;
    if ((i > 1.0F) && (i < obj->pMinHeapLength) && (obj->pBuf
         [static_cast<int32_T>(obj->pHeap[static_cast<int32_T>((i + 1.0F) +
           obj->pMidHeap) - 1]) - 1] < obj->pBuf[static_cast<int32_T>(obj->
          pHeap[static_cast<int32_T>(i + obj->pMidHeap) - 1]) - 1])) {
      i++;
    }

    u_tmp = i / 2.0F;
    if (u_tmp < 0.0F) {
      temp = std::ceil(u_tmp);
    } else {
      temp = std::floor(u_tmp);
    }

    ind1 = i + obj->pMidHeap;
    tmp = obj->pHeap[static_cast<int32_T>(ind1) - 1];
    if (!(obj->pBuf[static_cast<int32_T>(tmp) - 1] < obj->pBuf
          [static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(temp +
           obj->pMidHeap) - 1]) - 1])) {
      exitg1 = true;
    } else {
      if (u_tmp < 0.0F) {
        temp = std::ceil(u_tmp);
      } else {
        temp = std::floor(u_tmp);
      }

      u_tmp = temp + obj->pMidHeap;
      obj->pHeap[static_cast<int32_T>(ind1) - 1] = obj->pHeap
        [static_cast<int32_T>(u_tmp) - 1];
      obj->pHeap[static_cast<int32_T>(u_tmp) - 1] = tmp;
      obj->pPos[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(ind1) - 1])
        - 1] = ind1;
      obj->pPos[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(u_tmp) - 1])
        - 1] = u_tmp;
      i *= 2.0F;
    }
  }
}

// System initialize for referenced model: 'filter_current'
void filter_current_Init(void)
{
  // Start for MATLABSystem: '<Root>/Median Filter'
  filter_current_DW.obj.matlabCodegenIsDeleted = false;
  filter_current_DW.objisempty = true;
  filter_current_DW.obj.isInitialized = 1;
  filter_current_DW.obj.NumChannels = 1;
  filter_current_DW.obj.pMID.isInitialized = 0;
  filter_current_DW.obj.isSetupComplete = true;

  // InitializeConditions for MATLABSystem: '<Root>/Median Filter'
  if (filter_current_DW.obj.pMID.isInitialized == 1) {
    filter_MedianFilterCG_resetImpl(&filter_current_DW.obj.pMID);
  }

  // End of InitializeConditions for MATLABSystem: '<Root>/Median Filter'
}

// Output and update for referenced model: 'filter_current'
void filter_current(const ControlOutputs *rtu_ControlOutputs, MotorCurrent
                    *rty_FilteredCurrent)
{
  c_dsp_internal_MedianFilterCG_T *obj;
  real32_T p;
  real32_T vprev;

  // MATLABSystem: '<Root>/Median Filter'
  obj = &filter_current_DW.obj.pMID;
  if (filter_current_DW.obj.pMID.isInitialized != 1) {
    filter_current_DW.obj.pMID.isSetupComplete = false;
    filter_current_DW.obj.pMID.isInitialized = 1;
    filter_current_DW.obj.pMID.isSetupComplete = true;
    filter_MedianFilterCG_resetImpl(&filter_current_DW.obj.pMID);
  }

  vprev = filter_current_DW.obj.pMID.pBuf[static_cast<int32_T>
    (filter_current_DW.obj.pMID.pIdx) - 1];
  filter_current_DW.obj.pMID.pBuf[static_cast<int32_T>
    (filter_current_DW.obj.pMID.pIdx) - 1] = rtu_ControlOutputs->Iq_fbk.current;
  p = filter_current_DW.obj.pMID.pPos[static_cast<int32_T>
    (filter_current_DW.obj.pMID.pIdx) - 1];
  filter_current_DW.obj.pMID.pIdx++;
  if (filter_current_DW.obj.pMID.pWinLen + 1.0F ==
      filter_current_DW.obj.pMID.pIdx) {
    filter_current_DW.obj.pMID.pIdx = 1.0F;
  }

  if (p > filter_current_DW.obj.pMID.pMidHeap) {
    if (vprev < rtu_ControlOutputs->Iq_fbk.current) {
      vprev = p - filter_current_DW.obj.pMID.pMidHeap;
      f_MedianFilterCG_trickleDownMin(&filter_current_DW.obj.pMID, vprev * 2.0F);
    } else {
      boolean_T exitg1;
      vprev = p - filter_current_DW.obj.pMID.pMidHeap;
      exitg1 = false;
      while ((!exitg1) && (vprev > 0.0F)) {
        boolean_T flag;
        flag = (obj->pBuf[static_cast<int32_T>(obj->pHeap[static_cast<int32_T>
                 (vprev + obj->pMidHeap) - 1]) - 1] < obj->pBuf
                [static_cast<int32_T>(obj->pHeap[static_cast<int32_T>(std::floor
                  (vprev / 2.0F) + obj->pMidHeap) - 1]) - 1]);
        if (!flag) {
          exitg1 = true;
        } else {
          real32_T ind2;
          real32_T temp;
          p = vprev + obj->pMidHeap;
          ind2 = std::floor(vprev / 2.0F) + obj->pMidHeap;
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
        f_MedianFilterCG_trickleDownMax(&filter_current_DW.obj.pMID, -1.0F);
      }
    }
  } else if (p < filter_current_DW.obj.pMID.pMidHeap) {
    if (rtu_ControlOutputs->Iq_fbk.current < vprev) {
      vprev = p - filter_current_DW.obj.pMID.pMidHeap;
      f_MedianFilterCG_trickleDownMax(&filter_current_DW.obj.pMID, vprev * 2.0F);
    } else {
      boolean_T exitg1;
      vprev = p - filter_current_DW.obj.pMID.pMidHeap;
      exitg1 = false;
      while ((!exitg1) && (vprev < 0.0F)) {
        real32_T u_tmp;
        boolean_T flag;
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
          real32_T ind2;
          real32_T temp;
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
        f_MedianFilterCG_trickleDownMin(&filter_current_DW.obj.pMID, 1.0F);
      }
    }
  } else {
    if (filter_current_DW.obj.pMID.pMaxHeapLength != 0.0F) {
      f_MedianFilterCG_trickleDownMax(&filter_current_DW.obj.pMID, -1.0F);
    }

    if (filter_current_DW.obj.pMID.pMinHeapLength > 0.0F) {
      f_MedianFilterCG_trickleDownMin(&filter_current_DW.obj.pMID, 1.0F);
    }
  }

  vprev = filter_current_DW.obj.pMID.pBuf[static_cast<int32_T>
    (filter_current_DW.obj.pMID.pHeap[static_cast<int32_T>
     (filter_current_DW.obj.pMID.pMidHeap) - 1]) - 1];
  vprev += filter_current_DW.obj.pMID.pBuf[static_cast<int32_T>
    (filter_current_DW.obj.pMID.pHeap[static_cast<int32_T>
     (filter_current_DW.obj.pMID.pMidHeap - 1.0F) - 1]) - 1];
  rty_FilteredCurrent->current = vprev / 2.0F;

  // End of MATLABSystem: '<Root>/Median Filter'
}

// Termination for referenced model: 'filter_current'
void filter_current_Term(void)
{
  // Terminate for MATLABSystem: '<Root>/Median Filter'
  if (!filter_current_DW.obj.matlabCodegenIsDeleted) {
    filter_current_DW.obj.matlabCodegenIsDeleted = true;
    if ((filter_current_DW.obj.isInitialized == 1) &&
        filter_current_DW.obj.isSetupComplete) {
      filter_current_DW.obj.NumChannels = -1;
      if (filter_current_DW.obj.pMID.isInitialized == 1) {
        filter_current_DW.obj.pMID.isInitialized = 2;
      }
    }
  }

  // End of Terminate for MATLABSystem: '<Root>/Median Filter'
}

// Model initialize function
void filter_current_initialize(const char_T **rt_errorStatus)
{
  RT_MODEL_filter_current_T *const filter_current_M =
    &(filter_current_MdlrefDW.rtm);

  // Registration code

  // initialize error status
  rtmSetErrorStatusPointer(filter_current_M, rt_errorStatus);
}

//
// File trailer for generated code.
//
// [EOF]
//
