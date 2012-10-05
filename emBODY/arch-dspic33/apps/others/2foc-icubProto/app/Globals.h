
//
// * L O T S *  of DECLARATIONS of Global variables
//
#ifdef __GLOBALS_H
#error DONOTDOTHIS
#endif

#define __GLOBALS_H
//
// PARK-CLARKE related
//
tParkParm ParkParm;

unsigned int debug_spi_errors = 0 ;
unsigned int debug_i2t = 0;

// Space vector generator parameters
tSVGenParm SVGenParm;

// Id (flux) Iq (torque) speed (omega) reference values 
tCtrlReferences CtrlReferences;

//
// ADC (ia, ic, VDCLink)
//
	
//ADC offset correction
tMeasCurrParm MeasCurrParm;

volatile SFRAC16 VDCLink; // DC Bus measured continuously and stored in this variable
  // while motor is running. When voltasge ripple compensation is enabled this value will 
  // be compared with TargetDCbus and Vd and Vq will be compensated depending on difference
  // between DCbus and TargetDCbus

SFRAC16 TargetDCbus = 0; // DC Bus is measured before running motor and stored in this variable. 
  // Any variation on DC bus will be compared to this value and compensated linearly.	

//
// Encoder, position related
//

// Meccanical angle 
SFRAC16 UnalignedMecchanicalAngle=0;

// Meccanical angle corrected (aligned with) of EncoderSyncPulseposition 
SFRAC16 AlignedMecchanicalAngle;

// internal state for SPI state machine
tAS5045_state_machine as5045_state_machine;
//
tAS5045_data as5045_data[2];

// state machine for SPI data stream capture
volatile tTLE5012_state_machine tle5012_state_machine;
// useful data from TLE 
tTLE5012_data tle5012_data[2];
// Absolute number of errors occurred since turn on
int tle5012_crc_error_count;
// number of TLE5012 internal errors not related to CRC or SPI related
int tle5012_error_count;
// Quality of service on encoder data transmission
int tle5012_crc_illness;
// TODO: integrare nella QOS la presenza di errori 'tle5012_error_count' non relativi al CRC
 
// Structure for direct access without DMA 
#ifndef SPI_DMA
  tAS5045_hw_data as5045_hw_data;
#endif

//
// Per a fasatura meccanica del motore:
// Imporre la coppia alternante ENABLE_TOGGLIN_REFERENCE 
// graficare con Gulp ParkParm.qValpha e ParkParm.qAngle fasare l'encoder fino ad allineare
// i passaggi per lo zero della sinusoide e della rampa in entrambe le direzioni
//
// store the distance (normalized in the range -1..0.9999) of the maximum of the rotor flux 
// from the encoder index position
SFRAC16 Encoder_SyncPulsePosition;
// store the initial rotor position (normalized in the range -1..0.9999) 
SFRAC16 Encoder_RotorStartingPosition;

// this degradates encoder performances in order to do tests
// greater this number, worse the performances.
// when 1 the encoder works at its full resolution
unsigned int EncoderDegradation = 1;
// Number of rounds
// TODO: UNSIGNED? e se gira dall'altra parte?
volatile int Turns=0;

//open loop simulated encoder value
int OpenloopAngle;


//
// speed calculation related vars
//
volatile tVelocityParm VelocityParm;
// used for the velocity calculation undersampling
int speed_undersampler =  W_SIMPLE_CALCULATION_UNDERSAMPLE;

// Velocity measurement from TLE5012
volatile int tle_speed;

//
// Torque loop with reference from sensor related vars
//
// gain *after* PID calculation (shift factor) 
int TorqueGain=1;
// measured torque from F/T Sensor
volatile int SensorTorque = 0;
// variable for zeroing the offset of F/T Sensor
volatile int TorqueOffset = 0;
// Elaborated sensed torque (LPF, PID, Gain, ...) 
volatile int Torque = 0;
// TODO: rimuovere
int TorcAndersempling = 1;
int ASCounter=1;

//
// PID Error snoop variables (for gulp & dbg) 
//

SFRAC16 WPIDError;
SFRAC16 QPIDError;
SFRAC16 DPIDError;


//
// CAN command variables 
//
// this is NOT in syserror struct because we never wants
// to make this flag to be part of a CAN msg (because if
// we can still receive CAN msg from board, the we already
// known it it not in bus off
volatile int CAN_BusOff = 0;
//
// System  related 
//
// Machine status
tSysStatus SysStatus;
// Led multiplexing status, init as all LED off
// unsigned short LED_status = 0x00;
volatile tLED_status LED_status = {0x00};

