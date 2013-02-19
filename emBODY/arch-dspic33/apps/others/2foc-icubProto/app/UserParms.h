 /**********************************************************************
 *
 *    Filename:       UserParms.h
 *
 *    Tools used:     MPLAB IDE -> 8.30
 *                    C30 -> 3.12
 *
 *  In this file are grouped all the parameters that the user is likely 
 *  to change when a new motor, encoder or load are connected.
 *  Other parameters are related to HW configuration and control loops
 *  are not likely to be changed by the average user.
 *
 **********************************************************************/

#ifndef UserParms_H
#define UserParms_H

#define FIRMWARE_REVISION_STRING "2.7"

// HW revision 3 or previous
// #define HARDWARE_REVISION_3
// HW revision 4 or successive
#define HARDWARE_REVISION_4

#ifdef HARDWARE_REVISION_4
#define HARDWARE_REVISION_STRING "4.0"
#elif defined HARDWARE_REVISION_3
#define HARDWARE_REVISION_STRING "3.0"
#else
#define HARDWARE_REVISION_STRING "???"
#endif

// Enables output on pin RA3 (extfault) (P1.5) for debug pourpouses
// #define PIN_RA3_IS_DEBUG
#define PIN_RA3_IS_EXTERNAL_FAULT

// Enables a toggling reference (+/-) for torque or speed values
//#define ENABLE_TOGGLIN_REFERENCE
// period of torque reference reversal
#define TOGGLIN_REFERENCE_PER 0x4B0

//
// Motor Parameters
//
// Phase custom motor has 7 pole coupoles
//#define PHASE_MOTOR 1
#ifdef PHASE_MOTOR
  #define NPOLEPAIRS  7
#else
  // other motors has 4 pole couples
  #define NPOLEPAIRS  4
#endif

// Number of poles
#define NPOLES      (NPOLEPAIRS*2) 

//#define NOMINALSPEEDINRPM 3000      // Make sure NOMINALSPEEDINRPM generates a MAXOMEGA < 1.0, Use this formula:
  // MAXOMEGA = NOMINALSPEEDINRPM*SPEEDLOOPTIME*POLEPAIRS*2/60. If MAXOMEGA > 1.0, reduce NOMINALSPEEDINRPM 
  // or execute  speed loop faster by reducing SpeedLoopTime.
  // Maximum position of POT will set a reference of NOMINALSPEEDINRPM.
//#define PHASERES   ((float)2.67)    // Phase resistance in Ohms.
//#define PHASEIND   ((float)0.00192) // Phase inductance in Henrys.

//
// Choose the encoder type between:
//   ENCODER_QE, incremental Quadrature encoder(AB+Z) (AS5306, Avago, ...) 
//   ENCODER_ABS, absolute SPI encoder (AS5045)
//   ENCODER_AIE, Absolute SPI and quadrature Incremental quadrature (AIE, AS5040, AS5145)
//   ENCODER_DHES, digital Hall effect sensors as encoder
//   ENCODER_TLE, Infineon TLE5012 absolute magnetic GMR encoder
// #define ENCODER_QE
// #define ENCODER_ABS
// #define ENCODER_AIE
 #define ENCODER_DHES
// #define ENCODER_TLE


// when this is enabled it is possible to degradate encoder
// performances via CAN command.
// Comment this if you want to be sure encoder will always run at its 
// best performances (maybe in production releases).
// Also enabling this has a sligth 2foc loop computational load increase.
//#define ENCODER_DEGRADATION_ENABLED

// check consitency of encoder choose
#if !(defined(ENCODER_QE) || defined(ENCODER_ABS) || defined(ENCODER_AIE) || defined(ENCODER_DHES) || defined(ENCODER_TLE))
#error Missing DEFINITION ENCODER_ABS/ENCODER_QE/ENCODER_AIE/ENCODER_DHES 
#endif

// check for duplicate definitions
#ifdef ENCODER_QE
  #ifdef ENCODER_ABS
    #error Encoder type Defined two times
  #endif
  #ifdef ENCODER_AIE
    #error Encoder type Defined two times
  #endif
#endif
#ifdef ENCODER_ABS
  #ifdef ENCODER_AIE
    #error Encoder type Defined two times
  #endif
#endif

//
// Quadrature (incremental) Encoders parameters
// 

