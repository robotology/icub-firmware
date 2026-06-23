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
 *  TODO: far configurabili i connettori a cui sono attaccati QE e ABS (come hall)
 *
 **********************************************************************/

#include <p33FJ128MC802.h>
#include <libq.h>
#include <timer.h>
#include <ecan.h>
#include <spi.h>
#include <math.h>
#include <dsp.h>
#include <libpic30.h>  //__delay32
#include <i2c.h>

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

#include "MeasCurr.h"
#include "UserParms.h"
#include "System.h"
#include "PWM.h"
#include "ADC.h"
#include "Status.h"
#include "Faults.h"
#include "DCLink.h"
#include "Park.h"

#include "qep.h"
#include "DHES.h"
#include "2FOC.h"

#include "can_icubProto.h"
#include "stdint.h"

/////////////////////////////////////////

//#define CALIBRATION

#define BOARD_CAN_ADDR_DEFAULT 0xE
#define VOLT_REF_SHIFT 5

#define isDriveEnabled() bDriveEnabled

volatile tI2T I2Tdata;
volatile tLED_status LED_status = {0x00};
tSysStatus SysStatus;

volatile tMeasCurrParm MeasCurrParm;
volatile tCtrlReferences CtrlReferences;
tParkParm ParkParm;

/////////////////////////////////////////////////

volatile int gulp_0 = 0;
volatile int gulp_1 = 0;
volatile int gulp_2 = 0;
volatile int gulp_3 = 0;

volatile tMotorConfig MotorConfig;

volatile static char bDriveEnabled = 0;
volatile int gTemperature = 0;
volatile unsigned int i2cERRORS = 0;

volatile long I2Tlimit = 0;
volatile long I2Tacc = 0;
volatile long I2nom = 0;
volatile int  Ipeak = 0;
volatile int  Iovr = 0;

volatile unsigned char gControlMode = icubCanProto_controlmode_notConfigured;
volatile unsigned char gCtrlModeReq = icubCanProto_controlmode_no_request;
volatile unsigned int  gCtrlModeReqCnt = 0;

volatile int gSetpointWatchdog = -1;

//volatile BOOL gCalibrated = FALSE;

volatile long VqA = 0;
volatile long VqL = 0;
volatile long VqRef = 0;
volatile int  IqRef = 0;

volatile long Is = 0;
volatile int iQerror_old = 0;
volatile char limit = 0;

volatile int speed_error_old = 0;

static const int PWM_MAX = (8*LOOPINTCY)/20; // = 80%

volatile int  IKp =  8;
volatile int  IKi =  2;
volatile char IKs = 10;
volatile long IIntLimit = 0;//800L*1024L;

volatile int  SKp = 0x0C;
volatile int  SKi = 0x10;
volatile int  SKd = 0x00;
volatile char SKs = 0x0A;
volatile long SIntLimit = 0;//800L*1024L;

/////////////////////////////////////////

void setMaxCurrent(int nom, int peak, int ovr)
{
    Ipeak = peak;
    I2nom = (long)nom*(long)nom;
    I2Tlimit = ((long)peak*(long)peak - I2nom) * 10;
    Iovr = ovr;
}

void setIPid(int kp, int ki, char shift)
{
    IKp = kp;
    IKi = ki/2;
    IKs = shift;
    IIntLimit = ((long)PWM_MAX)<<shift;
}

void setSPid(int kp, int ki, int kd, char shift)
{
    SKp = kp;
    SKi = ki;
    SKs = shift;

    SIntLimit = ((long)PWM_MAX)<<shift;
}

void ZeroControlReferences()
{
    CtrlReferences.VqRef = 0;
    CtrlReferences.IqRef = 0;
    CtrlReferences.WRef = 0;

    speed_error_old = 0;

    VqA = 0;
    VqL = 0;
    VqRef = 0;
    IqRef = 0;
    Is = 0;
    iQerror_old = 0;
    limit = 0;
    // TODO: zero position and sensored torque references
}

