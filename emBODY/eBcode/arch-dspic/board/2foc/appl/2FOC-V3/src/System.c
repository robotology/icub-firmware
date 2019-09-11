//
// system related functions
// system clock, IO ports, system LEDs
//

#include <p33FJ128MC802.h>

#include <dsp.h>
#include <libpic30.h>  //__delay32
#include <timer.h>

#include "i2cTsens.h"

#include "UserParms.h"
#include "System.h"
#include "UserTypes.h"
#include "qep.h"
#include "faults.h"
#include "ecan.h"
#include "crc16.h"
#include "DHES.h"
#include "2FOC.h"

void ActuateMuxLed()
// Actuate Multiplexed led ports
{
    static char slot=0;

    if (slot)
    {
        // slot for red led
        if (LED_status.Red)
            TurnOnLedRed()
        else
            TurnOffLed();
        
        slot=0;
    }
    else
    {
        // slot for green led
        if (LED_status.Green )
            TurnOnLedGreen()
        else
            TurnOffLed();

        slot=1;
    }
}

void BlinkLed()
// blink leds according to the desired blinking rate
{
    static long RedCounter=0, GreenCounter=0;

    ++RedCounter;
    ++GreenCounter;

    switch (LED_status.GreenBlinkRate)
    {
        case BLINKRATE_STILL:
            LED_status.Green=1;
            break;

        case BLINKRATE_OFF:
            LED_status.Green=0;
            break;

        default:
            // green led blinker
            if(GreenCounter >= LED_status.GreenBlinkRate)
            {
                GreenCounter=0;

                LED_status.Green = !LED_status.Green;
            }
    }

    switch  (LED_status.RedBlinkRate)
    {
        case BLINKRATE_STILL:
            LED_status.Red=1;
            break;

        case BLINKRATE_OFF:
            LED_status.Red=0;
            break;

        default:
            if  (RedCounter >= LED_status.RedBlinkRate)
            {
                RedCounter = 0;

                LED_status.Red = !LED_status.Red;
            }
    }
}

void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt(void)
// Timer 3 IRQ service routine
// used to run I2T (in order to unwind it) when 2FOC loop is stopped
// and to perform encoder turn count
{
    extern volatile long VqRef;

    VqRef = 0;

    I2Tdata.IQMeasured = 0;
    I2Tdata.IDMeasured = 0;

    if (!MotorConfig.has_tsens)
    {
        static int i2t_undersampler = 0;

        if (++i2t_undersampler >= 20)
        {
            i2t_undersampler = 0;

            I2Twatcher();
        }
    }

    if (MotorConfig.has_hall) DHESRead();

    updateOdometry();

    IFS0bits.T3IF = 0; // clear flag
}

void __attribute__((interrupt, no_auto_psv)) _MI2C1Interrupt(void)
{
        //IFS1bits.MI2C1IF = 0;	//Clear the DMA0 Interrupt Flag;
}

//#define WAITFOR(flag,errcode,jump) for (wdog=12800; wdog && (flag); --wdog); if (!wdog) { gTemperature=errcode; goto jump; }

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)
//
// TIMER 1 IRQ Service Routine
// used for  low frequency operation (LED toggling reference....)
//
{
    // polls for recovered can error in order to reset status flags
    ECANCheckState();
    // blink leds according to the desired blinking rate
    BlinkLed();
    // light the leds
    ActuateMuxLed();

    if (MotorConfig.has_tsens)
    {
        static int cycle = 0;
        if (++cycle>=20)
        {
            cycle = 0;
            gTemperature = readI2CTsens();
        
            if (gTemperature < 0)
            {
                ++i2cERRORS;
            }
            else if (gTemperature > 3468) // = 100 C   T = 0.0297*i2c - 3;
            {
                //The temperature grew too much. Protect!
                SysError.I2TFailure = 1;
                FaultConditionsHandler();
            }
        }
    }
  
    IFS0bits.T1IF = 0; // clear flag
}

void Timer1Config()
// Setup timer 1 registers for low frequency operation (LED, .... )
{
    // reset timer
    WriteTimer1(0);
    // 4,82 msec (207.4Hz)
    OpenTimer1(T1_ON & T1_GATE_OFF & T1_IDLE_STOP & T1_PS_1_64 & T1_SYNC_EXT_OFF & T1_SOURCE_INT, 3000);
}

