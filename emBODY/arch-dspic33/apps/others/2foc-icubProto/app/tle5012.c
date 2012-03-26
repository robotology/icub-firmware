#include "tle5012.h"

#include "spi.h"
#include "dma.h"
#include "UserTypes.h"
#include "UserParms.h"


// the max value for the CRC illness counter.
#define TLE5012_CRC_ILLNESS_MAX 10000
// const that will be subtracted from the CRC illness counter every time
// the SPI transfer is error-free
#define TLE5012_CRC_ILLNESS_PRIZE 1
// const that will be summed to the CRC  illness counter every time the
// SPI transfer results damaged
#define TLE5012_CRC_ILLNESS_PENALITY 30


// the SPI hw writes takes 16 clock cycles to complete. 
// currently initializations are done @ 500Khz. so every clock cycle
// takes 2us. The whole SPI access takes 32us PLUS the DMA time.
// wait 100us here. The wait routine will take about 250ns for
// every TLE_SPI_TIMEOUT unit
#define TLE_SPI_TIMEOUT 400
 

// Constant definition for command used to start SPI reading
// of 3 consecutive registers, starting from TLE5012_AVAL.
// HW will send also a 4th data word (safety word)
static tTLE5012_command tle5012_cmd_start={
		.Bits.len = 3, // reads 2 word
		.Bits.addr = TLE5012_AVAL, // start address from the 1st interesting reg		
        .Bits.upd = 0, // read updated values
		.Bits.lock = 0, // must be zero for reg with addr < 5
		.Bits.rw = 1, // read operation
};

// This "pointer" always pointes to the buffer to read from
#define TLE5012_RBUF (tle5012_data[tle5012_state_machine.buffer_flip & 0x1])
// This "pointer" always pointes to the buffer to write to
#define TLE5012_WBUF (tle5012_data[!(tle5012_state_machine.buffer_flip & 0x1)])
// this makes internal read and write buffers to swap. In other words It makes 
// AS5045_RBUF and  AS5045_WBUF to swap.
#define TLE5012_FLIP {tle5012_state_machine.buffer_flip ^= 1;/*TLE5012_WBUF.angle.w=0xdead;*/}//!as5045_state_machine.buffer_flip;


#define TLE5012_INIT_FLIP tle5012_state_machine.buffer_flip = 1;




void tle5012_crc_init()
// Initialize the HW CRC module on the DSPIC
// and the SW CRC-related status variables.
{
    int timeout;

    // make sure the CRC fifo is empty:
    // enable CRC module
	CRCCONbits.CRCGO = 1;
    // waits for the fifo to be empty
    timeout = 0;
	while(!CRCCONbits.CRCMPT){
	  asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");

      timeout++;

      // wait for about 200ns * 100 = 200 us. this should be
      // *very* conservative
      if(1000 == timeout ){
        //TODO: report error
        break;
      }
	}
    // disable again CRC module. This is needed
    // in order to write the poly
	CRCCONbits.CRCGO = 0;

	// 8 bit CRC poly
	CRCCONbits.PLEN = 7;
	// J1850 CRC poly
	CRCXOR = 0x1d;
	
    // no errors yet..
	tle5012_crc_error_count = 0;
    tle5012_error_count = 0;
}


__inline__ static void tle5012_crc_push_byte(unsigned char data)
// Insert a byte in the HW CRC fifo
{
	if(CRCCONbits.CRCFUL){
		// TODO: ERROR, delete these nops and handle the disaster!
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
	}

	*(unsigned char*)&CRCDAT = data;

}

__inline__ static void tle5012_crc_push_word(unsigned int data)
// Insert a word by pushing its two bytes in the HW CRC fifo
{
  tle5012_crc_push_byte(data >> 8);
  tle5012_crc_push_byte(data & 0xff);
}


unsigned char tle5012_crc_getdata()
// Gets the CRC calculated by the DSP HW CRC module
{
    // push in the FIFO a dummy word in order to shift out the result
	tle5012_crc_push_byte(0x00);

    // waits for the FIFO to be empty
	while(!CRCCONbits.CRCMPT){

	}
	
    // explicitly stop the CRC module
    CRCCONbits.CRCGO = 0;

    // some nops have been seen in the Microchip Library before accessing the CRC result..
	asm("nop");
	asm("nop");
	asm("nop");

    // returns the CRC result
	return CRCWDAT; 
}

