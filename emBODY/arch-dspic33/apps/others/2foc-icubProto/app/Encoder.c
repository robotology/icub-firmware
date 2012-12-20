//   
//  Encoder related rouines:
//  
//  position, velocity, acceleration functions
//  Initial Phase alinement 
//  
//  Available drivers for:
//    Increental encoder with (AS5040) or without index pulse (AS530x) Avago, Mecapion...
//    Absolute SPI encoders AS5045
//    Absolute+incremental TLE5012
//    DHES

// TODO: fare tutto l'error checking possibile sull' encoder (counter overflow, missing index...)
//  deve essere possibile capire quando viene tagliato il cavo !!!

#include <p33FJ128MC802.h>
#include <dsp.h>

#include "UserParms.h"
#include "Park.h"
#include "Encoder.h"
#include "AS5045.h"
#include "Faults.h"
#include "controller.h"
#include "tle5012.h"
#include "dhes_lut.h"

static unsigned int encoder_buffer;
unsigned int eb;



void EncoderInit()
// setup the appropriate Registers for SPI or QEP encoder
{
#ifdef ENCODER_QE
  QEPInit();
#endif

#ifdef ENCODER_ABS 
  as5045Init();
#endif

#ifdef ENCODER_AIE 
  // init QEP peripheral 
  QEPInit();
 
  #ifdef AIE_TLE5012_PARAMS
  // init for TLE5012
    tle5012Init();
  #else
  // init for AMS5xxx series of encoders 
    as5045Init();
  #endif
#endif

#ifdef ENCODER_DHES
  // DHES doesn't need any initialization
#endif

#ifdef ENCODER_TLE
  tle5012Init();
#endif
}

void EncoderWaitUpdate()
// polling wait for encoder data available.
// this is empty for QEP (data always available from
// MCU QEP periph. It waits SPI for ABS encoders.
{
  unsigned int timeout = 60000;

#ifdef ENCODER_ABS 
  while(!as5045SamplingCompleted())
  {
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");

    if(0==timeout--)
      break;
  }
#endif
  
#ifdef ENCODER_TLE
  while(!tle5012SamplingCompleted())
  {
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");

    if(0==timeout--)
      break;
  }
#endif

#ifdef ENCODER_AIE 
#ifdef AIE_TLE5012_PARAMS
  while(!as5045SamplingCompleted())
  {
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    if(0==timeout--)
      break;
  }
#else
  while(!tle5012TriggerSample())
  {
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");

    if(0==timeout--)
      break;
   }
#endif
#endif

  if(timeout == 0)
  {
    SysError.FirmwareSPITimingError = 1;
  }
}

void DHESTriggerSample(void)
// trigger for the next DHES angle read
{
  // no operation is needed
}

void EncoderTriggerSample()
// This should be called ASAP in the low jitter ISR.
// It causes position to be freezed. Later access with
// Encoder_position & co will return freezed position.
// This logic is needed for SPI sensor, and it is keept
// for QEP to make APIs unchanged.
{

/* no need to trigger sampling for QE because reading result 
 * can be done very quickly (SFR read).
 * We still buffer MCU QE reading for AIE in order to keep
 * in sync the QE with SPI part as much as we can
 */

#ifdef ENCODER_AIE 
  encoder_buffer = QEPGet();

  #ifdef AIE_TLE5012_PARAMS
    tle5012TriggerSample();
  #else
    as5045TriggerSample();
  #endif
#endif

#ifdef ENCODER_DHES
  DHESTriggerSample();
#endif

#ifdef ENCODER_ABS
  as5045TriggerSample();
#endif

#ifdef ENCODER_TLE
  tle5012TriggerSample();
#endif
}

