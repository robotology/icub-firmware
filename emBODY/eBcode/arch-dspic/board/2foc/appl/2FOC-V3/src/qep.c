#include <p33FJ128MC802.h>
#include <stdlib.h>
#include <qei.h>

#include "Faults.h"
#include "qep.h"

volatile EncoderConfig_t gEncoderConfig={0,90,0};
volatile tQEError gEncoderError;

//volatile int32_t QE_RESOLUTION = 0;
volatile unsigned int QE_TICKS = 0;
int QE_ERR_THR = 0;
BOOL QE_USE_INDEX = FALSE;

volatile BOOL qe_ready = FALSE;
//volatile BOOL qe_moved = FALSE;

volatile int32_t revols = 0;

void QEinit(int ticks, int motor_num_poles, char use_index)
{
    // init the quadrature encoder peripheral
    // Configure QEI pins as digital inputs
    // ADPCFG |= (1<<5 | 1<<6 | 1<<7); //0x0038;

    QE_USE_INDEX = use_index;

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
        //QE_RESOLUTION = -4*(long)resolution;
        QE_TICKS = -ticks;
    }
    else
    {
        QEICONbits.SWPAB = 0;
        //QE_RESOLUTION =  4*(long)resolution;
        QE_TICKS =  ticks;
    }
    
    QE_ERR_THR = (4*QE_TICKS * (long)gEncoderConfig.tolerance) / 3600;
    gEncoderConfig.elettr_deg_per_rev = 360*motor_num_poles;

    //QEICONbits.SWPAB = 0; // iCub3 motor
    //QEICONbits.SWPAB = 1; // old motor and CER motor
    // do not output direction on IO pin
    QEICONbits.PCDOUT = 0;

    // Count error interrupts enabled
    DFLTCONbits.CEID = 0;
    // Digital filters output enabled for QEn and IDX pins
    DFLTCONbits.QEOUT = 1;
    // 1:4 clock divide for digital filter for QEn
    // main CK = 40Mhz. Filter CK = 10 Mhz
    // Filter accept edge if it persist over 3 CK
    // cycles. So max QEP pin freq is about 3.3Mhz
    DFLTCONbits.QECK = gEncoderConfig.QECK;//3;
    DFLTCONbits.IMV  = gEncoderConfig.IMV;//3;

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
    revols = 0;
    MAXCNT = 4*QE_TICKS-1;//0xFFFF;
    // Index pulse resets position counter
    QEICONbits.POSRES = 1;
    QEICONbits.QEIM = 6;
            
    //IEC3bits.QEI1IE = QE_USE_INDEX ? 1 : 0;
    // reset interrupt flag
    IFS3bits.QEI1IF = 0;
    // enable interrupts
    IEC3bits.QEI1IE = 1;
}

//inline unsigned int QEgetRaw() __attribute__((always_inline));
//inline unsigned int QEgetRaw()
//{
//    return POSCNT;
//}

//inline int QEcountError() __attribute__((always_inline));
//inline int QEcountError()
//{
//    return QEICONbits.CNTERR == TRUE;
//}

inline void QEcountErrorClear() __attribute__((always_inline));
inline void QEcountErrorClear()
{
    QEICONbits.CNTERR = FALSE;
}

inline int QEgetElettrDeg() __attribute__((always_inline));
inline int QEgetElettrDeg()
{   
    int degrees = gEncoderConfig.offset + (__builtin_mulss(__builtin_divsd((revols+(unsigned)POSCNT)<<14,QE_TICKS), gEncoderConfig.elettr_deg_per_rev) >> 16);
    
    while (degrees >= 360) degrees -= 360;
    while (degrees <    0) degrees += 360;
    
    return degrees;
}

inline int QEgetElettrDegUncal() __attribute__((always_inline));
inline int QEgetElettrDegUncal()
{   
    int degrees = __builtin_mulss(__builtin_divsd((revols+(unsigned)POSCNT)<<14,QE_TICKS), gEncoderConfig.elettr_deg_per_rev) >> 16;
    
    while (degrees >= 360) degrees -= 360;
    while (degrees <    0) degrees += 360;
    
    return degrees;
}

volatile int poscnt_old = 0x7FFF;

inline int QEgetPos() __attribute__((always_inline));
inline int QEgetPos()
{
    /*
    int poscnt = (int)POSCNT;
  
    if (!qe_moved)
    {
        int poscnt_delta = poscnt - poscnt_old;
    
        if (abs(poscnt_delta) > 32)
        {
            poscnt_old = poscnt;
            qe_moved = TRUE;
        }
    }

    if (QE_USE_INDEX && qe_ready)
    {
        if (poscnt < -QE_RESOLUTION-QE_ERR_THR || poscnt > QE_RESOLUTION+QE_ERR_THR)
        {
            // Index is broken
            gEncoderError.index_broken = TRUE;
            SysError.EncoderFault = TRUE;
            FaultConditionsHandler();
        }
    }
    */

    //return __builtin_divsd(((long)poscnt)<<16,QE_RESOLUTION);
    return __builtin_divsd((revols+(unsigned)POSCNT)<<14,QE_TICKS);
}

void QEHESCrossed(int up)
{
    POSCNT = up ? 0 : 0xFFFF;
    revols = 0;
}

extern volatile int dataA;
extern volatile int dataB;
extern volatile int dataC;
extern volatile int dataD;
extern volatile BOOL newencdata; 

// QE zero crossing interrupt manager
void __attribute__((__interrupt__,no_auto_psv)) _QEI1Interrupt(void)
{
    int poscnt = POSCNT;
    dataB = poscnt;
    // is index or rollover?
    if (!QEICONbits.CNTERR) // index
    {
        if (QEICONbits.UPDN)
        {
            dataA = 1;
            //POSCNT = 0;
            //revols = 0;
        }
        else
        {
            dataA = 0;
            //POSCNT = 0xFFFF;
            //POSCNT = 20799;
                    
            //revols = 0;
            //revols = -65536L;
        }

        qe_ready = TRUE;
    }
    else // rollover
    {
        QEICONbits.CNTERR = 0;
        
        if (QEICONbits.UPDN)
        {
            dataA = 3;
           
            if (poscnt > MAXCNT) poscnt -= MAXCNT;
            POSCNT = (unsigned)poscnt;
            //POSCNT = 0;
        }
        else
        {
            dataA = 2;
            
            if (poscnt < 0) poscnt += MAXCNT;
            POSCNT = (unsigned)poscnt;
            //POSCNT = 20799;
        }
        
        //revols = 0;
        //revols += QEICONbits.UPDN ? 65536L : -65536L; 
    }
 
    newencdata = TRUE;
 
    // clear interrupt flag
    IFS3bits.QEI1IF = 0;
}
