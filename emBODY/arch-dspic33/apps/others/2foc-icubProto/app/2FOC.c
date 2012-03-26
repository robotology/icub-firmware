 /**********************************************************************
 *
 *  Filename:       2FOC.c
 *  Date:           15/11/08
 *  Revision:       P R E L I M I N A R Y 
 *
 *  Tools:          MPLAB IDE 8.63
 *                  C30  3.25
 *                 
 ***********************************************************************
 *  ...creatively derived from Microchip AN1078 source code.
 *  This code uses willingly some discouraged practices (e.g. massive use 
 *  of global variables) in order to maximize control loop performance.
 *  If you don't like it try to get a FOC@40KHz rate any other way! (-;
 ***********************************************************************
 *  Description:
 *
 *  Field Oriented Control of 3 phase PMSM/BLDC motors with  position sensors (hes + encoder)
 *  2 Currents are measured and used for current (torque) control loop at maximum speed,
 *  SVM is used as the modulation strategy.
 *   
 *  Operating mode can be switched real time between:
 *   open loop free running movment
 *   current (torque) loop
 *   speed loop
 *   Sensored Torque Control Loop with reference given by a torque sensor (strain) LPF filtered
 *   position loop without trayectory generation ( * not * yet * implemented * )
 *   point to point positioner with trayectory generation ( * not * yet * implemented * )
 *
 *  Used resources:
 *   TIMER1 for low frequency operations (LED, togglin' references...)
 *   TIMER2 for speed measurement
 *   TIMER3 for I2T unwind when the board is faulted or shutdown(2FOC loop not running)
 *   TIMER4 for CAN TX
 *   DMA CHANNEL 0 (ADC)
 *   DMA CHANNEL 1 (CAN Tx)
 *   DMA CHANNEL 2 (CAN Rx)
 *   DMA CAHNNEL 3 (SPI Rx)
 *   CAN1
 *   SPI1, SPI2
 *   QEI
 *   ADC1 (3 channels -iu,-iw, vdclink)
 *   HW CRC calculator (TLE5012)  
 *
 *  TODO: la modalita' CAN non DMA *NON* funziona ancora (aspettare per la revisione >3 del silicio)
 *  TODO: Fare la lettura del DC-Link in modo acconcio (in un irq)
 *  TODO: speed measurement (with IIR filtering?)
 *  TODO:   acceleration and d(acceleration)/dt calculation
 *  TODO:   overspeed protection
 *  TODO:   Stall detection and signalling
 *  TODO: DCLink voltage reading and low pass filtering IIR
 *  TODO:   overvoltage/undervoltage signaling
 *  TODO:   Voltage ripple compensation
 *  TODO: usare Ia e Ic per il FOC al posto di Ib calcolata (+rumorosa)
 *  TODO: fare un comando per la visualizzazione dell'offset dei convertitori
 *    MeasCurrParm.Offseta, MeasCurrParm.Offsetbc un errore nella misura dell'offset si 
 *    traduce in  cogging a coppia zero (si vede come un offset su Ia o Ib o Ic)
 *  TODO: identificazione stallo motore
 *  TODO: fare tornare la configurazione via CAN (tipo encoder, risoluzione, tipo di controllo...
 *  TODO: Segnalazione della mancata convergenza (errore a regime) dei loop di controllo
 *
 *  TODO: Inserire indirizzo CAN scheda e indirizzo sensore forza/coppia
 *  TODO: verificare periodicamnete la presenza del sensore di coppia
 *  TODO: portare a 0 la coppia in caso di morte improvvisa del sensore di coppia
 *  TODO: guardia CAN
 *  TODO: Trajectory generator and position control!
 *  TODO: errori/segnalazioni di andata a regime dei loop di controllo (magari asincrono 
 *          al foc, su un timer piu lento/meno prioritario)
 *  TODO: mettere una CRC nella EEPROM e fare una verifica CRC durante la 
 *          lettura dei parametri 
 *
 **********************************************************************/

#include <p33FJ128MC802.h>
#include <libq.h>
#include <timer.h>
#include <ecan.h>
#include <spi.h>
#include <math.h>
#include <dsp.h>
#include <libpic30.h>  //__delay32

#include "UserTypes.h"
#include "Park.h"
#include "MeasCurr.h"
#include "SVGen.h"
#include "IControl.h"
#include "pi.h"
#include "Controller.h"
#include "UserParms.h"
#include "ecan.h"
#include "Encoder.h"
#include "System.h"
#include "PWM.h"
#include "ADC.h"
#include "encoder.h"
#include "Status.h"
#include "velocity.h"
#include "EmuROM.h"
#include "TLE5012.h"
#include "I2T.h"
#include "Faults.h"
#include "DCLink.h"
#include "DS402.h"
//#include "can_proto.h"
#include "can_icubProto.h"
#include "eEbasicStorage.h"
#include "stdint.h"

char TWOFOC_FW_VERSION[] = FIRMWARE_REVISION_STRING;
char TWOFOC_HW_VERSION[] = HARDWARE_REVISION_STRING;

#define BOARD_CAN_ADDR_DEFAULT 0xE
//
// Configuration bits
//
 
// no Boot sector and write protection disabled
_FBS (BSS_NO_FLASH & BWRP_WRPROTECT_OFF);

