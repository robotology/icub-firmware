//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, educational organizations only. Not for
// government, commercial, or other organizational use.
//
// File: wrist_decoupler.h
//
// Code generated for Simulink model 'wrist_decoupler'.
//
// Model version                  : 2.3
// Simulink Coder version         : 9.4 (R2020b) 29-Jul-2020
// C/C++ source code generated on : Tue Nov 17 12:30:21 2020
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#ifndef RTW_HEADER_wrist_decoupler_h_
#define RTW_HEADER_wrist_decoupler_h_
#include <stddef.h>
#include "rtwtypes.h"
#include <cfloat>
#include <cmath>
#include <cstring>
#include "rtwtypes.h"

// Model Code Variants

// Macros for accessing real-time model data structure
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

// Custom Type definition for MATLAB Function: '<S2>/motors2quat'
#ifndef struct_tag_v7m7NEstuUdktFlQqRQnjB
#define struct_tag_v7m7NEstuUdktFlQqRQnjB

struct tag_v7m7NEstuUdktFlQqRQnjB
{
  real_T a;
  real_T b;
  real_T c;
  real_T d;
};

#endif                                 //struct_tag_v7m7NEstuUdktFlQqRQnjB

#ifndef typedef_quaternion
#define typedef_quaternion

typedef tag_v7m7NEstuUdktFlQqRQnjB quaternion;

#endif                                 //typedef_quaternion

extern "C" {
  static real_T rtGetNaN(void);
  static real32_T rtGetNaNF(void);
}                                      // extern "C"
  extern "C"
{
  extern real_T rtInf;
  extern real_T rtMinusInf;
  extern real_T rtNaN;
  extern real32_T rtInfF;
  extern real32_T rtMinusInfF;
  extern real32_T rtNaNF;
  static void rt_InitInfAndNaN(size_t realSize);
  static boolean_T rtIsInf(real_T value);
  static boolean_T rtIsInfF(real32_T value);
  static boolean_T rtIsNaN(real_T value);
  static boolean_T rtIsNaNF(real32_T value);
  typedef struct {
    struct {
      uint32_T wordH;
      uint32_T wordL;
    } words;
  } BigEndianIEEEDouble;

  typedef struct {
    struct {
      uint32_T wordL;
      uint32_T wordH;
    } words;
  } LittleEndianIEEEDouble;

  typedef struct {
    union {
      real32_T wordLreal;
      uint32_T wordLuint;
    } wordL;
  } IEEESingle;
}                                      // extern "C"

extern "C" {
  static real_T rtGetInf(void);
  static real32_T rtGetInfF(void);
  static real_T rtGetMinusInf(void);
  static real32_T rtGetMinusInfF(void);
}                                      // extern "C"
  // Class declaration for model wrist_decoupler
  class wrist_decouplerModelClass
{
  // public data and function members
 public:
  // Block signals and states (default storage) for system '<Root>'
  typedef struct {
    quaternion T;                      // '<S2>/motors2quat'
    real_T singularity_reg;            // '<S2>/motors2quat'
  } DW;

  // Constant parameters (default storage)
  typedef struct {
    // Pooled Parameter (Expression: Q)
    //  Referenced by:
    //    '<S1>/ypr2motors'
    //    '<S2>/motors2quat'

    real_T pooled4[9];
  } ConstP;

  // External inputs (root inport signals with default storage)
  typedef struct {
    real_T theta_meas[3];              // '<Root>/theta_meas'
    real_T ypr[3];                     // '<Root>/ypr_star'
  } ExtU;

  // External outputs (root outports fed by signals with default storage)
  typedef struct {
    real_T theta_star[3];              // '<Root>/theta_star'
    real_T out_of_range;               // '<Root>/out_of_range'
    real_T ypr_meas[3];                // '<Root>/ypr_meas'
    real_T singularity;                // '<Root>/singularity'
    real_T cycles;                     // '<Root>/cycles'
  } ExtY;

  // Real-time Model Data Structure
  struct RT_MODEL {
    const char_T * volatile errorStatus;
  };

  // External inputs
  ExtU rtU;

