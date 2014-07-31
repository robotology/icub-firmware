/**********************************************************************
 *
 *  Filename:       2FOC.c
 *  Date:           15/11/08
 *  Revision:       P R E L I M I N A R Y
 *
 *  Tools:          MPLAB IDE 8.63
 *                  C30  3.25
 *
 ***********************************************************************
 *  ...creatively derived from Microchip AN1078 source code.
 *  This code uses willingly some discouraged practices (e.g. massive use
 *  of global variables) in order to maximize control loop performance.
 *  If you don't like it try to get a FOC@40KHz rate any other way! (-;
 ***********************************************************************
 *  Description:
 *
 *  Field Oriented Control of 3 phase PMSM/BLDC motors with  position sensors (hes + encoder)
 *  2 Currents are measured and used for current (torque) control loop at maximum speed,
 *  SVM is used as the modulation strategy.
 *
 *  Operating mode can be switched real time between:
 *   open loop free running movment
 *   current (torque) loop
 *   speed loop
 *   Sensored Torque Control Loop with reference given by a torque sensor (strain) LPF filtered
 *   position loop without trayectory generation ( * not * yet * implemented * )
 *   point to point positioner with trayectory generation ( * not * yet * implemented * )
 *
 *  Used resources:
 *   TIMER1 for low frequency operations (LED, togglin' references...)
 *   TIMER2 for speed measurement
 *   TIMER3 for I2T unwind when the board is faulted or shutdown(2FOC loop not running)
 *   TIMER4 for CAN TX
 *   TIMER5 for setpoint watchdog
 *   DMA CHANNEL 0 (ADC)
 *   DMA CHANNEL 1 (CAN Tx)
 *   DMA CHANNEL 2 (CAN Rx)
 *   DMA CAHNNEL 3 (SPI Rx)
 *   CAN1
 *   SPI1, SPI2
 *   QEI
 *   ADC1 (3 channels -iu,-iw, vdclink)
 *   HW CRC calculator (TLE5012)
 *
 *  TODO: la modalita' CAN DMA funziona solamente per la revisione >3 del silicio
 *  TODO:   fare la verifica del silicio nella funzione SiliconRevionTest()
 *  TODO: Fare la lettura del DC-Link in modo acconcio (in un irq)
 *  TODO: speed measurement with IIR filtering?
 *  TODO:   acceleration and d(acceleration)/dt calculation
 *  TODO:   overspeed protection
 *  TODO:   Stall detection and signalling
 *  TODO: Voltage ripple compensation
 *  TODO: usare Ia e Ic per il FOC al posto di Ib calcolata (+rumorosa)
 *  TODO: fare un comando per la visualizzazione dell'offset dei convertitori
 *    MeasCurrParm.Offseta, MeasCurrParm.Offsetbc un errore nella misura dell'offset si
 *    traduce in  cogging a coppia zero (si vede come un offset su Ia o Ib o Ic)
 *  TODO: fare tornare la configurazione via CAN (tipo encoder, risoluzione, tipo di controllo...
 *  TODO: Inserire indirizzo CAN scheda e indirizzo sensore forza/coppia
 *  TODO: verificare periodicamnete la presenza del sensore di coppia
 *  TODO: portare a 0 la coppia in caso di morte improvvisa del sensore di coppia
 *  TODO: guardia CAN
 *  TODO: Trajectory generator and position control!
 *  TODO: mettere una CRC nella EEPROM e fare una verifica CRC durante la
 *          lettura dei parametri
 *	TODO: far configurabili i connettori a cui sono attaccati QE e ABS (come hall)
 *
 **********************************************************************/

#include <p33FJ128MC802.h>
#include <libq.h>
#include <timer.h>
#include <ecan.h>
#include <spi.h>
#include <math.h>
#include <dsp.h>
#include <stdio.h>
#include <libpic30.h>  //__delay32

#include "UserTypes.h"
//#include "Park.h"
#include "MeasCurr.h"
//#include "SVGen.h"
#include "IControl.h"
//#include "pi.h"
#include "Controller.h"
#include "UserParms.h"
#include "ecan.h"
#include "Encoder.h"
#include "System.h"
#include "PWM.h"
#include "ADC.h"
#include "encoder.h"
#include "Status.h"
#include "velocity.h"
#include "EmuROM.h"
#include "TLE5012.h"
#include "I2T.h"
#include "Faults.h"
#include "DCLink.h"
#include "DS402.h"
//#include "rotor_alignment.h"
#include "crc16.h"

#include "can_icubProto.h"
#include "eEbasicStorage.h"
#include "stdint.h"

//
// firmware version related stuff
//

#ifdef FIRMARE_SAFE_COMMAND_SET
#warning compiling_in_SAFE_COMMAND_SET_mode
#else
#warning compiling_in_ADVANCED_COMMAND_SET_mode
#endif

char twofoc_fw_version[16];
char TWOFOC_HW_VERSION[] = HARDWARE_REVISION_STRING;

#define BOARD_CAN_ADDR_DEFAULT 0xE

//
// Configuration bits
//

// no Boot sector and write protection disabled
_FBS(BSS_NO_FLASH & BWRP_WRPROTECT_OFF);

