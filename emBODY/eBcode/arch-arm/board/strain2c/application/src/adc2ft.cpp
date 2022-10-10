//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: adc2ft.cpp
//
// Code generated for Simulink model 'adc2ft'.
//
// Model version                  : 2.41
// Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
// C/C++ source code generated on : Tue Apr 13 09:39:18 2021
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#include "adc2ft.h"
#ifndef UCHAR_MAX
#include <limits.h>
#endif

#if ( UCHAR_MAX != (0xFFU) ) || ( SCHAR_MAX != (0x7F) )
#error Code was generated for compiler with different sized uchar/char. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

#if ( USHRT_MAX != (0xFFFFU) ) || ( SHRT_MAX != (0x7FFF) )
#error Code was generated for compiler with different sized ushort/short. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

#if ( UINT_MAX != (0xFFFFFFFFU) ) || ( INT_MAX != (0x7FFFFFFF) )
#error Code was generated for compiler with different sized uint/int. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

#if ( ULONG_MAX != (0xFFFFFFFFU) ) || ( LONG_MAX != (0x7FFFFFFF) )
#error Code was generated for compiler with different sized ulong/long. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

// Skipping ulong_long/long_long check: insufficient preprocessor integer range. 
namespace adc2ft_ns
{
  // Model step function
  void adc2ft_class::step()
  {
    int32_T i;
    int32_T i_0;
    real32_T tmp[36];
    real32_T tmp_0[6];
    real32_T tmp_1[6];
    real32_T tmp_2;
    for (i = 0; i < 36; i++) {
      // Product: '<Root>/Divide1' incorporates:
      //   Constant: '<Root>/Constant'
      //   Constant: '<Root>/Constant2'

      rtDW.Divide1[i] = static_cast<real32_T>(rtP.calibration_matrix[i]) /
        32768.0F;
    }

    for (i = 0; i < 6; i++) {
      // DataTypeConversion: '<Root>/Data Type Conversion' incorporates:
      //   Inport: '<Root>/adc'

      rtDW.DataTypeConversion[i] = static_cast<real32_T>(rtU.adc[i]);

      // DataTypeConversion: '<Root>/Data Type Conversion2' incorporates:
      //   Constant: '<Root>/Constant1'

      rtDW.DataTypeConversion2[i] = static_cast<real32_T>
        (rtP.calibration_offsets[i]);

      // Sum: '<Root>/Sum' incorporates:
      //   Constant: '<Root>/Constant'
      //   DataTypeConversion: '<Root>/Data Type Conversion'
      //   DataTypeConversion: '<Root>/Data Type Conversion2'

      rtDW.Sum[i] = (rtDW.DataTypeConversion[i] - 32768.0F) +
        rtDW.DataTypeConversion2[i];

      // Product: '<Root>/Divide' incorporates:
      //   Constant: '<Root>/Constant'
      //   Sum: '<Root>/Sum'

      rtDW.Divide[i] = 3.05175781E-5F * rtDW.Sum[i];
    }

    // Product: '<Root>/Product' incorporates:
    //   Product: '<Root>/Divide'
    //   Product: '<Root>/Divide1'

    for (i_0 = 0; i_0 < 36; i_0++) {
      tmp[i_0] = rtDW.Divide1[i_0];
    }

    for (i = 0; i < 6; i++) {
      tmp_0[i] = rtDW.Divide[i];
      tmp_1[i] = 0.0F;
    }

    for (i = 0; i < 6; i++) {
      for (i_0 = 0; i_0 < 6; i_0++) {
        tmp_2 = tmp_1[i];
        tmp_2 += tmp[6 * i + i_0] * tmp_0[i_0];
        tmp_1[i] = tmp_2;
      }

      // Product: '<Root>/Product'
      rtDW.Product[i] = tmp_1[i];

      // Outport: '<Root>/ft_q15' incorporates:
      //   DataTypeConversion: '<Root>/Data Type Conversion1'
      //   Product: '<Root>/Product'

      rtY.ft_q15[i] = static_cast<int16_T>(std::floor(rtDW.Product[i] * 32768.0F));
    }

    // End of Product: '<Root>/Product'
  }

  // Model initialize function
  void adc2ft_class::initialize()
  {
    // (no initialization code required)
  }

  // Constructor
  adc2ft_class::adc2ft_class() :
    rtU(),
    rtY(),
    rtDW(),
    rtM()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  adc2ft_class::~adc2ft_class()
  {
    // Currently there is no destructor body generated.
  }

  // Real-Time Model get method
  adc2ft_ns::adc2ft_class::RT_MODEL * adc2ft_class::getRTM()
  {
    return (&rtM);
  }
}

//
// File trailer for generated code.
//
// [EOF]
//
