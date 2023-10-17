//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: wrist_decoupler.h
//
// Code generated for Simulink model 'wrist_decoupler'.
//
// Model version                  : 7.13
// Simulink Coder version         : 9.9 (R2023a) 19-Nov-2022
// C/C++ source code generated on : Tue Jul 25 11:18:40 2023
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
#include "rtwtypes.h"
#include <stddef.h>

// Macros for accessing real-time model data structure
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

// Custom Type definition for MATLAB Function: '<S1>/motors2ypr'
#ifndef struct_quaternion
#define struct_quaternion

struct quaternion
{
  real_T a;
  real_T b;
  real_T c;
  real_T d;
};

#endif                                 // struct_quaternion

extern "C"
{
  static real_T rtGetNaN(void);
  static real32_T rtGetNaNF(void);
}                                      // extern "C"

#define NOT_USING_NONFINITE_LITERALS   1

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
  struct BigEndianIEEEDouble {
    struct {
      uint32_T wordH;
      uint32_T wordL;
    } words;
  };

  struct LittleEndianIEEEDouble {
    struct {
      uint32_T wordL;
      uint32_T wordH;
    } words;
  };

  struct IEEESingle {
    union {
      real32_T wordLreal;
      uint32_T wordLuint;
    } wordL;
  };
}                                      // extern "C"

extern "C"
{
  static real_T rtGetInf(void);
  static real32_T rtGetInfF(void);
  static real_T rtGetMinusInf(void);
  static real32_T rtGetMinusInfF(void);
}                                      // extern "C"

// Class declaration for model wrist_decoupler
class wrist_decoupler
{
  // public data and function members
 public:
  // Block signals and states (default storage) for system '<Root>'
  struct DW {
    quaternion T;                      // '<S1>/motors2ypr'
    real_T Pz[3];                      // '<S1>/ypr2motors'
    real_T PQ[3];                      // '<S1>/ypr2motors'
    real_T Q[9];                       // '<S1>/ypr2motors'
    real_T last_theta_ok[3];           // '<S1>/ypr2motors'
    real_T Q_p[9];                     // '<S1>/motors2ypr'
    real_T PQ_f[3];                    // '<S1>/motors2ypr'
    real_T Pz_b[3];                    // '<S1>/motors2ypr'
    real_T singularity_reg;            // '<S1>/motors2ypr'
    boolean_T Pz_not_empty;            // '<S1>/ypr2motors'
    boolean_T singularity_reg_not_empty;// '<S1>/motors2ypr'
  };

  // External inputs (root inport signals with default storage)
  struct ExtU {
    real_T theta_diff[3];              // '<Root>/theta_diff'
    real_T theta_off[3];               // '<Root>/theta_off'
    real_T plat_off[3];                // '<Root>/plat_off'
    real_T arm_bend;                   // '<Root>/arm_bend'
    real_T ypr_star[3];                // '<Root>/ypr_star'
    boolean_T RL;                      // '<Root>/RL'
  };

  // External outputs (root outports fed by signals with default storage)
  struct ExtY {
    real_T theta_star[3];              // '<Root>/theta_star'
    real_T out_of_range;               // '<Root>/out_of_range'
    real_T ypr_meas[3];                // '<Root>/ypr_meas'
    real_T singularity;                // '<Root>/singularity'
    real_T cycles;                     // '<Root>/cycles'
  };

  // Real-time Model Data Structure
  struct RT_MODEL {
    const char_T * volatile errorStatus;
  };

  // Real-Time Model get method
  wrist_decoupler::RT_MODEL * getRTM();

  // External inputs
  ExtU rtU;

  // External outputs
  ExtY rtY;

  // model initialize function
  void initialize();

  // model step function
  void step();

  // Constructor
  wrist_decoupler();

  // Destructor
  ~wrist_decoupler();

  // private data and function members
 private:
  // Block states
  DW rtDW;

  // private member function(s) for subsystem '<Root>'
  void cosd(real_T x[3]);
  void sind(real_T x[3]);
  real_T maximum(const real_T x[3]);
  real_T minimum(const real_T x[3]);
  real_T det(const real_T x[9]);

  // Real-Time Model
  RT_MODEL rtM;
};

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
//  hilite_system('spherical_wrist/Position_Controller/wrist_decoupler')    - opens subsystem spherical_wrist/Position_Controller/wrist_decoupler
//  hilite_system('spherical_wrist/Position_Controller/wrist_decoupler/Kp') - opens and selects block Kp
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'spherical_wrist/Position_Controller'
//  '<S1>'   : 'spherical_wrist/Position_Controller/wrist_decoupler'
//  '<S2>'   : 'spherical_wrist/Position_Controller/wrist_decoupler/Quaternions to Rotation Angles'
//  '<S3>'   : 'spherical_wrist/Position_Controller/wrist_decoupler/Radians to Degrees'
//  '<S4>'   : 'spherical_wrist/Position_Controller/wrist_decoupler/motors2ypr'
//  '<S5>'   : 'spherical_wrist/Position_Controller/wrist_decoupler/ypr2motors'
//  '<S6>'   : 'spherical_wrist/Position_Controller/wrist_decoupler/Quaternions to Rotation Angles/Angle Calculation'
//  '<S7>'   : 'spherical_wrist/Position_Controller/wrist_decoupler/Quaternions to Rotation Angles/Quaternion Normalize'
//  '<S8>'   : 'spherical_wrist/Position_Controller/wrist_decoupler/Quaternions to Rotation Angles/Angle Calculation/Protect asincos input'
//  '<S9>'   : 'spherical_wrist/Position_Controller/wrist_decoupler/Quaternions to Rotation Angles/Angle Calculation/Protect asincos input/If Action Subsystem'
//  '<S10>'  : 'spherical_wrist/Position_Controller/wrist_decoupler/Quaternions to Rotation Angles/Angle Calculation/Protect asincos input/If Action Subsystem1'
//  '<S11>'  : 'spherical_wrist/Position_Controller/wrist_decoupler/Quaternions to Rotation Angles/Angle Calculation/Protect asincos input/If Action Subsystem2'
//  '<S12>'  : 'spherical_wrist/Position_Controller/wrist_decoupler/Quaternions to Rotation Angles/Quaternion Normalize/Quaternion Modulus'
//  '<S13>'  : 'spherical_wrist/Position_Controller/wrist_decoupler/Quaternions to Rotation Angles/Quaternion Normalize/Quaternion Modulus/Quaternion Norm'

#endif                                 // RTW_HEADER_wrist_decoupler_h_

//
// File trailer for generated code.
//
// [EOF]
//
