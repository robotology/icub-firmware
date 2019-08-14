//   
//  PWM related rouines:
//  
//  Initialization, control and fault management 
//  
// 

#include <libpic30.h>

#include "PWM.h"
#include "Faults.h"

extern tSysStatus SysStatus;

static short PWM_50_DUTY_CYCLE = 0;
static short PWM_DEADTIME = 0;
static short PWM_MAX = 0;

void pwmInit(short pwm_50_duty_cycle, short pwm_deadtime, short pwm_max)
{
    PWM_50_DUTY_CYCLE = pwm_50_duty_cycle;
    PWM_DEADTIME = pwm_deadtime;

    pwmSetMax(pwm_max);

    pwmZero();
    pwmOFF();

    // Center aligned PWM.
    // Note: The PWM period setting register is set to LoopInTcy/2 but since it counts up and
    // then down => the interrupt flag is set to 1 at zero => actual
    // interrupt period is LoopInTcy
    PTPER = PWM_50_DUTY_CYCLE; // Setup PWM period to Loop Time defined in parms.h
    PWMCON1 = 0x0077; // Enable PWM 1,2,3 pairs (L+H) for complementary mode
    DTCON1 = (0x00 | (PWM_DEADTIME)); // Dead time. Prescaler 0
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

volatile unsigned char PWM_ovcurr_fault = 0;
volatile unsigned char PWM_retry_enable = 0;
volatile unsigned char PWM_first_enable = 1;

void __attribute__((__interrupt__,no_auto_psv)) _FLTA1Interrupt(void)
// Interrupt handler for OVERCURRENT fault.
// HW will automatically stop PWM generation.
// We need to set SW flag here in order to update SW state information
{
    // Pin override register
    P1OVDCON = 0x0000;

    // disable PWM generation
    PTCONbits.PTEN = 0;

    PWM_ovcurr_fault = 1;

    if (PWM_first_enable)
    {
        PWM_retry_enable = 1;
        PWM_first_enable = 0;
    }
    else
    {
        PWM_retry_enable = 0;
        // signal overcurrent
        SysError.OverCurrentFailure = 1;
        // call fault handler
        FaultConditionsHandler();
    }

    // clear irq flag
    IFS3bits.FLTA1IF = 0;
}

void pwmSetMax(short pwm_max)
{
    if (pwm_max < (PWM_50_DUTY_CYCLE*9)/10) PWM_MAX = pwm_max;
}

char pwmON(void)
{
enable_retry:

    PWM_ovcurr_fault = 0;

    P1OVDCON = 0x0000; // mosfet all off, sw control
    __delay32(PWM_DEADTIME + 5);

    // preinit PWM

    PDC1 = PDC2 = PDC3 = PWM_50_DUTY_CYCLE; // IDLE to 50% PWM
    PTCONbits.PTEN = 1; // Enable PWM generation

#ifndef RELENTLESS
    OverCurrentFaultIntEnable();
#endif

    FLTACON = 7; // fault enabled

    // precharge boost caps.
    
    P1OVDCON = 0x0001; // enable L1
    if (PWM_ovcurr_fault) goto on_overcurrent;
    __delay32(PWM_DEADTIME + 250);
    if (PWM_ovcurr_fault) goto on_overcurrent;

    P1OVDCON = 0x0005; // enable L1, L2
    if (PWM_ovcurr_fault) goto on_overcurrent;
    __delay32(PWM_DEADTIME + 250);
    if (PWM_ovcurr_fault) goto on_overcurrent;
    
    P1OVDCON = 0x0015; // enable L1, L2, L3
    if (PWM_ovcurr_fault) goto on_overcurrent;
    __delay32(PWM_DEADTIME + 250);
    if (PWM_ovcurr_fault) goto on_overcurrent;

    // all off
    P1OVDCON = 0x0000;
    if (PWM_ovcurr_fault) goto on_overcurrent;
    __delay32(PWM_DEADTIME + 5);
    if (PWM_ovcurr_fault) goto on_overcurrent;

    // PWM control
    P1OVDCON = 0x3f00;
    if (PWM_ovcurr_fault) goto on_overcurrent;

    PWM_first_enable = 0;

    // ALL RIGHT, GO!
    return 1;

    on_overcurrent:

    PWM_first_enable = 0;

    if (PWM_retry_enable)
    {
        __delay32(1000);
        goto enable_retry;
    }
    
    return 0;
}

inline void pwmOFF(void) __attribute__((always_inline));
void pwmOFF(void)
{
    // Pin override register
    P1OVDCON = 0x0000;

    // disable PWM generation
    PTCONbits.PTEN = 0;

    PWM_retry_enable = 0;
    PWM_first_enable = 1;
}

void pwmZero(void)
{
    PDC1 = PDC2 = PDC3 = PWM_50_DUTY_CYCLE;
}

inline void pwmOut(int V1, int V2, int V3) __attribute__((always_inline));
void pwmOut(int V1, int V2, int V3)
{
    if (V1 > PWM_MAX)
    {
        V2 = __builtin_divsd(__builtin_mulss(V2,PWM_MAX), V1);
        V3 = __builtin_divsd(__builtin_mulss(V3,PWM_MAX), V1);
        V1 = PWM_MAX;
    }
    else if (V1 < -PWM_MAX)
    {
        V2 = __builtin_divsd(__builtin_mulss(V2,PWM_MAX),-V1);
        V3 = __builtin_divsd(__builtin_mulss(V3,PWM_MAX),-V1);
        V1 = -PWM_MAX;
    }

    if (V2 > PWM_MAX)
    {
        V1 = __builtin_divsd(__builtin_mulss(V1,PWM_MAX), V2);
        V3 = __builtin_divsd(__builtin_mulss(V3,PWM_MAX), V2);
        V2 = PWM_MAX;
    }
    else if (V2 < -PWM_MAX)
    {
        V1 = __builtin_divsd(__builtin_mulss(V1,PWM_MAX),-V2);
        V3 = __builtin_divsd(__builtin_mulss(V3,PWM_MAX),-V2);
        V2 = -PWM_MAX;
    }

    if (V3 > PWM_MAX)
    {
        V1 = __builtin_divsd(__builtin_mulss(V1,PWM_MAX), V3);
        V2 = __builtin_divsd(__builtin_mulss(V2,PWM_MAX), V3);
        V3 = PWM_MAX;
    }
    else if (V3 < -PWM_MAX)
    {
        V1 = __builtin_divsd(__builtin_mulss(V1,PWM_MAX),-V3);
        V2 = __builtin_divsd(__builtin_mulss(V2,PWM_MAX),-V3);
        V3 = -PWM_MAX;
    }

    PDC1 = PWM_50_DUTY_CYCLE + V1;
    PDC2 = PWM_50_DUTY_CYCLE + V2;
    PDC3 = PWM_50_DUTY_CYCLE + V3;
}

