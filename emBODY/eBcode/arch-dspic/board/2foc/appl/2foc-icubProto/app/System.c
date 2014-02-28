//
// system related functions
// system clock, IO ports, system LEDs 
//

#include <p33FJ128MC802.h>

#include <dsp.h>
#include <libpic30.h>  //__delay32
#include <timer.h>

#include "UserParms.h"
#include "System.h"
#include "UserTypes.h"
#include "I2T.h"
#include "controller.h"
#include "qep.h"
#include "faults.h"
#include "ecan.h"
#include "encoder.h"

void ActuateMuxLed()
// Actuate Multiplexed led ports
{
  static short Slot=0;

  if (0 == Slot)
  {
    // slot for green led
    if (1 == LED_status.Green )
      TurnOnLedGreen()
    else 
      TurnOffLed();
    Slot=1;
  }   
  else
  {
    // slot for red led
    if (1 == LED_status.Red)
      TurnOnLedRed()
    else 
      TurnOffLed();
    Slot=0;
  } 
}

void BlinkLed()
// blink leds according to the desired blinking rate
{
  static long RedCounter=0, GreenCounter=0;

  RedCounter++;
  GreenCounter++;
  
  switch(LED_status.GreenBlinkRate)
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

        // toggle green LED
        if(1==LED_status.Green)
        {
          LED_status.Green=0;
        }  
        else
        {
          LED_status.Green=1;
        }
      }
    break;
  } 

  switch(LED_status.RedBlinkRate)
  {
    case BLINKRATE_STILL:
      LED_status.Red=1;  
    break;

    case BLINKRATE_OFF:
      LED_status.Red=0;
    break;
   
    default: 
      // red blinker
      if(RedCounter >= LED_status.RedBlinkRate)
      {  
        RedCounter = 0;

        // toggle red LED
        if(1==LED_status.Red)
        {
          LED_status.Red=0;
        }  
        else
        {
          LED_status.Red=1;
        }
      }
    break;
  }
}

