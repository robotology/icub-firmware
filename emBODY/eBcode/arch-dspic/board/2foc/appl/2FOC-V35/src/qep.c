#include <p33FJ128MC802.h>
#include <stdlib.h>
#include <qei.h>

#include "Faults.h"
#include "qep.h"

volatile EncoderConfig_t gEncoderConfig={0,90,0};
volatile tQEError gEncoderError;

volatile int QE_RESOLUTION = 0;//14400;
int QE_ERR_THR = 0;//144;
BOOL QE_USE_INDEX = FALSE;

volatile BOOL qe_ready = FALSE;
volatile BOOL qe_moved = FALSE;

void QEinit(int resolution, int motor_num_poles, char use_index)
{
    // init the quadrature encoder peripheral
    // Configure QEI pins as digital inputs
    // ADPCFG |= (1<<5 | 1<<6 | 1<<7); //0x0038;

    QE_USE_INDEX = use_index;

    qe_ready = FALSE;
    qe_moved = FALSE;

    // Disable QEI Module
    QEICONbits.QEIM = 0;
    // Clear any count errors
    QEICONbits.CNTERR = 0;
    // Continue operation during sleep
    QEICONbits.QEISIDL = 0;
    // QEA and QEB eventually swapped
    
    if (resolution < 0)
    {
        QEICONbits.SWPAB = 1;
        QE_RESOLUTION = -resolution;
    }
    else
    {
        QEICONbits.SWPAB = 0;
        QE_RESOLUTION = resolution;
    }
    
    QE_ERR_THR = ((long)QE_RESOLUTION * (long)gEncoderConfig.tolerance) / 3600;
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
    //DFLTCONbits.QECK = 2;
    DFLTCONbits.QECK = 3;

    DFLTCONbits.IMV = 3;

    //#endif
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
    IFS3bits.QEI1IF = 0;
    POSCNT = 0;
    MAXCNT = 0xFFFF;
    // Index pulse does not reset position counter
    QEICONbits.POSRES = 0;
    QEICONbits.QEIM = 6;
            
    IEC3bits.QEI1IE = QE_USE_INDEX ? 1 : 0;
    
    /*
    if (QE_USE_INDEX)
    {
        MAXCNT = 0xFFFF;
        // Index pulse does not reset position counter
        QEICONbits.POSRES = 0;
        //X4 mode with position counter reset by Index Pulse
        QEICONbits.QEIM = 6;
        IEC3bits.QEI1IE = 1;
    }
    else
    {
        MAXCNT = QE_RESOLUTION - 1;
        QEICONbits.POSRES = 1;
        // X4 mode with position counter reset by match with MAXCNT
        QEICONbits.QEIM = 7;
        IEC3bits.QEI1IE = 0;
    }
    */
}

inline unsigned int QEgetRaw() __attribute__((always_inline));
inline unsigned int QEgetRaw()
{
    return POSCNT;
}

inline int QEcountError() __attribute__((always_inline));
inline int QEcountError()
{
    return QEICONbits.CNTERR == TRUE;
}

inline void QEcountErrorClear() __attribute__((always_inline));
inline void QEcountErrorClear()
{
    QEICONbits.CNTERR = FALSE;
}

inline int QEgetElettrDeg() __attribute__((always_inline));
inline int QEgetElettrDeg()
{   
    int degrees = gEncoderConfig.offset + __builtin_divsd(__builtin_mulss((int)POSCNT, gEncoderConfig.elettr_deg_per_rev),QE_RESOLUTION);
    
    while (degrees >= 360) degrees -= 360;
    while (degrees <    0) degrees += 360;
    
    return degrees;
}

extern volatile int dataA;
extern volatile int dataB;
extern volatile int dataC;
extern volatile int dataD;

volatile int poscnt_old = 0x7FFF;

volatile BOOL newencdata = FALSE;

inline int QEgetPos() __attribute__((always_inline));
inline int QEgetPos()
{
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

    return __builtin_divsd(((long)poscnt)<<16,QE_RESOLUTION);
}

void QEsignalZeroCrossing(int UPDN)
{
    static int UPDN_old = -1;  
    
    int poscnt = (int)POSCNT;
    
    if (qe_ready)
    {
        if ((poscnt<-QE_ERR_THR || poscnt>QE_ERR_THR) && 
            (poscnt<QE_RESOLUTION-QE_ERR_THR || poscnt>QE_RESOLUTION+QE_ERR_THR) && 
            (poscnt<-QE_RESOLUTION-QE_ERR_THR || poscnt>-QE_RESOLUTION+QE_ERR_THR))
        {
            //gEncoderError.dirty = TRUE;
            //SysError.EncoderFault = TRUE;
            //FaultConditionsHandler();
        }
    }
    
    dataA = gEncoderConfig.offset;
    dataB = POSCNT;
            
    newencdata = TRUE;
    
    POSCNT = UPDN ? 0 : 0xFFFF;
    
    qe_ready = TRUE;
    
    if (!qe_moved && (UPDN_old == UPDN))
    {
        gEncoderError.phase_broken = TRUE;
        SysError.EncoderFault = TRUE;
        FaultConditionsHandler();
    }
    
    UPDN_old = UPDN;
    poscnt_old = (int)POSCNT;
    qe_moved = FALSE;
}

// QE zero crossing interrupt manager
void __attribute__((__interrupt__,no_auto_psv)) _QEI1Interrupt(void)
{
    QEsignalZeroCrossing(QEICONbits.UPDN);
    
    // clear interrupt flag
    IFS3bits.QEI1IF = 0;
}

/*
// QE zero crossing interrupt manager
void __attribute__((__interrupt__,no_auto_psv)) _QEI1Interrupt(void)
{
    static int updn_old = -1;  
    
    int poscnt = (int)POSCNT;
    
    if (qe_ready)
    {
        if ((poscnt<-QE_ERR_THR || poscnt>QE_ERR_THR) && 
            (poscnt<QE_RESOLUTION-QE_ERR_THR || poscnt>QE_RESOLUTION+QE_ERR_THR) && 
            (poscnt<-QE_RESOLUTION-QE_ERR_THR || poscnt>-QE_RESOLUTION+QE_ERR_THR))
        {
            gEncoderError.dirty = TRUE;
            SysError.EncoderFault = TRUE;
            FaultConditionsHandler();
        }
    }
    
    dataA = gEncoderConfig.offset;
    dataB = POSCNT;
            
    newencdata = TRUE;
    
    POSCNT = QEICONbits.UPDN ? 0 : 0xFFFF;
    
    qe_ready = TRUE;
    
    if (!qe_moved && updn_old == QEICONbits.UPDN)
    {
        gEncoderError.phase_broken = TRUE;
        SysError.EncoderFault = TRUE;
        FaultConditionsHandler();
    }
    
    updn_old = QEICONbits.UPDN;
    poscnt_old = (int)POSCNT;
    qe_moved = FALSE;
    
    // clear interrupt flag
    IFS3bits.QEI1IF = 0;
}
*/