// define PPR and swap A/B strategy for known qadrature incremental encoders 
#ifdef ENCODER_QE
  // comment to use generic homing without index signal
  #define ENCODER_USE_ZERO_FOR_ROTOR_ALIGNMENT

  // comment to use the QE hardware to detect the index pin transition
  // uncomment to use the GPIO interrupt to detect the index pin transition
  // The QE hardware will achieve better precision because it will reset
  // the encoder counter in hardware but it is not able to detect index 
  // transitions that does not occour on the phase signal edge.
  // On the other hand using the GPIO interrupt will provide worse
  // precision because the encoder counter value is reset by sw.
  // Furthermore using the QE hardware will enables also digital filtering
  // on the idx signal.
  // #define ENCODER_ZERO_DETECTION_USE_INTERRUPT

  //following defines allows to choose which quadrature encoder to use 
  //
  //uncomment the following to use parameters for Mecapion
  //#define QE_MECAPION_PARAMS
  //uncomment the following to use parameters for Avago
  // #define QE_AVAGO_PARAMS
  //uncomment the following to use parameters for AS5306
  // #define QE_AS5306_PARAMS
  //uncomment the following to use ChipEncoder CE300-4 reflective encoder
  // #define QE_CE300_4_PARAMS
  //uncomment the following to use *THE REFERENCE* encoder RI58
  // #define QE_RI58
  //uncomment the following to use Avago AEDR-850x reflective encoder interpolation x1
  // #define QE_AEDR850xX1
  //uncomment the following to use Avago AEDR-850x reflective encoder interpolation x2
  // #define QE_AEDR850xX2 
  //uncomment the following to use Avago AEDR-850x reflective encoder interpolation x4
  // #define QE_AEDR850xX4 
  // uncomment the following to use AMS AS5311 madnetic encoder incremental mode
  // #define QE_AS5311_I 
  

  #ifdef QE_MECAPION_PARAMS
    // magic number of encoder pulses from encoder zero signal to 90 deegree field
    // for the three motors tested best values are 0x3395, 0x33a5 and 0x33d0
    #define QE_ZERO_SYNCPULSEPOSITION 0x33a5

    // PPR = Number of pulses (in one phase A or B) for revolution 
    // Sets actual PPR to mecapion value if we are using mecapion encoder
    #define QE_PPR                   2500
    // If the qep encoder counts in reverse directions A and B signals may be swapped here
    // For mecapion swap A and B
    #define QE_SWAPAB                1
    // the index pulse occours every x pulses
    #define QE_INDEX_EVERY_X_PULSES  (QE_PPR*4)
  #endif

  #ifdef QE_AVAGO_PARAMS
    // PPR (number of pulses in one phase) for Avago encoder
    #define QE_PPR                   4096
    // Avago do not need to swap A and B
    #define QE_SWAPAB                0
    // the index pulse occours every x pulses
    #define QE_INDEX_EVERY_X_PULSES  (QE_PPR*4)
  #endif  

  #ifdef QE_RI58
    // PPR (number of pulses in one phase) for RI58 encoder
    #define QE_PPR                   10000
    // Avago do not need to swap A and B
    #define QE_SWAPAB                1
    // the index pulse occours every x pulses
    #define QE_INDEX_EVERY_X_PULSES  (QE_PPR*4)
  #endif

  #ifdef QE_CE300_4_PARAMS
    // PPR (number of pulses in one phase) for ChipEncoder CE300-4 encoder
    #define QE_PPR                   1970
    // CE300 do not need to swap A and B
    #define QE_SWAPAB                1
    // the index pulse occours every x pulses
    #define QE_INDEX_EVERY_X_PULSES  (QE_PPR*4)
  #endif

  #ifdef QE_AS5306_PARAMS
    // AS5306 generates on A and B phases 40 pulses every polepair
    // for a 72 poles ring magnet gives 40*(72/2)=1440 pulses (1440*4 resolution)
    // one index pulse is generated every polepair.
    #define QE_AS5306_MAGNET_NPOLES  72
    // Actual PPR used by the QEP module is AS5306
    #define QE_PPR                   (40 * QE_AS5306_MAGNET_NPOLES / 2)
    // AS5306 encoder do not need to swap A and B
    #define QE_SWAPAB                0
    // the index pulse occours every x pulses
    #define QE_INDEX_EVERY_X_PULSES  40
  #endif

  #ifdef QE_AEDR850xX1
    // encoder pulses from encoder zero signal to obtain 90 deegree field
    #define QE_ZERO_SYNCPULSEPOSITION 0xa55

    // PPR = Number of pulses (in one phase A or B) for revolution 
    #define QE_PPR                   1000
    // If the qep encoder counts in reverse directions A and B signals may be swapped here
    #define QE_SWAPAB                0
    // the index pulse occours every x pulses
    #define QE_INDEX_EVERY_X_PULSES  (QE_PPR*4)
  #endif

  #ifdef QE_AEDR850xX2
    // encoder pulses from encoder zero signal to obtain 90 deegree field
    #define QE_ZERO_SYNCPULSEPOSITION 0xa55

    // PPR = Number of pulses (in one phase A or B) for revolution 
    #define QE_PPR                   2000
    // If the qep encoder counts in reverse directions A and B signals may be swapped here
    #define QE_SWAPAB                0
    // the index pulse occours every x pulses
    #define QE_INDEX_EVERY_X_PULSES  (QE_PPR*4)
  #endif

  #ifdef QE_AEDR850xX4
    // encoder pulses from encoder zero signal to obtain 90 deegree field
    #define QE_ZERO_SYNCPULSEPOSITION 0xa55

    // PPR = Number of pulses (in one phase A or B) for revolution 
    #define QE_PPR                   4000
    // If the qep encoder counts in reverse directions A and B signals may be swapped here
    #define QE_SWAPAB                0
    // the index pulse occours every x pulses
    #define QE_INDEX_EVERY_X_PULSES  (QE_PPR*4)
  #endif

  #ifdef QE_AS5311_I
    // encoder pulses from encoder zero signal to obtain 90 deegree field
    #define QE_ZERO_SYNCPULSEPOSITION 0x0

    // PPR = Number of pulses (in one phase A or B) for revolution 
    #define QE_PPR                   256
    // If the qep encoder counts in reverse directions A and B signals may be swapped here
    #define QE_SWAPAB                0
    // the index pulse occours every x pulses
    #define QE_INDEX_EVERY_X_PULSES  (QE_PPR*4)
  #endif

  // count each phase A and B edge 
  #define QE_RESOLUTION (QE_PPR * (unsigned int)4)
  #define AE_RESOLUTION -1 // avoid GCC complaints

  #ifdef ENCODER_USE_ZERO_FOR_ROTOR_ALIGNMENT
   #ifndef QE_ZERO_SYNCPULSEPOSITION
   #error The selected encoder does not support ENCODER_USE_ZERO_FOR_ROTOR_ALIGNMENT because QE_ZERO_SYNCPULSEPOSITION is not defined
  #endif
 #endif