void RequestControlMode(char cm)
{
    //if (cm == gControlMode) return;

    ZeroControlReferences();

    ++gCtrlModeReqCnt;
    gCtrlModeReq = cm;
}

void ResetSetpointWatchdog()
{
    WriteTimer5(0);
    IFS1bits.T5IF = 0;
}

// DMA0 IRQ Service Routine used for FOC loop

volatile int calibration_wdog = 5000;
volatile long calibration_pos_max = 0x80000000;
volatile long calibration_pos_min = 0x7FFFFFFF;

void __attribute__((__interrupt__, no_auto_psv)) _DMA0Interrupt(void)
{
    CORCON = 0b11110100;
    
    MeasAndCompIaIcCalculateIb();

    I2Tdata.IQMeasured = ParkParm.qIa/3;
    //ParkParm.qIc /= 3;

    if (MotorConfig.has_qe) QEupdateOdometry();
    
    ///////////////////////////////////////////////

    VqRef = CtrlReferences.VqRef >> VOLT_REF_SHIFT;
    
    if (gEncoderError.uncalibrated)
    { 
        if (VqRef<-100 || VqRef>100)
        {
            if (gQEPosition > calibration_pos_max)
            {
                calibration_pos_max = gQEPosition;
                calibration_wdog = 5000;
            }
            else if (gQEPosition < calibration_pos_min)
            {
                calibration_pos_min = gQEPosition;
                calibration_wdog = 5000;
            }
            else
            {
                if (calibration_wdog)
                {
                    --calibration_wdog;
                }
                else
                {
                    gEncoderError.uncalibrated = FALSE;

                    QEresetOdometry();
                }
            }
        }
    }

    DCOut(VqRef);
    
    //
    ////////////////////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////////////////////////////
    // if we are close enough to the PCD register shadow copy then wait until
    // its done before updating local PDCs
    // The condition will became false when the PWM is counting down but it is
    // still far from 0 or when it change direction counting up

    // reset the flag that indicates if the PWM registers update was delayed
    BOOL pwm_delayed = FALSE;

    // reset timeout counter that is used to wait for the PWM registers update
    int timeout = PWMSAFETIME;

    //     PWM count DN               PWM count < PWM_SAFE_TIME
    while ((P1TMR & 0x8000) && ((P1TMR & 0x7fff) < PWMSAFETIME))
    {
        pwm_delayed = TRUE;

        if (!--timeout)
        {
            SysError.FirmwarePWMFatalError = TRUE;
            FaultConditionsHandler();
            break;
        }
    }

    if (gSetpointWatchdog>0)
    {
        if (!--gSetpointWatchdog)
        {
            SysError.SetpointExpired = TRUE;
            FaultConditionsHandler();
        }
    }

    //
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // Invoke the I2T integrator with new current values
    {
        static int i2t_undersampler = 0;

        if (++i2t_undersampler >= 20)
        {
            i2t_undersampler = 0;

            I2Twatcher();
        }
    }

    //
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    //Clear the DMA0 Interrupt Flag
    IFS0bits.DMA0IF = 0;
}

// When the 2foc loop is not running, the aux service timer
// (hw timer3) will avoid starving of I2T and Encoder services.
// I2T shall continue to discharge integrated data, and
// Encoder service shall mantain turn count in case of
// external torques rotates the axis.

void EnableAuxServiceTimer()
{
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
}

void DriveInit()
// Perform drive SW/HW init
{
    //pwmInit(LOOPINTCY/2, DDEADTIME, (8*LOOPINTCY)/20 /*pwm max = 80%*/);

    pwmInit(LOOPINTCY/2, DDEADTIME, PWM_MAX /*pwm max = 80%*/);

    // setup and perform ADC offset calibration in MeasCurrParm.Offseta and Offsetb
    ADCDoOffsetCalibration();

    // Enable DMA interrupt, arm DMA for transfer
    ADCConfigPWMandDMAMode();

    // Setup timer for velocity calculation and velocity control loop
    //Timer2Config();

    // Setup timer for CAN periodic send
    // Timer4Config();

    // setup Hw related parameters (current gain, ecc...)
    SetupHWParameters();

    // define default control strategy *only*one*can*be*activated*
    SysStatus.b[0] = 0;

    // zero control set-point
    ZeroControlReferences();

    // Enables the external fault (pushbutton) interrupt
    ExternalFaultEnable();

    // Init timer for setpoint timeout.
    // Note: the timer has to be configured *after*
    // loading eeprom, otherwise the timeout value
    // to set in the timer registers is not known..
    //Timer5Config();
}

