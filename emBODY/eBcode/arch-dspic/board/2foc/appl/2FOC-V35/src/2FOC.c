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

// no secure sector and write protection disabled
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
#include "can_icubProto_trasmitter.h"
#include "stdint.h"

/////////////////////////////////////////

//#define CALIBRATION

#define BOARD_CAN_ADDR_DEFAULT 0xE
#define VOLT_REF_SHIFT 5

#define isDriveEnabled() bDriveEnabled

volatile tI2T I2Tdata;
volatile tLED_status LED_status = {0x00};
tSysStatus SysStatus;

volatile long gQEPosition = 0;
volatile int  gQEVelocity = 0;
volatile tMeasCurrParm MeasCurrParm;
volatile tCtrlReferences CtrlReferences;
volatile tParkParm ParkParm;

/////////////////////////////////////////////////

//volatile int gulp_sector = 0;
//volatile int gulp_delta = 0;

volatile tMotorConfig MotorConfig;

volatile static char bDriveEnabled = 0;
//volatile static char sAlignInProgress = 0;
volatile int gTemperature = 0;
volatile unsigned int i2cERRORS = 0;

volatile char sAlignInProgress = 0;

volatile long I2Tlimit = 0;
volatile long I2Tacc = 0;
volatile long I2nom = 0;
volatile int  Ipeak = 0;
volatile int  Iovr = 0;
volatile int  Inom = 0;

volatile unsigned char gControlMode = icubCanProto_controlmode_notConfigured;
volatile unsigned char gCtrlModeReq = icubCanProto_controlmode_no_request;
volatile unsigned int  gCtrlModeReqCnt = 0;

volatile int gSetpointWatchdog = -1;

volatile long VqA = 0;
volatile long VqL = 0;
volatile long VdA = 0;
volatile long VqRef = 0;
volatile int  IqRef = 0;

volatile int speed_error_old = 0;
volatile long Is = 0;
volatile int iQerror_old = 0;
volatile int iDerror_old = 0;
volatile char limit = 0;

static const int PWM_MAX = (8*LOOPINTCY)/20; // = 80%

volatile int gMaxCurrent = 0;
volatile long sI2Tlimit = 0;

volatile int  IKp = 0; //8;
volatile int  IKi = 0; //2;
volatile int  IKo = 0; //0;
volatile char IKs = 0; //10;
volatile long IIntLimit = 0;//800L*1024L;

volatile int  SKp = 0x0C;
volatile int  SKi = 0x10;
volatile char SKs = 0x0A;
volatile int  SKo = 512;
volatile long SIntLimit = 0;//800L*1024L;

/////////////////////////////////////////

void setMaxCurrent(int nom, int peak, int ovr)
{
    Inom = nom;
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
    if (IKs>0) IKo = 1<<(IKs-1); else IKo = 0;
    IIntLimit = ((long)PWM_MAX)<<shift;
}

void setSPid(int kp, int ki, char shift)
{
    SKp = kp;
    SKi = ki/2;
    SKs = shift;
    if (SKs>0) SKo = 1<<(SKs-1); else SKo = 0;
    SIntLimit = ((long)PWM_MAX)<<shift;
}