// VDCLink in 100mV units
unsigned int vdc;

// Toggling torque reference value
SFRAC16 TorqueTogglingReference=0;
// Toggling speed reference value
SFRAC16 OmegaTogglingReference=0;

unsigned int TogglingPeriod = TOGGLIN_REFERENCE_PER;
#ifdef ENABLE_TOGGLIN_REFERENCE
int TogglingEnable = 1;
#else
int TogglingEnable = 0;
#endif

// System error flags
tSysError SysError;
volatile unsigned int FirmwarePWMTimingErrorCount = 0;

// To switch from encoder to HES realtime
// TODO: remove global and variable
unsigned UseHES=0;

// Variables for GULP!ing
tGulp Gulp;
// define the contents of the periodic message used as an index for PeriodicData
unsigned char PeriodicMessageContents[4] = {0x6,0x15,0x16,0x17};

// a new set of variables are requested for Gulp!ing
// if this is 1 we are waiting for FOC to fill new values
// Note that the main loop does not really wait, it just does not
// perform operations on the pending variables.
volatile int gulp_update_request = 1;

//
// IIR Filtering
//

// maimum number of BiQuads for the IIR LPF 
#define IIR_LPF_N_MAX_BQ  5  

//
// IIR Trasposed filter
//
IIRTransposedStruct iirt;

// temporary storage
fractional IirTransposedState1[IIR_LPF_N_MAX_BQ * 1] __attribute__((__space__(ymemory)));
fractional IirTransposedState2[IIR_LPF_N_MAX_BQ * 1] __attribute__((__space__(ymemory)));

fractional destinazione[2]={0};
fractional sorgente[2]={0};

fractional IirTransposedCoefs[ 5 * IIR_LPF_N_MAX_BQ] __attribute__((__space__(xmemory))) =
// IIR transoposed filter coefs
// filter coefs LPF 100Hz
{
0x0099,
0xFFEB,
0x71B6,
0x0099,
0xCD2C,
0x065D,
0xF611,
0x745C,
0x065D,
0xC8D9,
0x107C,
0xE359,
0x789D,
0x107C,
0xC305,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000
};

//
// EmuROM related variables & declarations
//

// this instruction defines and initialize the data structure in the EmuROM
// values other than BoardAddress are placed for future upgrades
PLACE_EMU_ROM_DATA(EMURomSpace, tEepromData) = { 
  .EepromHeader.EE_B_EEErased=0,
  .EepromHeader.EE_B_EnableWD=0,
  .EepromHeader.EE_B_EnableBOR=0,
  .EepromHeader.EE_CAN_BoardAddress = 0x3,  // only this is necessary!
  .EepromHeader.EE_CAN_MessageDataRate = 0,
  .EepromHeader.EE_CAN_Speed= 0,
  // init user data
  .I2TParam = I2T_PARAM,
  .I2TThreshold = I2T_THRESHOLD, 
  .CPIDP = KPId,
  .CPIDI = KIId,
  .CPIDD = KDId,
  .WPIDP = KPWq,
  .WPIDI = KIWq,
  .WPIDD = KDWq,
};

// declare a variable in local space (ram) that will contain after the call
// 	INIT_EMU_ROM_READVAR(MyRomSpace, EmuROMData);
// the data stored in EmuROM
DECLARE_EMU_ROM_READVAR(ApplicationData, tEepromData)

//
// DMA Space
//

// copy of the internal register structure of TLE
unsigned tle5012_hw_buf __attribute__((space(dma),aligned(16)));

#ifdef SPI_DMA
tAS5045_hw_data as5045_hw_data __attribute__((space(dma),aligned(16)));
#endif

//TODO: del me if we use really the parser, and we never switch to virtual can memory map
// the virtual memory addressed by can extended frames.

// Buffers used to store ADC Samples
SFRAC16 ADCBuffer[4] __attribute__((space(dma),aligned(16))); 	

//
// I2T related variables
//
tI2T I2Tdata;

//
// DS402 state machine as defined in 'CiA DSP 402 V 2.0' pag. 45
//
volatile tControlword DS402_Controlword = { .W[0]=0 };

// init state as NOT READY TO SWITCH ON:  
// DS402 State xxxx xxxx x0xx 0000 
volatile tStatusword  DS402_Statusword = { .W[0]=0 };