#else
  #define QE_RESOLUTION 0 // avoid GCC complaints
 // #define QE_ZERO_SYNCPULSEPOSITION 0 // avoid GCC complaints
#endif

//
// Absolute Encoders parameters (AS5045)
//
#ifdef ENCODER_ABS
  // define the resolution of the encoder
  #define AE_RESOLUTION            (4096)
#endif

//
// Infineon Absolute GMR encoder (TLE5012)
//
#ifdef ENCODER_TLE
  // define the resolution of the encoder
  #define AE_RESOLUTION            (32768)
  // choose internal calculation of speed 
  #define TLE_INTERNAL_SPEED    
  // choose FW calculation of speed 
  // #undef TLE_INTERNAL_SPEED    
#endif

//
// Digital Hall Effect Sensor Encoders parameters
//
#ifdef ENCODER_DHES
  // define the electrical resolution 
  // the DHES give 6 different positions each ELECTRICAL turn
  #define DHES_RESOLUTION            (6)
  #define AE_RESOLUTION               -1 // avoid GCC complaints

  // define where are connected the HES
  //#define HES_CONNECTED_TO_P2_P5
  #define HES_CONNECTED_TO_P1_P6
#endif

//
// Absolute and Incremental Encoders parameters
//
#ifdef ENCODER_AIE
  // following defines allows to choose which Absolute and Incremental encoder to use 
  //
  //uncomment the following to use parameters for AIE board
  // #define AIE_AIE_PARAMS
  //uncomment the following to use parameters for AS5040
  //#define AIE_AS5040_PARAMS
  //uncomment the following to use parameters for AS5145
  // #define AIE_AS5145_PARAMS
  //uncomment the following to use parameters for TLE5012 in Absolute+incremental mode
  #define AIE_TLE5012_PARAMS

  #ifdef AIE_AIE_PARAMS
  // the AIE encoder is composed by an Absolute encoder AS5045 plus an incremental AS5306 
    // PPR of AS5306 used in the AAE encoder
    #define QE_PPR                     (40 * 72 / 2)
    // AIE encoder do not swap A and B
    #define QE_SWAPAB                  0
    // count each phase A and B edge 
    #define QE_RESOLUTION              (QE_PPR * 4)
    // define the resolution of absolute part of the encoder
    #define AE_RESOLUTION               (4096)
  #endif

  #ifdef AIE_AS5040_PARAMS
    // PPR of AS5040
    #define QE_PPR                     (256)
    // AIE encoder do not swap A and B
    #define QE_SWAPAB                  0
    // count each phase A and B edge 
    #define QE_RESOLUTION              (QE_PPR * 4)
    // define the resolution of absolute part of the encoder
    #define AE_RESOLUTION               (1024)
  #endif

  #ifdef AIE_AS5145_PARAMS
    // PPR of AS5145
    #define QE_PPR                     (1024)
    // AIE encoder do not swap A and B
    #define QE_SWAPAB                  0
    // count each phase A and B edge 
    #define QE_RESOLUTION              (QE_PPR * 4)
    // define the resolution of absolute part of the encoder
    #define AE_RESOLUTION               (4096)
  #endif

  #ifdef AIE_TLE5012_PARAMS
    // PPR of TLE5012 used in the AAE encoder
    #define QE_PPR                     (4096)
    // AIE encoder do not swap A and B
    #define QE_SWAPAB                  1
    // count each phase A and B edge 
    #define QE_RESOLUTION              (QE_PPR * 4)
    // define the resolution of absolute part of the encoder
    #define AE_RESOLUTION              (32768)
    
    // choose internal calculation of speed 
    // #define TLE_INTERNAL_SPEED    
    // choose FW calculation of speed 
    #undef TLE_INTERNAL_SPEED    

  #endif

