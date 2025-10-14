//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// File: AMC_BLDC.h
//
// Code generated for Simulink model 'AMC_BLDC'.
//
// Model version                  : 11.0
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Tue Oct 14 09:18:16 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef AMC_BLDC_h_
#define AMC_BLDC_h_
#include "rtwtypes.h"
#include "AMC_BLDC_types.h"
#include "process_sensors.h"
#include "control_foc.h"
#include "can_decoder.h"
#include "motion_controller_single.h"
#include "SupervisorFSM_TX.h"
#include "can_encoder.h"
#include "zero_crossing_types.h"

// Includes for objects with custom storage classes
#include "rtw_defines.h"

// Block signals (default storage)
struct B_AMC_BLDC_T {
  FOCSlowInputs TmpRTBAtFOCInport2;
  FOCSlowInputs MotionControllerSingle_o4;// '<Root>/Motion Controller Single'
  ReceivedEvents CAN_Decoder[4];       // '<S4>/CAN_Decoder'
  BUS_MESSAGES_TX MessagesTx;          // '<Root>/SupervisorFSM_TX'
  SensorsData SensorsDataCalibration;  // '<Root>/Motion Controller Single'
  BUS_STATUS_TX SupervisorFSM_TX_o2;   // '<Root>/SupervisorFSM_TX'
};

// Block states (default storage) for system '<Root>'
struct DW_AMC_BLDC_T {
  FOCSlowInputs TmpRTBAtFOCInport2_Buf[3];// synthesized block
  ActuatorConfiguration TmpRTBAtProcessSensorsInport1_Buf[3];// synthesized block 
  SensorsData TmpRTBAtFOCInport4_Buf[3];// synthesized block
  SensorsData TmpRTBAtSupervisorFSM_TXInport1_Buf0;// synthesized block
  SensorsData TmpRTBAtSupervisorFSM_TXInport1_Buf1;// synthesized block
  SensorsData TmpRTBAtSupervisorFSM_TXInport1_Buf2;// synthesized block
  SensorsData TmpRTBAtMotionControllerSingleInport1_Buf0;// synthesized block
  SensorsData TmpRTBAtMotionControllerSingleInport1_Buf1;// synthesized block
  SensorsData TmpRTBAtMotionControllerSingleInport1_Buf2;// synthesized block
  FOCOutputs RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0_Bu;// synthesized block 
  FOCOutputs RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0__h;// synthesized block 
  FOCOutputs RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0__a;// synthesized block 
  FOCOutputs TmpRTBAtMotionControllerSingleInport4_Buf0;// synthesized block
  FOCOutputs TmpRTBAtMotionControllerSingleInport4_Buf1;// synthesized block
  FOCOutputs TmpRTBAtMotionControllerSingleInport4_Buf2;// synthesized block
  Flags TmpRTBAtProcessSensorsInport2_Buf[3];// synthesized block
  Flags TmpRTBAtFOCInport3_Buf[3];     // synthesized block
  void* TmpRTBAtProcessSensorsInport1_SEMAPHORE;// synthesized block
  void* TmpRTBAtProcessSensorsInport2_SEMAPHORE;// synthesized block
  void* TmpRTBAtFOCInport2_SEMAPHORE;  // synthesized block
  void* TmpRTBAtFOCInport3_SEMAPHORE;  // synthesized block
  void* TmpRTBAtFOCInport4_SEMAPHORE;  // synthesized block
  void* RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0_SE;// synthesized block 
  void* TmpRTBAtMotionControllerSingleInport4_SEMAPHORE;// synthesized block
  void* TmpRTBAtSupervisorFSM_TXInport1_SEMAPHORE;// synthesized block
  void* TmpRTBAtMotionControllerSingleInport1_SEMAPHORE;// synthesized block
  int8_T TmpRTBAtProcessSensorsInport1_LstBufWR;// synthesized block
  int8_T TmpRTBAtProcessSensorsInport1_RDBuf;// synthesized block
  int8_T TmpRTBAtProcessSensorsInport2_LstBufWR;// synthesized block
  int8_T TmpRTBAtProcessSensorsInport2_RDBuf;// synthesized block
  int8_T TmpRTBAtFOCInport2_LstBufWR;  // synthesized block
  int8_T TmpRTBAtFOCInport2_RDBuf;     // synthesized block
  int8_T TmpRTBAtFOCInport3_LstBufWR;  // synthesized block
  int8_T TmpRTBAtFOCInport3_RDBuf;     // synthesized block
  int8_T TmpRTBAtFOCInport4_LstBufWR;  // synthesized block
  int8_T TmpRTBAtFOCInport4_RDBuf;     // synthesized block
  int8_T RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0_Ls;// synthesized block 
  int8_T RTBInsertedForAdapter_InsertedFor_Component2_at_outport_0_RD;// synthesized block 
  int8_T TmpRTBAtMotionControllerSingleInport4_LstBufWR;// synthesized block
  int8_T TmpRTBAtMotionControllerSingleInport4_RDBuf;// synthesized block
  int8_T TmpRTBAtSupervisorFSM_TXInport1_LstBufWR;// synthesized block
  int8_T TmpRTBAtSupervisorFSM_TXInport1_RDBuf;// synthesized block
  int8_T TmpRTBAtMotionControllerSingleInport1_LstBufWR;// synthesized block
  int8_T TmpRTBAtMotionControllerSingleInport1_RDBuf;// synthesized block
  MdlrefDW_process_sensors_T ProcessSensors_InstanceData;// '<Root>/Process Sensors' 
  MdlrefDW_control_foc_T FOC_InstanceData;// '<Root>/FOC'
  MdlrefDW_can_decoder_T CAN_Decoder_InstanceData;// '<S4>/CAN_Decoder'
  MdlrefDW_motion_controller_single_T MotionControllerSingle_InstanceData;// '<Root>/Motion Controller Single' 
  MdlrefDW_SupervisorFSM_TX_T SupervisorFSM_TX_InstanceData;// '<Root>/SupervisorFSM_TX' 
  MdlrefDW_can_encoder_T CAN_Encoder_InstanceData;// '<S4>/CAN_Encoder'
};

