//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: kalman_filter.h
//
// Code generated for Simulink model 'kalman_filter'.
//
// Model version                  : 1.10
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Mon Apr  4 11:53:10 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_kalman_filter_h_
#define RTW_HEADER_kalman_filter_h_
#include "rtwtypes.h"
#include "kalman_filter_types.h"

// Macros for accessing real-time model data structure
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

// Class declaration for model kalman_filter
namespace kalmanfilter
{
  class KalmanFilter
  {
    // public data and function members
   public:
    // Block signals (default storage)
    struct B_kalman_filter_T {
      real32_T Reshapexhat[3];         // '<S2>/Reshapexhat'
    };

    // Block states (default storage) for system '<Root>'
    struct DW_kalman_filter_T {
      real32_T MemoryP_DSTATE[9];      // '<S2>/MemoryP'
      real32_T MemoryX_DSTATE[3];      // '<S2>/MemoryX'
      boolean_T icLoad;                // '<S2>/MemoryP'
      boolean_T icLoad_g;              // '<S2>/MemoryX'
      boolean_T KalmanFilter_MODE;     // '<Root>/Kalman Filter'
      boolean_T EnabledSubsystem_MODE; // '<S29>/Enabled Subsystem'
      boolean_T MeasurementUpdate_MODE;// '<S22>/MeasurementUpdate'
    };

    // Invariant block signals (default storage)
    struct ConstB_kalman_filter_T {
      real32_T Bkuk[3];                // '<S22>/B[k]*u[k]'
      real32_T Transpose[3];           // '<S23>/Transpose'
      real32_T Transpose2[3];          // '<S23>/Transpose2'
      real32_T Product4;               // '<S23>/Product4'
      real32_T Transpose1[9];          // '<S23>/Transpose1'
      real32_T Dkuk;                   // '<S49>/D[k]*u[k]'
    };

    // Constant parameters (default storage)
    struct ConstP_kalman_filter_T {
      // Computed Parameter: A_Value
      //  Referenced by: '<S2>/A'

      real32_T A_Value[9];

      // Computed Parameter: C_Value
      //  Referenced by: '<S2>/C'

      real32_T C_Value[3];

      // Computed Parameter: G_Value
      //  Referenced by: '<S2>/G'

      real32_T G_Value[9];
    };

    // Real-time Model Data Structure
    struct RT_MODEL_kalman_filter_T {
      const char_T * volatile errorStatus;
    };

    // Copy Constructor
    KalmanFilter(KalmanFilter const&) =delete;

    // Assignment Operator
    KalmanFilter& operator= (KalmanFilter const&) & = delete;

    // Real-Time Model get method
    kalmanfilter::KalmanFilter::RT_MODEL_kalman_filter_T * getRTM();

    // model initialize function
    void initialize();

    // model step function
    void step(const boolean_T &arg_enable, const real32_T &arg_y1, const
              real32_T (&arg_x0)[3], const real32_T &arg_P0, const real32_T
              (&arg_Q)[3], const real32_T &arg_R, real32_T (&arg_xhat)[3]);

    // model terminate function
    static void terminate();

    // Constructor
    KalmanFilter();

    // Destructor
    ~KalmanFilter();

    // private data and function members
   private:
    // Block signals
    B_kalman_filter_T kalman_filter_B;

    // Block states
    DW_kalman_filter_T kalman_filter_DW;

    // Real-Time Model
    RT_MODEL_kalman_filter_T kalman_filter_M;
  };
}

extern const kalmanfilter::KalmanFilter::ConstB_kalman_filter_T
  kalman_filter_ConstB;                // constant block i/o

// Constant parameters (default storage)
extern const kalmanfilter::KalmanFilter::ConstP_kalman_filter_T
  kalman_filter_ConstP;

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S6>/Data Type Duplicate' : Unused code path elimination
//  Block '<S7>/Data Type Duplicate' : Unused code path elimination
//  Block '<S8>/Data Type Duplicate' : Unused code path elimination
//  Block '<S9>/Data Type Duplicate' : Unused code path elimination
//  Block '<S11>/Data Type Duplicate' : Unused code path elimination
//  Block '<S12>/Data Type Duplicate' : Unused code path elimination
//  Block '<S13>/Data Type Duplicate' : Unused code path elimination
//  Block '<S14>/Data Type Duplicate' : Unused code path elimination
//  Block '<S19>/Data Type Duplicate' : Unused code path elimination
//  Block '<S21>/Data Type Duplicate' : Unused code path elimination
//  Block '<S38>/CheckSignalProperties' : Unused code path elimination
//  Block '<S39>/CheckSignalProperties' : Unused code path elimination
//  Block '<S40>/CheckSignalProperties' : Unused code path elimination
//  Block '<S42>/CheckSignalProperties' : Unused code path elimination
//  Block '<S44>/CheckSignalProperties' : Unused code path elimination
//  Block '<S14>/Conversion' : Eliminate redundant data type conversion
//  Block '<S19>/Conversion' : Eliminate redundant data type conversion
//  Block '<S22>/Reshape' : Reshape block reduction
//  Block '<S2>/ReshapeX0' : Reshape block reduction
//  Block '<S2>/Reshapeu' : Reshape block reduction
//  Block '<S2>/Reshapey' : Reshape block reduction


