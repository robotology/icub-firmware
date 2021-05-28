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

volatile BOOL qe_index_found = FALSE;

void QEinit(int resolution, int motor_num_poles,char use_index)
{
    // init the quadrature encoder peripheral
    // Configure QEI pins as digital inputs
    // ADPCFG |= (1<<5 | 1<<6 | 1<<7); //0x0038;

    QE_USE_INDEX = use_index;

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

    DFLTCONbits.IMV = 3;

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

    // Clear interrupt flag
    IFS3bits.QEI1IF = 0;
    
    // Index pulse does not reset position counter
    QEICONbits.POSRES = 0;
    
    if (QE_USE_INDEX)
    {
        MAXCNT = 0xFFFF;

        //X4 mode with position counter reset by Index Pulse
        QEICONbits.QEIM = 6;
        // Enable interrupt on index
        IEC3bits.QEI1IE = 1;
    }
    else
    {
        MAXCNT = QE_RESOLUTION - 1;

        // X4 mode with position counter reset by match with MAXCNT
        QEICONbits.QEIM = 7;
        // Disable interrupt on MAXCNT
        IEC3bits.QEI1IE = 0;
    }
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

volatile int calibOffsetfbk = -1;
volatile int poscntfbk = 0;


inline int QEgetPos() __attribute__((always_inline));
inline int QEgetPos()
{
    int poscnt = (int)POSCNT;
    
    if (abs(poscnt) > QE_RESOLUTION)
    {
        gEncoderError.index_broken = TRUE;
    }

    return __builtin_divsd(((long)poscnt)<<16,QE_RESOLUTION);
}

volatile int maxCountfbk = 0;

// QE index zero crossing interrupt manager
void __attribute__((__interrupt__,no_auto_psv)) _QEI1Interrupt(void)
{
    static int UPDNold = -1;
    
    // disable interrupt
    IEC3bits.QEI1IE = 0;
    
    int poscnt = (int)POSCNT;
    
    int absposcnt = abs(poscnt);
    
    if (absposcnt > maxCountfbk) maxCountfbk = absposcnt;
    
    if (qe_index_found)
    {
        if (UPDNold == QEICONbits.UPDN)
        {
            if (absposcnt < 2)
            {
                gEncoderError.phase_broken = TRUE;
            }
            else if (absposcnt < (QE_RESOLUTION-QE_ERR_THR))
            {
                gEncoderError.dirty = TRUE;
            }        
        }
        else
        {
            if (abs(poscnt) > QE_ERR_THR)
            {
                gEncoderError.dirty = TRUE;
            }
        }
    }
    
    qe_index_found = TRUE;
   
    UPDNold = QEICONbits.UPDN;
    
    calibOffsetfbk = gEncoderConfig.offset;
    poscntfbk = POSCNT;
    
    POSCNT = QEICONbits.UPDN ? 0 : 0xFFFF;

    // clear interrupt flag
    IFS3bits.QEI1IF = 0;
    // enable interrupt
    IEC3bits.QEI1IE = 1;
}
