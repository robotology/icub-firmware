
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

// PARK-CLARKE related
tParkParm ParkParm;

// Id (flux) Iq (torque) speed (omega) reference values 
tCtrlReferences CtrlReferences;

// ADC (ia, ic, VDCLink)
//ADC offset correction
tMeasCurrParm MeasCurrParm;
volatile SFRAC16 VDCLink; // DC Bus measured continuously and stored in this variable
  // while motor is running. When voltasge ripple compensation is enabled this value will 
  // be compared with TargetDCbus and Vd and Vq will be compensated depending on difference
  // between DCbus and TargetDCbus

SFRAC16 TargetDCbus = 0; // DC Bus is measured before running motor and stored in this variable. 
  // Any variation on DC bus will be compared to this value and compensated linearly.	


// Encoder, position related
volatile long gQEPosition = 0;
volatile int  gQEVelocity = 0;


// Structure for direct access without DMA 
#ifndef SPI_DMA
  //tAS5045_hw_data as5045_hw_data;
#endif

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

// System error flags
tSysError SysError;

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
//int setpoint_watchdog_enabled  = 1;

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
fractional IirTransposedCoefs[ 5 * IIR_LPF_N_MAX_BQ] __attribute__((__space__(xmemory))) =
// IIR transoposed filter coefs
// filter coefs LPF 100Hz
{
0x0099, 0xFFEB, 0x71B6, 0x0099, 0xCD2C, 0x065D, 0xF611, 0x745C,
0x065D, 0xC8D9, 0x107C, 0xE359, 0x789D, 0x107C, 0xC305, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
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
  //.I2TParam = I2T_PARAM,
  //.I2TThreshold = I2T_THRESHOLD,
  /*
  .CPIDP = KPId,
  .CPIDI = KIId,
  .CPIDD = KDId,
  .CPIDM = KMId,
  .WPIDP = KPWq,
  .WPIDI = KIWq,
  .WPIDD = KDWq,
  .WPIDM = KMWq,
  */
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

//  .Ira_qVq = IRA_VQ,
//  .Ira_direction = IRA_DIRECTION,
//  .Ira_ramp_steepness = IRA_RAMP_STEEPNESS,

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


// zero value
unsigned int Zero = 0;

//
// list of possible contents of the periodic message
// 

const unsigned int * PeriodicData[ELEMENTS_IN_PREIODIC_DATA_LIST]={
    //
    // Trasformation related
    //
/* 0 */  (unsigned int*) &Zero,//&ParkParm.qIa,
/* 1 */  (unsigned int*) &Zero,//&ParkParm.qIb,
/* 2 */  (unsigned int*) &Zero,//&ParkParm.qIc,
   //
/* 3 */  (unsigned int*) &Zero,//&ParkParm.qIalpha,
/* 4 */  (unsigned int*) &Zero,//&ParkParm.qIbeta,
  //
/* 5 */  (unsigned int*) &Zero,//&ParkParm.qId,
/* 6 */  (unsigned int*) &Zero,//&ParkParm.qIq,
  //
/* 7 */  (unsigned int*) &Zero,//&ParkParm.qVd,
/* 8 */  (unsigned int*) &Zero,//&ParkParm.qVq,
  //
/* 9 */  (unsigned int*) &Zero,//&ParkParm.qValpha,
/* A */  (unsigned int*) &Zero,//&ParkParm.qVbeta,
  //
/* B */  (unsigned int*) &Zero,//&SVGenParm.qVr1,
/* C */  (unsigned int*) &Zero,//&SVGenParm.qVr2,
/* D */  (unsigned int*) &Zero,//&SVGenParm.qVr3,
    //
    // PWM related
    //
/* E */  (unsigned int*) &PDC1,
/* F */  (unsigned int*) &PDC2,
/* 10 */ (unsigned int*) &PDC3,
   //
   // angle related
   //
/* 11 */ (unsigned int*) &Zero,//&ParkParm.qAngle,
/* 12 */ (unsigned int*) &Zero,//&Zero,
/* 13 */ (unsigned int*) &Zero,//&ParkParm.qSin,
/* 14 */ (unsigned int*) &Zero,//&ParkParm.qCos,
/* 15 */ (unsigned int*) &gQEPosition,
/* 16 */ (unsigned int*) &gQEPosition + 1, // position is 32bit wide


  //
  // Velocity related
  //
/* 17 */ (unsigned int*) &gQEVelocity,
  //
  // PID related
  //
/* 18 */ (unsigned int*) &CtrlReferences.VqRef,
/* 19 */ (unsigned int*) &CtrlReferences.IqRef, 
/* 1A */ (unsigned int*) &CtrlReferences.WRef,
/* 1B */ (unsigned int*) &Zero,//&TorqueTogglingReference,
/* 1C */ (unsigned int*) &Zero,//&OmegaTogglingReference,

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
/* 21 */ (unsigned int*) &Zero,//&Zero,
/* 22 */ (unsigned int*) &DS402_Controlword,
/* 23 */ (unsigned int*) &DS402_Statusword,

/* 24 */ (unsigned int*) &Zero, //&as5045SpiErrors,
/* 25 */ (unsigned int*) &Zero,//&encoder_misalign,
/* 26 */ (unsigned int*) &Zero,//&encoder_aux_value,
/* 27 */ (unsigned int*) &Zero,//&encoder_value,
/* 28 */ (unsigned int*) &Zero,//&Encoder_SyncPulsePosition,
/* 29 */ (unsigned int*) &Zero,//&Zero,//&rotor_alignment_dhes_approx_espp,
/* 2A */ (unsigned int*) &I2Tdata.IQMeasured, //&Ifilt,
/* 2B */ (unsigned int*) &I2Tdata.IDMeasured,
/* 2C */ (unsigned int*) &Zero,
/* 2D */ (unsigned int*) &Zero,
/* 2E */ (unsigned int*) &Zero,
/* 2F */ (unsigned int*) &Zero
};