#endif

//
// Define if compensate the delay in the position reading delay that is measurable 
// using an SPI encoder, the fastest the speed the higer the weight of this error 
//
//#define COMP_POSITION_READ_DELAY
#define DO_NOT_COMP_POSITION_READ_DELAY

// This is used to detect encoder turns.
// When the position was close to max and wraps to 0
// or vice-versa then a "turn" is detected.
// The poisition is considered "close to" its max or
// min, when it is no more than 
// POSITION_TURN_DETECT_THRESHOLD units far from the
// considered limit
#define POSITION_TURN_DETECT_THRESHOLD 60000

//
// PID RELATED THINGS
//
/* Uncomment to enable the OLD, original, Microchip 
 * PI implementation for current loop.
 * This has been proven to be prone to overflows (et least)
 * on the error computation.
 * This have also the "excess" feature (Kc) but we never
 * used it.
 * For the straintorque loop the original Microchip PID 
 * was used (see below)
 * For speed loop you can choose the same Microchip PI
 * used for current loop or to use a C implementation of
 * the same algo.
 * This algo multiplies Kp for 2^KNo where KNo = 4
 *
 * Performances: about 1.5us per itaration
 *
 */
//#define OLD_PI
// Use C version of the velocity regualtor
// #define SPEEDLOOPINC 
// Use assembler version of the velocity regualtor
#define SPEEDLOOPINASSEMBLER 

/* Uncomment to enable the original Microchip PID
 * implementation.
 * This uses DSP instructions and should avoid original
 * PI overflow issue, and provide also the "D" component
 * but due to its particular implementation it is not
 * possible to clamp I value
 * Output limiting is currently done in C 
 * Warning: this algo does NOT multiply Kp for 2^KNo
 *
 * Performances (excluding C limiting): about 1.5us per itaration
 *
 */
//#define OLD_PID