// no secure sector and write protection disaabled
_FSS(RSS_NO_RAM & SSS_NO_FLASH & SWRP_WRPROTECT_OFF);

// Code protection disabled
_FGS(GSS_OFF);

// Clock switching disabled Fail safe Clock Monitor disabled
// Starts from safe internal fast RC
_FOSCSEL(IESO_OFF & FNOSC_FRC);

// Clock switching enabled and clock monitor disabled, EC external clock, OSCOUT as IOPIN
_FOSC(FCKSM_CSECMD & POSCMD_EC & OSCIOFNC_ON)

// Turn off Watchdog Timer
_FWDT(FWDTEN_OFF);
//_FWDT(WDT_ON & WDTPSA_512 & WDTPSB_1); // WD enabled 1:512*16

// PWM mode is Port registers PWM high & low active high
// alternate I2C mapped to SDA1/SCL1
// FPOR power on reset 128ms
_FPOR(PWMPIN_ON & HPOL_ON & LPOL_ON & FPWRT_PWR128);

// Use PGC3/PGD3 for programming and debugging
_FICD(ICS_PGD3 & JTAGEN_OFF); // & COE_ON ); //BKBUG_OFF

//
// Tons of globals
//
#include "globals.h"

void ResetSetpointWatchdog(); //prototype

#if defined SIX_STEP

// DMA0 IRQ Service Routine used for FOC loop

