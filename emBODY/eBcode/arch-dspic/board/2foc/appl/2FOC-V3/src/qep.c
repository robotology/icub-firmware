#include <p33FJ128MC802.h>
#include <stdlib.h>
#include <qei.h>

#include "Faults.h"
#include "qep.h"

volatile EncoderConfig_t gEncoderConfig={0,90,0};
volatile tQEError gEncoderError;

volatile uint16_t QE_TICKS = 0;
volatile uint32_t QE_RESOLUTION = 0;

uint16_t QE_ERR_THR = 0;

volatile BOOL qe_ready = FALSE;
//volatile BOOL qe_moved = FALSE;

//volatile int32_t position_check = 0;

volatile int32_t ovfCNT = 0;

void QEinit(int ticks, int motor_num_poles, BOOL use_index)
{
    // init the quadrature encoder peripheral
    // Configure QEI pins as digital inputs
    // ADPCFG |= (1<<5 | 1<<6 | 1<<7); //0x0038;

    qe_ready = FALSE;
    //qe_moved = FALSE;

    // Disable QEI Module
    QEICONbits.QEIM = 0;
    // Clear any count errors
    QEICONbits.CNTERR = 0;
    // Continue operation during sleep
    QEICONbits.QEISIDL = 0;
    // QEA and QEB eventually swapped
    
    if (ticks < 0)
    {
        QEICONbits.SWPAB = 1;
        QE_TICKS = -ticks;
    }
    else
    {
        QEICONbits.SWPAB = 0;
        QE_TICKS =  ticks;
    }
    
    QE_RESOLUTION = 4*QE_TICKS;
    
    QE_ERR_THR = (QE_TICKS * (long)gEncoderConfig.tolerance) / 3600;
    gEncoderConfig.elettr_deg_per_rev = 360*motor_num_poles;

    //QEICONbits.SWPAB = 0; // iCub3 motor
    //QEICONbits.SWPAB = 1; // old motor and CER motor
    // do not output direction on IO pin
    QEICONbits.PCDOUT = 0;

    // Count error interrupts disabled
    DFLTCONbits.CEID = 1;
    // Digital filters output enabled for QEn and IDX pins
    DFLTCONbits.QEOUT = 1;
    // 1:4 clock divide for digital filter for QEn
    // main CK = 40Mhz. Filter CK = 10 Mhz
    // Filter accept edge if it persist over 3 CK
    // cycles. So max QEP pin freq is about 3.3Mhz
    DFLTCONbits.QECK = 2;
    DFLTCONbits.IMV  = 2;//3;

    // Initialize the QEP module counter to run in modulus.
    //
    // Example (drawn in X1 mode): encoder 3ppr, MAXCNT = 2;
    //      __      __      __      __      __      __      __      __      __      __
    //     |  |    |  |    |  |    |  |    |  |    |  |    |  |    |  |    |  |    |  |
    //  ___|  |____|  |____|  |____|  |____|  |____|  |____|  |____|  |____|  |____|  |
    // 2   0       1       2       0       1       2       0       1       2       0
    //                       |                       |                       |
    //                     round                   round                   round
    
    // Reset position counter.
    POSCNT = 0;
    MAXCNT = 0xFFFF;

    // Index pulse resets position counter
    QEICONbits.POSRES = 0;
    QEICONbits.QEIM = 6;

    // reset interrupt flag
    IFS3bits.QEI1IF = 0;
    // enable interrupts
    IEC3bits.QEI1IE = 1;
}

inline void QEcountErrorClear() __attribute__((always_inline));
inline void QEcountErrorClear()
{
    QEICONbits.CNTERR = FALSE;
}

inline int QEgetElettrDeg() __attribute__((always_inline));
inline int QEgetElettrDeg()
{   
    //int degrees = 3600 + gEncoderConfig.offset + (__builtin_mulss(__builtin_divsd((ovfCNT+(unsigned)POSCNT)<<14,QE_TICKS), gEncoderConfig.elettr_deg_per_rev) >> 16);
    int degrees = 7200 + gEncoderConfig.offset +  __builtin_divsd(__builtin_mulss((int)POSCNT,gEncoderConfig.elettr_deg_per_rev),QE_RESOLUTION);
    
    return degrees % 360;
}

inline int QEgetPosition() __attribute__((always_inline));
inline int QEgetPosition()
{
    //return __builtin_divsd((ovfCNT+(unsigned)POSCNT)<<14,QE_TICKS);
    return __builtin_divsd(((long)(int)POSCNT)<<14,QE_TICKS);
}

//inline int32_t QEgetPosCheck() __attribute__((always_inline));
//inline int32_t QEgetPosCheck()
//{
//    return position_check;
//}

void QEHESCrossed(int up)
{
    static int cnt = 0;
    
    if (up)
    {
        if (++cnt == gEncoderConfig.numPoles || !qe_ready)
        {
            POSCNT = 0;
            ovfCNT = 0;
            cnt = 0;
        }
    }
    else 
    {
        if (--cnt == 0 || !qe_ready)
        {
            POSCNT =  0xFFFF;
            ovfCNT = -65536L;
            cnt = gEncoderConfig.numPoles;
        }
    }
    
    qe_ready = TRUE;
}

// QE zero crossing interrupt manager
void __attribute__((__interrupt__,no_auto_psv)) _QEI1Interrupt(void)
{   
    extern volatile long gQEPosition;
    
    // disable interrupts
    IEC3bits.QEI1IE = 0;
    
    if (!qe_ready)
    {
        qe_ready = TRUE;
        gQEPosition = QEICONbits.UPDN ? 0 : -1;
    }
    
    POSCNT = QEICONbits.UPDN ? 0 : 0xFFFF;
    
    // is it index or rollover?
    /*
    if (!QEICONbits.CNTERR) // INDEX
    {
        //int32_t counter = ovfCNT + (unsigned)POSCNT;
        //position_check += counter;
        
        if (QEICONbits.UPDN) // UP
        {            
            ovfCNT = 0;
            POSCNT = 0;
        }
        else // DOWN
        {                        
            POSCNT =  0xFFFF;         
            ovfCNT = -65536L;
        }

        qe_ready = TRUE;
    }
    else // ROLLOVER
    {
        QEICONbits.CNTERR = 0;
        
        if (QEICONbits.UPDN)
        {
            ovfCNT += 65536L;
        }
        else
        {
            ovfCNT -= 65536L;
        }
    }
    */
    
    // clear interrupt flag
    IFS3bits.QEI1IF = 0;
    
    // enable interrupts
    IEC3bits.QEI1IE = 1;
}