// no secure sector and write protection disaabled
_FSS (RSS_NO_RAM & SSS_NO_FLASH & SWRP_WRPROTECT_OFF);

// Code protection disabled
_FGS(GSS_OFF); 

// Clock switching disabled Fail safe Clock Monitor disabled
// Starts from safe internal fast RC
_FOSCSEL(IESO_OFF & FNOSC_FRC); 

// Clock switching enabled and clock monitor disabled, EC external clock, OSCOUT as IOPIN
_FOSC(FCKSM_CSECMD & POSCMD_EC & OSCIOFNC_ON)

// Turn off Watchdog Timer
_FWDT (FWDTEN_OFF);
//_FWDT(WDT_ON & WDTPSA_512 & WDTPSB_1); // WD enabled 1:512*16

// PWM mode is Port registers PWM high & low active high
// alternate I2C mapped to SDA1/SCL1
// FPOR power on reset 128ms
_FPOR (PWMPIN_ON & HPOL_ON & LPOL_ON & FPWRT_PWR128);

// Use PGC3/PGD3 for programming and debugging
_FICD (ICS_PGD3 & JTAGEN_OFF); // & COE_ON ); //BKBUG_OFF

//
// Tons of globals
//
#include "globals.h"

/* 
// TODO: presto vanno in vacanza....
// THOSE MUST BE MACROS, NEVER MAKE THEM FUNCTIONS, UNLESS
// YUO CHANGE IN ORDER TO SAVE CONTEXT ON VAR INSTEAD OF STACK !!
#define PUSHSTACK	\
// save core registers related to DSP functions
{ \
  asm volatile(						\
  "mov CORCON,w0 \n\t"				\
  "push.w w0\n\t"					\
  "mov ACCBL,w0 \n\t"				\
  "push.w w0\n\t"					\
  "mov ACCBH,w0 \n\t"				\
  "push.w w0\n\t"					\
  "mov ACCBU,w0 \n\t"				\
  "push.w w0\n\t"					\
   // save DSP accumulator A on the STACK 	\
  "mov ACCAL,w0 \n\t"				\
  "push.w w0\n\t"					\
  "mov ACCAH,w0 \n\t"				\
  "push.w w0\n\t"					\
  "mov ACCAU,w0 \n\t"				\
  "push.w w0\n\t"					\
  :			\
  :			\
  :"w0");	\
}

#define POPSTACK \
// recover core registers related to DSP functions
{\
  asm volatile\
  (  // Restore DSP accumulator A   \
  "pop.w w0\n\t"					\
  "mov w0,ACCAU \n\t"				\
  "pop.w w0\n\t"					\
  "mov w0,ACCAH \n\t"				\
  "pop.w w0\n\t"					\
  "mov w0,ACCAL \n\t"				\
  // Restore DSP accumulator B 	\
  "pop.w w0\n\t"					\
  "mov w0,ACCBU \n\t"				\
  "pop.w w0\n\t"					\
  "mov w0,ACCBH \n\t"				\
  "pop.w w0\n\t"					\
  "mov w0,ACCBL \n\t"				\
  "pop.w w0\n\t"					\
  "mov w0,CORCON \n\t"				\
  :			\
  :			\
  :"w0");	\
}
*/