void __attribute__((__interrupt__, no_auto_psv)) _DMA0Interrupt(void)
{
    static const int PWM_CENTER = LOOPINTCY / 2;
    //static const int PWM_MAX = (18*PWM_CENTER)/20; // 25%
    static const int PWM_MAX = PWM_CENTER / 4;

    static int *ppwmH = NULL, *ppwmL = NULL, *ppwm0 = NULL;
    static short *Isens = NULL,Isign = 0;

    // setting CORCON in this way rather than using CORCONbits is
    // slightly more efficient (because CORCONbits is volatile and
    // optimizer cannot collapse multiple bit operations)

    CORCON = 0b11110100;

    updateOdometry();

    // read and compensate ADC offset by MeasCurrParm.Offseta, Offsetb, Offsetc
    // scale currents by MeasCurrParm.qKa, qKb, qKc
    // Calculate ParkParm.qIa, qIb, qIc
    MeasAndCompIaIcCalculateIb();

    static char sector_stored = 0;
	static int base = 0;

    static const int ENC_PULSE_PER_SECTOR = QE_PPR/6;

    static bool align_in_progress = True;

    static int hes_state_stored = 0;

    /*
    static char ncross = 0;

    if (g_hes_state == 0b001 && hes_state_stored == 0b101) // from 6 to 1
    {
        ++ncross;
        
        if (ncross == 4)
        {
            ncross = 0;
            zero_crossed = True;
            POSCNT = 0;
        }
    }
    else if (g_hes_state == 0b101 && hes_state_stored == 0b001) // from 1 to 6
    {
        --ncross;

        if (ncross == -1)
        {
            ncross = 3;
            zero_crossed = True;
            POSCNT = MAXCNT;
        }
    }
    */

    hes_state_stored = g_hes_state;

    int hall_sector = 0;

        switch (g_hes_state)
        {
            case 0b001: // Sector 1: (0,0,1)   60-120 degrees
				hall_sector = 1;
				break;

            case 0b011: // Sector 2: (0,1,1)  120-180 degrees 
				hall_sector = 2;
				break;

            case 0b010: // Sector 3: (0,1,0)  180-240 degrees 
				hall_sector = 3;
				break;
		
            case 0b110: // Sector 4: (1,1,0)  240-300 degrees
				hall_sector = 4;
				break;

            case 0b100: // Sector 5: (1,0,0)  300-360 degrees 
				hall_sector = 5;
				break;

            case 0b101: // Sector 6: (1,0,1)    0- 60 degrees 
				hall_sector = 6;
				break;
        }

    unsigned int uenc = QEPGet();

    static const int ENC_OFFSET = 1500;

	unsigned int enc_raw = ENC_OFFSET + uenc;

    if (enc_raw >= QE_INDEX_EVERY_X_PULSES) enc_raw-=QE_INDEX_EVERY_X_PULSES;

    int enc = (int)enc_raw;

    if (align_in_progress)
    {
        if (uenc < QE_INDEX_EVERY_X_PULSES)
        {
            align_in_progress = False;
        }
        else
        {
            static int enc_fake = 0;

            if (enc_fake <14399) ++enc_fake; else enc_fake = 0;

            enc = enc_fake;
        }
    }

    enc %= QE_PPR;

    char sector = 1 + enc/ENC_PULSE_PER_SECTOR;

    if (sector_stored != sector)
    {
        sector_stored = sector;

        switch (sector)
        {
            case 1: // Sector 1: (0,0,1)   60-120 degrees
                Isens = &(ParkParm.qIa); Isign = 1;
                ppwmH = (int*) &PDC1; ppwmL = (int*) &PDC2; ppwm0 = (int*) &PDC3; 
				base = 0;
				break;

            case 2: // Sector 2: (0,1,1)  120-180 degrees
                Isens = &(ParkParm.qIa); Isign = 1;
                ppwmH = (int*) &PDC1; ppwm0 = (int*) &PDC2; ppwmL = (int*) &PDC3;
				base = ENC_PULSE_PER_SECTOR;
				break;

            case 3: // Sector 3: (0,1,0)  180-240 degrees
                Isens = &(ParkParm.qIc); Isign = -1;
                ppwm0 = (int*) &PDC1; ppwmH = (int*) &PDC2; ppwmL = (int*) &PDC3;
				base = 2*ENC_PULSE_PER_SECTOR;
				break;
		
            case 4: // Sector 4: (1,1,0)  240-300 degrees
                Isens = &(ParkParm.qIa); Isign =-1;
                ppwmL = (int*) &PDC1; ppwmH = (int*) &PDC2; ppwm0 = (int*) &PDC3;
				base = 3*ENC_PULSE_PER_SECTOR;			
				break;

            case 5: // Sector 5: (1,0,0)  300-360 degrees
                Isens = &(ParkParm.qIc); Isign = 1;
                ppwmL = (int*) &PDC1; ppwm0 = (int*) &PDC2; ppwmH = (int*) &PDC3;
				base = 4*ENC_PULSE_PER_SECTOR;
				break;

            case 6: // Sector 6: (1,0,1)    0- 60 degrees
                Isens = &(ParkParm.qIc); Isign = 1;
                ppwm0 = (int*) &PDC1; ppwmL = (int*) &PDC2; ppwmH = (int*) &PDC3;
				base = 5*ENC_PULSE_PER_SECTOR;
				break;
        }
    }

    int pwm = 0;
    
    if (align_in_progress)
    {
        pwm = (PWM_CENTER >> 4);
    }
    else if (SysStatus.OpenLoop) // OPEN LOOP
    {
        //pwm = CtrlReferences.qIqRef >> 5;

        pwm = (PWM_CENTER >> 4);
    }
    else if (SysStatus.SpeedControl)
    {
        int speed_error = VelocityParm.Velocity - CtrlReferences.qIqRef;

        static int speed_error_int = 0;

        speed_error_int += speed_error;

        if (speed_error_int > 5000) speed_error_int = 5000; else if (speed_error_int < -5000) speed_error_int = -5000;

        pwm = speed_error / 2 + speed_error_int / 200;
    }

    if (pwm >  PWM_MAX) pwm =  PWM_MAX; else if (pwm < -PWM_MAX) pwm = -PWM_MAX;

    // if we are close enough to the PCD register shadow copy then wait until
    // its done before updating local PDCs
    // The condition will became false when the PWM is counting down but it is still
    // far from 0 or when it change direction counting up

    // reset the flag that indicates if the PWM registers update was delayed
    bool pwm_delayed = False;

    // reset timeout counter that is used to wait for the PWM registers update
    int timeout = PWMSAFETIME;

    //     PWM count DN               PWM count < PWM_SAFE_TIME
    while ((P1TMR & 0x8000) && ((P1TMR & 0x7fff) < PWMSAFETIME))
    {
        pwm_delayed = True;

        if (!--timeout)
        {
            SysError.FirmwarePWMFatalError = 1;
            //FaultConditionsHandler();
            break;
        }
    }

    if (pwm_delayed)
    {
        // if the PWM PDC update has been delayed, then increment
        // the counter that keeps track of this event, but be
        // careful not to overflow it
        if (FirmwarePWMTimingErrorCount < 0xFF)
        {
            ++FirmwarePWMTimingErrorCount;
        }
    }

    static const int cos_table[] = {32767,32763,32748,32723,32688,32643,32588,32523,32449,32364,32270,32165,32051,31928,31794,31651,31498,31336,31164,30982,30791,30591,30381,30163,29935,29697,29451,29196,28932,28659,28377};
    static const int sin_table[] = {    0,  330,  660,  990, 1319, 1648, 1977, 2305, 2632, 2959, 3285, 3609, 3933, 4255, 4576, 4896, 5214, 5531, 5846, 6159, 6470, 6779, 7087, 7392, 7694, 7995, 8293, 8588, 8881, 9171, 9459};

	//static const int cos_table[] = {32767,32762,32744,32714,32672,32618,32552,32474,32384,32283,32169,32044,31907,31759,31599,31427,31243,31049,30843,30625,30397,30157,29906,29644,29372,29088,28795,28490,28175,27850,27514}; 
	//static const int sin_table[] = {    0,  362,  723, 1085, 1446, 1807, 2167, 2526, 2885, 3242, 3598, 3953, 4306, 4658, 5008, 5356, 5702, 6046, 6388, 6728, 7065, 7399, 7731, 8060, 8386, 8709, 9029, 9345, 9658, 9968,10274};

	bool sine_sign_change = sector%2;

	int delta = enc-base-ENC_PULSE_PER_SECTOR/2;
    
    static const int DIVISOR=ENC_PULSE_PER_SECTOR/60;
	delta /= DIVISOR;

    //delta /= 6;

	int index = delta;

	if (index<0)
	{
		index = -index;
		sine_sign_change = !sine_sign_change;
	}

	if (index>30) index = 30;

	//index = 0;

	int cosD=(int)(__builtin_mulss(pwm,cos_table[index])>>15);
	int sinD=(int)(__builtin_mulss(pwm,sin_table[index])>>15);

	if (sine_sign_change) sinD=-sinD;

	*ppwmH = PWM_CENTER +cosD -sinD;
    *ppwm0 = PWM_CENTER +sinD +sinD;
    *ppwmL = PWM_CENTER -cosD -sinD;

    I2Tdata.IQMeasured = I2Tdata.IDMeasured = Isign * (*Isens);

    // Invoke the I2T integrator with new current values
    if (I2T(&I2Tdata))
    {
        //The I2T grew too much. Protect!
        SysError.I2TFailure = 1;
        FaultConditionsHandler();
    }

    // when gulp_update_request is == 1, the main loop will not access those vars.
    if (gulp_update_request) // updateGulp();
	{
        Gulp.W[0] = sector;
        Gulp.W[1] = delta;
        Gulp.W[2] = hall_sector;
        Gulp.W[3] = enc_raw;  
        // unlock the main loop, so it will read values just updated
        gulp_update_request = 0;

        // read VDC link raw value;
        VDCLink = ADCGetVDCLink();
	    //////////////////////////
	}

    //Clear the DMA0 Interrupt Flag
    IFS0bits.DMA0IF = 0;
}

