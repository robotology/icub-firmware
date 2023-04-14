#include <p33FJ128MC802.h>
#include <stdlib.h>
#include <qei.h>

#include "Faults.h"
#include "qep.h"

volatile EncoderConfig_t gEncoderConfig={0,90,0};
volatile tQEError gEncoderError;

volatile BOOL QE_READY = FALSE;

volatile uint16_t QE_TICKS = 0;
volatile  int32_t QE_RESOLUTION = 0;
volatile  int16_t QE_NUM_POLES = 0;
volatile  int16_t QE_RES_POLE = 0;
volatile  int16_t QE_IDEG_POLE = 0;
volatile  int16_t QE_IDEG_POLE_HALF = 0;

volatile int16_t QE_16_old = 0;
volatile int16_t QE_position = 0;

//uint16_t QE_ERR_THR = 0;
//volatile int32_t position_check = 0;

void QEinit(int16_t ticks, int8_t num_poles, BOOL use_index)
{
    // init the quadrature encoder peripheral
    // Configure QEI pins as digital inputs
    // ADPCFG |= (1<<5 | 1<<6 | 1<<7); //0x0038;

    // Disable QEI Module
    QEICONbits.QEIM = 0;
    // Clear any count errors
    QEICONbits.CNTERR = 0;
    // Continue operation during sleep
    QEICONbits.QEISIDL = 0;
    // QEA and QEB eventually swapped

    QE_READY = FALSE;
    QE_NUM_POLES = num_poles;
    
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
    
    //QE_ERR_THR = (QE_TICKS * (long)gEncoderConfig.tolerance) / 3600;
    
    QE_RESOLUTION = 4*QE_TICKS;   
    QE_RES_POLE = QE_RESOLUTION/QE_NUM_POLES;

    
    gEncoderConfig.elettr_deg_per_rev = 360*QE_NUM_POLES;

    //QEICONbits.SWPAB = 0; // iCub3 motor
    //QEICONbits.SWPAB = 1; // old motor and CER motor
    // do not output direction on IO pin
    QEICONbits.PCDOUT = 0;
    
    // Digital filters output enabled for QEn and IDX pins
    DFLTCONbits.QEOUT = 1;
    // 1:4 clock divide for digital filter for QEn
    // main CK = 40Mhz. Filter CK = 10 Mhz
    // Filter accept edge if it persist over 3 CK
    // cycles. So max QEP pin freq is about 3.3Mhz
    DFLTCONbits.QECK = 2;
    DFLTCONbits.IMV  = 3; // iCub
    //DFLTCONbits.IMV  = 2; // ergoCub

    // Initialize the QEP module counter to run in modulus.
    //
    // Example (drawn in X1 mode): encoder 3ppr, MAXCNT = 2;
    //      __      __      __      __      __      __      __      __      __      __
    //     |  |    |  |    |  |    |  |    |  |    |  |    |  |    |  |    |  |    |  |
    //  ___|  |____|  |____|  |____|  |____|  |____|  |____|  |____|  |____|  |____|  |
    // 2   0       1       2       0       1       2       0       1       2       0
    //                       |                       |                       |
    //                     round                   round                   round
    
    // Count error interrupts disabled
    DFLTCONbits.CEID = 1;
    
    // Reset position counter.
    POSCNT = 0;
    MAXCNT = QE_RES_POLE-1;
    
    QE_16_old   = 0;
    QE_position = 0;
    QE_IDEG_POLE = 65536L/QE_NUM_POLES;
    QE_IDEG_POLE_HALF = 65536L/(2*QE_NUM_POLES);
    
    // Index pulse doesn't reset position counter
    QEICONbits.POSRES = 0;
    
    QEICONbits.QEIM = use_index ? 6 : 7; 
        
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
    uint16_t degrees = __builtin_divud(__builtin_muluu(POSCNT,360),QE_RES_POLE);
        
    return (gEncoderConfig.offset + (int)degrees) % 360;
}

inline int QEgetPosition() __attribute__((always_inline));
inline int QEgetPosition()
{
    int16_t idegs = (int16_t)__builtin_divud(POSCNT<<14,QE_TICKS);
    
    int16_t delta = idegs - QE_16_old;
    QE_16_old = idegs;
    
    while (delta >  QE_IDEG_POLE) delta -= QE_IDEG_POLE_HALF;
    while (delta < -QE_IDEG_POLE) delta += QE_IDEG_POLE_HALF;
    
    QE_position += delta;
    
    return QE_position;
}

//inline int32_t QEgetPosCheck() __attribute__((always_inline));
//inline int32_t QEgetPosCheck()
//{
//    return position_check;
//}

// this is the same of the index simulated by the hall sensors 
// so that it must be triggered every num_poles transitions 
// 1->6 or 6->1
void QEHESCrossed(BOOL up)
{
    extern volatile long gQEPosition;
    
    static int cnt = 0;
    
    if (!QE_READY)
    {
        QE_READY = TRUE;
        
        if (up)
        {
            gQEPosition = 0;
            QE_16_old   = 0;
            QE_position = 0;
            POSCNT = 0;
            cnt = 0;
        }
        else
        {
            gQEPosition = -1;
            QE_16_old   = -1;
            QE_position = -1;
            POSCNT = MAXCNT;
            cnt = QE_NUM_POLES-1;
        }
            
        return;
    }
    
    if (up)
    {
        if (++cnt >= QE_NUM_POLES)
        {
            cnt = 0;
            POSCNT = 0;
            QE_position = 0;
        }
    }
    else
    {
        if(--cnt < 0)
        {
            POSCNT = MAXCNT;
            cnt = QE_NUM_POLES-1;
            QE_position = -1;
        }
    }
}
    
// QE zero crossing interrupt manager
void __attribute__((__interrupt__,no_auto_psv)) _QEI1Interrupt(void)
{   
    extern volatile long gQEPosition;
    
    // disable interrupts
    IEC3bits.QEI1IE = 0;
    
    if (QEICONbits.UPDN)
    {
        POSCNT = 0;
        QE_position = 0;
    }
    else
    {
        POSCNT = MAXCNT;
        QE_position = -1;
    }
        
    if (!QE_READY)
    {
        QE_READY = TRUE;
        gQEPosition = QEICONbits.UPDN ? 0 : -1;
    }

    QEICONbits.CNTERR = FALSE;
    
    // clear interrupt flag
    IFS3bits.QEI1IF = 0;
    
    // enable interrupts
    IEC3bits.QEI1IE = 1;
}
