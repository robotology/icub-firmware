//
// system related functions
// system clock, IO ports, system LEDs 
//
#ifndef __System_H
#define __System_H

#include "UserParms.h"
#include "MeasCurr.h"
#include "SVGen.h"
#include "Status.h"
#include "iControl.h"
#include "emurom.h"

typedef struct
{
  int Red;
  int Green;

  unsigned int RedBlinkRate;
  unsigned int GreenBlinkRate;  
} /*__attribute__ ((__packed__))*/ tLED_status;

extern void oscConfig(void);
extern void SetupPorts( void );
extern volatile tLED_status LED_status; 
extern tMeasCurrParm MeasCurrParm;

extern tSVGenParm SVGenParm;

extern tSysStatus SysStatus;
extern tCtrlReferences CtrlReferences;
extern int TogglingEnable;
extern SFRAC16 OmegaTogglingReference;
extern SFRAC16 TorqueTogglingReference;
extern unsigned int TogglingPeriod;


void Timer1Config();
void Timer2Config();
void Timer3Config();
void Timer4Config();
void ExternalFaultEnable();
void OverCurrentFaultIntEnable();
void InterruptPriorityConfig( void );
void SetupHWParameters(void);
void EepromSave();
void EepromLoad();

// Variables for GULP!ing
typedef union
{
  unsigned char B[8];
  unsigned int  W[4];
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
#define LOOPTIMEINSEC (1.0/PWMFREQUENCY)           // PWM Period = 1.0 / PWMFREQUENCY
// PWM loops per velocity calculation (and fresh samples expected for velocity calc)
#define IRP_PERCALC (unsigned int)(SPEEDLOOPTIME/LOOPTIMEINSEC)
#define SPEEDLOOPTIME (float)(1.0/W_COMPLEX_CALCULATION_FREQ)   // Speed Control Period
#define LOOPINTCY	(LOOPTIMEINSEC/DTCY)             // Basic loop period in units of Tcy
#define LOCKTIME	(unsigned int)(LOCKTIMEINSEC*(1.0/LOOPTIMEINSEC))
   // Time it takes to ramp from zero to MINSPEEDINRPM. Time represented in seconds
#define DELTA_STARTUP_RAMP	(unsigned int)(MINSPEEDINRPM*POLEPAIRS*LOOPTIMEINSEC* \
  LOOPTIMEINSEC*65536*65536/(60*OPENLOOPTIMEINSEC))
// Number of control loops that must execute before the button routine is executed.
#define	BUTPOLLOOPCNT	(unsigned int)(BUTPOLLOOPTIME/LOOPTIMEINSEC)


// return state of external fault pin
#ifndef PIN_RA3_IS_DEBUG
  #define ExternaFaultIsAsserted()  (PORTA & (1<<3))
#else
  #define ExternaFaultIsAsserted()  (0)
#endif


// LED Driving
#ifdef HARDWARE_REVISION_4
  #define TurnOnLedGreen()  { TRISAbits.TRISA4 = 0; PORTAbits.RA4 = 1; }
  #define TurnOnLedRed()    { TRISAbits.TRISA4 = 0; PORTAbits.RA4 = 0; }
  #define TurnOffLed()      { TRISAbits.TRISA4 = 1; }
#else
  // previous revision has no led available (used for CAN communication)
  #define TurnOnLedGreen()  {;}
  #define TurnOnLedRed()    {;}
  #define TurnOffLed()      {;}
#endif

// choose witch pin is used as SPI-SS
#ifdef HARDWARE_REVISION_4
  #define SPI_SS PORTBbits.RB8
#else
  #define SPI_SS PORTBbits.RB1
#endif

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
#define BLINKRATE_OFF			0xffff


// EMUROM Data
typedef struct eeprom_data {
  //  Do not change the data contained in the Header
  tEepromHeader EepromHeader; 
    
  // User data  
  SFRAC16 I2TParam;
  SFRAC16 I2TThreshold; 
  SFRAC16 CPIDP;
  SFRAC16 CPIDI;
  SFRAC16 CPIDD;
  SFRAC16 CPIDM;
  SFRAC16 WPIDP;
  SFRAC16 WPIDI;
  SFRAC16 WPIDD;
  SFRAC16 WPIDM;
    
  // add user data ahead ...
  // .... 
  SFRAC16 CurLimit;			
  unsigned short chksum;


} __attribute__((__packed__)) tEepromData;

extern DECLARE_EMU_ROM_READVAR(ApplicationData, tEepromData);
extern PLACE_EMU_ROM_DATA(EMURomSpace, tEepromData);
#endif
