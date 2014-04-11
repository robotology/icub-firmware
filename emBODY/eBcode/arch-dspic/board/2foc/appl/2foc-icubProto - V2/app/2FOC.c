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
    extern tPID2 CurrentQPID;

    static const int PWM_CENTER = LOOPINTCY / 2;
    static const int PWM_MAX = (19*PWM_CENTER)/20; // 95%
    //static const int PWM_MAX = 475;

    static int hes_state_stored = 0;
    static int *ppwmH = NULL, *ppwmL = NULL, *ppwm0 = NULL;
    static int *piH = NULL, *piL = NULL;
    static int iHfilt = 0, iLfilt = 0;

    int pwmH, pwmL;

    // setting CORCON in this way rather than using CORCONbits is
    // slightly more efficient (because CORCONbits is volatile and
    // optimizer cannot collapse multiple bit operations)

    CORCON = 0b11110100;

    updateOdometry();

    // read and compensate ADC offset by MeasCurrParm.Offseta, Offsetb, Offsetc
    // scale currents by MeasCurrParm.qKa, qKb, qKc
    // Calculate ParkParm.qIa, qIb, qIc
    MeasAndCompIaIcCalculateIb();

    if (SysStatus.OpenLoop) // OPEN LOOP
    {
        if (hes_state_stored != g_hes_state)
        {
            hes_state_stored = g_hes_state;

            switch (g_hes_state)
            {
            case 0b001: // Sector 1: (0,0,1)   60-120 degrees
                ppwmH = (int*) &PDC1; ppwmL = (int*) &PDC2; ppwm0 = (int*) &PDC3; break;

            case 0b011: // Sector 2: (0,1,1)  120-180 degrees
                ppwmH = (int*) &PDC1; ppwm0 = (int*) &PDC2; ppwmL = (int*) &PDC3; break;

            case 0b010: // Sector 3: (0,1,0)  180-240 degrees
                ppwm0 = (int*) &PDC1; ppwmH = (int*) &PDC2; ppwmL = (int*) &PDC3; break;

            case 0b110: // Sector 4: (1,1,0)  240-300 degrees
                ppwmL = (int*) &PDC1; ppwmH = (int*) &PDC2; ppwm0 = (int*) &PDC3; break;

            case 0b100: // Sector 5: (1,0,0)  300-360 degrees
                ppwmL = (int*) &PDC1; ppwm0 = (int*) &PDC2; ppwmH = (int*) &PDC3; break;

            case 0b101: // Sector 6: (1,0,1)    0- 60 degrees
                ppwm0 = (int*) &PDC1; ppwmL = (int*) &PDC2; ppwmH = (int*) &PDC3; break;
            }
        }

        pwmH = CtrlReferences.qIqRef >> 6;

        if (pwmH >  PWM_MAX) pwmH =  PWM_MAX;
        else 
        if (pwmH < -PWM_MAX) pwmH = -PWM_MAX;

        pwmL = pwmH;
    }
    else if (SysStatus.TorqueControl) // start TORQUE CONTROL
    {
        static int KiErrIntH = 0, KiErrIntL = 0;

        int Err;

        if (hes_state_stored != g_hes_state)
        {
            hes_state_stored = g_hes_state;

            KiErrIntH = KiErrIntL = 0;

            switch (g_hes_state)
            {
            case 0b001: // Sector 1: (0,0,1)   60-120 degrees
                piH = (int*) &(ParkParm.qIa); piL = (int*) &(ParkParm.qIb);
                ppwmH = (int*) &PDC1; ppwmL = (int*) &PDC2; ppwm0 = (int*) &PDC3;
                break;

            case 0b011: // Sector 2: (0,1,1)  120-180 degrees
                piH = (int*) &(ParkParm.qIa); piL = (int*) &(ParkParm.qIc);
                ppwmH = (int*) &PDC1; ppwm0 = (int*) &PDC2; ppwmL = (int*) &PDC3;
                break;

            case 0b010: // Sector 3: (0,1,0)  180-240 degrees
                piH = (int*) &(ParkParm.qIb); piL = (int*) &(ParkParm.qIc);
                ppwm0 = (int*) &PDC1; ppwmH = (int*) &PDC2; ppwmL = (int*) &PDC3;
                break;

            case 0b110: // Sector 4: (1,1,0)  240-300 degrees
                piH = (int*) &(ParkParm.qIb); piL = (int*) &(ParkParm.qIa);
                ppwmL = (int*) &PDC1; ppwmH = (int*) &PDC2; ppwm0 = (int*) &PDC3;
                break;

            case 0b100: // Sector 5: (1,0,0)  300-360 degrees
                piH = (int*) &(ParkParm.qIc); piL = (int*) &(ParkParm.qIa);
                ppwmL = (int*) &PDC1; ppwm0 = (int*) &PDC2; ppwmH = (int*) &PDC3;
                break;

            case 0b101: // Sector 6: (1,0,1)    0- 60 degrees
                piH = (int*) &(ParkParm.qIc); piL = (int*) &(ParkParm.qIb);
                ppwm0 = (int*) &PDC1; ppwmL = (int*) &PDC2; ppwmH = (int*) &PDC3;
                break;
            }

            iHfilt = *piH;
            iLfilt = *piL;
        }

        //////////////////////////////////////////////////////

        iHfilt += *piH;
        iHfilt >>= 1;

        Err = CtrlReferences.qIqRef - iHfilt;

        pwmH = __builtin_mulss(CurrentQPID.IDPMCoefficients[2], Err) >> (16 + KSHIFT_Q_P);

        KiErrIntH += __builtin_mulss(CurrentQPID.IDPMCoefficients[0], Err) >> 8;

        if (KiErrIntH > CurrentQPID.IDPMCoefficients[3])
        {
            KiErrIntH = CurrentQPID.IDPMCoefficients[3];
        }
        else if (KiErrIntH < -CurrentQPID.IDPMCoefficients[3])
        {
            KiErrIntH = -CurrentQPID.IDPMCoefficients[3];
        }

        pwmH += KiErrIntH >> 8;

        if (pwmH >  PWM_MAX) pwmH =  PWM_MAX;
        else 
        if (pwmH < -PWM_MAX) pwmH = -PWM_MAX;

        //////////////////////////////////////////////////////

        iLfilt += *piL;
        iLfilt >>= 1;

        Err = CtrlReferences.qIqRef + iLfilt;

        pwmL = __builtin_mulss(CurrentQPID.IDPMCoefficients[2], Err) >> (16 + KSHIFT_Q_P);

        KiErrIntL += __builtin_mulss(CurrentQPID.IDPMCoefficients[0], Err) >> 8;

        if (KiErrIntL > CurrentQPID.IDPMCoefficients[3])
        {
            KiErrIntL = CurrentQPID.IDPMCoefficients[3];
        }
        else if (KiErrIntL < -CurrentQPID.IDPMCoefficients[3])
        {
            KiErrIntL = -CurrentQPID.IDPMCoefficients[3];
        }

        pwmL += KiErrIntL >> 8;

        if (pwmL >  PWM_MAX) pwmL =  PWM_MAX;
        else 
        if (pwmL < -PWM_MAX) pwmL = -PWM_MAX;
    }
    else // end TORQUE CONTROL
    {
        pwmH = pwmL = 0;
    }

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

    *ppwmH = PWM_CENTER + pwmH;
    *ppwm0 = PWM_CENTER;
    *ppwmL = PWM_CENTER - pwmL;

    // perform I2T protection from ParkParm.qId,qIq
    //I2Tdata.IQMeasured = /*abs*/(ParkParm.qIq);
    //I2Tdata.IDMeasured = /*abs*/(ParkParm.qId);

    I2Tdata.IQMeasured = I2Tdata.IDMeasured = ParkParm.qIa;
    I2Tdata.IQMeasured += ParkParm.qIb;
    I2Tdata.IQMeasured += ParkParm.qIb;
    I2Tdata.IQMeasured *= 10;
    I2Tdata.IQMeasured >>= 4;

    // Invoke the I2T integrator with new current values
    if (I2T(&I2Tdata))
    {
        //The I2T grew too much. Protect!
        SysError.I2TFailure = 1;
        FaultConditionsHandler();
    }

    // when gulp_update_request is == 1, the main loop will not access those vars.
    if (gulp_update_request) updateGulp();

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