void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt(void)
// Timer 3 IRQ service routine
// used to run I2T (in order to unwind it) when 2FOC loop is stopped
// and to perform encoder turn count
{
  I2Tdata.IQMeasured = 0;
  I2Tdata.IDMeasured = 0;

  I2T(&I2Tdata);

  EncoderPosition();

#ifndef ENCODER_DHES
  // Trigger encoder. Start to prepare data for the next reading (if needed..)
  // this for ABS encoder make SPI to initiate transfer.
  EncoderTriggerSample();

  
  if(1 == SysStatus.InitialRotorAlignmentComplete)
  {
      
      // detects and count complete rounds.
      if((unsigned int)AlignedMecchanicalAngle  > POSITION_TURN_DETECT_THRESHOLD && Previous_position < 65535-POSITION_TURN_DETECT_THRESHOLD)
      {
       // the position was close to its min and now it is jumped near to its max value. Decrease turns counter

        Turns--;
      }
      else 
      {

        if((unsigned int)AlignedMecchanicalAngle < 65535-POSITION_TURN_DETECT_THRESHOLD &&  Previous_position > POSITION_TURN_DETECT_THRESHOLD)
        {
            
          // the position was close to its max and now it is jumped near to its min value. Increase turns counter
          Turns++;
        }
      }
  }

  Previous_position = AlignedMecchanicalAngle;
#endif

  IFS0bits.T3IF = 0; // clear flag 
}

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

  if(TogglingEnable)
  // toggle reference value for the control
  {
    static long stepcounter=0;

    stepcounter++;
    if(TogglingPeriod<=stepcounter)
    {
      // it's time to toggle!

      // ATTENZIONE!!!!!!
      // NON e' detto che si possa fare il negativo di un intero!
      // l'interno negativo piu' grande ha magnitudine assoluta piu'
      // grande di un'unita' rispetto al numero positivo piu' grande.
      // In altre parole non si puo' fare il negativo di 0x8000
      // Dunque se il riferimento e' 0x8000 il toggolo NON VA 
      // (ed e' "giusto").

      stepcounter=0;
      if (1 == SysStatus.SpeedControl)
      {
        // toggle speed loop reference
        if (OmegaTogglingReference == CtrlReferences.qWRef )
        {
          CtrlReferences.qWRef = -OmegaTogglingReference;
        }
        else
        {
          CtrlReferences.qWRef = OmegaTogglingReference;
        }
      }

      if (1 == SysStatus.TorqueControl)
      {
        // toggle torque loop reference
        if (TorqueTogglingReference == CtrlReferences.qIqRef )
        {
          CtrlReferences.qIqRef = -TorqueTogglingReference;
        }
        else
        {
          CtrlReferences.qIqRef = TorqueTogglingReference;
        }
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
  OpenTimer1(T1_ON & T1_GATE_OFF & T1_IDLE_STOP & T1_PS_1_64 &
    T1_SYNC_EXT_OFF & T1_SOURCE_INT, 3000);
}

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

void Timer3Config()
// Setup timer 3 for I2T unwind when the board is faulted or in shutdown.. 
{
  // reset timer
  WriteTimer3(0);
  // should be at the same freq of 2FOC loop
  OpenTimer3(T3_ON & T3_GATE_OFF & T3_IDLE_STOP & T3_PS_1_1 &
  /*  T3_SYNC_EXT_OFF &*/ T3_SOURCE_INT, LOOPINTCY);
}

void Timer4Config()
// Setup timer 4 registers for CAN send
{
  unsigned int timertick;

  // reset timer
  WriteTimer4(0);
  // one timertick is 1.6 us
  timertick = ((CAN_OUTPUT_DATARATE * 100.0) / (1.6));
  OpenTimer4(T4_ON & T4_GATE_OFF & T4_IDLE_STOP & T4_PS_1_64 &
    T4_SOURCE_INT, timertick );
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

  if(OSCCONbits.COSC == 0b011){
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
#ifndef PIN_RA3_IS_DEBUG
  // Enable change notification for external fault
  //CNEN1 |= 0;
  CNEN2 |= 1<<13;
  // clear irq flag
  IFS1bits.CNIF = 0;
#endif
}

void OverCurrentFaultIntEnable()
// over current fault interrupt enable
{
  // clear irq flag
  IFS3bits.FLTA1IF = 0;
  //enable external fault interrupt
  IEC3bits.FLTA1IE = 1;
}

void OverCurrentFaultIntDisable()
// over current fault interrupt disable
{
  //disable external fault interrupt
  IEC3bits.FLTA1IE = 0;
  // clear irq flag
  IFS3bits.FLTA1IF = 0;
}

void SetupHWParameters(void)
// Setup variables related to HW implementation
// Current conversion factor and offset
// 
{
  unsigned long pwmmax,pwmoffs;
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
  MeasCurrParm.qKa = DQKA;
  MeasCurrParm.qKb = DQKB;
  MeasCurrParm.qKc = DQKC;

  //
  // ADC - VDC-Link Measure
  //

  //
  // SV Generator 
  //

  // Set PWM period to Loop Time
  SVGenParm.iPWMPeriod = LOOPINTCY;
  
  // pwm registers range for pwm HW periph is 0 to LOOPINTCY
  pwmmax = LOOPINTCY;
  // calculate the given clamp offset for PWM regs given the
  // PWM guard percentage.
  pwmoffs = (pwmmax /100) * PWMGUARD;

  // PWM greater value accepted is PWM max minus the guard
  SVGenParm.pwmmax = pwmmax - pwmoffs;
  // PWM smaller value accepted is PWM min (zero) plus the guard
  SVGenParm.pwmmin = pwmoffs;

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
  IPC1bits.T2IP    = 3; // TIMER2 Velocity calculation timer

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
  // incidentally this is also the same prio of the QEP zero homing (when int are used 
  // instead of QEP hw 

  IPC5 = 0;

  IPC6 = 0;
  IPC6bits.DMA2IP   = 4; // Can RX (DMA2) priority 4; 
  IPC6bits.T4IP     = 4; // CAN tx timer

  IPC7 = 0;
  IPC8 = 0;
  IPC8bits.C1RXIP = 0; // can RX not in DMA mode
  IPC8bits.C1IP = 4;
  //IPC8bits.C1IP1
  //IPC8bits.C1IP2
 
  IPC9 = 0;
  IPC9bits.DMA3IP = 5; //DMA3 (spi) priority 5;
  IPC11 = 0;
  IPC14 = 0;
  
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
#ifdef HARDWARE_REVISION_4
  // LED Green and Red
  TRISAbits.TRISA4 = 0; 
#else
  // previous revision has no led available (used for CAN communication)
#endif
}

void SetupPorts_CAN(void)
// config dsPIC pins for CAN usage
{
  // CAN bus pin configuration:
  // Set RP4 as output CANTX (valid for both HW revisions)
  RPOR2bits.RP4R = 0b10000;

#ifdef HARDWARE_REVISION_4
  // CANRx connesso a RP2
  RPINR26bits.C1RXR = 2;
#else
  // HW version 2  has:
  // RP8 as CANRX
 
  // CAN Rx
  // Connect ECAN1 Receive to RPx
  // GRRRRRRRRRRRrrrrrrr.......  (!/^çò@##ç@!!)
  //
  // CANRx connesso a RP8 (LEDRG)
  RPINR26bits.C1RXR = 8;
#endif
}

void SetupPorts_EXTF(void)
// config dsPIC pins for External Fault usage
{
#ifdef PIN_RA3_IS_DEBUG
  // Use RA3 (EXTF/PROG) as Debug output
  TRISAbits.TRISA3 = 0;
  PORTAbits.RA3 = 0;
#endif
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
#ifdef HARDWARE_REVISION_4
  // Overcurrent 
  // Connect PWM overcurrent Fault to RP1
  RPINR12bits.FLTA1R = 1;
#else
  // HW version 2  has:
  // RP9 as OVL
  // Overcurrent 
  //Connect PWM1 Fault FLTA1 ro RP9
  RPINR12bits.FLTA1R = 9;
#endif
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
#ifdef HARDWARE_REVISION_4
  //  RP8 as SPI-SS (HV)
  // set port P8 as output on HV 
  TRISBbits.TRISB8 = 0;
  // Slave select is driven in Bit Banging
  // P8 connected to B8 port
  RPOR4bits.RP8R = 0b0;
  // SPI1 Slave Select Output su rp1 HV molex
  // RPOR0bits.RP1R = 0b01001; 
#else
  // HW version 2  has:
  //  B1 as SPI-SS (HV)

  // set port B1 as output on HV 
  TRISBbits.TRISB1 = 0;
  // Slave select driven in Bit Banging
  RPOR0bits.RP1R = 0b0;
  // SPI1 Slave Select Output su rp1 HV molex
  // RPOR0bits.RP1R = 0b01001; 

#if defined(ENCODER_TLE) || defined(AIE_TLE5012_PARAMS) 
  // define pin function for data output to TLE encoder 
  RPOR3bits.RP7R = 0b00111;
#endif

  // for other encoders mosi is not defined (yet)

  // SPI pin configuratio
  // set port as output
  // TRISBbits.TRISB5 = 0;
//
// ATTENZIONE mappato sui pin per il DEBUG
// N O N   F U N Z I O N A   I N   D E B U G 
//
  // RPOR2bits.RP5R = 0b01000; // SPI1 Clock Output
  // set port as output
  // TRISBbits.TRISB6 = 0;
//
// ATTENZIONE mappato sui pin per il DEBUG
// N O N   F U N Z I O N A   I N   D E B U G 
//
  // RPOR3bits.RP6R = 0b00111; // SPI1 Data Output 
                   // 0b01001; // SPI1 Slave Select Output
#endif

  //  B3 as SPI-CLK (HU)
  // set port RB3 as output
  TRISBbits.TRISB3 = 0;
  // SPI1 Clock Output su RP3 HU
  RPOR1bits.RP3R = 0b01000;

#ifdef HARDWARE_REVISION_4
  // Connect SPI1 MISO on RP9. HW su molex
  RPINR20bits.SDI1R = 9; 
#else
  // HW version 2  has:
  // RP2 as SPI-MISO (HW)

  // Conect SPI1 Data Input SDI1 to RP7
  // RPINR20bits.SDI1R = 7;  
  // Connect SPI1 MISO on RP2. HW su molex
  RPINR20bits.SDI1R = 2; 
#endif
}

void SetupPorts_DHES(void)
// config dsPIC pins for Digital Hall Effect Sensors usage
{
  // No particular init needed, pins olready inputs
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

#if defined(ENCODER_ABS) || defined(ENCODER_TLE)  || defined(ENCODER_AIE)
  // SPI encoders
  SetupPorts_SPI();
#endif

#if defined(ENCODER_QE) || defined(ENCODER_AIE)
  // Quadrature encoders
  SetupPorts_QE();
#endif

#ifdef ENCODER_DHES
  // Digital Hall Effect Sensors
  SetupPorts_DHES();
#endif

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

  // the QEP zero signal has been detected
  if(PORTBbits.RB7)
  {
    QEPZeroInt();
  }

#ifndef PIN_RA3_IS_DEBUG
  // an external fault has been triggered
  if( ExternaFaultIsAsserted())
  {
    FaultExternalTriggered();
  }
#endif  
}

void EepromSave()
// TODO: posso anche immaginare dal nome ma preferirei che ci fosse un commentino 
// (ed e' pure identata acdc).
{
  // Update values in EEPROM RAM image 
  SFRAC16 p,i,d, max;
  unsigned short chksum;
  int j;

  // I2T parameters
  ApplicationData.I2TParam = I2Tdata.Param;
  ApplicationData.I2TThreshold = I2Tdata.IThreshold;
  
  // currently D and Q PID have the same parameters
  ControllerGetCurrentDPIDParm(&p,&i,&d, &max);
  //  ControllerGetCurrentQPIDParm(&p,&i,&d);
  // Current PI(D) parameters    


  ApplicationData.CPIDP =  p;
  ApplicationData.CPIDI =  i;
  ApplicationData.CPIDD =  d;
  ApplicationData.CPIDM =  max;

  ControllerGetWPIDParm(&p,&i,&d, &max);

    // velocity PI(D) parameters
  ApplicationData.WPIDP = p;
  ApplicationData.WPIDI = i;
  ApplicationData.WPIDD = d;
  ApplicationData.WPIDM = max;


  chksum = 0;

  for(j=0;j<sizeof(ApplicationData) - sizeof(unsigned short); j++)
   chksum += ((char*)&ApplicationData)[j];

  ApplicationData.chksum = chksum;
  // Reflash to the EEPROM
  REFLASH_EMU_ROM(EMURomSpace,ApplicationData);
}

void EepromLoad()
// TODO: posso anche immaginare dal nome ma preferirei che ci fosse un commentino.
// (ed e' pure identata acdc).
{
   //retrive data from emurom
   ReadFromEmuROM();

   // I2T parameters
   I2Tdata.Param =  ApplicationData.I2TParam;
   I2Tdata.IThreshold = ApplicationData.I2TThreshold;
  
   // currently D and Q PID have the same parameters
   // Current PI(D) parameters  
   ControllerSetCurrentDPIDParm(ApplicationData.CPIDP,ApplicationData.CPIDI,ApplicationData.CPIDD,ApplicationData.CPIDM);
   ControllerSetCurrentQPIDParm(ApplicationData.CPIDP,ApplicationData.CPIDI,ApplicationData.CPIDD,ApplicationData.CPIDM);
  

   // velocity PI(D) parameters
   ControllerSetWPIDParm(ApplicationData.WPIDP,ApplicationData.WPIDI,ApplicationData.WPIDD,ApplicationData.WPIDM);
}