void __attribute__((__interrupt__,no_auto_psv)) _DMA0Interrupt(void)
//
// DMA0 IRQ Service Routine
// used for FOC loop 
//
{

 int pwmdelay = 0;
 int timeout;

//DEADCATS: inizializzando idx con qualsiasi valore il controllo di velocita gratta in una direzione.
//  E' stata individuata la causa: la coincidenza tra il reload del contatore PWM e l`aggiornamento 
//  dei valori da parte del SVG quando vengono usati encoder spi.
//  per evitare tutto cio l'irq FOC deve terminare PRIMA della meta' della PWM!!!
//   |\      _,,,---,,_
//   /,`.-'`'    -.  ;-;;,_
//  |,4-  ) )-,_..;\ (  `'-'
// '-  ''(_/--'  `-'\_)
//
//  Per questa cosa e' stato implementato un workaround: Nel caso in cui si preveda
//  che i registri stiano per essere aggiornati nel momento sbagliato, il loro
//  aggiornamento viene leggermente ritardato fino ad oltre il momento critico.
//  In questo modo se il loop FOC dovesse durare piu di meta PWM abbiamo un jitter
//  (i valori PWM vengono settati per il giro dopo) ma almeno non si inkioda tutto.
//  Questa condizione e' comunque segnalata tramite flag di errore e sarebbe meglio
//  che non si verificasse...   
//
  static int W_PID_Undersample= W_PID_UNDERSAMPLE;

#ifdef PIN_RA3_IS_DEBUG
  // debug pin
  PORTAbits.RA3 = 1;
#endif

  // This ISR is *very* high priority.
  // It might preempt lower priority ASM DSP code.
  // Above we make it sure that the preempted code can continue
  // safely when this ISR finishes. Here we ensure that THIS
  // ISR (that will use DSP ASM funtions) can work properly, and
  // so we set the CORCON register in proper way. 

  // multiplication are NOT unsigned
  CORCONbits.US  = 0;
  // enable saturation on DSP data write
  CORCONbits.SATDW = 1;
  // DISABLE saturation on accumulator A. Required by MeasCurr algo
  CORCONbits.SATA  = 0;

  // do not do super saturation (probably don't care because SATA = 0
  CORCONbits.ACCSAT  = 0;
  // program space not visible in data space
  CORCONbits.PSV  = 0;
  // conventional rounding mode
  CORCONbits.RND  = 1;

  // DSP multiplication are in fractional mode.
  CORCONbits.IF  = 0;
  // TODO: <--- move to ASM calls

  // read Meccanical postion [-1..0.9999] and angle corrected with encsyncpulsepos
  // calculate AlignedMecchanicalAngle and UnalignedMecchanicalAngle from raw encoder data 
  EncoderPosition();
  // Trigger encoder. Start to prepare data for the next reading (if needed..)
  // this for ABS encoder make SPI to initiate transfer.
  EncoderTriggerSample();

#ifdef SAMPLE_IA_IB
  // read and compensate ADC offset by MeasCurrParm.Offseta, Offsetb
  // scale currents by MeasCurrParm.qKa, qKb
  // qIc is not calculated for better performance
  MeasAndCompIaIb();
// TODO: si, si, tutto bello ma non funziona....
#else
  // read and compensate ADC offset by MeasCurrParm.Offseta, Offsetb, Offsetc
  // scale currents by MeasCurrParm.qKa, qKb, qKc
  // Calculate ParkParm.qIa, qIb, qIc
  MeasAndCompIaIcCalculateIb();
#endif

  if (1 == SysStatus.OpenLoop)
  // For openloop movment measured qAngle, is overridden.
  // increase ParkParm.qAngle += 0x0010; to change speed.
  // Id anda Iq are imposed constants
  {
    static unsigned int cnt = 0;
    static unsigned int DeltaTheta=OL_MIN_DELTA_THETA;

    // force the angle increase
	if((cnt % OPENLOOP_DEGRADATION) == 0){
		ParkParm.qAngle = OpenloopAngle;
	}

    OpenloopAngle += DeltaTheta;
    cnt++;
    if( (cnt > OL_RAMP_SLOPE) && (DeltaTheta < OL_MAX_DELTA_THETA))
    { 
      DeltaTheta++;
      cnt = 0;
    }
  }
  else // Closed Loop (torque, speed, position)
  {
    // Using QE the starting phase must be allineated with the maximum of the flux.
    // this is done in open loop mode making a complete swap of electrical angle to drag the rotor
    // from angle 0x01 to 0xFFFF
// TODO: if the encoder is provided with a Index than align to the Index!

    if(1 == SysStatus.InitialRotorAlignmentInProgress)
    {
      // Rotor alignment is done spinning the field for a complete electrical turn and dragging
      // the rotor in position. 
      // TODO: verificare l'aggancio del rotore e catturare lo zero encoder per aumentare la precisione dello zero
      AlignRotor();
    }
    else
    {
      // Rotor alignment completed start normal control loops

      Current_position =  AlignedMecchanicalAngle;
      // get velocity measurement from encoder registers

#ifdef TLE_INTERNAL_SPEED
      // Attentione!!!!:
      // la lettura di velocita' deriva nel caso dell'encoder tle dalla lettura della 
      // posizione, calcolo CRC e accettazione dato  NON deve essere spostata dal loop FOC
      // in quanto deve completarsi entro il ciclo.
      tle_speed = EncoderVelocity();
#else
     // TODO: il calcolo della valocita` fatto oldstyle va infilato 
     // in alternativa in questo ramo ifdef
     
     // Use FW calculated speed
     // ....
#endif
     
// se ho capito come funziona mi sembra che sia una cagata. perchè da 60000 a 1000?
// non dovrebbe essere fatto con un delta di mezzascala (32735)?
// TODO: rivedere per benino 'sta roba! a occhio direi che "è una boiata pazesca" [cit.]
// TODO: place constans in #define
// TODO: what happen if even the 32 bit counter overflows?
// TODO: NON SI CAPISCE UNA MAZZA.

      // detects and count complete rounds.
      if(Current_position  > 20000 && Previous_position < -20000)
      {
        Turns--;
      }
      else 
      {
        if(Current_position < -20000 && Previous_position > 20000)
        {
          Turns++;
        }
      }
    Previous_position = Current_position;

// TODO: boh, io sto giro di conti per rimettere esattamente quanto letto da 
// EncoderPosition() due righe più su non lo capisco mica....

      Current_position = Current_position + ((long)Turns)* 0x10000;

#ifdef COMPLEX_VELOCITY_CALCULATION
// 
// for complex velocity calculation algorithm
//
// TODO: la verifica dello speedundersampler peggiora le prestazioni del jitter del looop foc!
//   tantovale farlo ad ogni ciclo!
//      if(speed_undersample++ == VELOCITY_CALCULATION_UNDERSAMPLE)
      {
        int idx;

//      speed_undersample = 0;
        // increment index for velocity calculation ring buffer
        idx = (VelocityParm.positions_buffer_index+1) % POSITIONS_BUFFER_NUM;
        // fill the ring buffer for velocity calculation
        VelocityParm.positions_buffer[idx] = (position);
        // never write VelocityParm.positions_buffer_index before filled that location
        VelocityParm.positions_buffer_index = idx;
      }
#else
// 
// simple velocity calculation method
//   no buffering used, only current position and last position stored at the previous speed cycle
// 
{
  if(speed_undersampler++ == W_SIMPLE_CALCULATION_UNDERSAMPLE)
  //  with FOC@30KHz velocity is calculated @1KHz
  {
    VelocityParm.Velocity = (Current_position - Previous_position_for_velocity_calc)/(W_SIMPLE_CALCULATION_DIVISOR);
    Previous_position_for_velocity_calc = Current_position;

    speed_undersampler = 0;
  }
}
#endif

#ifdef COMP_POSITION_READ_DELAY
// TODO: c'è ancora qualche casinetto...
//   l'encoder AS4045 fa casini girando, forse è roba di ottimizzazioni...
      
      // using SPI encoders the (fixed) time needed for data communication
      // can be taken in account and compensated. 
      // the angle read from SPI is corrected taking in account the rotor speed and the 
      // communication speed
      {
        int encoder_delay_correction;
        // compensate for encoder delay. 
        // This avoids unwanted flux disalignment when speeding up..
        // TODO: how the mplam c30 compiles the double operation? Check for algo
        // complexity and eventually change (fixed point, MPY etc)
        encoder_delay_correction = ((double)VelocityParm.last_velocity * ENCODER_DELAY);
//TODO: è fatto di merda. ENCODER_DELAY deve essere inizializzato in funzione del tipo di encoder definito in userparams!!! 

        // read Meccanical postion,calculate electrical position in ParkParm.qAngle
  #ifdef ENCODER_DHES
        // The DHES returns directly the electrical angle!
        ParkParm.qAngle =  (AlignedMecchanicalAngle + encoder_delay_correction);
  #else
        ParkParm.qAngle =  (AlignedMecchanicalAngle + encoder_delay_correction) * NPOLEPAIRS;
  #endif
      }
#else
  // do not compensate position delay
  #ifdef ENCODER_DHES
      // The DHES returns directly the electrical angle!
      ParkParm.qAngle =  AlignedMecchanicalAngle;
  #else
      // TODO: portare dentro, oppure fare una ElectricalPosition()
      // read Meccanical postion,calculate electrical position in ParkParm.qAngle
      ParkParm.qAngle =  AlignedMecchanicalAngle * NPOLEPAIRS;
  #endif
#endif
    }
  }

  // per fare un cambio al volo tra encoder e sonde di hall:
  // mettere un encoder assoluto e scommentare
  //
  // void ForceHESEmulation(); 

  // Calculate ParkParm.qSin,qCos from ParkParm.qAngle
  SinCos();

  // Calculate ParkParm.qId,qIq (and ParkParm.qIalpha,qIbeta) from ParkParm.qSin,qCos,qIa,qIb
  ClarkePark();

  // perform I2T protection from ParkParm.qId,qIq 
 
  // I2TProtect();

  // perform Torque Sensored Control Loop
  if(1 == SysStatus.TorqueSensorLoop)
  {
    // check if the zero torque from the sensor has been calibrated 
    // the name of the variable is extremly misleading but...
    if(1 == SysStatus.LoopRunning)
    {
// TODO: rimuovere l'undersampler
if ( ASCounter == 1)
{// set elaborated torque as torque reference   
CtrlReferences.qIqRef = Torque;
ASCounter=TorcAndersempling;}
else{ASCounter--;}
    }
  }
  // nel caso si voglia utilizzare l`undersampler sommentare un po di roba...
    if((1 == SysStatus.SpeedControl) && (0 == W_PID_Undersample--))
  // reset PID undersampling counter
  // rimettere all'interno del loop W_PID_Undersample = W_PID_UNDERSAMPLE;

  // do velocity control loop
  // calculate ICtrlParm.qIdRef from current velocity and PID parameters
  //if(1 == SysStatus.SpeedControl)
  {
W_PID_Undersample = W_PID_UNDERSAMPLE;
    // run speed loop PID calculate CtrlReferences.qIqRef from VelocityParm.last_velocity, 
    //   CtrlReferences.qWRef and PID parameters
    OmegaControl();

    // per il loop di controllo in assembler
    // CtrlReferences.qIqRef = PIParmW.qOut;
  }

  // Calculate ParkParm.Vd,Vq control values from ParkParm.qId,qIq and ICtrlParm.qIdRef,qIqRef
  // TODO: chiedere a Francesco se gli torna la banana...
  IdIqControl();

  if (1 == SysStatus.OpenLoop) 
  // For openloop movment calculated values for qVd, qVq are overridden
  {
    // Override Vd and Vq for open loop operation (deflux)
    ParkParm.qVd = IRA_VD; // 0;
    // force a value for Vq (controls Iq = torque)
    ParkParm.qVq = IRA_VQ; 
  }
  if ( 1 == SysStatus.InitialRotorAlignmentInProgress )
  // For Initial Rotor Alignment calculated values for qVd, qVq are overridden
  {
    // Override Vd and Vq for open loop operation (deflux)
    ParkParm.qVd = IraqVd; // 0;
    // force a value for Vq (controls Iq = torque)
    ParkParm.qVq = IraqVq; 
  }

  // Calculate ParkParm.qValpha,qVbeta from ParkParm.qSin,qCos,qVd,qVq
  InvPark();
  // Calculate SVGenParm.Vr1,Vr2,Vr3 from ParkParm.qValpha,qVbeta (InvClarke())
  CalcRefVec();

  pwmdelay =0;
  timeout = PWMSAFETIME;
  // if we are close enough to the PCD register shadow copy then wait until
  // its done before updating local PDCs
  // The condition will became false when the PWM is counting down but it is still
  // far from 0 or when it change direction counting up
  //      PWM count DN                    PWM count < PWM_SAFE_TIME
  while((P1TMR & 0x8000) && ((P1TMR & 0x7fff) < PWMSAFETIME)){
	pwmdelay = 1;
	if(0==timeout--){
		SysError.FirmwarePWMFatalError = 1;
		break;
	}
  }

  if(pwmdelay){
	// if the PWM PDC update has been delayed, then increment
    // the counter that keeps track of this event, but be
    // careful not to overflow it
	if(FirmwarePWMTimingErrorCount < 0xff)
		FirmwarePWMTimingErrorCount++;
  }

  // Calculate and set PWM duty cycles (PDC1, PDC2, PDC3) from SVGenParm.Vr1,Vr2,Vr3
  CalcSVGen();


  
  // when gulp_update_request is == 1, the main loop will not access those vars. 
  if(1 == gulp_update_request)
  {  
    // GULP!ed variables
    Gulp.W[0] = *PeriodicData[PeriodicMessageContents[0]];
    Gulp.W[1] = *PeriodicData[PeriodicMessageContents[1]];
    Gulp.W[2] = *PeriodicData[PeriodicMessageContents[2]];
    Gulp.W[3] = *PeriodicData[PeriodicMessageContents[3]];

    //Gulp.DW[1] = Current_position;

    // Draw Hall effect angle
    // DrawHallEffectAngle();
    
    // unlock the main loop, so it will read values just updated
    gulp_update_request = 0;

    // read VDC link raw value;
    VDCLink = ADCGetVDCLink();
  }

  // Give current measures to the I2T integrator
  I2Tdata.IQMeasured = /*abs*/(ParkParm.qIq);
  I2Tdata.IDMeasured = /*abs*/(ParkParm.qId);

  // Invoke the I2T integrator with new current values
  if(I2T(&I2Tdata)){
    //The I2T grew too much. Protect!
    SysError.I2TFailure = 1;
    FaultConditionsHandler();
  }

  debug_i2t= I2Tdata.Acc[1];

  //Clear the DMA0 Interrupt Flag
  IFS0bits.DMA0IF = 0;

#ifdef PIN_RA3_IS_DEBUG
  // debug pin
  PORTAbits.RA3 = 0;
#endif

}