#endif

void updateGulp(void)
{
    if (!gulp_update_request) return;

    Gulp.W[0] = *gulpadr1;
    Gulp.W[1] = *gulpadr2;
    Gulp.W[2] = *gulpadr3;
    Gulp.W[3] = *gulpadr4;

    // unlock the main loop, so it will read values just updated
    gulp_update_request = 0;

    // read VDC link raw value;
    VDCLink = ADCGetVDCLink();
}

bool updateOdometry(void)
{
    bool zero_crossed = False;
    // read Meccanical postion [-1..0.9999] and angle corrected with encsyncpulsepos
    // calculate AlignedMecchanicalAngle and UnalignedMecchanicalAngle from raw encoder data
    encoder_value = EncoderPosition(0);

#ifdef ENCODER_ENABLE_AUX
    static const int UNDERSAMPLING = PWMFREQUENCY / 1000;
    static long position_old = 0;
    static int speed_undersampler = 0;

    encoder_aux_value = EncoderPosition(1);

    long position = (int) encoder_aux_value;

    long delta = position - position_old;

    position_old = position;

    if (delta < -32000)
    {
        delta += 65536L;
        zero_crossed = True;
    }
    else if (delta >  32000)
    { 
        delta -= 65536L;
        zero_crossed = True;
    }
    
    Reported_position += delta;

    if (++speed_undersampler == UNDERSAMPLING) // we obtain ticks per ms
    {
        static long Reported_position_old = 0;

        speed_undersampler = 0;

        VelocityParm.Velocity = Reported_position - Reported_position_old;

        Reported_position_old = Reported_position;
    }
#else
    // to make it possible to avoid some #ifdef in the code.
    // when there is no AUX encoder emulate it with the same value of
    // the reference encoder. In this way some portions of code
    // can use the encoder aux value without worrying if it
    // really exists or not..
    encoder_aux_value = encoder_value;
#endif

    return zero_crossed;
}

/*
void UpdatePositionVariables(SFRAC16 encoder_value)
{
    //used by 2foc loop and by timer3 to update the local position/turn
    //variables when a new encoder value is read

    // detects and count complete rounds.
    // TODO: what happen if even the 32 bit counter overflows?
    if ((unsigned int) encoder_value > POSITION_TURN_DETECT_THRESHOLD && Previous_encoder_value < 65535 - POSITION_TURN_DETECT_THRESHOLD)
    {
        // the position was close to its min and now it is jumped near to its max value. Decrease turns counter

        Turns--;
    }
    else
    {

        if ((unsigned int) encoder_value < 65535 - POSITION_TURN_DETECT_THRESHOLD && Previous_encoder_value > POSITION_TURN_DETECT_THRESHOLD)
        {

            // the position was close to its max and now it is jumped near to its min value. Increase turns counter
            Turns++;
        }
    }

    Previous_encoder_value = encoder_value;

    // Compose the final Current_position var by merging lower word and hiword with
    // encoder value (normalized to 16bits) and turns counter
    Current_position = (unsigned int) encoder_value;
    Current_position += ((long) Turns)* 0x10000;

    // this is the position reported to outside.
    // it includes the sum with the position offset
    // coming from the CAN command
    Reported_position = Current_position - Offset4Zeroing;
}
*/

// When the 2foc loop is not running, the aux service timer
// (hw timer3) will avoid starving of I2T and Encoder services.
// I2T shall continue to discharge integrated data, and
// Encoder service shall mantain turn count in case of
// external torques rotates the axis.

void EnableAuxServiceTimer()
{

    // Since the Encoder module is timing-sensitive (when SPI encoders
    // are in use), we must avoid to fire any requesto to the
    // Encoder module too eary
    EncoderWaitUpdate();


    // Enable the aux timer
    Timer3Enable();

}