//static int a;

void tle5012_crc_new()
// initialize the CRC module in order to 
// calculate a new CRC
{
    // Set the initial reminder. WARNING. THIS IS MATH BLACK MAGIC:
    // the Infineon datasheet specify that the initial reminder should
    // be 0xff, however, for unknown reasons, possibly due to some 
    // direct to non-direct mode conversion, the initial value to write 
    // here is 0x7e
	CRCWDAT = 0x7e;

    // enable the CRC module. This makes the CRC module to begin to 
    // computate the CRC when the fifo starts to be filled.
	CRCCONbits.CRCGO = 1;
/*	*(unsigned char*)&CRCDAT = 0x53;

    a = tle5012_crc_getdata();	

    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");*/
}


int tle5012SamplingCompleted(void)
// tells if after the last invokation of tle5012_TriggerSample
// the value has been updated. The caller must ensure this will
// not run across tle5012TriggerSample
{
  if(0 ==  tle5012_state_machine.nPending)
   return 1;
  else
   return 0;
}

void tle5012TriggerSample(void)
// Triggers a new read process from SPI (SS freeze the position of the encoder)
{
  if (tle5012_state_machine.accessMutex)
    return;
  // encoder reading already in progress 
  if( tle5012_state_machine.nPending) 
  {
    // TODO: se pending significa che qualcosa di molto malato sta succedendo, agire in modo acconcio.

    return;
  }

  // asserts SPI CS
  SPI_SS = 0;

  // after asserting CS, wait a bit before transmitting data
  asm("nop");
  asm("nop");
  asm("nop");
  
  tle5012_state_machine.nPending = 1;
  
  // to be paranoid .. clear rxoverflow flag 
  SPI1STATbits.SPIROV = 0;

  // issue a new SPI request
  SPI1BUF = tle5012_cmd_start.w;
  
  // initialize the DSP HW CRC module
  tle5012_crc_new();
  
  // since the TLE5012 calculates the CRC over all data running on
  // the wire (exept safety word), included the command word sent by the
  // master, lets start to push in the DSP HW CRC FIFO the TX command.
  tle5012_crc_push_word(tle5012_cmd_start.w);

}

