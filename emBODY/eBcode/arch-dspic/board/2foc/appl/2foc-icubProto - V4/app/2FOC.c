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
#include <stdio.h>
#include <libpic30.h>  //__delay32

#include <i2c.h>

#include "UserTypes.h"
#include "MeasCurr.h"
#include "UserParms.h"
#include "Encoder.h"
#include "System.h"
#include "PWM.h"
#include "ADC.h"
#include "Status.h"
#include "I2T.h"
#include "Faults.h"
#include "DCLink.h"
#include "DS402.h"
#include "Park.h"
#include "qep.h"

#include "can_icubProto.h"
#include "stdint.h"

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

volatile static char QEisOk = 0;
volatile static char sZeroCrossed = 0;
volatile static char sAlignInProgress = 1;
volatile int gTemperature = 0;
volatile unsigned int i2cERRORS = 0;

unsigned int poscnt_max = 0, poscnt_min = 0xFFFF;
unsigned int dirty_cws = 0, dirty_ccw = 0;

char iCubProtControlMode = icubCanProto_controlmode_idle;

unsigned int overf_cnt = 0;

int encoder_error = 0;

//#define CALIBRATION
#define QE_MEC_PPR 14400

void __attribute__((__interrupt__,no_auto_psv)) _QEI1Interrupt(void)
{
    if (!sAlignInProgress)
    {
        static char dir_old = -1;
        char dir = QEICONbits.UPDN;

        if (dir == dir_old)
        {
            static const int ONE_PERCENT_THICKS = QE_MEC_PPR/100;

            dirty_cws = (QE_MEC_PPR-1) - poscnt_max;
            dirty_ccw = poscnt_min;
        
            if ((dirty_cws > ONE_PERCENT_THICKS) || (dirty_ccw > ONE_PERCENT_THICKS))
            {
#ifndef CALIBRATION
                encoder_error |= 1;
                SysError.EncoderFault = 1;
                FaultConditionsHandler();
#endif
            }
        }
        else
        {
            dir_old = dir;
            dirty_cws = 0;
            dirty_ccw = 0;
        }

        poscnt_max = 0;
        poscnt_min = 0xFFFF;
    }

    sZeroCrossed = 1;
    // clear interrupt flag
    IFS3bits.QEI1IF = 0;
}

bool updateOdometry()
{
    static const int UNDERSAMPLING = PWMFREQUENCY / 1000;
    static int speed_undersampler = 0;

    static long position_old = 32767;
    long position = EncoderPositionQE();
    long delta = position - position_old;
    position_old = position;

    static int qe_watchdog = 0;

    if (delta)
    {
        QEisOk = 1;
        qe_watchdog = 0;
    }
    else
    {
        if ((I2Tdata.IQMeasured > 3000) || (I2Tdata.IQMeasured < -3000))
        {
            if (++qe_watchdog > 10000)
            {
                // QE is broken
                qe_watchdog = 0;
/*
#ifndef CALIBRATION
                sAlignInProgress = 1;
                encoder_error |= 2;
                SysError.EncoderFault = 1;
                FaultConditionsHandler();
#endif
*/
            }
        }
    }

    if (!sAlignInProgress)
    {
        unsigned int poscnt=POSCNT;

        if (poscnt > poscnt_max) poscnt_max = poscnt;
        if (poscnt < poscnt_min) poscnt_min = poscnt;

        if ((poscnt>=QE_MEC_PPR+16) && (poscnt<=65520))
        {
            // Index is broken
#ifndef CALIBRATION
            //sAlignInProgress = 1;
            encoder_error |= 4;
            SysError.EncoderFault = 1;
            FaultConditionsHandler();
#endif
        }

        //////////////////////////
        int sposcnt = (int)POSCNT;
        if (sposcnt>=QE_MEC_PPR)
        {
            ++overf_cnt;
            POSCNT -= QE_MEC_PPR;
            encoder_error |= 4;
        }
        else if (sposcnt<0)
        {
            ++overf_cnt;
            POSCNT += QE_MEC_PPR;
            encoder_error |= 4;
        }
        //////////////////////////

        if (sZeroCrossed)
        {
            sZeroCrossed = 0;

            if (QEisOk)
            {
                QEisOk = 0;
            }
            else
            {
                // QE is broken
#ifndef CALIBRATION
                sAlignInProgress = 1;
                encoder_error |= 8;
                SysError.EncoderFault = 1;
                FaultConditionsHandler();
#endif
            }
        }
    }

    if (delta < -32000) delta += 65536L; else if (delta >  32000) delta -= 65536L;

    gQEPosition += delta;

    if (++speed_undersampler == UNDERSAMPLING) // we obtain ticks per ms
    {
        static long QEPosition_old = 0;

        speed_undersampler = 0;

        gQEVelocity = (1 + gQEVelocity + gQEPosition - QEPosition_old) / 2;

        QEPosition_old = gQEPosition;

        return True;
    }

    return False;
}