// When the 2FOC loop is enabled, all core services shall
// run on its behalf.
// In particular I2T and Encoder services are moved from the
// aux timer to the 2FOC loop context.

void DisableAuxServiceTimer()
{
    // do not reschedule the aux timer anymore..
    Timer3Disable();

    // The Encoder module is timing-sensitive (expecially when SPI encoders
    // are in use), and its APIs must be called with care.
    // Because the aux timer may just enqueued a request to the encoder
    // layer, we must avoid to fire any other request to the Encoder module
    // too eary (and if we aren't careful the 2FOC loop will..).
    // For this reason we don't yield the CPU to the caller until all
    // encoder tasks are finished.
    // Please note that encoder tasks that have been already started on
    // whathever context (2FOC or timer) are eventually completed on the
    // SPI/DMA irq context, thus they will complete even if neither
    // 2FOC loop and aux timer are running.
    EncoderWaitUpdate();

}

void SiliconRevionTest()
// checks for proper silicon revision
{
    unsigned int devrev;

    // read silicon chip revision
    _memcpy_p2d16(&devrev, 0xff0002, sizeof (devrev));

    if (devrev != 0x3003)
    {

        // TODO: trigger fault really ?

        SysError.SiliconRevisionFault = 1;
    }
}

void EncoderSelfTest()
// encoder selftest (when possible)
{
    // TODO: giustappunto.

}

void ADCDoOffsetTest()
// ADC Offset test
{
    // TODO: giustappunto.
    // A questo punto la calibrazioni e stata fatta
    // Se e' fallita e noi qui resettiamo il flag allora
    // non sapremo mai se e' fallita!!
    // SysError.ADCCalFailure = 0;
}

void ZeroControlReferences()
// zero control set-point
{
    // disable toggling references
    TogglingEnable = 0;
    OmegaTogglingReference = Q15(0.0);
    TorqueTogglingReference = Q15(0.0);

    // Id (flux) Iq (torque) reference values
    CtrlReferences.qIdRef = Q15(0.0);
    // this parameter controls the startup torque reference
    CtrlReferences.qIqRef = Q15(0.0);

    // Omega setpoint
    CtrlReferences.qWRef = Q15(0.0);

    // TODO: zero position and sensored torque references
}

void DriveInit()
// Perform drive SW/HW init
{


    // IIR filters initialization
    iirt.numSectionsLess1 = 2; // Nbq - 1
    iirt.coeffsBase = &IirTransposedCoefs[0];
    iirt.coeffsPage = COEFFS_IN_DATA;
    iirt.delayBase1 = &IirTransposedState1[0];
    iirt.delayBase2 = &IirTransposedState2[0];
    iirt.finalShift = 0;
    // Zero the filter state
    IIRTransposedInit(&iirt);

    I2TInit(&I2Tdata);
    I2Tdata.Acc[0] = 0;
    I2Tdata.Acc[1] = 0;
    I2Tdata.Acc[2] = 0;

    // setup for a center aigned PWM
    SetupPWM();

    // setup and perform ADC offset calibration in MeasCurrParm.Offseta and Offsetb
    ADCDoOffsetCalibration();

    // Enable DMA interrupt, arm DMA for transfer
    ADCConfigPWMandDMAMode();

    // setup the position sensor
    EncoderInit();

    // init total number of turns
    Turns = 0;

    // initialize parameters for velocity calculation
    // encodersyncpulseposition is set to 0
    // so the position buffer must be initialized with the initial position of the encoder
    VelocityInit();

    // Setup timer for velocity calculation and velocity control loop
    Timer2Config();

    // Setup timer for CAN periodic send
    Timer4Config();

    // setup Hw related parameters (current gain, ecc...)
    SetupHWParameters();

    // init PIDs and references
    SetupControlParameters();

    // define default control strategy *only*one*can*be*activated*
    SysStatus.OpenLoop = 1;
    //SysStatus.TorqueControl = 1;
    //SysStatus.TorqueSensorLoop = 1;
    //SysStatus.SpeedControl = 1;
    //SysStatus.PositionControl = 1;

    // zero control set-point
    ZeroControlReferences();

    // Enables the external fault (pushbutton) interrupt
    // conditioned to PIN_RA3_IS_DEBUG
    ExternalFaultEnable();


    // Init timer for setpoint timeout.
    // Note: the timer has to be configured *after*
    // loading eeprom, otherwise the timeout value
    // to set in the timer registers is not known..
    Timer5Config();
}

void ResetSetpointWatchdog()
{
    WriteTimer5(0);
    IFS1bits.T5IF = 0;
}

void DriveSelfTest()
// Perform drive HW selftest
{

    // Check silicon revion
    //SiliconRevionTest();

    // Selftest EMUROM
    //EepromTest();

    // Selftest EMUROM
    EncoderSelfTest();

    // Test ADC offset
    ADCDoOffsetTest();

    // TODO: vedere se possibile verificare l'oscillatore e la verifica di porte di IO
}

void ApplyBrake()
// 2FOC doesn't have a brake unit
//TODO: if we want to keep this stub, then we should add some other
//stub to release brake!
{
}

void ApplyVDCLink()
// 2FOC doesn't have a switchable VDCLink
{
    DS402_Statusword.Flags.VoltageEnabled = 1;
}