void Timer3Enable()
{
    WriteTimer3(0);
    IFS0bits.T3IF = 0; // clear flag
    EnableIntT3;
}

void Timer3Disable()
{
    DisableIntT3;
}

void Timer3Config()
// Setup timer 3 for I2T unwind when the board is faulted or in shutdown..
{
    // reset timer
    WriteTimer3(0);
    // should be at the same freq of 2FOC loop
    OpenTimer3(T3_ON & T3_GATE_OFF & T3_IDLE_STOP & T3_PS_1_1 & /*  T3_SYNC_EXT_OFF &*/ T3_SOURCE_INT, LOOPINTCY);
}

void Timer4Config()
// Setup timer 4 registers for CAN send
{
    unsigned int timertick;

    // reset timer
    WriteTimer4(0);
    // one timertick is 1.6 us
    timertick = ((CAN_OUTPUT_DATARATE * 100.0) / (1.6));
    OpenTimer4(T4_ON & T4_GATE_OFF & T4_IDLE_STOP & T4_PS_1_64 & T4_SOURCE_INT, timertick );
}

void oscConfig(void)
{
    /*  Configure Oscillator to operate the device at 40MIPS
    Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
    Fosc= 8M*40/(2*2)=80Mhz for 8M input clock */

    // set up the oscillator and PLL for 40 MIPS
    //            Crystal Frequency  * (DIVISOR+2)
    // Fcy =     ---------------------------------
    //              PLLPOST * (PRESCLR+2) * 4
    // Crystal  = Defined in UserParms.h
    // Fosc    = Crystal * dPLL defined in UserParms.h
    // Fcy    = DesiredMIPs

    if(OSCCONbits.COSC == 0b011)
    {
        // already running on PLL (set by bootloader)
        // TODO decide wheter to trust the bootloader or
        // to switch back to EC or FRC and reconfigure
        return;
    }

    // dPLL derived in UserParms.h
    PLLFBD = (int)(DPLL * 4 - 2);
    // N1=2
    CLKDIVbits.PLLPOST = 0;
    // N2=2
    CLKDIVbits.PLLPRE = 0;

    __builtin_write_OSCCONH(0x03);    // Initiate Clock Switch to Primary Oscillator (EC) with PLL (NOSC=0b011)
    __builtin_write_OSCCONL(0x01);    // Start clock switching

    while(OSCCONbits.COSC != 0b011);  // Wait for PLL to lock
    while(OSCCONbits.LOCK != 1);
}

void ExternalFaultEnable()
// Enables the external fault (pushbutton) interrupt
{
    // Enable change notification for external fault
    //CNEN1 |= 0;
    CNEN2 |= 1<<13;
    // clear irq flag
    IFS1bits.CNIF = 0;
}

void OverCurrentFaultIntEnable()
// over current fault interrupt enable
{
    // clear irq flag
    IFS3bits.FLTA1IF = 0;
    //enable overcurrent fault interrupt
    IEC3bits.FLTA1IE = 1;
}

void SetupHWParameters(void)
// Setup variables related to HW implementation
// Current conversion factor and offset
//
{
    //
    // configure DSP core
    //

    // multiplication are NOT unsigned
    CORCONbits.US  = 0;
    // enable saturation on DSP data write
    CORCONbits.SATDW = 1;
    // ENABLE saturation on accumulator A.
    CORCONbits.SATA  = 1;

    //  do super saturation
    CORCONbits.ACCSAT  = 1;
    // program space not visible in data space
    CORCONbits.PSV  = 0;
    // conventional rounding mode
    CORCONbits.RND  = 1;

    // DSP multiplication are in fractional mode.
    CORCONbits.IF  = 0;

    //
    // ADC - Current Measure
    //

    // Current scaling constants: Determined by hardware design
    //MeasCurrParm.qKa = DQKA;
    //MeasCurrParm.qKb = DQKB;
    //MeasCurrParm.qKc = DQKC;

    //
    // ADC - VDC-Link Measure
    //
}

