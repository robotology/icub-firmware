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
    // Motor PWM
    // IDLE to 50% PWM
    PDC1 = PDC2 = PDC3 = LOOPINTCY / 2;

    PWMDisable();

    // Center aligned PWM.
    // Note: The PWM period setting register is set to LoopInTcy/2 but since it counts up and
    // then down => the interrupt flag is set to 1 at zero => actual
    // interrupt period is LoopInTcy
    PTPER = LOOPINTCY / 2; // Setup PWM period to Loop Time defined in parms.h
    PWMCON1 = 0x0077; // Enable PWM 1,2,3 pairs (L+H) for complementary mode
    DTCON1 = (0x00 | (DDEADTIME)); // Dead time. Prescaler 0
    DTCON2 = 0;

    // TODO: configure FAULTA pin for OverCurrent protection
    // PWM fault pins not used
    FLTACON = 0;
    // Set PWM for center aligned operation
    PTCON = 0x2;

    // SEVTCMP: Special Event Compare Count Register
    // Phase of ADC capture set relative to PWM cycle: when arrive to PTPER offset and counting down
    // to avoid ripple on the current measurement
    SEVTCMP = 0;
    SEVTCMPbits.SEVTDIR = 1;
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
    SysStatus.PWM_is_disabled = 1;
}

void PWMEnable()
{
    // mosfet all off, sw control
    P1OVDCON = 0x0000;
    __delay32(DDEADTIME + 5);

    // preinit PWM
    // IDLE to 50% PWM
    PDC1 = PDC2 = PDC3 = LOOPINTCY / 2;
    // Enable PWM generation
    PTCONbits.PTEN = 1;

    // clear fault flag before start fault mechanism
    IFS3bits.FLTA1IF = 0;

    // PWM fault configuration
    // pin pair 1,2 and 3 are controlled by Fault Input A
#ifdef ENABLE_OVER_CURRENT_PROTECTION
    FLTACON = 7;
#else
    FLTACON = 0; // fault disabled
#endif

    // precharge boost caps.
    // enable L1
    P1OVDCON = 0x0001;
    __delay32(DDEADTIME + 250);
    // enable L1, L2
    P1OVDCON = 0x0005;
    __delay32(DDEADTIME + 250);
    // enable L1, L2, L3
    P1OVDCON = 0x0015;
    __delay32(DDEADTIME + 250);
    // all off
    P1OVDCON = 0x0000;
    __delay32(DDEADTIME + 5);
    // PWM control
    P1OVDCON = 0x3f00;

    SysStatus.PWM_is_disabled = 0;
}