inline void EncoderPositionQE() __attribute__((always_inline));
inline void EncoderPositionQE()
//
// Read raw mechanical angle from encoder and return in MecchanicalAngle a value normalized 
//  in the range [-1..0.9999999] for SPI, QEP or DHES encoders
//  and in AlignedMecchanicalAngle the value corrected with the EncoderSyncPulsePosition
//
{
  unsigned long tmp;
  // copy current encoder value to a temp 32 bit storage
  tmp = QEPGet();

  // mult by 16-bit full range (about +1 in SFRAC16)
  tmp *= 65536;
  // divide by encoder PPR*4 -1. 
  tmp /= QE_RESOLUTION - 1;
  
  // cast: if ulong = 0 then sfrac =0. if ulong > 0 and ulong < half-scale then sfrac > 0
  // if ulong > half-scale then sfrac <0.
  //
  //   SFRAC
  //  -0.01|            /
  //       |          /
  //     -1|        /
  //   +0.9|      /
  //       |    /
  //       |  /
  //     0 |/____________
  //ULONG  0     half  0xffff
  UnalignedMecchanicalAngle = (SFRAC16)(tmp);
}

inline void EncoderPositionAIE() __attribute__((always_inline));
inline void EncoderPositionAIE()
//
// Read raw mechanical angle from encoder and return in MecchanicalAngle a value normalized 
//  in the range [-1..0.9999999] for SPI, QEP or DHES encoders
//  and in AlignedMecchanicalAngle the value corrected with the EncoderSyncPulsePosition
//
{
  unsigned long tmp;

  // copy current encoder value to a temp 32 bit storage
  tmp = encoder_buffer;
  
  // mult by 16-bit full range (about +1 in SFRAC16)
  tmp *= 65536;
  // divide by encoder PPR*4 -1. 
  tmp /= QE_RESOLUTION - 1;
  
  // cast: if ulong = 0 then sfrac =0. if ulong > 0 and ulong < half-scale then sfrac > 0
  // if ulong > half-scale then sfrac <0.
  //
  //   SFRAC
  //  -0.01|            /
  //       |          /
  //     -1|        /
  //   +0.9|      /
  //       |    /
  //       |  /
  //     0 |/____________
  //ULONG  0     half  0xffff
  UnalignedMecchanicalAngle = (SFRAC16)(tmp);

}

inline void EncoderPositionABS() __attribute__((always_inline));
inline void EncoderPositionABS() 
//
// Read raw mechanical angle from encoder and return in MecchanicalAngle a value normalized 
//  in the range [-1..0.9999999] for SPI, QEP or DHES encoders
//  and in AlignedMecchanicalAngle the value corrected with the EncoderSyncPulsePosition
//
{
  unsigned long tmp;

  if(!as5045SamplingCompleted())
  {
  /* FOC ALGO HAS CALLED TOO FAST THIS.
     * Please read comment in as5045.c
     * CompleteStruct();
     */
    SysError.FirmwareSPITimingError = 1;
  }
  tmp = as5045Get().Angle;
  // tmp is in range 0..4096. Shifting left by 4 returns the greatest 
  // signed positive number if tmp is 4096. 
  tmp = ((SFRAC16)(tmp * (65536/AE_RESOLUTION)));
  UnalignedMecchanicalAngle = tmp;

}

inline void EncoderPositionTLE() __attribute__((always_inline));
inline void EncoderPositionTLE()
{
  tTLE5012_data tledata;
  int tmp=0;

  if(!tle5012SamplingCompleted())
  {
    // FOC ALGO HAS CALLED TOO FAST THIS.
    // Please read comment in as5045.c CompleteStruct();
    SysError.FirmwareSPITimingError = 1;
  }

  // get all encoder data  
  tledata = tle5012Get();

  if(0 == tledata.crcerror)
  // if CRC error keeps the last good value
  {
    // extract the angle
    tmp = tledata.angle.Bits.angleVal;
    // normalize encoder value (0..AE_RESOLUTION) to (0..65535)
    tmp = ((SFRAC16)((double)tmp * ((double)65536/(double)AE_RESOLUTION)));
    // here there is the meccanical angle 
    UnalignedMecchanicalAngle = tmp;
  }
}