/* Uncomment to enable the new custom PID implementation
 * (AKA PID2).
 * This uses DSP instructions and should work as the 
 * original Microchip PID, but this has also I clamping
 * for anti-windup feature. 
 * Output limiting is currently done in C 
 * In order to mantain the same coefficient scale 
 * as the original PI this algo multiplies Kp for 2^KNo 
 * where KNo = 4
 *
 * Performances (excluding C limiting): about 2us per itaration
 *
 */
#define NEW_PID


//
// PID scaling factor for P I and D factor (current D pid)
//

#define KSHIFT_D_P -4
#define KSHIFT_D_I 0
#define KSHIFT_D_D 0

//
// PID scaling factor for P I and D factor (current Q pid)
//

#define KSHIFT_Q_P -4
#define KSHIFT_Q_I 0
#define KSHIFT_Q_D 0

//
// PID scaling factor for P I and D factor (current W pid)
//

#define KSHIFT_W_P -9
#define KSHIFT_W_I 0
#define KSHIFT_W_D 0


//
//  Default PI Coefficients
//

// Id Control Loop Coefficients

// Proportional
//#define KPId        Q15(0.22) // 0.22 Per motori Emoteq
#define KPId          Q15(0.09) // 0.09 per motori Kollmorgen 
//#define KPId        Q15(0.06) // 0.02 Per motori Mecapion

// Integral
//#define KIId        Q15(0.05) // 0.05 Per motori emoteq
#define KIId          Q15(0.02) // 0.02 per motori Kollmorgen 
//#define KIId        Q15(0.02) // 0.01 per motori Kollmorgen

//Excess (used only in original Microchip PI implementation)
#define KCId          Q15(0.0)

//Derivative (used only in original Microhip PID and custom PID2 implementations)
#define KDId          Q15(0.0) 

// integral anti wind up
#define KMId          32000

// PID regulator output saturation value (Vd and Vq)
#ifdef ENCODER_DHES
  // for HES is needed more headroom
  #define     DOUTMAX    0.95
  #define     QOUTMAX    DOUTMAX
#else
  // Higer resolution encoders
  #define     DOUTMAX    0.95
  #define     QOUTMAX    DOUTMAX
#endif

// Iq Control Loop Coefficients
#define     KPIq       KPId
#define     KIIq       KIId
#define     KCIq       KCId
#define     KDIq       KDId
#define     KMIq       KMId

// Velocity (Omega) Control Loop Coefficients
#define     KPWq        Q15(0.0060)
#define     KIWq        Q15(0.00001)
#define     KCWq        Q15(0.0)
#define     KDWq        Q15(0.0)
#define     KMWq        32000 //Q15(0.9765)

// PID regulator saturation value 
#define     WOUTMAX     Q15(0.95)

//
// Torque Sensor Loop PID gain values
//
#define     KPTSensq    Q15(0.35)     //  P gain
#define     KITSensq    Q15(0.0004)   //  I gain
#define     KDTSensq    Q15(0.0006)   //  D gain

//
// Velocity calculation
//

// For a complex velocity calculation algorithm
// #define COMPLEX_VELOCITY_CALCULATION
// For the simplest method of velocity calculation
#define SIMPLE_VELOCITY_CALCULATION

// Define how much undersample the FOC loop for vel. calculation
// NOTE: if you change this you probably want also to change
// W_PID_UNDERSAMPLE (see below)
#ifdef ENCODER_DHES
 // configuration for low resolution encoders
 #define W_SIMPLE_CALCULATION_UNDERSAMPLE 199
 // As much as we undersample as much as the difference of the two
 // samples is big. We do subtraction in 32bit but the final velocity
 // must fit in 16 bits. For this reason we need to divide the final 
 // calculated velocity.
 #define W_SIMPLE_CALCULATION_DIVISOR W_SIMPLE_CALCULATION_UNDERSAMPLE
#else
 // configuration for high resolution encoders
 // 19 + 1 = 20 -> 20KHz / 20 = 1KHZ for velocity calculation
 #define W_SIMPLE_CALCULATION_UNDERSAMPLE 19 
 // TODO: va scritta i  funzione delle altre variabili!
 #define W_SIMPLE_CALCULATION_DIVISOR 1
#endif

