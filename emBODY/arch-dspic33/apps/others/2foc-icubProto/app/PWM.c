//   
//  PWM related rouines:
//  
//  Initialization, control and fault management 
//  
// 

#include <libpic30.h>

#include "PWM.h"
#include "UserParms.h"
#include "status.h"
#include "system.h"

extern tSysStatus SysStatus;

void SetupPWM(void)
// init for a center aligned pwm with sincronized ADC 
{
  // OpenMCPWM1(LOOPINTCY/2, LOOPINTCY/2 -80,0x8002, 0x0077, 0);
  // Motor PWM
  PDC1 = 0;
  PDC2 = 0;
  PDC3 = 0;

  // Center aligned PWM.
  // Note: The PWM period setting register is set to LoopInTcy/2 but since it counts up and 
  // then down => the interrupt flag is set to 1 at zero => actual 
  // interrupt period is LoopInTcy
  PTPER = LOOPINTCY/2;    // Setup PWM period to Loop Time defined in parms.h 
  PWMCON1 = 0x0077;       // Enable PWM 1,2,3 pairs (L+H) for complementary mode
  DTCON1 = (0x40 | (DDEADTIME/2)); // Dead time
  DTCON2 = 0;

// TODO: configure FAULTA pin for OverCurrent protection
  // PWM fault pins not used
  FLTACON = 0;            
  // Set PWM for center aligned operation
  PTCON = 0x2;         
	
  // SEVTCMP: Special Event Compare Count Register 
  // Phase of ADC capture set relative to PWM cycle: 80 offset and counting down
  // to avoid ripple on the current measurement
  SEVTCMP = PTPER - 80;
  SEVTCMPbits.SEVTDIR = 1;	

  // Connect and enable PWM pads to PWM module

// TODO: sei proprio uno ZOZZONE.
//   il commento era gia scritto bastava spostarlo o cancellarlo
}

void PWMDisable()
//Called on fault. All pins are disconnected from PWM module
//and forced to low state.
{
  // Pin override register
  P1OVDCON = 0x0000;

  // disable PWM generation
  PTCONbits.PTEN = 0;

  // Signal PWM disable on sys status
  SysStatus.PWM_is_disabled= 1;
}

void PWMEnable()
// TODO: metti un commento, please
{
  PDC1 = 0;
  PDC2 = 0;
  PDC3 = 0;

  // TODO: vedi sopra
  P1OVDCON = 0x3f00;
	
  // allows driver boost capacitors to charge
  __delay32(8);

  // clear fault flag before start fault mechanism
  IFS3bits.FLTA1IF = 0;
  
  // Enable PWM generation
  PTCONbits.PTEN = 1;
  // PWM fault configuration
  // pin pair 1,2 and 3 are controlled by Fault Input A
#ifdef ENABLE_OVER_CURRENT_PROTECTION
  FLTACON = 7;
#else
  FLTACON = 0; // fault disabled 
#endif
 SysStatus.PWM_is_disabled= 0;
}
