//
// AS5045 SPI magnetic absolute encoder related routines
//   for DMA SPI reading of AS5045
//
// TODO: Descrivere in modo acconcio il modo di funzionamento

/* NOTE:
 * the as5945 internally runs on RC osc, and it's DSP will produces
 * samples at about 10khz (fast mode).
 * Despite the encoder's resolution, that is now unrelated, getting
 * from the as5945 one sample or the next one (or the previous one)
 * will result in a relatively big variation of the returned value.
 * This variation is directly proportional to the rotor speed.
 * On a quadrature encoder this does not happen, as sampling before
 * or after a step will result in a variation by one unit.
 * For this reason when reading the as5045 sometimes you may
 * find samples that are nearly twice or half of the value expected 
 * or samples with the same value.
 * This is because the internal as5045 sample freq and the
 * external microcontroller sample freq (that triggers spi reads)
 * are not in sync.
 * Assume the motor is running @ vel=K and suppose the as5045 runs 
 * @ 10Khz and the micro will sample at 9.5 Khz. 
 * You will get sometimes a sample that is double than expected.
 * In fact sometimes you will miss a sample of as5045. If you refer
 * to your local microcontroller timer, you will see some samples, 
 * representing positions, that are equally different (Si - Sn = Sm - Si 
 * for each n where i = n+1 and m = i+1); then you may get a sample 
 * that is twice the value expected, because you lost one sample 
 * (remember that regardless the encoder resolution, the difference
 * between two adjacent samples may be considerable if the rotor is 
 * running fast.
 * The problem here, is that you are timestamping with the dspic
 * timestamp but samples that are taken from another discrete sample
 * set, that has been sampled with another different timer 
 * (as5045 rc osc).
 * On the other hand if you sample @ 10.5Khz (higher rate than the 
 * as5045) you will get some repeated samples, that will lead to 
 * some zero-velocity misreading.
 * However removing those repeated samples seems by far simplier than
 * correcting the "position jump" explained above.
 * So we must make sure we run spi sampling (not spi, clock; Think about
 * SPI complete 2words read timing) greater than as5045 DSP sampling rate. 
*/
/*
Since as5045 outputs up to 18bits on SPI, we make two consecutive 16bits readings.
In order to do that without restarting the read operation, 
the SIP chip select pin is used as a GPIO, and it is manually set and reset 
before and after the two reads sequence.
Note: it is not possible to perform a 16 bit and a 8 bit reading because it 
imply resetting the whole SPI module.
*/

// TODO: enforce delay to ensure slave select is deasserted for 
// enough time in order to avoid read failure if as5045_query 
// is called right after ISR has run.

// TODO: fare tutto l'error checking possibile checksum, magnetic, linearity, e CORDIC
//   esplicitando anche eventuali errori SPI, deve essere possibile capire quando viene 
//   tagliato il cavo !!!

// TODO: vedere se si riesce a creare una politica di QOS tipo quella dell error_passive sul CAN

#include "as5045.h"
#include "chksum_lut.h"
#include "faults.h"
#include <string.h>


extern tSysError SysError;
// This "pointer" always points to the buffer from which to read
#define AS5045_RBUF (as5045_data[as5045_state_machine.buffer_flip & 0x1])

// This "pointer" always pointes to the buffer to write to
#define AS5045_WBUF (as5045_data[!(as5045_state_machine.buffer_flip & 0x1)])

// this makes internal read and write buffers to swap. In other words It makes 
// AS5045_RBUF and  AS5045_WBUF to swap.
#define AS5045_FLIP as5045_state_machine.buffer_flip ^= 1;//!as5045_state_machine.buffer_flip;

#define AS5045_INIT_FLIP as5045_state_machine.buffer_flip = 1;

int as5045SamplingCompleted(void)
// tells if after the last invokation of as5045_TriggerSample
// the value has been updated. The caller must ensure this will
// not run across as5045TriggerSample
{
  if(0 ==  as5045_state_machine.nPending)
   return 1;
  else
   return 0;
}

void as5045TriggerSample(void)
// Trigger a new read process from SPI (SS freeze the position of the encoder)
{
  // encoder reading already in progress 
  if( as5045_state_machine.nPending) 
  {
    // TODO: se pending signifuiica che qualcosa di molto malato sta succedendo, agire in modo acconcio.
    return;
  }
   as5045_state_machine.nPending = 1;
  // TODO: to be paranoid .. clear rxoverflow flag 
  SPI1STATbits.SPIROV = 0;

  // issue a new SPI request
  SPI_SS = 0;
  SPI1BUF = 0x0000; 
}