void EMUROMTestCRC()
// EMUROM CRC checks 
{
  // TODO: giustappunto.

  SysError.EMUROMCRCFault = 0;
}

void EncoderSelfTest()
// encoder selftest (when possible)
{
  // TODO: giustappunto.

  SysError.EncoderFault = 0;
}

void ADCDoOffsetTest()
// ADC Offset test
{
  // TODO: giustappunto.
 // A questo punto la calibrazioni e stata fatta
 // Se e' fallita e noi qui resettiamo il flag allora
 // non sapremo mai se e' fallita!!
 // SysError.ADCCalFailure = 0;
}


void ZeroControlReferences()
// zero control set-point
{
  // disable toggling references
  TogglingEnable = 0;
  // 
  OmegaTogglingReference = Q15(0.0);
  TorqueTogglingReference = Q15(0.0);

  // Id (flux) Iq (torque) reference values 
  CtrlReferences.qIdRef = Q15(0.0);
  // this parameter controls the startup torque reference
  CtrlReferences.qIqRef = Q15(0.0);

  // Omega setpoint
  CtrlReferences.qWRef = Q15(0.0);

  // TODO: zero position and sensored torque references 
}

void DriveInit()
// Perform drive HW init
{


  // Reset all system error flags
  memset(SysError.b,0,sizeof(SysError));

  // Avoid leaving any random garbage in SysStatus
  memset(SysStatus.b, 0, sizeof(SysStatus));

  // IIR filters initialization
  iirt.numSectionsLess1 = 2;  // Nbq - 1
  iirt.coeffsBase = &IirTransposedCoefs[0];
  iirt.coeffsPage = COEFFS_IN_DATA;
  iirt.delayBase1 = &IirTransposedState1[0];
  iirt.delayBase2 = &IirTransposedState2[0];
  iirt.finalShift = 0;
  // Zero the filter state
  IIRTransposedInit(&iirt);


  I2TInit(&I2Tdata);
  

  // setup for a center aigned PWM
  SetupPWM();

  // setup and perform ADC offset calibration in MeasCurrParm.Offseta and Offsetb 
  ADCDoOffsetCalibration();

  // Enable DMA interrupt, arm DMA for transfer
  ADCConfigPWMandDMAMode();

  // setup the position sensor
  EncoderInit();

  // init total number of turns
  Turns = 0;

  // initialize parameters for velocity calculation
  // encodersyncpulseposition is set to 0
  // so the position buffer must be initialized with the initial position of the encoder 
  VelocityInit();

  // Setup timer for velocity calculation and velocity control loop
  Timer2Config();

  // Setup timer for CAN periodic send
  Timer4Config();

  // setup Hw related parameters (current gain, ecc...)
  SetupHWParameters();

  // init PIDs and references
  SetupControlParameters(); 

  // define default control strategy *only*one*can*be*activated* 
  //SysStatus.OpenLoop = 1;
  SysStatus.TorqueControl = 1;
  //SysStatus.TorqueSensorLoop = 1;
  //SysStatus.SpeedControl = 1;
  //SysStatus.PositionControl = 1;

  // zero control set-point
  ZeroControlReferences();

  // Enables the external fault (pushbutton) interrupt
  // conditioned to PIN_RA3_IS_DEBUG
  ExternalFaultEnable();

  // load eeprom data
  EepromLoad();

}