unsigned int tle5012ReadReg(char adr)
// Perform a single, synchronus, safe
// read on a TLE5012 register.
// The caller MUST ensure that no
// SPI operation is in progress when calling	
{
	int i;
	int timeout;

	tTLE5012_command cmd;

	// when this functions has been called
    // we can assume that the SPI operation is not in progress
    // but we can't make any assumption about how much time
    // is elapsed until last SPI operation.
    // the encoder needs 600ns recover time
	for(i=0;i<6;i++){
		asm("nop");
		asm("nop");
		asm("nop");
		//asm("nop");
	}
	// enable CS
 	SPI_SS = 0;

	// read one single word without safety word
	cmd.Bits.len = 1;
	// read updated values
	cmd.Bits.upd = 0;
    // set starting address
	cmd.Bits.addr = adr;
	
	// this is a magic cookie to lock access to sys regs.
	if(adr > 4){
		cmd.Bits.lock = TLE5012_MAGIC_SYSREG;
	}else{
		cmd.Bits.lock = 0;
	}

	// read operation
	cmd.Bits.rw = 1;
	 

    // The data are polled here, we don't want the ISR to
    // be invoked and trying processing it.  
	DisableIntDMA3;

	// at least 105 ns should be elapsed since
	// CS has been activated ( 5 asm instructions).
	// Do not make any precise assumpion about how much time 
    // the code takes to execute.
    // Delay 100 ns here (hw requires 105ns but the C
    // instructions above should take at least one 
    // asm instruction )
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");


	// FIRE the command over SPI
	SPI1BUF = cmd.w; 


    timeout = 0;

    // wait for the transfer to be completed
	while(!IFS2bits.DMA3IF){

		timeout++;

		if(timeout == TLE_SPI_TIMEOUT){
          // TODO set acconcing flag
		  break;
		}

		// 10 nops should cause about 250ns stall
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
	}

	// the ISR was not invoked but the ISR flag is set.
    // Reset here: when  the ISR will be enabled again we wants
    // to avoid that it will be run.
	IFS2bits.DMA3IF = 0;

	
	// Do not make any assumpion about how many time
    // the loop iterates, how much time does take
    // the INT flag to be asserted and how much time 
    // does the SPI take in order to start transfer.
    // Delay 125 ns here (hw requires 130 but the C
    // C instruction above should take at least one 
    // asm instruction).
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	
	// fire SPI transfer again
	SPI1BUF = 0x0000; 

    timeout = 0;

    // wait for the transfer to be completed
	while(!IFS2bits.DMA3IF){

		timeout++;

		if(timeout == TLE_SPI_TIMEOUT){
          // TODO set acconcing flag
		  break;
		}

		// 10 nops should cause about 250ns stall
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
	}

	// fire SPI . allows safety word
	SPI1BUF = 0x0000; 

    timeout = 0;

    // wait for the transfer to be completed
	while(!IFS2bits.DMA3IF){

		timeout++;

		if(timeout == TLE_SPI_TIMEOUT){
          // TODO set acconcing flag
		  break;
		}

		// 10 nops should cause about 250ns stall
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
	}

	// at least 105 ns should be elapsed since transfer 
	// finished before deactivate CS ( 5 asm instructions).
	// Do not make any 
    // precsise assumpion about how much time 
    // does the code to execute.
    // Delay 100 ns here (hw requires 105 but the C
    // instruction above should take at least one 
    // asm instruction )
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	// disable cs
 	SPI_SS = 1;

	// when this functions returs it must be perfectly
    // safe to assume that the SPI operation completed
    // and the SPI and encoder are ready to perform
    // another operation.
	// 600 ns are required before we can
    // return.
	for(i=0;i<6;i++){
		asm("nop");
		asm("nop");
		asm("nop");
		//asm("nop");
	}
	
    // clear ISR flag
	IFS2bits.DMA3IF = 0;

	// enable DMA interrupt for SPI transfer DONE
 	EnableIntDMA3;	

	return tle5012_hw_buf;
}

void tle5012WriteReg(char adr, unsigned int val)
	// Performs a single, synchronus, safe
    // write on a TLE5012 register.
    // The caller MUST ensure that no
    // SPI operation is progress when calling
{

	int i;
	int timeout;


	tTLE5012_command cmd;
	cmd.Bits.len = 1;
	cmd.Bits.upd = 0;
	cmd.Bits.addr = adr;
	
	// when this functions has been called
    // we can assume that the SPI operation is not in progress
    // but we can't make any assumption about how much time
    // is elapsed until last SPI operation.
    // the encoder needs 600ns recover time
	for(i=0;i<6;i++){
		asm("nop");
		asm("nop");
		asm("nop");
		//asm("nop");
	}

	// enable CS
	SPI_SS = 0;

	// if it is a system reg, then the magic word must be said
	if(adr > 4){
		cmd.Bits.lock = TLE5012_MAGIC_SYSREG;
	}else{
		cmd.Bits.lock = 0;
	}

	cmd.Bits.rw = 0;

	// do not make the DMA handler able to catch the 
    // SPI value: this is a synchro operation that
    // has nothing to do with the "normal" read mechanism
	// used by the FOC loop	  
	DisableIntDMA3;

	// at least 105 ns should be elapsed since
	// CS has been activated ( 5 asm instructions).
	// Do not make any 
    // precsise assumpion about how much time 
    // does the code to execute.
    // Delay 100 ns here (hw requires 105 but the C
    // instruction above should take at least one 
    // asm instruction )
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	
	// fire SPI. transmit the command
	SPI1BUF = cmd.w; 

    timeout = 0;

    // wait for the transfer to be completed
	while(!IFS2bits.DMA3IF){

		timeout++;

		if(timeout == TLE_SPI_TIMEOUT){
          // TODO set acconcing flag
		  break;
		}

		// 10 nops should cause about 250ns stall
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
	}

	IFS2bits.DMA3IF = 0;
	
	// Do not make any assumpion about how many time
    // the loop iterates and how much time does take
    // the INT flag to be asserted.
    // Delay 125 ns here (hw requires 130 but the C
    // C instruction above should take at least one 
    // asm instruction )
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");

	// fire SPI again, transmit the value to be written
	SPI1BUF = val; 

    timeout = 0;

    // wait for the transfer to be completed
	while(!IFS2bits.DMA3IF){

		timeout++;

		if(timeout == TLE_SPI_TIMEOUT){
          // TODO set acconcing flag
		  break;
		}

		// 10 nops should cause about 250ns stall
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
	}

	IFS2bits.DMA3IF = 0;
	

	// Do not make any assumpion about how many time
    // the loop iterates and how much time does take
    // the INT flag to be asserted.
    // Delay 125 ns here (hw requires 130 but the C
    // C instruction above should take at least one 
    // asm instruction )
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");

	// fire SPI again, allow safety word
	SPI1BUF = 0xffff; 

  
    timeout = 0;

    // wait for the transfer to be completed
	while(!IFS2bits.DMA3IF){

		timeout++;

		if(timeout == TLE_SPI_TIMEOUT){
          // TODO set acconcing flag
		  break;
		}

		// 10 nops should cause about 250ns stall
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
	}
	IFS2bits.DMA3IF = 0;
	
	// at least 105 ns should be elapsed since
	// before deactivate CS ( 5 asm instructions).
	// Do not make any 
    // precsise assumpion about how much time 
    // does the code to execute.
    // Delay 100 ns here (hw requires 105 but the C
    // instruction above should take at least one 
    // asm instruction )
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
 	// disable CS

	SPI_SS = 1;

	// enable again the DMA ISR, so that the trigger/get
    // scheme used in the FOC loop could work again
	EnableIntDMA3;	
		
}

 int test1,test2,test3;