inline void EncoderPositionDHS() __attribute__((always_inline));
inline void EncoderPositionDHS()
//
// Read raw mechanical angle from encoder and return in MecchanicalAngle a value normalized 
//  in the range [-1..0.9999999] for SPI, QEP or DHES encoders
//  and in AlignedMecchanicalAngle the value corrected with the EncoderSyncPulsePosition
//
{
  extern tSysError SysError;
  static int round = 0; // ELECTRICAL round conter inside a MECHANICAL round
  static int encoder_dhes_state = 0;
  signed char transition;

    // Read HES vaue from the uC pins of choice
#ifdef HES_CONNECTED_TO_P2_P5
  // mask the port where the HES are connected (RP5, RP6, RP7)
  encoder_buffer = (PORTB & 0b011100000) >> 5;
#endif

#ifdef HES_CONNECTED_TO_P1_P6
  // HU==RP3==RB3, HV==RP8==RB8, HW==RP9==RB9
  encoder_buffer = (PORTBbits.RB3) | (PORTBbits.RB8<<2) | (PORTBbits.RB9<<1);
#endif

  // NOTE: from the HES you get an Electrical angle! 

  // used to gulp the HES value
  eb=encoder_buffer;

 // encoder_buffer &= 0b111;

  if((encoder_buffer == 0) || (encoder_buffer == 7)){//if(DHES_VALUE_LUT[encoder_buffer]){ 
     SysError.HESInvalidValue=1;
     encoder_dhes_state = 0; // next time do not make assumption
     FaultConditionsHandler();
     return;
  }

  // the HES read the same val of last time. The angle
  // has not to change. Do nothing.
  // do not place before check for 0 and 7 : if the first time
  // encoder_buffer reads 000 it must fault. But the
  // first time encoder_dhes_state is 000 because it is
  // init val
  if(encoder_dhes_state == encoder_buffer)
    return;

  // if 0 no turn has been done
  // if 1 or -1 a CW or CCW turn has been done
  // if -2 error: invalid transition
  transition = DHES_TRANSITION_LUT[encoder_dhes_state][encoder_buffer];

  if(-2 == transition){
    SysError.HESInvalidSequence = 1;
    encoder_dhes_state = 0; // next time do not make assumption
    FaultConditionsHandler();
    return;
  }

  round += transition;

  UnalignedMecchanicalAngle = DHES_UNALIGNEDMECANGLE_LUT[encoder_buffer];

 // round += DHES_TURN_LUT[encoder_dhes_state][encoder_buffer];

  if(round == NPOLEPAIRS){
   round = 0;
  }else if(round == -1){
   round = NPOLEPAIRS -1;
  }

  encoder_dhes_state = encoder_buffer;

  // combine the electrical angle information with the 
  // electrical turn count to get mechanical angle
  UnalignedMecchanicalAngle  += round * (0xFFFFU/NPOLEPAIRS);

}



//   DHES PROCEDURAL REFERENCE CODE WITHOUT LUTs
//
//  Please do not delete it yet at least until
//  new code with LUTs is well tested!

