//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: control_foc.cpp
//
// Code generated for Simulink model 'control_foc'.
//
// Model version                  : 2.85
// Simulink Coder version         : 9.6 (R2021b) 14-May-2021
// C/C++ source code generated on : Fri Jan 14 20:51:10 2022
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "control_foc.h"
#include "control_foc_private.h"

namespace amc_bldc_codegen
{
  // System initialize for referenced model: 'control_foc'
  void control_foc::init(void)
  {
    // SystemInitialize for Atomic SubSystem: '<Root>/FOC inner loop'
    FOCInnerLoop_Init();

    // End of SystemInitialize for SubSystem: '<Root>/FOC inner loop'
  }

  // Output and update for referenced model: 'control_foc'
  void control_foc::step(const SensorsData *rtu_Sensors, const FOCSlowInputs
    *rtu_FOCSlowInputs, ControlOutputs *rty_FOCOutputs)
  {
    // Outputs for Atomic SubSystem: '<Root>/FOC inner loop'
    FOCInnerLoop(&rtu_FOCSlowInputs->flags,
                 &rtu_FOCSlowInputs->configurationparameters, rtu_Sensors,
                 &rtu_FOCSlowInputs->estimateddata, &rtu_FOCSlowInputs->targets,
                 &rtu_FOCSlowInputs->controlouteroutputs, rty_FOCOutputs);

    // End of Outputs for SubSystem: '<Root>/FOC inner loop'
  }

  // Model initialize function
  void control_foc::initialize()
  {
    // Registration code

    // initialize non-finites
    rt_InitInfAndNaN(sizeof(real_T));
    control_foc_PrevZCX.FilterDifferentiatorTF_Reset_ZC = POS_ZCSIG;
    control_foc_PrevZCX.FilterDifferentiatorTF_Reset__m = POS_ZCSIG;
  }

  // Constructor
  control_foc::control_foc() :
    control_foc_DW(),
    control_foc_PrevZCX(),
    control_foc_M()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  control_foc::~control_foc()
  {
    // Currently there is no destructor body generated.
  }

  // Real-Time Model get method
  amc_bldc_codegen::control_foc::RT_MODEL_control_foc_T * control_foc::getRTM()
  {
    return (&control_foc_M);
  }

  // member function to setup error status pointer
  void control_foc::setErrorStatusPointer(const char_T **rt_errorStatus)
  {
    rtmSetErrorStatusPointer((&control_foc_M), rt_errorStatus);
  }
}

//
// File trailer for generated code.
//
// [EOF]
//
