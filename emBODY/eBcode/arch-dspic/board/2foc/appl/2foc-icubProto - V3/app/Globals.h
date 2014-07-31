
//
// * L O T S *  of DECLARATIONS of Global variables
//
#ifdef __GLOBALS_H
#error DONOTDOTHIS
#endif

#define __GLOBALS_H

//
// default variable indexes for periodic data 
// packet (Gulp packet for data plot)
//
#define GULP_DEFAULT1 0x2A  //Measured Current
#define GULP_DEFAULT2 0x17  //Motor velocity
#define GULP_DEFAULT3 0x15  //Motor Position first 16 bit
#define GULP_DEFAULT4 0x16  //Motor Position last 16 bit 

//
// PARK-CLARKE related
//
tParkParm ParkParm;

unsigned int as5045SpiErrors = 0 ;

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



// Meccanical angle corrected (aligned with) of EncoderSyncPulseposition 
//SFRAC16 AlignedMecchanicalAngle;

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
//int speed_undersampler = 0; // W_SIMPLE_CALCULATION_UNDERSAMPLE;

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

unsigned int TogglingPeriod = 0;
int TogglingEnable = 0;

// System error flags
tSysError SysError;
volatile unsigned int FirmwarePWMTimingErrorCount = 0;

// To switch from encoder to HES realtime
// TODO: remove global and variable
unsigned UseHES=0;

// Variables for GULP!ing
tGulp Gulp;
tGulp GulpHistoryBuffer[GULP_HISTORY_BUFFER_SIZE];
int GulpHistoryBufferIdx;
// define the contents of the periodic message used as an index for PeriodicData
unsigned char PeriodicMessageContents[4] = {GULP_DEFAULT1,GULP_DEFAULT2,GULP_DEFAULT3,GULP_DEFAULT4};


// a new set of variables are requested for Gulp!ing
// if this is 1 we are waiting for FOC to fill new values
// Note that the main loop does not really wait, it just does not
// perform operations on the pending variables.
volatile int gulp_update_request = 1;

// these buffers aux vars make it possible to obtain the best optimization
// doing all precalculation off the 2foc loop
unsigned int *gulpadr1;
unsigned int *gulpadr2;
unsigned int *gulpadr3;
unsigned int *gulpadr4;

long position_limit_lower = 0;
long position_limit_upper = 0;
int position_limits_enabled = 0;

int torque_q_limit_upper = 0;
int torque_q_limit_lower = 0;
int torque_d_limit_upper = 0;
int torque_d_limit_lower = 0;

int speed_limit_upper = 0;
int speed_limit_lower = 0;


/* Default values for setpoint timeout are loaded from 
 * eeprom, so actual default values are NOT those two,
 * instead if you want to change default values you have
 * to change the eeprom initialization table.
 * If the eeprom load fails the board should go fault, so
 * it should never happen that the numbers written here
 * have any effect. If it happens, then there is something
 * going completely wrong, so put here valus that will
 * make the watchdog to trigger...
 */
unsigned long setpoint_watchdog_timeout = 1;
int setpoint_watchdog_enabled  = 1;

//
// IIR Filtering - used for I2T
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

 
  .EepromHeader.EE_CAN_BoardAddress = 0x3,  // only this is necessary!

  // init user data
  .app_data_default = 1,
  .I2TParam = I2T_PARAM,
  .I2TThreshold = I2T_THRESHOLD, 
  .CPIDP = KPId,
  .CPIDI = KIId,
  .CPIDD = KDId,
  .CPIDM = KMId,
  .WPIDP = KPWq,
  .WPIDI = KIWq,
  .WPIDD = KDWq,
  .WPIDM = KMWq,

  //torque
  .runmode = 2, 
  .position_limit_upper = 1,
  .position_limit_lower = 0,
  // safest values..
  .setpoint_watchdog_enabled = 1,
  .setpoint_watchdog_timeout = 1,

  .torque_q_limit_upper = 0,
  .torque_q_limit_lower = 0,

  .torque_d_limit_upper = 0,
  .torque_d_limit_lower = 0,

  .speed_limit_upper = 0,
  .speed_limit_lower = 0,

  .periodic_command1 = GULP_DEFAULT1,
  .periodic_command2 = GULP_DEFAULT2,
  .periodic_command3 = GULP_DEFAULT3,
  .periodic_command4 = GULP_DEFAULT4,

  .Ira_qVq = IRA_VQ,
  .Ira_direction = IRA_DIRECTION,
  .Ira_ramp_steepness = IRA_RAMP_STEEPNESS,

  .app_data_crc16 = 0xb71b,
 
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
// Vd and Vq during initial rotor alignment (for 
// strategies that requires moving rotor)
//
// actuated values, used for current ramping up-dn
volatile short IraqVq = 0;

// maximum values for ramp limit
volatile short IraqVq_max = IRA_VQ;


// rotor movement direction
volatile short Ira_direction = IRA_DIRECTION;

// the steepness of IRA current ramp
volatile short Ira_ramp_steepness = IRA_RAMP_STEEPNESS;

// updated each foc loop
// the mechanical position calculated using
// encoder value and encoder turn counter
volatile long Current_position = 0;

// the current position for EXTERNAL users.
// this is equal to the Current_position but it does
// take in account the CAN position zeroing command
volatile long Reported_position = 0;
//volatile int  Ifilt = 0;

// Offset needed for zeroing the axes position 
volatile long Offset4Zeroing=0;

// last foc cycle value
// used to count encoder turns
unsigned int Previous_encoder_value = 0;

// Freezed (undersampled at velocity undersampling rate) variable of the 
// previous position for velocity calculations
volatile long Previous_position_for_velocity_calc = 0;

// in range -65535, +65535
long encoder_aux_offset = 0;

// global to allow gulp plot
int encoder_misalign = 0;

// zero value
unsigned int Zero = 0;

// global only to enable gulp, could be local otherwise
// it's the (main) encoder value
unsigned int encoder_value = 0;

// global only to enable gulp, could be local otherwise
// it's the AUX encoder value (if present)
unsigned int encoder_aux_value = 0;

// DELME
//extern SFRAC16 rotor_alignment_dhes_approx_espp;

//
// list of possible contents of the periodic message
// 

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
/* 12 */ (unsigned int*) &Zero,
/* 13 */ (unsigned int*) &ParkParm.qSin,
/* 14 */ (unsigned int*) &ParkParm.qCos,
/* 15 */ (unsigned int*) &Reported_position, 
/* 16 */ (unsigned int*) &Reported_position + 1, // position is 32bit wide


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
/* 21 */ (unsigned int*) &Zero,
/* 22 */ (unsigned int*) &DS402_Controlword,
/* 23 */ (unsigned int*) &DS402_Statusword,

/* 24 */ (unsigned int*) &as5045SpiErrors,
/* 25 */ (unsigned int*) &encoder_misalign, 
/* 26 */ (unsigned int*) &encoder_aux_value,
/* 27 */ (unsigned int*) &encoder_value,
/* 28 */ (unsigned int*) &Encoder_SyncPulsePosition,
/* 29 */ (unsigned int*) &Zero,//&rotor_alignment_dhes_approx_espp,
/* 2A */ (unsigned int*) &I2Tdata.IQMeasured, //&Ifilt,
/* 2B */ (unsigned int*) &I2Tdata.IDMeasured,
/* 2C */ (unsigned int*) &Zero,
/* 2D */ (unsigned int*) &Zero,
/* 2E */ (unsigned int*) &Zero,
/* 2F */ (unsigned int*) &Zero
};

