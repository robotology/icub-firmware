//   
//  Encoder related rouines:
//  
#ifndef __ENCODER_H
#define __ENCODER_H

#include "qep.h"
#include "as5045.h"
#include "UserTypes.h"
#include "UserParms.h"
#include "pi.h"
#include "Velocity.h"

// TOTAL delay for AE (SPI + internal DSP)
// TODO: attenzione sono dei fp, probabilmente troncati dal compilatore
#define AE_DELAY 3.3
// TODO: trim me
#define QE_DELAY 0.0
// Set proper encoder delay
#ifdef ENCODER_QE
// Actual encoder delay for quadrature encoder
#define ENCODER_DELAY QE_DELAY
#else
// Actual encoder delay for spi encoder 
#define ENCODER_DELAY AE_DELAY
#endif

extern void EncoderInit();
extern void EncoderPosition();
// trigger the postion data from spi/quep encoder
extern void EncoderTriggerSample();
extern SFRAC16 EncoderSpeed();
extern void EncoderRotorAlign();
extern int EncoderVelocity();
extern void AlignRotor(void);
extern void EncoderWaitUpdate();

extern SFRAC16 TorqueTogglingReference;
extern SFRAC16 OmegaTogglingReference;
extern SFRAC16 Encoder_SyncPulsePosition;
extern SFRAC16 Encoder_RotorStartingPosition;
extern SFRAC16 UnalignedMecchanicalAngle;
extern SFRAC16 AlignedMecchanicalAngle;
extern tCtrlReferences CtrlReferences;
extern tSysStatus SysStatus; 
extern tPIParm PIParmD;
extern tPIParm PIParmQ;
extern volatile int Turns;
extern volatile tVelocityParm VelocityParm;
extern volatile long Previous_position;
extern volatile long Previous_position_for_velocity_calc;
extern volatile short IraqVd;
extern volatile short IraqVq; 
extern unsigned int EncoderDegradation;

#endif