/*  switch(encoder_buffer)
  {
// TODO: fare una verifica di quanto sono posizionate bene le sonde di hall:
//  confrontare encoder_buffer con il dato ricavato da un encoder vero per diversi motori
//  e vederne la ripetibilità.
//  eventualmente si può fare una taratura delle posizioni di commutazione
//  del case e vedere di nascosto l'effetto che fa.
//  vedere anche quale effetto ha la velocità di rotazione sulla lettura 

    // the HES activation sequence is 1,3,2,6,4,5
    case 1:
     
     
      if(encoder_dhes_state == 5){
        // we completed an electrical turn
        round++;

        if(round == NPOLEPAIRS){ 
        // we completed the LAST electrical turn in the 
        // mechanincal turn. So restart from electrical
        // turn 0
         round = 0;
        }

      
      }else if(encoder_dhes_state != 3){
        // if we did not 5->1 then the only
        // other good seq is 5->3. Otherwise trigger an error.
        // Exception: if prev seq val was -1 then this
        // this is the first algo iteration and we have not
        // any prev seq. val
        if(encoder_dhes_state != -1){
            SysError.HESInvalidSequence = 1;
            FaultConditionsHandler();
        }

      }

      UnalignedMecchanicalAngle = 0x0000U/NPOLEPAIRS; // 0°

      // save new dhes state
      encoder_dhes_state = 1;
     
      break;

    case 3: 

      // check if we did a valid transition or not
      if(encoder_dhes_state != 1 && encoder_dhes_state != 2){
        //if prev seq val was -1 then this
        // this is the first algo iteration and we have not
        // any prev seq. val
        if(encoder_dhes_state != -1){
            SysError.HESInvalidSequence = 1;
            FaultConditionsHandler();
        }

      }
      // insert in this variable the ELECTRICAL angle. It will
      // be converted to mechanincal below
      UnalignedMecchanicalAngle = 0x2AAAU/NPOLEPAIRS; // 60°
      encoder_dhes_state = 3;
      break;

    case 2:
      // check if we did a valid transition or not
      if(encoder_dhes_state != 3 && encoder_dhes_state != 6){
       //if prev seq val was -1 then this
       // this is the first algo iteration and we have not
       // any prev seq. val
       if(encoder_dhes_state != -1){
            SysError.HESInvalidSequence = 1;
            FaultConditionsHandler();
        }

      }

      UnalignedMecchanicalAngle = 0x5554U/NPOLEPAIRS; // 120°
      encoder_dhes_state = 2;
      break;

    case 6: 
      // check if we did a valid transition or not
      if(encoder_dhes_state != 2 && encoder_dhes_state != 4){
       //if prev seq val was -1 then this
       // this is the first algo iteration and we have not
       // any prev seq. val
       if(encoder_dhes_state != -1){
            SysError.HESInvalidSequence = 1;
            FaultConditionsHandler();
        }

      }
       // insert in this variable the ELECTRICAL angle. It will
      // be converted to mechanincal below
      UnalignedMecchanicalAngle = 0x7FFFU/NPOLEPAIRS; // 180°
      encoder_dhes_state = 6;
      break;

    case 4: 
      // check if we did a valid transition or not
      if(encoder_dhes_state != 6 && encoder_dhes_state != 5){
       //if prev seq val was -1 then this
       // this is the first algo iteration and we have not
       // any prev seq. val

       if(encoder_dhes_state != -1){
            SysError.HESInvalidSequence = 1;
            FaultConditionsHandler();
        }

      }
      // insert in this variable the ELECTRICAL angle. It will
      // be converted to mechanincal below
      UnalignedMecchanicalAngle = 0xAAA8U/NPOLEPAIRS; // 240°
      encoder_dhes_state = 4;
      break;

    case 5: 
     
      if(encoder_dhes_state == 1){
        // we completed an electrical turn
        round--;

        // we completed the LAST electrical turn in the 
        // mechanincal turn. So restart from electrical
        // turn 0
        if(round == -1) round = NPOLEPAIRS-1;

      }else if(encoder_dhes_state != 4){
        // if we did not 1->4 then the only
        // other good seq is 5->3. Otherwise trigger an error.
        // Exception: if prev seq val was -1 then this
        // this is the first algo iteration and we have not
        // any prev seq. val
       if(encoder_dhes_state != -1){
            SysError.HESInvalidSequence = 1;
            FaultConditionsHandler();
        }

      }

      // insert in this variable the ELECTRICAL angle. It will
      // be converted to mechanincal below
      UnalignedMecchanicalAngle = 0xD552U/NPOLEPAIRS; // 300°
      encoder_dhes_state = 5;
      break;

    default:
      // 000 and 111 are invalid values 
      SysError.HESInvalidValue=1;
      FaultConditionsHandler();

    break;
  }
  // combine the electrical angle information with the 
  // electrical turn count to get mechanical angle
  UnalignedMecchanicalAngle  += round * (0xFFFFU/NPOLEPAIRS);
}

*/