int tlv5012InitRegs()
// Initialize tle5012 registers
{
	int i;

	tTLE5012_stat stat;
	tTLE5012_mod1 mod1;
	tTLE5012_mod2 mod2;
	tTLE5012_mod3 mod3;
    tTLE5012_sil sil;


  // perform an initial full write
  // TODO: this should be removed. All registers
  // should be initialized with meaningful values
  // as done below for mod1, mod2 etc..
  for(i=0;i<0x11;i++)
  {
    tle5012WriteReg(i, 0);
  }
	

  //
  // Initializaztion for MODE1 register
  //
  // Set Infineon Data out as push pull 
  mod1.w = 0;
  // Set filter decimation
  mod1.Bits.FIRDecimation = TLE5012_FILTER_DECIMATION_170us;
  // Set IIF pins mode as disabled

  // if the TLE is used as AIE than enables the IIF inteface
#ifdef AIE_TLE5012_PARAMS
  mod1.Bits.IIFMode = TLE5012_IIF_MODE_INDEXONDATA;
#else
  mod1.Bits.IIFMode = TLE5012_IIF_MODE_DISABLED ;
#endif
  // write data on register
  tle5012WriteReg(TLE5012_MOD1, mod1.w);

  //
  // Initializaztion for MODE2 register
  //
  mod2.w = 0;
  // set angle range to 360 degrees
  mod2.Bits.angleRange = 0x80;
  // Enable prediction for velocity calculation (3pts)
  mod2.Bits.prediction = TLE5012_PREDICTION_ENABLE;

  // set autocalibration mode 
  // TODO: this is now a random value
  mod2.Bits.autocalMode = TLE5012_AUTOCAL_MODE_TUPD;

  // set the encoder direction to clockwise
  mod2.Bits.angleDirection = TLE5012_ANGLE_DIRECTION_CW;

  // write the prepared data to mod2 register
  tle5012WriteReg(TLE5012_MOD2, mod2.w);


  sil.w = 0;
  // Enabling filter has negative effects TODO: understand why
  sil.Bits.filterParallel = 0;
  sil.Bits.filterInverted = 0;
  
  tle5012WriteReg(TLE5012_SIL, sil.w);


  mod3.w = 0;
  // this has been seen to improve a lot performances of SPI communications.
  // this let us to increase SPI clock drammatically.
  mod3.Bits.analogSpikeFilter = 1;
  mod3.Bits.padDriverMode = 0;

  // writes the prepared data to mod3 register.
  tle5012WriteReg(TLE5012_MOD3, mod3.w);
  
	
  stat.w = tle5012ReadReg(TLE5012_STAT);

// TODO: controllare errori in modo acconcio
	/*if(stat.Bits.ROMCRC)
		return -1;
	if(stat.Bits.fuseCRC)
		return -2;
	if(stat.Bits.DSP)
		return -3;
	if(stat.Bits.voltageRegulator)
		return -4;
*/

	return 0;
}