int alignRotor(int* IqRef)
{
    static int enc_fake = 0;

    if (sZeroCrossed)
    {
        sZeroCrossed = 0;
        sAlignInProgress = -1;

        if (QEisOk)
        {
            QEisOk = 0;
        }
        else
        {
            // QE is broken
            *IqRef = 0;
#ifndef CALIBRATION
            sAlignInProgress = 1;
            encoder_error |= 16;
            SysError.EncoderFault = 1;
            FaultConditionsHandler();
#endif
        }
    }

    if (sAlignInProgress == -1)
    {
        if (*IqRef > 0) --*IqRef; else { sAlignInProgress = 0; QECountErrorClear(); }
    }
    else if (sAlignInProgress == 1)
    {
        if (*IqRef < UDEF_CURRENT_MAX/4) ++*IqRef; else sAlignInProgress = 2;
    }
    else if (sAlignInProgress == 2)
    {
        if (enc_fake <  ((QE_MEC_PPR/2)*3)) ++enc_fake; else sAlignInProgress = 3;
    }
    if (sAlignInProgress == 3)
    {
        if (enc_fake > -((QE_MEC_PPR/2)*3))
        {
            --enc_fake;
        }
        else
#ifdef CALIBRATION
            sAlignInProgress = 2;
#else
        {
            // Index is broken
            *IqRef = 0;
            sAlignInProgress = 1;
            encoder_error |= 32;
            SysError.EncoderFault = 1;
            FaultConditionsHandler();
        }
#endif
    }

    return enc_fake;
}

// DMA0 IRQ Service Routine used for FOC loop