void DriveSelfTest()
// Perform drive HW selftest
{
  // Selftest EMUROM
  EMUROMTestCRC();

  // Selftest EMUROM
  EncoderSelfTest();

  // Test ADC offset 
  ADCDoOffsetTest();
}

void ApplyBrake()
// 2FOC doesn't have a brake unit
//TODO: if we want to keep this stub, then we should add some other
//stub to release brake!
{
}

void ApplyVDCLink()
// 2FOC doesn't have a switchable VDCLink
{
  DS402_Statusword.Flags.VoltageEnabled = 1;
}

void RemoveVDCLink()
// 2FOC doesn't have a switchable VDCLink
{
  DS402_Statusword.Flags.VoltageEnabled = 0;
}

void DisableDrive()
// disable the drive functions:
// - disable and zero control reference and status
// - disable PWM generators
// - disable FOC loop
{
  // disable PWM
  PWMDisable();
  // I2T Will run in behalf of timer 3 when 2FOC loop is not active
  EnableIntT3;

  // Disable ADC IRQ and DMA
  ADCInterruptAndDMADisable();

  // zero torque and speed referemces
  ZeroControlReferences();
  // Zero regulators
  ZeroRegulators();
}

int main ( void )
// 
// drive functions are controlled according to DS402 standard (when possible)
// 
{
    eEbasicstrg_res_t eE_res;
    uint32_t boardCanAddr;

/*
 * MOVED OFF notreadytoswitchon to enable LED operation in notreadytoswitchon state
 */

  // Configure Oscillator Clock Source, PLL
  oscConfig();
 
  // set up port direction and peripheral pin mapping
  SetupPorts();

  // Interrupt Priority configuration
  InterruptPriorityConfig();

  // enable change notification INT.
  // this is shared for external fault and
  // QEP homing when zero signal is used in interrupt mode.
  IEC1bits.CNIE = 1;

  // Setup timer for low frequency (LED) operations
  Timer1Config();
  // Setup timer for I2T (when 2foc loop is not running)
  Timer3Config();
  // enable timer1 interrupt
  EnableIntT1;
/*
 * END MOVED OFF notreadytoswitchon to enable LED operation in notreadytoswitchon state
 */

//VALE
  ee_basicstrg_init();
 
notrreadytoswitchon:
  // DS402 State xxxx xxxx x0xx 0000 NOT READY TO SWITCH ON:  
  // Logic power has been applied to the drive. The drive is being initialized or is running self test.
  // A brake, if present, has to be applied in this state. The drive function is disabled.

  // already initialized as NOT READY TO SWITCH ON:
  // DS402_Statusword.Flags.ReadyToSwitchOn = 0;

 // start:
 //   DS402SM.statusword = DS402_START;

  // • State Transition 0: START => NOT READY TO SWITCH ON
  //   Event: unconditional.
  //   Action: The drive self-tests and/or self-initializes.

  // if available apply braking  
  ApplyBrake();
 
// TODO: vedere se possibile verificare l'oscillatore e la verifica di porte di IO
  
  // signal NOT READY TO SWITCH ON status with green LED blinking ultrafast
  // LED blinking velocity
  LED_status.GreenBlinkRate=BLINKRATE_ULTRAFAST;

  // Drive Init  
  DriveInit();

  // Drive selftest
  DriveSelfTest();

  // Test error flags and if everything is OK change status  
  if(0 != (SysError.W[0] | SysError.W[1]))
  {
    // at this stage the error is unrecoverable and communication isn't on yet 
    // so signal with led (red ON green Blinking madly)
    LED_status.RedBlinkRate=BLINKRATE_STILL;

    // retry the init/selftest phase
    goto notrreadytoswitchon;
  }
   
LED_status.RedBlinkRate=BLINKRATE_OFF;
  // • State Transition 1: NOT READY TO SWITCH ON => SWITCH ON DISABLED
  //   Event: The drive has self-tested and initialized successfully.
  //   Action: Activate communication.

switchondisabled:
  // DS402 State xxxx xxxx x1xx 0000 SWITCH ON DISABLED:
  // Drive initialization is complete, the drive parameters have been set up.
  // Drive parameters may be changed. High voltage may not be applied to the drive, 
  // (2FOC cannot be completely DS402 compliant 'couse doesn't have 
  //  separate logic/power supplies). The drive function is disabled.

  // update status
  DS402_Statusword.Flags.SwitchOnDisabled = 1;
  DS402_Statusword.Flags.ReadyToSwitchOn = 0;
  DS402_Statusword.Flags.SwitchedOn = 0;
  DS402_Statusword.Flags.OperationEnabled = 0;

  // LED blinking velocity
  LED_status.GreenBlinkRate=BLINKRATE_ULTRAFAST;
//VALE  
//  CanIcubProtoInit(ApplicationData.EepromHeader.EE_CAN_BoardAddress);
    eE_res = ee_basicstrg_canaddress_get(&boardCanAddr);
    if(ee_basicstrg_res_ok !=eE_res)
    {
        //should never happen!!!
        boardCanAddr = BOARD_CAN_ADDR_DEFAULT;   
    }
    CanIcubProtoInit(boardCanAddr);
  
// TODO: verificare la corretta inizializzazione della comunicazione e 
//   nel caso non cambiare stato ma segnalare con i led
  if(0) // TODO: il CAN e' su?
  {
    // at this stage the error is unrecoverable and communication isn't on yet 
    // so signal with led (red ON green Blinking madly)
    LED_status.RedBlinkRate=BLINKRATE_FAST;

    // retry the CAN initializzation
    goto switchondisabled;
  }

  // • State Transition 2: SWITCH ON DISABLED => READY TO SWITCH ON
  // Event: Communiction activated or 'Shutdown' command received from host.
  // Action: None

readytoswitchon:
  // DS402 State xxxx xxxx x01x 0001 READY TO SWITCH ON:
  // High voltage may be applied to the drive. The drive parameters may be changed.
  // The drive function is disabled.

  // update status
  DS402_Statusword.Flags.SwitchOnDisabled = 0;
  DS402_Statusword.Flags.ReadyToSwitchOn = 1;
  DS402_Statusword.Flags.SwitchedOn = 0;
  DS402_Statusword.Flags.OperationEnabled = 0;
 
  // LED blinking velocity
  LED_status.GreenBlinkRate=BLINKRATE_FAST;

  DS402_Controlword.Flags.SwitchOn = 0;
  DS402_Controlword.Flags.EnableVoltage = 0;
  DS402_Controlword.Flags.EnableOperation = 0;

  // Stays in READY TO SWITCH ON until the command 
  // SwitchOn received from CAN
  while( 0 == DS402_Controlword.Flags.SwitchOn ){;}

  // • State Transition 3: READY TO SWITCH ON => SWITCHED ON
  // Event: 'Switch On' command received.
  // Action: The power section is switched on if it is not already switched on.

switchedon:
  // DS402 Status xxxx xxxx x01x 0011 SWITCHED ON:
  // High voltage has been applied to the drive. The power amplifier is ready.
  // The drive parameters may be changed. The drive function is disabled.

  // update status
  DS402_Statusword.Flags.SwitchOnDisabled = 0;
  DS402_Statusword.Flags.ReadyToSwitchOn = 0;
  DS402_Statusword.Flags.SwitchedOn = 1;
  DS402_Statusword.Flags.OperationEnabled = 0;

  LED_status.GreenBlinkRate=BLINKRATE_NORMAL;

  // (fake) enable VDCLink
  ApplyVDCLink();
  DS402_Controlword.Flags.EnableVoltage = 1;

  // Stays in SWITCHED ON until the command 
  // EnableOperation or Shutdown received from CAN
  while(1){

  	while( (0 == DS402_Controlword.Flags.EnableOperation) && (1 == DS402_Controlword.Flags.EnableVoltage) ){;}

  	if (0 == DS402_Controlword.Flags.EnableVoltage)
  	{
      // • State Transition 6: SWITCHED ON => READY TO SWITCH ON
      // Event: 'Shutdown' command received from host.
      // Action: The power section is switched off.
      DS402_Controlword.Flags.SwitchOn = 0;

	  // reset the DC link reading to a good value:
      // it will be overwritten with actual value as soon
      // as the first measurement will be ready.
      DCLinkReset();

      // reset faults
      FaultReset();

      goto readytoswitchon;
    }

    // do not switch to operational until dclink is ok
    if( 0 != SysError.OverVoltageFailure || 0 != SysError.UnderVoltageFailure) 
    { 
      continue;
    }
#ifndef NO_FAULT    
    // do not switch to operational until emergency button is pressed!
    if( ExternaFaultIsAsserted() )
    {
		 FaultExternalTriggered();
		 continue;
    }
#endif

    // do not switch to operational until the overcurrent fault has been cleared (shutdown)
    if( 0 != SysError.OverCurrentFailure)
    {
      continue;
    }
	break;
  }
  // • State Transition 4: SWITCHED ON => OPERATION ENABLE
  // Event: 'Enable Operation' command received from host.
  // Action: The drive function is enabled.

operationenable:
  // DS402 Status xxxx xxxx x01x 0111 OPERATION ENABLE:
  // No faults have been detected. The drive function is enabled and power is applied to the motor.
  // The drive parameters may be changed. (This corresponds to normal operation of the drive.)

  // update status
  DS402_Statusword.Flags.SwitchOnDisabled = 0;
  DS402_Statusword.Flags.ReadyToSwitchOn = 0;
  DS402_Statusword.Flags.SwitchedOn = 0;
  DS402_Statusword.Flags.OperationEnabled = 1;

  LED_status.GreenBlinkRate=BLINKRATE_STILL;

  // init variables for speed calc
  // and current velocity to 0
  // get current position from encoder (some s.o.a.b. could have manually moved the axe while turned off!)
// TODO: qui c'e` un gattino: quando si passa da Disabled->Enabled dopo avere mosso il rotore la velocita` ha 
//   uno sparacchio!!!! (ma non capisco perche`! sembra tutto inizializzato a puntino!)

  EncoderTriggerSample();// degatting sparacchio 
  EncoderWaitUpdate(); // degatting sparacchio 
  EncoderPosition();

  Current_position = AlignedMecchanicalAngle;
  Current_position = Current_position + ((long)Turns)* 0x10000;
  Previous_position_for_velocity_calc = Current_position;
  // VelocityParm.Velocity = 0; 
  // speed_undersampler = VELOCITY_CALCULATION_UNDERSAMPLE-1;
  VelocityParm.Velocity =  Current_position-Previous_position_for_velocity_calc;
  Previous_position_for_velocity_calc = Current_position;
  speed_undersampler = 0;
#if 0
  CtrlReferences.qWRef = Q15(0.0);

  PIParmW.qdSum = 0;
  InitPI(&PIParmW);
#endif

  ZeroRegulators();

  // enable timer2 interrupt for speed calc
  EnableIntT2;

  // Arm all the automation (PWM->ADC->DMA->FOC) but don't start yet!
  ADCInterruptAndDMAEnable();
 
  // enable timer3 interrupt for CAN periodic send
#ifndef SYNC_2FOC_TO_EMS
  EnableIntT4;
#endif

  // if rotor is not aligned perform alignment (only once each turn on)
  if(0 == SysStatus.InitialRotorAlignmentComplete) 
  {
    // Align angle 0 with rotor flux maximum
    EncoderRotorAlign();
    // TODO: detect and signal a failed rotor alignment 
  }

// TODO: enable overcurrent fault during rotor allignment
  // enable the overcurrent interrupt
  OverCurrentFaultEnable();

  // I2T will run on behalf of 2FOC loop. Stop running it in behalf of Timer 3
  DisableIntT3;
  // Starts to pump current!
  PWMEnable();

  while(1)
  {
    // TODO: rimuovere i gattini dalle n variabili della tensione
    // TODO: spostare in un posto consono
    DCLinkCheck();

    // • State Transition 5: OPERATION ENABLE => SWITCHED ON
    // Event: 'Disable Operation' command received from host.
    // Action: The drive operation will be disabled.

    if (0 == DS402_Controlword.Flags.EnableOperation)
    {
      // Disable drive functions  
      DisableDrive();

      // Stop periodic CAN communication
      DisableIntT4;

      //goto switchedon;
      goto switchedon;
    }

    // • State Transition 8: OPERATION ENABLE => READY TO SWITCH ON
    // Event: 'Shutdown' command received from host.
    // Action: The power section is switched off immediately, and the motor is free to rotate if
    // unbraked.

    if (0 == DS402_Controlword.Flags.EnableVoltage)
    {
      // Disable drive functions  
      DisableDrive();

      // Stop periodic CAN communication
      DisableIntT4;
	 
      // change status
	  // reset the DC link reading to a good value:
      // it will be overwritten with actual value as soon
      // as the first measurement will be ready.
      DCLinkReset();

      // reset faults
      FaultReset();

      goto readytoswitchon;
    }

    // • State Transition 9: OPERATION ENABLE ? SWITCH ON DISABLED
    // Event: 'Disable Voltage' command received from host.
    // Action: The power section is switched off immediately, and the motor is free to rotate if
    // unbraked.
    // This status change is UNIMPLEMENTED for it has almost the same results as transiction 8
    // ( 2FOC board has no internal DCLink switch available!)  
  };


  // should never arrive here!
  while(1);

asm("NOP");
asm("NOP");
asm("NOP");
asm("NOP");
asm("NOP");

}