int tle5012Init(void)
// this is not the function call for initializing generic encoders!
// this is called only in encoder.c in order to initialize as5045-related sw/hw
{

  int ret;

  // deasserts SPI CS
  SPI_SS = 1;

  TLE5012_INIT_FLIP;
  // pending bytes from SPI encoder 
  tle5012_state_machine.nPending = 0;

  // initialize the HW DSP CRC module and the SW CRC-related vars.
  tle5012_crc_init();
 
  
  // Initially configure SPI at low speed (500Khz) in oreder to 
  // initialize safely all registers.
  // In particular during initialization the analog pad filter
  // will be enabled, allowing more reliable communication at
  // higher SPI clock 
  
  OpenSPI1( ENABLE_SCK_PIN & ENABLE_SDO_PIN & SPI_SMP_ON & SPI_CKE_OFF & 
    SPI_MODE16_ON & CLK_POL_ACTIVE_HIGH & MASTER_ENABLE_ON & 
    // 40Mhz /5/16 = about 500Khz.
    SEC_PRESCAL_5_1 & PRI_PRESCAL_16_1,
    FRAME_ENABLE_OFF,
    SPI_ENABLE & SPI_IDLE_CON & SPI_RX_OVFLOW_CLR);

  // Clears interrupt flag
  IFS2bits.DMA3IF =0;
  // Enable interrupt
  // TODO: interrupt DMA abilitato prima del resto dei settings? 
  IEC2bits.DMA3IE = 1;
  // DMA in continuous NOT ping-pong mode
  DMA3CON = 0x0000;

  // set DMA destination buffers 
  DMA3STA = __builtin_dmaoffset(&tle5012_hw_buf);
//  DMA3STB = DMA3STA + 2;
  // set DMA source address
  DMA3PAD = (volatile unsigned int) &SPI1BUF;
  // transfer one word at a time 
  DMA3CNT = 0;
  // Binds DMA3 CH to SPI module
  DMA3REQ = 0x000A;
  // Enables DMA CH3
  DMA3CONbits.CHEN=1;
  // Enables the DMA3 interrupt
  EnableIntDMA3;

  // Initialize the tle5012 HW registers
  ret = tlv5012InitRegs();


  // re-initialize SPI at high speed (3.3Mhz) in order to 
  // perform fast data read transfers
  CloseSPI1();
  OpenSPI1( ENABLE_SCK_PIN & ENABLE_SDO_PIN & SPI_SMP_ON & SPI_CKE_OFF & 
    SPI_MODE16_ON & CLK_POL_ACTIVE_HIGH & MASTER_ENABLE_ON & 
    // 40Mhz /6/1 = about 6.6Mhz. -> errors -> position spikes
    // 40Mhz /3/4 = about 3.3Mhz -> might not finish in time
    // 40Mhz /2/4 = about 5Mhz
    SEC_PRESCAL_2_1 & PRI_PRESCAL_4_1,
    FRAME_ENABLE_OFF,
    SPI_ENABLE & SPI_IDLE_CON & SPI_RX_OVFLOW_CLR);

	return ret;
}


tTLE5012_data tle5012Get()
// public API to be called to get last read position
{
  return TLE5012_RBUF;
}


