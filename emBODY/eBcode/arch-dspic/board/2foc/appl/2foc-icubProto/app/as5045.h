/* Created by AM for as5040 SPI encoder */

#ifndef __AS5045_H
#define __AS5045_H

#include "spi.h"
#include "dma.h"
#include "UserTypes.h"
#include "system.h"


// DO NOT REMOVE THIS INCLUDE
// this includse permits to compile the proper spi-dma-irq handler, 
//   if removed something very nasty happsns
#include "userparms.h"
// DO NOT REMOVE THIS INCLUDE
#ifndef UserParms_H
  #error userparms.h MUST be included
#endif

// Enable SPI DMA on Channel 3
// TODO: utilizzando NON DMA c'e' ancora qualche sparacchio 
#define SPI_DMA

// this is the RAW representation of the as5045 SPI data read on the wire
typedef union  tAS5045_hw_data
{
  struct
  {
    // Word 0 and 1 from SPI (18 bit)
    unsigned MagFieldInc:1; // magnetic increase
    unsigned LinearityError:1; // Linearity error detected
    unsigned CORDICOveerflow:1; // CORDIC overflow error detected
    unsigned OffsetCompensationFinished:1; //OCF (Offset Compensation Finished), 
      //  logic high indicates the finished Offset Compensation Algorithm
    unsigned Angle:12; // Angular position, 12 bit

    // Word 1 
    unsigned :14; // filling for 2 complete words. NOTE THAT 2nd READ WILL PRODUCE the 2-BITs DATA ON MSBs!!!
    unsigned Parity:1; // (Even) parity checksum
    unsigned MagFieldDec:1;  // magnetic decrease
  } Flags;

  // Permits to access the whole structure data in byte/word/array fashon 
  unsigned char b[4];
  // word
  unsigned int w[2];
  // double word
  unsigned long dw[1];

//           _   _
//          (_)-(_)
//  -----ooO-(x x)-Ooo------
// |         -(_)-          |
// |                        |
// |       ** RIP **        |
// |    Kilroy WAS here     |
//  ------------------------ 
//
// unsigned int kilroy[40]; 

} __attribute__ ((__packed__)) tAS5045_hw_data;


// this is the software representation of the cooked as5045 data 
typedef struct tAS5045_data {

	unsigned int Angle;
    
    // QOS accumulated errors
    unsigned AccumlatedErrors:8;  // +10 each error -1 each successful communication

    unsigned CheckSumError:1;     // indicates whether last packet from as5045 had incorrect csum
    unsigned MagneticFieldError:1;// indicates whether as5040 detected some DSP error
    unsigned CommunicationError:1; // not used yet.

} tAS5045_data;

typedef struct tAS5045_state_machine{

	int nPending;          // this is the state variable for the spi state machine
	int buffer_flip;       // this is the state of the ping-pong buffer "swapper"
} tAS5045_state_machine;

extern void as5045TriggerSample(void);
extern void as5045Init(void);
extern tAS5045_data as5045Get();
extern int as5045SamplingCompleted();

//extern unsigned volatile int  as5045_buffer_count; 
#ifdef SPI_DMA
  extern tAS5045_hw_data as5045_hw_data __attribute__((space(dma)));
#else
  extern tAS5045_hw_data as5045_hw_data;
#endif

extern tAS5045_data as5045_data[2];
extern tAS5045_state_machine as5045_state_machine;

#endif