//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Use the MATLAB hilite_system command to trace the generated code back
//  to the model.  For example,
//
//  hilite_system('<S3>')    - opens system 3
//  hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'kalman_filter'
//  '<S1>'   : 'kalman_filter/Kalman Filter'
//  '<S2>'   : 'kalman_filter/Kalman Filter/KF'
//  '<S3>'   : 'kalman_filter/Kalman Filter/KF/CalculatePL'
//  '<S4>'   : 'kalman_filter/Kalman Filter/KF/CalculateYhat'
//  '<S5>'   : 'kalman_filter/Kalman Filter/KF/CovarianceOutputConfigurator'
//  '<S6>'   : 'kalman_filter/Kalman Filter/KF/DataTypeConversionA'
//  '<S7>'   : 'kalman_filter/Kalman Filter/KF/DataTypeConversionB'
//  '<S8>'   : 'kalman_filter/Kalman Filter/KF/DataTypeConversionC'
//  '<S9>'   : 'kalman_filter/Kalman Filter/KF/DataTypeConversionD'
//  '<S10>'  : 'kalman_filter/Kalman Filter/KF/DataTypeConversionEnable'
//  '<S11>'  : 'kalman_filter/Kalman Filter/KF/DataTypeConversionG'
//  '<S12>'  : 'kalman_filter/Kalman Filter/KF/DataTypeConversionH'
//  '<S13>'  : 'kalman_filter/Kalman Filter/KF/DataTypeConversionN'
//  '<S14>'  : 'kalman_filter/Kalman Filter/KF/DataTypeConversionP'
//  '<S15>'  : 'kalman_filter/Kalman Filter/KF/DataTypeConversionP0'
//  '<S16>'  : 'kalman_filter/Kalman Filter/KF/DataTypeConversionQ'
//  '<S17>'  : 'kalman_filter/Kalman Filter/KF/DataTypeConversionR'
//  '<S18>'  : 'kalman_filter/Kalman Filter/KF/DataTypeConversionReset'
//  '<S19>'  : 'kalman_filter/Kalman Filter/KF/DataTypeConversionX'
//  '<S20>'  : 'kalman_filter/Kalman Filter/KF/DataTypeConversionX0'
//  '<S21>'  : 'kalman_filter/Kalman Filter/KF/DataTypeConversionu'
//  '<S22>'  : 'kalman_filter/Kalman Filter/KF/Observer'
//  '<S23>'  : 'kalman_filter/Kalman Filter/KF/ReducedQRN'
//  '<S24>'  : 'kalman_filter/Kalman Filter/KF/Reset'
//  '<S25>'  : 'kalman_filter/Kalman Filter/KF/Reshapeyhat'
//  '<S26>'  : 'kalman_filter/Kalman Filter/KF/ScalarExpansionP0'
//  '<S27>'  : 'kalman_filter/Kalman Filter/KF/ScalarExpansionQ'
//  '<S28>'  : 'kalman_filter/Kalman Filter/KF/ScalarExpansionR'
//  '<S29>'  : 'kalman_filter/Kalman Filter/KF/UseCurrentEstimator'
//  '<S30>'  : 'kalman_filter/Kalman Filter/KF/checkA'
//  '<S31>'  : 'kalman_filter/Kalman Filter/KF/checkB'
//  '<S32>'  : 'kalman_filter/Kalman Filter/KF/checkC'
//  '<S33>'  : 'kalman_filter/Kalman Filter/KF/checkD'
//  '<S34>'  : 'kalman_filter/Kalman Filter/KF/checkEnable'
//  '<S35>'  : 'kalman_filter/Kalman Filter/KF/checkG'
//  '<S36>'  : 'kalman_filter/Kalman Filter/KF/checkH'
//  '<S37>'  : 'kalman_filter/Kalman Filter/KF/checkN'
//  '<S38>'  : 'kalman_filter/Kalman Filter/KF/checkP0'
//  '<S39>'  : 'kalman_filter/Kalman Filter/KF/checkQ'
//  '<S40>'  : 'kalman_filter/Kalman Filter/KF/checkR'
//  '<S41>'  : 'kalman_filter/Kalman Filter/KF/checkReset'
//  '<S42>'  : 'kalman_filter/Kalman Filter/KF/checkX0'
//  '<S43>'  : 'kalman_filter/Kalman Filter/KF/checku'
//  '<S44>'  : 'kalman_filter/Kalman Filter/KF/checky'
//  '<S45>'  : 'kalman_filter/Kalman Filter/KF/CalculatePL/Discrete-Time KF - Calculate PLMZ'
//  '<S46>'  : 'kalman_filter/Kalman Filter/KF/CalculateYhat/Ground'
//  '<S47>'  : 'kalman_filter/Kalman Filter/KF/CovarianceOutputConfigurator/decideOutput'
//  '<S48>'  : 'kalman_filter/Kalman Filter/KF/CovarianceOutputConfigurator/decideOutput/SqrtUsedFcn'
//  '<S49>'  : 'kalman_filter/Kalman Filter/KF/Observer/MeasurementUpdate'
//  '<S50>'  : 'kalman_filter/Kalman Filter/KF/ScalarExpansionP0/ScalarExpansion'
//  '<S51>'  : 'kalman_filter/Kalman Filter/KF/ScalarExpansionQ/ScalarExpansion'
//  '<S52>'  : 'kalman_filter/Kalman Filter/KF/ScalarExpansionR/ScalarExpansion'
//  '<S53>'  : 'kalman_filter/Kalman Filter/KF/UseCurrentEstimator/Enabled Subsystem'

#endif                                 // RTW_HEADER_kalman_filter_h_

//
// File trailer for generated code.
//
// [EOF]
//
