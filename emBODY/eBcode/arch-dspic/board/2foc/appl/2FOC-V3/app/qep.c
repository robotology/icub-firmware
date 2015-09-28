#include <p33FJ128MC802.h>
#include <qei.h>

#include "Faults.h"
#include "qep.h"

volatile EncoderConfig_t gEncoderConfig={0,90,0};
volatile tQEError gEncoderError;

unsigned int QE_RESOLUTION = 0;//14400;
int QE_ONE_PERCENT_THICKS = 0;//144;
BOOL QE_USE_INDEX = FALSE;

volatile BOOL qe_ready = FALSE;
volatile BOOL qe_moved = FALSE;

volatile unsigned int poscnt_max = 0;
volatile unsigned int poscnt_min = 0xFFFF;

void QEinit(int resolution, int motor_num_poles,char use_index)
{
    // init the quadrature encoder peripheral
    // Configure QEI pins as digital inputs
    // ADPCFG |= (1<<5 | 1<<6 | 1<<7); //0x0038;

    QE_USE_INDEX = use_index;

    qe_ready = FALSE;
    qe_moved = FALSE;
    
    poscnt_max = 0;
    poscnt_min = 0xFFFF;

    // Disable QEI Module
    QEICONbits.QEIM = 0;
    // Clear any count errors
    QEICONbits.CNTERR = 0;
    // Continue operation during sleep
    QEICONbits.QEISIDL = 0;
    // QEA and QEB eventually swapped

    QEICONbits.SWPAB = resolution<0;
    if (resolution<0) resolution = -resolution;

    QE_RESOLUTION = resolution;
    
    QE_ONE_PERCENT_THICKS = QE_RESOLUTION/100;
    gEncoderConfig.elettr_deg_per_rev = 360*motor_num_poles;

    //QEICONbits.SWPAB = 0; // iCub3 motor
    //QEICONbits.SWPAB = 1; // old motor and CER motor
    // do not output direction on IO pin
    QEICONbits.PCDOUT = 0;
    // Index pulse resets position counter
    QEICONbits.POSRES = 1;

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

    MAXCNT = QE_RESOLUTION - 1;

    // Reset position counter.
    IFS3bits.QEI1IF = 0;
    POSCNT = 0;

    if (QE_USE_INDEX)
    {
        // X4 mode with position counter reset by Index Pulse
        QEICONbits.QEIM = 6;
        IEC3bits.QEI1IE = 1;
    }
    else
    {
        // X4 mode with position counter reset by match with MAXCNT
        QEICONbits.QEIM = 7;
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
    return (gEncoderConfig.offset + __builtin_divud(__builtin_muluu(POSCNT,gEncoderConfig.elettr_deg_per_rev),QE_RESOLUTION)) % 360;
}

inline unsigned int QEgetPos() __attribute__((always_inline));
inline unsigned int QEgetPos()
{
    unsigned int poscnt = POSCNT;

    static unsigned int poscnt_old = 0xFFFF;
    if (poscnt_old != poscnt)
    {
        poscnt_old = poscnt;
        qe_moved = TRUE;
    }

    if (QE_USE_INDEX && qe_ready)
    {
        if ((poscnt >= QE_RESOLUTION+16) && (poscnt <= 65520))
        {
            // Index is broken
            gEncoderError.index_broken = TRUE;
            SysError.EncoderFault = TRUE;
            FaultConditionsHandler();
        }

        if (poscnt < poscnt_min) poscnt_min = poscnt;
        if (poscnt > poscnt_max) poscnt_max = poscnt;
    }

    return __builtin_divud(((unsigned long)poscnt)<<16,QE_RESOLUTION);
}

// QE zero crossing interrupt manager
void __attribute__((__interrupt__,no_auto_psv)) _QEI1Interrupt(void)
{
    if (qe_ready)
    {
        static char dir_old = -1;
        char dir = QEICONbits.UPDN;

        if (dir == dir_old)
        {
            unsigned int dirty_cws = (QE_RESOLUTION-1) - poscnt_max;
            unsigned int dirty_ccw = poscnt_min;

            if ((dirty_cws > QE_ONE_PERCENT_THICKS) || (dirty_ccw > QE_ONE_PERCENT_THICKS))
            {
                gEncoderError.dirty = TRUE;
                SysError.EncoderFault = TRUE;
                FaultConditionsHandler();
            }
        }
        else
        {
            dir_old = dir;
        }
    }
    else
    {
        qe_ready = TRUE;
    }
    
    if (qe_moved)
    {
        qe_moved = FALSE;
    }
    else
    {
        gEncoderError.phase_broken = TRUE;
        SysError.EncoderFault = TRUE;
        FaultConditionsHandler();
    }

    poscnt_min = 0xFFFF;
    poscnt_max = 0;

    // clear interrupt flag
    IFS3bits.QEI1IF = 0;
}
