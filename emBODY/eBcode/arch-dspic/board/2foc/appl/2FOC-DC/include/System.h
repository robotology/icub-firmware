//
// system related functions
// system clock, IO ports, system LEDs 
//
#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "UserParms.h"
#include "MeasCurr.h"
#include "Status.h"
#include <string.h>

typedef struct
{
    int Red;
    int Green;

    unsigned int RedBlinkRate;
    unsigned int GreenBlinkRate;
} /*__attribute__ ((__packed__))*/ tLED_status;

extern void oscConfig(void);
extern void SetupPorts_I2C(void);
extern void SetupPorts( void );
extern volatile tLED_status LED_status; 
extern volatile tMeasCurrParm MeasCurrParm;

extern tSysStatus SysStatus;

extern unsigned char PeriodicMessageContents[4];

void Timer1Config();
void Timer3Config();
void Timer4Config();
void Timer5Config();
void ExternalFaultEnable();
void OverCurrentFaultIntEnable();
void InterruptPriorityConfig( void );
void SetupHWParameters(void);
void EepromSave();
void EepromLoad();
void EepromTest();
void Timer3Enable();
void Timer3Disable();

// Variables for GULP!ing
typedef union
{
    unsigned char  B[8];
    unsigned int   W[4];
    unsigned long DW[2];
} tGulp;

//
// Oscillator derived Parameters
//
#define DPLL (unsigned int)(2.0*DESIREDMIPS/PLLIN) // PLL ratio (8)
#define FOSC           (PLLIN*DPLL) // Clock frequency [Hz] (80Meg)
#define DFCY           (FOSC/2)  // Instruction cycle frequency (Hz) (40Meg)
#define DTCY           (1.0/DFCY) // Instruction cycle period (sec)

#define DDEADTIME (unsigned int)(DEADTIMESEC*DFCY) // Dead time in dTcys
#define LOOPINTCY       (DESIREDMIPS/PWMFREQUENCY)

// return state of external fault pin
#define EXTERNAL_FAULT_PRESSED()  (PORTA & 0x08)

// LED Driving
#define TurnOnLedGreen()  { TRISAbits.TRISA4 = 0; PORTAbits.RA4 = 1; }
#define TurnOnLedRed()    { TRISAbits.TRISA4 = 0; PORTAbits.RA4 = 0; }
#define TurnOffLed()      { TRISAbits.TRISA4 = 1; }

// choose witch pin is used as SPI-SS
#define SPI_SS PORTBbits.RB8

#define LED_GREEN_ON      LED_status |= 0x01
#define LED_YELLOW_ON     LED_status |= 0x02
#define LED_GREEN_OFF     LED_status &= 0xFE
#define LED_YELLOW_OFF    LED_status &= 0xFD

// LED blinking velocity
#define BLINKRATE_ULTRAFAST     10 
#define BLINKRATE_FAST          30 
#define BLINKRATE_NORMAL        150  
#define BLINKRATE_SLOW          400
#define BLINKRATE_ULTRASLOW     800
#define BLINKRATE_STILL         0
#define BLINKRATE_OFF           0xffff

#endif