//
// Overriders for Vd and Vq during initial rotor alignment
//
// controls Id = flux
volatile short IraqVd = IRA_VD;
// controls Iq = torque
volatile short IraqVq = IRA_VQ;

//TODO: 
//TODO: eliminare la variabile LOCALE position che non vuole dire nulla e fare funzionare 
//      il tutto in modo piu' charo senza magia nera utilizzando la variabile globale MeccanicalAngle
//TODO: 
//TODO: 
// volatile unsigned long Position = 0;

// updated each foc loop
volatile  long Current_position = 0;
// last foc cycle value
volatile  long Previous_position = 0;

// Freezed (undersampled at velocity undersampling rate) vaiable of the 
// previous position for velocity calculations
volatile long Previous_position_for_velocity_calc = 0;

// zero value
unsigned int Zero = 0;

//
// list of possible contents of the periodic message
// 

extern unsigned int eb;

const unsigned int * PeriodicData[ELEMENTS_IN_PREIODIC_DATA_LIST]={
    //
    // Trasformation related
    //
/* 0 */  (unsigned int*) &ParkParm.qIa,
/* 1 */  (unsigned int*) &ParkParm.qIb,
/* 2 */  (unsigned int*) &ParkParm.qIc,
   //
/* 3 */  (unsigned int*) &ParkParm.qIalpha,
/* 4 */  (unsigned int*) &ParkParm.qIbeta,
  //
/* 5 */  (unsigned int*) &ParkParm.qId,
/* 6 */  (unsigned int*) &ParkParm.qIq,
  //
/* 7 */  (unsigned int*) &ParkParm.qVd,
/* 8 */  (unsigned int*) &ParkParm.qVq,
  //
/* 9 */  (unsigned int*) &ParkParm.qValpha,
/* A */  (unsigned int*) &ParkParm.qVbeta,
  //
/* B */  (unsigned int*) &SVGenParm.qVr1,
/* C */  (unsigned int*) &SVGenParm.qVr2,
/* D */  (unsigned int*) &SVGenParm.qVr3,
    //
    // PWM related
    //
/* E */  (unsigned int*) &PDC1,
/* F */  (unsigned int*) &PDC2,
/* 10 */ (unsigned int*) &PDC3,
   //
   // angle related
   //
/* 11 */ (unsigned int*) &ParkParm.qAngle, 
/* 12 */ (unsigned int*) &AlignedMecchanicalAngle,
/* 13 */ (unsigned int*) &ParkParm.qSin,
/* 14 */ (unsigned int*) &ParkParm.qCos,
/* 15 */ (unsigned int*) (((void*)(&Current_position)) + 2), // position is 32bit wide
/* 16 */ (unsigned int*) ((void*)(&Current_position)),
  //
  // Velocity related
  //
/* 17 */ (unsigned int*) &VelocityParm.Velocity, 
  //
  // PID related
  //
/* 18 */ (unsigned int*) &CtrlReferences.qIdRef, 
/* 19 */ (unsigned int*) &CtrlReferences.qIqRef, 
/* 1A */ (unsigned int*) &CtrlReferences.qWRef,
/* 1B */ (unsigned int*) &TorqueTogglingReference,
/* 1C */ (unsigned int*) &OmegaTogglingReference,

  //
  // Voltage related
  //
/* 1D */ (unsigned int*) &VDCLink,
/* 1E */ (unsigned int*) &vdc,
  //
  // system related
  //
/* 1F */ (unsigned int*) (((void*)(&SysStatus))+2),  // SysStatus is 32 bit wide
/* 20 */ (unsigned int*) ((void*)(&SysStatus)),
/* 21 */ (unsigned int*) &SysError,
/* 22 */ (unsigned int*) &DS402_Controlword,
/* 23 */ (unsigned int*) &DS402_Statusword,

/* 24 */ (unsigned int*) &debug_spi_errors,
/* 25 */ (unsigned int*) &debug_i2t,
/* 26 */ (unsigned int*) &eb,  // encoder buffer or hall effects
/* 27 */ (unsigned int*) &UnalignedMecchanicalAngle,
/* 28 */ (unsigned int*) &Zero,
/* 29 */ (unsigned int*) &Zero,
/* 2A */ (unsigned int*) &Zero,
/* 2B */ (unsigned int*) &Zero,
/* 2C */ (unsigned int*) &Zero,
/* 2D */ (unsigned int*) &Zero,
/* 2E */ (unsigned int*) &Zero,
/* 2F */ (unsigned int*) &Zero
};