// Complex speed calculation Frequency in Hertz. This value must
// be an integer to avoid pre-compiler error
// note that we need at least 3 point per round with 20Khz pwm. so max is about 6.6K
// for 40Khz Pwm it should be about 8K (five point, as up to 4 points might be repeated)
#define W_COMPLEX_CALCULATION_FREQ  1000      

// Velocity PID loop undersampling, how many current loops for each W PID loop
#define W_PID_UNDERSAMPLE W_SIMPLE_CALCULATION_UNDERSAMPLE // 200

//
// Openloop parameters with a ramping speed
//
// torque generating current
#define OL_VQ         Q15(0.08) //Q15(0.2)
// defluxing current value
#define OL_VD         Q15(0.0)
// starting velocity increase
#define OL_MIN_DELTA_THETA 0
// maximum velocity increase
#define OL_MAX_DELTA_THETA 30
// ramp steepness
#define OL_RAMP_SLOPE      8000
// lower is this number better the resulting resolution
#define OPENLOOP_DEGRADATION 1
//#define OPENLOOP_DEGRADATION 300

//
// Initial Rotor Alignment parameters for IE 
//
// torque generating current
#define IRA_VQ         Q15(0.08)
// defluxing current value
#define IRA_VD         Q15(0.0)

// Current reading strategy:
// since the current sensors are placed on phase X and Z two possibility are available for reading:
// 1) connect X->A, Y->B, Z->C read Ia and Ic and then then calculate Ib
#define     SAMPLE_IA_IC
// 2) connect X->A, Y->C, Z->B read Ia and Ib 
//#define     SAMPLE_IA_IB 
// TODO: NON funziona come dovrebbe, Ialpha, Ibeta sembrano non perfettamente a 90 
//   e la velocita massima raggiungibile sembra piu alta (come se deflussasse)
//   probabilmente qualocsa mi sfugge nella composizione delle fasi motore.
//   in ogni caso il risultato sul rumore in generale non sembra eclatante... 
#ifndef SAMPLE_IA_IC
  #ifndef SAMPLE_IA_IB
    #error Missing DEFINITION SAMPLE_IA_IB/SAMPLE_IA_IC 
  #endif
#endif

//
// ADC Scaling for current reading. 
//
// Scaling constants: Determined by hardware design. 
// +-5 Amps sensor on PhaseA and PhaseC(B), current reading inverted in sign, 
// zero current on halfscale of Allegro ACS714 (2.5V)
// 
// ACS714-05 has a sensitivity 185mV/A, out of linearity range of 5A a 5V output is 
//  near 5[V]/0.185[V/A]=+-13.5[A]!!
//
// ADC voltage divider: 6.3K series 100K//15K = 6.3K series 13.043K -> scale factor = 0.657318
//  then when out = 5V vadc = 5*0.657318 = 3.286V
//
// full scale positive current reading is around 13.5[A], 1/13.5=0.074074
//  -> 32768==13.5A
// e.g the value for 1A reading is 32768:13.5A=x:1A
//
// For the ACS714-10 the sensitivity is 100mV/A
#define     DQKA       Q15(1.0)  // Ia 
#ifdef SAMPLE_IA_IB 
  #define   DQKB       Q15(-1.0) // Ib is sampled . 
#else
  #define   DQKB       Q15(-1.0) // Ib is calculated from Ia and Ic. 
    // due to optimizations in algo inverted sign is required !!!!!
#endif
#define     DQKC       Q15(1.0)  // Ic

// Enable/Disable Overcurrent protection
#define ENABLE_OVER_CURRENT_PROTECTION 

//
// DC-Link parameters
// 
// Undervoltage and Overvoltage values in 100mV units.
// The undervoltage protection will trigger if the VDCLink drops below this value. 
#define UNDERVOLTAGE_THRESHOLD 100 //12V
// The overvoltage detection will trigger if the VDCLink rise above this value. 
#define OVERVOLTAGE_THRESHOLD  600 //60V

//
// I2T protection parameters
//
#define I2T_FILTER_IMPLEMENTATION
//#define I2T_INTEGRATOR1_IMPLEMENTATION
//#define I2T_INTEGRATOR2_IMPLEMENTATION

//
// I2T parameters for INTEGRATOR implementation
//
// I2T Error is issued when the current integration raises over this value 
#define I2T_ERROR_THRESHOLD      0x1500

