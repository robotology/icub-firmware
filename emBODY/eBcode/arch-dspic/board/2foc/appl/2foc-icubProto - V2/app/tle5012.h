#ifndef __TLE5012_H__
#define __TLE5012_H__

#include "System.h"
#define TLE5012_MAGIC_SYSREG 0b1010

// command to be send over SPI, it causes read or write operations.
// read operations could be multiple with or without address autoincrement
typedef union  tTLE5012_command
{
  struct
  {
	unsigned len:4; // no of data word to read or write
	unsigned addr:6; // start address 
	unsigned upd:1; // updated or current values
	unsigned lock:4; // must be 0 if reg <5. Must be 0b1010 otherwise
	unsigned rw:1; // 1 means read. 0 means write

  }__attribute__ ((__packed__)) Bits;

  // Permits to access the whole structure data in byte/word/array fashon 
  unsigned int w;
    
} __attribute__ ((__packed__)) tTLE5012_command;


typedef union tTLE5012_safetyword
{
	struct {
		unsigned CRC:8;
		unsigned resp:4;
		unsigned angleIsValid:1;
		unsigned interfaceAccessError:1;
		unsigned systemError:1;
		unsigned chipReset:1;
	}__attribute__ ((__packed__)) Bits;
	
	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;

} __attribute__ ((__packed__)) tTLE5012_safetyword;



/* STATUS REGISTER */
#define TLE5012_STAT	0x0
typedef union tTLE5012_stat
{
	struct{
		unsigned reset:1;
		unsigned watchdog:1;
		unsigned voltageRegulator:1;
		unsigned fuseCRC:1;
		unsigned DSP:1;
		unsigned overflow:1;
		unsigned XYdataOutOfLimit:1;
		unsigned magnitudeOutOfLimit:1;
		unsigned reserved:1;
		unsigned ADCTest:1;
		unsigned ROMCRC:1;
		unsigned GMRXNotYvalid:1;
		unsigned GMRAngleNotValid:1;
		unsigned SlaveNumber:2;
		unsigned StatusChanged:1;
	}__attribute__ ((__packed__)) Bits;

	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;

} __attribute__ ((__packed__)) tTLE5012_stat;



/* ACTIVATION STATUS REGISTER */
#define TLE5012_ACSTAT	0x1
typedef union tTLE5012_acstat
{

	struct{
		unsigned HWReset:1;
		unsigned watchdog:1;
		unsigned regulator:1;
		unsigned fuseCRC:1;
		unsigned DSPUBIST:1;
		unsigned DSPUOverflow:1;
		unsigned ADCBIST:1;
		unsigned ADCRedundancyBIST:1;
		unsigned reserved2:1;
		unsigned GMRVector:1;
		unsigned reserved:6; /*0b010111*/
	}__attribute__ ((__packed__)) Bits;

} __attribute__ ((__packed__)) tTLE5012_acstat;


/* ANGLE VALUE REGISTER */
#define TLE5012_AVAL	0x2
typedef union tTLE5012_aval
{
	struct{
		unsigned angleVal:15;
		unsigned valueNew:1;
	}__attribute__ ((__packed__)) Bits;

	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;

} __attribute__ ((__packed__)) tTLE5012_aval;

/* ANGLE SPEED REGISTER */
#define TLE5012_ASPD	0x3
typedef union tTLE5012_aspd
{
	struct{
		signed angleSpeed:15;
		unsigned valueNew:1;
	}__attribute__ ((__packed__)) Bits;

	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;

} __attribute__ ((__packed__)) tTLE5012_aspd;

/* ANGLE REVOLUTION REGISTER */
#define TLE5012_AREV	0x4
typedef union tTLE5012_arev
{

	struct{
		signed revolutionsCount:9;
		unsigned frameCount:6;
		unsigned valueNew:1;
	}__attribute__ ((__packed__)) Bits;

	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;

} __attribute__ ((__packed__)) tTLE5012_arev;

/* FRAME SYNCHRONIZATION REGISTER */
#define TLE5012_FSYNC	0x5
typedef union tTLE5012_fsync
{
	struct{
		unsigned reserved:8;
		unsigned count:8;
	}__attribute__ ((__packed__)) Bits;
	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;

} __attribute__ ((__packed__)) tTLE5012_fsync;


/* INTERFACE MODE1 REGISTER */
#define TLE5012_MOD1	0x6
#define TLE5012_FILTER_DECIMATION_21us 0
#define TLE5012_FILTER_DECIMATION_42us 1
#define TLE5012_FILTER_DECIMATION_85us 2
#define TLE5012_FILTER_DECIMATION_170us 3
#define TLE5012_IIF_MODE_INDEXONDATA 1
#define TLE5012_IIF_MODE_DISABLED 0
typedef union tTLE5012_mod1
{
	struct{
		unsigned IIFMode:2;
		unsigned holdDSPU:1;
		unsigned SSCMode:1;
		unsigned CLKSource:1;
		unsigned reserved:9;
		unsigned FIRDecimation:2;
	}__attribute__ ((__packed__)) Bits;

	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;

} __attribute__ ((__packed__)) tTLE5012_mod1;

/* SIL REGISTER */
#define TLE5012_SIL		0x7
typedef union tTLE5012_sil
{
	struct{
		unsigned ADCTestVecX:3;
		unsigned ADCTestVecY:3;
		unsigned ADCTestVecEnable:1;
		unsigned reserved:3;
		unsigned fuseReload:1;
		unsigned reserved2:3;
		unsigned filterInverted:1;
		unsigned filterParallel:1;
	}__attribute__ ((__packed__)) Bits;

	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;

} __attribute__ ((__packed__)) tTLE5012_sil;


