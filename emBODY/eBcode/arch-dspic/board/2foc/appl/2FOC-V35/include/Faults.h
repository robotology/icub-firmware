//
// fault management 
//

#ifndef __FAULTS_H__
#define __FAULTS_H__

#include "System.h"
#include "Status.h"

//
// Flags for various Errors of the System
//
typedef union uSysError
{
  struct 
  {
/********************************************************************************************************/
/*    first byte  ==> data[0] in ICUBPROTO_PERIODICCMD_STATUS                                            */

/*b0*/  unsigned ExternalFaultAsserted:1;   /* External Fault */
/*b1*/  unsigned UnderVoltageFailure:1;     /* Undervoltage */
/*b2*/  unsigned OverVoltageFailure:1;      /* Overvoltage */
/*b3*/  unsigned OverCurrentFailure:1;      /* OverCurrent */ 
/*b4*/  unsigned DHESInvalidValue:1;        /* an invalid value of HES has been detected */ 
/*b5*/  unsigned AS5045CSumError:1;         
/*b6*/  unsigned DHESInvalidSequence:1;     /* an invalid sequence of HES activation has been detected */
/*b7*/  unsigned CANInvalidProtocol:1;      /* can protocol incompatible with EMS */


/********************************************************************************************************/
/*    second byte  ==> data[4] in ICUBPROTO_PERIODICCMD_STATUS                                          */

/*b0*/  unsigned CAN_BufferOverRun:1;        /* A CAN fifo full event has happened. 
                                               This should never happen because this FW on the 2FOC should 
                                               be able to handle CAN with full load. This might happen in 
                                               certain blocking operation are requested like save to eeprom 
                                               parameters. TODO: verify it. */
/*b1*/  unsigned SetpointExpired:1;           /* UNUSED: put here for mad in msg */
/*b2*/  unsigned CAN_TXIsPasv:1;              /* can IS is TX passive mode */
/*b3*/  unsigned CAN_RXIsPasv:1;              /* can IS in RX passive mode */
/*b4*/  unsigned CAN_IsWarnTX:1;              /* can IS in bus warn in tx (exist only one error for rx and tx, so the bits are used together)*/
/*b5*/  unsigned CAN_IsWarnRX:1;              /* can IS in bus warn in rx*/
/*b6*/  unsigned unused3:1;                   /* UNUSED: put here for mad in msg */
/*b7*/  unsigned unused4:1;                   /* UNUSED: put here for mad in msg */


/********************************************************************************************************/
/*    third byte  (not used in ICUBPROTO_PERIODICCMD_STATUS)                                            */

/*b0*/  unsigned ADCCalFailure:1;           /* ADC calibration failure */ 
/*b1*/  unsigned I2TFailure:1;              /* I2T protection */                     
/*b2*/  unsigned EMUROMFault:1;             /* EMUROM Fault */
/*b3*/  unsigned EMUROMCRCFault:1;          /* EMUROM CRC Fault */
/*b4*/  unsigned EncoderFault:1;            /*  Encoder Fault. 1 when homing with zero signal fail */ 
/*b5*/  unsigned FirmwareSPITimingError:1;  /* SPI reading has been interrupted before finishing by foc loop */		
/*b6*/  unsigned AS5045CalcError:1;
    
/*b7*/  unsigned FirmwarePWMFatalError:1;   /* This is true when the FOC loop tried to delay the PWM update (see below)
                                               but the wait for the PWM counter timed out (PWM counter freezed?)
                                               This should never happen, and that may indicate
                                               a firmware BUG or abnormal firmware behaviour due to
                                               unpredictable and exotic reasons (malfunctions, voodoo
                                               magic, hardware bugs, gravity inversions, alien invasions, 
                                               end of the world).
                                               In any case please consider that this is certainly NOT due
                                               to the firmware developer, but more likely it's electronic 
                                               eng. full responsibility :-) */


/********************************************************************************************************/
/*    fourth byte  (not used in ICUBPROTO_PERIODICCMD_STATUS)                                           */
/*b0*/  unsigned CAN_TXWasPasv:1;           /* can has been in TX passive mode at least one time. */
/*b1*/  unsigned CAN_RXWasPasv:1;           /* can has been in RX passive */
/*b2*/  unsigned CAN_RTRFlagActive:1;       /* an RTR flag has been seen over the wire. This is not OK for LorCan */
/*b3*/  unsigned CAN_WasWarn:1;             /* can has been in bus warn at least one time */
/*b4*/  unsigned CAN_DLCError:1;            /* at least one DLC error has been seen */
/*b5*/  unsigned SiliconRevisionFault:1;    /* see comments below */
/*b6*/  unsigned PositionLimitUpper:1; 
/*b7*/  unsigned PositionLimitLower:1; 

 } ;

  // Permits to access the whole structure data in byte/word/array fashon 
  unsigned int  W[2];
  unsigned char b[4];
  unsigned long L;
}  __attribute__((__packed__)) tSysError;

/********************************************************************************/

extern void FaultConditionsHandler(void);
extern void FaultReset(void);
extern int Fault();
extern void FaultRecheck();

volatile extern tSysError SysError;

#endif