void updateOdometry(void)
{
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

    if (delta < -32000) delta += 65536;
    if (delta > 32000) delta -= 65536;

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

#if defined DC_MOTOR

// DMA0 IRQ Service Routine used for FOC loop

void __attribute__((__interrupt__, no_auto_psv)) _DMA0Interrupt(void)
{
    //extern tPID2 CurrentQPID;

    static const int PWM_CENTER = LOOPINTCY / 2;
    static const int PWM_MAX = PWM_CENTER / 4;

    //static int i1filt=0,i2filt=0;

    char pwm1, pwm3;

    int pwmdelay = 0;
    int timeout;

    // setting CORCON in this way rather than using CORCONbits is
    // slightly more efficient (because CORCONbits is volatile and
    // optimizer cannot collapse multiple bit operations)

    CORCON = 0b11110100;

    // read Meccanical postion [-1..0.9999] and angle corrected with encsyncpulsepos
    // calculate AlignedMecchanicalAngle and UnalignedMecchanicalAngle from raw encoder data
    //EncoderPosition();

    // read and compensate ADC offset by MeasCurrParm.Offseta, Offsetb, Offsetc
    // scale currents by MeasCurrParm.qKa, qKb, qKc
    // Calculate ParkParm.qIa, qIb, qIc
    MeasAndCompIaIcCalculateIb();

    if (SysStatus.OpenLoop)
    {
        pwm1 = CtrlReferences.qIqRef & 0xFF;
        pwm3 = (CtrlReferences.qIqRef >> 8) & 0xFF;
    }
    else if (SysStatus.TorqueControl)
    {
        pwm1 = pwm3 = 0;

        /*
        static int KiErrIntH=0,KiErrIntL=0;

        int Err;

        if (encoder_dhes_state_stored != encoder_dhes_state)
        {
            encoder_dhes_state_stored  = encoder_dhes_state;

            KiErrIntH=KiErrIntL=0;

            switch (encoder_dhes_state)
            {
            case 0b001: // Sector 1: (0,0,1)   60-120 degrees
                piH=&(ParkParm.qIa); piL=&(ParkParm.qIb);
                ppwmH=&PDC1; ppwmL=&PDC2; ppwm0=&PDC3; break;

            case 0b011: // Sector 2: (0,1,1)  120-180 degrees
                piH=&(ParkParm.qIa); piL=&(ParkParm.qIc);
                ppwmH=&PDC1; ppwm0=&PDC2; ppwmL=&PDC3; break;

            case 0b010: // Sector 3: (0,1,0)  180-240 degrees
                piH=&(ParkParm.qIb); piL=&(ParkParm.qIc);
                ppwm0=&PDC1; ppwmH=&PDC2; ppwmL=&PDC3; break;

            case 0b110: // Sector 4: (1,1,0)  240-300 degrees
                piH=&(ParkParm.qIb); piL=&(ParkParm.qIa);
                ppwmL=&PDC1; ppwmH=&PDC2; ppwm0=&PDC3; break;

            case 0b100: // Sector 5: (1,0,0)  300-360 degrees
                piH=&(ParkParm.qIc); piL=&(ParkParm.qIa);
                ppwmL=&PDC1; ppwm0=&PDC2; ppwmH=&PDC3; break;

            case 0b101: // Sector 6: (1,0,1)    0- 60 degrees
                piH=&(ParkParm.qIc); piL=&(ParkParm.qIb);
                ppwm0=&PDC1; ppwmL=&PDC2; ppwmH=&PDC3; break;
            }

            iHfilt = *piH;
            iLfilt = *piL;
        }

        //////////////////////////////////////////////////////

        iHfilt += *piH;
        iHfilt >>= 1;

        Err = CtrlReferences.qIqRef - iHfilt;

        pwmH = __builtin_mulss(CurrentQPID.IDPMCoefficients[2], Err) >> (16+KSHIFT_Q_P);

        KiErrIntH += __builtin_mulss(CurrentQPID.IDPMCoefficients[0], Err) >> 8;

        if (KiErrIntH > CurrentQPID.IDPMCoefficients[3])
        {
            KiErrIntH = CurrentQPID.IDPMCoefficients[3];
        }
        else if (KiErrIntH < -CurrentQPID.IDPMCoefficients[3])
        {
           KiErrIntH = -CurrentQPID.IDPMCoefficients[3];
        }

        pwmH += KiErrIntH >> 8;

        if (pwmH>PWM_MAX) pwmH=PWM_MAX; else if (pwmH<-PWM_MAX) pwmH=-PWM_MAX;

        //////////////////////////////////////////////////////

        iLfilt += *piL;
        iLfilt >>= 1;

        Err = CtrlReferences.qIqRef + iLfilt;

        pwmL = __builtin_mulss(CurrentQPID.IDPMCoefficients[2], Err) >> (16+KSHIFT_Q_P);

        KiErrIntL += __builtin_mulss(CurrentQPID.IDPMCoefficients[0], Err) >> 8;

        if (KiErrIntL > CurrentQPID.IDPMCoefficients[3])
        {
            KiErrIntL = CurrentQPID.IDPMCoefficients[3];
        }
        else if (KiErrIntL < -CurrentQPID.IDPMCoefficients[3])
        {
            KiErrIntL = -CurrentQPID.IDPMCoefficients[3];
        }

        pwmL += KiErrIntL >> 8;

        if (pwmL>PWM_MAX) pwmL=PWM_MAX; else if (pwmL<-PWM_MAX) pwmL=-PWM_MAX;
         */
    }
    else
    {
        pwm1 = pwm3 = 0;
    }

    // if we are close enough to the PCD register shadow copy then wait until
    // its done before updating local PDCs
    // The condition will became false when the PWM is counting down but it is still
    // far from 0 or when it change direction counting up

    // reset the flag that indicates if the PWM registers update was delayed
    pwmdelay = 0;

    // reset timeout counter that is used to wait for the PWM registers update
    timeout = PWMSAFETIME;

    //     PWM count DN               PWM count < PWM_SAFE_TIME
    while ((P1TMR & 0x8000) && ((P1TMR & 0x7fff) < PWMSAFETIME))
    {
        pwmdelay = 1;

        if (0 == timeout--)
        {
            SysError.FirmwarePWMFatalError = 1;
            //FaultConditionsHandler();
            break;
        }
    }

    if (pwmdelay)
    {
        // if the PWM PDC update has been delayed, then increment
        // the counter that keeps track of this event, but be
        // careful not to overflow it
        if (FirmwarePWMTimingErrorCount < 0xff)
        {
            FirmwarePWMTimingErrorCount++;
        }
    }

    PDC1 = PWM_CENTER + pwm1;
    PDC2 = PWM_CENTER;
    PDC3 = PWM_CENTER + pwm3;

    // when gulp_update_request is == 1, the main loop will not access those vars.
    if (1 == gulp_update_request)
    {
        // GULP!ed variables
        Gulp.W[0] = *gulpadr1;
        Gulp.W[1] = *gulpadr2;
        Gulp.W[2] = *gulpadr3;
        Gulp.W[3] = *gulpadr4;

        // unlock the main loop, so it will read values just updated
        gulp_update_request = 0;

        // read VDC link raw value;
        VDCLink = ADCGetVDCLink();
    }

    // perform I2T protection from ParkParm.qId,qIq
    //I2Tdata.IQMeasured = /*abs*/(ParkParm.qIq);
    //I2Tdata.IDMeasured = /*abs*/(ParkParm.qId);

    I2Tdata.IQMeasured = I2Tdata.IDMeasured = ParkParm.qIa;
    I2Tdata.IQMeasured += ParkParm.qIb;
    I2Tdata.IQMeasured += ParkParm.qIb;
    I2Tdata.IQMeasured *= 10;
    I2Tdata.IQMeasured >>= 4;

    // Invoke the I2T integrator with new current values
    if (I2T(&I2Tdata))
    {
        //The I2T grew too much. Protect!
        SysError.I2TFailure = 1;
        FaultConditionsHandler();
    }

    //Clear the DMA0 Interrupt Flag
    IFS0bits.DMA0IF = 0;
}
#endif

#if 0
void __attribute__((__interrupt__, no_auto_psv)) _DMA0Interrupt(void)
//
// DMA0 IRQ Service Routine
// used for FOC loop
//
{
    int pwmdelay = 0;
    int timeout;
    static int W_PID_Undersample = W_PID_UNDERSAMPLE;
    static int encoder_sample_alignment_reference = 0;

    // depending by MOTOR_LINEAR this variable will
    // contains the position with or without turn count.
    // This is better explained where the variable is used..
#ifdef MOTOR_LINEAR
    long mechanical_position;
#else
    SFRAC16 mechanical_position;
#endif

    //DEADCATS: inizializzando idx con qualsiasi valore il controllo di velocita
    //  'grattava' in una direzione.
    //  E' stata individuata la causa: la coincidenza tra il reload del contatore PWM e l`aggiornamento
    //  dei valori da parte del SVG quando vengono usati encoder spi.
    //  per evitare tutto cio l'irq FOC deve terminare PRIMA della meta' della PWM!!!
    //   |\      _,,,---,,_
    //   /,`.-'`'    -.  ;-;;,_
    //  |,4-  ) )-,_..;\ (  `'-'
    // '-  ''(_/--'  `-'\_)
    //  Per questa cosa e' stato implementato un workaround: Nel caso in cui si preveda
    //  che i registri stiano per essere aggiornati nel momento sbagliato, il loro
    //  aggiornamento viene leggermente ritardato fino ad oltre il momento critico.
    //  In questo modo se il loop FOC dovesse durare piu di meta PWM abbiamo un jitter
    //  (i valori PWM vengono settati per il giro dopo) ma almeno non si inchioda tutto.
    //  Questa condizione e' comunque segnalata tramite flag di errore e sarebbe meglio
    //  che non si verificasse...
    //

#ifdef PIN_RA3_IS_DEBUG
    // debug pin
    PORTAbits.RA3 = 1;
#endif

    // This ISR is *very* high priority.
    // It might preempt lower priority ASM DSP code.
    // Above we make it sure that the preempted code can continue
    // safely when this ISR finishes. Here we ensure that THIS
    // ISR (that will use DSP ASM funtions) can work properly, and
    // so we set the CORCON register in proper way.

    // setting CORCON in this way rather than using CORCONbits is
    //slightly more efficient (because CORCONbits is volatile and
    // optimizer cannot collapse multiple bit operations)
    CORCON = (1 << 2) | // PSV for const data
            (1 << 4) | // enable super saturation
            (1 << 5) | // enable saturation on DSP write to mem
            (1 << 6) | // enable saturation on DSP accumulator B
            (1 << 7); // enable saturation on DSP accumulator A
    // conventional rounding mode (bit 1 set to 0)
    // fractional multiplier (bit 0 set to 0)
    // multiplier is signed (bit 12 set to 0)

    // read Meccanical postion [-1..0.9999] from main encoder
    encoder_value = EncoderPosition(0);

#ifdef ENCODER_ENABLE_AUX
    // read Meccanical postion [-1..0.9999] from aux encoder
    encoder_aux_value = EncoderPosition(1);
#else
    // to make it possible to avoid some #ifdef in the code.
    // when there is no AUX encoder emulate it with the same value of
    // the reference encoder. In this way some portions of code
    // can use the encoder aux value without worrying if it
    // really exists or not..
    encoder_aux_value = encoder_value;
#endif

    // Trigger encoder. Start to prepare data for the next reading (if needed..)
    // this for ABS encoder make SPI to initiate transfer.
    EncoderTriggerSample();


    // read and compensate ADC offset by MeasCurrParm.Offseta, Offsetb, Offsetc
    // scale currents by MeasCurrParm.qKa, qKb, qKc
    // Calculate ParkParm.qIa, qIb, qIc
    // connect X->A, Y->B, Z->C read Ia and Ic and then then calculate Ib
    MeasAndCompIaIcCalculateIb();


    if (1 == SysStatus.OpenLoop)
        // For openloop movment measured qAngle, is overridden.
        // increase ParkParm.qAngle to change rotation speed ramping up from zero.
        // Id and Iq are imposed constants
    {
        static unsigned int cnt = 0;
        static unsigned int DeltaTheta = OL_MIN_DELTA_THETA;

        // force the angle increase
        if ((cnt % OPENLOOP_DEGRADATION) == 0)
        {
            ParkParm.qAngle = OpenloopAngle;
        }

        // rotate mag field by DeltaTheta angle
        OpenloopAngle += DeltaTheta;

        // increase slope counter that causes the DeltaTheta to ramp up
        cnt++;

        if ((cnt > OL_RAMP_SLOPE) && (DeltaTheta < OL_MAX_DELTA_THETA))
        {
            DeltaTheta++;
            cnt = 0;
        }
    }
    else // Closed Loop (torque, speed, position)
    {
        // keep giving control to the rotor alignment routine until
        // the rotor has beel aligned
        if (1 == SysStatus.InitialRotorAlignmentInProgress)
        {
            RotorAlignStep(encoder_value, encoder_aux_value);
            // if we was performing rotor alignment, and we just finished
            if (0 == SysStatus.InitialRotorAlignmentInProgress)
            {
                // then restart setpoint watchdog
                ResetSetpointWatchdog();
                // next round the encoder reads (aux vs main) will be
                // taken as reference to detect encoder misalignment
                encoder_sample_alignment_reference = 1;

            }
        }
        else
        {
            // Rotor alignment completed start normal control loops

            // check for the setpoint CAN watchdog..
            if (setpoint_watchdog_enabled)
            {
                if (IFS1bits.T5IF)
                {
                    //ALE SysError.SetpointWatchdogTimeout = 1;
                    FaultConditionsHandler();
                }
            }


            // the #ifdef is not required, it can be always active, because if
            // the aux encoder is not present, the AUX encoder value variable
            // is the same of the reference (the only) encoder
#ifdef ENCODER_ENABLE_AUX
            // if the rotor alignment has been done in the previous foc loop
            // round, then take the reference values from both encocoder to
            // detect encoder misalignment
            if (encoder_sample_alignment_reference)
            {
                encoder_sample_alignment_reference = 0;
                // the #ifdef is not required, it can be always active, because if
                // the aux encoder is not present, the AUX encoder value variable
                // is the same of the reference (the only) encoder

                // If an auxiliary encoder is in use, save the offset from the main encoder.
                // To overcome the "wrapping" on encoder turn we use unsigned subtraction:
                // The result of a subtraction generating a negative number in an unsigned type is well-defined:
                // A computation involving unsigned operands can never overflow, because a result that cannot
                // be represented by the resulting unsigned integer type is reduced modulo the number
                // that is one greater than the largest value that can be represented by the resulting type.
                // Unsigned operations, thus, works in modular arithmetic, and have the same "wrapping"
                // behaviour of the encoders.
                // Since the encoders wrap to max value of (unsigned int), the "encoder" modular
                // arithmetic perfectly fits on the "unsinged int" modular arithmetic.
                encoder_aux_offset = (unsigned int) encoder_value - (unsigned int) encoder_aux_value;

            }


            // caluclate the difference between encoders, correct by subtracting the
            // initial difference and check if it is in the allowed tolerance range.
            // the trick of unsigned subtraction is explained where encoder_aux_offset is assigned.
            encoder_misalign = (unsigned int) encoder_value - (unsigned int) encoder_aux_value;
            encoder_misalign -= encoder_aux_offset;

            if ((encoder_misalign > ENCODER_MISALIGN_THRESHOLD) ||
                    (encoder_misalign < -ENCODER_MISALIGN_THRESHOLD))
            {

                SysError.EncoderFault = 1;
                FaultConditionsHandler();
            }

#endif


            // get velocity measurement from encoder registers
#ifdef TLE_INTERNAL_SPEED
            // Attentione!!!!:
            // la lettura di velocita' deriva nel caso dell'encoder tle dalla lettura della
            // posizione, calcolo CRC e accettazione dato  NON deve essere spostata dal loop FOC
            // in quanto deve completarsi entro il ciclo.
            tle_speed = EncoderVelocity();
#else
            // TODO: il calcolo della valocita` fatto oldstyle va infilato
            // in alternativa in questo ramo ifdef

            // Use FW calculated speed
            // ....
#endif

            UpdatePositionVariables(encoder_value);

#ifdef COMPLEX_VELOCITY_CALCULATION
            // TODO: comment please
            //
            // for complex velocity calculation algorithm
            //
            // TODO: la verifica dello speedundersampler peggiora le prestazioni del jitter del looop foc!
            //   tantovale farlo ad ogni ciclo!
            //      if(speed_undersample++ == VELOCITY_CALCULATION_UNDERSAMPLE)
            {
                int idx;

                // speed_undersample = 0;
                // increment index for velocity calculation ring buffer
                idx = (VelocityParm.positions_buffer_index + 1) % POSITIONS_BUFFER_NUM;
                // fill the ring buffer for velocity calculation
                VelocityParm.positions_buffer[idx] = (Current_position);
                // never write VelocityParm.positions_buffer_index before filled that location
                VelocityParm.positions_buffer_index = idx;
            }
#else
            // simple velocity calculation method no buffering used
            // only current position and last position stored at the previous speed cycle
            {
                if (speed_undersampler++ == W_SIMPLE_CALCULATION_UNDERSAMPLE)
                    // The speed calculation is done only 1 time every
                    // W_SIMPLE_CALCULATION_UNDERSAMPLE foc loop iterations
                {

                    long tmp = Current_position - Previous_position_for_velocity_calc;

                    // generic division has been changed with more efficient ASM division.
                    // performance increment is huge.
                    //VelocityParm.Velocity /= W_SIMPLE_CALCULATION_DIVISOR ;
                    VelocityParm.Velocity = __builtin_divsd(tmp, W_SIMPLE_CALCULATION_DIVISOR);

                    Previous_position_for_velocity_calc = Current_position;
                    speed_undersampler = 0;


                }
            }
#endif

            // The electrical position is always derived from mechanical position.
            //
            // - For rotative motors
            //   The mechanical position IS the encoder value because
            //   for each mechanical turn the situation is the same:
            //   At every turn the relation between mechanical angle and
            //   electric angle is the same.
            //   So there is no need to compute electrical angle from the full
            //   (with turns count) mechanical position.
            // - For linear motor the above is not true:
            //   every encoder turn is different and must be taken in account
            //   even for calculating electrical angle.

#ifdef MOTOR_LINEAR
            mechanical_position = Current_position;
#else
            mechanical_position = encoder_value;
#endif

#ifdef COMP_POSITION_READ_DELAY
            // using SPI encoders the (fixed) time needed for data communication
            // can be taken in account and compensated.
            // the angle read from SPI is corrected taking in account the rotor speed
            // and the communication speed
            {
                int encoder_delay_correction;
                // compensate for encoder delay.
                // This avoids unwanted flux disalignment when speeding up..
                // TODO: how the mplam c30 compiles the double operation? Check for algo
                // complexity and eventually change (fixed point, MPY etc)
                encoder_delay_correction = ((double) VelocityParm.last_velocity * ENCODER_DELAY);
                //TODO: ENCODER_DELAY deve essere inizializzato in funzione del tipo di encoder definito in userparams!!!

                // convert from mechanical angle to electical angle by multiplying, and add the encoder_sync_pulse_position for alignment
                ParkParm.qAngle = Encoder_SyncPulsePosition + (mechanical_position + encoder_delay_correction) * ELECTRICCYCLESPERENCODERCYCLE;

            }
#else
            // do not compensate position delay

            // convert from mechanical angle to electical angle by multiplying, and add the encoder_sync_pulse_position for alignment
            ParkParm.qAngle = Encoder_SyncPulsePosition + mechanical_position * ELECTRICCYCLESPERENCODERCYCLE;

#endif
        }
    }

    // Calculate ParkParm.qSin,qCos from ParkParm.qAngle
    SinCos();

    // Calculate ParkParm.qId,qIq (and ParkParm.qIalpha,qIbeta) from ParkParm.qSin,qCos,qIa,qIb
    ClarkePark();

    // perform Torque Sensored Control Loop
    if (1 == SysStatus.TorqueSensorLoop)
    {
        // check if the zero torque from the sensor has been calibrated
        // the name of the variable is extremly misleading but...
        if (1 == SysStatus.LoopRunning)
        {
            //use torque sensor read and elaborated value as torque reference
            CtrlReferences.qIqRef = Torque;
        }
    }

    // If we are in speed control then we must run the omega PID every
    // W_PID_UNDERSAMPLE 2foc cycles.
    // Decrease W_PID_Undersample counter until it reaches zero
    if ((1 == SysStatus.SpeedControl) && (0 == W_PID_Undersample--))
    {
        // do velocity control loop
        // calculate ICtrlParm.qIdRef from current velocity and PID parameters
        W_PID_Undersample = W_PID_UNDERSAMPLE;
        // run speed loop PID calculate CtrlReferences.qIqRef from VelocityParm.last_velocity,
        //   CtrlReferences.qWRef and PID parameters

        OmegaControl();


        // per il loop di controllo in assembler
        // CtrlReferences.qIqRef = PIParmW.qOut;
    }

    // Calculate ParkParm.Vd,Vq control values from ParkParm.qId,qIq and ICtrlParm.qIdRef,qIqRef
    IdIqControl();

    if (1 == SysStatus.OpenLoop)
        // For openloop movment calculated values for qVd, qVq are overridden
    {
        // Override Vd and Vq for open loop operation (deflux)
        ParkParm.qVd = OL_VD; // 0;
        // force a value for Vq (controls Iq = torque)
        ParkParm.qVq = OL_VQ;
    }

    if (1 == SysStatus.InitialRotorAlignmentInProgress)
        // For Initial Rotor Alignment calculated values for qVd, qVq are overridden
    {
        // Override Vd and Vq for open loop operation (deflux)
        ParkParm.qVd = 0;
        // force a value for Vq (controls Iq = torque)
        ParkParm.qVq = IraqVq;
    }

    // Calculate ParkParm.qValpha,qVbeta from ParkParm.qSin,qCos,qVd,qVq
    InvPark();
    // Calculate SVGenParm.Vr1,Vr2,Vr3 from ParkParm.qValpha,qVbeta (InvClarke())


    CalcRefVec();

    // reset the flag that indicates if the PWM registers update was delayed
    pwmdelay = 0;

    // reset timeout counter that is used to wait for the PWM registers update
    timeout = PWMSAFETIME;

    // if we are close enough to the PCD register shadow copy then wait until
    // its done before updating local PDCs
    // The condition will became false when the PWM is counting down but it is still
    // far from 0 or when it change direction counting up

    //      PWM count DN           PWM count < PWM_SAFE_TIME
    while ((P1TMR & 0x8000) && ((P1TMR & 0x7fff) < PWMSAFETIME))
    {
        pwmdelay = 1;
        if (0 == timeout--)
        {
            SysError.FirmwarePWMFatalError = 1;
            break;
        }
    }

    if (pwmdelay)
    {
        // if the PWM PDC update has been delayed, then increment
        // the counter that keeps track of this event, but be
        // careful not to overflow it
        if (FirmwarePWMTimingErrorCount < 0xff)
        {
            FirmwarePWMTimingErrorCount++;
        }
    }


    // Calculate and set PWM duty cycles (PDC1, PDC2, PDC3) from SVGenParm.Vr1,Vr2,Vr3
    CalcSVGen();

    // when gulp_update_request is == 1, the main loop will not access those vars.
    if (1 == gulp_update_request)
    {
        // GULP!ed variables
        Gulp.W[0] = *gulpadr1;
        Gulp.W[1] = *gulpadr2;
        Gulp.W[2] = *gulpadr3;
        Gulp.W[3] = *gulpadr4;

        //Gulp.DW[1] = Current_position;

        // unlock the main loop, so it will read values just updated
        gulp_update_request = 0;

        // read VDC link raw value;
        VDCLink = ADCGetVDCLink();
    }


    // perform I2T protection from ParkParm.qId,qIq
    I2Tdata.IQMeasured = /*abs*/(ParkParm.qIq);
    I2Tdata.IDMeasured = /*abs*/(ParkParm.qId);

    // Invoke the I2T integrator with new current values
    if (I2T(&I2Tdata))
    {
        //The I2T grew too much. Protect!
        SysError.I2TFailure = 1;
        FaultConditionsHandler();
    }

    GulpHistoryBuffer[GulpHistoryBufferIdx].W[0] = *gulpadr1;
    GulpHistoryBuffer[GulpHistoryBufferIdx].W[1] = *gulpadr2;
    GulpHistoryBuffer[GulpHistoryBufferIdx].W[2] = *gulpadr3;
    GulpHistoryBuffer[GulpHistoryBufferIdx].W[3] = *gulpadr4;
    GulpHistoryBufferIdx++;
    GulpHistoryBufferIdx &= GULP_HISTORY_BUFFER_MASK;

#ifdef PIN_RA3_IS_DEBUG
    // debug pin
    PORTAbits.RA3 = 0;
#endif


    if (position_limits_enabled)
    {
        if (Current_position < position_limit_lower)
        {
            SysError.PositionLimitLower = 1;
            FaultConditionsHandler();
        }
        else if (Current_position > position_limit_upper)
        {
            SysError.PositionLimitUpper = 1;
            FaultConditionsHandler();
        }
    }

    //Clear the DMA0 Interrupt Flag
    IFS0bits.DMA0IF = 0;


}

#endif