void EncoderPosition()
// This function provides an encoder-type independent way to update the 
// read position. It also sum the offset Encoder_syncPulsePosition to the
// read position in order to provide both AlignedMechanicalAngle and 
// UnalignedMechanicalAngle.
{

#if defined(ENCODER_AIE)
  EncoderPositionAIE();
#elif defined(ENCODER_QE)
  EncoderPositionQE();
#elif defined(ENCODER_ABS)
  EncoderPositionABS();
#elif defined (ENCODER_TLE)
  EncoderPositionTLE();
#elif defined (ENCODER_DHES)
  EncoderPositionDHS();
#else
#error EncoderPositionCannotHandleThisEncoder
#endif

  AlignedMecchanicalAngle = UnalignedMecchanicalAngle + Encoder_SyncPulsePosition;

#ifdef ENCODER_DEGRADATION_ENABLED
  AlignedMecchanicalAngle /= EncoderDegradation;
  AlignedMecchanicalAngle *= EncoderDegradation;
#endif //ENCODER_DEGRADATION_ENABLED
}


void EncoderRotorAlign()
// Align encoder index with maximum flux angle
{

  // assert that the rotor init is in progress 
  // for Incremental encoders the phase is completed when a complete open loop electrical 
  // angle sweep is completed   

//TODO: completare la fasatura iniziale 
  SysStatus.InitialRotorAlignmentInProgress = 1;

#if defined(ENCODER_ABS) || ( defined(ENCODER_AIE) && !(defined(AIE_TLE5012_PARAMS))) 
// ABS encoder and AMS5xxx AIE get the rotor alignment reading the SPI absolute position from AS504x   
  unsigned int tmp;
  unsigned int timeout = 60000;

// TODO: avoid double read.
// check for errors instead.
  as5045TriggerSample();

 while(!as5045SamplingCompleted()){
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");

    if(0==timeout--)
      break;
   }

  as5045TriggerSample();

 while(!as5045SamplingCompleted()){
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");

    if(0==timeout--)
      break;
   }

  tmp = as5045Get().Angle;
  // tmp is in range 0..4096. Shifting left by 4 returns the greatest 
  // signed positive number if tmp is 4096. 
    Encoder_RotorStartingPosition = ((SFRAC16)(tmp*(65536/AE_RESOLUTION)));
// TODO: pensaci un poco bene ma mi pare che in ogni caso Encoder_SyncPulsePosition va messo =0
  Encoder_SyncPulsePosition = 0;

  // init the variables used for position sensing 
  Previous_position = Encoder_RotorStartingPosition;
  Current_position = Encoder_RotorStartingPosition;
  Previous_position_for_velocity_calc = Encoder_RotorStartingPosition;

  // Rotor alignment completed
  SysStatus.InitialRotorAlignmentComplete=1;
  SysStatus.InitialRotorAlignmentInProgress=0;
#endif

#if defined(ENCODER_TLE) || ( defined(ENCODER_AIE) && (defined(AIE_TLE5012_PARAMS))) 
// TLE ABS encoder and AIE with TLE5012 get the rotor alignment reading the SPI absolute position from AS504x   
  unsigned int tmp;
  unsigned int timeout = 60000;
  
// TODO: avoid double read.
// check for errors instead.
  tle5012TriggerSample();

  while(!tle5012SamplingCompleted()){
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");

    if(0==timeout--)
  break;
 }

  tle5012TriggerSample();

  while(!tle5012SamplingCompleted()){
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");

    if(0==timeout--)
  break;
 }

//TODO: add watchdog!

  tmp = tle5012Get().angle.Bits.angleVal;
  // tmp is in range 0..4096. Shifting left by 4 returns the greatest 
  // signed positive number if tmp is 4096. 
  Encoder_RotorStartingPosition = ((SFRAC16)(tmp*(65536/AE_RESOLUTION)));
// TODO: pensaci un poco bene ma mi pare che in ogni caso Encoder_SyncPulsePosition va messo =0
  Encoder_SyncPulsePosition = 0;

  // init the variables used for position sensing 
  Previous_position = Encoder_RotorStartingPosition;
  Current_position = Encoder_RotorStartingPosition;
  Previous_position_for_velocity_calc = Encoder_RotorStartingPosition;

  // Rotor alignment completed
  SysStatus.InitialRotorAlignmentComplete=1;
  SysStatus.InitialRotorAlignmentInProgress=0;
#endif

#ifdef ENCODER_QE
#ifdef ENCODER_USE_ZERO_FOR_ROTOR_ALIGNMENT
  // if we have to use index signal to perform rotor alignment then
  // we need to temporarly turn the QEP hw configuration to use index
  // signal to reset counter
  QEPSetUseZero(1);
#endif
// Rotor alignment is completed in FOC irq
  Encoder_RotorStartingPosition = 0;
//Encoder_SyncPulsePosition = 0;
  // Encoder_SyncPulsePosition = 0;//Q15(0.0);

  // in Openloop the angle is not read from the encoder but forcedly increased
  // also during initial rotor alignment the movment are performed in open loop 
  //if ( ( 1 == SysStatus.OpenLoop ) || (1 == SysStatus.InitialRotorAlignmentInProgress) )
  {  
    // init starting value
    ParkParm.qAngle = 0x01; // Q15(0.999999);
    // init torque reference
    ParkParm.qVd = OL_VD;
    ParkParm.qVq = OL_VQ;
  }
#endif

#ifdef ENCODER_DHES
//TODO: completare 
  Encoder_RotorStartingPosition = 0;
  // TODO: inizializzaare per HES perche' serve per il calcolo della posizione!!

  // in order to simplify the reading from the port when connected to P2,P5
  // the phase in the two cases results different 
#ifdef HES_CONNECTED_TO_P1_P6
  Encoder_SyncPulsePosition = 0xB000; // for kollmorgen
#endif
#ifdef HES_CONNECTED_TO_P2_P5
  Encoder_SyncPulsePosition = 0x0;
#endif

  // Rotor alignment completed
  SysStatus.InitialRotorAlignmentComplete=1;
  SysStatus.InitialRotorAlignmentInProgress=0;

  // init the variables used for position sensing 
  Previous_position = Encoder_RotorStartingPosition;
  Current_position = Encoder_RotorStartingPosition;
  Previous_position_for_velocity_calc = Encoder_RotorStartingPosition;
#endif
}