/* INTERFACE MODE2 REGISTER */
#define TLE5012_MOD2	0x8
#define TLE5012_AUTOCAL_MODE_DISABLED 0
#define TLE5012_AUTOCAL_MODE_TUPD 1
#define TLE5012_AUTOCAL_MODE_22DEG 2
#define TLE5012_AUTOCAL_MODE_11DEG 3
#define TLE5012_PREDICTION_DISABLE 0
#define TLE5012_PREDICTION_ENABLE 1
#define TLE5012_ANGLE_DIRECTION_CW 1
#define TLE5012_ANGLE_DIRECTION_CCW 0
typedef union tTLE5012_mod2
{

	struct{
		unsigned autocalMode:2;
		unsigned prediction:1;
		unsigned angleDirection:1;
		unsigned angleRange:11;
		unsigned reserved:1;
	}__attribute__ ((__packed__)) Bits;

	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;

} __attribute__ ((__packed__)) tTLE5012_mod2;


/* INTERFACE MODE3 REGISTER */
#define TLE5012_MOD3	0x9
typedef union tTLE5012_mod3
{

	struct{
		unsigned padDriverMode:2;
		unsigned reserved:1;
		unsigned analogSpikeFilter:1;
		unsigned angleBase:12;
	}__attribute__ ((__packed__)) Bits;

	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;

} __attribute__ ((__packed__)) tTLE5012_mod3;


/* OFFSET X REGISTER */
#define TLE5012_OFFX	0xA
typedef union tTLE5012_offx
{
	struct{
		unsigned reserved:4;
		unsigned XOffset:12;
	}__attribute__ ((__packed__)) Bits;

	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;

} __attribute__ ((__packed__)) tTLE5012_offx;


/* OFFSET Y REGISTER */
#define TLE5012_OFFY	0xB
typedef union tTLE5012_offy
{
	struct{
		unsigned reserved:4;
		unsigned YOffset:12;
	}__attribute__ ((__packed__)) Bits;

	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;

} __attribute__ ((__packed__)) tTLE5012_offy;


/* SYNCHRONICITY REGISTER */
#define TLE5012_SYNCH	0xC
typedef union tTLE5012_sync
{
	struct{
		unsigned reserved:4;
		unsigned amplitudeSynchronicity:12;
	}__attribute__ ((__packed__)) Bits;

	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;

} __attribute__ ((__packed__)) tTLE5012_sync;


/* IFAB REGISTER */
#define TLE5012_IFAB	0xD
typedef union tTLE5012_ifab
{
	struct{
		unsigned HSMHysteresis:2;
		unsigned IFAIFBOpenDrain:1;
		unsigned XYOrthoCorrection:13;
	}__attribute__ ((__packed__)) Bits;

	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;

} __attribute__ ((__packed__)) tTLE5012_ifab;


/* INTERFACE MODE4 REGISTER */
#define TLE5012_MOD_4	0xE
typedef union tTLE5012_mod_4
{
	struct{
		unsigned interfaceMode:3;
		unsigned IFABResolution:2;
		unsigned HallSwitchMode:3;
		unsigned reserved:1;
		unsigned XOffsetTemperatureCoeff:7;	
	}__attribute__ ((__packed__)) Bits;

	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;

} __attribute__ ((__packed__)) tTLE5012_mod_4;

/* TEMPERATURE COEFFICIENT REGISTER */
#define TLE5012_TCO_Y	0xF
typedef union tTLE5012_tco_y
{
	struct{
		unsigned paramCRC:8;
		unsigned reserved:1;
		unsigned YOffestTemperatureCoeff:7;
	}__attribute__ ((__packed__)) Bits;

	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;

} __attribute__ ((__packed__)) tTLE5012_tco_y;

/* X-RAW VALUE REGISTER */
#define TLE5012_ADC_X	0x10
typedef union tTLE5012_adc_x
{
	struct{
	/* reading this causes ADC Y GMR to be updated */
		unsigned ADCXGMR:16;
	}__attribute__ ((__packed__)) Bits;
		
	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;

} __attribute__ ((__packed__)) tTLE5012_adc_x;

/* Y-RAW VALUE REGISTER */
#define TLE5012_ADC_Y	0x11
typedef union tTLE5012_adc_y
{
	struct{
	/* reading ADCXGMR or reading this causes this to be updated */
		unsigned ADCYGMR:16;
	}__attribute__ ((__packed__)) Bits;

	// Permits to access the whole structure data in byte/word/array fashon 
	unsigned int w;
	
} __attribute__ ((__packed__)) tTLE5012_adc_y;

#endif

typedef struct tTLE5012_data{
	tTLE5012_aval angle;
	tTLE5012_aspd speed;
	tTLE5012_arev revolution;
	tTLE5012_safetyword safetyword;
	int crcerror;
} tTLE5012_data;

typedef struct tTLE5012_state_machine{

	int nPending;          // this is the state variable for the spi state machine

	int buffer_flip;       // this is the state of the ping-pong buffer "swapper"

    int accessMutex;       // this is used to arbitrate SPI access. The FOC loop
                           // and the "reading extended status" funcion may in fact
                           // need to write over the SPI bus concurrently.

} tTLE5012_state_machine;

extern volatile tTLE5012_state_machine tle5012_state_machine;
extern tTLE5012_data tle5012_data[2];
extern int tle5012_crc_error_count;
extern int tle5012_error_count;
extern int tle5012_crc_illness;
extern unsigned tle5012_hw_buf __attribute__((space(dma),aligned(16)));

tTLE5012_data tle5012Get();
tTLE5012_stat tle5012GetStatus();
int tle5012Init(void);
void tle5012TriggerSample(void);
int tle5012SamplingCompleted(void);