void ZeroControlReferences()
{
    CtrlReferences.VqRef = 0;
    CtrlReferences.IqRef = 0;
    CtrlReferences.WRef = 0;

    VdA = 0;
    VqA = 0;
    VqL = 0;
    VqRef = 0;
    IqRef = 0;
    speed_error_old = 0;
    Is = 0;
    iQerror_old = 0;
    iDerror_old = 0;
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

BOOL updateOdometry()
{
    if (MotorConfig.has_qe || MotorConfig.has_speed_qe)
    {
        static const int UNDERSAMPLING = PWMFREQUENCY / 1000;
        static int speed_undersampler = 0;

        static int position_old = 0;
        int position = QEgetPos();
        int delta = position - position_old;

        position_old = position;

        if (sAlignInProgress)
        {
            gQEPosition = 0;
            gQEVelocity = 0;
            return FALSE;
        }

        gQEPosition += delta;

        if (++speed_undersampler == UNDERSAMPLING) // we obtain ticks per ms
        {
            speed_undersampler = 0;

            static long QEPosition_old = 0;

            gQEVelocity = (1 + gQEVelocity + gQEPosition - QEPosition_old) / 2;

            QEPosition_old = gQEPosition;

            return TRUE;
        }

        return FALSE;
    }
    else if (MotorConfig.has_hall)
    {
        gQEPosition = DHESPosition();
        gQEVelocity = DHESVelocity();

        return FALSE;
    }

    return FALSE;
}

volatile int dataA = 0;
volatile int dataB = 0;
volatile int dataC = 0;
volatile int dataD = 0;
    
extern volatile BOOL newencdata;

int alignRotorHES(volatile int* IqRef)
{
    if (!sAlignInProgress) return 0;

    static int encoder_fake = 0;
    
    static int IqRef_fake = 0;
    
    static BOOL moved = FALSE;
    
    if (abs((int)POSCNT) > 32) moved = TRUE;
    
    const int ENCODER_1_5_REV = (3*QE_ELETTR_DEG_PER_REV())/2;
    
    if (sAlignInProgress < 3)
    {       
        if (QEready())
        {
            if (!moved)
            {
                // phase is broken
                *IqRef = IqRef_fake = 0;
                QE_RISE_ERROR(phase_broken);
                sAlignInProgress = 0;
            }
            
            sAlignInProgress = 0;
            gEncoderError.uncalibrated = 0;
            QEcountErrorClear();
        }
    }
    
    if (sAlignInProgress == 1)
    {
        static int timer = 0;
        
        if (IqRef_fake < Inom/2)
        {
            ++IqRef_fake;
        }
        else if (encoder_fake < ENCODER_1_5_REV)
        {
            if (++timer > 200) { timer = 0; ++encoder_fake; }
        }
        else
        {
            sAlignInProgress = 2;
        }
    }
    
    if (sAlignInProgress == 2)
    {
        static int timer = 0;
        
        if (encoder_fake > -ENCODER_1_5_REV)
        {
            if (++timer > 200) { timer = 0; --encoder_fake; }
        }
        else
        {
            // Index is broken
            *IqRef = IqRef_fake = 0;
            QE_RISE_ERROR(index_broken);
            sAlignInProgress = 0;
        }
    }
    
    *IqRef = IqRef_fake;
    
    int encoder = encoder_fake;
    
    while (encoder >= 360) encoder -= 360;
    while (encoder <    0) encoder += 360;
    
    return encoder;
}

int alignRotorIndex(volatile int* IqRef)
{
    if (MotorConfig.has_hall) return 0;
    
    if (!sAlignInProgress) return 0;

    static int encoder_fake = 0;
    
    static int IqRef_fake = 0;
    
    static BOOL moved = FALSE;
    
    if (abs((int)POSCNT) > 32) moved = TRUE;
    
    const int ENCODER_1_5_REV = (3*QE_ELETTR_DEG_PER_REV())/2;
    
    if (sAlignInProgress < 3)
    {       
        if (QEready())
        {
            if (!moved)
            {
                // phase is broken
                *IqRef = IqRef_fake = 0;
                QE_RISE_ERROR(phase_broken);
                sAlignInProgress = 0;
            }
            
            sAlignInProgress = gEncoderConfig.full_calibration ? 3 : 0;
            gEncoderError.uncalibrated = 0;
            QEcountErrorClear();
        }
    }
    
    static int deltaA =  60;
    static int deltaB = -60;
    
    //static int deltaA = -120;
    //static int deltaB =    0;
    
    if (sAlignInProgress == 1)
    {
        static int timer = 0;
        
        if (IqRef_fake < Inom/2)
        {
            ++IqRef_fake;
        }
        else if (encoder_fake < ENCODER_1_5_REV)
        {
            if (++timer > 200) { timer = 0; ++encoder_fake; }
        }
        else
        {
            deltaA = -120;
            deltaB = 0;
            sAlignInProgress = 2;
        }
    }
    
    if (sAlignInProgress == 2)
    {
        static int timer = 0;
        
        if (encoder_fake > -ENCODER_1_5_REV)
        {
            if (++timer > 200) { timer = 0; --encoder_fake; }
        }
        else
        {
            // Index is broken
            *IqRef = IqRef_fake = 0;
            QE_RISE_ERROR(index_broken);
            sAlignInProgress = 0;
        }
    }
    
    ////////////////////////////////////
    
    static int encoder_fake_0 = 0x7FFF;
    
    if (sAlignInProgress == 3)
    {
        static int timer = 0;
         
        encoder_fake_0 = encoder_fake;
        
        if (IqRef_fake < Inom)
        {
            ++IqRef_fake;
        }
        else if (++timer > 10000)
        {
            sAlignInProgress = 4;
        }
    }
    
    static int rotorA;
    static int rotorB;
    
    static unsigned int timer = 0;
    
    if (sAlignInProgress == 4)
    {
        encoder_fake = encoder_fake_0 + deltaA;
        
        if (++timer > 30000)
        {
            rotorA = __builtin_divsd(__builtin_mulss((int)POSCNT, gEncoderConfig.elettr_deg_per_rev),QE_RESOLUTION);
            
            timer = 0;
            
            sAlignInProgress = 5;
        }
    }
    
    if (sAlignInProgress == 5)
    {
        encoder_fake = encoder_fake_0 + deltaB;
        
        if (++timer > 60000)
        {
            IqRef_fake = 0;
            
            rotorB = __builtin_divsd(__builtin_mulss((int)POSCNT, gEncoderConfig.elettr_deg_per_rev),QE_RESOLUTION);
            
            int delta = rotorB - rotorA;
            
            while (delta >= 180) delta -= 360;
            while (delta < -180) delta += 360;
            
            if (abs(delta) > 90)
            {
                int angle = rotorA + delta/2;
            
                while (angle >= 360) angle -= 360;
                while (angle <    0) angle += 360;
            
                dataC = rotorA;
                dataD = rotorB;
            
                gEncoderConfig.offset = encoder_fake_0 + (deltaA + deltaB)/2 + 90 - angle;
                
                while (gEncoderConfig.offset >= 360) gEncoderConfig.offset -= 360;
                while (gEncoderConfig.offset <    0) gEncoderConfig.offset += 360;
                
                sAlignInProgress = 0;
            }
            else
            {
                if (deltaB)
                {
                    deltaB = 0;
                    deltaA = -120;
                    timer = 0;
                    sAlignInProgress = 4;
                }
                else
                {
                    // Index is broken
                    *IqRef = IqRef_fake = 0;
                    gEncoderError.uncalibrated = 0;
                    QE_RISE_ERROR(autocalib);
                    sAlignInProgress = 0;
                }
            }
        }
    }
    
    /////////////////////////////////////
    
    *IqRef = IqRef_fake;
    
    int encoder = encoder_fake;
    
    while (encoder >= 360) encoder -= 360;
    while (encoder <    0) encoder += 360;
    
    return encoder;
}

// DMA0 IRQ Service Routine used for FOC loop

extern volatile int dataC;
extern volatile int dataD;

volatile int angle_feedback = 0;
volatile int sectr_feedback = 0;

volatile short Ia = 0, Ib = 0, Ic = 0;
volatile short Va = 0, Vb = 0, Vc = 0;

void __attribute__((__interrupt__, no_auto_psv)) _DMA0Interrupt(void)
{
    int Vq = 0;

    static const int cos_table[] = {32767,32763,32748,32723,32688,32643,32588,32523,32449,32364,32270,32165,32051,31928,31794,31651,31498,31336,31164,30982,30791,30591,30381,30163,29935,29697,29451,29196,28932,28659,28377};
    static const int sin_table[] = {    0,  330,  660,  990, 1319, 1648, 1977, 2305, 2632, 2959, 3285, 3609, 3933, 4255, 4576, 4896, 5214, 5531, 5846, 6159, 6470, 6779, 7087, 7392, 7694, 7995, 8293, 8588, 8881, 9171, 9459};
    
    volatile static short *iH, *iL, *i0;
    volatile static short *ppwmH, *ppwmL, *ppwm0;
    
    // setting CORCON in this way rather than using CORCONbits is
    // slightly more efficient (because CORCONbits is volatile and
    // optimizer cannot collapse multiple bit operations)
    CORCON = 0b11110100;

    // read and compensate ADC offset by MeasCurrParm.Offseta, Offsetb, Offsetc
    // scale currents by MeasCurrParm.qKa, qKb, qKc
    // Calculate ParkParm.qIa, qIb, qIc
    //MeasAndCompIaIcCalculateIb();
    
    ParkParm.qIa = (int)((__builtin_mulss(MeasCurrParm.Offseta-ADCBuffer[0],MeasCurrParm.qKa)+8192L)>>14); 
    ParkParm.qIc = (int)((__builtin_mulss(MeasCurrParm.Offsetc-ADCBuffer[1],MeasCurrParm.qKc)+8192L)>>14);
    
    ParkParm.qIb = -ParkParm.qIa-ParkParm.qIc;
    
    // qIx = 64 is equal to 50 mA current here
    // since we have 10 bits resolution, left aligned, with LSB = 50 mA
    
    // gain = 64/49.03
    
    static short Ia_old = 0, Ib_old = 0, Ic_old = 0;
    
    Ia = (ParkParm.qIa + Ia_old)/3;
    Ib = (ParkParm.qIb + Ib_old)/3;
    Ic = (ParkParm.qIc + Ic_old)/3;
    
    // gain = (64/49.03) * (2/3)
    
    Ia_old = ParkParm.qIa;
    Ib_old = ParkParm.qIb;
    Ic_old = ParkParm.qIc;

    //if (Ia>=0) Ia=(Ia+3)/6; else Ia=(Ia-3)/6;
    //if (Ib>=0) Ib=(Ib+3)/6; else Ib=(Ib-3)/6;
    //if (Ic>=0) Ic=(Ic+3)/6; else Ic=(Ic-3)/6;
    
    if (MotorConfig.has_hall && MotorConfig.has_qe)
    {
        static int hall_sector_old = 0;
                
        int hall_sector = DHESSector();
        
        sectr_feedback = hall_sector;
        
        if (hall_sector != hall_sector_old)
        {
            if (hall_sector_old == 6 && hall_sector == 1)
            {
                QEsignalZeroCrossing(1);
            }
            else if (hall_sector_old == 1 && hall_sector == 6)
            {
                QEsignalZeroCrossing(0);
            }
            
            hall_sector_old = hall_sector;
        }
    }
    
    int enc = 0;
    
    if (MotorConfig.has_qe)
    {
        // overwrite encoders during alignment
        if (sAlignInProgress)
        {
            if (MotorConfig.has_hall)
            {
                enc = alignRotorHES(&IqRef);
            }
            else if (MotorConfig.has_index)
            {
                enc = alignRotorIndex(&IqRef);
            }
        }
        else
        {            
            enc = QEgetElettrDeg();
        }
    }
    else if (MotorConfig.has_hall)
    {
        enc = 60*(DHESSector()-1);
    }
    else // no HALL, no encoders, just test
    {
        static int t = 0;
        static int senc = 0;

        if (++t > 2000)
        {
            t = 0;
            ++senc;
            if (senc > 359) senc = 0;
        }
        
        enc = senc;
    }
    
    BOOL has_speed_sample = updateOdometry();

    // enc is in [0 - 360) range here

    angle_feedback = enc;
    
    char sector = 1 + enc/60;

    static char sector_stored = 0;

    if (sector_stored != sector)
    {                
        sector_stored = sector;
        
        #define HI(Ix,Vx) iH = &Ix; ppwmH = &Vx;
        #define LO(Ix,Vx) iL = &Ix; ppwmL = &Vx;
        #define NE(Ix,Vx) i0 = &Ix; ppwm0 = &Vx;
        
        switch (sector) // original
        {
            case 1: HI(Ia,Va) LO(Ib,Vb) NE(Ic,Vc) break; // 1 0 1   5 -> 1
            case 2: HI(Ia,Va) LO(Ic,Vc) NE(Ib,Vb) break; // 1 0 0   4 -> 2
            case 3: HI(Ib,Vb) LO(Ic,Vc) NE(Ia,Va) break; // 1 1 0   6 -> 3
            case 4: HI(Ib,Vb) LO(Ia,Va) NE(Ic,Vc) break; // 0 1 0   2 -> 4
            case 5: HI(Ic,Vc) LO(Ia,Va) NE(Ib,Vb) break; // 0 1 1   3 -> 5
            case 6: HI(Ic,Vc) LO(Ib,Vb) NE(Ia,Va) break; // 0 0 1   1 -> 6
        }
    }

    BOOL negative_sec = sector%2;

    int delta = (enc%60)-30;

    int sinT,cosT;

    if (delta> 30) delta =  30;
    if (delta<-30) delta = -30;
    
    if (delta<0)
    {
        cosT =  cos_table[-delta];
        sinT = -sin_table[-delta];
    }
    else
    {
        cosT =  cos_table[ delta];
        sinT =  sin_table[ delta];
    }
    
    if (negative_sec)
    {
        // gain = (64/49.03) * (2/3) * (2/sqrt3) = 1.0048 OK!
        
        I2Tdata.IQMeasured = /* sqrt3/2 */  (int)((__builtin_mulss(*iH-*iL,cosT)-__builtin_mulss( *i0*3 ,sinT)+16384L)>>15);
        I2Tdata.IDMeasured = /* 3/2 */     -(int)((__builtin_mulss(  *i0  ,cosT)+__builtin_mulss(*iH-*iL,sinT)+16384L)>>15);
        
        //I2Tdata.IQMeasured = /* sqrt3/2 */  (int)(__builtin_mulss((*iH-*iL),cosT)>>15)-3*(int)(__builtin_mulss(   *i0   ,sinT)>>15);
        //I2Tdata.IDMeasured = /* 3/2 */     -(int)(__builtin_mulss(   *i0   ,cosT)>>15)-  (int)(__builtin_mulss((*iH-*iL),sinT)>>15);
    }
    else
    {
        I2Tdata.IQMeasured = /* sqrt3/2 */  (int)((__builtin_mulss(*iH-*iL,cosT)+__builtin_mulss( *i0*3 ,sinT)+16384L)>>15);
        I2Tdata.IDMeasured = /* 3/2 */      (int)((__builtin_mulss(  *i0  ,cosT)-__builtin_mulss(*iH-*iL,sinT)+16384L)>>15);
        
        //I2Tdata.IQMeasured = /* sqrt3/2 */  (int)(__builtin_mulss((*iH-*iL),cosT)>>15)+3*(int)(__builtin_mulss(   *i0   ,sinT)>>15);
        //I2Tdata.IDMeasured = /* 3/2 */      (int)(__builtin_mulss(   *i0   ,cosT)>>15)-  (int)(__builtin_mulss((*iH-*iL),sinT)>>15);
    }
    
    if (!sAlignInProgress)
    {
        if (gControlMode == icubCanProto_controlmode_speed_voltage || gControlMode == icubCanProto_controlmode_speed_current)
        {
            if (has_speed_sample)
            {
                int speed_error = CtrlReferences.WRef - gQEVelocity;

                if (gControlMode == icubCanProto_controlmode_speed_current)
                {
                    Is += (((long) (speed_error - speed_error_old)) << 4) + (long) (speed_error + speed_error_old);

                    if (Is > Ipeak) Is = Ipeak; else if (Is < -Ipeak) Is = -Ipeak;

                    static BOOL starting = TRUE;
                    
                    if (starting)
                    {
                        if (Is > 0)
                        {
                            if (Is > IqRef) ++IqRef; else { IqRef = (int)Is; starting = FALSE; }
                        }
                        else
                        {
                            if (Is < IqRef) --IqRef; else { IqRef = (int)Is; starting = FALSE; }
                        }
                    }
                    else
                    {
                        IqRef = (int)Is;
                    }
                }
                else
                {
#ifdef R1_UPPER_ARM
                    if (speed_error || CtrlReferences.WRef)
                    {
#endif
                        VqRef += __builtin_mulss(speed_error-speed_error_old,SKp) + __builtin_mulss(speed_error + speed_error_old,SKi);
                        
                        if (VqRef > SIntLimit) VqRef = SIntLimit; else if (VqRef < -SIntLimit) VqRef = -SIntLimit;
#ifdef R1_UPPER_ARM
                    }
                    else
                    {
                        VqRef = 0;
                    }
#endif
                    
                    IqRef = 0;
                }

                speed_error_old = speed_error;
            } // has_speed_sample
        }
        else if (gControlMode == icubCanProto_controlmode_current)
        {
            VqRef = 0;
            IqRef = CtrlReferences.IqRef;

            if (IqRef>Ipeak) IqRef = Ipeak; else if (IqRef<-Ipeak) IqRef = -Ipeak;
        }
        else if (gControlMode == icubCanProto_controlmode_openloop)
        {
            VqRef = ((long)CtrlReferences.VqRef)<<(IKs-VOLT_REF_SHIFT);
            IqRef = 0;
        }
        else
        {
            VqRef = 0;
            IqRef = 0;
        }
    }
    ////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // Iq control loop

    Vq = 0;

    // current closed loop
    if (gControlMode == icubCanProto_controlmode_current || gControlMode == icubCanProto_controlmode_speed_current || sAlignInProgress)
    {
        int iQerror = IqRef-I2Tdata.IQMeasured;

        VqA += __builtin_mulss(iQerror-iQerror_old,IKp) + __builtin_mulss(iQerror+iQerror_old,IKi);

        iQerror_old = iQerror;

        if (VqA > IIntLimit) VqA = IIntLimit; else if (VqA < -IIntLimit) VqA = -IIntLimit;

        Vq = (int)((VqA+IKo)>>IKs);

        // alternative formulation with ff term
        //VqA += __builtin_mulss(iQerror+iQerror_old,Ki);
        //if (VqA > V_INT_LIMIT) VqA = V_INT_LIMIT; else if (VqA < -V_INT_LIMIT) VqA = -V_INT_LIMIT;
        //long VqF = VqA + __builtin_mulss(43,IqRef) + __builtin_mulss(93,gQEVelocity);
        //if (VqF > V_INT_LIMIT) VqF = V_INT_LIMIT; else if (VqF < -V_INT_LIMIT) VqF = -V_INT_LIMIT;
        //Vq = (int)(VqF>>Kshift);
    }
    else // current open loop
    {
#ifdef RELENTLESS
        if ((I2Tdata.IQMeasured > Iovr) || (I2Tdata.IQMeasured < -Iovr))
        {
            SysError.OverCurrentFailure = 1;
            // call fault handler
            FaultConditionsHandler();

            return;
        }   
#else
        if (I2Tdata.IQMeasured > Ipeak)
        {
            limit =  1;
        }
        else if (I2Tdata.IQMeasured < -Ipeak)
        {
            limit = -1;
        }

        if (limit)
        {
            if (limit == 1)
            {
                int iQerror =  Ipeak-I2Tdata.IQMeasured;
                VqL += __builtin_mulss(iQerror-iQerror_old,IKp) + __builtin_mulss(iQerror+iQerror_old,IKi);
                iQerror_old = iQerror;

                if (VqL >= 0) { VqL = 0; limit = 0; iQerror_old = 0; }
            }
            else if (limit == -1)
            {
                int iQerror = -Ipeak-I2Tdata.IQMeasured;
                VqL += __builtin_mulss(iQerror-iQerror_old,IKp) + __builtin_mulss(iQerror+iQerror_old,IKi);
                iQerror_old = iQerror;

                if (VqL <= 0) { VqL = 0; limit = 0; iQerror_old = 0; }
            }

            Vq = (int)((VqRef+VqL+IKo)>>IKs);
        }
        else
#endif // RELENTLESS
        {
            if (gControlMode == icubCanProto_controlmode_openloop)
            {
                Vq = (int)((VqRef+IKo)>>IKs);
            }
            else // if (gControlMode == icubCanProto_controlmode_speed_voltage)
            {
                Vq = (int)((VqRef+SKo)>>SKs);
            }
        }
    }
    //
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // BEMF section
    int iDerror = -I2Tdata.IDMeasured;

    VdA += __builtin_mulss(iDerror-iDerror_old,IKp) + __builtin_mulss(iDerror+iDerror_old,IKi);
    
    iDerror_old = iDerror;

    if (VdA > IIntLimit) VdA = IIntLimit; else if (VdA < -IIntLimit) VdA = -IIntLimit;
  
    int Vd = (int)((VdA+IKo)>>IKs);
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
    // inv transform and PWM drive
    //int V1 = (int)(__builtin_mulss(Vq,cosT)>>15)-3*(int)(__builtin_mulss(Vd,sinT)>>15);
    //int V2 = (int)(__builtin_mulss(Vq,sinT)>>15)+  (int)(__builtin_mulss(Vd,cosT)>>15);
    
    int V1 = (int)((__builtin_mulss(Vq,cosT)-__builtin_mulss(Vd*3,sinT)+16384L)>>15);
    int V2 = (int)((__builtin_mulss(Vq,sinT)+__builtin_mulss(Vd  ,cosT)+16384L)>>15);
    
    if (negative_sec) V2=-V2;

    *ppwmH =  V1-V2;
    *ppwm0 =  V2+V2;
    *ppwmL = -V1-V2;

    pwmOut(Va,Vb,Vc);
    //
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // Invoke the I2T integrator with new current values

    if (!MotorConfig.has_tsens)
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
    int pwm_max = (8*LOOPINTCY)/20; // 80%
    int pwm3_00 = pwm_max/25;
    
    pwmInit(LOOPINTCY/2, DDEADTIME, pwm_max);

    pwmON();
    
    pwmZero();
    
    // setup and perform ADC offset calibration in MeasCurrParm.Offseta and Offsetb
    ADCDoOffsetCalibration();
    
    pwmOut(pwm3_00,-2*pwm3_00,pwm3_00);
    
    int d;
    
    for (d=0; d<5000; ++d) __delay32(4000); // 500 ms
    
    ADCDoGainCalibration();

    pwmOFF();
    
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
#ifndef RELENTLESS
    OverCurrentFaultIntEnable();
#endif

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

        if (!IIntLimit) continue;

        break; // board is configured
    }

    if (MotorConfig.has_qe || MotorConfig.has_speed_qe)
    {
        QEinit(gEncoderConfig.resolution,
               gEncoderConfig.numPoles,
               MotorConfig.has_index);
    }
    else
    {
        IFS3bits.QEI1IF = 0;
        IEC3bits.QEI1IE = 0;
    }

    if (MotorConfig.has_qe)
    {
        sAlignInProgress = 1;
        gEncoderError.uncalibrated = 1;
    }
    else
    {
        sAlignInProgress = 0;
        gEncoderError.uncalibrated = 0;
    }
    
    if (MotorConfig.has_hall)
    {
        MotorConfig.has_tsens = FALSE;
        DHESInit(65536UL/(6*gEncoderConfig.numPoles));
    }
        
    if (MotorConfig.has_tsens) SetupPorts_I2C();
    
    setSPid(SKp, SKi, SKs);

    Timer3Enable(); // EnableAuxServiceTimer();

    gControlMode = icubCanProto_controlmode_idle;
    LED_status.GreenBlinkRate = BLINKRATE_NORMAL;
    LED_status.RedBlinkRate = BLINKRATE_OFF;

    unsigned int ctrlModeReqCnt = 0;
    unsigned char ctrlModeReq = icubCanProto_controlmode_no_request;

    CanIcubProtoTrasmitterSendPeriodicData();

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
            SysError.ExternalFaultAsserted = 1;

            if (isDriveEnabled()) DisableDrive();

            gControlMode = icubCanProto_controlmode_idle;
            LED_status.GreenBlinkRate=BLINKRATE_NORMAL;
            LED_status.RedBlinkRate=BLINKRATE_OFF;

            continue;
        }

        SysError.ExternalFaultAsserted = 0;

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