void RemoveVDCLink()
// 2FOC doesn't have a switchable VDCLink
{
    DS402_Statusword.Flags.VoltageEnabled = 0;
}

void DisableDrive()
// disable the drive functions:
// - disable and zero control reference and status
// - disable PWM generators
// - disable FOC loop
{
    // disable PWM
    PWMDisable();

    // Disable ADC IRQ and DMA
    ADCInterruptAndDMADisable();

    // I2T and encoder traking will run in behalf of timer 3 when 2FOC loop is not active
    EnableAuxServiceTimer();

    // zero torque and speed referemces
    ZeroControlReferences();
    // Zero regulators
    ZeroRegulators();
}

int main(void)
//
// drive functions are controlled according to DS402 standard (when possible)
//
{
    uint32_t boardCanAddr;

    char fw_minor[16];
    // Configure Oscillator Clock Source, PLL
    oscConfig();

    // set up port direction and peripheral pin mapping
    SetupPorts();

    // do not leave it in random state.. until DS402 state machine starts
    LED_status.RedBlinkRate = BLINKRATE_OFF;
    // do not leave it in random state.. until DS402 state machine starts
    LED_status.GreenBlinkRate = BLINKRATE_ULTRAFAST;

    // Interrupt Priority configuration
    InterruptPriorityConfig();

    // enable change notification INT.
    // this is shared for external fault and
    // QEP rotor alignment when zero signal is used in interrupt mode.
    IEC1bits.CNIE = 1;

    // Setup timer for low frequency (LED) operations
    Timer1Config();
    // Setup timer for I2T (when 2foc loop is not running)
    Timer3Config();
    // enable timer1 interrupt
    EnableIntT1;

    // Retrive data from emurom FLASH page and copy in local RAM buffer.
    // CRC and other sanity check are done in DriveSelfTest.
    // Actual interpretation is done in DriveInit
    //ReadFromEmuROM();

    // build up the firmware string version
    // that will be returned over CAN
    strcpy(twofoc_fw_version, FIRMWARE_VERSION_PREFIX);
    // switched from itoa to sprintf to allow fixed len
    // with leading zeroes. NOTE: heap has been added to
    // linker options
    // was: itoa(fw_minor,FIRMWARE_VERSION_MINOR,10);
    sprintf(fw_minor, "%02d", FIRMWARE_VERSION_MINOR);
    strcat(twofoc_fw_version, fw_minor);

    // Reset all system error flags
    memset(SysError.b, 0, sizeof (SysError));

    // Avoid leaving any random garbage in SysStatus
    memset(SysStatus.b, 0, sizeof (SysStatus));

    //ee_basicstrg_init();

notrreadytoswitchon:
    // DS402 State xxxx xxxx x0xx 0000 NOT READY TO SWITCH ON:
    // Logic power has been applied to the drive. The drive is being initialized or is running self test.
    // A brake, if present, has to be applied in this state. The drive function is disabled.

    // already initialized as NOT READY TO SWITCH ON:
    // DS402_Statusword.Flags.ReadyToSwitchOn = 0;

    // start:
    //   DS402SM.statusword = DS402_START;

    // • State Transition 0: START => NOT READY TO SWITCH ON
    //   Event: unconditional.
    //   Action: The drive self-tests and/or self-initializes.

    // if available apply braking
    ApplyBrake();

    // signal NOT READY TO SWITCH ON status with green LED blinking ultrafast
    // LED blinking velocity
    LED_status.GreenBlinkRate = BLINKRATE_ULTRAFAST;

    // Drive Init
    DriveInit();

    // Drive selftest
    DriveSelfTest();

    // load eprom data in control variables
    //EepromLoad();

    // Test error flags and if everything is OK change status
    if (Fault())
    {
        // at this stage the error is unrecoverable and communication isn't on yet
        // so signal with led (red ON green Blinking madly)
        LED_status.RedBlinkRate = BLINKRATE_STILL;

        // retry the init/selftest phase
        goto notrreadytoswitchon;
    }

    LED_status.RedBlinkRate = BLINKRATE_OFF;
    // • State Transition 1: NOT READY TO SWITCH ON => SWITCH ON DISABLED
    //   Event: The drive has self-tested and initialized successfully.
    //   Action: Activate communication.

switchondisabled:
    // DS402 State xxxx xxxx x1xx 0000 SWITCH ON DISABLED:
    // Drive initialization is complete, the drive parameters have been set up.
    // Drive parameters may be changed. High voltage may not be applied to the drive,
    // (2FOC cannot be completely DS402 compliant 'couse doesn't have
    //  separate logic/power supplies). The drive function is disabled.

    // update status
    DS402_Statusword.Flags.SwitchOnDisabled = 1;
    DS402_Statusword.Flags.ReadyToSwitchOn = 0;
    DS402_Statusword.Flags.SwitchedOn = 0;
    DS402_Statusword.Flags.OperationEnabled = 0;

    // LED blinking velocity
    LED_status.GreenBlinkRate = BLINKRATE_ULTRAFAST;

    if (ee_basicstrg_res_ok != ee_basicstrg_canaddress_get(&boardCanAddr))
    {
        //should never happen!!!
        boardCanAddr = BOARD_CAN_ADDR_DEFAULT;
    }

    CanIcubProtoInit(boardCanAddr);

#ifdef EEPROM_CRC_CALCULATE_AND_TX_ON_CAN
    tCanData candata;
    candata.w[0] = ApplicationData.app_data_crc16;
    ECANSend(0x123, 2, &candata);
    while (1);
#endif

    ///////////////////////////////////////////////////////////////////////////////////
    // TODO: verificare la corretta inizializzazione della comunicazione e
    //   nel caso non cambiare stato ma segnalare con i led
    if (0) // TODO: il CAN e' su?
    {
        // at this stage the error is unrecoverable and communication isn't on yet
        // so signal with led (red ON green Blinking madly)
        LED_status.RedBlinkRate = BLINKRATE_FAST;

        // retry the CAN initializzation
        goto switchondisabled;
    }
    ///////////////////////////////////////////////////////////////////////////////////

    // • State Transition 2: SWITCH ON DISABLED => READY TO SWITCH ON
    // Event: Communiction activated or 'Shutdown' command received from host.
    // Action: None

readytoswitchon:
    // DS402 State xxxx xxxx x01x 0001 READY TO SWITCH ON:
    // High voltage may be applied to the drive. The drive parameters may be changed.
    // The drive function is disabled.

    // update status
    DS402_Statusword.Flags.SwitchOnDisabled = 0;
    DS402_Statusword.Flags.ReadyToSwitchOn = 1;
    DS402_Statusword.Flags.SwitchedOn = 0;
    DS402_Statusword.Flags.OperationEnabled = 0;

    // LED blinking velocity
    LED_status.GreenBlinkRate = BLINKRATE_FAST;

    DS402_Controlword.Flags.SwitchOn = 0;
    DS402_Controlword.Flags.EnableVoltage = 0;
    DS402_Controlword.Flags.EnableOperation = 0;

    EnableIntT4;

    // Stays in READY TO SWITCH ON until the command
    // SwitchOn received from CAN
    while (0 == DS402_Controlword.Flags.SwitchOn)
    {
        if (!Fault()) LED_status.GreenBlinkRate = BLINKRATE_FAST;
    }

    // • State Transition 3: READY TO SWITCH ON => SWITCHED ON
    // Event: 'Switch On' command received.
    // Action: The power section is switched on if it is not already switched on.

switchedon:
    // DS402 Status xxxx xxxx x01x 0011 SWITCHED ON:
    // High voltage has been applied to the drive. The power amplifier is ready.
    // The drive parameters may be changed. The drive function is disabled.

    // update status
    DS402_Statusword.Flags.SwitchOnDisabled = 0;
    DS402_Statusword.Flags.ReadyToSwitchOn = 0;
    DS402_Statusword.Flags.SwitchedOn = 1;
    DS402_Statusword.Flags.OperationEnabled = 0;

    LED_status.GreenBlinkRate = BLINKRATE_NORMAL;

    // (fake) enable VDCLink
    ApplyVDCLink();
    DS402_Controlword.Flags.EnableVoltage = 1;

    // Stays in SWITCHED ON until the command
    // EnableOperation or Shutdown received from CAN
    // TODO: conform coding style
    while (1)
    {

        // TODO: conform coding style
        // TODO: comment please
        while ((0 == DS402_Controlword.Flags.EnableOperation) && (1 == DS402_Controlword.Flags.EnableVoltage))
        {
            ;
        }

        // TODO: comment please
        if (0 == DS402_Controlword.Flags.EnableVoltage)
        {
            // • State Transition 6: SWITCHED ON => READY TO SWITCH ON
            // Event: 'Shutdown' command received from host.
            // Action: The power section is switched off.
            DS402_Controlword.Flags.SwitchOn = 0;

            // reset the DC link reading to a good value:
            // it will be overwritten with actual value as soon
            // as the first measurement will be ready.
            DCLinkReset();

            // reset faults
            FaultReset();

            goto readytoswitchon;
        }

        // do not switch to operational until dclink is ok
        if (0 != SysError.OverVoltageFailure || 0 != SysError.UnderVoltageFailure)
        {
            continue;
        }

        // do not switch to operational until emergency button is pressed!
        if (ExternaFaultIsAsserted())
        {
            FaultExternalTriggered();
            continue;
        }

        // do not switch to operational until the overcurrent fault has been cleared (shutdown)
        if (0 != SysError.OverCurrentFailure)
        {
            continue;
        }

        // The board is in un-clearable, un-recoverable fault, then fault again
        if ((SysError.W[0] != 0) || (SysError.W[1] != 0))
        {
            FaultConditionsHandler();
        }

        break;
    }
    // • State Transition 4: SWITCHED ON => OPERATION ENABLE
    // Event: 'Enable Operation' command received from host.
    // Action: The drive function is enabled.

    //operationenabled:
    // DS402 Status xxxx xxxx x01x 0111 OPERATION ENABLE:
    // No faults have been detected. The drive function is enabled and power is applied to the motor.
    // The drive parameters may be changed. (This corresponds to normal operation of the drive.)

    // update status
    DS402_Statusword.Flags.SwitchOnDisabled = 0;
    DS402_Statusword.Flags.ReadyToSwitchOn = 0;
    DS402_Statusword.Flags.SwitchedOn = 0;
    DS402_Statusword.Flags.OperationEnabled = 1;

    LED_status.GreenBlinkRate = BLINKRATE_STILL;

    /////////////////////////////////////////////////////////////////////////////////
    // TODO: questa sezione di codice è fatta di merda.
    //
    // init variables for speed calc and current velocity to 0
    VelocityParm.Velocity = 0;

#if 0
    CtrlReferences.qWRef = Q15(0.0);
    PIParmW.qdSum = 0;
    InitPI(&PIParmW);
#endif
    /////////////////////////////////////////////////////////////////////////////////

    ZeroRegulators();

    // enable timer2 interrupt for speed calc
    EnableIntT2;

    // Arm all the automation (PWM->ADC->DMA->FOC) but don't start yet!
    ADCInterruptAndDMAEnable();

#if 0
    // if rotor is not aligned perform alignment (only once each turn on)
    if (0 == SysStatus.InitialRotorAlignmentComplete)
    {
        // Align angle 0 with rotor flux maximum
        RotorAlignStart();
        // TODO: detect and signal a failed rotor alignment
    }
#endif

    // TODO: enable overcurrent fault during rotor allignment
    // enable the overcurrent interrupt
    OverCurrentFaultIntEnable();

    // I2T will run on behalf of 2FOC loop. Stop running it in behalf of Timer 3
    DisableAuxServiceTimer();
    // Starts to pump current!
    PWMEnable();

    FaultRecheck();
    while (1)
    {
        // TODO: rimuovere le n variabili relative alla tensione
        DCLinkCheck();

        // • State Transition 5: OPERATION ENABLE => SWITCHED ON
        // Event: 'Disable Operation' command received from host.
        // Action: The drive operation will be disabled.

        if (0 == DS402_Controlword.Flags.EnableOperation)
        {
            // Disable drive functions
            DisableDrive();

            // change status
            // reset the DC link reading to a good value:
            // it will be overwritten with actual value as soon
            // as the first measurement will be ready.
            DCLinkReset();

            // reset faults
            FaultReset();
            //goto switchedon;
            goto switchedon;
        }

        // • State Transition 8: OPERATION ENABLE => READY TO SWITCH ON
        // Event: 'Shutdown' command received from host.
        // Action: The power section is switched off immediately, and the motor is free to rotate if
        // unbraked.

        if (0 == DS402_Controlword.Flags.EnableVoltage)
        {
            // Disable drive functions
            DisableDrive();

            // change status
            // reset the DC link reading to a good value:
            // it will be overwritten with actual value as soon
            // as the first measurement will be ready.
            DCLinkReset();

            // reset faults
            FaultReset();

            goto readytoswitchon;
        }

        // • State Transition 9: OPERATION ENABLE ? SWITCH ON DISABLED
        // Event: 'Disable Voltage' command received from host.
        // Action: The power section is switched off immediately, and the motor is free to rotate if
        // unbraked.
        // This status change is UNIMPLEMENTED for it has almost the same results as transiction 8
        // ( 2FOC board has no internal DCLink switch available!)
    };

    // should never arrive here!
    while (1);

    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");

}