inline void I2Twatcher(void) __attribute__((always_inline));
inline void I2Twatcher(void)
{
    I2Tacc += (__builtin_mulss(I2Tdata.IQMeasured,I2Tdata.IQMeasured) - I2nom) >> 10;

    if (I2Tacc < 0) I2Tacc = 0;

    if (I2Tacc > I2Tlimit)
    {
        //The temperature grew too much. Protect!
        SysError.I2TFailure = TRUE;
        FaultConditionsHandler();
    }
}

void EnableDrive()
{
    bDriveEnabled = 1;

    Timer3Disable();

    pwmZero();

    // Arm all the automation (PWM->ADC->DMA->FOC) but don't start yet!
    ADCInterruptAndDMAEnable();

    // enable the overcurrent interrupt
    OverCurrentFaultIntEnable();

    // I2T will run on behalf of 2FOC loop. Stop running it in behalf of Timer 3

    pwmON();
}

void DisableDrive()
// disable the drive functions:
// - disable and zero control reference and status
// - disable PWM generators
// - disable FOC loop
{
    // disable PWM
    pwmOFF();
    pwmZero();

    // Disable ADC IRQ and DMA
    ADCInterruptAndDMADisable();

    // I2T and encoder traking will run in behalf of timer 3 when 2FOC loop is not active
    Timer3Enable();

    // zero torque and speed referemces
    ZeroControlReferences();
    // Zero regulators
    //ZeroRegulators();

    bDriveEnabled = 0;
}

// drive functions are controlled according to DS402 standard (when possible)