#ifdef CALIBRATION
    else
    {
        static unsigned long cycle = 0;

        ++cycle;

        if (cycle < 20000)
        {
            enc = 2400;
            if (IqCalib<UDEF_CURRENT_MAX/2) ++IqCalib;
        }
        else if (cycle < 100000)
        {
            enc = 2400;
            if (IqCalib>0) --IqCalib;
        }
        else if (cycle < 120000)
        {
            enc = 0;
            if (IqCalib<UDEF_CURRENT_MAX/2) ++IqCalib;
        }
        else if (cycle < 200000)
        {
            enc = 0;
            if (IqCalib>0) --IqCalib;
        }
        else if (cycle < 220000)
        {
            enc = 1200;
            if (IqCalib<UDEF_CURRENT_MAX/2) ++IqCalib;
        }
        else if (cycle < 300000)
        {
            enc = 1200;
            if (IqCalib>0) --IqCalib;
        }
        else if (cycle < 320000)
        {
            enc = 0;
            if (IqCalib<UDEF_CURRENT_MAX/2) ++IqCalib;
        }
        else if (cycle < 400000)
        {
            enc = 0;
            if (IqCalib>0) --IqCalib;
        }
        else cycle = 0;

    }
#endif

/*
void DriveSelfTest()
// Perform drive HW selftest
{

    // Check silicon revion
    //SiliconRevionTest();

    // Selftest EMUROM
    //EepromTest();

    // Selftest EMUROM
    //EncoderSelfTest();

    // Test ADC offset
    //ADCDoOffsetTest();

    // TODO: vedere se possibile verificare l'oscillatore e la verifica di porte di IO
}
*/