/*

• State Transition 7: READY TO SWITCH ON ? SWITCH ON DISABLED
Event: 'Quick Stop' and ‘Disable Voltage’ command received from host.
Action: None

• State Transition 10: SWITCHED ON ? SWITCH ON DISABLED
Event: 'Disable Voltage' or 'Quick Stop' command received from host.
Action: The power section is switched off immediatly, and the motor is free to rotate if
unbraked.
DEVICE CONTROL Drives and Motion Control CiA DSP 402 V 2.048

• State Transition 11: OPERATION ENABLE ? QUICK STOP ACTIVE
Event: 'Quick Stop' command received from host.
Action: The quick stop function is executed.

• State Transition 12: QUICK STOP ACTIVE ? SWITCH ON DISABLED
Event: 'Quick Stop' is completed or 'Disable Voltage' command received from host.
This transition is possible, if the Quick-Stop-Option-Code is different 5 (stay in the state ‘Quick
Stop Active’).
Action: The power section is switched off.

• State Transition 13: All states ? FAULT REACTION ACTIVE
A fault has occurred in the drive.
Action: Execute appropriate fault reaction.

• State Transition 14: FAULT REACTION ACTIVE ? FAULT
Event: The fault reaction is completed.
Action: The drive function is disabled. The power section may be switched off.

• State Transition 15: FAULT ? SWITCH ON DISABLED
Event: 'Fault Reset' command received from host.
Action: A reset of the fault condition is carried out if no fault exists currently on the drive.
After leaving the state Fault the Bit 'Fault Reset' of the controlword has to be cleared by the
host.

• State Transition 16: QUICK STOP ACTIVE ? OPERATION ENABLE
Event: 'Enable Operation' command received from host. This transition is possible if the
Quick-Stop-Option-Code is 5, 6, 7 or 8 (? Chapter 10.3.5).
Action: The drive function is enabled.
*/