#if (defined(ENCODER_TLE) || ( defined(ENCODER_AIE) ) && (defined(AIE_TLE5012_PARAMS)) )
void __attribute__((__interrupt__, no_auto_psv)) _DMA3Interrupt(void)
// SPI DMA Handler 
{

  unsigned char dsp_crc, tle_crc;

  IFS2bits.DMA3IF = 0; // Clear the DMA1 Interrupt Flag
  switch(tle5012_state_machine.nPending ){
  
	// command word sent, next round we should get something back
	case 1:
		// FIRE SPI AGAIN

		tle5012_state_machine.nPending = 2;
  	    asm("nop");
        asm("nop");
 	    asm("nop");
	  
    	SPI1BUF = 0x0000;
	break;
	case 2:

		// update state machine. Transition to next state
	 	tle5012_state_machine.nPending = 3;

		// read AVAL from the DMA buffer;
		TLE5012_WBUF.angle.w = tle5012_hw_buf;
        // push the readed value to the DSP CRC HW fifo
		tle5012_crc_push_word(TLE5012_WBUF.angle.w);
    	// FIRE SPI AGAIN
    	SPI1BUF = 0x0000;

  	break;

	case 3:
	
		// update state machine. Transition to next state
     	tle5012_state_machine.nPending = 4;
	
		// read speed reg from the DMA buffer
		TLE5012_WBUF.speed.w = tle5012_hw_buf;
        // push the readed value to the DSP CRC HW fifo
		tle5012_crc_push_word(TLE5012_WBUF.speed.w);
    	// FIRE SPI AGAIN
    	SPI1BUF = 0x0000;
  	break;

	case 4:
        // update state machine. Transition to next state
     	tle5012_state_machine.nPending = 5;

		// read revolution reg
	    TLE5012_WBUF.revolution.w = tle5012_hw_buf;
        // push the readed value to the DSP CRC HW fifo
        tle5012_crc_push_word(TLE5012_WBUF.revolution.w);
    	// FIRE SPI AGAIN
    	SPI1BUF = 0x0000;
  	break;

	case 5:
        // read the safery word from the DMA buffer
		TLE5012_WBUF.safetyword.w = tle5012_hw_buf;
		
        // here we read the safety word. It does contains some flags (upper byte) and
        // the CRC (lower byte). Despite the CRC is done at bytes level, and despite
        // the tle5012 datasheets explicitly says that the CRC is calculated over
        // ALL things that runs on the wire, the flags byte does NOT partecipate in
        // the CRC calculation. In other word ALL the safety word does not partecipate
        // to the CRC calculation.

		// SPI operation is terminated. Deassert CS
		SPI_SS = 1;

        tle_crc = TLE5012_WBUF.safetyword.Bits.CRC;

        // TLE5012 CRC is bit inverted. Do the same here
		dsp_crc = tle5012_crc_getdata()^0xff;
     	
		// verify if the CRC contained in the safety word matches with the CRC calculated
        // over the received data.
		if(tle_crc != dsp_crc){
            // increase raw CRC error counter
			tle5012_crc_error_count++;
			TLE5012_WBUF.crcerror = 1;
            // illness status. Increases every time we got an error (clamping to TLE5012_ILLNESS_MAX)
	        if((tle5012_crc_illness + TLE5012_CRC_ILLNESS_PENALITY) < TLE5012_CRC_ILLNESS_MAX){
				tle5012_crc_illness += TLE5012_CRC_ILLNESS_PENALITY;
            }else{
                tle5012_crc_illness = TLE5012_CRC_ILLNESS_MAX;
            }

		}else{
            TLE5012_WBUF.crcerror = 0;
		    // illness status. Decreases every time we got a good SPI transfer
			if(tle5012_crc_illness > TLE5012_CRC_ILLNESS_PRIZE){
				tle5012_crc_illness -= TLE5012_CRC_ILLNESS_PRIZE;

            }else{
                tle5012_crc_illness = 0;
            }
		}
        if(!TLE5012_WBUF.safetyword.Bits.systemError)
		  tle5012_error_count++;
		// swap output buffers (the new one is now readable by outside)
    	TLE5012_FLIP;	 
        // update state machine. reset to initial state
   		tle5012_state_machine.nPending = 0;
	break;

  }
}


tTLE5012_stat tle5012GetStatus()
// get the extended status error.
// To be called by lower priority ISR than SPI ISR 
// and TriggerSample caller (FOC DMA0),
// otherwise it can be racy.
{

  tTLE5012_stat ret;


  // SPI isr / triggersample caller has higher priority 
  // than this.
  // if the triggersample func get executed after accessMutex 
  // is set to 1, it immediately returns and the SPI is not read anymore. 
  // Otherwise the nPending flag is set, and never reset until 
  // the whole SPI procedure finishes.   
  tle5012_state_machine.accessMutex = 1;

  while(tle5012_state_machine.nPending);

  ret.w = tle5012ReadReg(TLE5012_STAT);

  tle5012_state_machine.accessMutex = 0;
  
  return ret;
	
}

#endif