int main(void)
{
    volatile extern unsigned char gCanProtocolCompatible;

    oscConfig(); // Configure Oscillator Clock Source, PLL

    SetupPorts(); // set up port direction and peripheral pin mapping

    FaultReset();

    LED_status.GreenBlinkRate = BLINKRATE_ULTRAFAST;

    InterruptPriorityConfig();

    // enable change notification INT for external fault
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;

    Timer1Config(); // Setup timer for low frequency (LED) operations

    Timer3Config(); // Setup timer for I2T (when 2foc loop is not running)
    //Timer3Enable(); // EnableAuxServiceTimer();

    // enable timer1 interrupt
    EnableIntT1;

    // Avoid leaving any random garbage in SysStatus
    memset(SysStatus.b, 0, sizeof(SysStatus));
    gEncoderError.bitmask = 0;

    setMaxCurrent(0, 0, 0);

    while (1)
    {
        DriveInit();
        //DriveSelfTest();

        if (Fault())
        {
            LED_status.RedBlinkRate = BLINKRATE_STILL;
            continue;
        }

        break;
    }

    LED_status.RedBlinkRate = BLINKRATE_OFF;

    // Drives OK

    DCLinkReset();
    ZeroControlReferences();

    uint32_t boardCanAddr = BOARD_CAN_ADDR_DEFAULT;
    _memcpy_p2d16(&boardCanAddr, 0x15000, 4);
    EnableIntT4;
    CanIcubProtoInit(boardCanAddr);

    MotorConfig.bitmask = 0;

    while (1)
    {
        if (!gCanProtocolCompatible) continue;

        if (!MotorConfig.configured) continue;

        if (!Ipeak) continue;
        
        //if (!IIntLimit) continue;

        //if (!SIntLimit) continue;
        
        break; // board is configured
    }

    if (MotorConfig.has_qe)
    {
        QEstart();
    }
    else
    {
        gEncoderError.uncalibrated = FALSE;
    }

    setSPid(SKp, SKi, SKd, SKs);

    Timer3Enable(); // EnableAuxServiceTimer();

    gControlMode = icubCanProto_controlmode_idle;
    LED_status.GreenBlinkRate = BLINKRATE_NORMAL;
    LED_status.RedBlinkRate = BLINKRATE_OFF;

    unsigned int ctrlModeReqCnt = 0;
    unsigned char ctrlModeReq = icubCanProto_controlmode_no_request;

    while (1)
    {
        if (Fault())
        {
            if (isDriveEnabled()) DisableDrive();

            gControlMode = icubCanProto_controlmode_hwFault;
            LED_status.GreenBlinkRate = BLINKRATE_OFF;
            LED_status.RedBlinkRate = BLINKRATE_FAST;
        }
        else if (EXTERNAL_FAULT_PRESSED())
        {
            SysError.ExternalFaultAsserted = TRUE;

            if (isDriveEnabled()) DisableDrive();

            gControlMode = icubCanProto_controlmode_idle;
            LED_status.GreenBlinkRate=BLINKRATE_NORMAL;
            LED_status.RedBlinkRate=BLINKRATE_OFF;

            continue;
        }

        SysError.ExternalFaultAsserted = FALSE;

        if (gControlMode != icubCanProto_controlmode_idle && gControlMode != icubCanProto_controlmode_hwFault)
        {
            DCLinkCheck(ADCVDCLinkTo100mV(ADCGetVDCLink()));
        }

        if (ctrlModeReqCnt == gCtrlModeReqCnt)
        {
            continue;
        }
        else
        {
            ctrlModeReqCnt = gCtrlModeReqCnt;
            ctrlModeReq = gCtrlModeReq;
        }

        if (gControlMode == icubCanProto_controlmode_hwFault)
        {
            if (ctrlModeReq == icubCanProto_controlmode_forceIdle)
            {
                DCLinkReset();
                FaultReset();
                gEncoderError.bitmask = 0;
                gControlMode = icubCanProto_controlmode_idle;
                LED_status.GreenBlinkRate = BLINKRATE_NORMAL;
                LED_status.RedBlinkRate = BLINKRATE_OFF;
                SETPOINT_WATCHDOG_REARM()
            }

            continue;
        }

        if (gControlMode == icubCanProto_controlmode_idle)
        {
            if (ctrlModeReq==icubCanProto_controlmode_current
              ||ctrlModeReq==icubCanProto_controlmode_speed_voltage
              ||ctrlModeReq==icubCanProto_controlmode_speed_current
              ||ctrlModeReq==icubCanProto_controlmode_openloop)
            {
                gControlMode = ctrlModeReq;
                LED_status.GreenBlinkRate=BLINKRATE_STILL;
                LED_status.RedBlinkRate=BLINKRATE_OFF;
                SETPOINT_WATCHDOG_REARM()
                EnableDrive();
            }
            
            continue;
        }
        
        if (gControlMode == icubCanProto_controlmode_current
         || gControlMode == icubCanProto_controlmode_speed_voltage
         || gControlMode == icubCanProto_controlmode_speed_current
         || gControlMode == icubCanProto_controlmode_openloop)
        {
            if (ctrlModeReq == icubCanProto_controlmode_idle
             || ctrlModeReq == icubCanProto_controlmode_forceIdle)
            {
                DisableDrive();

                gControlMode = icubCanProto_controlmode_idle;
                LED_status.GreenBlinkRate = BLINKRATE_NORMAL;
                LED_status.RedBlinkRate=BLINKRATE_OFF;

                continue;
            }

            if (ctrlModeReq == icubCanProto_controlmode_current
             || ctrlModeReq == icubCanProto_controlmode_speed_voltage
             || ctrlModeReq == icubCanProto_controlmode_speed_current
             || ctrlModeReq == icubCanProto_controlmode_openloop)
            {
                ZeroControlReferences();
                SETPOINT_WATCHDOG_REARM()
                gControlMode = ctrlModeReq;
                
                continue;
            }

            continue;
        }
    }
    // should never arrive here!
    while (1);

    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
}