/*

• State Transition 7: READY TO SWITCH ON ? SWITCH ON DISABLED
Event: 'Quick Stop' and ‘Disable Voltage’ command received from host.
Action: None

• State Transition 10: SWITCHED ON ? SWITCH ON DISABLED
Event: 'Disable Voltage' or 'Quick Stop' command received from host.
Action: The power section is switched off immediatly, and the motor is free to rotate if
unbraked.
DEVICE CONTROL Drives and Motion Control CiA DSP 402 V 2.048

• State Transition 11: OPERATION ENABLE ? QUICK STOP ACTIVE
Event: 'Quick Stop' command received from host.
Action: The quick stop function is executed.

• State Transition 12: QUICK STOP ACTIVE ? SWITCH ON DISABLED
Event: 'Quick Stop' is completed or 'Disable Voltage' command received from host.
This transition is possible, if the Quick-Stop-Option-Code is different 5 (stay in the state ‘Quick
Stop Active’).
Action: The power section is switched off.

• State Transition 13: All states ? FAULT REACTION ACTIVE
A fault has occurred in the drive.
Action: Execute appropriate fault reaction.

• State Transition 14: FAULT REACTION ACTIVE ? FAULT
Event: The fault reaction is completed.
Action: The drive function is disabled. The power section may be switched off.

• State Transition 15: FAULT ? SWITCH ON DISABLED
Event: 'Fault Reset' command received from host.
Action: A reset of the fault condition is carried out if no fault exists currently on the drive.
After leaving the state Fault the Bit 'Fault Reset' of the controlword has to be cleared by the
host.

• State Transition 16: QUICK STOP ACTIVE ? OPERATION ENABLE
Event: 'Enable Operation' command received from host. This transition is possible if the
Quick-Stop-Option-Code is 5, 6, 7 or 8 (? Chapter 10.3.5).
Action: The drive function is enabled.
 */