void __attribute__((__interrupt__, no_auto_psv)) _DMA0Interrupt(void)
{
    static const int cos_table[] = {32767,32763,32748,32723,32688,32643,32588,32523,32449,32364,32270,32165,32051,31928,31794,31651,31498,31336,31164,30982,30791,30591,30381,30163,29935,29697,29451,29196,28932,28659,28377};
    static const int sin_table[] = {    0,  330,  660,  990, 1319, 1648, 1977, 2305, 2632, 2959, 3285, 3609, 3933, 4255, 4576, 4896, 5214, 5531, 5846, 6159, 6470, 6779, 7087, 7392, 7694, 7995, 8293, 8588, 8881, 9171, 9459};

    static const int PWM_MAX = (8*LOOPINTCY)/20; // = 80%

    //static long VqA = 0;

    static int *ppwmH = NULL, *ppwmL = NULL, *ppwm0 = NULL;
    static int Va = 0, Vb = 0, Vc = 0;
    
    static short *iH,*iL,*i0;

    static bool starting = True;

    // setting CORCON in this way rather than using CORCONbits is
    // slightly more efficient (because CORCONbits is volatile and
    // optimizer cannot collapse multiple bit operations)
    CORCON = 0b11110100;

    // read and compensate ADC offset by MeasCurrParm.Offseta, Offsetb, Offsetc
    // scale currents by MeasCurrParm.qKa, qKb, qKc
    // Calculate ParkParm.qIa, qIb, qIc
    MeasAndCompIaIcCalculateIb();

    ParkParm.qIb = -ParkParm.qIa-ParkParm.qIc;

    static char sector_stored = 0;
    static int base = 0;

    static const int ENC_PULSE_PER_SECTOR = QE_PPR/6;

    unsigned int uenc = QEPGet();

#ifdef CALIBRATION
    static int IqCalib = 0;
    static const int ENC_OFFSET = 0;
#else
    static const int ENC_OFFSET = 900;//3550; // new motor
    //static const int ENC_OFFSET = 1550; // old motor
#endif

    unsigned int enc_raw = ENC_OFFSET + uenc;

    if (enc_raw >= QE_INDEX_EVERY_X_PULSES) enc_raw-=QE_INDEX_EVERY_X_PULSES;

    int enc = (int)enc_raw;

    bool has_speed_sample = updateOdometry();

    static int IqRef = 0;

    if (sAlignInProgress)
    {
        enc = alignRotor(&IqRef);
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

    enc %= QE_PPR;

    char sector = 1 + enc/ENC_PULSE_PER_SECTOR;

    if (sector_stored != sector)
    {
        sector_stored = sector;

        switch (sector)
        {
            case 1: // Sector 1: (0,0,1)   60-120 degrees
                iH = &(ParkParm.qIa); iL = &(ParkParm.qIb); i0 = &(ParkParm.qIc);
                ppwmH = &Va; ppwmL = &Vb; ppwm0 = &Vc;
                base = 0;
                break;

            case 2: // Sector 2: (0,1,1)  120-180 degrees
                iH = &(ParkParm.qIa); iL = &(ParkParm.qIc); i0 = &(ParkParm.qIb);
                ppwmH = &Va; ppwm0 = &Vb; ppwmL = &Vc;
                base =   ENC_PULSE_PER_SECTOR;
                break;

            case 3: // Sector 3: (0,1,0)  180-240 degrees
                iH = &(ParkParm.qIb); iL = &(ParkParm.qIc); i0 = &(ParkParm.qIa);
                ppwm0 = &Va; ppwmH = &Vb; ppwmL = &Vc;
                base = 2*ENC_PULSE_PER_SECTOR;
                break;

            case 4: // Sector 4: (1,1,0)  240-300 degrees
                iH = &(ParkParm.qIb); iL = &(ParkParm.qIa); i0 = &(ParkParm.qIc);
                ppwmL = &Va; ppwmH = &Vb; ppwm0 = &Vc;
                base = 3*ENC_PULSE_PER_SECTOR;
                break;

            case 5: // Sector 5: (1,0,0)  300-360 degrees
                iH = &(ParkParm.qIc); iL = &(ParkParm.qIa); i0 = &(ParkParm.qIb);
                ppwmL = &Va; ppwm0 = &Vb; ppwmH = &Vc;
                base = 4*ENC_PULSE_PER_SECTOR;
                break;

            case 6: // Sector 6: (1,0,1)    0- 60 degrees
                iH = &(ParkParm.qIc); iL = &(ParkParm.qIb); i0 = &(ParkParm.qIa);
                ppwm0 = &Va; ppwmL = &Vb; ppwmH = &Vc;
                base = 5*ENC_PULSE_PER_SECTOR;
                break;
        }
    }

    bool negative_sec = sector%2;

    int delta = enc-base-ENC_PULSE_PER_SECTOR/2;

    //static const int DIVISOR=ENC_PULSE_PER_SECTOR/60;
    static const int DIVISOR=QE_PPR/360;
    delta /= DIVISOR;

    int sinT;

    if (delta<0)
    {
        delta = -delta;
        if (delta>30) delta = 30;    
        sinT = -sin_table[delta];
    }
    else
    {
        if (delta>30) delta = 30;    
        sinT =  sin_table[delta];
    }

    int cosT = cos_table[delta];

    if (negative_sec)
    {
        I2Tdata.IQMeasured += 1 + /* sqrt3/2 */  (int)(__builtin_mulss((*iH-*iL),cosT)>>15)-3*(int)(__builtin_mulss(   *i0   ,sinT)>>15);
        I2Tdata.IDMeasured += 1 + /* 3/2 */     -(int)(__builtin_mulss(   *i0   ,cosT)>>15)-  (int)(__builtin_mulss((*iH-*iL),sinT)>>15);
    }
    else
    {
        I2Tdata.IQMeasured += 1 + /* sqrt3/2 */  (int)(__builtin_mulss((*iH-*iL),cosT)>>15)+3*(int)(__builtin_mulss(  *i0    ,sinT)>>15);
        I2Tdata.IDMeasured += 1 + /* 3/2 */      (int)(__builtin_mulss(   *i0   ,cosT)>>15)-  (int)(__builtin_mulss((*iH-*iL),sinT)>>15);
    }

    I2Tdata.IQMeasured >>= 1;
    I2Tdata.IDMeasured >>= 1;

    static const long V_INT_LIMIT = ((long)PWM_MAX)*1024L;
    static long VqRef = 0;


    if (!sAlignInProgress)
    {
#ifdef CALIBRATION
    IqRef = IqCalib;
#else
        if (SysStatus.SpeedLoop)
        {
            if (has_speed_sample)
            {
                int speed_error = CtrlReferences.WRef - gQEVelocity;

                static int speed_error_old = 0;

                if (SysStatus.CurrentLoop)
                {
                    static long Is = 0;

                    //Is += __builtin_mulss(speed_error - speed_error_old, 32) + speed_error + speed_error_old;

                    Is += (((long) (speed_error - speed_error_old)) << 5) + (long) (speed_error + speed_error_old);

                    if (Is > UDEF_CURRENT_MAX) Is = UDEF_CURRENT_MAX; else if (Is < -UDEF_CURRENT_MAX) Is = -UDEF_CURRENT_MAX;

                    if (starting)
                    {
                        if (Is > 0)
                        {
                            if (Is > IqRef) ++IqRef; else { IqRef = (int)Is; starting = False; }
                        }
                        else
                        {
                            if (Is < IqRef) --IqRef; else { IqRef = (int)Is; starting = False; }
                        }
                    }
                    else
                    {
                        IqRef = (int)Is;
                    }
                }
                else
                {
                    VqRef += (((long) (speed_error - speed_error_old)) << 9) + ((long) (speed_error + speed_error_old)<<2);

                    //VqRef = (speed_error << 9) + (speed_error_int << 3);

                    if (VqRef > V_INT_LIMIT) VqRef = V_INT_LIMIT; else if (VqRef < -V_INT_LIMIT) VqRef = -V_INT_LIMIT;

                    IqRef = 0;
                }

                speed_error_old = speed_error;
            } // has_speed_sample
        }
        else if (SysStatus.CurrentLoop)
        {
            VqRef = 0;
            IqRef = CtrlReferences.IqRef;

            if (IqRef>UDEF_CURRENT_MAX) IqRef = UDEF_CURRENT_MAX; else if (IqRef<-UDEF_CURRENT_MAX) IqRef = -UDEF_CURRENT_MAX;
        }
        else
        {
            VqRef = ((long)CtrlReferences.VqRef)<<5;
            IqRef = 0;
        }
#endif
    }
    ////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // Iq control loop

    int Vq = 0;

    if (SysStatus.CurrentLoop || sAlignInProgress) // current closed loop
    {
        int iQerror = IqRef-I2Tdata.IQMeasured;
        static int iQerror_old = 0;

        static long VqA = 0;

        //VqA += __builtin_mulss(iQerror - iQerror_old, 16) + (long)(iQerror + iQerror_old);

        VqA += (((long)(iQerror-iQerror_old))<<4) + (long)(iQerror+iQerror_old);

        iQerror_old = iQerror;

        if (VqA > V_INT_LIMIT) VqA = V_INT_LIMIT; else if (VqA < -V_INT_LIMIT) VqA = -V_INT_LIMIT;

        Vq = (int)(VqA>>10);
    }
    else // current open loop
    {
        static char limit = 0;

        if (I2Tdata.IQMeasured > UDEF_CURRENT_MAX)
        {
            limit = 1;
        }
        else if (I2Tdata.IQMeasured < -UDEF_CURRENT_MAX)
        {
            limit = -1;
        }

        if (limit)
        {
            static long VqL = 0;
            static int iQerror_old = 0;

            if (limit == 1)
            {
                int iQerror =  UDEF_CURRENT_MAX-I2Tdata.IQMeasured;
                VqL += (((long)(iQerror-iQerror_old))<<4) + (long)(iQerror+iQerror_old);
                iQerror_old = iQerror;

                if (VqL >= 0) { VqL = 0; limit = 0; iQerror_old = 0; }
            }
            else if (limit == -1)
            {
                int iQerror = -UDEF_CURRENT_MAX-I2Tdata.IQMeasured;
                VqL += (((long)(iQerror-iQerror_old))<<4) + (long)(iQerror+iQerror_old);
                iQerror_old = iQerror;

                if (VqL <= 0) { VqL = 0; limit = 0; iQerror_old = 0; }
            }

            Vq = (int)((VqRef+VqL)>>10);
        }
        else
        {
            Vq = (int)(VqRef>>10);
        }
    }
    //
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // BEMF section
    int iDerror = -I2Tdata.IDMeasured;
    static int iDerror_old = 0;

    static long VdA = 0;

   //VdA += __builtin_mulss(iDerror - iDerror_old, 16) + (long)(iDerror + iDerror_old);

    VdA += (((long)(iDerror-iDerror_old))<<4) + (long)(iDerror+iDerror_old);

    iDerror_old = iDerror;

    if (VdA > V_INT_LIMIT) VdA = V_INT_LIMIT; else if (VdA < -V_INT_LIMIT) VdA = -V_INT_LIMIT;

    int Vd = (int)(VdA>>10);
    //
    ////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////
    // if we are close enough to the PCD register shadow copy then wait until
    // its done before updating local PDCs
    // The condition will became false when the PWM is counting down but it is 
    // still far from 0 or when it change direction counting up

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
    //
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // inv transform and PWM drive
    int V1 = (int)(__builtin_mulss(Vq,cosT)>>15)-3*(int)(__builtin_mulss(Vd,sinT)>>15);
    int V2 = (int)(__builtin_mulss(Vq,sinT)>>15)+  (int)(__builtin_mulss(Vd,cosT)>>15);

    if (negative_sec) V2=-V2;

    *ppwmH =  V1-V2;
    *ppwm0 =  V2+V2;
    *ppwmL = -V1-V2;

    pwmOut(Va,Vb,Vc);
    //
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // Invoke the I2T integrator with new current values
    //static const long I2T_LIMIT = ((long)(UDEF_CURRENT_MAX/100)*(long)(UDEF_CURRENT_MAX/100))*(UDEF_I2T_LIMIT*UDEF_I2T_LIMIT);

    //static long I2Tacc = 0;
    //long I2 = __builtin_mulss(I2Tdata.IQMeasured,I2Tdata.IQMeasured);

    //I2 -= I2Tacc;
    //I2 >>= 15;
    //I2Tacc += I2;

    //if (I2Tacc > I2T_LIMIT)
    if (gTemperature > 3468) // = 100 C   T = 0.0297*i2c - 3;
    {
        //The I2T grew too much. Protect!
        SysError.I2TFailure = 1;
        FaultConditionsHandler();
    }
    //
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // when gulp_update_request is == 1, the main loop will not access those vars.
    if (gulp_update_request) updateGulp();
    //
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    //Clear the DMA0 Interrupt Flag
    IFS0bits.DMA0IF = 0;
}

void updateGulp(void)
{
    if (!gulp_update_request) return;

    Gulp.W[0] = I2Tdata.IQMeasured;
    Gulp.W[1] = POSCNT; //gQEVelocity;//gTemperature; //encoder_error;
    Gulp.W[2] = gTemperature;//overf_cnt; // gQEPosition & 0xFFFF;
    Gulp.W[3] = i2cERRORS;    // gQEPosition>>16;

    //Gulp.W[0] = *gulpadr1;
    //Gulp.W[1] = *gulpadr2;
    //Gulp.W[2] = *gulpadr3;
    //Gulp.W[3] = *gulpadr4;

    // unlock the main loop, so it will read values just updated
    gulp_update_request = 0;

    // read VDC link raw value;
    VDCLink = ADCGetVDCLink();
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
    // Id (flux) Iq (torque) reference values
    CtrlReferences.VqRef = 0;
    // this parameter controls the startup torque reference
    CtrlReferences.IqRef = 0;
    // Omega setpoint
    CtrlReferences.WRef = 0;

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

    //I2TInit(&I2Tdata);
    //I2Tdata.Acc[0] = 0;
    //I2Tdata.Acc[1] = 0;
    //I2Tdata.Acc[2] = 0;

    // setup for a center aigned PWM
    //SetupPWM();
    pwmInit(LOOPINTCY/2, DDEADTIME, (8*LOOPINTCY)/20 /*pwm max = 80%*/);

    // setup and perform ADC offset calibration in MeasCurrParm.Offseta and Offsetb
    ADCDoOffsetCalibration();

    // Enable DMA interrupt, arm DMA for transfer
    ADCConfigPWMandDMAMode();

    // setup the position sensor
    EncoderInit();

    // Setup timer for velocity calculation and velocity control loop
    //Timer2Config();

    // Setup timer for CAN periodic send
    Timer4Config();

    // setup Hw related parameters (current gain, ecc...)
    SetupHWParameters();

    // init PIDs and references
    //SetupControlParameters();

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
    //PWMDisable();
    pwmOFF();
    
    // Disable ADC IRQ and DMA
    ADCInterruptAndDMADisable();

    // I2T and encoder traking will run in behalf of timer 3 when 2FOC loop is not active
    EnableAuxServiceTimer();

    // zero torque and speed referemces
    ZeroControlReferences();
    // Zero regulators
    //ZeroRegulators();
}

int main(void)
//
// drive functions are controlled according to DS402 standard (when possible)
//
{    
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
    EnableAuxServiceTimer();
    // enable timer1 interrupt
    EnableIntT1;

    // Reset all system error flags
    memset(SysError.b, 0, sizeof (SysError));
    // Avoid leaving any random garbage in SysStatus
    memset(SysStatus.b, 0, sizeof (SysStatus));

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

//switchondisabled:
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

    uint32_t boardCanAddr = BOARD_CAN_ADDR_DEFAULT;
    _memcpy_p2d16(&boardCanAddr, 0x15000, 4);
    CanIcubProtoInit(boardCanAddr);

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
    while (1)
    {
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
    gQEVelocity = 0;

    /////////////////////////////////////////////////////////////////////////////////

    //ZeroRegulators();

    // enable timer2 interrupt for speed calc
    //EnableIntT2;

    // Arm all the automation (PWM->ADC->DMA->FOC) but don't start yet!
    ADCInterruptAndDMAEnable();

    // TODO: enable overcurrent fault during rotor allignment
    // enable the overcurrent interrupt
    OverCurrentFaultIntEnable();

    // I2T will run on behalf of 2FOC loop. Stop running it in behalf of Timer 3
    DisableAuxServiceTimer();
    // Starts to pump current!
    //PWMEnable();
    pwmON();
    
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