//void updateGulp(void)
//{
    //if (!gulp_update_request) return;

    //Gulp.W[0] = POSCNT;//gulp_mec;
    //Gulp.W[1] = gulp_enc;//gQEVelocity; //gQEVelocity;//gTemperature; //encoder_error;
    //Gulp.W[2] = gulp_sector;//gTemperature;//overf_cnt; // gQEPosition & 0xFFFF;
    //Gulp.W[3] = I2Tdata.IQMeasured;//i2cERRORS;    // gQEPosition>>16;

    //Gulp.W[0] = I2Tdata.IQMeasured;
    //Gulp.W[1] = gQEVelocity;
    //Gulp.W[2] = gQEPosition & 0xFFFF;
    //Gulp.W[3] = gQEPosition >> 16;

    //Gulp.W[0] = *gulpadr1;
    //Gulp.W[1] = *gulpadr2;
    //Gulp.W[2] = *gulpadr3;
    //Gulp.W[3] = *gulpadr4;

    // unlock the main loop, so it will read values just updated
    //gulp_update_request = 0;

    // read VDC link raw value;
    //VDCLink = ADCGetVDCLink();
//}

    /*
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
*/

/*
void EncoderSelfTest()
// encoder selftest (when possible)
{
    // TODO: giustappunto.

}
*/

/*
void ADCDoOffsetTest()
// ADC Offset test
{
    // TODO: giustappunto.
    // A questo punto la calibrazioni e stata fatta
    // Se e' fallita e noi qui resettiamo il flag allora
    // non sapremo mai se e' fallita!!
    // SysError.ADCCalFailure = 0;
}
*/