void InterruptPriorityConfig( void )
// Initialize interrupt priority registers
// WARNING: Unused interrupt are currently set to priority 0 (disabled !).
// WARNING: Faults are handled at max priority, even when int are diabled.
// Pay attention to race conditions (classic scenario:
//  lower priority int preempted by fault isr reenables all things
//  after fault isr terminates).
//
// The higher the number the highest the priority
//  7: FAULT(Overload+External)
//  6: ADCDMA
//  5: SPI(DMA)
//  4: CANTX-TIMER4
//  4: CAN RX
//  3: VELOCITY calc TIMER2
//  1: TIMER1... low freq.
{
    // all IPCx irqs disabled
    IPC0 = 0;
    IPC0bits.T1IP    = 1; // Timer1 (IDLE time operations) prioriy 1 (lowest)

    // all IPCx irqs disabled
    IPC1 = 0;
    IPC1bits.DMA0IP  = 6; // DMA0 (adc triggered) priority 6
    //IPC1bits.T2IP    = 3; // TIMER2 Velocity calculation timer

    // all IPCx irqs disabled
    IPC2 = 0;
    IPC2bits.SPI1IP  = 5; // SPI int priority 5
    IPC2bits.SPI1EIP = 5; // SPI error INT prioriy 5;
    IPC2bits.T3IP    = 5; // I2T when the 2foc loop is not running (fault,shutdown)

    // TODO . handle SPI error int.

    IPC3 = 0;
    IPC3bits.DMA1IP  = 4; // Can TX (DMA1) priority 4;
    IPC3bits.AD1IP   = 6; // ADC interrupt (if not in dma mode)

    IPC4 = 0;
    IPC4bits.CNIP    = 7; // external fault interrupt priority 7( highest,
    // not masked by DISI instruction. for fault)
    // incidentally this is also the same prio of the QEP rotor alignement (when int are used
    // instead of QEP hw)

    IPC5 = 0;

    IPC6 = 0;
    IPC6bits.DMA2IP   = 4; // Can RX (DMA2) priority 4;
    IPC6bits.T4IP     = 4; // CAN tx timer

    IPC7 = 0;

    IPC8 = 0;
    IPC8bits.C1RXIP   = 0; // can RX not in DMA mode
    IPC8bits.C1IP     = 4;
    //IPC8bits.C1IP1
    //IPC8bits.C1IP2

    IPC9 = 0;
    IPC9bits.DMA3IP   = 5; //DMA3 (spi) priority 5;
  
    IPC11 = 0;

    IPC14 = 0;
    IPC14bits.QEI1IP  = 3; // Quadrature encoder interrupt

    IPC15 = 0;
    IPC15bits.FLTA1IP = 7; // Fault max priority

    IPC16 = 0;
    IPC17 = 0;
    IPC18 = 0;

    // Ensure interrupt nesting is enabled
    INTCON1bits.NSTDIS = 0;
}

void SetupPorts_LED(void)
// config dsPIC pins for LED usage
{
    // LED Green and Red
    TRISAbits.TRISA4 = 0;
}

void SetupPorts_CAN(void)
// config dsPIC pins for CAN usage
{
    // CAN bus pin configuration:
    // Set RP4 as output CANTX (valid for both HW revisions)
    RPOR2bits.RP4R = 0b10000;

    // CANRx connesso a RP2
    RPINR26bits.C1RXR = 2;
}

void SetupPorts_EXTF(void)
// config dsPIC pins for External Fault usage
{
}

void SetupPorts_QE(void)
// config dsPIC pins for quadrature encoder usage
{
    // QEA su HEV
    // Connect QEI1 Phase A to RP5
    RPINR14bits.QEA1R = 5;
    // QEB su HEW
    // Connect QEI1 Phase B to RP6
    RPINR14bits.QEB1R = 6;
    // QEZ su HEU
    // Connect QEI1 Index to RP7
    RPINR15bits.INDX1R = 7;
    // RP5, RP6, RP7 are used for Quadrature encoders and for Hall Effect Sesnsors
    // Configure QEI or HES pins as digital inputs

    // TODO: verificare l'inizializzazione di ADPCFG
    ADPCFG |= (1<<5) | (1<<6) | (1<<7); // QEA/HESU=pin5, QEB/HESV=pin6, QEZ/HESW=pin7
}

void SetupPorts_OCF(void)
// config dsPIC pins for Over Current Fault usage
{
    // Overcurrent
    // Connect PWM overcurrent Fault to RP1
    RPINR12bits.FLTA1R = 1;
}