  // External outputs
  ExtY rtY;

  // model initialize function
  void initialize();

  // model step function
  void step();

  // Constructor
  wrist_decouplerModelClass();

  // Destructor
  ~wrist_decouplerModelClass();

  // Real-Time Model get method
  wrist_decouplerModelClass::RT_MODEL * getRTM();

  // private data and function members
 private:
  // Block signals and states
  DW rtDW;

  // Real-Time Model
  RT_MODEL rtM;

  // private member function(s) for subsystem '<Root>'
  real_T maximum(const real_T x[3]);
  real_T minimum(const real_T x[3]);
  void sind(real_T x[3]);
  real_T det(const real_T x[9]);
  void mldivide(const real_T A[9], const real_T B_0[3], real_T Y[3]);
}

;

// Constant parameters (default storage)
extern const wrist_decouplerModelClass::ConstP rtConstP;

//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Note that this particular code originates from a subsystem build,
//  and has its own system numbers different from the parent model.
//  Refer to the system hierarchy for this subsystem below, and use the
//  MATLAB hilite_system command to trace the generated code back
//  to the parent model.  For example,
//
//  hilite_system('ok_wrist_simple_motors_3/Position_Controller/wrist_decoupler')    - opens subsystem ok_wrist_simple_motors_3/Position_Controller/wrist_decoupler
//  hilite_system('ok_wrist_simple_motors_3/Position_Controller/wrist_decoupler/Kp') - opens and selects block Kp
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'ok_wrist_simple_motors_3/Position_Controller'
//  '<S1>'   : 'ok_wrist_simple_motors_3/Position_Controller/wrist_decoupler'
//  '<S2>'   : 'ok_wrist_simple_motors_3/Position_Controller/wrist_decoupler/motors2ypr'
//  '<S3>'   : 'ok_wrist_simple_motors_3/Position_Controller/wrist_decoupler/ypr2motors'
//  '<S4>'   : 'ok_wrist_simple_motors_3/Position_Controller/wrist_decoupler/motors2ypr/Quaternions to Rotation Angles1'
//  '<S5>'   : 'ok_wrist_simple_motors_3/Position_Controller/wrist_decoupler/motors2ypr/motors2quat'
//  '<S6>'   : 'ok_wrist_simple_motors_3/Position_Controller/wrist_decoupler/motors2ypr/Quaternions to Rotation Angles1/Angle Calculation'
//  '<S7>'   : 'ok_wrist_simple_motors_3/Position_Controller/wrist_decoupler/motors2ypr/Quaternions to Rotation Angles1/Quaternion Normalize'
//  '<S8>'   : 'ok_wrist_simple_motors_3/Position_Controller/wrist_decoupler/motors2ypr/Quaternions to Rotation Angles1/Angle Calculation/Protect asincos input'
//  '<S9>'   : 'ok_wrist_simple_motors_3/Position_Controller/wrist_decoupler/motors2ypr/Quaternions to Rotation Angles1/Angle Calculation/Protect asincos input/If Action Subsystem'
//  '<S10>'  : 'ok_wrist_simple_motors_3/Position_Controller/wrist_decoupler/motors2ypr/Quaternions to Rotation Angles1/Angle Calculation/Protect asincos input/If Action Subsystem1'
//  '<S11>'  : 'ok_wrist_simple_motors_3/Position_Controller/wrist_decoupler/motors2ypr/Quaternions to Rotation Angles1/Angle Calculation/Protect asincos input/If Action Subsystem2'
//  '<S12>'  : 'ok_wrist_simple_motors_3/Position_Controller/wrist_decoupler/motors2ypr/Quaternions to Rotation Angles1/Quaternion Normalize/Quaternion Modulus'
//  '<S13>'  : 'ok_wrist_simple_motors_3/Position_Controller/wrist_decoupler/motors2ypr/Quaternions to Rotation Angles1/Quaternion Normalize/Quaternion Modulus/Quaternion Norm'

#endif                                 // RTW_HEADER_wrist_decoupler_h_

//
// File trailer for generated code.
//
// [EOF]
//
