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
#include "DHES.h"

// used for QE value buffering to keep in sync with SPI
// static unsigned int encoder_buffer;

// used to graphicate with gulp
extern unsigned int gulp_eb;
extern unsigned int gulp_encoder_val;


void EncoderWaitUpdate()
// polling wait for encoder data available.
// this is empty for QEP (data always available from
// MCU QEP periph. It waits SPI for ABS encoders.
{
  unsigned int timeout = 60000;

#if defined(ENCODER_ABS) || defined(ENCODER_AUX_ABS)

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
  
#if defined(ENCODER_TLE) || defined(ENCODER_AUX_TLE)
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



  if(timeout == 0)
  {
    SysError.FirmwareSPITimingError = 1;
  }
}

void DHESTriggerSample(void)
// trigger for the next DHES angle read
{
  // no operation is needed
#warning maybe_buffering_is_needed_for_dhes_and_qe
}

void QETriggerSample(void)
// trigger for the next DHES angle read
{
  // no operation is needed
#warning maybe_buffering_is_needed_for_dhes_and_qe
}

void EncoderTriggerSample()
// This should be called ASAP in the low jitter ISR.
// It causes position to be freezed. Later access with
// Encoder_position & co will return freezed position.
// This logic is needed for SPI sensor, and it is keept
// for QEP to make APIs unchanged.
{

#if defined (ENCODER_QE) || defined (ENCODER_AUX_QE)
  QETriggerSample();
#endif

#if defined (ENCODER_DHES) || defined (ENCODER_AUX_DHES)
  DHESTriggerSample();
#endif

#if defined (ENCODER_ABS) || defined (ENCODER_AUX_ABS)
  as5045TriggerSample();
#endif

#if defined (ENCODER_TLE) || defined (ENCODER_AUX_TLE)
  tle5012TriggerSample();
#endif
}

inline SFRAC16 EncoderPositionQE() __attribute__((always_inline));
inline SFRAC16 EncoderPositionQE()
//
// Read raw mechanical angle from QE encoder and return it
//
{
  /*
  unsigned long tmp;
 
  

  // copy current encoder value to a temp 32 bit storage
  tmp = QEPGet();

  // mult by 16-bit full range (about +1 in SFRAC16)
  //tmp *= 65536;
  tmp <<= 16;

  // divide by encoder PPR*4 -1. 
  tmp /= QE_RESOLUTION; // - 1;
  
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
  return (SFRAC16)(tmp);
  */

  return (SFRAC16)QEPGet();
}



inline SFRAC16 EncoderPositionABS() __attribute__((always_inline));
inline SFRAC16 EncoderPositionABS() 
//
// Read raw mechanical angle from encoder ABSOLUTE ASXXXX encoder and return it
//
{
  static SFRAC16 abs_angle = 0;
  unsigned long tmp;

  if(!as5045SamplingCompleted()){
  /* FOC ALGO HAS CALLED TOO FAST THIS.
     * Please read comment in as5045.c
     * CompleteStruct();
     */

    SysError.FirmwareSPITimingError = 1;
  }else{

#ifdef AE_REVERSE
    tmp = -as5045Get().Angle;
#else
    tmp = as5045Get().Angle;
#endif
    // tmp is in range 0..4096. Shifting left by 4 returns the greatest 
    // signed positive number if tmp is 4096. 
    abs_angle = ((SFRAC16)(tmp * (65536/AE_RESOLUTION)));

  }

  return abs_angle;

}

//
// Read raw mechanical angle from encoder ABSOLUTE TLE5012 encoder and return it
//
inline SFRAC16 EncoderPositionTLE() __attribute__((always_inline));
inline SFRAC16 EncoderPositionTLE()
{
  tTLE5012_data tledata;
  int tmp=0;
  static SFRAC16 tle_angle = 0;

  if(!tle5012SamplingCompleted())
  {
    // FOC ALGO HAS CALLED TOO FAST THIS.
    // Please read comment in as5045.c CompleteStruct();
    SysError.FirmwareSPITimingError = 1;
  }else{

  // get all encoder data  
    tledata = tle5012Get();

    if(0 == tledata.crcerror)
    // if CRC error keeps the last good value
    {
      // extract the angle
#ifdef ABS_ENCODER_REVERSE
      tmp = -tledata.angle.Bits.angleVal;
#else
      tmp = tledata.angle.Bits.angleVal;
#endif
      // normalize encoder value (0..AE_RESOLUTION) to (0..65535)
      tmp = ((SFRAC16)((double)tmp * ((double)65536/(double)AE_RESOLUTION)));
      // here there is the meccanical angle 
      tle_angle = tmp;
    }
  }
  return tle_angle;
}


inline SFRAC16 EncoderPositionDHS() __attribute__((always_inline));
inline SFRAC16 EncoderPositionDHS()
//
// Read raw mechanical angle from HES encoder
//
{
  // HES encoder is emulated from hes signals. The hes emulation
  // already does thing well, so here we have nothing to do.. 

  //SFRAC16 dhes_val;

  //dhes_val = DHESRead();

  //return dhes_val;

  return DHESRead();
}



SFRAC16 EncoderPosition(int aux)
// This function provides an encoder-type independent way to update the 
// read position. 
// If more than one source is available 
// the parameter aux can be use to choose wich source read.
{
 
SFRAC16 position = 0;
if(aux){
  #if defined(ENCODER_AUX_QE)
    position = EncoderPositionQE();
  #elif defined(ENCODER_AUX_ABS)
    position = EncoderPositionABS();
  #elif defined (ENCODER_AUX_TLE)
    position = EncoderPositionTLE();
  #elif defined (ENCODER_AUX_DHES)
    position = EncoderPositionDHS();
  #else
  // no aux encoder :(
  #endif
}else{
  #if defined(ENCODER_QE)
    position = EncoderPositionQE();
  #elif defined(ENCODER_ABS)
    position = EncoderPositionABS();
  #elif defined (ENCODER_TLE)
    position = EncoderPositionTLE();
  #elif defined (ENCODER_DHES)
    position = EncoderPositionDHS();
  #else
  #error EncoderPositionCannotHandleThisEncoder
  #endif


}


  return position;
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

void EncoderInit()
// setup the appropriate Registers for SPI or QEP encoder
{
#if defined (ENCODER_QE) || defined (ENCODER_AUX_QE)
  QEPInit();
#endif

#if defined (ENCODER_ABS) || defined (ENCODER_AUX_ABS)
  as5045Init();
#endif

#if defined (ENCODER_DHES) || defined (ENCODER_AUX_DHES)
  DHESInit();
#endif

#if defined (ENCODER_TLE) || defined (ENCODER_AUX_TLE)
  tle5012Init();
#endif

  EncoderTriggerSample();
  EncoderWaitUpdate();

}