int EncoderVelocity()
// returns encoder HW calculated speed (supported only for tl5012)
{
  static int Encoder_LastVelocity=0;

  tTLE5012_data tledata;
  tledata = tle5012Get();

  if(0 == tledata.crcerror)
  {
    Encoder_LastVelocity = tledata.speed.Bits.angleSpeed;
  }

  return Encoder_LastVelocity;
}

void RampUPCurrent()
// increase gradually the current generated
// overriding the Vq and Vd variables from 0 to IRA_VD, IRA_VQ
{
  static int rampUstatus = IRA_VQ;

  // set ramp initial value
  if( False == SysStatus.CurrentRampUPInProgress )
  {
    IraqVd = 0;
    IraqVq = 0;
  }

  // set the ramping flag
  SysStatus.CurrentRampUPInProgress = True;

  if (0 == rampUstatus)
  {
    // Ramp finished 
    SysStatus.CurrentRampUPInProgress = 0;
    SysStatus.CurrentRampUPComplete = 1;
  }
  else
  {
    // update ramp
    rampUstatus--;
    IraqVq++; 
  }
}

void RampDOWNCurrent()
// decrease gradually the current generated
// overriding the Vq and Vd variables from IRA_VD, IRA_VQ to 0
{
  static int rampDstatus = IRA_VQ;

  // set ramp initial value
  if( False == SysStatus.CurrentRampDOWNInProgress )
  {
    IraqVd = 0;
    IraqVq = IRA_VQ;
  }

  // set the ramping flag
  SysStatus.CurrentRampDOWNInProgress = True;

  if (0 == rampDstatus)
  {
    // Ramp down finished 
    SysStatus.CurrentRampDOWNInProgress = 0;
    SysStatus.CurrentRampDOWNComplete = 1;
  }
  else
  {
    // update ramp
    rampDstatus--;
    IraqVq = rampDstatus; 
  }
}