void as5045Init(void)
// this is not the function call for initializing generic encoders!
// this is called only in encoder.c in order to initialize as5045-related sw/hw
{
  AS5045_INIT_FLIP;
  // pending bytes from SPI encoder 
  as5045_state_machine.nPending = 0;
 
  // TODO: Commentare in modo acconcio
  OpenSPI1( ENABLE_SCK_PIN /*& ENABLE_SDO_PIN */& SPI_SMP_ON & SPI_CKE_OFF & 
    SPI_MODE16_ON& CLK_POL_ACTIVE_LOW & MASTER_ENABLE_ON & 
    // 40Mhz /3/16 = about 833khz.
    SEC_PRESCAL_3_1 & PRI_PRESCAL_16_1,
    FRAME_ENABLE_OFF, // & FRAME_SYNC_OUTPUT & FRAME_POL_ACTIVE_LOW & FRAME_SYNC_EDGE_COINCIDE
    SPI_ENABLE & SPI_IDLE_CON & SPI_RX_OVFLOW_CLR);
 
#ifdef SPI_DMA
  // Clears interrupt flag
  IFS2bits.DMA3IF =0;
  // Enable interrupt
  // TODO: interrupt DMA abilitato prima del resto dei settings? 
  IEC2bits.DMA3IE = 1;
  // DMA in continuous ping-pong mode
  DMA3CON = 0x0002;

  // set DMA destination buffers 
  DMA3STA = __builtin_dmaoffset(&as5045_hw_data);
  DMA3STB = DMA3STA + 2;
  // set DMA source address
  DMA3PAD = (volatile unsigned int) &SPI1BUF;
  // transfer one word at a time 
  DMA3CNT = 0;
  // Binds DMA3 CH to SPI module
  DMA3REQ = 0x000A;
  // Enables DMA CH3
  DMA3CONbits.CHEN=1;
  EnableIntDMA3;
#else
  // TODO: forse è meglio esplicitare nel main
  EnableIntSPI1;
#endif
}
extern unsigned int debug_spi_errors;
void CompleteStruct()
// computes checksum and create a copy for proper ping pong operation
{
  int chksum;

  //TurnOnLedGreen();	
  chksum = 0;
  /*for(i=0;i<16;i++)
  {
    if(as5045_hw_data.w[0] & (1<<i))
    chksum++;
  }*/

  chksum = chksum_lut[as5045_hw_data.b[0]];
  chksum += chksum_lut[as5045_hw_data.b[1]];
  chksum += chksum_lut[as5045_hw_data.b[3] & 0xc0];
  /*if(as5045_hw_data.Flags.MagFieldInc)
  { 
    chksum++;
  }
  if(as5045_hw_data.Flags.Parity)
  {
    chksum++;
  }	*/
  if(0 == (chksum %2))
  {
  	AS5045_WBUF.CheckSumError = 0;
    AS5045_WBUF.Angle = as5045_hw_data.Flags.Angle;
    /// TODO as5045_data[AS5045_WBUF].AccumlatedErrors:8;  // +10 each error -1 each successful communication

    AS5045_WBUF.MagneticFieldError =  
		as5045_hw_data.Flags.LinearityError | as5045_hw_data.Flags.CORDICOveerflow; // CORDIC overflow error detected
  
    if( AS5045_WBUF.MagneticFieldError){
	  SysError.AS5045CalcError = 1;
	
    }else{
      // TODO as5045_data[AS5045_WBUF].CommunicationError; 
	
	  /* WARNING WARNING WARNING the following operation must be atomic, in order to
       * make sure that as5045_get() will never be called across this operation. 
	   * Currently as CompleteStruct() runs on SPI or DMA ISR, and as5045_get is only
       * called in main loop, it is safe.
       * If as5045_get() has to be called in ISR context one must make sure that the
       * calling ISR can't preempt the SPI or DMA ISR, or implement a locking mechanism
       *
       * Currently both Triggering and Getting are done in highest int priority.
       * So this function CAN be preempted by the get operation.
       * We trust that timing is deterministic enough to avoid races.
       * Currently (40 Khz loop)get is called two times. One in the middle of the 
       * SPI transfer, so that this functions has not been called yet and
       * One just after this operation completed (hopefully).
       */

 	   // memcpy( &as5045_struct, &as5045_tmp_struct, sizeof(tAS5045));

	   AS5045_FLIP;	 
	}
  }
  else
  {
// TODO: sometimes first packet after POR is damaged. check why.
    AS5045_WBUF.CheckSumError = 1;
	SysError.AS5045CSumError = 1;

debug_spi_errors++;
  }




   as5045_state_machine.nPending = 0;
///TurnOffLed();
}

tAS5045_data as5045Get()
// public API to be called to get last read position
{
  // MODIFIED BY AM TO CHECK PARITY AND UPDATE CHECKSUM ERROR FLAG
  return AS5045_RBUF;
}

void __attribute__((interrupt, no_auto_psv)) _SPI1Interrupt(void)
//
// SPI IRQ Service Routines
// 
{   

#ifndef SPI_DMA
  unsigned int word;
#endif
  unsigned int isr;

  // Clear interrupt flag
  isr = SPI1STAT;

  IFS0bits.SPI1IF = 0;

  if(!(isr &1) )
  { 
    return;
  }

#ifndef SPI_DMA
  word = SPI1BUF;

  if(1 ==  as5045_state_machine.nPending)
  {
    as5045_hw_data.w[0] = word;
    as5045_state_machine.nPending = 2;
    // FIRE SPI AGAIN
    SPI1BUF = 0x0000;
  }
  else 
  {  
    if(2 ==  as5045_state_machine.nPending)
    {
      SPI_SS = 1;
      // Only 2 bits (MSB!!) are meaningful
      as5045_hw_data.w[1] = word;
      CompleteStruct();	 
    }
  }
#endif
}  

#ifdef SPI_DMA
  #if defined(ENCODER_ABS) || ( defined(ENCODER_AIE) && !(defined(AIE_TLE5012_PARAMS)))

void __attribute__((__interrupt__, no_auto_psv)) _DMA3Interrupt(void)
// SPI DMA Handler 
{
  IFS2bits.DMA3IF = 0; // Clear the DMA1 Interrupt Flag
  if(1 == as5045_state_machine.nPending )
  // read spi buffer register, extract position and save to position buffer variable 
  {
     as5045_state_machine.nPending = 2;
    // FIRE SPI AGAIN
    SPI1BUF = 0x0000;
  }
  else
  {
    SPI_SS = 1;
    CompleteStruct();
  }
}
  #endif
#endif