void SetupPorts_AI(void)
// config dsPIC pins for Analog Input usage
{
    // Analog inputs
    AD1PCFGLbits.PCFG0 = 0; // AN0 analog - IA
    AD1PCFGLbits.PCFG1 = 0; // AN1 analog - IB
    AD1PCFGLbits.PCFG2 = 0; // AN2 analog - VBUS
}

void SetupPorts_SPI(void)
// config dsPIC pins for SPI usage
{
    //  RP8 as SPI-SS (HV)
    // set port P8 as output on HV
    TRISBbits.TRISB8 = 0;
    // Slave select is driven in Bit Banging
    // P8 connected to B8 port
    RPOR4bits.RP8R = 0b0;
    // SPI1 Slave Select Output su rp1 HV molex
    // RPOR0bits.RP1R = 0b01001;

    //  B3 as SPI-CLK (HU)
    // set port RB3 as output
    TRISBbits.TRISB3 = 0;
    // SPI1 Clock Output su RP3 HU
    RPOR1bits.RP3R = 0b01000;

    // Connect SPI1 MISO on RP9. HW su molex
    RPINR20bits.SDI1R = 9;
}

void SetupPorts_DHES(void)
// config dsPIC pins for Digital Hall Effect Sensors usage
{
    // No particular init needed, pins already inputs
}

void SetupPorts_I2C(void)
{
    setupI2CTsens();
}

void SetupPorts( void )
//
//  init dsPic ports and peripheral mapping
//
{
    // set everything as input
    LATA  = 0x0000;
    TRISA = 0xFFFF;

    LATB  = 0x0000;
    TRISB = 0xFFFF;

    SetupPorts_LED();

    // define remappable peripheral allocation
    // see chapter 11 dsPic datasheet

    // issue an UNLOCK sequence to change OSCCON.IOLOCK (bit 6)
    __builtin_write_OSCCONL( OSCCON & ~0x40 );

    // default all pins to DIGITAL
    AD1PCFGL = 0xffff;

    // CAN
    SetupPorts_CAN();

    // External fault
    SetupPorts_EXTF();

    // Over Current Fault
    SetupPorts_OCF();

    // Analog inputs
    SetupPorts_AI();

    // SPI encoders
    //SetupPorts_SPI();

    // Quadrature encoders
    SetupPorts_QE();

    // issue an LOCK sequence
    __builtin_write_OSCCONL( OSCCON | 0x40 );

    // reset change notification peripheral
    CNEN1 = 0;
    CNEN2 = 0;
}

void __attribute__((__interrupt__,no_auto_psv)) _CNInterrupt(void)
// pin change notification int handler
{
    // clear interrupt flag
    IFS1bits.CNIF = 0;

    if (EXTERNAL_FAULT_PRESSED())
    {
        SysError.ExternalFaultAsserted = 1;

        DisableDrive();
    }
    //else
    //{
    //    SysError.ExternalFaultAsserted = 0;
    //}
}

/*
void Timer5Config()
// setup timer 5 for can setpoint timeout
{
  unsigned long timertick;
  WriteTimer5(0);

  // one timer tick is 6.4 us
  timertick = ((setpoint_watchdog_timeout ) / (6.4));

  // clamp to valid-range bounds.
  if(timertick > 0xffff) timertick = 0xffff;
  if(timertick == 0) timertick = 1;

  CloseTimer5();

  IFS1bits.T5IF = 0; // clear flag

  OpenTimer5(T5_ON  & T5_GATE_OFF & T5_IDLE_STOP & T5_PS_1_256 &
    T5_SOURCE_INT, timertick);
}
*/

/*
void Timer2Config()
// Setup timer 2 registers for velocity calculation
{
  unsigned int timertick;

  // reset timer
  WriteTimer2(0);
  // TODO: blindly stolen from timer1. Change if needed. NO, YOU CHANGE IT!
  // TODO: non ho capito! (LC)
  // 4,82 msec (207.4Hz) TODO: perchè copi e non cambi i commenti?
  // one timertick is 1.6 us
  // TODO: un commento che si capisca pare di troppo.
  timertick = ((SPEEDLOOPTIME * 1000.0*1000.0) / 1.6);
  OpenTimer2(T4_ON & T4_GATE_OFF & T4_IDLE_STOP & T4_PS_1_64 &
    T4_SOURCE_INT, timertick );
}
*/