void AlignRotor(void)
// Rotor alignment is done spinning the field for a complete electrical turn and dragging
// the rotor in position.
// To avoid spiking during turn on and off the current is ramped on and off
//  
// TODO: verificare con l'encoder l'aggancio del rotore 
//    catturare lo zero encoder per aumentare la precisione della fasatura
{
  static long int align_position = 1;
#ifdef ENCODER_USE_ZERO_FOR_ROTOR_ALIGNMENT
  static int zero_found = 0;
#endif

  // SysStatus.CurrentRampUPComplete is 0 at startup;
  // Ramp-up current
  if(0 == SysStatus.CurrentRampUPComplete)
  {
    RampUPCurrent();
  }
  else
  {
    // current ramp up completed start dragging the rotor
#ifdef ENCODER_USE_ZERO_FOR_ROTOR_ALIGNMENT
    // if the rotor completed a mechanic turn plus some extra pulses
    // to be really sure it crossed the zero
  if( zero_found || ((0x10000 * NPOLEPAIRS + 0x100) == align_position)){
    // adjust Encoder_SyncPulsePosition in order to obtain position 0 from EncoderPosition()
    Encoder_SyncPulsePosition = QE_ZERO_SYNCPULSEPOSITION;
#else

    // if the rotor did a complete electric turn
    if( 0x10000 == align_position ){
    // TODO: generalizzare il numero in funzione del numero di poli del motore
     // GENERIC 90 deegree for homing without zero signal
      // adjust Encoder_SyncPulsePosition in order to obtain position 0 from EncoderPosition()
        Encoder_SyncPulsePosition = 0x0F00; 
#endif
      // a complete turn has completed (hopefully)
      
      // init Rotor Starting Position in order to zero the velocity calculation related vars
      Encoder_RotorStartingPosition = Encoder_SyncPulsePosition;
      VelocityParm.Velocity = 0;
      VelocityInit();
// TODO: azzerare lo stato del regolatore di velocita' 

      // start ramping down the current

      // SysStatus.CurrentRampDOWNComplete is 0 at startup;
      if(0 == SysStatus.CurrentRampDOWNComplete)
      {
        // ramp down the current
        RampDOWNCurrent();
      }
      else
      {  
        // current ramped down successfully
#ifndef ENCODER_USE_ZERO_FOR_ROTOR_ALIGNMENT
        // Reset QEI peripheral position counter.
        POSCNT = 0;
#else
    // do not trust the index signal for foc loop: use
        // maxcount to recirculate QEP to zero
    QEPSetUseZero(0);
#endif
        //zero the revolutions counter 
        Turns = 0;

        // Null current PID status 
        //PIParmD.qdSum = 0;
        //PIParmQ.qdSum = 0;
    ControllerCurrentDPIDClear();
    ControllerCurrentQPIDClear();

        // init the variables used for position sensing 
        Previous_position = Encoder_RotorStartingPosition;
        Current_position = Encoder_RotorStartingPosition;
        Previous_position_for_velocity_calc = Encoder_RotorStartingPosition;

        // Rotor alignment completed 
        SysStatus.InitialRotorAlignmentComplete=1;
        SysStatus.InitialRotorAlignmentInProgress = 0;
#ifdef ENCODER_USE_ZERO_FOR_ROTOR_ALIGNMENT
    if(0 == zero_found){
      SysError.EncoderFault = 1;
      FaultConditionsHandler();
    }
#endif
    
      }
    }
    else
    {
      // complete rotation by increasing angle
      ParkParm.qAngle += 0x01;
    align_position++;

#ifdef ENCODER_USE_ZERO_FOR_ROTOR_ALIGNMENT
       if(QEPZeroCrossed()){
      zero_found = 1;
     }
#endif

    }
  }
}
