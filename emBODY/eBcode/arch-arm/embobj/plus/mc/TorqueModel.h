//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: TorqueModel.h
//
// Code generated for Simulink model 'TorqueModel'.
//
// Model version                  : 3.9
// Simulink Coder version         : 9.8 (R2022b) 13-May-2022
// C/C++ source code generated on : Thu Oct 27 10:27:11 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#ifndef RTW_HEADER_TorqueModel_h_
#define RTW_HEADER_TorqueModel_h_
#include "rtwtypes.h"
#include "rtw_modelmap.h"

// Macros for accessing real-time model data structure
#ifndef rtmGetDataMapInfo
#define rtmGetDataMapInfo(rtm)         ((rtm)->DataMapInfo)
#endif

#ifndef rtmSetDataMapInfo
#define rtmSetDataMapInfo(rtm, val)    ((rtm)->DataMapInfo = (val))
#endif

#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

#ifndef SS_UINT64
#define SS_UINT64                      17
#endif

#ifndef SS_INT64
#define SS_INT64                       18
#endif

// Function to get C API Model Mapping Static Info
extern const rtwCAPI_ModelMappingStaticInfo*
  TorqueModel_GetCAPIStaticMap(void);

// Class declaration for model TorqueModel
class TorqueModel final
{
  // public data and function members
 public:
  // Block signals and states (default storage) for system '<Root>/TorqueModel'
  struct DW_TorqueModel {
    real32_T PrevY;                    // '<S1>/Backlash'
  };

  // Block signals and states (default storage) for system '<Root>'
  struct DW {
    DW_TorqueModel TorqueModel_dk;     // '<Root>/TorqueModel'
  };

  // External inputs (root inport signals with default storage)
  struct ExtU {
    real32_T Torsion;                  // '<Root>/Torsion'
  };

  // External outputs (root outports fed by signals with default storage)
  struct ExtY {
    real32_T Torque;                   // '<Root>/Torque'
  };

  // Real-time Model Data Structure
  struct RT_MODEL {
    const char_T * volatile errorStatus;

    //
    //  DataMapInfo:
    //  The following substructure contains information regarding
    //  structures generated in the model's C API.

    struct {
      rtwCAPI_ModelMappingInfo mmi;
    } DataMapInfo;
  };

  // Copy Constructor
  TorqueModel(TorqueModel const&) = delete;

  // Assignment Operator
  TorqueModel& operator= (TorqueModel const&) & = delete;

  // Move Constructor
  TorqueModel(TorqueModel &&) = delete;

  // Move Assignment Operator
  TorqueModel& operator= (TorqueModel &&) = delete;

  // Real-Time Model get method
  TorqueModel::RT_MODEL * getRTM();

  // External inputs
  ExtU rtU;

  // External outputs
  ExtY rtY;

  // model initialize function
  void initialize();

  // model step function
  void step();

  // Constructor
  TorqueModel();

  // Destructor
  ~TorqueModel();

  // private data and function members
 private:
  // Block states
  DW rtDW;

  // private member function(s) for subsystem '<Root>/TorqueModel'
  static void TorqueModel_d(real32_T rtu_Torsion, real32_T *rty_Torque,
    DW_TorqueModel *localDW);

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
//  hilite_system('ErgoJointTorqueControl2/TorqueModel')    - opens subsystem ErgoJointTorqueControl2/TorqueModel
//  hilite_system('ErgoJointTorqueControl2/TorqueModel/Kp') - opens and selects block Kp
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'ErgoJointTorqueControl2'
//  '<S1>'   : 'ErgoJointTorqueControl2/TorqueModel'

#endif                                 // RTW_HEADER_TorqueModel_h_

//
// File trailer for generated code.
//
// [EOF]
//