// External inputs (root inport signals with default storage)
struct ExtU_AMC_BLDC_T {
  SensorsData SensorsData_p;           // '<Root>/SensorsData'
  ExternalFlags ExternalFlags_p;       // '<Root>/ExternalFlags'
  BUS_CAN_MULTIPLE PacketsRx;          // '<Root>/PacketsRx'
};

// External outputs (root outports fed by signals with default storage)
struct ExtY_AMC_BLDC_T {
  FOCOutputs ControlOutputs;           // '<Root>/ControlOutputs'
  ActuatorConfiguration ConfigurationParameters;// '<Root>/ConfigurationParameters' 
  Flags Flags_p;                       // '<Root>/Flags'
  EstimatedData EstimatedData_p;       // '<Root>/EstimatedData'
  BUS_CAN_MULTIPLE PacketsTx;          // '<Root>/PacketsTx'
};

// Real-time Model Data Structure
struct tag_RTM_AMC_BLDC_T {
  const char_T *errorStatus;

  //
  //  Timing:
  //  The following substructure contains information regarding
  //  the timing information for the model.

  struct {
    struct {
      uint32_T TID[3];
      uint32_T cLimit[3];
    } TaskCounters;
  } Timing;

  boolean_T StepTask(int32_T idx) const;
  uint32_T &CounterLimit(int32_T idx);
  const char_T* getErrorStatus() const;
  void setErrorStatus(const char_T* const aErrorStatus);
  uint32_T &TaskCounter(int32_T idx);
  const char_T** getErrorStatusPointer();
};

// Block signals (default storage)
#ifdef __cplusplus

extern "C"
{

#endif

  extern struct B_AMC_BLDC_T AMC_BLDC_B;

#ifdef __cplusplus

}

#endif

// Block states (default storage)
extern struct DW_AMC_BLDC_T AMC_BLDC_DW;

#ifdef __cplusplus

extern "C"
{

#endif

  // External inputs (root inport signals with default storage)
  extern struct ExtU_AMC_BLDC_T AMC_BLDC_U;

  // External outputs (root outports fed by signals with default storage)
  extern struct ExtY_AMC_BLDC_T AMC_BLDC_Y;

#ifdef __cplusplus

}

#endif

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern ActuatorConfiguration AmcbldcInitConf;// Variable: AmcbldcInitConf
                                                //  Referenced by: '<Root>/Motion Controller Single'

extern real32_T CAN_ANGLE_DEG2ICUB;    // Variable: CAN_ANGLE_DEG2ICUB
                                          //  Referenced by: '<S4>/CAN_Encoder'
                                          //  2^16/360

extern real32_T CAN_ANGLE_ICUB2DEG;    // Variable: CAN_ANGLE_ICUB2DEG
                                          //  Referenced by: '<S4>/CAN_Decoder'
                                          //  360/2^16

extern uint8_T CAN_ID_AMC;             // Variable: CAN_ID_AMC
                                          //  Referenced by:
                                          //    '<S4>/CAN_Decoder'
                                          //    '<S4>/CAN_Encoder'
                                          //  4 bits defining the ID of the AMC_BLDC board.


#ifdef __cplusplus

extern "C"
{

#endif

  // Model entry point functions
  extern void AMC_BLDC_initialize(void);
  extern void AMC_BLDC_step0(void);    // Sample time: [5e-06s, 0.0s]
  extern void AMC_BLDC_step_FOC(void); // Sample time: [4.5e-05s, 0.0s]
  extern void AMC_BLDC_step_1ms(void); // Sample time: [0.001s, 0.0s]
  extern void AMC_BLDC_terminate(void);

#ifdef __cplusplus

}

#endif

// Real-time Model object
#ifdef __cplusplus

extern "C"
{

#endif

  extern RT_MODEL_AMC_BLDC_T *const AMC_BLDC_M;

#ifdef __cplusplus

}

#endif

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
//  '<Root>' : 'AMC_BLDC'
//  '<S1>'   : 'AMC_BLDC/Component'
//  '<S2>'   : 'AMC_BLDC/Component2'
//  '<S3>'   : 'AMC_BLDC/Component7'
//  '<S4>'   : 'AMC_BLDC/Messaging'

#endif                                 // AMC_BLDC_h_

//
// File trailer for generated code.
//
// [EOF]
//