// I2T does not integrate currents below this
// value. This is the nominal current value
// that windings should tolerate well for an
// unlimited time amount
// This value is in I AD units.
// For example AD reads about 6550 for 5A current,
#define I2T_NOMINAL_IMAX	6550 // for kollmorgen 5A nominal current (5.8 real nominal in datasheet)


//
// I2T parameters for FILTER implementation
//
// This is the current threshold that will cause protection to fire
// This value is in I AD units scaled to 16bit.
// For example AD reads about 6553 for 5A current, 1310 for 1A
#define I2T_CURRENT_THRESHOLD   3310
// This is 2024*K where K is
// K = 1-(e^(-tc/tau))
// where TC is 2FOC loop time
#define I2T_TAU2048 Q15(0.0204)

// I2T warning not implemented yet
// I2T warning is issued over this value
//#define I2T_WARNING_THRESHOLD    170

//CURRENT LIMIT. It is the saturation for the reference of the current expressed in AD reads (1310 for 1A)
#define CURRENT_LIMIT Q15(0.1999)//6553 // for Kolmorgeen  

//
// PWM and Control Timing Parameters
//
// PWM Frequency in Hertz
#ifdef ENCODER_DHES
 // HES can run up to 40KHz
 #define PWMFREQUENCY   40000     
#else
 // Higer resolution encoders
 #define PWMFREQUENCY   20000     
#endif


// Deadtime in seconds (range 1.6 us to 25 ns)
#ifdef ENCODER_DHES
 // HES accept a greater zero cross distortion in order to keep lower temperature
 #define DEADTIMESEC	   0.00000025 
#else
 // hi-res encoders try to minimize zero cross distortion
 #define DEADTIMESEC	   0.00000020 
#endif

// this forces the 2foc interrupt to delay PWM registers update when it is 
// about to do it very close to the PWM hardware register buffering.
// The number represent the estimated clock cycles taken by SVgen to
// update those registers. The call to SVgen is thus delayed if the
// algo foresee that the HW will shadows register during SVGen (in the
// next PWMSAFETIME clock cycles)
#define PWMSAFETIME 50 // svgen takes 34 cycles. be conservative

// PWMmax and PWMmin are PWMGUARD% and (100% - PWMGUARD%)
// for example if PWMGUARD is 2 then max PWM is 98% and min is 2%
#define PWMGUARD 2 

//
// CAN communication parameters
//
// Data rate of outgoing messages in 0.1 millisecond units
#define CAN_OUTPUT_DATARATE (float)2.0
// every CAN_STATE_PER messages a status message is generated
#define CAN_STATUS_MSG_PERIOD 5000
// Enable ack. for CAN set commands
// #define CAN_ACK_EACH_COMMAND
//the board send a periodic msg on received set point current, in oreder to synchronize the communication
#define SYNC_2FOC_TO_EMS


//this macro say how many ems can msg the 2foc must have received before sends a status message
//the ems send a can msg every 1 ms 
#define CAN_STATUS_MSG_PERIOD_SYNC_WITH_EMS  1000 //every 1 sec 2foc send one status message   


//this macro let you to work with 2foc without fault
//#define NO_FAULT
//the ems send desired current in periodic message (the ems can send only one message for all 4 2FOC)
//NOTE: if you use this mode, you must be sure that 2FOC can addres belong to [1, 4].
#define DESIRED_CURR_IN_PER_MSG_BY_EMS          
   

// Enable accept all commands (including setpoints) when in switch off state
#define CAN_CMD_ALWAYS_ACCEPT

// Oscillator Parameters
#define PLLIN          10000000  // External Crystal or Clock Frequency (Hz)
#define DESIREDMIPS	   40000000  // Enter desired MIPS

// number of elements in the list of possible contents of periodic message
#define ELEMENTS_IN_PREIODIC_DATA_LIST 0x30

//number of points acquired in gulp circular buffer.
//used to debug: it will contain last gulp trace before fault
//NOTE FOR OPTIMIZATION PURPOSE THIS MUST BE A POW OF 2
#define GULP_HISTORY_BUFFER_SIZE 512
// this is the bitmask that is used to recirculate the ring buffer
#define GULP_HISTORY_BUFFER_MASK (GULP_HISTORY_BUFFER_SIZE-1)

#endif
