//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: TorqueModel.cpp
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
#include "TorqueModel.h"
#include "rtwtypes.h"
#include <cmath>
#include "TorqueModel_capi.h"

// Output and update for atomic system: '<Root>/TorqueModel'
void TorqueModel::TorqueModel_d(real32_T rtu_Torsion, real32_T *rty_Torque,
  DW_TorqueModel *localDW)
{
  real32_T Backlash;
  real32_T rtb_Abs1;

  // Backlash: '<S1>/Backlash'
  if (rtu_Torsion < localDW->PrevY - 5.99211262E-6F) {
    // Backlash: '<S1>/Backlash'
    Backlash = rtu_Torsion + 5.99211262E-6F;
  } else if (rtu_Torsion <= localDW->PrevY + 5.99211262E-6F) {
    // Backlash: '<S1>/Backlash'
    Backlash = localDW->PrevY;
  } else {
    // Backlash: '<S1>/Backlash'
    Backlash = rtu_Torsion - 5.99211262E-6F;
  }

  // End of Backlash: '<S1>/Backlash'

  // Abs: '<S1>/Abs1'
  rtb_Abs1 = std::abs(Backlash);

  // Switch: '<S1>/Switch' incorporates:
  //   Bias: '<S1>/Add Constant'
  //   Bias: '<S1>/Add Constant1'
  //   Bias: '<S1>/Add Constant2'
  //   Gain: '<S1>/Gain1'
  //   Switch: '<S1>/Switch1'

  if (rtb_Abs1 - 0.00115750008F > 0.0F) {
    *rty_Torque = (rtb_Abs1 - 0.00115750008F) * 29000.0F + 25.0F;
  } else if (rtb_Abs1 - 0.0004375F > 0.0F) {
    // Switch: '<S1>/Switch1' incorporates:
    //   Bias: '<S1>/Add Constant2'
    //   Bias: '<S1>/Add Constant3'
    //   Gain: '<S1>/Gain2'

    *rty_Torque = (rtb_Abs1 - 0.0004375F) * 25000.0F + 7.0F;
  } else {
    // Switch: '<S1>/Switch1' incorporates:
    //   Gain: '<S1>/Gain3'

    *rty_Torque = 16000.0F * rtb_Abs1;
  }

  // End of Switch: '<S1>/Switch'

  // Signum: '<S1>/Sign'
  if (Backlash < 0.0F) {
    rtb_Abs1 = -1.0F;
  } else {
    rtb_Abs1 = (Backlash > 0.0F);
  }

  // Product: '<S1>/Product' incorporates:
  //   Signum: '<S1>/Sign'

  *rty_Torque *= rtb_Abs1;

  // Update for Backlash: '<S1>/Backlash'
  localDW->PrevY = Backlash;
}

// Model step function
void TorqueModel::step()
{
  // Outputs for Atomic SubSystem: '<Root>/TorqueModel'

  // Inport: '<Root>/Torsion' incorporates:
  //   Outport: '<Root>/Torque'

  TorqueModel_d(rtU.Torsion, &rtY.Torque, &rtDW.TorqueModel_dk);

  // End of Outputs for SubSystem: '<Root>/TorqueModel'
}

// Model initialize function
void TorqueModel::initialize()
{
  // Registration code

  // Initialize DataMapInfo substructure containing ModelMap for C API
  TorqueModel_InitializeDataMapInfo((&rtM));
}

// Constructor
TorqueModel::TorqueModel() :
  rtU(),
  rtY(),
  rtDW(),
  rtM()
{
  // Currently there is no constructor body generated.
}

// Destructor
TorqueModel::~TorqueModel()
{
  // Currently there is no destructor body generated.
}

// Real-Time Model get method
TorqueModel::RT_MODEL * TorqueModel::getRTM()
{
  return (&rtM);
}

//
// File trailer for generated code.
//
// [EOF]
//
